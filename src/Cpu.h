//
//  Cpu.h
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#ifndef __OldNES__Cpu__
#define __OldNES__Cpu__

#include <stdio.h>
#include <memory>

#include "Bus.h"

class Cpu
{
    
public:
    Cpu( std::unique_ptr<Bus> );
    ~Cpu();
    
    u8 Step();
    
    void Nmi();
    void Reset();
    void Break();
    
private:
    
    void SetNZ( byte );
    byte Ror( byte );
    byte Rol( byte );
    void Sbc( byte );
    void Adc( byte );
    u8 Branch();
    
    u8 GetFlags();
    void SetFlags( u8 );
    
    // stack
    byte PopByte();
    void PushByte( byte );
    word PopWord();
    void PushWord( word );
    
    // addressing
    byte ZeroPageRead();
    void ZeroPageStore( byte);
    byte ZeroPageReadX();
    void ZeroPageStoreX( byte );
    byte ZeroPageReadY();
    void ZeroPageStoreY( byte );
    byte AbsoluteRead();
    void AbsoluteStore( byte );
    byte AbsoluteReadX();
    void AbsoluteStoreX( byte );
    byte AbsoluteReadY();
    void AbsoluteStoreY( byte );
    byte IndirectReadX();
    void IndirectStoreX( byte );
    byte IndirectReadY();
    void IndirectStoreY( byte );
    
    std::unique_ptr<Bus> _bus;
    
    const byte           _defaultFlags = 4;
    
    const addr           _resetVector = 0xFFFC;
    const addr           _nmiVector   = 0xFFFA;
    const addr           _brkVector   = 0xFFFE;
    
    addr                 _regPC;	// Program counter
    byte                 _regSP;    // Stack pointer
    
    byte                 _regA;	    // Accumulator
    byte                 _regX;	    // X
    byte                 _regY;	    // Y
    
    byte                 _flags;    // all flags
    
    u8                   _flagC;	// Carry flag
    u8                   _flagZ;	// Zero flag
    u8                   _flagI;	// Interupt disable flag
    u8                   _flagD;	// Decimal flag
    u8                   _flagV;	// Overflow flag
    u8                   _flagN;	// Negative flag
    
    byte                 _tmpByte;
    
    u8                   _opcode;
    u8                   _lastOpcode;
};

#endif /* defined(__OldNES__Cpu__) */
