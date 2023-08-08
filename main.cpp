#include "AsyncKey.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

using namespace AsyncKey;

static void signal_handler( int signo )
{
    if( signo == SIGINT )
        Key::deinit();

    exit(0);
}

int main( void )
{
    Key::init();
    signal( SIGINT, signal_handler );

    bool isQuit = false;
    while( !isQuit ) {
        // Async Read Key
        KeyBoard input = Key::readAsyncKey();

        // If you use Key::readKey(), Until a key is entered,
        // '.' is not output and waits in readKey()
        printf(".");

        switch (input)
        {
        case KeyBoard::UP_ARROW:    printf("[UP]");    break;
        case KeyBoard::DOWN_ARROW:  printf("[DOWN]");  break;
        case KeyBoard::RIGHT_ARROW: printf("[RIGHT]"); break;
        case KeyBoard::LEFT_ARROW:  printf("[LEFT]");  break;
        case KeyBoard::SPACEBAR:    printf("[SPACE]"); break;
        case KeyBoard::ENTER:       printf("[ENTER]"); break;

        // Quit
        case KeyBoard::ESCAPE:
            isQuit = true;
            break;

        case KeyBoard::NONE_INPUT:
            break;

        default:
            printf("[%c]", static_cast<int>( input ));
            break;
        }

        fflush(stdout);
        usleep(1000000/60);
    }

    Key::deinit();
    printf("\n");

    return 0;
}