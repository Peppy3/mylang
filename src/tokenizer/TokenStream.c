#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdlib.h>

#include "Tokenizer.h"

#include "TokenStream.h"

#define TOKEN_STREAM_START_CAP 512
int TokenStream_Generate(TokenStream *stream, ParserFile *src) {
	uint32_t len = 0, cap = TOKEN_STREAM_START_CAP;

	Token *token_list = malloc(sizeof(Token) * cap);
	if (token_list == NULL) {
		return -1;
	}

	for (;;) {
		uint32_t grow_by = cap >> 1;
		if (len > grow_by) {
			grow_by = len;
		}
		
		if (len >= cap) {
			cap += grow_by;
			Token *new_list = realloc(token_list, sizeof(Token) * cap); 
			if (new_list == NULL) { 
				free(token_list);
				return -1; 
			} 
			token_list = new_list; 
		}

		Token tok = NextToken(src);

		token_list[len++] = tok;
		if (tok.type == TOKEN_eof) {
			break;
		}
	}
	
	*stream = (TokenStream){
		.len = len,
		.pos = 0,
		.tokens = token_list,
	};

	return 0;
}

