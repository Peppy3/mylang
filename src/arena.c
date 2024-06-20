#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "arena.h"

#define ARRAY_START_SIZE 32
enum ArenaError ArenaInit(struct Arena *arena) {
	void *pool = malloc(ARRAY_START_SIZE);
	if (pool == NULL) return ArenaAllocError;
	
	*arena = (struct Arena){
		.cap = ARRAY_START_SIZE,
		.size = 0,
		.pool = memset(pool, 0, ARRAY_START_SIZE)
	};
	
	return ArenaSuccess;
}

void ArenaDestroy(struct Arena *arena) {
	free(arena->pool);
	memset(arena, 0, sizeof *arena);
}

// If this funciton returns a negative number then it does not mean that the arena is deallocated
arena_index ArenaAlloc(struct Arena *arena, size_t size) {
	arena_index obj = -1;
	void *new_pool = NULL;
	size_t arena_cap = arena->cap;
	size_t new_cap = 0;
	size_t arena_size = arena->size;

	assert(arena_cap > arena_size);

	if ((arena_cap - arena_size) <= size) {
		new_cap = arena_cap + size;
		new_cap += (((new_cap >> 3) + 12) & ~((size_t)3));

		assert(arena->cap + size < new_cap);
		
		new_pool = realloc(arena->pool, new_cap);
		if (new_pool == NULL) return -1;

		memset(((uint8_t *)new_pool) + arena_cap, 0, new_cap - arena_cap);

		arena->pool = new_pool;
		arena->cap = new_cap;

		arena_cap = new_cap;
	}
	
	assert(arena_size + size <= arena_cap);

	obj = arena_size;
	arena->size = arena_size + size;
	
	return obj;
}

enum ArenaError ArenaWrite(struct Arena *arena, size_t size, const void *buff) {
	void *obj;
	arena_index idx = ArenaAlloc(arena, size);
	if (idx < 0) {
		return ArenaAllocError;
	}

	obj = ArenaGetObject(*arena, idx);

	switch (size) {
		case 1:
			*(uint8_t *)obj = *(uint8_t *)buff;
			break;
		case 2:
			*(uint16_t *)obj = *(uint16_t *)buff;
			break;
		case 4:
			*(uint32_t *)obj = *(uint32_t *)buff;
			break;
		case 8:
			*(uint64_t *)obj = *(uint64_t *)buff;
			break;

		default:
			memcpy(obj, buff, size);
			break;
	}

	return ArenaSuccess;
}

