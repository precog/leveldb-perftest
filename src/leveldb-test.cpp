#include<assert.h>

#include <time.h>
#include <arpa/inet.h>

#include <iostream>

#include "chunk.h"

void run_iteration(leveldb::DB* db) {
  clock_t start = clock();

  leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());

  // Emulate chunk buffers
  int bufferSize = 32 * 1024;
  char* keyBuffer = new char[bufferSize];
  char* valBuffer = new char[bufferSize];

  ChunkMetadata metadata;

  for (it->SeekToFirst(); it->Valid(); ) {
    chunk_pairs(it, &metadata, bufferSize, bufferSize, keyBuffer, valBuffer, false, false, 8, 8);
  }

  clock_t end = clock();

  delete keyBuffer;
  delete valBuffer;
  delete it;

  std::cout << "  Iteration complete in " << (end - start) << " clocks" << std::endl;
}

int main (int argc, char **argv) {
  leveldb::DB* db;
  leveldb::Options options;
  
  options.create_if_missing = true;
  options.block_size = 64 * 1024;
  //options.block_cache = leveldb::NewLRUCache(100 * 1024 * 1024);

  leveldb::Status status = leveldb::DB::Open(options, "/tmp/leveldb-c-test", &db);

  if (!status.ok()) {
    std::cerr << status.ToString() << std::endl;
    return -1;
  }

  clock_t start = clock();

  leveldb::WriteBatch batch;

  for (long i = 0; i < 1000000; ++i) {
    long bigEndian = ((long)htonl(i)) << 32;

    leveldb::Slice key((const char *) &bigEndian, 8);
    leveldb::Slice val((const char *) &bigEndian, 8);

    batch.Put(key, val);
  }

  leveldb::WriteOptions writeOpts;
  writeOpts.sync = true;

  db->Write(writeOpts, &batch);

  clock_t end = clock();

  std::cout << "Insertion complete in " << (end - start) << " clocks" << std::endl;

  delete db;

  // Open the DB again
  
  status = leveldb::DB::Open(options, "/tmp/leveldb-c-test", &db);

  if (!status.ok()) {
    std::cerr << status.ToString() << std::endl;
    return -1;
  }

  for (int i = 0; i < 20; ++i) {
    run_iteration(db);
  }

  delete db;

  return 0;
}
