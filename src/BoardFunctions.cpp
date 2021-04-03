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
#include <memory.h>
#include <string.h>
#include <math.h>

// STL includes
#include <list>
#include <string>

// Debug includes
#define	INCLUDE_DEBUG_MACROS
#include <debug.h>

// Project includes
#include <CommonElements.h>
#include <BoardFunctions.h>

namespace tictactoe {

// Necessary constants
const uint8_t SUM_O_WIN = 3 * BOARD_VALUE_O;
const uint8_t SUM_X_WIN = 3 * BOARD_VALUE_X;
const uint64_t MASK_X_WIN = (((uint64_t) SUM_X_WIN) << 56) + (((uint64_t) SUM_X_WIN) << 48)
		+ (((uint64_t) SUM_X_WIN) << 40) + (((uint64_t) SUM_X_WIN) << 32) + (((uint64_t) SUM_X_WIN) << 24)
		+ (((uint64_t) SUM_X_WIN) << 16) + (((uint64_t) SUM_X_WIN) << 8) + ((uint64_t) SUM_X_WIN);
const uint64_t MASK_O_WIN = (((uint64_t) SUM_O_WIN) << 56) + (((uint64_t) SUM_O_WIN) << 48)
		+ (((uint64_t) SUM_O_WIN) << 40) + (((uint64_t) SUM_O_WIN) << 32) + (((uint64_t) SUM_O_WIN) << 24)
		+ (((uint64_t) SUM_O_WIN) << 16) + (((uint64_t) SUM_O_WIN) << 8) + ((uint64_t) SUM_O_WIN);

//
// Initialize a board to initial empty state
//
// @param theBoard The board to initialize
//

void initializeBoard(S_TTT_BOARD &theBoard) {
	memset(&theBoard, 0x0, sizeof(S_TTT_BOARD));
}

//
// Initialize a board to initial board state
//
// @param theBoard The board to initialize
// @param boardValues The board positions to initialize
//

void initializeBoardFromState(S_TTT_BOARD &theBoard, const uint8_t *boardValues) {
	memset(&theBoard, 0x0, sizeof(S_TTT_BOARD));
	memcpy(theBoard.board, boardValues, sizeof(theBoard.board));
}

//
// Initialize a board to an initial state
//
// @param theBoard The board to initialize
// @param boardString A string describing the board
//

void initializeBoardFromString( S_TTT_BOARD &theBoard, const char boardString[]) {

	// Clear it
	initializeBoard( theBoard);
	if( (const char *) 0x0 == boardString) return;

	// Assign values
	for( int nPos = 0; ((9 > nPos) && (0x0 != boardString[nPos])); ++ nPos) {
		if( ('x' == boardString[nPos]) || ('X' == boardString[nPos]))
			theBoard.board[nPos] = BOARD_VALUE_X;
		else if( ('o' == boardString[nPos]) || ('O' == boardString[nPos]))
			theBoard.board[nPos] = BOARD_VALUE_O;
		else
			theBoard.board[nPos] = BOARD_VALUE_EMPTY;
	}

}

//
// Is this board a win?
//
// @param theBoard The board to check
// @return The board value of the winner (BOARD_VALUE_O | BOARD_VALUE_X), or BOARD_VALUE_EMPTY for no winner.
//

uint8_t isBoardWinner(const S_TTT_BOARD &theBoard) {

	//
	// The board summary will be done as values
	//
	// bits		meaning
	// 0-7		leftmost column
	// 8-15		middle column
	// 16-23	rightmost column
	// 24-31	top row
	// 32-39	middle row
	// 40-47	bottom row
	// 48-55	diagonal upper right to lower left
	// 56-63	diagonal upper left to lower right

	// Variables
	uint64_t boardSummary = 0x0;
	uint8_t summaryO = 0x0;
	uint8_t summaryX = 0x0;

	// Add up the board
	boardSummary = (((uint64_t) (theBoard.board[0] + theBoard.board[3] + theBoard.board[6])))
			+ (((uint64_t) (theBoard.board[1] + theBoard.board[4] + theBoard.board[7])) << 8)
			+ (((uint64_t) (theBoard.board[2] + theBoard.board[5] + theBoard.board[8])) << 16)
			+ (((uint64_t) (theBoard.board[0] + theBoard.board[1] + theBoard.board[2])) << 24)
			+ (((uint64_t) (theBoard.board[3] + theBoard.board[4] + theBoard.board[5])) << 32)
			+ (((uint64_t) (theBoard.board[6] + theBoard.board[7] + theBoard.board[8])) << 40)
			+ (((uint64_t) (theBoard.board[2] + theBoard.board[4] + theBoard.board[6])) << 48)
			+ (((uint64_t) (theBoard.board[0] + theBoard.board[4] + theBoard.board[8])) << 56);

	// Mask it
	uint64_t boardMaskedO = boardSummary & MASK_O_WIN;
	uint64_t boardMaskedX = boardSummary & MASK_X_WIN;

	// Unwrapped loop
	summaryO |= (boardMaskedO & SUM_O_WIN) / SUM_O_WIN;
	summaryX |= (boardMaskedX & SUM_X_WIN) / SUM_X_WIN;
	boardMaskedO = (boardMaskedO >> 8);
	boardMaskedX = (boardMaskedX >> 8);
	summaryO |= (boardMaskedO & SUM_O_WIN) / SUM_O_WIN;
	summaryX |= (boardMaskedX & SUM_X_WIN) / SUM_X_WIN;
	boardMaskedO = (boardMaskedO >> 8);
	boardMaskedX = (boardMaskedX >> 8);
	summaryO |= (boardMaskedO & SUM_O_WIN) / SUM_O_WIN;
	summaryX |= (boardMaskedX & SUM_X_WIN) / SUM_X_WIN;
	boardMaskedO = (boardMaskedO >> 8);
	boardMaskedX = (boardMaskedX >> 8);
	summaryO |= (boardMaskedO & SUM_O_WIN) / SUM_O_WIN;
	summaryX |= (boardMaskedX & SUM_X_WIN) / SUM_X_WIN;
	boardMaskedO = (boardMaskedO >> 8);
	boardMaskedX = (boardMaskedX >> 8);
	summaryO |= (boardMaskedO & SUM_O_WIN) / SUM_O_WIN;
	summaryX |= (boardMaskedX & SUM_X_WIN) / SUM_X_WIN;
	boardMaskedO = (boardMaskedO >> 8);
	boardMaskedX = (boardMaskedX >> 8);
	summaryO |= (boardMaskedO & SUM_O_WIN) / SUM_O_WIN;
	summaryX |= (boardMaskedX & SUM_X_WIN) / SUM_X_WIN;
	boardMaskedO = (boardMaskedO >> 8);
	boardMaskedX = (boardMaskedX >> 8);
	summaryO |= (boardMaskedO & SUM_O_WIN) / SUM_O_WIN;
	summaryX |= (boardMaskedX & SUM_X_WIN) / SUM_X_WIN;
	boardMaskedO = (boardMaskedO >> 8);
	boardMaskedX = (boardMaskedX >> 8);
	summaryO |= (boardMaskedO & SUM_O_WIN) / SUM_O_WIN;
	summaryX |= (boardMaskedX & SUM_X_WIN) / SUM_X_WIN;

	// And done
	return ((summaryO * BOARD_VALUE_O) + (summaryX * BOARD_VALUE_X));

}

//
// Generate new possible boards from the input.
//
// WARNING!  As an optimization step this function will not consider
// the possibility of an invalid board as input.  An invalid input board
// will simply generate invalid output boards!
//
// WARNING!  The outputBoards parameter must be sized to hold at least
// 9 boards or a memory overwrite may occur.
//
// @param theBoard The input board to start
// @param thePlayer The player for this turn
// @param outputBoards An array (of at least size 9)
// @return The number of boards written to outputBoards
//

int generateNewBoards(const S_TTT_BOARD &theBoard, const uint8_t thePlayer, S_TTT_BOARD *outputBoards) {

	// Initialize return values
	int numBoards = 0;

	// Loop over board positions
	for (int nPos = 0; 9 > nPos; ++nPos) {

		// If the position is empty, that's the next board!
		if (BOARD_VALUE_EMPTY == theBoard.board[nPos]) {
			initializeBoardFromState(outputBoards[numBoards], theBoard.board);
			outputBoards[numBoards].board[nPos] = thePlayer;
			++numBoards;
		}

	}

	// And done
	return (numBoards);

}

//
// Determine the next player from the board
//
// @param theBoard The board to consider
//

uint8_t getNextPlayer(const S_TTT_BOARD &theBoard) {

	// Result lookup table
	static const uint8_t RETURN_VALUES[] = { BOARD_VALUE_X, BOARD_VALUE_O };

	//
	// * Unrolled loop
	// * An even number of pieces means X moves
	// * An odd number of pieces means O moves
	// * Do this with a simple XOR
	//

	uint8_t holdValue = 0x0;
	holdValue ^= theBoard.board[0];
	holdValue ^= theBoard.board[1];
	holdValue ^= theBoard.board[2];
	holdValue ^= theBoard.board[3];
	holdValue ^= theBoard.board[4];
	holdValue ^= theBoard.board[5];
	holdValue ^= theBoard.board[6];
	holdValue ^= theBoard.board[7];
	holdValue ^= theBoard.board[8];

	// It's counted even X's and even O's
	// Need to XOR the X and O together

	holdValue = ((holdValue >> 2) ^ (holdValue & 0x03));

	// And lookup to get result
	return (RETURN_VALUES[holdValue]);

}

//
// Process a board for wins and losses.
//
// This function will calculate all of the wins, losses, and draws for the players
// starting from the input position and player.
//
// @param startingBoard The board to start from.
// @param forPlayer The player making the move.
//

void processBoard(S_TTT_BOARD &startingBoard, const uint8_t forPlayer) {

	// Helpful typedefs
	typedef std::list<S_TTT_BOARD> LST_BOARDS;

	// Helpful constants
	const uint8_t VAL_E = tictactoe::BOARD_VALUE_EMPTY;
	const uint8_t VAL_O = tictactoe::BOARD_VALUE_O;
	const uint8_t VAL_X = tictactoe::BOARD_VALUE_X;

	// Generate new boards
	uint8_t curPlayer = forPlayer;
	const uint8_t otherPlayer = (forPlayer ^ (VAL_X + VAL_O));

	// Make a queue of boards to process
	LST_BOARDS boardQueue;

	// Loop over empty squares
	S_TTT_BOARD checkBoard;
	initializeBoardFromState(checkBoard, startingBoard.board);
	for (int nPlace = 0; 9 > nPlace; ++nPlace) {

		// If it's empty
		if (VAL_E == checkBoard.board[nPlace]) {

			// Is it a block?
			checkBoard.board[nPlace] = otherPlayer;
			uint8_t winner = isBoardWinner(checkBoard);
			if (otherPlayer == winner) {
				startingBoard.immediateBlocks[startingBoard.numImmediateBlocks] = nPlace;
				++startingBoard.numImmediateBlocks;
			}

			// Is it a win?
			checkBoard.board[nPlace] = forPlayer;
			winner = isBoardWinner(checkBoard);
			if (forPlayer == winner) {
				startingBoard.immediateWins[startingBoard.numImmediateWins] = nPlace;
				++startingBoard.numImmediateWins;
			}

			// Push it for current player
			boardQueue.push_back(checkBoard);

			// While there are items in the queue
			while (!boardQueue.empty()) {

				// Get a board
				S_TTT_BOARD &popBoard = boardQueue.front();

				// Is this a winning board?
				uint8_t winner = isBoardWinner(popBoard);
				if (VAL_X == winner) {
					++startingBoard.winsX[nPlace];
				} else if (VAL_O == winner) {
					++startingBoard.winsO[nPlace];
				} else {
					// Make new boards and add them to the queue
					S_TTT_BOARD nextMoves[9];
					curPlayer = getNextPlayer(popBoard);
					const int nextBoards = generateNewBoards(popBoard, curPlayer, nextMoves);
					for (int newBoard = 0; nextBoards > newBoard; ++newBoard) {
						boardQueue.push_back(nextMoves[newBoard]);
					}
					if (0x0 == nextBoards) {
						++startingBoard.draws[nPlace];
					}
				}

				// And pop the board
				boardQueue.pop_front();

			} // endwhile board queue not empty

			// And clear for next loop
			checkBoard.board[nPlace] = VAL_E;

		} // endif spot was empty

	} // endfor loop looking for empty

}

//
// Convert a board into a JSON object
//
// See "usage.txt" in the documents section for more information.
//
// @param theBoard The board to convert.
// @param outBuffer The output character buffer.
// @param outSize The output character buffer size.
// @return The number of bytes used for the conversion, or -1 for insufficient space.
//

int convertBoardToJSON(const S_TTT_BOARD &theBoard, char *outBuffer, const size_t outSize) {

	// Use an STL string to hold the results
	std::string outputVal;

	// Holding space
	char holding[100];

	// Construct
	outputVal.append("{\n");
	outputVal.append("\"moves\" : [");
	for (int nPos = 0; 9 > nPos; ++nPos) {
		if (0 != nPos)
			outputVal.append(", ");
		outputVal.append(" { \"xwins\" : ");
		sprintf(holding, "%u", theBoard.winsX[nPos]);
		outputVal.append(holding);
		outputVal.append(", \"owins\" : ");
		sprintf(holding, "%u", theBoard.winsO[nPos]);
		outputVal.append(holding);
		outputVal.append(", \"draws\" : ");
		sprintf(holding, "%u", theBoard.draws[nPos]);
		outputVal.append(holding);
		outputVal.append(" }");
	}
	outputVal.append(" ],\n");
	outputVal.append("\"wins\" : [ ");
	bool bApplyComma = false;
	for (int nPos = 0; theBoard.numImmediateWins > nPos; ++nPos) {
		if (bApplyComma)
			outputVal.append(", ");
		sprintf(holding, "%u", theBoard.immediateWins[nPos]);
		outputVal.append(holding);
		bApplyComma = true;
	}
	outputVal.append(" ],\n");
	outputVal.append("\"blocks\" : [ ");
	bApplyComma = false;
	for (int nPos = 0; theBoard.numImmediateBlocks > nPos; ++nPos) {
		if (bApplyComma)
			outputVal.append(", ");
		sprintf(holding, "%u", theBoard.immediateBlocks[nPos]);
		outputVal.append(holding);
		bApplyComma = true;
	}
	outputVal.append(" ]\n");
	outputVal.append("}\n");

	// Convert the string
	int retValue = outputVal.length();
	if (outputVal.length() >= outSize) {
		retValue = -1;
	} else {
		memset(outBuffer, 0x0, outSize);
		strncpy(outBuffer, outputVal.c_str(), outSize);
	}

	// And done
	return (retValue);

}

}
