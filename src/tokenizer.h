#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stddef.h>

enum TokenType {
	INVALID,
	_EOF,

	SPACE, // any kind of whitespace
	COMMENT,

	__assign_start,
	ASSIGN,
	ADD_ASSIGN,
	SUB_ASSIGN,
	MUL_ASSIGN,
	DIV_ASSIGN,
	REM_ASSIGN,
	SHL_ASSIGN,
	SHR_ASSIGN,
	__assign_end,

	__additive_start,
	ADDITION,
	SUBTRACTION,
	__additive_end,

	__multiplicative_start,
	MULTIPLICATION,
	DIVITION,
	REMINDER,
	__multiplicative_end,
	
	__shift_start,
	SHIFT_LEFT,
	SHIFT_RIGHT,
	__shift_end,

	EQUAL,
	LESS,
	GREATER,
	LESS_EQ,
	GREATER_EQ,

	PERIOD,
	COMMA,

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
