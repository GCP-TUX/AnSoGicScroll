#include "ScreenshotManager.h"
#include "CaptureOverlay.h"
#include "ScreenGrabber.h"

#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QDir>
#include <QFileDialog>
#include <QStandardPaths>
#include <QSystemTrayIcon>

#if defined(Q_OS_LINUX)
#include "waylandportalcapturebackend.h"
#include <QMessageBox>
#include <QThread>
#endif

ScreenshotManager::ScreenshotManager(QSystemTrayIcon *trayIcon, QObject *parent)
    : QObject(parent)
    , m_trayIcon(trayIcon)
{
}

ScreenshotManager::~ScreenshotManager()
{
#if defined(Q_OS_LINUX)
    cleanupWaylandBackend();
#endif
}

void ScreenshotManager::start()
{
    if (m_overlay) {
        return; // capture already in progress
    }

#if defined(Q_OS_LINUX)
    const QByteArray waylandDisplay = qgetenv("WAYLAND_DISPLAY");
    const QByteArray sessionType = qgetenv("XDG_SESSION_TYPE").toLower();
    const bool isWayland = !waylandDisplay.isEmpty() || sessionType == "wayland";

    if (isWayland) {
        startWayland();
        return;
    }
#endif

    m_overlay = new CaptureOverlay();
    connect(m_overlay, &CaptureOverlay::regionSelected, this, &ScreenshotManager::onRegionSelected);
    connect(m_overlay, &CaptureOverlay::cancelled, this, &ScreenshotManager::onCancelled);
    m_overlay->showOverlay();
}

#if defined(Q_OS_LINUX)
void ScreenshotManager::startWayland()
{
    cleanupWaylandBackend(); // just in case a previous run left something behind

    m_waylandBackend = new WaylandPortalCaptureBackend(this);

    if (!m_waylandBackend->initialize(QString())) {
        QMessageBox::warning(
            nullptr,
            tr("Captura de pantalla"),
            tr("Failed to initialize Wayland capture using "
               "xdg-desktop-portal + PipeWire."));
        cleanupWaylandBackend();
        return;
    }

    QImage firstFrame;
    for (int i = 0; i < 30; ++i) {
        QThread::msleep(100);
        firstFrame = m_waylandBackend->latestFrame();
        if (!firstFrame.isNull()) {
            break;
        }
        QApplication::processEvents();
    }

    if (firstFrame.isNull()) {
        cleanupWaylandBackend();
        QMessageBox::warning(
            nullptr,
            tr("Captura de pantalla"),
            tr("Failed to get the initial screen frame on Wayland."));
        return;
    }

    m_waylandFrame = firstFrame;

    m_overlay = new CaptureOverlay(m_waylandFrame);
    connect(m_overlay, &CaptureOverlay::regionSelected, this, &ScreenshotManager::onRegionSelected);
    connect(m_overlay, &CaptureOverlay::cancelled, this, &ScreenshotManager::onCancelled);
    m_overlay->showOverlay();
}

void ScreenshotManager::cleanupWaylandBackend()
{
    if (m_waylandBackend) {
        m_waylandBackend->shutdown();
        m_waylandBackend->deleteLater();
        m_waylandBackend = nullptr;
    }
    m_waylandFrame = QImage();
}
#endif

void ScreenshotManager::onRegionSelected(const QRect &rect)
{
    QPixmap capture;

#if defined(Q_OS_LINUX)
    if (!m_waylandFrame.isNull()) {
        // Crop directly from the already-captured portal frame — rect is
        // already in that image's own local coordinates (see the
        // CaptureOverlay Wayland constructor).
        capture = QPixmap::fromImage(m_waylandFrame.copy(rect));
        cleanupWaylandBackend();
    } else
#endif
    {
        capture = ScreenGrabber::grabRegion(rect);
    }

    if (capture.isNull()) {
        return;
    }

    QApplication::clipboard()->setPixmap(capture);

    const QString picturesDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    QDir().mkpath(picturesDir);
    const QString fileName = QString("AnSoGicScroll_%1.png")
                                 .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    const QString defaultPath = QDir(picturesDir).filePath(fileName);

    const QString filePath = QFileDialog::getSaveFileName(nullptr,
                                                          tr("Guardar captura de pantalla"),
                                                          defaultPath,
                                                          tr("Imagen PNG (*.png)"));
    if (filePath.isEmpty()) {
        return; // user cancelled the save dialog; clipboard copy is kept
    }

    capture.save(filePath, "PNG");

    if (m_trayIcon) {
        m_trayIcon->showMessage(tr("Captura de pantalla"),
                                tr("Copiada al portapapeles y guardada en:\n%1").arg(filePath),
                                QSystemTrayIcon::Information, 4000);
    }
}

void ScreenshotManager::onCancelled()
{
#if defined(Q_OS_LINUX)
    cleanupWaylandBackend();
#endif
    // Nothing else to clean up: the overlay closes itself and is deleted
    // automatically (WA_DeleteOnClose).
}
