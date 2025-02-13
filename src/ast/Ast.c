#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <errno.h>
#include <stdio.h>

#include "util.h"
#include "ParserFile.h"
#include <tokenizer/Token.h>

#include <ast/Ast.h>

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
	AstNodeHandle handle = ast->len;
	ast->len += len;

	uint32_t grow_by = ast->cap >> 1;
	if (len > grow_by) {
		grow_by = len;
	}

	if (ast->len >= ast->cap) {
		ast->cap += grow_by;
		AstNode *new_data = realloc(ast->data, sizeof(AstNode) * ast->cap);
		if (new_data == NULL) {
			perror("Ast_AllocNode_()");
			exit(errno);
		}
		ast->data = new_data;
	}
	return handle;
} 

AstNodeHandle Ast_ListAppend(Ast *ast, AstNodeHandle back) {
	AstNodeHandle next_handle = Ast_AllocNode(ast, AstList);

	if (back != AST_INVALID_HANDLE) {
		AstList *back_ref = (AstList*)Ast_GetNodeRef(ast, back);
		back_ref->next = next_handle;
	}
	
	AstList *next_ref = (AstList*)Ast_GetNodeRef(ast, next_handle);

	*next_ref = (AstList){
		.type = AST_TYPE_List,
		.next = AST_INVALID_HANDLE,
	};
	
	return next_handle;
}

#define X(name)\
AstNodeHandle Ast_Make_##name(Ast *ast, Ast##name node)\
{\
	AstNodeHandle handle = Ast_AllocNode_(ast, sizeof(Ast##name) >> 2);\
	Ast##name *ref = (Ast##name*)Ast_GetNodeRef(ast, handle);\
	*ref = node;\
	return handle;\
}
AST_NODE_X_LIST
#undef X

