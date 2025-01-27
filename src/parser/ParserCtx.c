
#include <stdlib.h>
#include <string.h>

#include <ParserFile.h>
#include <tokenizer/TokenStream.h>
#include <ast/Ast.h>

#include <parser/ParserCtx.h>

int ParserCtx_Setup(ParserCtx *ctx, char *filepath) {
	size_t filepath_len = strlen(filepath) + 1;
	char *filepath_cpy = malloc(filepath_len);
	if (filepath_cpy == NULL) {
		return -1;
	}
	ctx->source_path = memcpy(filepath_cpy, filepath, filepath_len);
	
	if (ParserFile_Open(&ctx->src, filepath)) {
		free(filepath_cpy);
		return -1;
	}

	if (TokenStream_Generate(&ctx->tokens, &ctx->src)) {
		free(filepath_cpy);
		ParserFile_Close(&ctx->src);
		return -1;
	}

	if (Ast_New(&ctx->ast)) {
		free(filepath_cpy);
		ParserFile_Close(&ctx->src);
		TokenStream_Free(&ctx->tokens);
		return -1;
	}
	
	return 0;
}

void ParserCtx_Teardown(ParserCtx *ctx) {
	free(ctx->source_path);
	ParserFile_Close(&ctx->src);
	TokenStream_Free(&ctx->tokens);
	Ast_Delete(&ctx->ast);
}

