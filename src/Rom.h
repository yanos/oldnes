//
//  Rom.h
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#ifndef __OldNES__Rom__
#define __OldNES__Rom__

#include <stdio.h>
#include <memory>

#include "Common.h"

class Rom
{

public:
    
    Rom( u32, u32, u32, bool, bool, bool, bool, bool, u8, MirroringMode, std::unique_ptr<u8[]>, std::unique_ptr<u8[]> );
    ~Rom() {}
    
    static std::unique_ptr<Rom> Load( string );
    
    const u32           PrgRomSize;
    const u32           PrgRamSize;
    const u32           ChrRomSize;
    const bool          HasSRAM;
    const bool          HasTrainer;
    const bool          VSUnisystem;
    const bool          PlayChoice;
    const bool          Nes2Format;
    const u8            MapperId;
    const MirroringMode Mirroring;
    
    const std::unique_ptr<u8[]> PrgRomData;
    const std::unique_ptr<u8[]> ChrRomData;
    
private:
    static const int    _headerSize = 16;
    
};

#endif /* defined(__OldNES__Rom__) */
