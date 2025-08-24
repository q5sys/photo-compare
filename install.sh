#!/bin/bash

# Photo Compare Desktop File Installation Script
# This script installs the .desktop file for Photo Compare application

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${GREEN}Photo Compare Desktop File Installer${NC}"
echo "======================================"

# Check if PhotoCompare executable exists
if [ ! -f "./build/PhotoCompare" ]; then
    echo -e "${YELLOW}Warning: PhotoCompare executable not found in build directory.${NC}"
    echo "Make sure to build the application first using: cd build && cmake .. && make"
    echo ""
fi

# Check if .desktop file exists
if [ ! -f "./PhotoCompare.desktop" ]; then
    echo -e "${RED}Error: PhotoCompare.desktop file not found!${NC}"
    exit 1
fi

# Check if icon exists
if [ ! -f "./icons/PhotoCompare-256.png" ]; then
    echo -e "${RED}Error: PhotoCompare icon not found!${NC}"
    exit 1
fi

# Create directories if they don't exist
mkdir -p ~/.local/share/applications
mkdir -p ~/.local/share/pixmaps
mkdir -p ~/.local/bin

# Copy executable to local bin (if it exists)
if [ -f "./build/PhotoCompare" ]; then
    echo "Installing PhotoCompare executable to ~/.local/bin/"
    cp ./build/PhotoCompare ~/.local/bin/
    chmod +x ~/.local/bin/PhotoCompare
    echo -e "${GREEN}✓ Executable installed${NC}"
else
    echo -e "${YELLOW}⚠ Skipping executable installation (not found)${NC}"
fi

# Install icon
echo "Installing icon to ~/.local/share/pixmaps/"
cp ./icons/PhotoCompare-256.png ~/.local/share/pixmaps/PhotoCompare.png
echo -e "${GREEN}✓ Icon installed${NC}"

# Install desktop file
echo "Installing desktop file to ~/.local/share/applications/"
cp ./PhotoCompare.desktop ~/.local/share/applications/
chmod +x ~/.local/share/applications/PhotoCompare.desktop

# Update desktop database
if command -v update-desktop-database >/dev/null 2>&1; then
    echo "Updating desktop database..."
    update-desktop-database ~/.local/share/applications/
    echo -e "${GREEN}✓ Desktop database updated${NC}"
fi

echo ""
echo -e "${GREEN}Installation completed!${NC}"
echo ""
echo "The Photo Compare application should now be available in your desktop environment's"
echo "application menu under Graphics/Photography categories with its custom icon."
echo ""
echo "If you don't see it immediately, try:"
echo "1. Logging out and back in"
echo "2. Restarting your desktop environment"
echo "3. Running: update-desktop-database ~/.local/share/applications/"
echo ""
echo "To uninstall, remove these files:"
echo "  ~/.local/share/applications/PhotoCompare.desktop"
echo "  ~/.local/share/pixmaps/PhotoCompare.png"
echo "  ~/.local/bin/PhotoCompare"
