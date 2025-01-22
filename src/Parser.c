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
#define NEXT() parser_next(this)

size_t calc_line_num(const ParserCtx *ctx, const Token *tok) {
	size_t line_num = 1;
	for (size_t i = 0; i < tok->pos; i++) {
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

PARSER_FUNC(primary_expr) {
	AstNodeHandle handle = AST_INVALID_HANDLE;

	if (Token_is_literal(CURRENT) || CURRENT.type == TOKEN_identifier) {
		TokenPos token_pos = NEXT();
		handle = Ast_AllocNode(&this->ast, AstLiteral);
		AstLiteral *ref = (AstLiteral*)Ast_GetNodeRef(&this->ast, handle);
		ref->type = AST_TYPE_Literal;
		ref->val = token_pos;
		return handle;
	}

	if (CURRENT.type != TOKEN_lparen) {
		return AST_INVALID_HANDLE;
	}
	NEXT();

	handle = PARSE(expr);

	if (CURRENT.type != TOKEN_rparen) {
		return AST_INVALID_HANDLE;
	}
	NEXT();

	return handle;
}

PARSER_FUNC(postfix_expr, AstNodeHandle primary) {
	if (primary == AST_INVALID_HANDLE) {
		primary = PARSE(primary_expr);
	}
	
	if (CURRENT.type == TOKEN_inc || CURRENT.type == TOKEN_dec) {
		TokenPos op_pos = NEXT();
		AstNodeHandle op_handle = Ast_AllocNode(&this->ast, AstPostOp);
		AstPostOp *op = (AstPostOp*)Ast_GetNodeRef(&this->ast, op_handle);
		*op = (AstPostOp){.type = AST_TYPE_PostOp, .op = op_pos, .val = primary};
		
		primary = PARSE(postfix_expr, op_handle);
	}
	
	return primary;
}

PARSER_FUNC(assignment_expr) {
	
	AstNodeHandle lhs = PARSE(primary_expr);
	if (!Token_is_assignment(CURRENT) || lhs == AST_INVALID_HANDLE) {
		return lhs;
	}

	TokenPos assignment_pos = NEXT();

	AstNodeHandle rhs = PARSE(postfix_expr, AST_INVALID_HANDLE);
	
	AstNodeHandle handle = Ast_AllocNode(&this->ast, AstBinOp);
	AstBinOp *assign = (AstBinOp*)Ast_GetNodeRef(&this->ast, handle);
	*assign = (AstBinOp) {
		.type = AST_TYPE_BinOp,
		.op = assignment_pos,
		.lhs = lhs,
		.rhs = rhs,
	};

	return handle;
}

PARSER_FUNC(expr) {
	AstNodeHandle handle = PARSE(assignment_expr);

	return handle;
}

PARSER_FUNC(statement) {
	if (CURRENT.type == TOKEN_semicolon) {
		NEXT();
		AstNodeHandle none = Ast_AllocNode(&this->ast, AstNone);
		AstNone *none_ref = (AstNone*)Ast_GetNodeRef(&this->ast, none);
		none_ref->type = AST_TYPE_None;
		return none;
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

PARSER_FUNC(statement_list) {
	if (CURRENT.type != TOKEN_lcurly) {
		ERROR("Unexpected %s Missing left curly brace\n", Token_GetStringRep(CURRENT.type));
		return AST_INVALID_HANDLE;
	}
	NEXT();

	AstNodeHandle list_head = AST_INVALID_HANDLE;
	if (CURRENT.type != TOKEN_rcurly || CURRENT.type == TOKEN_eof) {
		list_head = Ast_ListAppend(&this->ast, list_head);
		AstNodeHandle back = list_head;
		for (;;) {
			AstNodeHandle stmt = PARSE(statement);
			AstList *list_ref = (AstList*)Ast_GetNodeRef(&this->ast, back);
			list_ref->val = stmt;

			if (CURRENT.type == TOKEN_rcurly || CURRENT.type == TOKEN_eof) {
				break;
			}
			back = Ast_ListAppend(&this->ast, back);
		}
	}
	else {
		list_head = Ast_AllocNode(&this->ast, AstNone);
		AstNone *none = (AstNone*)Ast_GetNodeRef(&this->ast, list_head);
		none->type = AST_TYPE_None;
	}

	if (CURRENT.type == TOKEN_eof) {
		ERROR("Unexpected EOF. Missing right curly brace\n");
		return AST_INVALID_HANDLE;
	}
	NEXT();

	return list_head;
}

int parse(ParserCtx *this) {
	
	PARSE(statement_list);
	
	return this->num_errors;
}

