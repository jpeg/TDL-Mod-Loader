#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <configmodifier.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;

    ConfigModifier config;
};

#endif // MAINWINDOW_H
