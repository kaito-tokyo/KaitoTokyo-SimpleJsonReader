// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: CC0-1.0

#pragma once

const char *g_testNames[] = {
  "isStartObject",
  "isEndObject",
  "isStartArray",
  "isEndArray",
  "isKey",
  "isString",
  "isNumber",
  "isTrue",
  "isFalse",
  "isNull",
  "matchesPathPrefix_emptyPrefixAlwaysMatchEmptyPath",
  "matchesPathPrefix_emptyPrefixAlwaysMatchOneComponentPath",
  "matchesPathPrefix_emptyPrefixAlwaysMatchTwoComponentsPath",
  "matchesPathPrefix_stringPrefixCanMatchStringPath",
  "matchesPathPrefix_indexPrefixCanMatchIndexPath",
  "matchesPathPrefix_indexPrefixNeverMatchStringPath",
  "matchesPathPrefix_stringPrefixNeverMatchIndexPath",
  "matchesPathPrefix_stringAndIndexPrefixCanMatchStringAndIndexPath",
  "matchesPathPrefix_indexAndStringPrefixCanMatchIndexAndStringPath",
  "matchesPathPrefix_OneComponentPrefixNeverMatchEmptyPath",
  "matchesPathPrefix_TwoComponentsPrefixNeverMatchOneComponentsPath",
  "matchesPathPrefix_initializerList",
  "matchesExactPath_neverMatchIfLengthMismatch",
  "matchesExactPath_matchesWhenLengthAndComponentsMatch",
  "matchesExactPath_notMatchWhenLengthMatchesButComponentsNotMatch",
  "matchesExactPath_initializerList",
  "forEachPathComponent",
};

const TestFunc g_testFunctions[] = {
  test_isStartObject,
  test_isEndObject,
  test_isStartArray,
  test_isEndArray,
  test_isKey,
  test_isString,
  test_isNumber,
  test_isTrue,
  test_isFalse,
  test_isNull,
  test_matchesPathPrefix_emptyPrefixAlwaysMatchEmptyPath,
  test_matchesPathPrefix_emptyPrefixAlwaysMatchOneComponentPath,
  test_matchesPathPrefix_emptyPrefixAlwaysMatchTwoComponentsPath,
  test_matchesPathPrefix_stringPrefixCanMatchStringPath,
  test_matchesPathPrefix_indexPrefixCanMatchIndexPath,
  test_matchesPathPrefix_indexPrefixNeverMatchStringPath,
  test_matchesPathPrefix_stringPrefixNeverMatchIndexPath,
  test_matchesPathPrefix_stringAndIndexPrefixCanMatchStringAndIndexPath,
  test_matchesPathPrefix_indexAndStringPrefixCanMatchIndexAndStringPath,
  test_matchesPathPrefix_OneComponentPrefixNeverMatchEmptyPath,
  test_matchesPathPrefix_TwoComponentsPrefixNeverMatchOneComponentsPath,
  test_matchesPathPrefix_initializerList,
  test_matchesExactPath_neverMatchIfLengthMismatch,
  test_matchesExactPath_matchesWhenLengthAndComponentsMatch,
  test_matchesExactPath_notMatchWhenLengthMatchesButComponentsNotMatch,
  test_matchesExactPath_initializerList,
  test_forEachPathComponent,
};

const int g_numTests = 27;
