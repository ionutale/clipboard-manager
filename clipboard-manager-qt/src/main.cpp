#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include "database/Database.h"
#include "ui/TrayManager.h"
#include "models/ClipboardListModel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Clipboard Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("clipboard-manager");

    QIcon appIcon = QIcon(":/app_icon_64.png");
    if (appIcon.isNull())
        appIcon = QIcon::fromTheme("edit-paste");
    app.setWindowIcon(appIcon);

    Database db;
    ClipboardListModel clipboardModel(&db);
    TrayManager tray(&db);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("clipboardModel", &clipboardModel);

    engine.addImportPath("qrc:/");

    engine.load(QUrl("qrc:/ClipboardManager/Popup.qml"));

    if (engine.rootObjects().isEmpty())
        return -1;

    QObject *root = engine.rootObjects().first();

    QObject::connect(&tray, &TrayManager::toggleRequested, [root]() {
        QMetaObject::invokeMethod(root, "toggle");
    });

    QObject::connect(&clipboardModel, &ClipboardListModel::countChanged, [root]() {
        if (!root->property("visible").toBool())
            QMetaObject::invokeMethod(root, "showPopup");
    });

    return app.exec();
}
