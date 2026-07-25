#pragma once
#include <QImage>
#include <QPixmap>
#include <QPoint>
#include <QRect>
#include <QWidget>
// Fullscreen (all-monitors) translucent overlay that lets the user drag out
// a rectangular selection on top of a frozen screenshot of the desktop.
// Used both for the simple screenshot capture and to pick the region that
// will be re-captured repeatedly for the scrolling screenshot feature.
class CaptureOverlay : public QWidget
{
    Q_OBJECT
public:
    // Default (X11 / other platforms where screen->grabWindow(0) works):
    // self-captures the whole virtual desktop and spans all monitors via
    // Qt::BypassWindowManagerHint.
    explicit CaptureOverlay(QWidget *parent = nullptr);

    // Wayland: screen->grabWindow(0) cannot capture real pixels there (the
    // compositor blocks arbitrary screen grabs), so the caller must supply
    // an already-captured frame obtained via the xdg-desktop-portal +
    // PipeWire backend (see WaylandPortalCaptureBackend). The overlay is
    // sized to that image and shown with showFullScreen() instead of
    // BypassWindowManagerHint, since Wayland compositors ignore the latter
    // (same fix already applied to ScrollRegionSelector).
    explicit CaptureOverlay(const QImage &screenshot, QWidget *parent = nullptr);

    // Shows the overlay and explicitly grabs keyboard/mouse input, which is
    // required because this window bypasses the window manager (see the
    // constructor) and therefore never receives focus automatically.
    void showOverlay();
signals:
    // rect is in global/virtual-desktop coordinates (X11 constructor), or
    // in the supplied screenshot's own local coordinates (Wayland
    // constructor) — in both cases, coordinates the caller can use
    // directly to crop the corresponding source image.
    void regionSelected(const QRect &rect);
    void cancelled();
protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
private:
    QRect selectionInLocalCoords() const;
    QPixmap m_background;
    QRect m_virtualDesktopRect;
    QPoint m_dragStart;
    QPoint m_dragCurrent;
    bool m_dragging = false;
    bool m_useFullScreen = false;
};
