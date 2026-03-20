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

void printEvent(SimpleJsonReader::Event event);
void assertEvent(std::deque<ExpectedEvent>& expectedEvents,
                 SimpleJsonReader::Event receivedEvent);
void assertEnd(std::deque<ExpectedEvent>& expectedEvents);

// MARK: - Test Cases

void test_emptyObject() {
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
  SimpleJsonReader::parseJson(std::move(jsonString), handler);
  assertEnd(expectedEvents);
}

void test_nestedStringsAndJsonPaths() {
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
  SimpleJsonReader::parseJson(std::move(jsonString), handler);
  assertEnd(expectedEvents);
}

// MARK: - Test Runner

int main(int argc, char* argv[]) {
  std::string testName = argc == 1 ? "" : argv[1];

  if (argc == 1 || testName == "emptyObject") test_emptyObject();
  if (argc == 1 || testName == "nestedStringsAndJsonPaths")
    test_nestedStringsAndJsonPaths();

  return 0;
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
    throw std::runtime_error("Event type mismatch");
  }

  if (receivedEvent.value != expectedEvent.value) {
    throw std::runtime_error("Event value mismatch");
  }

  if (jsonPathToString(receivedEvent.jsonPath) != expectedEvent.jsonPath) {
    throw std::runtime_error("JSON path mismatch");
  }

  expectedEvents.pop_front();
}

void assertEnd(std::deque<ExpectedEvent>& expectedEvents) {
  if (!expectedEvents.empty()) {
    throw std::runtime_error("Not all expected events were received");
  }
}
