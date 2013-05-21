/******************************************************************************
 * configmodifier.cpp
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

#include "configmodifier.h"

ConfigModifier::ConfigModifier()
{
    MOD_HEADER = "#[Mods]"; //not commenting out will result in TDL ignoring mods section
    MODE_MARKER = "#MODE ";
    OPTION_MARKER = "#OPTION ";
    MODS_DIR = "content/exported/Mods/";
    m_version = -1;
}

ConfigModifier::~ConfigModifier()
{
    // Clean up mod list
    for(int i=0; i<m_modList.size(); i++)
        delete m_modList[i];
}

ErrorCode ConfigModifier::init(const QString &versionFilename, const QString &pluginsFilename,
                               const QString &resourcesFilename, QVector<QString>& activeMods,
                               QVector<int>& activeModes, QVector<QVector<int>*>& activeOptions)
{
    // Read version
    ErrorCode versionError = Error::NO_ERROR;
    QFile versionFile(versionFilename);
    if(!versionFile.open(QIODevice::ReadOnly | QIODevice::Text))
        versionError = Error::FAILED_OPEN_VERSION_FILE;
    else
    {
        QTextStream versionIn(&versionFile);
        m_version = -1;
        if(!versionFile.atEnd())
            versionIn >> m_version;
    }
    versionFile.close();

    // Load config files
    m_pluginsFilename = pluginsFilename;
    m_resourcesFilename = resourcesFilename;
    QFile vanillaPluginsFile(m_pluginsFilename);
    QFile vanillaResourcesFile(m_resourcesFilename);
    if(!vanillaPluginsFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return Error::FAILED_OPEN_PLUGINS_FILE;
    if(!vanillaResourcesFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return Error::FAILED_OPEN_RESOURCES_FILE;
    QTextStream pluginsIn(&vanillaPluginsFile);
    QTextStream resourcesIn(&vanillaResourcesFile);

    while(!pluginsIn.atEnd())
    {
        QString line = pluginsIn.readLine();
        if(line.contains(MOD_HEADER, Qt::CaseInsensitive))
            break; //figure out loaded plugins based on resources file
        m_vanillaPlugins.push_back(line);
    }

    bool vanilla = true;
    while(!resourcesIn.atEnd())
    {
        QString line = resourcesIn.readLine();
        if(line.contains(MOD_HEADER, Qt::CaseInsensitive))
            vanilla = false;

        if(vanilla)
            m_vanillaResources.push_back(line);
        else
        {
            // Load into mod list since enabled
            bool mode = false;
            if(line.contains(MODE_MARKER, Qt::CaseInsensitive))
            {
                mode = true;
            }

            // Set a mode/option as active
            if(mode || line.contains(OPTION_MARKER, Qt::CaseInsensitive))
            {
                // Find substrings for mod and mode/option number
                line.remove(0, (mode ? MODE_MARKER.length() : OPTION_MARKER.length()));
                int index = 0;
                while(line[index] != QChar('/'))
                    index++;
                QString num = line;
                line.truncate(index); //mod name
                num.remove(0, index+1); //mode/option number

                int modIndex = -1;

                // Add the mod to our list of active mods
                if(!activeMods.contains(line))
                {
                    modIndex = activeMods.size();
                    activeMods.push_back(line); //mod name
                    activeModes.push_back(0);
                    activeOptions.push_back(new QVector<int>);
                }

                // Find index in active mod list
                if(modIndex == -1)
                {
                    for(int i=0; i<activeMods.size(); i++)
                    {
                        if(activeMods[i] == line)
                        {
                            modIndex = i;
                            break;
                        }
                    }
                }

                // Store the mode/option as active
                if(modIndex != -1)
                {
                    if(mode)
                        activeModes[modIndex] = num.toInt();
                    else
                        activeOptions[modIndex]->push_back(num.toInt());
                }
            }
        }
    }

    vanillaPluginsFile.close();
    vanillaResourcesFile.close();

    return versionError;
}

int ConfigModifier::getVersion()
{
    return m_version;
}

int ConfigModifier::addMod(int mod, const QString& modName)
{
    if(!(mod < m_modList.size() && mod >= 0))
    {
        ModConfig* m = new ModConfig;
        m->name = modName;
        m_modList.push_back(m);
        mod = m_modList.size() - 1; //id for new mod
    }

    return mod;
}

int ConfigModifier::addPlugin(int mod, const QString& modName, const QString& plugin)
{
    if(mod < m_modList.size() && mod >= 0)
        m_modList[mod]->name = modName;
    else
        mod = addMod(mod, modName);

    m_modList[mod]->plugins.push_back(plugin);
    return mod;
}

int ConfigModifier::addResource(int mod, const QString &modName, const QString &resource)
{
    if(mod < m_modList.size() && mod >= 0)
        m_modList[mod]->name = modName;
    else
        mod = addMod(mod, modName);

    m_modList[mod]->resources.push_back(resource);
    return mod;
}

int ConfigModifier::setActiveMode(int mod, const QString& modName, int mode)
{
    if(mod < m_modList.size() && mod >= 0)
        m_modList[mod]->name = modName;
    else
        mod = addMod(mod, modName);

    m_modList[mod]->activeMode = mode;
    return mod;
}

int ConfigModifier::setActiveOptions(int mod, const QString& modName, QVector<bool>& options)
{
    if(mod < m_modList.size() && mod >= 0)
        m_modList[mod]->name = modName;
    else
        mod = addMod(mod, modName);

    m_modList[mod]->activeOptions.clear();
    for(int i=0; i<options.size(); i++)
        m_modList[mod]->activeOptions.push_back(options[i]);
    return mod;
}

ErrorCode ConfigModifier::save()
{
    QFile pluginsFile(m_pluginsFilename);
    QFile resourcesFile(m_resourcesFilename);
    if(!pluginsFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return Error::FAILED_OPEN_PLUGINS_FILE;
    if(!resourcesFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return Error::FAILED_OPEN_RESOURCES_FILE;
    QTextStream pluginsOut(&pluginsFile);
    QTextStream resourcesOut(&resourcesFile);

    // Output vanilla
    for(int i=0; i<m_vanillaPlugins.size(); i++)
        pluginsOut << m_vanillaPlugins[i] << endl;
    for(int i=0; i<m_vanillaResources.size(); i++)
        resourcesOut << m_vanillaResources[i] << endl;

    // Output mod header
    if(m_vanillaPlugins.back() != MOD_HEADER)
        pluginsOut << MOD_HEADER << endl;
    if(m_vanillaResources.back() != MOD_HEADER)
        resourcesOut << MOD_HEADER << endl;

    // Output mods
    for(int i=0; i<m_modList.size(); i++)
    {
        // Plugins
        for(int j=0; j<m_modList[i]->plugins.size(); j++)
            pluginsOut << "Plugin=" << m_modList[i]->plugins[j] << endl;

        // Mode marker
        resourcesOut << MODE_MARKER << m_modList[i]->name << "/" << m_modList[i]->activeMode << endl;

        // Options markers
        for(int j=0; j<m_modList[i]->activeOptions.size(); j++)
        {
            if(m_modList[i]->activeOptions[j])
                resourcesOut << OPTION_MARKER << m_modList[i]->name << "/" << j << endl;
        }

        // Resources
        for(int j=0; j<m_modList[i]->resources.size(); j++)
            resourcesOut << "FileSystem=./" << MODS_DIR << m_modList[i]->name << "/data/" << m_modList[i]->resources[j] << endl;
    }

    pluginsFile.close();
    resourcesFile.close();

    return Error::NO_ERROR;
}

void ConfigModifier::swap(int mod1, int mod2)
{
    if(mod1 != mod2 && mod1 < m_modList.size() && mod2 < m_modList.size() && mod1 >= 0 && mod2 >= 0)
    {
        ModConfig* temp = m_modList[mod1];
        m_modList[mod1] = m_modList[mod2];
        m_modList[mod2] = temp;
    }
}

void ConfigModifier::remove(int mod)
{
    if(mod < m_modList.size() && mod >= 0)
    {
        delete m_modList[mod];
        m_modList.remove(mod);
    }
}

void ConfigModifier::removeAll()
{
    for(int i=0; i<m_modList.size(); i++)
        delete m_modList[i];
    m_modList.clear();
}
