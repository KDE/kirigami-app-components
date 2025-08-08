#include <QGuiApplication>
#include <QQmlApplicationEngine>
//#include "actioncollection.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
                     &app, []() { QCoreApplication::exit(-1); },
    Qt::QueuedConnection);
    //qmlRegisterUncreatableType<ActionCollectionData>("actioncollection", 1, 0, "ActionCollectionData", "Error class uncreatable");
    engine.loadFromModule("actioncollection", "Main");

    return app.exec();
}
