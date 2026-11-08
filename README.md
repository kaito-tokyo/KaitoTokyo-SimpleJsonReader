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

Pass a view such as `std::string_view` when you keep fragments after parsing. `parseJsonUtf8` takes its input by value, so passing a `std::string` parses a temporary copy and leaves the fragments dangling.

```cpp
#include <KaitoTokyo/SimpleJsonReader/SimpleJsonReader.hpp>
#include <KaitoTokyo/SimpleJsonReader/SimpleJsonReaderHelpers.hpp>

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

using namespace KaitoTokyo::SimpleJsonReader;

int main()
{
    std::string json = R"({"records": [{}, {}, {}, {"name": "Alice"}]})";

    SimpleJsonReader<std::string_view> reader;
    std::string_view rawName;
    auto [rest, error] = reader.parseJsonUtf8(std::string_view{json}, [&rawName](Event<std::string_view> event) {
        if (isString(event) && matchesPathPrefix(event, {"records", std::size_t{3}, "name"}))
        {
            rawName = event.fragment;
        }
    });

    if (error != ErrorType::OK)
    {
        std::cerr << "Failed to parse JSON" << std::endl;
        return 1;
    }

    // rawName points into json and still contains any escape sequences such as \n.
    std::cout << "Name: " << rawName << std::endl;
}
```
