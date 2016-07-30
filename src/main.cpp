#include <QtCore/QCoreApplication>
#include <QApplication>
#include <QtQuick/QQuickView>
#include <QtQml>
#include <QDebug>

#include "kernelmodel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("KCM");
    QCoreApplication::setOrganizationDomain("org.plasma_light");


    qmlRegisterType<KernelModel>("org.plasma_light.kcm", 1, 0, "Kernels");


    QQuickView quickView;
    // Setup view
    quickView.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    quickView.setResizeMode(QQuickView::SizeRootObjectToView);
    quickView.show();

    return app.exec();
}
