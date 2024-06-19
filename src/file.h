#ifndef FILE_H
#define FILE_H

#include <stddef.h>

// Just a file buffer
struct File;

// returns NULL if any errors
struct File *FileOpen(const char *filename);

void FileClose(struct File *file);

size_t FileGetSize(struct File *file);
char *FileGetData(struct File *file);

#endif /* FILE_H */
