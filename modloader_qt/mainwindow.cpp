#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Load settings
    QCoreApplication::setOrganizationName("Utudio");
    QCoreApplication::setApplicationName("Jackal Mod Loader");
    QCoreApplication::setApplicationVersion("0.1.0");
    settings = new QSettings("settings.ini", QSettings::IniFormat);
    QString gamePath = settings->value("game/path", "C:/Sandswept Studios/The Dead Linger Alpha/").toString();
    int gameVersion = settings->value("game/version", -1).toInt();

    errorMessageBox = new QMessageBox;
    errorMessageBox->setIcon(QMessageBox::Critical);

    // Initialize Mod Manager
    modManager = new ModManager(gamePath);
    while(!modManager->getGameDirectoryValid())
    {
        // Check gamepath and have user select if invalid
        gamePath = QFileDialog::getOpenFileName(this, "Select TDL executable...", "./", "TDL Executable (TDL.exe)");
        if(gamePath.isEmpty() || gamePath.isNull())
            exit(0);
        for(int i=gamePath.size()-1; i>=0; i--)
        {
            if(gamePath[i] == QChar('/'))
            {
                gamePath.truncate(i);
                break;
            }
        }qDebug()<<gamePath;
        modManager->checkGameDirectory(gamePath);
    }
    settings->setValue("game/path", gamePath);
    showError(modManager->load("C:/Users/Jason/AppData/Roaming/Sandswept Studios/The Dead Linger/tdlversion.txt"));
    if(gameVersion != modManager->getVersion())
        settings->setValue("game/version", modManager->getVersion()); //may need to do something special for updates?
modManager->save();//TODO fix and remove
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

    ui->buttonEnableMod->setEnabled(false);
    ui->buttonModOrderUp->setEnabled(false);
    ui->buttonModOrderDown->setEnabled(false);
    ui->buttonRemoveMod->setEnabled(false);

    ui->treeViewMods->setHeaderHidden(true);
    ui->treeViewMods->setModel(&modListTreeModel);

    //TODO actually have settings
    ui->actionSettings->setEnabled(false);
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

int MainWindow::getSelectedModIndex()
{
    int modIndex = -1;

    QModelIndex index;
    if(ui->treeViewMods->selectionModel()->selectedIndexes().size() > 0)
        index = ui->treeViewMods->selectionModel()->selectedIndexes().at(0);
    else
        return modIndex;

    if(index.parent() == enabledModsItem->index())
    {
        for(int i=0; i<modManager->getMods()->size(); i++)
        {
            if(modManager->getMods()->at(i) == modManager->getEnabledModOrder()->at(index.row()))
                modIndex = i;
        }
    }
    else if(index.parent() == allModsItem->index())
        modIndex = index.row();

    return modIndex; //selected catagory label rather than mod
}

void MainWindow::on_actionSettings_triggered()
{
    //TODO display settings window
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}

void MainWindow::on_actionOnline_Help_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/jpeg/TDL-Mod-Loader/wiki"));
}

void MainWindow::on_actionAbout_triggered()
{
    About* w = new About(this);
    w->show();
}

void MainWindow::on_buttonInstallMod_clicked()
{
    QString modArchive = QFileDialog::getOpenFileName(this, "Select mod to install...", "./", "ZIP Files (*.zip)");
    showError(modManager->install(modArchive));
}

void MainWindow::on_buttonEnableMod_clicked()
{
    int modIndex = getSelectedModIndex();

    if(modIndex >= 0)
    {
        if(modManager->getMods()->at(modIndex)->enabled)
        {
            // Disable mod
            for(int i=0; i<modManager->getEnabledModOrder()->size(); i++)
            {
                if(modManager->getMods()->at(modIndex) == modManager->getEnabledModOrder()->at(i))
                {
                    enabledModsItem->removeRow(i);
                    break;
                }
            }
            ui->buttonEnableMod->setText("Enable");
            showError(modManager->disableMod(modIndex));
            showError(modManager->save());
        }
        else
        {
            // Enable mod
            QStandardItem* modItem = new QStandardItem(/*TODO icon,*/ modManager->getMods()->at(modIndex)->prettyName);
            modItem->setFlags(modItem->flags() & ~Qt::ItemIsEditable);
            enabledModsItem->appendRow(modItem);
            ui->buttonEnableMod->setText("Disable");
            showError(modManager->enableMod(modIndex));
            showError(modManager->save());
        }
    }
}

void MainWindow::on_buttonModOrderUp_clicked()
{
    int modIndex = getSelectedModIndex();

    if(modIndex >= 0)
    {
        int enabledModIndex = -1;
        for(int i=0; i<modManager->getEnabledModOrder()->size(); i++)
        {
            if(modManager->getMods()->at(modIndex) == modManager->getEnabledModOrder()->at(i))
            {
                enabledModIndex = i;
                break;
            }
        }
        if(enabledModIndex >= 1 && enabledModIndex < modManager->getEnabledModOrder()->size())
        {
            modManager->modOrderUp(enabledModIndex);
            modManager->save();
            QString temp = enabledModsItem->child(enabledModIndex-1)->text();
            enabledModsItem->child(enabledModIndex-1)->setText(enabledModsItem->child(enabledModIndex)->text());
            enabledModsItem->child(enabledModIndex)->setText(temp);
        }
    }
}

void MainWindow::on_buttonModOrderDown_clicked()
{
    int modIndex = getSelectedModIndex();

    if(modIndex >= 0)
    {
        int enabledModIndex = -1;
        for(int i=0; i<modManager->getEnabledModOrder()->size(); i++)
        {
            if(modManager->getMods()->at(modIndex) == modManager->getEnabledModOrder()->at(i))
            {
                enabledModIndex = i;
                break;
            }
        }
        if(enabledModIndex >= 0 && enabledModIndex < modManager->getEnabledModOrder()->size() - 1)
        {
            modManager->modOrderDown(enabledModIndex);
            modManager->save();
            QString temp = enabledModsItem->child(enabledModIndex+1)->text();
            enabledModsItem->child(enabledModIndex+1)->setText(enabledModsItem->child(enabledModIndex)->text());
            enabledModsItem->child(enabledModIndex)->setText(temp);
        }
    }
}

void MainWindow::on_buttonRemoveMod_clicked()
{
    int modIndex = getSelectedModIndex();

    if(modIndex >= 0)
    {
        if(modManager->getMods()->at(modIndex)->enabled)
        {
            // Disable mod
            for(int i=0; i<modManager->getEnabledModOrder()->size(); i++)
            {
                if(modManager->getMods()->at(modIndex) == modManager->getEnabledModOrder()->at(i))
                {
                    enabledModsItem->removeRow(i);
                    break;
                }
            }
            showError(modManager->disableMod(modIndex));
            showError(modManager->save());
        }
        allModsItem->removeRow(modIndex);
        modManager->remove(modIndex);
    }
}

void MainWindow::on_treeViewMods_clicked(const QModelIndex &index)
{
    ModManager::Mod* mod;
    if(index.parent() == enabledModsItem->index())
    {
        mod = modManager->getEnabledModOrder()->at(index.row());
        if(index.row() > 0)
            ui->buttonModOrderUp->setEnabled(true);
        else
            ui->buttonModOrderUp->setEnabled(false);
        if(index.row() < enabledModsItem->rowCount()-1)
            ui->buttonModOrderDown->setEnabled(true);
        else
            ui->buttonModOrderDown->setEnabled(false);
    }
    else if(index.parent() == allModsItem->index())
    {
        mod = modManager->getMods()->at(index.row());
        ui->buttonModOrderUp->setEnabled(false);
        ui->buttonModOrderDown->setEnabled(false);
    }
    else
    {
        // Selected catagory label rather than mod
        ui->buttonEnableMod->setEnabled(false);
        ui->buttonModOrderUp->setEnabled(false);
        ui->buttonModOrderDown->setEnabled(false);
        ui->buttonRemoveMod->setEnabled(false);
        return;
    }

    ui->labelModName->setText(mod->prettyName);
    ui->labelModAuthor->setText(mod->author);
    ui->labelModVersion->setText(mod->version);
    QString version;
    QTextStream versionStream(&version);
    versionStream << mod->gameVersion;
    ui->labelModGameVersion->setText(version);

    ui->buttonEnableMod->setEnabled(true);
    if(mod->enabled)
        ui->buttonEnableMod->setText("Disable");
    else
        ui->buttonEnableMod->setText("Enable");
    ui->buttonRemoveMod->setEnabled(true);
}

void MainWindow::on_treeViewMods_activated(const QModelIndex &index)
{
    on_treeViewMods_clicked(index);
}
