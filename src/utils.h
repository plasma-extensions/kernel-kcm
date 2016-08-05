#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QList>
#include <QVariantMap>

namespace QApt {
    class Backend;
}

class Utils
{
public:
    static QList<QVariantMap> loadKernelsData();

    static void setAsDefault(QString name);
    static void installKernel(QString name);
    static void removeKernel(QString name);
    static void updateKernel(QString name);

    static QApt::Backend *getAptBackend();
private:
    static QApt::Backend * backend;
};

#endif // UTILS_H
