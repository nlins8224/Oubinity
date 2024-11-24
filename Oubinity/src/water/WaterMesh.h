#pragma once
#include <array>

namespace Water {

constexpr uint8_t VERTICES_AMOUNT = 6;
const std::array<float, VERTICES_AMOUNT * 5> vertices{
    //  x    y    z    u    v
    0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0};
}  // namespace Water