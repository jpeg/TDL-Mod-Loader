/******************************************************************************
 * mainwindow.h
 *
 * Copyright (C) 2012 Jason Gassel. All rights reserved.
 *
 * This file is part of the Jackal Mod Loader.
 *
 * Jackal Mod Loader is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Jackal Mod Loader is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Jackal Mod Loader.  If not, see <http://www.gnu.org/licenses/>.
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

#include "about.h"

#include "modmanager.h"

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
    
private slots:
    void on_actionExit_triggered();

    void on_actionAbout_triggered();

    void on_actionSettings_triggered();

    void on_buttonInstallMod_clicked();

    void on_buttonEnableMod_clicked();

    void on_buttonModOrderUp_clicked();

    void on_buttonModOrderDown_clicked();

    void on_buttonRemoveMod_clicked();

    void on_treeViewMods_clicked(const QModelIndex &index);

    void on_treeViewMods_activated(const QModelIndex &index);

    void on_actionOnline_Help_triggered();

private:
    Ui::MainWindow *ui;

    QSettings* settings;
    QMessageBox* errorMessageBox;

    ModManager* modManager;
    QStandardItemModel modListTreeModel;
    QStandardItem* enabledModsItem;
    QStandardItem* allModsItem;
};

#endif // MAINWINDOW_H
