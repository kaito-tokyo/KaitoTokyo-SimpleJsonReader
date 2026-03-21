// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: CC0-1.0

#include <cstdint>
#include <string_view>
#include <string>
#include <variant>
#include <vector>

#include <KaitoTokyo/SimpleJsonReader/SimpleJsonReader.hpp>

std::string eventTypeToString(KaitoTokyo::SimpleJsonReader::EventType type) {
  using KaitoTokyo::SimpleJsonReader::EventType;

  std::uint8_t bType = static_cast<std::uint8_t>(type);

  if ((bType & 0b01100000) == 0b01100000) {
    std::uint8_t xIndex = bType & 0b00011111;
    return "x-" + std::to_string(xIndex);
  }

  switch (type) {
    case EventType::Undefined:
      return "Undefined";
    case EventType::StartObject:
      return "StartObject";
    case EventType::EndObject:
      return "EndObject";
    case EventType::StartArray:
      return "StartArray";
    case EventType::EndArray:
      return "EndArray";
    case EventType::Key:
      return "Key";
    case EventType::String:
      return "String";
    case EventType::Number:
      return "Number";
    case EventType::True:
      return "True";
    case EventType::False:
      return "False";
    case EventType::Null:
      return "Null";
    default:
      return "(INVALID)";
  }
}

std::string jsonPathToString(KaitoTokyo::SimpleJsonReader::JsonPath* jsonPath) {
  std::vector<std::string> pathComponents(jsonPath->depth);

  while (jsonPath != nullptr && jsonPath->parent != nullptr) {
    if (std::holds_alternative<std::string_view>(jsonPath->component)) {
      std::string_view fieldName =
          std::get<std::string_view>(jsonPath->component);
      pathComponents[jsonPath->depth - 1] = "." + std::string(fieldName);
    } else {
      std::size_t index = std::get<std::size_t>(jsonPath->component);
      pathComponents[jsonPath->depth - 1] = "[" + std::to_string(index) + "]";
    }
    jsonPath = jsonPath->parent;
  }

  if (pathComponents.empty()) {
    return ".";
  } else {
    std::string result;
    for (const std::string& component : pathComponents) {
      result += component;
    }
    return result;
  }
}

std::string errorTypeToString(KaitoTokyo::SimpleJsonReader::ErrorType error) {
  using KaitoTokyo::SimpleJsonReader::ErrorType;

  switch (error) {
    case ErrorType::OK:
      return "OK";
    case ErrorType::EmptyJSONError:
      return "EmptyJSONError";
    case ErrorType::UnexpectedEndWhileParsingObjectError:
      return "UnexpectedEndWhileParsingObjectError";
    case ErrorType::UnexpectedEndWhileParsingArrayError:
      return "UnexpectedEndWhileParsingArrayError";
    case ErrorType::UnexpectedEndWhileParsingStringError:
      return "UnexpectedEndWhileParsingStringError";
    case ErrorType::FieldDelimiterMissingError:
      return "FieldDelimiterMissingError";
    case ErrorType::InvalidTokenWhileParsingValueError:
      return "InvalidTokenWhileParsingValueError";
    case ErrorType::InvalidTokenWhileParsingObjectError:
      return "InvalidTokenWhileParsingObjectError";
    case ErrorType::InvalidTokenWhileParsingArrayError:
      return "InvalidTokenWhileParsingArrayError";
    case ErrorType::InvalidTokenLikelyTrueError:
      return "InvalidTokenLikelyTrueError";
    case ErrorType::InvalidTokenLikelyFalseError:
      return "InvalidTokenLikelyFalseError";
    case ErrorType::InvalidTokenLikelyNullError:
      return "InvalidTokenLikelyNullError";
    case ErrorType::InvalidTokenLikelyLiteralError:
      return "InvalidTokenLikelyLiteralError";
    case ErrorType::DepthLimitExceededInArrayError:
      return "DepthLimitExceededInArrayError";
    case ErrorType::DepthLimitExceededInObjectError:
      return "DepthLimitExceededInObjectError";
    default:
      return "(INVALID)";
  }
}
