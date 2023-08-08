#ifndef _ASYNC_KEY_LINUX_CPP_
#define _ASYNC_KEY_LINUX_CPP_

#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <memory.h>
#include <sys/select.h>

namespace AsyncKey
{
constexpr int ARROW_ADD_VALUE = 1000;
enum KeyBoard
{
    // alphabet : only use uppercase
    A = 65, B = 66, C = 67, D = 68, E = 69, F = 70, G = 71, H = 72, I = 73,
    J = 74, K = 75, L = 76, M = 77, N = 78, O = 79, P = 80, Q = 81, R = 82,
    S = 83, T = 84, U = 85, V = 86, W = 87, X = 88, Y = 89, Z = 90,

    // numbers
    NUM_0 = 48, NUM_1 = 49, NUM_2 = 50, NUM_3 = 51, NUM_4 = 52,
    NUM_5 = 53, NUM_6 = 54, NUM_7 = 55, NUM_8 = 56, NUM_9 = 57,

    // special keys
    TAB = 9, ENTER = 10, ESCAPE = 27, SPACEBAR = 32, BACKSPACE = 127,

    // arrows
    UP_ARROW    = 65 + ARROW_ADD_VALUE,
    DOWN_ARROW  = 66 + ARROW_ADD_VALUE,
    RIGHT_ARROW = 67 + ARROW_ADD_VALUE,
    LEFT_ARROW  = 68 + ARROW_ADD_VALUE,

    // default : none defined
    NONE_INPUT = -2,
};

// for check & capitalize
enum LowerKeyBoard
{
    a = A + 32,
    z = Z + 32
};

typedef struct termios Termios;

// The class consists of pure static functions without constructors.
class Key
{
public:
    /**
     * Key class initialize.
     *
     * Before using Key::readKey() or Key::readAsyncKey()
     * of the Key class, you MUST do Key::init().
     */
    static void init();

    /**
     * Key class deinitialize.
     *
     * After using keystrokes or before exiting
     * the program, you MUST do Key::deinit().
     */
    static void deinit();

    /**
     * Reading key values synchronously.
     *
     * The function waits until you enter a key.
     *
     * @return entered key
     */
    static const KeyBoard readKey();

    /**
     * Reading key values asynchronously.
     *
     * The function goes to the next code
     * without waiting if no key is entered.
     *
     * @return key value if you entered, or NONE_INPUT if not entered.
    */
    static const KeyBoard readAsyncKey();

private:
    static Termios* getSaveAttr() {
        static Termios save_attr;
        return &save_attr;
    }

    static bool* isOnDirectInput() {
        static bool isOn = false;
        return &isOn;
    }

    /**
     * A function that turns on/off so that
     * keystrokes can be received immediately
     * without pressing Enter.
     */
    static void setOnDirectInput();
    static void setOffDirectInput();

}; // cls: Key

// Hide cursor on console
inline static void cursor_off( void )
{
    printf("\33[?25l");
}

inline static void cursor_on( void )
{
    printf("\33[?25h");
}

static Termios setNonCanonicalOption( Termios *set_attr )
{
    Termios _save_attr = *set_attr;

    // Set new stdin options to accept input immediately
    set_attr->c_lflag       &= ~(ICANON | ECHO);
    set_attr->c_cc[VMIN]    = 2;
    set_attr->c_cc[VTIME]   = 1;

    // set
    tcsetattr( STDIN_FILENO, TCSANOW, set_attr );

    return _save_attr;
}

void Key::setOnDirectInput()
{
    bool* isOnDirect = Key::isOnDirectInput();
    if ( *isOnDirect )
        return;

    Termios stdin_attr;
    memset( &stdin_attr, 0x00, sizeof(Termios) );

    // Execute function to get existing stdin options and assign new options
    tcgetattr( STDIN_FILENO, &stdin_attr );

    Termios* save_attr = Key::getSaveAttr();

    memset( save_attr, 0x00, sizeof(Termios) );
    *save_attr = setNonCanonicalOption( &stdin_attr );

    *isOnDirect = true;
}

void Key::setOffDirectInput()
{
    bool* isOnDirect = Key::isOnDirectInput();
    if ( *isOnDirect == false )
        return;

    Termios* save_attr = Key::getSaveAttr();

    // rollback to existing stdin options
    tcsetattr( STDIN_FILENO, TCSANOW, save_attr );
    *isOnDirect = false;
}

void Key::init()
{
    cursor_off();
    Key::setOnDirectInput();
}

void Key::deinit()
{
    cursor_on();
    Key::setOffDirectInput();
}

/**
 * Unlike normal ASCII values, the ASCII values of the direction keys are
 * read as consecutive ASCII values, such as '27 91 65'. At this time,
 * the second key value of the direction keys is fixed at 91.
 */
constexpr int ARROW_CHECK_VALUE = 91;
constexpr int GENERAL_KEY_INDEX = 0;    // Index of general key values excluding 'arrow' keys or 'ESC' keys
constexpr int ARROW_CHECK_INDEX = 1;    // Index of the value to check whether it is an arrow key or an ESC key
constexpr int ARROW_VALUE_INDEX = 2;    // Index of the actual key value when the input value is a direction key

static int verifyKeyInput( char* read_data )
{
    // General Case
    if( read_data[GENERAL_KEY_INDEX] != KeyBoard::ESCAPE )
        return read_data[GENERAL_KEY_INDEX];

    // Arrow or ESC
    if( read_data[ARROW_CHECK_INDEX] == ARROW_CHECK_VALUE )
        return read_data[ARROW_VALUE_INDEX] + ARROW_ADD_VALUE;
    else
        return KeyBoard::ESCAPE;
}

constexpr int INPUT_MAX_LEN = ( 3 + 1 );

const KeyBoard Key::readKey()
{
    int  res                      = KeyBoard::NONE_INPUT;
    int  read_bytes               = 0;
    char read_data[INPUT_MAX_LEN] = { 0x00, };

    read_bytes = read( STDIN_FILENO, read_data, INPUT_MAX_LEN );
    if( read_bytes > 0 )
        res = verifyKeyInput( read_data );

    // capitalize
    if( res >= LowerKeyBoard::a && res <= LowerKeyBoard::z )
        res -= 32;

    return static_cast<KeyBoard>(res);
}

static int onEvent()
{
    struct timeval tv = { 0L, 0L };

    fd_set fds;
    FD_ZERO( &fds );
    FD_SET( 0, &fds );

    return select( 1, &fds, NULL, NULL, &tv );
}

const KeyBoard Key::readAsyncKey()
{
    int  res                        = KeyBoard::NONE_INPUT;
    int  read_bytes                 = 0;
    char read_data[INPUT_MAX_LEN]   = { 0x00, };

    if( onEvent() ) {
        read_bytes = read( STDIN_FILENO, read_data, INPUT_MAX_LEN );
        if( read_bytes > 0 )
            res = verifyKeyInput( read_data );
    }

    // capitalize
    if( res >= LowerKeyBoard::a && res <= LowerKeyBoard::z )
        res -= 32;

    return static_cast<KeyBoard>(res);
}
}; // nsp: AsyncKey
#endif