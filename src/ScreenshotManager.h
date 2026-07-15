#pragma once

#include <QObject>
#include <QPointer>

class QSystemTrayIcon;
class CaptureOverlay;

// Drives the "simple" screenshot flow: shows the region-selection overlay,
// then copies the captured region to the clipboard and saves it to disk.
class ScreenshotManager : public QObject
{
    Q_OBJECT

public:
    explicit ScreenshotManager(QSystemTrayIcon *trayIcon, QObject *parent = nullptr);

public slots:
    void start();

private slots:
    void onRegionSelected(const QRect &rect);
    void onCancelled();

private:
    QSystemTrayIcon *m_trayIcon;
    QPointer<CaptureOverlay> m_overlay;
};
