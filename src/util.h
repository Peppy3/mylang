#ifndef UTIL_H_
#define UTIL_H_

#include <stdio.h>
#include <stdlib.h>

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

#endif /* UTIL_H_ */
