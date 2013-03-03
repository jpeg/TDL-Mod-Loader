/******************************************************************************
 * servermanager.cpp
 *
 * Copyright (C) 2012-2013 Jason Gassel. All rights reserved.
 *
 * This file is part of the Jackal Mod Manager.
 *
 * Jackal Mod Manager is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Jackal Mod Manager is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Jackal Mod Manager.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "servermanager.h"

ServerManager::ServerManager(QSettings* settings)
{
    m_settings = settings;

    m_serverName = m_settings->value("server/serverName", "Jackal Mod Manager Server").toString();
    m_maxPlayers = m_settings->value("settings/onlineHelpUrl", 8).toInt();
    m_gamemodePublic = m_settings->value("server/gamemodePublic", true).toBool();
    m_usePassword = m_settings->value("server/usePassword", false).toBool();
    m_password = m_settings->value("server/password", "default").toString();
    m_adminPassword = m_settings->value("server/adminPassword", "default").toString();
    m_customContentConfig = m_settings->value("server/customContentConfig", false).toBool();
}

ServerManager::~ServerManager()
{
}

void ServerManager::launch()
{
    //TODO "TDLServerMain.exe" --dedicated --maxplayers=8 --gamemode=public --servername="Jackal Mod Manager Server" --adminpass="password" --contentconfig="joesContentConfig.xml"
}
