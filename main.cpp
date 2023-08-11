#include "VoxWriter.h"
#include <cstdint>   // int32_t
#include <chrono>    // std::chrono
#include <iostream>  // std::cout

#define USE_ANIMATED_WAVE
//#define JULIA_REVOLUTE

#ifdef JULIA_REVOLUTE
static double mix(const double& x, const double& y, const double& a) { return x * (1.0 - a) + y * a; }
int main() {
    const int32_t SIZE = 375;
    const double ZOOM_XZ     = 7.5;
    const double ZOOM_Y      = 7.5;
    const int32_t ITERATIONS  = 5;
    const int32_t FRAMES      = 1;

    double time = 0.0f;
    double kk, hh, px, pz, an, cx, cy, path, rev_x, rev_y, tmp_x, tmp_y, rev_x_squared, rev_y_squared, df;
    double rot2D[4] = {1, 0, 0, 1}; // c,-s,s,c for t=0

    vox::VoxWriter vox;
    vox.SetKeyFrameTimeLoggingFunctor([=](const vox::KeyFrame& vKeyFrame, const double& vValue) {  //
        std::cout << "Elapsed time for Frame " << vKeyFrame << "/" << FRAMES << " : " << vValue << " secs " << std::endl;
    });
    vox.StartTimeLogging();
    int32_t cube_color;
    double  time_step = 6.28318 / (double)FRAMES;
    for (int32_t f = 0; f < FRAMES; ++f) {
        vox.SetKeyFrame(f);
        time += time_step;
        for (int32_t i = -SIZE; i < SIZE; ++i) {
            px = ((double)i * 2.0 / (double)SIZE - 1.0) * ZOOM_XZ;
            for (int32_t k = -SIZE; k < SIZE; ++k) {
                pz   = ((double)k * 2.0 / (double)SIZE - 1.0) * ZOOM_XZ;
                an   = std::atan2(px, pz);
                cx   = mix(0.2, -0.5, std::sin(an * 2.0));
                cy   = mix(0.5, 0.0, std::sin(an * 3.0));
                path     = sqrt(px * px + pz * pz) - 3.0;
                rot2D[0] = std::cos(an + time);
                rot2D[1] = -std::sin(an + time);
                rot2D[2] = std::sin(an + time);
                rot2D[3] = std::cos(an + time);
                for (int32_t j = -SIZE; j < SIZE; ++j) {
                    tmp_y = ((double)j * 2.0 / (double)SIZE - 1.0) * ZOOM_Y;
                    tmp_x = path;
                    rev_x = rot2D[0] * tmp_x + rot2D[1] * tmp_y; // rx2d
                    rev_y = rot2D[2] * tmp_x + rot2D[3] * tmp_y; // ry2d
                    kk = 1.0;
                    hh = 1.0;
                    for (int32_t idx = 0; idx < ITERATIONS; ++idx) {
                        rev_x_squared = rev_x * rev_x;
                        rev_y_squared = rev_y * rev_y;
                        hh *= 4.0 * kk;
                        kk = rev_x_squared + rev_y_squared;
                        if (kk > 4.0) {
                            break;
                        }
                        rev_y = 2.0 * rev_x * rev_y + cy;
                        rev_x = rev_x_squared - rev_y_squared + cx;
                    }
                    df = sqrt(kk / hh) * std::log10(kk);
                    if (std::abs(df) - 0.01 < 0.0) {
                        cube_color = (int32_t)((std::sin(rev_x + rev_y) * 0.5 + 0.5) * 6.0) + 249;
                        vox.AddVoxel(i + SIZE, k + SIZE, j + SIZE, cube_color);  // magicavoxel use the z as up axis
                    }
                }
            }
        }
    }
    vox.StopTimeLogging();
    vox.SaveToFile("julia_revolute.vox");
    vox.PrintStats();
}
#endif

#ifdef USE_ANIMATED_WAVE
int main() {
    const int32_t  SIZE      = 189;
    const int32_t  OFFSET    = SIZE;
    const float    Z_SCALE   = 1.0f;
    const int32_t  FRAMES    = 30;
    const float    len_ratio = 1.0f / (SIZE * SIZE);
    vox::VoxWriter vox;
    vox.SetKeyFrameTimeLoggingFunctor([](const vox::KeyFrame& vKeyFrame, const double& vValue) {  //
        std::cout << "Elapsed time for Frame " << vKeyFrame << " : " << vValue << " secs" << std::endl;
    });
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
#endif