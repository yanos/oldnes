//
//  Input.cpp
//  OldNES
//
//  Created by Yannick Cholette on 2015-05-23.
//  Copyright (c) 2015 El Choletto. All rights reserved.
//

#include <SDL.h>

#include "Input.h"

Input::Input()
{

}

Input::~Input()
{

}

byte Input::ReadByte( addr address )
{
    u8 val = 0x40;
    
    if (address != 0x4016)
        return val;
    
    if (_bitNum > 23)
        return val;
    
    switch (_bitNum)
    {
        case 0:     // A
            val = _buttonStates[A] == true ? 0x41 : 0x40;
            break;
            
        case 1:     // B
            val = _buttonStates[B] == true ? 0x41 : 0x40;
            break;
            
        case 2:     // SELECT
            val = _buttonStates[Select] == true ? 0x41 : 0x40;
            break;
            
        case 3:     // START
            val = _buttonStates[Start] == true ? 0x41 : 0x40;
            if (_buttonStates[Start])
            {
                //_buttonStates[Start] = false;
            }
            break;
            
        case 4:     // UP
            val = _buttonStates[Up] == true ? 0x41 : 0x40;
            break;
            
        case 5:     // DOWN
            val = _buttonStates[Down] == true ? 0x41 : 0x40;
            break;
            
        case 6:     // LEFT
            val = _buttonStates[Left] == true ? 0x41 : 0x40;
            break;
            
        case 7:     // RIGHT
            val = _buttonStates[Right] == true ? 0x41 : 0x40;
            break;
            
        case 19:    // ctrl 1 pluged
            val = 0x41;
            break;
    }
    
    _bitNum++;
    
    return val;
}

word Input::ReadWord( addr )
{
    return 0;
}

void Input::WriteByte( addr address, byte value )
{
    if (address == 0x4016)
    {
        value &= 1;
        
        if (value == 0 && _strobe == 1)
        {
            _bitNum = 0;
        }
        
        _strobe = value;
    }
}

void Input::ReadInputs()
{
    SDL_PumpEvents();
    
    auto states = SDL_GetKeyboardState( nullptr );
    
    _buttonStates[ A ]      = states[SDL_SCANCODE_X] != 0;
    _buttonStates[ B ]      = states[SDL_SCANCODE_Z] != 0;
    _buttonStates[ Start ]  = states[SDL_SCANCODE_RETURN] != 0;
    _buttonStates[ Select ] = states[SDL_SCANCODE_TAB] != 0;
    _buttonStates[ Up ]     = states[SDL_SCANCODE_UP] != 0;
    _buttonStates[ Down ]   = states[SDL_SCANCODE_DOWN] != 0;
    _buttonStates[ Left ]   = states[SDL_SCANCODE_LEFT] != 0;
    _buttonStates[ Right ]  = states[SDL_SCANCODE_RIGHT] != 0;
}