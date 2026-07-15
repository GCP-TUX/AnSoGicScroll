#pragma once

#include <QIcon>

// Generates the application/tray icon in code, so the project has no
// external asset dependency and builds identically on every platform.
// For a production release, replace this with real .png/.ico/.icns assets
// (see resources/windows/app.rc and resources/macos/AppIcon.icns).
namespace IconFactory
{
QIcon appIcon();
}
