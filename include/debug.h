//
//  debug.h
//  TicTacToe
//
//  Created by Louis Gehrig on 3/21/21.
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

#ifndef INCLUDE_DEBUG_H
#define INCLUDE_DEBUG_H

#ifdef INCLUDE_DEBUG_MACROS

#define	LP			fprintf( stderr, "Now at %s:%d\n", __FILE__, __LINE__)
#define	LPd(x)		fprintf( stderr, "Now at %s:%d - (%s) = %d\n", __FILE__, __LINE__, #x, (x))
#define	LPu(x)		fprintf( stderr, "Now at %s:%d - (%s) = %u\n", __FILE__, __LINE__, #x, (x))
#define	LPx(x)		fprintf( stderr, "Now at %s:%d - (%s) = 0x%016x\n", __FILE__, __LINE__, #x, (x))
#define	LPlu(x)		fprintf( stderr, "Now at %s:%d - (%s) = %lu\n", __FILE__, __LINE__, #x, (x))
#define	LPllx(x)	fprintf( stderr, "Now at %s:%d - (%s) = 0x%016llx\n", __FILE__, __LINE__, #x, (x))
#define	LPs(x)		fprintf( stderr, "Now at %s:%d - (%s) = '%s'\n", __FILE__, __LINE__, #x, (x))

#endif

#ifndef	INCLUDE_DEBUG_MACROS

#define	LP
#define	LPd(x)
#define	LPu(x)
#define	LPx(x)
#define	LPllx(x)
#define	LPs(x)

#endif

#endif
