#ifndef AST_H_
#define AST_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#include "Token.h"

enum AstNodeType_e {
	AST_NODE_NONE,
	AST_NODE_LITERAL,
	AST_NODE_OPERAION,
	AST_NODE_LIST,
	AST_NODE_LINKNAME,
	AST_NODE_FUNC_DEF,

	AST_NODE_TYPE_END
};

typedef int32_t AstNodeHandle;
typedef enum AstNodeType_e AstNodeType;
#define AST_INVALID_HANDLE (-1)

// Identifiers are also considered literals in the ast as it uses the same structure
typedef struct {
	Token tok;
} AstLiteral;

// either lhs or rhs will be AST_INVALID_HANDLE if it's either postfix or unary
typedef struct {
	Token op;
	AstNodeHandle lhs, rhs;
} AstOperation;

// next == AST_INVALID_HANDLE if end of list
typedef struct {
	AstNodeHandle val;
	AstNodeHandle next;
} AstList;

typedef struct {
	Token name;
} AstLinkname;

typedef struct {
	AstNodeHandle name;
	AstNodeHandle args;
	AstNodeHandle return_types;
	AstNodeHandle body;
} AstFuncDef;

typedef struct {
	AstNodeType type;
	union {
		AstLiteral literal;
		AstOperation operation;
		AstList list;
		AstLinkname linkname;
		AstFuncDef func_def;
	};
} AstNode;

typedef struct {
	size_t cap;
	size_t len;
	AstNode *data;
} Ast;

// returns true on error
bool Ast_New(Ast *ast);

static inline void Ast_Delete(const Ast ast) {
	free(ast.data);
}

// gives you a new node handle
AstNodeHandle Ast_AllocNode(Ast *ast);

// if back == AST_INVALID_HANDLE, it generates a new list
AstNodeHandle Ast_ListAppend(Ast *ast, AstNodeHandle back);

void Ast_PrettyPrint(Ast *ast, ParserFile *src, AstNodeHandle start, FILE *stream);

static inline AstNode *Ast_GetHead(const Ast ast) { return ast.data; }

static inline AstNode *Ast_GetNodeRef(const Ast *ast, AstNodeHandle handle) {
	assert(handle > -1 && handle < ast->len);
	return &ast->data[handle];
}

#endif /* AST_H_ */
