#ifndef KERNELMODEL_H
#define KERNELMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QList>
#include <QVariantMap>

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
        IsUpgradeable
    };

    KernelModel(QObject *parent = 0);

    QHash< int, QByteArray > roleNames() const Q_DECL_OVERRIDE;

    int rowCount(const QModelIndex&) const Q_DECL_OVERRIDE;
    QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

signals:

public slots:
    void loadKernelsData();
    bool installKernel(QString id);
    bool removeKernel(QString id);
    bool setDefaultKernel(QString id);

private:
    QList<QVariantMap> entries;
};

#endif // KERNELMODEL_H
