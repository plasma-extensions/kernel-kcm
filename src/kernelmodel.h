#ifndef KERNELMODEL_H
#define KERNELMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QList>
#include <QVariantMap>
#include <QFuture>
#include <QFutureWatcher>

typedef QFutureWatcher<void> * Task;

class KernelModel : public QAbstractListModel
{
    Q_OBJECT
public:
    /**
     * @enum Roles of the model, they will be accessible from delegates
     */
    enum Roles {
        Name = Qt::UserRole + 1,
        IsInstalled,
        IsLTS,
        IsActive,
        IsDefault,
        IsUpgradable
    };

    KernelModel(QObject *parent = 0);

    QHash< int, QByteArray > roleNames() const Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex&) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

signals:

public slots:

    QFutureWatcher<void> &installKernel(int index);
    QFutureWatcher<void> &removeKernel(int index);
    QFutureWatcher<void> &mekeDefaultKernel(int index);
    QFutureWatcher<void> &updateKernel(int index);

private slots:
    void handleLoadKernelsDataFinished();


private:
    QList<QVariantMap> originalEntries;
    QList<QVariantMap> entries;

    QFutureWatcher<QList<QVariantMap>> futureEntriesWatcher;
    QFutureWatcher<void> removeKernelWatcher;
    QFutureWatcher<void> installKernelWatcher;
    QFutureWatcher<void> updateKernelWatcher;
    QFutureWatcher<void> makeDefaultWatcher;

};

#endif // KERNELMODEL_H
