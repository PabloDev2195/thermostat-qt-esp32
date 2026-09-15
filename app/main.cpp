#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "blemanager/blemanager.h"

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    BleManager bleManager;

    engine.rootContext()->setContextProperty("bleManager", &bleManager);

    bleManager.startScan();

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("Thermostat", "Main");

    if (engine.rootObjects().isEmpty())
        return -1;

    return QGuiApplication::exec();
}