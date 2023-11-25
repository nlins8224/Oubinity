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
        BACK,
        FRONT,
        LEFT,
        RIGHT,
        TOP,
        BOTTOM
    };

    /*
    Real shading values will be obtained by dividing by 5 shading values in vertex shader
    [2, 3, 4, 5] values can be stored using 3 bits, which wouldn't be possible with floats.

    Example: 2 / 5.0 = 0.4, thus 2 value represents 0.4 value
    */

    // TODO: Move to vertex shader
    static const uint8_t shading[FACE_AMOUNT]
    {
        4, // BACK
        4, // FRONT
        3, // LEFT
        3, // RIGHT
        5,  // TOP
        2 // BOTTOM
    };
}