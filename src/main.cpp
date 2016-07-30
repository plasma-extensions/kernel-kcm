#include <QtCore/QCoreApplication>
#include <QApplication>
#include <QtQuick/QQuickView>
#include <QtQml>
#include <QDebug>

// #include "kernelsmodel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName("KCM");
    QCoreApplication::setOrganizationDomain("com.plasma_light.kcm");


    // qmlRegisterType<KernelModel>("com.plasma_light.ice_utility", 1, 0, "Kernels");

    QQuickView quickView;
    // Setup view
    quickView.setSource(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    quickView.setResizeMode(QQuickView::SizeRootObjectToView);
    quickView.show();

    return app.exec();
}
