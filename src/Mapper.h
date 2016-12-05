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

class Mapper
{
    
public:
    Mapper( std::shared_ptr<Rom> );
    ~Mapper();
    
    virtual byte ReadByte( addr ) = 0;
    virtual word ReadWord( addr ) = 0;
    virtual void WriteByte( addr, byte ) = 0;
    
    virtual byte ReadChr( word ) = 0;
    virtual void WriteChr( word, byte ) = 0;
    
    const MirroringMode GetMirroringMode() { return _mirroring; }
    
//    std::function<byte(addr)>       _readByte;
//    std::function<word(addr)>       _readWord;
//    std::function<void(addr, byte)> _writeByte;
    
protected:
    MirroringMode        _mirroring;
    
    u32                  _prgDataSize;
    u32                  _chrDataSize;

    byte*                _prgData = nullptr;
    byte*                _chrData = nullptr;
};

#endif /* defined(__OldNES__Mapper__) */
