// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: CC0-1.0

#include <KaitoTokyo/SimpleJsonReader/SimpleJsonReader.hpp>
#include <KaitoTokyo/SimpleJsonReader/SimpleJsonReaderHelpers.hpp>
#include <array>
#include <iostream>
#include <vector>

#include "test_helper.hpp"

using namespace std::string_view_literals;
using namespace KaitoTokyo::SimpleJsonReader;
using KaitoTokyo::SimpleJsonReader::EventType;

void  printPreamble(std::string_view testName);
void  printPostamble(std::string_view testName, std::string_view status);

// MARK: - Test Cases

Event undefinedEvent{"", nullptr, EventType::Undefined};

void  test_isStartObject() {
  printPreamble("isStartObject");

  Event event{"", nullptr, EventType::StartObject};
  if (!isStartObject(event)) {
    throw std::runtime_error("Expected isStartObject to return true");
  }

  if (isStartObject(undefinedEvent)) {
    throw std::runtime_error(
        "Expected isStartObject to return false for undefinedEvent");
  }

  printPostamble("isStartObject", "PASSED");
}

void test_isEndObject() {
  printPreamble("isEndObject");

  Event event{"", nullptr, EventType::EndObject};
  if (!isEndObject(event)) {
    throw std::runtime_error("Expected isEndObject to return true");
  }

  if (isEndObject(undefinedEvent)) {
    throw std::runtime_error(
        "Expected isEndObject to return false for undefinedEvent");
  }

  printPostamble("isEndObject", "PASSED");
}

void test_isStartArray() {
  printPreamble("isStartArray");

  Event event{"", nullptr, EventType::StartArray};
  if (!isStartArray(event)) {
    throw std::runtime_error("Expected isStartArray to return true");
  }

  if (isStartArray(undefinedEvent)) {
    throw std::runtime_error(
        "Expected isStartArray to return false for undefinedEvent");
  }

  printPostamble("isStartArray", "PASSED");
}

void test_isEndArray() {
  printPreamble("isEndArray");

  Event event{"", nullptr, EventType::EndArray};
  if (!isEndArray(event)) {
    throw std::runtime_error("Expected isEndArray to return true");
  }

  if (isEndArray(undefinedEvent)) {
    throw std::runtime_error(
        "Expected isEndArray to return false for undefinedEvent");
  }

  printPostamble("isEndArray", "PASSED");
}

void test_isKey() {
  printPreamble("isKey");

  Event event{"", nullptr, EventType::Key};
  if (!isKey(event)) {
    throw std::runtime_error("Expected isKey to return true");
  }

  if (isKey(undefinedEvent)) {
    throw std::runtime_error(
        "Expected isKey to return false for undefinedEvent");
  }

  printPostamble("isKey", "PASSED");
}

void test_isString() {
  printPreamble("isString");

  Event event{"", nullptr, EventType::String};
  if (!isString(event)) {
    throw std::runtime_error("Expected isString to return true");
  }

  if (isString(undefinedEvent)) {
    throw std::runtime_error(
        "Expected isString to return false for undefinedEvent");
  }

  printPostamble("isString", "PASSED");
}

void test_isNumber() {
  printPreamble("isNumber");

  Event event{"", nullptr, EventType::Number};
  if (!isNumber(event)) {
    throw std::runtime_error("Expected isNumber to return true");
  }

  if (isNumber(undefinedEvent)) {
    throw std::runtime_error(
        "Expected isNumber to return false for undefinedEvent");
  }

  printPostamble("isNumber", "PASSED");
}

void test_isTrue() {
  printPreamble("isTrue");

  Event event{"", nullptr, EventType::True};
  if (!isTrue(event)) {
    throw std::runtime_error("Expected isTrue to return true");
  }

  if (isTrue(undefinedEvent)) {
    throw std::runtime_error(
        "Expected isTrue to return false for undefinedEvent");
  }

  printPostamble("isTrue", "PASSED");
}

void test_isFalse() {
  printPreamble("isFalse");

  Event event{"", nullptr, EventType::False};
  if (!isFalse(event)) {
    throw std::runtime_error("Expected isFalse to return true");
  }

  if (isFalse(undefinedEvent)) {
    throw std::runtime_error(
        "Expected isFalse to return false for undefinedEvent");
  }

  printPostamble("isFalse", "PASSED");
}

void test_isNull() {
  printPreamble("isNull");

  Event event{"", nullptr, EventType::Null};
  if (!isNull(event)) {
    throw std::runtime_error("Expected isNull to return true");
  }

  if (isNull(undefinedEvent)) {
    throw std::runtime_error(
        "Expected isNull to return false for undefinedEvent");
  }

  printPostamble("isNull", "PASSED");
}

void test_matchesPathPrefix_emptyPrefixAlwaysMatchEmptyPath() {
  printPreamble("matchesPathPrefix_emptyPrefixAlwaysMatchEmptyPath");

  JsonPath root{nullptr, ""sv, 0};
  Event    event{"", &root, EventType::String};

  if (!matchesPathPrefix(event, nullptr, 0)) {
    throw std::runtime_error("Expected an empty prefix to match an empty path");
  }

  printPostamble("matchesPathPrefix_emptyPrefixAlwaysMatchEmptyPath", "PASSED");
}

void test_matchesPathPrefix_emptyPrefixAlwaysMatchOneComponentPath() {
  printPreamble("matchesPathPrefix_emptyPrefixAlwaysMatchOneComponentPath");

  JsonPath root{nullptr, ""sv, 0};
  JsonPath child{&root, "child"sv, 1};
  Event    event{"", &child, EventType::String};

  if (!matchesPathPrefix(event, nullptr, 0)) {
    throw std::runtime_error(
        "Expected an empty prefix to match a one-component path");
  }

  printPostamble("matchesPathPrefix_emptyPrefixAlwaysMatchOneComponentPath",
                 "PASSED");
}

void test_matchesPathPrefix_emptyPrefixAlwaysMatchTwoComponentsPath() {
  printPreamble("matchesPathPrefix_emptyPrefixAlwaysMatchTwoComponentsPath");

  JsonPath root{nullptr, ""sv, 0};
  JsonPath child1{&root, "child1"sv, 1};
  JsonPath child2{&child1, "child2"sv, 2};
  Event    event{"", &child2, EventType::String};

  if (!matchesPathPrefix(event, nullptr, 0)) {
    throw std::runtime_error(
        "Expected an empty prefix to match a two-component path");
  }

  printPostamble("matchesPathPrefix_emptyPrefixAlwaysMatchTwoComponentsPath",
                 "PASSED");
}

void test_matchesPathPrefix_stringPrefixCanMatchStringPath() {
  printPreamble("matchesPathPrefix_stringPrefixCanMatchStringPath");

  JsonPath                         root{nullptr, ""sv, 0};
  JsonPath                         child{&root, "child"sv, 1};
  Event                            event{"", &child, EventType::String};

  std::array<JsonPathComponent, 1> prefix{JsonPathComponent{"child"sv}};
  if (!matchesPathPrefix(event, prefix.data(), 1)) {
    throw std::runtime_error("Expected a string prefix to match a string path");
  }

  printPostamble("matchesPathPrefix_stringPrefixCanMatchStringPath", "PASSED");
}

void test_matchesPathPrefix_indexPrefixCanMatchIndexPath() {
  printPreamble("matchesPathPrefix_indexPrefixCanMatchIndexPath");

  JsonPath                         root{nullptr, ""sv, 0};
  JsonPath                         child{&root, std::size_t{0}, 1};
  Event                            event{"", &child, EventType::String};

  std::array<JsonPathComponent, 1> prefix{JsonPathComponent{std::size_t{0}}};
  if (!matchesPathPrefix(event, prefix.data(), 1)) {
    throw std::runtime_error("Expected an index prefix to match an index path");
  }

  printPostamble("matchesPathPrefix_indexPrefixCanMatchIndexPath", "PASSED");
}

void test_matchesPathPrefix_indexPrefixNeverMatchStringPath() {
  printPreamble("matchesPathPrefix_indexPrefixNeverMatchStringPath");

  JsonPath                         root{nullptr, ""sv, 0};
  JsonPath                         child{&root, "child"sv, 1};
  Event                            event{"", &child, EventType::String};

  std::array<JsonPathComponent, 1> prefix{JsonPathComponent{std::size_t{0}}};
  if (matchesPathPrefix(event, prefix.data(), 1)) {
    throw std::runtime_error(
        "Expected an index prefix to not match a string path");
  }

  printPostamble("matchesPathPrefix_indexPrefixNeverMatchStringPath", "PASSED");
}

void test_matchesPathPrefix_stringPrefixNeverMatchIndexPath() {
  printPreamble("matchesPathPrefix_stringPrefixNeverMatchIndexPath");

  JsonPath                         root{nullptr, ""sv, 0};
  JsonPath                         child{&root, std::size_t{0}, 1};
  Event                            event{"", &child, EventType::String};

  std::array<JsonPathComponent, 1> prefix{JsonPathComponent{"child"sv}};
  if (matchesPathPrefix(event, prefix.data(), 1)) {
    throw std::runtime_error(
        "Expected a string prefix to not match an index path");
  }

  printPostamble("matchesPathPrefix_stringPrefixNeverMatchIndexPath", "PASSED");
}

void test_matchesPathPrefix_stringAndIndexPrefixCanMatchStringAndIndexPath() {
  printPreamble(
      "matchesPathPrefix_stringAndIndexPrefixCanMatchStringAndIndexPath");

  JsonPath                         root{nullptr, ""sv, 0};
  JsonPath                         child1{&root, "child1"sv, 1};
  JsonPath                         child2{&child1, std::size_t{0}, 2};
  Event                            event{"", &child2, EventType::String};

  std::array<JsonPathComponent, 2> prefix{JsonPathComponent{"child1"sv},
                                          JsonPathComponent{std::size_t{0}}};
  if (!matchesPathPrefix(event, prefix.data(), 2)) {
    throw std::runtime_error(
        "Expected a string/index prefix to match a string/index path");
  }

  printPostamble(
      "matchesPathPrefix_stringAndIndexPrefixCanMatchStringAndIndexPath",
      "PASSED");
}

void test_matchesPathPrefix_indexAndStringPrefixCanMatchIndexAndStringPath() {
  printPreamble(
      "matchesPathPrefix_indexAndStringPrefixCanMatchIndexAndStringPath");

  JsonPath                         root{nullptr, ""sv, 0};
  JsonPath                         child1{&root, std::size_t{0}, 1};
  JsonPath                         child2{&child1, "child2"sv, 2};
  Event                            event{"", &child2, EventType::String};

  std::array<JsonPathComponent, 2> prefix{JsonPathComponent{std::size_t{0}},
                                          JsonPathComponent{"child2"sv}};
  if (!matchesPathPrefix(event, prefix.data(), 2)) {
    throw std::runtime_error(
        "Expected an index/string prefix to match an index/string path");
  }

  printPostamble(
      "matchesPathPrefix_indexAndStringPrefixCanMatchIndexAndStringPath",
      "PASSED");
}

void test_matchesPathPrefix_OneComponentPrefixNeverMatchEmptyPath() {
  printPreamble("matchesPathPrefix_OneComponentPrefixNeverMatchEmptyPath");

  JsonPath                         root{nullptr, ""sv, 0};
  Event                            event{"", &root, EventType::String};

  std::array<JsonPathComponent, 1> prefix{JsonPathComponent{"child"sv}};
  if (matchesPathPrefix(event, prefix.data(), 1)) {
    throw std::runtime_error(
        "Expected a one-component prefix to not match an empty path");
  }

  printPostamble("matchesPathPrefix_OneComponentPrefixNeverMatchEmptyPath",
                 "PASSED");
}

void test_matchesPathPrefix_TwoComponentsPrefixNeverMatchOneComponentsPath() {
  printPreamble(
      "matchesPathPrefix_TwoComponentsPrefixNeverMatchOneComponentsPath");

  JsonPath                         root{nullptr, ""sv, 0};
  JsonPath                         child{&root, "child"sv, 1};
  Event                            event{"", &child, EventType::String};

  std::array<JsonPathComponent, 2> prefix{JsonPathComponent{"child"sv},
                                          JsonPathComponent{"other"sv}};
  if (matchesPathPrefix(event, prefix.data(), 2)) {
    throw std::runtime_error(
        "Expected a two-component prefix to not match a one-component path");
  }

  printPostamble(
      "matchesPathPrefix_TwoComponentsPrefixNeverMatchOneComponentsPath",
      "PASSED");
}

void test_matchesPathPrefix_initializerList() {
  printPreamble("matchesPathPrefix_initializerList");

  JsonPath root{nullptr, ""sv, 0};
  JsonPath child1{&root, "child1"sv, 1};
  JsonPath child2{&child1, std::size_t{0}, 2};
  Event    event{"", &child2, EventType::String};

  if (!matchesPathPrefix(event, {"child1"sv, std::size_t{0}})) {
    throw std::runtime_error(
        "Expected the initializer list overload of matchesPathPrefix to work");
  }

  printPostamble("matchesPathPrefix_initializerList", "PASSED");
}

void test_matchesExactPath_neverMatchIfLengthMismatch() {
  printPreamble("matchesExactPath_neverMatchIfLengthMismatch");

  JsonPath                         root{nullptr, ""sv, 0};
  JsonPath                         child{&root, "child"sv, 1};
  Event                            event{"", &child, EventType::String};

  std::array<JsonPathComponent, 2> prefix{JsonPathComponent{"child"sv},
                                          JsonPathComponent{"grandchild"sv}};
  if (matchesExactPath(event, prefix.data(), 2)) {
    throw std::runtime_error(
        "Expected matchesExactPath to return false when the length mismatches");
  }

  printPostamble("matchesExactPath_neverMatchIfLengthMismatch", "PASSED");
}

void test_matchesExactPath_matchesWhenLengthAndComponentsMatch() {
  printPreamble("matchesExactPath_matchesWhenLengthAndComponentsMatch");

  JsonPath                         root{nullptr, ""sv, 0};
  JsonPath                         child1{&root, "child1"sv, 1};
  JsonPath                         child2{&child1, std::size_t{0}, 2};
  Event                            event{"", &child2, EventType::String};

  std::array<JsonPathComponent, 2> path{JsonPathComponent{"child1"sv},
                                        JsonPathComponent{std::size_t{0}}};
  if (!matchesExactPath(event, path.data(), 2)) {
    throw std::runtime_error(
        "Expected matchesExactPath to return true when length and components "
        "match");
  }

  printPostamble("matchesExactPath_matchesWhenLengthAndComponentsMatch",
                 "PASSED");
}

void test_matchesExactPath_notMatchWhenLengthMatchesButComponentsNotMatch() {
  printPreamble(
      "matchesExactPath_notMatchWhenLengthMatchesButComponentsNotMatch");

  JsonPath                         root{nullptr, ""sv, 0};
  JsonPath                         child1{&root, "child1"sv, 1};
  JsonPath                         child2{&child1, std::size_t{0}, 2};
  Event                            event{"", &child2, EventType::String};

  std::array<JsonPathComponent, 2> path{JsonPathComponent{"child1"sv},
                                        JsonPathComponent{std::size_t{1}}};
  if (matchesExactPath(event, path.data(), 2)) {
    throw std::runtime_error(
        "Expected matchesExactPath to return false when the components "
        "mismatch");
  }

  printPostamble(
      "matchesExactPath_notMatchWhenLengthMatchesButComponentsNotMatch",
      "PASSED");
}

void test_matchesExactPath_initializerList() {
  printPreamble("matchesExactPath_initializerList");

  JsonPath root{nullptr, ""sv, 0};
  JsonPath child1{&root, "child1"sv, 1};
  JsonPath child2{&child1, std::size_t{0}, 2};
  Event    event{"", &child2, EventType::String};

  if (!matchesExactPath(event, {"child1"sv, std::size_t{0}})) {
    throw std::runtime_error(
        "Expected the initializer list overload of matchesExactPath to work");
  }

  printPostamble("matchesExactPath_initializerList", "PASSED");
}

void test_forEachPathComponent() {
  printPreamble("forEachPathComponent");

  JsonPath                       root{nullptr, ""sv, 0};
  JsonPath                       child1{&root, "child1"sv, 1};
  JsonPath                       child2{&child1, std::size_t{2}, 2};
  JsonPath                       child3{&child2, "child3"sv, 3};

  Event                          event{"", &child3, EventType::String};

  std::vector<JsonPathComponent> components;
  forEachPathComponent(event,
                       [&components](const JsonPathComponent& component) {
                         components.push_back(component);
                       });

  std::vector<JsonPathComponent> expected{JsonPathComponent{"child1"sv},
                                          JsonPathComponent{std::size_t{2}},
                                          JsonPathComponent{"child3"sv}};

  if (components != expected) {
    throw std::runtime_error(
        "Expected forEachPathComponent to visit path components from root to "
        "leaf");
  }

  printPostamble("forEachPathComponent", "PASSED");
}

// MARK: - Test Runner

int main(int argc, char* argv[]) {
  std::string testName = argc == 1 ? "" : argv[1];

  if (testName.empty() || testName == "isStartObject") test_isStartObject();
  if (testName.empty() || testName == "isEndObject") test_isEndObject();
  if (testName.empty() || testName == "isStartArray") test_isStartArray();
  if (testName.empty() || testName == "isEndArray") test_isEndArray();
  if (testName.empty() || testName == "isKey") test_isKey();
  if (testName.empty() || testName == "isString") test_isString();
  if (testName.empty() || testName == "isNumber") test_isNumber();
  if (testName.empty() || testName == "isTrue") test_isTrue();
  if (testName.empty() || testName == "isFalse") test_isFalse();
  if (testName.empty() || testName == "isNull") test_isNull();
  if (testName.empty() ||
      testName == "matchesPathPrefix_emptyPrefixAlwaysMatchEmptyPath")
    test_matchesPathPrefix_emptyPrefixAlwaysMatchEmptyPath();
  if (testName.empty() ||
      testName == "matchesPathPrefix_emptyPrefixAlwaysMatchOneComponentPath")
    test_matchesPathPrefix_emptyPrefixAlwaysMatchOneComponentPath();
  if (testName.empty() ||
      testName == "matchesPathPrefix_emptyPrefixAlwaysMatchTwoComponentsPath")
    test_matchesPathPrefix_emptyPrefixAlwaysMatchTwoComponentsPath();
  if (testName.empty() ||
      testName == "matchesPathPrefix_stringPrefixCanMatchStringPath")
    test_matchesPathPrefix_stringPrefixCanMatchStringPath();
  if (testName.empty() ||
      testName == "matchesPathPrefix_indexPrefixCanMatchIndexPath")
    test_matchesPathPrefix_indexPrefixCanMatchIndexPath();
  if (testName.empty() ||
      testName == "matchesPathPrefix_indexPrefixNeverMatchStringPath")
    test_matchesPathPrefix_indexPrefixNeverMatchStringPath();
  if (testName.empty() ||
      testName == "matchesPathPrefix_stringPrefixNeverMatchIndexPath")
    test_matchesPathPrefix_stringPrefixNeverMatchIndexPath();
  if (testName.empty() ||
      testName ==
          "matchesPathPrefix_stringAndIndexPrefixCanMatchStringAndIndexPath")
    test_matchesPathPrefix_stringAndIndexPrefixCanMatchStringAndIndexPath();
  if (testName.empty() ||
      testName ==
          "matchesPathPrefix_indexAndStringPrefixCanMatchIndexAndStringPath")
    test_matchesPathPrefix_indexAndStringPrefixCanMatchIndexAndStringPath();
  if (testName.empty() ||
      testName == "matchesPathPrefix_OneComponentPrefixNeverMatchEmptyPath")
    test_matchesPathPrefix_OneComponentPrefixNeverMatchEmptyPath();
  if (testName.empty() ||
      testName ==
          "matchesPathPrefix_TwoComponentsPrefixNeverMatchOneComponentsPath")
    test_matchesPathPrefix_TwoComponentsPrefixNeverMatchOneComponentsPath();
  if (testName.empty() ||
      testName == "matchesPathPrefix_initializerListOverload")
    test_matchesPathPrefix_initializerList();
  if (testName.empty() ||
      testName == "matchesExactPath_neverMatchIfLengthMismatch")
    test_matchesExactPath_neverMatchIfLengthMismatch();
  if (testName.empty() ||
      testName == "matchesExactPath_matchesWhenLengthAndComponentsMatch")
    test_matchesExactPath_matchesWhenLengthAndComponentsMatch();
  if (testName.empty() ||
      testName ==
          "matchesExactPath_notMatchWhenLengthMatchesButComponentsNotMatch")
    test_matchesExactPath_notMatchWhenLengthMatchesButComponentsNotMatch();
  if (testName.empty() || testName == "matchesExactPath_initializerList")
    test_matchesExactPath_initializerList();
  if (testName.empty() || testName == "forEachPathComponent")
    test_forEachPathComponent();

  return 0;
}

void printPreamble(std::string_view testName) {
  std::cout << "=== Running test: " << testName << " ===" << std::endl;
}

void printPostamble(std::string_view testName, std::string_view status) {
  std::cout << "=== Finished test: " << testName << " (" << status
            << ") ===" << std::endl;
}
