// OldNES.cpp : Defines the entry point for the console application.
//

#include <tchar.h>

#include "../Engine.h"

int _tmain(int argc, _TCHAR* argv[])
{
    //auto engine = new Engine( "D:\\dev\\testroms\\charlie.nes");
    //auto engine = new Engine( "D:\\dev\\testroms\\donkey.nes");
    //auto engine = new Engine( "D:\\dev\\testroms\\10yards.nes");
    //auto engine = new Engine( "D:\\dev\\testroms\\elevator.nes");
    //auto engine = new Engine( "D:\\dev\\testroms\\ice.nes");
    auto engine = new Engine( "D:\\dev\\testroms\\smb.nes" );
    //auto engine = new Engine( "D:\\dev\\testroms\\digdug.nes" );
    //auto engine = new Engine( "D:\\dev\\testroms\\blargg_palette_ram.nes");
    //auto engine = new Engine( "D:\\dev\\testroms\\blargg_power_up_palette.nes");
    //auto engine = new Engine( "D:\\dev\\testroms\\blargg_sprite_ram.nes");
    //auto engine = new Engine( "D:\\dev\\testroms\\blargg_vbl_clear_time.nes"); // VBL flag cleared too late
    //auto engine = new Engine( "D:\\dev\\testroms\\blargg_vram_access.nes"); // pass!
    //auto engine = new Engine( "D:\\dev\\testroms\\nestest.nes");   // pass!
    //auto engine = new Engine( "D:\\dev\\testroms\\nestress.nes"); 
    //auto engine = new Engine( "F:\\dev\\testroms\\cpu_official_only.nes" ); // pass!
    //auto engine = new Engine( "F:\\dev\\testroms\\mm2.nes" );

    engine->Run();
    return 0;
}



