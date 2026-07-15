#include "ScrollCaptureToolbar.h"

#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QScreen>

ScrollCaptureToolbar::ScrollCaptureToolbar(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_DeleteOnClose);
    setStyleSheet(
        "QWidget { background-color: #263238; border-radius: 6px; }"
        "QLabel { color: white; padding: 0 8px; }"
        "QPushButton { color: white; background-color: #37474F; border: none;"
        "  padding: 6px 12px; border-radius: 4px; }"
        "QPushButton:hover { background-color: #455A64; }");

    m_countLabel = new QLabel(tr("Capturas: 1"), this);
    m_captureButton = new QPushButton(tr("Capturar siguiente (Espacio)"), this);
    m_finishButton = new QPushButton(tr("Finalizar (Enter)"), this);
    m_cancelButton = new QPushButton(tr("Cancelar (Esc)"), this);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 8, 10, 8);
    layout->addWidget(m_countLabel);
    layout->addWidget(m_captureButton);
    layout->addWidget(m_finishButton);
    layout->addWidget(m_cancelButton);

    connect(m_captureButton, &QPushButton::clicked, this, &ScrollCaptureToolbar::captureNextRequested);
    connect(m_finishButton, &QPushButton::clicked, this, &ScrollCaptureToolbar::finishRequested);
    connect(m_cancelButton, &QPushButton::clicked, this, &ScrollCaptureToolbar::cancelRequested);

    adjustSize();

    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        const QRect avail = screen->availableGeometry();
        move(avail.center().x() - width() / 2, avail.bottom() - height() - 24);
    }
}

void ScrollCaptureToolbar::setFrameCount(int count)
{
    m_countLabel->setText(tr("Capturas: %1").arg(count));
}

void ScrollCaptureToolbar::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Space:
        emit captureNextRequested();
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        emit finishRequested();
        return;
    case Qt::Key_Escape:
        emit cancelRequested();
        return;
    default:
        break;
    }
    QWidget::keyPressEvent(event);
}
