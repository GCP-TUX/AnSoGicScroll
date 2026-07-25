#include "scrollregionselector.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPen>
ScrollRegionSelector::ScrollRegionSelector(QWidget* parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint |
                   Qt::Tool |
                   Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::CrossCursor);
}
void ScrollRegionSelector::setScreenshot(const QImage& image)
{
    m_screenshot = image;
    update();
}
bool ScrollRegionSelector::hasScreenshot() const
{
    return !m_screenshot.isNull();
}
QRect ScrollRegionSelector::currentRect() const
{
    return QRect(m_start, m_end).normalized();
}
void ScrollRegionSelector::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    const QRect sel = currentRect();

    // IMPORTANT: darkening/selection styling is gated on m_selecting (the
    // user has actually pressed the mouse and started dragging), NOT on
    // "!sel.isNull()". Before any drag, m_start == m_end == (0,0), and
    // QRect's two-point constructor can still yield a non-null (1x1) rect
    // in that case — checking only sel.isNull() made the dark overlay
    // appear across almost the whole screen from the very first frame,
    // before the user had done anything, making it hard to see what to
    // capture. Checking m_selecting fixes that: the screenshot now shows
    // fully crisp and undimmed until the user actually starts dragging.
    const bool showSelectionStyling = m_selecting && !sel.isNull();

    if (!m_screenshot.isNull()) {
        // Wayland mode: draw the screenshot crisp; only shade the area
        // outside the selection, and only once dragging has begun.
        p.drawImage(rect(), m_screenshot);
        if (showSelectionStyling) {
            QPainterPath full;
            full.addRect(rect());
            QPainterPath hole;
            hole.addRect(sel);
            p.fillPath(full.subtracted(hole), QColor(0, 0, 0, 90));
            p.fillRect(sel, QColor(180, 0, 255, 30));
        }
    } else {
        // Modo Xorg/X11: NO oscurecer fondo, dejar totalmente transparente
        p.fillRect(rect(), Qt::transparent);
        if (showSelectionStyling) {
            p.fillRect(sel, QColor(255, 0, 0, 20));
        }
    }
    if (showSelectionStyling) {
        QPen pen(QColor(220, 100, 255), 2);
        p.setPen(pen);
        p.drawRect(sel.adjusted(0, 0, -1, -1));
    }
}
void ScrollRegionSelector::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        m_start = event->position().toPoint();
#else
        m_start = event->pos();
#endif
        m_end = m_start;
        m_selecting = true;
        update();
    }
}
void ScrollRegionSelector::mouseMoveEvent(QMouseEvent* event)
{
    if (!m_selecting) {
        return;
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    m_end = event->position().toPoint();
#else
    m_end = event->pos();
#endif
    update();
}
void ScrollRegionSelector::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_selecting) {
        m_selecting = false;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        m_end = event->position().toPoint();
#else
        m_end = event->pos();
#endif
        emit selectionFinished(currentRect());
    }
}
void ScrollRegionSelector::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        emit selectionCanceled();
        return;
    }
    QWidget::keyPressEvent(event);
}
