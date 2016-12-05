//
//  Rom.cpp
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-22.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#include <fstream>

#include "Rom.h"

std::shared_ptr<Rom> Rom::Load( string filename )
{
    // TODO: iNES 2.0 support
    
    std::ifstream is( filename, std::ifstream::binary );
    if (!is)
        return nullptr;
    
    std::unique_ptr<u8[]> header( new u8[_headerSize] );
    is.read( (char*)header.get(), _headerSize );
        
    if (header[0] != 0x4e || header[1] != 0x45 || header[2] != 0x53 || header[3] != 0x1a)
    {
        return nullptr;
    }
        
    u32 prgRomSize = header[4] * 0x4000U;
    u32 chrRomSize = header[5] * 0x2000U;
        
    // Flag 6
    u8 flag6        = header[6];
    bool hasSRAM    = (flag6 & 0x2) > 0;
    bool hasTrainer = (flag6 & 0x4) > 0;
    u8 lMapper      = flag6 >> 4;
    
    MirroringMode mirroring = Horizontal;
    if ((flag6 & 0x8) > 0)
        mirroring = FourScreen;
    else if((flag6 & 0x1) == 0 && (flag6 & 0x8) == 0)
        mirroring = Horizontal;
    else if ((flag6 & 0x1) > 0 && (flag6 & 0x8) == 0)
        mirroring = Vertical;
    //else
      //  TODO ASSERT! Logger.Log( Logger.Level.Error, "Rom(): Invalid mirroring mode." );
        
    // Flag 7
    u8 flag7         = header[7];
    bool vsUnisystem = (flag7 & 0x1) > 0;
    bool playChoice  = (flag7 & 0x2) > 0;
    bool nes2Format  = (flag7 & 0x4) == 0 && (flag7 & 0x5) > 0;
    u8 hMapper       = flag7 >> 4;
        
    // Flag 8
    u8 prgRamSize = header[8];
    
    // combine mapper value
    u8 mapperId = (lMapper | (hMapper << 4));
        
    // seek past header and possible trainer
    is.seekg( (_headerSize + (hasTrainer ? 512 : 0)) );
    
    std::unique_ptr<u8[]> prgRomData;
    std::unique_ptr<u8[]> chrRomData;
    
    if (prgRomSize > 0)
    {
        prgRomData = std::unique_ptr<u8[]>( new u8[prgRomSize] );
        is.read( (char*)prgRomData.get(), prgRomSize );
    }
        
    if (chrRomSize > 0)
    {
        chrRomData = std::unique_ptr<u8[]>( new u8[chrRomSize] );
        is.read( (char*)chrRomData.get(), chrRomSize );
    }
    
    return std::shared_ptr<Rom>(
        new Rom( prgRomSize,
                 prgRamSize,
                 chrRomSize,
                 hasSRAM,
                 hasTrainer,
                 vsUnisystem,
                 playChoice,
                 nes2Format,
                 mapperId,
                 mirroring,
                 std::move( prgRomData ),
                 std::move( chrRomData ) ) );
}

Rom::Rom( u32                   prgRomSize,
          u32                   prgRamSize,
          u32                   chrRomSize,
          bool                  hasSRAM,
          bool                  hasTrainer,
          bool                  vsUnisystem,
          bool                  playChoice,
          bool                  nes2Format,
          u8                    mapperId,
          MirroringMode         mirroring,
          std::unique_ptr<u8[]> prgRomData,
          std::unique_ptr<u8[]> chrRomData)

    :   PrgRomSize  ( prgRomSize              ),
        PrgRamSize  ( prgRamSize              ),
        ChrRomSize  ( chrRomSize              ),
        HasSRAM     ( hasSRAM                 ),
        HasTrainer  ( hasTrainer              ),
        VSUnisystem ( vsUnisystem             ),
        PlayChoice  ( playChoice              ),
        Nes2Format  ( nes2Format              ),
        MapperId    ( mapperId                ),
        Mirroring   ( mirroring               ),
        PrgRomData  ( std::move( prgRomData ) ),
        ChrRomData  ( std::move( chrRomData ) )
{

}