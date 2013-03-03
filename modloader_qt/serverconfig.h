#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include <QDialog>
#include <QSettings>
#include <QAbstractButton>

namespace Ui {
class ServerConfig;
}

class ServerConfig : public QDialog
{
    Q_OBJECT
    
public:
    explicit ServerConfig(QWidget *parent, QSettings* settings);
    ~ServerConfig();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    
private:
    Ui::ServerConfig *ui;
    QSettings* m_settings;
    bool m_accept;
};

#endif // SERVERCONFIG_H
