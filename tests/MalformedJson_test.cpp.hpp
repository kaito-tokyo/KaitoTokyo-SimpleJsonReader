// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: CC0-1.0

#pragma once

const char *g_testNames[] = {
  "emptyString",
  "unterminatedString",
  "unterminatedObject",
  "unterminatedArray",
  "missingFieldDelimiter",
  "invalidTokenOrdering",
  "startEndElementsMixedAtRoot",
  "startEndElementsMixedInObject",
  "closeBracketWhereObjectValueShouldBe",
  "closeBraceWhereArrayValueShouldBe",
  "nestedStartEndChaos",
  "tooDeepArrayNesting",
};

const TestFunc g_testFunctions[] = {
  test_emptyString,
  test_unterminatedString,
  test_unterminatedObject,
  test_unterminatedArray,
  test_missingFieldDelimiter,
  test_invalidTokenOrdering,
  test_startEndElementsMixedAtRoot,
  test_startEndElementsMixedInObject,
  test_closeBracketWhereObjectValueShouldBe,
  test_closeBraceWhereArrayValueShouldBe,
  test_nestedStartEndChaos,
  test_tooDeepArrayNesting,
};

const int g_numTests = 12;
