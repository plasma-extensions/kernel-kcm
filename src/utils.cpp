#include "utils.h"
#include <QThreadPool>

#include "loadkerneldatatask.h"
#include "installkerneltask.h"
#include "uninstallkerneltask.h"
#include "makedefaulttask.h"

Utils::Utils()
{

}

QFuture<QList<QVariantMap> > Utils::loadKernelsData()
{
    LoadKernelDataTask * task = new LoadKernelDataTask();
    QThreadPool::globalInstance()->start(task);
}

QFuture<void> Utils::makeDefault(QString name)
{

}

QFuture<void> Utils::installKernel(QString name)
{

}

QFuture<void> Utils::removeKernel(QString name)
{

}

QString Utils::getActiveKernel()
{

}

QString Utils::getDefaultKernel()
{

}

bool Utils::isLts(QString name)
{

}

bool Utils::isActive(QString name)
{

}

bool Utils::isDefault(QString name)
{

}

bool Utils::isInstalled(QString name)
{

}

