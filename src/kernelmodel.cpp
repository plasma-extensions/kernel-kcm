#include "kernelmodel.h"

#include <QProcess>
#include <QFutureWatcher>
#include <QThreadPool>
#include <QtQml>
#include <QDebug>

#include "loadkerneldatatask.h"
#include "installkerneltask.h"
#include "uninstallkerneltask.h"
#include "makedefaulttask.h"

KernelModel::KernelModel(QObject *parent) : QAbstractListModel(parent)
{
    qmlRegisterInterface<Task>("Task");


    LoadKernelDataTask * loadTask =  new LoadKernelDataTask();
    QThreadPool::globalInstance()->start(loadTask);

    connect(&futureEntriesWatcher,SIGNAL(finished()), this, SLOT(handleLoadKernelsDataFinished()));
    futureEntriesWatcher.setFuture(loadTask->future());
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

QFutureWatcher<void> &KernelModel::installKernel(int index)
{
    QVariantMap entry = entries.at(index);

    InstallKernelTask *task = new InstallKernelTask(entry["Name"].toString());
    QThreadPool::globalInstance()->start(task);

    installKernelWatcher.setFuture(task->future());

    return installKernelWatcher;
}

QFutureWatcher<void> &KernelModel::removeKernel(int index)
{
    QVariantMap entry = entries.at(index);

    UninstallKernelTask *task = new UninstallKernelTask(entry["Name"].toString());
    QThreadPool::globalInstance()->start(task);

    removeKernelWatcher.setFuture(task->future());
    return removeKernelWatcher;
}

QFutureWatcher<void> &KernelModel::mekeDefaultKernel(int index)
{

}


QFutureWatcher<void> &KernelModel::updateKernel(int index)
{

}



void KernelModel::handleLoadKernelsDataFinished()
{
    beginResetModel();
    entries = futureEntriesWatcher.result();
    endResetModel();
}
