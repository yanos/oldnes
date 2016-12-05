//
//  Bus.cpp
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#include <cassert>

#include "Bus.h"

Bus::Bus( std::shared_ptr<Mapper> mapper,
          std::shared_ptr<Ppu> ppu,
          std::shared_ptr<Input> input )
{
    _mapper = mapper;
    _ppu = ppu;
    _input = input;
}

Bus::~Bus()
{

}
        
byte Bus::ReadByte( addr address )
{
    if (address < 0x2000)
        return _ram[address & _ramMask];
    else if (address < 0x4000)
        return _ppu->ReadByte (address);
    else if (address < 0x4020)
    {
        if (address == 0x4016 || address == 0x4017)
            return _input->ReadByte( address );
    }
    else
        return _mapper->ReadByte(address);
            
    assert (false);
}

word Bus::ReadWord( addr address )
{
    if (address < 0x2000)
        return _ram[address & _ramMask] | ((_ram[(address & _ramMask) + 1]) << 8);
    else if (address < 0x4000)
        return _ppu->ReadWord( address );
    else
        return _mapper->ReadWord( address );
    //else if (address >= 0x6000 && address < 0x8000)
    //    return _sram[address - _sramOffset] | ((_sram[address + 1U - _sramOffset]) << 8);
    
    assert (false);
}

void Bus::WriteByte( addr address, byte value )
{
    if (address < 0x2000)
    {
        _ram[address & _ramMask] = value;
        return;
    }
    else if (address < 0x4000)
    {
        _ppu->WriteByte( address, value );
        return;
    }
    else if (address < 0x4020)
    {
        if (address == 0x4016 || address == 0x4017)
        {
            _input->WriteByte( address, value );
            return;
        }
        else if (address == 0x4014)
        {
            // TODO not accurate according to sprite test

            //addr baseAddr = value << 8;
            //const addr baseAddr = _ppu->GetOamAddr();

            for (u16 i=0; i<256; ++i)
            {
                _ppu->WriteOam( i, ReadByte( (0x100 * value) + i ) );
            }
        
            // TODO: add 513 or 514 cycles
            return;
        }

        // TODO sound stuff
        return;
    }
    else 
    {
        _mapper->WriteByte( address, value );
        return;
    }
                
    assert (false);
}

