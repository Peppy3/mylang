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

typedef struct {
	char *source_path;
	size_t line_pos;
	size_t num_errors;
	ParserFile src;

	Token prev_token;
	Token curr_token;
	Token lookahead;

	Ast ast;
} Parser;

#define PARSER_FUNC(name) AstNodeHandle parse_##name(Parser *this)
#define PARSE(parser_func) ((handle = parse_##parser_func(this)) != -1)
#define CURRENT (this->curr_token)
#define NEXT() parser_next(this)
#define EXPECT(type) parser_expect(this, type)
#define AST_NODE_CONSTRUCT(handle) *Ast_GetNodeRef(&this->ast, handle) = (AstNode)

size_t calc_line_num(const Parser *parser, const Token tok) {
	size_t line_num = 1;
	for (size_t i = 0; i < tok.pos; i++) {
		if (parser->src.data[i] == '\n') {
			line_num++;
		}
	}
	return line_num;
}

__attribute__((format (printf, 3, 4)))
void parser_error(Parser *parser, Token *tok, const char *err_fmt, ...) {
	const size_t TAB_LEN = 4;
	parser->num_errors += 1;
	size_t line_num = calc_line_num(parser, *tok);
	size_t col_num = tok->pos - tok->line_pos;
	
	fprintf(stderr, "%s:%lu:%lu ", parser->source_path,
		line_num, col_num + 1);

	va_list vargs;
	va_start(vargs, err_fmt);
	vfprintf(stderr, err_fmt, vargs);
	va_end(vargs);

	ParserFile_PrintLine(parser->src, tok->line_pos, TAB_LEN, stderr);

	// FIXME: fix tab spacing (i know it's broken)
	for (size_t i = tok->line_pos; i < (tok->line_pos + col_num); i++) {
		if (parser->src.data[i] == '\t') {
			for (size_t j = 0; j < TAB_LEN; j++) {
				fputc(' ', stderr);
			}
		}
		else {
			fputc(' ', stderr);
		}
	}
	fprintf(stderr, "^\n\n");

}

Token parser_next(Parser *parser) {
	parser->prev_token = parser->curr_token;
	parser->curr_token = parser->lookahead;
	parser->lookahead = NextToken(&parser->src, &parser->line_pos);
	return parser->prev_token;
}

bool parser_expect(Parser *parser, enum TokenType expected_type) {
	if (parser->curr_token.type != expected_type) {
		parser_error(parser, &parser->curr_token, "Expected %s but got %s\n", 
			Token_GetStringRep(expected_type), 
			Token_GetStringRep(parser->curr_token.type)
		);
		return false;
	}

	parser->curr_token = parser->lookahead;
	parser->lookahead = NextToken(&parser->src, &parser->line_pos);

	return true;
}

PARSER_FUNC(expr);

PARSER_FUNC(constant) {
	AstNodeHandle handle = AST_INVALID_HANDLE;
	// add more constants later
	if (Token_is_literal(this->curr_token)) {
		handle = Ast_AllocNode(&this->ast);
		AST_NODE_CONSTRUCT(handle){.type = AST_NODE_LITERAL, .literal.tok = NEXT()};
	}
	return handle;
}

PARSER_FUNC(identifier) {
	AstNodeHandle handle = AST_INVALID_HANDLE;
	if (CURRENT.type == TOKEN_identifier) {
		handle = Ast_AllocNode(&this->ast);
		AST_NODE_CONSTRUCT(handle){.type = AST_NODE_LITERAL, .literal.tok = NEXT()};
	}
	return AST_INVALID_HANDLE;
}

PARSER_FUNC(primary_expr) {
	AstNodeHandle handle = AST_INVALID_HANDLE;
	if (PARSE(identifier)) {
		return handle;
	}
	if (PARSE(constant)) {
		return handle;
	}
	if (CURRENT.type == TOKEN_lparen) {
		NEXT();

		if (PARSE(primary_expr)) {
			return handle;
		}
		if (CURRENT.type != TOKEN_rparen) {
			parser_error(this, &CURRENT, "Unmatched parenthesis\n");
			return AST_INVALID_HANDLE;
		}
		NEXT();
	}
	return handle;
}

PARSER_FUNC(expr_statement) {
	AstNodeHandle handle = AST_INVALID_HANDLE;
	
	if (PARSE(primary_expr)) {
		if (CURRENT.type != TOKEN_semicolon) {
			parser_error(this, &this->prev_token, 
				"Unexpected token. No semicolon at the end of statemt\n");
			return AST_INVALID_HANDLE;
		}
		NEXT();
	}
	else if (EXPECT(TOKEN_semicolon)) {
		handle = Ast_AllocNode(&this->ast);
		Ast_GetNodeRef(&this->ast, handle)->type = AST_NODE_NONE;
	}

	return handle;
}

PARSER_FUNC(expr_stmt_list) {
	AstNodeHandle list_back = AST_INVALID_HANDLE;
	AstNodeHandle list_head = AST_INVALID_HANDLE;

	if (CURRENT.type != TOKEN_lcurly) {
		return AST_INVALID_HANDLE;
	}
	NEXT();

	while (CURRENT.type != TOKEN_rcurly) {

		list_back = Ast_ListAppend(&this->ast, list_back);
		AstNodeHandle stmt = parse_expr_statement(this);

		Ast_GetNodeRef(&this->ast, list_back)->list.val = stmt;
		if (list_head == AST_INVALID_HANDLE) {
			list_head = list_back;
		}
	}
	NEXT();

	return list_head;
}

PARSER_FUNC(linkname) {
	if (CURRENT.type != TOKEN_linkname) {
		return AST_INVALID_HANDLE;
	}
	NEXT();

	if (CURRENT.type != TOKEN_string) {
		return AST_INVALID_HANDLE;
	}

	Token name = NEXT();
	AstNodeHandle handle = Ast_AllocNode(&this->ast);
	AST_NODE_CONSTRUCT(handle) {.type = AST_NODE_LINKNAME, .linkname.name = name};

	if (CURRENT.type != TOKEN_semicolon) {
		parser_error(this, &this->prev_token, "No semicolon at the end of linkname statement\n");
		return AST_INVALID_HANDLE;
	}
	NEXT();

	return handle;
}

PARSER_FUNC(external_def) {
	AstNodeHandle handle = AST_INVALID_HANDLE;
	if (PARSE(linkname)) {
		return handle;
	}
	
	return handle;
}

int parse(char *source_path) {
	assert(source_path != NULL);
	Parser parser = {
		.source_path = source_path,
		.line_pos = 0,
		.num_errors = 0,
	};

	if (ParserFile_Open(&parser.src, source_path) != 0) {
		return -1;
	}
	if (Ast_New(&parser.ast)) {
		ParserFile_Close(parser.src);
		return -1;
	}
	
	parser.curr_token = NextToken(&parser.src, &parser.line_pos);
	parser.lookahead = NextToken(&parser.src, &parser.line_pos);
	
	AstNodeHandle list_back = AST_INVALID_HANDLE;
	while (parser.curr_token.type != TOKEN_eof) {
		if (parser.curr_token.type == TOKEN_invalid) {
			parser_error(&parser, &parser.curr_token, "Invalid token\n");
		}

		list_back = Ast_ListAppend(&parser.ast, list_back);
		AstNodeHandle stmt_list = parse_expr_stmt_list(&parser);

		Ast_GetNodeRef(&parser.ast, list_back)->list.val = stmt_list;
		if (stmt_list == AST_INVALID_HANDLE) {
			parser_error(&parser, &parser.curr_token, "Expected statement list\n");
			parser_next(&parser);
		}
	}

	Ast_PrettyPrint(&parser.ast, &parser.src, 0, stdout);

	Ast_Delete(parser.ast);
	ParserFile_Close(parser.src);
	return parser.num_errors;
}

