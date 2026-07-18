// ================= windowhighlightoverlay.cpp =================
#include "windowhighlightoverlay.h"
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>

#if defined( Q_OS_WIN )
#include <dwmapi.h>
#include <shellscalingapi.h>
#endif

#if defined( Q_OS_WIN )
#include <qwindowdefs_win.h>
#include <dwmapi.h>
#include <shellscalingapi.h>
#endif

static constexpr QColor BORDER_COLOR(136, 0, 170, 255);
static constexpr int    BORDER_WIDTH = 3;
#if defined( Q_OS_WIN )
WindowHighlightOverlay* WindowHighlightOverlay::s_instance_ = nullptr;

WindowHighlightOverlay::WindowHighlightOverlay(QWidget *parent)
    : QWidget(parent)
{
    //setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute( Qt::WA_NoSystemBackground );
    setAttribute(Qt::WA_TransparentForMouseEvents );
    setAttribute( Qt::WA_TranslucentBackground );
    setAttribute(Qt::WA_ShowWithoutActivating);

    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::Tool);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    // setOverlayWindowMouseBlocking( this );
    //blockMouseEventsOn( this );
}

WindowHighlightOverlay::~WindowHighlightOverlay()
{
    stopTracking();
}

void WindowHighlightOverlay::initVirtualDesktop()
{
    QRect geo;
    for (QScreen* s : QGuiApplication::screens())
        geo |= s->geometry();
    setGeometry(geo);
}

void WindowHighlightOverlay::startTracking(int intervalMs)
{
    if (!isVisible()) show();
    raise();
    installMouseHook();
    connect(&timer_, &QTimer::timeout, this, &WindowHighlightOverlay::updateTarget, Qt::UniqueConnection);
    timer_.start(intervalMs);
}

void WindowHighlightOverlay::stopTracking()
{
    timer_.stop();
    uninstallMouseHook();
    hide();
}

void WindowHighlightOverlay::installMouseHook()
{
    if (!mouseHook_) {
        s_instance_ = this;
        mouseHook_ = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);
    }
}

void WindowHighlightOverlay::uninstallMouseHook()
{
    if (mouseHook_) {
        UnhookWindowsHookEx(mouseHook_);
        mouseHook_ = nullptr;
        s_instance_ = nullptr;
    }
}

LRESULT CALLBACK WindowHighlightOverlay::LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION && wParam == WM_LBUTTONDOWN && s_instance_) {
        const MSLLHOOKSTRUCT* info = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
        HWND hwnd = WindowFromPoint(info->pt);
        if (hwnd && hwnd != reinterpret_cast<HWND>(s_instance_->winId())) {
            s_instance_->selectedHwnd_ = hwnd;
            emit s_instance_->panelSelected(hwnd);
            QMetaObject::invokeMethod(s_instance_, "stopTracking", Qt::QueuedConnection);
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}


void WindowHighlightOverlay::updateTarget()
{
    QRect r = getWindowUnderCursor();
    if (r != targetRect_) {
        targetRect_ = r;
        update();
    }
}

/*QRect WindowHighlightOverlay::getWindowUnderCursor() const
{
    POINT pt;
    GetCursorPos(&pt);
    HWND hwnd = WindowFromPoint(pt);
    if (!hwnd || hwnd == reinterpret_cast<HWND>(winId())) return {};
    RECT rc;
    if (!GetWindowRect(hwnd, &rc)) return {};
    return QRect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
}*/

QRect WindowHighlightOverlay::getWindowUnderCursor() const
{
    POINT pt;
    GetCursorPos(&pt);

    HWND hwnd = WindowFromPoint(pt);
    if (!hwnd || hwnd == reinterpret_cast<HWND>(winId())) {
        return {};
    }

    // --- Rectángulo de la ventana completa (con borde/barra de título) ---
    RECT rc{};
    HRESULT hr = DwmGetWindowAttribute(
        hwnd,
        DWMWA_EXTENDED_FRAME_BOUNDS,
        &rc,
        sizeof(rc)
        );

    if (FAILED(hr)) {
        if (!GetWindowRect(hwnd, &rc)) {
            return {};
        }
    }

    // --- Rectángulo del área de contenido (sin barra de título/bordes) ---
    // GetClientRect() devuelve coordenadas relativas a la propia ventana
    // (siempre empieza en 0,0); ClientToScreen() las convierte a
    // coordenadas absolutas de pantalla, igual que hace ShareX para
    // distinguir "toda la ventana" del "área de contenido" (por ejemplo,
    // el panel negro de una consola vs. su barra de título gris).
    RECT clientRc{};
    bool hasClientRect = false;
    if (GetClientRect(hwnd, &clientRc)) {
        POINT clientOrigin{0, 0};
        if (ClientToScreen(hwnd, &clientOrigin)) {
            clientRc.left   += clientOrigin.x;
            clientRc.top    += clientOrigin.y;
            clientRc.right  += clientOrigin.x;
            clientRc.bottom += clientOrigin.y;
            hasClientRect = (clientRc.right > clientRc.left && clientRc.bottom > clientRc.top);
        }
    }

    // Si el cursor está dentro del área de contenido, resalta solo esa
    // área (como ShareX); si está fuera de ella pero dentro de la ventana
    // (barra de título, bordes), resalta la ventana completa.
    const bool cursorInClientArea = hasClientRect &&
                                    pt.x >= clientRc.left && pt.x < clientRc.right &&
                                    pt.y >= clientRc.top  && pt.y < clientRc.bottom;

    RECT chosenRc = (cursorInClientArea) ? clientRc : rc;

    QRect rect(chosenRc.left, chosenRc.top,
               chosenRc.right - chosenRc.left, chosenRc.bottom - chosenRc.top);

    // Optional fine adjustment to avoid clipping borders
    rect = rect.adjusted(1, 1, -1, -1);

    // --- DPI fix -----------------------------------------------------
    // GetWindowRect/DwmGetWindowAttribute return PHYSICAL pixel
    // coordinates (now that the process is declared per-monitor DPI
    // aware in main.cpp). Qt widgets, however, are always positioned
    // and sized in LOGICAL pixels (QScreen::geometry(), QWidget's own
    // geometry() used elsewhere to build this overlay). On a display
    // at 100% scaling both spaces coincide, which is why this worked
    // fine in a VM; on a real laptop with 125%/150% scaling they
    // diverge, causing the highlight rectangle to appear offset/wrong
    // sized. Convert physical -> logical here so downstream code
    // (paintEvent, which mixes this rect with this widget's own
    // logical-pixel geometry) works in a single, consistent space.
    //
    // Deliberately use the DPI of the *monitor* (GetDpiForMonitor) that
    // the target window is on, not GetDpiForWindow(hwnd). The latter
    // depends on whether the *target* application itself declares
    // per-monitor DPI awareness — for third-party apps that don't
    // (common), Windows can report a virtualized DPI for that window
    // that doesn't match the real monitor scale, producing a
    // wrong-sized (not just offset) highlight rectangle. The monitor's
    // DPI is always the true physical value, independent of how the
    // target app handles DPI itself.
    HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    UINT dpiX = 96;
    UINT dpiY = 96;
    if (!hMonitor || FAILED(GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        dpiX = 96;
    }
    const qreal scale = static_cast<qreal>(dpiX) / 96.0;

    if (!qFuzzyCompare(scale, 1.0)) {
        rect = QRect(
            qRound(rect.x() / scale),
            qRound(rect.y() / scale),
            qRound(rect.width() / scale),
            qRound(rect.height() / scale));
    }

    return rect;
}
#endif

void WindowHighlightOverlay::paintEvent(QPaintEvent *)
{
    if (!targetRect_.isValid()) return;

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // Nearly transparent color that still blocks mouse events (Qt::transparent does not)
    QColor maskBlocker(0, 0, 0, 1);
    p.fillRect(this->rect(), maskBlocker);

    // Draw border
    p.setPen(QPen(BORDER_COLOR, BORDER_WIDTH));
    const QPoint offset = -geometry().topLeft();
    QRect local = targetRect_.translated(offset);
    p.drawRect(local.adjusted(BORDER_WIDTH / 2, BORDER_WIDTH / 2,
                              -BORDER_WIDTH / 2, -BORDER_WIDTH / 2));
}

#if defined( Q_OS_WIN )
void WindowHighlightOverlay::setOverlayWindowMouseBlocking(QWidget* widget) {
    HWND hwnd = reinterpret_cast<HWND>(widget->winId());

    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

    exStyle &= ~WS_EX_TRANSPARENT;                 // Block click-through
    exStyle |= WS_EX_LAYERED | WS_EX_TOOLWINDOW;   // Floating layer, no taskbar icon

    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);

    // Optional: keep the window always on top
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
}

void WindowHighlightOverlay::blockMouseEventsOn(QWidget* widget) {
    HWND hwnd = reinterpret_cast<HWND>(widget->winId());

    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

    exStyle &= ~WS_EX_TRANSPARENT;                 // Block click-through
    exStyle |= WS_EX_LAYERED | WS_EX_TOOLWINDOW;   // No taskbar icon, can be made transparent

    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);

    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
}
#endif
