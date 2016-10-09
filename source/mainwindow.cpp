#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
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
    QStringList itemsToAdd;
    itemsToAdd << "1. Ubuntu 16.04 (64-Bit)";
    itemsToAdd << "2. Debian 8.6.0 (64-Bit, Internet Installer)";
    itemsToAdd << "3. Ubuntu 16.04 (32-Bit)";
    itemsToAdd << "4. Debian 8.6.0 (32-Bit, Internet Installer)";
    ui->osSelector->addItems(itemsToAdd);
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
void MainWindow::on_openISO_clicked() {
    QProcess getUsername;
    getUsername.start("sh findUsername.sh");
    getUsername.waitForFinished();
    QFile file("username.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    QString filePath = QFileDialog::getOpenFileName(this,tr("Select An Operating System File"),"/Users/" + in.readLine() + "/Downloads","ISO (*.iso)");
    if (filePath != "") osPath = filePath;
    ui->pathReadOut->setText(osPath);
}
void MainWindow::on_openIMG_clicked() {
    QProcess getUsername;
    getUsername.start("sh findUsername.sh");
    getUsername.waitForFinished();
    QFile file("username.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    QString filePath = QFileDialog::getOpenFileName(this,tr("Select An Operating System File"),"/Users/" + in.readLine() + "/Downloads","IMG (*.img)");
    if (filePath != "") osPath = filePath;
    ui->pathReadOut->setText(osPath);
}
void MainWindow::on_startStop_clicked() {
    if (hasStarted) {
        close();
        return;
    }
    if (ui->devID->currentText().split(":")[0] == "ERROR") {
        ui->plainTextEdit->appendPlainText("Cannot Start. Please See Error Above.");
        return;
    }
    if (ui->devID->currentText() == "No External Devices Found") {
        ui->plainTextEdit->appendPlainText("Please Put A USB Drive Into The Computer Or Enable The Use Of Internal Drives And Refresh The Device List.");
        return;
    }
    if (osPath == "") {
        ui->plainTextEdit->appendPlainText("You need to select a file first.");
        return;
    }
    if (ui->devID->currentText().contains("internal")) {
        QMessageBox msgBox;
        msgBox.setText("You have selected an internal device for use.\nTo abort, click cancel in the authorization prompt after closing this message.\nTo continue, just close this message.");
        msgBox.exec();
    }
    hasStarted = true;
    ui->log->setText("Starting...");
    ui->startStop->setText("Cancel");
    ui->devID->setEnabled(false);
    ui->openISO->setEnabled(false);
    ui->openIMG->setEnabled(false);
    ui->downloadOS->setEnabled(false);
    ui->osSelector->setEnabled(false);
    ui->allowNonExtern->setEnabled(false);
    ui->refreshDevs->setEnabled(false);
    ui->pathReadOut->setEnabled(false);
    ui->label->setEnabled(false);
    ui->label_2->setEnabled(false);
    ui->label_3->setEnabled(false);
    ui->label_4->setEnabled(false);

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
    if (allDevs.length() < 3) {
        ui->plainTextEdit->clear();
        ui->plainTextEdit->appendPlainText("\nCould Not Find Any Connected Devices.");
        ui->plainTextEdit->appendPlainText("Either this application is not in /Applications (or directly in your Documents or Desktop folder) or something is seriously wrong.");
        ui->plainTextEdit->appendPlainText("Cannot Continue. Please Correct This Problem And Try Again.");
        ui->devID->clear();
        ui->devID->addItem("ERROR: Could Not Find Devices. See Below.");
        return;
    }
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
    if (!ui->allowNonExtern->isChecked()) {
        for (int i = 0; i < devs.length(); ++i) {
            if (!devs.at(i).contains("external")) {
                devs.removeAt(i);
                --i;
            }
        }
    }
    if (!ui->allowNonExtern->isChecked() && devs.isEmpty()) {
        devs << "No External Devices Found";
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
void MainWindow::on_downloadOS_clicked() {
    switch(ui->osSelector->currentText().split(".")[0].toInt()) {
    case 1:
        if (!QDesktopServices::openUrl(QUrl(QString("http://mirrors.mit.edu/ubuntu-releases/16.04/ubuntu-16.04-desktop-amd64.iso"))))
            ui->plainTextEdit->appendPlainText("Failed To Open URL.");
        break;
    case 2:
        if (!QDesktopServices::openUrl(QUrl(QString("http://gensho.acc.umu.se/debian-cd/8.6.0/amd64/iso-cd/debian-8.6.0-amd64-netinst.iso"))))
            ui->plainTextEdit->appendPlainText("Failed To Open URL.");
        break;
    case 3:
        if (!QDesktopServices::openUrl(QUrl(QString("http://mirrors.mit.edu/ubuntu-releases/16.04/ubuntu-16.04-desktop-i386.iso"))))
            ui->plainTextEdit->appendPlainText("Failed To Open URL.");
        break;
    case 4:
        if (!QDesktopServices::openUrl(QUrl(QString("http://gensho.acc.umu.se/debian-cd/8.6.0/i386/iso-cd/debian-8.6.0-i386-netinst.iso"))))
            ui->plainTextEdit->appendPlainText("Failed To Open URL.");
        break;
    }
}
void MainWindow::on_allowNonExtern_clicked() {
    on_refreshDevs_clicked();
}
void MainWindow::on_actionSelectISO_triggered() { if(!hasStarted) on_openISO_clicked(); }
void MainWindow::on_actionSelectIMG_triggered() { if(!hasStarted) on_openIMG_clicked(); }
void MainWindow::on_actionContact_triggered() {
    QMessageBox::about(this,"Contact","Email: contact@etcg.pw\nWebsite: http://www.etcg.pw");
}
void MainWindow::on_actionCopyright_triggered() {
    QFile copyNotice("../Resources/copyrightNotice.txt"),
          copy      ("../Resources/COPYING"),
          copyLesser("../Resources/COPYING.LESSER");
    if (copyNotice.open(QIODevice::ReadOnly) && copy.open(QIODevice::ReadOnly)
        && copyLesser.open(QIODevice::ReadOnly)) {
        QTextStream inCopyNotice(&copyNotice), inCopy(&copy), inCopyLesser(&copyLesser);
        QDialog *dialog = new QDialog;
        QLabel *lb1 = new QLabel, *lb2 = new QLabel, *lb3 = new QLabel;
        lb1->setText("Copyright Notice:");
        lb2->setText("GPL License:");
        lb3->setText("LGPL License:");
        QList<QPlainTextEdit*> textHolders;
        for (int i = 0; i < 3; ++i) {
            QString textToAdd = "";
            switch (i) {
            case 0:
                textToAdd = inCopyNotice.readAll();
                break;
            case 1:
                textToAdd = inCopy.readAll();
                break;
            case 2:
                textToAdd = inCopyLesser.readAll();
                break;
            }
            textHolders.append(new QPlainTextEdit);
            textHolders.at(i)->appendPlainText(textToAdd);
            textHolders.at(i)->setReadOnly(true);
            QTextCursor cursor = textHolders.at(i)->textCursor();
            cursor.movePosition(QTextCursor::Start);
            textHolders.at(i)->setTextCursor(cursor);
        }
        QVBoxLayout *v1 = new QVBoxLayout, *v2 = new QVBoxLayout, *v3 = new QVBoxLayout;
        v1->addWidget(lb1);
        v1->addWidget(textHolders.at(0));
        v2->addWidget(lb2);
        v2->addWidget(textHolders.at(1));
        v3->addWidget(lb3);
        v3->addWidget(textHolders.at(2));
        QHBoxLayout *hbox = new QHBoxLayout;
        hbox->addLayout(v1);
        hbox->addLayout(v2);
        hbox->addLayout(v3);
        dialog->setLayout(hbox);
        dialog->setFixedSize(1100,600);
        dialog->setWindowTitle("Copyright");
        dialog->exec();
        delete dialog;
    }
    else QMessageBox::about(this,"Failed To Open","Failed To Open One Or More Copyright Files.");
}
void MainWindow::on_actionREADME_triggered() {
    QFile README("../Resources/README.md");
    if (README.open(QIODevice::ReadOnly)) {
        QTextStream in(&README);
        //Make dialog
        QDialog dialog;
        //Make textHolder for dialog
        QPlainTextEdit textHolder;
        textHolder.appendPlainText(in.readAll());
        textHolder.setReadOnly(true);
        QTextCursor cursor = textHolder.textCursor();
        cursor.movePosition(QTextCursor::Start);
        textHolder.setTextCursor(cursor);
        //Make textHolder take up all of dialog
        QVBoxLayout layout;
        layout.addWidget(&textHolder);
        //Setup dialog
        dialog.setLayout(&layout);
        dialog.setFixedSize(500,500);
        dialog.setWindowTitle("README");
        dialog.exec();
    }
    else QMessageBox::about(this,"Failed To Open README","Failed To Open README.");
}
