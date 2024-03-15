#pragma once
// so much for pre-compiled headers being "only essential" includes..

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <random>
#include <ranges>
#include <stdexcept>
#include <stdio.h>
#include <thread>
#include <utility>

// External Libraries.. oh the linking..
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>


// Boost!
#include <boost/container_hash/hash.hpp>

// Data Structures
#include <array>
#include <deque>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Type Definitions
using u8  = uint8_t;  // unsigned char
using u16 = uint16_t; // unsigned short
using u32 = uint32_t; // unsigned long int (equal to int on 64 bit systems)
using u64 = uint64_t; // unsigned long long

using s8  = int8_t;  // char
using s16 = int16_t; // short
using s32 = int32_t; // long int (equal to int on 64 bit systems)
using s64 = int64_t; // long long

using f32 = float;  // float
using f64 = double; // double (long float)

using GLubyte = unsigned char;
using GLuint  = unsigned int;
