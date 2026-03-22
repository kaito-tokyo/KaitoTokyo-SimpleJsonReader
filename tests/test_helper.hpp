// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: Apache-2.0

#include <KaitoTokyo/SimpleJsonReader/SimpleJsonReader.hpp>
#include <cstddef>
#include <deque>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

typedef void (*TestFunc)();
extern const char* g_testNames[];
extern const TestFunc g_testFunctions[];
extern const int g_numTests;

inline std::ostream& operator<<(
    std::ostream& os, const KaitoTokyo::SimpleJsonReader::C8StringView& sv) {
  return os << std::string_view(sv.data(), sv.size());
}

struct ExpectedEvent {
  KaitoTokyo::SimpleJsonReader::EventType type;
  std::string jsonPathString;
  std::string fragment;
};

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

template <typename string_view>
std::string jsonPathToString(
    KaitoTokyo::SimpleJsonReader::JsonPath<string_view>* jsonPath) {
  std::vector<std::string> pathComponents(jsonPath->depth);

  while (jsonPath != nullptr && jsonPath->parent != nullptr) {
    if (std::holds_alternative<string_view>(jsonPath->component)) {
      auto keySv = std::get<string_view>(jsonPath->component);
      pathComponents[jsonPath->depth - 1] =
          "[\"" + std::string(keySv.data(), keySv.size()) + "\"]";
    } else {
      std::size_t index = std::get<std::size_t>(jsonPath->component);
      pathComponents[jsonPath->depth - 1] = "[" + std::to_string(index) + "]";
    }
    jsonPath = jsonPath->parent;
  }

  std::string resultString = ".";
  for (const auto& component : pathComponents) {
    resultString += component;
  }
  return resultString;
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

void printPreamble(std::string testName) {
  std::cout << "=== TEST CASE STARTED: " << testName << " ===" << std::endl;
}

void printPostamble(std::string testName) {
  std::cout << "=== TEST CASE FINISHED: " << testName << " ===" << std::endl
            << std::endl;
}

template <typename string_view>
void printEvent(const KaitoTokyo::SimpleJsonReader::Event<string_view>& event) {
  std::cout << "Event:" << eventTypeToString(event.type)
            << "\tFragment:" << event.fragment
            << "\tPath:" << jsonPathToString(event.jsonPath) << std::endl;
}

template <typename string_view>
bool assertEvent(
    const std::string& testName, std::deque<ExpectedEvent>& expectedEvents,
    const KaitoTokyo::SimpleJsonReader::Event<string_view>& receivedEvent) {
  bool ok = true;

  ExpectedEvent expectedEvent = expectedEvents.front();
  expectedEvents.pop_front();

  if (receivedEvent.type != expectedEvent.type) {
    std::cout << "## assertEvent FAIL in " << testName << std::endl
              << std::endl;
    std::cout << "Cause: EventType is not as expected." << std::endl;
    std::cout << "**Expected EventType**: "
              << eventTypeToString(expectedEvent.type) << std::endl;
    std::cout << "**Received EventType**: "
              << eventTypeToString(receivedEvent.type) << std::endl
              << std::endl;
    ok = false;
  }

  std::string receivedFragment(receivedEvent.fragment.data(),
                               receivedEvent.fragment.size());
  if (receivedFragment != expectedEvent.fragment) {
    std::cout << "## assertEvent FAIL in " << testName << std::endl
              << std::endl;
    std::cout << "Cause: EventFragment is not as expected." << std::endl;
    std::cout << "**Expected EventFragment**: " << expectedEvent.fragment
              << std::endl;
    std::cout << "**Received EventFragment**: " << receivedFragment << std::endl
              << std::endl;
    ok = false;
  }

  std::string receivedJsonPathString = jsonPathToString(receivedEvent.jsonPath);
  if (receivedJsonPathString != expectedEvent.jsonPathString) {
    std::cout << "## assertEvent FAIL in " << testName << std::endl
              << std::endl;
    std::cout << "Cause: EventJsonPath is not as expected." << std::endl;
    std::cout << "**Expected EventJsonPath**: " << expectedEvent.jsonPathString
              << std::endl;
    std::cout << "**Received EventJsonPath**: " << receivedJsonPathString
              << std::endl
              << std::endl;
    ok = false;
  }

  return ok;
}

template <typename string_view>
bool assertEnd(
    const std::string& testName,
    const KaitoTokyo::SimpleJsonReader::ParseResult<string_view>& result,
    const std::deque<ExpectedEvent>& remainingExpectedEvents) {
  auto [tail, err] = result;
  if (err != KaitoTokyo::SimpleJsonReader::ErrorType::OK) {
    std::cout << "## assertEnd FAIL in " << testName << std::endl << std::endl;
    std::cout << "Cause: Result is not ErrorType::OK." << std::endl
              << std::endl;
    std::cout << "**ErrorType**: " << errorTypeToString(err) << std::endl
              << std::endl;
    std::cout << "<Tail>" << std::endl;
    std::cout << tail << std::endl;
    std::cout << "</Tail>" << std::endl;
    return false;
  }

  if (!remainingExpectedEvents.empty()) {
    std::cout << "## assertEnd FAIL in " << testName << std::endl << std::endl;
    std::cout << "Cause: Not all expected events were received." << std::endl
              << std::endl;
    std::cout << "<Tail>" << std::endl;
    std::cout << tail << std::endl;
    std::cout << "</Tail>" << std::endl << std::endl;
    std::cout << "<RemainingExpectedEvents>" << std::endl;
    std::cout << remainingExpectedEvents.size()
              << " event(s) were expected but not received:" << std::endl;
    std::cout << "</RemainingExpectedEvents>" << std::endl;
    return false;
  }

  return true;
}
