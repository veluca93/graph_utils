CXXFLAGS=-O3 -Wall -std=c++17 -I/usr/include/freetype2/ -I. -flto
LDFLAGS=-lpng -lfreetype -flto
CC=g++-7

all: bin/el2nde bin/el2png bin/nde2el bin/nde2oly bin/nde2tsv bin/oly2nde bin/tsv2nde

build/pngwriter.o: pngwriter.cc
	${CC} $^ -o $@ -c ${CXXFLAGS}

build/io.o: io.cpp
	${CC} $^ -o $@ -c ${CXXFLAGS}

bin/el2nde: el2nde.cpp build/io.o
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/el2png: el2png.cpp build/pngwriter.o
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/nde2el: nde2el.cpp
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/nde2oly: nde2oly.cpp
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/nde2tsv: nde2tsv.cpp
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/oly2nde: oly2nde.cpp
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

bin/tsv2nde: tsv2nde.cpp
	${CC} $^ -o $@ ${CXXFLAGS} ${LDFLAGS}

clean:
	rm -f bin/* build/*
