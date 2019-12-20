//
//  Ppu.cpp
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#include "Ppu.h"

#include <cassert>

Ppu::Ppu( std::shared_ptr<Mapper> mapper )
{
    _mapper = mapper;
}

Ppu::~Ppu() { }

void Ppu::FlushTileBuffer()
{
    u8 paletteMask = 0x3;

    // shiftAmout = 0 or 2 or 4 or 6
    u8 shiftAmount = (((_tileBuffer.NtAddr / 64) & 0x1) * 4)     // 0 or 4
        + (_tileBuffer.NtAddr & 0x2);                            // 0 or 2

    paletteMask <<= shiftAmount;
    u16 paletteHighByte = (_tileBuffer.AttribByte & paletteMask) << 8;

    // shift by 6, 8, 10 or 12
    paletteHighByte >>= (shiftAmount + 6);

    u32 yOffset = _tileBuffer.Scanline * _screenBufferWidth;

    // we can now draw 8 pixels
    int xOffset = _tileBuffer.ScanlinePixel - _fineXScroll - 1 + 8;

    u8 lowPtByte = _tileBuffer.LowPatternByte;
    u8 highPtByte = _tileBuffer.HighPatternByte;

    u8 pixValues[8];

    pixValues[0] = ( (lowPtByte & 0x80) >> 7 )
                 | ( (highPtByte & 0x80) >> 6 ) 
                 | paletteHighByte;

    pixValues[1] = ( (lowPtByte & 0x40) >> 6 )
                 | ( (highPtByte & 0x40) >> 5 ) 
                 | paletteHighByte;

    pixValues[2] = ( (lowPtByte & 0x20) >> 5 )
                 | ( (highPtByte & 0x20) >> 4 ) 
                 | paletteHighByte;

    pixValues[3] = ( (lowPtByte & 0x10) >> 4 )
                 | ( (highPtByte & 0x10) >> 3 ) 
                 | paletteHighByte;

    pixValues[4] = ( (lowPtByte & 0x8)  >> 3 )
                 | ( (highPtByte & 0x8)  >> 2 ) 
                 | paletteHighByte;

    pixValues[5] = ( (lowPtByte & 0x4)  >> 2 )
                 | ( (highPtByte & 0x4)  >> 1 ) 
                 | paletteHighByte;

    pixValues[6] = ( (lowPtByte & 0x2)  >> 1 )
                 | (  highPtByte & 0x2 ) 
                 | paletteHighByte;

    pixValues[7] = ( lowPtByte & 0x1 )
                 | ( (highPtByte & 0x1)  << 1 ) 
                 | paletteHighByte;

    std::memcpy( &_frameData[xOffset + yOffset], pixValues, 8 );

    if (    _tileBuffer.Scanline == _firstNonZeroSpriteY + 1
         && xOffset > _firstNonZeroSpriteX
         && ((pixValues[0] & 0x3) != 0 || (pixValues[1] & 0x3) != 0 || (pixValues[2] & 0x3) != 0 || (pixValues[3] & 0x3) != 0 || (pixValues[4] & 0x3) != 0 || (pixValues[5] & 0x3) != 0 || (pixValues[6] & 0x3) != 0 || (pixValues[7] & 0x3) != 0) )
    {
        _ppuStatus |= 0x40;
    }

    assert (xOffset + yOffset < _screenBufferWidth * _screenBufferHeight);
}

u32 Ppu::Step()
{
    // if spr and bg are enabled
    if ((_ppuMask & 0x18) != 0)
    {
        if (_currentScanline < 240 || _currentScanline == 261) 
        {
            if (_currentScanlinePixel != 0 && _currentScanlinePixel < 258)
            {
                if (_currentScanlinePixel == 256)
                {
                    // If rendering is enabled, fine Y is incremented at dot 256 of each scanline, overflowing to coarse Y, and finally adjusted to wrap among the nametables vertically.
                    // Bits 12-14 are fine Y. Bits 5-9 are coarse Y. Bit 11 selects the vertical nametable.

                    if ((_vramAddr & 0x7000) != 0x7000)                 // if fine Y < 7
                        _vramAddr += 0x1000;                            // increment fine Y
                    else
                    {
                        _vramAddr &= ~0x7000;                           // fine Y = 0
                        int y = (_vramAddr & 0x3e0) >> 5;               // let y = coarse Y
                        if (y == 29)
                        {
                            y = 0;                                      // coarse Y = 0
                            _vramAddr ^= 0x800;                         // switch vertical nametable
                        }
                        else if (y == 31)
                            y = 0;                                      // coarse Y = 0, nametable not switched
                        else
                            y += 1;                                     // increment coarse Y

                        _vramAddr = (_vramAddr & ~0x3e0) | (y << 5);    // put coarse Y back into v
                    }

                    // also increment X
                    IncHorizontalVramAddr();
                }
                else if (_currentScanlinePixel == 257)
                {
                    // at dot 257 of every scanline
                    // v: ....F.. ...EDCBA = t: ....F.. ...EDCBA
                    _vramAddr &= ~0x41f;
                    _vramAddr |= (_tmpVramAddr & 0x41f);
                }
                else if ((_currentScanlinePixel & 0x7) == 0)
                {
                    IncHorizontalVramAddr();
                }

                // fetch nt byte
                else if (((_currentScanlinePixel - 1) & 0x7) == 0)
                {
                    if (_tileBuffer.Scanline != _dummyScanline)
                        FlushTileBuffer();

                    u16 ntAddr = (_vramAddr & _mapper->GetMirroringMode()) & 0xfff;

                    // start fetching next tile
                    _tileBuffer.Scanline = _currentScanline;
                    _tileBuffer.ScanlinePixel = _currentScanlinePixel + 15;

                    _tileBuffer.NtAddr = ntAddr;
                    _tileBuffer.NtByte = _nameTables[ntAddr];

                    u16 baseAttribAddr = ((_vramAddr & _mapper->GetMirroringMode()) & 0xc00) + 0x3c0;
                    u16 attribOffset = ((((ntAddr & 0x3ff) / 4) & 0x7)+ (8 * ((ntAddr & 0x3ff) / 128)));
                    _tileBuffer.AttribAddr = baseAttribAddr + attribOffset;
                }

                // fetch attrib byte
                else if (((_currentScanlinePixel - 3) & 0x7) == 0)
                {
                    _tileBuffer.AttribByte = _nameTables[_tileBuffer.AttribAddr];
                }

                // fetch low pattern byte
                else if (((_currentScanlinePixel - 5) & 0x7) == 0)
                {
                    u8 fineY = (_vramAddr & 0x7000) >> 12;

                    u16 offset = 0x1000 * (_ppuCtrl & 0x10) >> 4;
                    u16 baseIdx = (_tileBuffer.NtByte * 16) + fineY;

                    _tileBuffer.LowPatternByte = _mapper->ReadChr( baseIdx + offset );
                }

                // fetch high pattern byte
                else if (((_currentScanlinePixel - 7) & 0x7) == 0)
                {
                    u8 fineY = (_vramAddr & 0x7000) >> 12;

                    u16 offset = 0x1000 * (_ppuCtrl & 0x10) >> 4;
                    u16 baseIdx = (_tileBuffer.NtByte * 16) + fineY;

                    _tileBuffer.HighPatternByte = _mapper->ReadChr( baseIdx + offset + 8 );
                }
            }
            else if (_currentScanlinePixel > 320 && _currentScanlinePixel < 337)
            {
                if (_currentScanlinePixel == 328 || _currentScanlinePixel == 336)
                {
                    IncHorizontalVramAddr();
                }
                else if (_currentScanlinePixel == 321 || _currentScanlinePixel == 329)
                {
                    if (_tileBuffer.Scanline != _dummyScanline)
                        FlushTileBuffer();

                    u16 ntAddr = (_vramAddr & _mapper->GetMirroringMode()) & 0xfff;

                    _tileBuffer.Scanline = (_currentScanline + 1) % 240;
                    _tileBuffer.ScanlinePixel = (_currentScanlinePixel & 0x8); // 0 for 321, or 8 for 329

                    _tileBuffer.NtAddr = ntAddr;
                    _tileBuffer.NtByte = _nameTables[ntAddr];

                    u16 baseAttribAddr = ((_vramAddr & _mapper->GetMirroringMode()) & 0xc00) + 0x3c0;
                    u16 attribOffset = ((((ntAddr & 0x3ff) / 4) & 0x7)+ (8 * ((ntAddr & 0x3ff) / 128)));
                    _tileBuffer.AttribAddr = baseAttribAddr + attribOffset;
                }
                else if (_currentScanlinePixel == 323 || _currentScanlinePixel == 331)
                {
                    _tileBuffer.AttribByte = _nameTables[_tileBuffer.AttribAddr];
                }
                else if (_currentScanlinePixel == 325 || _currentScanlinePixel == 333)
                {
                    u8 fineY = (_vramAddr & 0x7000) >> 12;
                    u16 offset = 0x1000 * (_ppuCtrl & 0x10) >> 4;
                    u16 baseIdx = (_tileBuffer.NtByte * 16) + fineY;

                    _tileBuffer.LowPatternByte = _mapper->ReadChr( baseIdx + offset );
                }
                else if (_currentScanlinePixel == 327 || _currentScanlinePixel == 335)
                {
                    u8 fineY = (_vramAddr & 0x7000) >> 12;
                    u16 offset = 0x1000 * (_ppuCtrl & 0x10) >> 4;
                    u16 baseIdx = (_tileBuffer.NtByte * 16) + fineY;

                    _tileBuffer.HighPatternByte
                        = _mapper->ReadChr( baseIdx + offset + 8 );
                }
            }
        }
    }

    if (_currentScanline == 261)
    {
        if (_currentScanlinePixel >= 280 && _currentScanlinePixel <= 304)
        {
            // During dots 280 to 304 of the pre-render scanline (end of vblank)
            // v: IHGF.ED CBA..... = t: IHGF.ED CBA.....
            if ((_ppuMask & 0x18) != 0)
            {
                _vramAddr &= ~0x7be0;
                _vramAddr |= (_tmpVramAddr & 0x7be0);
            }
        }
        else if(_currentScanlinePixel == 1)
        {
            // clear sprite overflow and 0 hit flag at pixel 1 of the prerender scanline
            _ppuStatus &= ~0x60;
            
            // vblank end, clear flag
            _ppuStatus &= 0x7f;
        }
    }

    // vblank start
    else if (_currentScanline == _vblankScanline)
    {
        if (_currentScanlinePixel == 1)
            _ppuStatus |= 0x80;
    }

    _vramAddr &= 0x7fff;

    _currentFramePixel = (_currentFramePixel + 1) % (_pixelPerScanline * _scanlinePerFrame);
    _currentScanlinePixel = _currentFramePixel % _pixelPerScanline;
    _currentScanline = _currentFramePixel / _pixelPerScanline;

    return _currentFramePixel;
}

byte Ppu::ReadByte( addr address )
{
    byte retValue;

    address &= 0x2007;

    switch( address )
    {
        case 0x2002:    // PPUSTATUS

            // check for sprite 0 hit
            // TODO check if the background tile underneath
            //      the sprite is not transparent
            if ((_ppuMask & 0x18) != 0)
            {
                if (_sprite0FlagDirty)
                {
                    _sprite0FlagDirty = false;

                    _firstNonZeroSpriteX = 0xffff;
                    _firstNonZeroSpriteY = 0xffff;

                    u16 baseTblAddr = 0;
                    if ((_ppuCtrl & 0x8) != 0)
                        baseTblAddr += 0x1000;

                    u8 yPos    = _oamData[0];
                    u8 xPos    = _oamData[3];
                    u8 tileNum = _oamData[1];
                    u8 hPal    = (_oamData[2] & 0x3) << 2;
                    u16 patternTblAddr = baseTblAddr + (tileNum * 16);

                    bool firstPixelFound = false;
                    for (u8 j=0; j<8 && !firstPixelFound; ++j)
                    {
                        u8 lByte = _mapper->ReadChr( patternTblAddr + j );
                        // todo why was this unused? u8 hByte = _mapper->ReadChr( patternTblAddr + j + 8 );

                        for (u8 i=0; i<8 && !firstPixelFound; ++i)
                        {
                            if ((((lByte & (0x80>>i) | (lByte & (0x80>>i) << 1)) >> (7 - i)) | (hPal << 2)) & 0x3)
                            {
                                _firstNonZeroSpriteX = i + xPos;
                                _firstNonZeroSpriteY = j + yPos;
                                firstPixelFound = true;
                            }
                        }
                    }
                }
            }

            // w:                  = 0
            _firstWrite = true;

            retValue = _ppuStatus;
            _ppuStatus &= 0x7f;

            break;

        case 0x2004:    // OAMDATA
            retValue = _oamData[_oamAddr];
            break;

        case 0x2007:    // PPUDATA
        {
            u16 addrRead = _vramAddr;
            u8 valRead = ReadVramByte( addrRead );

            if (_vramAddr < 0x3f00)
            {
                retValue = _vramReadBuffer;
                _vramReadBuffer = valRead;
            }
            else
            {
                // Reading palette data from $3F00-$3FFF works differently.
                // The palette data is placed immediately on the data bus,
                // and hence no dummy read is required. Reading the palettes still
                // updates the internal buffer though, but the data placed in it
                // is the mirrored nametable data that would appear "underneath" the palette.
                retValue = valRead;
                _vramReadBuffer = ReadVramByte( addrRead & 0x2fff );
            }
        }
        break;

        default:
            assert (false);
            break;
    }

    return retValue;
}

word Ppu::ReadWord( addr )
{
    assert (false);
    return 0;
}

void Ppu::WriteByte( addr address, byte value )
{
    address &= 0x2007;

    switch( address )
    {
        case 0x2000:    // PPUCTRL
            _ppuCtrl = value;

            // t: ...BA.. ........ = d: ......BA
            _tmpVramAddr &= 0x73ff;
            _tmpVramAddr |= ((value & 0x3) << 10);

            // sprite pattern table address for 8x8 sprites or
            // sprite size
            if (value & 0x8 || value & 20)
            {
                _spriteDirty = true;
            }

            // Background pattern table address
            if (value & 0x10)
            {
                _nameTableDirty = true;
            }

            //_scrollX &= 0x80;
            //_scrollX |= (value & 0x1) << 7;

            //_scrollY &= 0x80;
            //_scrollY |= (value & 0x2) << 6;
            break;

        case 0x2001:    // PPUMASK
            _ppuMask = value;
            break;

        case 0x2003:    // OAMADDR
            _oamAddr = value;
            break;

        case 0x2004:    // OAMDATA
            _spriteDirty = true;
            _sprite0FlagDirty = _sprite0FlagDirty || _oamAddr < 4;
            _oamData[_oamAddr++] = value;

            break;

        case 0x2005:    // PPUSCROLL
            if (_firstWrite)
            {
                // t: ....... ...HGFED = d: HGFED...
                // x:              CBA = d: .....CBA
                // w:                  = 1
                _tmpVramAddr &= 0x7fe0;
                _tmpVramAddr |= (value >> 3);

                _fineXScroll = value & 0x7;
            }
            else
            {
                // t: CBA..HG FED..... = d: HGFEDCBA
                // w:                  = 0
                _tmpVramAddr &= 0xc1f;
                _tmpVramAddr |= ((value & 0x7) << 12);
                _tmpVramAddr |= ((value & 0xf8) << 2);
            }

            _firstWrite = !_firstWrite;
            break;

        case 0x2006:    // PPUADDR

            if (_firstWrite)
            {
                // t: .FEDCBA ........ = d: ..FEDCBA
                // t: X...... ........ = 0
                // w:                  = 1
                _tmpVramAddr &= 0x00ff;
                _tmpVramAddr |= ((value & 0x3f) << 8);
            }
            else
            {
                // t: ....... HGFEDCBA = d: HGFEDCBA
                // v                   = t
                // w:                  = 0
                _tmpVramAddr &= 0xff00;
                _tmpVramAddr |= value;
                _vramAddr = _tmpVramAddr;
            }

            _firstWrite = !_firstWrite;
            break;

        case 0x2007:    // PPUDATA
            WriteVramByte( _vramAddr, value );
            break;

        default:
            assert (false);
            break;
    }
}

byte Ppu::ReadVramByte( addr address )
{
    byte returnVal = 0;
    
    address &= 0x3fff;
    
    if (address < 0x2000)
    {
        returnVal =  _mapper->ReadChr( address );
    }
    else if (address >= 0x2000 && address < 0x3f00)
    {
        returnVal =  _nameTables[(address & _mapper->GetMirroringMode()) & 0xfff];
    }
    else if (address >= 0x3f00 && address < 0x4000)
    {
        returnVal = _palettes[address & 0x1f];
    }
    else
    {
        assert (false);
    }
    
    _vramAddr += (_ppuCtrl & 0x4) > 0 ? 32 : 1;
    
    return returnVal;
}

void Ppu::WriteVramByte( addr address, byte value )
{
    address &= 0x3fff;
    
    if (address < 0x2000)
    {
        _mapper->WriteChr( address, value );
        _patternTableDirty = true;
    }
    else if (address >= 0x2000 && address < 0x3f00)
    {
        _nameTables[(address & _mapper->GetMirroringMode()) & 0xfff] = value;

        _nameTableDirty = true;
    }
    else if (address >= 0x3f00 && address < 0x4000)
    {
        value &= 0x3f; // not sure, what about a palette read? 
                       // will it expect the last 2 bits?

        address &= 0x1f;

        _palettes[address] = value;
        
        if (address > 0x10)
        {
            _palettes[0x0] = _palettes[0x10];
            _palettes[0x4] = _palettes[0x14];
            _palettes[0x8] = _palettes[0x18];
            _palettes[0xc] = _palettes[0x1c];
        }

        _palettes[0x4] = _palettes[0];
        _palettes[0x8] = _palettes[0];
        _palettes[0xc] = _palettes[0];
        
        _paletteDirty = true;
    }
    else
    {
        assert (false);
    }
    
    _vramAddr += (_ppuCtrl & 0x4) > 0 ? 32 : 1;
}