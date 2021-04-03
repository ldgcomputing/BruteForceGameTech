//
//  BoardFunctions.h
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

#ifndef INCLUDE_BOARDFUNCTIONS_H
#define INCLUDE_BOARDFUNCTIONS_H

// Required includes
#include <CommonElements.h>

namespace tictactoe {

//
// Initialize a board to initial empty state
//
// @param theBoard The board to initialize
//

void initializeBoard( S_TTT_BOARD &theBoard);

//
// Initialize a board to initial board state
//
// @param theBoard The board to initialize
// @param boardValues The board positions to initialize
//

void initializeBoardFromState( S_TTT_BOARD &theBoard, const uint8_t *boardValues);

//
// Initialize a board to an initial state
//
// @param theBoard The board to initialize
// @param boardString A string describing the board
//

void initializeBoardFromString( S_TTT_BOARD &theBoard, const char boardString[]);

//
// Determine the next player from the board
//
// @param theBoard The board to consider
//

uint8_t getNextPlayer( const S_TTT_BOARD &theBoard);

//
// Is this board a win?
//
// WARNING!  As an optimization step this function will not consider
// a case when there are multiple triples to win (including simultaneous
// X and O wins).  These would be invalid board states in any event, so
// this optimization will not actually impact general game flow.
//
// @param theBoard The board to check
// @return The board value of the winner (BOARD_VALUE_O | BOARD_VALUE_X), or BOARD_VALUE_EMPTY for no winner.
//

uint8_t isBoardWinner( const S_TTT_BOARD &theBoard);

//
// Generate new possible boards from the input.
//
// WARNING!  As an optimization step this function will not consider
// the possibility of an invalid board or player as input.  Invalid
// input will simply generate invalid output boards!
//
// WARNING!  The outputBoards parameter must be sized to hold at least
// 9 boards or a memory overwrite may occur.
//
// @param theBoard The input board to start
// @param thePlayer The player for this turn
// @param outputBoards An array (of at least size 9)
// @return The number of boards written to outputBoards
//

int generateNewBoards( const S_TTT_BOARD &theBoard, const uint8_t thePlayer, S_TTT_BOARD *outputBoards);

//
// Process a board for wins and losses.
//
// This function will calculate all of the wins, losses, and draws for the players
// starting from the input position and player.
//
// @param startingBoard The board to start from.
// @param forPlayer The player making the move.
//

void processBoard( S_TTT_BOARD &startingBoard, const uint8_t forPlayer);

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

int convertBoardToJSON( const S_TTT_BOARD &theBoard, char *outBuffer, const size_t outSize);


};

#endif

