#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include "guihandler.h"

namespace Ui { class MainWindow; }
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    bool hasStarted;
    QProcess *process;
    guiHandler guihandler;
    QString osPath;
    QString devIDToInt();
private slots:
    void setLineLog(QString);
    void setBigLog(QString);
    void done(int,QProcess::ExitStatus);
    void on_startStop_clicked();
    void autoScroll();
    void on_refreshDevs_clicked();
    void on_downloadOS_clicked();
    void on_allowNonExtern_clicked();
    void on_openISO_clicked();
    void on_openIMG_clicked();
    void on_openDMG_clicked();
    void on_actionSelectISO_triggered();
    void on_actionSelectIMG_triggered();
    void on_actionSelectDMG_triggered();
    void on_actionContact_triggered();
    void on_actionCopyright_triggered();
    void on_actionREADME_triggered();
};

#endif // MAINWINDOW_H
