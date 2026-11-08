// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: Apache-2.0

/**
 * @file    KaitoTokyo/SimpleJsonReader/SimpleJsonReaderHelpers.hpp
 * @brief   Recursive Descent JSON parser for C++ with SAX-like interface.
 * @author  Kaito Udagawa <umireon@kaito.tokyo>
 * @version 1.1.0
 * @date    2026-11-08
 * @see     https://github.com/kaito-tokyo/KaitoTokyo-SimpleJsonReader/
 */

#pragma once

#include <initializer_list>
#include <stdexcept>
#include <string>

#include "SimpleJsonReader.hpp"

namespace KaitoTokyo::SimpleJsonReader
{

constexpr bool operator==(C8StringView lhs, C8StringView rhs) noexcept
{
    return lhs.size() == rhs.size() && std::char_traits<char>::compare(lhs.data(), rhs.data(), lhs.size()) == 0;
}

constexpr bool operator!=(C8StringView lhs, C8StringView rhs) noexcept
{
    return !(lhs == rhs);
}

template <typename StringView> using JsonPathComponent = std::variant<StringView, std::size_t>;

template <typename StringView> inline bool isStartObject(const Event<StringView> &event)
{
    return event.type == EventType::StartObject;
}

template <typename StringView> inline bool isEndObject(const Event<StringView> &event)
{
    return event.type == EventType::EndObject;
}

template <typename StringView> inline bool isStartArray(const Event<StringView> &event)
{
    return event.type == EventType::StartArray;
}

template <typename StringView> inline bool isEndArray(const Event<StringView> &event)
{
    return event.type == EventType::EndArray;
}

template <typename StringView> inline bool isKey(const Event<StringView> &event)
{
    return event.type == EventType::Key;
}

template <typename StringView> inline bool isString(const Event<StringView> &event)
{
    return event.type == EventType::String;
}

template <typename StringView> inline bool isNumber(const Event<StringView> &event)
{
    return event.type == EventType::Number;
}

template <typename StringView> inline bool isTrue(const Event<StringView> &event)
{
    return event.type == EventType::True;
}

template <typename StringView> inline bool isFalse(const Event<StringView> &event)
{
    return event.type == EventType::False;
}

template <typename StringView> inline bool isNull(const Event<StringView> &event)
{
    return event.type == EventType::Null;
}

template <typename StringView>
inline bool matchesPathPrefix(const Event<StringView> &event, const decltype(JsonPath<StringView>::component) *prefix,
                              std::int32_t prefixLength)
{
    if (event.jsonPath == nullptr)
    {
        throw std::invalid_argument("Event's jsonPath cannot be null");
    }
    else if (prefixLength < 0)
    {
        throw std::invalid_argument("Prefix length cannot be negative");
    }
    else if (prefixLength == 0)
    {
        return true;
    }
    else if (prefix == nullptr)
    {
        throw std::invalid_argument("Prefix cannot be null");
    }

    if (event.jsonPath == nullptr || event.jsonPath->depth < prefixLength)
    {
        return false;
    }

    JsonPath<StringView> *current = event.jsonPath;

    while (current != nullptr && current->depth > prefixLength)
    {
        current = current->parent;
    }

    while (current != nullptr && current->parent != nullptr)
    {
        if (current->component != prefix[current->depth - 1])
        {
            return false;
        }
        current = current->parent;
    }

    return current != nullptr && current->parent == nullptr;
}

template <typename StringView>
inline bool matchesPathPrefix(const Event<StringView> &event,
                              std::initializer_list<decltype(JsonPath<StringView>::component)> prefix)
{
    return matchesPathPrefix(event, prefix.begin(), static_cast<std::int32_t>(prefix.size()));
}

template <typename StringView>
inline bool matchesExactPath(const Event<StringView> &event, const decltype(JsonPath<StringView>::component) *path,
                             std::int32_t pathLength)
{
    if (event.jsonPath == nullptr)
    {
        throw std::invalid_argument("Event's jsonPath cannot be null");
    }
    else if (pathLength == 0)
    {
        return event.jsonPath->depth == 0;
    }
    else if (path == nullptr)
    {
        throw std::invalid_argument("Path cannot be null");
    }

    if (event.jsonPath->depth == pathLength)
    {
        return matchesPathPrefix(event, path, pathLength);
    }
    else
    {
        return false;
    }
}

template <typename StringView>
inline bool matchesExactPath(const Event<StringView> &event,
                             std::initializer_list<decltype(JsonPath<StringView>::component)> path)
{
    return matchesExactPath(event, path.begin(), static_cast<std::int32_t>(path.size()));
}

template <typename StringView, typename Callback>
inline void forEachPathComponent(const Event<StringView> &event, Callback callback)
{
    if (event.jsonPath == nullptr)
    {
        throw std::invalid_argument("Event's jsonPath cannot be null");
    }

    auto visit = [&](auto self, JsonPath<StringView> *jsonPath) -> void {
        if (jsonPath != nullptr && jsonPath->parent != nullptr)
        {
            self(self, jsonPath->parent);
            callback(jsonPath->component);
        }
    };
    visit(visit, event.jsonPath);
}

} // namespace KaitoTokyo::SimpleJsonReader
