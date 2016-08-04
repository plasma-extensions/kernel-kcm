#ifndef UPDATEKERNELTASK_H
#define UPDATEKERNELTASK_H

#include <QFutureInterface>
#include <QRunnable>
#include <QFuture>
#include <QString>

class UpdateKernelTask : public QRunnable
{
public:
    UpdateKernelTask(QString name);

    void run() Q_DECL_OVERRIDE;
    QFuture<void> future();
private:
    QString _name;
    QFutureInterface<void> futureInterface;
};

#endif // UPDATEKERNELTASK_H
