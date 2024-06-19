#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "file.h"
#include "scanner.h"
#include "token.h"

void printToken(char *file, struct Token *token) {
	printf("type: %s, pos: %ld, prec: %d lit: ", 
			TokenGetStringRep(token), token->pos, TokenGetPrecedence(token)
		  );
	fwrite(file + token->pos, sizeof(char), token->len, stdout);
	puts("");
}

int main(int argc, char *const *argv) {
	struct File *file;
	struct Scanner scanner;
	struct Token token = {0};

	if (argc != 2) {
		fprintf(stderr, "%s [file]\n", argv[0]);
		return 1;
	}

	file = FileOpen(argv[1]);
	if (file == NULL) {
		return 1;
	}

	if (ScannerInit(&scanner, FileGetSize(file), FileGetData(file)) != ScannerSuccess) {
		return 1;
	}
	
	while (token.type != EOF_TOKEN) {
		ScannerScanNext(&scanner, &token);
		if (scanner.err != NULL) {
			printf("pos %lu: '%c' %s\n", 
					token.pos, FileGetData(file)[token.pos], scanner.err);
		}
		else {
			printToken(FileGetData(file), &token);
		}
	}

	ScannerDestroy(&scanner);

	FileClose(file);

	return 0;
}


