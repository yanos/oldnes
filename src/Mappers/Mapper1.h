//
//  Mapper0.h
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#ifndef __OldNES__Mapper1__
#define __OldNES__Mapper1__

#include <stdio.h>
#include <memory>

#include "../DataTypes.h"
#include "../Rom.h"
#include "../Ppu.h"

class Mapper1
{

public:
    
    Mapper1() {}
    Mapper1( std::shared_ptr<Rom>, std::shared_ptr<Ppu> ppu );
    ~Mapper1();
    
    byte ReadByte( addr );
    word ReadWord( addr );
    void WriteByte( addr, byte );
    
private:
    std::shared_ptr<Rom> _rom;
    
    u16 _addrMask;
    
};

#endif /* defined(__OldNES__Mapper1__) */
