/**********************************************************
 * modmanager.cpp
 *
 * Author: Jason Gassel
 **********************************************************/

#include "modmanager.h"

ModManager::ModManager(const QString& gameDirectory, const QString& dataDirectory)
{
    m_gameConfig = new ConfigModifier;

#ifdef WIN32
    DATA_SUB_DIR = "Roaming/Sandswept Studios/The Dead Linger/";
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

void ModManager::checkGameDirectory(const QString& gameDirectory)
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
}

void ModManager::checkDataDirectory(const QString& dataDirectory)
{
    m_dataDir = dataDirectory;
    m_dataDirValid = true;

    QDir dataDir("");
    if(!dataDir.cd(m_dataDir))
    {
        m_dataDirValid = false;
    }
}

ErrorCode ModManager::install(const QString& modArchivePath)
{
    qDebug() << "Installing mod" << modArchivePath;

    if(m_gameDirValid)
    {
        //TODO quazip to move files to mods/ dir

        // Load the newly installed mod if mods have been loaded
        if(m_loaded)
        {
            //TODO
        }
    }
    else
        return Error::MOD_GAME_DIR_INVALID;

    return Error::NO_ERROR;
}

ErrorCode ModManager::remove(int mod)
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
                Mod* mod = new Mod;
                bool modName = false, modPrettyName = false, modAuthor = false, modVersion = false, modGameVersion = false, modDoesSomething = false;
                mod->enabled = false;
                mod->refreshScriptCache = false;
                qDebug() << "Loading mod" << m_mods.size();

                QTextStream modConfigIn(&modConfigFile);
                while(!modConfigIn.atEnd())
                {
                    QString line = modConfigIn.readLine();

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
                                mod->prettyName = value;
                                modPrettyName = true;
                                qDebug() << "Mod Pretty Name:" << mod->prettyName;
                            }
                            else if(line.contains("name=", Qt::CaseInsensitive))
                            {
                                mod->name = value.remove(QRegExp("\\s")); //remove whitespace from internal name
                                modName = true;
                                qDebug() << "Mod Name:" << mod->name;
                            }
                            else if(line.contains("author=", Qt::CaseInsensitive))
                            {
                                mod->author = value;
                                modAuthor = true;
                                qDebug() << "Mod Author:" << mod->author;
                            }
                            else if(line.contains("gameVersion=", Qt::CaseInsensitive))
                            {
                                QTextStream convert(&value, QIODevice::ReadOnly);
                                convert >> mod->gameVersion;
                                if(mod->gameVersion > 0)
                                    modGameVersion = true;
                                qDebug() << "Mod Game Version:" << mod->gameVersion;
                            }
                            else if(line.contains("version=", Qt::CaseInsensitive))
                            {
                                mod->version = value;
                                modVersion = true;
                                qDebug() << "Mod Version:" << mod->version;
                            }
                            else if(line.contains("plugin=", Qt::CaseInsensitive))
                            {
                                mod->plugins.push_back(value);
                                modDoesSomething = true;
                                qDebug() << "Mod Plugin:" << mod->plugins.back();
                            }
                            else if(line.contains("resource=", Qt::CaseInsensitive))
                            {
                                mod->resources.push_back(value);
                                modDoesSomething = true;
                                qDebug() << "Mod Resource:" << mod->resources.back();
                            }
                            else if(line.contains("refreshScriptCache=true", Qt::CaseInsensitive))
                            {
                                mod->refreshScriptCache = true;
                                qDebug() << "Mod Refresh Script Cache: true";
                            }
                            else if(line.contains("refreshWorld=true", Qt::CaseInsensitive))
                            {
                                mod->refreshWorld = true;
                                qDebug() << "Mod Refresh World: true";
                            }
                            else if(line.contains("refreshInventory=true", Qt::CaseInsensitive))
                            {
                                mod->refreshInventory = true;
                                qDebug() << "Mod Refresh Inventory: true";
                            }
                        }
                    }
                }

                // Add mod if it has all config needed
                if(modName && modPrettyName && modAuthor && modVersion && modGameVersion && modDoesSomething)
                {
                    qDebug() << "Added mod" << m_mods.size() << mod->name;
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
    if(m_loaded)
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
    if(m_loaded)
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

int ModManager::installPlugin(int modIndex, QString modName, QString plugin)
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
