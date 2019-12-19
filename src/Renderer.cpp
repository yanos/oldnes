//
//  Renderer.cpp
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-23.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#include <cmath>

#ifdef _MSC_VER 
    #include <stdio.h>    
    #define snprintf _snprintf
#endif

#include "Renderer.h"

// TODO: use an SDL_Color[] and get rid of _masterColors[]
static const u32 NesPalette[] =
{
    0xff7c7c7c, 0xff0000fc, 0xff0000BC, 0xff4428BC, 0xff940084, 0xffA80020, 0xffA81000, 0xff881400,
    0xff503000, 0xff007800, 0xff006800, 0xff005800, 0xff004058, 0xff000000, 0xff000000, 0xff000000,
    0xffBCBCBC, 0xff0078F8, 0xff0058F8, 0xff6844FC, 0xffD800CC, 0xffE40058, 0xffF83800, 0xffE45C10,
    0xffAC7C00, 0xff00B800, 0xff00A800, 0xff00A844, 0xff008888, 0xff000000, 0xff000000, 0xff000000,
    0xffF8F8F8, 0xff3CBCFC, 0xff6888FC, 0xff9878F8, 0xffF878F8, 0xffF85898, 0xffF87858, 0xffFCA044,
    0xffF8B800, 0xffB8F818, 0xff58D854, 0xff58F898, 0xff00E8D8, 0xff787878, 0xff000000, 0xff000000,
    0xffFCFCFC, 0xffA4E4FC, 0xffB8B8F8, 0xffD8B8F8, 0xffF8B8F8, 0xffF8A4C0, 0xffF0D0B0, 0xffFCE0A8,
    0xffF8D878, 0xffD8F878, 0xffB8F8B8, 0xffB8F8D8, 0xff00FCFC, 0xffF8D8F8, 0xff000000, 0xff000000
};

Renderer::Renderer( std::shared_ptr<Ppu> ppu, std::shared_ptr<Mapper> mapper )
{
    _mapper = mapper;
    _ppu = ppu;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        //std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
    }

    _winSizes[NormalWinSize]         = { 256, 240 };
    _winSizes[DebugWinSize]          = { 768, 512 };
    _winSizes[NormalScanlineWinSize] = { 768, 512 };
    _winSizes[DebugScanlineWinSize]  = { 768, 512 };

    // TODO: const that shit once MS likes it
    _mainScreenRect           = { 0,   0,   256, 240 };
    _bgPaletteRect            = { 0,   240, 128, 8   };
    _objPaletteRect           = { 0,   248, 128, 8   };
    _leftPatternTableRect     = { 0,   256, 128, 128 };
    _rightPatternTableRect    = { 128, 256, 128, 128 };
    _nameTableRect            = { 256, 0,   512, 480 };
    _debugOutputRect          = { 0,   384, 256, 128 };

    _nameTableSurface         = SDL_CreateRGBSurface( 0, _nameTableRect.w,         _nameTableRect.h,         8, 0, 0, 0, 0 );
    _bgPaletteSurface         = SDL_CreateRGBSurface( 0, _bgPaletteRect.w,         _bgPaletteRect.h,         8, 0, 0, 0, 0 );
    _objPaletteSurface        = SDL_CreateRGBSurface( 0, _objPaletteRect.w,        _objPaletteRect.h,        8, 0, 0, 0, 0 );
    _leftPatternTableSurface  = SDL_CreateRGBSurface( 0, _leftPatternTableRect.w,  _leftPatternTableRect.h,  8, 0, 0, 0, 0 );
    _rightPatternTableSurface = SDL_CreateRGBSurface( 0, _rightPatternTableRect.w, _rightPatternTableRect.h, 8, 0, 0, 0, 0 );
    _spriteSurface            = SDL_CreateRGBSurface( 0, _mainScreenRect.w,        _mainScreenRect.h,        8, 0, 0, 0, 0 );
    _debugOutputSurface       = SDL_CreateRGBSurface( 0, _debugOutputRect.w,       _debugOutputRect.h,       8, 0, 0, 0, 0 );

    for (int i=0; i<64; ++i)
    {
        _masterColors[i].r = (NesPalette[i] & 0xff0000) >> 16;
        _masterColors[i].g = (NesPalette[i] & 0x00ff00) >> 8;
        _masterColors[i].b = NesPalette[i] & 0x0000ff;
    }

    SetWinSize( NormalWinSize );

    TTF_Init();
    
#if defined(_WIN64)
    _courrierFont = TTF_OpenFont( "fonts/courier new.ttf", 12 );
#elif defined(__APPLE__)
    _courrierFont = TTF_OpenFont( "/Library/Fonts/Courier New.ttf", 12 );
#endif

}

Renderer::~Renderer()
{
    if (_mainWindow != nullptr)
    {
        SDL_DestroyWindow( _mainWindow );
    }
    
    if (_sdlRenderer != nullptr)
    {
        SDL_DestroyRenderer( _sdlRenderer );
    }
    
    if (_screenSurface != nullptr)
    {
        SDL_FreeSurface( _screenSurface );
    }
}

void Renderer::SetWinSize( WinSize winSize )
{
    if (_mainWindow != nullptr)
    {
        SDL_DestroyWindow( _mainWindow );
    }
    
    _mainWindow = SDL_CreateWindow(
        "OldNES NEW",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        _winSizes[winSize].x,
        _winSizes[winSize].y,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL );
    
    if (_sdlRenderer != nullptr)
    {
        SDL_DestroyRenderer( _sdlRenderer );
    }
    
    _sdlRenderer = SDL_CreateRenderer(
        _mainWindow,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    
    if (_screenSurface != nullptr)
    {
        SDL_FreeSurface( _screenSurface );
    }
    
    _screenSurface = SDL_CreateRGBSurface(
        0,
        _winSizes[winSize].x,
        _winSizes[winSize].y,
        8,
        0,
        0,
        0,
        0 );
    
    SDL_SetPaletteColors( _screenSurface->format->palette, _masterColors, 0, 64 );
}

void Renderer::PresentScreen()
{
    // clear screen
    SDL_RenderClear( _sdlRenderer );
    
    auto texture = SDL_CreateTextureFromSurface(
        _sdlRenderer,
        _screenSurface );
    
    SDL_RenderCopy( _sdlRenderer,
                    texture,
                    nullptr,
                    nullptr );
    
    SDL_RenderPresent( _sdlRenderer );
    
    // TODO update texture?
    SDL_DestroyTexture( texture );
}

void Renderer::DrawFrame( const u8* frameData, const u8* palettes )
{
    // update palettes
    SDL_Color bgColors[16], objColors[16];
    for (u8 i=0; i<0x10; ++i)
    {
        bgColors[i] = _masterColors[palettes[i] & 0x3f];
        objColors[i] = _masterColors[palettes[i+0x10] & 0x3f];
    }

    SDL_SetPaletteColors( _bgPaletteSurface->format->palette, bgColors, 0, 16 );
    SDL_SetPaletteColors( _objPaletteSurface->format->palette, objColors, 0, 16 );
    SDL_SetPaletteColors( _spriteSurface->format->palette, objColors, 0, 16 );
    SDL_SetPaletteColors( _leftPatternTableSurface->format->palette, bgColors, 0, 16 );
    SDL_SetPaletteColors( _rightPatternTableSurface->format->palette, bgColors, 0, 16 );
    SDL_SetPaletteColors( _nameTableSurface->format->palette, bgColors, 0, 16 );
    SDL_SetPaletteColors( _debugOutputSurface->format->palette, bgColors, 0, 16 );
    
    SDL_SetColorKey( _leftPatternTableSurface, SDL_RLEACCEL, 0 );
    SDL_SetColorKey( _rightPatternTableSurface, SDL_RLEACCEL, 0 );
    SDL_SetColorKey( _spriteSurface, SDL_RLEACCEL, 0 );

    SDL_FillRect( _screenSurface, nullptr, palettes[12] );

    SDL_BlitSurface( _debugOutputSurface, nullptr, _screenSurface, &_debugOutputRect );

    DrawSprites( BackPriority );

    // draw frame
    SDL_LockSurface( _screenSurface );
    
    for (int y=0; y<240; ++y)
    {
        for (int x=0; x<256; ++x)
        {
            u8 pixValue = frameData[(y * _screenBufferWidth) + x + 8];
            if ((pixValue & 3) != 0)
            {
                u8 *p = (u8*)_screenSurface->pixels
                    + (y * _screenSurface->pitch)
                    + (x * _screenSurface->format->BytesPerPixel);
            
                *p = palettes[ frameData[(y * _screenBufferWidth) + x + 8] ];
            }
        }
    }
    
    SDL_UnlockSurface( _screenSurface );

    // draw sprites on top
    DrawSprites( FrontPriority );
}

// todo: probably need to be drawn line by line to deal with overflow falg correctly
void Renderer::DrawSprites( SpritePriority priority )
{
    const u8 *oam = _ppu->GetOam();
    const u8 ppuCtrl = _ppu->GetPpuCtrl();

    // clear with color key
    SDL_FillRect( _spriteSurface, nullptr, 0 );

    bool tallSprite = ppuCtrl & 0x20;

    SDL_Surface* srcPatternSurface = (ppuCtrl & 0x8) == 0
        ? _leftPatternTableSurface
        : _rightPatternTableSurface;

    SDL_Rect src = { 0, 0, 8, 8 };
    SDL_Rect dst = { 0, 0, 8, 8 };

    SDL_LockSurface(_spriteSurface);
    SDL_LockSurface(_leftPatternTableSurface);
    SDL_LockSurface(_rightPatternTableSurface);

    for (u8 i=252; i>0; i-=4)
    {
        // byte 2
        u8 attrib = oam[i + 2];
        SpritePriority prio = (attrib & 0x20) == 0
            ? FrontPriority
            : BackPriority;

        if (prio != priority)
            continue;

        // byte 0
        dst.y = oam[i];

        // byte 1
        u8 tileNum = tallSprite 
            ? oam[i + 1] & 0xfe
            : oam[i + 1];

        if (tallSprite)
            srcPatternSurface = (oam[i + 1] & 0x1) == 0
                ? _leftPatternTableSurface
                : _rightPatternTableSurface;

        Flip flip = (Flip)(attrib >> 6);
        u8 hPalette = attrib & 0x3;

        // byte 3
        dst.x = oam[i+3];

        // blit
        src.x = (tileNum << 3) & 0x7f;          // (tileNum * 8) & 0x7f
        src.y = ((tileNum >> 4) << 3) & 0x7f;   // ((tileNum / 16) * 8) & 0x7f
        BlitSprite( srcPatternSurface, &src, &dst, flip, hPalette );

        if (tallSprite)
        {
            tileNum++;
            dst.y += 8;
            src.x = (tileNum << 3) & 0x7f;
            src.y = ((tileNum >> 4) << 3) & 0x7f;
            BlitSprite(srcPatternSurface, &src, &dst, flip, hPalette);
        }
    }

    SDL_UnlockSurface(_spriteSurface);
    SDL_UnlockSurface(_leftPatternTableSurface);
    SDL_UnlockSurface(_rightPatternTableSurface);

    // final blit on the screen
    SDL_BlitSurface( _spriteSurface, nullptr, _screenSurface, &_mainScreenRect );
}

void Renderer::BlitSprite( SDL_Surface* patternSurface,
                           SDL_Rect *src,
                           SDL_Rect *dst,
                           Flip flip,
                           u8 palette )
{
    for (u8 y=0; y<8; ++y)
    {
        if (dst->y + y >= 240)
            break;

        u8 ySrcOffset = flip & Vertical
            ? 7 - y
            : y;

        u8* pSrc = (u8*)patternSurface->pixels
            + ((src->y + ySrcOffset) * patternSurface->pitch)
            + (src->x * patternSurface->format->BytesPerPixel);

        u8* pDst = (u8*)_spriteSurface->pixels
           + ((dst->y + y) * _spriteSurface->pitch)
           + (dst->x * _spriteSurface->format->BytesPerPixel);

        for (u8 x=0; x<8; ++x)
        {
            if (dst->x + x >= 256)
                break;

            u8 xSrcOffset = flip & Horizontal
                ? 7 - x
                : x;

            u8 val = (*(pSrc + xSrcOffset)) | (palette << 2);
            if ((val & 0x3) != 0)
                *(pDst + x) = val;
        }
    }
}

void Renderer::DrawDebugOutput( const DebugOutput &debugOutput )
{
    auto txtColor = _masterColors[60];

    const u8 buffSize = 64;
    char buffer[buffSize];

    _snprintf_s( buffer,
                buffSize,
                "fps %.1f max fps %.1f",
                debugOutput.Fps,
                debugOutput.MaxFps );

    auto txtSurface = TTF_RenderText_Solid( _courrierFont, buffer, txtColor );

    SDL_BlitSurface( txtSurface, nullptr, _screenSurface, nullptr );
    SDL_FreeSurface( txtSurface );
}

void Renderer::DrawPalettes( const u8* palettes )
{
    (palettes);

    //if (_ppu->IsPaletteDirty())
    {
        SDL_LockSurface( _bgPaletteSurface );
        SDL_LockSurface( _objPaletteSurface );

        for (u8 palIdx = 0; palIdx < 0x10; ++palIdx)
        {
            u16 offsetX = 8 * (palIdx & 0xf);

            for (u8 y = 0; y < 8; ++y)
            {
                for (u8 x = 0; x < 8; ++x)
                {
                    u8 *bgP = (u8*)_bgPaletteSurface->pixels
                        + (y * _bgPaletteSurface->pitch)
                        + (x + offsetX) * _bgPaletteSurface->format->BytesPerPixel;

                    u8 *objP = (u8*)_objPaletteSurface->pixels
                        + (y * _objPaletteSurface->pitch)
                        + (x + offsetX) * _objPaletteSurface->format->BytesPerPixel;

                    *bgP = palIdx;
                    *objP = palIdx;
                }
            }
        }

        SDL_UnlockSurface( _bgPaletteSurface );
        SDL_UnlockSurface( _objPaletteSurface );
    }

    SDL_BlitSurface( _bgPaletteSurface, nullptr, _screenSurface, &_bgPaletteRect );
    SDL_BlitSurface( _objPaletteSurface, nullptr, _screenSurface, &_objPaletteRect );
}

void Renderer::DrawPatternTables()
{
    //if (_ppu->IsPaletteDirty())
    {
        SDL_LockSurface( _leftPatternTableSurface );
        SDL_LockSurface( _rightPatternTableSurface );

        u8* pixelPtr;
        u8 patternLine[8];
        for (u16 i=0; i<0x800; ++i)
        {
            u16 tileStart = ((i/8) * 16) + (i & 0x7); // TODO: simplify!

            u16 x = i & 0x78; // aka ((i/8)*8) & 0x7f aka increment by 8 and wrap at 128
            u16 y = (i & 7) + (8 * ((i & ~7) / 128)); // TODO: simplifly!

            // left pattern table
            DecodePatternLine( patternLine,
                               _mapper->ReadChr( tileStart ),
                               _mapper->ReadChr( tileStart + 8 ) );
            pixelPtr = 
                (u8*)_leftPatternTableSurface->pixels
                    + y * _leftPatternTableSurface->pitch
                    + x * _leftPatternTableSurface->format->BytesPerPixel;

            std::memcpy( pixelPtr, patternLine, 8 );

            // right pattern table
            DecodePatternLine( patternLine,
                               _mapper->ReadChr( 0x1000 + tileStart ),
                               _mapper->ReadChr( 0x1000 + tileStart + 8 ) );
            pixelPtr = 
                (u8*)_rightPatternTableSurface->pixels
                    + y * _rightPatternTableSurface->pitch
                    + x * _rightPatternTableSurface->format->BytesPerPixel;

            std::memcpy( pixelPtr, patternLine, 8 );
        }

        SDL_UnlockSurface( _leftPatternTableSurface );
        SDL_UnlockSurface( _rightPatternTableSurface );
    }

    SDL_BlitSurface( _leftPatternTableSurface, nullptr, _screenSurface, &_leftPatternTableRect);
    SDL_BlitSurface( _rightPatternTableSurface, nullptr, _screenSurface, &_rightPatternTableRect );
}

void Renderer::DrawNameTables( const u8* nameTables, const u8 ppuCtrl )
{
    //if (_ppu->IsNameTableDirty())
    {
        SDL_Surface* patternSurface = _leftPatternTableSurface;
        if ((ppuCtrl & 0x10) != 0)
        {
            patternSurface = _rightPatternTableSurface;
        }

        SDL_Rect src = { 0, 0, 8, 8 };
        SDL_Rect dst = { 0, 0, 8, 8 };

        for (u8 nt=0; nt<4; ++nt)
        {
            // name tables
            for (int i=0; i<0x3c0; ++i)
            {
                u8 ptAddr = nameTables[i + (nt * 0x400)];
                
                // TODO: simplify this shit
                src.x = (ptAddr * 8) & 0x7f;
                src.y = ((ptAddr / 16) * 8) & 0x7f;

                dst.x = ((i * 8) & 0xff) + ((nt & 0x1) * 256);
                dst.y = ((i / 32) * 8) + (((nt & 0x2) >> 1) * 240);

                SDL_BlitSurface( patternSurface, &src, _nameTableSurface, &dst );
            }

            // attribute tables
            SDL_LockSurface( _nameTableSurface );

            u16 ntXOffset = 256 * (nt & 1);
            u16 ntYOffset = 240 * (nt >> 1);

            for (u16 i=0; i<0x40; ++i)
            {
                u8 attrib = nameTables[i + 0x3c0 + (nt * 0x400)];
                u8 tl = attrib & 0x3 << 2;
                u8 tr = (attrib & 0xc);
                u8 bl = (attrib & 0x30) >> 2;
                u8 br = (attrib & 0xc0) >> 4;

                u16 xOffset = ((i * 32) & 0xff) + ntXOffset;
                u16 yOffset = ((i / 8) * 32) + ntYOffset;
                
                // aarrff! there's probably a more efficient way
                u16 yLimit = 240 * ((nt >> 1) ^ 1);
                for (u8 y=0; y<32 && y < yLimit; ++y)
                {
                    for (u8 x=0; x<32; ++x)
                    {
                        u8* p = (u8*)_nameTableSurface->pixels
                            + ((y + yOffset) * _nameTableSurface->pitch)
                            + ((x + xOffset) * _nameTableSurface->format->BytesPerPixel);
                        
                        u8 val = 0;
                        if (x < 16 && y < 16)
                            val = (*p) | tl;
                        else if (x >= 16 && y < 16)
                            val = (*p) | tr;
                        else if (x < 16 && y >= 16)
                            val = (*p) | bl;
                        else
                            val = (*p) | br;
                        
                        *p = (val & 0x3) == 0
                            ? 0
                            : val;
                    }
                }
            }

            SDL_UnlockSurface( _nameTableSurface );
        }
    }

    SDL_BlitSurface( _nameTableSurface, nullptr, _screenSurface, &_nameTableRect );
}

inline void Renderer::DecodePatternLine( u8 result[8], u8 byte1, u8 byte2 )
{
    result[0] = ( (byte1 & 0x80) >> 7 ) | ( (byte2 & 0x80) >> 6 );
    result[1] = ( (byte1 & 0x40) >> 6 ) | ( (byte2 & 0x40) >> 5 );
    result[2] = ( (byte1 & 0x20) >> 5 ) | ( (byte2 & 0x20) >> 4 );
    result[3] = ( (byte1 & 0x10) >> 4 ) | ( (byte2 & 0x10) >> 3 );
    result[4] = ( (byte1 & 0x8)  >> 3 ) | ( (byte2 & 0x8)  >> 2 );
    result[5] = ( (byte1 & 0x4)  >> 2 ) | ( (byte2 & 0x4)  >> 1 );
    result[6] = ( (byte1 & 0x2)  >> 1 ) | (  byte2 & 0x2        );
    result[7] = (  byte1 & 0x1        ) | ( (byte2 & 0x1)  << 1 );
}
