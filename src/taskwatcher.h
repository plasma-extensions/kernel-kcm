#ifndef TASKWATCHER_H
#define TASKWATCHER_H

#include <QObject>
#include <QString>
#include <QFuture>
#include <QFutureWatcher>

class TaskWatcher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(String message READ message)
public:
    TaskWatcher(QFuture task);

    enum Status {
        WORKING = 0,
        FINISHED_SUCCESS,
        FINISHED_ERROR
    }
    Q_ENUM(Status);

    Status status();
    QString message();

signals:
    void statusChanged(Status status);

private:
    QFuture<void> task;
    QFutureWatcher<void> taskWatcher;
};

#endif // TASKWATCHER_H
