#pragma once

#include <cstddef>
#include <type_traits>

#include "assert.hpp"

namespace libk {
size_t strlen(const char* text);
char* strcpy(char* dst, const char* src);

void* memchr(const void* ptr, int value, size_t length);
int memcmp(const void* lhs, const void* rhs, size_t length);
void* memset(void* dst, int value, size_t length);
void* memcpy(void* dst, const void* src, size_t length);
void* memmove(void* dst, const void* src, size_t length);
}  // namespace libk
