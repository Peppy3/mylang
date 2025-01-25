#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "ParserFile.h"

int ParserFile_Open(ParserFile *file, const char *filename) {
	assert(file != NULL);
	int fd;
	struct stat st;

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "%s: Could not open file. %s\n", filename, strerror(errno));
		return -1;
	}
	
	if (fstat(fd, &st) < 0) {
		perror(filename);
		return -1;
	}

	if (st.st_size > UINT32_MAX) {
		fprintf(stderr, "%s: File can not be bigger than 4.3G\n", filename);
		return -1;
	}

	file->size = st.st_size;
	file->pos = 0;
	
	file->data = malloc(file->size);

	if (file->data == NULL) {
		fprintf(stderr, "ParserFile_Open: %s\n", strerror(errno));
		return -1;
	}

	if (read(fd, file->data, file->size) < 0) {
		perror(filename);
		return -1;
	}

	return 0;
}

int ParserFile_FromStr(ParserFile *file, char *str, uint32_t size) {
	char *data = malloc(size);
	if (data == NULL) {
		return -1;
	}
	*file = (ParserFile){
		.size = size,
		.pos = 0,
		.data = data,
	};

	memcpy(data, str, size);

	return 0;
}

void ParserFile_PrintLine(const ParserFile file, size_t line_start_pos, int tab_len, FILE *stream) {
	assert(line_start_pos < file.size);

	for (size_t i = line_start_pos; file.data[i] != '\n' && i < file.size; i++) {
		if (file.data[i] == '\t') {
			for (int j = 0; j < tab_len; j++) {
				fputc(' ', stream);
			}
		}
		else {
			fputc(file.data[i], stream);
		}
	}
	fputc('\n', stream);
}

