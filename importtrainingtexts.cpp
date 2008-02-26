//
// C++ Implementation: importtrainingtexts
//
// Description: 
//
//
// Author: Peter Grasch <bedahr@gmx.net>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "importtrainingtexts.h"
#include <QWizardPage>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QProgressBar>
#include <QRadioButton>
#include <QPushButton>
#include <QFileDialog>
#include "quickdownloader.h"
#include "xmltrainingtextlist.h"
#include "importlocalwizardpage.h"
#include "importremotewizardpage.h"
#include "importworkingwizardpage.h"
#include "selectsourcewizardpage.h"
#include "xmltrainingtext.h"


/**
 * \brief Constructor
 * \author Peter Grasch
 */
ImportTrainingTexts::ImportTrainingTexts(QWidget *parent) : QWizard(parent)
{
	this->addPage(createIntroPage());
	
	QWizardPage *source = createSourcePage();
	QWizardPage *local = createLocalImportPage();
	QWizardPage *remote = createRemoteImportPage();
	QWizardPage *working = createWorkingPage();
	
	this->addPage(source);
	this->addPage(local);
	this->addPage(remote);
	this->addPage(working);
	
	this->addPage(createFinishedPage());
	setWindowTitle(tr("Trainingstext importieren"));
	setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/banners/importtexts.png"));
	prevId = 0;
	
}

/**
 * \brief Shows the wizard
 * \author Peter Grasch
 */
void ImportTrainingTexts::start()
{
	show();
}


/**
 * \brief Destructor
 * \author Peter Grasch
 */
ImportTrainingTexts::~ImportTrainingTexts()
{
    delete fd;
}


/**
 * \brief Creates the intropage
 * \author Peter Grasch
 * @return the wizardpage
 */
QWizardPage* ImportTrainingTexts::createIntroPage()
{
	QWizardPage *intro = new QWizardPage(this);
	intro->setTitle(tr("Importieren eines neuen Trainingstextes"));
	QLabel *label = new QLabel(intro);
	label->setText(tr("Mit Hilfe dieses Assistenten k�nnen Sie neue Trainingstexte\n aus dem Internet oder aus lokalen Dateien importieren.\n\nSo wird das Trainieren von simon nie langweilig!"));
	QVBoxLayout *layout = new QVBoxLayout(intro);
	layout->addWidget(label);
	intro->setLayout(layout);
	
	return intro;
}

/**
 * \brief Creates the remoteimport
 * \author Peter Grasch
 * @return the wizardpage
 */
QWizardPage* ImportTrainingTexts::createRemoteImportPage()
{
	ImportRemoteWizardPage *remoteImport = new ImportRemoteWizardPage(this);
	
	
	return remoteImport;
}

/**
 * \brief Creates the localimportpage
 * \author Peter Grasch
 * @return the wizardpage
 */
QWizardPage* ImportTrainingTexts::createLocalImportPage()
{
	ImportLocalWizardPage *localImport = new ImportLocalWizardPage(this);
	
	return localImport;
}


/**
 * \brief Creates the sourcepage
 * 
 * Here you can select where you want to import from (internet/file)
 * 
 * \author Peter Grasch
 * @return the wizardpage
 */
QWizardPage* ImportTrainingTexts::createSourcePage()
{
	SelectSourceWizardPage *source = new SelectSourceWizardPage(this);
	return source;
}

/**
 * \brief Creates the working page
 * \author Peter Grasch
 * @return the wizardpage
 */
QWizardPage* ImportTrainingTexts::createWorkingPage()
{
	ImportWorkingWizardPage *working= new ImportWorkingWizardPage(this);
	return working;
}

/**
 * \brief Creates the finished-page
 * @return the QWizardPage
 */
QWizardPage* ImportTrainingTexts::createFinishedPage()
{
	QWizardPage *finished = new QWizardPage(this);
	finished->setTitle(tr("Text hinzugef�gt"));
	QLabel *label = new QLabel(finished);
	label->setText(tr("Hiermit haben Sie den neuen Text hinzugef�gt.\n\nEr wird nun neben allen anderen Texten in Ihrem\nTrainingsdialog gelistet.\n\nVielen Dank und viel Spa� mit dem neuen Text!"));
	QVBoxLayout *layout = new QVBoxLayout(finished);
	layout->addWidget(label);
	finished->setLayout(layout);
	
	return finished;
}

