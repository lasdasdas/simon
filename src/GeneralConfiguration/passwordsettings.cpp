//
// C++ Implementation: passwordsettings
//
// Description: 
//
//
// Author: Peter Grasch <bedahr@gmx.net>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "passwordsettings.h"
#include "../SimonLib/Settings/settings.h"
#include <QCryptographicHash>

PasswordSettings::PasswordSettings(QWidget* parent): SystemWidget(tr("Passwort-Sicherung"), QIcon(":/images/icons/lock.svg"), tr("Sichern Sie simons Systemeinstellungen mit einem Passwort"), parent)
{
	ui.setupUi(this);
	help=tr("Wenn der Passwortschutz deaktiviert ist, muss nur auf den Knopf \"Systemverwaltungsmodus\" geklickt werden, damit alle (System-) Optionen angezeigt werden.");
	connect(ui.lePassword2, SIGNAL(editingFinished()), this, SLOT(checkPassword()));
	connect(ui.lePassword, SIGNAL(textChanged(QString)), this, SLOT(unsetHashed()));
	connect(ui.lePassword2, SIGNAL(textChanged(QString)), this, SLOT(unsetHashed()));
	connect(ui.pbResetPassword, SIGNAL(toggled(bool)), this, SLOT(resetButtonClicked(bool)));


	connect(ui.cbCheckPassword, SIGNAL(stateChanged(int)), this, SIGNAL(changed()));
	connect(ui.lePassword, SIGNAL(editingFinished()), this, SIGNAL(changed()));
	connect(ui.lePassword2, SIGNAL(editingFinished()), this, SIGNAL(changed()));
}

void PasswordSettings::checkPassword()
{
	if (!checkIfPasswordsSane())
		QMessageBox::critical(this, tr("Passwortschutz"), tr("Die eingegebenen Passw�rter des simon-Passwortschutz' stimmen nicht �berein.\n\nBitte geben Sie sie erneut ein."));
}

bool PasswordSettings::checkIfPasswordsSane()
{
	return (ui.lePassword->text() == ui.lePassword2->text());
}


PasswordSettings::~PasswordSettings()
{
}

void PasswordSettings::resetButtonClicked(bool isDown)
{
	if (isDown) {
		ui.lePassword->clear();
		ui.lePassword2->clear();
	} else
	{
		ui.lePassword->setText(originalHash);
		ui.lePassword2->setText(originalHash);
	}
}

bool PasswordSettings::apply()
{
	//password
	Settings::set("Passwordprotected", ui.cbCheckPassword->isChecked());
	if (checkIfPasswordsSane())
	{
		QString hash;
		if (!isHashed)
		{
			QCryptographicHash *hasher = new QCryptographicHash(QCryptographicHash::Md5);
			hasher->addData(ui.lePassword->text().toLatin1());
			hash = hasher->result();
		} else hash = ui.lePassword->text();

		Settings::set("Password", hash);
	} else {
		checkPassword(); //shows error
		return false;
	}
	if (ui.pbResetPassword->isChecked())
	{
		ui.pbResetPassword->animateClick();
	}

	return true;
}

bool PasswordSettings::isComplete()
{
	return (!ui.cbCheckPassword->isChecked() || ((checkIfPasswordsSane()) && (!ui.lePassword->text().isEmpty())));
}

bool PasswordSettings::init()
{
	//password
	ui.cbCheckPassword->setChecked(Settings::getB("Passwordprotected"));
	originalHash = Settings::getS("Password");
	ui.lePassword->setText(originalHash);
	ui.lePassword2->setText(originalHash);
	isHashed = true;
	return true;
}

bool PasswordSettings::reset()
{
	return init();
}

