CXXFLAGS=-O3 -Wall -std=c++14 -I/usr/include/freetype2/ -Iutil -Igraph -flto -g -lgflags
LDFLAGS=-lpng -lfreetype -flto -ltcmalloc
CC=g++

all: bin/el2nde bin/el2png bin/nde2el bin/nde2oly bin/nde2tsv bin/oly2nde bin/tsv2nde bin/clean_nde

build/pngwriter.o: util/pngwriter.cc util/pngwriter.h
	${CC} $< -o $@ -c ${CXXFLAGS}

build/io.o: util/io.cpp util/io.hpp util/strtk.hpp
	${CC} $< -o $@ -c ${CXXFLAGS}

build/graph.o: graph/graph.cpp graph/graph.hpp build/io.o
	${CC} $< -o $@ -c ${CXXFLAGS}

build/nde_format.o: graph/nde_format.cpp graph/nde_format.hpp build/graph.o
	${CC} $< -o $@ -c ${CXXFLAGS}

build/el_format.o: graph/el_format.cpp graph/el_format.hpp build/graph.o
	${CC} $< -o $@ -c ${CXXFLAGS}

build/oly_format.o: graph/oly_format.cpp graph/oly_format.hpp build/graph.o
	${CC} $< -o $@ -c ${CXXFLAGS}

build/tsv_format.o: graph/tsv_format.cpp graph/tsv_format.hpp build/graph.o
	${CC} $< -o $@ -c ${CXXFLAGS}

bin/el2nde: el2nde.cpp build/io.o build/graph.o build/nde_format.o build/el_format.o
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/el2png: el2png.cpp build/pngwriter.o build/io.o
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/nde2el: nde2el.cpp build/io.o build/graph.o build/nde_format.o build/el_format.o
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/nde2oly: nde2oly.cpp build/io.o build/graph.o build/nde_format.o build/oly_format.o
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/nde2tsv: nde2tsv.cpp build/io.o build/graph.o build/nde_format.o build/tsv_format.o
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/oly2nde: oly2nde.cpp build/io.o build/graph.o build/nde_format.o build/oly_format.o
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/tsv2nde: tsv2nde.cpp build/io.o build/graph.o build/nde_format.o build/tsv_format.o
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/clean_nde: clean_nde.cpp build/io.o build/graph.o build/nde_format.o
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

clean:
	rm -f bin/* build/*
