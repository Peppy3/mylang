#include <stddef.h>

#include "token.h"

#if __STDC_VERSION__ > 201710L
# define FALLTHOUGH [[fallthough]]
#else
# define FALLTHOUGH
#endif

#define X(name) #name,
static char *TokenStrings[] = {
TOKEN_LIST_MACRO
	NULL
};
#undef X

char *TokenGetStringRep(struct Token *token) {
	enum TokenType type = token->type;
	if (type >= TokenTypeCount) {
		return "Token type out of range";
	}
	return TokenStrings[type];
}

int TokenGetPrecedence(struct Token *token) {
	int result = 0;

	switch (token->type) {
		case INCREMENT: FALLTHOUGH
		case DECREMENT:
			result = 1; break;

		case LOGICAL_NOT: FALLTHOUGH
		case BOOLEAN_NOT:
			result = 2; break;

		case ASTERISK: FALLTHOUGH
		case DIVITION: FALLTHOUGH
		case MODULUS:
			result = 3; break;

		case ADDITION: FALLTHOUGH
		case SUBTRACTION: 
			result = 4; break;

		case SHIFT_LEFT: FALLTHOUGH
		case SHIFT_RIGHT:
			result = 5; break;

		case EQUALS: FALLTHOUGH
		case NOT_EQUALS:
			result = 6; break;

		case LOGICAL_AND:
			result = 7; break;

		case EXCLUSIVE_OR:
			result = 8; break;

		case LOGICAL_OR:
			result = 9; break;

		case BOOLEAN_AND:
			result = 10; break;

		case BOOLEAN_OR:
			result = 11; break;
		
		case LESS_THAN: FALLTHOUGH
		case LESS_THAN_EQUALS: FALLTHOUGH
		case GREATER_THAN: FALLTHOUGH
		case GREATER_THAN_EQUALS:
			result = 12; break;

		default: 
			result = -1; break;
	}

	return result;
}

