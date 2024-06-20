#ifndef PARSER_H
#define PARSER_H

#include "arena.h"

struct Parser {
	struct File *src;
	struct Scanner scanner;

	struct Token nextToken;

	struct Arena *arena;

	char *err;
};

enum ParserError {
	ParserSuccess = 0,
	ParserScannerError = -1,
	ParserFileLoadError = -2,
	ParserParseError = -3,
};

enum ParserError ParserInit(struct Parser *parser, const char *sourceName, struct Arena *arena);
void ParserDestroy(struct Parser *parser);

#endif /* PARSER_H */

