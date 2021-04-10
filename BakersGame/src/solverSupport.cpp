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

//
// Check for a database solution and, if not found, allocate and run a solver
//
// @param pBoard A pointer to the board to solve.
// @param pTimeout If not null, will be set to TRUE on a solver timeout
// @return A list of moves to solve - or an empty set if there is no solution
//

LST_MOVES findOrSolveBoard(CSolitaireBoard *pBoard, bool *pTimeout) {

	// Allocate vars
	CSolitaireSolver *pSolver = (CSolitaireSolver *) 0x0;
	LST_MOVES retMoves;

	// Wrap it all
	try {

		// Check the database for a solution

		// Okay - allocate a solver and execute
		pSolver = CSolitaireSolverFactory::GetSolver(pBoard);
		pSolver -> SetBoardsToCheck( 1500000) ;
		pSolver -> SetIntelligence( 12) ;
		pSolver -> SetMaximumTime( 100);
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
	if( (CSolitaireSolver *) 0x0 != pSolver) {
		delete pSolver;
		pSolver = (CSolitaireSolver *) 0x0;
	}

	// And done
	return (retMoves);

}

