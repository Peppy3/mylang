#include <stddef.h>

#include <string.h>
#include <stdio.h>

#include "arena.h"
#include "file.h"
#include "token.h"
#include "scanner.h"
#include "ast.h"

#include "parser.h"

enum ParserError ParserInit(struct Parser *parser, const char *sourceName, struct Arena *arena) {
	uint32_t sourceNameLen = strlen(sourceName);
	struct AstHead astHead = {
		.magic = AST_FILE_MAGIC,
		.verMajor = AST_FILE_MAJOR_VERSION,
		.verMinor = AST_FILE_MINOR_VERSION,
		.filenameLen = sourceNameLen
	};
	struct File *src = FileOpen(sourceName);

	if (src == NULL) {
		return ParserFileLoadError;
	}

	
	if (ScannerInit(&parser->scanner, FileGetSize(src), FileGetData(src)) != ScannerSuccess) {
		FileClose(src);
		return ParserScannerError;
	}

	if (ScannerScanNext(&parser->scanner, &parser->nextToken) != ScannerSuccess) {
		parser->err = "Probably not a [Unknown Language] source file!\n";
		FileClose(src);
		return ParserScannerError;
	}

	parser->src = src;
	parser->arena = arena;
	
	ArenaWrite(arena, sizeof astHead, &astHead);
	ArenaWrite(arena, sourceNameLen, sourceName);

	return ParserSuccess;
}

void ParserDestroy(struct Parser *parser) {
	ScannerDestroy(&parser->scanner);
	FileClose(parser->src);
	*parser = (struct Parser){0};
}

enum ParserError parserGetNextToken(struct Parser *parser, struct Token *token) {
	enum ScannerError scanErr;
	*token = parser->nextToken;
	
	do {
		scanErr = ScannerScanNext(&parser->scanner, &parser->nextToken);
		if (parser->scanner.err != NULL) {
			fprintf(stderr, "%s\n", parser->scanner.err);
		}
	} while (parser->nextToken.type == LINE_COMMENT 
			|| parser->nextToken.type == MULTILINE_COMMENT);

	return (scanErr == ScannerSuccess) ? ParserSuccess : ParserScannerError;
}

enum ParserError parseExpression(struct Parser *parser, struct Token *lhs, int minPrecedence) {
	struct Token *operand;
	struct Token *lookahead = &parser->nextToken;
	
	while (TokenGetPrecedence(lookahead) >= minPrecedence) {
		
		

	}


	return ParserSuccess;
}

enum ParserError ParserParseFile(struct Parser *parser) {
	struct Token token;

	if (parserGetNextToken(parser, &token) != ParserSuccess) {
		parser->err = "Probably not a [Unknown Language] source file!\n";
		return ParserScannerError;
	}

	while (token.type == EOF_TOKEN) {

		if (token.type == INVALID) {
			fprintf(stderr, "%s\n", parser->scanner.err);
		}

		parseExpression(parser, &token, 0);

	}
	
	return ParserSuccess;
}

