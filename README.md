# AsyncKey

- ### Linux Asynchronous keystroke function using C++
- ### Header-only file

<br>

## Environment
- It works ``Linux``
- ``C++``
- <u>No library dependencies</u>


<br>

## HOW TO USE

```cpp
#include "AsyncKey.hpp
```

<br>

## Example

```cpp
#include "AsyncKey.hpp"

using namespace AsyncKey;

int main()
{
    // Key class initialize.
    Key::init();

    // Reading key values asynchronously.
    KeyBoard input = Key::readAsyncKey();

    // Key class deinitialize.
    Key::deinit();

    return 0;
}
```
- More example [**HERE (main.cpp)**](./main.cpp)