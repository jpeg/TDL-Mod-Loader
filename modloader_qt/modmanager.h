/******************************************************************************
 * modmanager.h
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
 *
 *
 * Desc: Class to manage and install mods.
 *
 * Interface:
 *   ModManager(const QString& gameDirectory, const QString& dataDirectory)
 *     Checks if given base game and data directories are valid by looking to
 *     see if the game executable exists is in the game directory. Creates the
 *     mods directory if it doesn't already exist.
 *
 *   ErrorCode install(const QString& modArchivePath)
 *     Copies all files from the given mod archive into the mods directory,
 *     error code value returned. For now only ZIP format archive files will
 *     be supported.
 *
 *   ErrorCode remove(int mod)
 *     Deletes a mod from disk and removes it from the mod lists. Returns an
 *     error code value.
 *
 *   ErrorCode load()
 *     Loads all mods in mods directory into a list and figures out which are
 *     enabled from game config files. Returns an error code value.
 *
 *   int getVersion()
 *     Returns the game version from game config.
 *
 *   QVector<Mod*>* getMods()
 *     Returns a pointer to the list of mods.
 *
 *   QVector<Mod*>* getEnabledModOrder()
 *     Returns a pointer to the list of enabled mods.
 *
 *   ErrorCode enableMod(int mod)
 *     Adds mod to the list of enabled mods and enables it in the config
 *     manager. Deletes the script cache if mod requires it. Returns an error
 *     code value.
 *
 *   ErrorCode disableMod(int mod)
 *     Removes mod from the list of enabled mods and removes it from the config
 *     manager files. Returns an error code value.
 *
 *   ErrorCode modOrderUp(int mod)
 *     Moves mod up in enabled mods list and in config manager. Returns an
 *     error code value.
 *
 *   ErrorCode modOrderDown(int mod)
 *     Moves mod down in enabled mods list and in config manager. Returns an
 *     error code value.
 *
 *   ErrorCode save()
 *     Saves the currently enabled mods to game config. Returns an error code
 *     value.
 *
 *   ErrorCode refreshWorld()
 *    Deletes the game world save directory.
 *
 *   ErrorCode refreshInventory()
 *     Deletes the game inventory save directory.
 ******************************************************************************/

#ifndef MODMANAGER_H
#define MODMANAGER_H

#include <QString>
#include <QVector>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <QXmlStreamReader>

#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include "quazip/quazipdir.h"

#include "configmodifier.h"
#include "errorcodes.h"

class ModManager
{
public:
    ModManager(const QString& gameDirectory, const QString& dataDirectory);
    virtual ~ModManager();

private:
    QString INSTALL_CONFIG_FILE;
    QString DATA_SUB_DIR;
    QString VERSION_FILE;
    QString PLUGINS_FILE;
    QString RESOURCES_FILE;
    QString MODS_DIR;
    QString WORLDS_DIR;
    QString INVENTORY_DIR;
    QVector<QString> DISALLOWED_PLUGIN_FILENAMES;

public:
    struct Mod
    {
        struct Optional
        {
            bool optionEnabled; //for options only, not modes
            QString prettyName;
            QVector<QString> plugins;
            QVector<QString> resources;
        };

        bool enabled;
        QString name;
        QString prettyName;
        QString author;
        QString version;
        int gameVersion;
        QString description;
        QVector<QString> commonPlugins;
        QVector<QString> commonResources;
        int enabledMode;
        QVector<Optional*> modes;
        QVector<Optional*> options;
        bool refreshScriptCache;
        bool refreshWorld;
        bool refreshInventory;
        QPixmap icon;
    };

private:
    QString m_gameDir;
    bool m_gameDirValid;
    QString m_dataDir;
    bool m_dataDirValid;
    bool m_loaded;

    ConfigModifier* m_gameConfig;

    QVector<Mod*> m_mods;
    QVector<Mod*> m_enabledModOrder;

public:
    bool checkGameDirectory(const QString& gameDirectory);
    bool checkDataDirectory(const QString& dataDirectory);
    ErrorCode install(const QString& modArchivePath, int& existingMod);
    ErrorCode remove(int mod, bool keepSpot = false);
    ErrorCode load();
    bool getGameDirectoryValid();
    int getVersion();
    QVector<Mod*>* getMods();
    QVector<Mod*>* getEnabledModOrder();
    ErrorCode enableMod(int mod);
    ErrorCode disableMod(int mod);
    ErrorCode modOrderUp(int mod);
    ErrorCode modOrderDown(int mod);
    ErrorCode save();
    ErrorCode refreshWorld();
    ErrorCode refreshInventory();

private:
    ErrorCode parseModXmlConfig(Mod *modPtr, QXmlStreamReader *modConfigIn);
    int installPlugin(int modIndex, const QString& modName, const QString& plugin);
    void addModGameConfig(int enabledMod, Mod* modPtr);
    bool deleteDir(const QString& path);
    ErrorCode createArchiveDirectoryStructure(QuaZipDir* currentDir, QDir* destinationDir);
};

#endif // MODMANAGER_H
