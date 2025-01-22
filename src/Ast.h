#ifndef AST_H_
#define AST_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include <stdlib.h>

#include "util.h"
#include "Token.h"

#define AST_NODE_X_LIST \
	X(None)\
	X(Literal)\
	X(List)\
	X(BinOp)\
	X(PostOp)\
	X(UnaryOp)

enum AstNodeType_e {
#define X(name) AST_TYPE_##name,
AST_NODE_X_LIST
#undef X
	AST_NODE_TYPE_END
};

typedef int32_t AstNodeType;
typedef int32_t AstNode;
typedef int32_t AstNodeHandle;
#define AST_INVALID_HANDLE (-1)

#define X(name) typedef struct Ast##name Ast##name;
AST_NODE_X_LIST
#undef X

// Doesn't work in -Wpedantic mode
struct AstNone {
	AstNodeType type;
};

struct AstLiteral {
	AstNodeType type;
	TokenPos val;
};

// the value stored should come right after this
struct AstList {
	AstNodeType type;
	AstNodeHandle next;
	AstNodeHandle val;
};

struct AstBinOp {
	AstNodeType type;
	TokenPos op;
	AstNodeHandle lhs;
	AstNodeHandle rhs;
};

struct AstPostOp {
	AstNodeType type;
	TokenPos op;
	AstNodeHandle val;
};

struct AstUnaryOp {
	AstNodeType type;
	TokenPos op;
	AstNodeHandle val;
};

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

// if back == AST_INVALID_HANDLE, it starts a new list
// The element in the list should be the value at the index
AstNodeHandle Ast_ListAppend(Ast *ast, AstNodeHandle back);

static inline AstNode *Ast_GetNodeRef(const Ast *ast, AstNodeHandle handle) {
	util_assert(handle > -1 && handle < ast->len);
	return &ast->data[handle];
}

#endif /* AST_H_ */

