#ifndef CHUNK_H
#define CHUNK_H

#include "leveldb/db.h"
#include "leveldb/write_batch.h"
#include "leveldb/cache.h"

#define ERR_INVALID_META       -1
#define ERR_INVALID_ITER       -2
#define ERR_INVALID_BUFFER     -3
#define ERR_INSUFFICIENT_BYTES -4
#define ERR_JNI_INTERNAL       -5

struct ChunkMetadata {
  int keyByteLength;
  int valByteLength;
  int pairLength;
};

int chunk_pairs(leveldb::Iterator *iterPtr, struct ChunkMetadata* meta, int keyLimit, int valLimit, char* keyBuffer, char* valBuffer, int encodeKeys, int encodeVals, int keyWidth, int valWidth);

#endif CHUNK_H
