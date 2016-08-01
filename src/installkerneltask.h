#ifndef INSTALLKERNELTASK_H
#define INSTALLKERNELTASK_H

#include <QRunnable>
#include <QFuture>
#include <QString>
#include <QFutureInterface>

class InstallKernelTask : public QRunnable
{
public:
    InstallKernelTask(QString name);

    void run() Q_DECL_OVERRIDE;
    QFuture<void> future();
private:
    QString _name;
    QFutureInterface<void> futureInterface;
};

#endif // INSTALLKERNELTASK_H
