#pragma once

#include <QImage>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QRect>

class QSystemTrayIcon;
class CaptureOverlay;
class ScrollCaptureToolbar;

// Drives the scrolling-screenshot flow:
//   1. User selects the region of the window that will be scrolled.
//   2. A floating toolbar lets the user scroll the target window and
//      request another capture of that same region as many times as needed.
//   3. On "Finish", all captured frames are stitched into one tall image
//      (see ImageStitcher) and copied to the clipboard / saved to disk.
class ScrollCaptureManager : public QObject
{
    Q_OBJECT

public:
    explicit ScrollCaptureManager(QSystemTrayIcon *trayIcon, QObject *parent = nullptr);

public slots:
    void start();

private slots:
    void onRegionSelected(const QRect &rect);
    void onSelectionCancelled();
    void onCaptureNextRequested();
    void onFinishRequested();
    void onCancelRequested();

private:
    void reset();

    QSystemTrayIcon *m_trayIcon;
    QPointer<CaptureOverlay> m_overlay;
    QPointer<ScrollCaptureToolbar> m_toolbar;
    QRect m_region;
    QList<QImage> m_frames;
};
