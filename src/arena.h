#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

struct ArenaVector {
	size_t cap;
	size_t allocated;
	void *pool;
};

typedef ssize_t arena_index;

enum ArenaVectorError {
	ArenaSuccess = 0,
	ArenaAllocError = -1,
};

enum ArenaVectorError ArenaVectorInit(struct ArenaVector *arena);

void ArenaVectorDestroy(struct ArenaVector *arena);

// If this funciton returns a negative number then,
// it does not mean that the arena is deallocated
arena_index ArenaVectorAlloc(struct ArenaVector *arena, size_t size);

static inline void *ArenaVectorGetObject(struct ArenaVector arena, arena_index idx) {
	assert(idx >= 0 && arena.allocated >= (size_t)idx);
	return ((uint8_t *)arena.pool) + idx;
}

static inline size_t ArenaVectorGetCap(struct ArenaVector arena) {
	return arena.cap;
}

static inline size_t ArenaVectorGetAllocated(struct ArenaVector arena) {
	return arena.allocated;
}

static inline void *ArenaVectorGetData(struct ArenaVector arena) {
	return arena.pool;
}

#endif /* ARENA_H */
