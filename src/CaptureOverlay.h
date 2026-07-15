#pragma once

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
    explicit CaptureOverlay(QWidget *parent = nullptr);

    // Shows the overlay and explicitly grabs keyboard/mouse input, which is
    // required because this window bypasses the window manager (see the
    // constructor) and therefore never receives focus automatically.
    void showOverlay();

signals:
    // rect is in global/virtual-desktop coordinates.
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
    QPixmap m_blurredBackground;
    QRect m_virtualDesktopRect;
    QPoint m_dragStart;
    QPoint m_dragCurrent;
    bool m_dragging = false;
};
