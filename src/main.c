#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "file.h"
#include "scanner.h"
#include "token.h"

#include "parser.h"

void printToken(struct Token *token) {
	printf("type: %s, prec: %d lit: ", 
			TokenGetStringRep(token), TokenGetPrecedence(token)
		  );
	fwrite(token->pos, sizeof(char), token->len, stdout);
	puts("");
}

int main(int argc, char *const *argv) {
	struct File *file;
	struct Scanner scanner;
	struct Token token = {0};
	//struct Parser parser;

	if (argc != 2) {
		fprintf(stderr, "%s [file]\n", argv[0]);
		return 1;
	}
	
	/*
	if (ParserInit(&parser, argv[1], stdout) != ParserSuccess) {
		return 1;
	}
	*/

	file = FileOpen(argv[1]);
	if (file == NULL) {
		return 1;
	}

	if (ScannerInit(&scanner, FileGetSize(file), FileGetData(file)) != ScannerSuccess) {
		return 1;
	}

	do {
		ScannerScanNext(&scanner, &token);
		if (scanner.err != NULL) {
			printf("'%c' %s\n", *token.pos, scanner.err);
		}
		else {
			printToken(&token);
		}
	} while (token.type != EOF_TOKEN);

	ScannerDestroy(&scanner);

	FileClose(file);

	//ParserDestroy(&parser);

	return 0;
}


