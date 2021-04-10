
//
// Supporting functions for database operations of the solver.
//
//  Created by Louis Gehrig on 4/04/21.
//  Copyright © 2021 Louis Gehrig. All rights reserved.
//

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

#ifndef	INCLUDE_SOLVERSUPPORT_H
#define	INCLUDE_SOLVERSUPPORT_H

// Required includes
#include <SolitaireBoard.h>
#include <SolitaireSolver.h>

//
// Check for a database solution and, if not found, allocate and run a solver
//
// @param pBoard A pointer to the board to solve.
// @param pTimeout If not null, will be set to TRUE on a solver timeout
// @return A list of moves to solve - or an empty set if there is no solution
//

LST_MOVES findOrSolveBoard( CSolitaireBoard *pBoard, bool *pTimeout);

#endif
