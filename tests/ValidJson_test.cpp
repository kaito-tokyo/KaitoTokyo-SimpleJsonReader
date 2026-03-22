// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: CC0-1.0

#include <KaitoTokyo/SimpleJsonReader/SimpleJsonReader.hpp>

#include "test_helper.hpp"

#ifdef USE_STD_STRING_VIEW
using string_view = std::string_view;
#else
using string_view = KaitoTokyo::SimpleJsonReader::C8StringView;
#endif

using namespace KaitoTokyo::SimpleJsonReader;
using KaitoTokyo::SimpleJsonReader::EventType;

// MARK: - Test Cases

void test_emptyObject() {
  printPreamble(__func__);

  std::string jsonString = "{}";

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::StartObject, ".", "{"},
      {EventType::EndObject, ".", "}"},
  };

  SimpleJsonReader<string_view> reader;
  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(__func__, expectedEvents, event);
  };
  auto result = reader.parseJsonUtf8(jsonString, handler);
  assertEnd(__func__, result, expectedEvents);

  printPostamble(__func__);
}

void test_nestedStringsAndJsonPaths() {
  printPreamble(__func__);

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

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::StartObject, ".", "{"},
      {EventType::Key, ".user", "user"},
      {EventType::StartObject, ".user", "{"},
      {EventType::Key, ".user.name", "name"},
      {EventType::String, ".user.name", "山田太郎"},
      {EventType::Key, ".user.profile", "profile"},
      {EventType::StartObject, ".user.profile", "{"},
      {EventType::Key, ".user.profile.nickname", "nickname"},
      {EventType::String, ".user.profile.nickname", "たろう"},
      {EventType::EndObject, ".user.profile", "}"},
      {EventType::EndObject, ".user", "}"},
      {EventType::Key, ".items", "items"},
      {EventType::StartArray, ".items", "["},
      {EventType::String, ".items[0]", "りんご"},
      {EventType::String, ".items[1]", "🍎"},
      {EventType::String, ".items[2]", "abc"},
      {EventType::EndArray, ".items", "]"},
      {EventType::Key, ".message", "message"},
      {EventType::String, ".message", "こんにちは世界"},
      {EventType::EndObject, ".", "}"},
  };

  SimpleJsonReader<string_view> reader;
  auto handler = [&expectedEvents](auto event) {
        printEvent(event);
        assertEvent(__func__, expectedEvents, event);
      };
  auto result = reader.parseJsonUtf8(jsonString, handler);
  assertEnd(__func__, result, expectedEvents);

  printPostamble(__func__);
}

void test_variousLanguages() {
  printPreamble(__func__);

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

  SimpleJsonReader<string_view> reader;
  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(__func__, expectedEvents, event);
  };
  auto result = reader.parseJsonUtf8(jsonString, handler);
  assertEnd(__func__, result, expectedEvents);

  printPostamble(__func__);
}

void test_rootArray() {
  printPreamble(__func__);

  std::string jsonString = R"(  ["root"]  )";

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::StartArray, ".", "["},
      {EventType::String, "[0]", "root"},
      {EventType::EndArray, ".", "]"},
  };

  SimpleJsonReader<string_view> reader;
  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(__func__, expectedEvents, event);
  };
  auto result = reader.parseJsonUtf8(jsonString, handler);
  assertEnd(__func__, result, expectedEvents);

  printPostamble(__func__);
}

void test_rootString() {
  printPreamble(__func__);

  std::string jsonString = R"(  "root string"  )";

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::String, ".", "root string"},
  };

  SimpleJsonReader<string_view> reader;
  auto result =
      reader.parseJsonUtf8(jsonString, [&expectedEvents](auto event) {
        printEvent(event);
        assertEvent(__func__, expectedEvents, event);
      });
  assertEnd(__func__, result, expectedEvents);

  printPostamble(__func__);
}

void test_rootNumber() {
  printPreamble(__func__);

  std::string jsonString = R"(  123  )";

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::Number, ".", "123"},
  };

  SimpleJsonReader<string_view> reader;
  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(__func__, expectedEvents, event);
  };
  auto result = reader.parseJsonUtf8(jsonString, handler);
  assertEnd(__func__, result, expectedEvents);

  printPostamble(__func__);
}

void test_rootTrue() {
  printPreamble(__func__);

  std::string jsonString = R"(true)";

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::True, ".", "true"},
  };

  SimpleJsonReader<string_view> reader;
  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(__func__, expectedEvents, event);
  };
  auto result = reader.parseJsonUtf8(jsonString, handler);
  assertEnd(__func__, result, expectedEvents);

  printPostamble(__func__);
}

void test_rootFalse() {
  printPreamble(__func__);

  std::string jsonString = R"( false )";

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::False, ".", "false"},
  };

  SimpleJsonReader<string_view> reader;
  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(__func__, expectedEvents, event);
  };
  auto result = reader.parseJsonUtf8(jsonString, handler);
  assertEnd(__func__, result, expectedEvents);

  printPostamble(__func__);
}

void test_rootNull() {
  printPreamble(__func__);

  std::string jsonString = R"( null )";

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::Null, ".", "null"},
  };

  SimpleJsonReader<string_view> reader;
  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(__func__, expectedEvents, event);
  };
  auto result = reader.parseJsonUtf8(jsonString, handler);
  assertEnd(__func__, result, expectedEvents);

  printPostamble(__func__);
}

void test_whitespaceAroundTokens() {
  printPreamble(__func__);

  std::string jsonString =
      " \n\t { \r\n"
      "  \t\"outer\"  :  [  \n"
      "    {  \"inner\"  :  1  }  ,  \n"
      "    null  \n"
      "  ]  ,  \n"
      "  \"flag\"  :  true  ,  \n"
      "  \"text\"  :  \"ok\"  \n"
      "} \n \t ";

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::StartObject, ".", "{"},
      {EventType::Key, ".outer", "outer"},
      {EventType::StartArray, ".outer", "["},
      {EventType::StartObject, ".outer[0]", "{"},
      {EventType::Key, ".outer[0].inner", "inner"},
      {EventType::Number, ".outer[0].inner", "1"},
      {EventType::EndObject, ".outer[0]", "}"},
      {EventType::Null, ".outer[1]", "null"},
      {EventType::EndArray, ".outer", "]"},
      {EventType::Key, ".flag", "flag"},
      {EventType::True, ".flag", "true"},
      {EventType::Key, ".text", "text"},
      {EventType::String, ".text", "ok"},
      {EventType::EndObject, ".", "}"},
  };

  SimpleJsonReader<string_view> reader;
  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(__func__, expectedEvents, event);
  };
  auto result = reader.parseJsonUtf8(jsonString, handler);
  assertEnd(__func__, result, expectedEvents);

  printPostamble(__func__);
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

  std::deque<ExpectedEvent> expectedEvents{
      {EventType::StartObject, R"(.)", R"({)"},
      {EventType::Key, R"(.single)", R"(single)"},
      {EventType::String, R"(.single)", R"(\")"},
      {EventType::Key, R"(.\"quoted\")", R"(\"quoted\")"},
      {EventType::String, R"(.\"quoted\")", R"(title)"},
      {EventType::Key, R"(.sentence)", R"(sentence)"},
      {EventType::String, R"(.sentence)", R"(She said \"hello\" and left)"},
      {EventType::Key, R"(.array)", R"(array)"},
      {EventType::StartArray, R"(.array)", R"([)"},
      {EventType::String, R"(.array[0])", R"(a\"b)"},
      {EventType::String, R"(.array[1])", R"(\"start)"},
      {EventType::String, R"(.array[2])", R"(end\")"},
      {EventType::EndArray, R"(.array)", R"(])"},
      {EventType::Key, R"(.nested)", R"(nested)"},
      {EventType::StartObject, R"(.nested)", R"({)"},
      {EventType::Key, R"(.nested.inner)", R"(inner)"},
      {EventType::String, R"(.nested.inner)", R"(x \" y \" z)"},
      {EventType::EndObject, R"(.nested)", R"(})"},
      {EventType::EndObject, R"(.)", R"(})"},
  };

  SimpleJsonReader<string_view> reader;
  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(__func__, expectedEvents, event);
  };
  auto result = reader.parseJsonUtf8(jsonString, handler);
  assertEnd(__func__, result, expectedEvents);

  printPostamble(__func__);
}

void test_manyBackslashesBeforeQuote() {
  printPreamble(__func__);

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

  SimpleJsonReader<string_view> reader;
  auto handler = [&expectedEvents](auto event) {
    printEvent(event);
    assertEvent(__func__, expectedEvents, event);
  };
  auto result = reader.parseJsonUtf8(jsonString, handler);
  assertEnd(__func__, result, expectedEvents);

  printPostamble(__func__);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args(argv + 1, argv + argc);

  for (int i = 0; i < g_numTests; i++) {
    std::string testName = g_testNames[i];
    TestFunc testFunc = g_testFunctions[i];

    auto testNameContains = [&testName](const auto &arg) {
      return testName.find(arg) != std::string::npos;
    };

    if (args.empty() || std::find_if(args.begin(), args.end(), testNameContains) != args.end()) {
      testFunc();
    }
  }

  return 0;
}

#include "ValidJson_test.cpp.hpp"
