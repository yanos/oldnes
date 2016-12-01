//
//  Input.h
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-23.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#ifndef __OldNES__Input__
#define __OldNES__Input__

#include <stdio.h>
#include "Common.h"

class Input
{
    
public:
    
    Input();
    ~Input();
    
    byte ReadByte( addr );
    word ReadWord( addr );
    void WriteByte( addr, byte );
    
    void ReadInputs();
    
private:
    
    enum Button { A, B, Select, Start, Up, Down, Left, Right };
    
    u8   _strobe = 0;
    u8   _bitNum = 0;
    bool _buttonStates[8];

};

#endif /* defined(__OldNES__Input__) */
