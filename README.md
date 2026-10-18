# Aevis Dock

Application dock component for the Aevis Desktop Environment.

Part of the **[Project Aevis](https://github.com/project-Aevis)** suite.

## Features

- Dynamic app pinning and launcher integration
- Active window management and window previews
- Autohide and position customization
- Native FishUI dark/light mode and blur styling

## Dependencies

Requires Qt 6, **[FishUI](https://github.com/project-Aevis/fishui)**, **[cutefish-framework](https://github.com/project-Aevis/cutefish-framework)**, and KDE KWindowSystem.

### Debian/Ubuntu

```shell
sudo apt install cmake extra-cmake-modules build-essential \
    qt6-base-dev qt6-declarative-dev libkf6windowsystem-dev
```

## Build and Install

```shell
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make
sudo make install
```

## License

This project is licensed under the GNU General Public License v3.0 (GPLv3).
