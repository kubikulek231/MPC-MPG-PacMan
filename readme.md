# MPG-MPC-PacMan

**Jakub Lepik | 239122**

A simple 3D Pac-Man-style game built fully from scratch in modern C++ and raw OpenGL 💻🔥. The game uses the [stb_image.h](https://github.com/nothings/stb) library for image loading 📸, [SDL3_mixer](https://wiki.libsdl.org/SDL3_mixer/FrontPage) for basic sound effects 🔊, and a custom font rendering library, [glfreetype](https://github.com/benhj/glfreetype), which I modified to render FreeType fonts in 3D 🎨.

This repository provides the ability to play the game right away on Windows by downloading a compiled binary or the source code from the [latest release](https://github.com/kubikulek231/MPC-MPG-PacMan/releases/tag/v1.0.0), or you can follow the [Build Instructions](#⚙️-build-instructions) to build the game from source! 😎

![PacMan3D](readme_assets/screenshot3.png)

## 💥 Features:
- 🧱 **Tile-based world**  
  - Smooth, rounded corners for cleaner-looking walls 😎
- 👻 **Basic ghost AI**: Random movement with an A* algorithm for pathfinding
- 🔊 **Simple sounds** powered by SDL3_mixer
- 🎮 **Custom game loop**, comprehensive input handling & game logic
- 📷 **Dynamic camera system** with options to switch to:
  - Free movement
  - Chasing player
  - Full animated board overview
- 📜 **Simple menu UI**: Allows pausing and game mode selection (Sandbox or Normal)
- 🌍 **World sphere texture** based on image taken by [ESA Euclid satellite](https://euclid.caltech.edu/image/euclid20250319k-euclid-deep-field-fornax-preview)

## 🚀 Release
If you're using a Windows machine, you can try out PacMan3D right away! The latest release version is available in the [releases section](https://github.com/kubikulek231/MPC-MPG-PacMan/releases/tag/v1.0.0) of the repository. You can download either the compiled binaries or the source code it was built on. Follow the instructions in the [Build Instructions](#⚙️-build-instructions) section to build the game from source.

### ▶️ To run the game:
1. Download the binary or source from the [releases section](https://github.com/kubikulek231/MPC-MPG-PacMan/releases/tag/v1.0.0).
2. If you downloaded the binary, simply extract the ZIP file 📂.
3. Inside the extracted folder, run the `MPG-PacMan.exe` to start the game! 🎮😎

## 🎮 Controls:
- ⬆️ **W**: Move up
- ⬅️ **A**: Move left
- ⬇️ **S**: Move down
- ➡️ **D**: Move right
- 🎥 **C**: Change camera mode (free movement, chasing player, or overview)
- 🖱️💫 **Mouse Movement**: Navigate the menu and interact with the game
- 🖱️✨ **Mouse Click**: Select menu options
- 🖱️🔄 **Left Mouse Click + Drag**: Adjust camera position
- 🖱️🔁 **Right Mouse Click + Drag**: Adjust camera rotation
- 🖱️🎯 **Mouse Wheel**: Zoom in and out

## 🏗️ Build Instructions

The recommended way to build the project is by using Visual Studio on a Windows machine:
> The build process has been simplified using custom CMake configurations and LFS. All build and runtime dependencies, including core assets like fonts, textures, and libraries such as [NupenGL](https://www.nuget.org/packages/nupengl.core) and [SDL3_mixer](https://wiki.libsdl.org/SDL3_mixer/FrontPage), are stored in LFS. By simply cloning the repository, these assets will automatically be downloaded, extracted, and linked during the CMake configuration, making the process seamless with minimal setup required. 🚀

1. **Clone the repository:**
    - Either download the source from [releases section](https://github.com/kubikulek231/MPC-MPG-PacMan/releases/tag/v1.0.0) or get the latest source by cloning this repository:
        ```bash
        git clone https://github.com/kubikulek231/MPG-MPC-PacMan.git 
        ```
2. **Open the project in Visual Studio**:
    - Open Visual Studio and use the "Open Local Folder" option to open the cloned repository directory.
    - Visual Studio will automatically detect the CMake configuration and generate the necessary build files using Ninja 🛠️.
3. **Choose the configuration and build**:
    - Once CMake is done, select your build configuration (e.g., Debug or Release) in Visual Studio.
    - Click Build to compile the game.
4. **Run the game**:
    - After the build is complete, run the game directly from Visual Studio or locate the executable in the output directory.

## 📂 Repository Structure:
- [`assets`](assets) - Contains all the external resources used in the game.
  - Fonts, maps, sounds, and textures are included in this directory.
- [`cmake`](cmake) - CMake configuration files to build the project.
- [`deps`](deps) - Dependencies and external libraries required for the project.
  - Includes `SDL3_mixer`, `freetype`, and `stb_image.h`.
- [`include`](include) - Header files defining the project's interface.
- [`lib`](lib) - Third-party libraries used in the project, including `glft2` and `stb`.
- [`readme_assets`](readme_assets) - Assets used specifically in the README, such as images or diagrams.
- [`resources`](resources) - Miscellaneous resources used in the game, like shaders or scripts.
- [`src`](src) - Main source code for the game, including game logic, rendering, and more.
- [`texture_enhance`](texture_enhance) - Utilities for enhancing and manipulating textures.