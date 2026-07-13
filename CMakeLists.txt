cmake_minimum_required(VERSION 3.16)
project(AnSoGicScroll VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core DBus)
find_package(OpenCV REQUIRED)

# --- Linux-only: PkgConfig / PipeWire / X11 --------------------------------
# PkgConfig y libpipewire no existen en Windows/macOS, así que se detectan
# únicamente en Linux (se usan para el backend de captura vía Wayland/Portal).
if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PIPEWIRE REQUIRED IMPORTED_TARGET libpipewire-0.3)
    find_package(X11 REQUIRED)
endif()

set(SOURCES
    src/main.cpp
    src/TrayApplication.cpp
    src/CaptureOverlay.cpp
    src/ScreenGrabber.cpp
    src/ScreenshotManager.cpp
    src/ScrollCaptureManager.cpp
    src/ScrollCaptureToolbar.cpp
    src/ImageStitcher.cpp
    src/AboutDialog.cpp
    src/IconFactory.cpp
)
set(HEADERS
    src/TrayApplication.h
    src/CaptureOverlay.h
    src/ScreenGrabber.h
    src/ScreenshotManager.h
    src/ScrollCaptureManager.h
    src/ScrollCaptureToolbar.h
    src/ImageStitcher.h
    src/AboutDialog.h
    src/IconFactory.h
)

# --- Fuentes multiplataforma (usadas en Windows, macOS y Linux) -----------
# Estas clases implementan comportamiento condicional internamente (#ifdef),
# por eso deben compilarse en TODAS las plataformas, no solo en Linux.
list(APPEND SOURCES
    src/capturescreenscroll.cpp
    src/scrollregionselector.cpp
    src/windowhighlightoverlay.cpp
)
list(APPEND HEADERS
    src/capturescreenscroll.h
    src/scrollregionselector.h
    src/windowhighlightoverlay.h
)

# --- Fuentes exclusivas de Linux (Wayland / uinput / xdg-desktop-portal) --
if(UNIX AND NOT APPLE)
    list(APPEND SOURCES
        src/uinputscrollcontroller.cpp
        src/waylandportalcapturebackend.cpp
        src/portalrequesthelper.cpp
    )
    list(APPEND HEADERS
        src/uinputscrollcontroller.h
        src/waylandportalcapturebackend.h
        src/portalrequesthelper.h
    )
endif()

# --- Platform specific application target setup ---------------------------
# On Windows we build a WIN32 (GUI subsystem) executable so no console window
# is spawned. On macOS we build a proper .app bundle so the system tray /
# menu bar integration behaves like a native app.
set(APP_ICON_RESOURCE_WINDOWS "")
if(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/resources/windows/app.ico")
    set(APP_ICON_ABSOLUTE_PATH "${CMAKE_SOURCE_DIR}/resources/windows/app.ico")
    configure_file(
        "${CMAKE_SOURCE_DIR}/resources/windows/app.rc.in"
        "${CMAKE_BINARY_DIR}/app.rc"
        @ONLY)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_BINARY_DIR}/app.rc")
endif()

set(MACOSX_ICON_FILE "")
if(APPLE AND EXISTS "${CMAKE_SOURCE_DIR}/resources/macos/AppIcon.icns")
    set(MACOSX_ICON_FILE "${CMAKE_SOURCE_DIR}/resources/macos/AppIcon.icns")
    set_source_files_properties(${MACOSX_ICON_FILE} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
endif()

if(WIN32)
    add_executable(AnSoGicScroll WIN32 ${SOURCES} ${HEADERS} ${APP_ICON_RESOURCE_WINDOWS})
elseif(APPLE)
    add_executable(AnSoGicScroll MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${MACOSX_ICON_FILE})
else()
    add_executable(AnSoGicScroll ${SOURCES} ${HEADERS})
endif()

target_link_libraries(AnSoGicScroll PRIVATE
    Qt6::Widgets Qt6::Gui Qt6::Core Qt6::DBus
    ${OpenCV_LIBS}
)

if(UNIX AND NOT APPLE)
    target_link_libraries(AnSoGicScroll PRIVATE X11::X11 PkgConfig::PIPEWIRE)
endif()

if(WIN32)
    target_link_libraries(AnSoGicScroll PRIVATE dwmapi)
endif()

target_include_directories(AnSoGicScroll PRIVATE src ${OpenCV_INCLUDE_DIRS})

# --- macOS bundle metadata --------------------------------------------------
if(APPLE)
    set_target_properties(AnSoGicScroll PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "AnSoGicScroll"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.ansogicscroll.app"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_ICON_FILE "AppIcon.icns"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/resources/macos/Info.plist.in"
    )
endif()

# --- Linux install rules (desktop entry + binary) ---------------------------
if(UNIX AND NOT APPLE)
    install(TARGETS AnSoGicScroll RUNTIME DESTINATION bin)
    install(FILES resources/linux/ansogicscroll.desktop
            DESTINATION share/applications)
endif()
