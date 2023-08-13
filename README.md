# MagicaVoxel File Writer

Rust Version available here : [MagicaVoxel File Writer for Rust](https://github.com/aiekick/MagicaVoxel_File_Writer_for_Rust)

vox.hm is the file format descriptor for HexaMonkey :
- original topic about it : https://github.com/ephtracy/voxel-model/issues/19
- HexaMonkey tool : http://hexamonkey.com/

## App

the main.cpp file show you how to generate a quick file :

With this simple code

```cpp
#include "VoxWriter.h"
int main() 
{
    const int32_t  SIZE      = 189;
    const int32_t  OFFSET    = SIZE;
    const float    Z_SCALE   = 1.0f;
    const int32_t  FRAMES    = 30;
    const float    len_ratio = 1.0f / (SIZE * SIZE);
	
    vox::VoxWriter vox;
    
    vox.StartTimeLogging();
    float time = 0.0f;
    for (int32_t k = 0; k < FRAMES; ++k) {
        vox.SetKeyFrame(k);
        for (int32_t i = -SIZE; i < SIZE; ++i) {
            for (int32_t j = -SIZE; j < SIZE; ++j) {
                float   len        = (i * i + j * j) * len_ratio;
                int32_t pz         = (int32_t)((std::sin(len * 10.0 + time) * 0.5 + 0.5) * (std::abs(50.0f - 25.0f * len)) * Z_SCALE);
                int32_t cube_color = (int32_t)(len * 100.0) % 255 + 1;
                vox.AddVoxel(i + OFFSET, j + OFFSET, pz, cube_color);  // magicavoxel use the z as up axis
            }
        }
        time += 0.5f;
    }
    vox.StopTimeLogging();
    vox.SaveToFile("output_voxwriter.vox");
    vox.PrintStats();
}
```

you can generate that (previewed in [Magicavoxel](https://ephtracy.github.io/)

![main](main.gif)

possible console print ( vox.PrintStats() ) : 

```cpp
---- Stats ------------------------------
Volume : 377 x 377 x 49
count cubes : 9
count key frames : 30
-----------------------------------------
 o--\-> key frame : 0
     \-> voxels count : 142884
      \-> elapsed time : 0.045 secs
 o--\-> key frame : 1
     \-> voxels count : 142884
      \-> elapsed time : 0.046 secs
 o--\-> key frame : 2
     \-> voxels count : 142884
      \-> elapsed time : 0.046 secs
 o--\-> key frame : 3
     \-> voxels count : 142884
      \-> elapsed time : 0.047 secs
 o--\-> key frame : 4
     \-> voxels count : 142884
      \-> elapsed time : 0.047 secs
 o--\-> key frame : 5
     \-> voxels count : 142884
      \-> elapsed time : 0.048 secs
 o--\-> key frame : 6
     \-> voxels count : 142884
      \-> elapsed time : 0.048 secs
 o--\-> key frame : 7
     \-> voxels count : 142884
      \-> elapsed time : 0.047 secs
 o--\-> key frame : 8
     \-> voxels count : 142884
      \-> elapsed time : 0.048 secs
 o--\-> key frame : 9
     \-> voxels count : 142884
      \-> elapsed time : 0.048 secs
 o--\-> key frame : 10
     \-> voxels count : 142884
      \-> elapsed time : 0.048 secs
 o--\-> key frame : 11
     \-> voxels count : 142884
      \-> elapsed time : 0.047 secs
 o--\-> key frame : 12
     \-> voxels count : 142884
      \-> elapsed time : 0.047 secs
 o--\-> key frame : 13
     \-> voxels count : 142884
      \-> elapsed time : 0.049 secs
 o--\-> key frame : 14
     \-> voxels count : 142884
      \-> elapsed time : 0.049 secs
 o--\-> key frame : 15
     \-> voxels count : 142884
      \-> elapsed time : 0.049 secs
 o--\-> key frame : 16
     \-> voxels count : 142884
      \-> elapsed time : 0.05 secs
 o--\-> key frame : 17
     \-> voxels count : 142884
      \-> elapsed time : 0.048 secs
 o--\-> key frame : 18
     \-> voxels count : 142884
      \-> elapsed time : 0.049 secs
 o--\-> key frame : 19
     \-> voxels count : 142884
      \-> elapsed time : 0.049 secs
 o--\-> key frame : 20
     \-> voxels count : 142884
      \-> elapsed time : 0.049 secs
 o--\-> key frame : 21
     \-> voxels count : 142884
      \-> elapsed time : 0.05 secs
 o--\-> key frame : 22
     \-> voxels count : 142884
      \-> elapsed time : 0.049 secs
 o--\-> key frame : 23
     \-> voxels count : 142884
      \-> elapsed time : 0.05 secs
 o--\-> key frame : 24
     \-> voxels count : 142884
      \-> elapsed time : 0.049 secs
 o--\-> key frame : 25
     \-> voxels count : 142884
      \-> elapsed time : 0.05 secs
 o--\-> key frame : 26
     \-> voxels count : 142884
      \-> elapsed time : 0.05 secs
 o--\-> key frame : 27
     \-> voxels count : 142884
      \-> elapsed time : 0.051 secs
 o--\-> key frame : 28
     \-> voxels count : 142884
      \-> elapsed time : 0.051 secs
 o--\-> key frame : 29
     \-> voxels count : 142884
      \-> elapsed time : 0.051 secs
-----------------------------------------
voxels total : 4286520
total elapsed time : 1.472 secs
-----------------------------------------
```
