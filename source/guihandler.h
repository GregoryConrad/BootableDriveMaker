#ifndef GUIHANDLER_H
#define GUIHANDLER_H

#include <QThread>
#include <QProcess>

class guiHandler : public QThread
{
    Q_OBJECT
    void run();
    bool hasFinished;
public:
    guiHandler();
signals:
    void setLineLog(QString);
    void setBigLog(QString);
    void autoScroll();
public slots:
    void done(int,QProcess::ExitStatus);
};

#endif // GUIHANDLER_H
