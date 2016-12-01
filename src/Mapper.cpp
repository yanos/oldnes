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

Mapper::Mapper( std::shared_ptr<Rom> rom, std::shared_ptr<Ppu> ppu )
{
    using namespace std::placeholders;
    
    switch (rom.get()->MapperId)
    {
        case 0:
        {
            auto mapper = std::shared_ptr<Mapper0>( new Mapper0( rom, ppu ) );
            
            _readByte = std::bind( &Mapper0::ReadByte, mapper, _1 );
            _readWord = std::bind( &Mapper0::ReadWord, mapper, _1 );
            _writeByte = std::bind( &Mapper0::WriteByte, mapper, _1, _2 );

            break;
        }
            
        case 1:
        {
            auto mapper = std::shared_ptr<Mapper1>( new Mapper1( rom, ppu ) );
            
            _readByte = std::bind( &Mapper1::ReadByte, mapper, _1 );
            _readWord = std::bind( &Mapper1::ReadWord, mapper, _1 );
            _writeByte = std::bind( &Mapper1::WriteByte, mapper, _1, _2 );

            break;
        }

        default:
            break;
    }
}

Mapper::~Mapper()
{
    
}

byte Mapper::ReadByte( addr address )
{
    return _readByte( address );
}

word Mapper::ReadWord( addr address )
{
    return _readWord( address );
}

void Mapper::WriteByte( addr address, byte val )
{
    _writeByte( address, val );
}

