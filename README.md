# Photo Compare - Qt/C++ Image Comparison Tool

A Qt/C++ GUI application that allows users to compare two photos with an interactive mouse-over reveal effect.

## Features

- **Image Selection**: Select two images using file dialogs
- **Direction Control**: Choose between "Left to Right"/"Right to Left" or "Top to Bottom"/"Bottom to Top" comparison modes
- **Interactive Reveal**: Mouse over the images to reveal the second image in the selected direction
- **Smooth Scaling**: Images are automatically scaled to fit while maintaining aspect ratio

## Requirements

- Qt6 (Core and Widgets modules)
- CMake 3.16 or higher
- C++17 compatible compiler
- Linux/Unix system (tested on Linux, but should work on the BSDs)

## Installation

- use cmake

## Usage

1. Run the application:
2. Click "Select First Image" to choose your base image
3. Click "Select Second Image" to choose the comparison image
4. Select your preferred comparison direction:
   - **Left to Right**: Move mouse horizontally from left to reveal the second image
   - **Right to Left**: Move mouse horizontally from right to reveal the second image
   - **Top to Bottom**: Move mouse vertically from top to reveal the second image
   - **Bottom to Top**: Move mouse vertically from bottom to reveal the second image
5. Move your mouse over the image area to see the comparison effect

## Supported Image Formats

- PNG (.png)
- JPEG (.jpg, .jpeg)
- BMP (.bmp)
- GIF (.gif)
- TIFF (.tiff)

## License

MIT
