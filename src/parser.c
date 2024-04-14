
#include "tokenizer.h"

int get_precedence(const Token *token) {
	enum TokenType t = token->type;
	if (t > __multiplicative_start & t < __multiplicative_end) return 0;
	if (t > __additive_start & t < __additive_end) return 1;
	if (t > __assign_start & t < __assign_end) return 2;
	if (t > __shift_start & t < __shift_end) return 3;

	return -1;
}
