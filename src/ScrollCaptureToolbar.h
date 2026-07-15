#pragma once

#include <QWidget>

class QLabel;
class QPushButton;

// Small always-on-top floating control panel shown during a scrolling
// screenshot session. Lets the user grab another frame after they've
// scrolled the target window, or finish/cancel the session.
class ScrollCaptureToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit ScrollCaptureToolbar(QWidget *parent = nullptr);

    void setFrameCount(int count);

signals:
    void captureNextRequested();
    void finishRequested();
    void cancelRequested();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QLabel *m_countLabel;
    QPushButton *m_captureButton;
    QPushButton *m_finishButton;
    QPushButton *m_cancelButton;
};
