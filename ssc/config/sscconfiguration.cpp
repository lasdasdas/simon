/*
 *   Copyright (C) 2008 Peter Grasch <peter.grasch@bedahr.org>
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

#include "sscconfiguration.h"
#include "sscconfig.h"
#include <KConfigSkeleton>
#include <KMessageBox>
#include <QSslSocket>
#include <QSslCipher>
#include "../../simonlib/sscdaccess/sscdaccess.h"
#include <sscdaccess/sscdaccesssingleton.h>

SSCConfiguration::SSCConfiguration(QWidget* parent, const QVariantList& args)
: KCModule(KGlobal::mainComponent(), parent)
{
  Q_UNUSED(args);

  ui.setupUi(this);

  addConfig(SSCConfig::self(), this);
  // 	connect(ui.cbCipher, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChanged()));
  connect(ui.cbAskForOfflineMode, SIGNAL(toggled(bool)), this, SLOT(slotChanged()));
  connect(ui.cbShowSampleWarning, SIGNAL(toggled(bool)), this, SLOT(slotChanged()));
}


void SSCConfiguration::slotChanged()
{
  emit changed(true);
}


void SSCConfiguration::load()
{
  SSCConfig::self()->config()->sync();
  // 	ui.cbCipher->clear();
  // 	QString selectedCipher = SSCConfiguration::encryptionMethod();
  //
  //         QList<QSslCipher> ciphers = QSslSocket::supportedCiphers();
  //         QStringList cipherStrs;
  //         QString cipherName;
  // 	int selectedIndex=0;
  //         for (int i=0; i < ciphers.count(); i++)
  // 	{
  // 		cipherName = ciphers[i].name();
  // 		if (cipherName == selectedCipher)
  // 			selectedIndex =i;
  // 		cipherStrs << cipherName;
  // 	}
  //
  // 	ui.cbCipher->addItems(cipherStrs);
  // 	ui.cbCipher->setCurrentIndex(selectedIndex);

  if (SSCConfig::useInstitutionSpecificIDs()) {
    ui.lbInstitute->setEnabled(true);
    ui.kcfg_ReferenceInstitute->setEnabled(true);
  }
  KCModule::load();

  KSharedConfig::Ptr config = KSharedConfig::openConfig("sscrc");
  KConfigGroup group(config, "Notification Messages");
  kDebug() << "Setting show samples warning: " << group.readEntry("ShowSampleWarning", false);
  ui.cbShowSampleWarning->setChecked(!group.readEntry("ShowSampleWarning", false));
  ui.cbAskForOfflineMode->setChecked(!group.readEntry("AskOfflineMode", false));
}


void SSCConfiguration::save()
{
  // 	SSCConfiguration::setEncryptionMethod(ui.cbCipher->currentText());

  KCModule::save();

  KSharedConfig::Ptr config = KSharedConfig::openConfig("sscrc");
  KConfigGroup group(config, "Notification Messages");
  if (ui.cbShowSampleWarning->isChecked())
    group.deleteEntry("ShowSampleWarning");
  else
    group.writeEntry("ShowSampleWarning", true);

  if (ui.cbAskForOfflineMode->isChecked()) {
    kDebug() << "Asking for offline mode is deleted";
    group.deleteEntry("AskOfflineMode");
  }else
  group.writeEntry("AskOfflineMode", true);
  config->sync();

  SSCConfig::setUseInstitutionSpecificIDs(ui.kcfg_UseInstitutionSpecificIDs->isChecked());
  SSCConfig::setReferenceInstitute(ui.kcfg_ReferenceInstitute->value());
  SSCConfig::self()->config()->sync();
  
  kDebug() << SSCConfig::self();
  kDebug() << "Getting sscd access...";
  SSCDAccessSingleton::getInstance()->setTimeout(SSCConfig::timeout());
 kDebug() << "Got sscd access...";
  kDebug() << "timeout synced";
  // 	if (ui.cbAskForOfflineMode->isChecked())
  // 	{
  // 		kDebug() << "Asking for offline mode is deleted before";
  // 		KMessageBox::information(this, "offline mode!");
  // 	} else
  // 		KMessageBox::information(this, "Not checked!");
}


SSCConfiguration::~SSCConfiguration()
{
}
