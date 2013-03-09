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

    CUSTOM_CONTENT_CONFIG_FILE = "jackalContentGenConfig.xml";

    m_serverName = m_settings->value("server/serverName", "Jackal Mod Manager Server").toString();
    m_maxPlayers = m_settings->value("server/maxPlayers", 8).toInt();
    m_gamemodePublic = m_settings->value("server/gamemodePublic", true).toBool();
    m_password = m_settings->value("server/password", "").toString();
    m_adminPassword = m_settings->value("server/adminPassword", "default").toString();
    m_customContentConfig = m_settings->value("server/customContentConfig", false).toBool();

    m_serverProcess = new QProcess;
}

ServerManager::~ServerManager()
{
}

void ServerManager::launch()
{
    QString exe = "start \"tst\" /D \"C:\\Sandswept Studios\\The Dead Linger Alpha\" TDLServerMain.exe --dedicated";
    QString maxPlayers;
    QTextStream maxPlayersStream(&maxPlayers);
    QString gamePath = m_settings->value("game/path").toString();
    for(int i=0; i<gamePath.length(); i++)
    {
        if(gamePath[i] == QChar('/'))
            gamePath[i] = QChar('\\');
    }
    maxPlayersStream << m_maxPlayers;
    QStringList args;
    args << "\"TDL Dedicated Server\""
         << "/D\"" + gamePath + "\""
         << "TDLServerMain.exe"
         << "--dedicated"
         << "--servername=\"" + m_serverName + "\""
         << "--maxplayers=" + maxPlayers
         << (m_gamemodePublic ? "--gamemode=public" : "--gamemode=protected")
         << "--adminpass=\"" + m_adminPassword + "\"";
    if(!m_password.isEmpty() && !m_password.isNull())
        args << "--password=\"" + m_password + "\"";
    if(m_customContentConfig)
        args << "--contentconfig=" + CUSTOM_CONTENT_CONFIG_FILE;
    m_serverProcess->start(exe);//, args);
    qDebug() << "Starting dedicated server:" << exe << args;
}
