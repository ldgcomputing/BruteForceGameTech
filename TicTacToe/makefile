#
# Makefile for TicTacToe sample project
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
INCLUDES = -I Include
TARGET ?= ${DEFAULT_TARGET}

# Specific to target
ifeq "$(TARGET)" "debug"
	BIN = bin/debug
	CC_OPTS = 
	LIB_TICTACTOE = tictactoed.a
	TARGET_FILE = bruteForce
else
	BIN = bin/release
	CC_OPTS = -O3 
	LIB_TICTACTOE = tictactoe.a
	TARGET_FILE = bruteForce
endif

all : ${TARGET_FILE}

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

${TARGET_FILE} : ${LIB_TICTACTOE} ${BIN}/BruteForce.o
	${CC} ${INCLUDES} ${CC_OPTS} -o ${TARGET_FILE} ${BIN}/BruteForce.o ${LIB_TICTACTOE}

${BIN}/BruteForce.o: include/BoardFunctions.h include/CommonElements.h src/BruteForce.cpp
	${CC} -c ${INCLUDES} ${CC_OPTS} -o ${BIN}/BruteForce.o src/BruteForce.cpp

${LIB_TICTACTOE}: ${BIN}/BoardFunctions.o ${BIN}/CommonElements.o
	cd ${BIN} && ${AR} -r -c ../../${LIB_TICTACTOE} BoardFunctions.o CommonElements.o

${BIN}/BoardFunctions.o: include/CommonElements.h include/BoardFunctions.h src/BoardFunctions.cpp
	${CC} -c ${INCLUDES} ${CC_OPTS} -o ${BIN}/BoardFunctions.o src/BoardFunctions.cpp

${BIN}/CommonElements.o: include/CommonElements.h src/CommonElements.cpp
	${CC} -c ${INCLUDES} ${CC_OPTS} -o ${BIN}/CommonElements.o src/CommonElements.cpp

