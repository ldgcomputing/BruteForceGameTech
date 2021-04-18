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

#ifndef	INCLUDE_DBSUPPORT_H
#define	INCLUDE_DBSUPPORT_H

// SQLITE include
#include <sqlite3.h>

// Necessary typedefs
typedef sqlite3_int64 BOARDID_T;
typedef sqlite3_int64 SOLUTIONID_T;

class SolitaireDB {

public:

	// The default DB location
	static const char * DEFAULT_DB_PATH;

	// Construction
	SolitaireDB();
	SolitaireDB( const char *useDBPath);

	// Destruction
	~SolitaireDB();

	//
	// Initialize the database
	//
	// @return TRUE if the database is initialized, FALSE otherwise
	//

	bool initDB();

	//
	// Has the database been initialized?
	//
	// @return TRUE if the database has been initialized, FALSE otherwise
	//

	bool isInit() const { return( isSuccessInit); }

	//
	// Close the database
	//

	void close();

	//
	// Get the last DB error
	//
	// @return The text explanation of the last DB error.
	//

	const char *getLastDBError() const;

	//
	// Get the primary board id
	//
	// The primary board id wil be returned to the caller, but
	// by using the isAlt parameter the caller may determine if
	// the board was found as a primary or as an alternate.
	//
	// @param pBoard The board to lookup.
	// @param isAlt If provided, will be set to TRUE if the board is alternate, or set FALSE otherwise
	// @return The primary board id, if found, zero if not, and negative on error
	//

	BOARDID_T getPrimaryBoardId( CSolitaireBoard *pBoard, bool *isAlt);

	//
	// Create a new primary board
	//
	// @param pBoard The board to allocate
	// @return The new board id, or negative on error
	//

	BOARDID_T createNewBoard( CSolitaireBoard *pBoard);

	//
	// Insert an alternate board
	//
	// @param boardId The primary board id
	// @param altBoard The alternate board information to insert
	// @return TRUE on success, FALSE otherwise
	//

	bool insertAlternateBoard( BOARDID_T boardId, const S_ALTERNATE_BOARD &altBoard);

	//
	// Pull an alternate board details
	//
	// @param pBoard The alternate board to lookup
	// @param altBoard The alternate board structure to fill
	// @return TRUE on success, FALSE otherwise
	//

	bool getAlternateBoard( CSolitaireBoard *pBoard, S_ALTERNATE_BOARD &altBoard);

	//
	// Create solution header
	//
	// @param originSolution The origin solution id (or zero for top solution)
	// @param numMoves The number of moves in this solution
	// @param timeTaken The number of milliseconds required for the solution
	// @return The solution id, or negative on failure
	//

	SOLUTIONID_T createSolutionHeader( SOLUTIONID_T originSolution, size_t numMoves, unsigned long timeTaken);

	//
	// Insert a solution detail line
	//
	// @param solutionId The solution id
	// @param moveNum The move number (order)
	// @param move The move detail
	// @return TRUE on success, FALSE otherwise
	//

	bool insertSolutionDetail( SOLUTIONID_T solutionId, int moveNum, const MOVE_T &move);

	//
	// Attach a board and solution together
	//
	// @param boardId The board id
	// @param solutionId The solution id
	// @return TRUE on success, FALSE otherwise
	//

	bool attachBoardAndSolution( BOARDID_T boardId, SOLUTIONID_T solutionId);

	//
	// Get the solution for a board
	//
	// @param boardId The board id
	// @return The solutionId, zero for not found, or negative for error
	//

	SOLUTIONID_T getSolutionForBoard( BOARDID_T boardId);

	//
	// Get a collection of moves for a solution
	//
	// @param solutionId The solution id
	// @return A collection of moves
	//

	LST_MOVES getSolutionMoves( SOLUTIONID_T solutionId);

protected:

	// Setup the prepared statements - but do not prepare
	void setupPreparedStatements();

	// The path to the database
	const char *dbPath;

	// The handle to the SQLITE database
	sqlite3 *ppDB;

	// An enumeration of the prepared statements
	enum e_prepared_statements {
		STMT_LOOKUP_ALTERNATE_BOARD,
		STMT_LOOKUP_PRIMARY_BOARD,
		STMT_GET_HIGHEST_BOARD_ID,
		STMT_INSERT_NEW_BOARD,
		STMT_INSERT_ALT_BOARD,
		STMT_GET_HIGHEST_SOLUTION_ID,
		STMT_INSERT_SOLUTION_HEADER,
		STMT_INSERT_SOLUTION_DETAIL,
		STMT_INSERT_BOARD_SOLUTION,
		STMT_GET_ALTERNATE_BOARD_BY_COMP_BOARD,
		STMT_GET_SOLUTION_ID,
		STMT_EXTRACT_SOLUTION_DETAIL,
		STMT_INVALID_PREPARED_STATEMENT
	};
	typedef enum e_prepared_statements E_PREPARED_STATEMENTS;

	// The prepared statements
	struct s_prepared_statements {
		E_PREPARED_STATEMENTS	eStatement;
		const char				*strStatement;
		sqlite3_stmt *			stmtPrepared;
	};
	typedef	struct s_prepared_statements	S_PREPARED_STATEMENT;
	S_PREPARED_STATEMENT preparedStatements [STMT_INVALID_PREPARED_STATEMENT];

	// The last sql error code
	int lastErrorCode;

	// Set when the database is initialized
	bool isSuccessInit;

	//
	// Convert the compressed board into a text representation
	//
	// @param inpBytes The bytes to convert.
	// @param inpSize The number of bytes to convert.
	// @param outBuffer The output buffer.
	// @param outSize The output buffer size.
	// @return The number of bytes written, or negative on error.
	//

	int convertToText(const unsigned char *inpBytes, const size_t inpSize, char *outBuffer, const size_t outSize);

};

//
// Save moves to the database
//
// The board, and all of its alternates, and all of the moves for the board,
// will be stored to the database.
//
// @param pBoard The originating board for the solution
// @param lstMoves The moves used to solve the board
// @param timeTaken The number of milliseconds required to solve the board
// @return TRUE if everything was saved, FALSE otherwise

bool saveMovesToDB( const CSolitaireBoard *pBoard, const LST_MOVES &lstMoves, unsigned long timeTaken);

#endif
