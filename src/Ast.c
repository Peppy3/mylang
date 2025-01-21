#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "ParserFile.h"
#include "Token.h"

#include "Ast.h"

#define AST_START_CAP (512)
int Ast_New(Ast *ast) {
	int32_t *data = malloc(sizeof(ast->data[0]) * AST_START_CAP);
	if (data == NULL) {
		perror("Ast_New()");
		return -1;
	}

	*ast = (Ast){
		.cap = AST_START_CAP,
		.len = 0,
		.data = data,
	};

	return 0;
}

AstNodeHandle Ast_AllocNode_(Ast *ast, uint32_t len) { 
	if (ast->len >= ast->cap) {
		ast->cap += AST_START_CAP;
		AstNode *new_data = realloc(ast->data, sizeof(AstNode) * ast->cap);
		if (new_data == NULL) {
			perror("Ast_AllocNode_()");
			exit(errno);
		}
		ast->data = new_data;
	}
	AstNodeHandle handle = ast->len;
	ast->len += 1 + len;
	return handle;
} 

AstNodeHandle Ast_ListAppend(Ast *ast, AstNodeHandle back) {
	AstNodeHandle next_handle = Ast_AllocNode(ast, AstList);

	if (back != AST_INVALID_HANDLE) {
		AstNode *back_ref = Ast_GetNodeRef(ast, back);
		back_ref[1] = next_handle;
	}
	
	AstNode *next_ref = Ast_GetNodeRef(ast, next_handle);

	next_ref[0] = AST_TYPE_List;
	next_ref[1] = AST_INVALID_HANDLE;
	
	return next_handle;
}


