/*
 *  SolitaireSolver_BakersGameStnd.h
 *  Solitaire Test
 *
 *  Created by Louis Gehrig on 11/8/10.
 *  Copyright 2010 Louis Gehrig. All rights reserved.
 *
 */

/*

 This class provides solutions for the Bakers Game Standard solitaire.

*/

#include <list>
#include "SolitaireSolver.h"

#ifndef	INCLUDE_SolitaireSolver_BakersGameStnd_H
#define INCLUDE_SolitaireSolver_BakersGameStnd_H

// Forward declares
class CSB_BakersGame ;
class CSolitaireBoard ;

//
// The CSS_BakersGameStnd class
//

class	CSS_BakersGameStnd : public CSolitaireSolver
{

protected:

	// Variables
	CSB_BakersGame			*m_pcCurBoard ;
	CSB_BakersGame			*m_pcInitialBoard ;
	LST_MOVES				m_lstMovesToSolve ;

public:

	// Construction / Destruction
	CSS_BakersGameStnd( CSolitaireBoard *pBoard) ;
	virtual ~CSS_BakersGameStnd( ) ;

	// Find any solutions
	virtual bool			FindAnySolution( ) ;
	LST_MOVES				*GetSolution( ) { return( &m_lstMovesToSolve) ; } ;

} ;

#endif
