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
		return EXIT_FAILURE;
	}
	
	if (ParserCtx_Setup(&ctx, argv[1])) {
		return EXIT_FAILURE;
	}

	int num_errors = parse(&ctx);

	if (num_errors > 0) {
		printf("Number of parser errors: %d\n", num_errors);
	}
	else if (num_errors < 0) {
		fprintf(stderr, "Inernal parser error\n");
		return EXIT_FAILURE;
	}
	else if (num_errors == 0) {
		Ast_PrettyPrint(&ctx, stdout);
	}
	
	ParserCtx_Teardown(&ctx);

	return EXIT_SUCCESS;
}


