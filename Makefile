CXXFLAGS=-O3 -Wall -std=c++14 -Iutil -Igraph -flto -g
SDSL_CXXFLAGS=-I ~/include -L ~/lib
LDFLAGS=-flto -lgflags -Wl,--as-needed # -ltcmalloc
SDSL_LDFLAGS=-lsdsl -ldivsufsort -ldivsufsort64
CC=g++

all: bin/gconvert bin/draw_graph bin/degeneracy bin/draw_info bin/compressed_adj_list \
	bin/gpermute

COMMON_OBJECTS= \
	build/io.o build/graph.o build/nde_format.o build/el_format.o build/tsv_format.o \
	build/oly_format.o build/bin_format.o build/graph_io.o

COMMON_HEADERS= \
	graph/el_format.hpp graph/tsv_format.hpp graph/nde_format.hpp graph/graph.hpp \
	graph/oly_format.hpp util/span.hpp util/assert.hpp util/io.hpp util/strtk.hpp \
	graph/graph_io.hpp graph/bin_format.hpp util/common_defs.hpp

build/io.o: util/io.cpp util/io.hpp util/strtk.hpp util/span.hpp
	${CC} $< -o $@ -c ${CXXFLAGS}

build/graph.o: graph/graph.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

build/nde_format.o: graph/nde_format.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

build/el_format.o: graph/el_format.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

build/oly_format.o: graph/oly_format.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

build/tsv_format.o: graph/tsv_format.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

build/bin_format.o: graph/bin_format.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

build/graph_io.o: graph/graph_io.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

build/gconvert.o: gconvert.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

build/gpermute.o: gpermute.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

build/draw_graph.o: draw_graph.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

build/degeneracy.o: degeneracy.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

build/draw_info.o: draw_info.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

build/compressed_adj_list.o: compressed_adj_list.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS} ${SDSL_CXXFLAGS}

bin/gconvert: build/gconvert.o ${COMMON_OBJECTS}
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/draw_graph: build/draw_graph.o build/io.o
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/degeneracy: build/degeneracy.o ${COMMON_OBJECTS}
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/draw_info: build/draw_info.o ${COMMON_OBJECTS}
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/gpermute: build/gpermute.o ${COMMON_OBJECTS}
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/compressed_adj_list: build/compressed_adj_list.o ${COMMON_OBJECTS}
	${CC} $^ -o $@ ${CXXFLAGS} ${SDSL_CXXFLAGS} ${LDFLAGS} ${SDSL_LDFLAGS}

clean:
	rm -f bin/* build/*
