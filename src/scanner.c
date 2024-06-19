#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#include <ctype.h>
#include <stdio.h>

#include "token.h"

#include "scanner.h"

enum ScannerError ScannerInit(struct Scanner *scanner, size_t code_size, char *code) {

	if (code_size <= 2) {
		fputs("Code too small!\n", stderr);
		return ScannerCodeTooSmallError;
	}
	
	*scanner = (struct Scanner){
		.codeSize = code_size,
		.code = code,
		.ch = code[0],
		.nextCh = code[1],
		.pos = 0,
		.lineOffset = 0,
		.err = NULL
	};
	
	return ScannerSuccess;
}

void ScannerDestroy(struct Scanner *scanner) {
	assert(scanner != NULL);
	*scanner = (struct Scanner){0};
}

int lower(int ch) {
	return ch | ' ';
}

bool isLetter(int ch) {
	return ('a' <= lower(ch) && lower(ch) <= 'z') || ch == '_';
}

bool isTerminating(int ch) {
	return isspace(ch) || ispunct(ch) || ch == EOF;
}

// iterates to the next character
int ScannerNextChar(struct Scanner *scanner) {
	int ch = scanner->ch;
	size_t next_pos = scanner->pos + 1;

	if (ch == '\n') {
		scanner->lineOffset = next_pos;
	}
	
	ch = scanner->nextCh;
	scanner->ch = ch;

	if (scanner->codeSize <= next_pos) {
		scanner->nextCh = EOF;
	}
	else {
		scanner->nextCh = (char)(scanner->code[next_pos + 1] & 0xff);
		scanner->pos = next_pos;
	}

	return scanner->ch;
}

long scannerTokenOffset(struct Scanner *scanner) {
	return scanner->pos;
}

void ScannerSkipWhiteSpace(struct Scanner *scanner) {
	int ch = scanner->ch;
	while (isspace(ch)) {
		ch = ScannerNextChar(scanner);
	}
}

bool scanEscape(struct Scanner *scanner) {
	const int max_hex_chars = 2;
	int ch = ScannerNextChar(scanner);

	switch (ch) {
		case '0': case 'b': case 'f': case 'n': 
		case 'r': case 't': case 'v': 
		case '\\': case '"': case '\'':
			{
				ScannerNextChar(scanner);
				return true;
			} break;
		case 'x': 
			{
				ScannerNextChar(scanner);
				for (int i = 0; i < max_hex_chars; i++) {
					ScannerNextChar(scanner);
					if (!isxdigit(scanner->ch)) {
						scanner->err = "Invalid hex literal in escape sequence";
						return false;
					}
				}
				ScannerNextChar(scanner);
				return true;
			} break;
		default:
			{
				scanner->err = "unknown escape sequence";
				return false;
			} break;
	}
	return false; 
}

enum ScannerError scanLine(struct Scanner *scanner) {
	int ch = scanner->ch;
	while (ch != '\n') {
		ch = ScannerNextChar(scanner);
	}
	return ScannerSuccess;
}

enum ScannerError scanMultilineComment(struct Scanner *scanner) {

	while (scanner->ch != '*' || scanner->nextCh != '/') {
		ScannerNextChar(scanner);
	}

	// capture the final "*/"
	ScannerNextChar(scanner);
	ScannerNextChar(scanner);

	return ScannerSuccess;
}

enum ScannerError scanIdentifier(struct Scanner *scanner) {
	char ch = scanner->ch;
	while (isLetter(ch) || isdigit(ch)) {
		ch = ScannerNextChar(scanner);
	}

	if (!isTerminating(ch)) {
		return ScannerTokenError;
	}

	return ScannerSuccess;
}

enum ScannerError scanNumber(struct Scanner *scanner) {
	// TODO: Hex, float, bin?, octal?
	char ch = scanner->ch;
	
	while (isdigit(ch)) {
		ch = ScannerNextChar(scanner);
	}

	if (!isTerminating(ch)) {
		return ScannerTokenError;
	}

	return ScannerSuccess;
}

enum ScannerError scanChar(struct Scanner *scanner) {
	int ch = ScannerNextChar(scanner);

	if (ch == '\\') {
		if (!scanEscape(scanner)) {
			return ScannerTokenError;
		}
	}
	else {
		ScannerNextChar(scanner);
	}
	
	// capture the ending '\''
	ch = scanner->ch;
	if (ch != '\'') {
		scanner->err = "Character literal not terminated";
		return ScannerTokenError;
	}
	ScannerNextChar(scanner);

	return ScannerSuccess;
}

enum ScannerError scanString(struct Scanner *scanner) {
	// '"' opening aleady captured
	int ch = scanner->ch;

	while (1) {
		if (ch == '\n' || ch == EOF) {
			scanner->err = "String literal not terminated";
			return ScannerTokenError;
		}

		ch = ScannerNextChar(scanner);

		if (ch == '"') {
			break;
		}

		if (ch == '\\') {
			if (!scanEscape(scanner)) {
				return ScannerTokenError;
			}
			ch = ScannerNextChar(scanner);
		}
	}
	// capture ending '"'
	ScannerNextChar(scanner);

	return ScannerSuccess;
}

enum ScannerError ScannerScanNext(struct Scanner *scanner, struct Token *token) {
	assert(scanner != NULL);
	assert(token != NULL);
	enum ScannerError error = ScannerSuccess;
	int ch;
	
	ScannerSkipWhiteSpace(scanner);
	
	*token = (struct Token){
		.pos = scanner->pos,
		.type = INVALID
	};

	ch = scanner->ch;

	if (ch == EOF) {
		token->type = EOF_TOKEN;
		token->len = 0;
		return ScannerSuccess;
	}
	else if (isLetter(ch)) {
		error = scanIdentifier(scanner);
		if (error == ScannerTokenError) {
			token->type = INVALID;
		}
		token->type = IDENTIFIER;
	}
	else if (isdigit(ch)) {
		error = scanNumber(scanner);
		if (error == ScannerTokenError) {
			token->type = INVALID;
		}
		token->type = INT_LITERAL;
	}
	else {
		switch (ch) {
			case '(': 
				{
					ScannerNextChar(scanner);
					token->type = LPAREN;
				} break;
			case ')': 
				{
					ScannerNextChar(scanner);
					token->type = RPAREN;
				} break;
			case '{': 
				{
					ScannerNextChar(scanner);
					token->type = LCURLY;
				} break;
			case '}': 
				{
					ScannerNextChar(scanner);
					token->type = RCURLY;
				} break;
			case '[': 
				{
					ScannerNextChar(scanner);
					token->type = LSQUARE;
				} break;
			case ']': 
				{
					ScannerNextChar(scanner);
					token->type = RSQUARE;
				} break;
			case ';': 
				{
					ScannerNextChar(scanner);
					token->type = SEMICOLON;
				} break;
			case ',': 
				{
					ScannerNextChar(scanner);
					token->type = COMMA;
				} break;
			case '.': 
				{
					ScannerNextChar(scanner);
					token->type = PERIOD;
				} break;
			case ':': 
				{
					ScannerNextChar(scanner);
					token->type = COLON;
				} break;
			case '?': 
				{
					ScannerNextChar(scanner);
					token->type = QUESTION_MARK;
				} break;
			case '\'': 
				{
					error = scanChar(scanner);
					token->type = CHAR_LITERAL;
				} break;
			case '"': 
				{
					error = scanString(scanner);
					token->type = STRING_LITERAL;
				} break;
			case '+': 
				{
					ScannerNextChar(scanner);
					if (scanner->ch == '+') {
						ScannerNextChar(scanner);
						token->type = INCREMENT;
					}
					else if (scanner->ch == '=') {
						ScannerNextChar(scanner);
						token->type = ASSIGN_ADDITION;
					}
					else {
						token->type = ADDITION;
					}
				} break;
			case '-': 
				{
					ScannerNextChar(scanner);
					if (scanner->ch == '-') {
						ScannerNextChar(scanner);
						token->type = DECREMENT;
					}
					if (scanner->ch == '>') {
						ScannerNextChar(scanner);
						token->type = ARROW;
					}
					else if (scanner->ch == '=') {
						ScannerNextChar(scanner);
						token->type = ASSIGN_SUBTRACTION;
					}
					else {
						token->type = SUBTRACTION;
					}
				} break;
			case '*': 
				{
					if (scanner->ch == '=') {
						ScannerNextChar(scanner);
						token->type = ASSIGN_ASTERISK;
					}
					else {
						ScannerNextChar(scanner);
						token->type = ASTERISK;
					}
				} break;
			case '/': 
				{
					if (scanner->nextCh == '/') {
						error = scanLine(scanner);
						if (error == ScannerTokenError) {
							token->type = INVALID;
						}
					}
					else if (scanner->nextCh == '*') {
						error = scanMultilineComment(scanner);
						if (error == ScannerTokenError) {
							token->type = INVALID;
						}
					}
					else if (scanner->ch == '=') {
						ScannerNextChar(scanner);
						token->type = ASSIGN_DIVITION;
					}
					else {
						ScannerNextChar(scanner);
						token->type = DIVITION;
					}
				} break;
			case '%': 
				{
					if (scanner->ch == '=') {
						ScannerNextChar(scanner);
						token->type = ASSIGN_MODULUS;
					}
					else {
						ScannerNextChar(scanner);
						token->type = MODULUS;
					}
				} break;
			case '~': 
				{
					if (scanner->ch == '=') {
						ScannerNextChar(scanner);
						token->type = ASSIGN_LOGICAL_NOT;
					}
					else {
						ScannerNextChar(scanner);
						token->type = LOGICAL_NOT;
					}
				} break;
			case '^': 
				{
					if (scanner->ch == '=') {
						ScannerNextChar(scanner);
						token->type = ASSIGN_EXCLUSIVE_OR;
					}
					else {
						ScannerNextChar(scanner);
						token->type = EXCLUSIVE_OR;
					}
				} break;
			case '&': 
				{
					ScannerNextChar(scanner);
					if (scanner->ch == '&') {
						ScannerNextChar(scanner);
						token->type = BOOLEAN_AND;
					}
					else if (scanner->ch == '=') {
						ScannerNextChar(scanner);
						token->type = ASSIGN_LOGICAL_AND;
					}
					else {
						token->type = LOGICAL_AND;
					}
				} break;
			case '|': 
				{
					ScannerNextChar(scanner);
					if (scanner->ch == '|') {
						ScannerNextChar(scanner);
					  	token->type = BOOLEAN_OR;
					}
					else if (scanner->ch == '=') {
						ScannerNextChar(scanner);
						token->type = ASSIGN_LOGICAL_OR;
					}
					else {
					  	token->type = LOGICAL_OR;
					}
				} break;
			case '=': 
				{
					ScannerNextChar(scanner);
					if (scanner->ch == '=') {
						ScannerNextChar(scanner);
					  	token->type = EQUALS;
					}
					else {
					 	token->type = ASSIGN;
					}
				} break;
			case '!': 
				{
					ScannerNextChar(scanner);
					if (scanner->ch == '=') {
						ScannerNextChar(scanner);
					 	token->type = NOT_EQUALS;
					 }
					 else {
					  	token->type = BOOLEAN_NOT;
					 }
				} break;
			case '<': 
				{
					ScannerNextChar(scanner);
					if (scanner->ch == '=') {
						ScannerNextChar(scanner);
					  	token->type = LESS_THAN_EQUALS;
					}
					else if (scanner->ch == '<') {
						ScannerNextChar(scanner);
					 	token->type = SHIFT_LEFT;
					}
					else {
					  	token->type = LESS_THAN;
					}
				} break;
			case '>': 
				{
					ScannerNextChar(scanner);
					if (scanner->ch == '=') {
						ScannerNextChar(scanner);
					  	token->type = GREATER_THAN;
					}
					else if (scanner->ch == '>') {
						ScannerNextChar(scanner);
						token->type = SHIFT_RIGHT;
					}
					else {
					 	token->type = GREATER_THAN_EQUALS;
					}
				} break;
			default: 
				{
					ScannerNextChar(scanner);
					token->type = INVALID;
					error = ScannerTokenError;
				}
		}
	}
	token->len = scanner->pos - token->pos;
	return error;
}

