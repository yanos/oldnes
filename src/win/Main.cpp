// OldNES.cpp : Defines the entry point for the console application.
//

#include <tchar.h>

#include "../Engine.h"

int _tmain( int argc, _TCHAR* argv[] )
{
    if (argc != 2)
        return -1;

    auto engine = new Engine( argv[1] );
    engine->Run();

    delete engine;

    return 0;
}



