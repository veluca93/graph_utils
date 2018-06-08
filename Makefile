CXXFLAGS=\
	-O3 -Wall -std=c++14 -Iutil -Igraph -Iexternal -Icommands \
	-flto -g -march=native -DNDEBUG
SDSL_CXXFLAGS=-I ~/include -L ~/lib
LDFLAGS=-flto -Wl,--as-needed -march=native -ltcmalloc
SDSL_LDFLAGS=-lsdsl -ldivsufsort -ldivsufsort64
CXX=g++

SRCS=$(wildcard **/*.cpp) $(wildcard *.cpp)
OBJS=$(SRCS:%.cpp=build/%.o)
DEPS=$(SRCS:%.cpp=.deps/%.d)
DIRS=$(dir ${OBJS}) $(dir ${DEPS}) bin

$(shell mkdir -p $(DIRS))

all: bin/gutil

.deps/%.d: %.cpp Makefile
	${CXX} $< -M -MM -MP -MT $(patsubst .deps/%.d,build/%.o,$@) -o $@ ${CXXFLAGS} \
		${SDSL_CXXFLAGS} ${LDFLAGS}

build/%.o: %.cpp .deps/%.d
	${CXX} $< -c -o $@ ${CXXFLAGS} ${SDSL_CXXFLAGS} ${LDFLAGS}

bin/gutil: ${OBJS}
	${CXX} $^ -o $@ ${CXXFLAGS} ${SDSL_CXXFLAGS} ${SDSL_LDFLAGS} ${LDFLAGS}

clean:
	rm -rf bin/ build/ .deps/

.PRECIOUS: ${DEPS}

-include ${DEPS}
