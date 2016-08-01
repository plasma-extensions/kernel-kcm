#ifndef LOADKERNELDATATASK_H
#define LOADKERNELDATATASK_H

#include <QRunnable>
#include <QFuture>
#include <QFutureInterface>

class LoadKernelDataTask : public QRunnable
{
public:
    LoadKernelDataTask();
    ~LoadKernelDataTask();

    void run() Q_DECL_OVERRIDE;
    QFuture<QList<QVariantMap>> future();
private:
    QFutureInterface<QList<QVariantMap>> futureInterface;
};

#endif // LOADKERNELDATATASK_H
