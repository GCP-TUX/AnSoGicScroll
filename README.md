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

### Linux (via APT)

```bash
curl -fsSL https://GCP-TUX.github.io/AnSoGicScroll/pubkey.gpg | \
    sudo gpg --dearmor -o /usr/share/keyrings/ansogicscroll.gpg

echo "deb [signed-by=/usr/share/keyrings/ansogicscroll.gpg] \
https://GCP-TUX.github.io/AnSoGicScroll stable main" | \
    sudo tee /etc/apt/sources.list.d/ansogicscroll.list

sudo apt update
sudo apt install ansogicscroll
```

See [INSTALL_APT.md](INSTALL_APT.md) for more details.

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
