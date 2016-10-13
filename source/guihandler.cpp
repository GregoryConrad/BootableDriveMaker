#include "guihandler.h"
#include <QFile>
#include <QTextStream>

guiHandler::guiHandler() : hasFinished(false) {}
void guiHandler::run() {
    QString content, oldBigLog = "";
    while(!hasFinished) {
        //Update linelog
        QFile linelog("currStep.txt");
        if (linelog.open(QIODevice::ReadOnly)) {
            emit setLineLog(QTextStream(&linelog).readLine());
            linelog.close();
        }
        //Update biglog
        QFile biglog("cmdOut.txt");
        if (biglog.open(QIODevice::ReadOnly)) {
            content = QTextStream(&biglog).readAll();
            if (content != oldBigLog) {
                oldBigLog = content;
                emit setBigLog(content);
                msleep(50);
                emit autoScroll();
            }
            biglog.close();
        }
        msleep(200);
    }
}
void guiHandler::done(int,QProcess::ExitStatus) { hasFinished = true; }
