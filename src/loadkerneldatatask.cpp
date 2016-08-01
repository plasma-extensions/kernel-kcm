#include "loadkerneldatatask.h"

#include <QException>
#include <QString>
#include <QProcess>
#include <QDebug>

LoadKernelDataTask::LoadKernelDataTask()
{
}

LoadKernelDataTask::~LoadKernelDataTask()
{
}

void LoadKernelDataTask::run()
{

    futureInterface.reportStarted();

    QList<QVariantMap> * kernelsInfo = new QList<QVariantMap>();

    /**
     * Discover current active kernel
     */
    QProcess process;
    process.start("uname", QStringList() << "-r");
    if (!process.waitForFinished()) {
        futureInterface.reportException(QException());
        return;
    }

    QString current_kernel = QString(process.readAll()).trimmed();

    // qDebug() << "Current active Kernel " << current_kernel;

    /**
     * Get upgradeable Kernels
     */
    process.start("apt-get", QStringList() << "-qqs" << "upgrade");
    if (!process.waitForFinished()) {
        futureInterface.reportException(QException());
        return;
    }
    QString upgradable_packages = QString(process.readAll()).trimmed();

    /**
     * List all available kernels from apt-cache
     */
    QProcess apt_cache;
    apt_cache.start("apt-cache", QStringList() << "search" << "Linux kernel image" << "linux-image");

    if (!apt_cache.waitForFinished()){
        futureInterface.reportException(QException());
        return;
    }

    QStringList lines = QString(apt_cache.readAll()).split("\n");

    futureInterface.setProgressRange(0,lines.size());
    futureInterface.setProgressValueAndText(0, "Getting Kernels Info");
    int i = 0;
    for (QString line: lines) {
        futureInterface.setProgressValue(i++);
        if (line.isEmpty())
            continue;
        QVariantMap item;
        QStringList parts = line.split(" - ");
        QString name = parts.at(0);
        item["Name"] = name;
        if (parts.size() > 1)
            item["Description"] = parts.at(1);

        // Check if installed
        QProcess dpkg_query;
        dpkg_query.start("dpkg-query", QStringList() << "-f" << "${db:Status-Abbrev}\n}" << "-W" << item["Name"].toString());
        if (!dpkg_query.waitForFinished()){
            futureInterface.reportException(QException());
            return;
        }

        QByteArray dpkg_query_result = dpkg_query.readAll();

        item["IsInstalled"] = dpkg_query_result.startsWith("ii");

        item["IsActive"] = name.contains(current_kernel);

        item["IsUpgradable"] = upgradable_packages.contains(name);

        //qDebug() << item;
        kernelsInfo->append(item);
    }

    futureInterface.reportFinished(kernelsInfo);
}

QFuture<QList<QVariantMap> > LoadKernelDataTask::future()
{
    return futureInterface.future();
}

