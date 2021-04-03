//
//  CommonElements.h
//  TicTacToe
//
//  Created by Louis Gehrig on 3/21/21.
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

#ifndef INCLUDE_COMMONELEMENTS_H
#define INCLUDE_COMMONELEMENTS_H

#include <cstdint>

namespace tictactoe {

// Values for X, O, and empty
extern const uint8_t BOARD_VALUE_EMPTY;
extern const uint8_t BOARD_VALUE_O;
extern const uint8_t BOARD_VALUE_X;

// Win values
extern const uint8_t WIN_VALUE;

// The structure of a board
struct s_ttt_board {

	//
	// The actual board
	//
	// The board is arranged as follows:
	//
	//		0 | 1 | 2
	//		--+---+--
	//		3 | 4 | 5
	//		--+---+--
	//		6 | 7 | 8
	//

	uint8_t board[9];

	// The number of X wins if the current player takes the position
	unsigned int winsX[9];

	// The number of O wins if the current player takes the position
	unsigned int winsO[9];

	// The number of draws if the current players takes the position
	unsigned int draws[9];

	// The number of spaces that lead to an immediate win by the current player
	int numImmediateWins;

	// The board positions leading to an immediate win
	unsigned int immediateWins[9];

	// The number of spaces that lead to an immediate block by the current player
	int numImmediateBlocks;

	// The board positions leading to an immediate block
	unsigned int immediateBlocks[9];

};
typedef struct s_ttt_board S_TTT_BOARD;


};

#endif

