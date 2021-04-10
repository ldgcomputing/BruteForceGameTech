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
	virtual LST_MOVES		*GetSolution( ) { return( &m_lstMovesToSolve) ; } ;

} ;

#endif
