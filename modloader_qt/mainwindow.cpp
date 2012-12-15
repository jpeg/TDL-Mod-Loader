#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    int error = config.init("C:\\Users\\Jason\\AppData\\Roaming\\Sandswept Studios\\The Dead Linger\\tdlversion.txt", "C:\\Sandswept Studios\\The Dead Linger Alpha\\Plugins.cfg", "C:\\Sandswept Studios\\The Dead Linger Alpha\\content\\resources.cfg");
    if(error != Error::NO_ERROR)
    {
        QString errStr;
        QTextStream out(&errStr);
        out << "Failed to init config: " << error;
        ui->labelTDLVersion->setText(errStr);
        return;
    }

    QString outStr;
    QTextStream out(&outStr);
    out << "TDL Version: " << config.getVersion();
    ui->labelTDLVersion->setText(outStr);

    config.addPlugin(0, "testMod", "testPlugin");
    config.addResource(0, "testMod", "testResource");

    config.save();
}

MainWindow::~MainWindow()
{
    delete ui;
}
