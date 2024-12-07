#ifndef STANDARD_INCLUDES_H
#define STANDARD_INCLUDES_H

#define SDL_MAIN_HANDLED

#define NATIVE_XRES 1920
#define NATIVE_YRES 1080

// Include standard headers
#include <string>
#include <vector>
#include <iostream>
#include <cinttypes>
#include <stdio.h>
#include <thread>
// Code for WEEK 4 for ASSET MANAGEMENT
#include <map> // WEEK 4
// Code for Pool Based Allocation
#include <fstream>
// Code for week 6 Graphics Core 1
#include <SDL.h>
// Code for week 7 Graphics Core 2
#include <SDL_ttf.h>
#include <SDL_pixels.h>
// Code for Week 10 Audio Core
#include <SDL_mixer.h>
#include <algorithm>

// Project headers
//#include "Singleton.h"
//#include "BasicStructs.h"

// OS specific includes and defines
#ifdef _WIN32
#include <Windows.h>
#include <direct.h>
#define M_ASSERT(_cond, _msg) \
    if (!(_cond)) { OutputDebugStringA(_msg); std::abort(); }
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

using namespace std;

#endif // STANDARD_INCLUDES_H