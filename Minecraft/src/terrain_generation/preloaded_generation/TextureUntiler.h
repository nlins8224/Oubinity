#pragma once
#include <glm/glm.hpp>
#include "../TerrainGenerationTypes.h"
#include "../../level_of_detail/LevelOfDetail.h"
#include "PngFileReader.h"
#include "TextureGrad.h"

namespace PreloadedGeneration
{
    inline static float smoothstep(float edge0, float edge1, float x) {
        // Scale, and clamp x to 0..1 range
        x = std::clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
        return x * x * (3.0f - 2.0f * x);
    }

    inline static glm::vec2 hash2f(glm::vec2 p) {
        p = glm::vec2(dot(p, glm::vec2(127.1, 311.7)), dot(p, glm::vec2(269.5, 183.3)));
        return fract(sin(p) * 43758.5453f);
    }

    inline static glm::vec4 hash4(glm::vec2 p) {
        return glm::fract(sin(glm::vec4(1.0 + dot(p, glm::vec2(37.0, 17.0)),
            2.0 + dot(p, glm::vec2(11.0, 47.0)),
            3.0 + dot(p, glm::vec2(41.0, 29.0)),
            4.0 + dot(p, glm::vec2(23.0, 31.0)))));
    }


    inline static float smoothVoronoi(const unsigned char* texture, int width, int height, float u, float v) {
        glm::vec2 uv{ u, v };
        glm::vec2 p = floor(uv);
        glm::vec2 f = fract(uv);

        // voronoi contribution
        float va = 0.0;
        float wt = 0.0;
        for (int j = -1; j <= 1; j++) {
            for (int i = -1; i <= 1; i++) {
                glm::vec2 g = glm::vec2(float(i), float(j));
                glm::vec4 o = hash4(p + g);
                glm::vec2 r = g - f + glm::vec2{ o.x, o.y };
                float d = dot(r, r);
                float w = exp(-5.0 * d);
                glm::vec2 uv_m = uv + glm::vec2{ o.z, o.w };
                float u_m = std::min(uv_m[0], 1.0f);
                float v_m = std::min(uv_m[1], 1.0f);
                va += w * sampleTexture(texture, width, height, u_m, v_m);
                wt += w;
            }
        }


        // normalization
        return va / wt;
    }

    inline ImageBundle untile(ImageBundle png_image) {
        int width{ png_image.width }, height{ png_image.height };
   
        float incr = 1.0 / (float)width;
        int x = 0;
        int z = 0;
        for (float u = 0.0; u < 1.0; u += incr) {
            for (float v = 0.0; v < 1.0; v += incr) {
                float h = png_image.image[x * width + z];
                png_image.image[x * width + z] = h * sampleTexture(png_image.image, width, height, u, v);
                //LOG_F(INFO, "tex grad: %f", smoothVoronoi(png_image.image, width, height, u, v));
                z++;
            }
            x++;
            z = 0;
        }
        return png_image;
    }
}