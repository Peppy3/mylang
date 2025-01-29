
#include <stdlib.h>
#include <string.h>

#include <ParserFile.h>
#include <tokenizer/TokenStream.h>
#include <ast/Ast.h>

#include <CompileUnit.h>

int CompileUnit_Setup(CompileUnit *unit, char *filepath) {
	size_t filepath_len = strlen(filepath) + 1;
	char *filepath_cpy = malloc(filepath_len);
	if (filepath_cpy == NULL) {
		return -1;
	}

	unit->source_path = memcpy(filepath_cpy, filepath, filepath_len);
	
	if (ParserFile_Open(&unit->src, filepath)) {
		free(filepath_cpy);
		return -1;
	}

	if (Ast_New(&unit->ast)) {
		free(filepath_cpy);
		ParserFile_Close(&unit->src);
		return -1;
	}
	
	return 0;
}

void CompileUnit_Teardown(CompileUnit *unit) {
	free(unit->source_path);
	ParserFile_Close(&unit->src);
	Ast_Delete(&unit->ast);
}

