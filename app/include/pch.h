#pragma once
// so much for pre-compiled headers being "only essential" includes..

#include <iomanip>
#include <iostream> 
#include <functional>
#include <memory>
#include <thread>
#include <utility>
#include <chrono>
#include <ranges>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <stdio.h>

// External Libraries.. oh the linking..
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>


// Data Structures
#include <string>
#include <stack>
#include <deque>
#include <array>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

// Type Definitions
using u8  = uint8_t;	// unsigned char
using u16 = uint16_t;	// unsigned short
using u32 = uint32_t;	// unsigned long int (equal to int on 64 bit systems)
using u64 = uint64_t;	// unsigned long long
            
using s8  = int8_t;		// char
using s16 = int16_t;	// short
using s32 = int32_t;	// long int (equal to int on 64 bit systems)
using s64 = int64_t;	// long long

using f32 = float;		// float
using f64 = double;		// double (long float)

using GLubyte = unsigned char;
using GLuint  = unsigned int;