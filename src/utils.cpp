#include "utils.h"

#include <QString>
#include <QFile>
#include <QTemporaryFile>
#include <QTextStream>
#include <QLocale>
#include <QProcess>

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

    // Package name associed with the default grub entry
    QString defaultKernel = getDefult();

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
        newEntry["IsDefault"] = defaultKernel.compare(pkg->name()) == 0;

        // qDebug() << newEntry;
        newEntries.append(newEntry);
    }

    return newEntries;
}

QString Utils::getDefult()
{
    // Get the DEFAULT value
    QString grubDefault;

    QFile defaultGrub("/etc/default/grub");
    if (defaultGrub.open(QFile::ReadOnly)) {
        QTextStream in(&defaultGrub);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("GRUB_DEFAULT=")) {
                QStringList parts = line.split("=");
                if (parts.size() >= 2)
                    grubDefault = parts.at(1);
                break;
            }
        }
    }

    // qDebug() << "GRUB_DEFAULT=" << grubDefault;
    Grub::GrubCFGParser parser;
    Grub::SubMenu entries = parser.getMenuEntries();

    bool isNumber;
    int grubDefaultIdx = grubDefault.toInt(&isNumber);

    Grub::MenuEntry defaultEntry;
    if (isNumber) {
        if (entries.size() > grubDefaultIdx)
            defaultEntry = entries.at(grubDefaultIdx);
    } else {
        // Remove quotes
        grubDefault = grubDefault.section('\'',1,1);

        for (Grub::MenuEntry entry : entries) {
            if (entry["id"].compare(grubDefault) == 0) {
                defaultEntry = entry;
                break;
            }
            if (entry["title"].compare(grubDefault) == 0) {
                defaultEntry = entry;
                break;
            }
        }
    }
    // qDebug() << "DEFAULT ENTRY" << defaultEntry;

    if (!defaultEntry.isEmpty() && defaultEntry.contains("kernel")) {
        QApt::Backend *apt = getAptBackend();
        QApt::Package *package = apt->packageForFile(defaultEntry["kernel"]);

        if (package != NULL)
            return package->name();
    }

    return "";
}

void Utils::setAsDefault(QString name)
{
    QString grubEntryId;
    QApt::Backend *apt = getAptBackend();

    Grub::GrubCFGParser parser;
    Grub::SubMenu entries = parser.getMenuEntries();

    // Find entry that contains the kernel installed by the package to be set as default
    for (Grub::MenuEntry entry: entries) {
        QApt::Package *package = apt->packageForFile(entry["kernel"]);
        if (package != NULL) {
            if (name.compare(package->name()) == 0) {
                grubEntryId = entry["id"];
                break;
            }

        }
    }

    if (grubEntryId.isEmpty()) {
        qDebug() << "Unable to find a grub entry that contains the kernel "
                    "installed by the package to be set as default";
        return;
    }
    qDebug() << "Entry ID " << grubEntryId;

    // WRITE /etc/grub/default

    QFile defaultGrub("/etc/default/grub");
    QTemporaryFile newDefaultGrub;
    if (defaultGrub.open(QFile::ReadOnly) && newDefaultGrub.open()) {
        QTextStream in(&defaultGrub);
        QTextStream out(&newDefaultGrub);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("GRUB_DEFAULT=")) {
                line = "GRUB_DEFAULT='" + grubEntryId + "'";
            }

            out << line;
            endl(out);
        }
    }

    QProcess process;
    process.start("kdesudo", QStringList() << "-c" << "mv " + newDefaultGrub.fileName() +" /etc/default/grub"
                  << "--comment" << "Allow to write the new bootloader configuration.");
    process.waitForFinished();

    process.start("kdesudo", QStringList() << "-c" << "update-grub"
                  << "--comment" << "Update bootloader.");
    process.waitForFinished();
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
