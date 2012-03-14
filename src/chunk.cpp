#include "chunk.h"

#include <iostream>
#include <algorithm>

#include <arpa/inet.h>

#define DEBUG_CHUNK_PAIRS(x) 0;
//#define DEBUG_CHUNK_PAIRS(x) x

int chunk_pairs(leveldb::Iterator *iterPtr, struct ChunkMetadata* meta, int keyLimit, int valLimit, char* keyBuffer, char* valBuffer, int encodeKeys, int encodeVals, int keyWidth, int valWidth) {
  DEBUG_CHUNK_PAIRS(std::cout << "Starting chunk fill" << std::endl;);

  leveldb::Iterator* iter = (leveldb::Iterator *) iterPtr;

  int count = 0;
  long usedKeyBufferSize = 0;
  long usedValBufferSize = 0;

  // Error check first
  if (meta == NULL) {
    return ERR_INVALID_META;
  }

  if (iterPtr == NULL) {
    return ERR_INVALID_ITER;
  }

  if (keyBuffer == NULL || valBuffer == NULL) {
    return ERR_INVALID_BUFFER;
  }

  DEBUG_CHUNK_PAIRS(std::cout << "Safety checks complete" << std::endl;);

  // We simply iterate as long as the iterator is valid. We do free space checks on our buffers later
  for (int i = 0; iter->Valid(); ++i) {
    leveldb::Slice ks = iter->key();
    leveldb::Slice vs = iter->value();

    // Actual allocated space depends on whether we're variable length, fixed non-zero length, 
    // or if we're fixed at zero bytes we just encode the whole thing
    int keyByteWidth = encodeKeys ? 4 + ks.size() : (keyWidth == 0 ? ks.size() : keyWidth);
    int valByteWidth = encodeVals ? 4 + vs.size() : (valWidth == 0 ? vs.size() : valWidth);

    // We have to stop if the combine key/value pair would exceed our chunk buffer
    if ((usedKeyBufferSize + keyByteWidth) > keyLimit || (usedValBufferSize  + valByteWidth) > valLimit) {
      break;
    }

    DEBUG_CHUNK_PAIRS(std::cout << "Space OK" << std::endl;);

    ++count;
      
    // Compact key and value onto the end of the buffer
    if (encodeKeys) {
      *(int *)(keyBuffer + usedKeyBufferSize) = htonl(ks.size());
      usedKeyBufferSize += 4;
    }
      
    int keyDataWidth = encodeKeys ? ks.size() : (keyWidth == 0 ? ks.size() : keyWidth);

    // If we don't have enough bytes to fulfill (fixed width but insufficient data size), return an error
    if (ks.size() < keyDataWidth) {
      return ERR_INSUFFICIENT_BYTES;
    }

    memcpy(keyBuffer + usedKeyBufferSize, ks.data(), keyDataWidth);
    usedKeyBufferSize += keyDataWidth;

    DEBUG_CHUNK_PAIRS(std::cout << "  Key encoded" << std::endl;);

    if (encodeVals) {
      *(int *)(valBuffer + usedValBufferSize) = htonl(vs.size());
      usedValBufferSize += 4;
    }
        
    int valDataWidth = encodeVals ? vs.size() : (valWidth == 0 ? vs.size() : valWidth);

    if (vs.size() < valDataWidth) {
      return ERR_INSUFFICIENT_BYTES;
    }

    memcpy(valBuffer + usedValBufferSize, vs.data(), valDataWidth);
    usedValBufferSize += valDataWidth;

    DEBUG_CHUNK_PAIRS(std::cout << "  Val encoded" << std::endl;);

    iter->Next();
  }

  DEBUG_CHUNK_PAIRS(std::cout << "Chunk filled" << std::endl;);

  meta->keyByteLength = usedKeyBufferSize;
  meta->valByteLength = usedValBufferSize;
  meta->pairLength = count;

  return count;
}


