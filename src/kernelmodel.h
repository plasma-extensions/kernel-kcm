#ifndef KERNELMODEL_H
#define KERNELMODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QList>
#include <QVariantMap>

#include "utils.h"

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
    void jobStarted(QString message);
    void jobUpdated(int progress, QString message);
    void jobFinished(bool succeed, QString message);

public slots:
    void fetchData();
    void install(int index);
    void remove(int index);
    void setAsDefault(int index);
    void update(int index);
    QString getChangeLogUrl(int index);


private:
    QList<QVariantMap> entries;
    Utils utils;
};

#endif // KERNELMODEL_H
