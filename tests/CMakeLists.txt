# SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
#
# SPDX-License-Identifier: Apache-2.0

add_executable(ValidJson_test ValidJson_test.cpp)
target_link_libraries(ValidJson_test PRIVATE KaitoTokyo::SimpleJsonReader)
add_test(NAME emptyObject COMMAND ValidJson_test emptyObject)
