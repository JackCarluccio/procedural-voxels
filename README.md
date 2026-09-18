# Procedural Voxels

![View of a forest, desert, and mountain biome with a tundra in the distance](/assets/promo/ground_shot_1.png)
![Birds-eye view of a 196 chunk render radius](/assets/promo/birds_eye.png)

A highly-performant, infinite, procedurally-generated, C++ Minecraft-akin project using the OpenGL graphics api.

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

```cpp
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

## Performance

The following timings were benchmarked on my laptop with these specs and flags:
* CPU: 11th Gen Intel(R) Core(TM) i5-1135G7 @ 2.40GHz
* GPU: Intel Corporation TigerLake-LP GT2 [Iris Xe Graphics]
* Memory: 8GB DDR4 3200Mhz
* Flags: -Ofast -march=native

**Chunk Shaping** is building the stone/air blocks of a chunk by generating a height map, then using that to fill the chunk. Since every chunk gets shaped, and all of their 65,536 blocks must receive some form of operation, it is essential that the steps involved be highly performant.

* Optimizing the height map generation meant optimizing the underlying Perlin noise operations. The following points detail how I accomplished this.

  * Note: Each of these methods were benchmarked with the same world-gen settings, including number of octaves, for a fair comparison.
    
  * My first rendition of Perlin noise followed a [youtube video](https://youtu.be/kCIaHqb60Cw), which, while providing an excellent explanation of the mathematics behind Perlin noise, was far from performant.
    
    * The core of the algorithm involved hashing grid coordinates to produce a random angle, then forming a gradient: 
      ```cpp
      float random = HeavyHashingMethod(x, y);
      return {
        x = cos(random),
        y = sin(random)
      };
      ```
    * The first performance problem was from the heavy hashing method, which involved many operations including multiple divisions. Furthermore, sine and cosine calculations are expensive. Both of these two problems must be repeated 4 times, one for each of the surrounding grid corners, resulting in an average `60.9µs` per height map.

  * Upon researching further, I found a permutation-table based approach. This method eliminated the two performance problems of the prior approach.

    * But, under two key assumptions:

      1. The quality of the gradient from each grid point is not very important. That is, having a continuous range of possible gradients vs a few discrete, pre-computed gradients has almost no visual effect.
      2. The quality of the hashing algorithm for each grid point also is not particularly important. Since, again, quality gradients are not important.

    * With these assumptions, our gradient function turns into:
      ```cpp
      uint8_t permutation = perms[(perms[x & 255] + y) & 255];
      return gradients[permutation & 7];
      ```

    * Now, the hashing function and gradient calculations are incredibly fast, bringing our average height map generation time down to `10.95µs`.

  * The final, and current approach, optimizes sampling the entire height map, since each hash and gradient calculation are as fast as possible.

    * When sampling multiple points within one grid cell independently, many calculations are repeated. This includes finding the locations of the surrounding grid points and their gradients.

    * So, I created a new method, `PerlinNoise2D::SampleMap2D()`. By calculating gradients and surrounding grid points once, then sampling each point with these already computed values, massive performance gains are on the table.

    * However, this method only works simply if all the points are within one grid cell. In reality, especially at higher frequencies, this often is not the case. But this can be mitigated with multiple SampleMap2D() calls, one for each grid.

    * Nonetheless, by sampling an entire map at once, height map generation time now averages an incredible `1.45µs`.

  * Combining these techniques resulted in a **42x speedup from `60.9µs` to `1.45µs`**!

* Filling the block data of the chunk from the height map is simple in theory, but to be done as fast as possible requires an understanding of the underlying computer architecture.

  * Note: The memory layout of the chunk's blocks follows `blocks[y][x][z]`. This way, each floor/slice of the chunk is a contiguous 256 bytes. I chose this layout for potential future optimizations, including splitting chunks into 16x16x16 sub-chunks. 
  * My first naive approach to filling the chunk from the height map was:
    ```cpp
    for (int x = 0; x < chunk::WIDTH; x++) {
      for (int z = 0; z < chunk::WIDTH; z++) {
        int terrain_height = ...;
        for (int y = 0; y <= terrain_height; y++)
          chunk.SetBlock(x, y, z, block::Stone);
        for (int y = terrain_height + 1; y < chunk::HEIGHT; y++)
          chunk.SetBlock(x, y, z, block::Air);
      }
    }
    ```
    * Since y increments by 1 between writes, each write is 256 bytes away from the previous. However, the 64 byte cache lines which make contiguous reads incredibly fast, make this naive approach very slow. The CPU cannot write one byte to memory, it must write an entire cache line. But, if we flush a cache line with only 1 byte set, the other 63 bytes are garbage, corrupting adjacent memory. To prevent this, the CPU must first read the cache line from memory before it can modify a single byte.
    
    * Although the hardware prefetcher, caches, and memory-level parallelism techniques significantly speed this function up more than it otherwise would be, it still takes an average `101.6µs` to fill the chunk.

  * A more advanced approach takes advantage of the memory layout and SIMD operations:
    ```cpp
    int min_height = *std::min_element(...);
    std::memset(blocks, block::Stone, min_height*256);
    std::memset(blocks + min_height*256, block::Air, ...);
    ```
    * The memory layout allows us to write all blocks below the minimum terrain height, and above, in just two memsets. Because of the contiguous memory layout, the compiler will vectorize the memory writes, writing up to 64 bytes at a time. In addition, calculating the minimum height can easily be vectorized.

    * This results in a **5.7x speedup to `17.8µs`**.

* With the optimizations on the height map generation and filling procedure, average chunk shaping time was shrunk **9.1x from `180µs` to `19.8µs`**

**Chunk Meshing** is the process of turning the raw block data of the chunk into a series of vertices and triangles the GPU is capable of rendering.

* The default approach, with no occlusion culling, averaged `7,820µs` per mesh.

* The most obvious and largest optimization comes from omitting (culling) faces hidden by other blocks.
  * Since our world is a grid of blocks, checking occlusion state just means determining if there is a neighboring block, a simple memory read.
    * Note: Blocks on the border of a chunk will need to read blocks in neighboring chunks, requiring special looping logic and increasing mesher complexity.
  
  * This yields the largest single-trick optimization in the entire project, bringing average meshing time down to `586µs`, a **13.3x speedup!**

* Optimization 3 was developed almost accidentally after bit-packing vertex and index data.
  
  * Previously, each vertex used 24 bytes of video-memory:
    * Position: 3 floats = 12 bytes
    * Texture coordinates: 2 floats = 8 bytes
    * Ambient occlusion: 1 int = 4 bytes

  * But many bits are wasted:
    * Since each position can only be 0-17 for x and z (5 bits), and 0-257 for y (9 bits), only 19 bits are needed.
    * Each vertex can only be in one of four corners for each texture. Assuming we have 256 textures, 8 bits will suffice.
      * Note: Since every set of 4 vertices are added in the same order, we can use gl_VertexID within the vertex shader to get the corner index without using any bits.
    * Ambient occlusion has 4 lighting levels, so only 2 bits are needed.

  * Combined, each vertex only needs 29 bits, represented as a 4 byte `uint32_t`, substantial savings from 24 bytes. Additionally, by limiting the number of vertices to 65,536 (2^16), each index can be represented with just a 2 byte `uint16_t` instead of a 4 byte `uint32_t`.

  * After noticing a visually-substantial increase in meshing rates, I decided to tinker more with the looping mechanics within the mesher.
    * This mostly involved calculating the occlusion state for all 6 faces before adding any to the data.

  * Now, each mesh averaged `176µs` to construct.

* The mesh data is only needed by the GPU to draw the chunk. Because of this, after generating the mesh data and uploading it to the GPU, it can be discarded.
  
  * Instead of generating a new `std::vector` for every mesh construction, just storing it within the Mesher object avoids memory allocations after the first few chunks.

  * To improve on this further, a `std::array` with the worst possible capacity allocated, just 65,536 elements, avoids all additional allocations. And more importantly, eliminates the need for `std::vector::push_back`, allowing us to write directly to memory.

  * Using scratch buffers drops average mesh time to `134µs`.

* Combining the techniques of block-face occlusion culling, memory reductions, and scratch buffers speeds up average meshing time by **58.4x from `7,820µs` to `134µs`**.

The **Chunk Queue** prioritizes chunks within the render radius for generation.
* Note: All implementations use a heap via `std::make_heap` and `std::pop_heap` for O(n) construction and O(logn) pop times.
* The simplest approach, my first approach, looked like:
  ```cpp
  for (int x = -radius; x <= radius; x++) {
    for (int z = -radius; z <= radius; z++) {
      int distance = x * x + z * z;
      if (distance > radius * radius) continue;
      if (manager.HasChunk(x + plr.x, z + plr.z)) continue;
      heap_vector.push_back({ x + plr.x, z + plr.z, distance });
    }
  }
  ```
  * With a 128-chunk render radius, this implementation averaged `1650µs`.

* Profiling revealed that 1/2 of the queue build time was spent on `manager.HasChunk()`. With very large chunk counts, most nodes within the `std::unordered_map` chunks_ won't be in the L1 cache. Instead, they'll be in L2/L3, or cold in memory.

  * Thankfully, this check can be moved out of build time; when popping out of the queue, check if the chunk already exists. If it does, pop again. This spreads the load out over many frames instead of one, for no additional cost.

  * Removing the `manager.HasChunk()` check dropped the average queue build time to `830µs`.
* The final optimization came from scrutinizing what data the heap really needs to track.

  * Previously, each element in the heap was a 12-byte struct:
    ```cpp
    struct {
      int x, z, distance;
    }
    ```

  * But using 4 bytes each for both x and z seemed wasteful. Instead of storing their absolute location, storing their offset from the player's chunk meant just 2 bytes each for x and z would be sufficient.
    * Note: Technically, we could use 1 byte each for x and z, as long as the render radius stays under 255 chunks. But, the struct would be padded to 8 bytes anyways to maintain a 4-byte alignment, so we might as well use the available bytes.

  * Now, each element is an 8-byte struct:
    ```cpp
    struct {
      int distance;
      short x, z;
    }
    ```

  * Furthermore, we can bit-pack this data into a single `uint64_t`. By storing `distance` in the most-significant 32 bits, we can use native integer operations for comparison.

  * Cutting the memory footprint of each element from 12 bytes to an 8 byte primitive brought down the average build time to `690µs`.

* Spreading the existence check over multiple frames and minimizing the memory footprint resulted in a **2.39x speedup** from `1650µs` to `690µs`.

The **Chunk** class contains the following members:
```cpp
Stage stage_; // enum
glm::ivec2 position_
std::optional<Mesh> mesh_;
std::array<Block, chunk::VOLUME> blocks_;
```

* Wrapping the Mesh in a `std::optional` instead of a std::unique_ptr avoids a pointer indirection, reduces heap fragmentation since theres no heap allocation, and reduces the overhead from an 8 byte pointer to a 1 byte boolean (padded to 4 bytes).
  * Note: While a `std::optional` always reserves space for the underlying data type, since most chunks are meshed and a mesh is only 12 bytes, we still save memory compared to the unique_ptr.
  
* By storing the blocks inside a `std::array` and not a `std::vector`, we save 24 bytes of overhead, avoid a pointer indirection, and reduce heap fragmentation.

**Frustum Culling** reduces or eliminates processing outside of the camera's viewing frustum (visible area). Calculating a frustum cull means determining if the bounding volume in question is behind any of the frustum planes. If it is, then it's not visible. As a result of how lightweight this calculation is, frustum culling is an essential optimization technique used in two places in this engine:

* Render-time frustum culling checks every chunk's bounding box against the view frustum to avoid drawing meshes which cannot be seen. With a horizontal fov of 120°, 2/3 of the world is invisible, resulting in 1/3 the usual draw call count.
  
* Generation-time frustum culling checks every chunk's bounding box against the view frustum to avoid generating chunks which cannot be seen. Technically, because of the staggered chunk generation stages, the two invisible rings of chunks mean we actually need to triple the bounding box dimensions. At low render radiuses, more than 1/3 of chunks within the radius are generated, but this fraction approaches 1/3 with large render radii.
  * Note: Now that generating chunks is tied to camera rotation, we have to rebuild the chunk queue every time the player moves their camera, as opposed to only when entering a new chunk. But the massive benefits of only loading 1/3 of the world's chunks drastically outweighs more frequent queue rebuild requests.

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

## Generation Details

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

## Miscellaneous Features

* **Ambient Occlusion** darkens vertices based off the number of surrounding blocks. [Reference Article](https://medium.com/@andrebluntindie/vertex-ambient-occlusion-for-voxel-games-the-principle-and-implementation-e5340bd62845)
  * Without this, determining terrain shape (hills vs. valleys vs. plains) when viewing large quantities of the same block type, would be near impossible.
  * To accomplish this, when meshing, each vertex checks the occlusion state (visibility) of the neighboring three blocks to determine one of four lighting levels.
* **Block Types** allow blocks to have different textures, occlusion behaviors, and more.
  * To accomplish this, each block is represented as a number (`uint8_t`) corresponding to its type. E.g. Air = 0, Stone = 1, etc.

## Attributions

* [Coterie Craft Lushbloom Classic](https://www.curseforge.com/minecraft/texture-packs/coterie-craft-classic) - for block textures - by MrCptCorn [(License)](https://creativecommons.org/licenses/by-sa/4.0/deed.en)
* [GLAD](https://github.com/Dav1dde/glad) - for accessing OpenGL functions
* [GLFW](https://github.com/glfw/glfw) - for window management
* [GLM](https://github.com/g-truc/glm) - for graphics mathematics
* [json](https://github.com/nlohmann/json) - for reading JSON data
* [stb_image](https://github.com/nothings/stb) - for loading textures
