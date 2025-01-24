#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>

#include "ParserFile.h"
#include "Token.h"

void print_token(FILE *stream, ParserFile *src, Token *token);

__attribute__((noreturn))
void util_assert_impl(const char *expr, 
	const char *file, unsigned int line);

#ifdef NDEBUG
#define util_assert(expr) (__ASSERT_VOID_CAST(0))
#else 
#define util_assert(expr)\
	((expr)\
		? 0\
		: util_assert_impl(#expr, __FILE__, __LINE__)\
		)
#endif

#ifdef MEMORY_CHECK

#include <stdlib.h>

void *check_malloc(size_t size, const char *file, int line);
void *check_realloc(void *ptr, size_t size, const char *file, int line);
void check_free(void *ptr, const char *file, int line);

#define malloc(sz) check_malloc(sz, __FILE__, __LINE__)
#define realloc(ptr, sz) check_realloc(ptr, sz, __FILE__, __LINE__)
#define free(ptr) check_free(ptr, __FILE__, __LINE__)

#endif /* MEMORY_CHECK */

#endif /* UTIL_H_ */
