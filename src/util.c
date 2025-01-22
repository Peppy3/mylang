#include <stddef.h>
#include <stdint.h>

#include <stdio.h>

#include <signal.h>

#include "Token.h"
#include "ParserFile.h"

void print_token(FILE *stream, ParserFile *src, Token *token)
{
	fprintf(stream, "%s: ", Token_GetStringRep(token->type));
	fwrite(&src->data[token->pos], sizeof(char), token->len, stream);
}

__attribute__((noreturn))
void util_assert_impl(const char *expr, const char *file, unsigned int line)
{
	fprintf(stderr, "%s:%d: util_assert(%s)\n",
		file, line, expr
		);

	raise(SIGTRAP);

	exit(1);
}

