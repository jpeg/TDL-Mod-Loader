#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);

    QString version;
    QTextStream versionStream(&version);
    versionStream << "Version: " << QCoreApplication::applicationVersion();
    ui->labelVersion->setText(version);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setFixedSize(this->width(), this->height());
}

About::~About()
{
    delete ui;
}
