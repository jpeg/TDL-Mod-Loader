/******************************************************************************
 * configmodifier.h
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
 * Desc: Class to handle modifying of config files. All
 *       config files use a [Mods] section to seperate
 *       mods section from the base game. The mod number
 *       defines load order, each should have a unique
 *       number with lower ones loading sooner.
 *
 * Interface:
 *   int init(const QString& versionFilename, const QString& pluginsFilename, const QString& resourceFilename, QVector<QString>& activeMods)
 *     Loads vanilla config from files and finds version
 *     number. Returns error code and vector of active
 *     mods.
 *
 *   int getVersion()
 *     Returns the current version read from the version
 *     file.
 *
 *   int addPlugin(int mod, const QString& modName, const QString& plugin)
 *     Adds plugin DLL filename string to plugins config
 *     of given mod if not already in list of mod plugins.
 *     Returns load order index of the mod.
 *
 *   int addResource(int mod, const QString& modName, const QString& resource)
 *     Adds resource directory string to resource config
 *     of given mod if not already in list of mod
 *     resources. Returns load order index of the mod.
 *
 *   int save()
 *     Writes current config out to files. Returns error
 *     code.
 *
 *   void swap(int mod1, int mod2)
 *     Swaps order of given mods.
 *
 *   void remove(int mod)
 *     Removes all config of given mod.
 *
 *   void removeAll()
 *     Removes config of all mods.
 ******************************************************************************/

#ifndef CONFIGMODIFIER_H
#define CONFIGMODIFIER_H

#include <QString>
#include <QVector>
#include <QFile>
#include <QTextStream>

#include "errorcodes.h"

class ConfigModifier
{    
public:
    ConfigModifier();
    virtual ~ConfigModifier();

public:
    QString MOD_HEADER; //pretend this is const
    QString MODE_MARKER;
    QString OPTION_MARKER;
    QString MODS_DIR;

private:
    int m_version;
    QString m_pluginsFilename;
    QString m_resourcesFilename;
    QVector<QString> m_vanillaPlugins;
    QVector<QString> m_vanillaResources;

    struct ModConfig
    {
        QString name;
        QVector<QString> plugins;
        QVector<QString> resources;
        int activeMode;
        QVector<bool> activeOptions;
    };
    QVector<ModConfig*> m_modList;

public:
    ErrorCode init(const QString& versionFilename, const QString& pluginsFilename, const QString& resourcesFilename,
                   QVector<QString>& activeMods, QVector<int>& activeModes, QVector<QVector<int>*>& activeOptions);
    int getVersion();
    int addPlugin(int mod, const QString& modName, const QString& plugin);
    int addResource(int mod, const QString& modName, const QString& resource);
    int setActiveMode(int mod, const QString& modName, int mode);
    int setActiveOptions(int mod, const QString& modName, QVector<bool>& options);
    ErrorCode save();
    void swap(int mod1, int mod2);
    void remove(int mod);
    void removeAll();

private:
    int addMod(int mod, const QString& modName);
};

#endif // CONFIGMODIFIER_H
