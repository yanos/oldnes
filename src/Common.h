//
//  Common.h
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#ifndef OldNES_Common_h
#define OldNES_Common_h

#include <string>

#include "DataTypes.h"
#include "Common.h"

using string = std::wstring;

// TODO move theses in contants.h or something
static const u32   _pixelPerScanline  = 341;
static const u32   _pixelPerCpuCycles = 3;
static const u32   _vblankScanline    = 241;
static const u32   _dummyScanline     = 261;
static const u32   _scanlinePerFrame  = 262;
static const float _frameDuration     = 16.6667f;

static const u32   _screenBufferWidth = 272;
static const u32   _screenBufferHeight = 256;

enum MirroringMode { Horizontal = ~0x400, Vertical = ~0x800, OneScreen, FourScreen };

struct DebugOutput
{
    float Fps;
    float MaxFps;
};

struct Settings
{
    bool ShowDebugOutput = true;
    bool ShowDebugViews = false;
    bool Scanline = false;
    bool ShowSprite = true;
};

#endif