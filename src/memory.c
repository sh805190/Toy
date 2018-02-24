#include "memory.h"
#include "common.h"

#include <stdlib.h>

void* reallocate(void* previous, size_t oldSize, size_t newSize) {
	return realloc(previous, newSize);
}