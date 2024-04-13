#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stddef.h>

enum TokenType {
	INVALID,
	_EOF,

	SPACE, // any kind of whitespace
	COMMENT,

	COLON,
	SEMICOLON,

	LPAREN,
	LCURLY,
	RPAREN,
	RCURLY,

	IDENTIFIER,

	INT_LITERAL,
	STRING_LITERAL,
};

typedef struct Token {
	enum TokenType type;
	char *str;
	size_t len;
	size_t pos;
} Token;

typedef struct Filebuff {
	size_t pos;
	size_t size;
	char file[];
} Filebuff;

Filebuff *tokenizer_init(const char *filename);

void tokenizer_free(Filebuff *fb);

Token next_token(Filebuff *fb);

#endif /* TOKENIZER_H */
