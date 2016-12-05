//
//  Mapper0.h
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#ifndef __OldNES__Mapper0__
#define __OldNES__Mapper0__

#include <stdio.h>
#include <memory>

#include "../DataTypes.h"
#include "../Mapper.h"
#include "../Rom.h"


class Mapper0 : public Mapper
{

public:
    
    Mapper0( std::shared_ptr<Rom> rom );
    ~Mapper0();
    
    byte ReadByte( addr );
    word ReadWord( addr );
    void WriteByte( addr, byte );
    
    byte ReadChr( word );
    void WriteChr( word, byte );
    
private:
    u16     _addrMask;
    
};

#endif /* defined(__OldNES__Mapper0__) */
