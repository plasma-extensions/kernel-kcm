#include "kernelmodel.h"

#include <QProcess>

#include <QDebug>

KernelModel::KernelModel(QObject *parent) : QAbstractListModel(parent)
{

    // TODO: Load kernels info from the system instead from this fixed list
    QVariantMap entry;
    entry["Name"] = "Linux Image 4.12.2-generic";
    entry["IsActive"] = true;
    entry["IsDefault"] = true;
    entry["IsLTS"] = true;
    entry["IsInstalled"] = true;
    entry["IsUpgradeable"] = true;

    entries.append(entry);
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
    roles.insert(IsUpgradeable, "isUpgradeable");

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
    case IsUpgradeable:
        return entry["IsUpgradeable"];
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


void KernelModel::loadKernelsData() {
    QProcess gzip;
    gzip.start("gzip", QStringList() << "-c");
    if (!gzip.waitForStarted())
        return;

    gzip.write("Qt rocks!");
    gzip.closeWriteChannel();

    if (!gzip.waitForFinished())
        return;

    QByteArray result = gzip.readAll();

    qDebug() << result;
}

bool KernelModel::installKernel(QString id) {

}


bool KernelModel::removeKernel(QString id) {

}

bool KernelModel::setDefaultKernel(QString id) {

}
