# Win32 GDI Graphics App (Linux Cross-Compiled)

This project is a Windows-based graphical application developed in C++ using the Win32 API and GDI. It features custom implementations of **Bresenham's Line Algorithm** and a **Parallel Non-Recursive Flood Fill** using OpenMP.

## Features

- **Manual Polygon Drawing**: Left-click 5 times to define a boundary.
- **Parallel Flood Fill**: The 6th click acts as a seed to fill the polygon using multi-threaded logic.
- **OpenMP Integration**: Uses task-based parallelism to accelerate pixel processing.
- **Static Linking**: Compiled to be a self-contained executable with no external DLL dependencies.

## Prerequisites for Linux (Rocky Linux / RHEL)

To compile and run this on Linux, you need the MinGW cross-compiler and a Windows compatibility layer.

### 1) Install the Cross-Compiler

```bash
sudo dnf install mingw64-gcc-c++
```

### 2) Install a Runner (Bottles/Wine)

Since native Wine can be difficult to find on Rocky 10 repos, use Bottles via Flatpak:

```bash
flatpak install flathub com.usebottles.bottles
```

## How to Build

The build uses **static linking** so C++ and OpenMP runtime libraries are baked into the `.exe`, helping avoid `"DLL not found"` errors in Wine.

Run this command in the project directory:

```bash
x86_64-w64-mingw32-g++ graphics_template.cpp -o drawing_app.exe \
    -lgdi32 -mwindows -fopenmp -static
```

### Flags Explained

- `-lgdi32`: Links the Windows Graphics Device Interface.
- `-mwindows`: Launches the app as a GUI (no console window).
- `-fopenmp`: Enables multi-threading support.
- `-static`: Bakes dependencies (libstdc++, libgcc, pthreads) into the EXE.

## How to Run

Because the app is in a Linux directory, grant Bottles access to your home filesystem:

```bash
flatpak run --filesystem=home --command=wine com.usebottles.bottles "$PWD/drawing_app.exe"
```

## Usage Instructions

1. Launch the application.
2. **Define Boundary**: Click the Left Mouse Button 5 times to draw a red polygon.
3. **Fill Area**: The 6th click triggers Flood Fill.
4. The algorithm fills from the seed point until it reaches the red boundary.

## OpenMP Note

The fill may appear very fast and handle complex regions efficiently because it uses a multi-threaded tasking approach.
