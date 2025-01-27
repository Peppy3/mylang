#include <stddef.h>

#include <stdio.h>
#include <string.h>

#include "args.h"

const char *help_txt =
"mylang [subcommand] ...\n"
"\n"
"   run [file]        - Runs the program though the build in interpereter\n"
"   dump-ast [file]   - Dumps the abstract syntax trer for the file to stdout\n"
;

void print_help(void) {
	fprintf(stderr, help_txt);
}

int args_parse_dump_ast(int argc, char **argv, ArgOptions *options, int arg_idx) {
	options->command = ArgSubcommand_dump_ast;
	
	if (argc < 3) {
		fprintf(stderr, "ERROR: Not enough arguments for 'dump-ast'\n");
		print_help();
		return -1;
	}

	options->input_file = argv[arg_idx];

	return 0;
}

int args_parse_run(int argc, char **argv, ArgOptions *options, int arg_idx) {
	options->command = ArgSubcommand_run;
	
	if (argc < 3) {
		fprintf(stderr, "ERROR: Not enough arguments for 'run'\n");
		print_help();
		return -1;
	}

	options->input_file = argv[arg_idx];

	return 0;
}

int args_parse(int argc, char **argv, ArgOptions *options) {
	*options = (ArgOptions){0};
	int arg_idx = 1;

	if (argc < 2) {
		fprintf(stderr, "ERROR: Not enough arguments\n");
		print_help();
		return -1;
	}
	
	if (strcmp("dump-ast", argv[arg_idx]) == 0) {
		return args_parse_dump_ast(argc, argv, options, arg_idx + 1);
	}
	else if (strcmp("run", argv[arg_idx]) == 0) {
		return args_parse_run(argc, argv, options, arg_idx + 1);
	}
	else {
		options->command = ArgSubcommand_none;
		fprintf(stderr, "ERROR: Unknown subcommand '%s'\n", argv[arg_idx]);
		print_help();
		return -1;
	}
	return 0;
}


