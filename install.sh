#!/bin/bash

# Photo Compare Installation Script
# This script builds and installs Photo Compare on Linux/FreeBSD systems

set -e

echo "Photo Compare Installation Script"
echo "================================="

# Check if we're on a supported system
if [[ "$OSTYPE" != "linux-gnu"* ]] && [[ "$OSTYPE" != "freebsd"* ]]; then
    echo "Warning: This script is designed for Linux and FreeBSD systems."
    echo "Your system type: $OSTYPE"
    read -p "Continue anyway? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Check for required dependencies
echo "Checking dependencies..."

# Check for Qt6
if ! pkg-config --exists Qt6Core Qt6Widgets Qt6Gui; then
    echo "Error: Qt6 development libraries not found."
    echo "Please install Qt6 development packages:"
    echo "  Ubuntu/Debian: sudo apt install qt6-base-dev"
    echo "  Fedora: sudo dnf install qt6-qtbase-devel"
    echo "  Arch: sudo pacman -S qt6-base"
    echo "  FreeBSD: sudo pkg install qt6-base"
    exit 1
fi

# Check for CMake
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake not found."
    echo "Please install CMake:"
    echo "  Ubuntu/Debian: sudo apt install cmake"
    echo "  Fedora: sudo dnf install cmake"
    echo "  Arch: sudo pacman -S cmake"
    echo "  FreeBSD: sudo pkg install cmake"
    exit 1
fi

# Check for make
if ! command -v make &> /dev/null; then
    echo "Error: make not found."
    echo "Please install build tools."
    exit 1
fi

echo "All dependencies found!"

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring build..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building PhotoCompare..."
make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Install (requires sudo for system-wide installation)
echo "Installing PhotoCompare..."
if [[ $EUID -eq 0 ]]; then
    make install
else
    echo "Installing to system directories requires root privileges."
    sudo make install
fi

# Update desktop database
echo "Updating desktop database..."
if command -v update-desktop-database &> /dev/null; then
    if [[ $EUID -eq 0 ]]; then
        update-desktop-database /usr/local/share/applications
    else
        sudo update-desktop-database /usr/local/share/applications
    fi
fi

# Update icon cache
echo "Updating icon cache..."
if command -v gtk-update-icon-cache &> /dev/null; then
    if [[ $EUID -eq 0 ]]; then
        gtk-update-icon-cache -f -t /usr/local/share/icons/hicolor
    else
        sudo gtk-update-icon-cache -f -t /usr/local/share/icons/hicolor
    fi
fi

echo ""
echo "Installation completed successfully!"
echo ""
echo "You can now:"
echo "1. Run PhotoCompare from the command line: PhotoCompare"
echo "2. Find it in your desktop environment's application menu under Graphics"
echo "3. Right-click on image files to open them with PhotoCompare"
echo ""
echo "To uninstall, run: sudo make uninstall (from the build directory)"
