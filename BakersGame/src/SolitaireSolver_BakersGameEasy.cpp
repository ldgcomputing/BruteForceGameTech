/*
 *  SolitaireSolver_BakersGameEasy.cpp
 *  Solitaire Test
 *
 *  Created by Louis Gehrig on 11/8/10.
 *  Copyright 2010 Louis Gehrig. All rights reserved.
 *
 */

// STL includes
#include <algorithm>
#include <iterator>
#include <list>
#include <queue>
#include <set>
#include <stack>
#include <vector>

// Project includes
#include "SolitaireBoard.h"
#include "SolitaireSolver.h"
#include "SolitaireSolver_BakersGameEasy.h"

// Initial typedefs
typedef	std::set< COMP_BOARD >			SET_BOARDS ;
typedef	SET_BOARDS::iterator			ITRS_BOARDS ;
typedef	std::pair< ITRS_BOARDS , bool >	ITRC_BOARDS ;

// Structures
struct	s_board_node {
	int						nPriority ;
	int						nMovesSinceFoundationPlacement ;
	CSB_BakersGame			*pcTheBoard ;
	LST_MOVES				cntPreviousMoves ;
} ;

// Helpful typedefs
typedef	struct s_board_node						BOARD_NODE ;
typedef	std::stack< BOARD_NODE >				CNT_SBOARDS ;
typedef	std::vector< BOARD_NODE >				VEC_SBOARDS ;
typedef	VEC_SBOARDS::iterator					ITR_SBOARDS ;

// Utility functions
void ClearBoardNode( BOARD_NODE &cNode)
{
	cNode.nPriority = 0x0 ;
	cNode.cntPreviousMoves.clear( ) ;
	cNode.nMovesSinceFoundationPlacement = 0x0 ;
	if( (CSB_BakersGame *) 0x0 != cNode.pcTheBoard) delete cNode.pcTheBoard ;
	cNode.pcTheBoard = (CSB_BakersGame *) 0x0 ;
}

bool sortBoardsByPriority( const BOARD_NODE &left, const BOARD_NODE &right)
{
	return( left.nPriority < right.nPriority) ;
}

//
// The CSS_BakersGameEasy class
//

// Construction
CSS_BakersGameEasy::CSS_BakersGameEasy( CSolitaireBoard *pBoard) :
	CSolitaireSolver( pBoard)
{

	// Initialize
	m_pcCurBoard =		(CSB_BakersGame *) pBoard ;
	m_pcInitialBoard =	(CSB_BakersGame *) CSolitaireBoard::CreateBoardCopy( pBoard) ;

}

// Destruction
CSS_BakersGameEasy::~CSS_BakersGameEasy( )
{

	// Free up allocated resources
	if( (CSB_BakersGame *) 0x0 != m_pcInitialBoard) delete m_pcInitialBoard ;
	m_pcInitialBoard = (CSB_BakersGame *) 0x0 ;

}

// Find any solution
bool CSS_BakersGameEasy::FindAnySolution( )
{

	// Implementation notes:
	//
	// A stack structure is used to provide a more depth-first search rather than a breadth-first search.
	// A breadth-first would likely use more memory.
	//
	// Fortunately, the nature of the game prevents the "move-back-and-forth" of cards, since a stack can
	// only be moved to the same suit (unlike, say, Klondike where a stack could bounce between identical
	// colors).  However, the algorithm WILL prevent the movement of an entire stack to an empty line, as
	// that's a fairly pointless move.
	//

	// Constants
	const int					NUM_COLS = m_pcInitialBoard -> NumCols( ) ;
	const int					NUM_RESV = m_pcInitialBoard -> NumReserveSpaces( ) ;
	const int					NUM_ROWS = m_pcInitialBoard -> NumRows( ) ;
	const int					REPORTING_INTERVAL = 60 ;
	const size_t				VECTOR_RESERVE_AMOUNT = 20 ;

	// Variables
	bool						bIsWinnable = false ;
	bool						bMovedStackToOpen = false ;
	BOARD_NODE					cNextBoardNode ;
	BOARD_NODE					cCurBoardNode ;
	BOARD_NODE					*pcCurBoardNode = (BOARD_NODE *) 0x0 ;
	CARD_T						cAboveCard, cCurCard, cMoveCard ;
	CNT_SBOARDS					cntBoards ;
	CNT_SBOARDS					cntReserveMoveOnly ;
	COMP_BOARD					cCompBoard ;
	double						fDiffTime ;
	int							nCardVal1, nCardVal2 ;
	int							nCol, nReserve, nRow ;
	int							nLoopCol, nLoopRow ;
	ITRC_BOARDS					itrcBoards ;
	ITR_SBOARDS					itrNextBoards ;
	MOVE_T						cNextMove ;
	SET_BOARDS					cntBoardsSeen ;
	size_t						nBoardsChecked = 0 ;
	size_t						nBoardsSkipped = 0 ;
	size_t						nMaxBoardSize = 0 ;
	time_t						tCurTime, tLastBoardReport, tStartSolver ;
	VEC_SBOARDS					cntNextBoards ;

	// Wrap it
	try
	{

		// Clear the current solution
		m_lstMovesToSolve.clear( ) ;
		cntNextBoards.reserve( VECTOR_RESERVE_AMOUNT) ;

		// Setup
		tStartSolver = tCurTime = tLastBoardReport = time( (time_t *) 0x0) ;

		// Throw the initial board on the stack
		cNextBoardNode.pcTheBoard = (CSB_BakersGame *) 0x0 ;
		ClearBoardNode( cNextBoardNode) ;
		cNextBoardNode.pcTheBoard = (CSB_BakersGame *) CSolitaireBoard::CreateBoardCopy( m_pcCurBoard) ;
		if( (CSB_BakersGame *) 0x0 == cNextBoardNode.pcTheBoard) throw( "Failed to create board copy at start") ;
		cntBoards.push( cNextBoardNode) ;

		// While there is something on the stack ...
		while( (! bIsWinnable) && ((0x0 < cntBoards.size( )) || (0x0 < cntReserveMoveOnly.size( ))))
		{

			// Pull the reference
			if( ! cntBoards.empty( ))
			{
				cCurBoardNode = cntBoards.top( ) ;
				cntBoards.pop( ) ;
			}
			else if( ! cntReserveMoveOnly.empty( ))
			{
				cCurBoardNode = cntReserveMoveOnly.top( ) ;
				cntReserveMoveOnly.pop( ) ;
			}

			// Check if this board has been seen
			if( cCurBoardNode.pcTheBoard -> RotateAndCompressBoard( &cCompBoard))
			// if( cCurBoardNode.pcTheBoard -> CompressBoard( &cCompBoard))
			{
				itrcBoards = cntBoardsSeen.insert( cCompBoard) ;
				if( ! itrcBoards.second) {
					ClearBoardNode( cCurBoardNode) ;
					++ nBoardsSkipped ;
					continue ;
				}
			}

			// Report it?
			tCurTime = time( (time_t *) 0x0) ;
			if( (cntBoards.size( ) > nMaxBoardSize) || (REPORTING_INTERVAL < (tCurTime - tLastBoardReport)))
			{
				int			nBoardValue = 0 ;
				nBoardValue += cCurBoardNode.pcTheBoard -> GetFoundationCardValue( CS_CLUBS) ;
				nBoardValue += cCurBoardNode.pcTheBoard -> GetFoundationCardValue( CS_DIAMONDS) ;
				nBoardValue += cCurBoardNode.pcTheBoard -> GetFoundationCardValue( CS_HEARTS) ;
				nBoardValue += cCurBoardNode.pcTheBoard -> GetFoundationCardValue( CS_SPADES) ;
#ifdef	ACTIVE_LOGGING
				char		strOutput [1024 + 1] ;
				sprintf(
						strOutput,
						"Stack sizes %lu / %lu, at depth %d, %lu moves, %lu boards checked, %lu boards skipped, %d priority, %d value",
						cntBoards.size( ),
						cntReserveMoveOnly.size( ),
						cCurBoardNode.nMovesSinceFoundationPlacement,
						cCurBoardNode.cntPreviousMoves.size( ),
						nBoardsChecked,
						nBoardsSkipped,
						cCurBoardNode.nPriority,
						nBoardValue) ;
				fprintf( stderr, "%s\n", strOutput) ;
#endif
				if( cntBoards.size( ) > nMaxBoardSize) nMaxBoardSize = cntBoards.size( ) ;
				tLastBoardReport = time( (time_t *) 0x0) ;
			}

			// Is this a winning board?
			if( cCurBoardNode.pcTheBoard -> IsWinner( ))
			{
				m_lstMovesToSolve.insert( m_lstMovesToSolve.begin( ), cCurBoardNode.cntPreviousMoves.begin( ), cCurBoardNode.cntPreviousMoves.end( )) ; // @suppress("Invalid arguments")
				ClearBoardNode( cCurBoardNode) ;
				bIsWinnable = true ;
				break ;
			}

			// First - look for anything that can be pushed up to the foundation piles
			// Because of the stack structure, everything gets moved that can get moved
			cNextMove = cCurBoardNode.pcTheBoard -> AutoComplete( ) ;
			if( MP_INVALID != cNextMove.eMoveFrom)
			{

				// Setup the next node - note, cNextBoardNode will always have a pointer to an existing board that should not be deleted!
				cNextBoardNode.pcTheBoard = (CSB_BakersGame *) 0x0 ;
				ClearBoardNode( cNextBoardNode) ;
				cNextBoardNode.pcTheBoard = (CSB_BakersGame *) CSolitaireBoard::CreateBoardCopy( cCurBoardNode.pcTheBoard) ;
				if( (CSB_BakersGame *) 0x0 == cNextBoardNode.pcTheBoard) throw( "Failed to create a board copy - auto complete") ;
				cNextBoardNode.cntPreviousMoves.insert( // @suppress("Invalid arguments")
													   cNextBoardNode.cntPreviousMoves.begin( ),
													   cCurBoardNode.cntPreviousMoves.begin( ),
													   cCurBoardNode.cntPreviousMoves.end( )) ;

				// Apply the decided move, and push it in
				cNextBoardNode.pcTheBoard -> ApplyMove( cNextMove) ;
				cNextBoardNode.cntPreviousMoves.push_back( cNextMove) ;
				cntBoards.push( cNextBoardNode) ;
				ClearBoardNode( cCurBoardNode) ;

				// Since it's an automatic, loop back
				continue ;

			}

			// Gone past the point of intelligence?
			if( GetIntelligence( ) < cCurBoardNode.nMovesSinceFoundationPlacement)
			{
				ClearBoardNode( cCurBoardNode) ;
				continue ;
			}
			if( (size_t) GetMaximumMoves( ) < cCurBoardNode.cntPreviousMoves.size( ))
			{
				ClearBoardNode( cCurBoardNode) ;
				continue ;
			}
			if( (size_t) GetBoardsToCheck( ) < nBoardsChecked)
			{
				ClearBoardNode( cCurBoardNode) ;
				break ;
			}
			fDiffTime = difftime( tCurTime, tStartSolver) ;
			if( GetMaximumTime( ) < fDiffTime)
			{
				ClearBoardNode( cCurBoardNode) ;
				break ;
			}
			if( ((size_t) GetMaximumStackSize( ) < cntBoards.size( )) || ((size_t) GetMaximumStackSize( ) < cntReserveMoveOnly.size( )))
			{

				// Sub-variables
				BOARD_NODE					cInnerBoardNode ;
				CNT_SBOARDS					cntAllBoards ;
				SET_BOARDS					cntBoardsDuplicate( cntBoardsSeen) ;
				size_t						nBoardsRemoved = 0x0 ;

				// Dump all the boards
				cntAllBoards.push( cCurBoardNode) ;
				while( ! cntBoards.empty( ))
				{
					cInnerBoardNode = cntBoards.top( ) ;
					if( cInnerBoardNode.pcTheBoard -> RotateAndCompressBoard( &cCompBoard))
					{
						itrcBoards = cntBoardsDuplicate.insert( cCompBoard) ;
						if( ! itrcBoards.second) {
							ClearBoardNode( cInnerBoardNode) ;
							++ nBoardsRemoved ;
						}
						else
						{
							cntAllBoards.push( cntBoards.top( )) ;
						}
					}
					cntBoards.pop( ) ;
				}
				while( ! cntReserveMoveOnly.empty( ))
				{
					cInnerBoardNode = cntReserveMoveOnly.top( ) ;
					if( cInnerBoardNode.pcTheBoard -> RotateAndCompressBoard( &cCompBoard))
					{
						itrcBoards = cntBoardsDuplicate.insert( cCompBoard) ;
						if( ! itrcBoards.second) {
							ClearBoardNode( cInnerBoardNode) ;
							++ nBoardsRemoved ;
						}
						else
						{
							cntAllBoards.push( cntReserveMoveOnly.top( )) ;
						}
					}
					cntReserveMoveOnly.pop( ) ;
				}

				// Add it up
				nBoardsSkipped += nBoardsRemoved ;

				// Still too many?
				if( (size_t) GetMaximumStackSize( ) < cntAllBoards.size( ))
				{
#ifdef	ACTIVE_LOGGING
					fprintf( stderr, "Stack limit has been exceeded ... even after removal of %lu data\n", nBoardsRemoved) ;
					fprintf( stderr, "cntBoards.size( ) = %lu\n", cntBoards.size( )) ;
					fprintf( stderr, "cntReserveMoveOnly.size( ) = %lu\n", cntReserveMoveOnly.size( )) ;
					fprintf( stderr, "cntBoardsSeen.size( ) = %lu\n", cntBoardsSeen.size( )) ;
#endif
					ClearBoardNode( cCurBoardNode) ;
					break ;
				}

				// Push them all back
				while( ! cntAllBoards.empty( ))
				{
					cInnerBoardNode = cntAllBoards.top( ) ;
					if( 0x0 == cInnerBoardNode.nPriority)
						cntReserveMoveOnly.push( cInnerBoardNode) ;
					else
						cntBoards.push( cInnerBoardNode) ;
					cntAllBoards.pop( ) ;
				}
				/*
				asl_log( NULL, NULL, ASL_LEVEL_WARNING, "Stack limit exceeded but okay after removal of %lu data", nBoardsRemoved) ;
				asl_log( NULL, NULL, ASL_LEVEL_WARNING, "cntBoards.size( ) = %lu", cntBoards.size( )) ;
				asl_log( NULL, NULL, ASL_LEVEL_WARNING, "cntReserveMoveOnly.size( ) = %lu", cntReserveMoveOnly.size( )) ;
				asl_log( NULL, NULL, ASL_LEVEL_WARNING, "cntBoardsSeen.size( ) = %lu", cntBoardsSeen.size( )) ;
				*/

				// And clear the sets
				cntBoardsDuplicate.clear( ) ;

				// Call continue, since will have duplicates on the current board
				continue ;

			}

			// Board has never been seen before and is good to go ...
			++ nBoardsChecked ;

			// Second - look for any open reserve slots
			for( nReserve = NUM_RESV - 1 ; 0 <= nReserve ; -- nReserve)
			{
				cCurCard = cCurBoardNode.pcTheBoard -> GetReserveCard( nReserve) ;
				if( CS_INVALID == cCurCard.eSuit) break ;
			}
			if( 0 <= nReserve)
			{
				// Put the lowest card of every column in the reserve space
				for( nCol = NUM_COLS - 1 ; 0 <= nCol ; -- nCol)
				{
					for( nRow = NUM_ROWS - 1 ; 0 <= nRow ; -- nRow)
					{
						cCurCard = cCurBoardNode.pcTheBoard -> GetTableauCardAt( nCol, nRow) ;
						if( CS_INVALID != cCurCard.eSuit)
						{

							// Throw this card in
							cNextMove.eMoveFrom =	MP_TABLEAU ;
							cNextMove.nFromCol =	nCol ;
							cNextMove.nFromRow =	nRow ;
							cNextMove.eMoveTo =		MP_RESERVE ;
							cNextMove.nToCol =		nReserve ;
							cNextMove.nToRow =		0x0 ;

							// Setup the next node - note, cNextBoardNode will always havea pointer to an existing board that should not be deleted!
							cNextBoardNode.pcTheBoard = (CSB_BakersGame *) 0x0 ;
							ClearBoardNode( cNextBoardNode) ;
							cNextBoardNode.pcTheBoard = (CSB_BakersGame *) CSolitaireBoard::CreateBoardCopy( cCurBoardNode.pcTheBoard) ;
							if( (CSB_BakersGame *) 0x0 == cNextBoardNode.pcTheBoard) throw( "Failed to create a board copy - reserve up") ;
							cNextBoardNode.cntPreviousMoves.insert( // @suppress("Invalid arguments")
																   cNextBoardNode.cntPreviousMoves.begin( ),
																   cCurBoardNode.cntPreviousMoves.begin( ),
																   cCurBoardNode.cntPreviousMoves.end( )) ;

							// Apply the decided move, and push it in
							if( cNextBoardNode.pcTheBoard -> ApplyMove( cNextMove))
							{
								cNextBoardNode.nMovesSinceFoundationPlacement = cCurBoardNode.nMovesSinceFoundationPlacement + 1 ;
								cNextBoardNode.cntPreviousMoves.push_back( cNextMove) ;
								cntNextBoards.push_back( cNextBoardNode) ;
							}

							// And move to next column
							break ;

						} // endif got a card
					} // endfor nRow
				} // endfor nCol

			} // endif open reserve slot

			// Third - can anything be moved from a reserve position?
			for( nReserve = NUM_RESV - 1 ; 0 <= nReserve ; -- nReserve)
			{
				cCurCard = cCurBoardNode.pcTheBoard -> GetReserveCard( nReserve) ;
				if( CS_INVALID != cCurCard.eSuit)
				{

					// Look for the lowest spot to put it
					for( nCol = NUM_COLS - 1 ; 0 <= nCol ; -- nCol)
					{
						for( nRow = NUM_ROWS - 1 ; 0 <= nRow ; -- nRow)
						{

							// First card?
							cMoveCard = cCurBoardNode.pcTheBoard -> GetTableauCardAt( nCol, nRow) ;
							if( CS_INVALID != cMoveCard.eSuit)
							{

								// Not the same suit?  Not a good value?x	
								if( cCurCard.eSuit != cMoveCard.eSuit) break ;
								nCardVal1 = cCurCard.eValue ;
								nCardVal2 = cMoveCard.eValue ;
								if( (nCardVal1 + 1) != nCardVal2) break ;

								// Push this move
								cNextMove.eMoveFrom =	MP_RESERVE ;
								cNextMove.nFromCol =	nReserve ;
								cNextMove.nFromRow =	0x0 ;
								cNextMove.eMoveTo =		MP_TABLEAU ;
								cNextMove.nToCol =		nCol ;
								cNextMove.nToRow =		nRow ;

								// Setup the next node - note, cNextBoardNode will always havea pointer to an existing board that should not be deleted!
								cNextBoardNode.pcTheBoard = (CSB_BakersGame *) 0x0 ;
								ClearBoardNode( cNextBoardNode) ;
								cNextBoardNode.pcTheBoard = (CSB_BakersGame *) CSolitaireBoard::CreateBoardCopy( cCurBoardNode.pcTheBoard) ;
								if( (CSB_BakersGame *) 0x0 == cNextBoardNode.pcTheBoard) throw( "Failed to create a board copy - reserve down") ;
								cNextBoardNode.cntPreviousMoves.insert( // @suppress("Invalid arguments")
																	   cNextBoardNode.cntPreviousMoves.begin( ),
																	   cCurBoardNode.cntPreviousMoves.begin( ),
																	   cCurBoardNode.cntPreviousMoves.end( )) ;

								// Apply the decided move, and push it in
								if( cNextBoardNode.pcTheBoard -> ApplyMove( cNextMove))
								{
									cNextBoardNode.nMovesSinceFoundationPlacement = cCurBoardNode.nMovesSinceFoundationPlacement + 1 ;
									cNextBoardNode.cntPreviousMoves.push_back( cNextMove) ;
									cntNextBoards.push_back( cNextBoardNode) ;
								}

								// And move to next column
								break ;

							}

						} // endfor ROW
					} // endfor COL
				} // endif found a card
			} // endfor loop over reserve

			// Fourth - can a stack be moved onto another stack?
			for( nLoopCol = NUM_COLS - 1 ; 0 <= nLoopCol ; -- nLoopCol)
			{
				for( nLoopRow = NUM_ROWS - 1 ; 0 <= nLoopRow ; -- nLoopRow)
				{

					// Got a card?
					cCurCard = cCurBoardNode.pcTheBoard -> GetTableauCardAt( nLoopCol, nLoopRow) ;
					nCardVal1 = cCurCard.eValue ;
					if( CS_INVALID != cCurCard.eSuit)
					{

						// Pull the card above, and above, and above ... make a stack
						while( 0 < nLoopRow)
						{
							cCurCard = cCurBoardNode.pcTheBoard -> GetTableauCardAt( nLoopCol, nLoopRow) ;
							cAboveCard = cCurBoardNode.pcTheBoard -> GetTableauCardAt( nLoopCol, nLoopRow - 1) ;
							if( cAboveCard.eSuit != cCurCard.eSuit) break ;
							nCardVal1 = cCurCard.eValue ;
							nCardVal2 = cAboveCard.eValue ;
							if( (nCardVal1 + 1) != nCardVal2) break ;
							-- nLoopRow ;
						}

						// Now look for somewhere to put it
						for( bMovedStackToOpen = false, nCol = NUM_COLS - 1 ; 0 <= nCol ; -- nCol)
						{

							if( bMovedStackToOpen) break ;		// if moved a stack to an open col, don't look for other open cols
							if( nCol == nLoopCol) continue ;	// ignore self-moves
							for( nRow = NUM_ROWS - 1 ; 0 <= nRow ; -- nRow)
							{

								// Pull the card
								cMoveCard = cCurBoardNode.pcTheBoard -> GetTableauCardAt( nCol, nRow) ;
								nCardVal2 = cMoveCard.eValue ;

								// Not found anything?
								if( (CS_INVALID == cMoveCard.eSuit) && (0 < nRow)) continue ;

								// Same suit, or empty row 0, required
								if(
								   ((cMoveCard.eSuit == cCurCard.eSuit) && ((nCardVal1 + 1) == nCardVal2)) ||
								   ((CS_INVALID == cMoveCard.eSuit) && (0 == nRow))
								)
								{

									// Move to an open col?
									bMovedStackToOpen = ((CS_INVALID == cMoveCard.eSuit) && (0 == nRow)) ;

									// Throw this card in
									cNextMove.eMoveFrom =	MP_TABLEAU ;
									cNextMove.nFromCol =	nLoopCol ;
									cNextMove.nFromRow =	nLoopRow ;
									cNextMove.eMoveTo =		MP_TABLEAU ;
									cNextMove.nToCol =		nCol ;
									cNextMove.nToRow =		nRow ;

									// Setup the next node - note, cNextBoardNode will always havea pointer to an existing board that should not be deleted!
									cNextBoardNode.pcTheBoard = (CSB_BakersGame *) 0x0 ;
									ClearBoardNode( cNextBoardNode) ;
									cNextBoardNode.pcTheBoard = (CSB_BakersGame *) CSolitaireBoard::CreateBoardCopy( cCurBoardNode.pcTheBoard) ;
									if( (CSB_BakersGame *) 0x0 == cNextBoardNode.pcTheBoard) throw( "Failed to create a board copy - tableau") ;
									cNextBoardNode.cntPreviousMoves.insert( // @suppress("Invalid arguments")
																		   cNextBoardNode.cntPreviousMoves.begin( ),
																		   cCurBoardNode.cntPreviousMoves.begin( ),
																		   cCurBoardNode.cntPreviousMoves.end( )) ;

									// Apply the decided move, and push it in
									if( cNextBoardNode.pcTheBoard -> ApplyMove( cNextMove))
									{
										cNextBoardNode.nMovesSinceFoundationPlacement = cCurBoardNode.nMovesSinceFoundationPlacement + 1 ;
										cNextBoardNode.cntPreviousMoves.push_back( cNextMove) ;
										cntNextBoards.push_back( cNextBoardNode) ;
									}
									else
									{
#ifdef	ACTIVE_LOGGING
										fprintf( stderr, "Failure to apply a move in the tableau\n") ;
#endif
									}

									// And move to next column
									break ;

								}

								// Found the lowest card, and it doesn't match
								else
								{
									break ;
								}

							} // endfor nRow
						} // endfor nCol

						// And move to the next column
						break ;

					} // endif got the card

				} // endfor loopRow
			} // endfor loopCol

			// Determine priority on each next board
			for( itrNextBoards = cntNextBoards.begin( ) ; cntNextBoards.end( ) != itrNextBoards ; ++ itrNextBoards)
			{

				// Pull board
				pcCurBoardNode = & (*itrNextBoards) ;
				pcCurBoardNode -> nPriority = 0 ;

				// Is there an automatic solution?
				cNextMove = pcCurBoardNode -> pcTheBoard -> AutoComplete( ) ;
				if( MP_INVALID != cNextMove.eMoveFrom)
				{
					pcCurBoardNode -> nPriority += 1000 ;
				}

				// Tableau to tableau is second best
				cNextMove = pcCurBoardNode -> cntPreviousMoves.back( ) ;
				if( (MP_TABLEAU == cNextMove.eMoveFrom) && (MP_TABLEAU == cNextMove.eMoveTo))
				{
					pcCurBoardNode -> nPriority += 500 ;
				}

				// Reserve to tableau is third best
				if( (MP_RESERVE == cNextMove.eMoveFrom) && (MP_TABLEAU == cNextMove.eMoveTo))
				{
					pcCurBoardNode -> nPriority += 200 ;
				}

			}

			// Sort the next boards and push them onto the stack
			std::sort( cntNextBoards.begin( ), cntNextBoards.end( ), sortBoardsByPriority) ;
#ifdef	DEBUG_INSTRUMENT
			for( itrNextBoards = cntNextBoards.begin( ) ; cntNextBoards.end( ) != itrNextBoards ; ++ itrNextBoards)
			{
				pcCurBoardNode = & (*itrNextBoards) ;
				cNextMove = pcCurBoardNode -> cntPreviousMoves.back( ) ;
				printf( "With priority %d -> ", pcCurBoardNode -> nPriority) ;
				PrintMove( stdout, cNextMove) ;
				printf( "\r\n") ;
			}
			printf( "--------------------------\r\n") ;
#endif
			for( itrNextBoards = cntNextBoards.begin( ) ; cntNextBoards.end( ) != itrNextBoards ; ++ itrNextBoards)
			{
				if( 0 < itrNextBoards -> nPriority)
					cntBoards.push( *itrNextBoards) ;
				else
					cntReserveMoveOnly.push( *itrNextBoards) ;
			}
			cntNextBoards.clear( ) ;
			cntNextBoards.reserve( VECTOR_RESERVE_AMOUNT) ;

			// Clear the current board
			ClearBoardNode( cCurBoardNode) ;

		} // endwhile something on the stack

		// Clear the stack of anything remaining (in event of a win)
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "cntBoards.size( ) = %lu\n", cntBoards.size( )) ;
#endif
		while( 0x0 < cntBoards.size( ))
		{
			cCurBoardNode = cntBoards.top( ) ;
			ClearBoardNode( cCurBoardNode) ;
			cntBoards.pop( ) ;
		}
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "cntReserveMoveOnly.size( ) = %lu\n", cntReserveMoveOnly.size( )) ;
#endif
		while( 0x0 < cntReserveMoveOnly.size( ))
		{
			cCurBoardNode = cntReserveMoveOnly.top( ) ;
			ClearBoardNode( cCurBoardNode) ;
			cntReserveMoveOnly.pop( ) ;
		}
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "cntBoardsSeen.size( ) = %lu\n", cntBoardsSeen.size( )) ;
#endif
		cntBoardsSeen.clear( ) ;

	}

	catch( const char *e)
	{
		char	strMessage [1024 + 50 + 1] ;
		strncpy( strMessage, "CSS_BakersGameEasy::FindAnySolution -> ", 50) ;
		strncat( strMessage, e, 1024) ;
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "%s\n", strMessage) ;
#endif
		bIsWinnable = false ;
	}

	catch( ... )
	{
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Unknown exception caught from CSS_BakersGameEasy::FindAnySolution\n") ;
#endif
		bIsWinnable = false ;
	}

	// And return
	return( bIsWinnable) ;

}
