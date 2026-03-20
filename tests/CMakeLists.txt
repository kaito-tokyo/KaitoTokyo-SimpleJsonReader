# SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
#
# SPDX-License-Identifier: Apache-2.0

add_executable(ValidJson_test ValidJson_test.cpp)
target_link_libraries(ValidJson_test PRIVATE KaitoTokyo::SimpleJsonReader)
add_test(NAME emptyObject COMMAND ValidJson_test emptyObject)
add_test(NAME nestedStringsAndJsonPaths COMMAND ValidJson_test nestedStringsAndJsonPaths)
add_test(NAME variousLanguages COMMAND ValidJson_test variousLanguages)
add_test(NAME rootArray COMMAND ValidJson_test rootArray)
add_test(NAME rootString COMMAND ValidJson_test rootString)
add_test(NAME rootNumber COMMAND ValidJson_test rootNumber)
add_test(NAME rootTrue COMMAND ValidJson_test rootTrue)
add_test(NAME rootFalse COMMAND ValidJson_test rootFalse)
add_test(NAME rootNull COMMAND ValidJson_test rootNull)
add_test(NAME whitespaceAroundTokens COMMAND ValidJson_test whitespaceAroundTokens)
add_test(NAME quotesInStrings COMMAND ValidJson_test quotesInStrings)

add_executable(MalformedJson_test MalformedJson_test.cpp)
target_link_libraries(MalformedJson_test PRIVATE KaitoTokyo::SimpleJsonReader)
add_test(NAME emptyInput COMMAND MalformedJson_test emptyInput)
