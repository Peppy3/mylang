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

#ifdef MEMORY_CHECK

void *check_malloc(size_t size, const char *file, int line) {
	void *ptr = malloc(size);
	fprintf(stderr, "%s:%d %p malloc(%lu)\n", file, line, ptr, size);
	return ptr;
}

void *check_realloc(void *ptr, size_t size, const char *file, int line) {
	void *new = realloc(ptr, size);
	fprintf(stderr, "%s:%d %p realloc(%p, %lu)\n", file, line, new, ptr, size);
	return new;
}

void check_free(void *ptr, const char *file, int line) {
	fprintf(stderr, "%s:%d free(%p)\n", file, line, ptr);
	free(ptr);
}

#endif /* MEMORY_CHECK */

