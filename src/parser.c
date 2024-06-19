#include <stddef.h>

#include <stdio.h>

#include "file.h"
#include "scanner.h"

struct Parser {
	struct File *file;
	struct Scanner scanner;

	struct Token nextToken;
};

enum ParserError {
	ParserSuccess = 0,
	ParserScannerError = -1,
	ParserFileLoadError = -2,
	ParserParseError = -3,
};

enum ParserError ParserInit(struct Parser *parser, const char *filename) {
	struct File *file = FileOpen(filename);
	if (file == NULL) {
		return ParserFileLoadError;
	}
	
	if (ScannerInit(&parser->scanner, FileGetSize(file), FileGetData(file)) != ScannerSuccess) {
		FileClose(file);
		return ParserScannerError;
	}

	if (ScannerScanNext(&parser->scanner, &parser->nextToken) != ScannerSuccess) {
		fprintf(stderr, "Probably not a [Unknown Language] file!\n");
		FileClose(file);
		return ParserScannerError;
	}

	parser->file = file;
	
	return ParserSuccess;
}

void ParserDestroy(struct Parser *parser) {
	ScannerDestroy(&parser->scanner);
	FileClose(parser->file);
}



