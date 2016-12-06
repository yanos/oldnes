//
//  Cpu.cpp
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#include <cassert>

#include "Cpu.h"

const u8 OpcodeSizes[]
{
    //  0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
    1,  2,  0,  0,  0,  2,  2,  0,  1,  2,  1,  0,  0,  3,  3,  0, // 0
    2,  2,  0,  0,  0,  2,  2,  0,  1,  3,  0,  0,  0,  3,  3,  0, // 1
    3,  2,  2,  0,  2,  2,  2,  0,  1,  2,  1,  0,  3,  3,  3,  0, // 2
    2,  2,  0,  0,  0,  2,  2,  0,  1,  3,  0,  0,  0,  3,  3,  0, // 3
    1,  2,  0,  0,  0,  2,  2,  0,  1,  2,  1,  0,  3,  3,  3,  0, // 4
    2,  2,  0,  0,  0,  2,  2,  0,  1,  3,  0,  0,  0,  3,  3,  0, // 5
    1,  2,  0,  0,  0,  2,  2,  0,  1,  2,  1,  0,  3,  3,  3,  0, // 6
    2,  2,  0,  0,  0,  2,  2,  0,  1,  3,  0,  0,  0,  3,  3,  0, // 7
    3,  2,  0,  0,  2,  2,  2,  0,  1,  0,  1,  0,  3,  3,  3,  0, // 8
    2,  2,  0,  0,  2,  2,  2,  0,  1,  3,  1,  0,  0,  3,  0,  0, // 9
    2,  2,  2,  0,  2,  2,  2,  0,  1,  2,  1,  0,  3,  3,  3,  0, // a
    2,  2,  0,  0,  2,  2,  2,  0,  1,  3,  1,  0,  3,  3,  3,  0, // b
    2,  2,  0,  0,  2,  2,  2,  0,  1,  2,  1,  0,  3,  3,  3,  0, // c
    2,  2,  0,  0,  0,  2,  2,  0,  1,  3,  0,  0,  0,  3,  3,  0, // d
    2,  2,  0,  0,  2,  2,  2,  0,  1,  2,  1,  0,  3,  3,  3,  0, // e
    2,  2,  0,  0,  0,  2,  2,  0,  1,  3,  0,  0,  0,  3,  3,  0  // f
};

const u8 OpcodeCycles[]
{
    //  0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
    7,  6,  0,  0,  0,  3,  5,  0,  3,  2,  2,  0,  0,  4,  3,  0, // 0
    2,  5,  0,  0,  0,  4,  6,  0,  2,  4,  0,  0,  0,  4,  7,  0, // 1
    6,  6,  2,  0,  3,  0,  5,  0,  4,  2,  2,  0,  4,  4,  6,  0, // 2
    2,  5,  0,  0,  0,  4,  5,  0,  2,  4,  0,  0,  0,  4,  7,  0, // 3
    6,  6,  0,  0,  0,  3,  5,  0,  3,  2,  2,  0,  3,  4,  6,  0, // 4
    2,  5,  0,  0,  0,  4,  6,  0,  2,  4,  0,  0,  0,  4,  7,  0, // 5
    6,  6,  0,  0,  0,  3,  6,  0,  4,  2,  2,  0,  5,  4,  6,  0, // 6
    2,  5,  0,  0,  0,  4,  0,  0,  2,  4,  0,  0,  0,  4,  7,  0, // 7
    4,  6,  0,  0,  3,  3,  3,  0,  2,  0,  2,  0,  4,  4,  4,  0, // 8
    2,  6,  0,  0,  4,  5,  4,  0,  2,  5,  2,  0,  0,  5,  0,  0, // 9
    2,  6,  2,  0,  3,  3,  3,  0,  2,  2,  2,  0,  4,  4,  4,  0, // a
    2,  5,  0,  0,  4,  4,  4,  0,  2,  4,  2,  0,  4,  4,  4,  0, // b
    2,  6,  0,  0,  3,  3,  5,  0,  2,  2,  2,  0,  4,  4,  6,  0, // c
    2,  5,  0,  0,  0,  4,  6,  0,  2,  4,  0,  0,  0,  4,  7,  0, // d
    2,  6,  0,  0,  3,  3,  5,  0,  2,  2,  2,  0,  4,  4,  6,  0, // e
    2,  5,  0,  0,  0,  4,  6,  0,  2,  4,  0,  0,  0,  4,  7,  0  // f
};

Cpu::Cpu( std::unique_ptr<Bus> bus )
{
    _bus = std::move( bus );
}

Cpu::~Cpu()
{
    
}

// Zero Page
byte Cpu::ZeroPageRead()
{
    return _bus->ReadByte( _bus->ReadByte( _regPC + 1 ) );
}

void Cpu::ZeroPageStore( byte value )
{
    _bus->WriteByte( _bus->ReadByte( _regPC + 1 ), value );
}

byte Cpu::ZeroPageReadX()
{
    return _bus->ReadByte( ( _bus->ReadByte( _regPC + 1 ) + _regX ) & 0xff );
}

void Cpu::ZeroPageStoreX( byte value )
{
    _bus->WriteByte( ( _bus->ReadByte( _regPC + 1 ) + _regX ) & 0xff, value );
}

byte Cpu::ZeroPageReadY()
{
    return _bus->ReadByte( ( _bus->ReadByte( _regPC + 1 ) + _regY ) & 0xff );
}

void Cpu::ZeroPageStoreY( byte value )
{
    _bus->WriteByte( ( _bus->ReadByte( _regPC + 1 ) + _regY ) & 0xff, value );
}

// Abolsute
byte Cpu::AbsoluteRead()
{
    return _bus->ReadByte( _bus->ReadWord( _regPC + 1 ) );
}

void Cpu::AbsoluteStore( byte value )
{
    _bus->WriteByte( _bus->ReadWord( _regPC + 1 ), value );
}

byte Cpu::AbsoluteReadX()
{
    return _bus->ReadByte( _bus->ReadWord( _regPC + 1 ) + _regX );
}

void Cpu::AbsoluteStoreX( byte value )
{
    _bus->WriteByte( _bus->ReadWord( _regPC + 1 ) + _regX, value );
}

byte Cpu::AbsoluteReadY()
{
    return _bus->ReadByte( _bus->ReadWord( _regPC + 1 ) + _regY );
}

void Cpu::AbsoluteStoreY( byte value )
{
    _bus->WriteByte( _bus->ReadWord( _regPC + 1 ) + _regY, value );
}

// Indirect
byte Cpu::IndirectReadX()
{
    addr baseAddr = ( _bus->ReadByte( _regPC + 1 ) + _regX ) & 0xff;
    return _bus->ReadByte( _bus->ReadByte( baseAddr ) | ( _bus->ReadByte( (baseAddr + 1) & 0xff ) ) << 8);
}

void Cpu::IndirectStoreX( byte value )
{
    addr baseAddr = ( _bus->ReadByte( _regPC + 1 ) + _regX ) & 0xff;
    _bus->WriteByte( _bus->ReadByte( baseAddr ) | ( _bus->ReadByte( (baseAddr + 1) & 0xff ) ) << 8, value );
}

byte Cpu::IndirectReadY()
{
    addr baseAddr = _bus->ReadByte( _regPC + 1 );
    addr lowEffAddr = _bus->ReadByte( baseAddr ) + _regY;
    addr highEffAddr = _bus->ReadByte( (baseAddr + 1) & 0xff);
    
    if (lowEffAddr > 0x100) // TODO ~0xff != 0
    {
        //cycle++
        highEffAddr++;
        lowEffAddr &= 0xff;
    }
    
    addr effAddr = ( lowEffAddr | (highEffAddr << 8) ) & 0xffff;
    return _bus->ReadByte( effAddr );
}

void Cpu::IndirectStoreY( byte value )
{
    addr baseAddr = _bus->ReadByte( _regPC + 1 );
    addr lowEffAddr = _bus->ReadByte( baseAddr ) + _regY;
    addr highEffAddr = _bus->ReadByte( (baseAddr + 1) & 0xff);
    
    if (lowEffAddr > 0x100) // TODO ~0xff != 0
    {
        //cycle++
        highEffAddr++;
        lowEffAddr &= 0xff;
    }
    
    addr effAddr = ( lowEffAddr | (highEffAddr << 8) ) & 0xffff;
    _bus->WriteByte( effAddr, value );
}


void Cpu::PushWord( word value )
{
    _bus->WriteByte( _regSP + 0x100, value >> 8 );
    _regSP = (_regSP - 1);
    
    _bus->WriteByte( _regSP + 0x100, value & 0xff );
    _regSP = (_regSP - 1);
}

word Cpu::PopWord()
{
    _regSP = (_regSP + 1);
    byte lval = _bus->ReadByte( _regSP + 0x100 );
    
    _regSP = (_regSP + 1);
    byte hval = _bus->ReadByte( _regSP + 0x100 );
    
    return lval | (hval << 8);
}

void Cpu::PushByte( byte value )
{
    _bus->WriteByte( _regSP + 0x100, value & 0xff );
    _regSP = (_regSP - 1);
}

byte Cpu::PopByte()
{
    _regSP = (_regSP + 1);
    return _bus->ReadByte( _regSP + 0x100 );
}

void Cpu::Reset()
{
    // reset all flags
    SetFlags( _defaultFlags );
    
    // sp to the top of the stack
    _regSP = 0xfd;
    
    // next instruction is at address _resetVector
    _regPC = _bus->ReadWord( _resetVector );
}

void Cpu::Nmi()
{
    PushWord( _regPC );
    
    PushByte( GetFlags() );
    _regPC = _bus->ReadWord( _nmiVector );
    
    _flagI = 1;
}

void Cpu::Break()
{
    PushWord( _regPC );
    
    PushByte( GetFlags() );
    _regPC = _bus->ReadWord( _brkVector );
    
    _flagI = 1;
}

u8 Cpu::GetFlags()
{
    return  _flagC       |
           (_flagZ << 1) |
           (_flagI << 2) |
           (_flagD << 3) |
           (     1 << 5) | // Unused flag is 1 at all time
           (_flagV << 6) |
           (_flagN << 7);
}

void Cpu::SetFlags( u8 flags )
{
    _flagC =  flags & 0x1;
    _flagZ = (flags & 0x2)  >> 1;
    _flagI = (flags & 0x4)  >> 2;
    _flagD = (flags & 0x8)  >> 3;
    _flagV = (flags & 0x40) >> 6;
    _flagN = (flags & 0x80) >> 7;
}

void Cpu::SetNZ( byte value )
{
    _flagN = (value & 0x80) >> 7;
    _flagZ = value == 0 ? 1 : 0;
}

u8 Cpu::Branch()
{
    addr pc = _regPC + 2;
    s8 offset = _bus->ReadByte( _regPC + 1 );
    _regPC = pc + offset;
    return 3; // TODO: add 1 when crossing page boundary
}

void Cpu::Adc( byte value )
{
    word result = _regA + value + _flagC;
    _flagN = (result & 0x80) >> 7;
    _flagZ = (result & 0xff) == 0 ? 1U : 0;
    _flagC = (result & 0x100) >> 8;
    _flagV = ( ((result ^ value) & (result ^ _regA)) & 0x80 ) >> 7;
    _regA = result & 0xff;
}

void Cpu::Sbc( byte value )
{
    word result = _regA - value - (1 - _flagC);
    _flagN = (result & 0x80) >> 7;
    _flagZ = (result & 0xff) == 0 ? 1 : 0;
    _flagC = 1 - ((result & 0x100) >> 8);
    _flagV = ( ((result ^ (0xff - value)) & (result ^ _regA)) & 0x80 ) >> 7;
    _regA = result & 0xff;
}

byte Cpu::Rol( byte value )
{
    word val = value;
    val <<= 1;
    val |= _flagC;
    _flagC = (val & 0x100) >> 8;
    value = (val & 0xff);
    SetNZ( value );
    return value;
}

byte Cpu::Ror( byte value )
{
    word val = value;
    val |= (_flagC << 8);
    _flagC = val & 1;
    value = val >> 1;
    SetNZ( value );
    return value;
}

u8 Cpu::Step()
{
    _lastOpcode = _opcode;
    
    // fetch opcode
    _opcode = _bus->ReadByte( _regPC );
    
    switch (_opcode)
    {
        case 0x69:  // ADC Immediate
            Adc( _bus->ReadByte( _regPC + 1 ) );
            break;
            
        case 0x65:  // ADC Zero Page
            Adc( ZeroPageRead() );
            break;
            
        case 0x75:  // ADC Zero Page, X
            Adc( ZeroPageReadX() );
            break;
            
        case 0x6d:  // ADC Absolute
            Adc( AbsoluteRead() );
            break;
            
        case 0x7d:  // ADC Absolute, X
            Adc( AbsoluteReadX() );
            break;
            
        case 0x79:  // ADC Absolute, Y
            Adc( AbsoluteReadY() );
            break;
            
        case 0x61:  // ADC (Indirect, X)
            Adc( IndirectReadX() );
            break;
            
        case 0x71:  // ADC (Indirect), Y
            Adc( IndirectReadY() );
            break;
            
        case 0x29:  // AND Immediate
            _regA = _regA & _bus->ReadByte( _regPC + 1 );
            SetNZ( _regA );
            break;
            
        case 0x25:  // AND Zero Page
            _regA = _regA & ZeroPageRead();
            SetNZ( _regA );
            break;
            
        case 0x35:  // AND Zero Page, X
            _regA = _regA & ZeroPageReadX();
            SetNZ( _regA );
            break;
            
        case 0x2d:  // AND Absolute
            _regA = _regA & AbsoluteRead();
            SetNZ( _regA );
            break;
            
        case 0x3d:  // AND Absolute, X
            _tmpByte = AbsoluteReadX();
            _regA &= _tmpByte;
            SetNZ( _regA );
            break;
            
        case 0x39:  // AND Absolute, Y
            _tmpByte = AbsoluteReadY();
            _regA &= _tmpByte;
            SetNZ( _regA );
            break;
            
        case 0x21:  // AND (Indirect, X)
            _regA = _regA & IndirectReadX();
            SetNZ( _regA );
            break;
            
        case 0x31:  // AND (Indirect), Y
            _regA = _regA & IndirectReadY();
            SetNZ( _regA );
            break;
            
        case 0x0a:  // ASL Accumulator
            _flagC = (_regA & 0x80) >> 7;
            _regA <<= 1;
            //_regA &= 0xff;
            SetNZ( _regA );
            break;
            
        case 0x06:  // ASL Zero Page
            _tmpByte = ZeroPageRead();
            _flagC = (_tmpByte & 0x80) >> 7;
            _tmpByte <<= 1;
            //_tmpByte &= 0xff;
            SetNZ( _tmpByte );
            ZeroPageStore( _tmpByte );
            break;
            
        case 0x16:  // ASL Zero Page, X
            _tmpByte = ZeroPageReadX();
            _flagC = (_tmpByte & 0x80) >> 7;
            _tmpByte <<= 1;
            //_tmpByte &= 0xff;
            SetNZ( _tmpByte );
            ZeroPageStoreX( _tmpByte );
            break;
            
        case 0x0e:  // ASL Absolute
            _tmpByte = AbsoluteRead();
            _flagC = (_tmpByte & 0x80) >> 7;
            _tmpByte <<= 1;
            //_tmpByte &= 0xff;
            SetNZ( _tmpByte );
            AbsoluteStore( _tmpByte );
            break;
            
        case 0x1e:  // ASL Absolute, X
            _tmpByte = AbsoluteReadX();
            _flagC = (_tmpByte & 0x80) >> 7;
            _tmpByte <<= 1;
            SetNZ( _tmpByte );
            AbsoluteStoreX( _tmpByte );
            break;
            
        case 0x24:  // BIT Zero Page
            _tmpByte = ZeroPageRead();
            _flagZ = (_regA & _tmpByte) == 0 ? 1 : 0;
            _flagV = (_tmpByte & 0x40) >> 6;
            _flagN = (_tmpByte & 0x80) >> 7;
            break;
            
        case 0x2c:  // BIT Absolute
            _tmpByte = AbsoluteRead();
            _flagZ = (_regA & _tmpByte) == 0 ? 1 : 0;
            _flagV = (_tmpByte & 0x40) >> 6;
            _flagN = (_tmpByte & 0x80) >> 7;
            break;
            
        case 0x90:  // BCC Branch if Carry Clear
            if (_flagC == 0)
            {
                return Branch();
            }
            break;
            
        case 0xb0:  // BCS Branch if Carry Set
            if (_flagC == 1)
            {
                return Branch();
            }
            break;
            
        case 0xf0:  // BEQ Branch if Equal
            if (_flagZ == 1)
            {
                return Branch();
            }
            break;
            
        case 0x30:  // BMI Branch if Minus
            if (_flagN == 1)
            {
                return Branch();
            }
            break;
            
        case 0xd0:  // BNE Branch if Not Equal
            if (_flagZ == 0)
            {
                return Branch();
            }
            break;
            
        case 0x10:  // BPL Branch if Positive
            if (_flagN == 0)
            {
                return Branch();
            }
            break;
            
        case 0x50:  // BVC Branch if Overflow Clear
            if (_flagV == 0)
            {
                return Branch();
            }
            break;
            
        case 0x70:  // BVS Branch if Overflow Set
            if (_flagV == 1)
            {
                return Branch();
            }
            break;
            
        case 0x00: 	// BRK
            PushWord( _regPC + 2 );
            
            PushByte( GetFlags() | 0x10);
            _flagI = 1;
            
            _regPC = _bus->ReadWord( _brkVector );
            return OpcodeCycles[_opcode];
            
        case 0x18:  // CLC
            _flagC = 0;
            break;
            
        case 0x58:  // CLI
            _flagI = 0;
            break;
            
        case 0xd8:  // CLD
            _flagD = 0;
            break;
            
        case 0xb8:  // CLV
            _flagV = 0;
            break;
            
        case 0xc9:  // CMP Immediate
            _tmpByte = _bus->ReadByte( _regPC + 1 );
            SetNZ( _regA - _tmpByte );
            _flagC = _regA >= _tmpByte ? 1 : 0;
            break;
            
        case 0xc5:  // CMP Zero Page
            _tmpByte = ZeroPageRead();
            SetNZ( _regA - _tmpByte );
            _flagC = _regA >= _tmpByte ? 1 : 0;
            break;
            
        case 0xd5:  // CMP Zero Page, X
            _tmpByte = ZeroPageReadX();
            SetNZ( _regA - _tmpByte );
            _flagC = _regA >= _tmpByte ? 1 : 0;
            break;
            
        case 0xcd:  // CMP Absolute
            _tmpByte = AbsoluteRead();
            SetNZ( _regA - _tmpByte );
            _flagC = _regA >= _tmpByte ? 1 : 0;
            break;
            
        case 0xdd:  // CMP Absolute, X
            _tmpByte = AbsoluteReadX();
            SetNZ( _regA - _tmpByte );
            _flagC = _regA >= _tmpByte ? 1 : 0;
            break;
            
        case 0xd9:  // CMP Absolute Y
            _tmpByte = AbsoluteReadY();
            SetNZ( _regA - _tmpByte );
            _flagC = _regA >= _tmpByte ? 1 : 0;
            break;
            
        case 0xc1:  // CMP (Indirect, X)
            _tmpByte = IndirectReadX();
            SetNZ( _regA - _tmpByte );
            _flagC = _regA >= _tmpByte ? 1 : 0;
            break;
            
        case 0xd1:  // CMP (Indirect), Y
            _tmpByte = IndirectReadY();
            SetNZ( _regA - _tmpByte );
            _flagC = _regA >= _tmpByte ? 1 : 0;
            break;
            
        case 0xe0:  // CPX Immediate
            _tmpByte = _bus->ReadByte( _regPC + 1 );
            SetNZ( _regX - _tmpByte );
            _flagC = _regX >= _tmpByte ? 1 : 0;
            break;
            
        case 0xe4:  // CPX Zero Page
            _tmpByte = ZeroPageRead();
            SetNZ( _regX - _tmpByte );
            _flagC = _regX >= _tmpByte ? 1 : 0;
            break;
            
        case 0xec:  // CPX Absolute
            _tmpByte = AbsoluteRead();
            SetNZ( _regX - _tmpByte );
            _flagC = _regX >= _tmpByte ? 1 : 0;
            break;
            
        case 0xc0:  // CPY Immediate
            _tmpByte = _bus->ReadByte( _regPC + 1 );
            SetNZ( _regY - _tmpByte );
            _flagC = _regY >= _tmpByte ? 1 : 0;
            break;
            
        case 0xc4:  // CPY Zero Page
            _tmpByte = ZeroPageRead();
            SetNZ( _regY - _tmpByte );
            _flagC = _regY >= _tmpByte ? 1 : 0;
            break;
            
        case 0xcc:  // CPY Absolute
            _tmpByte = AbsoluteRead();
            SetNZ( _regY - _tmpByte );
            _flagC = _regY >= _tmpByte ? 1 : 0;
            break;
            
        case 0xc6:  // DEC Zero Page
            _tmpByte = (ZeroPageRead() - 1);
            ZeroPageStore( _tmpByte );
            SetNZ( _tmpByte );
            break;
            
        case 0xd6:  // DEC Zero Page, X
            _tmpByte = (ZeroPageReadX() - 1);
            ZeroPageStoreX( _tmpByte );
            SetNZ( _tmpByte );
            break;
            
        case 0xce:  // DEC Absolute
            _tmpByte = (AbsoluteRead() - 1);
            AbsoluteStore( _tmpByte );
            SetNZ( _tmpByte );
            break;
            
        case 0xde:  // DEC Absolute X
            _tmpByte = (AbsoluteReadX() - 1);
            AbsoluteStoreX( _tmpByte );
            SetNZ( _tmpByte );
            break;
            
        case 0xca:  // DEX
            _regX = (_regX - 1 );
            SetNZ( _regX );
            break;
            
        case 0x88:  // DEY
            _regY = (_regY - 1 );
            SetNZ( _regY );
            break;
            
        case 0x49:  // EOR Immediate
            _regA ^= _bus->ReadByte( _regPC + 1);
            SetNZ( _regA );
            break;
            
        case 0x45:  // EOR Zero Page
            _regA ^= ZeroPageRead();
            SetNZ( _regA );
            break;
            
        case 0x55:  // EOR Zero Page, X
            _regA ^= ZeroPageReadX();
            SetNZ( _regA );
            break;
            
        case 0x4d:  // EOR Absolute
            _regA ^= AbsoluteRead();
            SetNZ( _regA );
            break;
            
        case 0x5d:  // EOR Absolute, X
            _regA ^= AbsoluteReadX();
            SetNZ( _regA );
            break;
            
        case 0x59:  // EOR Absolute, Y
            _regA ^= AbsoluteReadY();
            SetNZ( _regA );
            break;
            
        case 0x41:  // EOR (Indirect, X)
            _regA ^= IndirectReadX();
            SetNZ( _regA );
            break;
            
        case 0x51:  // EOR (Indirect), Y
            _regA ^= IndirectReadY();
            SetNZ( _regA );
            break;
            
        case 0xe6:  // INC Zero Page
            _tmpByte = (ZeroPageRead() + 1);
            ZeroPageStore( _tmpByte );
            SetNZ( _tmpByte );
            break;
            
        case 0xf6:  // INC Zero Page, X
            _tmpByte = (ZeroPageReadX() + 1);
            ZeroPageStoreX( _tmpByte );
            SetNZ( _tmpByte );
            break;
            
        case 0xee:  // INC Absolute
            _tmpByte = (AbsoluteRead() + 1);
            AbsoluteStore( _tmpByte );
            SetNZ( _tmpByte );
            break;
            
        case 0xfe:  // INC Absolute, X
            _tmpByte = (AbsoluteReadX() + 1);
            AbsoluteStoreX( _tmpByte );
            SetNZ( _tmpByte );
            break;
            
        case 0xe8:  // INX
            _regX = (_regX + 1);
            SetNZ( _regX );
            break;
            
        case 0xc8:  // INY
            _regY = (_regY + 1);
            SetNZ( _regY );
            break;
            
        case 0x4c:  // JMP Absolute
            _regPC = _bus->ReadWord( _regPC + 1 );
            return OpcodeCycles[_opcode];
            
        case 0x6c:  // JMP (Indirect)
        {
            u16 tmpAddr = _bus->ReadWord(_regPC + 1);

            // 6502 indirect JMP bug
            if ((tmpAddr & 0xff) != 0xff)
            {
                _regPC = _bus->ReadWord(tmpAddr);
            }
            else
            {
                _regPC = _bus->ReadByte(tmpAddr);
                _regPC |= _bus->ReadByte(tmpAddr & 0xff00) << 8;
            }

            return OpcodeCycles[_opcode];;
        }
            
        case 0x20:  // JSR
            PushWord( _regPC + 2 );
            _regPC = _bus->ReadWord( _regPC + 1 );
            return OpcodeCycles[_opcode];;
            
        case 0xa9:  // LDA Immediate
            _regA = _bus->ReadByte( _regPC + 1 );
            SetNZ( _regA );
            break;
            
        case 0xa5:  // LDA Zero Page
            _regA = ZeroPageRead();
            SetNZ( _regA );
            break;
            
        case 0xb5:  // LDA Zero Page, X
            _regA = ZeroPageReadX();
            SetNZ( _regA );
            break;
            
        case 0xaD:  // LDA Absolute
            _regA = AbsoluteRead();
            SetNZ( _regA );
            break;
            
        case 0xbD:  // LDA Absolute, X
            _regA = AbsoluteReadX();
            SetNZ( _regA );
            break;
            
        case 0xb9:  // LDA Absolute, Y
            _regA = AbsoluteReadY();
            SetNZ( _regA );
            break;
            
        case 0xa1:  // LDA (Indirect, X)
            _regA = IndirectReadX();
            SetNZ( _regA );
            break;
            
        case 0xb1:  // LDA (Indirect), Y
            _regA = IndirectReadY();
            SetNZ( _regA );
            break;
            
        case 0xa2:  // LDX Immediate
            _regX = _bus->ReadByte( _regPC + 1 );
            SetNZ( _regX );
            break;
            
        case 0xa6:  // LDX Zero Page
            _regX = ZeroPageRead();
            SetNZ( _regX );
            break;
            
        case 0xb6:  // LDX Zero Page, Y
            _regX = ZeroPageReadY();
            SetNZ( _regX );
            break;
            
        case 0xae:  // LDX Absolute
            _regX = AbsoluteRead();
            SetNZ( _regX );
            break;
            
        case 0xbe:  // LDX Absolute, Y
            _regX = AbsoluteReadY();
            SetNZ( _regX );
            break;
            
        case 0xa0:  // LDY Immediate
            _regY = _bus->ReadByte( _regPC + 1 );
            SetNZ( _regY );
            break;
            
        case 0xa4:  // LDY Zero Page
            _regY = ZeroPageRead();
            SetNZ( _regY );
            break;
            
        case 0xb4:  // LDY Zero Page, X
            _regY = ZeroPageReadX();
            SetNZ( _regY );
            break;
            
        case 0xac:  // LDY Absolute
            _regY = AbsoluteRead();
            SetNZ( _regY );
            break;
            
        case 0xbc:  // LDY Absolute, X
            _regY = AbsoluteReadX();
            SetNZ( _regY );
            break;
            
        case 0x4a:  // LSR Accumulator
            _flagC = _regA & 1;
            _regA >>= 1;
            SetNZ( _regA );
            break;
            
        case 0x46:  // LSR Zero Page
            _tmpByte = ZeroPageRead();
            _flagC = _tmpByte & 1;
            _tmpByte >>= 1;
            ZeroPageStore( _tmpByte );
            SetNZ( _tmpByte );
            break;
            
        case 0x56:  // LSR Zero Page, X
            _tmpByte = ZeroPageReadX();
            _flagC = _tmpByte & 1;
            _tmpByte >>= 1;
            ZeroPageStoreX( _tmpByte );
            SetNZ( _tmpByte );
            break;
            
        case 0x4e:  // LSR Absolute
            _tmpByte = AbsoluteRead();
            _flagC = _tmpByte & 1;
            _tmpByte >>= 1;
            AbsoluteStore( _tmpByte );
            SetNZ( _tmpByte );
            break;
            
        case 0x5e:  // LSR Absolute, X
            _tmpByte = AbsoluteReadX();
            _flagC = _tmpByte & 1;
            _tmpByte >>= 1;
            AbsoluteStoreX( _tmpByte );
            SetNZ( _tmpByte );
            break;
            
        case 0xea:  // NOP
            break;
            
        case 0x09:  // ORA Immediate
            _regA |= _bus->ReadByte( _regPC + 1 );
            SetNZ( _regA );
            break;
            
        case 0x05:  // ORA Zero Page
            _regA |= ZeroPageRead();
            SetNZ( _regA );
            break;
            
        case 0x15:  // ORA Zero Page, X
            _regA |= ZeroPageReadX();
            SetNZ( _regA );
            break;
            
        case 0x0d:  // ORA Absolute
            _regA |= AbsoluteRead();
            SetNZ( _regA );
            break;
            
        case 0x1d:  // ORA Absolute, X
            _regA = _regA | AbsoluteReadX();
            SetNZ( _regA );
            break;
            
        case 0x19:  // ORA Absolute, Y
            _regA = _regA | AbsoluteReadY();
            SetNZ( _regA );
            break;
            
        case 0x01:  // ORA (Indirect, X)
            _regA |= IndirectReadX();
            SetNZ( _regA );
            break;
            
        case 0x11:  // ORA (Indirect), Y
            _regA |= IndirectReadY();
            SetNZ( _regA );
            break;
            
        case 0x48:  // PHA
            PushByte( _regA );
            break;
            
        case 0x08:  // PHP
            PushByte( GetFlags() | 0x10);
            break;
            
        case 0x68:  // PLA
            _regA = PopByte();
            SetNZ( _regA );
            break;
            
        case 0x28:  // PLP
            SetFlags( PopByte() );
            break;
            
        case 0x2a:  // ROL Accumulator
            _regA = Rol( _regA );
            break;
            
        case 0x26:  // ROL Zero Page
            ZeroPageStore( Rol( ZeroPageRead() ) );
            break;
            
        case 0x36:  // ROL Zero Page, X
            ZeroPageStoreX( Rol( ZeroPageReadX() ) );
            break;
            
        case 0x2e:  // ROL Absolute
            AbsoluteStore( Rol( AbsoluteRead() ) );
            break;
            
        case 0x3e:  // ROL Absolute, X
            AbsoluteStoreX( Rol( AbsoluteReadX() ) );
            break;
            
        case 0x6a:  // ROR Accumulator
            _regA = ( Ror( _regA ) );
            break;
            
        case 0x66:  // ROR Zero Page
            ZeroPageStore( Ror( ZeroPageRead() ) );
            break;
            
        case 0x76:  // ROR Zero Page, X
            ZeroPageStoreX( Ror( ZeroPageReadX() ) );
            break;
            
        case 0x6e:  // ROR Absolute
            AbsoluteStore( Ror( AbsoluteRead() ) );
            break;
            
        case 0x7e:  // ROR Absolute, X
            AbsoluteStoreX( Ror( AbsoluteReadX() ) );
            break;
            
        case 0x40:  // RTI
            SetFlags( PopByte() );
            _regPC = PopWord();
            return OpcodeCycles[_opcode];
            
        case 0x60:  // RTS
            _regPC = PopWord() + 1;
            return OpcodeCycles[_opcode];;
            
        case 0xe9:  // SBC Immediate
            Sbc( _bus->ReadByte( _regPC + 1 ) );
            break;
            
        case 0xe5:  // SBC Zero Page
            Sbc( ZeroPageRead() );
            break;
            
        case 0xf5:  // SBC Zero Page, X
            Sbc( ZeroPageReadX() );
            break;
            
        case 0xed:  // SBC Absolute
            Sbc( AbsoluteRead() );
            break;
            
        case 0xfd:  // SBC Absolute X
            Sbc( AbsoluteReadX() );
            break;
            
        case 0xf9:  // SBC Absolute Y
            Sbc( AbsoluteReadY() );
            break;
            
        case 0xe1:  // SBC (Indirect, X)
            Sbc( IndirectReadX() );
            break;
            
        case 0xf1:  // SBC (Indirect), Y
            Sbc( IndirectReadY() );
            break;
            
        case 0x38:  // SEC
            _flagC = 1;
            break;
            
        case 0xf8:  // SED
            _flagD = 1;
            break;
            
        case 0x78:  // SEI
            _flagI = 1;
            break;
            
        case 0x8d:  // STA Absolute
            AbsoluteStore( _regA );
            break;
            
        case 0x9d:  // STA Absolute, X
            AbsoluteStoreX( _regA );
            break;
            
        case 0x99:  // STA Absolute, Y
            AbsoluteStoreY( _regA );
            break;
            
        case 0x85:  // STA Zero Page
            ZeroPageStore( _regA );
            break;
            
        case 0x95:  // STA Zero Page, X
            ZeroPageStoreX( _regA );
            break;
            
        case 0x81:  // STA (Indirect, X)
            IndirectStoreX( _regA );
            break;
            
        case 0x91:  // STA (Indirect), Y
            IndirectStoreY( _regA );
            break;
            
        case 0x86:  // STX Zero Page
            ZeroPageStore( _regX );
            break;
            
        case 0x96:  // STX Zero Page, Y
            ZeroPageStoreY( _regX );
            break;
            
        case 0x8e:  // STX Absolute
            AbsoluteStore( _regX );
            break;
            
        case 0x84:  // STY Zero Page
            ZeroPageStore( _regY );
            break;
            
        case 0x94:  // STY Zero Page, X
            ZeroPageStoreX( _regY );
            break;
            
        case 0x8c:  // STY Absolute
            AbsoluteStore( _regY );
            break;
            
        case 0xa8:  // TAY
            _regY = _regA;
            SetNZ( _regY );
            break;
            
        case 0xaa:  // TAX
            _regX = _regA;
            SetNZ( _regX );
            break;
            
        case 0xba:  // TSX
            _regX = _regSP;
            SetNZ( _regX );
            break;
            
        case 0x8a:  // TXA
            _regA = _regX;
            SetNZ( _regA );
            break;
            
        case 0x9a:  // TXS
            _regSP = _regX;
            break;
            
        case 0x98:  // TYA
            _regA = _regY;
            SetNZ( _regA );
            break;
            
        default:
            assert( false );
            return 0;
    }
    
    if (OpcodeSizes[_opcode] == 0)
    {
        assert( false );
    }
    
    _regPC = (_regPC + OpcodeSizes[_opcode]);
    return OpcodeCycles[_opcode];;
}



