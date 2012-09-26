/*
 *   Copyright (C) 2012 Vladislav Sitalo <root@stvad.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version 2,
 *   or (at your option) any later version, as published by the Free
 *   Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "modelcompilationadaptersphinx.h"
#include "modelcompilersphinx.h"
#include "modelcompilationmanagersphinx.h"
#include <KStandardDirs>
#include <QFileInfo>
#include <QUuid>
#include <KGlobal>
#include <KAboutData>

ModelCompilationManagerSPHINX::ModelCompilationManagerSPHINX(const QString& userName, QObject *parent) : ModelCompilationManager(userName, parent)
//  tryAgain(false)
{
  compiler = new ModelCompilerSPHINX(userName, this);
  adapter = new ModelCompilationAdapterSPHINX(userName, this);

  connect(adapter, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
  connect(adapter, SIGNAL(adaptionAborted()), this, SIGNAL(modelCompilationAborted()));
  connect(adapter, SIGNAL(status(QString,int,int)), this, SIGNAL(status(QString,int,int)));

  connect(compiler, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
  connect(compiler, SIGNAL(status(QString,int,int)), this, SIGNAL(status(QString,int,int)));
  connect(compiler, SIGNAL(activeModelCompilationAborted()), this, SIGNAL(modelCompilationAborted()));

  connect(compiler, SIGNAL(wordUndefined(QString)), this, SIGNAL(wordUndefined(QString)));
  connect(compiler, SIGNAL(classUndefined(QString)), this, SIGNAL(classUndefined(QString)));
  connect(compiler, SIGNAL(phonemeUndefined(QString)), this, SIGNAL(phonemeUndefined(QString)));
}

void ModelCompilationManagerSPHINX::run()
{
  //first, adapt the input to sphinx readable formats using the adapter
  QHash<QString,QString> adaptionArgs;

  QString activeDir = KStandardDirs::locateLocal("appdata", "models/"+userName+"/active/");
  QUuid modelUuid = QUuid::createUuid();

  ModelCompilationAdapter::AdaptionType adaptionType = (baseModelType == 0) ?
                                                         (ModelCompilationAdapter::AdaptLanguageModel) :
                                                         (ModelCompilationAdapter::AdaptionType) (ModelCompilationAdapter::AdaptAcousticModel|ModelCompilationAdapter::AdaptLanguageModel);

  QString compilationDir = KStandardDirs::locateLocal("tmp", KGlobal::mainComponent().aboutData()->appName()+'/'+userName+"/compile/sphinx/");

  QString modelName = userName+modelUuid.toString();
  adaptionArgs.insert("workingDir", compilationDir);
  adaptionArgs.insert("modelName", modelName);

  //then, compile the model using the model compilation manager
  QHash<QString,QString> compilerArgs;

  compilerArgs.insert("modelDir", compilationDir);
  compilerArgs.insert("audioPath",KStandardDirs::locateLocal("appdata", "models/"+userName+"/samples/"));
  compilerArgs.insert("modelName", modelName);

  adapter->clearPoisonedPhonemes();

  do
  {
    if (!keepGoing) return;

    tryAgain = false;
    if (!adapter->startAdaption(adaptionType, scenarioPaths, promptsPathIn, adaptionArgs))
    {
      kWarning() << "Model adaption failed for user " << userName;
      return;
    }
    if (!keepGoing) return;

//    QString activeDir = KStandardDirs::locateLocal("appdata", "models/"+userName+"/active/");

    QString fetc = compilationDir+"/"+modelName+"/etc/"+modelName;

//    kDebug() << "Data\n" <<fetc<< "\n"<<activeDir;
    QFileInfo fiGrammar(fetc+GRAMMAR_EXT);
    bool hasGrammar = (fiGrammar.size() > 0);

    if (!hasGrammar)
    {
      kDebug() << "No Grammar!  Model recompilation aborting!";
      emit modelCompilationAborted(ModelCompilation::InsufficientInput);
      return;
    }

    ModelCompiler::CompilationType compilationType = getCompilationType(baseModelType);


    //build fingerprint and search cache for it
    uint fingerprint = 0;
    QStringList componentsToParse(QStringList() << GRAMMAR_EXT << PHONE_EXT << DICT_EXT);
    if (baseModelType > 0)
      componentsToParse << TRAIN_TRANSCRIPTION << TRAIN_FIELDS ;

    fingerprint = getFingerPrint(fetc, componentsToParse, compilationType);

    bool exists;
    QString outPath = cachedModelPath(fingerprint, &exists);

    if (!keepGoing) return;

    if (exists || compiler->startCompilation(compilationType, outPath, activeDir, compilerArgs))
    {
      emit modelReady(fingerprint, outPath);
      return;
    } else
      kWarning() << "Model compilation failed for user " << userName;
  } while (tryAgain);
  emit modelCompilationAborted(ModelCompilation::InsufficientInput);
}