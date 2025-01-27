#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "Token.h"
#include "Keywords.h"

#include "Tokenizer.h"

int lower(int ch) {
	return ch | ' ';
}

bool isLetter(int ch) {
	return ('a' <= lower(ch) && lower(ch) <= 'z') || ch == '_';
}

bool scanEscape(ParserFile *file) {
	int ch = ParserFile_GetCh(file);
	
	switch (ch) {
		case '0': case 'a': case 'b': case 'e': case 'f': case 'n':
		case 'r': case 't': case 'v': case '\\': case '\'': case '"': {
			return true;
		} break;
		case 'x': {
			ch = ParserFile_GetCh(file);
			if (!isxdigit(ch)) {
				return false;
			}
			ch = ParserFile_GetCh(file);
			if (!isxdigit(ch)) {
				return false;
			}
			return true;
		} break;
	}
	return false;
}

void scanMultilineComment(ParserFile *file) {
	int ch;
	size_t nesting = 1;

	while (nesting > 0) {
		ch = ParserFile_GetCh(file);
		if (ch == EOF) {
			return;
		}

		if (ch == '*' && ParserFile_Peek(file) == '/') {
			nesting -= 1;
		}
		else if (ch == '/' && ParserFile_Peek(file) == '*') {
			nesting += 1;
		}
	}

	// capture the final '/'
	ParserFile_GetCh(file);
}

void scanSingleLineComment(ParserFile *file) {
	int ch;
	do {
		ch = ParserFile_GetCh(file);
		if (ch == EOF) {
			return;
		}

	} while (ch != '\n');
}

void scanIdentifier(ParserFile *file) {
	int ch = ParserFile_Peek(file);

	while (isLetter(ch) || isdigit(ch)) {
		(void)ParserFile_GetCh(file);
		ch = ParserFile_Peek(file);
	}
	return;
}

enum TokenType scanNumber(ParserFile *file) {
	char ch = ParserFile_Peek(file);

	// do "file->pos++" because we are sure the char is valid
	if (ch == 'x') {
		file->pos++;

		if (!isxdigit(ParserFile_Peek(file))) {
			return TOKEN_invalid;
		}
		while (isxdigit(ParserFile_Peek(file))) {
			file->pos++;
		}
		return TOKEN_hexadecimal;
	}
	
	while (isdigit(ch)) {
		file->pos++;
		ch = ParserFile_Peek(file);
	}

	if (ch == '.') {
		file->pos++;
		if (!isdigit(ParserFile_Peek(file))) {
			return TOKEN_invalid;
		}
		while (isdigit(ParserFile_Peek(file))) {
			file->pos++;
		}
		if (ParserFile_Peek(file) == 'e') {
			file->pos++;

			if (!isdigit(ParserFile_Peek(file))) {
				return TOKEN_invalid;
			}
			while (isdigit(ParserFile_Peek(file))) {
				file->pos++;
			}
		}
		return TOKEN_float_lit;
	}
	else {
		return TOKEN_integer;
	}

}

enum TokenType scanChar(ParserFile *file) {
	int ch = ParserFile_GetCh(file);

	if (ch == '\\' && !scanEscape(file)) {
		return TOKEN_invalid;
	}
	else if (iscntrl(ch)) {
		return TOKEN_invalid;
	}
	
	// capture the ending '\''
	ch = ParserFile_GetCh(file);
	if (ch != '\'') {
		return TOKEN_invalid;
	}

	return TOKEN_char_lit;
}

enum TokenType scanString(ParserFile *file) {
	int ch = ParserFile_GetCh(file);

	while (ch != '"') {
		if (iscntrl(ch)) {
			return TOKEN_invalid;
		}

		if (ch == '\\' && !scanEscape(file)) {
			return TOKEN_invalid;
		}
		ch = ParserFile_GetCh(file);
	}

	return TOKEN_string;
}


int is_assignment(ParserFile *file) {
	if (ParserFile_Peek(file) == '=') {
		return ParserFile_GetCh(file);
	}
	return 0;
}

// So we can save outselves from typing out boilerplate
#define MAKE_TOKEN(typ) \
(Token){ \
	.type = typ, \
	.pos = begin_pos, \
	.len = file->pos - begin_pos, \
}

// parses the next token
// you might see some file->pos++ in here. but only when we are sure it's a valid char
Token NextToken(ParserFile *file) {
	assert(file != NULL && "NextToken");
	uint32_t begin_pos;
	int ch;

NextToken_continue:
	begin_pos = file->pos;
	ch = ParserFile_GetCh(file);

	if (ch == EOF) {
		return MAKE_TOKEN(TOKEN_eof);
	}
	else if (isspace(ch)) {
		goto NextToken_continue;
	}
	else if (isLetter(ch)) {
		scanIdentifier(file);

		KeywordResult *kw_res = KeywordLookup(&file->data[begin_pos], file->pos - begin_pos);
		if (kw_res != NULL) {
			return MAKE_TOKEN(kw_res->type);
		}
		else {
			return MAKE_TOKEN(TOKEN_identifier);
		}
	}
	else if (isdigit(ch)) {
		enum TokenType type = scanNumber(file);
		return MAKE_TOKEN(type);
	}

	switch (ch) {
		case '(': {
			return MAKE_TOKEN(TOKEN_lparen);
		} break;
		case ')': {
			return MAKE_TOKEN(TOKEN_rparen);
		} break;
		case '{': {
				return MAKE_TOKEN(TOKEN_lcurly);
			} break;
		case '}': {
				return MAKE_TOKEN(TOKEN_rcurly);
			} break;
		case '[': {
				return MAKE_TOKEN(TOKEN_lsquare);
			} break;
		case ']': {
				return MAKE_TOKEN(TOKEN_rsquare);
			} break;

		case '.': {
				if (ParserFile_Peek(file) == '.') {
					file->pos++;
					if (ParserFile_Peek(file) == '.') {
						file->pos++;
						return MAKE_TOKEN(TOKEN_ellipsis);
					}
				}
				else {
					return MAKE_TOKEN(TOKEN_period);
				}
			} break;
		case ':': {
				return MAKE_TOKEN(TOKEN_colon);
			} break;
		case ',': {
			return MAKE_TOKEN(TOKEN_comma);
		} break;
		case ';': {
			return MAKE_TOKEN(TOKEN_semicolon);
		} break;
		case '@': {
			return MAKE_TOKEN(TOKEN_at);
		} break;

		case '+': {
			if (ParserFile_Peek(file) == '+') {
				file->pos++;
				return MAKE_TOKEN(TOKEN_inc);
			}
			if ((ch = is_assignment(file))) {
				return MAKE_TOKEN(TOKEN_assign_add);
			}
			return MAKE_TOKEN(TOKEN_add);
		} break;
		case '-': {
			if (ParserFile_Peek(file) == '-') {
				file->pos++;
				return MAKE_TOKEN(TOKEN_dec);
			}
			if ((ch = is_assignment(file))) {
				return MAKE_TOKEN(TOKEN_assign_sub);
			}
			if (ParserFile_Peek(file) == '>') {
				file->pos++;
				return MAKE_TOKEN(TOKEN_arrow);
			}
			return MAKE_TOKEN(TOKEN_sub);
		} break;
		case '*': {
			if ((ch = is_assignment(file))) {
				return MAKE_TOKEN(TOKEN_assign_mul);
			}
			return MAKE_TOKEN(TOKEN_asterisk);
		} break;
		case '/': {
			if ((ch = is_assignment(file))) {
				return MAKE_TOKEN(TOKEN_assign_div);
			}
			else if (ParserFile_Peek(file) == '/') {
				scanSingleLineComment(file);
				goto NextToken_continue;
			}
			else if (ParserFile_Peek(file) == '*') {
				file->pos++;
				scanMultilineComment(file);
				goto NextToken_continue;
			}
			return MAKE_TOKEN(TOKEN_div);
		} break;
		case '%': {
			if ((ch = is_assignment(file))) {
				return MAKE_TOKEN(TOKEN_assign_mod);
			}
			return MAKE_TOKEN(TOKEN_mod);
		} break;

		case '~': {
			if ((ch = is_assignment(file))) {
				return MAKE_TOKEN(TOKEN_assign_not);
			}
			return MAKE_TOKEN(TOKEN_not);
		} break;
		case '^': {
			if ((ch = is_assignment(file))) {
				return MAKE_TOKEN(TOKEN_assign_xor);
			}
			return MAKE_TOKEN(TOKEN_xor);
		} break;
		case '&': {
			if ((ch = is_assignment(file))) {
				return MAKE_TOKEN(TOKEN_assign_and);
			}
			return MAKE_TOKEN(TOKEN_ampersand);
		} break;
		case '|': {
			if ((ch = is_assignment(file))) {
				return MAKE_TOKEN(TOKEN_assign_or);
			}
			return MAKE_TOKEN(TOKEN_or);
		} break;

		case '=': {
			if ((ch = is_assignment(file))) {
				return MAKE_TOKEN(TOKEN_eq);
			}
			return MAKE_TOKEN(TOKEN_assignment);
		} break;
		case '!': {
			if ((ch = is_assignment(file))) {
				return MAKE_TOKEN(TOKEN_neq);
			}
			return MAKE_TOKEN(TOKEN_invalid);
		} break;
		case '<': {
			if (ParserFile_Peek(file) == '<') {
				file->pos++;
				if ((ch = is_assignment(file))) {
					return MAKE_TOKEN(TOKEN_assign_shl);
				}
				return MAKE_TOKEN(TOKEN_shl);
			}
			if ((ch = is_assignment(file))) {
				return MAKE_TOKEN(TOKEN_lte);
			}
			return MAKE_TOKEN(TOKEN_lt);
		} break;
		case '>': {
			if (ParserFile_Peek(file) == '>') {
				file->pos++;
				if ((ch = is_assignment(file))) {
					return MAKE_TOKEN(TOKEN_assign_shr);
				}
				return MAKE_TOKEN(TOKEN_shr);
			}
			if ((ch = is_assignment(file))) {
				return MAKE_TOKEN(TOKEN_gte);
			}
			return MAKE_TOKEN(TOKEN_gt);
		} break;

		case '\'': {
			enum TokenType type = scanChar(file);
			return MAKE_TOKEN(type);
		} break;
		case '"': {
			enum TokenType type = scanString(file);
			return MAKE_TOKEN(type);
		} break;

		default: {
			return MAKE_TOKEN(TOKEN_invalid);
		}
	}
	return MAKE_TOKEN(TOKEN_invalid);
}
#undef MAKE_TOKEN 


