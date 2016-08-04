#include "loadkerneldatatask.h"

#include <QException>
#include <QString>
#include <QProcess>
#include <QDebug>

#include <QSysInfo>

#include <QApt/Backend>
#include <QApt/Globals>

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

    // Search for Linux kernel packages
    QApt::Backend apt;
    apt.init();
    apt.openXapianIndex();

    QString searchString("linux-image-");

    // The function below return a lot of wrong an duplicated results that we must filter later
    // TODO: Find a better serach function.
    QApt::PackageList pkgList = apt.search(searchString);

    int i = 0;
    for (QApt::Package* pkg: pkgList ) {
        futureInterface.setProgressValue(i++);

        // Excliding wrong search results.

        // By their name
        QString pkgName = pkg->name();
        if (!pkgName.startsWith(searchString))
            continue;

        // Packages are filtered by their short description, this may cause thas some relevant
        // results to be ignored
        if (!pkg->shortDescription().contains("Linux kernel image"))
            continue;

        // Ignore duplicated entries
        bool isDuplicated;
        for (QVariantMap entry: *kernelsInfo)
            if (pkgName.compare(entry["Name"].toString()) == 0) {
                isDuplicated = true;
                break;
            }


        if (isDuplicated)
            break;

        QVariantMap newEntry;

        newEntry["Name"] = pkg->name();
        newEntry["IsInstalled"] = QApt::Package::Installed & pkg->state();
        newEntry["IsActive"] = QString(pkg->name()).contains(QSysInfo::kernelVersion());
        newEntry["IsUpgradable"] = QApt::Package::Upgradeable & pkg->state();

        //qDebug() << item;
        kernelsInfo->append(newEntry);
    }

    futureInterface.reportFinished(kernelsInfo);
}

QFuture<QList<QVariantMap> > LoadKernelDataTask::future()
{
    return futureInterface.future();
}

