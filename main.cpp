#include "DirectKey.h"

#include <stdio.h>
#include <stdlib.h>

using namespace DirectKey;

int main( void )
{
    Key::SetReadKeyAwaitTimeout( KeyDelay::FPS_60 );

    bool isQuit = false;
    while( !isQuit ) {

        // If you use Key::GetKeyTimeout(), Until a key is entered,
        // '.' is output repeatedly in the console.
        // KeyBoard input = Key::GetKeyTimeout();

        // If you use Key::GetKey(), Until a key is entered,
        // '.' is not output and waits in GetKey()
        KeyBoard input = Key::GetKey();

        switch( input ) {
        case KeyBoard::ARROW_UP:    printf("[UP]");    break;
        case KeyBoard::ARROW_DOWN:  printf("[DOWN]");  break;
        case KeyBoard::ARROW_RIGHT: printf("[RIGHT]"); break;
        case KeyBoard::ARROW_LEFT:  printf("[LEFT]");  break;

        case KeyBoard::SPACE:       printf("[SPACE]"); break;
        case KeyBoard::ENTER:       printf("[ENTER]"); break;
        case KeyBoard::TAB:         printf("[TAB]");   break;

        case KeyBoard::F1:          printf("[F1]");    break;
        case KeyBoard::F2:          printf("[F2]");    break;
        case KeyBoard::F3:          printf("[F3]");    break;
        case KeyBoard::F4:          printf("[F4]");    break;
        case KeyBoard::F5:          printf("[F5]");    break;
        case KeyBoard::F6:          printf("[F6]");    break;
        case KeyBoard::F7:          printf("[F7]");    break;
        case KeyBoard::F8:          printf("[F8]");    break;
        case KeyBoard::F9:          printf("[F9]");    break;
        case KeyBoard::F10:         printf("[F10]");   break;
        case KeyBoard::F11:         printf("[F11]");   break;
        case KeyBoard::F12:         printf("[F12]");   break;

        // For Multi-Thread thread-safe, not use it
        case KeyBoard::ALREADY_OCCUPIED:
            printf("{ KEY INPUt ALREADY OCCUPIED }\n");
            break;

        // Quit
        case KeyBoard::ESC:
            isQuit = true;
            break;

        // If the Key::GetKeyTimeout() function is over time, it enters the case.
        case KeyBoard::NONE_INPUT:
            printf(".");
            break;

        default:
            printf("[%c]", static_cast<int>( input ));
            break;
        }
        fflush( stdout ); // buffer flush
    } // while( !isQuit )

    printf("\n");
    return 0;
}