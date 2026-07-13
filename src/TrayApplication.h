#pragma once

#include <QSystemTrayIcon>
#include <QObject>
#include <QPointer>
#include <QSystemTrayIcon>

#if defined(Q_OS_LINUX)
#include "waylandportalcapturebackend.h"
#endif

class QMenu;
class ScreenshotManager;
class ScrollCaptureManager;
class AboutDialog;
class captureScreenScroll;

// Owns the system tray icon and its context menu (screenshot capture,
// scrolling screenshot, about, exit) — the part equivalent to Flameshot's
// tray integration. QSystemTrayIcon itself is already implemented natively
// per platform by Qt:
//   - Linux:   StatusNotifierItem / XEmbed tray protocol (via the desktop's
//              notification area / tray, e.g. KDE, GNOME + extension, XFCE...)
//   - Windows: Shell_NotifyIcon (the notification area next to the clock)
//   - macOS:   NSStatusBar (the menu bar at the top of the screen)
class TrayApplication : public QObject
{
    Q_OBJECT

public:
    explicit TrayApplication(QObject *parent = nullptr);

    // Returns false if the current desktop/OS reports no system tray
    // support at all, in which case the caller should abort startup.
    bool isTraySupported() const;

    // TEMPORARY manual test hook, removed once the overlay is verified.
    void debugTriggerScreenshot();

private slots:


    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);

private:

    struct ScrollCaptureContext {
        captureScreenScroll* captureSS = nullptr;
        QString baseDir;
        int shotIdx = 0;
        QImage previousCapture;
        bool hasPrevious = false;

        std::function<QImage()> grabFrame;
        std::function<bool()> doScroll;
        std::function<void()> cleanup;
    };

#if defined(Q_OS_LINUX)
    WaylandPortalCaptureBackend* m_waylandBackend = nullptr;
#endif

    void buildMenu();

    void startScrollingCapture();

    QString createScrollCaptureDir() const;
    bool runCaptureLoop(ScrollCaptureContext& ctx) const;
    void waitAfterScroll(const QImage& beforeScroll) const;
    bool stitchAndSaveResult(captureScreenScroll* captureSS, const QString& baseDir) const;


#if defined(Q_OS_LINUX)
    QRect selectScrollRegionFromImage(const QImage& screenshot);
    bool setupLinuxScrollingCapture(ScrollCaptureContext& ctx);
#endif

#if defined(Q_OS_WIN)
    bool setupWindowsScrollingCapture(ScrollCaptureContext& ctx);
#endif


    QSystemTrayIcon *m_trayIcon;
    QMenu *m_menu;
    ScreenshotManager *m_screenshotManager;
    ScrollCaptureManager *m_scrollCaptureManager;
    QPointer<AboutDialog> m_aboutDialog;
};
