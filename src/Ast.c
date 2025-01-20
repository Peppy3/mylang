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
bool Ast_New(Ast *ast) {
	ast->data = malloc(AST_START_CAP * sizeof(AstNode));
	if (ast->data == NULL) {
		perror("Ast_New()");
		return true;
	}

	ast->cap = AST_START_CAP;
	ast->len = 0;
	return false;
}

AstNodeHandle Ast_AllocNode(Ast *ast) { 
	if (ast->len >= ast->cap) { 
		ast->cap += AST_START_CAP; 
		AstNode *new_data = realloc(ast->data, ast->cap * sizeof(AstNode)); 
		if (new_data == NULL) { 
			exit(errno); 
		} 
		ast->data = new_data; 
	} 
	AstNodeHandle handle = ast->len++;
	return handle;
} 

AstNodeHandle Ast_ListAppend(Ast *ast, AstNodeHandle back) {
	AstNodeHandle next_handle = Ast_AllocNode(ast);

	if (back != AST_INVALID_HANDLE) {
		Ast_GetNodeRef(ast, back)->list.next = next_handle;
	}
	
	AstNode *next_ref = Ast_GetNodeRef(ast, next_handle);

	*next_ref = (AstNode){
		.type = AST_NODE_LIST,
		.list.val = AST_INVALID_HANDLE,
		.list.next = AST_INVALID_HANDLE,
	};
	
	return next_handle;
}


void Ast_PrettyPrint(Ast *ast, ParserFile *src, AstNodeHandle start, FILE *stream) {
	if (start == AST_INVALID_HANDLE) {
		fputs("Invalid Handle", stream);
		return;
	}
	
	AstNode *start_ref = Ast_GetNodeRef(ast, start);

	switch (start_ref->type) {
		case AST_NODE_NONE: {
			fputs("None", stream);
		} break;
		case AST_NODE_LITERAL: {
			fputs("Literal: ", stream);
			fwrite(&src->data[start_ref->literal.tok.pos], 
				sizeof(char), start_ref->literal.tok.len, stream
				);
		} break;
		case AST_NODE_OPERAION: {
			if (start_ref->operation.rhs == AST_INVALID_HANDLE) {
				fputs("UnaryOp: {\n", stream);
				Ast_PrettyPrint(ast, src, start_ref->operation.lhs, stream);
			}
			else if (start_ref->operation.lhs == AST_INVALID_HANDLE) {
				fputs("PostfixOp: {\n", stream);
				Ast_PrettyPrint(ast, src, start_ref->operation.rhs, stream);
			}
			else {
				fputs("BinaryOp: {\n", stream);
				fwrite(&src->data[start_ref->operation.op.pos], 
					sizeof(char), start_ref->operation.op.len, stream
					);
				Ast_PrettyPrint(ast, src, start_ref->operation.lhs, stream);
				Ast_PrettyPrint(ast, src, start_ref->operation.rhs, stream);
			}
			fputs("}\n", stream);
		} break;
		case AST_NODE_LIST: {
			fputs("List: {\n", stream);
			Ast_PrettyPrint(ast, src, start_ref->list.val, stream);
			
			while (start_ref->list.next != AST_INVALID_HANDLE) {
				fputs(",\n", stream);
				AstNode *tmp = Ast_GetNodeRef(ast, start_ref->list.next);
				start_ref = tmp;
				Ast_PrettyPrint(ast, src, start_ref->list.val, stream);
			}

			fputs("}\n", stream);
		} break;
		case AST_NODE_LINKNAME: {
			fputs("Linkname: ", stream);
			fwrite(&src->data[start_ref->linkname.name.pos], 
				sizeof(char), start_ref->linkname.name.len, stream
				);
		} break;
		default: {
			fprintf(stream, "Unknown Node\n");
		} break;
	}
}

