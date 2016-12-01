//
//  Mapper0.cpp
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#include "Mapper0.h"

Mapper0::Mapper0( std::shared_ptr<Rom> rom, std::shared_ptr<Ppu> ppu )
{
    _rom = rom;
    _ppu = ppu;

    if (_rom->PrgRomSize > 0x4000)
        _addrMask = 0x7fff;
    else
        _addrMask = 0x3fff;

    _ppu->SetMirroring( _rom->Mirroring );
}

Mapper0::~Mapper0()
{
    
}

byte Mapper0::ReadByte( addr address )
{
    return _rom.get()->PrgRomData[address & _addrMask];

}

word Mapper0::ReadWord( addr address )
{
    byte lbyte = _rom.get()->PrgRomData[address & _addrMask];
    byte hbyte = _rom.get()->PrgRomData[(address + 1) & _addrMask];
    return lbyte | (hbyte << 8);
}

void Mapper0::WriteByte( addr, byte )
{

}