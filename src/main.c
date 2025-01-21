#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"
#include "ParserCtx.h"
#include "Ast.h"
#include "AstPrint.h"
#include "Parser.h"

int main(int argc, char **argv) {
	ParserCtx ctx = {0};

	if (argc != 2) {
		fprintf(stderr, "%s [file]\n", argv[0]);
		return 1;
	}
	
	if (ParserCtx_Setup(&ctx, argv[1])) {
		return 1;
	}

	int num_errors = parse(&ctx);
	printf("Number of parser errors: %d\n", num_errors);

	Ast_PrettyPrint(&ctx, stdout);
	
	ParserCtx_Teardown(&ctx);

	return 0;
}


