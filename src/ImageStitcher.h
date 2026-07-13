#pragma once

#include <QImage>
#include <QList>

// Stitches a sequence of overlapping vertical screenshots (captured while
// the user scrolled a window down between each shot) into a single tall
// image, by detecting the vertical pixel overlap between consecutive
// frames and discarding the duplicated region.
namespace ImageStitcher
{
QImage stitchVertically(const QList<QImage> &frames);
}
