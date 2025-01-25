#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>

#include <string.h>
#include <stdio.h>

#include "ParserFile.h"
#include "Token.h"
#include "Tokenizer.h"
#include "Ast.h"

#include "Parser.h"

#define PARSER_FUNC(name, ...) \
	static AstNodeHandle parse_##name(ParserCtx *this __VA_OPT__(,) __VA_ARGS__)

#define PARSE(func, ...) parse_##func(this __VA_OPT__(,) __VA_ARGS__)
#define ERROR(msg, ...) parser_error(this, msg __VA_OPT__(,) __VA_ARGS__)
#define CURRENT (this->tokens.tokens[this->tokens.pos])
#define LOOKAHEAD (this->tokens.tokens[this->tokens.pos + 1])
#define AST (this->ast)
#define NEXT() parser_next(this)

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
	Token tok = CURRENT;
	const size_t tab_len = 4;
	this->num_errors += 1;
	size_t line_num = calc_line_num(this, &tok);
	size_t line_pos = calc_line_pos(this, &tok);
	size_t col_num = tok.pos - line_pos;
	
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
TokenPos parser_next(ParserCtx *this) {
	if (CURRENT.type == TOKEN_invalid) {
		ERROR("Invalid token\n");
	}
	TokenPos pos = TokenStream_Iter(&this->tokens);
	return pos;
}

PARSER_FUNC(expr);
PARSER_FUNC(assignment_expr);
PARSER_FUNC(statement);

PARSER_FUNC(primary_expr) {
	AstNodeHandle handle = AST_INVALID_HANDLE;

	if (Token_is_literal(CURRENT) || CURRENT.type == TOKEN_identifier) {
		TokenPos token_pos = NEXT();
		handle = Ast_Make_Literal(&this->ast, (AstLiteral){
			.type = AST_TYPE_Literal,
			.val = token_pos,
		});
		return handle;
	}

	if (CURRENT.type != TOKEN_lparen) {
		return AST_INVALID_HANDLE;
	}
	NEXT();

	handle = PARSE(expr);

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
	TokenPos lsquare_pos = NEXT();

	AstNodeHandle index_expr = PARSE(expr);

	if (CURRENT.type != TOKEN_rsquare) {
		ERROR("Missing right square bracket after index expression\n");
		return AST_INVALID_HANDLE;
	}
	NEXT();

	return Ast_Make_BinOp(&this->ast, (AstBinOp){
		.type = AST_TYPE_BinOp,
		.op = lsquare_pos,
		.lhs = slice,
		.rhs = index_expr,
	});
}

PARSER_FUNC(func_call, AstNodeHandle func) {
	if (CURRENT.type != TOKEN_lparen) {
		ERROR("Unreachable \n");
		return AST_INVALID_HANDLE;
	}

	TokenPos lparen_pos = NEXT();

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
		.op = lparen_pos,
		.lhs = func,
		.rhs = args_head,
	});
}

PARSER_FUNC(postfix_expr, AstNodeHandle primary) {
	if (primary == AST_INVALID_HANDLE) {
		primary = PARSE(primary_expr);
	}
	
	if (CURRENT.type == TOKEN_inc || CURRENT.type == TOKEN_dec) {
		TokenPos op_pos = NEXT();
		AstNodeHandle op_handle = Ast_Make_PostOp(&this->ast, (AstPostOp){
			.type = AST_TYPE_PostOp,
			.op = op_pos,
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

	TokenPos unary_pos = NEXT();
	
	AstNodeHandle val = PARSE(unary_expr);

	return Ast_Make_UnaryOp(&this->ast, (AstUnaryOp){
		.type = AST_TYPE_UnaryOp,
		.op = unary_pos,
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

		TokenPos op = NEXT();

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

	TokenPos assignment_pos = NEXT();

	AstNodeHandle rhs = PARSE(binary_expr);
	
	AstNodeHandle handle = Ast_Make_BinOp(&this->ast, (AstBinOp){
		.type = AST_TYPE_BinOp,
		.op = assignment_pos,
		.lhs = lhs,
		.rhs = rhs,
	});

	return handle;
}

PARSER_FUNC(expr) {
	AstNodeHandle handle = PARSE(assignment_expr);

	return handle;
}

PARSER_FUNC(expr_statement) {
	if (CURRENT.type == TOKEN_semicolon) {
		NEXT();
		return Ast_Make_None(&this->ast, (AstNone){.type = AST_TYPE_None});
	}

	AstNodeHandle handle = PARSE(expr);

	if (handle == AST_INVALID_HANDLE) {
		ERROR("Could not parse expression\n");
		while (!Token_is_terminating(CURRENT)) {
			NEXT();
		}
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

PARSER_FUNC(declarator) {
	TokenPos ident_pos = NEXT();
	NEXT();

	if (!Token_is_type_specifier(CURRENT)) {
		ERROR("Missing type specifier in declaration\n");
		return AST_INVALID_HANDLE;
	}
	TokenPos type_pos = NEXT();

	AstNodeHandle decl_handle = Ast_Make_Declarator(&AST, (AstDeclarator){
		.type = AST_TYPE_Declarator,
		.ident = ident_pos,
		.typename = type_pos,
	});

	if (CURRENT.type == TOKEN_semicolon) {
		AstNodeHandle none_expr = Ast_Make_None(&AST, (AstNone){.type = AST_TYPE_None});

		AstDeclarator *ref = (AstDeclarator*)Ast_GetNodeRef(&AST, decl_handle);
		ref->expr = none_expr;

		return decl_handle;
	}
	else if (CURRENT.type != TOKEN_assignment) {
		ERROR("Missing assignment operator at in declaration\n");
		return AST_INVALID_HANDLE;
	}
	NEXT();

	AstNodeHandle expr = PARSE(expr_statement);

	AstDeclarator *ref = (AstDeclarator*)Ast_GetNodeRef(&AST, decl_handle);
	ref->expr = expr;

	return decl_handle;
}

PARSER_FUNC(statement) {
	if (CURRENT.type == TOKEN_identifier && LOOKAHEAD.type == TOKEN_colon) {
		return PARSE(declarator);
	}
	else {
		return PARSE(expr_statement);
	}
}

PARSER_FUNC(statement_list, AstNodeHandle list) {
	if (CURRENT.type == TOKEN_eof) {
		return AST_INVALID_HANDLE;
	}
	
	AstNodeHandle back = Ast_ListAppend(&AST, list);
	back = Ast_ListAddVal(&AST, back, PARSE(statement));
	
	return PARSE(statement_list, back);
}

int parse(ParserCtx *this) {
	
	PARSE(statement_list, AST_INVALID_HANDLE);
	
	return this->num_errors;
}

