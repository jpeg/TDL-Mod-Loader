/**********************************************************
 * configmodifier.cpp
 *
 * Author: Jason Gassel
 **********************************************************/

#include "configmodifier.h"

ConfigModifier::ConfigModifier()
{
    MOD_HEADER = "#[Mods]"; //not commenting out will result in TDL ignoring mods section
    MODS_DIR = "mods/";
    m_version = -1;
}

ConfigModifier::~ConfigModifier()
{
    // Clean up mod list
    for(int i=0; i<m_modList.size(); i++)
        delete m_modList[i];
}

ErrorCode ConfigModifier::init(const QString &versionFilename, const QString &pluginsFilename, const QString &resourcesFilename, QVector<QString>& activeMods)
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
            break;
        m_vanillaPlugins.push_back(line);
        // PLUGINS DO NOT LOAD INTO MOD LIST YET, RATHER MOD MANAGER WILL FIGURE THAT OUT FOR NOW
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
            if(line.contains("FileSystem=./" + MODS_DIR, Qt::CaseInsensitive))
            {
                line.remove(0, QString("FileSystem=./" + MODS_DIR).length());
                int index = 0;
                while(line[index] != QChar('/'))
                    index++;
                line.truncate(index);
                QString modName = line;
                activeMods.push_back(modName);
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

int ConfigModifier::addPlugin(int mod, const QString& modName, const QString& plugin)
{
    if(mod < m_modList.size() && mod >= 0)
    {
        m_modList[mod]->name = modName;
        m_modList[mod]->plugins.push_back(plugin);
        return mod;
    }
    else
    {
        ModConfig* m = new ModConfig;
        m->name = modName;
        m->plugins.push_back(plugin);
        m_modList.push_back(m);
        return m_modList.size() - 1; //id for new mod
    }
}

int ConfigModifier::addResource(int mod, const QString &modName, const QString &resource)
{
    if(mod < m_modList.size() && mod >= 0)
    {
        m_modList[mod]->name = modName;
        m_modList[mod]->resources.push_back(resource);
        return mod;
    }
    else
    {
        ModConfig* m = new ModConfig;
        m->name = modName;
        m->resources.push_back(resource);
        m_modList.push_back(m);
        return m_modList.size() - 1; //id for new mod
    }
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
        for(int j=0; j<m_modList[i]->plugins.size(); j++)
            pluginsOut << "#Plugin=" << m_modList[i]->plugins[j] << endl;
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
