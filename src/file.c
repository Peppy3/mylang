#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

struct File {
	size_t size;
	char data[];
};

struct File *FileOpen(const char *filename) {
	struct File *file;
	FILE *fp;
	size_t size;

	fp = fopen(filename, "rb");
	
	if (fp == NULL) {
		perror(filename);
		return NULL;
	}

	if (fseek(fp, 0, SEEK_END) == -1L) {
		fclose(fp);
		return NULL;
	}

	size = ftell(fp);

	if (fseek(fp, 0, SEEK_SET) == -1L) {
		fclose(fp);
		return NULL;
	}

	file = malloc(sizeof *file + size + 1);
	size_t read_size = fread(file->data, sizeof(char), size, fp);
	if (read_size < size) {
		free(file);
		fclose(fp);
		return NULL;
	}
	
	fclose(fp);
	
	file->size = size;
	return file;
}

void FileClose(struct File *file) {
	free(file);
}

size_t FileGetSize(struct File *file) {
	return file->size;
}

char *FileGetData(struct File *file) {
	return file->data;
}

