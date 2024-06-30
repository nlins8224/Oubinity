#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <array>

namespace Block
{
    
    // indices, texture coords, face coords are calculated directly in vertex shader
    
    static const uint8_t FACE_AMOUNT{ 6 };
    static const uint8_t VERTICES_PER_FACE{ 6 };

    enum block_mesh
    {
        TOP,
        BOTTOM,
        RIGHT,
        LEFT,
        FRONT,
        BACK
    };
}