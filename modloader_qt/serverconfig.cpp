#include "serverconfig.h"
#include "ui_serverconfig.h"

ServerConfig::ServerConfig(QWidget *parent, ServerManager* serverManager) :
    QDialog(parent),
    ui(new Ui::ServerConfig)
{
    m_accept = false;

    ui->setupUi(this);

    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setFixedSize(this->width(), this->height());

    m_serverManager = serverManager;

    ui->serverNameLineEdit->setText(m_serverManager->getServerName());
    ui->serverPasswordLineEdit->setText(m_serverManager->getPassword());
    ui->adminPasswordLineEdit->setText(m_serverManager->getAdminPassword());
    ui->maxPlayersSpinBox->setValue(m_serverManager->getMaxPlayers());
    ui->checkBoxGameModePublic->setChecked(m_serverManager->getGamemodePublic());
    ui->checkBoxCustomContentConfig->setChecked(m_serverManager->getCustomContentConfig());
}

ServerConfig::~ServerConfig()
{
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
        m_serverManager->setServerName(ui->serverNameLineEdit->text());
        m_serverManager->setPassword(ui->serverPasswordLineEdit->text());
        m_serverManager->setAdminPassword(ui->adminPasswordLineEdit->text());
        m_serverManager->setMaxPlayers(ui->maxPlayersSpinBox->value());
        m_serverManager->setGamemodePublic(ui->checkBoxGameModePublic->isChecked());
        m_serverManager->setCustomContentConfig(ui->checkBoxCustomContentConfig->isChecked());
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
