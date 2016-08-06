#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QList>
#include <QVariantMap>

#include "grubcfgparser.h"

namespace QApt {
    class Backend;
}

class Utils
{
public:
    static QList<QVariantMap> loadKernelsData();

    static QString getDefult();
    static void setAsDefault(QString name);


    static QApt::Backend *getAptBackend();
private:
    static QApt::Backend * backend;
};

#endif // UTILS_H
