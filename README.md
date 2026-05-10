# Win32 GDI Graphics App (Linux Cross-Compiled)

This project is a Windows-based graphical application developed in C++ using the Win32 API and GDI. It features custom implementations of **Bresenham's Line Algorithm** and a **Parallel Non-Recursive Flood Fill** using **OpenMP**.

## 🛠 Prerequisites for Rocky Linux

To compile and run this on Linux, you need the MinGW cross-compiler and a Windows compatibility layer.

### 1. Install the Cross-Compiler
Bash:
sudo dnf install mingw64-gcc-c++
2. Install a Runner (Bottles/Wine)
Since native Wine can be difficult to find on Rocky 10 repos, we use Bottles (Flatpak):

Bash
flatpak install flathub com.usebottles.bottles
🚀 How to Build
We use Static Linking to ensure all C++ and OpenMP libraries are "baked" into the .exe. This prevents "DLL not found" errors when running through Wine.

Run this command in your terminal:

Bash
x86_64-w64-mingw32-g++ graphics_template.cpp -o drawing_app.exe \
    -lgdi32 -mwindows -fopenmp -static
Flags Explained:

-lgdi32: Links the Windows Graphics Device Interface.

-mwindows: Ensures the app launches as a GUI (no console window).

-fopenmp: Enables multi-threading support.

-static: Bakes all dependencies (libstdc++, libgcc, pthreads) into the EXE.

🏃 How to Run
Because the app is in a Linux directory, you must give the Bottles sandbox permission to see your files. Use the following command:

Bash
flatpak run --filesystem=home --command=wine com.usebottles.bottles "$PWD/drawing_app.exe"
🎮 Usage Instructions
Launch the application.

Define Boundary: Click the Left Mouse Button 5 times on the screen to draw a red polygon.

Fill Area: The 6th click will trigger the Flood Fill. The algorithm will fill the area starting from that point until it hits the red boundary.

Parallelism: The fill uses OpenMP tasks to process pixel neighbors across multiple CPU cores.
