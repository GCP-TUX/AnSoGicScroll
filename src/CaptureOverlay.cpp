#include "CaptureOverlay.h"
#include "ScreenGrabber.h"

#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

CaptureOverlay::CaptureOverlay(QWidget *parent)
    : QWidget(parent)
{
    // BypassWindowManagerHint is essential here: regular managed windows
    // (even frameless/Tool ones) get clamped by the window manager to a
    // single monitor's work area on X11, no matter what geometry is
    // requested. Bypassing the WM (override-redirect) lets this window
    // truly span every connected screen, the same trick Flameshot uses for
    // its selection overlay. The trade-off is that the WM will no longer
    // hand us keyboard/mouse focus automatically, so it's grabbed manually
    // after show() (see CaptureOverlay::showOverlay()).
    //
    // NOTE: this only works on X11. Wayland compositors ignore
    // BypassWindowManagerHint entirely — use the other constructor
    // (taking a pre-captured QImage) on Wayland instead.
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::BypassWindowManagerHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setCursor(Qt::CrossCursor);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_background = ScreenGrabber::grabVirtualDesktop();
    m_virtualDesktopRect = ScreenGrabber::virtualDesktopGeometry();

    setGeometry(m_virtualDesktopRect);
    qDebug() << "[CaptureOverlay] virtualDesktopRect=" << m_virtualDesktopRect
             << "backgroundSize=" << m_background.size();
}

CaptureOverlay::CaptureOverlay(const QImage &screenshot, QWidget *parent)
    : QWidget(parent)
    , m_useFullScreen(true)
{
    // Same widget behaviour as the X11 path, minus BypassWindowManagerHint
    // (ignored by Wayland compositors anyway) — mirrors the flags already
    // proven to work for the Wayland scroll-region selector.
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setCursor(Qt::CrossCursor);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_background = QPixmap::fromImage(screenshot);

    // Treat the captured frame as if it WERE the whole virtual desktop,
    // with its own origin at (0,0). Every place below that translates
    // selections by "-m_virtualDesktopRect.topLeft()" then becomes a
    // no-op, so the rest of the class needs no Wayland-specific branches.
    m_virtualDesktopRect = QRect(QPoint(0, 0), screenshot.size());

    setGeometry(m_virtualDesktopRect);
    qDebug() << "[CaptureOverlay] (Wayland) screenshotSize=" << screenshot.size();
}

void CaptureOverlay::showOverlay()
{
    if (m_useFullScreen) {
        // Wayland: showFullScreen() is honoured by compositors (it's an
        // explicit protocol request, xdg_toplevel::set_fullscreen), unlike
        // manual positioning/BypassWindowManagerHint which are ignored.
        showFullScreen();
    } else {
        show();
    }
    raise();
    activateWindow();
    qDebug() << "[CaptureOverlay] after show() geometry=" << geometry()
             << "isVisible=" << isVisible();
    setFocus(Qt::ActiveWindowFocusReason);
}

QRect CaptureOverlay::selectionInLocalCoords() const
{
    return QRect(m_dragStart, m_dragCurrent).normalized();
}

void CaptureOverlay::paintEvent(QPaintEvent * /*event*/)
{
    QPainter painter(this);

    if (m_dragging) {
        const QRect sel = selectionInLocalCoords();

        // Crisp full background, dim only OUTSIDE the active selection —
        // matches the UX of Flameshot/ShareX and lets the user still see
        // context around the area they're selecting.
        painter.drawPixmap(rect(), m_background);

        QPainterPath full;
        full.addRect(rect());
        QPainterPath hole;
        hole.addRect(sel);
        painter.fillPath(full.subtracted(hole), QColor(0, 0, 0, 100));

        painter.setPen(QPen(QColor("#4FC3F7"), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(sel.adjusted(0, 0, -1, -1));

        const QString sizeText = QString("%1 x %2").arg(sel.width()).arg(sel.height());
        const QPoint labelPos = sel.topLeft() + QPoint(4, -6);
        QFontMetrics fm(painter.font());
        QRect textRect = fm.boundingRect(sizeText).adjusted(-4, -2, 4, 2);
        textRect.moveBottomLeft(labelPos.y() > textRect.height() ? labelPos
                                                                 : sel.topLeft() + QPoint(4, 16));
        painter.fillRect(textRect, QColor(0, 0, 0, 180));
        painter.setPen(Qt::white);
        painter.drawText(textRect, Qt::AlignCenter, sizeText);
    } else {
        // Before dragging starts: show the screenshot crisp and clear, with
        // just a very light dim so the hint text stays readable — no blur,
        // so the user can actually see what they're about to select.
        painter.drawPixmap(rect(), m_background);
        painter.fillRect(rect(), QColor(0, 0, 0, 25));

        painter.setPen(Qt::white);
        const QString hint = tr("Arrastra para seleccionar una region en cualquier pantalla. Esc para cancelar.");
        QFontMetrics fm(painter.font());
        QRect textRect = fm.boundingRect(hint).adjusted(-8, -4, 8, 4);
        textRect.moveCenter(QPoint(width() / 2, 40));
        painter.fillRect(textRect, QColor(0, 0, 0, 160));
        painter.drawText(textRect, Qt::AlignCenter, hint);
    }
}

void CaptureOverlay::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragStart = event->pos();
        m_dragCurrent = event->pos();
        update();
    }
}

void CaptureOverlay::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging) {
        m_dragCurrent = event->pos();
        update();
    }
}

void CaptureOverlay::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !m_dragging) {
        return;
    }
    m_dragging = false;

    const QRect sel = selectionInLocalCoords();
    if (sel.width() < 4 || sel.height() < 4) {
        close();
        emit cancelled();
        return;
    }

    const QRect globalRect = sel.translated(m_virtualDesktopRect.topLeft());
    // Close (and hide) the overlay before notifying listeners: it bypasses
    // the window manager, so any dialog a slot opens (e.g. a save dialog)
    // would otherwise stay stacked behind this always-on-top window.
    close();
    emit regionSelected(globalRect);
}

void CaptureOverlay::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
        emit cancelled();
        return;
    }
    QWidget::keyPressEvent(event);
}

void CaptureOverlay::closeEvent(QCloseEvent *event)
{
    releaseMouse();
    releaseKeyboard();
    QWidget::closeEvent(event);
}
