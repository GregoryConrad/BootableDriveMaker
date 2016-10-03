#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Bootable Drive Maker For Mac");
    hasStarted = false;
    process = new QProcess();
    connect(process,SIGNAL(finished(int,QProcess::ExitStatus)),this,SLOT(done(int,QProcess::ExitStatus)));
    guihandler = new guiHandler();
    connect(guihandler,SIGNAL(setLineLog(QString)),this,SLOT(setLineLog(QString)));
    connect(guihandler,SIGNAL(setBigLog(QString)),this,SLOT(setBigLog(QString)));
    connect(process,SIGNAL(finished(int,QProcess::ExitStatus)),guihandler,SLOT(done(int,QProcess::ExitStatus)));
    isoPath = "";
}
MainWindow::~MainWindow() {
    delete ui;
    process->kill();
    delete process;
    delete guihandler;
}
void MainWindow::setLineLog(QString str) {
    ui->log->setText(str);
}
void MainWindow::setBigLog(QString str) {
    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText(str);
}
void MainWindow::done(int exitCode, QProcess::ExitStatus exitStatus) {
    ui->startStop->setText("Quit");
    if (exitCode != 0 || exitStatus != QProcess::NormalExit) {
        ui->plainTextEdit->appendPlainText("\n\nAn Error Has Occured. Error Code: " + QString::number(exitCode));
    }
    else {
        ui->plainTextEdit->appendPlainText("\n\nProcess Finished.");
    }
}
void MainWindow::on_ubuntu_clicked() {
    if (!QDesktopServices::openUrl(QUrl(QString("http://mirrors.mit.edu/ubuntu-releases/16.04/ubuntu-16.04-desktop-amd64.iso")))) {
        ui->plainTextEdit->appendPlainText("Failed To Open URL.");
    }
}
void MainWindow::on_debian_clicked() {
    if (!QDesktopServices::openUrl(QUrl(QString("http://gensho.acc.umu.se/debian-cd/8.6.0/amd64/iso-cd/debian-8.6.0-amd64-netinst.iso")))) {
        ui->plainTextEdit->appendPlainText("Failed To Open URL.");
    }
}
void MainWindow::on_openFile_clicked() {
    isoPath = QFileDialog::getOpenFileName(this,tr("Select An Operating System (ISO)"),"/Users/","ISO (*.iso)");
    ui->pathReadOut->setText(isoPath);
}
void MainWindow::on_startStop_clicked() {
    if (isoPath == "") {
        ui->plainTextEdit->appendPlainText("You need to select an ISO file first.");
        return;
    }
    if (hasStarted) {
        close();
        return;
    }
    hasStarted = true;
    ui->log->setText("Starting...");
    ui->startStop->setText("Cancel");
    ui->deviceID->setReadOnly(true);
    ui->openFile->setEnabled(false);
    ui->ubuntu->setEnabled(false);
    ui->debian->setEnabled(false);

    //Print isoPath to path.txt
    QFile location("path.txt");
    location.open(QIODevice::WriteOnly);
    QTextStream(&location) << isoPath;
    location.close();
    //Print "/dev/rdisk" + ui->deviceID->value() to devID.txt
    QFile dev("devID.txt");
    dev.open(QIODevice::WriteOnly);
    QTextStream(&dev) <<"/dev/rdisk" << ui->deviceID->value();
    dev.close();

    process->start("osascript", QStringList() << "-e" << "do shell script \"sh run.sh\" with administrator privileges");
    guihandler->start();
}
