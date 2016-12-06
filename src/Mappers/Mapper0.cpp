//
//  Mapper0.cpp
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#include "Mapper0.h"

Mapper0::Mapper0( std::unique_ptr<Rom> rom )
    : Mapper( rom.get() )
{
    if (rom->PrgRomSize > 0x4000)
        _addrMask = 0x7fff;
    else
        _addrMask = 0x3fff;
}

Mapper0::~Mapper0()
{
    
}

byte Mapper0::ReadChr( word addr )
{
    return _chrData[ addr ];
}

void Mapper0::WriteChr( word addr, byte value )
{
    _chrData[addr] = value;
}

byte Mapper0::ReadByte( addr address )
{
    return _prgData[address & _addrMask];
}

word Mapper0::ReadWord( addr address )
{
    word effectiveAddr = address & _addrMask;
    byte lbyte = _prgData[effectiveAddr];
    byte hbyte = _prgData[effectiveAddr + 1];
    return lbyte | (hbyte << 8);
}

void Mapper0::WriteByte( addr, byte )
{

}