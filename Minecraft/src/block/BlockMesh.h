#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <array>

namespace Block
{
    static const int FACE_SIZE{ 6 * 7 };
    static const int FACE_AMOUNT{ 6 };
    static const int FACE_ROWS{ 6 };
    static const int FACE_COLUMNS{ 6 };

    const std::array<float, FACE_SIZE> BACK_FACE
    { //  x      y      z      u     v     w   shading
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.8f, 
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.8f, 
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.8f,       
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.8f, 
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f, 0.8f, 
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.8f 
    };

    const std::array<float, FACE_SIZE> FRONT_FACE
    {
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.8f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.8f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 0.8f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 0.8f,
       -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.8f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.8f
    };

    const std::array<float, FACE_SIZE> LEFT_FACE
    {
       -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.6f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.6f,
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.6f,
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.6f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.6f,
       -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.6f
    };

    const std::array<float, FACE_SIZE> RIGHT_FACE
    {
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.6f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.6f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.6f,      
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.6f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.6f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.6f
    };

    const std::array<float, FACE_SIZE> BOTTOM_FACE
    {
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.4f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 0.4f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.4f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.4f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 0.4f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.4f
    };

    const std::array<float, FACE_SIZE> TOP_FACE
    {
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f, 1.0f, 
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f
    };

    enum block_mesh
    {
        BACK,
        FRONT,
        LEFT,
        RIGHT,
        BOTTOM,
        TOP
    };

    const std::array<std::array<float, FACE_SIZE>, FACE_AMOUNT> faces
    {
        BACK_FACE,
        FRONT_FACE,
        LEFT_FACE,
        RIGHT_FACE,
        BOTTOM_FACE,
        TOP_FACE
    };
}