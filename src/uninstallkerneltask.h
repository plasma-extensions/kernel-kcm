#ifndef UNINSTALLKERNELTASK_H
#define UNINSTALLKERNELTASK_H

#include <QRunnable>
#include <QFuture>
#include <QFutureInterface>
#include <QString>

class UninstallKernelTask : public QRunnable
{
public:
    UninstallKernelTask(QString name);

    void run() Q_DECL_OVERRIDE;
    QFuture<void> future();
private:
    QString _name;
    QFutureInterface<void> futureInterface;
};

#endif // UNINSTALLKERNELTASK_H
