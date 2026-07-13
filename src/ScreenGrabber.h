#pragma once

#include <QPixmap>
#include <QRect>

// Cross-platform desktop capture helpers built purely on top of QScreen,
// which already abstracts the platform-specific grabbing APIs
// (X11/Wayland on Linux, GDI on Windows, CoreGraphics on macOS).
namespace ScreenGrabber
{
// Bounding rectangle (in device-independent pixels) of every connected
// screen combined, i.e. the full virtual desktop.
QRect virtualDesktopGeometry();

// Grabs every screen and composites them into a single pixmap covering
// virtualDesktopGeometry(). The returned pixmap's top-left corner
// corresponds to virtualDesktopGeometry().topLeft().
QPixmap grabVirtualDesktop();

// Grabs only the given rectangle (in global/virtual-desktop coordinates).
// The rectangle is expected to lie within a single screen, which is the
// common case for window-region captures; if it spans multiple screens it
// is grabbed from whichever screen contains its top-left corner.
QPixmap grabRegion(const QRect &globalRect);
}
