#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    modManager = new ModManager("C:\\Sandswept Studios\\The Dead Linger Alpha\\");

    ErrorCode error = modManager->install("Z:\\Source\\Github\\TDL-Mod-Loader\\ModLoaderFormatExample.zip");
    if(error != Error::NO_ERROR)
    {
        QString errStr;
        QTextStream out(&errStr);
        out << "Failed to install mod: " << error;
        ui->labelTDLVersion->setText(errStr);
        return;
    }

    error = modManager->load("C:\\Users\\Jason\\AppData\\Roaming\\Sandswept Studios\\The Dead Linger\\tdlversion.txt");
    if(error != Error::NO_ERROR)
    {
        QString errStr;
        QTextStream out(&errStr);
        out << "Failed to load mods: " << error;
        ui->labelTDLVersion->setText(errStr);
        return;
    }

    const QVector<const ModManager::Mod* const>* modList = modManager->getMods();
    if(modList->size() != 1)
    {
        QString errStr;
        QTextStream out(&errStr);
        out << "Bad mod list size: " << modList->size();
        ui->labelTDLVersion->setText(errStr);
        return;
    }

    error = modManager->enableMod(0);
    if(error != Error::NO_ERROR)
    {
        QString errStr;
        QTextStream out(&errStr);
        out << "Failed to enable mod: " << error;
        ui->labelTDLVersion->setText(errStr);
        return;
    }

    const QVector<const ModManager::Mod* const>* enabledModOrder = modManager->getEnabledModOrder();
    if(enabledModOrder->size() != 1)
    {
        QString errStr;
        QTextStream out(&errStr);
        out << "Bad enabled mod order size: " << enabledModOrder->size();
        ui->labelTDLVersion->setText(errStr);
        return;
    }

    error = modManager->save();
    if(error != Error::NO_ERROR)
    {
        QString errStr;
        QTextStream out(&errStr);
        out << "Failed to save: " << error;
        ui->labelTDLVersion->setText(errStr);
        return;
    }

    QString outStr;
    QTextStream out(&outStr);
    out << "Succuss";
    ui->labelTDLVersion->setText(outStr);
}

MainWindow::~MainWindow()
{
    delete ui;
}
