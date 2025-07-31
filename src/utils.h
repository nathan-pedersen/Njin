#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <random>

#define i8 int8_t
#define i16 int16_t
#define i32 int32_t

#define i8max INT8_MAX
#define i16max INT16_MAX
#define i32max INT32_MAX

#define i8min INT8_MIN
#define i16min INT16_MIN
#define i32min INT32_MIN

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define u8max UINT8_MAX
#define u16max UINT16_MAX
#define u32max UINT32_MAX

#define f32 float
#define f64 double

inline i32 rand_in_range_i32(i32 min, i32 max)
{
    static std::random_device       rd;
    static std::mt19937             gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}

inline f64 rand_in_range_f64(f64 min, f64 max)
{
    static std::random_device        rd;
    static std::mt19937              gen(rd());
    std::uniform_real_distribution<> distrib(min, max);
    return distrib(gen);
}

#endif