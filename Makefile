LEVELDB=../leveldb
SNAPPY=../snappy-1.0.3

CXXFLAGS=-I$(LEVELDB)/include
LDFLAGS=-L$(LEVELDB) -L$(SNAPPY)/.libs -lleveldb -lsnappy

OBJECTS = src/leveldb-test.o src/chunk.o

leveldb-test	: $(OBJECTS)
	g++ $? $(LDFLAGS) -o $@

clean	:
	rm -f leveldb-test
	rm -f src/*.o
	rm -rf /tmp/leveldb-c-test