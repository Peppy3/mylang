#include <stdint.h>

#include <stdio.h>

#include <util.h>
#include <tokenizer/Token.h>
#include <CompileUnit.h>
#include <ast/Ast.h>

#include <ast/AstPrint.h>

void Ast_PrettyPrint_internal(CompileUnit *unit, FILE *fp, AstNodeHandle handle) {
	if (handle == AST_INVALID_HANDLE) {
		fprintf(fp, "INVALID_HANDLE");
		return;
	}

	AstNode *node = Ast_GetNodeRef(&unit->ast, handle);
// I know this is kinda undebuggable but it's 
// much better than having to miss printing something
// Workaround is to break at the function you're interested in
	switch (*node) {
#define X(name)\
		case AST_TYPE_##name: {\
			Ast_Print_##name(unit, fp, handle);\
		} break;
AST_NODE_X_LIST
#undef X
	}
}

void Ast_PrettyPrint(CompileUnit *unit, FILE *fp) {
	fprintf(fp, "Ast for %s:\n\n", unit->source_path);

	Ast_PrettyPrint_internal(unit, fp, 0);
	fputc('\n', fp);
}

void Ast_Print_None(CompileUnit *unit, FILE *fp, AstNodeHandle handle) {
	fprintf(fp, "None");
}

void Ast_Print_Literal(CompileUnit *unit, FILE *fp, AstNodeHandle handle) {
	AstLiteral *lit = (AstLiteral*)Ast_GetNodeRef(&unit->ast, handle);

	print_token(fp, &unit->src, &lit->val);
}

void Ast_Print_List(CompileUnit *unit, FILE *fp, AstNodeHandle handle) {
	AstList *list;

	fprintf(fp, "List: {\n");

	for (;;) {

		list = (AstList*)Ast_GetNodeRef(&unit->ast, handle);
		Ast_PrettyPrint_internal(unit, fp, list->val);

		if (list->next == AST_INVALID_HANDLE) {
			break;
		}
		fprintf(fp, ",\n");
		handle = list->next;
		list = (AstList*)Ast_GetNodeRef(&unit->ast, handle);
	}

	fprintf(fp, "\n}");
}


void Ast_Print_BinOp(CompileUnit *unit, FILE *fp, AstNodeHandle handle) {
	fprintf(fp, "BinOp: {\n");
	
	AstBinOp *op = (AstBinOp*)Ast_GetNodeRef(&unit->ast, handle);

	fprintf(fp, "op: ");
	print_token(fp, &unit->src, &op->op);

	fprintf(fp, ",\nlhs: ");
	Ast_PrettyPrint_internal(unit, fp, op->lhs);

	fprintf(fp, ",\nrhs: ");
	Ast_PrettyPrint_internal(unit, fp, op->rhs);

	fprintf(fp, "\n}");
}

void Ast_Print_PostOp(CompileUnit *unit, FILE *fp, AstNodeHandle handle) {
	fprintf(fp, "PostOp: {\n");

	AstPostOp *op = (AstPostOp*)Ast_GetNodeRef(&unit->ast, handle);

	fprintf(fp, "op: ");
	print_token(fp, &unit->src, &op->op);

	fprintf(fp, ",\nval: ");
	Ast_PrettyPrint_internal(unit, fp, op->val);

	fprintf(fp, "\n}");
}

void Ast_Print_UnaryOp(CompileUnit *unit, FILE *fp, AstNodeHandle handle) {
	fprintf(fp, "UnaryOp: {\n");

	AstUnaryOp *op = (AstUnaryOp*)Ast_GetNodeRef(&unit->ast, handle);

	fprintf(fp, "op: ");
	print_token(fp, &unit->src, &op->op);

	fprintf(fp, ",\nval: ");
	Ast_PrettyPrint_internal(unit, fp, op->val);

	fprintf(fp, "\n}");
}

void Ast_Print_Declaration(CompileUnit *unit, FILE *fp, AstNodeHandle handle) {
	fprintf(fp, "Declaration: {\n");

	AstDeclaration *stmt = (AstDeclaration*)Ast_GetNodeRef(&unit->ast, handle);

	fprintf(fp, "ident: ");
	print_token(fp, &unit->src, &stmt->ident);

	fprintf(fp, ",\ntype_expr: ");
	Ast_PrettyPrint_internal(unit, fp, stmt->type_expr);

	fprintf(fp, ",\nexpr: ");
	Ast_PrettyPrint_internal(unit, fp, stmt->expr);

	fprintf(fp, "\n}");
}

void Ast_Print_FuncType(CompileUnit *unit, FILE *fp, AstNodeHandle handle) {
	fprintf(fp, "FuncType: {\n");

	AstFuncType *type = (AstFuncType*)Ast_GetNodeRef(&unit->ast, handle);

	fprintf(fp, "args: ");
	Ast_PrettyPrint_internal(unit, fp, type->args);

	fprintf(fp, ",\nreturns: ");
	Ast_PrettyPrint_internal(unit, fp, type->returns);

	fprintf(fp, "\n}");
}

void Ast_Print_ReturnStmt(CompileUnit *unit, FILE *fp, AstNodeHandle handle) {
	fprintf(fp, "ReturnStmt: {\n");

	AstReturnStmt *stmt = (AstReturnStmt*)Ast_GetNodeRef(&unit->ast, handle);

	fprintf(fp, "expr: ");
	Ast_PrettyPrint_internal(unit, fp, stmt->expr);

	fprintf(fp, "\n}");
}

void Ast_Print_IfStmt(CompileUnit *unit, FILE *fp, AstNodeHandle handle) {
	fprintf(fp, "IfStmt: {\n");

	AstIfStmt *stmt = (AstIfStmt*)Ast_GetNodeRef(&unit->ast, handle);

	fprintf(fp, "expr: ");
	Ast_PrettyPrint_internal(unit, fp, stmt->expr);

	fprintf(fp, ",\nif_block: ");
	Ast_PrettyPrint_internal(unit, fp, stmt->if_block);

	fprintf(fp, ",\nelse_block: ");
	Ast_PrettyPrint_internal(unit, fp, stmt->else_block);

	fprintf(fp, "\n}");
}

