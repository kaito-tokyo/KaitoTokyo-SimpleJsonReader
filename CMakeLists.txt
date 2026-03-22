# SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21)

project(KaitoTokyoSimpleJsonReader VERSION 1.0.0 LANGUAGES CXX)

option(
    KaitoTokyoSimpleJsonReader_ENABLE_TESTS
    "Enable tests for KaitoTokyoSimpleJsonReader"
    ${PROJECT_IS_TOP_LEVEL}
)

add_library(SimpleJsonReader INTERFACE)
add_library(KaitoTokyo::SimpleJsonReader ALIAS SimpleJsonReader)

target_include_directories(
    SimpleJsonReader
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/include
        $<INSTALL_INTERFACE:include>
)

install(
    TARGETS SimpleJsonReader
    EXPORT KaitoTokyoSimpleJsonReaderTargets
    INCLUDES DESTINATION include
)

install(
    FILES KaitoTokyo/SimpleJsonReader/SimpleJsonReader.hpp
    DESTINATION include/KaitoTokyo/SimpleJsonReader
)

if(KaitoTokyoSimpleJsonReader_ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
