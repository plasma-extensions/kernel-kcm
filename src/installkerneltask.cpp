#include "installkerneltask.h"

#include <QProcess>

InstallKernelTask::InstallKernelTask(QString name)
{
    _name = name;
}

void InstallKernelTask::run()
{
    QProcess apt;
    apt.start("kdesudo", QStringList() << "apt-get -y install " + _name);

    if (!apt.waitForFinished()) {
        futureInterface.reportException(QException());
        return;
    }

    futureInterface.reportFinished();
}

QFuture<void> InstallKernelTask::future()
{
    return futureInterface.future();
}

