#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //TODO load settings

    errorMessageBox = new QMessageBox;
    errorMessageBox->setIcon(QMessageBox::Critical);

    modManager = new ModManager("C:/Sandswept Studios/The Dead Linger Alpha/");
    showError(modManager->load("C:/Users/Jason/AppData/Roaming/Sandswept Studios/The Dead Linger/tdlversion.txt"));

    // Setup tree model for mod list
    QStandardItem* root = modListTreeModel.invisibleRootItem();
    QStandardItem* enabledModModel = new QStandardItem(/*TODO icon,*/ "Enabled Mods");
    QStandardItem* allModModel = new QStandardItem(/*TODO icon,*/ "All Mods");
    enabledModModel->setFlags(enabledModModel->flags() & ~Qt::ItemIsEditable);
    allModModel->setFlags(allModModel->flags() & ~Qt::ItemIsEditable);
    root->appendRow(enabledModModel);
    root->appendRow(allModModel);
    for(int i=0; i<modManager->getEnabledModOrder()->size(); i++)
    {
        QStandardItem* enabledModItem = new QStandardItem(/*TODO icon,*/ modManager->getEnabledModOrder()->at(i)->prettyName);
        enabledModItem->setFlags(enabledModItem->flags() & ~Qt::ItemIsEditable);
        enabledModModel->appendRow(enabledModItem);
    }
    for(int i=0; i<modManager->getMods()->size(); i++)
    {
        QStandardItem* modItem = new QStandardItem(/*TODO icon,*/ modManager->getMods()->at(i)->prettyName);
        modItem->setFlags(modItem->flags() & ~Qt::ItemIsEditable);
        allModModel->appendRow(modItem);
    }

    ui->setupUi(this);

    QString version;
    QTextStream versionStream(&version);
    versionStream << "Installed TDL Version: " << modManager->getVersion();
    ui->labelTDLVersion->setText(version);

    ui->treeViewMods->setHeaderHidden(true);
    ui->treeViewMods->setModel(&modListTreeModel);
}

MainWindow::~MainWindow()
{
    delete modManager;
    delete ui;
}

void MainWindow::showError(ErrorCode error)
{
    if(error != Error::NO_ERROR)
    {
        QString errorMsg;
        QTextStream errorStream(&errorMsg);
        errorStream << "Error: " << error;
        errorMessageBox->setText(errorMsg);
        errorMessageBox->exec();
    }
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_actionAbout_triggered()
{
    //TODO display about window
}

void MainWindow::on_buttonInstallMod_clicked()
{
    QString modArchive = QFileDialog::getOpenFileName(this, "Select mod to install...", "./", "ZIP Files (*.zip)");
    showError(modManager->install(modArchive));
}
