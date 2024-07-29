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
        TOP,    // 0
        BOTTOM, // 1
        RIGHT,  // 2
        LEFT,   // 3
        FRONT,  // 4
        BACK    // 5
    };
}