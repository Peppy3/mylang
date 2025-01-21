#ifndef TOKEN_STREAM_H_
#define TOKEN_STREAM_H_

#include <stdint.h>

#include <stdlib.h>

#include "util.h"
#include "Token.h"

typedef struct {
	uint32_t len;
	uint32_t pos;
	Token *tokens;
} TokenStream;

int TokenStream_Generate(TokenStream *stream, ParserFile *src);

static inline void TokenStream_Free(TokenStream *stream) {
	free(stream->tokens);
}

static inline bool TokenStream_IsEof(TokenStream *stream) {
	return stream->pos >= stream->len;
}

static inline int32_t TokenStream_Iter(TokenStream *stream) {
	if (TokenStream_IsEof(stream))
		return -1;

	int32_t idx = stream->pos;
	stream->pos += 1;

	return idx;
}

static inline enum TokenType TokenStream_Peek(TokenStream *stream) {
	if (TokenStream_IsEof(stream))
		return TOKEN_eof;

	Token tok = stream->tokens[stream->pos];

	return tok.type;
}

#endif /* TOKEN_STREAM_H_ */

