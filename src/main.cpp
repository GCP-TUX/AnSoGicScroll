#include "IconFactory.h"
#include "TrayApplication.h"

#include <QApplication>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QTimer>
#include <cstdlib>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("AnSoGicScroll");
    QApplication::setApplicationVersion("0.1.0");
    QApplication::setOrganizationName("AnSoGicScroll");
    QApplication::setWindowIcon(IconFactory::appIcon());

    // A tray-only application must not quit just because it has no
    // top-level windows open (dialogs are shown on demand).
    QApplication::setQuitOnLastWindowClosed(false);

    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, QObject::tr("AnSoGicScroll"),
                               QObject::tr("No se encontro una bandeja del sistema en este "
                                           "entorno de escritorio. La aplicacion no puede "
                                           "continuar."));
        return 1;
    }

    TrayApplication trayApplication;

    // TEMPORARY manual test hook, removed once the overlay is verified.
    if (std::getenv("ANSOGICSCROLL_TEST_CAPTURE")) {
        QTimer::singleShot(500, &trayApplication, &TrayApplication::debugTriggerScreenshot);
    }

    return QApplication::exec();
}
