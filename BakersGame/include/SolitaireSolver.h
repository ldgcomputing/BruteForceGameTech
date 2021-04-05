/*
 *  SolitaireSolver.h
 *  Solitaire Test
 *
 *  Created by Louis Gehrig on 11/8/10.
 *  Copyright 2010 Louis Gehrig. All rights reserved.
 *
 */

#ifndef	INCLUDE_SolitaireSolver_H
#define	INCLUDE_SolitaireSolver_H

/*

 The CSolitaireSolver is an abstract class that defines the interfaces necessary for solving all known solitaire games.
 The CSolitaireSolverFactory is a class factory that returns an instantiation of CSolitaireSolver for the appropriate board.

*/

//
// Forward declarations
//

class CSolitaireBoard ;
class CSolitaireSolverFactory ;

//
// The CSolitaireSolver class
//

class CSolitaireSolver
{

public:

	// Constants
	static const int		DEFAULT_BOARDS_TO_CHECK ;
	static const int		DEFAULT_INTELLIGENCE_LEVEL ;
	static const int		DEFAULT_MOVES_ALLOWED ;
	static const int		DEFAULT_STACK_SIZE ;
	static const int		DEFAULT_TIME_PERMITTED ;
	static const int		MINIMUM_BOARDS_TO_CHECK ;
	static const int		MINIMUM_INTELLIGENCE_LEVEL ;
	static const int		MINIMUM_MOVES_ALLOWED ;
	static const int		MINIMUM_STACK_SIZE ;
	static const int		MINIMUM_TIME_PERMITTED ;

	// Destruction
	virtual ~CSolitaireSolver( ) ;

	// Set intelligence
	bool					SetBoardsToCheck( const int nBoards) ;
	bool					SetIntelligence( const int nIntelLevel) ;
	bool					SetMaximumMoves( const int nMaxMoves) ;
	bool					SetMaximumStackSize( const int nMaxStack) ;
	bool					SetMaximumTime( const int nSeconds) ;
	int						GetBoardsToCheck( ) const ;
	int						GetIntelligence( ) const ;
	int						GetMaximumMoves( ) const ;
	int						GetMaximumStackSize( ) const ;
	int						GetMaximumTime( ) const ;

	// Find any solutions
	virtual bool			FindAnySolution( ) = 0 ;

protected:

	// Construction
	CSolitaireSolver( CSolitaireBoard *pBoard) ;

	// Member vars
	CSolitaireBoard			*m_pCurrentAbstractBoard ;
	int						m_nBoardsToCheck ;
	int						m_nIntelLevel ;
	int						m_nMaxMoves ;
	int						m_nMaxStackSize ;
	int						m_nTimePermitted ;

} ;

//
// The CSolitaireSolverFactory class
//

class CSolitaireSolverFactory
{

public:

	// Destruction
	virtual ~CSolitaireSolverFactory( ) ;

	// Obtain a solver
	static CSolitaireSolver	* GetSolver( CSolitaireBoard *pBoard) ;

protected:

	// Construction
	CSolitaireSolverFactory( ) ;

} ;

#endif
