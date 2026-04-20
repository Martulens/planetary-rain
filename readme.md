# Planetary Rain

A procedural planet generator and editor written in C++17 with OpenGL 3.3. Planets have noise-based terrain, spherical Gerstner ocean waves, dynamic cubemap reflections, and refractive/reflective materials, all tweakable live through an ImGui panel.

<p align="center">
  <img src="screenshots/planet1.png" alt="Planet 1" width="350"/>
  <img src="screenshots/planet2.png" alt="Planet 2" width="350"/>
</p>

<p align="center">
  <img src="screenshots/planet3.png" alt="Planet 3" width="350"/>
  <img src="screenshots/planet4.png" alt="Metal planet" width="350"/>
</p>

## Features

### Terrain
Simplex noise on the GPU, layered with per-layer octaves, frequency, amplitude, persistence and roughness. Three noise modes: normal, turbulent, ridged. Coloring is height-based, with automatic land/ocean split at a configurable ocean level.

### Spherical Gerstner waves
Ocean waves computed directly on the sphere using the model from [Michelic 2018](https://cescg.org/wp-content/uploads/2018/04/Michelic-Real-Time-Rendering-of-Procedurally-Generated-Planets-2.pdf). Analytical normals (no finite differences), geodesic distance for wavelength scaling, and smoothstep fading near wave origins and antipodes to avoid displacement loops. Up to 10 layers with independent frequency, speed, steepness and origin.

### Cube-sphere geometry
The mesh is a subdivided cube projected to a sphere using the [analytic mapping](https://mathproofs.blogspot.com/2005/07/mapping-cube-to-sphere.html). Subdivision level is 1 to 500 per face.

### Rendering
- Blinn-Phong shading with point and directional lights
- Schlick Fresnel reflections for water and metals
- Refractive shader with IOR control and total internal reflection
- Per-object dynamic environment cubemaps, rendered each frame
- Cubemap skybox with five texture sets
- Exponential distance fog

### Live editor
ImGui panel for geometry (radius, detail, position), material (diffuse, specular, shininess, reflectivity, IOR, transparency), noise layers, wave parameters, and rotation. A randomizer button picks one of four archetypes: terrestrial, earth-like, desert, ocean.

## Getting started

### Prerequisites

- CMake 3.16+
- C++17 compiler (GCC 9+, Clang 10+, MSVC 2019+)
- OpenGL 3.3+ GPU and drivers
- Linux only: system dev packages

```bash
# Ubuntu / Debian
sudo apt install build-essential cmake freeglut3-dev libxi-dev libxrandr-dev

# Fedora
sudo dnf install gcc-c++ cmake freeglut-devel libXi-devel libXrandr-devel

# macOS and Windows fetch all deps automatically
```

### Build

```bash
cd src
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

The first build takes a few minutes because CMake fetches all the dependencies via `FetchContent`.

### Run

```bash
./PlanetaryRain
```

The `textures/` and `shaders/` folders need to be in the working directory. The build copies them into `build/` automatically.

## Controls

| Key | Action |
|-----|--------|
| `Q` | Toggle between UI mode (mouse controls editor) and FPS mode (mouse controls camera) |
| `W` `A` `S` `D` | Move camera forward / left / backward / right |
| `Space` | Move camera up |
| `Scroll wheel` | Zoom (adjusts field of view) |
| `F` | Toggle wireframe rendering |
| `R` | Restart scene with a new random skybox |
| `ESC` | Quit |

## Dependencies

All fetched by CMake, nothing to install manually (except system GL/GLUT on Linux).

| Library | Version | Purpose |
|---------|---------|---------|
| [GLM](https://github.com/g-truc/glm) | 1.0.1 | Vector and matrix math |
| [GLEW](https://github.com/nigels-com/glew) | 2.2.0 | OpenGL extension loading |
| [FreeGLUT](https://github.com/FreeGLUTProject/freeglut) | 3.2.2 | Windowing, input, and context creation |
| [Assimp](https://github.com/assimp/assimp) | 5.3.1 | 3D model importing |
| [stb_image](https://github.com/nothings/stb) | latest | Image loading (textures, skybox faces) |
| [Dear ImGui](https://github.com/ocornut/imgui) | 1.90.1 | Immediate-mode GUI |

## Project structure

```
src/
├── main.cpp                 # Entry point, GLUT initialization
├── game.cpp/h               # Top-level game state and restart logic
├── gamestate.h              # Input state, window dimensions, flags
├── scene.cpp/h              # Scene graph: objects, lights, env map passes
├── sphere.cpp/h             # Cube-sphere mesh generation, planet parameter updates
├── object.cpp/h             # Base renderable object (abstract)
├── draw.cpp/h               # Rendering pipeline: uniform setup, draw calls
├── camera.cpp/h             # FPS camera with pitch/yaw and projection
├── ui.cpp/h                 # ImGui editor, planet randomizer, wave randomizer
├── noise.cpp/h              # Noise layer settings (evaluated in vertex shader)
├── shaderProgram.cpp/h      # Shader compilation, linking, uniform locations
├── skybox.cpp/h             # Cubemap skybox loading and rendering
├── envMap.cpp/h             # Dynamic per-object environment cubemap
├── modelTexture.cpp/h       # Material properties and texture loading
├── meshGeometry.cpp/h       # VAO/VBO management, Assimp model loading
├── framework.cpp/h          # OpenGL initialization, shader/texture utilities
├── callbacks.cpp/h          # GLUT input callbacks
├── config.h                 # Compile-time constants
├── variables.cpp/h          # Global singletons (Game, UI)
├── utilities.cpp/h          # FPS counter
│
├── shaders/
│   ├── planet.vert          # Terrain displacement + spherical Gerstner waves
│   ├── planet.frag          # Terrain coloring, Blinn-Phong, water rendering
│   ├── refractive.frag      # Refraction with Fresnel and environment sampling
│   ├── skybox.vert/frag     # Cubemap skybox
│   ├── template.vert/frag   # Generic object shaders
│   └── shadow.vert/frag     # Shadow map (placeholder)
│
└── textures/
    └── skybox/
        ├── 0/ ... 4/        # Five skybox texture sets (right/left/top/bottom/front/back.png)
```

## How it works

### Terrain

The vertex shader displaces sphere vertices radially based on stacked noise layers. Each layer has independent parameters:

```
elevation = Σ noise_i(position × frequency_i) × amplitude_i
```

Noise is 3D simplex noise with configurable octaves, persistence and roughness. Vertices below the ocean level are clamped and shaded as water in a separate path.

### Ocean waves

Water vertices are displaced with spherical Gerstner waves. Unlike planar Gerstner waves these work on the sphere surface using geodesic distance from each wave origin. The displacement follows equations 7 and 8 in [Michelic 2018](https://cescg.org/wp-content/uploads/2018/04/Michelic-Real-Time-Rendering-of-Procedurally-Generated-Planets-2.pdf), with analytical normals from equation 10 so no finite-difference normal computation is needed. Steepness is faded near wave origins and antipodes (equation 11) to avoid degenerate geometry.

### Environment mapping

Objects that ask for environment mapping get a cubemap rebuilt every frame from their center. The scene is rendered (without the object itself) into six framebuffer faces, giving reflections of the skybox and other objects in real time. The cubemap is sampled in the fragment shader alongside the static skybox cubemap, blended with Fresnel.

## License

MIT. See [LICENSE](LICENSE).

## References

- Spherical Gerstner waves: [Michelic, *Real-Time Rendering of Procedurally Generated Planets*, CESCG 2018](https://cescg.org/wp-content/uploads/2018/04/Michelic-Real-Time-Rendering-of-Procedurally-Generated-Planets-2.pdf)
- Cube-to-sphere mapping: [Philip Nowell, *Mapping a Cube to a Sphere*](https://mathproofs.blogspot.com/2005/07/mapping-cube-to-sphere.html)
- Simplex noise GLSL adapted from [Ashima Arts](https://github.com/ashima/webgl-noise)
- Gerstner waves: [Catlike Coding waves tutorial](https://catlikecoding.com/unity/tutorials/flow/waves/), [GameIdea ocean shader](https://gameidea.org/2023/12/01/3d-ocean-shader-using-gerstner-waves/)
