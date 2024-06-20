#ifndef AST_H
#define AST_H

#include <stddef.h>
#include <stdint.h>

#include "arena.h"
#include "token.h"

/*
 * The AST is stored as a packed linear tree
 * This makes it easy to put it into a binary file
 * The stream is in little endian.
 */

enum AstNodeType {
	AstEof = 0x00,
	AstEndNode = 0x01, // End of node scope 
	AstNop = 0x02, // No operation, skip to next byte
	AstExpression = 0x03,
	AstIdentifier = 0x04, // just a length and the name
};

#define AST_FILE_MAGIC {0x7f, 0x41, 0x53, 0x54, 0x00, 0x00, 0x00}
#define AST_FILE_MAJOR_VERSION 0x0
#define AST_FILE_MINOR_VERSION 0x1

struct AstHead {
	char magic[7];
	char flags; // empty
	uint32_t verMajor;
	uint32_t verMinor;
	uint32_t filenameLen;
	char sourceFilename[];
};

/*
 * The AST file is structured as a bit stream.
 * 
 * Every node starts with its type for easy traversal.
 */

struct AstNode {
	char nodeType;
};

struct AstIdentifier {
	uint32_t nameLen;
	char name[];
};

#endif /* AST_H */

