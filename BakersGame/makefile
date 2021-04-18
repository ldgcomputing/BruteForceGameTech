#
# Makefile for BakersGame solver project
#
# Copyright (c) 2021 Louis Gehrig. All rights reserved.
#

#
#	MIT License
#
#	Copyright (c) 2021 Louis Gehrig
#
#	Permission is hereby granted, free of charge, to any person obtaining a copy
#	of this software and associated documentation files (the "Software"), to deal
#	in the Software without restriction, including without limitation the rights
#	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#	copies of the Software, and to permit persons to whom the Software is
#	furnished to do so, subject to the following conditions:
#
#	The above copyright notice and this permission notice shall be included in all
#	copies or substantial portions of the Software.
#
#	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#	SOFTWARE.
#

# Certain defaults
AR = ar
CC = g++
DEFAULT_TARGET = debug
INCLUDES = -I include -I /usr/local/include/node
LIBS = -lpthread -ldl -lm
TARGET ?= ${DEFAULT_TARGET}

# Specific to target
ifeq "$(TARGET)" "debug"
	BIN = bin/debug
	C_OPTS = -stdlib=libc++
	CC_OPTS = -Wall -DACTIVE_LOGGING -std=gnu++1y -fno-rtti -fno-strict-aliasing -stdlib=libc++
	LIB_SOLVER = bakersgamed.a
	TARGET_FILE = bruteForce
else
	BIN = bin/release
	C_OPTS = -stdlib=libc++ -O3
	CC_OPTS = -O3 -std=gnu++1y -fno-rtti -fno-strict-aliasing -stdlib=libc++ -DACTIVE_LOGGING 
	LIB_SOLVER = bakersgame.a
	TARGET_FILE = bruteForce
endif

all : ${TARGET_FILE}

test : ${TARGET_FILE}
	./${TARGET_FILE} 1 /tmp/blah.bin 100 5

testalt : ${TARGET_FILE}
	./${TARGET_FILE} 1 -showalt /tmp/blah.bin 1848153258 1

lib : ${LIB_SOLVER}

run : ${TARGET_FILE}
	./${TARGET_FILE}

clean:
	rm -rf ${TARGET_FILE} ${BIN}
	mkdir -p ${BIN}
	chmod 777 ${BIN}

cleanall:
	rm -rf bin ${TARGET_FILE}
	mkdir bin
	mkdir bin/debug
	mkdir bin/release
	chmod 777 bin bin/debug bin/release

${TARGET_FILE} : ${LIB_SOLVER} ${BIN}/BruteForce.o
	${CC} ${INCLUDES} ${CC_OPTS} -o ${TARGET_FILE} ${BIN}/BruteForce.o ${LIB_SOLVER} ${LIBS}

${BIN}/BruteForce.o: include/nodeSupport.h include/SolitaireBoard.h include/SolitaireSolver_BakersGameEasy.h include/SolitaireSolver_BakersGameStnd.h include/SolitaireSolver.h include/solverSupport.h include/sqlite3.h src/BruteForce.cpp
	${CC} -c ${INCLUDES} ${CC_OPTS} -o ${BIN}/BruteForce.o src/BruteForce.cpp

${LIB_SOLVER}: ${BIN}/dbSupport.o ${BIN}/nodeSupport.o ${BIN}/SolitaireBoard.o ${BIN}/SolitaireSolver_BakersGameEasy.o ${BIN}/SolitaireSolver_BakersGameStnd.o ${BIN}/SolitaireSolver.o ${BIN}/solverSupport.o ${BIN}/sqlite3.o
	cd ${BIN} && ${AR} -r -c ../../${LIB_SOLVER} dbSupport.o nodeSupport.o SolitaireBoard.o SolitaireSolver_BakersGameEasy.o SolitaireSolver_BakersGameStnd.o SolitaireSolver.o solverSupport.o sqlite3.o

${BIN}/dbSupport.o : include/dbSupport.h include/sqlite3.h src/dbSupport.cpp
	${CC} -c ${INCLUDES} ${CC_OPTS} -o ${BIN}/dbSupport.o src/dbSupport.cpp

${BIN}/nodeSupport.o: include/nodeSupport.h src/nodeSupport.cpp
	${CC} -c ${INCLUDES} ${CC_OPTS} -o ${BIN}/nodeSupport.o src/nodeSupport.cpp

${BIN}/SolitaireBoard.o: include/SolitaireBoard.h src/SolitaireBoard.cpp
	${CC} -c ${INCLUDES} ${CC_OPTS} -o ${BIN}/SolitaireBoard.o src/SolitaireBoard.cpp

${BIN}/SolitaireSolver_BakersGameEasy.o: include/SolitaireBoard.h include/SolitaireSolver.h include/SolitaireSolver_BakersGameEasy.h src/SolitaireSolver_BakersGameEasy.cpp
	${CC} -c ${INCLUDES} ${CC_OPTS} -o ${BIN}/SolitaireSolver_BakersGameEasy.o src/SolitaireSolver_BakersGameEasy.cpp

${BIN}/SolitaireSolver_BakersGameStnd.o: include/SolitaireBoard.h include/SolitaireSolver.h include/SolitaireSolver_BakersGameStnd.h src/SolitaireSolver_BakersGameStnd.cpp
	${CC} -c ${INCLUDES} ${CC_OPTS} -o ${BIN}/SolitaireSolver_BakersGameStnd.o src/SolitaireSolver_BakersGameStnd.cpp

${BIN}/SolitaireSolver.o: include/SolitaireBoard.h include/SolitaireSolver.h src/SolitaireSolver.cpp
	${CC} -c ${INCLUDES} ${CC_OPTS} -o ${BIN}/SolitaireSolver.o src/SolitaireSolver.cpp

${BIN}/solverSupport.o: include/SolitaireBoard.h include/SolitaireSolver_BakersGameStnd.h src/SolitaireSolver_BakersGameStnd.cpp include/SolitaireSolver.h include/solverSupport.h src/solverSupport.cpp
	${CC} -c ${INCLUDES} ${CC_OPTS} -o ${BIN}/solverSupport.o src/solverSupport.cpp

${BIN}/sqlite3.o: include/sqlite3.h src/sqlite3.c
	gcc -c ${INCLUDES} ${C_OPTS} -o ${BIN}/sqlite3.o src/sqlite3.c
