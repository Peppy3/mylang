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

#define PARSER_FUNC(name) static AstNodeHandle parse_##name(ParserCtx *this)
#define PARSE(func) parse_##func(this)
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

	while (ctx->src.data[line_pos] != '\n') {
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
TokenPos parser_next(ParserCtx *ctx) {
	return TokenStream_Iter(&ctx->tokens);
}

PARSER_FUNC(statement) {
	AstNodeHandle handle = AST_INVALID_HANDLE;
	
	if (CURRENT.type != TOKEN_integer) {
		ERROR("I did not get an integer\n");
		return handle;
	}

	TokenPos token = NEXT();

	handle = Ast_AllocNode(&this->ast, AstLiteral);
	AstNode *node = Ast_GetNodeRef(&this->ast, handle);
	node[0] = AST_TYPE_Literal;
	node[1] = token;
	
	if (CURRENT.type != TOKEN_semicolon) {
		ERROR("No semicolon at the end of statement\n");
		return AST_INVALID_HANDLE;
	}
	NEXT();

	return handle;
}

PARSER_FUNC(statement_list) {
	if (CURRENT.type != TOKEN_lcurly) {
		return AST_INVALID_HANDLE;
	}
	NEXT();

	AstNodeHandle list_head = AST_INVALID_HANDLE;
	if (CURRENT.type != TOKEN_rcurly || CURRENT.type == TOKEN_eof) {
		list_head = Ast_ListAppend(&this->ast, list_head);
		AstNodeHandle curr = list_head;
		while (CURRENT.type != TOKEN_rcurly || CURRENT.type == TOKEN_eof) {
			PARSE(statement);
			if (CURRENT.type != TOKEN_rcurly || CURRENT.type == TOKEN_eof) {
				break;
			}
			curr = Ast_ListAppend(&this->ast, curr);
		}
	}
	else {
		list_head = Ast_AllocNode(&this->ast, AstNone);
		AstNode *node = Ast_GetNodeRef(&this->ast, list_head);
		node[0] = AST_TYPE_None;
	}

	if (CURRENT.type == TOKEN_eof) {
		ERROR("Unexpected EOF Missing right curly brace\n");
		return AST_INVALID_HANDLE;
	}
	NEXT();

	return list_head;
}

int parse(ParserCtx *this) {
	
	PARSE(statement_list);
	
	return this->num_errors;
}

