#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"
#include "ParserCtx.h"
#include "Ast.h"
#include "AstPrint.h"
#include "Parser.h"

#include "args/args.h"

int main(int argc, char **argv) {
	ParserCtx ctx = {0};
	ArgOptions options;

	if (args_parse(argc, argv, &options)) {
		return EXIT_FAILURE;
	}

	if (options.command == ArgSubcommand_run) {
		fprintf(stderr, "Subcommand 'run' not implemented\n");
		return EXIT_FAILURE;
	}
	
	if (ParserCtx_Setup(&ctx, options.input_file)) {
		return EXIT_FAILURE;
	}

	int num_errors = parse(&ctx);

	if (num_errors > 0) {
		printf("Number of parser errors: %d\n", num_errors);
		return EXIT_FAILURE;
	}
	else if (num_errors < 0) {
		fprintf(stderr, "Inernal parser error\n");
		return EXIT_FAILURE;
	}

	if (options.command == ArgSubcommand_dump_ast) {
		Ast_PrettyPrint(&ctx, stdout);
		return EXIT_SUCCESS;
	}

	
	
	ParserCtx_Teardown(&ctx);

	return EXIT_SUCCESS;
}


