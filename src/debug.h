#ifndef CTOY_DEBUG_H
#define CTOY_DEBUG_H

#include "chunk.h"

void disassembleChunk(Chunk* chunk, const char* name);
int disassembleInstruction(Chunk* chunk, int i);

#endif