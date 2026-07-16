#include <QtGlobal>

#if defined(Q_OS_WIN)
// Debe definirse ANTES de incluir <windows.h> para que declare
// SetProcessDpiAwarenessContext (disponible desde Windows 10 1703+).
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <windows.h>
#endif

#include "IconFactory.h"
#include "TrayApplication.h"
#include <QApplication>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QTimer>
#include <cstdlib>

int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    // Declara el proceso como "Per-Monitor DPI Aware V2" ANTES de crear
    // QApplication. Sin esto, Windows trata el proceso como no consciente
    // de DPI y "virtualiza" las coordenadas que devuelven las llamadas
    // nativas Win32 (GetCursorPos, WindowFromPoint, GetWindowRect,
    // DwmGetWindowAttribute en windowhighlightoverlay.cpp), asumiendo
    // 96 DPI (100%). Qt, por su parte, sí escala su propio renderizado
    // según el DPI real del monitor. El resultado es un desajuste entre
    // ambos sistemas de coordenadas, visible como el recuadro de
    // resaltado de ventana desplazado/mal alineado en pantallas con
    // escala distinta a 100% (125%, 150%, etc.) — que es exactamente lo
    // que no ocurre en una VM corriendo a 100% pero sí en una laptop real
    // con escalado activo.
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
#endif

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
