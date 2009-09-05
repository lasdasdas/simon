/*
 *   Copyright (C) 2008 Peter Grasch <grasch@simon-listens.org>
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


#include "soundsettings.h"
#include "soundcontrol.h"

#include "simoncombobox.h"
#include "simonspinbox.h"
#include "soundconfig.h"

#include <adinstreamer/adinstreamer.h>

#include <KMessageBox>
#include <QVBoxLayout>
#include <KIcon>

#include <KLocalizedString>
#include <KAboutData>
#include <KPageWidgetItem>
#include <KPageWidget>
#include <KDebug>
#include <kgenericfactory.h>

K_PLUGIN_FACTORY( SoundSettingsFactory, 
			registerPlugin< SoundSettings >(); 
		)
        
K_EXPORT_PLUGIN( SoundSettingsFactory("simonlib") )

/**
 * \brief Constructor - inits the help text and the gui
 * \author Peter Grasch
 * @param parent the parent of the widget
 */
SoundSettings::SoundSettings(QWidget* parent, const QVariantList& args):
		KCModule(KGlobal::mainComponent(), parent),
	sc(new SoundControl()),
	enabled(true)
{
	Q_UNUSED(args);

	QVBoxLayout *lay = new QVBoxLayout(this);
	KPageWidget *pageWidget = new KPageWidget(this);
	lay->addWidget(pageWidget);

	if (args.count() == 1)
		pageWidget->setFaceType(KPageView::Tabbed);
	
	QWidget *coreConfig = new QWidget(this);
	deviceUi.setupUi(coreConfig);
	deviceUi.pbTest->setIcon(KIcon("help-hint"));
	deviceUi.pbReload->setIcon(KIcon("view-refresh"));
	connect(deviceUi.pbTest, SIGNAL(clicked()), this, SLOT(checkWithSuccessMessage()));
	connect(deviceUi.pbReload, SIGNAL(clicked()), this, SLOT(load()));
	connect(deviceUi.cbSoundInputDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChanged()));
	connect(deviceUi.cbSoundOutputDevice, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChanged()));
	
	QWidget *postProcessingConfig = new QWidget(this);
	postProcUi.setupUi(postProcessingConfig);

	QWidget *promptConfig = new QWidget(this);
	promptUi.setupUi(promptConfig);

	KPageWidgetItem *deviceConfItem = pageWidget->addPage(coreConfig, i18n("Device Configuration"));
	KPageWidgetItem *promptConfItem = pageWidget->addPage(promptConfig, i18n("Prompt Font"));
	KPageWidgetItem *postProcConfItem = pageWidget->addPage(postProcessingConfig, i18n("Post-Processing"));

	deviceConfItem->setIcon(KIcon("audio-card"));
	promptConfItem->setIcon(KIcon("draw-text"));
	postProcConfItem->setIcon(KIcon("applications-other"));

	deviceConfItem->setHeader("");
	promptConfItem->setHeader("");
	postProcConfItem->setHeader("");
	
	KAboutData *about = new KAboutData(
				"soundsettings", "", ki18n("Recordings"),
				"0.1", ki18n("Configuration for the Recording and Playback of sounds"), KAboutData::License_GPL);
#if KDE_IS_VERSION(4,0,80)
	about->setProgramIconName("preferences-desktop-sound");
#endif
	setAboutData( about );

	addConfig(SoundConfiguration::self(), this);

	connect(AdinStreamer::getInstance(), SIGNAL(audioDeviceError()), this, SLOT(show()));
}



void SoundSettings::checkWithSuccessMessage()
{
	if (check())
		KMessageBox::information(this, i18n("The soundconfiguration has been tested successfully."));
}

/**
 * \author Peter Grasch
 * \brief Asks the SoundControl for the needed infos and inserts the data in the comboboxes
 * \return success
 */
void SoundSettings::load()
{
#ifdef Q_OS_LINUX
	AdinStreamer::getInstance()->stopSoundStream();
#endif

	deviceUi.cbSoundInputDevice->clear();
	deviceUi.cbSoundOutputDevice->clear();

	SoundDeviceList *in = sc->getInputDevices();

	deviceUi.cbSoundInputDevice->clear();
	for ( int i=0; i<in->count(); i++ )
	{
		int deviceid= in->at(i).getDeviceID();
		deviceUi.cbSoundInputDevice->addItem (in->at(i).getName(),deviceid );
	}
	int paInputDevice = SoundConfiguration::soundInputDevice();
	int inputDevice = deviceUi.cbSoundInputDevice->findData(paInputDevice);

	SoundDeviceList *out = sc->getOutputDevices();
	deviceUi.cbSoundOutputDevice->clear();
	for ( int i=0; i<out->count(); i++ )
	{
		int deviceid= out->at (i).getDeviceID();
		deviceUi.cbSoundOutputDevice->addItem (out->at(i).getName(),deviceid );
	}
	int paOutputDevice = SoundConfiguration::soundOutputDevice();
	int outputDevice = deviceUi.cbSoundOutputDevice->findData(paOutputDevice);


#ifdef Q_OS_UNIX
	bool hasChanged=false;

	KSharedConfig::Ptr config = KSharedConfig::openConfig("simonsoundrc");
	KConfigGroup group(config, "Devices");
	QString inputALSAName = group.readEntry("SoundInputDeviceALSAName", "");
	QString outputALSAName = group.readEntry("SoundOutputDeviceALSAName", "");

	if ( ((!inputALSAName.isEmpty()) && (inputALSAName != sc->idToALSAName(paInputDevice))) ||
		((!outputALSAName.isEmpty()) && (outputALSAName != sc->idToALSAName(paOutputDevice))) )
	{
		if (KMessageBox::questionYesNoCancel(this, i18n("simon noticed that not all of the sound devices you selected to use previously are available.\n\nThis is perfectly normal if you are connected to simond or are otherwise using an application that uses an ALSA device directly.\n\nDid you plug / unplug a device or otherwise change your systems audio setup?\n\nSelecting \"Yes\" will allow you to change your soundconfiguration, essentially deleting your previous configuration. Selecting \"No\" will temporarily deactivate the sound configuration in order to protect your previous configuration from being overwritten.")) == KMessageBox::Yes)
		{
			inputDevice = deviceUi.cbSoundInputDevice->findData(SoundControl::getDefaultInputDevice());
			outputDevice = deviceUi.cbSoundOutputDevice->findData(SoundControl::getDefaultOutputDevice());
			hasChanged=true;
			KMessageBox::information(this, i18n("Please adjust your soundconfiguration accordingly."));
			enable();
		} else disable();

	} else enable();

#endif

	deviceUi.cbSoundInputDevice->setCurrentIndex(inputDevice);
	deviceUi.cbSoundOutputDevice->setCurrentIndex(outputDevice);

	KCModule::load();

#ifdef Q_OS_UNIX
	if (hasChanged) emit changed(true);
	AdinStreamer::getInstance()->restartSoundStream();
#endif
}

void SoundSettings::enable()
{
	deviceUi.lbInDevice->setEnabled(true);
	deviceUi.cbSoundInputDevice->setEnabled(true);

	deviceUi.lbOutDevice->setEnabled(true);
	deviceUi.cbSoundOutputDevice->setEnabled(true);

	deviceUi.kcfg_SoundChannels->setEnabled(true);
	deviceUi.kcfg_SoundSampleRate->setEnabled(true);
	deviceUi.lbHz->setEnabled(true);
	deviceUi.lbChannels->setEnabled(true);
	deviceUi.lbSamplerate->setEnabled(true);

	deviceUi.pbTest->setEnabled(true);
	enabled=true;
}

void SoundSettings::disable()
{
	deviceUi.lbInDevice->setEnabled(false);
	deviceUi.cbSoundInputDevice->setEnabled(false);

	deviceUi.lbOutDevice->setEnabled(false);
	deviceUi.cbSoundOutputDevice->setEnabled(false);

	deviceUi.kcfg_SoundChannels->setEnabled(false);
	deviceUi.kcfg_SoundSampleRate->setEnabled(false);
	deviceUi.lbHz->setEnabled(false);
	deviceUi.lbChannels->setEnabled(false);
	deviceUi.lbSamplerate->setEnabled(false);

	deviceUi.pbTest->setEnabled(false);
	enabled=false;
}


bool SoundSettings::check()
{
	int inputDevice = getSelectedInputDeviceId();
	int outputDevice = getSelectedOutputDeviceId();
	int channels = deviceUi.kcfg_SoundChannels->value();
	int samplerate = deviceUi.kcfg_SoundSampleRate->value();

	bool ok = this->sc->checkDeviceSupport(inputDevice, outputDevice, channels, samplerate);

	if (!ok)
		KMessageBox::error(this, i18n("The selected sound configuration is not supported by your hardware.\n\nPlease double-check your configuration and, if necessairy, please contact your vendor."));

	return ok;
}

int SoundSettings::getSelectedInputDeviceId()
{
	return deviceUi.cbSoundInputDevice->itemData(deviceUi.cbSoundInputDevice->currentIndex()).toInt();
}

int SoundSettings::getSelectedOutputDeviceId()
{
	return deviceUi.cbSoundOutputDevice->itemData(deviceUi.cbSoundOutputDevice->currentIndex()).toInt();
}

void SoundSettings::save()
{
	//even when not enabled this call will be save
	//The sound input / output devices are not affected by this
	//and this way we store the configuration regarding the prompt font
	//and the postprocessing commands
	KCModule::save();

	if (!enabled) return;

	check();
	SoundConfiguration::setSoundInputDevice(getSelectedInputDeviceId());
	SoundConfiguration::setSoundOutputDevice(getSelectedOutputDeviceId());

	KSharedConfig::Ptr config = KSharedConfig::openConfig("simonsoundrc");
	KConfigGroup group(config, "Devices");
	group.writeEntry("SoundInputDevice", getSelectedInputDeviceId());
	group.writeEntry("SoundOutputDevice", getSelectedOutputDeviceId());
#ifdef Q_OS_LINUX
	group.writeEntry("SoundInputDeviceALSAName", sc->idToALSAName(getSelectedInputDeviceId()));
	group.writeEntry("SoundOutputDeviceALSAName", sc->idToALSAName(getSelectedOutputDeviceId()));
#endif
	config->sync();

	AdinStreamer::getInstance()->stopSoundStream();
	AdinStreamer::getInstance()->restartSoundStream();
}

void SoundSettings::slotChanged()
{
	emit changed(true);
}

/**
 * \brief Destructor
 * \author Peter Grasch
 */
SoundSettings::~SoundSettings()
{
	if (sc) delete sc;
}


