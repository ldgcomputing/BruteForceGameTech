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

	//
	// Set solver intelligence limits
	//
	// SetBoardsToCheck		The number of unique boards to evaluate before
	//						concluding there is no solution.
	// SetIntelligence		The number of moves made without placing a
	//						foundation piece before concluding that
	//						there is no solution.
	// SetMaximumMoves		The maximum size considered for a solution.
	// SetMaximumStackSize	After the stack of boards to process has
	//						exceeded this limit a trim operation
	//						will dump boards from consideration.
	// SetMaximumTime		The maximum number of seconds before
	//						abandoning the attempt to find a solution.
	//

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

	// If a solution was found, return it
	virtual LST_MOVES		*GetSolution( ) = 0;


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
