/*
 *  SolitaireSolver.cpp
 *  Solitaire Test
 *
 *  Created by Louis Gehrig on 11/8/10.
 *  Copyright 2010 Louis Gehrig. All rights reserved.
 *
 */

#include "SolitaireBoard.h"
#include "SolitaireSolver.h"
#include "SolitaireSolver_BakersGameEasy.h"
#include "SolitaireSolver_BakersGameStnd.h"

//
// The CSolitaireSolver class
//

// Construction
const int CSolitaireSolver::DEFAULT_BOARDS_TO_CHECK = 200000 ;
const int CSolitaireSolver::DEFAULT_INTELLIGENCE_LEVEL = 4 ;
const int CSolitaireSolver::DEFAULT_MOVES_ALLOWED = 150 ;
const int CSolitaireSolver::DEFAULT_STACK_SIZE = 250000 ;
const int CSolitaireSolver::DEFAULT_TIME_PERMITTED = 600 ;
const int CSolitaireSolver::MINIMUM_BOARDS_TO_CHECK = 1000 ;
const int CSolitaireSolver::MINIMUM_INTELLIGENCE_LEVEL = 1 ;
const int CSolitaireSolver::MINIMUM_MOVES_ALLOWED = 53 ;
const int CSolitaireSolver::MINIMUM_STACK_SIZE = 100 ;
const int CSolitaireSolver::MINIMUM_TIME_PERMITTED = 1 ;
CSolitaireSolver::CSolitaireSolver( CSolitaireBoard *pBoard)
{
	m_nBoardsToCheck =			DEFAULT_BOARDS_TO_CHECK ;
	m_nIntelLevel =				DEFAULT_INTELLIGENCE_LEVEL ;
	m_nMaxMoves =				DEFAULT_MOVES_ALLOWED ;
	m_nMaxStackSize =			DEFAULT_STACK_SIZE ;
	m_nTimePermitted =			DEFAULT_TIME_PERMITTED ;
	m_pCurrentAbstractBoard =	pBoard ;
}

// Destruction
CSolitaireSolver::~CSolitaireSolver( )
{

}

// Intelligence
bool CSolitaireSolver::SetBoardsToCheck( const int nBoards)
{
	if( MINIMUM_BOARDS_TO_CHECK > nBoards) return( false) ;
	m_nBoardsToCheck = nBoards ;
	return( true) ;
}
bool CSolitaireSolver::SetIntelligence( const int nIntelLevel)
{
	if( MINIMUM_INTELLIGENCE_LEVEL > nIntelLevel) return( false) ;
	m_nIntelLevel = nIntelLevel ;
	return( true) ;
}
bool CSolitaireSolver::SetMaximumMoves( const int nMaxMoves)
{
	if( MINIMUM_MOVES_ALLOWED > nMaxMoves) return( false) ;
	m_nMaxMoves = nMaxMoves ;
	return( true) ;
}
bool CSolitaireSolver::SetMaximumStackSize( const int nMaxStack)
{
	if( MINIMUM_STACK_SIZE > nMaxStack) return( false) ;
	m_nMaxStackSize = nMaxStack ;
	return( true) ;
}
bool CSolitaireSolver::SetMaximumTime( const int nSeconds)
{
	if( MINIMUM_TIME_PERMITTED > nSeconds) return( false) ;
	m_nTimePermitted = nSeconds ;
	return( true) ;
}
int CSolitaireSolver::GetBoardsToCheck( ) const
{
	return( m_nBoardsToCheck) ;
}
int CSolitaireSolver::GetIntelligence( ) const
{
	return( m_nIntelLevel) ;
}
int CSolitaireSolver::GetMaximumMoves( ) const
{
	return( m_nMaxMoves) ;
}
int CSolitaireSolver::GetMaximumStackSize( ) const
{
	return( m_nMaxStackSize) ;
}
int CSolitaireSolver::GetMaximumTime( ) const
{
	return( m_nTimePermitted) ;
}

//
// The CSolitaireSolverFactory class
//

// Construction
CSolitaireSolverFactory::CSolitaireSolverFactory( )
{

}

// Destruction
CSolitaireSolverFactory::~CSolitaireSolverFactory( )
{

}

// Obtain a solver
CSolitaireSolver * CSolitaireSolverFactory::GetSolver( CSolitaireBoard *pBoard)
{

	// Variables
	CSolitaireSolver				*pSolver = (CSolitaireSolver *) 0x0 ;

	// Validate input
	if( (CSolitaireBoard *) 0x0 == pBoard) return ((CSolitaireSolver *) 0x0) ;

	// Create a solver
	switch( pBoard -> GetSolType( ))
	{

		case ST_BAKERS_GAME :
		case ST_BAKERS_GAME_TEST :
			pSolver = new CSS_BakersGameStnd( pBoard) ;
			break ;
			
		case ST_BAKERS_GAME_EASY :
		case ST_BAKERS_GAME_EASY_TEST :
			pSolver = new CSS_BakersGameEasy( pBoard) ;
			break ;

		default :
			pSolver = (CSolitaireSolver *) 0x0 ;
			break ;

	}

	// And done
	return( pSolver) ;

}
