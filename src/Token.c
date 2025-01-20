#include <stddef.h>

#include "Token.h"

#if __STDC_VERSION__ > 201710L
# define FALLTHOUGH [[fallthough]]
#else
# define FALLTHOUGH
#endif

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

