#ifndef AST_PRINT_H_
#define AST_PRINT_H_

#include <stdio.h>

#include <ast/Ast.h>
#include <CompileUnit.h>

// pretty prints the entire ast
void Ast_PrettyPrint(CompileUnit *unit, FILE *fp);

#define X(name) void Ast_Print_##name(CompileUnit *unit, FILE *fp, AstNodeHandle handle);
AST_NODE_X_LIST
#undef X

#endif /* AST_PRINT_H_ */

