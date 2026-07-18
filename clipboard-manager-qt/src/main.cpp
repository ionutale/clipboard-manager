#include <QApplication>
#include <QIcon>
#include "ui/PopupWidget.h"

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

    PopupWidget popup;

    return app.exec();
}
