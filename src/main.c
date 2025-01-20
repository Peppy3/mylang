#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"
#include "ParserFile.h"
#include "Token.h"
#include "Tokenizer.h"
#include "Parser.h"

int main(int argc, char *const *argv) {
	ParserFile file;
	size_t line_pos = 0;

	if (argc != 2) {
		fprintf(stderr, "%s [file]\n", argv[0]);
		return 1;
	}

	
	if (ParserFile_Open(&file, argv[1])) {
		return 1;
	}

	Token tok = NextToken(&file, &line_pos);
	while (tok.type != TOKEN_eof) {
		print_token(stdout, &file, &tok);
		tok = NextToken(&file, &line_pos);
	}
	
	ParserFile_Close(file);

	return 0;
}


