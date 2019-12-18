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
#include <cmath>
#include <cassert>

#include "Engine.h"
#include "Rom.h"
#include "Bus.h"
#include "Mapper.h"
#include "Mappers/Mapper0.h"
#include "Mappers/Mapper1.h"

Engine::Engine( string filename )
{
    _mapper = std::shared_ptr<Mapper>( 
        MakeMapper( Rom::Load( filename ) ) );

    _ppu = std::shared_ptr<Ppu>( 
        new Ppu( _mapper ) );

    _renderer = std::unique_ptr<Renderer>( 
        new Renderer( _ppu, _mapper ) );

    _input = std::shared_ptr<Input>( new Input() );

    _cpu = std::unique_ptr<Cpu>( 
        new Cpu( std::unique_ptr<Bus>( 
            new Bus( _mapper, _ppu, _input ) ) ) );

    _cpuCycles = 0;
    _cpu->Reset();
}

Engine::~Engine()
{

}

void Engine::Run()
{
    const int targetFPS = 60;
    const float targetFrameTime = 1000.0f / targetFPS;
    float avgFPS = 1.0;
    auto frequency = SDL_GetPerformanceFrequency();

    SDL_Event evt;
    while (1)
    {
        while (SDL_PollEvent( &evt ))
        {
            switch (evt.type)
            {
                case SDL_KEYDOWN:
                {
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
                    else if (states[SDL_SCANCODE_S] != 0)
                    {
                        _settings.ShowSprite = !_settings.ShowSprite;
                    }
                    else if (states[SDL_SCANCODE_Q] != 0 || states[SDL_SCANCODE_ESCAPE] != 0)
                    {
                        exit( 0 );
                    }
                }
                break;
            }
        }

        u64 startTicks = SDL_GetPerformanceCounter();

        RenderFrame();

        // Wait to mantain framerate:
        float frameTime = ((SDL_GetPerformanceCounter() - startTicks) * 1000.0f) / frequency;
        float deltaTarget = targetFrameTime - frameTime;

        if (deltaTarget > 0)
            SDL_Delay(deltaTarget);

        if (frameTime > 0)
            avgFPS = 0.9f * avgFPS + (1.0f - 0.9f) * (1000.0f / frameTime);

        _debugOutput.Fps = (float)frequency / (float)(SDL_GetPerformanceCounter() - startTicks);
        _debugOutput.MaxFps = avgFPS;
    }
}

void Engine::RenderFrame()
{
    Cpu    *cpu    = _cpu.get();
    Ppu    *ppu    = _ppu.get();
    //Mapper *mapper = _mapper.get();

    bool vblankDone = false;

    do
    {
        _currentFramePixel = ppu->Step();

        if ((_currentFramePixel % 3) == 0)
            _cpuCycles += cpu->Step();

        if (ppu->IsInVBlank() && !vblankDone)
        {
            vblankDone = true;

            // TODO move this when sprite caching is implemented
            //_renderer->DrawPatternTables( mapper->GetPatternTables() );

            _renderer->DrawPatternTables();

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

std::shared_ptr<Mapper> Engine::MakeMapper( std::unique_ptr<Rom> rom )
{
    switch (rom->MapperId)
    {
        case 0:
        {
            return std::shared_ptr<Mapper>( new Mapper0( std::move( rom ) ) );
            break;
        }

        case 1:
        {
            return std::shared_ptr<Mapper1>( new Mapper1( std::move( rom ) ) );
            break;
        }

        default:
            assert( false );
            break;
    }

    return nullptr;
}


