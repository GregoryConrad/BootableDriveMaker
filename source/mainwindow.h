#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include "guihandler.h"

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
    bool hasStarted;
    QProcess *process;
    guiHandler *guihandler;
    QString isoPath;
private slots:
    void setLineLog(QString);
    void setBigLog(QString);
    void done(int,QProcess::ExitStatus);
    void on_ubuntu_clicked();
    void on_debian_clicked();
    void on_openFile_clicked();
    void on_startStop_clicked();
};

#endif // MAINWINDOW_H
