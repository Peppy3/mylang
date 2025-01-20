#ifndef FILE_H_
#define FILE_H_

#include <stddef.h>
#include <stdbool.h>

#include <stdlib.h>
#include <stdio.h>

// Just a file buffer
typedef struct {
	size_t size;
	size_t pos;
	char *data;
} ParserFile;

// returns non- zero on error
int ParserFile_Open(ParserFile *file, const char *filename);

static inline void ParserFile_Close(const ParserFile file) {
	free(file.data);
}

void ParserFile_PrintLine(const ParserFile file, size_t line_start_pos, int tab_len, FILE *stream);

static inline bool ParserFile_IsEof(const ParserFile file) {
	return file.pos >= file.size;
}

static inline int ParserFile_GetCh(ParserFile *file) {
	if (ParserFile_IsEof(*file))
		return EOF;

	int ch = (int)(file->data[file->pos]);
	file->pos += 1;

	return ch;
}

static inline int ParserFile_Peek(ParserFile *file) {
	if (ParserFile_IsEof(*file))
		return EOF;

	int ch = (int)(file->data[file->pos]);

	return ch;
}

#endif /* FILE_H_ */
