#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

struct Arena {
	size_t cap;
	size_t size;
	void *pool;
};

typedef ssize_t arena_index;

enum ArenaError {
	ArenaSuccess = 0,
	ArenaAllocError = -1,
};

enum ArenaError ArenaInit(struct Arena *arena);

void ArenaDestroy(struct Arena *arena);

// If this funciton returns a negative number then,
// it does not mean that the arena is deallocated
arena_index ArenaAlloc(struct Arena *arena, size_t size);

enum ArenaError ArenaWrite(struct Arena *arena, size_t size, const void *buff);


static inline void *ArenaGetObject(struct Arena arena, arena_index idx) {
	assert(idx >= 0 && arena.size >= (size_t)idx);
	return ((uint8_t *)arena.pool) + idx;
}

static inline size_t ArenaGetCap(struct Arena arena) {
	return arena.cap;
}

static inline size_t ArenaGetSize(struct Arena arena) {
	return arena.size;
}

static inline void *ArenaGetData(struct Arena arena) {
	return arena.pool;
}

#endif /* ARENA_H */

