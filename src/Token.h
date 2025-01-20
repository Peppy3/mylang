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

typedef struct {
	struct {
		enum TokenType type: 8;
		uint32_t len: 24;
	};
	uint32_t pos;
} Token;

static inline bool Token_is_type_specifier(const Token tok) {
	return TOKEN_TYPE_START < tok.type && tok.type < TOKEN_TYPE_END;
}

static inline bool Token_is_literal(const Token tok) {
	return TOKEN_LITERAL_START < tok.type && tok.type < TOKEN_LITERAL_END;
}

const char *Token_GetStringRep(enum TokenType type);

#endif /* TOKEN_H_*/

