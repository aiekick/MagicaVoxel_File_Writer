#include "VoxWriter.h"
#include <cstdint>   // int32_t
#include <chrono>    // std::chrono
#include <iostream>  // std::cout

int main() {
    const auto now = std::chrono::steady_clock::now();

    const int32_t SIZE = 30; // 189;
    const int32_t  OFFSET    = 126 / 2; // SIZE;
    const int32_t  FRAMES    = 2;
    const float    len_ratio = 1.0f / (SIZE * SIZE);
    vox::VoxWriter vox;

    // vox.set_key_frame(0);
    // vox.set_key_frame(1);

    float time = 0.0f;
    for (int32_t k = 0; k < FRAMES; ++k) {
        vox.SetKeyFrame(k);
        time += 0.5f;
        for (int32_t i = -SIZE; i < SIZE; ++i) {
            for (int32_t j = -SIZE; j < SIZE; ++j) {
                float   len        = (i * i + j * j) * len_ratio;
                int32_t pz         = (int32_t)((std::sin(len * 10.0 + time) * 0.5 + 0.5) * (std::abs(50.0f - 25.0f * len)));
                int32_t cube_color = (int32_t)(len * 100.0) % 255 + 1;
                vox.AddVoxel(i + OFFSET, j + OFFSET, pz, cube_color);  // magicavoxel use the z as up axis
            }
        }
    }
    vox.SaveToFile("output_voxwriter.vox");
    vox.PrintStats();
    const auto elapsed = std::chrono::steady_clock::now() - now;
    std::cout << "Eleasped time : " << (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()) * 1e-3 << " secs" << std::endl;
}