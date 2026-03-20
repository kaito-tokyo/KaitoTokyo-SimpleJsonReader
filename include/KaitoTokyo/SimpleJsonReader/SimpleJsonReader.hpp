// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: Apache-2.0

/**
 * @file    KaitoTokyo/SimpleJsonReader/SimpleJsonReader.hpp
 * @brief   Recursive Descent JSON parser for C++ with SAX-like interface.
 * @version 2026.03.21
 * @author  Kaito Udagawa <umireon@kaito.tokyo>
 * @see     https://github.com/kaito-tokyo/KaitoTokyo-SimpleJsonReader/
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace KaitoTokyo::SimpleJsonReader {

// clang-format off
enum class EventType : std::uint8_t {
  Undefined   = 0b00000000,
  StartObject = 0b00000001,
  EndObject   = 0b10000001,
  StartArray  = 0b00000010,
  EndArray    = 0b10000010,
  Key         = 0b00000100,
  String      = 0b00001000,
  Number      = 0b00010000,
  True        = 0b00100000,
  False       = 0b10100000,
  Null        = 0b01000000,
  XPrefix     = 0b01100000,
};
// clang-format on

struct JsonPath {
  JsonPath* parent = nullptr;
  std::variant<std::string_view, std::size_t> component = std::string_view{};
  std::int32_t depth = 0;
};

struct Event {
  std::string_view value;
  JsonPath* jsonPath;
  EventType type;
};

using EventHandler = std::function<void(Event)>;

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

constexpr auto kDefaultMaxDepth = 1000;

namespace Detail {

constexpr auto kSanitizedStructuralDoubleQuote = '\x01';

ErrorType parseValue(std::string_view* json, EventHandler handler,
                     JsonPath* jsonPath, std::int32_t depthLimit) noexcept;

void skipWhitespaces(std::string_view* json) noexcept {
  using namespace std::string_view_literals;
  std::size_t first = json->find_first_not_of(" \t\n\r"sv);
  json->remove_prefix((first == std::string_view::npos) ? json->size() : first);
}

ErrorType parseString(std::string_view* json, EventHandler handler,
                      JsonPath* jsonPath) noexcept {
  using namespace std::string_view_literals;
  std::size_t pos = json->find(kSanitizedStructuralDoubleQuote, 1);
  if (pos == std::string_view::npos) {
    return ErrorType::UnexpectedEndWhileParsingStringError;
  } else {
    handler(Event{json->substr(1, pos - 1), jsonPath, EventType::String});
    json->remove_prefix(pos + 1);
    return ErrorType::OK;
  }
}

ErrorType parseKey(std::string_view* json, EventHandler handler,
                   JsonPath* jsonPath) noexcept {
  std::size_t pos = json->find(kSanitizedStructuralDoubleQuote, 1);
  if (pos == std::string_view::npos) {
    return ErrorType::UnexpectedEndWhileParsingStringError;
  } else {
    std::string_view key = json->substr(1, pos - 1);
    jsonPath->component = key;
    handler(Event{key, jsonPath, EventType::Key});
    json->remove_prefix(pos + 1);
    return ErrorType::OK;
  }
}

ErrorType parseNumber(std::string_view* json, EventHandler handler,
                      JsonPath* jsonPath) noexcept {
  using namespace std::string_view_literals;
  std::size_t end = json->find_first_of(",]} \t\n\r"sv);
  std::size_t length = (end == std::string_view::npos) ? json->size() : end;
  handler(Event{json->substr(0, length), jsonPath, EventType::Number});
  json->remove_prefix(length);
  return ErrorType::OK;
}

ErrorType parseLiteral(std::string_view* json, EventHandler handler,
                       JsonPath* jsonPath) noexcept {
  using namespace std::string_view_literals;
  switch (json->front()) {
    case 't':
      if (json->size() >= 4 && json->compare(1, 3, "rue"sv) == 0) {
        json->remove_prefix(4);
        handler(Event{"true"sv, jsonPath, EventType::True});
        return ErrorType::OK;
      } else {
        return ErrorType::InvalidTokenLikelyTrueError;
      }
    case 'f':
      if (json->size() >= 5 && json->compare(1, 4, "alse"sv) == 0) {
        json->remove_prefix(5);
        handler(Event{"false"sv, jsonPath, EventType::False});
        return ErrorType::OK;
      } else {
        return ErrorType::InvalidTokenLikelyFalseError;
      }
    case 'n':
      if (json->size() >= 4 && json->compare(1, 3, "ull"sv) == 0) {
        json->remove_prefix(4);
        handler(Event{"null"sv, jsonPath, EventType::Null});
        return ErrorType::OK;
      } else {
        return ErrorType::InvalidTokenLikelyNullError;
      }
    default:
      return ErrorType::InvalidTokenLikelyLiteralError;
  }
}

ErrorType parseArray(std::string_view* json, EventHandler handler,
                     JsonPath* parentPath, std::uint32_t depthLimit) noexcept {
  using namespace std::string_view_literals;

  std::size_t index = 0;

  json->remove_prefix(1);
  handler(Event{"["sv, parentPath, EventType::StartArray});
  JsonPath elemPath{parentPath, std::size_t(0), parentPath->depth + 1};

  while (true) {
    skipWhitespaces(json);
    if (json->empty()) return ErrorType::UnexpectedEndWhileParsingArrayError;

    if (json->front() == ']') {
      json->remove_prefix(1);
      handler(Event{"]"sv, parentPath, EventType::EndArray});
      return ErrorType::OK;
    }

    if (depthLimit <= 0) return ErrorType::DepthLimitExceededInArrayError;

    elemPath.component = index;
    if (ErrorType err = parseValue(json, handler, &elemPath, depthLimit - 1);
        err != ErrorType::OK)
      return err;

    skipWhitespaces(json);
    if (json->empty()) return ErrorType::UnexpectedEndWhileParsingArrayError;

    switch (json->front()) {
      case ',':
        index += 1;
        json->remove_prefix(1);
        continue;
      case ']':
        json->remove_prefix(1);
        handler(Event{"]"sv, parentPath, EventType::EndArray});
        return ErrorType::OK;
      default:
        return ErrorType::InvalidTokenWhileParsingArrayError;
    }
  }
}

ErrorType parseObject(std::string_view* json, EventHandler handler,
                      JsonPath* parentPath, std::int32_t depthLimit = kDefaultMaxDepth) noexcept {
  using namespace std::string_view_literals;

  json->remove_prefix(1);
  handler(Event{"{"sv, parentPath, EventType::StartObject});
  JsonPath fieldPath{parentPath, ""sv, parentPath->depth + 1};

  while (true) {
    skipWhitespaces(json);
    if (json->empty()) return ErrorType::UnexpectedEndWhileParsingObjectError;

    if (json->front() == '}') {
      json->remove_prefix(1);
      handler(Event{"}"sv, parentPath, EventType::EndObject});
      return ErrorType::OK;
    }

    if (ErrorType err = parseKey(json, handler, &fieldPath);
        err != ErrorType::OK)
      return err;

    skipWhitespaces(json);
    if (json->empty()) return ErrorType::UnexpectedEndWhileParsingObjectError;

    if (json->front() != ':') return ErrorType::FieldDelimiterMissingError;
    json->remove_prefix(1);

    if (depthLimit <= 0) return ErrorType::DepthLimitExceededInObjectError;

    if (ErrorType err = parseValue(json, handler, &fieldPath, depthLimit - 1);
        err != ErrorType::OK)
      return err;

    skipWhitespaces(json);
    if (json->empty()) {
      return ErrorType::UnexpectedEndWhileParsingObjectError;
    }

    switch (json->front()) {
      case ',':
        json->remove_prefix(1);
        continue;
      case '}':
        json->remove_prefix(1);
        handler(Event{"}"sv, parentPath, EventType::EndObject});
        return ErrorType::OK;
      default:
        return ErrorType::InvalidTokenWhileParsingObjectError;
    }
  }
}

ErrorType parseValue(std::string_view* json, EventHandler handler,
                     JsonPath* jsonPath, std::int32_t depthLimit = kDefaultMaxDepth) noexcept {
  skipWhitespaces(json);

  if (json->empty()) return ErrorType::EmptyJSONError;

  switch (json->front()) {
    case '{':
      return parseObject(json, handler, jsonPath, depthLimit);
    case '[':
      return parseArray(json, handler, jsonPath, depthLimit);
    case kSanitizedStructuralDoubleQuote:
      return parseString(json, handler, jsonPath);
    case 't':
    case 'f':
    case 'n':
      return parseLiteral(json, handler, jsonPath);
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
      return parseNumber(json, handler, jsonPath);
    default:
      return ErrorType::InvalidTokenWhileParsingValueError;
  }
}

}  // namespace Detail

ErrorType parseJson(std::string jsonString, EventHandler handler, std::int32_t depthLimit = kDefaultMaxDepth) noexcept {
  using namespace std::string_view_literals;

  std::size_t pos = 0;
  bool inString = false;

  while (true) {
    pos = jsonString.find('"', pos);
    if (pos == std::string::npos) break;

    if (inString) {
      std::size_t rpos = pos;

      while (rpos > 0 && jsonString[rpos - 1] == '\\') {
        rpos -= 1;
      }

      if ((pos - rpos) % 2 == 0) {
        jsonString[pos] = '\x01';
        inString = false;
      }
    } else {
      jsonString[pos] = '\x01';
      inString = true;
    }

    pos += 1;
  }

  std::string_view json(jsonString);
  JsonPath jsonPath{nullptr, ""sv, 0};
  return Detail::parseValue(&json, handler, &jsonPath, depthLimit);
}

}  // namespace KaitoTokyo::SimpleJsonReader
