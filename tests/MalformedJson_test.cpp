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
void assertParseFails(std::string_view testName, std::string jsonString);

// MARK: - Test Cases

void test_emptyString() {
  assertParseFails("emptyString", "");
}

void test_unterminatedString() {
  assertParseFails("unterminatedString", R"("unterminated)");
}

void test_unterminatedObject() {
  assertParseFails("unterminatedObject", R"({"key": 1)");
}

void test_unterminatedArray() {
  assertParseFails("unterminatedArray", R"([true, false)");
}

void test_missingFieldDelimiter() {
  assertParseFails("missingFieldDelimiter", R"({"key" 1})");
}

void test_invalidTokenOrdering() {
  assertParseFails("invalidTokenOrdering", R"([1 false])");
}

void test_startEndElementsMixedAtRoot() {
  assertParseFails("startEndElementsMixedAtRoot", R"([}])");
}

void test_startEndElementsMixedInObject() {
  assertParseFails("startEndElementsMixedInObject", R"({])");
}

void test_closeBracketWhereObjectValueShouldBe() {
  assertParseFails("closeBracketWhereObjectValueShouldBe", R"({"a": ]})");
}

void test_closeBraceWhereArrayValueShouldBe() {
  assertParseFails("closeBraceWhereArrayValueShouldBe", R"([1, }])");
}

void test_nestedStartEndChaos() {
  assertParseFails("nestedStartEndChaos", R"([{"a":[{]}])");
}

// MARK: - Test Runner

int main(int argc, char* argv[]) {
  std::string testName = argc == 1 ? "" : argv[1];

  if (argc == 1 || testName == "emptyString") test_emptyString();
  if (argc == 1 || testName == "unterminatedString") test_unterminatedString();
  if (argc == 1 || testName == "unterminatedObject") test_unterminatedObject();
  if (argc == 1 || testName == "unterminatedArray") test_unterminatedArray();
  if (argc == 1 || testName == "missingFieldDelimiter")
    test_missingFieldDelimiter();
  if (argc == 1 || testName == "invalidTokenOrdering")
    test_invalidTokenOrdering();
  if (argc == 1 || testName == "startEndElementsMixedAtRoot")
    test_startEndElementsMixedAtRoot();
  if (argc == 1 || testName == "startEndElementsMixedInObject")
    test_startEndElementsMixedInObject();
  if (argc == 1 || testName == "closeBracketWhereObjectValueShouldBe")
    test_closeBracketWhereObjectValueShouldBe();
  if (argc == 1 || testName == "closeBraceWhereArrayValueShouldBe")
    test_closeBraceWhereArrayValueShouldBe();
  if (argc == 1 || testName == "nestedStartEndChaos")
    test_nestedStartEndChaos();

  return 0;
}

void printPreamble(std::string_view testName) {
  std::cout << "=== Running test: " << testName << " ===" << std::endl;
}

void printPostamble(std::string_view testName, std::string_view status) {
  std::cout << "=== Finished test: " << testName << " (" << status
            << ") ===" << std::endl;
}

void assertParseFails(std::string_view testName, std::string jsonString) {
  printPreamble(testName);

  SimpleJsonReader::ErrorType err =
      SimpleJsonReader::parseJson(std::move(jsonString), [](auto event) {});
  if (err == SimpleJsonReader::ErrorType::OK) {
    throw std::runtime_error("Expected parseJson to fail, but got OK");
  }

  printPostamble(testName, "PASSED");
}
