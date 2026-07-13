#include "CaptureOverlay.h"
#include "ScreenGrabber.h"

#include <QApplication>
#include <QGraphicsBlurEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QDebug>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>

namespace
{
// Blurs a pixmap by downscaling, applying QGraphicsBlurEffect on the much
// smaller image (fast even for large multi-monitor desktops), and scaling
// back up. The upscale softens edges further, which reads as a pleasant
// "out of focus" background rather than a sharp mosaic.
QPixmap blurredCopy(const QPixmap &source)
{
    if (source.isNull()) {
        return source;
    }

    const QSize smallSize = (source.size() / 6).expandedTo(QSize(1, 1));
    const QPixmap small = source.scaled(smallSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QGraphicsScene scene;
    auto *item = scene.addPixmap(small);
    auto *blur = new QGraphicsBlurEffect;
    blur->setBlurRadius(6.0);
    blur->setBlurHints(QGraphicsBlurEffect::QualityHint);
    item->setGraphicsEffect(blur);

    QImage blurredSmall(small.size(), QImage::Format_ARGB32_Premultiplied);
    blurredSmall.fill(Qt::transparent);
    QPainter painter(&blurredSmall);
    scene.render(&painter, QRectF(0, 0, small.width(), small.height()),
                 QRectF(0, 0, small.width(), small.height()));
    painter.end();

    return QPixmap::fromImage(blurredSmall)
        .scaled(source.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}
} // namespace

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
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::BypassWindowManagerHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setCursor(Qt::CrossCursor);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_background = ScreenGrabber::grabVirtualDesktop();
    m_blurredBackground = blurredCopy(m_background);
    m_virtualDesktopRect = ScreenGrabber::virtualDesktopGeometry();

    setGeometry(m_virtualDesktopRect);
    qDebug() << "[CaptureOverlay] virtualDesktopRect=" << m_virtualDesktopRect
             << "backgroundSize=" << m_background.size();
}

void CaptureOverlay::showOverlay()
{
    show();
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

    // Blurred/out-of-focus everywhere by default...
    painter.drawPixmap(rect(), m_blurredBackground);
    painter.fillRect(rect(), QColor(0, 0, 0, 60));

    if (m_dragging) {
        const QRect sel = selectionInLocalCoords();

        // ...except the selected region, which is shown crisp and in focus.
        painter.drawPixmap(sel, m_background, sel);

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
