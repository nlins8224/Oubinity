#pragma once
namespace Util {
static inline int getMod(int pos, int mod) { return ((pos % mod) + mod) % mod; }
}  // namespace Util
