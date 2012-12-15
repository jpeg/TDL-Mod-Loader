/**********************************************************
 * configmodifier.cpp
 *
 * Author: Jason Gassel
 **********************************************************/

#include "configmodifier.h"

ConfigModifier::ConfigModifier()
{
    MOD_HEADER = "[Mods]";
    m_version = -1;
}

ConfigModifier::~ConfigModifier()
{
    // Clean up mod list
    for(int i=0; i<m_modList.size(); i++)
        delete m_modList[i];
}

int ConfigModifier::init(QString versionFileName, QString pluginsFileName, QString resourcesFileName)
{
    // Read version
    QFile versionFile(versionFileName);
    if(!versionFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return Error::FAILED_OPEN_VERSION_FILE;
    QTextStream versionIn(&versionFile);
    m_version = -1;
    if(!versionFile.atEnd())
        versionIn >> m_version;
    versionFile.close();

    // Load config files
    m_pluginsFileName = pluginsFileName;
    m_resourcesFileName = resourcesFileName;
    QFile vanillaPluginsFile(m_pluginsFileName);
    QFile vanillaResourcesFile(m_resourcesFileName);
    if(!vanillaPluginsFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return Error::FAILED_OPEN_PLUGINS_FILE;
    if(!vanillaResourcesFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return Error::FAILED_OPEN_RESOURCES_FILE;
    QTextStream pluginsIn(&vanillaPluginsFile);
    QTextStream resourcesIn(&vanillaResourcesFile);
    while(!pluginsIn.atEnd())
    {
        QString temp = pluginsIn.readLine();
        if(temp.contains(MOD_HEADER, Qt::CaseInsensitive))
            break;
        m_vanillaPlugins.push_back(temp);
    }
    while(!resourcesIn.atEnd())
    {
        QString temp = resourcesIn.readLine();
        if(temp.contains(MOD_HEADER, Qt::CaseInsensitive))
            break;
        m_vanillaResources.push_back(temp);
    }
    vanillaPluginsFile.close();
    vanillaResourcesFile.close();

    return Error::NO_ERROR;
}

int ConfigModifier::getVersion()
{
    return m_version;
}

int ConfigModifier::addPlugin(int mod, QString modName, QString plugin)
{
    if(mod < m_modList.size() && mod >= 0)
    {
        m_modList[mod]->name = modName;
        m_modList[mod]->plugins.push_back(plugin);
        return mod;
    }
    else
    {
        Mod* m = new Mod;
        m->name = modName;
        m->plugins.push_back(plugin);
        m_modList.push_back(m);
        return m_modList.size() - 1; //id for new mod
    }
}

int ConfigModifier::addResource(int mod, QString modName, QString resource)
{
    if(mod < m_modList.size() && mod >= 0)
    {
        m_modList[mod]->name = modName;
        m_modList[mod]->resources.push_back(resource);
        return mod;
    }
    else
    {
        Mod* m = new Mod;
        m->name = modName;
        m->resources.push_back(resource);
        m_modList.push_back(m);
        return m_modList.size() - 1; //id for new mod
    }
}

int ConfigModifier::save()
{
    QFile pluginsFile(m_pluginsFileName);
    QFile resourcesFile(m_resourcesFileName);
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
            resourcesOut << "FileSystem=./mods/" << m_modList[i]->name << "/" << m_modList[i]->resources[j] << endl;
    }

    pluginsFile.close();
    resourcesFile.close();

    return Error::NO_ERROR;
}

void ConfigModifier::swap(int mod1, int mod2)
{
    if(mod1 != mod2 && mod1 < m_modList.size() && mod2 < m_modList.size() && mod1 >= 0 && mod2 >= 0)
    {
        Mod* temp = m_modList[mod1];
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
