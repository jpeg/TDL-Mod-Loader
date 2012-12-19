#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <modmanager.h>

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

    ModManager* modManager;
};

#endif // MAINWINDOW_H
