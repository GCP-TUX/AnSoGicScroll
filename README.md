# AnSoGicScroll

A cross-platform screenshot tool with **smart auto-scroll capture**: it captures windows or regions that don't fit on a single screen, automatically detects when scrolling has reached the end, and stitches all the captures together into one continuous image.

## Supported platforms

| Platform | Status |
|---|---|
| Linux (X11) | ✅ Supported |
| Linux (Wayland) | ✅ Supported (via xdg-desktop-portal + PipeWire) |
| Windows | ✅ Supported |
| macOS | 🚧 Coming soon |

## Demo

### Linux (X11)
[![Linux X11 demo](https://img.youtube.com/vi/nu78ZcpUBis/maxresdefault.jpg)](https://www.youtube.com/watch?v=nu78ZcpUBis)

### Linux (Wayland)
[![Linux Wayland demo](https://img.youtube.com/vi/GsLFDsY_g1k/maxresdefault.jpg)](https://www.youtube.com/watch?v=GsLFDsY_g1k)

### Windows
[![Windows demo](https://img.youtube.com/vi/YBKilkOGQpA/maxresdefault.jpg)](https://www.youtube.com/watch?v=YBKilkOGQpA)

## Installation

### Linux — Debian / Ubuntu 24.04+ (via APT)

```bash
curl -fsSL https://gcp-tux.github.io/AnSoGicScroll/pubkey.gpg | \
    sudo gpg --dearmor -o /usr/share/keyrings/ansogicscroll.gpg

echo "deb [signed-by=/usr/share/keyrings/ansogicscroll.gpg] \
https://gcp-tux.github.io/AnSoGicScroll stable main" | \
    sudo tee /etc/apt/sources.list.d/ansogicscroll.list

sudo apt update
sudo apt install ansogicscroll
```

### Linux — Ubuntu 22.04 (via APT)

Same as above, but use `jammy` instead of `stable`:

```bash
curl -fsSL https://gcp-tux.github.io/AnSoGicScroll/pubkey.gpg | \
    sudo gpg --dearmor -o /usr/share/keyrings/ansogicscroll.gpg

echo "deb [signed-by=/usr/share/keyrings/ansogicscroll.gpg] \
https://gcp-tux.github.io/AnSoGicScroll jammy main" | \
    sudo tee /etc/apt/sources.list.d/ansogicscroll.list

sudo apt update
sudo apt install ansogicscroll
```

See [INSTALL_APT.md](INSTALL_APT.md) for more details.

### Linux — Fedora (via DNF)

```bash
sudo curl -fsSL https://gcp-tux.github.io/AnSoGicScroll/pubkey.gpg \
    -o /etc/pki/rpm-gpg/RPM-GPG-KEY-ansogicscroll

sudo curl -fsSL https://raw.githubusercontent.com/GCP-TUX/AnSoGicScroll/master/resources/linux/ansogicscroll.repo \
    -o /etc/yum.repos.d/ansogicscroll.repo

sudo dnf install ansogicscroll
```

### Linux — any distro (AppImage)

No installation or root access required:

```bash
wget https://github.com/GCP-TUX/AnSoGicScroll/releases/latest/download/AnSoGicScroll-x86_64.AppImage
chmod +x AnSoGicScroll-x86_64.AppImage
./AnSoGicScroll-x86_64.AppImage
```

> **Note (Linux, X11 auto-scroll):** on the first install via APT or DNF, the package automatically grants your user access to `/dev/uinput` (required for simulating scroll events on X11) by adding you to the `input` group. This only takes effect after you **log out and back in** (or reboot) — a one-time step required by Linux itself, not something the installer can skip. If auto-scroll shows a "could not access /dev/uinput" error right after installing, just log out/in once and it will work. This step is not needed on Wayland.

### Windows

Download the latest `.zip` or installer from the [Releases](https://github.com/GCP-TUX/AnSoGicScroll/releases) page.

## Building from source

### Requirements

- CMake ≥ 3.16
- Qt 6 (Widgets, Gui, Core, DBus)
- OpenCV ≥ 4.5
- **Linux:** X11, `libpipewire-0.3-dev`, `pkg-config`, `wayland-dev`
- **Windows:** MinGW or MSVC, `dwmapi`

### Build (Linux)

```bash
git clone https://github.com/GCP-TUX/AnSoGicScroll.git
cd AnSoGicScroll
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Build (Windows, Qt Creator)

1. Open `CMakeLists.txt` in Qt Creator.
2. Select the "Desktop Qt 6.9.1 MinGW 64-bit" kit (or whichever matches your setup).
3. Build.

## Features

- Window, custom region, or full-screen capture
- Auto-scroll with automatic end-of-content detection
- Stitching of overlapping captures using OpenCV
- System tray icon
- Cross-platform: Linux (X11/Wayland) and Windows

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you'd like to change.

## License

This project is licensed under the **GNU General Public License v3.0 (GPLv3)**.

You are free to use, study, share, and modify this software, provided that any distributed derivative work is also released under the GPLv3 and its source code is made available. See the [LICENSE](LICENSE) file for the full license text.

```
AnSoGicScroll
Copyright (C) 2026  GCP-TUX

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
```

## Credits

Developed by [GCP-TUX](https://github.com/GCP-TUX).
