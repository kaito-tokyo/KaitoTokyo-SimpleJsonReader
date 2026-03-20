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
constexpr auto True = SimpleJsonReader::EventType::True;
constexpr auto False = SimpleJsonReader::EventType::False;
constexpr auto Null = SimpleJsonReader::EventType::Null;

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
void assertEnd(SimpleJsonReader::ErrorType err, std::deque<ExpectedEvent>& expectedEvents);

// MARK: - Test Cases

void test_emptyObject() {
  printPreamble("emptyObject");

  std::string jsonString = "{}";

  // clang-format off
  std::deque<ExpectedEvent> expectedEvents{
      {StartObject, ".", "{"},
      {EndObject,   ".", "}"},
  };
  // clang-format on

  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };
  SimpleJsonReader::ErrorType err = SimpleJsonReader::parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);
  printPostamble("emptyObject", "PASSED");
}

void test_nestedStringsAndJsonPaths() {
  printPreamble("nestedStringsAndJsonPaths");

  std::string jsonString = R"({
    "user": {
      "name": "山田太郎",
      "profile": {
        "nickname": "たろう"
      }
    },
    "items": ["りんご", "🍎", "abc"],
    "message": "こんにちは世界"
  })";

  // clang-format off
  std::deque<ExpectedEvent> expectedEvents{
      {StartObject, ".",                      "{"},
      {Key,         ".user",                  "user"},
      {StartObject, ".user",                  "{"},
      {Key,         ".user.name",             "name"},
      {String,      ".user.name",             "山田太郎"},
      {Key,         ".user.profile",          "profile"},
      {StartObject, ".user.profile",          "{"},
      {Key,         ".user.profile.nickname", "nickname"},
      {String,      ".user.profile.nickname", "たろう"},
      {EndObject,   ".user.profile",          "}"},
      {EndObject,   ".user",                  "}"},
      {Key,         ".items",                 "items"},
      {StartArray,  ".items",                 "["},
      {String,      ".items[0]",              "りんご"},
      {String,      ".items[1]",              "🍎"},
      {String,      ".items[2]",              "abc"},
      {EndArray,    ".items",                 "]"},
      {Key,         ".message",               "message"},
      {String,      ".message",               "こんにちは世界"},
      {EndObject,   ".",                      "}"},
  };
  // clang-format on

  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };
  SimpleJsonReader::ErrorType err = SimpleJsonReader::parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);
  printPostamble("nestedStringsAndJsonPaths", "PASSED");
}

void test_variousLanguages() {
  printPreamble("variousLanguages");

  std::string jsonString = R"({
    "languages": {
      "japanese": "こんにちは",
      "english": "Hello",
      "chinese": "你好",
      "korean": "안녕하세요",
      "arabic": "مرحبا",
      "russian": "Здравствуйте"
    },
    "symbols": ["😀", "©", "€", "™"],
    "mix": "東京 Tokyo 北京 서울"
  })";

  // clang-format off
  std::deque<ExpectedEvent> expectedEvents{
      {StartObject, ".", "{"},
      {Key, ".languages", "languages"},
      {StartObject, ".languages", "{"},
      {Key, ".languages.japanese", "japanese"},
      {String, ".languages.japanese", "こんにちは"},
      {Key, ".languages.english", "english"},
      {String, ".languages.english", "Hello"},
      {Key, ".languages.chinese", "chinese"},
      {String, ".languages.chinese", "你好"},
      {Key, ".languages.korean", "korean"},
      {String, ".languages.korean", "안녕하세요"},
      {Key, ".languages.arabic", "arabic"},
      {String, ".languages.arabic", "مرحبا"},
      {Key, ".languages.russian", "russian"},
      {String, ".languages.russian", "Здравствуйте"},
      {EndObject, ".languages", "}"},
      {Key, ".symbols", "symbols"},
      {StartArray, ".symbols", "["},
      {String, ".symbols[0]", "😀"},
      {String, ".symbols[1]", "©"},
      {String, ".symbols[2]", "€"},
      {String, ".symbols[3]", "™"},
      {EndArray, ".symbols", "]"},
      {Key, ".mix", "mix"},
      {String, ".mix", "東京 Tokyo 北京 서울"},
      {EndObject, ".", "}"},
  };
  // clang-format on

  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };
  SimpleJsonReader::ErrorType err = SimpleJsonReader::parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);
  printPostamble("variousLanguages", "PASSED");
}

void test_rootArray() {
  printPreamble("rootArray");

  std::string jsonString = R"(  ["root"]  )";

  std::deque<ExpectedEvent> expectedEvents{
      {StartArray, ".", "["},
      {String, "[0]", "root"},
      {EndArray, ".", "]"},
  };

  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  SimpleJsonReader::ErrorType err = SimpleJsonReader::parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);

  printPostamble("rootArray", "PASSED");
}

void test_rootString() {
  printPreamble("rootString");

  std::string jsonString = R"(  "root string"  )";

  std::deque<ExpectedEvent> expectedEvents{
      {String, ".", "root string"},
  };

  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  SimpleJsonReader::ErrorType err = SimpleJsonReader::parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);

  printPostamble("rootString", "PASSED");
}

void test_rootNumber() {
  printPreamble("rootNumber");

  std::string jsonString = R"(  123  )";

  std::deque<ExpectedEvent> expectedEvents{
      {Number, ".", "123"},
  };

  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  SimpleJsonReader::ErrorType err = SimpleJsonReader::parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);

  printPostamble("rootNumber", "PASSED");
}

void test_rootTrue() {
  printPreamble("rootTrue");

  std::string jsonString = R"(true)";

  std::deque<ExpectedEvent> expectedEvents{
      {True, ".", "true"},
  };

  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  SimpleJsonReader::ErrorType err = SimpleJsonReader::parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);

  printPostamble("rootTrue", "PASSED");
}

void test_rootFalse() {
  printPreamble("rootFalse");

  std::string jsonString = R"( false )";

  std::deque<ExpectedEvent> expectedEvents{
      {False, ".", "false"},
  };

  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  SimpleJsonReader::ErrorType err = SimpleJsonReader::parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);

  printPostamble("rootFalse", "PASSED");
}

void test_rootNull() {
  printPreamble("rootNull");

  std::string jsonString = R"( null )";

  std::deque<ExpectedEvent> expectedEvents{
      {Null, ".", "null"},
  };

  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  SimpleJsonReader::ErrorType err = SimpleJsonReader::parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);

  printPostamble("rootNull", "PASSED");
}

void test_whitespaceAroundTokens() {
  printPreamble("whitespaceAroundTokens");

  std::string jsonString =
      " \n\t { \r\n"
      "  \t\"outer\"  :  [  \n"
      "    {  \"inner\"  :  1  }  ,  \n"
      "    null  \n"
      "  ]  ,  \n"
      "  \"flag\"  :  true  ,  \n"
      "  \"text\"  :  \"ok\"  \n"
      "} \n \t ";

  // clang-format off
  std::deque<ExpectedEvent> expectedEvents{
      {StartObject, ".",              "{"},
      {Key,         ".outer",         "outer"},
      {StartArray,  ".outer",         "["},
      {StartObject, ".outer[0]",      "{"},
      {Key,         ".outer[0].inner", "inner"},
      {Number,      ".outer[0].inner", "1"},
      {EndObject,   ".outer[0]",       "}"},
      {Null,        ".outer[1]",       "null"},
      {EndArray,    ".outer",          "]"},
      {Key,         ".flag",           "flag"},
      {True,        ".flag",           "true"},
      {Key,         ".text",           "text"},
      {String,      ".text",           "ok"},
      {EndObject,   ".",               "}"},

  };
  // clang-format on

  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  SimpleJsonReader::ErrorType err = SimpleJsonReader::parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);

  printPostamble("whitespaceAroundTokens", "PASSED");
}

void test_quotesInStrings() {
  printPreamble("quotesInStrings");

  std::string jsonString = R"({
  "single": "\"",
  "\"quoted\"": "title",
  "sentence": "She said \"hello\" and left",
  "array": ["a\"b", "\"start", "end\""],
  "nested": { "inner": "x \" y \" z" }
})";

  // clang-format off
  std::deque<ExpectedEvent> expectedEvents{
      {StartObject, R"(.)",             R"({)"},
      {Key,         R"(.single)",       R"(single)"},
      {String,      R"(.single)",       R"(\")"},
      {Key,         R"(.\"quoted\")", R"(\"quoted\")"},
      {String,      R"(.\"quoted\")", R"(title)"},
      {Key,         R"(.sentence)",     R"(sentence)"},
      {String,      R"(.sentence)",     R"(She said \"hello\" and left)"},
      {Key,         R"(.array)",        R"(array)"},
      {StartArray,  R"(.array)",        R"([)"},
      {String,      R"(.array[0])",     R"(a\"b)"},
      {String,      R"(.array[1])",     R"(\"start)"},
      {String,      R"(.array[2])",     R"(end\")"},
      {EndArray,    R"(.array)",        R"(])"},
      {Key,         R"(.nested)",       R"(nested)"},
      {StartObject, R"(.nested)",       R"({)"},
      {Key,         R"(.nested.inner)", R"(inner)"},
      {String,      R"(.nested.inner)", R"(x \" y \" z)"},
      {EndObject,   R"(.nested)",       R"(})"},
      {EndObject,   R"(.)",             R"(})"},
  };
  // clang-format on

  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  SimpleJsonReader::ErrorType err = SimpleJsonReader::parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);

  printPostamble("quotesInStrings", "PASSED");
}

// MARK: - Test Runner

int main(int argc, char* argv[]) {
  std::string testName = argc == 1 ? "" : argv[1];

  if (argc == 1 || testName == "emptyObject") test_emptyObject();
  if (argc == 1 || testName == "nestedStringsAndJsonPaths")
    test_nestedStringsAndJsonPaths();
  if (argc == 1 || testName == "variousLanguages") test_variousLanguages();
  if (argc == 1 || testName == "rootArray") test_rootArray();
  if (argc == 1 || testName == "rootString") test_rootString();
  if (argc == 1 || testName == "rootNumber") test_rootNumber();
  if (argc == 1 || testName == "rootTrue") test_rootTrue();
  if (argc == 1 || testName == "rootFalse") test_rootFalse();
  if (argc == 1 || testName == "rootNull") test_rootNull();
  if (argc == 1 || testName == "whitespaceAroundTokens") test_whitespaceAroundTokens();
  if (argc == 1 || testName == "quotesInStrings") test_quotesInStrings();

  return 0;
}

void printPreamble(std::string_view testName) {
  std::cout << "=== Running test: " << testName << " ===" << std::endl;
}

void printPostamble(std::string_view testName, std::string_view status) {
  std::cout << "=== Finished test: " << testName << " (" << status << ") ===" << std::endl;
}

void printEvent(SimpleJsonReader::Event event) {
  std::cout << "Event:" << eventTypeToString(event.type)
            << "\tValue:" << event.value
            << "\tPath:" << jsonPathToString(event.jsonPath) << std::endl;
}

void assertEvent(std::deque<ExpectedEvent>& expectedEvents,
                 SimpleJsonReader::Event receivedEvent) {
  ExpectedEvent expectedEvent = expectedEvents.front();

  if (receivedEvent.type != expectedEvent.type) {
    std::cout << "Expected event type: " << eventTypeToString(expectedEvent.type)
              << ", but got: " << eventTypeToString(receivedEvent.type) << std::endl;
    throw std::runtime_error("Event type mismatch");
  }

  if (receivedEvent.value != expectedEvent.value) {
    std::cout << "Expected event value: " << expectedEvent.value
              << ", but got: " << receivedEvent.value << std::endl;
    throw std::runtime_error("Event value mismatch");
  }

  if (jsonPathToString(receivedEvent.jsonPath) != expectedEvent.jsonPath) {
    throw std::runtime_error("JSON path mismatch");
  }

  expectedEvents.pop_front();
}

void assertEnd(SimpleJsonReader::ErrorType error, std::deque<ExpectedEvent>& expectedEvents) {
  if (error != SimpleJsonReader::ErrorType::OK) {
    throw std::runtime_error("Parsing error: " + errorTypeToString(error));
  }
  if (!expectedEvents.empty()) {
    throw std::runtime_error("Not all expected events were received");
  }
}
