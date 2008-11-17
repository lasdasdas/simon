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


#include "simoncontrol.h"

#include <simonactions/actionmanager.h>

#include "coreconfiguration.h"

#include <KMessageBox>
#include <KLocalizedString>
#include <simonlogging/logger.h>
#include <simoninfo/simoninfo.h>

/**
 * @brief Constructor
 * 
 * Sets the activitionstate to true
 *
 *	@author Peter Grasch
*/
SimonControl::SimonControl(QWidget *parent) : QObject (parent)
{
	setStatus(SimonControl::Disconnected);
	this->recognitionControl = new RecognitionControl(parent);
	QObject::connect(ActionManager::getInstance(), SIGNAL(guiAction(QString)), this, SIGNAL(guiAction(QString)));
	
	QObject::connect(recognitionControl, SIGNAL(connected()), this, SLOT(connectedToServer()));
	QObject::connect(recognitionControl, SIGNAL(disconnected()), this, SLOT(disconnectedFromServer()));
	
	QObject::connect(recognitionControl, SIGNAL(connectionError(const QString&)), this, SLOT(slotConnectionError(const QString&)));
	QObject::connect(recognitionControl, SIGNAL(simondSystemError(const QString&)), this, SLOT(slotSimondSystemError(const QString&)));
	QObject::connect(recognitionControl, SIGNAL(synchronisationError(const QString&)), this, SLOT(slotSynchronisationError(const QString&)));
	QObject::connect(recognitionControl, SIGNAL(recognitionError(const QString&)), this, SLOT(slotRecognitionError(const QString&)));
	QObject::connect(recognitionControl, SIGNAL(compilationError(const QString&)), this, SLOT(slotCompilationError(const QString&)));

	QObject::connect(recognitionControl, SIGNAL(simondSystemWarning(const QString&)), this, SLOT(slotSimondSystemWarning(const QString&)));
	QObject::connect(recognitionControl, SIGNAL(synchronisationWarning(const QString&)), this, SLOT(slotSynchronisationWarning(const QString&)));
	QObject::connect(recognitionControl, SIGNAL(recognitionWarning(const QString&)), this, SLOT(slotRecognitionWarning(const QString&)));
	QObject::connect(recognitionControl, SIGNAL(compilationWarning(const QString&)), this, SLOT(slotCompilationWarning(const QString&)));

	connect(recognitionControl, SIGNAL(modelCompilationWordUndefined(const QString&)), this, 
			SLOT(slotModelCompilationClassUndefined(const QString&)));
	connect(recognitionControl, SIGNAL(modelCompilationClassUndefined(const QString&)), this, 
			SLOT(slotModelCompilationWordUndefined(const QString&)));
	connect(recognitionControl, SIGNAL(modelCompilationPhonemeUndefined(const QString&)), this, 
			SLOT(slotModelCompilationPhonemeUndefined(const QString&)));
	
	QObject::connect(recognitionControl, SIGNAL(loggedIn()), this, SLOT(loggedIn()));
	
	QObject::connect(recognitionControl, SIGNAL(recognised(const QString&,const QString&,const QString&)), this, SLOT(wordRecognised(QString,QString,QString)));
	QObject::connect(recognitionControl, SIGNAL(recognitionStatusChanged(RecognitionControl::RecognitionStatus)), this, SLOT(recognitionStatusChanged(RecognitionControl::RecognitionStatus)));
}

bool SimonControl::askBeforeQuit()
{ return CoreConfiguration::askBeforeQuit(); }

void SimonControl::loggedIn()
{
	SimonInfo::showMessage(i18n("Benutzer authentifiziert"), 1500);
}

void SimonControl::slotConnectionError(const QString &err)
{
	KMessageBox::error(0, i18n("Verbindungsfehler: \n%1", err));
	setStatus(SimonControl::Disconnected);
}

void SimonControl::slotSimondSystemError(const QString &err)
{
	KMessageBox::error(0, i18n("Der Erkennungsserver liefert folgenden fatalen Fehler: \n%1", err));
}

void SimonControl::slotSynchronisationError(const QString &err)
{
	KMessageBox::error(0, i18n("Bei der Modellsynchronisation ist folgender Fehler aufgetreten: \n%1", err));	
}

void SimonControl::slotRecognitionError(const QString &err)
{
	KMessageBox::error(0, i18n("Bei der Erkennung ist folgender Fehler aufgetreten: \n%1", err));
}

void SimonControl::slotCompilationError(const QString &err)
{
	KMessageBox::error(0, i18n("Als der Server das Modell kompilieren wollte trat dieser Fehler auf: \n%1", err));
}


void SimonControl::slotSimondSystemWarning(const QString& warning)
{
	SimonInfo::showMessage(i18n("simond: %1", warning), 5000);
}

void SimonControl::slotSynchronisationWarning(const QString& warning)
{
	SimonInfo::showMessage(i18n("Modellsynchronisation: %1", warning), 5000);
}

void SimonControl::slotRecognitionWarning(const QString& warning)
{
	SimonInfo::showMessage(i18n("Erkennung: %1", warning), 5000);
}

void SimonControl::slotCompilationWarning(const QString& warning)
{
	SimonInfo::showMessage(i18n("Modellverwaltung: %1", warning), 5000);
}

void SimonControl::slotModelCompilationWordUndefined(const QString& word)
{
	if (KMessageBox::questionYesNoCancel(0, i18n("Beim Generieren des Sprachmodells wurde erkannt, dass das Wort \"%1\" in den Trainingsdaten vorkommt aber nicht phonetisch definiert ist.\n\nMöchten Sie das Wort jetzt hinzufügen?", word)) != KMessageBox::Yes)
		return;


}

void SimonControl::slotModelCompilationClassUndefined(const QString& undefClass)
{
}

void SimonControl::slotModelCompilationPhonemeUndefined(const QString& phoneme)
{
}



/**
 * @brief Connects to recognitionControl
 *
 *	@author Peter Grasch
 */
void SimonControl::connectToServer()
{
	setStatus(SimonControl::Connecting);
	recognitionControl->startConnecting();
}


/**
 * @brief disconnects from recognitionControl
 *
 *	@author Peter Grasch
 */
void SimonControl::disconnectFromServer()
{
	recognitionControl->disconnectFromServer();
}


/**
 * @brief Word recognised
 * 
 * usually called when the server recognised a word
 *
 *	@author Peter Grasch
 *	@param QString word
 *	the recognised word
 */
void SimonControl::wordRecognised(QString word,QString sampa, QString samparaw)
{
	Q_UNUSED(sampa);
	Q_UNUSED(samparaw);

	if (status != SimonControl::ConnectedActivated) return;
	
	kDebug() << "Recognized: " << word;
	
	word = word.remove("<s>");
	word = word.remove("</s>");
	ActionManager::getInstance()->process(word.trimmed());
}


void SimonControl::recognitionStatusChanged(RecognitionControl::RecognitionStatus status)
{
	switch (status)
	{
		case RecognitionControl::Ready:
		{
			setStatus(SimonControl::ConnectedDeactivatedReady);
			break;
		}
		
		case RecognitionControl::Started:
		{
			setStatus(SimonControl::ConnectedActivated);
			break;
		}
		
		case RecognitionControl::Paused:
		{
			setStatus(SimonControl::ConnectedPaused);
			break;
		}
		
		case RecognitionControl::Resumed:
		{
			setStatus(SimonControl::ConnectedActivated);
			break;
		}
		
		case RecognitionControl::Stopped:
		{
			setStatus(SimonControl::ConnectedDeactivatedNotReady);
			break;
		}
		
	}
}

void SimonControl::setStatus(SimonControl::SystemStatus status)
{
	this->status = status;
	emit systemStatusChanged(status);
}

/**
 * @brief Server connected
 *
 * This is just a feedback function provided to react to the fact that the
 * connection to the recognitionControl socket was established
 *
 * @author Peter Grasch
 */
void SimonControl::connectedToServer()
{
	setStatus(SimonControl::ConnectedDeactivatedNotReady);
	Logger::log(i18n("[INF]")+" "+i18n("Verbunden zu Server"));
}

/**
 * @brief Server disconnected
 *
 * This is just a feedback function provided to react to the fact that the
 * connection to the recognitionControl socket was lost
 *
 * @author Peter Grasch
 */
void SimonControl::disconnectedFromServer()
{
	setStatus(SimonControl::Disconnected);
	Logger::log(i18n("[INF] Verbindung von Server getrennt"));
}

/**
 * @brief We want to abort connecting to recognitionControl
 * 
 * @author Peter Grasch
 */
void SimonControl::abortConnecting()
{
	Logger::log(i18n("[INF] Verbinden abgebrochen"));
	this->recognitionControl->disconnectFromServer();
}



/**
 * @brief Toggles the activition
 *
 *	@author Peter Grasch
 */
SimonControl::SystemStatus SimonControl::toggleActivition()
{
	if (status==SimonControl::ConnectedActivated)
	{
		deactivateSimon();
	} else if ((status==SimonControl::ConnectedDeactivatedReady) || (status==SimonControl::ConnectedPaused))
		activateSimon();
	
	return status;
}

/**
 * @brief Activates Simon
 *
 *	@author Peter Grasch
 */
SimonControl::SystemStatus SimonControl::activateSimon()
{
	if (status == SimonControl::ConnectedDeactivatedReady)
	{
		Logger::log(i18n("[INF] Simon wird aktiviert"));
		setStatus(SimonControl::ConnectedActivating);
		recognitionControl->startRecognition();
	}
	if (status == SimonControl::ConnectedPaused)
	{
		Logger::log(i18n("[INF] Erkennung wird fortgesetzt"));
		setStatus(SimonControl::ConnectedResuming);
		recognitionControl->resumeRecognition();
	}
	return status;
}


/**
 * @brief Deactivates Simon
 *
 *	@author Peter Grasch
 */
SimonControl::SystemStatus SimonControl::deactivateSimon()
{
	if (status == SimonControl::ConnectedActivated)
	{
		setStatus(SimonControl::ConnectedDeactivating);
		Logger::log(i18n("[INF] Simon deaktiviert"));
		recognitionControl->pauseRecognition();
	}
	return status;
}

void SimonControl::compileModel()
{
	recognitionControl->startSynchronisation();
}

/**
 * @brief Destructor
 *
 *	@author Peter Grasch
 */
SimonControl::~SimonControl()
{
	recognitionControl->deleteLater();
//     delete eventHandler;
}
