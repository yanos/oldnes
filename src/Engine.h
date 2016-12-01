//
//  Engine.h
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#ifndef __OldNES__Engine__
#define __OldNES__Engine__

#include <list>
#include <stdio.h>
#include <string>

#include "Cpu.h"
#include "Ppu.h"
#include "Input.h"
#include "Renderer.h"

class Engine
{
    
public:
    Engine( string );
    ~Engine();

    void Run();
    void RenderFrames( u32 nbFrames );

private:
    std::shared_ptr<Cpu>        _cpu;
    std::shared_ptr<Ppu>        _ppu;
    std::shared_ptr<Input>      _input;
    std::shared_ptr<Renderer>   _renderer;
    
    std::list<float>            _fpsValues;
    std::list<float>            _internalFpsValues;

    DebugOutput                 _debugOutput;
    Settings                    _settings;
    
    u64                         _cpuCycles = 0;
    u32                         _ppuCycles = 0;
    u32                         _currentFramePixel = 0;
    u16                         _curScanlinePixel = 0;
    
    const int                   _maxFpsValues = 100;
};

#endif /* defined(__OldNES__Engine__) */
