//
//  Ppu.h
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#ifndef __OldNES__Ppu__
#define __OldNES__Ppu__

#include <stdio.h>

#include "Common.h"
#include "Mapper.h"

class Ppu
{
public:
    Ppu( std::shared_ptr<Mapper> mapper );
    ~Ppu();

    byte ReadByte( addr );
    word ReadWord( addr );
    void WriteByte( addr, byte );

    u32 Step();

    void VBlankStart();
    void VBlankStop();
    void WriteOam( addr address, byte value )
        {
            _sprite0FlagDirty = _sprite0FlagDirty || address < 4;
            
            _oamData[address] = value;

            //if (address >= 0x100)
            //{
            //    int patate = 0;
            //}
        }

    bool IsInVBlank()                                       { return (_ppuStatus & 0x80) > 0; }
    bool NmiAtVBlank()                                      { return (_ppuCtrl & 0x80) > 0;   }

    const u8  GetPpuCtrl()                                  { return _ppuCtrl;                }
    const u8  GetPpuMask()                                  { return _ppuMask;                }
    const u8  GetPpuStatus()                                { return _ppuStatus;              }
    const u8* GetPalettes()                                 { return _palettes;               }
    //const u8* GetPatternTables()                            { return _paternTables;           }
    const u8* GetNameTables()                               { return _nameTables;             }
    const u8* GetOam()                                      { return _oamData;                }
    const u16 GetOamAddr()                                  { return _oamAddr;                }

    const u8*  GetFrameData()                               { return _frameData;              }

    const bool IsPaletteDirty()                             { return _paletteDirty;           }
    const bool IsPatternTableDirty()                        { return _patternTableDirty;      }
    const bool IsNameTableDirty()                           { return _nameTableDirty;         }

    const u16  GetCurrentScanline()                         { return _currentScanline;        }
    const u32  GetCurrentFramePixel()                       { return _currentFramePixel;      }

private:

    struct TileBuffer
    {
        u16 Scanline = 0;
        u16 ScanlinePixel = 0;

        u16 NtAddr = 0;
        u8  NtByte = 0;

        u16 AttribAddr = 0;
        u8  AttribByte = 0;
        
        u8  LowPatternByte = 0;
        u8  HighPatternByte = 0;
    };

    inline void IncHorizontalVramAddr()
    {
        // The coarse X component of v needs to be incremented when the next tile is reached. 
        // Bits 0-4 are incremented, with overflow toggling bit 10. 
        // This means that bits 0-4 count from 0 to 31 across a single nametable, 
        // and bit 10 selects the current nametable horizontally.
        if ((_vramAddr & 0x1f) == 31)   // if coarse X == 31
        {
            _vramAddr &= ~0x1f;         // coarse X = 0
            _vramAddr ^= 0x400;         // switch horizontal nametable
        }
        else
        {
            ++_vramAddr;                // increment coarse X
        }
    }

    void FlushTileBuffer();
    void WriteVramByte( addr, byte );
    byte ReadVramByte( addr );

    static const u16        _patternTableSize = 0x1000;
    static const u16        _nameTableSize = 0x400;

    std::shared_ptr<Mapper> _mapper;

    u32                     _currentFramePixel = 261 * _pixelPerScanline;
    u16                     _currentScanlinePixel = 0;
    u16                     _currentScanline = 261;
    u16                     _firstNonZeroSpriteX = 0xffff;
    u16                     _firstNonZeroSpriteY = 0xffff;

    byte                    _oamData[0x100];
    addr                    _oamAddr = 0;

    u8                      _ppuCtrl = 0;
    u8                      _ppuMask = 0;
    u8                      _ppuStatus = 0;
                         
    u8                      _frameData[_screenBufferWidth * _screenBufferHeight]; // 2 extra rows to remove some ifs

    u8                      _palettes[0x20];
    u8                      _nameTables[_nameTableSize * 4];
                         
    u8                      _fineXScroll = 0;              // 3 bits

    TileBuffer              _tileBuffer;

    addr                    _vramAddr = 0;                 // 15 bits
    addr                    _tmpVramAddr = 0;              // 15 bits
    byte                    _vramReadBuffer = 0;
    bool                    _firstWrite = true;

    bool                    _sprite0FlagDirty = true;
    bool                    _spriteDirty = true;
    bool                    _nameTableDirty = true;
    bool                    _patternTableDirty = true;
    bool                    _paletteDirty = true;
};

#endif /* defined(__OldNES__Ppu__) */
