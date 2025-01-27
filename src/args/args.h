#ifndef ARGS_ARGS_H_
#define ARGS_ARGS_H_

enum ArgSubcommand {
	ArgSubcommand_none = 0,
	ArgSubcommand_dump_ast = 1,
	ArgSubcommand_run = 2,
};

typedef struct {
	enum ArgSubcommand command;
	char *input_file;
} ArgOptions;

void print_help(void);

// returns 0 on success and -1 on error
// argv should be a list of null terminated strings
int args_parse(int argc, char **argv, ArgOptions *options);

#endif /* ARGS_ARGS_H_ */
