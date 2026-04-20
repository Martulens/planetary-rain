# Planetary Rain

A procedural planet generator and editor written in C++17 with OpenGL 3.3. Planets have noise-based terrain, spherical Gerstner ocean waves, volumetric cloud shells, dynamic cubemap reflections, and refractive/reflective materials, all tweakable live through an ImGui panel.

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
GPU simplex noise, stacked per-layer with octaves, frequency, amplitude, persistence, roughness and offset. Three noise modes (normal, turbulent, ridged). Coloring uses three user-chosen stops (low / mid / high) with smoothstep-eased transitions, plus an automatic land/ocean split at a configurable ocean level.

### Spherical Gerstner waves
Ocean waves computed directly on the sphere using the model from [Michelic 2018](https://cescg.org/wp-content/uploads/2018/04/Michelic-Real-Time-Rendering-of-Procedurally-Generated-Planets-2.pdf). Analytical normals (no finite differences), geodesic distance for wavelength scaling, and smoothstep fading near wave origins and antipodes to avoid displacement loops. Up to 10 layers with independent frequency, speed, steepness and origin.

### Volumetric clouds
Ray-marched cloud shell rendered around the planet. 3D simplex noise (3 octaves of fBm, time-drifted) accumulates density along view rays, lit with a cheap radial-vs-sun term and composited with Beer-Lambert transmittance. Adaptive step count scales cost with shell thickness. Optional latitudinal band modulation gives gas giants a layered look. The shell auto-lifts over the terrain bulge so it stays above mountains regardless of amplitude.

### Cube-sphere geometry
The mesh is a subdivided cube projected to a sphere using the [analytic mapping](https://mathproofs.blogspot.com/2005/07/mapping-cube-to-sphere.html). Subdivision level is 1 to 500 per face.

### Rendering
- Blinn-Phong shading with point and directional lights
- Schlick Fresnel reflections unified for both water and land (reflectivity slider feeds the same path)
- Refractive path (IOR, transparency, total internal reflection) merged into the planet shader
- Per-object dynamic environment cubemaps, rebuilt each frame
- Cubemap skybox with five texture sets
- Exponential distance fog

### Live editor
ImGui panel for geometry, material (low / mid / high colors plus diffuse, specular, shininess, reflectivity, IOR, transparency), noise layers, waves, clouds and rotation. Seven one-click presets: Earthlike, Terrestrial, Icy, Barren, Volcanic, Gas Giant, Ocean. Each preset randomizes colors, material, noise and cloud parameters within a themed range, so repeated clicks give variations. Dedicated Randomize Waves and Randomize Clouds buttons for targeted re-rolls.

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
├── scene.cpp/h              # Scene graph: objects, lights, env map passes, planet + cloud shader
├── sphere.cpp/h             # Cube-sphere mesh generation, planet parameter updates
├── object.cpp/h             # Base renderable object (abstract)
├── draw.cpp/h               # Rendering pipeline: uniform setup, draw calls, cloud pass
├── camera.cpp/h             # FPS camera with pitch/yaw and projection
├── ui.cpp/h                 # ImGui editor, presets, waves and clouds randomizers
├── noise.h                  # Noise layer settings (evaluated in vertex shader)
├── shaderProgram.cpp/h      # Shader compilation, linking, uniform locations
├── skybox.cpp/h             # Cubemap skybox loading and rendering
├── envMap.cpp/h             # Dynamic per-object environment cubemap
├── modelTexture.cpp/h       # Material properties (3 colors, PBR-ish terms, IOR)
├── meshGeometry.cpp/h       # VAO/VBO management, Assimp model loading
├── framework.cpp/h          # OpenGL initialization, shader/texture utilities
├── callbacks.cpp/h          # GLUT input callbacks
├── config.h                 # Compile-time constants (cube vertices, caps)
├── variables.cpp/h          # Global singletons (Game, UI)
├── utilities.cpp/h          # FPS counter
│
├── shaders/
│   ├── planet.vert          # Terrain displacement + spherical Gerstner waves
│   ├── planet.frag          # Terrain coloring, Blinn-Phong, reflection + refraction in one shader
│   ├── clouds.vert          # Cloud shell vertex pass (unit sphere scaled to cloud radius)
│   ├── clouds.frag          # Volumetric ray-march, density, lighting, atmospheric alpha
│   ├── skybox.vert/frag     # Cubemap skybox
│   └── template.vert/frag   # Generic object shaders
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

Fragment color uses a four-stop palette derived from the three user-chosen colors: `colorLow → baseColor → mix(baseColor, colorHigh, 0.5) → colorHigh`, sampled across three equal elevation bands with smoothstep easing so the segment joins are not visible.

### Ocean waves

Water vertices are displaced with spherical Gerstner waves. Unlike planar Gerstner waves these work on the sphere surface using geodesic distance from each wave origin. The displacement follows equations 7 and 8 in [Michelic 2018](https://cescg.org/wp-content/uploads/2018/04/Michelic-Real-Time-Rendering-of-Procedurally-Generated-Planets-2.pdf), with analytical normals from equation 10 so no finite-difference normal computation is needed. Steepness is faded near wave origins and antipodes (equation 11) to avoid degenerate geometry.

### Clouds

A second sphere rendered at `planetRadius * (1 + surfaceElevation) + altitude`, sharing the planet's model matrix so the shell stays concentric. For each visible fragment, the shader:

1. Constructs a view ray from the camera through the fragment and solves for the inner/outer sphere intersections analytically.
2. Steps along the ray inside the shell (4 to 16 samples, adaptive to shell thickness).
3. At each step, samples 3 octaves of 3D simplex noise, thresholds it by coverage, and optionally multiplies by a `sin(y * bandFreq)` term for gas-giant bands.
4. Accumulates light using Beer-Lambert transmittance (`transmittance *= exp(-density * stepSize)`), weighted by a cheap radial-vs-sun dot product.
5. Outputs `color = light_accumulated`, `alpha = smoothed(1 - transmittance)` capped at 0.80 so the planet always shows through.

Noise is sampled in world space, which means the noise field drifts over the surface as the planet spins, reading as atmospheric wind motion.

### Environment mapping

Objects that ask for environment mapping get a cubemap rebuilt every frame from their center. The scene is rendered (without the object itself) into six framebuffer faces, giving reflections of the skybox and other objects in real time. The cubemap is sampled in the fragment shader alongside the static skybox cubemap, blended with Fresnel.

### Reflection and refraction (single shader)

`planet.frag` handles both opaque and transparent materials in one pass. Reflectivity drives an environment reflection blend on the lit surface (water gets a small floor so it still shines with low slider values). When `transparency > 0 && ior > 1`, an additional refractive path samples refraction through the surface, Fresnel-mixes it against the reflection, and composites it over the opaque lighting weighted by `transparency`.

## License

MIT. See [LICENSE](LICENSE).

## References

- Spherical Gerstner waves: [Michelic, *Real-Time Rendering of Procedurally Generated Planets*, CESCG 2018](https://cescg.org/wp-content/uploads/2018/04/Michelic-Real-Time-Rendering-of-Procedurally-Generated-Planets-2.pdf)
- Cube-to-sphere mapping: [Philip Nowell, *Mapping a Cube to a Sphere*](https://mathproofs.blogspot.com/2005/07/mapping-cube-to-sphere.html)
- Simplex noise GLSL adapted from [Ashima Arts](https://github.com/ashima/webgl-noise)
- Gerstner waves: [Catlike Coding waves tutorial](https://catlikecoding.com/unity/tutorials/flow/waves/), [GameIdea ocean shader](https://gameidea.org/2023/12/01/3d-ocean-shader-using-gerstner-waves/)
