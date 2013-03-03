/******************************************************************************
 * mainwindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QStandardItemModel>
#include <QIcon>

#include "about.h"
#include "settings.h"
#include "customization.h"
#include "serverconfig.h"

#include "modmanager.h"
#include "updater.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void showError(ErrorCode error);
    int getSelectedModIndex();
    void updateStatusBar(QString message);
    
private slots:
    void statusBar_message_changed(QString message);

    void on_actionExit_triggered();

    void on_actionSettings_triggered();

    void on_actionStartDedicatedServer_triggered();

    void on_actionConfigureServer_triggered();

    void on_actionOnline_Help_triggered();

    void on_actionAbout_triggered();

    void on_actionInstall_Update_Mod_triggered();

    void on_buttonInstallMod_clicked();

    void on_buttonEnableMod_clicked();

    void on_buttonModOrderUp_clicked();

    void on_buttonModOrderDown_clicked();

    void on_buttonRemoveMod_clicked();

    void on_treeViewMods_clicked(const QModelIndex &index);

    void on_treeViewMods_activated(const QModelIndex &index);

    void on_buttonRefreshWorld_clicked();

    void on_buttonRefreshInventory_clicked();

    void on_textBrowserModDescription_anchorClicked(const QUrl &arg1);

private:
    Ui::MainWindow *ui;

    Updater* updater;

    QSettings* settings;
    QString onlineHelpUrl;
    QMessageBox* errorMessageBox;

    ModManager* modManager;
    QStandardItemModel modListTreeModel;
    QStandardItem* enabledModsItem;
    QStandardItem* allModsItem;

    // Icons
    QIcon* iconFolder;
    QIcon* iconEnabledMod;
    QIcon* iconDisabledMod;
    QIcon* iconAddMod;
    QIcon* iconRemoveMod;
};

#endif // MAINWINDOW_H
