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

    /*
    xyz coords

    FRONT FACE: 
    (0,1,1)  (1,1,1)
      |-------|
      |       |
      |       |
      |-------|
    (0,0,1)  (1,0,1)

    left-bottom-near  = (0, 0, 1)
    right-bottom-near = (1, 0, 1)
    right-top-near    = (1, 1, 1)
    left-top-near     = (0, 1, 1)

    left-bottom-far   = (0, 0, 0)
    right-bottom-far  = (1, 0, 0)
    right-top-far     = (1, 1, 0)
    left-top-far      = (0, 1, 0)

    */

    const std::array<float, FACE_SIZE> BACK_FACE
    { //  x      y      z      u     v     w   shading
        0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.8f, // left-bottom-far
        1.0f,  1.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0.8f, // right-top-far
        1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f, 0.8f, // right-bottom-far     
        1.0f,  1.0f, 0.0f,  1.0f, 1.0f, 0.0f, 0.8f, // right-top-far
        0.0f,  0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.8f, // left-bottom-far
        0.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.8f  // left-top-far
    };

    const std::array<float, FACE_SIZE> FRONT_FACE
    {
        0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.8f, // left-bottom-near
        1.0f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.8f, // right-bottom-near
        1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.8f, // right-top-near
        1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.8f, // right-top-near
        0.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.8f, // left-top-near
        0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.8f  // left-bottom-near
    };

    const std::array<float, FACE_SIZE> LEFT_FACE
    {
        0.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.6f, // left-top-near
        0.0f,  1.0f,  0.0f,  1.0f, 1.0f, 0.0f, 0.6f, // left-top-far
        0.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.6f, // left-bottom-far
        0.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.6f, // left-bottom-far
        0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.6f, // left-bottom-near
        0.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.6f  // left-top-near
    };

    const std::array<float, FACE_SIZE> RIGHT_FACE
    {
        1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.6f, // right-top-near
        1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.6f, // right-bottom-far
        1.0f,  1.0f,  0.0f,  1.0f, 1.0f, 0.0f, 0.6f, // right-top-far      
        1.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.6f, // left-bottom-far
        1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.6f, // right-top-near
        1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.6f  // right-bottom-near
    };

    const std::array<float, FACE_SIZE> BOTTOM_FACE
    {
        0.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.4f, // left-bottom-far
        1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, 0.4f, // right-bottom-far
        1.0f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.4f, // right-bottom-near
        1.0f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.4f, // right-bottom-near
        0.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.4f, // left-bottom-near
        0.0f,  0.0f,  0.0f,  0.0f, 1.0f, 0.0f, 0.4f  // left-bottom-far
    };

    const std::array<float, FACE_SIZE> TOP_FACE
    {
        0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f, // left-top-far
        1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // right-top-near
        1.0f,  1.0f,  0.0f,  1.0f, 1.0f, 0.0f, 1.0f, // right-top-far
        1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // right-top-near
        0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f, // left-top-far
        0.0f,  1.0f,  1.0f,  0.0f, 0.0f, 0.0f, 1.0f  // left-top-near
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