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
#include <string.h>
#include <time.h>
#include <sys/timeb.h>

// Project includes
#include <SolitaireBoard.h>
#include <SolitaireSolver.h>
#include <SolitaireSolver_BakersGameEasy.h>
#include <nodeSupport.h>
#include <solverSupport.h>
#include <dbSupport.h>

// Constants
static const int DEFAULT_BOARDS_TO_CHECK = 3000000;
static const int DEFAULT_INTELLIGENCE = 15;
static const int DEFAULT_MAX_TIME = 900;
static const int DEFAULT_NUM_BOARDS = 1000;
static const char DEFAULT_OUTPUT_FILE_EASY[] = "validSeedsBakersGameEasy.txt";
static const char DEFAULT_OUTPUT_FILE_STND[] = "validSeedsBakersGame.txt";
static const long DEFAULT_SEED = 481066668;
static const SOLITAIRE_T DEFAULT_BOARD_TYPE = ST_BAKERS_GAME_EASY;

//
// Show usage
//

void ShowUsage(const char progName[]) {

	fprintf( stderr, "Usage: %s < board type > [ output filename ] [ starting seed ] [ number to run ]\r\n", progName);
	fprintf( stderr, "Board type 1 = Easy\r\n");
	fprintf( stderr, "Board type 2 = Standard\r\n");
	fprintf( stderr, "Board type 3 = Test mode - EASY\r\n");

}

//
// Generate solution for a board.
//
// If a solution is found then it will be recorded in the database.
// The solver invoked by this method is a heavy-duty process, using
// far more resources than the method Method_BF_BakersGame_SolveBoard,
// as it should be used during generation and external solving.
//
// @param pBoard The board to generate the solution for.
// @param numBoards See Solver.h
// @param intelligence See Solver.h
// @param maxTIme See Solver.h
// @return TRUE if a solution was found, FALSE otherwise.
//

bool generateSolution(CSolitaireBoard *pBoard, int numBoards = DEFAULT_BOARDS_TO_CHECK, int intelligence =
		DEFAULT_INTELLIGENCE, int maxTime = DEFAULT_MAX_TIME) {

	// Variables
	bool bSolutionFound = false;
	struct timeb tStart;
	struct timeb tEnd;

	// Okay - allocate a solver and execute
	ftime(&tStart);
	CSolitaireSolver *pSolver = CSolitaireSolverFactory::GetSolver(pBoard);
	pSolver->SetBoardsToCheck(numBoards);
	pSolver->SetIntelligence(intelligence);
	pSolver->SetMaximumTime(maxTime);
	if (pSolver->FindAnySolution()) {

		// Found a solution!
		ftime(&tEnd);
		LST_MOVES *pSolutions = pSolver->GetSolution();

		// Save it all
		long duration = (1000L * (tEnd.time - tStart.time)) + (tEnd.millitm - tStart.millitm);
		saveMovesToDB(pBoard, *pSolutions, duration);
		bSolutionFound = true;

	}

	// Free solver
	delete pSolver;
	pSolver = (CSolitaireSolver*) 0x0;

	// And done
	return (bSolutionFound);

}

//
// THE MAIN FUNCTION
//

int main(int argc, const char *argv[]) {

	// Variables
	bool bInvalidArgs = false;
	CSB_BakersGame *pBakersGameBoard = (CSB_BakersGame*) 0x0;
	const char *pOutputFileName = (const char*) 0x0;
	double dblRunTime;
	FILE *fOutputBoards = (FILE*) 0x0;
	int nBoardsToRun = DEFAULT_NUM_BOARDS;
	int nBoardType = DEFAULT_BOARD_TYPE;
	int nLoop, nNonDashArg;
	int nRetCode = EXIT_FAILURE;
	int nRandom = DEFAULT_SEED;
	SOLITAIRE_T eBoardType = DEFAULT_BOARD_TYPE;
	time_t tNow, tProgStart, tProgStop, tStart;

	// Wrap it all
	try {

		// Setup checks for ints
		if (4 != sizeof(int))
			throw("4 != sizeof( int)");
		if (2 != sizeof(short))
			throw("2 != sizeof( short)");

		// Arguments
		nNonDashArg = 0;
		for (nLoop = 1; nLoop < argc; ++nLoop) {

			// Control?
			if ('-' == argv[nLoop][0]) {
				if (('h' == argv[nLoop][1]) || ('H' == argv[nLoop][1]) || ('?' == argv[nLoop][1])) {
					ShowUsage(argv[0]);
					return (EXIT_SUCCESS);
				} else {
					bInvalidArgs = true;
				}
			}

			// Board type?
			else if (0 == nNonDashArg) {
				bInvalidArgs |= (1 != sscanf(argv[nLoop], "%d", &nBoardType));
				++nNonDashArg;
			}

			// Output filename?
			else if (1 == nNonDashArg) {
				pOutputFileName = argv[nLoop];
				++nNonDashArg;
			}

			// Random seed?
			else if (2 == nNonDashArg) {
				bInvalidArgs |= (1 != sscanf(argv[nLoop], "%d", &nRandom));
				++nNonDashArg;
			}

			// Boards to run
			else if (3 == nNonDashArg) {
				bInvalidArgs |= (1 != sscanf(argv[nLoop], "%d", &nBoardsToRun));
				++nNonDashArg;
			}

			// So ... invalid
			else
				bInvalidArgs = true;

		}

		// Check values
		bInvalidArgs |= (0 >= nBoardType);
		bInvalidArgs |= (4 <= nBoardType);
		bInvalidArgs |= (0 >= nBoardsToRun);

		// Any invalid arguments?
		if (bInvalidArgs) {
			ShowUsage(argv[0]);
			throw("Invalid program arguments");
		}

		// What type
		switch (nBoardType) {
		case 1:
			eBoardType = ST_BAKERS_GAME_EASY;
			if ((const char*) 0x0 == pOutputFileName) {
				pOutputFileName = DEFAULT_OUTPUT_FILE_EASY;
			}
			break;
		case 2:
			eBoardType = ST_BAKERS_GAME;
			if ((const char*) 0x0 == pOutputFileName) {
				pOutputFileName = DEFAULT_OUTPUT_FILE_STND;
			}
			break;
		case 3:
			eBoardType = ST_BAKERS_GAME_EASY_TEST;
			nRandom = 1848153258;
			if ((const char*) 0x0 == pOutputFileName) {
				pOutputFileName = DEFAULT_OUTPUT_FILE_EASY;
			}
			break;
		default:
			eBoardType = ST_BAKERS_GAME_EASY;
			break;
		}

		fOutputBoards = fopen(pOutputFileName, "a");
		if ((FILE*) 0x0 == fOutputBoards)
			throw("Failed to open output file");

		// Note the starting time
		tProgStart = time((time_t*) 0x0);
		printf("Program PID: %d\r\n", getpid());
		printf("Program start: %s", ctime(&tProgStart));

		// Loop over the number of boards to create
		for (nLoop = 0; nBoardsToRun > nLoop; ++nLoop) {

			// Randomize
			srandom(nRandom);
			tNow = time((time_t*) 0x0);
			dblRunTime = difftime(tNow, tProgStart);
			printf("Starting board %d of %d (seed %d) at %sProgram runtime %f secs\r\n", nLoop + 1, nBoardsToRun,
					nRandom, ctime(&tNow), dblRunTime);
			fflush( stdout);

			// Make up a board and call for a solution
			pBakersGameBoard = (CSB_BakersGame*) CSolitaireBoard::CreateBoard(eBoardType);
			pBakersGameBoard->InitRandomBoard();
			tStart = time((time_t*) 0x0);
			if (generateSolution(pBakersGameBoard)) {
				tNow = time((time_t*) 0x0);
				dblRunTime = difftime(tNow, tStart);
				fprintf(fOutputBoards, "%d\n", nRandom);
				printf("Generated a solution for board %d in %f seconds\n", nRandom, dblRunTime);
			} else {
				tNow = time((time_t*) 0x0);
				dblRunTime = difftime(tNow, tStart);
				printf("Failure to generate a solution for board %d in %f seconds\n", nRandom, dblRunTime);
			}

			// Cleanup
			delete pBakersGameBoard;
			pBakersGameBoard = (CSB_BakersGame*) 0x0;

			// Create next random seed and pause - let the kernel do some memory cleanup maybe
			nRandom = random();
			usleep(200);

		} /// endfor loop over boards to create

		// Close file
		fclose(fOutputBoards);
		fOutputBoards = (FILE *) 0x0;

		// Note the ending time
		tProgStop = time((time_t*) 0x0);
		dblRunTime = difftime(tProgStop, tProgStart);
		printf("\n\nTotal run time: %f seconds\n", dblRunTime);
		printf("Program start: %s", ctime(&tProgStart));
		printf("Program end: %s", ctime(&tProgStop));

		// All is well
		nRetCode = EXIT_SUCCESS;

	}

	catch (const char *e) {
#ifdef	ACTIVE_LOGGING
		char strMessage[1024 + 50 + 1];
		strncpy(strMessage, "MAIN -> ", 50);
		strncat(strMessage, e, 1024);
		fprintf( stderr, "%s\n", strMessage) ;
#endif
		nRetCode = EXIT_FAILURE;
	}

	catch (...) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "MAIN -> unknown exception caught") ;
#endif
		nRetCode = EXIT_FAILURE;
	}

	// And return
	return (nRetCode);

}

