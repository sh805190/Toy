#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, const char* argv[]) {
	Chunk chunk;
	initChunk(&chunk);

	int constIndex = addConstant(&chunk, 1.2);
	writeChunk(&chunk, OP_CONSTANT, 123);
	writeChunk(&chunk, constIndex, 123);

	writeChunk(&chunk, OP_RETURN, 123);
	writeChunk(&chunk, OP_RETURN, 125);

	disassembleChunk(&chunk, "test chunk");
	freeChunk(&chunk);
	return 0;
}