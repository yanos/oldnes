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

    _regs[Ctrl] |= 0xc; 

    Apply();
}

Mapper1::~Mapper1()
{

}

byte Mapper1::ReadChr( word address )
{
    auto ptr = address < 0x1000
        ? _chrLowBankPtr
        : _chrHighBankPtr;

    return *(ptr + (address & 0xfff));
}

void Mapper1::WriteChr( word address, byte value )
{
    auto ptr = address < 0x1000
        ? _chrLowBankPtr
        : _chrHighBankPtr;

    *(ptr + (address & 0xfff)) = value;
}

byte Mapper1::ReadByte( addr address )
{
    // TODO: what to do on prgram reads when it's disabled
    if (address < 0x8000)
    {
        return _prgRam[address - 0x6000];
    }

    auto ptr = address < 0xC000
        ? _prgLowBankPtr
        : _prgHighBankPtr;

    return *(ptr + (address & 0x3fff));
}

word Mapper1::ReadWord( word address )
{
    byte lbyte = 0;
    byte hbyte = 0;

    if (address < 0x8000)
    {
        assert(address - 0x6000 + 1 < 0x2000);

        lbyte = _prgRam[address - 0x6000];
        hbyte = _prgRam[address - 0x6000 + 1];
    }
    else
    {
        auto ptr = address < 0xc000
            ? _prgLowBankPtr
            : _prgHighBankPtr;

        lbyte = *(ptr + (address & 0x3fff));
        hbyte = *(ptr + ((address + 1) & 0x3fff));
    }

    return lbyte | (hbyte << 8);
}

void Mapper1::WriteByte( word address, byte value )
{
    if (address < 0x8000)
    {
        _prgRam[address - 0x6000] = value;
        return;
    }

    if (value & 0x80)
    {
        _writeCount = 0;
        _loadReg = 0;

        _regs[Ctrl] |= 0xc; 

        Apply();
    }
    else
    {
        // add new bit
        _loadReg = (((value & 1) << 4) | _loadReg >> 1);

        if (++_writeCount == 5)
        {
            // store result in the correct register based on last address
            _regs[(address >> 13) & 0b11] = _loadReg;

            Apply();

            _writeCount = 0;
            _loadReg = 0;
        }
    }
}

void Mapper1::Apply()
{
    // Prg switching
    if ((_regs[Ctrl] & 0b1000) == 0)
    {
        // 32kb switching
        u32 offset = ((_regs[PRG] & 0xf) >> 1) * 0x8000;

        assert (offset < _prgDataSize);

        _prgLowBankPtr = &_prgData[offset];
        _prgHighBankPtr = _prgLowBankPtr + 0x4000;
    }
    else
    {
        u32 offset = (_regs[PRG] & 0xf) * 0x4000;

        assert (offset < _prgDataSize);

        if (_regs[Ctrl] & 0b100)
        {
            // low 16kb switching
            _prgLowBankPtr = &_prgData[offset];
            _prgHighBankPtr = &_prgData[_prgDataSize - 0x4000];
        }
        else
        {
            // high 16kb switching
            _prgLowBankPtr = &_prgData[0];
            _prgHighBankPtr = &_prgData[offset];
        }
    }

    // Chr switching
    if (_regs[Ctrl] & 0b10000)
    {
        _chrLowBankPtr = &_chrData[_regs[CHR0] * 0x1000];
        _chrHighBankPtr = &_chrData[_regs[CHR1] * 0x1000];
    }
    else
    {
        _chrLowBankPtr = &_chrData[(_regs[CHR0] >> 1) * 0x2000];
        _chrHighBankPtr = _chrLowBankPtr + 0x1000;
    }

    // mirroring 
    switch (_regs[Ctrl] & 0b11)
    {
        case 0:
            _mirroring = OneScreen;
            _chrHighBankPtr = _chrLowBankPtr;
            break;
        case 1:
            _mirroring = OneScreen;
            _chrLowBankPtr = _chrHighBankPtr;
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