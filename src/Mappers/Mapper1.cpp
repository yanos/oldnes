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
    std::memset( _regs, 0, sizeof(_regs) );
    Apply();
}

Mapper1::~Mapper1()
{

}

byte Mapper1::ReadChr( word address )
{
    auto ptr = address < 0x1000
        ? _chrBankAPtr
        : _chrBankBPtr;

    return *(ptr + (address & 0xfff));
}

void Mapper1::WriteChr( word address, byte value )
{
    auto ptr = address < 0x1000
        ? _chrBankAPtr
        : _chrBankBPtr;

    *(ptr + (address & 0xfff)) = value;
}

byte Mapper1::ReadByte( addr address )
{
    // TODO: what to do on prgram reads when its disabled
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
    if (address < 0x8000)
    {
        byte lbyte = _prgRam[address - 0x6000];
        byte hbyte = _prgRam[address - 0x6000 + 1];
        return lbyte | (hbyte << 8);
    }

    auto ptr = address < 0xc000
        ? _prgBankAPtr
        : _prgBankBPtr;

    byte lbyte = *(ptr + (address & 0x3fff));
    byte hbyte = *(ptr + ((address + 1) & 0x3fff));
    return lbyte | (hbyte << 8);
}

void Mapper1::WriteByte( word address, byte value )
{
    if (address < 0x8000 && _prgRamEnabled)
    {
        _prgRam[address - 0x6000] = value;
        return;
    }

    if (value & 0x80)
    {
        _writeCount = 0;
        _shiftReg = 0;

        _prgBankBPtr = &_prgData[_prgDataSize - 0x4000];
    }
    else
    {
        // add new bit
        _shiftReg = (((value & 1) << 4) | _shiftReg >> 1);

        if (++_writeCount == 5)
        {
            // store result in the correct register based on last address
            _regs[(address >> 13) & 0b11] = _shiftReg;
            
            Apply();

            _writeCount = 0;
            _shiftReg = 0;
        }
    }
}

void Mapper1::Apply()
{
    // Prg switching
    if ((_regs[Ctrl] & 0b1000) == 0)
    {
        // 32kb switching
        auto offset = ((_regs[PRG] & 0xf) >> 1) * 0x8000;

        assert (offset < _prgDataSize);

        _prgBankAPtr = &_prgData[offset];
        _prgBankBPtr = _prgBankAPtr + 0x4000;
    }
    else
    {
        auto offset = (_regs[PRG] & 0xf) * 0x4000;

        assert (offset < _prgDataSize);

        if (_regs[Ctrl] & 0b100)
        {
            // low 16kb switching
            _prgBankAPtr = &_prgData[offset];
            _prgBankBPtr = &_prgData[_prgDataSize - 0x4000];
        }
        else
        {
            // low 16kb switching
            _prgBankAPtr = &_prgData[0];
            _prgBankBPtr = &_prgData[offset];
        }
    }

    // Chr switching
    if (_regs[Ctrl] & 0b10000)
    {
        _chrBankAPtr = &_chrData[_regs[CHR0] * 0x1000];
        _chrBankBPtr = &_chrData[_regs[CHR1] * 0x1000];
    }
    else
    {
        _chrBankAPtr = &_chrData[(_regs[CHR0] & 0xe) * 0x2000];
        _chrBankBPtr = _chrBankAPtr + 0x1000;
    }

    // mirroring 
    switch (_regs[Ctrl] & 0b11)
    {
        case 0:
            _mirroring = OneScreen;
            _chrBankBPtr = _chrBankAPtr;
            break;
        case 1:
            _mirroring = OneScreen;
            _chrBankAPtr = _chrBankBPtr;
            break;
        case 2:
            _mirroring = Vertical;
            break;
        case 3:
            _mirroring = Horizontal;
            break;
    }

    _prgRamEnabled = (_regs[Ctrl] & 0x10) == 0;
}