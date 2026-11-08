// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: Apache-2.0

#include <KaitoTokyo/SimpleJsonReader/SimpleJsonReader.hpp>
#include <KaitoTokyo/SimpleJsonReader/SimpleJsonReaderHelpers.hpp>
#include <algorithm>
#include <array>
#include <cstring>
#include <stdexcept>
#include <vector>

#include "test_helper.hpp"

#ifdef USE_STD_STRING_VIEW
using string_view = std::string_view;
#else
using string_view = KaitoTokyo::SimpleJsonReader::C8StringView;
#endif

using namespace KaitoTokyo::SimpleJsonReader;
using KaitoTokyo::SimpleJsonReader::EventType;

using JsonPath_t = JsonPath<string_view>;
using Event_t = Event<string_view>;
using Component_t = JsonPathComponent<string_view>;

namespace
{

string_view sv(const char *str)
{
    return string_view{str, std::strlen(str)};
}

Component_t key(const char *str)
{
    return Component_t{sv(str)};
}

Component_t idx(std::size_t i)
{
    return Component_t{i};
}

void expect(bool condition, const char *message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

template <typename Predicate> void testPredicate(Predicate predicate, EventType matchingType)
{
    Event_t event{sv(""), nullptr, matchingType};
    expect(predicate(event), "Expected the predicate to return true");

    Event_t undefinedEvent{sv(""), nullptr, EventType::Undefined};
    expect(!predicate(undefinedEvent), "Expected the predicate to return false for undefinedEvent");
}

} // namespace

// MARK: - Test Cases

void test_isStartObject()
{
    printPreamble(__func__);
    testPredicate([](const Event_t &e) { return isStartObject(e); }, EventType::StartObject);
    printPostamble(__func__);
}

void test_isEndObject()
{
    printPreamble(__func__);
    testPredicate([](const Event_t &e) { return isEndObject(e); }, EventType::EndObject);
    printPostamble(__func__);
}

void test_isStartArray()
{
    printPreamble(__func__);
    testPredicate([](const Event_t &e) { return isStartArray(e); }, EventType::StartArray);
    printPostamble(__func__);
}

void test_isEndArray()
{
    printPreamble(__func__);
    testPredicate([](const Event_t &e) { return isEndArray(e); }, EventType::EndArray);
    printPostamble(__func__);
}

void test_isKey()
{
    printPreamble(__func__);
    testPredicate([](const Event_t &e) { return isKey(e); }, EventType::Key);
    printPostamble(__func__);
}

void test_isString()
{
    printPreamble(__func__);
    testPredicate([](const Event_t &e) { return isString(e); }, EventType::String);
    printPostamble(__func__);
}

void test_isNumber()
{
    printPreamble(__func__);
    testPredicate([](const Event_t &e) { return isNumber(e); }, EventType::Number);
    printPostamble(__func__);
}

void test_isTrue()
{
    printPreamble(__func__);
    testPredicate([](const Event_t &e) { return isTrue(e); }, EventType::True);
    printPostamble(__func__);
}

void test_isFalse()
{
    printPreamble(__func__);
    testPredicate([](const Event_t &e) { return isFalse(e); }, EventType::False);
    printPostamble(__func__);
}

void test_isNull()
{
    printPreamble(__func__);
    testPredicate([](const Event_t &e) { return isNull(e); }, EventType::Null);
    printPostamble(__func__);
}

void test_matchesPathPrefix_emptyPrefixAlwaysMatchEmptyPath()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    Event_t event{sv(""), &root, EventType::String};

    expect(matchesPathPrefix(event, nullptr, 0), "Expected an empty prefix to match an empty path");

    printPostamble(__func__);
}

void test_matchesPathPrefix_emptyPrefixAlwaysMatchOneComponentPath()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child{&root, sv("child"), 1};
    Event_t event{sv(""), &child, EventType::String};

    expect(matchesPathPrefix(event, nullptr, 0), "Expected an empty prefix to match a one-component path");

    printPostamble(__func__);
}

void test_matchesPathPrefix_emptyPrefixAlwaysMatchTwoComponentsPath()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child1{&root, sv("child1"), 1};
    JsonPath_t child2{&child1, sv("child2"), 2};
    Event_t event{sv(""), &child2, EventType::String};

    expect(matchesPathPrefix(event, nullptr, 0), "Expected an empty prefix to match a two-component path");

    printPostamble(__func__);
}

void test_matchesPathPrefix_stringPrefixCanMatchStringPath()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child{&root, sv("child"), 1};
    Event_t event{sv(""), &child, EventType::String};

    std::array<Component_t, 1> prefix{key("child")};
    expect(matchesPathPrefix(event, prefix.data(), 1), "Expected a string prefix to match a string path");

    printPostamble(__func__);
}

void test_matchesPathPrefix_stringPrefixNeverMatchDifferentString()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child{&root, sv("child"), 1};
    Event_t event{sv(""), &child, EventType::String};

    std::array<Component_t, 1> prefix{key("chile")};
    expect(!matchesPathPrefix(event, prefix.data(), 1),
           "Expected a string prefix to not match a different string path");

    printPostamble(__func__);
}

void test_matchesPathPrefix_indexPrefixCanMatchIndexPath()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child{&root, std::size_t{0}, 1};
    Event_t event{sv(""), &child, EventType::String};

    std::array<Component_t, 1> prefix{idx(0)};
    expect(matchesPathPrefix(event, prefix.data(), 1), "Expected an index prefix to match an index path");

    printPostamble(__func__);
}

void test_matchesPathPrefix_indexPrefixNeverMatchStringPath()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child{&root, sv("child"), 1};
    Event_t event{sv(""), &child, EventType::String};

    std::array<Component_t, 1> prefix{idx(0)};
    expect(!matchesPathPrefix(event, prefix.data(), 1), "Expected an index prefix to not match a string path");

    printPostamble(__func__);
}

void test_matchesPathPrefix_stringPrefixNeverMatchIndexPath()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child{&root, std::size_t{0}, 1};
    Event_t event{sv(""), &child, EventType::String};

    std::array<Component_t, 1> prefix{key("child")};
    expect(!matchesPathPrefix(event, prefix.data(), 1), "Expected a string prefix to not match an index path");

    printPostamble(__func__);
}

void test_matchesPathPrefix_stringAndIndexPrefixCanMatchStringAndIndexPath()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child1{&root, sv("child1"), 1};
    JsonPath_t child2{&child1, std::size_t{0}, 2};
    Event_t event{sv(""), &child2, EventType::String};

    std::array<Component_t, 2> prefix{key("child1"), idx(0)};
    expect(matchesPathPrefix(event, prefix.data(), 2), "Expected a string/index prefix to match a string/index path");

    printPostamble(__func__);
}

void test_matchesPathPrefix_indexAndStringPrefixCanMatchIndexAndStringPath()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child1{&root, std::size_t{0}, 1};
    JsonPath_t child2{&child1, sv("child2"), 2};
    Event_t event{sv(""), &child2, EventType::String};

    std::array<Component_t, 2> prefix{idx(0), key("child2")};
    expect(matchesPathPrefix(event, prefix.data(), 2), "Expected an index/string prefix to match an index/string path");

    printPostamble(__func__);
}

void test_matchesPathPrefix_shorterPrefixMatchesLongerPath()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child1{&root, sv("child1"), 1};
    JsonPath_t child2{&child1, std::size_t{0}, 2};
    Event_t event{sv(""), &child2, EventType::String};

    std::array<Component_t, 1> prefix{key("child1")};
    expect(matchesPathPrefix(event, prefix.data(), 1), "Expected a one-component prefix to match a two-component path");

    printPostamble(__func__);
}

void test_matchesPathPrefix_oneComponentPrefixNeverMatchEmptyPath()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    Event_t event{sv(""), &root, EventType::String};

    std::array<Component_t, 1> prefix{key("child")};
    expect(!matchesPathPrefix(event, prefix.data(), 1), "Expected a one-component prefix to not match an empty path");

    printPostamble(__func__);
}

void test_matchesPathPrefix_twoComponentsPrefixNeverMatchOneComponentPath()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child{&root, sv("child"), 1};
    Event_t event{sv(""), &child, EventType::String};

    std::array<Component_t, 2> prefix{key("child"), key("other")};
    expect(!matchesPathPrefix(event, prefix.data(), 2),
           "Expected a two-component prefix to not match a one-component path");

    printPostamble(__func__);
}

void test_matchesPathPrefix_initializerList()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child1{&root, sv("child1"), 1};
    JsonPath_t child2{&child1, std::size_t{0}, 2};
    Event_t event{sv(""), &child2, EventType::String};

    expect(matchesPathPrefix(event, {key("child1"), idx(0)}),
           "Expected the initializer list overload of matchesPathPrefix to work");
    expect(matchesPathPrefix(event, {sv("child1"), std::size_t{0}}),
           "Expected the initializer list overload of matchesPathPrefix to accept convertible values");

    printPostamble(__func__);
}

void test_matchesPathPrefix_throwsOnInvalidArguments()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child{&root, sv("child"), 1};
    Event_t event{sv(""), &child, EventType::String};
    Event_t nullPathEvent{sv(""), nullptr, EventType::String};
    std::array<Component_t, 1> prefix{key("child")};

    auto throwsInvalidArgument = [](auto func) {
        try
        {
            func();
        }
        catch (const std::invalid_argument &)
        {
            return true;
        }
        return false;
    };

    expect(throwsInvalidArgument([&] { matchesPathPrefix(nullPathEvent, nullptr, 0); }),
           "Expected a null jsonPath to throw");
    expect(throwsInvalidArgument([&] { matchesPathPrefix(event, nullptr, 1); }),
           "Expected a null prefix with a positive length to throw");
    expect(throwsInvalidArgument([&] { matchesPathPrefix(event, prefix.data(), -1); }),
           "Expected a negative prefix length to throw");

    printPostamble(__func__);
}

void test_matchesExactPath_neverMatchIfLengthMismatch()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child{&root, sv("child"), 1};
    Event_t event{sv(""), &child, EventType::String};

    std::array<Component_t, 2> path{key("child"), key("grandchild")};
    expect(!matchesExactPath(event, path.data(), 2),
           "Expected matchesExactPath to return false when the length is longer");

    JsonPath_t grandchild{&child, sv("grandchild"), 2};
    Event_t deeperEvent{sv(""), &grandchild, EventType::String};
    expect(!matchesExactPath(deeperEvent, path.data(), 1),
           "Expected matchesExactPath to return false when the length is shorter");

    printPostamble(__func__);
}

void test_matchesExactPath_matchesWhenLengthAndComponentsMatch()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child1{&root, sv("child1"), 1};
    JsonPath_t child2{&child1, std::size_t{0}, 2};
    Event_t event{sv(""), &child2, EventType::String};

    std::array<Component_t, 2> path{key("child1"), idx(0)};
    expect(matchesExactPath(event, path.data(), 2),
           "Expected matchesExactPath to return true when length and components "
           "match");

    printPostamble(__func__);
}

void test_matchesExactPath_notMatchWhenLengthMatchesButComponentsNotMatch()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child1{&root, sv("child1"), 1};
    JsonPath_t child2{&child1, std::size_t{0}, 2};
    Event_t event{sv(""), &child2, EventType::String};

    std::array<Component_t, 2> path{key("child1"), idx(1)};
    expect(!matchesExactPath(event, path.data(), 2), "Expected matchesExactPath to return false when the components "
                                                     "mismatch");

    printPostamble(__func__);
}

void test_matchesExactPath_emptyPathMatchesOnlyRoot()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child{&root, sv("child"), 1};
    Event_t rootEvent{sv(""), &root, EventType::String};
    Event_t childEvent{sv(""), &child, EventType::String};

    expect(matchesExactPath(rootEvent, nullptr, 0), "Expected an empty path to match the root");
    expect(!matchesExactPath(childEvent, nullptr, 0), "Expected an empty path to not match a child");

    printPostamble(__func__);
}

void test_matchesExactPath_initializerList()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child1{&root, sv("child1"), 1};
    JsonPath_t child2{&child1, std::size_t{0}, 2};
    Event_t event{sv(""), &child2, EventType::String};

    expect(matchesExactPath(event, {key("child1"), idx(0)}),
           "Expected the initializer list overload of matchesExactPath to work");
    expect(matchesExactPath(event, {sv("child1"), std::size_t{0}}),
           "Expected the initializer list overload of matchesExactPath to accept convertible values");

    printPostamble(__func__);
}

void test_forEachPathComponent()
{
    printPreamble(__func__);

    JsonPath_t root{nullptr, {}, 0};
    JsonPath_t child1{&root, sv("child1"), 1};
    JsonPath_t child2{&child1, std::size_t{2}, 2};
    JsonPath_t child3{&child2, sv("child3"), 3};
    Event_t event{sv(""), &child3, EventType::String};

    std::vector<std::string> components;
    forEachPathComponent(event, [&components](const Component_t &component) {
        if (const auto *k = std::get_if<string_view>(&component))
        {
            components.emplace_back(k->data(), k->size());
        }
        else
        {
            components.push_back("#" + std::to_string(std::get<std::size_t>(component)));
        }
    });

    std::vector<std::string> expected{"child1", "#2", "child3"};
    expect(components == expected, "Expected forEachPathComponent to visit path components from root to "
                                   "leaf");

    printPostamble(__func__);
}

// MARK: - Test Runner

int main(int argc, char *argv[])
{
    std::vector<std::string> args(argv + 1, argv + argc);

    for (int i = 0; i < g_numTests; i++)
    {
        std::string testName = g_testNames[i];
        TestFunc testFunc = g_testFunctions[i];

        auto testNameContains = [&testName](const auto &arg) { return testName.find(arg) != std::string::npos; };

        if (args.empty() || std::find_if(args.begin(), args.end(), testNameContains) != args.end())
        {
            testFunc();
        }
    }

    return 0;
}

#include "ReaderHelper_test.cpp.hpp"
