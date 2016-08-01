#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QList>
#include <QVariantMap>
#include <QFuture>
#include <QFutureWatcher>

class Utils
{
public:
    Utils();
    QFuture<QList<QVariantMap>> loadKernelsData();

    QFuture<void> makeDefault(QString name);
    QFuture<void> installKernel(QString name);
    QFuture<void> removeKernel(QString name);

private:
    QString getActiveKernel();
    QString getDefaultKernel();

    bool isActive(QString name);
    bool isLts(QString name);
    bool isDefault(QString name);
    bool isInstalled(QString name);

    class LoadKernelsDataTask: QRunnable {
        void run();
        QFuture<QList<QVariantMap>> future;
    };
};

#endif // UTILS_H
