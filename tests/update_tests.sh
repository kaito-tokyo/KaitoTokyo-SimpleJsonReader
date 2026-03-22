#!/bin/bash

set -euo pipefail

testSources=(*_test.cpp)
for file in "${testSources[@]}"; do
  testFuncs=($(awk -F'[ ()]' '/void test_.*/ { print $2 }' "$file"))

  (
    echo '// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>'
    echo '//'
    echo '// SPDX-License-Identifier: CC0-1.0'
    echo
    echo '#pragma once'
    echo
    echo 'const char *g_testNames[] = {'
    for func in "${testFuncs[@]}"; do
      echo "  \"${func#test_}\","
    done
    echo '};'
    echo
    echo 'const TestFunc g_testFunctions[] = {'
    for func in "${testFuncs[@]}"; do
      echo "  $func,"
    done
    echo '};'
    echo
    echo "const int g_numTests = ${#testFuncs[@]};"
  ) > "$file.hpp"
done


