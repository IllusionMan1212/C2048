#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "core.h"

void _core_assert_failed(const char* cond, const char* file, int line, const char* message, ...) {
  fprintf(stderr, "assertion failed: %s\nmessage: ", cond);

  va_list va_args;
  va_start(va_args, message);
  vfprintf(stderr, message, va_args);
  va_end(va_args);

  fprintf(stderr, "\nfile: %s:%u\n", file, line);
  /* char* stacktrace = core_get_stacktrace(); */
  /* fprintf(stderr, "stacktrace:\n%s\n\n", stacktrace); */
  abort();
}

_Noreturn uintptr_t _core_abort(const char* file, int line, const char* message, ...) {
  fprintf(stderr, "abort: ");

  va_list va_args;
  va_start(va_args, message);
  vfprintf(stderr, message, va_args);
  va_end(va_args);

  fprintf(stderr, "\nfile: %s:%u\n", file, line);
  /* char* stacktrace = core_get_stacktrace(); */
  /* fprintf(stderr, "stacktrace:\n%s\n\n", stacktrace); */
  abort();
}


///////////////////////////
//
//
// Hash
//
//
///////////////////////////

u32 core_fnv_hash32(const void* bytes, uptr size, u32 hash) {
	const u8* b = bytes;
	for (uptr idx = 0; idx < size; idx++) {
		hash ^= b[idx];
		hash *= CORE_FNV_HASH32_PRIME;
	}

	return hash;
}

u64 core_fnv_hash64(const void* bytes, uptr size, u64 hash) {
	const u8* b = bytes;
	for(uptr idx = 0; idx < size; idx++) {
		hash ^= b[idx];
		hash *= CORE_FNV_HASH64_PRIME;
	}

	return hash;
}

