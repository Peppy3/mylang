#include <stddef.h>
#include <stdint.h>

#include "Token.h"

int32_t Token_get_precedence(const Token tok) {
	
	switch (tok.type) {
		case TOKEN_bool_or:
			return 1;
		case TOKEN_bool_and:
			return 2;
		case TOKEN_eq: case TOKEN_neq: case TOKEN_lt:
		case TOKEN_gt: case TOKEN_lte: case TOKEN_gte:
			return 3;
		case TOKEN_add: case TOKEN_sub: 
		case TOKEN_or: case TOKEN_xor:
			return 4;
		case TOKEN_asterisk: case TOKEN_div: case TOKEN_mod:
		case TOKEN_shl: case TOKEN_shr: case TOKEN_ampersand:
			return 5;
		default:
			return 0;
	}
}

#define X(name) #name,
static const char *TokenStrings[] = {
#include "TokenMacro.h"
	NULL
};
#undef X

const char *Token_GetStringRep(enum TokenType type) {
	if (type >= TokenTypeCount) {
		return "[Token type out of range]";
	}
	return TokenStrings[type];
}

