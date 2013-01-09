/******************************************************************************
 * modmanager.cpp
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

#include "modmanager.h"

ModManager::ModManager(const QString& gameDirectory, const QString& dataDirectory)
{
    m_gameConfig = new ConfigModifier;

#ifdef WIN32
    switch(QSysInfo::windowsVersion())
    {
    case QSysInfo::WV_2000:
    case QSysInfo::WV_XP:
        DATA_SUB_DIR = "Sandswept Studios/The Dead Linger/";
        break;
    case QSysInfo::WV_VISTA:
    case QSysInfo::WV_WINDOWS7:
    case QSysInfo::WV_WINDOWS8:
    default:
        DATA_SUB_DIR = "Roaming/Sandswept Studios/The Dead Linger/";
        break;
    }
#endif

    VERSION_FILE = DATA_SUB_DIR + "tdlversion.txt";
    PLUGINS_FILE = "Plugins.cfg";
    RESOURCES_FILE = "content/resources.cfg";
    MODS_DIR = m_gameConfig->MODS_DIR;
    WORLDS_DIR = DATA_SUB_DIR + "SavedWorlds";
    INVENTORY_DIR = DATA_SUB_DIR + "inventory";

    DISALLOWED_PLUGIN_FILENAMES.push_back("cg");
    DISALLOWED_PLUGIN_FILENAMES.push_back("libmysql");
    DISALLOWED_PLUGIN_FILENAMES.push_back("OgreMain");
    DISALLOWED_PLUGIN_FILENAMES.push_back("OgreOggSound");
    DISALLOWED_PLUGIN_FILENAMES.push_back("OgrePaging");
    DISALLOWED_PLUGIN_FILENAMES.push_back("OgreProperty");
    DISALLOWED_PLUGIN_FILENAMES.push_back("OgreRTShaderSystem");
    DISALLOWED_PLUGIN_FILENAMES.push_back("OIS");
    DISALLOWED_PLUGIN_FILENAMES.push_back("Plugin_BSPSceneManager");
    DISALLOWED_PLUGIN_FILENAMES.push_back("Plugin_CgProgramManager");
    DISALLOWED_PLUGIN_FILENAMES.push_back("Plugin_OctreeSceneManager");
    DISALLOWED_PLUGIN_FILENAMES.push_back("Plugin_OctreeZone");
    DISALLOWED_PLUGIN_FILENAMES.push_back("Plugin_ParticleFX");
    DISALLOWED_PLUGIN_FILENAMES.push_back("Plugin_PCZSceneManager");
    DISALLOWED_PLUGIN_FILENAMES.push_back("Plugin_ScriptSerializer");
    DISALLOWED_PLUGIN_FILENAMES.push_back("RenderSystem_Direct3D9");
    DISALLOWED_PLUGIN_FILENAMES.push_back("RenderSystem_GL");

    checkGameDirectory(gameDirectory);
    checkDataDirectory(dataDirectory);

    m_loaded = false;
}

ModManager::~ModManager()
{
    delete m_gameConfig;
    for(int i=0; i<m_mods.size(); i++)
        delete m_mods[i];
}

bool ModManager::checkGameDirectory(const QString& gameDirectory)
{
    m_gameDir = gameDirectory;
    m_gameDirValid = true;

    // Check if gameDir valid, may also (hopefully) fail if TDL is currently running?
    if(m_gameDir[m_gameDir.size()-1] != QChar('/') && m_gameDir[m_gameDir.size()-1] != QChar('\\'))
        m_gameDir.append("/");
    QFile gameFile(m_gameDir + "TDL.exe");
    if(!gameFile.open(QIODevice::ReadOnly))
        m_gameDirValid = false;
    gameFile.close();
    QDir gameDir(m_gameDir);
    gameDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    if(!gameDir.cd(MODS_DIR))
    {
        if(!gameDir.mkdir(MODS_DIR))
            m_gameDirValid = false;
    }

    return m_gameDirValid;
}

bool ModManager::checkDataDirectory(const QString& dataDirectory)
{
    m_dataDir = dataDirectory;
    m_dataDirValid = true;

    QDir dataDir("");
    if(!dataDir.cd(m_dataDir))
    {
        m_dataDirValid = false;
    }

    return m_dataDirValid;
}

ErrorCode ModManager::install(const QString& modArchivePath)
{
    qDebug() << "Installing mod" << modArchivePath;

    if(m_gameDirValid)
    {
        QuaZip zip(modArchivePath);
        if(!zip.open(QuaZip::mdUnzip))
            return Error::FAILED_OPEN_MOD_ARCHIVE;

        // Read mod name from install.cfg in ZIP file
        QuaZipFile configFile(&zip);
        zip.setCurrentFile("install.cfg", QuaZip::csInsensitive);
        Mod* newMod = new Mod;
        ErrorCode error = Error::INVALID_MOD_ARCHIVE_CONFIG;
        if(configFile.open(QIODevice::ReadOnly) && configFile.getZipError() == UNZ_OK)
        {
            QTextStream configIn(&configFile);
            error = parseModConfig(newMod, &configIn);
        }
        configFile.close();
        if(error != Error::NO_ERROR)
        {
            delete newMod;
            return error;
        }

        // If a version of this mod already is installed remove it
        int existingMod = -1;
        for(int i=0; i<m_mods.size(); i++)
        {
            if(m_mods[i]->name == newMod->name)
            {
                remove(i, true);
                existingMod = i;
                break;
            }
        }

        QString modDirectory = m_gameDir + MODS_DIR + newMod->name + QChar('/');

        // Build directory structure in mods/ dir
        QuaZipDir currentDir(&zip);
        QDir destinationDir(m_gameDir + MODS_DIR);
        destinationDir.mkpath(modDirectory);
        destinationDir.cd(newMod->name);
        createArchiveDirectoryStructure(&currentDir, &destinationDir);

        // Move files from ZIP to mods/ dir
        QuaZipFile currentFile(&zip);
        QFile destinationFile;
        bool currentFileValid = zip.goToFirstFile();
        char buffer[4096];
        while(currentFileValid)
        {
            if(!(zip.getCurrentFileName()[zip.getCurrentFileName().length()] == QChar('/')))
            {
                // Write to file since not directory
                currentFile.open(QIODevice::ReadOnly);
                destinationFile.setFileName(modDirectory + zip.getCurrentFileName());
                destinationFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
                while(!currentFile.atEnd())
                {
                    qint64 bytes = currentFile.read(buffer, 4096);
                    destinationFile.write(buffer, bytes);
                }
                destinationFile.close();
                currentFile.close();
            }
            currentFileValid = zip.goToNextFile();
        }

        zip.close();

        // Load the newly installed mod if mods have been loaded
        if(m_loaded)
        {
            newMod->icon.load(modDirectory + "icon.png", "PNG");
            if(existingMod < 0)
            {
                qDebug() << "Added mod" << m_mods.size() << newMod->name;
                m_mods.push_back(newMod);
            }
            else
            {
                qDebug() << "Replaced mod" << existingMod << newMod->name;
                m_mods[existingMod] = newMod;
            }
        }
    }
    else
        return Error::MOD_GAME_DIR_INVALID;

    return Error::NO_ERROR;
}

ErrorCode ModManager::remove(int mod, bool keepSpot)
{
    if(!(mod >=0 && mod < m_mods.size()))
        return Error::INVALID_MOD_ID;

    if(m_enabledModOrder.contains(m_mods[mod]))
    {
        disableMod(mod);
        save();
    }

    // Delete mod directory
    QDir modsDir("");
    modsDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    if(!modsDir.cd(m_gameDir + MODS_DIR))
        return Error::MOD_GAME_DIR_INVALID;
    if(!deleteDir(m_gameDir + MODS_DIR + m_mods[mod]->name))
        return Error::FAILED_DELETE_MOD;

    // Remove from mod list
    if(m_loaded)
    {
        qDebug() << "Removed mod" << mod << m_mods[mod]->name;
        delete m_mods[mod];
        if(!keepSpot)
            m_mods.remove(mod);
    }

    return Error::NO_ERROR;
}

ErrorCode ModManager::load()
{
    if(m_gameDirValid)
    {
        for(int i=0; i<m_mods.size(); i++)
            delete m_mods[i];
        m_mods.clear();

        m_enabledModOrder.clear();

        // Find all directories containing a mod config file to load
        QDir modsDir("");
        modsDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        if(!modsDir.cd(m_gameDir + MODS_DIR))
            return Error::MOD_GAME_DIR_INVALID;
        for(unsigned int i=0; i<modsDir.count(); i++)
        {
            QString modDirPath = modsDir.entryList().at(i);
            QDir modDir(modsDir.absolutePath() + QChar('/') + modDirPath, "install.cfg", QDir::SortFlags(QDir::IgnoreCase), QDir::Filters(QDir::AllEntries));
            if(modDir.count() == 1)
            {
                // Read mod info from config file
                QFile modConfigFile(modDir.absolutePath() + QChar('/') + modDir[0]);
                if(!modConfigFile.open(QIODevice::ReadOnly | QIODevice::Text))
                    return Error::FAILED_OPEN_MOD_CONFIG_FILE;
                QTextStream modConfigIn(&modConfigFile);
                Mod* mod = new Mod;
                if(parseModConfig(mod, &modConfigIn) == Error::NO_ERROR)
                {
                    qDebug() << "Added mod" << m_mods.size() << mod->name;
                    mod->icon.load(modDir.absolutePath() + QChar('/') + "icon.png", "PNG");
                    m_mods.push_back(mod);
                }
                else
                {
                    qWarning() << "Failed to load mod" << m_mods.size() << mod->name;
                    delete mod;
                }
            }
        }
    }

    // Figure out which mods are enabled
    QVector<QString> activeMods;
    ErrorCode initErr = m_gameConfig->init(m_dataDir + VERSION_FILE, m_gameDir + PLUGINS_FILE, m_gameDir + RESOURCES_FILE, activeMods);
    if(initErr != Error::NO_ERROR)
        return initErr;
    for(int i=0; i<activeMods.size(); i++)
    {
        bool activeModFound = false;
        for(int j=0; j<m_mods.size(); j++)
        {
            if(m_mods[j]->name == activeMods[i])
            {
                m_mods[j]->enabled = true;
                if(!m_enabledModOrder.contains(m_mods[j]))
                    m_enabledModOrder.push_back(m_mods[j]);
                for(int k=0; k<m_mods[j]->plugins.size(); k++)
                    installPlugin(i, m_mods[j]->name, m_mods[j]->plugins[k]);
                for(int k=0; k<m_mods[j]->resources.size(); k++)
                    m_gameConfig->addResource(i, m_mods[j]->name, m_mods[j]->resources[k]);
                activeModFound = true;
                break;
            }
        }
        if(!activeModFound)
            m_gameConfig->remove(i);
    }

    m_loaded = true;

    return Error::NO_ERROR;
}

bool ModManager::getGameDirectoryValid()
{
    return m_gameDirValid;
}

int ModManager::getVersion()
{
    return m_gameConfig->getVersion();
}

QVector<ModManager::Mod*>* ModManager::getMods()
{
    return &m_mods;
}

QVector<ModManager::Mod *> *ModManager::getEnabledModOrder()
{
    return &m_enabledModOrder;
}

ErrorCode ModManager::enableMod(int mod)
{
    if(m_loaded && m_gameDirValid)
    {
        if(mod >= 0 && mod < m_mods.size())
        {
            if(!m_enabledModOrder.contains(m_mods[mod]))
            {
                for(int i=0; i<m_mods[mod]->plugins.size(); i++)
                    installPlugin(m_enabledModOrder.size(), m_mods[mod]->name, m_mods[mod]->plugins[i]);
                for(int i=0; i<m_mods[mod]->resources.size(); i++)
                    m_gameConfig->addResource(m_enabledModOrder.size(), m_mods[mod]->name, m_mods[mod]->resources[i]);
                m_enabledModOrder.push_back(m_mods[mod]);
                m_mods[mod]->enabled = true;

                if(m_mods[mod]->refreshScriptCache)
                {
                    // Delete script cache
                    if(!deleteDir(m_gameDir + "ScriptCache"))
                        return Error::FAILED_DELETE_SCRIPT_CACHE;
                }

                // Refresh world and inventory in parent window due to prompts
            }
        }
        else
            return Error::INVALID_MOD_ID;
    }
    else
        return Error::MODS_NOT_LOADED;

    return Error::NO_ERROR;
}

ErrorCode ModManager::disableMod(int mod)
{
    if(m_loaded && m_gameDirValid)
    {
        if(mod >= 0 && mod < m_mods.size())
        {
            for(int i=0; i<m_enabledModOrder.size(); i++)
            {
                if(m_enabledModOrder[i] == m_mods[mod])
                {
                    m_gameConfig->remove(i);
                    m_enabledModOrder.remove(i);
                    m_mods[mod]->enabled = false;
                    for(int j=0; j<m_mods[mod]->plugins.size(); j++)
                    {
                        if(!DISALLOWED_PLUGIN_FILENAMES.contains(m_mods[mod]->plugins[j]))
                            QFile::remove(m_gameDir + m_mods[mod]->plugins[j] + ".dll");
                    }
                    if(m_mods[mod]->refreshScriptCache)
                    {
                        // Delete script cache
                        if(!deleteDir(m_gameDir + "ScriptCache"))
                            return Error::FAILED_DELETE_SCRIPT_CACHE;
                    }
                    break;
                }
            }
        }
        else
            return Error::INVALID_MOD_ID;
    }
    else
        return Error::MODS_NOT_LOADED;

    return Error::NO_ERROR;
}

ErrorCode ModManager::modOrderUp(int mod)
{
    if(m_loaded)
    {
        if(mod >= 1 && mod < m_enabledModOrder.size())
        {
            // Move up in order
            m_gameConfig->swap(mod, mod-1);
            Mod* temp = m_enabledModOrder[mod];
            m_enabledModOrder[mod] = m_enabledModOrder[mod-1];
            m_enabledModOrder[mod-1] = temp;
        }
        else
            return Error::INVALID_MOD_ID;
    }
    else
        return Error::MODS_NOT_LOADED;

    return Error::NO_ERROR;
}

ErrorCode ModManager::modOrderDown(int mod)
{
    if(m_loaded)
    {
        if(mod >= 0 && mod < m_enabledModOrder.size()-1)
        {
            // Move back in order
            m_gameConfig->swap(mod, mod+1);
            Mod* temp = m_enabledModOrder[mod];
            m_enabledModOrder[mod] = m_enabledModOrder[mod+1];
            m_enabledModOrder[mod+1] = temp;
        }
        else
            return Error::INVALID_MOD_ID;
    }
    else
        return Error::MODS_NOT_LOADED;

    return Error::NO_ERROR;
}

ErrorCode ModManager::save()
{
    if(m_loaded)
        return m_gameConfig->save();
    return Error::MODS_NOT_LOADED;
}

ErrorCode ModManager::refreshWorld()
{
    if(m_dataDirValid)
        deleteDir(m_dataDir + WORLDS_DIR);
    else
        return Error::DATA_DIR_INVALID;

    return Error::NO_ERROR;
}

ErrorCode ModManager::refreshInventory()
{
    if(m_dataDirValid)
        deleteDir(m_dataDir + INVENTORY_DIR);
    else
        return Error::DATA_DIR_INVALID;

    return Error::NO_ERROR;
}

ErrorCode ModManager::parseModConfig(Mod* modPtr, QTextStream* modConfigIn)
{
    bool modName = false, modPrettyName = false, modAuthor = false, modVersion = false, modGameVersion = false, modDoesSomething = false;
    modPtr->enabled = false;
    modPtr->refreshScriptCache = false;
    modPtr->refreshWorld = false;
    modPtr->refreshInventory = false;
    modPtr->description = "";
    qDebug() << "Loading mod" << m_mods.size();

    while(!modConfigIn->atEnd())
    {
        QString line = modConfigIn->readLine();

        // Ignore comments
        if(line.contains("#"))
        {
            for(int j=0; j<line.length(); j++)
            {
                if(line[j] == QChar('#'))
                {
                    line.truncate(j);
                    break;
                }
            }
        }

        if(line.contains("="))
        {
            // Find value after '=' in config
            int index = 1;
            while(line[index-1] != QChar('='))
                index++;
            if(index >= line.length())
                continue;
            QString value = "";
            for(int j=index; j<line.length(); j++)
                value.append(line[j]);

            // Store config values
            if(!value.isEmpty() && value != "")
            {
                if(line.contains("prettyName=", Qt::CaseInsensitive))
                {
                    modPtr->prettyName = value;
                    modPrettyName = true;
                    qDebug() << "Mod Pretty Name:" << modPtr->prettyName;
                }
                else if(line.contains("name=", Qt::CaseInsensitive))
                {
                    modPtr->name = value.remove(QRegExp("\\s")); //remove whitespace from internal name
                    modName = true;
                    qDebug() << "Mod Name:" << modPtr->name;
                }
                else if(line.contains("author=", Qt::CaseInsensitive))
                {
                    modPtr->author = value;
                    modAuthor = true;
                    qDebug() << "Mod Author:" << modPtr->author;
                }
                else if(line.contains("gameVersion=", Qt::CaseInsensitive))
                {
                    QTextStream convert(&value, QIODevice::ReadOnly);
                    convert >> modPtr->gameVersion;
                    if(modPtr->gameVersion > 0)
                        modGameVersion = true;
                    qDebug() << "Mod Game Version:" << modPtr->gameVersion;
                }
                else if(line.contains("version=", Qt::CaseInsensitive))
                {
                    modPtr->version = value;
                    modVersion = true;
                    qDebug() << "Mod Version:" << modPtr->version;
                }
                else if(line.contains("description=", Qt::CaseInsensitive))
                {
                    modPtr->description = value;
                    qDebug() << "Mod Description:" << modPtr->description;
                }
                else if(line.contains("plugin=", Qt::CaseInsensitive))
                {
                    modPtr->plugins.push_back(value);
                    modDoesSomething = true;
                    qDebug() << "Mod Plugin:" << modPtr->plugins.back();
                }
                else if(line.contains("resource=", Qt::CaseInsensitive))
                {
                    modPtr->resources.push_back(value);
                    modDoesSomething = true;
                    qDebug() << "Mod Resource:" << modPtr->resources.back();
                }
                else if(line.contains("refreshScriptCache=true", Qt::CaseInsensitive))
                {
                    modPtr->refreshScriptCache = true;
                    qDebug() << "Mod Refresh Script Cache: true";
                }
                else if(line.contains("refreshWorld=true", Qt::CaseInsensitive))
                {
                    modPtr->refreshWorld = true;
                    qDebug() << "Mod Refresh World: true";
                }
                else if(line.contains("refreshInventory=true", Qt::CaseInsensitive))
                {
                    modPtr->refreshInventory = true;
                    qDebug() << "Mod Refresh Inventory: true";
                }
            }
        }
    }

    if(modName && modPrettyName && modAuthor && modVersion && modGameVersion && modDoesSomething)
    {
        return Error::NO_ERROR;
    }
    return Error::FAILED_PARSE_MOD_CONFIG;
}

int ModManager::installPlugin(int modIndex, const QString& modName, const QString& plugin)
{
    // Install plugin only if name not disallowed
    if(!DISALLOWED_PLUGIN_FILENAMES.contains(plugin))
    {
        QString sourceFile = m_gameDir + MODS_DIR + modName + "/data/" + plugin + ".dll";
        QFileInfo pluginFileInfo(sourceFile);
        QString destinationFile = m_gameDir + pluginFileInfo.fileName();qDebug()<<sourceFile<<destinationFile;
        QFile::copy(sourceFile, destinationFile);
        return m_gameConfig->addPlugin(modIndex, modName, plugin);
    }

    return -1;
}

bool ModManager::deleteDir(const QString& path)
{
    bool deleted;

    QDir dir("");
    dir.setFilter(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files);
    if(!dir.cd(path))
        return true; //doesn't exist so no need to delete

    // Recursively delete directories and files
    for(unsigned int i=0; i<dir.count(); i++)
    {
        QFileInfo fileInfo(dir, dir[i]);
        if(fileInfo.isDir())
            deleted = deleteDir(fileInfo.absoluteFilePath());
        else
            deleted = QFile::remove(fileInfo.absoluteFilePath());
    }
    deleted = dir.rmdir(path);

    return deleted;
}

ErrorCode ModManager::createArchiveDirectoryStructure(QuaZipDir* currentDir, QDir* destinationDir)
{
    for(int i=0; i<currentDir->entryList().size(); i++)
    {
        if(currentDir->entryList()[i].at(currentDir->entryList()[i].length()-1) == QChar('/'))
        {
            destinationDir->mkdir(currentDir->entryList()[i]);
            destinationDir->cd(currentDir->entryList()[i]);
            currentDir->cd(currentDir->entryList()[i]);
            createArchiveDirectoryStructure(currentDir, destinationDir);
            currentDir->cdUp();
            destinationDir->cdUp();
        }
    }
    return Error::NO_ERROR;
}
