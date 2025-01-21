#ifndef PARSER_CTX_H_
#define PARSER_CTX_H_

#include "ParserFile.h"
#include "TokenStream.h"
#include "Ast.h"

struct ParserCtx {
	char *source_path;
	ParserFile src;
	TokenStream tokens;
	Ast ast;
	int num_errors;
};

typedef struct ParserCtx ParserCtx;

int ParserCtx_Setup(ParserCtx *ctx, char *filepath);

void ParserCtx_Teardown(ParserCtx *ctx);

#endif /* PARSER_CTX_H_ */

