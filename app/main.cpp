#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "blemanager/blemanager.h"

int main(int argc, char *argv[])
{
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

    QGuiApplication app(argc, argv);

    qmlRegisterType<BleManager>("Thermostat", 1, 0, "BleManager");

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("Thermostat", "Main");

    return QGuiApplication::exec();
}
