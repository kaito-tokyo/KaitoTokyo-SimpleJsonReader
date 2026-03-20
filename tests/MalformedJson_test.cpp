// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: CC0-1.0

#include <KaitoTokyo/SimpleJsonReader/SimpleJsonReader.hpp>
#include <deque>
#include <iostream>
#include <string>
#include <string_view>

#include "test_helper.hpp"

using namespace KaitoTokyo;

constexpr auto StartObject = SimpleJsonReader::EventType::StartObject;
constexpr auto EndObject = SimpleJsonReader::EventType::EndObject;
constexpr auto StartArray = SimpleJsonReader::EventType::StartArray;
constexpr auto EndArray = SimpleJsonReader::EventType::EndArray;
constexpr auto Key = SimpleJsonReader::EventType::Key;
constexpr auto String = SimpleJsonReader::EventType::String;
constexpr auto Number = SimpleJsonReader::EventType::Number;

struct ExpectedEvent {
  SimpleJsonReader::EventType type;
  std::string jsonPath;
  std::string value;
};

void printPreamble(std::string_view testName);
void printPostamble(std::string_view testName, std::string_view status);
void printEvent(SimpleJsonReader::Event event);
void assertEvent(std::deque<ExpectedEvent>& expectedEvents,
                 SimpleJsonReader::Event receivedEvent);
void assertEnd(std::deque<ExpectedEvent>& expectedEvents);

// MARK: - Test Cases

void test_emptyString() {
  printPreamble("emptyString");

  std::string jsonString = "";

  SimpleJsonReader::ErrorType err = SimpleJsonReader::parseJson(std::move(jsonString), [](auto event) {});
  if (err != SimpleJsonReader::ErrorType::EmptyJSONError) {
    throw std::runtime_error("Expected EmptyJSONError, but got: " + errorTypeToString(err));
  }

  printPostamble("emptyString", "PASSED");
}

// MARK: - Test Runner

int main(int argc, char* argv[]) {
  std::string testName = argc == 1 ? "" : argv[1];

  if (argc == 1 || testName == "emptyString") test_emptyString();

  return 0;
}

void printPreamble(std::string_view testName) {
  std::cout << "=== Running test: " << testName << " ===" << std::endl;
}

void printPostamble(std::string_view testName, std::string_view status) {
  std::cout << "=== Finished test: " << testName << " (" << status << ") ===" << std::endl;
}
