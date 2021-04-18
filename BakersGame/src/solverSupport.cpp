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

// Required includes
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <string.h>
#include <time.h>

// STL includes
#include <set>
#include <string>
#include <utility>

// Project includes
#include <SolitaireBoard.h>
#include <SolitaireSolver.h>
#include <solverSupport.h>
#include <dbSupport.h>

//
// Check for a database solution
//
// @param pBoard A pointer to the board to solve
// @param lstMoves The list to which the moves will be appended
// @return The number of moves found, or zero if no solution was found, or negative on error
//

long findSolutionToBoard(CSolitaireBoard *pBoard, LST_MOVES &lstMoves) {

	// Variables
	bool isAlt = false;
	long retMoves = -1;
	S_ALTERNATE_BOARD altBoard;

	// Wrap it all
	try {

		// Allocate a DB object
		SolitaireDB sDB;
		if (!sDB.initDB())
			throw("Failure to init db");

		// See if the board has a solution
		BOARDID_T boardId = sDB.getPrimaryBoardId(pBoard, &isAlt);

		// Lookup the solution
		SOLUTIONID_T solutionId = sDB.getSolutionForBoard(boardId);
		if (0x0 == solutionId)
			return (0x0);
		else if (0 > solutionId)
			return (-1);

		// Now get the solution
		LST_MOVES lstMoves = sDB.getSolutionMoves(solutionId);
		if (lstMoves.empty())
			return (0x0);

		// Need to adjust for alternate board?
		if (isAlt) {
			if (sDB.getAlternateBoard(pBoard, altBoard)) { // @suppress("Invalid arguments")
				ITR_MOVES itrMove = lstMoves.begin();
				for (; lstMoves.end() != itrMove; ++itrMove) {
					MOVE_T theMove = *itrMove;
					if (MP_FOUNDATION == theMove.eMoveTo) {
						theMove.nToCol = altBoard.mapOfTo[theMove.nToCol];
					}
				}
			} else {
				lstMoves.clear();
				throw("Unable to lookup alternate board");
			}
		}

		// And get the number of moves
		retMoves = lstMoves.size();

	}

	catch (const char *e) {
#ifdef	ACTIVE_LOGGING
			fprintf( stderr, "Caught an exception in findSolutionToBoard -> %s\n", e);
#endif
		retMoves = -2;
	}

	// And done
	return (retMoves);

}

//
// Check for a database solution and, if not found, allocate and run a solver
//
// @param pBoard A pointer to the board to solve.
// @param pTimeout If not null, will be set to TRUE on a solver timeout
// @return A list of moves to solve - or an empty set if there is no solution
//

LST_MOVES findOrSolveBoard(CSolitaireBoard *pBoard, bool *pTimeout) {

	// Allocate vars
	CSolitaireSolver *pSolver = (CSolitaireSolver*) 0x0;
	LST_MOVES retMoves;

	// Wrap it all
	try {

		// Try the database
		long numMoves = findSolutionToBoard( pBoard, retMoves);
		if( 0 < numMoves) {
			return( retMoves);
		}

		// Okay - allocate a solver and execute
		pSolver = CSolitaireSolverFactory::GetSolver(pBoard);
		pSolver->SetBoardsToCheck(1500000);
		pSolver->SetIntelligence(12);
		pSolver->SetMaximumTime(100);
		if (pSolver->FindAnySolution()) {
			LST_MOVES *pSolutions = pSolver->GetSolution();
			retMoves.insert(retMoves.begin(), pSolutions->cbegin(), pSolutions->cend()); // @suppress("Invalid arguments")
		} else {
			if ((bool*) 0x0 != pTimeout) {
				*pTimeout = true;
			}
		}

	}

	catch (const char *e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Caught an exception in findOrSolveBoard -> %s\n", e);
#endif
		retMoves.clear();
	}

	// Clear resources
	if ((CSolitaireSolver*) 0x0 != pSolver) {
		delete pSolver;
		pSolver = (CSolitaireSolver*) 0x0;
	}

	// And done
	return (retMoves);

}

//
// Find alternate boards
//
// An alternate board is one where the suits are altered, for example,
// by swapping all of the clubs and diamonds.  All 24 potential boards
// will be returned by this function.
//
// @param pBoard A pointer to the board to solve.
// @return A list of every potential alternate board.
//

LST_ALTERNATE_BOARD findAlternateBoards(CSolitaireBoard *pBoard) {

	// Variables
	LST_ALTERNATE_BOARD retBoards;

	// Push initial board
	S_ALTERNATE_BOARD sNextValue;
	sNextValue.mapOfTo[CS_CLUBS] = CS_CLUBS;
	sNextValue.mapOfTo[CS_DIAMONDS] = CS_DIAMONDS;
	sNextValue.mapOfTo[CS_HEARTS] = CS_HEARTS;
	sNextValue.mapOfTo[CS_SPADES] = CS_SPADES;
	sNextValue.pBoard = pBoard;
	retBoards.push_back(sNextValue);

	// If not a BakersGame board, skip it
	if ((ST_BAKERS_GAME_EASY != pBoard->GetSolType()) && (ST_BAKERS_GAME != pBoard->GetSolType())) {
		return (retBoards);
	}
	CSB_BakersGame *pbgBoard = (CSB_BakersGame*) pBoard;

	// Start loops
	for (int c1 = 0; 4 > c1; ++c1) {
		for (int c2 = 0; 4 > c2; ++c2) {

			// Same?
			if (c1 == c2)
				continue;

			for (int c3 = 0; 4 > c3; ++c3) {

				// Same?
				if ((c1 == c3) || (c2 == c3))
					continue;

				for (int c4 = 0; 4 > c4; ++c4) {

					// Same?
					if ((c1 == c4) || (c2 == c4) || (c3 == c4))
						continue;

					// Is it the original board?
					if ((0 == c1) && (1 == c2) && (2 == c3) && (3 == c4)) {
						continue;
					}

					// Make the conversion
					CARDSUITS_T conversion[] =
							{ (CARDSUITS_T) c1, (CARDSUITS_T) c2, (CARDSUITS_T) c3, (CARDSUITS_T) c4 };

					// Make a new board
					CSB_BakersGameModifiable board(pbgBoard->isEasyMode());
					board.setPlayable(pbgBoard->IsPlayable());

					// Set the foundations
					board.setFoundation(CS_CLUBS, pbgBoard->GetFoundationCardValue(conversion[CS_CLUBS]));
					board.setFoundation(CS_DIAMONDS, pbgBoard->GetFoundationCardValue(conversion[CS_DIAMONDS]));
					board.setFoundation(CS_HEARTS, pbgBoard->GetFoundationCardValue(conversion[CS_HEARTS]));
					board.setFoundation(CS_SPADES, pbgBoard->GetFoundationCardValue(conversion[CS_SPADES]));

					// Set the reserve
					for (int nPos = 0; board.NumReserveSpaces() > nPos; ++nPos) {
						CARD_T rsvCard = pbgBoard->GetReserveCard(nPos);
						if ((rsvCard.eValue = CV_EMPTY) || (rsvCard.eSuit == CS_INVALID)) {
							board.setReserveSpace(nPos, rsvCard);
						} else {
							rsvCard.eSuit = conversion[rsvCard.eSuit];
							board.setReserveSpace(nPos, rsvCard);
						}
					}

					// Set the tableau
					for (int nRow = 0; pbgBoard->NumRows() > nRow; ++nRow) {
						for (int nCol = 0; pbgBoard->NumCols() > nCol; ++nCol) {

							// Get the card, convert, and store
							CARD_T tblCard = pbgBoard->GetTableauCardAt(nCol, nRow);
							if ((CV_EMPTY == tblCard.eValue) || (CS_INVALID == tblCard.eSuit)) {
								board.setTableau(nCol, nRow, tblCard);
							} else {
								tblCard.eSuit = conversion[tblCard.eSuit];
								board.setTableau(nCol, nRow, tblCard);
							}

						} // endfor cols
					} // endfor rows

					// Make a copy of this board and push it
					memcpy(sNextValue.mapOfTo, conversion, sizeof(sNextValue.mapOfTo));
					sNextValue.pBoard = CSolitaireBoard::CreateBoardCopy(&board);
					retBoards.push_back(sNextValue);

				} // endfor c4
			} // endfor c3
		} // endfor c2
	} // endfor c1

	// And done
	return (retBoards);

}
