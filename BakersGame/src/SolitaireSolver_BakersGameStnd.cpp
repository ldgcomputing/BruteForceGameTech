/*
 *  SolitaireSolver_BakersGameStnd.cpp
 *  Solitaire Test
 *
 *  Created by Louis Gehrig on 11/8/10.
 *  Copyright 2010 Louis Gehrig. All rights reserved.
 *
 */

// Standard includes
#include <stdlib.h>
#include <stdio.h>

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
#include "SolitaireSolver_BakersGameStnd.h"

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
typedef	std::list< BOARD_NODE >					LST_SBOARDS ;
typedef	std::stack< BOARD_NODE >				CNT_SBOARDS ;
typedef	std::vector< BOARD_NODE >				VEC_SBOARDS ;
typedef	LST_SBOARDS::iterator					LITR_SBOARDS ;
typedef	VEC_SBOARDS::iterator					ITR_SBOARDS ;

// Utility functions
void ClearBoardNodeStnd( BOARD_NODE &cNode)
{
	cNode.nPriority = 0x0 ;
	cNode.cntPreviousMoves.clear( ) ;
	cNode.nMovesSinceFoundationPlacement = 0x0 ;
	if( (CSB_BakersGame *) 0x0 != cNode.pcTheBoard) delete cNode.pcTheBoard ;
	cNode.pcTheBoard = (CSB_BakersGame *) 0x0 ;
}

bool sortBoardsByPriorityStnd( const BOARD_NODE &left, const BOARD_NODE &right)
{
	int		nScoreLeft, nScoreRight ;
	if( left.nPriority < right.nPriority) return( true) ;
	if( left.nPriority > right.nPriority) return( false) ;
	nScoreLeft =	left.pcTheBoard -> GetFoundationCardValue( CS_CLUBS) ;
	nScoreLeft +=	left.pcTheBoard -> GetFoundationCardValue( CS_DIAMONDS) ;
	nScoreLeft +=	left.pcTheBoard -> GetFoundationCardValue( CS_HEARTS) ;
	nScoreLeft +=	left.pcTheBoard -> GetFoundationCardValue( CS_SPADES) ;
	nScoreRight =	right.pcTheBoard -> GetFoundationCardValue( CS_CLUBS) ;
	nScoreRight +=	right.pcTheBoard -> GetFoundationCardValue( CS_DIAMONDS) ;
	nScoreRight +=	right.pcTheBoard -> GetFoundationCardValue( CS_HEARTS) ;
	nScoreRight +=	right.pcTheBoard -> GetFoundationCardValue( CS_SPADES) ;
	return( nScoreLeft < nScoreRight) ;
}

//
// The CSS_BakersGameEasy class
//

// Construction
CSS_BakersGameStnd::CSS_BakersGameStnd( CSolitaireBoard *pBoard) :
	CSolitaireSolver( pBoard)
{

	// Initialize
	m_pcCurBoard =		(CSB_BakersGame *) pBoard ;
	m_pcInitialBoard =	(CSB_BakersGame *) CSolitaireBoard::CreateBoardCopy( pBoard) ;

}

// Destruction
CSS_BakersGameStnd::~CSS_BakersGameStnd( )
{

	// Free up allocated resources
	if( (CSB_BakersGame *) 0x0 != m_pcInitialBoard) delete m_pcInitialBoard ;
	m_pcInitialBoard = (CSB_BakersGame *) 0x0 ;

}

// Find any solution
bool CSS_BakersGameStnd::FindAnySolution( )
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
	int							nFreeSlots, nOpenCols ;
	int							nLoopCol, nLoopRow ;
	int							nNumToMove ;
	int							nSpaceToMove ;
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
		ClearBoardNodeStnd( cNextBoardNode) ;
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
					ClearBoardNodeStnd( cCurBoardNode) ;
					++ nBoardsSkipped ;
					continue ;
				}
			}

			// Report it?
			tCurTime = time( (time_t *) 0x0) ;
			if( (cntBoards.size( ) > nMaxBoardSize) || (REPORTING_INTERVAL < (tCurTime - tLastBoardReport)))
			{
				int			nBoardValue = 0 ;
				// cCurBoardNode.pcTheBoard -> DumpBoardToDebug( ) ;
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
				m_lstMovesToSolve.insert( m_lstMovesToSolve.cbegin( ), cCurBoardNode.cntPreviousMoves.cbegin( ), cCurBoardNode.cntPreviousMoves.cend( )) ; // @suppress("Invalid arguments")
				ClearBoardNodeStnd( cCurBoardNode) ;
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
				ClearBoardNodeStnd( cNextBoardNode) ;
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
				ClearBoardNodeStnd( cCurBoardNode) ;

				// Since it's an automatic, loop back
				continue ;

			}

			// Gone past the point of intelligence?
			if( GetIntelligence( ) < cCurBoardNode.nMovesSinceFoundationPlacement)
			{
				ClearBoardNodeStnd( cCurBoardNode) ;
				continue ;
			}
			if( (size_t) GetMaximumMoves( ) < cCurBoardNode.cntPreviousMoves.size( ))
			{
				ClearBoardNodeStnd( cCurBoardNode) ;
				continue ;
			}
			if( (size_t) GetBoardsToCheck( ) < nBoardsChecked)
			{
				ClearBoardNodeStnd( cCurBoardNode) ;
				break ;
			}
			fDiffTime = difftime( tCurTime, tStartSolver) ;
			if( GetMaximumTime( ) < fDiffTime)
			{
				ClearBoardNodeStnd( cCurBoardNode) ;
				break ;
			}
			if( ((size_t) GetMaximumStackSize( ) < cntBoards.size( )) || ((size_t) GetMaximumStackSize( ) < cntReserveMoveOnly.size( )))
			{

				// Sub-variables
				BOARD_NODE					cInnerBoardNode ;
				ITR_SBOARDS					itrAllBoards ;
				VEC_SBOARDS					cntAllBoards ;
				SET_BOARDS					cntBoardsDuplicate( cntBoardsSeen) ;
				size_t						nBoardsRemoved = 0x0 ;
				int							nEntriesPriority = 0x0 ;
				int							nEntriesReserve = 0x0 ;

				// Dump all the boards
				cntAllBoards.reserve( cntBoards.size( ) + cntReserveMoveOnly.size( ) + 1) ;
				cntAllBoards.push_back( cCurBoardNode) ;
				while( ! cntBoards.empty( ))
				{
					cInnerBoardNode = cntBoards.top( ) ;
					if( cInnerBoardNode.pcTheBoard -> RotateAndCompressBoard( &cCompBoard))
					{
						itrcBoards = cntBoardsDuplicate.insert( cCompBoard) ;
						if( ! itrcBoards.second) {
							ClearBoardNodeStnd( cInnerBoardNode) ;
							++ nBoardsRemoved ;
						}
						else
						{
							cntAllBoards.push_back( cntBoards.top( )) ;
							++ nEntriesPriority ;
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
							ClearBoardNodeStnd( cInnerBoardNode) ;
							++ nBoardsRemoved ;
						}
						else
						{
							cntAllBoards.push_back( cntReserveMoveOnly.top( )) ;
							++ nEntriesReserve ;
						}
					}
					cntReserveMoveOnly.pop( ) ;
				}

				// Add it up
				nBoardsSkipped += nBoardsRemoved ;

				// Still too many?
				if( (GetMaximumStackSize( ) < nEntriesReserve) || (GetMaximumStackSize( ) < nEntriesPriority))
				{
#ifdef	ACTIVE_LOGGING
					fprintf( stderr, "Stack limit has been exceeded ... even after removal of %lu data\n", nBoardsRemoved) ;
					fprintf( stderr, "cntBoards.size( ) = %lu\n", cntBoards.size( )) ;
					fprintf( stderr, "cntReserveMoveOnly.size( ) = %lu\n", cntReserveMoveOnly.size( )) ;
					fprintf( stderr, "cntBoardsSeen.size( ) = %lu\n", cntBoardsSeen.size( )) ;
#endif
					ClearBoardNodeStnd( cCurBoardNode) ;
					break ;
				}

				// Sort it
				std::sort( cntAllBoards.begin( ), cntAllBoards.end( ), sortBoardsByPriorityStnd) ;

				// Push them all back
				for( itrAllBoards = cntAllBoards.begin( ) ; cntAllBoards.end( ) != itrAllBoards ; ++ itrAllBoards)
				{
					if( 0x0 == cInnerBoardNode.nPriority)
						cntReserveMoveOnly.push( *itrAllBoards) ;
					else
						cntBoards.push( *itrAllBoards) ;
				}
				/*
				os_log_error( m_logging, "Stack limit exceeded but okay after removal of %lu data", nBoardsRemoved) ;
				os_log_error( m_logging, "cntBoards.size( ) = %lu", cntBoards.size( )) ;
				os_log_error( m_logging, "cntReserveMoveOnly.size( ) = %lu", cntReserveMoveOnly.size( )) ;
				os_log_error( m_logging, "cntBoardsSeen.size( ) = %lu", cntBoardsSeen.size( )) ;
				*/

				// And clear the sets
				cntBoardsDuplicate.clear( ) ;
				cntAllBoards.clear( ) ;

				// Call continue, since will have duplicates on the current board
				continue ;

			}

			// Board has never been seen before and is good to go ...
			++ nBoardsChecked ;

			// Second - look for any open reserve slots
			for( nReserve = cCurBoardNode.pcTheBoard -> NumReserveSpaces( ) - 1 ; 0 <= nReserve ; -- nReserve)
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
							ClearBoardNodeStnd( cNextBoardNode) ;
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
			for( nReserve = cCurBoardNode.pcTheBoard -> NumReserveSpaces( ) - 1 ; 0 <= nReserve ; -- nReserve)
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
								ClearBoardNodeStnd( cNextBoardNode) ;
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

			// Before moving stacks, count open slots
			for( nFreeSlots = 0, nLoopCol = 0 ; NUM_RESV > nLoopCol ; ++ nLoopCol)
			{
				cCurCard = cCurBoardNode.pcTheBoard -> GetReserveCard( nLoopCol) ;
				if( CS_INVALID == cCurCard.eSuit) ++ nFreeSlots ;
			}
			for( nOpenCols = 0, nLoopCol = 0 ; NUM_COLS > nLoopCol ; ++ nLoopCol)
			{
				cCurCard = cCurBoardNode.pcTheBoard -> GetTableauCardAt( nLoopCol, 0) ;
				if( CS_INVALID == cCurCard.eSuit) ++ nOpenCols ;
			}

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
						nNumToMove = 1 ;
						while( 0 < nLoopRow)
						{
							cCurCard = cCurBoardNode.pcTheBoard -> GetTableauCardAt( nLoopCol, nLoopRow) ;
							cAboveCard = cCurBoardNode.pcTheBoard -> GetTableauCardAt( nLoopCol, nLoopRow - 1) ;
							if( cAboveCard.eSuit != cCurCard.eSuit) break ;
							nCardVal1 = cCurCard.eValue ;
							nCardVal2 = cAboveCard.eValue ;
							if( (nCardVal1 + 1) != nCardVal2) break ;
							-- nLoopRow ;
							++ nNumToMove ;
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
									
									// Need enough space to make the move
									nSpaceToMove = -1 ;
									if( bMovedStackToOpen)
									{
										if( 0x0 == nFreeSlots)
											nSpaceToMove = nOpenCols ;
										else if( 1 >= nOpenCols)
											nSpaceToMove = nFreeSlots + 1 ;
										else
											nSpaceToMove = (nFreeSlots + 1) * (nOpenCols - 1) ;
									}
									else
									{
										if( 0x0 == nFreeSlots)
											nSpaceToMove = 2 * nOpenCols ;
										else
											nSpaceToMove = nFreeSlots * (nOpenCols + 1) + 1 ;
									}

									// If moving to an open slot, try trimming to make space
									if( bMovedStackToOpen && (nNumToMove > nSpaceToMove))
									{
										while( (1 < nNumToMove) && (nNumToMove > nSpaceToMove))
										{
											++ nLoopRow ;
											-- nNumToMove ;
										}
									}

									// Enough space to move?
									if( (1 == nNumToMove) || (nSpaceToMove >= nNumToMove))
									{

										// Throw this card in
										cNextMove.eMoveFrom =	MP_TABLEAU ;
										cNextMove.nFromCol =	nLoopCol ;
										cNextMove.nFromRow =	nLoopRow ;
										cNextMove.eMoveTo =		MP_TABLEAU ;
										cNextMove.nToCol =		nCol ;
										cNextMove.nToRow =		nRow ;

										// Setup the next node - note, cNextBoardNode will always havea pointer to an existing board that should not be deleted!
										cNextBoardNode.pcTheBoard = (CSB_BakersGame *) 0x0 ;
										ClearBoardNodeStnd( cNextBoardNode) ;
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
			std::sort( cntNextBoards.begin( ), cntNextBoards.end( ), sortBoardsByPriorityStnd) ;
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
			ClearBoardNodeStnd( cCurBoardNode) ;

		} // endwhile something on the stack

		// Clear the stack of anything remaining (in event of a win)
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "cntBoards.size( ) = %lu\n", cntBoards.size( )) ;
#endif
		while( 0x0 < cntBoards.size( ))
		{
			cCurBoardNode = cntBoards.top( ) ;
			ClearBoardNodeStnd( cCurBoardNode) ;
			cntBoards.pop( ) ;
		}
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "cntReserveMoveOnly.size( ) = %lu\n", cntReserveMoveOnly.size( )) ;
#endif
		while( 0x0 < cntReserveMoveOnly.size( ))
		{
			cCurBoardNode = cntReserveMoveOnly.top( ) ;
			ClearBoardNodeStnd( cCurBoardNode) ;
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
