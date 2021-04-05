
// Standard includes
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// Project includes
#include "SolitaireBoard.h"
#include "SolitaireSolver.h"
#include "SolitaireSolver_BakersGameEasy.h"

//
// Show usage
//

void ShowUsage( const char progName [])
{

	fprintf( stderr, "Usage: %s < board type > [ output filename ] [ starting seed ] [ number to run ] \r\n", progName) ;
	fprintf( stderr, "Board type 1 = Easy\r\n") ;
	fprintf( stderr, "Board type 2 = Standard\r\n") ;
	fprintf( stderr, "Board type 3 = Test mode - EASY\r\n") ;

}

//
// THE MAIN FUNCTION
//

int main (int argc, const char * argv[])
{

	// Constants
	const char					OUTPUT_FILE_EASY [] = "EasyBoards.bin" ;
	const char					OUTPUT_FILE_STND [] = "StndBoards.bin" ;
	const int					BOARDS_TO_CHECK = 1500000 ;
	const int					NUM_BOARDS = 1000 ;
	const int					USE_INTELLIGENCE = 12 ;
	const long					DEFAULT_SEED = 481066668 ;
	const SOLITAIRE_T			DEFAULT_BOARD_TYPE = ST_BAKERS_GAME_EASY ;

	// Variables
	bool						bInvalidArgs = false ;
	CSB_BakersGame				*pBakersGameBoard = (CSB_BakersGame *) 0x0 ;
	const char					*pOutputFileName = (const char *) 0x0 ;
	FILE						*fOutputBoards = (FILE *) 0x0 ;
	int							nBoardsToRun = NUM_BOARDS ;
	int							nBoardType = 0 ;
	int							nLoop ;
	int							nRetCode = EXIT_FAILURE ;
	int							nRandom = DEFAULT_SEED ;
	int							nRuntime ;
	long						nWinningBoards = 0x0 ;
	ITR_MOVES					itrMoves ;
	LST_MOVES					*pFinalMoves = (LST_MOVES *) 0x0 ;
	short						nNumMoves ;
	short						nTimeTotal ;
	SOLITAIRE_T					eBoardType = DEFAULT_BOARD_TYPE ;
	time_t						tNow, tProgStart, tProgStop, tStart, tStop ;

	// Wrap it all
	try
	{

		// Setup checks
		if( 4 != sizeof( int)) throw( "4 != sizeof( int)") ;
		if( 2 != sizeof( short)) throw( "2 != sizeof( short)") ;
		
		// Arguments
		for( nLoop = 1 ; nLoop < argc ; ++ nLoop)
		{

			// Control?
			if( '-' == argv [nLoop][0])
			{
				if( ('h' == argv [nLoop][1]) || ('H' == argv [nLoop][1]) || ('?' == argv [nLoop][1]))
				{
					ShowUsage( argv [0]) ;
					return (EXIT_SUCCESS) ;
				}
				else
				{
					bInvalidArgs = true ;
				}
			}
			
			// Board type?
			else if( 0 == nBoardType)
			{
				bInvalidArgs |= (1 != sscanf( argv [nLoop], "%d", &nBoardType)) ;
			}

			// Output filename?
			else if( (const char *) 0x0 == pOutputFileName)
			{
				pOutputFileName = argv [nLoop] ;
			}
			
			// Random seed?
			else if( DEFAULT_SEED == nRandom)
			{
				bInvalidArgs |= (1 != sscanf( argv [nLoop], "%d", &nRandom)) ;
			}

			// Boards to run
			else if( NUM_BOARDS == nBoardsToRun)
			{
				bInvalidArgs |= (1 != sscanf( argv [nLoop], "%d", &nBoardsToRun)) ;
			}

			// So ... invalid
			else bInvalidArgs = true ;

		}

		// Check values
		bInvalidArgs |= (0 >= nBoardType) ;
		bInvalidArgs |= (4 <= nBoardType) ;
		bInvalidArgs |= (0 >= nBoardsToRun) ;

		// Any invalid arguemnts?
		if( bInvalidArgs)
		{
			ShowUsage( argv [0]) ;
			throw( "Invalid program arguments") ;
		}

		// What type
		switch( nBoardType)
		{
			case 1 : 	eBoardType = ST_BAKERS_GAME_EASY ; break ;
			case 2 : 	eBoardType = ST_BAKERS_GAME ; break ;
			case 3 :	eBoardType = ST_BAKERS_GAME_EASY_TEST ; break ;
			default :	eBoardType = ST_BAKERS_GAME_EASY ; break ;
		}

		// Attempt to open output file
		if( (const char *) 0x0 == pOutputFileName)
		{
			switch( eBoardType)
			{
				case ST_BAKERS_GAME_EASY_TEST :	pOutputFileName = OUTPUT_FILE_EASY ; break ;
				case ST_BAKERS_GAME_EASY :		pOutputFileName = OUTPUT_FILE_EASY ; break ;
				case ST_BAKERS_GAME :			pOutputFileName = OUTPUT_FILE_STND ; break ;
				default :						throw( "Bad board type in MAIN") ; break ;
			}
		}
		fOutputBoards = fopen( pOutputFileName, "wb") ;
		if( (FILE *) 0x0 == fOutputBoards) throw( "Failed to open output file") ;

		// Note the starting time
		tProgStart = time( (time_t *) 0x0) ;
		printf( "Program PID: %d\r\n", getpid( )) ;
		printf( "Program start: %s", ctime( &tProgStart)) ;

		// Loop over the number of boards to create
		for( nLoop = 0 ; nBoardsToRun > nLoop ; ++ nLoop)
		{

			// Randomize
			srandom( nRandom) ;
			tNow = time( (time_t *) 0x0) ;
			nRuntime = tNow - tProgStart ;
			printf( "Starting board %d of %d (seed %d) at %sProgram runtime %u\r\n", nLoop + 1, nBoardsToRun, nRandom, ctime( &tNow), nRuntime) ;
			fflush( stdout) ;

			// Make up a board, a solver, and test it
			pBakersGameBoard = (CSB_BakersGame *) CSolitaireBoard::CreateBoard( eBoardType) ;
			if( (ST_BAKERS_GAME_TEST != eBoardType) && (ST_BAKERS_GAME_EASY_TEST != eBoardType)) pBakersGameBoard -> InitRandomBoard( ) ;
			if( (CSB_BakersGame *) 0x0 != pBakersGameBoard)
			{
				CSS_BakersGameEasy	*pSolver = (CSS_BakersGameEasy *) CSolitaireSolverFactory::GetSolver( pBakersGameBoard) ;
				pSolver -> SetBoardsToCheck( BOARDS_TO_CHECK) ;
				pSolver -> SetIntelligence( USE_INTELLIGENCE) ;
				tStart = time( (time_t *) 0x0) ;
				if( pSolver -> FindAnySolution( ))
				{
					tStop = time( (time_t *) 0x0) ;
					nTimeTotal = (short) (tStop - tStart) ;
					pFinalMoves = pSolver -> GetSolution( ) ;
					printf( "Found a solution!  The solution for board %d requires %lu moves (%hd seconds)\r\n", nRandom, pFinalMoves -> size( ), nTimeTotal) ;
					++ nWinningBoards ;
					nNumMoves = (short) pFinalMoves -> size( ) ;
					fwrite( &nRandom, sizeof( int), 1, fOutputBoards) ;
					fwrite( &nNumMoves, sizeof( short), 1, fOutputBoards) ;
					fwrite( &nTimeTotal, sizeof( short), 1, fOutputBoards) ;
					fflush( fOutputBoards) ;
					fflush( stdout) ;
					if( 1 == nBoardsToRun)
					{
						int	nSubLoop ;
						for( nSubLoop = 1, itrMoves = pFinalMoves -> begin( ) ; pFinalMoves -> end( ) != itrMoves ; ++ itrMoves, ++ nSubLoop)
						{
							printf( "Move %d: ", nSubLoop) ;
							PrintMove( stdout, *itrMoves) ;
							printf( "\r\n") ;
						}
					}
				}
				else
				{
					tStop = time( (time_t *) 0x0) ;
					nTimeTotal = (short) (tStop - tStart) ;
					printf( "No solution for board %d (%hd seconds)...\r\n", nRandom, nTimeTotal) ;
				}
				delete pBakersGameBoard ;
				delete pSolver ;
				pSolver = (CSS_BakersGameEasy *) 0x0 ;
				pBakersGameBoard = (CSB_BakersGame *) 0x0 ;
			}

			// Create new random seed
			nRandom = random( ) ;

			// Pause - maybe let kernel try to do some memory cleanup
			usleep( 200) ;
			
		} // endfor loop NUM_BOARDS times

		// Close file
		fclose( fOutputBoards) ;
		printf( "%ld winning boards identified!\r\n", nWinningBoards) ;

#ifdef	_DEBUG
		printf( "Press <Return> to exit ...") ;
		// char	buffer [1024] ;
		// gets( buffer) ;
#endif

		// Note the ending time
		tProgStop = time( (time_t *) 0x0) ;
		printf( "\r\nProgram start: %s", ctime( &tProgStart)) ;
		printf( "Program end: %s", ctime( &tProgStop)) ;

		// And all is well
		nRetCode = EXIT_SUCCESS ;
		
	}
	
	catch( const char *e)
	{
		char	strMessage [1024 + 50 + 1] ;
		strncpy( strMessage, "MAIN -> ", 50) ;
		strncat( strMessage, e, 1024) ;
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "%s", strMessage) ;
#endif
		fprintf( stderr, "%s\r\n", e) ;
		nRetCode = EXIT_FAILURE ;
	}
	
	catch( ... )
	{
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "MAIN -> unknown exception caught") ;
#endif
		nRetCode = EXIT_FAILURE ;
	}

	// And return
	return( nRetCode) ;

}
