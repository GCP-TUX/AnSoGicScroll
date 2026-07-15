#include "IconFactory.h"

#include <QPainter>
#include <QPixmap>

namespace
{
QPixmap renderIcon(int size)
{
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const qreal m = size * 0.08;
    const QRectF bodyRect(m, size * 0.28, size - 2 * m, size * 0.58);

    // Camera body
    QLinearGradient gradient(bodyRect.topLeft(), bodyRect.bottomRight());
    gradient.setColorAt(0.0, QColor("#4FC3F7"));
    gradient.setColorAt(1.0, QColor("#0288D1"));
    painter.setBrush(gradient);
    painter.setPen(QPen(QColor("#01579B"), size * 0.03));
    painter.drawRoundedRect(bodyRect, size * 0.08, size * 0.08);

    // Viewfinder bump
    QRectF bump(size * 0.36, size * 0.16, size * 0.28, size * 0.16);
    painter.drawRoundedRect(bump, size * 0.03, size * 0.03);

    // Lens
    QRectF lens(size * 0.32, size * 0.40, size * 0.36, size * 0.36);
    painter.setBrush(QColor("#01579B"));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(lens);
    painter.setBrush(QColor("#B3E5FC"));
    QRectF lensInner(size * 0.38, size * 0.46, size * 0.24, size * 0.24);
    painter.drawEllipse(lensInner);

    painter.end();
    return pixmap;
}
} // namespace

QIcon IconFactory::appIcon()
{
    QIcon icon;
    for (int size : {16, 24, 32, 48, 64, 128, 256}) {
        icon.addPixmap(renderIcon(size));
    }
    return icon;
}
