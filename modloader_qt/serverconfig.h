#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include <QDialog>
#include <QSettings>
#include <QAbstractButton>

#include "servermanager.h"

namespace Ui {
class ServerConfig;
}

class ServerConfig : public QDialog
{
    Q_OBJECT
    
public:
    explicit ServerConfig(QWidget *parent, ServerManager* serverManager);
    ~ServerConfig();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    
private:
    Ui::ServerConfig *ui;
    ServerManager* m_serverManager;
    bool m_accept;
};

#endif // SERVERCONFIG_H
