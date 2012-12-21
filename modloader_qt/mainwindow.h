#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
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

    void on_buttonInstallMod_clicked();

private:
    Ui::MainWindow *ui;

    QMessageBox* errorMessageBox;

    ModManager* modManager;
    const QVector<const ModManager::Mod* const>* modList;
    QStandardItemModel modListTreeModel;
};

#endif // MAINWINDOW_H
