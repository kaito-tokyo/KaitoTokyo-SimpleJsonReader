// SPDX-FileCopyrightText: 2026 Kaito Udagawa <umireon@kaito.tokyo>
//
// SPDX-License-Identifier: Apache-2.0

/**
 * @file    KaitoTokyo/SimpleJsonReader/SimpleJsonReaderHelpers.hpp
 * @brief   Recursive Descent JSON parser for C++ with SAX-like interface.
 * @author  Kaito Udagawa <umireon@kaito.tokyo>
 * @version 1.0.0
 * @date    2026-03-22
 * @see     https://github.com/kaito-tokyo/KaitoTokyo-SimpleJsonReader/
 */

#pragma once

#include <stdexcept>

#include "SimpleJsonReader.hpp"

namespace KaitoTokyo::SimpleJsonReader {

inline bool isStartObject(const Event& event) {
  return event.type == EventType::StartObject;
}

inline bool isEndObject(const Event& event) {
  return event.type == EventType::EndObject;
}@

inline bool isStartArray(const Event& event) {
  return event.type == EventType::StartArray;
}

inline bool isEndArray(const Event& event) {
  return event.type == EventType::EndArray;
}

inline bool isKey(const Event& event) { return event.type == EventType::Key; }

inline bool isString(const Event& event) {
  return event.type == EventType::String;
}

inline bool isNumber(const Event& event) {
  return event.type == EventType::Number;
}

inline bool isTrue(const Event& event) { return event.type == EventType::True; }

inline bool isFalse(const Event& event) {
  return event.type == EventType::False;
}

inline bool isNull(const Event& event) { return event.type == EventType::Null; }

inline bool matchesPathPrefix(const Event& event,
                              const JsonPathComponent* prefix,
                              std::int32_t prefixLength) {
  if (event.jsonPath == nullptr) {
    throw std::invalid_argument("Event's jsonPath cannot be null");
  } else if (prefixLength == 0) {
    return true;
  } else if (prefix == nullptr) {
    throw std::invalid_argument("Prefix cannot be null");
  }

  if (event.jsonPath == nullptr || event.jsonPath->depth < prefixLength) {
    return false;
  }

  JsonPath* current = event.jsonPath;

  while (current != nullptr && current->depth > prefixLength) {
    current = current->parent;
  }

  while (current != nullptr && current->parent != nullptr) {
    if (current->component != prefix[current->depth - 1]) {
      return false;
    }
    current = current->parent;
  }

  return current != nullptr && current->parent == nullptr;
}

inline bool matchesPathPrefix(const Event& event,
                              std::initializer_list<JsonPathComponent> prefix) {
  return matchesPathPrefix(event, prefix.begin(),
                           static_cast<std::int32_t>(prefix.size()));
}

inline bool matchesExactPath(const Event& event, const JsonPathComponent* path,
                             std::int32_t pathLength) {
  if (event.jsonPath == nullptr) {
    throw std::invalid_argument("Event's jsonPath cannot be null");
  } else if (pathLength == 0) {
    return event.jsonPath->depth == 0;
  } else if (path == nullptr) {
    throw std::invalid_argument("Path cannot be null");
  }

  if (event.jsonPath->depth == pathLength) {
    return matchesPathPrefix(event, path, pathLength);
  } else {
    return false;
  }
}

inline bool matchesExactPath(const Event& event,
                             std::initializer_list<JsonPathComponent> path) {
  return matchesExactPath(event, path.begin(),
                          static_cast<std::int32_t>(path.size()));
}

template <typename Callback>
inline void forEachPathComponent(const Event& event, Callback callback) {
  if (event.jsonPath == nullptr) {
    throw std::invalid_argument("Event's jsonPath cannot be null");
  }

  auto visit = [&](auto self, JsonPath* jsonPath) -> void {
    if (jsonPath != nullptr && jsonPath->parent != nullptr) {
      self(self, jsonPath->parent);
      callback(jsonPath->component);
    }
  };
  visit(visit, event.jsonPath);
}

}  // namespace KaitoTokyo::SimpleJsonReader
