//
//  BoardFunctions.cpp
//  TicTacToe
//
//  Created by Louis Gehrig on 3/21/19.
//  Copyright © 2021 Louis Gehrig. All rights reserved.
//

/***

 MIT License

 Copyright (c) 2021 Louis Gehrig

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 ***/

// Standard includes
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <memory.h>
#include <string.h>

// STL includes
#include <list>

// Project includes
#include <CommonElements.h>
#include <BoardFunctions.h>

// Debug
#define INCLUDE_DEBUG_MACROS
#include <debug.h>

// Helpful constants
const uint8_t VAL_E = tictactoe::BOARD_VALUE_EMPTY;
const uint8_t VAL_O = tictactoe::BOARD_VALUE_O;
const uint8_t VAL_X = tictactoe::BOARD_VALUE_X;

//
// Debug helper - generate board output to string
// WARNING!  Not thread safe!
//

const char* debugQuickPrintBoard(tictactoe::S_TTT_BOARD &theBoard) {
	static char retValue[200 + 1];
	sprintf(retValue, "{ %d %d %d / %d %d %d / %d %d %d }", theBoard.board[0], theBoard.board[1], theBoard.board[2],
			theBoard.board[3], theBoard.board[4], theBoard.board[5], theBoard.board[6], theBoard.board[7],
			theBoard.board[8]);
	return (retValue);
}

//
// Unit test - getNextPlayer
//

bool ut_getNextPlayer() {

	// Terminal board
	const uint8_t TERMINAL_BOARD[9] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	const size_t BOARD_COPY_SIZE = sizeof(TERMINAL_BOARD);

	// X and O next move boards
	const uint8_t XBoards[][9] = { //
			{ VAL_E, VAL_E, VAL_E, VAL_E, VAL_E, VAL_E, VAL_E, VAL_E, VAL_E }, // - - - / - - - / - - -
					{ VAL_O, VAL_O, VAL_E, VAL_E, VAL_O, VAL_E, VAL_X, VAL_X, VAL_X }, // bottom row			O O - / - O - / X X X
					{ VAL_O, VAL_O, VAL_E, VAL_X, VAL_X, VAL_X, VAL_O, VAL_E, VAL_E }, // center row	O O - / X X X / O - -
					{ VAL_X, VAL_X, VAL_X, VAL_O, VAL_E, VAL_O, VAL_O, VAL_E, VAL_E }, // top row		X X X / O - O / O - -
					{ VAL_O, VAL_X, VAL_O, VAL_X, VAL_E, VAL_E, VAL_E, VAL_E, VAL_E }, // O X O / X - - / - - -
					{ VAL_X, VAL_X, VAL_O, VAL_O, VAL_E, VAL_E, VAL_E, VAL_E, VAL_E }, // X X O / O - - / - - -
					{ VAL_X, VAL_O, VAL_O, VAL_X, VAL_E, VAL_O, VAL_X, VAL_E, VAL_E }, // left col		X O O / X - O / X - -
					{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } // terminal
			};
	const uint8_t OBoards[][9] = { //
			{ VAL_X, VAL_O, VAL_E, VAL_O, VAL_X, VAL_E, VAL_E, VAL_E, VAL_X }, // diag l->r		X O - / O X - / - - X
					{ VAL_X, VAL_O, VAL_E, VAL_O, VAL_X, VAL_E, VAL_E, VAL_E, VAL_X }, // diag r->l		X O - / O X - / - - X
					{ VAL_E, VAL_X, VAL_O, VAL_E, VAL_X, VAL_O, VAL_E, VAL_X, VAL_E }, // mid col		- X O / - X O / - X -
					{ VAL_O, VAL_E, VAL_X, VAL_E, VAL_O, VAL_X, VAL_E, VAL_E, VAL_X }, // right col		O - X / - O X / - - X
					{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } // terminal
			};

	// Variables
	bool success = true;

	// Loop over X boards
	for (int xb = 0;; ++xb) {

		// Terminal?
		if (0x0 == memcmp(TERMINAL_BOARD, XBoards[xb], BOARD_COPY_SIZE)) {
			break;
		}

		// Construct the board
		tictactoe::S_TTT_BOARD testBoard;
		memcpy(testBoard.board, XBoards[xb], BOARD_COPY_SIZE);

		// Test
		uint8_t nextPlayer = getNextPlayer(testBoard);
		if (VAL_X != nextPlayer) {
			fprintf( stderr, "ut_getNextPlayer -- FAILURE X board -- test %d %s produces %u\n", xb,
					debugQuickPrintBoard(testBoard), nextPlayer);
			success = false;
		}

	}

	// Loop over O boards
	for (int ob = 0;; ++ob) {

		// Terminal?
		if (0x0 == memcmp(TERMINAL_BOARD, OBoards[ob], BOARD_COPY_SIZE)) {
			break;
		}

		// Construct the board
		tictactoe::S_TTT_BOARD testBoard;
		memcpy(testBoard.board, OBoards[ob], BOARD_COPY_SIZE);

		// Test
		uint8_t nextPlayer = getNextPlayer(testBoard);
		if (VAL_O != nextPlayer) {
			fprintf( stderr, "ut_getNextPlayer -- FAILURE O board -- test %d %s produces %u\n", ob,
					debugQuickPrintBoard(testBoard), nextPlayer);
			success = false;
		}

	}

	// And done
	return (success);

}

//
// Unit test - isBoardWinner
//

bool ut_isBoardWinner() {

	// Terminal board
	const uint8_t TERMINAL_BOARD[9] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	const size_t BOARD_COPY_SIZE = sizeof(TERMINAL_BOARD);

	// Not winning boards
	const uint8_t notWinningBoards[][9] = { //
			{ VAL_E, VAL_E, VAL_E, VAL_E, VAL_E, VAL_E, VAL_E, VAL_E, VAL_E }, // - - - / - - - / - - -
					{ VAL_O, VAL_X, VAL_O, VAL_X, VAL_E, VAL_E, VAL_E, VAL_E, VAL_E }, // O X O / X - - / - - -
					{ VAL_X, VAL_X, VAL_O, VAL_O, VAL_E, VAL_E, VAL_E, VAL_E, VAL_E }, // X X O / O - - / - - -
					{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } // terminal
			};
	const uint8_t XWinningBoards[][9] = { //
			{ VAL_O, VAL_O, VAL_E, VAL_E, VAL_O, VAL_E, VAL_X, VAL_X, VAL_X }, // bottom row			O O - / - O - / X X X
					{ VAL_O, VAL_O, VAL_E, VAL_X, VAL_X, VAL_X, VAL_O, VAL_E, VAL_E }, // center row	O O - / X X X / O - -
					{ VAL_X, VAL_X, VAL_X, VAL_O, VAL_E, VAL_O, VAL_O, VAL_E, VAL_E }, // top row		X X X / O - O / O - -
					{ VAL_X, VAL_O, VAL_E, VAL_O, VAL_X, VAL_E, VAL_E, VAL_E, VAL_X }, // diag l->r		X O - / O X - / - - X
					{ VAL_X, VAL_O, VAL_E, VAL_O, VAL_X, VAL_E, VAL_E, VAL_E, VAL_X }, // diag r->l		X O - / O X - / - - X
					{ VAL_X, VAL_O, VAL_O, VAL_X, VAL_E, VAL_O, VAL_X, VAL_E, VAL_E }, // left col		X O O / X - O / X - -
					{ VAL_E, VAL_X, VAL_O, VAL_E, VAL_X, VAL_O, VAL_E, VAL_X, VAL_E }, // mid col		- X O / - X O / - X -
					{ VAL_O, VAL_E, VAL_X, VAL_E, VAL_O, VAL_X, VAL_E, VAL_E, VAL_X }, // right col		O - X / - O X / - - X
					{ VAL_X, VAL_O, VAL_E, VAL_X, VAL_O, VAL_E, VAL_X, VAL_E, VAL_E }, // parallel		X O - / X O - / X - -
					{ VAL_O, VAL_O, VAL_X, VAL_O, VAL_O, VAL_X, VAL_X, VAL_X, VAL_X }, // dual			O O X / O O X / X X X
					{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } // terminal
			};

	// Variables
	bool success = true;

	// Loop over not winning boards
	for (int nwb = 0;; ++nwb) {

		// Terminal?
		if (0x0 == memcmp(TERMINAL_BOARD, notWinningBoards[nwb], BOARD_COPY_SIZE)) {
			break;
		}

		// Construct the board
		tictactoe::S_TTT_BOARD testBoard;
		memcpy(testBoard.board, notWinningBoards[nwb], BOARD_COPY_SIZE);
		uint8_t winValue = isBoardWinner(testBoard);
		if (0x0 != winValue) {
			fprintf( stderr, "ut_isBoardWinner -- FAILURE not winning board -- test %d %s produces %u\n", nwb,
					debugQuickPrintBoard(testBoard), winValue);
			success = false;
		}

	}

	// Loop over winning boards
	for (int wx = 0;; ++wx) {

		// Terminal?
		if (0x0 == memcmp(TERMINAL_BOARD, XWinningBoards[wx], BOARD_COPY_SIZE)) {
			break;
		}

		// Construct the board
		tictactoe::S_TTT_BOARD testBoard;
		memcpy(testBoard.board, XWinningBoards[wx], BOARD_COPY_SIZE);
		uint8_t winValue = isBoardWinner(testBoard);
		if (VAL_X != winValue) {
			fprintf( stderr, "ut_isBoardWinner -- FAILURE X win -- test %d %s produces %u\n", wx,
					debugQuickPrintBoard(testBoard), winValue);
			success = false;
		}

		// Re-construct the board for O
		for (int nPos = 0; 9 > nPos; ++nPos) {
			if (VAL_X == testBoard.board[nPos])
				testBoard.board[nPos] = VAL_O;
			else if (VAL_O == testBoard.board[nPos])
				testBoard.board[nPos] = VAL_X;
		}
		winValue = isBoardWinner(testBoard);
		if (VAL_O != winValue) {
			fprintf( stderr, "ut_isBoardWinner -- FAILURE O win -- test %d %s produces %u\n", wx,
					debugQuickPrintBoard(testBoard), winValue);
			success = false;
		}

	}

	// And done
	return (success);

}

//
// Unit test - generate boards
//

bool ut_generateBoards() {

	// Helpful constants
	const uint8_t VAL_E = tictactoe::BOARD_VALUE_EMPTY;
	const uint8_t VAL_X = tictactoe::BOARD_VALUE_X;
	const uint8_t VAL_O = tictactoe::BOARD_VALUE_O;

	// The boards
	tictactoe::S_TTT_BOARD reportBoard;
	tictactoe::S_TTT_BOARD startBoard;
	tictactoe::S_TTT_BOARD genBoards[9];
	int numGenerated;

	// X O - / X O - / - - -
	const int TEST_1_EXPECTED_CNT = 5;
	const uint8_t TEST_1_EXPECTED[][9] = { //
			{ VAL_X, VAL_O, VAL_X, VAL_X, VAL_O, VAL_E, VAL_E, VAL_E, VAL_E }, //
					{ VAL_X, VAL_O, VAL_E, VAL_X, VAL_O, VAL_X, VAL_E, VAL_E, VAL_E }, //
					{ VAL_X, VAL_O, VAL_E, VAL_X, VAL_O, VAL_E, VAL_X, VAL_E, VAL_E }, //
					{ VAL_X, VAL_O, VAL_E, VAL_X, VAL_O, VAL_E, VAL_E, VAL_X, VAL_E }, //
					{ VAL_X, VAL_O, VAL_E, VAL_X, VAL_O, VAL_E, VAL_E, VAL_E, VAL_X } //
			};
	tictactoe::initializeBoard(startBoard);
	startBoard.board[0] = tictactoe::BOARD_VALUE_X;
	startBoard.board[1] = tictactoe::BOARD_VALUE_O;
	startBoard.board[3] = tictactoe::BOARD_VALUE_X;
	startBoard.board[4] = tictactoe::BOARD_VALUE_O;
	numGenerated = generateNewBoards(startBoard, tictactoe::BOARD_VALUE_X, genBoards);
	if (TEST_1_EXPECTED_CNT != numGenerated) {
		fprintf( stderr, "ut_generateBoards --- FAILURE first board --- %d boards produced, expected %d\n",
				numGenerated, TEST_1_EXPECTED_CNT);
		return (false);
	}
	bool bPass = true;
	for (int nPos = 0; numGenerated > nPos; ++nPos) {
		initializeBoardFromState(reportBoard, TEST_1_EXPECTED[nPos]);
		initializeBoardFromState(reportBoard, genBoards[nPos].board);
		int cmp = memcmp(TEST_1_EXPECTED[nPos], genBoards[nPos].board, sizeof(startBoard.board));
		if (0x0 != cmp) {
			char expected[100];
			char got[100];
			strncpy(expected, debugQuickPrintBoard(reportBoard), 99);
			strncpy(got, debugQuickPrintBoard(genBoards[nPos]), 99);
			fprintf( stderr, "ut_generateBoards --- FAILURE first board --- %d compare, expected { %s } got { %s }\n",
					nPos, expected, got);
			bPass = false;
		}
	}
	if (!bPass)
		return (false);

	// And all is well
	return (true);

}

//
// Report on the results of a board
//

void reportBoard(const tictactoe::S_TTT_BOARD &theBoard) {

	const char BREAK[] = "----------";

	printf("X: %10u | X: %10u | X: %10u\n", theBoard.winsX[0], theBoard.winsX[1], theBoard.winsX[2]);
	printf("O: %10u | O: %10u | O: %10u\n", theBoard.winsO[0], theBoard.winsO[1], theBoard.winsO[2]);
	printf("D: %10u | D: %10u | D: %10u\n", theBoard.draws[0], theBoard.draws[1], theBoard.draws[2]);
	printf("---%10s-+----%10s-+----%10s\n", BREAK, BREAK, BREAK);
	printf("X: %10u | X: %10u | X: %10u\n", theBoard.winsX[3], theBoard.winsX[4], theBoard.winsX[5]);
	printf("O: %10u | O: %10u | O: %10u\n", theBoard.winsO[3], theBoard.winsO[4], theBoard.winsO[5]);
	printf("D: %10u | D: %10u | D: %10u\n", theBoard.draws[3], theBoard.draws[4], theBoard.draws[5]);
	printf("---%10s-+----%10s-+----%10s\n", BREAK, BREAK, BREAK);
	printf("X: %10u | X: %10u | X: %10u\n", theBoard.winsX[6], theBoard.winsX[7], theBoard.winsX[8]);
	printf("O: %10u | O: %10u | O: %10u\n", theBoard.winsO[6], theBoard.winsO[7], theBoard.winsO[8]);
	printf("D: %10u | D: %10u | D: %10u\n", theBoard.draws[6], theBoard.draws[7], theBoard.draws[8]);

}

//
// MAIN
//

int main(const int argc, const char *argv[]) {

	// Run unit tests
	bool bTestsPassed = ut_isBoardWinner();
	bTestsPassed &= ut_getNextPlayer();
	bTestsPassed &= ut_generateBoards();
	printf("Unit Tests: %s\n", bTestsPassed ? "Success" : "** FAILURE **");
	if (!bTestsPassed)
		exit( EXIT_FAILURE);

	// Start with an empty board
	tictactoe::S_TTT_BOARD startBoard;
	tictactoe::initializeBoardFromString(startBoard, "---------");

	// Generate
	uint8_t forPlayer = tictactoe::getNextPlayer( startBoard);
	processBoard(startBoard, forPlayer);

	// Report
	printf("Starting board = %s\n", debugQuickPrintBoard(startBoard));
	reportBoard(startBoard);

	// And show the JSON
	printf("\n\n");
	char *pBuffer = new char[90000];
	int nSize = convertBoardToJSON(startBoard, pBuffer, 90000);
	if (0 >= nSize)
		printf("Unable to convert to JSON");
	else
		printf("JSON: %s", pBuffer);
	delete[] pBuffer;

	// And done
	return ( EXIT_SUCCESS);

}
