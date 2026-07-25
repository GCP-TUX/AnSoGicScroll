#pragma once
#include <QImage>
#include <QObject>
#include <QPointer>

class QSystemTrayIcon;
class CaptureOverlay;

#if defined(Q_OS_LINUX)
class WaylandPortalCaptureBackend;
#endif

// Drives the "simple" screenshot flow: shows the region-selection overlay,
// then copies the captured region to the clipboard and saves it to disk.
class ScreenshotManager : public QObject
{
    Q_OBJECT
public:
    explicit ScreenshotManager(QSystemTrayIcon *trayIcon, QObject *parent = nullptr);
    ~ScreenshotManager();
public slots:
    void start();
private slots:
    void onRegionSelected(const QRect &rect);
    void onCancelled();
private:
#if defined(Q_OS_LINUX)
         // Wayland has no working screen->grabWindow(0), so the simple
    // screenshot flow needs the same xdg-desktop-portal + PipeWire path
    // already used by the scrolling capture feature: grab a single frame
    // first, then let CaptureOverlay show/crop that frame instead of
    // self-capturing.
    void startWayland();
    void cleanupWaylandBackend();

    WaylandPortalCaptureBackend *m_waylandBackend = nullptr;
    QImage m_waylandFrame;
#endif

    QSystemTrayIcon *m_trayIcon;
    QPointer<CaptureOverlay> m_overlay;
};
