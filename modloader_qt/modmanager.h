/**********************************************************
 * modmanager.h
 *
 * Author: Jason Gassel
 * Desc: Class to manage and install mods.
 *
 *
 * Interface:
 *   ModManager(const QString& gameDirectory)
 *     Checks if given base game directory is valid by
 *     looking to see if the game executable exists there.
 *     Creates the mods directory if it doesn't already
 *     exist.
 *
 *   ErrorCode install(const QString& modArchivePath)
 *     Copies all files from the given mod archive into
 *     the mods directory, error code value returned. For
 *     now only ZIP format archive files will be supported.
 *
 *   ErrorCode remove(int mod)
 *     Deletes a mod from disk and removes it from the mod
 *     lists. Returns an error code value.
 *
 *   ErrorCode load(const QString& versionFileName)
 *     Loads all mods in mods directory into a list and
 *     figures out which are enabled from game config
 *     files. Returns an error code value.
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
 *     Adds mod to the list of enabled mods and enables
 *     it in the config manager. Deletes the script cache
 *     if mod requires it. Returns an error code value.
 *
 *   ErrorCode disableMod(int mod)
 *     Removes mod from the list of enabled mods and
 *     removes it from the config manager files. Returns
 *     an error code value.
 *
 *   ErrorCode modOrderUp(int mod)
 *     Moves mod up in enabled mods list and in config
 *     manager. Returns an error code value.
 *
 *   ErrorCode modOrderDown(int mod)
 *     Moves mod down in enabled mods list and in config
 *     manager. Returns an error code value.
 *
 *   ErrorCode save()
 *      Saves the currently enabled mods to game config.
 *      Returns an error code value.
 **********************************************************/

#ifndef MODMANAGER_H
#define MODMANAGER_H

#include <QString>
#include <QVector>
#include <QDir>
#include <QFile>
#include <QDebug>

#include <configmodifier.h>
#include <errorcodes.h>

class ModManager
{
public:
    ModManager(const QString& gameDirectory);
    virtual ~ModManager();

private:
    QString MODS_DIR;
    QVector<QString> DISALLOWED_PLUGIN_FILENAMES;

public:
    struct Mod
    {
        bool enabled;
        QString name;
        QString prettyName;
        QString author;
        QString version;
        int gameVersion;
        QVector<QString> plugins;
        QVector<QString> resources;
        bool refreshScriptCache;
    };

private:
    QString m_gameDir;
    bool m_gameDirValid;
    bool m_loaded;

    ConfigModifier* m_gameConfig;

    QVector<Mod*> m_mods;
    QVector<Mod*> m_enabledModOrder;

public:
    void checkGameDirectory(QString gameDirectory);
    ErrorCode install(const QString& modArchivePath);
    ErrorCode remove(int mod);
    ErrorCode load(const QString& versionFileName);
    bool getGameDirectoryValid();
    int getVersion();
    QVector<Mod*>* getMods();
    QVector<Mod*>* getEnabledModOrder();
    ErrorCode enableMod(int mod);
    ErrorCode disableMod(int mod);
    ErrorCode modOrderUp(int mod);
    ErrorCode modOrderDown(int mod);
    ErrorCode save();

private:
    bool deleteDir(const QString& path);
};

#endif // MODMANAGER_H
