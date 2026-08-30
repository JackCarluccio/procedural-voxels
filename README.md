# Procedural Voxels

![Birds-eye view of a 196 chunk render radius](/assets/promo/birds_eye.png)

A highly-performant, infinite, procedurally-generated C++ Minecraft clone with an OpenGL graphics api.

## Build Instructions

This project uses CMake and requires a C++20 compatible compiler (such as GCC, Clang, or MSVC). All external dependencies (GLFW, GLAD, GLM, stb, and JSON) are bundled directly in the `external/` directory, so there is no need to install third-party libraries manually.

To build the engine, clone the repository and run the following commands from the project root:

```bash
# Clone the repository
git clone https://github.com/JackCarluccio/procedural-voxels.git
cd procedural-voxels

# Configure the build directory
cmake -S . -B build/

# Compile the project
cmake --build build/

# Run
./build/ProceduralVoxels
```

Note: The project is configured with strict compiler flags, treating warnings as errors (-Werror for GCC/Clang, /WX for MSVC). Warnings from the bundled stb_image implementation are explicitly suppressed in the build configuration.

## Controls

* `WASD` - to move forward, left, backward, right
* `Q/SPACE` - to move upwards
* `E/CTRL` - to move downwards
* `Mouse Movement` - to look around
<!-- * `Escape` - to quit -->

## Architecture

#### Object Ownership

```
main.cc
└─ Application
    ├─ Window
    ├─ InputManager
    ├─ Renderer
    │   ├─ ShaderProgram
    │   └─ TextureAtlas
    └─ Scene
    │   ├─ Camera
    │   └─ Skybox
    └─ ChunkManager
        ├─ Queue
        ├─ Generator
        ├─ Mesher
        └─ Chunks
            └─ Chunk
                ├─ Mesh
                ├─ Stage
                └─ BlockData
```

#### Game Loop

```
while (is_running_ && !window_->ShouldClose()) {

  1. Application::Update()
    2. glfwPollEvents();
    3. window_->HasChangedSize();
    4. input_manager_->Update();
      5. ProcessCameraMovement();
      6. ProcessCameraRotation();
      7. CheckForClosure();
    8. chunk_manager_->Update();
      9. chunk_queue_.Update();
      10. GenerateChunks();
  11. Application::Draw();
    12. Renderer::Draw();
      13. glClear();
      14. shader_program->Use();
      15. glBindTexture();
      16. chunk_mesh->Bind();
      17. glDrawElements();
    18. Window::SwapBuffers();

}
```

#### Additional Details

* The **Chunk Manager** owns the chunks via an `unordered_map<ivec2, unique_ptr<Chunk>>`. By using a `unique_ptr`, `unordered_map::rehash()` is kept very cheap, eliminating stutters at the cost of an additional pointer chase.

## Generation

**Chunks** may exist in one of four stages: `Empty`, `Shaped`, `Decorated`, `Meshed`.
* `Empty` - A chunk may only be in this stage after being constructed.
* `Shaped` - Immediately after constructing a chunk, it is shaped. The chunk is given to the ChunkGenerator, which fills the chunk with a basic terrain shape consisting of only stone.
* `Decorated` - The decoration phase consists of applying biome surface details and features. However, since features can place blocks into neighboring chunks, such as a tree on the border of its chunk, a chunk may only be decorated if its 8 neighbors have been `Shaped`.
* `Meshed` - Similarly to decorated, a chunk may only be meshed if its 8 neighbors are `Decorated`. Technically, we can mesh a chunk as long as its 8 neighbors exist, in any form. But, if we mesh prematurely, when a neighboring chunk gets decorated or shaped, we would have to re-mesh this chunk.

Because of the `Decorated` and `Meshed` stages, there are two invisible rings of chunks which exist in memory, but are not visible to the player.

**Noise Algorithms** take locational inputs and produce patterned outputs that can be used in many applications, most notably, procedural generation.
* **[Perlin Noise](https://en.wikipedia.org/wiki/Perlin_noise)** produces smooth, coherent patterns by interpolating between pseudo-random gradient vectors assigned to a grid, ensuring that nearby points have similar values.
  * When viewed as an image, it appears to produce a blurry landscape. This makes it a wonderful method of generating terrain height.
  ![Perlin noise viewed as an image](https://rtouti.github.io/assets/images/perlin-noise-texture.png)
  * However, the output terrain is too smooth and lacks detail. To fix this, we can layer multiple *octaves* of noise by sampling at higher frequencies (*lacunarity*), and decreasing their contributions (*persistence*).
  ![Layered Perlin noise viewed as an image](https://raw.githubusercontent.com/csaddison/Perlin-Noise/refs/heads/master/images/octaves.png)
  ![GIF Demonstration of octaves](https://blog.lslabs.dev/assets/images/post21/octaves_loop.gif)

**Hashing Algorithms** are stateless, deterministic, pseudo-random number generators with any number of inputs. A good hash function will flip half its output bits when a single input bit changes. Their stateless nature is essential for random access generation.
* Note: While it's possible to instance a standard random number engine, such as `std::mt19937`, just to produce one value, the construction costs are typically very expensive making this impractical.
* **Squirrel Noise 5** is fast, lightweight, and specifically optimized for procedural generation. Thus, it's the primary hashing algorithm used for generation.

**Splines** are "special functions defined piecewise by polynomials." Rather than having the outputs from our Perlin noise directly affect the terrain height, we plug the results into a linear spline. This allows us to create more advanced terrain, such as plateaus.
  * Note: Plateaus can be accomplished by defining an interval with constant height, such as `{x=0.8, y=1.4}, {x=1.0, y=1.4}`.
  ![Demonstration of a spline](https://blog.djnavarro.net/posts/2025-09-06_p-splines/index_files/figure-html/my-spline-1-1.png)

**Terrain Shape** is calculated during the shaping phase. Generating the shape of the terrain can easily be the most complex step of chunk generation, since there is no right answer. The current process is as follows.
1. Generate the height map.
   1. A noise value is calculated from a high-octave 2d Perlin noise generator.
   2. The noise is then plugged into a linear spline whose output determines the terrain height.
2. Fill in all blocks below the height map with stone.
   1. Note: The surface and a few subsurface blocks are replaced after the biomes are determined in the decoration phase.

**Biomes** split the world into distinct regions. They're determined during the decoration phase and are essential to reducing repetition within the world. Importantly, biomes are not generated per-chunk but per-block-column. These steps explain their relatively simple generation.
1. Using very low-frequency noise algorithms, every block-column gets a humidity and temperature.
2. These raw floats are turned into `Humidity` and `Temperature` enums, such as `Low` and `High` based off which interval they fall into.
3. The enum values are then turned into the biome via a large table. E.g. `(Humidity::Low, Temperature::High) = Biome::Desert`.

**Features** are the decorations found around the world. They're generated during the decoration phase after the biome map, and are one of the more complicated systems to implement. The following steps explain how they're generated.
1. The list of unique biomes within the chunk is calculated from the biome map.
2. For each of these unique biomes, and for each of the features that can spawn within them, as defined by their settings, the quantity to place, n, is calculated from a `std::poisson_distribution<int>(settings.average_count)`.
   1. Using a poisson distribution allows for dense and sparse regions of features, since some chunks may have 0 of that feature, and others may have many more than the average count.
   2. More naive approaches would involve a simple for-loop and always place the same quantity of the feature within one chunk, leading to a less-natural looking world.
3. Next, n times, the x-z position is generated from a `mersenne-twister engine (std::mt19937)`. If this position is not within the biome it originated from, checked by the biome map, the feature is not placed.
   1. The y position is looked up in the height map, which is regenerated during the decoration phase, since height map generation is so cheap.
      1. Note: The height map could be stored within the chunk, using `16 x 16 x sizeof(uint8_t) = 256 bytes`, but considering it's only used during shaping and decorating, storing it permanently would be a waste of memory.
   2. Checking against the biome map ensures features won't be found outside their natural habitat along biome boundaries. Without this, chunk boundaries along biome boundaries would be visible when viewed from a distance.
   3. By not attempting to re-place the feature, if one biome only covers 20% of a chunk, only 20% of its features will be placed, on average
4. Before placing the feature, we must ensure it can fit. Every feature has a CanGenerate(location) method. As long as this method returns true, the feature is finally placed.
   1. Note: Most features check a subset of the locations blocks would be placed in during its generation.
   2. Note: Most features don't allow their blocks to override solid blocks. That way, trees, for example, won't carve into mountains.

## Performance

## Benchmarks

The first image showcases a 196 chunk render radius. Assuming each block measures 1m<sup>3</sup> with an average density of 2,700 kg/m<sup>3</sup>, typical of the Earth's continental crust, the following numbers arise:

Surface Area: 30,897,664 m<sup>2</sup>
* 4 countries (Vatican City, Monaco, Nauru, Tuvalu)
* 9 Central Parks

Volume: 7,909,801,984 m<sup>3</sup>
* 3,053 Great Pyramids of Giza
* 3,163,920 Olypmic-sized swimming pools

Weight: 10,674,251,366,400 kg
* 37x the weight of all living people
* 38,202,428 Statues of Liberty

## Miscellaneous Features

* **Ambient Occlusion** darkens vertices based off the number of surrounding blocks.
  * Without this, determining terrain shape (hills vs. valleys vs. plains) when viewing large quantities of the same block type, would be near impossible.
  * To accomplish this, when meshing, each vertex checks the occlusion state (visibility) of the neighboring three blocks to determine one of four lighting levels.
  ![Ambient Occlusion demonstration](https://miro.medium.com/v2/resize:fit:720/format:webp/1*Ux9c_0b43hMB5z1BSemBnA.png)
* **Block Types** allow blocks to have different textures, occlusion behaviors, and more.
  * To accomplish this, each block is represented as a number (`uint8_t`) corresponding to its type. E.g. Air = 0, Stone = 1, etc.
* The **Chunk Queue** is responsible for keeping track of all chunks within the render radius that need generated. The chunk queue also prioritizes chunks according to their distance from the player by maintaining a min-heap.

## Attributions

* [Coterie Craft Lushbloom Classic](https://www.curseforge.com/minecraft/texture-packs/coterie-craft-classic) - for block textures - by MrCptCorn [(License)](https://creativecommons.org/licenses/by-sa/4.0/deed.en)
* [GLAD](https://github.com/Dav1dde/glad) - for accessing OpenGL functions
* [GLFW](https://github.com/glfw/glfw) - for window management
* [GLM](https://github.com/g-truc/glm) - for graphics mathematics
* [json](https://github.com/nlohmann/json) - for reading JSON data
* [stb_image](https://github.com/nothings/stb) - for loading textures
