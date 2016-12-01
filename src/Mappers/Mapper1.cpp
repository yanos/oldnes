//
//  Mapper0.cpp
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#include "Mapper1.h"

Mapper1::Mapper1( std::shared_ptr<Rom> rom, std::shared_ptr<Ppu> ppu )
{
    _rom = rom;
    
    if (_rom.get()->PrgRomSize > 0x4000)
        _addrMask = 0x7fff;
    else
        _addrMask = 0x3fff;
}

Mapper1::~Mapper1()
{
    
}

byte Mapper1::ReadByte( addr address )
{
    return _rom.get()->PrgRomData[address & _addrMask];
}

word Mapper1::ReadWord( addr address )
{
    byte lbyte = _rom.get()->PrgRomData[address & _addrMask];
    byte hbyte = _rom.get()->PrgRomData[(address + 1) & _addrMask];
    return lbyte | (hbyte << 8);
}

void Mapper1::WriteByte( addr, byte )
{

}