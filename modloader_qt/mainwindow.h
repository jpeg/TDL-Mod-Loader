#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QStandardItemModel>

#include <modmanager.h>

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
    
private slots:
    void on_actionExit_triggered();

    void on_actionAbout_triggered();

    void on_actionSettings_triggered();

    void on_buttonInstallMod_clicked();

    void on_buttonEnableMod_clicked();

    void on_buttonDisableMod_clicked();

    void on_buttonRemoveMod_clicked();

    void on_treeViewMods_clicked(const QModelIndex &index);

    void on_treeViewMods_activated(const QModelIndex &index);

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
