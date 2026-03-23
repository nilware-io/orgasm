#pragma once
// NanoLang Runtime - type aliases, containers, math, and stubs
// Generated programs #include this header.

#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <functional>
#include <algorithm>
#include <array>
#include <mutex>

// Scalar type aliases
using u8  = uint8_t;
using s8  = int8_t;
using u16 = uint16_t;
using s16 = int16_t;
using u32 = uint32_t;
using s32 = int32_t;
using u64 = uint64_t;
using s64 = int64_t;
using f32 = float;
using f64 = double;

// Math constants
constexpr f64 nano_pi  = 3.14159265358979323846;
constexpr f64 nano_e   = 2.71828182845904523536;
constexpr f64 nano_tau = 6.28318530717958647692;

// Audio output accumulator — accumulates per-sample output during on_audio_tick
inline f32 _nano_mix_accum = 0.0f;

inline void output_mix(f32 value) {
    _nano_mix_accum += value;
}

inline f32 nano_consume_mix() {
    f32 v = _nano_mix_accum;
    _nano_mix_accum = 0.0f;
    return v;
}

// Standard event handler declarations — implemented by generated code
extern void on_start(std::vector<std::string> args, std::vector<std::string> envs);
