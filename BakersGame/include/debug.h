
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

#ifndef	INCLUDE_DEBUG_H
#define	INCLUDE_DEBUG_H

#ifdef	ACTIVE_LOGGING

#define LP			fprintf( stderr, "At %s:%d\n", __FILE__, __LINE__)
#define LPd(x)		fprintf( stderr, "At %s:%d - %s = %d\n", __FILE__, __LINE__, #x, (x))
#define	LPlld(x)	fprintf( stderr, "At %s:%d - %s = %lld\n", __FILE__, __LINE__, #x, (x))
#define LPp(x)		fprintf( stderr, "At %s:%d - %s = %p\n", __FILE__, __LINE__, #x, (x))
#define LPs(x)		fprintf( stderr, "At %s:%d - %s = '%s'\n", __FILE__, __LINE__, #x, (x))

#endif

#endif
