#ifndef TOKEN_H_
#define TOKEN_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define X(name) TOKEN_##name,
enum TokenType {
#include "TokenMacro.h"
	TokenTypeCount
};
#undef X

typedef int32_t TokenPos;

typedef struct {
	struct {
		enum TokenType type: 8;
		uint32_t len: 24;
	};
	TokenPos pos;
} Token;

static inline bool Token_is_type_specifier(const Token tok) {
	return TOKEN_TYPE_START < tok.type && tok.type < TOKEN_TYPE_END;
}

static inline bool Token_is_literal(const Token tok) {
	return TOKEN_LITERAL_START < tok.type && tok.type < TOKEN_LITERAL_END;
}

static inline bool Token_is_assignment(const Token tok) {
	return TOKEN_ASSIGNMENT_START < tok.type && tok.type < TOKEN_ASSIGNMENT_END;
}

static inline bool Token_is_terminating(const Token tok) {
	return tok.type == TOKEN_comma
		|| tok.type == TOKEN_colon
		|| tok.type == TOKEN_semicolon
		|| tok.type == TOKEN_rcurly
		|| tok.type == TOKEN_lsquare
		|| tok.type == TOKEN_eof
		;
}

static inline bool Token_is_unary(const Token tok) {
	return tok.type == TOKEN_inc
		|| tok.type == TOKEN_dec
		|| tok.type == TOKEN_sub
		|| tok.type == TOKEN_asterisk
		|| tok.type == TOKEN_not
		|| tok.type == TOKEN_bool_not
		|| tok.type == TOKEN_ampersand
		;
}

#define TOKEN_MIN_PRECEDENCE (0)
int32_t Token_get_precedence(const Token tok);

const char *Token_GetStringRep(enum TokenType type);

#endif /* TOKEN_H_*/

