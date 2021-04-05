//
// Supporting functions necessary for the NodeJS exports
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

#ifndef INCLUDE_NODESUPPORT_H
#define	INCLUDE_NODESUPPORT_H

// Forward declares
class CSolitaireBoard;

// Typedefs for the projects
typedef unsigned int SEED_T;

//
// Load the list of valid seed values into memory.
//

void loadValidSeedValues();

//
// Validate a seed value
//
// This function will check that the seed value parameter exists in the file
// of input seed values.  If the seed value exists then it will be returned,
// otherwise a random valid seed value will be returned in its place.
//
// @param seedValue The value to check.
// @return A valid seed value (may not be the original parameter)
//

SEED_T validateSeedValue(SEED_T seedValue);

//
// Convert a board to a JSON representation
//
// See usage.txt for more details.
//
// @param seedVal The seed value used to create the board.
// @param pBoard A pointer to the board.
// @param pBuffer The output buffer to write the JSON to.
// @param bufSize The size of the output buffer.
// @return The number of bytes written to the buffer or, if negative, an error code.
//

int convertToJSON(SEED_T seedVal, CSolitaireBoard *pBoard, char *pBuffer, const size_t bufSize);

#endif
