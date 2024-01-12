# LinuxDirectKey

- ### Linux Direct keystroke function using C++

<br>

## Environment
- It works ``Linux``
- ``C++``
- **No library dependencies**


<br>

## Example

```cpp
/* =========================================================== *
 * COMPILE : g++ -o runFile main.cpp DirectKey.h DirectKey.cpp *
 * =========================================================== */

#include "DirectKey.h"

using namespace DirectKey;

int main()
{
    // Reading key values direct.
    KeyBoard input = Key::GetKey();

    return 0;
}
```
- More example [**HERE (main.cpp)**](./main.cpp)
