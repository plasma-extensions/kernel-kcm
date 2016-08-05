#include "taskwatcher.h"

TaskWatcher::TaskWatcher(QFuture task) : QObject(parent)
{
    this->task = task;
    taskWatcher.setFuture(this->task);
}

TaskWatcher::Status TaskWatcher::status()
{

}

QString TaskWatcher::message()
{

}

