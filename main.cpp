#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQuick/QQuickView>
#include <QDebug>

#include "controller.h"

int main(int argc, char *argv[])
{
    // OpenGL backend is required to make AbstractSeries.useOpenGL work.
    // We don't force it programmatically, as OpenGL is not guaranteed to be available everywhere.
    bool openGLSupported = QQuickWindow::graphicsApi() == QSGRendererInterface::OpenGLRhi;
    if (!openGLSupported) {
        qWarning() << "OpenGL is not set as the graphics backend, so AbstractSeries.useOpenGL will not work.";
        qWarning() << "Set QSG_RHI_BACKEND=opengl environment variable to force the OpenGL backend to be used.";
    }

    qmlRegisterType<Controller>("fr.ecgviewer.qmlcomponents", 1, 0, "Controller");

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/ecgViewer/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
