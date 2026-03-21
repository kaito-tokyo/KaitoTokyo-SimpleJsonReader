// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: CC0-1.0

#include <KaitoTokyo/SimpleJsonReader/SimpleJsonReader.hpp>
#include <deque>
#include <iostream>
#include <string>
#include <string_view>

#include "test_helper.hpp"

using namespace std::string_view_literals;
using namespace KaitoTokyo::SimpleJsonReader;
using KaitoTokyo::SimpleJsonReader::EventType;

struct ExpectedEvent {
  EventType type;
  std::string jsonPath;
  std::string fragment;
};

void printPreamble(std::string_view testName);
void printPostamble(std::string_view testName, std::string_view status);
void printEvent(Event event);
void assertEvent(std::deque<ExpectedEvent>& expectedEvents,
                 Event receivedEvent);
void assertEnd(ErrorType err,
               std::deque<ExpectedEvent>& expectedEvents);

// MARK: - Test Cases

void test_emptyObject() {
  printPreamble("emptyObject");

  std::string jsonString = "{}";

  // clang-format off
  std::deque<ExpectedEvent> expectedEvents{
      {EventType::StartObject, ".", "{"},
      {EventType::EndObject,   ".", "}"},
  };
  // clang-format on

  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };
  ErrorType err =
      parseJson(std::move(jsonString), handler);
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
      {EventType::StartObject, ".",                      "{"},
      {EventType::Key,         ".user",                  "user"},
      {EventType::StartObject, ".user",                  "{"},
      {EventType::Key,         ".user.name",             "name"},
      {EventType::String,      ".user.name",             "山田太郎"},
      {EventType::Key,         ".user.profile",          "profile"},
      {EventType::StartObject, ".user.profile",          "{"},
      {EventType::Key,         ".user.profile.nickname", "nickname"},
      {EventType::String,      ".user.profile.nickname", "たろう"},
      {EventType::EndObject,   ".user.profile",          "}"},
      {EventType::EndObject,   ".user",                  "}"},
      {EventType::Key,         ".items",                 "items"},
      {EventType::StartArray,  ".items",                 "["},
      {EventType::String,      ".items[0]",              "りんご"},
      {EventType::String,      ".items[1]",              "🍎"},
      {EventType::String,      ".items[2]",              "abc"},
      {EventType::EndArray,    ".items",                 "]"},
      {EventType::Key,         ".message",               "message"},
      {EventType::String,      ".message",               "こんにちは世界"},
      {EventType::EndObject,   ".",                      "}"},

  };
  // clang-format on

  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };
  ErrorType err = parseJson(std::move(jsonString), handler);
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
      {EventType::StartObject, ".", "{"},
      {EventType::Key, ".languages", "languages"},
      {EventType::StartObject, ".languages", "{"},
      {EventType::Key, ".languages.japanese", "japanese"},
      {EventType::String, ".languages.japanese", "こんにちは"},
      {EventType::Key, ".languages.english", "english"},
      {EventType::String, ".languages.english", "Hello"},
      {EventType::Key, ".languages.chinese", "chinese"},
      {EventType::String, ".languages.chinese", "你好"},
      {EventType::Key, ".languages.korean", "korean"},
      {EventType::String, ".languages.korean", "안녕하세요"},
      {EventType::Key, ".languages.arabic", "arabic"},
      {EventType::String, ".languages.arabic", "مرحبا"},
      {EventType::Key, ".languages.russian", "russian"},
      {EventType::String, ".languages.russian", "Здравствуйте"},
      {EventType::EndObject, ".languages", "}"},
      {EventType::Key, ".symbols", "symbols"},
      {EventType::StartArray, ".symbols", "["},
      {EventType::String, ".symbols[0]", "😀"},
      {EventType::String, ".symbols[1]", "©"},
      {EventType::String, ".symbols[2]", "€"},
      {EventType::String, ".symbols[3]", "™"},
      {EventType::EndArray, ".symbols", "]"},
      {EventType::Key, ".mix", "mix"},
      {EventType::String, ".mix", "東京 Tokyo 北京 서울"},
      {EventType::EndObject, ".", "}"},

  };
  // clang-format on

  auto handler = [&expectedEvents](Event event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };
  ErrorType err = parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);
  printPostamble("variousLanguages", "PASSED");
}

void test_rootArray() {
  printPreamble("rootArray");

  std::string jsonString = R"(  ["root"]  )";

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::StartArray, ".", "["},
      {EventType::String, "[0]", "root"},
      {EventType::EndArray, ".", "]"},
  };

  auto handler = [&expectedEvents](Event event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  ErrorType err = parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);

  printPostamble("rootArray", "PASSED");
}

void test_rootString() {
  printPreamble("rootString");

  std::string jsonString = R"(  "root string"  )";

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::String, ".", "root string"},
  };

  auto handler = [&expectedEvents](Event event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  ErrorType err = parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);

  printPostamble("rootString", "PASSED");
}

void test_rootNumber() {
  printPreamble("rootNumber");

  std::string jsonString = R"(  123  )";

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::Number, ".", "123"},
  };

  auto handler = [&expectedEvents](Event event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  ErrorType err = parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);

  printPostamble("rootNumber", "PASSED");
}

void test_rootTrue() {
  printPreamble("rootTrue");

  std::string jsonString = R"(true)";

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::True, ".", "true"},
  };

  auto handler = [&expectedEvents](Event event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  ErrorType err = parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);

  printPostamble("rootTrue", "PASSED");
}

void test_rootFalse() {
  printPreamble("rootFalse");

  std::string jsonString = R"( false )";

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::False, ".", "false"},
  };

  auto handler = [&expectedEvents](Event event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  ErrorType err = parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);

  printPostamble("rootFalse", "PASSED");
}

void test_rootNull() {
  printPreamble("rootNull");

  std::string jsonString = R"( null )";

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::Null, ".", "null"},
  };

  auto handler = [&expectedEvents](Event event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  ErrorType err = parseJson(std::move(jsonString), handler);
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
      {EventType::StartObject, ".",              "{"},
      {EventType::Key,         ".outer",         "outer"},
      {EventType::StartArray,  ".outer",         "["},
      {EventType::StartObject, ".outer[0]",      "{"},
      {EventType::Key,         ".outer[0].inner", "inner"},
      {EventType::Number,      ".outer[0].inner", "1"},
      {EventType::EndObject,   ".outer[0]",       "}"},
      {EventType::Null,        ".outer[1]",       "null"},
      {EventType::EndArray,    ".outer",          "]"},
      {EventType::Key,         ".flag",           "flag"},
      {EventType::True,        ".flag",           "true"},
      {EventType::Key,         ".text",           "text"},
      {EventType::String,      ".text",           "ok"},
      {EventType::EndObject,   ".",               "}"},
  };
  // clang-format on

  auto handler = [&expectedEvents](Event event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  ErrorType err = parseJson(std::move(jsonString), handler);
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
      {EventType::StartObject, R"(.)",             R"({)"},
      {EventType::Key,         R"(.single)",       R"(single)"},
      {EventType::String,      R"(.single)",       R"(\")"},
      {EventType::Key,         R"(.\"quoted\")", R"(\"quoted\")"},
      {EventType::String,      R"(.\"quoted\")", R"(title)"},
      {EventType::Key,         R"(.sentence)",     R"(sentence)"},
      {EventType::String,      R"(.sentence)",     R"(She said \"hello\" and left)"},
      {EventType::Key,         R"(.array)",        R"(array)"},
      {EventType::StartArray,  R"(.array)",        R"([)"},
      {EventType::String,      R"(.array[0])",     R"(a\"b)"},
      {EventType::String,      R"(.array[1])",     R"(\"start)"},
      {EventType::String,      R"(.array[2])",     R"(end\")"},
      {EventType::EndArray,    R"(.array)",        R"(])"},
      {EventType::Key,         R"(.nested)",       R"(nested)"},
      {EventType::StartObject, R"(.nested)",       R"({)"},
      {EventType::Key,         R"(.nested.inner)", R"(inner)"},
      {EventType::String,      R"(.nested.inner)", R"(x \" y \" z)"},
      {EventType::EndObject,   R"(.nested)",       R"(})"},
      {EventType::EndObject,   R"(.)",             R"(})"},
  };
  // clang-format on

  auto handler = [&expectedEvents](Event event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  ErrorType err = parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);

  printPostamble("quotesInStrings", "PASSED");
}

void test_manyBackslashesBeforeQuote() {
  printPreamble("manyBackslashesBeforeQuote");

  std::string jsonString =
      R"(["trail\"","trail\\","trail\\\"","trail\\\\","trail\\\\\"","trail\\\\\\","trail\\\\\\\"","trail\\\\\\\\","trail\\\\\\\\\"","trail\\\\\\\\\\"])";

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::StartArray, ".", "["},
      {EventType::String, "[0]", R"(trail\")"},
      {EventType::String, "[1]", R"(trail\\)"},
      {EventType::String, "[2]", R"(trail\\\")"},
      {EventType::String, "[3]", R"(trail\\\\)"},
      {EventType::String, "[4]", R"(trail\\\\\")"},
      {EventType::String, "[5]", R"(trail\\\\\\)"},
      {EventType::String, "[6]", R"(trail\\\\\\\")"},
      {EventType::String, "[7]", R"(trail\\\\\\\\)"},
      {EventType::String, "[8]", R"(trail\\\\\\\\\")"},
      {EventType::String, "[9]", R"(trail\\\\\\\\\\)"},
      {EventType::EndArray, ".", "]"},
  };

  auto handler = [&expectedEvents](Event event) {
    printEvent(event);
    assertEvent(expectedEvents, event);
  };

  ErrorType err = parseJson(std::move(jsonString), handler);
  assertEnd(err, expectedEvents);

  printPostamble("manyBackslashesBeforeQuote", "PASSED");
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
  if (argc == 1 || testName == "whitespaceAroundTokens")
    test_whitespaceAroundTokens();
  if (argc == 1 || testName == "quotesInStrings") test_quotesInStrings();
  if (argc == 1 || testName == "manyBackslashesBeforeQuote")
    test_manyBackslashesBeforeQuote();

  return 0;
}

void printPreamble(std::string_view testName) {
  std::cout << "=== Running test: " << testName << " ===" << std::endl;
}

void printPostamble(std::string_view testName, std::string_view status) {
  std::cout << "=== Finished test: " << testName << " (" << status
            << ") ===" << std::endl;
}

void printEvent(Event event) {
  std::cout << "Event:" << eventTypeToString(event.type)
            << "\tValue:" << event.fragment
            << "\tPath:" << jsonPathToString(event.jsonPath) << std::endl;
}

void assertEvent(std::deque<ExpectedEvent>& expectedEvents,
                 Event receivedEvent) {
  ExpectedEvent expectedEvent = expectedEvents.front();

  if (receivedEvent.type != expectedEvent.type) {
    std::cout << "Expected event type: "
              << eventTypeToString(expectedEvent.type)
              << ", but got: " << eventTypeToString(receivedEvent.type)
              << std::endl;
    throw std::runtime_error("Event type mismatch");
  }

  if (receivedEvent.fragment != expectedEvent.fragment) {
    std::cout << "Expected event fragment: " << expectedEvent.fragment
              << ", but got: " << receivedEvent.fragment << std::endl;
    throw std::runtime_error("Event fragment mismatch");
  }

  if (jsonPathToString(receivedEvent.jsonPath) != expectedEvent.jsonPath) {
    throw std::runtime_error("JSON path mismatch");
  }

  expectedEvents.pop_front();
}

void assertEnd(ErrorType error,
               std::deque<ExpectedEvent>& expectedEvents) {
  if (error != ErrorType::OK) {
    throw std::runtime_error("Parsing error: " + errorTypeToString(error));
  }
  if (!expectedEvents.empty()) {
    throw std::runtime_error("Not all expected events were received");
  }
}
