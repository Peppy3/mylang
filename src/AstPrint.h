#ifndef AST_PRINT_H_
#define AST_PRINT_H_

#include <stdio.h>

#include "Ast.h"
#include "ParserCtx.h"

// pretty prints the entire ast
void Ast_PrettyPrint(ParserCtx *ctx, FILE *fp);

#define X(name) void Ast_Print_##name(ParserCtx *ctx, FILE *fp, AstNodeHandle handle);
AST_NODE_X_LIST
#undef X

#endif /* AST_PRINT_H_ */

