/******************************************************************************
 * updater.cpp
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

#include "updater.h"

Updater::Updater()
{
    m_networkManager = new QNetworkAccessManager(this);
    connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(networkManager_reply_finished(QNetworkReply*)));

    m_downloading = false;
    m_update = false;

    QString currentVersion = QCoreApplication::applicationVersion();
    m_currentVersion = parseVersion(currentVersion);
}

Updater::~Updater()
{
    delete m_networkManager;
}

void Updater::checkVersion()
{
    m_networkManager->get(QNetworkRequest(QUrl("http://dl.dropbox.com/u/24782509/TDL/Jackal/latest.txt"))); //TODO don't hardcode...
    m_downloading = true;
}

void Updater::networkManager_reply_finished(QNetworkReply* reply)
{
    QString data = reply->readAll();
    Version* receivedVersion = parseVersion(data);
    qDebug() << "Current version:" << m_currentVersion->major << '.' << m_currentVersion->minor << '.'
             << m_currentVersion->maintenance << (int)m_currentVersion->revision;
    qDebug() << "Received latest version:" << receivedVersion->major << '.' << receivedVersion->minor << '.'
             << receivedVersion->maintenance << (int)receivedVersion->revision;

    if(receivedVersion->major > m_currentVersion->major)
        m_update = true;
    else if(receivedVersion->major == m_currentVersion->major)
    {
        if(receivedVersion->minor > m_currentVersion->minor)
            m_update = true;
        else if(receivedVersion->minor == m_currentVersion->minor)
        {
            if(receivedVersion->maintenance > m_currentVersion->maintenance)
                m_update = true;
            else if(receivedVersion->maintenance == m_currentVersion->maintenance)
            {
                if((int)receivedVersion->revision > (int)m_currentVersion->revision)
                    m_update = true;
            }
        }
    }

    if(m_update)
    {
        QString newVersionString;
        QTextStream newVersionIn(&newVersionString);
        newVersionIn << "There is a newer version of " << QCoreApplication::applicationName() << " available: "
                     << receivedVersion->major << QChar('.') << receivedVersion->minor << QChar('.')
                     << receivedVersion->maintenance << receivedVersion->revision;
        QMessageBox::information(NULL, "Update Available", newVersionString, QMessageBox::Ok, QMessageBox::Ok);
    }

    m_downloading = false;
    reply->deleteLater();
}

Updater::Version* Updater::parseVersion(QString& version)
{
    QTextStream versionIn(&version);
    Version* result = new Version;
    QChar temp;
    versionIn >> result->major >> temp >> result->minor >> temp >> result->maintenance >> result->revision;

    return result;
}
