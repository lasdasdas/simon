/*
 *   Copyright (C) 2008 Peter Grasch <peter.grasch@bedahr.org>
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
 *
 *   In addition, as a special exception, the copyright holders give
 *   permission to link this portion of this program with the
 *   Julius library and distribute linked combinations including the two.
 *
 *   You must obey the GNU General Public License in all respects
 *   for all of the code used other than Julius.  If you modify
 *   file(s) with this exception, you may extend this exception to your
 *   version of the file(s), but you are not obligated to do so.  If you
 *   do not wish to do so, delete this exception statement from your
 *   version.
 *
 *   Powered By:
 *
 *   Large Vocabulary Continuous Speech Recognition Engine Julius
 *   Copyright (c) 1997-2000 Information-technology Promotion Agency, Japan
 *   Copyright (c) 1991-2010 Kawahara Lab., Kyoto University
 *   Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 *   Copyright (c) 2005-2010 Julius project team, Nagoya Institute of Technology
 */

#include "juliuscontrol.h"
#include <simonrecognizer/recognitionconfiguration.h>
#include <simonrecognizer/juliusrecognitionconfiguration.h>
#include <simonutils/fileutils.h>

#include <QFile>
#include <KLocalizedString>
#include <KStandardDirs>
#include <KConfig>
#include <KDebug>
#include <KConfigGroup>
#include <KMimeType>
#include <KTar>
#include <locale.h>

JuliusControl::JuliusControl(const QString& username, QObject* parent) : RecognitionControl(username, RecognitionControl::HTK, parent)
{
  recog = new JuliusRecognizer();
}

bool JuliusControl::initializeRecognition(const QString& modelPath)
{
  if (modelPath != m_lastModel) { //already initialized / tried to initialize with this exact model
  
    kDebug() << "Initializing for model: " << modelPath << " old model path: " << m_lastModel;
    m_lastModel = modelPath;
    uninitialize();
    m_startRequests = 0;

    QString path = KStandardDirs::locateLocal("tmp", "/simond/"+username+"/julius/");
    if (QFile::exists(path+"hmmdefs") && !QFile::remove(path+"hmmdefs")) return false;
    if (QFile::exists(path+"tiedlist") && !QFile::remove(path+"tiedlist")) return false;
    if (QFile::exists(path+"model.dfa") && !QFile::remove(path+"model.dfa")) return false;
    if (QFile::exists(path+"model.dict") && !QFile::remove(path+"model.dict")) return false;
    if (QFile::exists(path+"julius.jconf") && !QFile::remove(path+"julius.jconf")) return false;

    if (!FileUtils::unpackAll(modelPath, path))//, (QStringList() << "hmmdefs" << "tiedlist" << "macros" << "stats")
      return false;
  }

  kDebug() << "Emitting recognition ready";
  emit recognitionReady();
  return true;
}

RecognitionConfiguration* JuliusControl::setupConfig()
{
  QByteArray dirPath = KStandardDirs::locateLocal("tmp", "/simond/"+username+"/julius/").toUtf8();
  QByteArray jConfPath = dirPath+"julius.jconf";
  QByteArray gram = dirPath+"model";
  QByteArray tiedList = dirPath+"tiedlist";
  QByteArray hmmDefs = dirPath+"hmmdefs";

  return new JuliusRecognitionConfiguration(jConfPath, gram, hmmDefs, tiedList);
}


void JuliusControl::emitError(const QString& error)
{
  QString specificError = error;
  QByteArray buildLog = getBuildLog();

  int indexStartVocaError = buildLog.indexOf("Error: voca_load_htkdict");
  if (indexStartVocaError != -1) {
    QByteArray findPhonesLog = buildLog;
    findPhonesLog  = findPhonesLog.replace("<br />", "\n");
    indexStartVocaError = findPhonesLog.indexOf("Error: voca_load_htkdict");
    int indexEndMissingPhones = findPhonesLog.indexOf("end missing phones");

    QList<QByteArray> lines = findPhonesLog.mid(indexStartVocaError, indexEndMissingPhones - indexStartVocaError).split('\n');

    QStringList missingPhones;
    QStringList affectedWords;

    bool thisLineMoreInfoForMissingTriphone = false;
    bool thisLineMoreMissingPhones = false;
    foreach (const QByteArray& lineByte, lines) {
      QString line = QString::fromUtf8(lineByte);
      if (line.contains(QRegExp("line [0-9]+: triphone \".*\" (or biphone \".*\" )?not found"))) {
        //Error: voca_load_htkdict: line 33: triphone "*-dh+ix" or biphone "dh+ix" not found
        //Error: voca_load_htkdict: line 33: triphone "dh-ix+s" not found
        thisLineMoreInfoForMissingTriphone = true;
      }
      else {
        if (thisLineMoreInfoForMissingTriphone) {
          //Error: voca_load_htkdict: the line content was: 2       [Towel] t aw ax l
          line = line.mid(line.lastIndexOf("       ")).trimmed();
          QString word = line.mid(line.indexOf('[')+1);
          word = word.left(word.indexOf(']'));
          affectedWords << word;

          thisLineMoreInfoForMissingTriphone = false;
        }
        else {
          if (thisLineMoreMissingPhones) {
            if (line.contains("end missing phones"))
              break;

            //Error: voca_load_htkdict: ax-m+b
            missingPhones << line.mid(26).trimmed();
          } else
          {
            if (line.contains("begin missing phones")) {
              thisLineMoreMissingPhones = true;
            }
          }
        }
      }
    }

    QString missingPhonesStr = missingPhones.join(", ");
    QString affectedWordsStr = affectedWords.join(", ");
    if (missingPhonesStr.length() > 200)
      missingPhonesStr = missingPhonesStr.left(200)+"...";
    if (affectedWordsStr.length() > 200)
      affectedWordsStr = affectedWordsStr.left(200)+"...";

    specificError = i18nc("%1 is a list of affected words, %2 is a list of phonemes", "The recognition could not be started because your model contains words that consists of sounds that are not covered by your acoustic model.\n\nYou need to either remove those words, transcribe them differently or train them.\n\nWarning: The latter will not work if you are using static base models!\n\nThis could also be a sign of a base model that uses a different phoneme set than your scenario vocabulary.\n\nThe following words are affected (list may not be complete):\n%1\n\nThe following phonemes are affected (list may not be complete):\n%2", affectedWordsStr, missingPhonesStr);
  }

  emit recognitionError(specificError, buildLog);
}

JuliusControl::~JuliusControl()
{
  uninitialize();
}
