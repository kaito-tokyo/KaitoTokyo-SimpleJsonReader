// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: CC0-1.0

#pragma once

const char *g_testNames[] = {
  "emptyObject",
  "nestedStringsAndJsonPaths",
  "variousLanguages",
  "rootArray",
  "rootString",
  "rootNumber",
  "rootTrue",
  "rootFalse",
  "rootNull",
  "whitespaceAroundTokens",
  "quotesInStrings",
  "manyBackslashesBeforeQuote",
};

const TestFunc g_testFunctions[] = {
  test_emptyObject,
  test_nestedStringsAndJsonPaths,
  test_variousLanguages,
  test_rootArray,
  test_rootString,
  test_rootNumber,
  test_rootTrue,
  test_rootFalse,
  test_rootNull,
  test_whitespaceAroundTokens,
  test_quotesInStrings,
  test_manyBackslashesBeforeQuote,
};

const int g_numTests = 12;
