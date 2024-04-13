#include <assert.h>
#include <stddef.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "tokenizer.h"

// returns the initial length of the string containint characters 
// specified in ranges e.g. "AZaz__"
int initialLength(const char *str, char *ranges) {
	if (strlen(ranges) & 1) return -1;

	int length = 0;
	while (str[length] != '\0') {
		char ch = str[length];
		char *range = ranges;
		int valid = 0;
		while (*range) {
			char start = *range++;
			char end = *range++;
			if (ch >= start && ch <= end) {
				valid = 1;
				break;
			}
		}
		if (!valid) {
			break;
		}
		length++;
	}
	return length;
}

Filebuff *tokenizer_init(const char *filename) {
	FILE *stream = fopen(filename, "rb");
	if (stream == NULL) return NULL;
	if (fseek(stream, 0, SEEK_END)) {
		fclose(stream);
		return NULL;
	}
	long size = ftell(stream);
	if (size < 0) {
		fclose(stream);
		return NULL;
	}
	if (fseek(stream, 0, SEEK_SET)) {
		fclose(stream);
		return NULL;
	}
	Filebuff *fb = malloc(sizeof(Filebuff) + size + 1);
	memset(fb, 0, sizeof(Filebuff) + size + 1);
	if (fb == NULL) {
		fclose(stream);
		return NULL;
	}

	fb->pos = 0;
	fb->size = fread(fb->file, 1, size, stream);
	if (ferror(stream)) {
		fclose(stream);
		free(fb);
		return NULL;
	}

	fclose(stream);
	return fb;
}

void tokenizer_free(Filebuff *fb) {
	free(fb);
}

Token next_token(Filebuff *fb) {
	char *str = &fb->file[fb->pos];
	
	Token tok = {
		.type = INVALID,
		.str = str,
		.pos = fb->pos,
		.len = 0
	};

	if (fb->size < fb->pos) {
		tok.type = _EOF;
		return tok;
	}
	long len = 0;
	char ch = *str;
	
	if (isalpha(ch) || ch == '_') {
		len = initialLength(str, "09AZaz__");
		if (len < 0) {
			return tok;
		}
		tok.type = IDENTIFIER;
		tok.len = len;
		fb->pos += len;
	}
	else if (isspace(ch)) {
		len = initialLength(str, "\t\r  ");
		if (len < 0) {
			return tok;
		}
		tok.type = SPACE;
		tok.len = len;
		fb->pos += len;
	}
	else {
		switch (ch) {
			/*case '"':
				{ // TODO: string parsing
					tok.type = STRING_LITERAL;
				}*/
			case '0': // TODO: hex and binary
			case '1': case '2': case '3': case '4': case '5':
			case '6': case '7': case '8': case '9': 
					  {
						len = initialLength(str, "09");
						if (len < 0) {
							return tok;
						}
						tok.type = INT_LITERAL;
						tok.len = len;
						fb->pos += len;
					  } break;

			case ':': tok.type = COLON; tok.len = 1; fb->pos += 1; break;
			case ';': tok.type = SEMICOLON; tok.len = 1; fb->pos += 1; break;

			case '(': tok.type = LPAREN; tok.len = 1; fb->pos += 1; break;
			case '{': tok.type = LCURLY; tok.len = 1; fb->pos += 1; break;
			case ')': tok.type = RPAREN; tok.len = 1; fb->pos += 1; break;
			case '}': tok.type = RCURLY; tok.len = 1; fb->pos += 1; break;

			default: tok.type = INVALID; tok.len = 1; fb->pos += 1; break;
		}
	}

	return tok;
}

