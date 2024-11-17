#pragma once
#include <cmath>
#include <algorithm>
#include "../TerrainGenerationTypes.h"

namespace PreloadedGeneration {
    static inline unsigned char bilinearInterpolateGrayscale(
        const unsigned char* src, int src_width, int src_height,
        float x, float z
    ) {
        int x1 = std::floor(x);
        int z1 = std::floor(z);
        int x2 = std::min(x1 + 1, src_width - 1);
        int z2 = std::min(z1 + 1, src_height - 1);

        float a = x - x1;
        float b = z - z1;

        auto get_pixel = [&](int px, int pz) -> unsigned char {
            return src[px * src_width + pz];
            };

        unsigned char p1 = get_pixel(x1, z1);
        unsigned char p2 = get_pixel(x2, z1);
        unsigned char p3 = get_pixel(x1, z2);
        unsigned char p4 = get_pixel(x2, z2);

        return static_cast<unsigned char>(
            (1 - a) * (1 - b) * p1 +
            a * (1 - b) * p2 +
            (1 - a) * b * p3 +
            a * b * p4
            );
    }

    inline ImageBundle resizeImageGrayscale(
        ImageBundle src, int dst_width, int dst_height) {
        ImageBundle dst_img{
            .width = dst_width,
            .height = dst_height,
            .channels = 1,
            .image = new unsigned char[dst_height * dst_width]
        };
        dst_img.height = dst_height;
        dst_img.width = dst_width;

        float x_scale = static_cast<float>(src.width) / dst_width;
        float z_scale = static_cast<float>(src.height) / dst_height;

        for (int x = 0; x < dst_width; ++x) {
            for (int z = 0; z < dst_height; ++z) {
                float src_x = x * x_scale;
                float src_z = z * z_scale;
                dst_img.image[x * dst_width + z] = bilinearInterpolateGrayscale(src.image, src.width, src.height, src_x, src_z);
            }
        }
        return dst_img;
    }

}