#ifndef AST_H
#define AST_H

#include <stddef.h>
#include <stdint.h>

/*
 * The AST is stored as a flattened tree in memory.
 * All strings are stored as null terminated strings.
 */

enum AstNodeType {
	AST_END,
	AST_DEF,
	AST_EXPR,
}

typedef struct AstHead {
	uint32_t size; // size of the entire tree
	uint32_t nodeOff; // offset of the first node from the beginning
	
	// name of the file as null terninated string aligned to 4 byte boundry
	char     filename[]; 
} AstHead;

typedef struct AstIdNode{
	uint32_t assignTypeOff; 
	char     name[];
}

typedef struct AstExpressionNode {
	uint32_t exprType;
	uint32_t rhsOff;
	// lhs comes rigth after
}

typedef struct AstNode {
	uint32_t  type; // type of node
	uint32_t size; // size of the node, including everything nested inside it
	union {
		AstExpressionNode expr;
	};
} AstNode;

#endif /* AST_H */
