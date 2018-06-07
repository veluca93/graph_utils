CXXFLAGS=-O3 -Wall -std=c++14 -Iutil -Igraph -flto -g -march=native -DNDEBUG
SDSL_CXXFLAGS=-I ~/include -L ~/lib
LDFLAGS=-flto -lgflags -Wl,--as-needed -march=native # -ltcmalloc
SDSL_LDFLAGS=-lsdsl -ldivsufsort -ldivsufsort64
CC=g++

all: bin/gconvert bin/draw_graph bin/degeneracy bin/draw_info bin/compressed_adj_list \
	bin/gpermute bin/compressed_triangle_counting bin/triangle_counting bin/triangle_counting_finger

COMMON_OBJECTS= \
	build/io.o build/graph.o build/nde_format.o build/el_format.o build/tsv_format.o \
	build/oly_format.o build/bin_format.o build/bel_format.o

COMMON_HEADERS= \
	util/span.hpp util/assert.hpp util/io.hpp util/strtk.hpp \
	graph/graph.hpp util/common_defs.hpp

build/io.o: util/io.cpp util/io.hpp util/strtk.hpp util/span.hpp
	${CC} $< -o $@ -c ${CXXFLAGS}

build/graph.o: graph/graph.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

build/nde_format.o: graph/nde_format.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

build/el_format.o: graph/el_format.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

build/bel_format.o: graph/bel_format.cpp ${COMMON_HEADERS}
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

build/compressed_triangle_counting.o: compressed_triangle_counting.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS} ${SDSL_CXXFLAGS}

build/triangle_counting.o: triangle_counting.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

build/triangle_counting_finger.o: triangle_counting_finger.cpp ${COMMON_HEADERS}
	${CC} $< -o $@ -c ${CXXFLAGS}

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

bin/compressed_triangle_counting: build/compressed_triangle_counting.o ${COMMON_OBJECTS}
	${CC} $^ -o $@ ${CXXFLAGS} ${SDSL_CXXFLAGS} ${LDFLAGS} ${SDSL_LDFLAGS}

bin/triangle_counting: build/triangle_counting.o ${COMMON_OBJECTS}
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/triangle_counting_finger: build/triangle_counting_finger.o ${COMMON_OBJECTS}
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

clean:
	rm -f bin/* build/*
