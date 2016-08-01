#include "uninstallkerneltask.h"

#include <QProcess>
#include <QStringList>


UninstallKernelTask::UninstallKernelTask(QString name)
{
    _name = name;
}

void UninstallKernelTask::run()
{
    qDebug() << "uninstalling " << _name;
    QProcess apt;
    apt.start("kdesudo", QStringList() << "apt-get -y purge " + _name );

    if (!apt.waitForFinished()) {
        qDebug() << apt.readAllStandardError();
        futureInterface.reportException(QException());
        return;
    }

    qDebug() << apt.readAll();
    qDebug() << apt.readAllStandardError();
    futureInterface.reportFinished();
}

QFuture<void> UninstallKernelTask::future()
{
    return futureInterface.future();
}


