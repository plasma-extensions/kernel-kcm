#include "updatekerneltask.h"

#include <QProcess>

UpdateKernelTask::UpdateKernelTask(QString name)
{
    _name = name;
}

void UpdateKernelTask::run()
{
    qDebug() << "Updating " << _name;
    QProcess apt;
    apt.start("kdesudo", QStringList() << "apt-get -y install " + _name );

    if (!apt.waitForFinished()) {
        qDebug() << apt.readAllStandardError();
        futureInterface.reportException(QException());
        return;
    }

    qDebug() << apt.readAll();
    qDebug() << apt.readAllStandardError();
    futureInterface.reportFinished();
}

QFuture<void> UpdateKernelTask::future()
{
    return futureInterface.future();
}
