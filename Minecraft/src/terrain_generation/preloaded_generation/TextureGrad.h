#pragma once
#include "../../Util.h"
namespace PreloadedGeneration
{
    static inline int index(int x, int y, int width) {
        return x * width + y;
    }

    static inline float lerp(float a, float b, float t) {
        return (1 - t) * a + t * b;
    }

    inline float sampleTexture(const unsigned char* texture, int width, int height, float u, float v) {
        // Normalize texture coordinates to pixel indices
        float x = u * (width);  // [0,1]*[0,k]
        float y = v * (height); 

        // Determine the four surrounding pixel indices
        int x0 = static_cast<int>(std::floor(x));
        int y0 = static_cast<int>(std::floor(y));
        int x1 = Util::getMod(x0 - 1, width);
        int y1 = Util::getMod(y0 - 1, height);

        // Compute the fractional parts for interpolation
        float uFrac = x / width;
        float vFrac = y / height;

        // Fetch nine neighboring pixel values
        float c00 = texture[index(x0, y0, width)] / 255.0f;
        float c10 = texture[index(x1, y0, width)] / 255.0f;
        float c01 = texture[index(x0, y1, width)] / 255.0f;
        float c11 = texture[index(x1, y1, width)] / 255.0f;

        // Bilinear interpolation
        float blendedX1 = lerp(c00, c10, 0.5f);
        float blendedX2 = lerp(c01, c11, 0.5f);
        return lerp(blendedX1, blendedX2, vFrac);
    }
}