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
// Workaround is to break at the function you're interested in
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
	AstLiteral *lit = (AstLiteral*)Ast_GetNodeRef(&ctx->ast, handle);
	Token *token = &(ctx->tokens.tokens[lit->val]);

	print_token(fp, &ctx->src, token);
}

void Ast_Print_List(ParserCtx *ctx, FILE *fp, AstNodeHandle handle) {
	AstList *list;

	fprintf(fp, "List: {\n");

	for (;;) {

		list = (AstList*)Ast_GetNodeRef(&ctx->ast, handle);
		Ast_PrettyPrint_internal(ctx, fp, list->val);

		if (list->next == AST_INVALID_HANDLE) {
			break;
		}
		fprintf(fp, ",\n");
		handle = list->next;
		list = (AstList*)Ast_GetNodeRef(&ctx->ast, handle);
	}

	fprintf(fp, "\n}");
}


void Ast_Print_BinOp(ParserCtx *ctx, FILE *fp, AstNodeHandle handle) {
	fprintf(fp, "BinOp: {\n");
	
	AstBinOp *op = (AstBinOp*)Ast_GetNodeRef(&ctx->ast, handle);

	fprintf(fp, "op: ");
	Token *op_token = &(ctx->tokens.tokens[op->op]);
	print_token(fp, &ctx->src, op_token);

	fprintf(fp, ",\nlhs: ");
	Ast_PrettyPrint_internal(ctx, fp, op->lhs);

	fprintf(fp, ",\nrhs: ");
	Ast_PrettyPrint_internal(ctx, fp, op->rhs);

	fprintf(fp, "\n}");
}

void Ast_Print_PostOp(ParserCtx *ctx, FILE *fp, AstNodeHandle handle) {
	fprintf(fp, "PostOp: {\n");

	AstPostOp *op = (AstPostOp*)Ast_GetNodeRef(&ctx->ast, handle);

	fprintf(fp, "op: ");
	Token *op_token = &(ctx->tokens.tokens[op->op]);
	print_token(fp, &ctx->src, op_token);

	fprintf(fp, ",\nval: ");
	Ast_PrettyPrint_internal(ctx, fp, op->val);

	fprintf(fp, "\n}");
}

void Ast_Print_UnaryOp(ParserCtx *ctx, FILE *fp, AstNodeHandle handle) {
	fprintf(fp, "UnaryOp: {\n");

	AstUnaryOp *op = (AstUnaryOp*)Ast_GetNodeRef(&ctx->ast, handle);

	fprintf(fp, "op: ");
	Token *op_token = &(ctx->tokens.tokens[op->op]);
	print_token(fp, &ctx->src, op_token);

	fprintf(fp, ",\nval: ");
	Ast_PrettyPrint_internal(ctx, fp, op->val);

	fprintf(fp, "\n}");
}

void Ast_Print_Declarator(ParserCtx *ctx, FILE *fp, AstNodeHandle handle) {
	fprintf(fp, "Declarator: {\n");

	AstDeclarator *stmt = (AstDeclarator*)Ast_GetNodeRef(&ctx->ast, handle);

	fprintf(fp, "ident: ");
	Token *ident_token = &(ctx->tokens.tokens[stmt->ident]);
	print_token(fp, &ctx->src, ident_token);

	fprintf(fp, ",\ntypename: ");
	Token *typename_token = &(ctx->tokens.tokens[stmt->typename]);
	print_token(fp, &ctx->src, typename_token);

	fprintf(fp, ",\nexpr: ");
	Ast_PrettyPrint_internal(ctx, fp, stmt->expr);

	fprintf(fp, "\n}");
}

