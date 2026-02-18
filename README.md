# ShotPlot

A shot group analysis application for rifle shooters. Analyze shooting accuracy by photographing targets and marking shot locations to calculate various statistical measures of group size and performance.

## Features

- **Image Processing**: Load target photographs (JPEG, PNG, BMP)
- **Calibration**: User-defined scale reference (pixels to inches conversion)
- **Shot Analysis**: Mark shot impact locations and calculate group size statistics
- **Key Metrics**:
  - Full Group Size (100%) - smallest enclosing circle
  - 80% and 90% Group Sizes
  - Mean Radius and Standard Deviation
- **Data Management**: Save/load sessions in .spz format (ZIP-based)
- **Export**: Annotated images with statistics overlay

## Building

### Prerequisites

- CMake 3.16 or later
- C++17 compatible compiler
- Qt6 (Core, Widgets, Gui)
- libzip

### Ubuntu/Debian

```bash
# Install dependencies
sudo apt-get install qt6-base-dev qt6-tools-dev qt6-svg-dev libzip-dev cmake build-essential

# Build
mkdir build && cd build
cmake ..
cmake --build .

# Run
./ShotPlot
```

### Windows and MacOS
Instructions coming...later.

## Usage

1. **Import** a target photograph (File → Import)
2. **Set Caliber** - Select your bullet diameter
3. **Calibrate Scale** - Click two points of a known distance
4. **Mark Point of Aim** (optional) - Click your intended impact point
5. **Mark Impacts** - Click each shot hole
6. **View Statistics** - See group sizes in inches, MOA, or milliradians
7. **Save/Export** - Save session or export annotated image

## License

MIT License - see LICENSE file for details.
