//
//  Mapper.cpp
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#include "Mapper.h"
#include "Mappers/Mapper0.h"
#include "Mappers/Mapper1.h"

Mapper::Mapper( std::shared_ptr<Rom> rom )
{
    _mirroring = rom->Mirroring;
    
    // copy prg data
    _prgDataSize = rom->PrgRomSize;
    _prgData = new byte[_prgDataSize];
    std::memcpy( _prgData, rom->PrgRomData.get(), _prgDataSize );
    
    // copy chr data
    _chrDataSize = rom->ChrRomSize;
    if (_chrDataSize > 0)
    {
        _chrData = new byte[_chrDataSize];
        std::memcpy( _chrData, rom->ChrRomData.get(), _chrDataSize );
    }
    else
    {
        _chrData = new byte[0x40000];
    }
}

Mapper::~Mapper()
{
    if (_prgData != nullptr)
        delete[] _prgData;
    
    if (_chrData != nullptr)
        delete[] _chrData;
}
