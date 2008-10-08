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

#include "desktopgridcommandmanager.h"
#include <simoncommandpluginbase_export.h>
#include <simonlogging/logger.h>
#include "screengrid.h"
#include <KLocalizedString>
#include <KGenericFactory>
#include <KMessageBox>
#include <QDebug>
#include "desktopgridconfiguration.h"

K_PLUGIN_FACTORY( DesktopGridPluginFactory, 
			registerPlugin< DesktopGridCommandManager >(); 
			/*registerPlugin< DesktopGridConfiguration >();*/ 
		)
        
K_EXPORT_PLUGIN( DesktopGridPluginFactory("DesktopGridCommandManager") )



DesktopGridCommandManager::DesktopGridCommandManager(QObject *parent, const QVariantList& args) : CommandManager(parent, args)
{
	DesktopGridConfiguration::getInstance(dynamic_cast<QWidget*>(parent), QVariantList())->load();
}

const QString DesktopGridCommandManager::name() const
{
	i18n("Desktopgitter");
}

CommandConfiguration* DesktopGridCommandManager::getConfigurationPage()
{
	return DesktopGridConfiguration::getInstance();
}

bool DesktopGridCommandManager::trigger(const QString& triggerName)
{
	if (triggerName != DesktopGridConfiguration::getInstance()->trigger()) return false;

	Logger::log(i18n("[INF] Aktiviere Desktopgitter"));
	ScreenGrid *screenGrid = new ScreenGrid();
	screenGrid->show();
	return true;
}

bool DesktopGridCommandManager::load()
{
	return true;
}

bool DesktopGridCommandManager::save()
{
	return true;
}
