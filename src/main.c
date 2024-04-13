#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"

int main(int argc, char *const *argv) {
	if (argc != 2) {
		fprintf(stderr,"%s: [file]\n", argv[0]);
		return 1;
	}

	Filebuff *fb = tokenizer_init(argv[1]);
	if (fb == NULL) {
		fprintf(stderr, "Could not read %s\n", argv[1]);
		return 1;
	}
	
	while (1) {
		Token token = next_token(fb);
		if (token.type == _EOF) {
			break;
		}
		if (token.str == NULL) continue;

		fwrite(token.str, sizeof(char), token.len, stdout);
		printf("<\ntype: %d, len: %ld, pos: %ld\n", 
				token.type, token.len, token.pos);
	}
	
	tokenizer_free(fb);
	
	return 0;
}

