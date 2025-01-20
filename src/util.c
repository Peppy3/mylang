#include <stddef.h>
#include <stdint.h>

#include <stdio.h>

#include "Token.h"
#include "ParserFile.h"

void print_token(FILE *stream, ParserFile *src, Token *token) {
	fprintf(stream, "%s: '", Token_GetStringRep(token->type));
	fwrite(&src->data[token->pos], sizeof(char), token->len, stream);
	fprintf(stream, "'\n");
}

