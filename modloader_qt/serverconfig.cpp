#include "serverconfig.h"
#include "ui_serverconfig.h"

ServerConfig::ServerConfig(QWidget *parent, QSettings* settings) :
    QDialog(parent),
    ui(new Ui::ServerConfig)
{
    m_accept = false;

    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setFixedSize(this->width(), this->height());

    m_settings = settings;
    //TODO init config ui
}

ServerConfig::~ServerConfig()
{
    delete ui;
}

void ServerConfig::on_buttonBox_clicked(QAbstractButton *button)
{
    bool close = false;
    switch(ui->buttonBox->standardButton(button))
    {
    case QDialogButtonBox::Ok:
        close = true;
    case QDialogButtonBox::Apply:
        m_accept = true;
        //TODO save server settings
        break;
    case QDialogButtonBox::Cancel:
        close = true;
        break;
    default:
        break;
    }
    if(close && m_accept)
        this->accept();
    else if(close)
        this->reject();
}
