//
//  Bus.h
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#ifndef __OldNES__Bus__
#define __OldNES__Bus__

#include <stdio.h>
#include <memory>

#include "Common.h"
#include "Ppu.h"
#include "Mapper.h"
#include "Input.h"

class Bus
{
    
public:
    Bus( std::shared_ptr<Mapper> mapper,
         std::shared_ptr<Ppu> ppu,
         std::shared_ptr<Input> input );
    ~Bus();
    
    byte ReadByte( addr );
    word ReadWord( addr );
    void WriteByte( addr, byte );

private:
    u8 	                    _ram[0x800];
    u8                      _sram[0x2000];
    std::shared_ptr<Ppu>    _ppu;
    std::shared_ptr<Mapper> _mapper;
    std::shared_ptr<Input>  _input;
    
    const u16 _sramOffset = 0x6000;
    const u16 _ramMask = 0x7ff;        // ram is 2kb mirrored 4 times
    
};

#endif /* defined(__OldNES__Bus__) */
