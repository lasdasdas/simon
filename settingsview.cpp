//
// C++ Implementation: settingsview
//
// Description: 
//
//
// Author: Peter Grasch <bedahr@gmx.net>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "settingsview.h"
#include "simoninfo.h"
/**
 * \brief Constructor
 * 
 * This instances a new dialog and connects the signals/slots
 * It also enables/disables the plattform dependant options (alsa/dsound)
 * 
 * \author Peter Grasch
 * 
 * \param QWidget *parent
 * The parent of the dialog
 */

SettingsView::SettingsView(QWidget *parent)
 : QDialog(parent)
{
	ui.setupUi(this);
	
	connect(ui.pbSystemSettings, SIGNAL(clicked()), this, SLOT(switchToSystem()));
	connect(ui.pbSoundSettings, SIGNAL(clicked()), this, SLOT(switchToSound()));
	connect(ui.pbCommandSettings, SIGNAL(clicked()), this, SLOT(switchToCommands()));
	connect(ui.pbProtocolSettings, SIGNAL(clicked ()), this, SLOT(switchToProtocols()));
	connect(ui.pbRevert, SIGNAL(clicked()), this, SLOT(switchToHistory()));
	connect(ui.pbApply, SIGNAL(clicked()), this, SLOT(apply()));
	connect(ui.pbConfirm, SIGNAL(clicked()), this, SLOT(apply()));
	
	this->settings = new QSettings(QSettings::IniFormat,QSettings::UserScope,"CyberByte","simon");
	this->sc= new SoundControl();
    this->readConfig();
#ifdef linux
    ui.lbDirectX->setVisible(false);
    ui.lbALSA->setVisible(true);
#endif
#ifdef __WIN32
	ui.lbDirectX->setVisible(true);
	ui.lbALSA->setVisible(false);
#endif
    this->switchToSystem();
}





/**********************************************/
/******************Tab Stuff*******************/
/**********************************************/

/**
 * \brief Untoggles all the "tab" buttons on top
 * 
 * \author Peter Grasch
 */
void SettingsView::unsetAllTabs()
{
	ui.pbSystemSettings->setChecked(false);
	ui.pbSoundSettings->setChecked(false);
	ui.pbCommandSettings->setChecked(false);
	ui.pbProtocolSettings->setChecked(false);
	ui.pbRevert->setChecked(false);
	
}

/**
 * \brief Switches to the "system" tab
 * 
 * \author Peter Grasch
 */
void SettingsView::readConfig()
{
    settings->sync();

    ui.cbAskBeforeExit->setCheckState((settings->value("askbeforeexit").toBool()) ? Qt::Checked : Qt::Unchecked);
    ui.cbStartSimonOnBoot->setCheckState((settings->value("simonautostart").toBool()) ? Qt::Checked : Qt::Unchecked);
    ui.cbStartJuliusdOnBoot->setCheckState((settings->value("juliusdautostart").toBool()) ? Qt::Checked : Qt::Unchecked);
    ui.cbStartJuliusAsNeeded->setCheckState((settings->value("juliusdrequired").toBool()) ? Qt::Checked : Qt::Unchecked);
    ui.leLexicon->setText(settings->value("paths/lexicon").toString());
    ui.leGrammar->setText(settings->value("paths/grammar").toString());
    ui.leCommands->setText(settings->value("paths/commando").toString());
    ui.leVocab->setText(settings->value("paths/vocabul").toString());
    ui.lePrompts->setText(settings->value("paths/prompts").toString());
    
   	SoundDeviceList *sd=sc->getInputDevices();
    ui.cbInDevice->clear();
    int defindevice=settings->value("defindevice").toInt();
    for (int i=0; i<sd->count(); i++)
    {
        QString deviceid= ((SoundDevice)sd->at(i)).getDeviceID();
        ui.cbInDevice->addItem(((SoundDevice) sd->at(i)).getName(),deviceid);
        if (deviceid.toInt()==defindevice) ui.cbInDevice->setCurrentIndex(ui.cbInDevice->count());  
    }
    
   	sd=sc->getOutputDevices();
    ui.cbOutDevice->clear();
    int defoutdevice=settings->value("defoutdevice").toInt();
    for (int i=0; i<sd->count(); i++)
    {
        QString deviceid= ((SoundDevice)sd->at(i)).getDeviceID();
        ui.cbOutDevice->addItem(((SoundDevice) sd->at(i)).getName(),deviceid);
        if (deviceid.toInt()==defoutdevice) ui.cbOutDevice->setCurrentIndex(ui.cbOutDevice->count());  
    }
    
    ui.cbSaveAllRecordings->setCheckState((settings->value("saveallrecordings").toBool()) ? Qt::Checked : Qt::Unchecked);
    ui.leSaveRecordingsTo->setText(settings->value("pathtosaverecordings").toString());
    int channel=settings->value("channel").toInt();
    int j=0;
    while ((j<ui.cbChannels->count()) && (ui.cbChannels->itemData(j)!=channel))
    {
            j++;
    }
    if (j==ui.cbChannels->count())
    {
       QMessageBox::critical(this,"Lesen der Kan�le fehlgeschlagen","Beim Auslesen der Kan�le aus der Konfigurationsdatei ist ein Fehler aufgetreten");
       return;
    }
    ui.cbChannels->setCurrentIndex(j);
    ui.sbSamplerate->setValue(settings->value("samplerate").toInt());
    ui.hsMic->setValue(settings->value("volume").toInt());
   	

                    
}

void SettingsView::apply()
{    
     settings->setValue("simonautostart",ui.cbStartSimonOnBoot->checkState()==Qt::Checked);
     settings->setValue("juliusdautostart",ui.cbStartJuliusdOnBoot->checkState()==Qt::Checked);
     settings->setValue("juliusdrequired",ui.cbStartJuliusAsNeeded->checkState()==Qt::Checked);
     settings->setValue("askbeforeexit",ui.cbAskBeforeExit->checkState()==Qt::Checked);
     settings->setValue("setsaveallrecordings",ui.cbSaveAllRecordings->checkState()==Qt::Checked);
     settings->setValue("channel",ui.cbChannels->itemData(ui.cbChannels->currentIndex(),Qt::UserRole).toString()); 
     settings->setValue("samplerate",ui.sbSamplerate->value());
     settings->setValue("volume",ui.hsMic->sliderPosition());
     settings->setValue("defaultdeviceid",ui.cbInDevice->itemData(ui.cbInDevice->currentIndex(),Qt::UserRole).toString());
  
     settings->setValue("paths/lexicon",ui.leLexicon->text());
     settings->setValue("paths/grammar",ui.leGrammar->text());
     settings->setValue("paths/vocabul",ui.leVocab->text());
     settings->setValue("paths/prompts",ui.lePrompts->text());
     settings->setValue("paths/saverecordings",ui.leSaveRecordingsTo->text());

     settings->sync();  
}
void SettingsView::switchToSystem()
{
	unsetAllTabs();
	ui.pbSystemSettings->setChecked(true);
	ui.swSettings->setCurrentIndex( 0 );
     
}

/**
 * \brief Switches to the "sound" tab
 * 
 * \author Peter Grasch
 */
void SettingsView::switchToSound()
{
	unsetAllTabs();
    ui.pbSoundSettings->setChecked(true);
	ui.swSettings->setCurrentIndex( 1 );
	
}

/**
 * \brief Switches to the "command" tab
 * 
 * \author Peter Grasch
 */
void SettingsView::switchToCommands()
{
	unsetAllTabs();
	
	ui.pbCommandSettings->setChecked(true);
	ui.swSettings->setCurrentIndex( 2 );
}

/**
 * \brief Switches to the "command" tab
 * 
 * \author Peter Grasch
 */
void SettingsView::switchToProtocols()
{
	unsetAllTabs();
	
	ui.pbProtocolSettings->setChecked(true);
	ui.swSettings->setCurrentIndex( 3 );
}

/**
 * \brief Switches to the "history" tab
 * 
 * \author Peter Grasch
 */
void SettingsView::switchToHistory()
{
	unsetAllTabs();
	
	ui.pbRevert->setChecked(true);
	ui.swSettings->setCurrentIndex( 4 );
}


/**
 * \brief Destructor
 * 
 * \author Peter Grasch
 */
SettingsView::~SettingsView()
{
}


