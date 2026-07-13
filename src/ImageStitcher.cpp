#include "ImageStitcher.h"

#include <QPainter>
#include <QVector>
#include <QtGlobal>

namespace
{
constexpr int kMinOverlapRows = 8;
constexpr int kMaxAvgDiff = 14; // 0..255 grayscale, empirically chosen

// One integer "brightness" value per row, sampled across the row's width.
// Comparing these signatures is much cheaper than comparing full rows and
// is enough to reliably locate the scroll offset between two frames.
QVector<int> rowSignatures(const QImage &img)
{
    QVector<int> sig(img.height(), 0);
    const int step = qMax(1, img.width() / 64);
    for (int y = 0; y < img.height(); ++y) {
        int sum = 0;
        int count = 0;
        for (int x = 0; x < img.width(); x += step) {
            sum += qGray(img.pixel(x, y));
            ++count;
        }
        sig[y] = count ? sum / count : 0;
    }
    return sig;
}

// Finds how many pixels the window scrolled between prev and next, i.e. the
// number of rows to drop from the top of `next` because they duplicate the
// bottom of `prev`. Returns the overlap length in prev's coordinate system,
// or 0 if no reliable overlap could be found.
int findOverlapRows(const QVector<int> &prevSig, const QVector<int> &nextSig)
{
    const int prevHeight = prevSig.size();

    int bestScroll = -1;
    int bestAvgDiff = kMaxAvgDiff + 1;

    for (int scroll = 0; scroll <= prevHeight - kMinOverlapRows; ++scroll) {
        const int overlapLen = prevHeight - scroll;
        qint64 diff = 0;
        for (int k = 0; k < overlapLen; ++k) {
            diff += qAbs(prevSig[scroll + k] - nextSig[k]);
        }
        const int avgDiff = int(diff / overlapLen);
        if (avgDiff < bestAvgDiff) {
            bestAvgDiff = avgDiff;
            bestScroll = scroll;
        }
    }

    if (bestScroll < 0 || bestAvgDiff > kMaxAvgDiff) {
        return 0; // no reliable overlap detected
    }
    return prevHeight - bestScroll; // overlap length
}
} // namespace

QImage ImageStitcher::stitchVertically(const QList<QImage> &frames)
{
    if (frames.isEmpty()) {
        return QImage();
    }
    if (frames.size() == 1) {
        return frames.first();
    }

    const int width = frames.first().width();

    QList<QImage> pieces; // first piece is the full first frame; the rest
                           // are the non-overlapping bottom slices.
    pieces.append(frames.first());

    QVector<int> prevSig = rowSignatures(frames.first());
    int totalHeight = frames.first().height();

    for (int i = 1; i < frames.size(); ++i) {
        const QImage &next = frames.at(i);
        const QVector<int> nextSig = rowSignatures(next);

        const int overlap = findOverlapRows(prevSig, nextSig);
        const int skipRows = qBound(0, overlap, next.height());

        const QImage slice = next.copy(0, skipRows, width, next.height() - skipRows);
        if (!slice.isNull() && slice.height() > 0) {
            pieces.append(slice);
            totalHeight += slice.height();
        }

        prevSig = nextSig;
    }

    QImage result(width, totalHeight, QImage::Format_RGB32);
    QPainter painter(&result);
    int y = 0;
    for (const QImage &piece : pieces) {
        painter.drawImage(0, y, piece);
        y += piece.height();
    }
    painter.end();

    return result;
}
