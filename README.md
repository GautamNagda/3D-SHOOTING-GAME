# CyberStrike 3D – A Real-Time 3D Shooting Game Using Computer Graphics

**Academic Course Project** | **5th-Semester B.Tech Computer Science & Engineering**  
**Technology Stack:** Modern C++ (C++14/17), OpenGL 3.3 Core Profile, GLFW 3.3, GLAD, GLM, stb_image, GLSL Shaders, CMake  

---

## 1. Project Title & Overview
**CyberStrike 3D** is a real-time single-player 3D first-person shooter developed using modern C++ and the OpenGL 3.3 Core Profile graphics pipeline. Built specifically to demonstrate core Computer Graphics principles from scratch without relying on pre-built commercial engines (such as Unity or Unreal), the project provides complete architectural transparency, mathematical rigor, and viva-ready conceptual clarity.

---

## 2. Main Objective
To create a fully functional single-player 3D shooting game in which the player navigates a 3D sci-fi combat arena, controls a first-person perspective camera with mouse look and WASD movement, aims and shoots at autonomous enemy drones using ray-casting hit detection, experiences real-time Blinn-Phong lighting and 2D texture mapping, survives escalating enemy waves, tracks score and ammunition on a 2D orthographic HUD, and manages game state transitions (Start Menu, Active Combat, Pause, Game Over, and Victory).

---

## 3. Technology Stack & Dependencies

| Component | Library / Specification | Purpose in Project |
| :--- | :--- | :--- |
| **Language** | Modern C++ (C++14 / C++17) | Core engine logic, OOP architecture, physics, and state management |
| **Graphics API** | OpenGL 3.3 (Core Profile) | Programmable GPU rendering pipeline (VAO, VBO, Shaders, Z-Buffer) |
| **Window & Input** | GLFW 3.3.8 | Cross-platform window creation, mouse locking, and keyboard event polling |
| **Function Loader** | GLAD | Dynamic runtime loading of GPU driver OpenGL function pointers |
| **Mathematics** | GLM 0.9.9.8 | 3D vectors, affine matrices, camera `lookAt`, and `perspective` projection |
| **Texture Loading**| stb_image | Image decoding for 2D diffuse texture maps (PNG format) |
| **Build System** | CMake 4.4 + MinGW GCC | Cross-platform compilation and dependency orchestration |

---

## 4. Key Gameplay Features

1. **3D Combat Arena**:
   - Metallic sci-fi floor with circuit grid styling ($26\text{m} \times 26\text{m}$).
   - Enclosing perimeter armor walls keeping the player and enemies inside the battlefield.
   - Four corner cyber-energy towers, tactical cover crates, defensive barricades, and a central bunker.
2. **First-Person Player Controller**:
   - Smooth 360° mouse-look powered by Euler angles (Yaw & Pitch) with Gimbal Lock prevention.
   - Delta-time ($\Delta t$) normalized horizontal walking (W, A, S, D) and flying (Space / Shift).
   - Dynamic perspective zooming with mouse scroll wheel ($30^\circ$ to $90^\circ$ FOV).
3. **Firearm & Weapon Mechanics**:
   - 3D viewmodel gun rendered directly in front of the camera with procedural walking bobbing.
   - Realistic kickback recoil animation upon firing with spring recovery.
   - Ammunition clip system (30 rounds per magazine, 90 reserve rounds, reload cooldown).
4. **Ray-Casting Shooting System**:
   - Fires a 3D ray directly from the camera eye through the screen center crosshair.
   - Uses the **Kay-Kajiya slab algorithm** to calculate exact intersection distances against enemy and obstacle AABBs.
5. **Autonomous Enemy Drones & FSM AI**:
   - Procedurally animated floating drones with glowing crimson sensor eyes and flight thrusters.
   - Finite State Machine AI: `IDLE` $\rightarrow$ `CHASE` $\rightarrow$ `ATTACK` $\rightarrow$ `TAKE DAMAGE` $\rightarrow$ `DEAD`.
   - Navigates around arena obstacles and deals melee/proximity damage to the player.
6. **Escalating Wave System**:
   - **Wave 1**: 5 combat drones.
   - **Wave 2**: 8 faster combat drones.
   - **Wave 3**: 10 aggressive combat drones.
   - Defeating all 3 waves triggers **Level Complete (Victory)**.
7. **Heads-Up Display (HUD) & Menus**:
   - Rendered using **2D Orthographic Projection** and alpha blending.
   - Precision center targeting crosshair.
   - Dynamic health bar (Green $\rightarrow$ Yellow $\rightarrow$ Red) and numeric HP display.
   - Ammo display with 30 individual bullet pips.
   - Score tracker and segmented wave progress blocks.
   - Full-screen Start Menu, Pause, Game Over, and Victory dialog overlays.
8. **Particle Systems & Visual Polish**:
   - Bright cyan plasma muzzle flash burst at the gun barrel.
   - Ricochet and impact sparks flying off hit surfaces.
   - Multi-particle fiery explosion when an enemy drone is neutralized.
   - Red vignette damage flash on the player's screen when taking damage.

---

## 5. Controls Guide

| Input Key | In-Game Action |
| :--- | :--- |
| **Mouse Movement** | Aim & look around (Horizontal Yaw & Vertical Pitch) |
| **Left Mouse Button** | Shoot weapon (Ray-casting hit detection) |
| **W / S** | Walk Forward / Move Backward |
| **A / D** | Strafe Left / Strafe Right |
| **SPACE / Left Shift** | Move Up / Move Down (Noclip inspection mode) |
| **R** | Reload ammunition clip / Restart on Game Over or Victory |
| **Mouse Scroll Wheel** | Zoom camera Field of View in / out |
| **P / ESC** | Pause game / Unlock mouse cursor |
| **ENTER / SPACE** | Start game from Menu / Resume from Pause |
| **TAB** | Toggle cursor capture lock on / off |

---

## 6. Project Architecture & File Structure

```
CyberStrike3D/
├── CMakeLists.txt                 # Primary CMake build system configuration
├── build.bat                      # One-click Windows build script
├── README.md                      # Academic documentation & viva reference
│
├── assets/
│   └── textures/                  # 2D Diffuse texture maps (PNG)
│       ├── floor.png              # Dark metallic floor with cyan circuit grid
│       ├── wall.png               # Industrial armor plate with hazard trim
│       ├── crate.png              # Military supply crate with orange frame
│       ├── enemy.png              # Biomechanical drone skin with crimson sensor
│       └── gun.png                # Gunmetal polymer and plasma emitter
│
├── shaders/                       # GLSL programmable shader source files
│   ├── vertex.glsl                # MVP transformations, normals, and UV coordinates
│   └── fragment.glsl              # Blinn-Phong lighting, texture sampling, and colors
│
├── external/                      # Third-party libraries
│   ├── glad/                      # OpenGL 3.3 Core function loader (glad.h, glad.c)
│   ├── glfw/                      # GLFW 3.3.8 windowing library (libglfw3dll.a, glfw3.dll)
│   ├── glm/                       # Header-only GLM math library (vec3, mat4, transforms)
│   └── stb/                       # Single-header image loader (stb_image.h)
│
├── include/                       # C++ Class Headers (.h)
│   ├── Game.h                     # Master game loop, wave orchestration, and state machine
│   ├── Player.h                   # Player health, score, weapon, and AABB
│   ├── Enemy.h                    # Autonomous drone entity, FSM AI, and combat logic
│   ├── Weapon.h                   # Firearm ammo, recoil animation, and viewmodel rendering
│   ├── Collision.h                # AABB-AABB overlap and Kay-Kajiya Ray-AABB intersection
│   ├── Particle.h                 # Muzzle flashes, impact sparks, and death explosions
│   ├── Arena.h                    # 3D combat arena layout, obstacles, and spawn zones
│   ├── Camera.h                   # First-person view camera, Euler angles, and lookAt matrix
│   ├── UI.h                       # 2D Orthographic HUD, crosshair, and menu overlays
│   ├── Core/
│   │   ├── Window.h               # GLFW window wrapper, input callbacks, context setup
│   │   └── Transform.h            # 3D Position, Rotation, Scale, and TRS Model matrix
│   └── Renderer/
│       ├── Shader.h               # GLSL compilation, error checking, and uniform setters
│       ├── CubeMesh.h             # GPU VAO/VBO buffer geometry (Pos, Normals, UVs, Color)
│       └── Texture.h              # 2D texture loading, bilinear filtering, and mipmapping
│
└── src/                           # C++ Implementation Files (.cpp)
    ├── main.cpp                   # Application entry point and self-test verification
    ├── Game.cpp                   # Main update/render loop, shooting, and wave manager
    ├── Player.cpp                 # Player health and damage processing
    ├── Enemy.cpp                  # Enemy AI transitions and 3D drone rendering
    ├── Weapon.cpp                 # Procedural recoil and 3D gun viewmodel
    ├── Collision.cpp              # Ray-casting math and wall sliding resolution
    ├── Particle.cpp               # Particle physics integration and rendering
    ├── Arena.cpp                  # Obstacle setup and AABB bounds generation
    ├── Camera.cpp                 # Spherical direction trigonometry and View matrix
    ├── UI.cpp                     # 2D UI quad renderer, digits, and health bar
    ├── Core/
    │   ├── Window.cpp             # Window creation and event polling
    │   └── Transform.cpp          # TRS matrix calculation
    └── Renderer/
        ├── Shader.cpp             # GLSL shader program management
        ├── CubeMesh.cpp           # 11-float stride vertex buffer configuration
        └── Texture.cpp            # stb_image integration and mipmap generation
```

---

## 7. Computer Graphics Concepts Demonstrated

| Computer Graphics Concept | File / Class | Mathematical & Algorithmic Foundation |
| :--- | :--- | :--- |
| **Windowing & OpenGL Context** | `Window.cpp` | `glfwInit()`, `glfwCreateWindow()`, `glfwMakeContextCurrent()`. Creates WGL drawing surface. |
| **Runtime Pointer Loading** | `Window.cpp` | `gladLoadGLLoader` queries GPU driver addresses (`wglGetProcAddress`) for OpenGL 3.3+ calls. |
| **Double Buffering & V-Sync** | `Window.cpp` | `glfwSwapBuffers()` swaps front and back framebuffers atomically during vertical blanking. |
| **Normalized Device Coordinates (NDC)** | `Window.cpp` | `glViewport(0, 0, W, H)` maps $[-1, 1]^3$ cube to physical pixel raster $[0, W] \times [0, H]$. |
| **GPU Vertex Buffers (VAO/VBO)** | `CubeMesh.cpp` | Allocates VRAM buffers; binds attribute pointers with 11-float stride ($44\text{ bytes}$). |
| **Depth Testing & Z-Buffering** | `Window.cpp` | `glEnable(GL_DEPTH_TEST)`, `glClear(GL_DEPTH_BUFFER_BIT)`. Resolves visibility via 24-bit depth comparison. |
| **Affine Transformations (TRS)** | `Transform.cpp` | $4 \times 4$ homogeneous matrix multiplication: $\mathbf{M} = \mathbf{T} \cdot \mathbf{R}_y \cdot \mathbf{R}_x \cdot \mathbf{R}_z \cdot \mathbf{S}$. |
| **Coordinate Space Pipeline** | `vertex.glsl` | Local $\rightarrow$ World $\rightarrow$ View $\rightarrow$ Clip $\rightarrow$ NDC $\rightarrow$ Screen: $\mathbf{v}_{\text{clip}} = \mathbf{P} \cdot \mathbf{V} \cdot \mathbf{M} \cdot \mathbf{v}_{\text{local}}$. |
| **First-Person View Camera** | `Camera.cpp` | $\mathbf{V} = \text{glm::lookAt}(\mathbf{P}, \mathbf{P} + \mathbf{F}, \mathbf{U})$ constructing orthonormal camera basis $(\mathbf{R}, \mathbf{U}, \mathbf{F})$. |
| **Euler Angles (Yaw & Pitch)** | `Camera.cpp` | Converts spherical angles to Cartesian unit direction: $F_x = \cos\psi\cos\theta, F_y = \sin\theta, F_z = \sin\psi\cos\theta$. |
| **Perspective Projection** | `Game.cpp` | $\mathbf{P} = \text{glm::perspective}(\text{fov}, \text{aspect}, z_{\text{near}}, z_{\text{far}})$ simulating human eye foreshortening. |
| **Normal Matrix Transformation** | `vertex.glsl` | $\mathbf{N}_{\text{world}} = \text{mat3}((\mathbf{M}^{-1})^T) \cdot \mathbf{N}_{\text{local}}$ preserves perpendicularity under non-uniform scaling. |
| **Blinn-Phong Illumination** | `fragment.glsl` | $I = I_a k_a + I_d k_d (\mathbf{N} \cdot \mathbf{L}) + I_s k_s (\mathbf{N} \cdot \mathbf{H})^\alpha$ using Halfway Vector $\mathbf{H} = \frac{\mathbf{L}+\mathbf{V}}{\|\mathbf{L}+\mathbf{V}\|}$. |
| **Distance Attenuation** | `fragment.glsl` | Point light falloff: $\text{Atten} = \frac{1.0}{K_c + K_l \cdot d + K_q \cdot d^2}$. |
| **Texture Mapping & Mipmapping** | `Texture.cpp` | Maps $(U, V)$ coordinates $[0, 1]$; generates multi-level mipmaps to prevent distance aliasing. |
| **Ray Casting for Shooting** | `Collision.cpp` | Parametric 3D ray equation $\mathbf{R}(t) = \mathbf{O} + t\mathbf{D}$ tested against target bounding boxes. |
| **Kay-Kajiya Slab Algorithm** | `Collision.cpp` | Fast Ray-AABB intersection computing entry $t_{\min}$ and exit $t_{\max}$ across 3 axis slabs. |
| **Collision Resolution & Sliding** | `Collision.cpp` | Resolves movement per Cartesian axis independently, permitting smooth wall sliding. |
| **2D Orthographic Projection** | `UI.cpp` | $\mathbf{P}_{\text{ortho}} = \text{glm::ortho}(0, W, 0, H)$ projects 2D HUD pixels with zero perspective distortion. |
| **Alpha Blending** | `UI.cpp` | `glEnable(GL_BLEND)`, `glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)` for transparent UI overlays. |
| **Particle Simulation** | `Particle.cpp` | Simulates muzzle flashes, sparks, and explosions using Euler physics integration and lifetime decay. |

---

## 8. Build & Execution Instructions

### One-Click Automated Build (`build.bat`)
Open PowerShell or Command Prompt inside the `CyberStrike3D` folder and execute:
```bat
.\build.bat
```
This script runs CMake, compiles all C++ sources with MinGW GCC, copies runtime dependencies (`glfw3.dll`), and organizes shaders and textures into `build\`.

### Running the Game Interactively
```powershell
.\build\CyberStrike3D.exe
```

### Running the Automated Self-Test
```powershell
.\build\CyberStrike3D.exe --test
```

---

## 9. Viva Questions & Model Answers

### Q1: What is the Graphics Pipeline and what are its main stages?
**Answer:** The modern OpenGL programmable graphics pipeline is a sequence of processing steps executed on the GPU that converts 3D vertex data into final 2D pixels displayed on a monitor:
1. **Vertex Specification**: Vertex attributes (positions, normals, texture coordinates) are uploaded to GPU VRAM via Vertex Buffer Objects (VBOs).
2. **Vertex Shader**: A programmable shader executed once per vertex that transforms 3D local coordinates into Clip Space via the Model-View-Projection matrix: $\mathbf{v}_{\text{clip}} = \mathbf{P} \cdot \mathbf{V} \cdot \mathbf{M} \cdot \mathbf{v}_{\text{local}}$.
3. **Primitive Assembly**: Vertices are grouped into geometric primitives (such as triangles, `GL_TRIANGLES`).
4. **Rasterization**: Decomposes triangles into pixel-sized fragments and linearly interpolates vertex attributes (normals, UVs) across the triangle face.
5. **Fragment Shader**: A programmable shader executed for every fragment that computes lighting (Blinn-Phong), samples diffuse textures, and outputs final RGBA colors.
6. **Per-Sample Operations**: Evaluates depth testing (Z-buffering) and alpha blending before writing to the final framebuffer.

### Q2: Why is the Blinn-Phong lighting model preferred over classic Phong?
**Answer:** In classic Phong reflection, the specular component is calculated using the angle between the reflection vector $\mathbf{R}$ and the view vector $\mathbf{V}$: $(\mathbf{R} \cdot \mathbf{V})^\alpha$. When this angle exceeds $90^\circ$, the dot product becomes negative and is clamped to zero, creating an abrupt cutoff artifact. 
**Blinn-Phong** solves this by evaluating the angle between the surface normal $\mathbf{N}$ and the **Halfway Vector** $\mathbf{H} = \frac{\mathbf{L} + \mathbf{V}}{\|\mathbf{L} + \mathbf{V}\|}$: $(\mathbf{N} \cdot \mathbf{H})^\alpha$. This produces softer, more physically plausible specular highlights and is computationally faster because the halfway vector is smoother to calculate.

### Q3: Why is the Normal Matrix defined as the transpose of the inverse of the model matrix?
**Answer:** If a 3D model undergoes **non-uniform scaling** (such as scaling $X$ by 10 and $Y$ by 1), transforming normal vectors using the standard model matrix tilts the normals, so they are no longer perpendicular to the transformed surface. The mathematical derivation proves that surface tangent vectors $\mathbf{T}$ transform with $\mathbf{M}$ ($\mathbf{T}' = \mathbf{M}\mathbf{T}$). To keep the normal vector perpendicular ($\mathbf{N}' \cdot \mathbf{T}' = 0$), the normal transformation matrix must be the **transpose of the inverse of the model matrix**:
$$\text{NormalMatrix} = (\mathbf{M}_{3 \times 3}^{-1})^T$$

### Q4: How does Ray-Casting work for hit detection in an FPS game?
**Answer:** Ray-casting models the path of a bullet as a 3D parametric line:
$$\mathbf{R}(t) = \mathbf{O} + t \cdot \mathbf{D}, \quad t \ge 0$$
Where $\mathbf{O}$ is the camera position (eye) and $\mathbf{D}$ is the normalized camera forward direction. Using the **Kay-Kajiya slab method**, the ray is tested against the Axis-Aligned Bounding Box (AABB) of every enemy and obstacle by calculating entry ($t_{\min}$) and exit ($t_{\max}$) times across the $X, Y, Z$ boundary planes. The object with the smallest positive distance ($t > 0$) is registered as the nearest hit.

### Q5: How do we render 2D HUD text and bars without interfering with the 3D world?
**Answer:** We render the 2D interface in a dedicated pass after the 3D scene:
1. Disable Depth Testing (`glDisable(GL_DEPTH_TEST)`) so HUD elements are never occluded by 3D geometry.
2. Enable Alpha Blending (`glEnable(GL_BLEND)`) to support translucent background plates and vignette flashes.
3. Switch from Perspective Projection to **2D Orthographic Projection** (`glm::ortho(0, width, 0, height)`), mapping screen pixel coordinates $[0, \text{width}] \times [0, \text{height}]$ directly to Normalized Device Coordinates $[-1, 1]$.
