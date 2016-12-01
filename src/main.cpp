//
//  main.cpp
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-21.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#include <iostream>
#include <unistd.h>

#include "engine.h"

int main( int argc, const char* argv[] )
{
    Engine *engine;
    //engine = new Engine( "/Users/Karinee/Documents/yanos/smb.nes" );
    
    //engine = new Engine( "/Users/yanos/Downloads/testroms/charlie.nes" );
    engine = new Engine( "/Users/yanos/Downloads/testroms/smb.nes" );
    
    //engine = new Engine( "/Users/yanos/Downloads/testroms/10yards.nes" );
    //engine = new Engine( "/Users/yanos/Downloads/testroms/donkey.nes" );
    //engine = new Engine( "/Users/yanos/Downloads/testroms/ice.nes" );
    //engine = new Engine( "/Users/yanos/Downloads/testroms/elevator.nes" );
    //engine = new Engine( "/Users/yanos/Downloads/testroms/digdug.nes" );
    //engine = new Engine( "/Users/yanos/Downloads/testroms/excitebike.nes" );
    //engine = new Engine( "/Users/yanos/Downloads/testroms/pacman.nes" );
    //auto engine = new Engine( "/Users/yanos/Downloads/testroms/blargg_vbl_clear_time.nes" );
    //auto engine = new Engine( "/Users/yanos/Downloads/testroms/blargg_vram_access.nes" );
    //auto engine = new Engine( "/Users/yanos/Downloads/testroms/blargg_palette_ram.nes" );
    //auto engine = new Engine( "/Users/yanos/Downloads/testroms/blargg_sprite_ram.nes" );
    //auto engine = new Engine( "/Users/yanos/Downloads/testroms/official_only.nes" );
    //auto engine = new Engine( "/Users/yanos/Downloads/testroms/nestest.nes" );
    //auto engine = new Engine( "/Users/yanos/Downloads/testroms/nestress.nes" );
    
    engine->Run();
    
    return 0;
}
