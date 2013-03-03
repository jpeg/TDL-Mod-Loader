/******************************************************************************
 * servermanager.h
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

#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QSettings>
#include <QProcess>

class ServerManager
{
public:
    ServerManager(QSettings* settings);
    virtual ~ServerManager();

    void launch();

private:
    QSettings* m_settings;

    QString m_serverName;
    int m_maxPlayers;
    bool m_gamemodePublic;
    bool m_usePassword;
    QString m_password;
    QString m_adminPassword;
    bool m_customContentConfig;
};

#endif // SERVERMANAGER_H
