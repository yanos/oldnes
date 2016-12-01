//
//  Engine.cpp
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#include <chrono>
#include <thread>
#include <numeric>

#include "Engine.h"
#include "Rom.h"
#include "Bus.h"
#include "Mapper.h"

u32 globalTickStart = 0;

u32 Step( u32 elapsed, void* param )
{
    // provoke an user event so that the main loop
    // call RenderFrames. This is done to prevent
    // multithreading problem with sdl
    SDL_Event event;
    SDL_UserEvent userevent;
    
    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;
    
    event.type = SDL_USEREVENT;
    event.user = userevent;
    
    SDL_PushEvent( &event );
    
    return elapsed;
}

Engine::Engine( string filename )
{
    auto rom = Rom::Load( filename );
    _ppu = std::shared_ptr<Ppu>( new Ppu() );
    _renderer = std::shared_ptr<Renderer>(new Renderer( _ppu ));
    auto mapper = std::shared_ptr<Mapper>( new Mapper( rom, _ppu ) );
    _input = std::shared_ptr<Input>( new Input() );
    auto bus = std::shared_ptr<Bus>( new Bus( mapper, _ppu, _input ) );
    _cpu = std::shared_ptr<Cpu>( new Cpu( bus ) );
    
    _cpuCycles = 0;
    _cpu->Reset();
}

void Engine::Run()
{
    bool keyTrigger = true;
    int framesToRender = 1;
    SDL_AddTimer( 16.666666 * framesToRender, Step, nullptr );
    
    SDL_Event evt;
    while (1)
    {
        u32 startTicks = SDL_GetTicks();
        
        /*while (SDL_PollEvent(&evt))
        {
            switch(evt.type)
            {
                case SDL_USEREVENT:
                    RenderFrames( framesToRender );
                    break;
            }
        }
        
        RenderFrames(framesToRender);
        
        while (SDL_GetTicks() - startTicks < (1000 / 60))
        {
            std::this_thread::yield();
        }*/
        
        
        SDL_WaitEvent( &evt );
        
        switch(evt.type)
        {
            case SDL_USEREVENT:
                RenderFrames( framesToRender );
                break;
                
            case SDL_KEYUP:
                keyTrigger = true;
                break;
                
            case SDL_KEYDOWN:
                if (keyTrigger)
                {
                    keyTrigger = false;
                    
                    auto states = SDL_GetKeyboardState( nullptr );
                    if (states[SDL_SCANCODE_V] != 0)
                    {
                        _settings.ShowDebugViews = !_settings.ShowDebugViews;
                    
                        auto winSize = _settings.ShowDebugViews
                            ? Renderer::DebugWinSize
                            : Renderer::NormalWinSize;
                    
                        _renderer->SetWinSize( winSize );
                    }
                    else if (states[SDL_SCANCODE_O] != 0)
                    {
                        _settings.ShowDebugOutput = !_settings.ShowDebugOutput;
                    }
                    else if (states[SDL_SCANCODE_L] != 0)
                    {
                        _settings.Scanline = !_settings.Scanline;
                    }
                    else if (states[SDL_SCANCODE_Q] != 0)
                    {
                        exit( 0 );
                    }
                }
                
                break;
        }
        
        int deltaTicks = SDL_GetTicks() - startTicks;
        if (deltaTicks > 0)
        {
            _fpsValues.push_front( (framesToRender * 1000.0f) / deltaTicks );
            if (_fpsValues.size() > _maxFpsValues)
                _fpsValues.pop_back();

            float sum = std::accumulate( _fpsValues.begin(),
                                         _fpsValues.end(),
                                         0.0f );

            _debugOutput.Fps = sum / _fpsValues.size();
        }
    }
}

void Engine::RenderFrames( u32 framesToRender )
{
    u32 tickStart = SDL_GetTicks();

    Cpu *cpu = _cpu.get();
    Ppu *ppu = _ppu.get();

    bool vblankDone = false;

    for (u32 i=0; i<framesToRender; ++i)
    {
        do 
        {
            _currentFramePixel = ppu->Step();

            if ((_currentFramePixel % 3) == 0)
                _cpuCycles += cpu->Step();

            if (ppu->IsInVBlank() && !vblankDone)
            {
                vblankDone = true;
                
                // TODO move this when sprite caching is implemented
                _renderer->DrawPatternTables( ppu->GetPatternTables() );
                
                _renderer->DrawFrame( ppu->GetFrameData(), ppu->GetPalettes() );

                if (_settings.ShowDebugViews)
                {
                    _renderer->DrawPalettes( ppu->GetPalettes() );
                    _renderer->DrawNameTables( ppu->GetNameTables(),
                                               ppu->GetPpuCtrl() );
                }
                
                if (_settings.ShowDebugOutput)
                {
                    _renderer->DrawDebugOutput( _debugOutput );
                }
                
                _renderer->PresentScreen();
                
                _input->ReadInputs();

                if (ppu->NmiAtVBlank())
                    cpu->Nmi();
            }
            else if (!ppu->IsInVBlank() && vblankDone)
            {
                vblankDone = false;
            }

        } while (_currentFramePixel != 0);
    }

    u32 deltaTicks = SDL_GetTicks() - tickStart;
    if (deltaTicks > 0)
    {
        _internalFpsValues.push_front(
            (framesToRender * 1000.0f) / deltaTicks );
    
        if (_internalFpsValues.size() > _maxFpsValues)
            _internalFpsValues.pop_back();
    
        float sum = 0;
        float min = std::numeric_limits<float>::max();
        float max = std::numeric_limits<float>::min();
    
        for (auto f : _internalFpsValues)
        {
            sum += f;
            if (f < min) min = f;
            if (f > max) max = f;
        }
    
        _debugOutput.InternalFps = sum / _internalFpsValues.size();
        _debugOutput.MinFps = min;
        _debugOutput.MaxFps = max;
    }
}


