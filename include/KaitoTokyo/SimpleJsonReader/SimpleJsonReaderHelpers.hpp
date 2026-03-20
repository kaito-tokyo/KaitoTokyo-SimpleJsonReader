// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: Apache-2.0

/**
 * @file    KaitoTokyo/SimpleJsonReader/SimpleJsonReaderHelpers.hpp
 * @brief   Recursive Descent JSON parser for C++ with SAX-like interface.
 * @version 2026.03.21
 * @author  Kaito Udagawa <umireon@kaito.tokyo>
 * @see     https://github.com/kaito-tokyo/KaitoTokyo-SimpleJsonReader/
 */

#pragma once

#include "SimpleJsonReader.hpp"

namespace KaitoTokyo::SimpleJsonReader {

bool isStartObject(EventType type) {
  return type == EventType::StartObject;
}

bool isEndObject(EventType type) {
  return type == EventType::EndObject;
}

bool isStartArray(EventType type) {
  return type == EventType::StartArray;
}

bool isEndArray(EventType type) {
  return type == EventType::EndArray;
}

bool isKey(EventType type) {
  return type == EventType::Key;
}

bool isString(EventType type) {
  return type == EventType::String;
}

bool isNumber(EventType type) {
  return type == EventType::Number;
}

bool isTrue(EventType type) {
  return type == EventType::True;
}

bool isFalse(EventType type) {
  return type == EventType::False;
}

bool isNull(EventType type) {
  return type == EventType::Null;
}

}  // namespace KaitoTokyo::SimpleJsonReader
