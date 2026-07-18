cmake_minimum_required(VERSION 3.16)
project(AnSoGicScroll VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core DBus)
find_package(OpenCV REQUIRED)

# --- Windows: evitar que <windows.h> defina macros min/max ----------------
# Sin esto, cualquier header que incluya <windows.h> antes que OpenCV rompe
# std::numeric_limits<T>::max() en los headers de OpenCV (error C2589, etc.)
# _WIN32_WINNT=0x0A00 (Windows 10) habilita APIs usadas para DPI awareness
# (SetProcessDpiAwarenessContext, GetDpiForWindow) en todo el proyecto.
if(WIN32)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0A00)
endif()

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
    target_link_libraries(AnSoGicScroll PRIVATE dwmapi Shcore)

    # Corre windeployqt automáticamente después de cada build. IMPORTANTE:
    # se usa --dir para que las DLLs/plugins se copien a una subcarpeta
    # AISLADA (build/deploy/), NUNCA directo en build/. Si se copiaran en
    # build/ (la raíz), install(DIRECTORY ...) más abajo terminaría
    # copiando también la carpeta de trabajo temporal de CPack
    # (build/_CPack_Packages/...) dentro de sí misma, generando una
    # recursión infinita que llena el disco (esto fue exactamente lo que
    # causó que "cpack -G NSIS" fallara/colgara la primera vez).
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt)
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET AnSoGicScroll POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/deploy"
            COMMAND "${WINDEPLOYQT_EXECUTABLE}" --release --no-translations
                    --dir "${CMAKE_BINARY_DIR}/deploy"
                    "$<TARGET_FILE:AnSoGicScroll>"
            COMMENT "Empaquetando dependencias de Qt con windeployqt (aislado en deploy/)")
    else()
        message(WARNING "windeployqt no encontrado: las DLLs de Qt no se "
                         "copiaran automaticamente junto al ejecutable.")
    endif()
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

    # Instala el ícono en cada resolución del tema hicolor estándar
    foreach(size 16 24 32 48 64 128 256)
        install(FILES "resources/linux/icons/${size}x${size}/ansogicscroll.png"
                DESTINATION "share/icons/hicolor/${size}x${size}/apps"
                RENAME "ansogicscroll.png")
    endforeach()

    # Regla udev para /dev/uinput (autoscroll en X11). Se instala como
    # archivo del paquete; los scripts postinst/%post además la reescriben
    # y recargan udev automáticamente al instalar.
    install(FILES resources/linux/udev/99-ansogicscroll-uinput.rules
            DESTINATION /etc/udev/rules.d)
endif()

# --- Windows install rules (exe + DLLs/plugins de windeployqt) -------------
if(WIN32)
    install(TARGETS AnSoGicScroll RUNTIME DESTINATION .)

    # Fuente = build/deploy/ (carpeta aislada, ver POST_BUILD arriba), NO
    # el directorio raíz de build/ — así se evita el auto-copiado
    # recursivo con la carpeta de trabajo de CPack (_CPack_Packages/...).
    install(DIRECTORY "${CMAKE_BINARY_DIR}/deploy/"
            DESTINATION .
            FILES_MATCHING
                PATTERN "*.dll"
                PATTERN "platforms/*"
                PATTERN "styles/*"
                PATTERN "imageformats/*"
                PATTERN "generic/*")
endif()

# =============================================================
# CPack: empaquetado (.deb en Linux, instalador NSIS en Windows)
# =============================================================
include(GNUInstallDirs)

set(CPACK_PACKAGE_NAME "AnSoGicScroll")
set(CPACK_PACKAGE_VENDOR "GCP-TUX")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Captura de pantalla con autoscroll inteligente")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "Gicel Cordoba Pech <gicel.cordoba@hotmail.com>")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

# --------------------------------------------------------------
# LINUX -> genera .deb (APT) y .rpm (DNF/YUM/Zypper)
# --------------------------------------------------------------
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CPACK_GENERATOR "DEB;RPM")

    # --- .deb ---
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Gicel Cordoba Pech <gicel.cordoba@hotmail.com>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
    set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
    # Detecta automáticamente las dependencias reales del binario compilado
    # (Qt6, OpenCV, X11, PipeWire, etc.) inspeccionando los .so enlazados
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    # Scripts post-instalación/desinstalación: configuran udev + grupo
    # 'input' automáticamente para que el autoscroll en X11 funcione sin
    # pasos manuales del usuario.
    set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
        "${CMAKE_SOURCE_DIR}/scripts/packaging/postinst;${CMAKE_SOURCE_DIR}/scripts/packaging/postrm")

    # --- .rpm ---
    set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/Multimedia")
    set(CPACK_RPM_PACKAGE_URL "https://github.com/GCP-TUX/AnSoGicScroll")
    set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
    # Equivalente de SHLIBDEPS para RPM: detecta dependencias automáticamente
    set(CPACK_RPM_PACKAGE_AUTOREQPROV ON)
    # Mismo propósito que CONTROL_EXTRA arriba, pero en formato RPM
    set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/scripts/packaging/rpm-post.sh")
    set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/scripts/packaging/postrm")
endif()

# --------------------------------------------------------------
# WINDOWS -> instalador NSIS
# --------------------------------------------------------------
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_PACKAGE_NAME "AnSoGicScroll")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_MODIFY_PATH ON)
    # Crea accesos directos en el menú Inicio (y ofrece uno en el escritorio)
    set(CPACK_PACKAGE_EXECUTABLES "AnSoGicScroll" "AnSoGicScroll")
    set(CPACK_CREATE_DESKTOP_LINKS "AnSoGicScroll")
endif()

include(CPack)
