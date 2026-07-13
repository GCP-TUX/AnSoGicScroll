#include "ScrollCaptureManager.h"
#include "CaptureOverlay.h"
#include "ImageStitcher.h"
#include "ScreenGrabber.h"
#include "ScrollCaptureToolbar.h"

#include <QApplication>
#include <QClipboard>
#include <QDateTime>
#include <QDir>
#include <QStandardPaths>
#include <QSystemTrayIcon>

ScrollCaptureManager::ScrollCaptureManager(QSystemTrayIcon *trayIcon, QObject *parent)
    : QObject(parent)
    , m_trayIcon(trayIcon)
{
}

void ScrollCaptureManager::start()
{
    if (m_overlay || m_toolbar) {
        return; // a session is already in progress
    }

    m_overlay = new CaptureOverlay();
    connect(m_overlay, &CaptureOverlay::regionSelected, this, &ScrollCaptureManager::onRegionSelected);
    connect(m_overlay, &CaptureOverlay::cancelled, this, &ScrollCaptureManager::onSelectionCancelled);
    m_overlay->showOverlay();
}

void ScrollCaptureManager::onRegionSelected(const QRect &rect)
{
    m_region = rect;
    m_frames.clear();

    const QPixmap first = ScreenGrabber::grabRegion(m_region);
    if (!first.isNull()) {
        m_frames.append(first.toImage());
    }

    m_toolbar = new ScrollCaptureToolbar();
    connect(m_toolbar, &ScrollCaptureToolbar::captureNextRequested,
            this, &ScrollCaptureManager::onCaptureNextRequested);
    connect(m_toolbar, &ScrollCaptureToolbar::finishRequested,
            this, &ScrollCaptureManager::onFinishRequested);
    connect(m_toolbar, &ScrollCaptureToolbar::cancelRequested,
            this, &ScrollCaptureManager::onCancelRequested);
    m_toolbar->setFrameCount(m_frames.size());
    m_toolbar->show();
    m_toolbar->activateWindow();
    m_toolbar->setFocus(Qt::ActiveWindowFocusReason);
}

void ScrollCaptureManager::onSelectionCancelled()
{
    reset();
}

void ScrollCaptureManager::onCaptureNextRequested()
{
    const QPixmap frame = ScreenGrabber::grabRegion(m_region);
    if (!frame.isNull()) {
        m_frames.append(frame.toImage());
    }
    if (m_toolbar) {
        m_toolbar->setFrameCount(m_frames.size());
    }
}

void ScrollCaptureManager::onFinishRequested()
{
    const QList<QImage> frames = m_frames;

    if (m_toolbar) {
        m_toolbar->close();
    }
    m_overlay = nullptr;
    m_toolbar = nullptr;
    m_frames.clear();

    if (frames.isEmpty()) {
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    const QImage stitched = ImageStitcher::stitchVertically(frames);
    QApplication::restoreOverrideCursor();

    if (stitched.isNull()) {
        return;
    }

    QApplication::clipboard()->setImage(stitched);

    const QString picturesDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    QDir().mkpath(picturesDir);
    const QString fileName = QString("AnSoGicScroll_scroll_%1.png")
                                  .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    const QString filePath = QDir(picturesDir).filePath(fileName);
    stitched.save(filePath, "PNG");

    if (m_trayIcon) {
        m_trayIcon->showMessage(tr("Captura con desplazamiento"),
                                 tr("%1 capturas unidas. Copiada al portapapeles y guardada en:\n%2")
                                     .arg(frames.size())
                                     .arg(filePath),
                                 QSystemTrayIcon::Information, 4000);
    }
}

void ScrollCaptureManager::onCancelRequested()
{
    reset();
}

void ScrollCaptureManager::reset()
{
    if (m_toolbar) {
        m_toolbar->close();
    }
    if (m_overlay) {
        m_overlay->close();
    }
    m_overlay = nullptr;
    m_toolbar = nullptr;
    m_frames.clear();
}
