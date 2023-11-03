# LinuxDirectKey

- ### Linux Direct keystroke function using C++
- ### Header-only file

<br>

## Environment
- It works ``Linux``
- ``C++``
- **No library dependencies**


<br>

## HOW TO USE

```cpp
#include "DirectKey.hpp"
```

<br>

## Example

```cpp
#include "DirectKey.hpp"

using namespace AsyncKey;

int main()
{
    // Reading key values direct.
    KeyBoard input = Key::GetKey();

    return 0;
}
```
- More example [**HERE (main.cpp)**](./main.cpp)
