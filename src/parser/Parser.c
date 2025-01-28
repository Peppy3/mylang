#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
#include <setjmp.h>

#include <string.h>
#include <stdio.h>

#include <ParserFile.h>
#include <tokenizer/Token.h>
#include <tokenizer/Tokenizer.h>
#include <ast/Ast.h>

#include <parser/Parser.h>

#define PARSER_FUNC(name, ...) \
	static AstNodeHandle parse_##name(ParserCtx *this __VA_OPT__(,) __VA_ARGS__)

#define PARSE(func, ...) parse_##func(this __VA_OPT__(,) __VA_ARGS__)
#define ERROR(msg, ...) parser_error(this, msg __VA_OPT__(,) __VA_ARGS__)
#define CURRENT (this->current_token)
#define LOOKAHEAD (this->lookahead_token)
#define AST (this->ast)
#define NEXT() parser_next(this)

static jmp_buf parser_env;

size_t calc_line_num(const ParserCtx *ctx, const Token *tok) {
	size_t line_num = 1;
	for (int32_t i = 0; i < tok->pos; i++) {
		if (ctx->src.data[i] == '\n') {
			line_num++;
		}
	}
	return line_num;
}

size_t calc_line_pos(const ParserCtx *ctx, const Token *tok) {
	size_t line_pos = tok->pos;

	while (line_pos > 0 && ctx->src.data[line_pos] != '\n') {
		line_pos -= 1;
	}

	return line_pos + 1;
}

__attribute__((format (printf, 2, 3)))
void parser_error(ParserCtx *this, const char *err_fmt, ...) {
	this->num_errors += 1;

	if (CURRENT.type == TOKEN_eof) {
		fprintf(stderr, "%s: ", this->source_path);

		va_list vargs;
		va_start(vargs, err_fmt);
		vfprintf(stderr, err_fmt, vargs);
		va_end(vargs);

		fprintf(stderr, "Got an error but reached end of file\n");
		longjmp(parser_env, 1);
	}

	const size_t tab_len = 4;
	size_t line_num = calc_line_num(this, &this->current_token);
	size_t line_pos = calc_line_pos(this, &this->current_token);
	size_t col_num = this->current_token.pos - line_pos;
	
	fprintf(stderr, "%s:%lu:%lu ", this->source_path,
		line_num, col_num + 1);

	va_list vargs;
	va_start(vargs, err_fmt);
	vfprintf(stderr, err_fmt, vargs);
	va_end(vargs);


	ParserFile_PrintLine(this->src, line_pos, tab_len, stderr);

	// FIXME: tab spacing (i know it's broken)
	for (size_t i = line_pos; i < (line_pos + col_num); i++) {
		if (this->src.data[i] == '\t') {
			for (size_t j = 0; j < tab_len; j++) {
				fputc(' ', stderr);
			}
		}
		else {
			fputc(' ', stderr);
		}
	}
	fprintf(stderr, "^\n\n");
}

// gives the idex of the token or -1 if it's the end of the list
Token parser_next(ParserCtx *this) {
	Token next = CURRENT;
	CURRENT = LOOKAHEAD;
	LOOKAHEAD = NextToken(&this->src);

	if (CURRENT.type == TOKEN_invalid) {
		ERROR("Invalid token\n");
	}

	return next;
}

PARSER_FUNC(expression);
PARSER_FUNC(assignment_expr);
PARSER_FUNC(declaration_list, AstNodeHandle list);
PARSER_FUNC(statement);
PARSER_FUNC(compound_statement);

PARSER_FUNC(primary_expr) {
	AstNodeHandle handle = AST_INVALID_HANDLE;

	if (Token_is_literal(CURRENT) 
		|| CURRENT.type == TOKEN_identifier 
		|| Token_is_type_specifier(CURRENT)) {

		Token token = NEXT();
		handle = Ast_Make_Literal(&this->ast, (AstLiteral){
			.type = AST_TYPE_Literal,
			.val = token,
		});
		return handle;
	}

	if (CURRENT.type != TOKEN_lparen) {
		return AST_INVALID_HANDLE;
	}
	NEXT();

	handle = PARSE(expression);

	if (CURRENT.type != TOKEN_rparen) {
		ERROR("No closing paren. Got %s\n", Token_GetStringRep(CURRENT.type));
		return AST_INVALID_HANDLE;
	}
	NEXT();

	return handle;
}

PARSER_FUNC(slice_subscript, AstNodeHandle slice) {
	if (CURRENT.type != TOKEN_lsquare) {
		ERROR("Unreachable\n");
		return AST_INVALID_HANDLE;
	}
	Token lsquare = NEXT();

	AstNodeHandle index_expr = PARSE(expression);

	if (CURRENT.type != TOKEN_rsquare) {
		ERROR("Missing right square bracket after index expression\n");
		return AST_INVALID_HANDLE;
	}
	NEXT();

	return Ast_Make_BinOp(&this->ast, (AstBinOp){
		.type = AST_TYPE_BinOp,
		.op = lsquare,
		.lhs = slice,
		.rhs = index_expr,
	});
}

PARSER_FUNC(func_call, AstNodeHandle func) {
	if (CURRENT.type != TOKEN_lparen) {
		ERROR("Unreachable \n");
		return AST_INVALID_HANDLE;
	}

	Token lparen = NEXT();

	AstNodeHandle args_head = Ast_ListAppend(&AST, AST_INVALID_HANDLE);
	args_head = Ast_ListAddVal(&AST, args_head, PARSE(assignment_expr));

	if (CURRENT.type == TOKEN_comma) {
		NEXT();

		AstNodeHandle args_back = args_head;
		while (CURRENT.type != TOKEN_rparen && CURRENT.type != TOKEN_eof) {

			args_back = Ast_ListAppend(&AST, args_back);
			args_back = Ast_ListAddVal(&AST, args_back, PARSE(assignment_expr));

			if (CURRENT.type != TOKEN_comma) {
				break;
			}

			NEXT();
		}
	}
	if (CURRENT.type != TOKEN_rparen) {
		ERROR("Missing right paren in function call\n");
		return AST_INVALID_HANDLE;
	}
	NEXT();

	return Ast_Make_BinOp(&AST, (AstBinOp){
		.type = AST_TYPE_BinOp,
		.op = lparen,
		.lhs = func,
		.rhs = args_head,
	});
}

PARSER_FUNC(postfix_expr, AstNodeHandle primary) {
	if (primary == AST_INVALID_HANDLE) {
		primary = PARSE(primary_expr);
	}
	
	if (CURRENT.type == TOKEN_inc || CURRENT.type == TOKEN_dec) {
		Token op = NEXT();
		AstNodeHandle op_handle = Ast_Make_PostOp(&this->ast, (AstPostOp){
			.type = AST_TYPE_PostOp,
			.op = op,
			.val = primary,
		});
		
		primary = PARSE(postfix_expr, op_handle);
	}
	else if (CURRENT.type == TOKEN_lsquare) {
		AstNodeHandle subscipt = PARSE(slice_subscript, primary);
		primary = PARSE(postfix_expr, subscipt);
	}
	else if (CURRENT.type == TOKEN_lparen) {
		AstNodeHandle call = PARSE(func_call, primary);
		primary = PARSE(postfix_expr, call);
	}
	
	return primary;
}


PARSER_FUNC(unary_expr) {
	if (!Token_is_unary(CURRENT)) {
		return PARSE(postfix_expr, AST_INVALID_HANDLE);
	}

	Token unary = NEXT();
	
	AstNodeHandle val = PARSE(unary_expr);

	return Ast_Make_UnaryOp(&this->ast, (AstUnaryOp){
		.type = AST_TYPE_UnaryOp,
		.op = unary,
		.val = val,
	});
}

// Operator precedence parser for binary operators
PARSER_FUNC(binary_expr1, AstNodeHandle lhs, int32_t min_precedence) {
	
	if (lhs == AST_INVALID_HANDLE) {
		lhs = PARSE(unary_expr);
	}

	for (;;) {
		
		int32_t op_prec = Token_get_precedence(CURRENT);

		if (op_prec < min_precedence) {
			return lhs;
		}

		Token op = NEXT();

		AstNodeHandle rhs = PARSE(binary_expr1, AST_INVALID_HANDLE, op_prec + 1);
	
		lhs = Ast_Make_BinOp(&this->ast, (AstBinOp){
			.type = AST_TYPE_BinOp,
			.op = op,
			.lhs = lhs,
			.rhs = rhs
		});
	}
}

PARSER_FUNC(binary_expr) {
	return PARSE(binary_expr1, AST_INVALID_HANDLE, TOKEN_MIN_PRECEDENCE + 1);
}

PARSER_FUNC(assignment_expr) {
	
	AstNodeHandle lhs = PARSE(binary_expr);
	if (!Token_is_assignment(CURRENT) || lhs == AST_INVALID_HANDLE) {
		return lhs;
	}

	Token assignment = NEXT();

	AstNodeHandle rhs = PARSE(binary_expr);
	
	AstNodeHandle handle = Ast_Make_BinOp(&this->ast, (AstBinOp){
		.type = AST_TYPE_BinOp,
		.op = assignment,
		.lhs = lhs,
		.rhs = rhs,
	});

	return handle;
}

PARSER_FUNC(expression) {
	AstNodeHandle handle = PARSE(assignment_expr);

	return handle;
}

PARSER_FUNC(expr_statement) {
	if (CURRENT.type == TOKEN_semicolon) {
		NEXT();
		return Ast_Make_None(&this->ast, (AstNone){.type = AST_TYPE_None});
	}

	AstNodeHandle handle = PARSE(expression);

	if (handle == AST_INVALID_HANDLE) {
		ERROR("Could not parse expression\n");
		NEXT();
		return AST_INVALID_HANDLE;
	}
	
	if (CURRENT.type != TOKEN_semicolon) {
		ERROR("No semicolon at the end of statement\n");
		NEXT();
		return AST_INVALID_HANDLE;
	}
	NEXT();

	return handle;
}

PARSER_FUNC(func_type, Token ident) {
	AstNodeHandle decl_handle = Ast_Make_Declaration(&AST, (AstDeclaration){
		.type = AST_TYPE_Declaration,
		.ident = ident,
	});
	NEXT(); // TOKEN_lparen
	
	AstNodeHandle type_handle = Ast_AllocNode(&AST, AstFuncType);

	AstNodeHandle args = AST_INVALID_HANDLE;
	if (CURRENT.type == TOKEN_rparen) {
		args = Ast_Make_None(&AST, (AstNone){.type = AST_TYPE_None});
	}
	else {
		args = PARSE(declaration_list, AST_INVALID_HANDLE);
	}

	if (CURRENT.type != TOKEN_rparen) {
		ERROR("Missing closing paren in argument list\n");
		return AST_INVALID_HANDLE;
	}
	NEXT();
	
	if (CURRENT.type != TOKEN_arrow) {
		ERROR("Missing right arrow in funciton type\n");
		return AST_INVALID_HANDLE;
	}
	NEXT();
	
	if (!Token_is_type_specifier(CURRENT)) {
		ERROR("Missing type specifier in variable declaration\n");
		return AST_INVALID_HANDLE;
	}

	AstNodeHandle return_type = Ast_Make_Literal(&AST, (AstLiteral){
		.type = AST_TYPE_Literal,
		.val = NEXT(),
	});

	AstFuncType *func_def = (AstFuncType*)Ast_GetNodeRef(&AST, type_handle);
	*func_def = (AstFuncType){
		.type = AST_TYPE_FuncType,
		.args = args,
		.returns = return_type,
	};

	AstDeclaration *ref = (AstDeclaration*)Ast_GetNodeRef(&AST, decl_handle);
	ref->type_expr = type_handle;
	
	return decl_handle;
}

PARSER_FUNC(var_type, Token ident) {
	AstNodeHandle decl_handle = Ast_Make_Declaration(&AST, (AstDeclaration){
		.type = AST_TYPE_Declaration,
		.ident = ident,
	});

	if (!Token_is_type_specifier(CURRENT)) {
		ERROR("Missing type specifier in variable declaration\n");
		return AST_INVALID_HANDLE;
	}

	AstNodeHandle type_expr = Ast_Make_Literal(&AST, (AstLiteral){
		.type = AST_TYPE_Literal,
		.val = NEXT(),
	});

	AstDeclaration *ref = (AstDeclaration*)Ast_GetNodeRef(&AST, decl_handle);
	ref->type_expr = type_expr;

	return decl_handle;
}

PARSER_FUNC(declaration) {
	Token ident = NEXT();
	NEXT();

	if (CURRENT.type == TOKEN_lparen) {
		return PARSE(func_type, ident);
	}
	else if (Token_is_type_specifier(CURRENT)) {
		return PARSE(var_type, ident);
	}
	else {
		ERROR("Missing type specifier in declaration\n");
		return AST_INVALID_HANDLE;
	}
}

PARSER_FUNC(declaration_list, AstNodeHandle list) {
	list = Ast_ListAppend(&AST, list);

	AstNodeHandle decl = PARSE(declaration);

	AstNodeHandle none_hanlde = Ast_Make_None(&AST, (AstNone){.type = AST_TYPE_None});

	// Patch in the expression here
	AstDeclaration *ref = (AstDeclaration*)Ast_GetNodeRef(&AST, decl);
	ref->expr = none_hanlde;

	Ast_ListAddVal(&AST, list, decl);

	if (CURRENT.type != TOKEN_comma) {
		return list;
	}
	NEXT();

	PARSE(declaration_list, list);
	return list;
}

PARSER_FUNC(declaration_statement) {
	
	AstNodeHandle decl = PARSE(declaration);

	if (decl == AST_INVALID_HANDLE) {
		return AST_INVALID_HANDLE;
	}
	
	AstNodeHandle expr = AST_INVALID_HANDLE;
	if (CURRENT.type == TOKEN_assignment) {
		NEXT();
		expr = PARSE(expr_statement);
	}
	else if (CURRENT.type == TOKEN_lcurly) {
		expr = PARSE(compound_statement);
	}
	else if (CURRENT.type == TOKEN_semicolon) {
		NEXT();
		expr = Ast_Make_None(&AST, (AstNone){.type = AST_TYPE_None});
	}
	else {
		ERROR("Invalid declaration statement\n");
		return AST_INVALID_HANDLE;
	}

	// Patch in the expression here
	AstDeclaration *ref = (AstDeclaration*)Ast_GetNodeRef(&AST, decl);
	ref->expr = expr;
	
	return decl;
}

PARSER_FUNC(return_statement) {
	NEXT();
	return Ast_Make_ReturnStmt(&AST, (AstReturnStmt){
		.type = AST_TYPE_ReturnStmt,
		.expr = PARSE(expr_statement),
	});
}

PARSER_FUNC(if_statement) {
	NEXT();

	AstNodeHandle stmt = Ast_AllocNode(&AST, AstIfStmt);

	AstNodeHandle expr = PARSE(expression);

	AstNodeHandle if_block = PARSE(compound_statement);

	AstNodeHandle else_block = AST_INVALID_HANDLE;
	if (CURRENT.type == TOKEN_else) {
		NEXT();
		if (CURRENT.type == TOKEN_if) {
			else_block = PARSE(if_statement);
		}
		else {
			else_block = PARSE(compound_statement);
		}
	}
	else {
		else_block = Ast_Make_None(&AST, (AstNone){.type = AST_TYPE_None});
	}

	AstIfStmt *stmt_ref = (AstIfStmt*)Ast_GetNodeRef(&AST, stmt);
	*stmt_ref = (AstIfStmt){
		.type = AST_TYPE_IfStmt,
		.expr = expr,
		.if_block = if_block,
		.else_block = else_block,
	};

	return stmt;
}

PARSER_FUNC(statement) {
	if (CURRENT.type == TOKEN_identifier && LOOKAHEAD.type == TOKEN_colon) {
		return PARSE(declaration_statement);
	}
	
	switch (CURRENT.type) {
	
		case TOKEN_return:
			return PARSE(return_statement);
		case TOKEN_if:
			return PARSE(if_statement);
		default:
			return PARSE(expr_statement);
	}
}

PARSER_FUNC(statement_list, AstNodeHandle list) {
	if (CURRENT.type == TOKEN_rcurly || CURRENT.type == TOKEN_eof) {
		return AST_INVALID_HANDLE;
	}
	
	list = Ast_ListAppend(&AST, list);
	list = Ast_ListAddVal(&AST, list, PARSE(statement));
	
	PARSE(statement_list, list);
	return list;
}

PARSER_FUNC(compound_statement) {

	if (CURRENT.type != TOKEN_lcurly) {
		ERROR("Expected left curly brace before compund statement\n");
		return AST_INVALID_HANDLE;
	}
	NEXT();
	
	if (CURRENT.type == TOKEN_rcurly) {
		NEXT();
		return Ast_Make_None(&AST, (AstNone){.type = AST_TYPE_None});
	}

	AstNodeHandle list = PARSE(statement_list, AST_INVALID_HANDLE);

	if (CURRENT.type != TOKEN_rcurly) {
		ERROR("Expected right curly brace after compund statement\n");
		return AST_INVALID_HANDLE;
	}
	NEXT();
	
	return list;
}

int parse(ParserCtx *this) {

	CURRENT = NextToken(&this->src);
	LOOKAHEAD = NextToken(&this->src);

	if (setjmp(parser_env)) {
		return this->num_errors;
	}
	
	PARSE(statement_list, AST_INVALID_HANDLE);
	
	return this->num_errors;
}

