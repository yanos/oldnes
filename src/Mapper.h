//
//  Mapper.h
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#ifndef __OldNES__Mapper__
#define __OldNES__Mapper__

#include <stdio.h>
#include <functional>

#include "Common.h"
#include "Rom.h"
#include "Ppu.h"

class Mapper
{
    
public:
    Mapper( std::shared_ptr<Rom>, std::shared_ptr<Ppu> );
    ~Mapper();
    
    byte ReadByte( addr );
    word ReadWord( addr );
    void WriteByte( addr, byte );
    
    std::function<byte(addr)>       _readByte;
    std::function<word(addr)>       _readWord;
    std::function<void(addr, byte)> _writeByte;
};

#endif /* defined(__OldNES__Mapper__) */
