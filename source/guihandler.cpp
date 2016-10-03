#include "guihandler.h"
#include <QFile>
#include <QTextStream>
guiHandler::guiHandler() : hasFinished(false) {}
void guiHandler::run() {
    QString content;
    while(!hasFinished) {
        //Update linelog
        QFile linelog("currStep.txt");
        if (linelog.open(QIODevice::ReadOnly)) {
           content = QTextStream(&linelog).readLine();
           linelog.close();
           emit setLineLog(content);
        }
        //Update biglog
        QFile biglog("cmdOut.txt");
        if (biglog.open(QIODevice::ReadOnly)) {
            content = "";
            QTextStream in(&biglog);
            while(!in.atEnd()) {
                content += "\n" + in.readLine();
            }
            biglog.close();
            emit setBigLog(content);
        }
        emit autoScroll();
        msleep(200);
    }
}
void guiHandler::done(int,QProcess::ExitStatus) {
    hasFinished = true;
}
