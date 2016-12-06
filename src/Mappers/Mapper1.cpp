//
//  Mapper0.cpp
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#include <cassert>
#include "Mapper1.h"

Mapper1::Mapper1( std::unique_ptr<Rom> rom )
    : Mapper( rom.get() )
{
    _prgBankAPtr = &_prgData[0];
    _prgBankBPtr = &_prgData[_prgDataSize - 0x4000];

    _chrBankAPtr = &_chrData[0];
    _chrBankBPtr = &_chrData[0x1000];
}

Mapper1::~Mapper1()
{

}

byte Mapper1::ReadChr( word addr )
{
    auto ptr = addr < 0x1000
        ? _chrBankAPtr
        : _chrBankBPtr;

    return *(ptr + (addr & 0xfff));
}

void Mapper1::WriteChr( word addr, byte value )
{
    auto ptr = addr < 0x1000
        ? _chrBankAPtr
        : _chrBankBPtr;

    *(ptr + (addr & 0xfff)) = value;
}

byte Mapper1::ReadByte( addr address )
{
    if (address < 0x8000)
    {
        return _prgRam[address - 0x6000];
    }

    auto ptr = address < 0xc000
        ? _prgBankAPtr
        : _prgBankBPtr;

    return *(ptr + (address & 0x3fff));
}

word Mapper1::ReadWord( word address )
{
    assert( address >= 0x8000 );

    auto ptr = address < 0xc000
        ? _prgBankAPtr
        : _prgBankBPtr;

    byte lbyte = *(ptr + (address & 0x3fff));
    byte hbyte = *(ptr + ((address + 1) & 0x3fff));
    return lbyte | (hbyte << 8);
}

void Mapper1::WriteByte( word address, byte value )
{
    {
        _prgRam[address - 0x6000] = value;
    }

    if (value & 0x80)
    {
        _writeCount = 0;
        _loadReg = 0;

        _prgBankAPtr = &_prgData[0];
        _prgBankBPtr = &_prgData[_prgDataSize - 0x4000];

        _chrBankAPtr = &_chrData[0];
        _chrBankBPtr = &_chrData[0x1000];
    }
    else
    {
        _loadReg = (((value & 1) << 4) | _loadReg >> 1);

        if (++_writeCount == 5)
        {
            if (address < 0xa000)                          // ctrl reg
            {
                // mirroring mode
                switch (_loadReg & 0x3)
                {
                    case 0:
                        _mirroring = OneScreen;
                        break;
                    case 1:
                        _mirroring = OneScreen;
                        break;
                    case 2:
                        _mirroring = Vertical;
                        break;
                    case 3:
                        _mirroring = Horizontal;
                        break;
                }
                
                // prg switching mode
                switch (_loadReg & 0xc)
                {
                    case 0x0:
                    case 0x4:
                        _prgBankSwitchMode = Switch32k;
                        break;
                    case 0x8:
                        _prgBankSwitchMode = SwitchHigh16k;
                        break;
                    case 0xc:
                        _prgBankSwitchMode = SwitchLow16k;
                        break;
                }
                
                // chr switching mode
                _chrBankSwitchMode = (_loadReg & 0x10) != 0
                    ? Switch8k
                    : Switch4k;

            }
            else if (address < 0xc000)                     // chr bank 0
            {
                switch (_chrBankSwitchMode)
                {
                    case Switch4k:
                        _chrBankAPtr = &_chrData[_loadReg * 0x1000];
                        break;    

                    case Switch8k:
                        _chrBankAPtr = &_chrData[(_loadReg & 0xe) * 0x2000];
                        _chrBankBPtr = _chrBankAPtr + 0x1000;
                        break;
                }
            }
            else if (address < 0xe000)                     // chr bank 1
            {
                switch (_chrBankSwitchMode)
                {
                    case Switch4k:
                        _chrBankBPtr = &_chrData[_loadReg * 0x1000];
                        break;

                    case Switch8k:
                        break;
                }
            }
            else                                          // prg bank
            {
                _prgRamEnabled = _loadReg & 0x10;

                switch (_prgBankSwitchMode)
                {
                    case Switch32k:
                    {
                        auto offset = ((_loadReg & 0xf) >> 1) * 0x8000;
                        _prgBankAPtr = &_prgData[offset];
                        _prgBankBPtr = _prgBankAPtr + 0x4000;

                        assert (offset < _prgDataSize);
                        break;
                    }
                    
                    case SwitchLow16k:
                    {
                        auto offset = (_loadReg & 0xf) * 0x4000;
                        _prgBankAPtr = &_prgData[offset];
                        _prgBankBPtr = &_prgData[_prgDataSize - 0x4000];

                        assert (offset < _prgDataSize);
                        break;
                    }
                        
                    case SwitchHigh16k:
                    {
                        auto offset = (_loadReg & 0xf) * 0x4000;
                        _prgBankAPtr = &_prgData[0];
                        _prgBankBPtr = &_prgData[offset];

                        assert (offset < _prgDataSize);
                        break;
                    }
                }
            }

            _writeCount = 0;
            _loadReg = 0;
        }
    }
}