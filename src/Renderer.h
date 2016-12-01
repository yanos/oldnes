//
//  Renderer.h
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-23.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#ifndef __OldNES__Renderer__
#define __OldNES__Renderer__

#include <stdio.h>
#include <memory>
#include <SDL.h>
#include <SDL_ttf.h>

#include "Common.h"
#include "Ppu.h"

class Renderer
{
    
public:
    
    enum WinSize { NormalWinSize = 0, DebugWinSize, NormalScanlineWinSize, DebugScanlineWinSize, MaxWinSize };
    enum SpritePriority { FrontPriority, BackPriority };

    Renderer( std::shared_ptr<Ppu> );
    ~Renderer();

    void DrawFrame( const u8* frameData, const u8* palettes );

    void DrawPalettes( const u8* palettes );
    void DrawPatternTables( const u8* patternTables );
    void DrawNameTables( const u8* nameTables, const u8 ppuCtrl );
    void DrawSprites( SpritePriority priority );
    void DrawDebugOutput( const DebugOutput &debugOutput );

    void PresentScreen();
    
    void SetWinSize( WinSize winSize );
    
private:
    
    enum Flip { None = 0, Horizontal = 1, Vertical = 2 };

    void BlitSprite( SDL_Surface*, SDL_Rect*, SDL_Rect*, Flip, u8 );
    u8* DecodePatternLine( u8, u8 );

    std::shared_ptr<Ppu> _ppu;

    SDL_Window*          _mainWindow = nullptr;
    
    TTF_Font*            _courrierFont = nullptr;
    
    SDL_Rect             _mainScreenRect;
    SDL_Rect             _bgPaletteRect;
    SDL_Rect             _objPaletteRect;
    SDL_Rect             _leftPatternTableRect;
    SDL_Rect             _rightPatternTableRect;
    SDL_Rect             _nameTableRect;
    SDL_Rect             _debugOutputRect;
    
    SDL_Point            _winSizes[MaxWinSize];
                         
    SDL_Color            _masterColors[64];
                         
    SDL_Renderer*        _sdlRenderer = nullptr;
    SDL_Surface*         _screenSurface = nullptr;
    SDL_Surface*         _bgPaletteSurface = nullptr;
    SDL_Surface*         _objPaletteSurface = nullptr;
    SDL_Surface*         _leftPatternTableSurface = nullptr;
    SDL_Surface*         _rightPatternTableSurface = nullptr;
    SDL_Surface*         _nameTableSurface = nullptr;
    SDL_Surface*         _spriteSurface = nullptr;
    SDL_Surface*         _debugOutputSurface = nullptr;
};

#endif /* defined(__OldNES__Renderer__) */
