#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <util.h>
#include <CompileUnit.h>
#include <ast/Ast.h>
#include <ast/AstPrint.h>
#include <parser/Parser.h>

#include <args/args.h>

int main(int argc, char **argv) {
	CompileUnit unit = {0};
	ArgOptions options;

	if (args_parse(argc, argv, &options)) {
		return EXIT_FAILURE;
	}

	if (options.command == ArgSubcommand_run) {
		fprintf(stderr, "Subcommand 'run' not implemented\n");
		return EXIT_FAILURE;
	}
	
	if (CompileUnit_Setup(&unit, options.input_file)) {
		return EXIT_FAILURE;
	}

	int num_errors = parse(&unit);

	if (num_errors > 0) {
		printf("Number of parser errors: %d\n", num_errors);
		CompileUnit_Teardown(&unit);
		return EXIT_FAILURE;
	}
	else if (num_errors < 0) {
		fprintf(stderr, "Inernal parser error\n");
		CompileUnit_Teardown(&unit);
		return EXIT_FAILURE;
	}

	if (options.command == ArgSubcommand_dump_ast) {
		Ast_PrettyPrint(&unit, stdout);
		CompileUnit_Teardown(&unit);
		return EXIT_SUCCESS;
	}

	
	
	CompileUnit_Teardown(&unit);

	return EXIT_SUCCESS;
}


