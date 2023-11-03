#ifndef _DIRECT_KEY_LINUX_HPP_
#define _DIRECT_KEY_LINUX_HPP_

// STL :: C
#include <stdio.h>      //
#include <unistd.h>     // read
#include <termios.h>    // struct termios
#include <memory.h>     // memset
#include <sys/select.h> // select

// STL :: CPP
#include <memory>       // std::shared_ptr
#include <mutex>        // mutex, unique_lock

namespace DirectKey 
{
// Enum for key identifier
constexpr int ARROW_ADD_VALUE = 1000;
constexpr int FUNCIONAL_VALUE = 2000;
enum KeyBoard {
    // Alphabet : Only use uppercase
    A = 65, B = 66, C = 67, D = 68, E = 69, F = 70, G = 71, H = 72, I = 73,
    J = 74, K = 75, L = 76, M = 77, N = 78, O = 79, P = 80, Q = 81, R = 82,
    S = 83, T = 84, U = 85, V = 86, W = 87, X = 88, Y = 89, Z = 90,

    // Numbers
    NUM_0 = 48, NUM_1 = 49, NUM_2 = 50, NUM_3 = 51, NUM_4 = 52,
    NUM_5 = 53, NUM_6 = 54, NUM_7 = 55, NUM_8 = 56, NUM_9 = 57,

    // Special keys
    TAB = 9, ENTER = 10, ESCAPE = 27, SPACEBAR = 32, BACKSPACE = 127,

    // Funcional keys
    F1  = 11 + FUNCIONAL_VALUE,
    F2  = 12 + FUNCIONAL_VALUE,
    F3  = 13 + FUNCIONAL_VALUE,
    F4  = 14 + FUNCIONAL_VALUE,
    F5  = 15 + FUNCIONAL_VALUE,
    F6  = 16 + FUNCIONAL_VALUE,
    F7  = 17 + FUNCIONAL_VALUE,
    F8  = 18 + FUNCIONAL_VALUE,
    F9  = 19 + FUNCIONAL_VALUE,
    F10 = 20 + FUNCIONAL_VALUE,
    F11 = 21 + FUNCIONAL_VALUE,
    F12 = 22 + FUNCIONAL_VALUE,

    // Arrows
    UP_ARROW    = 65 + ARROW_ADD_VALUE,
    DOWN_ARROW  = 66 + ARROW_ADD_VALUE,
    RIGHT_ARROW = 67 + ARROW_ADD_VALUE,
    LEFT_ARROW  = 68 + ARROW_ADD_VALUE,

    // Default : None defined
    NONE_INPUT       = -2,
    ALREADY_OCCUPIED = -3,
};
// For check & capitalize
constexpr int CAPITALIZE_OFFSET = 32;
enum LowerKeyBoard {
    a = A + CAPITALIZE_OFFSET,
    z = Z + CAPITALIZE_OFFSET
};

// For convinient timeout set
enum KeyDelay {
    FPS_01 = 1000 / 1,
    FPS_10 = 1000 / 10,
    FPS_15 = 1000 / 15,
    FPS_20 = 1000 / 20,
    FPS_25 = 1000 / 25,
    FPS_30 = 1000 / 30,
    FPS_60 = 1000 / 60,
};

// Typename contraction
using Termios = struct termios;
using Timeval = struct timeval;

class Key
{
// Main key stroke read functions ----------------------------------------------------------------------------------------
public:
    /**
     * @brief  Reading key values synchronously.
     * @note   This function waits until you stroke a key
     * @return Pressed key value converted to KeyBoard enum value
     */
    static const KeyBoard GetKey( void ) noexcept;

    /**
     * @brief  Reading key values synchronously until timeout (millisec)
     * @note   This function waits for a timeout or until you stroke a key
     * @param  timeout_milsec If value >= 0, a one-time timeout waits for the value in millisec when the function operates.
     * @return Pressed key value converted to KeyBoard enum value, or 'KeyBoard::NONE_INPUT' if not pressed until timeout.
     *         If another keystroke waiting function is already in progress, it returns 'KeyBoard::ALREADY_OCCUPIED'.
     */
    static const KeyBoard GetKeyTimeout( const int timeout_milsec = -1 ) noexcept;

// Set flag or config functions ------------------------------------------------------------------------------------------
public:
    /**
     * @brief  Function to set timeout milliseconds to wait for key input synchronously
     * @param  timeout_milsec If value >= 0, set the value as the default synchronous key input waiting timeout value.
     * @return True if the change was applied, false if the change failed cuz other code was already waiting for key input.
     */
    static const bool SetReadKeyAwaitTimeout( const int timeout_milsec ) noexcept;

// Utilities -------------------------------------------------------------------------------------------------------------
public:
    /**
     * @brief  A function that attempts to temporarily disable 'immediate key input' and 'cursor hiding'
     * @return true if the attempt was successful, false if the attempt failed because key input was waiting elsewhere
     */
    static const bool TryPause( void ) noexcept;

    /**
     * @brief A function that temporarily force disable 'immediate key input' and 'cursor hiding'
     * @note  If you are waiting for a key input somewhere else, wait until the task is completed and then force a pause.
     */
    static void ForcePause( void ) noexcept;

    /**
     * @brief Resume 'immediate key input' and 'cursor hiding' functions stopped with TryPause() or ForcePause()
     */
    static void Resume( void ) noexcept;

// Others ----------------------------------------------------------------------------------------------------------------
public:
    /**
     * @brief public destructor for RAII of shared_ptr
     */
    ~Key() { Key::Deinit(); }

// Private methodes ------------------------------------------------------------------------------------------------------
private:
    /**
     * @brief private constructor for RAII and singleton
     */
    explicit Key() 
        : is_key_stroke_direct( false )
        , await_timeout_milsec( KeyDelay::FPS_30 )
    { Key::Init(); }

    /**
     * @brief Key class initialize
     */
    void Init( void ) noexcept;

    /**
     * @brief Key class deinitialize
     */
    void Deinit( void ) noexcept;

    /**
     * @brief  Get singleton Key shared_ptr
     * @return Singleton pattern DirectKey::Key pointer
     */
    static std::shared_ptr<DirectKey::Key> GetKeyPtr( void ) noexcept;

    /**
     * @brief  Function to change the key input time to when the key is pressed
     * @return true if the attempt was successful, false if the attempt failed because key input was waiting elsewhere
     */
    const bool SetKeyStrokeDirect( void ) noexcept;

    /**
     * @brief  Function to change the key input time to when 'Enter' key pressed
     * @return true if the attempt was successful, false if the attempt failed because key input was waiting elsewhere
     */
    const bool SetKeyStrokeWhenPressEnter( void ) noexcept;

    const bool IsKeyStrokeDirect( void ) noexcept 
    { return this->is_key_stroke_direct; }

// Private members -------------------------------------------------------------------------------------------------------
private:
    Termios save_attr;
    bool    is_key_stroke_direct;
    int     await_timeout_milsec;
    mutable std::mutex mtx;
}; // cls: Key


// Hide cursor on console
inline static void _CursorOff( void ) noexcept { printf("\33[?25l"); }

// Reveal cursor on console
inline static void _CursorOn ( void ) noexcept { printf("\33[?25h"); }

// Set terminel non-canonical mode
static Termios _SetNonCanonicalMode( Termios* origin_attr ) noexcept
{
    Termios save_attr = *origin_attr; // save attribute origin

    // Set new STDIN attribute to accept input immediately
    origin_attr->c_lflag     &= ~(ICANON | ECHO); // Non-canonical & hide echo
    origin_attr->c_cc[VMIN]  = 2;                 // Input buffer minimum number of characters returned
    origin_attr->c_cc[VTIME] = 1;                 // Wait maximum time ( 0.01 sec )

    // Apply
    tcsetattr( STDIN_FILENO, TCSANOW, origin_attr );

    // Return saved origin attribute
    return save_attr;
}

/**
 * Unlike normal ASCII values, the ASCII values of the direction keys are
 * read as consecutive ASCII values, such as '27 91 65'. At this time,
 * the second key value of the direction keys is fixed at 91.
 */
constexpr int ARROW_CHECK_VALUE = 91;
constexpr int GENERAL_KEY_INDEX = 0;  // Index of general key values excluding 'arrow' keys or 'ESC' keys
constexpr int ARROW_CHECK_INDEX = 1;  // Index of the value to check whether it is an arrow key or an ESC key
constexpr int ARROW_VALUE_INDEX = 2;  // Index of the actual key value when the input value is a direction key
constexpr int FUNC_KEY_INDEX_01 = 2;
constexpr int FUNC_KEY_INDEX_02 = 3;

inline static const KeyBoard Fnckey_cast_keyboards( const char v1, const char v2 )
{
    if      ( v1 == 49 && v2 == 49 ) return KeyBoard::F1;
    else if ( v1 == 49 && v2 == 50 ) return KeyBoard::F2;
    else if ( v1 == 49 && v2 == 51 ) return KeyBoard::F3;
    else if ( v1 == 49 && v2 == 52 ) return KeyBoard::F4;
    else if ( v1 == 49 && v2 == 53 ) return KeyBoard::F5;
    else if ( v1 == 49 && v2 == 55 ) return KeyBoard::F6;
    else if ( v1 == 49 && v2 == 56 ) return KeyBoard::F7;
    else if ( v1 == 49 && v2 == 57 ) return KeyBoard::F8;
    else if ( v1 == 50 && v2 == 48 ) return KeyBoard::F9;
    else if ( v1 == 50 && v2 == 49 ) return KeyBoard::F10;
    else if ( v1 == 50 && v2 == 51 ) return KeyBoard::F11;
    else if ( v1 == 50 && v2 == 52 ) return KeyBoard::F12;
    else                             return KeyBoard::NONE_INPUT;
}

static const KeyBoard _VerifyKeyInput( const char* const read_data, const ssize_t byte_len )
{
    switch( byte_len )
    {
    // General Word keys
    case 1:  return static_cast<KeyBoard>( read_data[GENERAL_KEY_INDEX] );
    case 2:  return static_cast<KeyBoard>( read_data[GENERAL_KEY_INDEX] );
    // Arrow keys
    case 3:  return static_cast<KeyBoard>( read_data[ARROW_VALUE_INDEX] + ARROW_ADD_VALUE );
    // Functional keys
    case 4:  return static_cast<KeyBoard>( read_data[FUNC_KEY_INDEX_01] + FUNCIONAL_VALUE );
    case 5:  return Fnckey_cast_keyboards( read_data[FUNC_KEY_INDEX_01] , read_data[FUNC_KEY_INDEX_02] );
    default: return KeyBoard::NONE_INPUT;
    }
}

static const KeyBoard _CapitalizeIfAlphabet( const KeyBoard target )
{
    int n_target = static_cast<int>( target );
    if( n_target >= LowerKeyBoard::a && n_target <= LowerKeyBoard::z )
        return static_cast<KeyBoard>( n_target - CAPITALIZE_OFFSET );
    return target;
}

static const int _OnEvent( const int timeout_milsec ) noexcept
{
    const int usec = ( timeout_milsec > 0 ) ? timeout_milsec * 1000 : 0;
    Timeval   tv   = { 0L, static_cast<long>( usec ) };

    fd_set fds;
    FD_ZERO( &fds );
    FD_SET ( 0, &fds );

    return select( 1, &fds, NULL, NULL, &tv );
}

constexpr int INPUT_MAX_LEN = ( 5 + 1 );
const KeyBoard Key::GetKey( void ) noexcept
{
    auto sptr_key = Key::GetKeyPtr();

    std::unique_lock<std::mutex> lck( sptr_key->mtx, std::defer_lock );
    if( lck.try_lock() == false )
        return KeyBoard::ALREADY_OCCUPIED;

    if( sptr_key->IsKeyStrokeDirect() == false )
        sptr_key->Resume();

    KeyBoard input_key                = KeyBoard::NONE_INPUT;
    ssize_t  read_bytes               = 0;
    char     read_data[INPUT_MAX_LEN] = { 0x00, };

    read_bytes = read( STDIN_FILENO, read_data, INPUT_MAX_LEN );
    if( read_bytes > 0 )
        input_key = _CapitalizeIfAlphabet( _VerifyKeyInput( read_data, read_bytes ) );
    return input_key;
}

const KeyBoard Key::GetKeyTimeout( const int timeout_milsec ) noexcept
{
    auto sptr_key = Key::GetKeyPtr();

    std::unique_lock<std::mutex> lck( sptr_key->mtx, std::defer_lock );
    if( lck.try_lock() == false )
        return KeyBoard::ALREADY_OCCUPIED;

    if( sptr_key->IsKeyStrokeDirect() == false )
        sptr_key->Resume();

    KeyBoard  input_key                = KeyBoard::NONE_INPUT;
    ssize_t   read_bytes               = 0;
    char      read_data[INPUT_MAX_LEN] = { 0x00, };
    const int wait_timeout_on_event    = ( timeout_milsec < 0 ) ? sptr_key->await_timeout_milsec : timeout_milsec;

    if( _OnEvent( wait_timeout_on_event ) > 0 ) {
        read_bytes = read( STDIN_FILENO, read_data, INPUT_MAX_LEN );
        if( read_bytes > 0 ) {
            // printf("<%d>", read_bytes);
            input_key = _CapitalizeIfAlphabet( _VerifyKeyInput( read_data, read_bytes ) );
        }
    }
    return input_key;
}

const bool Key::SetReadKeyAwaitTimeout( const int timeout_milsec ) noexcept
{
    auto sptr_key = Key::GetKeyPtr();

    std::unique_lock<std::mutex> lck( sptr_key->mtx, std::defer_lock );
    if( lck.try_lock() == false )
        return false;

    if( timeout_milsec > 0 ) {
        sptr_key->await_timeout_milsec = timeout_milsec;
        return true;
    }
    return false;
}

const bool Key::TryPause( void ) noexcept
{
    auto sptr_key = Key::GetKeyPtr();
    
    std::unique_lock<std::mutex> lck( sptr_key->mtx, std::defer_lock );
    if( lck.try_lock() == false )
        return false;

    _CursorOn();
    sptr_key->SetKeyStrokeWhenPressEnter();

    return true;
}

void Key::ForcePause( void ) noexcept
{
    auto sptr_key = Key::GetKeyPtr();
    std::unique_lock<std::mutex> lck( sptr_key->mtx );

    _CursorOn();
    sptr_key->SetKeyStrokeWhenPressEnter(); 
}

void Key::Resume( void ) noexcept
{
    _CursorOff();
    Key::GetKeyPtr()->SetKeyStrokeDirect();
}

void Key::Init( void ) noexcept
{
    _CursorOff();
    this->SetKeyStrokeDirect();
}

void Key::Deinit( void ) noexcept
{
    _CursorOn();
    this->SetKeyStrokeWhenPressEnter();
}

const bool Key::SetKeyStrokeDirect() noexcept
{
    if( this->is_key_stroke_direct == true )
        return true;

    Termios stdin_attr;
    memset( &stdin_attr, 0x00, sizeof(Termios) );

    // Execute function to get existing stdin options and assign new options
    tcgetattr( STDIN_FILENO, &stdin_attr );

    memset( &( this->save_attr ), 0x00, sizeof(Termios) );
    this->save_attr = _SetNonCanonicalMode( &stdin_attr );
    this->is_key_stroke_direct = true;

    return true;
}

const bool Key::SetKeyStrokeWhenPressEnter() noexcept
{
    if( this->is_key_stroke_direct == false )
        return true;

    // rollback to existing stdin options
    tcsetattr( STDIN_FILENO, TCSANOW, &( this->save_attr ) );
    this->is_key_stroke_direct = false;

    return true;
}

std::shared_ptr<DirectKey::Key> Key::GetKeyPtr( void ) noexcept
{
    static std::shared_ptr<DirectKey::Key> singleton( new Key() );
    return singleton;
}

}; // nsp: DirectKey
#endif