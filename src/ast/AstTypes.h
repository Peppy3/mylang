#ifndef AST_TYPES_H_
#define AST_TYPES_H_

#include <tokenizer/Token.h>

#define AST_NODE_X_LIST \
	X(None)\
	X(Literal)\
	X(List)\
	X(BinOp)\
	X(PostOp)\
	X(UnaryOp)\
	X(Declaration)\
	X(FuncType)\
	X(ReturnStmt)\
	X(IfStmt)

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

// Valid node that means that nothing is here
struct AstNone {
	AstNodeType type;
};

struct AstLiteral {
	AstNodeType type;
	Token val;
};

struct AstList {
	AstNodeType type;
	AstNodeHandle next;
	AstNodeHandle val;
};

struct AstBinOp {
	AstNodeType type;
	AstNodeHandle lhs;
	AstNodeHandle rhs;
	Token op;
};

struct AstPostOp {
	AstNodeType type;
	AstNodeHandle val;
	Token op;
};

struct AstUnaryOp {
	AstNodeType type;
	AstNodeHandle val;
	Token op;
};

struct AstDeclaration {
	AstNodeType type;
	AstNodeHandle type_expr;
	Token ident;
	AstNodeHandle expr;
};

struct AstFuncType {
	AstNodeType type;
	AstNodeHandle args;
	AstNodeHandle returns;
};

struct AstReturnStmt {
	AstNodeType type;
	AstNodeHandle expr;
};

struct AstIfStmt {
	AstNodeType type;
	AstNodeHandle expr;
	AstNodeHandle if_block;
	AstNodeHandle else_block;
};

#endif /* AST_TYPES_H_ */
