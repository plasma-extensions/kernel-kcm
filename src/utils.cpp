#include "utils.h"

#include <QDebug>

#include <QApt/Backend>
#include <QApt/Package>
#include <QApt/Transaction>

QApt::Backend* Utils::backend = NULL;

using namespace QApt;

QList<QVariantMap> Utils::loadKernelsData()
{
    // qDebug() << "loadKernelsData";

    QList<QVariantMap> newEntries;
    QString searchString("linux-image-");

    // The function below returns a lot of wrong/duplicated results that we must filter later
    // TODO: Find a better search function.
    QApt::PackageList pkgList = Utils::getAptBackend()->search(searchString);

    for (QApt::Package* pkg: pkgList ) {
        // Excluding wrong search results.

        QString pkgName = pkg->name();
        if (!pkgName.startsWith(searchString))
            continue;

        // Packages are filtered by their short description, this may cause thas some relevant
        // results to be ignored
        if (!pkg->shortDescription().contains("Linux kernel image"))
            continue;

        // Ignore duplicated entries
        bool isDuplicated;
        for (QVariantMap entry: newEntries)
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

        // qDebug() << newEntry;
        newEntries.append(newEntry);
    }

    return newEntries;
}

void Utils::setAsDefault(QString name)
{

}

void Utils::installKernel(QString name)
{
    Package *package  = getAptBackend()->package(name);
    if (!package == 0) {
        PackageList pkgs;

        pkgs << package;
        Transaction *transaction = getAptBackend()->installPackages(pkgs);


        qDebug() << transaction->untrustedPackages();
        transaction->replyUntrustedPrompt(true);
        transaction->setLocale(QLatin1String(setlocale(LC_MESSAGES, 0)));

        QObject::connect(transaction, &Transaction::statusChanged, [=] (TransactionStatus status) {
            qDebug() << "status:" << status;
            qDebug() << transaction->statusDetails();
        });

        QObject::connect(transaction, &Transaction::promptUntrusted, [=] (QStringList untrustedPackages) {
            qDebug() << "untrustedPackages " << untrustedPackages;
            transaction->replyUntrustedPrompt(true);
        });

        QObject::connect(transaction, &Transaction::progressChanged, [=] (int progress) {
            qDebug() << "progress" << progress ;
        });

        QObject::connect(transaction, &Transaction::finished, [=] (ExitStatus exitStatus) {
            qDebug() << "exitStatus: " << exitStatus;
            qDebug() << transaction->errorString();

        });

        transaction->run();
    }


}

void Utils::removeKernel(QString name)
{

}

void Utils::updateKernel(QString name)
{

}

QApt::Backend * Utils::getAptBackend()
{
    if (backend == 0) {
        backend = new QApt::Backend();


        backend->init();
        backend->openXapianIndex();
    }
    return backend;
}
