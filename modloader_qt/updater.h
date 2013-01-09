/******************************************************************************
 * updater.h
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

#ifndef UPDATER_H
#define UPDATER_H

#include <QObject>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>

class Updater : public QObject
{
    Q_OBJECT

public:
    Updater();
    virtual ~Updater();

public:
    void checkVersion();

private slots:
    void networkManager_reply_finished(QNetworkReply* reply);

private:
    QNetworkAccessManager* m_networkManager;

    bool m_downloading;
    bool m_update;

    struct Version
    {
        int major;
        int minor;
        int maintenance;
        char revision;
    };
    Version* m_currentVersion;

private:
    Version* parseVersion(QString& version);
};

#endif // UPDATER_H
