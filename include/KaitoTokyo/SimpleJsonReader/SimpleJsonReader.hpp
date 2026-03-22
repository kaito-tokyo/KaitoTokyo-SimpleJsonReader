// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: Apache-2.0

/**
 * @file    KaitoTokyo/SimpleJsonReader/SimpleJsonReader.hpp
 * @brief   Recursive Descent JSON parser for C++ with SAX-like interface.
 * @version 2026.03.22
 * @author  Kaito Udagawa <umireon@kaito.tokyo>
 * @see     https://github.com/kaito-tokyo/KaitoTokyo-SimpleJsonReader/
 */

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <utility>
#include <variant>

#if __cplusplus >= 202002L && defined(__has_include) && \
    __has_include(<concepts>) && defined(__cpp_concepts)
#include <concepts>
#endif

namespace KaitoTokyo {
namespace SimpleJsonReader {

enum class EventType : std::uint8_t {
  Undefined = 0b00000000,
  StartObject = 0b00000001,
  EndObject = 0b10000001,
  StartArray = 0b00000010,
  EndArray = 0b10000010,
  Key = 0b00000100,
  String = 0b00001000,
  Number = 0b00010000,
  True = 0b00100000,
  False = 0b10100000,
  Null = 0b01000000,
  XPrefix = 0b01100000,
};

enum class ErrorType {
  OK,
  EmptyJSONError,
  UnexpectedEndWhileParsingObjectError,
  UnexpectedEndWhileParsingArrayError,
  UnexpectedEndWhileParsingStringError,
  FieldDelimiterMissingError,
  InvalidTokenWhileParsingValueError,
  InvalidTokenWhileParsingObjectError,
  InvalidTokenWhileParsingArrayError,
  InvalidTokenLikelyTrueError,
  InvalidTokenLikelyFalseError,
  InvalidTokenLikelyNullError,
  InvalidTokenLikelyLiteralError,
  DepthLimitExceededInArrayError,
  DepthLimitExceededInObjectError,
};

#ifdef __cpp_lib_concepts
template <typename T>
concept IStringView = requires(const T a) {
  { a.data() } -> std::convertible_to<const char*>;
  { a.size() } -> std::convertible_to<std::size_t>;
  requires std::is_trivially_copyable_v<T>;
  requires std::is_constructible_v<T, const char*, std::size_t>;
  requires std::destructible<T>;
};

template <typename T>
concept IJsonString = requires(const T a) {
  { a.data() } -> std::convertible_to<const void*>;
  { a.size() } -> std::convertible_to<std::size_t>;
};
#endif

#ifdef __cpp_lib_concepts
template <IStringView StringView>
#else
template <typename StringView>
#endif
struct JsonPath {
  JsonPath<StringView>* parent{nullptr};
  std::variant<StringView, std::size_t> component{};
  std::int32_t depth = 0;
};

#ifdef __cpp_lib_concepts
template <IStringView StringView>
#else
template <typename StringView>
#endif
struct Event {
  StringView fragment;
  JsonPath<StringView>* jsonPath;
  EventType type;
};

#ifdef __cpp_lib_concepts
template <IStringView StringView>
#else
template <typename StringView>
#endif
using ParseResult = std::pair<StringView, ErrorType>;

#ifdef __cpp_lib_concepts
template <typename T, typename StringView>
concept IEventHandler = requires(T handler, Event<StringView> event) {
  requires IStringView<StringView>;
  { handler(event) } -> std::same_as<void>;
};
#endif

struct C8StringView {
  const char* str{nullptr};
  std::size_t len{0};

  const char* data() const noexcept { return str; }
  std::size_t size() const noexcept { return len; }
};

template <typename string_view = C8StringView, int SWAR = 64>
class SimpleJsonReader {
  static_assert(SWAR == 64 || SWAR == 32 || SWAR == 0,
                "SWAR must be either 64 or 32, or 0 to disable");

 public:
  using JsonPath_t = ::KaitoTokyo::SimpleJsonReader::JsonPath<string_view>;
  using Event_t = ::KaitoTokyo::SimpleJsonReader::Event<string_view>;
  using ParseResult_t =
      ::KaitoTokyo::SimpleJsonReader::ParseResult<string_view>;

  SimpleJsonReader(std::int32_t maxDepth = 1000) : maxDepth_(maxDepth) {}

  inline string_view skipWhitespaces(string_view sv) noexcept {
    auto* p = sv.data();
    auto const* end = sv.data() + sv.size();

    if constexpr (SWAR == 64 || SWAR == 32) {
      using uint_t =
          std::conditional_t<SWAR == 64, std::uint64_t, std::uint32_t>;
      constexpr uint_t add =
          (SWAR == 64) ? 0x5F5F5F5F5F5F5F5FULL : 0x5F5F5F5FUL;
      constexpr uint_t mask =
          (SWAR == 64) ? 0x8080808080808080ULL : 0x80808080UL;

      while (p + sizeof(uint_t) <= end) {
        uint_t chunk;
        std::memcpy(&chunk, p, sizeof(uint_t));
        if ((chunk + add) & mask)
#if __cplusplus >= 202002L && defined(__has_cpp_attribute) && \
    __has_cpp_attribute(unlikely)
            [[unlikely]]
#endif
        {
          break;
        }
        p += sizeof(uint_t);
      }
    }

    while (p < end && static_cast<std::uint8_t>(*p) <= 0x20) p++;
    const std::size_t offset = static_cast<std::size_t>(p - sv.data());
    return {p, sv.size() - offset};
  }

  /**
   * @pre First char of sv.str MUST be `"`.
   * @post Handler called with String fragment.
   * @pre If IsKey is true, Head of jsonPath MUST be empty.
   * @post When IsKey is true, Head of jsonPath is updated with the found key.
   * @retval Pair of Tail and ErrorType.
   */
#ifdef __cpp_lib_concepts
  template <bool IsKey = false, IEventHandler<string_view> EventHandler>
#else
  template <bool IsKey = false, typename EventHandler>
#endif
  inline ParseResult_t parseString(string_view sv, EventHandler handler,
                                   JsonPath_t* jsonPath) noexcept {
    const auto* start = sv.data();
    const auto* end = sv.data() + sv.size();

    if (start == nullptr || sv.size() < 2)
#if __cplusplus >= 202002L && defined(__has_cpp_attribute) && \
    __has_cpp_attribute(unlikely)
        [[unlikely]]
#endif
    {
      return {sv, ErrorType::UnexpectedEndWhileParsingStringError};
    }

    /// @post Set p to be Position of Ending structural double quote.
    auto* p = start;

    while (p < end) {
      p = std::find(p + 1, end, '"');
      if (p == end)
#if __cplusplus >= 202002L && defined(__has_cpp_attribute) && \
    __has_cpp_attribute(unlikely)
          [[unlikely]]
#endif
      {
        return {sv, ErrorType::UnexpectedEndWhileParsingStringError};
      }

      if (p[-1] != '\\')
#if __cplusplus >= 202002L && defined(__has_cpp_attribute) && \
    __has_cpp_attribute(unlikely)
          [[likely]]
#endif
      {
        break;  // Structural
      }

      auto* rpos = p - 2;
      while (rpos > start && *rpos == '\\') rpos -= 1;
      if ((p - rpos) % 2 == 1) break;  // Structural
    }

    /// @pre p points to Ending structural double quote.
    /// @post Fragment represents Raw JSON string without Structural double
    /// quotes.
    const auto* fragmentStart = start + 1;
    const auto* fragmentEnd = p;
    const std::size_t fragmentLength =
        static_cast<std::size_t>(fragmentEnd - fragmentStart);
    const string_view fragment{fragmentStart, fragmentLength};

    if constexpr (IsKey) {
      jsonPath->component = fragment;
      handler(Event_t{fragment, jsonPath, EventType::Key});
    } else {
      handler(Event_t{fragment, jsonPath, EventType::String});
    }

    /// @post Tail represents the remaining buffer.
    const auto* tailStart = p + 1;
    const auto* tailEnd = end;
    const std::size_t tailLength =
        static_cast<std::size_t>(tailEnd - tailStart);
    return {{tailStart, tailLength}, ErrorType::OK};
  }

#ifdef __cpp_lib_concepts
  template <IEventHandler<string_view> EventHandler>
#else
  template <typename EventHandler>
#endif
  inline ParseResult_t parseNumber(string_view sv, EventHandler handler,
                                   JsonPath_t* jsonPath) noexcept {
    const auto* end = sv.data() + sv.size();

    auto* p = sv.data();
    while (p < end) {
      if (static_cast<std::uint8_t>(*p) <= 0x20)
#if __cplusplus >= 202002L && defined(__has_cpp_attribute) && \
    __has_cpp_attribute(unlikely)
          [[unlikely]]
#endif
      {
        break;
      }
      switch (*p) {
        default:
          p++;
          continue;
        case ',':
        case ']':
        case '}':
#if __cplusplus >= 202002L && defined(__has_cpp_attribute) && \
    __has_cpp_attribute(unlikely)
          [[unlikely]]
#endif
          break;
      }
      break;
    }

    /// @pre p points to End of number. It MUST NOT be a numeric char.

    const std::size_t fragmentLength = static_cast<std::size_t>(p - sv.data());
    handler(Event_t{{sv.data(), fragmentLength}, jsonPath, EventType::Number});

    const std::size_t tailLength = static_cast<std::size_t>(end - p);
    return {{p, tailLength}, ErrorType::OK};
  }

#ifdef __cpp_lib_concepts
  template <IEventHandler<string_view> EventHandler>
#else
  template <typename EventHandler>
#endif
  inline ParseResult_t parseLiteral(string_view sv, EventHandler handler,
                                    JsonPath_t* jsonPath) noexcept {
    const auto svSize = sv.size();
    const auto svData = sv.data();
    switch (svData[0]) {
      case 't':
        if (svSize >= 4 && svData[1] == 'r' && svData[2] == 'u' &&
            svData[3] == 'e') {
          handler(Event_t{{"true", 4}, jsonPath, EventType::True});
          return {{svData + 4, svSize - 4}, ErrorType::OK};
        } else {
          return {sv, ErrorType::InvalidTokenLikelyTrueError};
        }
      case 'f':
        if (svSize >= 5 && svData[1] == 'a' && svData[2] == 'l' &&
            svData[3] == 's' && svData[4] == 'e') {
          handler(Event_t{{"false", 5}, jsonPath, EventType::False});
          return {{svData + 5, svSize - 5}, ErrorType::OK};
        } else {
          return {sv, ErrorType::InvalidTokenLikelyFalseError};
        }
      case 'n':
        if (svSize >= 4 && svData[1] == 'u' && svData[2] == 'l' &&
            svData[3] == 'l') {
          handler(Event_t{{"null", 4}, jsonPath, EventType::Null});
          return {{svData + 4, svSize - 4}, ErrorType::OK};
        } else {
          return {sv, ErrorType::InvalidTokenLikelyNullError};
        }
      default:
        return {sv, ErrorType::InvalidTokenLikelyLiteralError};
    }
  }

#ifdef __cpp_lib_concepts
  template <IEventHandler<string_view> EventHandler>
#else
  template <typename EventHandler>
#endif
  inline ParseResult_t parseArray(string_view sv, EventHandler handler,
                                  JsonPath_t* parentPath,
                                  std::uint32_t depthLimit) noexcept {
    if (depthLimit <= 0) return {sv, ErrorType::DepthLimitExceededInArrayError};

    handler(Event_t{{"[", 1}, parentPath, EventType::StartArray});

    sv = {sv.data() + 1, sv.size() - 1};

    std::size_t index = 0;
    JsonPath_t elemPath{parentPath, {}, parentPath->depth + 1};
    while (true) {
      sv = skipWhitespaces(sv);

      if (sv.size() < 1)
#if __cplusplus >= 202002L && defined(__has_cpp_attribute) && \
    __has_cpp_attribute(unlikely)
          [[unlikely]]
#endif
      {
        return {sv, ErrorType::UnexpectedEndWhileParsingArrayError};
      }

      if (sv.data()[0] == ']') {
        handler(Event_t{{"]", 1}, parentPath, EventType::EndArray});
        return {{sv.data() + 1, sv.size() - 1}, ErrorType::OK};
      }

      elemPath.component = index;

      sv = skipWhitespaces(sv);

      auto [valueSv, valueErr] =
          parseValue(sv, handler, &elemPath, depthLimit - 1);
      if (valueErr != ErrorType::OK) {
        return {valueSv, valueErr};
      }

      sv = skipWhitespaces(valueSv);

      if (sv.size() < 1)
#if __cplusplus >= 202002L && defined(__has_cpp_attribute) && \
    __has_cpp_attribute(unlikely)
          [[unlikely]]
#endif
      {
        return {sv, ErrorType::UnexpectedEndWhileParsingArrayError};
      }

      if (sv.data()[0] == ',') {
        index += 1;
        sv = {sv.data() + 1, sv.size() - 1};
        continue;
      }

      if (sv.data()[0] != ']')
#if __cplusplus >= 202002L && defined(__has_cpp_attribute) && \
    __has_cpp_attribute(unlikely)
          [[unlikely]]
#endif
      {
        return {sv, ErrorType::InvalidTokenWhileParsingArrayError};
      }
    }
  }

#ifdef __cpp_lib_concepts
  template <IEventHandler<string_view> EventHandler>
#else
  template <typename EventHandler>
#endif
  inline ParseResult_t parseObject(string_view sv, EventHandler handler,
                                   JsonPath_t* parentPath,
                                   std::int32_t depthLimit) noexcept {
    if (depthLimit <= 0)
      return {sv, ErrorType::DepthLimitExceededInObjectError};

    handler(Event_t{{"{", 1}, parentPath, EventType::StartObject});

    JsonPath_t fieldPath{parentPath, {}, parentPath->depth + 1};
    sv = {sv.data() + 1, sv.size() - 1};
    while (true) {
      sv = skipWhitespaces(sv);

      if (sv.size() < 1)
#if __cplusplus >= 202002L && defined(__has_cpp_attribute) && \
    __has_cpp_attribute(unlikely)
          [[unlikely]]
#endif
        return {sv, ErrorType::UnexpectedEndWhileParsingObjectError};

      if (sv.data()[0] == '}') {
        handler(Event_t{{"}", 1}, parentPath, EventType::EndObject});
        return {{sv.data() + 1, sv.size() - 1}, ErrorType::OK};
      }

      auto valueResult = parseString<true>(sv, handler, &fieldPath);
      auto [valueSv, valueErr] = valueResult;
      if (valueErr != ErrorType::OK) {
        return valueResult;
      }

      sv = skipWhitespaces(valueSv);

      if (sv.size() < 1)
#if __cplusplus >= 202002L && defined(__has_cpp_attribute) && \
    __has_cpp_attribute(unlikely)
          [[unlikely]]
#endif
      {
        return {sv, ErrorType::UnexpectedEndWhileParsingObjectError};
      }

      if (sv.data()[0] != ':')
#if __cplusplus >= 202002L && defined(__has_cpp_attribute) && \
    __has_cpp_attribute(unlikely)
          [[unlikely]]
#endif
      {
        return {sv, ErrorType::FieldDelimiterMissingError};
      }
      sv = {sv.data() + 1, sv.size() - 1};

      if (sv.size() < 1)
#if __cplusplus >= 202002L && defined(__has_cpp_attribute) && \
    __has_cpp_attribute(unlikely)
          [[unlikely]]
#endif
      {
        return {sv, ErrorType::UnexpectedEndWhileParsingObjectError};
      }

      sv = skipWhitespaces(sv);

      {
        auto [newSv, err] = parseValue(sv, handler, &fieldPath, depthLimit - 1);
        if (err != ErrorType::OK) {
          return {newSv, err};
        }
        sv = newSv;
      }

      sv = skipWhitespaces(sv);
      if (sv.size() < 1)
#if __cplusplus >= 202002L && defined(__has_cpp_attribute) && \
    __has_cpp_attribute(unlikely)
          [[unlikely]]
#endif
      {
        return {sv, ErrorType::UnexpectedEndWhileParsingObjectError};
      }

      if (sv.data()[0] == ',') {
        sv = {sv.data() + 1, sv.size() - 1};
        continue;
      }

      if (sv.data()[0] != '}')
#if __cplusplus >= 202002L && defined(__has_cpp_attribute) && \
    __has_cpp_attribute(unlikely)
          [[unlikely]]
#endif
      {
        return {sv, ErrorType::InvalidTokenWhileParsingObjectError};
      }
    }
  }

#ifdef __cpp_lib_concepts
  template <IEventHandler<string_view> EventHandler>
#else
  template <typename EventHandler>
#endif
  inline ParseResult_t parseValue(string_view sv, EventHandler handler,
                                  JsonPath_t* jsonPath,
                                  std::int32_t depthLimit) noexcept {
    if (sv.size() < 1) return {sv, ErrorType::EmptyJSONError};

    switch (sv.data()[0]) {
      case '{':
        return parseObject(sv, handler, jsonPath, depthLimit);
      case '[':
        return parseArray(sv, handler, jsonPath, depthLimit);
      case '"':
        return parseString(sv, handler, jsonPath);
      case 't':
      case 'f':
      case 'n':
        return parseLiteral(sv, handler, jsonPath);
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case '-':
        return parseNumber(sv, handler, jsonPath);
      default:
        return {sv, ErrorType::InvalidTokenWhileParsingValueError};
    }
  }

  /**
   * @brief Parses a JSON string and emits events to the provided handler.
   * @param jsonSv The JSON string to parse. Move semantics are supported for
   * efficiency.
   * @param handler A synchronous callback function to receive parsing events.
   * @return OK if parsing succeeded, otherwise an error code.
   * @code
   * parseJson(std::move(view), [](auto event) {
   *   // Write your code to handle JSON content here.
   *   // event          - The context of the parsing event. Only valid during
   * this callback execution.
   *   // event.type     - The type of parsing event like StartArrayo or String.
   *   // event.frament  - The raw JSON fragment corresponding to the event.
   *   // envet.jsonPath - The JSON path of the current event. Matcher helpers
   * are available in the other header file.
   * });
   * @endcode
   */
#ifdef __cpp_lib_concepts
  template <IEventHandler<string_view> EventHandler, IJsonString String>
#else
  template <typename EventHandler, typename String>
#endif
  inline ParseResult_t parseJsonUtf8(String jsonString,
                                     EventHandler handler) noexcept {
    string_view sv{reinterpret_cast<const char*>(jsonString.data()),
                   jsonString.size()};
    sv = skipWhitespaces(sv);

    JsonPath_t jsonPath;
    return parseValue(sv, handler, &jsonPath, maxDepth_);
  }

 private:
  std::int32_t maxDepth_;
};

}  // namespace SimpleJsonReader
}  // namespace KaitoTokyo
