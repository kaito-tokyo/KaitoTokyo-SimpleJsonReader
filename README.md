<!--
SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>

SPDX-License-Identifier: Apache-2.0
-->

# KaitoTokyo-SimpleJsonReader

Recursive Descent JSON parser for C++ with SAX-like interface. Written by Kaito Udagawa a.k.a. [umireon](https://github.com/umireon).

## Dependencies

- C++17

## How to use?

1. Include `SimpleJsonReader.hpp` in a way you like. You can commit my works on your tree publicly.

## License

- **Apache-2.0**: A permissive license whose main conditions require preservation of copyright and license notices. Contributors provide an express grant of patent rights. Licensed works, modifications, and larger works may be distributed under different terms and without source code.

## Example of code

```cpp
#include <KaitoTokyo/SimpleJsonReader.hpp>

using namespace KaitoTokyo::SimpleJsonReader;

std::string_view rawName;
parseJson(std::move(buf), [](auto event) {  
    if (isString(event) && matchPathPrefix(event, {"records", 3, "name"})) {
        rawName = event.fragment;
    }
});

std::cout << "Name:" << my_unescape(rawName) << std::endl;
```
