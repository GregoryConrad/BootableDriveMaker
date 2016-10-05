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
    connect(guihandler,SIGNAL(autoScroll()),this,SLOT(autoScroll()));
    connect(process,SIGNAL(finished(int,QProcess::ExitStatus)),guihandler,SLOT(done(int,QProcess::ExitStatus)));
    osPath = "";
    on_refreshDevs_clicked();
    QProcess removeFiles;
    removeFiles.start("sh removeFiles.sh");
    removeFiles.waitForFinished();
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
        ui->log->setText("Process Finished.");
        ui->plainTextEdit->appendPlainText("\nProcess Finished.");
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
void MainWindow::on_openISO_clicked() {
    QProcess getUsername;
    getUsername.start("sh findUsername.sh");
    getUsername.waitForFinished();
    QFile file("username.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    osPath = QFileDialog::getOpenFileName(this,tr("Select An Operating System File"),"/Users/" + in.readLine() + "/Downloads","ISO (*.iso)");
    ui->pathReadOut->setText(osPath);
}
void MainWindow::on_openIMG_clicked() {
    QProcess getUsername;
    getUsername.start("sh findUsername.sh");
    getUsername.waitForFinished();
    QFile file("username.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    osPath = QFileDialog::getOpenFileName(this,tr("Select An Operating System File"),"/Users/" + in.readLine() + "/Downloads","IMG (*.img)");
    ui->pathReadOut->setText(osPath);
}
void MainWindow::on_startStop_clicked() {
    if (osPath == "") {
        ui->plainTextEdit->appendPlainText("You need to select a file first.");
        return;
    }
    if (hasStarted) {
        close();
        return;
    }
    hasStarted = true;
    ui->log->setText("Starting...");
    ui->startStop->setText("Cancel");
    ui->devID->setEnabled(false);
    ui->openISO->setEnabled(false);
    ui->openIMG->setEnabled(false);
    ui->ubuntu->setEnabled(false);
    ui->debian->setEnabled(false);
    ui->refreshDevs->setEnabled(false);

    //Print osPath to path.txt
    QFile location("path.txt");
    location.open(QIODevice::WriteOnly);
    QTextStream(&location) << osPath;
    location.close();
    //Print rdisk device to devID.txt
    QFile dev("devID.txt");
    dev.open(QIODevice::WriteOnly);
    QTextStream(&dev) << "/dev/rdisk" << devIDToInt();
    dev.close();

    process->start("osascript", QStringList() << "-e" << "do shell script \"sh run.sh\" with administrator privileges");
    guihandler->start();
}
void MainWindow::autoScroll() {
    QTextCursor c = ui->plainTextEdit->textCursor();
    c.movePosition(QTextCursor::End);
    ui->plainTextEdit->setTextCursor(c);
}
void MainWindow::on_refreshDevs_clicked() {
    QProcess getDevs;
    getDevs.start("sh findDevs.sh");
    getDevs.waitForFinished();
    QFile file("connectedDevs.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    QString allDevs = in.readAll(), currLine = "";
    file.close();
    QStringList devs;
    for (int i = 0; i < allDevs.length(); ++i) {
        QString c = allDevs.at(i);
        if (c != ":") {
            if (c != "\n") {
                currLine += c;
            }
        }
        else {
            devs.append(currLine);
            currLine = "";
        }
    }
    ui->devID->clear();
    ui->devID->addItems(devs);
}
QString MainWindow::devIDToInt() {
    QString newStr = "";
    for (int i = 0; i < ui->devID->currentText().length(); ++i) {
        QString curr = ui->devID->currentText().at(i);
        if (curr == "0") newStr += "0";
        else if (curr == "1") newStr += curr;
        else if (curr == "2") newStr += curr;
        else if (curr == "3") newStr += curr;
        else if (curr == "4") newStr += curr;
        else if (curr == "5") newStr += curr;
        else if (curr == "6") newStr += curr;
        else if (curr == "7") newStr += curr;
        else if (curr == "8") newStr += curr;
        else if (curr == "9") newStr += curr;
    }
    return newStr;
}
