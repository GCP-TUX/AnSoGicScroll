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

ScreenshotManager::ScreenshotManager(QSystemTrayIcon *trayIcon, QObject *parent)
    : QObject(parent)
    , m_trayIcon(trayIcon)
{
}

void ScreenshotManager::start()
{
    if (m_overlay) {
        return; // capture already in progress
    }

    m_overlay = new CaptureOverlay();
    connect(m_overlay, &CaptureOverlay::regionSelected, this, &ScreenshotManager::onRegionSelected);
    connect(m_overlay, &CaptureOverlay::cancelled, this, &ScreenshotManager::onCancelled);
    m_overlay->showOverlay();
}

void ScreenshotManager::onRegionSelected(const QRect &rect)
{
    const QPixmap capture = ScreenGrabber::grabRegion(rect);
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
    // Nothing to clean up: the overlay closes itself and is deleted
    // automatically (WA_DeleteOnClose).
}
