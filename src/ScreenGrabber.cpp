#include "ScreenGrabber.h"

#include <QGuiApplication>
#include <QPainter>
#include <QScreen>

QRect ScreenGrabber::virtualDesktopGeometry()
{
    QRect geometry;
    for (QScreen *screen : QGuiApplication::screens()) {
        geometry = geometry.united(screen->geometry());
    }
    return geometry;
}

QPixmap ScreenGrabber::grabVirtualDesktop()
{
    const QRect desktopRect = virtualDesktopGeometry();
    QPixmap composite(desktopRect.size());
    composite.fill(Qt::black);

    QPainter painter(&composite);
    for (QScreen *screen : QGuiApplication::screens()) {
        const QPixmap shot = screen->grabWindow(0);
        const QPoint dest = screen->geometry().topLeft() - desktopRect.topLeft();
        painter.drawPixmap(dest, shot.scaled(screen->geometry().size(),
                                              Qt::IgnoreAspectRatio,
                                              Qt::SmoothTransformation));
    }
    painter.end();
    return composite;
}

QPixmap ScreenGrabber::grabRegion(const QRect &globalRect)
{
    QScreen *screen = QGuiApplication::screenAt(globalRect.topLeft());
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }
    if (!screen) {
        return QPixmap();
    }

    const QRect screenGeom = screen->geometry();
    const QRect localRect = globalRect.translated(-screenGeom.topLeft());

    const QPixmap fullShot = screen->grabWindow(0);
    // The grabbed pixmap resolution may differ from the screen's logical
    // size (HiDPI), so scale the requested rectangle accordingly.
    const qreal scaleX = qreal(fullShot.width()) / qreal(screenGeom.width());
    const qreal scaleY = qreal(fullShot.height()) / qreal(screenGeom.height());
    const QRect deviceRect(qRound(localRect.x() * scaleX),
                            qRound(localRect.y() * scaleY),
                            qRound(localRect.width() * scaleX),
                            qRound(localRect.height() * scaleY));

    return fullShot.copy(deviceRect.intersected(fullShot.rect()));
}
