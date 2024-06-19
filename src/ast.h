#ifndef AST_H
#define AST_H

#include <stddef.h>
#include <stdint.h>

#include "arena.h"
#include "token.h"

enum AstNodeType {
	AstEnd,
	AstExpression,
	AstDefiniton,
	AstIdentifier,
};

/*
 * The Ast is stored as a packed linear tree
 * This makes it easy to put it into a binary file
 */

struct AstToken {
	uint32_t type;
	uint32_t litSize; // size of the literal
	char lit[]; // token literal (should be padded to the next 32 bits)
};

struct AstNode {
	uint32_t nodeType;
	uint32_t nextNode; // Index to the next node (this could be far away)
	struct AstToken token;
};


#endif /* AST_H */
