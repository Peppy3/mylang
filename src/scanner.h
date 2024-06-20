#ifndef SCANNER_H
#define SCANNER_H

#include <stddef.h>
#include <stdio.h>

#include "token.h"

struct Scanner {
	size_t codeSize;
	char *code;

	int ch;
	int nextCh;

	char *pos;
	char *lineOffset; // current line offset
	
	char *err;
};

enum ScannerError {
	ScannerSuccess = 0,
	ScannerTokenError = -1,
	ScannerCodeTooSmallError = -2,
};

enum ScannerError ScannerInit(struct Scanner *scanner, size_t code_size, char *code);

void ScannerDestroy(struct Scanner *scanner);

enum ScannerError ScannerScanNext(struct Scanner *scanner, struct Token *token);

#endif /* SCANNER_H */
