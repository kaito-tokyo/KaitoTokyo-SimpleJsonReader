// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: Apache-2.0

#include <KaitoTokyo/SimpleJsonReader/SimpleJsonReader.hpp>
#include <deque>
#include <iostream>
#include <string>
#include <string_view>

#include "test_helper.hpp"

using namespace KaitoTokyo::SimpleJsonReader;
using KaitoTokyo::SimpleJsonReader::EventType;

#ifdef USE_STD_STRING_VIEW
using string_view = std::string_view;
#else
using string_view = KaitoTokyo::SimpleJsonReader::C8StringView;
#endif

void assertParseFails(std::string testName, std::string jsonString) {
  printPreamble(testName);

  SimpleJsonReader<string_view> reader;
  auto [tail, err] = reader.parseJsonUtf8(jsonString, [](auto) {});
  if (err == ErrorType::OK) {
    std::cout << "## assertParseFails FAIL in " << testName << std::endl
              << std::endl;
    std::cout << "Cause: Expected parseJson to fail, but got OK." << std::endl
              << std::endl;
  } else {
    std::cout << "Expected parse failure, got error: " << errorTypeToString(err)
              << std::endl
              << std::endl;
  }

  printPostamble(testName);
}

void test_emptyString() { assertParseFails(__func__, ""); }

void test_unterminatedString() {
  assertParseFails(__func__, R"("unterminated)");
}

void test_unterminatedObject() { assertParseFails(__func__, R"({"key": 1)"); }

void test_unterminatedArray() { assertParseFails(__func__, R"([true, false)"); }

void test_missingFieldDelimiter() {
  assertParseFails(__func__, R"({"key" 1})");
}

void test_invalidTokenOrdering() { assertParseFails(__func__, R"([1 false])"); }

void test_startEndElementsMixedAtRoot() {
  assertParseFails(__func__, R"([}])");
}

void test_startEndElementsMixedInObject() {
  assertParseFails(__func__, R"({])");
}

void test_closeBracketWhereObjectValueShouldBe() {
  assertParseFails(__func__, R"({"a": ]})");
}

void test_closeBraceWhereArrayValueShouldBe() {
  assertParseFails(__func__, R"([1, }])");
}

void test_nestedStartEndChaos() {
  assertParseFails(__func__, R"([{"a":[{]}])");
}

void test_tooDeepArrayNesting() {
  std::string prefix(50000, '[');
  std::string suffix(50000, ']');
  std::string jsonString = prefix + suffix;
  assertParseFails(__func__, jsonString);
}

int main(int argc, char* argv[]) {
  std::vector<std::string> args(argv + 1, argv + argc);

  for (int i = 0; i < g_numTests; i++) {
    std::string testName = g_testNames[i];
    TestFunc testFunc = g_testFunctions[i];

    auto testNameContains = [&testName](const auto& arg) {
      return testName.find(arg) != std::string::npos;
    };

    if (args.empty() || std::find_if(args.begin(), args.end(),
                                     testNameContains) != args.end()) {
      testFunc();
    }
  }

  return 0;
}

#include "MalformedJson_test.cpp.hpp"
