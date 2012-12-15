/**********************************************************
 * configmodifier.h
 *
 * Author: Jason Gassel
 * Desc: Class to handle modifying of config files. All
 *       config files use a [Mods] section to seperate
 *       mods section from the base game. The mod number
 *       defines load order, each should have a unique
 *       number with lower ones loading sooner.
 *
 *
 * Interface:
 *   int Init(QString versionFile, QString pluginsFileName, QString resourceFileName)
 *     Loads vanilla config from files and finds version
 *     number. Returns error code.
 *
 *   int getVersion()
 *     Returns the current version read from the version
 *     file.
 *
 *   int addPlugin(int mod, modName, QString plugin)
 *     Adds plugin DLL filename string to plugins config
 *     of given mod if not already in list of mod plugins.
 *     Returns load order index of the mod.
 *
 *   int addResource(int mod, QString modName, QString resource)
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
 **********************************************************/

#ifndef CONFIGMODIFIER_H
#define CONFIGMODIFIER_H

#include <QString>
#include <QVector>
#include <QFile>
#include <QTextStream>

#include <errorcodes.h>

class ConfigModifier
{    
public:
    ConfigModifier();
    virtual ~ConfigModifier();

public:
    QString MOD_HEADER; //pretend this is const

private:
    int m_version;
    QString m_pluginsFileName;
    QString m_resourcesFileName;
    QVector<QString> m_vanillaPlugins;
    QVector<QString> m_vanillaResources;

    struct Mod
    {
        QString name;
        QVector<QString> plugins;
        QVector<QString> resources;
    };
    QVector<Mod*> m_modList;

public:
    int init(QString versionFileName, QString pluginsFileName, QString resourcesFileName);
    int getVersion();
    int addPlugin(int mod, QString modName, QString plugin);
    int addResource(int mod, QString modName, QString resource);
    int save();
    void swap(int mod1, int mod2);
    void remove(int mod);
    void removeAll();
};

#endif // CONFIGMODIFIER_H
