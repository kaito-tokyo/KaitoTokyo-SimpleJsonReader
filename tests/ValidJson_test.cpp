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
using namespace KaitoTokyo;

void printEvent(SimpleJsonReader::Event event);
void assertEvent(std::deque<SimpleJsonReader::Event>& expectedEvents,
                 SimpleJsonReader::Event receivedEvent);
void assertEnd(std::deque<SimpleJsonReader::Event>& expectedEvents);

// MARK: - Test Cases

void test_emptyObject() {
  std::string jsonString = "{}";

  std::deque<SimpleJsonReader::Event> expectedEvents{
      {"{"sv, nullptr, SimpleJsonReader::EventType::StartObject},
      {"}"sv, nullptr, SimpleJsonReader::EventType::EndObject},
  };

  SimpleJsonReader::parseJson(std::move(jsonString),
                              [&expectedEvents](auto event) {
                                printEvent(event);
                                assertEvent(expectedEvents, event);
                              });

  assertEnd(expectedEvents);
}

// MARK: - Test Runner

int main(int argc, char* argv[]) {
  std::string testName = argc == 1 ? "" : argv[1];

  if (argc == 1 || testName == "emptyObject") test_emptyObject();

  return 0;
}

void printEvent(SimpleJsonReader::Event event) {
  std::cout << "Event:" << eventTypeToString(event.type)
            << "\tValue:" << event.value
            << "\tPath:" << jsonPathToString(event.jsonPath) << std::endl;
}

void assertEvent(std::deque<SimpleJsonReader::Event>& expectedEvents,
                 SimpleJsonReader::Event receivedEvent) {
  SimpleJsonReader::Event expectedEvent = expectedEvents.front();

  if (receivedEvent.type != expectedEvent.type) {
    throw std::runtime_error("Event type mismatch");
  }

  if (receivedEvent.value != expectedEvent.value) {
    throw std::runtime_error("Event value mismatch");
  }

  expectedEvents.pop_front();
}

void assertEnd(std::deque<SimpleJsonReader::Event>& expectedEvents) {
  if (!expectedEvents.empty()) {
    throw std::runtime_error("Not all expected events were received");
  }
}