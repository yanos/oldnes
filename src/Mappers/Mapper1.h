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
#include "../Mapper.h"

class Mapper1 : public Mapper
{

public:
    
    Mapper1( std::unique_ptr<Rom> );
    ~Mapper1();
    
    byte ReadByte( addr );
    word ReadWord( addr );
    void WriteByte( addr, byte );
    
    byte ReadChr( word );
    void WriteChr( word, byte );
    
private:
    
    enum Regs { Ctrl, CHR0, CHR1, PRG, RegMax };

    void Apply();

    bool                 _prgRamEnabled;

    byte                 _regs[RegMax];

    byte                 _prgRam[0x2000];

    u8*                  _prgBankAPtr;
    u8*                  _prgBankBPtr;
    
    u8*                  _chrBankAPtr;
    u8*                  _chrBankBPtr;

    u8                   _writeCount = 0;
    u8                   _shiftReg = 0;
    
};

#endif /* defined(__OldNES__Mapper1__) */
