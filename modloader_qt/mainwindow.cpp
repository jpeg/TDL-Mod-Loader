#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Load settings
    settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Utudio", "Jackal Mod Loader");
    QString gamePath = settings->value("game/path", "C:/Sandswept Studios/The Dead Linger Alpha/").toString();
    int gameVersion = settings->value("game/version", -1).toInt();

    errorMessageBox = new QMessageBox;
    errorMessageBox->setIcon(QMessageBox::Critical);

    // Initialize Mod Manager
    modManager = new ModManager(gamePath);
    showError(modManager->load("C:/Users/Jason/AppData/Roaming/Sandswept Studios/The Dead Linger/tdlversion.txt"));
    if(gameVersion != modManager->getVersion())
        settings->setValue("game/version", modManager->getVersion()); //may need to do something special for updates?

    // Set up tree view model for mod list
    QStandardItem* root = modListTreeModel.invisibleRootItem();
    enabledModsItem = new QStandardItem(/*TODO icon,*/ "Enabled Mods");
    allModsItem = new QStandardItem(/*TODO icon,*/ "All Mods");
    enabledModsItem->setFlags(enabledModsItem->flags() & ~Qt::ItemIsEditable);
    allModsItem->setFlags(allModsItem->flags() & ~Qt::ItemIsEditable);
    root->appendRow(enabledModsItem);
    root->appendRow(allModsItem);
    for(int i=0; i<modManager->getEnabledModOrder()->size(); i++)
    {
        QStandardItem* modItem = new QStandardItem(/*TODO icon,*/ modManager->getEnabledModOrder()->at(i)->prettyName);
        modItem->setFlags(modItem->flags() & ~Qt::ItemIsEditable);
        enabledModsItem->appendRow(modItem);
    }
    for(int i=0; i<modManager->getMods()->size(); i++)
    {
        QStandardItem* modItem = new QStandardItem(/*TODO icon,*/ modManager->getMods()->at(i)->prettyName);
        modItem->setFlags(modItem->flags() & ~Qt::ItemIsEditable);
        allModsItem->appendRow(modItem);
    }

    ui->setupUi(this);

    // Set some initial GUI display
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

void MainWindow::on_actionSettings_triggered()
{
    //TODO display settings window
}

void MainWindow::on_buttonInstallMod_clicked()
{
    QString modArchive = QFileDialog::getOpenFileName(this, "Select mod to install...", "./", "ZIP Files (*.zip)");
    showError(modManager->install(modArchive));
}

void MainWindow::on_buttonEnableMod_clicked()
{
    //TODO enable mod
}

void MainWindow::on_buttonDisableMod_clicked()
{
    //TODO disable mod
}

void MainWindow::on_buttonRemoveMod_clicked()
{
    //TODO remove mod
}

void MainWindow::on_treeViewMods_clicked(const QModelIndex &index)
{
    ModManager::Mod* mod;
    if(index.parent() == enabledModsItem->index())
        mod = modManager->getEnabledModOrder()->at(index.row());
    else if(index.parent() == allModsItem->index())
        mod = modManager->getMods()->at(index.row());
    else
        return; //selected catagory label rather than mod

    ui->labelModName->setText(mod->prettyName);
    ui->labelModAuthor->setText(modManager->getEnabledModOrder()->at(index.row())->author);
    ui->labelModVersion->setText(modManager->getEnabledModOrder()->at(index.row())->version);
    QString version;
    QTextStream versionStream(&version);
    versionStream << modManager->getEnabledModOrder()->at(index.row())->gameVersion;
    ui->labelModGameVersion->setText(version);
}

void MainWindow::on_treeViewMods_activated(const QModelIndex &index)
{
    on_treeViewMods_clicked(index);
}
