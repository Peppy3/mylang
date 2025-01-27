#ifndef AST_H_
#define AST_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include <stdlib.h>

#include <util.h>
#include <tokenizer/Token.h>

#include <ast/AstTypes.h>

typedef struct {
	uint32_t cap;
	uint32_t len;
	int32_t *data;
} Ast;

// returns non- zero on error
int Ast_New(Ast *ast);

static inline void Ast_Delete(const Ast *ast) {
	free(ast->data);
}

// len: length of the node in bytes
// gives you a new node handle
AstNodeHandle Ast_AllocNode_(Ast *ast, uint32_t len);
#define Ast_AllocNode(ast, T) Ast_AllocNode_(ast, sizeof(T) >> 2)

static inline AstNode *Ast_GetNodeRef(Ast *ast, AstNodeHandle handle) {
	util_assert(handle > -1 && handle < (int32_t)ast->len);
	return &ast->data[handle];
}

// if back == AST_INVALID_HANDLE, it starts a new list
// The element in the list should be the value at the index
AstNodeHandle Ast_ListAppend(Ast *ast, AstNodeHandle back);

static inline AstNodeHandle Ast_ListAddVal(Ast *ast, AstNodeHandle back, AstNodeHandle val) {
	AstList *back_ref = (AstList*)Ast_GetNodeRef(ast, back);
	back_ref->val = val;
	return back;
}

#define X(name) AstNodeHandle Ast_Make_##name(Ast *ast, Ast##name node);
AST_NODE_X_LIST
#undef X

#endif /* AST_H_ */

