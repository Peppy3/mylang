#ifndef PARSER_CTX_H_
#define PARSER_CTX_H_

#include <ParserFile.h>
#include <tokenizer/Token.h>
#include <ast/Ast.h>

struct CompileUnit {
	char *source_path;
	ParserFile src;
	Ast ast;
	Token current_token;
	Token lookahead_token;
	int num_errors;
};

typedef struct CompileUnit CompileUnit;

// returns 0 on success and -1 on error
int CompileUnit_Setup(CompileUnit *unit, char *filepath);

void CompileUnit_Teardown(CompileUnit *unit);

#endif /* PARSER_CTX_H_ */

