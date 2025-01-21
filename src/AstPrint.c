#include <stdint.h>

#include <stdio.h>

#include "util.h"
#include "Token.h"
#include "Ast.h"

#include "AstPrint.h"

void Ast_PrettyPrint_internal(ParserCtx *ctx, FILE *fp, AstNodeHandle handle) {
	if (handle == AST_INVALID_HANDLE) {
		fprintf(fp, "INVALID_HANDLE\n");
		return;
	}

	AstNode *node = Ast_GetNodeRef(&ctx->ast, handle);
// I know this is kinda undebuggable but it's 
// much better than having to miss printing something
	switch (*node) {
#define X(name)\
		case AST_TYPE_##name: {\
			Ast_Print_##name(ctx, fp, handle);\
		} break;
AST_NODE_X_LIST
#undef X
	}
}

void Ast_PrettyPrint(ParserCtx *ctx, FILE *fp) {
	fprintf(fp, "Ast for %s:\n\n", ctx->source_path);

	Ast_PrettyPrint_internal(ctx, fp, 0);
	fputc('\n', fp);
}

void Ast_Print_None(ParserCtx *ctx, FILE *fp, AstNodeHandle handle) {
	fprintf(fp, "None");
}

void Ast_Print_Literal(ParserCtx *ctx, FILE *fp, AstNodeHandle handle) {
	AstNode *token_node = Ast_GetNodeRef(&ctx->ast, handle);
	Token *token = &(ctx->tokens.tokens[token_node[1]]);

	print_token(fp, &ctx->src, token);
}

void Ast_Print_List(ParserCtx *ctx, FILE *fp, AstNodeHandle handle) {
	AstNode *ref;

	fprintf(fp, "List: {\n");

	for (;;) {

		Ast_PrettyPrint_internal(ctx, fp, handle + 2);
		ref = Ast_GetNodeRef(&ctx->ast, handle);

		if (ref[1] == AST_INVALID_HANDLE) {
			break;
		}

		handle = ref[1];
		ref = Ast_GetNodeRef(&ctx->ast, handle);
	}

	fprintf(fp, "}");
}

