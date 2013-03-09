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
#include <QTextStream>
#include <QDebug>

class ServerManager
{
private:
    QString CUSTOM_CONTENT_CONFIG_FILE;

public:
    ServerManager(QSettings* settings);
    virtual ~ServerManager();

    void launch();

    QString getServerName() { return m_serverName; }
    void setServerName(QString serverName) { m_serverName = serverName; m_settings->setValue("server/serverName", m_serverName); }
    int getMaxPlayers() { return m_maxPlayers; }
    void setMaxPlayers(int maxPlayers) { m_maxPlayers = maxPlayers; m_settings->setValue("server/maxPlayers", m_maxPlayers); }
    bool getGamemodePublic() { return m_gamemodePublic; }
    void setGamemodePublic(bool gamemodePublic) { m_gamemodePublic = gamemodePublic; m_settings->setValue("server/gamemodePublic", m_gamemodePublic); }
    QString getPassword() { return m_password; }
    void setPassword(QString password) { m_password = password; m_settings->setValue("server/password", m_password); }
    QString getAdminPassword() { return m_adminPassword; }
    void setAdminPassword(QString adminPassword) { m_adminPassword = adminPassword; m_settings->setValue("server/adminPassword", m_adminPassword); }
    bool getCustomContentConfig() { return m_customContentConfig; }
    void setCustomContentConfig(bool customContentConfig) {m_customContentConfig = customContentConfig; m_settings->setValue("server/customContentConfig", m_customContentConfig); }

private:
    QSettings* m_settings;

    QString m_serverName;
    int m_maxPlayers;
    bool m_gamemodePublic;
    QString m_password;
    QString m_adminPassword;
    bool m_customContentConfig;

    QProcess* m_serverProcess;
};

#endif // SERVERMANAGER_H
