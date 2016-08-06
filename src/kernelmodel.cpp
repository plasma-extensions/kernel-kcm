#include "kernelmodel.h"

#include <QProcess>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>

#include <QDebug>

#include <QApt/Backend>
#include <QApt/Transaction>
#include <QApt/Package>


#include "utils.h"


KernelModel::KernelModel(QObject *parent) : QAbstractListModel(parent)
{
}


QHash<int, QByteArray> KernelModel::roleNames() const
{
    QHash<int, QByteArray> roles = QAbstractItemModel::roleNames();
    roles.insert(Qt::DisplayRole, "display");
    roles.insert(Qt::DecorationRole, "decoration");
    roles.insert(Name, "name");
    roles.insert(IsActive, "isActive");
    roles.insert(IsDefault, "isDefault");
    roles.insert(IsInstalled, "isInstalled");
    roles.insert(IsLTS, "isLts");
    roles.insert(IsUpgradable, "isUpgradable");

    return roles;
}

int KernelModel::rowCount(const QModelIndex& index) const {
    return index.isValid() ? 0 : entries.size();
}


QVariant KernelModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.parent().isValid() ||
        index.column() > 0 || index.row() < 0 || index.row() >= entries.size()) {
        // index requested must be valid, but we have no child items!
        return QVariant();
    }

    QVariantMap entry = entries.at(index.row());

    switch(role) {
    case Qt::DisplayRole:
    case Name:
        return entry["Name"];
    case IsActive:
        return entry["IsActive"];
    case IsDefault:
        return entry["IsDefault"];
    case IsInstalled:
        return entry["IsInstalled"];
    case IsLTS:
        return entry["IsLTS"];
    case IsUpgradable:
        return entry["IsUpgradable"];
    }
    return QVariant();
}

bool KernelModel::setData(const QModelIndex & index, const QVariant & value, int role) {
    if (!index.isValid() || index.parent().isValid() ||
        index.column() > 0 || index.row() < 0 || index.row() >= entries.size()) {
        // index requested must be valid, but we have no child items!
        return false;
    }
     qDebug() << "Setting data " << role << value;

    QVariantMap entry = entries.at(index.row());
    entries.removeAt(index.row());

    switch(role) {
        case IsDefault:
            entry["IsDefault"] = value;
            break;
        case IsInstalled:
            entry["IsInstalled"] = value;
            break;
    }

    entries.insert(index.row(), entry);

    emit dataChanged(index, index);
}

void KernelModel::fetchData()
{
    emit(jobStarted("Loading system data"));

    QFuture<QList<QVariantMap>> futureResult = QtConcurrent::run(Utils::loadKernelsData);
    QFutureWatcher<QList<QVariantMap>> *watcher = new QFutureWatcher<QList<QVariantMap>>();
    watcher->setFuture(futureResult);

    connect(watcher, &QFutureWatcher<QList<QVariantMap>>::finished, [=] () {
        beginResetModel();
        entries = futureResult.result();
        endResetModel();

        emit(jobFinished(true, ""));

        watcher->deleteLater();
    });
}

void KernelModel::install(int index)
{
    QVariantMap entry = entries.at(index);
    emit(jobStarted("Installing " + entry["Name"].toString()));

    QApt::Package *package  = Utils::getAptBackend()->package(entry["Name"].toString());
    if (!package == 0) {
        QApt::Transaction *transaction = Utils::getAptBackend()->installPackages(QApt::PackageList() << package);

        transaction->replyUntrustedPrompt(true);
        transaction->setLocale(QLatin1String(setlocale(LC_MESSAGES, 0)));

        QObject::connect(transaction, &QApt::Transaction::promptUntrusted, [=] (QStringList untrustedPackages) {
            qDebug() << "untrustedPackages " << untrustedPackages;
            transaction->replyUntrustedPrompt(true);
        });

        QObject::connect(transaction, &QApt::Transaction::progressChanged, [=] (int progress) {
            qDebug() << "progress" << progress << " " << transaction->statusDetails();
            emit(jobUpdated(progress, transaction->statusDetails()));
        });

        QObject::connect(transaction, &QApt::Transaction::finished, [=] (QApt::ExitStatus exitStatus) {
            qDebug() << "exitStatus: " << exitStatus;
            qDebug() << transaction->errorString();

            if (exitStatus == QApt::ExitStatus::ExitSuccess) {
                emit(jobFinished(true, ""));
                emit(systemDataChanged());
            } else
                emit(jobFinished(false, transaction->errorString()));

        });

        transaction->run();
    } else {
        emit(jobFinished(false, "Unable to resolve package name."));
    }
}

void KernelModel::remove(int index)
{
    QVariantMap entry = entries.at(index);
    emit(jobStarted("Removing " + entry["Name"].toString()));

    QApt::Package *package  = Utils::getAptBackend()->package(entry["Name"].toString());
    if (!package == 0) {
        QApt::Transaction *transaction = Utils::getAptBackend()->removePackages(QApt::PackageList() << package);

        transaction->setLocale(QLatin1String(setlocale(LC_MESSAGES, 0)));

        QObject::connect(transaction, &QApt::Transaction::promptUntrusted, [=] (QStringList untrustedPackages) {
            qDebug() << "untrustedPackages " << untrustedPackages;
            transaction->replyUntrustedPrompt(true);
        });

        QObject::connect(transaction, &QApt::Transaction::progressChanged, [=] (int progress) {
            qDebug() << "progress" << progress << " " << transaction->statusDetails();
            emit(jobUpdated(progress, transaction->statusDetails()));
        });

        QObject::connect(transaction, &QApt::Transaction::finished, [=] (QApt::ExitStatus exitStatus) {
            qDebug() << "exitStatus: " << exitStatus;
            qDebug() << transaction->errorString();

            if (exitStatus == QApt::ExitStatus::ExitSuccess) {
                emit(jobFinished(true, ""));
                emit(systemDataChanged());
            } else
                emit(jobFinished(false, transaction->errorString()));

        });

        transaction->run();
    } else {
        emit(jobFinished(false, "Unable to resolve package name."));
    }
}

void KernelModel::setAsDefault(int index)
{
    QVariantMap entry = entries.at(index);
    emit(jobStarted("Setting " + entry["Name"].toString() + " as default kernel." ));

    QFuture<void> futureResult = QtConcurrent::run(Utils::setAsDefault, entry["Name"].toString());
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
    watcher->setFuture(futureResult);

    connect(watcher, &QFutureWatcher<void>::finished, [=] () {
        emit(jobFinished(true, ""));
        emit(systemDataChanged());
        watcher->deleteLater();
    });
}


void KernelModel::update(int index)
{
    // Call install on the installed package will update it if possible
    install(index);
}

QString KernelModel::getChangeLogUrl(int index)
{
    QVariantMap entry = entries.at(index);
    QApt::Package *package  = Utils::getAptBackend()->package(entry["Name"].toString());
    if (package != NULL)
        return package->changelogUrl().toString();
    else
        return "";
}
