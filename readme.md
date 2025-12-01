## queueVisualization

a simple visualizer for the "Queue" data structure
written in c++ and using [Dear imgui](https://github.com/ocornut/imgui)
## building
building only been tested on Arch Linux and on windows using mingw. In the future build instructions for MSVC will be added.
no plans on macOS testing so YMMV.

Build instructions for arch linux:

    sudo pacman -S --needed git base-devel
    git clone git@github.com:mahyar1w/queueVisualization.git
    cd queueVisualization
    mkdir -p build
    cd build
    cmake ..
    make
The executable should be in build/release.
## screenshoots

![](/home/mahyar/Pictures/Screenshots/Screenshot From 2025-12-01 18-11-29.png)

![](/home/mahyar/Pictures/Screenshots/Screenshot From 2025-12-01 18-11-38.png)

![](/home/mahyar/Pictures/Screenshots/Screenshot From 2025-12-01 18-11-20.png)
