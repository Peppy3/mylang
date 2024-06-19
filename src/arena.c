#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "arena.h"

#define ARRAY_VECTOR_START_SIZE 32
enum ArenaVectorError ArenaVectorInit(struct ArenaVector *arena) {
	void *pool = malloc(ARRAY_VECTOR_START_SIZE);
	if (pool == NULL) return ArenaAllocError;
	
	arena->cap = ARRAY_VECTOR_START_SIZE;
	arena->allocated = 0;
	arena->pool = memset(pool, 0, ARRAY_VECTOR_START_SIZE);
	
	return ArenaSuccess;
}

void ArenaVectorDestroy(struct ArenaVector *arena) {
	free(arena->pool);
	memset(arena, 0, sizeof *arena);
}

// If this funciton returns a negative number then it does not mean that the arena is deallocated
arena_index ArenaVectorAlloc(struct ArenaVector *arena, size_t size) {
	arena_index obj = -1;
	void *new_pool = NULL;
	size_t arena_cap = arena->cap;
	size_t new_cap = 0;
	size_t arena_allocated = arena->allocated;

	assert(arena_cap > arena_allocated);

	if ((arena_cap - arena_allocated) <= size) {
		new_cap = arena_cap + size;
		new_cap += (((new_cap >> 3) + 12) & ~((size_t)3));

		assert(arena->cap + size < new_cap);
		
		new_pool = realloc(arena->pool, new_cap);
		if (new_pool == NULL) return -1;

		memset(((uint8_t *)new_pool) + arena_cap, 0, new_cap - arena_cap);

		arena->pool = new_pool;
		arena_cap = new_cap;
		arena->cap = new_cap;
	}
	
	assert(arena_allocated + size <= arena_cap);

	obj = arena_allocated;
	arena->allocated = arena_allocated + size;
	
	return obj;
}

