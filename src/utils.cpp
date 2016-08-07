#include "utils.h"

#include <QString>
#include <QFile>
#include <QTemporaryFile>
#include <QTextStream>
#include <QLocale>
#include <QProcess>
#include <QCoreApplication>
#include <QThread>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QMutex>

#include <QRegExp>


#include <QDebug>


#include <QApt/Backend>
#include <QApt/Package>
#include <QApt/Transaction>

QApt::Backend* Utils::backend = NULL;

using namespace QApt;

QList<QVariantMap> Utils::loadKernelsData()
{
    // qDebug() << "loadKernelsData";

    // Get kernels support info
    QMap<QString, QString> supportInfo = getSupportInfo();

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

        QStringList parts = QString(pkg->name()).split("-");
        QString kernelVersion;
        if (parts.size() > 3)
            kernelVersion = parts[2];
        // qDebug() << "Kernel Version" << kernelVersion;

        newEntry["Version"] = kernelVersion;
        newEntry["IsInstalled"] = QApt::Package::Installed & pkg->state();
        newEntry["IsActive"] = QString(pkg->name()).contains(QSysInfo::kernelVersion());
        newEntry["IsUpgradable"] = QApt::Package::Upgradeable & pkg->state();
        newEntry["IsDefault"] = defaultKernel.compare(pkg->name()) == 0;



        newEntry["Support"] = supportInfo[kernelVersion];

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

QMap<QString, QString> Utils::getSupportInfo()
{
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("https://www.kernel.org/"));
    QNetworkReply *reply = manager.get(request);

    // Whait for netwtork results
    // TODO: Improve this dirty hack
    while (!reply->isFinished()) {
        QCoreApplication::processEvents();
        usleep(200);
    }

    QMap<QString, QString> info;

    QString result = reply->readAll();
    // qDebug() << result;
    QRegExp rx("(?:<td>)(mainline|stable|longterm|linux-next)(?::</td>)\\s*(<td><strong>)((?:\\d+|\\.)+)+(</strong></td>)");

    int pos = 0;

    while ((pos = rx.indexIn(result, pos)) != -1) {
        // qDebug() << rx.capturedTexts();
        QString version = rx.cap(3);
        QString support = rx.cap(1);
        info[version] = support;

        pos += rx.matchedLength();
    }

    // qDebug() << info;
    return info;
}
