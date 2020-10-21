# LiquidSimulation
Real-time fluid simulation parallelized on a GPU with OpenCL.
[Video of bacherlor's thesis](https://www.youtube.com/watch?v=2ggfZliqLjA)

![dambreak](/showcase/dambreak.gif)
![wavebreaker](/showcase/wavebreaker.gif)
![waterfall](/showcase/waterfall.gif)

**Features:**
- SPH (Smoothed Particle Hydrodynamics) method
- PCISPH (Predictive-Corrective Incompressible SPH) method
- Single-phase fluid represented with particles
- Static collision objects (boundary and obstacle)
- Sequential CPU and parallel CPU and GPU implementation
- Different scenarios

## Build Instructions
For Visual Studio 2019:
```
git clone https://github.com/GitDaroth/LiquidSimulation
cd LiquidSimulation
cmake_generate_VS2019.bat
```
Open the generated Visual Studio solution in the "build" folder and build the "LiquidSimulation" target.

Don't forget to copy the needed dll files from your Qt5 installation next to your executable:
```
QT5_INSTALLATION_PATH/bin/Qt5Core.dll
QT5_INSTALLATION_PATH/bin/Qt5Gui.dll
QT5_INSTALLATION_PATH/bin/Qt5Widgets.dll
QT5_INSTALLATION_PATH/plugins/platforms/qwindows.dll
```

## Dependencies
**LiquidPhysics:**
- [OpenCL 1.2 for NVIDIA, AMD or Intel](https://www.khronos.org/opencl/)

**LiquidSimulation:**
- LiquidPhysics
- [Qt5 (Modules: Core, Gui, Widgets)](https://www.qt.io)