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

// Standard includes
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

// Project includes
#include <SolitaireBoard.h>
#include <solverSupport.h>
#include <sqlite3.h>
#include <dbSupport.h>
#include <debug.h>

// Construction
const char *SolitaireDB::DEFAULT_DB_PATH = "resources/bakersgame.db";
SolitaireDB::SolitaireDB() :
		dbPath(SolitaireDB::DEFAULT_DB_PATH), ppDB((sqlite3*) 0x0), lastErrorCode(0x0), isSuccessInit(false) {
	setupPreparedStatements();
}
SolitaireDB::SolitaireDB(const char *useDBPath) :
		dbPath(useDBPath), ppDB((sqlite3*) 0x0), lastErrorCode(0x0), isSuccessInit(false) {
	setupPreparedStatements();
}

// Destruction
SolitaireDB::~SolitaireDB() {

	// If the database is still open, close it!
	if (isInit()) {
		close();
	}

}

// Setup the prepared statements
void SolitaireDB::setupPreparedStatements() {

	preparedStatements[STMT_LOOKUP_PRIMARY_BOARD].eStatement = STMT_LOOKUP_PRIMARY_BOARD;
	preparedStatements[STMT_LOOKUP_PRIMARY_BOARD].strStatement =
			"select boardNumber from primary_boards where compressedBoard = ? and solType = ?";

	preparedStatements[STMT_LOOKUP_ALTERNATE_BOARD].eStatement = STMT_LOOKUP_ALTERNATE_BOARD;
	preparedStatements[STMT_LOOKUP_ALTERNATE_BOARD].strStatement =
			"select primaryBoardNumber from alternate_boards where compressedBoard = ? and solType = ?";

	preparedStatements[STMT_GET_HIGHEST_BOARD_ID].eStatement = STMT_GET_HIGHEST_BOARD_ID;
	preparedStatements[STMT_GET_HIGHEST_BOARD_ID].strStatement = "select max(boardNumber) from primary_boards";

	preparedStatements[STMT_INSERT_NEW_BOARD].eStatement = STMT_INSERT_NEW_BOARD;
	preparedStatements[STMT_INSERT_NEW_BOARD].strStatement =
			"insert into primary_boards ( boardNumber , solType , compressedBoard ) values ( ? , ? , ? )";

	preparedStatements[STMT_INSERT_ALT_BOARD].eStatement = STMT_INSERT_ALT_BOARD;
	preparedStatements[STMT_INSERT_ALT_BOARD].strStatement = "insert into alternate_boards ( "
			"primaryBoardNumber , solType , compressedBoard , "
			"clubsMapTo, diamondsMapTo, heartsMapTo, spadesMapTo ) "
			"values ( ? , ? , ? , ? , ? , ? , ? )";

	preparedStatements[STMT_GET_HIGHEST_SOLUTION_ID].eStatement = STMT_GET_HIGHEST_SOLUTION_ID;
	preparedStatements[STMT_GET_HIGHEST_SOLUTION_ID].strStatement = "select max(solutionNumber) from solutions_header";

	preparedStatements[STMT_INSERT_SOLUTION_HEADER].eStatement = STMT_INSERT_SOLUTION_HEADER;
	preparedStatements[STMT_INSERT_SOLUTION_HEADER].strStatement = "insert into solutions_header ( "
			"solutionNumber , originSolutionNumber , numberMoves , "
			"dateTimeCreatedUTC, solutionTimeMilliSeconds ) "
			"values ( ? , ? , ? , ? , ? )";

	preparedStatements[STMT_INSERT_SOLUTION_DETAIL].eStatement = STMT_INSERT_SOLUTION_DETAIL;
	preparedStatements[STMT_INSERT_SOLUTION_DETAIL].strStatement = "insert into solutions_detail ( "
			"solutionNumber , solutionOrder , moveFrom , fromCol , fromRow ,"
			"moveTo, toCol , toRow ) "
			"values ( ? , ? , ? , ? , ? , ? , ? , ? )";

	preparedStatements[STMT_INSERT_BOARD_SOLUTION].eStatement = STMT_INSERT_BOARD_SOLUTION;
	preparedStatements[STMT_INSERT_BOARD_SOLUTION].strStatement =
			"insert into boards_to_solutions ( primaryBoardNumber , solutionNumber ) values ( ? , ? )";

	preparedStatements[STMT_GET_ALTERNATE_BOARD_BY_COMP_BOARD].eStatement = STMT_GET_ALTERNATE_BOARD_BY_COMP_BOARD;
	preparedStatements[STMT_GET_ALTERNATE_BOARD_BY_COMP_BOARD].strStatement =
			"select primaryBoardNumber, solType, clubsMapTo, diamondsMapTo, heartsMapTo, spadesMapTo from alternate_boards where compressedBoard = ? and solType = ?";

	preparedStatements[STMT_GET_SOLUTION_ID].eStatement = STMT_GET_SOLUTION_ID;
	preparedStatements[STMT_GET_SOLUTION_ID].strStatement =
			"select solutionNumber from boards_to_solutions where primaryBoardNumber = ?";

	preparedStatements[STMT_EXTRACT_SOLUTION_DETAIL].eStatement = STMT_EXTRACT_SOLUTION_DETAIL;
	preparedStatements[STMT_EXTRACT_SOLUTION_DETAIL].strStatement =
			"select solutionOrder, moveFrom, fromCol, fromRow, moveTo, toCol, toRow from solutions_detail where solutionId = ? order by solutionOrder";

}

// Get the last database error
const char* SolitaireDB::getLastDBError() const {

	static char retValue[2048 + 1];
	memset(retValue, 0x0, sizeof(retValue));
	sprintf(retValue, "errcde: %d (%s)", lastErrorCode, sqlite3_errstr(lastErrorCode));
	return (retValue);

}

// Init the database
bool SolitaireDB::initDB() {

	// If already initialized, return a true
	if (isInit()) {
		return (true);
	}

	// Wrap it all
	bool bRetValue = false;
	try {

		// Open the database
		int errCode = sqlite3_open(dbPath, &ppDB); // @suppress("Invalid arguments")
		if ( SQLITE_OK != errCode) {
			throw(errCode);
		}

		// Loop over all the statements to be prepared
		for (int nStmt = 0; STMT_INVALID_PREPARED_STATEMENT > nStmt; ++nStmt) {
			const char *query = preparedStatements[nStmt].strStatement;
			sqlite3_stmt **pStmt = &(preparedStatements[nStmt].stmtPrepared);
			errCode = sqlite3_prepare_v2(ppDB, query, -1, pStmt, (const char**) 0x0); // @suppress("Invalid arguments")
			if ( SQLITE_OK != errCode) {
#ifdef ACTIVE_LOGGING
				fprintf( stderr, "Failure while preparing statement %d - %s\n", nStmt, preparedStatements[nStmt].strStatement);
#endif
				throw(errCode);
			}
		}

		// All good
		lastErrorCode = 0x0;
		isSuccessInit = true;
		bRetValue = true;

	}

	catch (int e) {
#ifdef	ACTIVE_LOGGING
			fprintf( stderr, "Error during SolitaireDB::initDB -> sqlite3 reports %s\n", sqlite3_errstr( e));
#endif
		lastErrorCode = e;
		bRetValue = false;
	}

	catch (const char *e) {
#ifdef	ACTIVE_LOGGING
			fprintf( stderr, "Error during SolitaireDB::initDB -> %s\n", e);
#endif
		lastErrorCode = 0;
		bRetValue = false;
	}

	// And done
	return (bRetValue);

}

// Close
void SolitaireDB::close() {

	// Close any opened statements
	for (int nStmt = 0; STMT_INVALID_PREPARED_STATEMENT > nStmt; ++nStmt) {
		sqlite3_stmt *pStmt = preparedStatements[nStmt].stmtPrepared;
		sqlite3_finalize(pStmt); // @suppress("Invalid arguments")
		preparedStatements[nStmt].stmtPrepared = (sqlite3_stmt*) 0x0;
	}

	// Close the database
	sqlite3_close(ppDB); // @suppress("Invalid arguments")
	ppDB = (sqlite3*) 0x0;

	// And no longer init
	isSuccessInit = false;

}

// Get the solution details
LST_MOVES SolitaireDB::getSolutionMoves(SOLUTIONID_T solutionId) {

	// Allocate the return
	LST_MOVES lstMoves;
	int errCode;
	sqlite3_stmt *pStmt = preparedStatements[STMT_EXTRACT_SOLUTION_DETAIL].stmtPrepared;

	// If not init, return an error
	if (!isInit())
		return (lstMoves);

	// Wrap it
	try {

		// Execute the lookup
		sqlite3_reset(pStmt); // @suppress("Invalid arguments")
		sqlite3_clear_bindings(pStmt); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int64(pStmt, 1, solutionId); // @suppress("Invalid arguments")
		errCode = sqlite3_step(pStmt); // @suppress("Invalid arguments")

		// While there is data
		while ( SQLITE_ROW == errCode) {

			// Extract data
			MOVE_T nextMove;
			nextMove.eMoveFrom = (MOVEPOS_T) sqlite3_column_int(pStmt, 1); // @suppress("Invalid arguments")
			nextMove.nFromCol = sqlite3_column_int(pStmt, 2); // @suppress("Invalid arguments")
			nextMove.nFromRow = sqlite3_column_int(pStmt, 3); // @suppress("Invalid arguments")
			nextMove.eMoveTo = (MOVEPOS_T) sqlite3_column_int(pStmt, 4); // @suppress("Invalid arguments")
			nextMove.nToCol = sqlite3_column_int(pStmt, 5); // @suppress("Invalid arguments")
			nextMove.nToRow = sqlite3_column_int(pStmt, 6); // @suppress("Invalid arguments")
			lstMoves.push_back(nextMove);

		}

		// And solve
		if ( SQLITE_DONE != errCode) {
			throw(errCode);
		}

	}

	catch (int e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::getSolutionMoves -> sqlite3 reports %s\n", sqlite3_errstr( e));
#endif
		lastErrorCode = e;
	}

	catch (const char *e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::getSolutionMoves -> %s\n", e);
#endif
		lastErrorCode = 0;
	}

	// And done
	return (lstMoves);

}

// Insert a detail solution line
bool SolitaireDB::insertSolutionDetail(SOLUTIONID_T solutionId, int moveNum, const MOVE_T &move) {

	// If not init, return an error
	if (!isInit())
		return (-1);

	// Variables
	bool retValue = false;
	int errCode;
	sqlite3_stmt *pStmt = (sqlite3_stmt*) 0x0;

	// Wrap it
	try {

		// And insert
		pStmt = preparedStatements[STMT_INSERT_SOLUTION_DETAIL].stmtPrepared;
		sqlite3_reset(pStmt); // @suppress("Invalid arguments")
		sqlite3_clear_bindings(pStmt); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int64(pStmt, 1, solutionId); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int(pStmt, 2, moveNum); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int(pStmt, 3, move.eMoveFrom); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int(pStmt, 4, move.nFromCol); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int(pStmt, 5, move.nFromRow); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int(pStmt, 6, move.eMoveTo); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int(pStmt, 7, move.nToCol); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int(pStmt, 8, move.nToRow); // @suppress("Invalid arguments")
		errCode = sqlite3_step(pStmt); // @suppress("Invalid arguments")
		if ( SQLITE_DONE != errCode)
			throw(errCode);

		// All is well
		lastErrorCode = 0x0;
		retValue = true;

	}

	catch (int e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::createNewBoard -> sqlite3 reports %s\n", sqlite3_errstr( e));
#endif
		lastErrorCode = e;
		retValue = false;
	}

	catch (const char *e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::createNewBoard -> %s\n", e);
#endif
		lastErrorCode = 0;
		retValue = false;
	}

	// And done
	return (retValue);

}

// Insert an alternate board
bool SolitaireDB::insertAlternateBoard(BOARDID_T boardId, const S_ALTERNATE_BOARD &altBoard) {

	// If not init, return an error
	if (!isInit())
		return (-1);

	// Constants
	const size_t CONVERSION_BUFFER_SIZE = 200;

	// Variables
	bool retValue = false;
	char converted[CONVERSION_BUFFER_SIZE];
	int errCode;
	sqlite3_stmt *pStmt = (sqlite3_stmt*) 0x0;

	// Wrap it
	try {

		// Compress the board
		COMP_BOARD compBoard;
		if (!altBoard.pBoard->CompressBoard(&compBoard)) {
			throw("Failure to compress original board");
		}

		// Convert the board to readable text
		memset(converted, 0x0, CONVERSION_BUFFER_SIZE);
		int convertSize = convertToText(compBoard.uacData, sizeof(compBoard.uacData), converted,
				CONVERSION_BUFFER_SIZE);

		// And insert
		pStmt = preparedStatements[STMT_INSERT_ALT_BOARD].stmtPrepared;
		sqlite3_reset(pStmt); // @suppress("Invalid arguments")
		sqlite3_clear_bindings(pStmt); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int64(pStmt, 1, boardId); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int(pStmt, 2, altBoard.pBoard->GetSolType()); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_text(pStmt, 3, converted, convertSize, SQLITE_STATIC); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int(pStmt, 4, altBoard.mapOfTo[CS_CLUBS]); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int(pStmt, 5, altBoard.mapOfTo[CS_DIAMONDS]); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int(pStmt, 6, altBoard.mapOfTo[CS_HEARTS]); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int(pStmt, 7, altBoard.mapOfTo[CS_SPADES]); // @suppress("Invalid arguments")
		errCode = sqlite3_step(pStmt); // @suppress("Invalid arguments")
		if ( SQLITE_DONE != errCode)
			throw(errCode);

		// All is well
		lastErrorCode = 0x0;
		retValue = true;

	}

	catch (int e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::createNewBoard -> sqlite3 reports %s\n", sqlite3_errstr( e));
#endif
		lastErrorCode = e;
		retValue = false;
	}

	catch (const char *e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::createNewBoard -> %s\n", e);
#endif
		lastErrorCode = 0;
		retValue = false;
	}

	// And done
	return (retValue);

}

// Create a solution header
SOLUTIONID_T SolitaireDB::createSolutionHeader(SOLUTIONID_T originSolution, size_t numMoves, unsigned long timeTaken) {

	// If not init, return an error
	if (!isInit())
		return (-1);

	// Variables
	int colType;
	int errCode;
	SOLUTIONID_T solutionId = -2;
	sqlite3_stmt *pStmt = (sqlite3_stmt*) 0x0;

	// Compute UTC time
	struct tm tmUTC;
	char strUTCTime[100];
	time_t tNow = time((time_t*) 0x0);
	gmtime_r(&tNow, &tmUTC);
	sprintf(strUTCTime, "%04d-%02d-%02d %02d:%02d:%02d", tmUTC.tm_year + 1900, tmUTC.tm_mon + 1, tmUTC.tm_mday,
			tmUTC.tm_hour, tmUTC.tm_min, tmUTC.tm_sec);

	// Wrap it
	try {

		// Get the highest solution id
		pStmt = preparedStatements[STMT_GET_HIGHEST_SOLUTION_ID].stmtPrepared;
		sqlite3_reset(pStmt); // @suppress("Invalid arguments")
		errCode = sqlite3_step(pStmt); // @suppress("Invalid arguments")
		switch (errCode) {

		case SQLITE_DONE:
			// Wasn't found
			solutionId = 0x0;
			break;

		case SQLITE_ROW:
			// Found a value?
			colType = sqlite3_column_type(pStmt, 0); // @suppress("Invalid arguments")
			if ( SQLITE_NULL == colType)
				solutionId = 0x0;
			else
				solutionId = sqlite3_column_int64(pStmt, 0); // @suppress("Invalid arguments")
			break;

		default:
			throw(errCode);

		}

		// Add one
		++solutionId;

		// And insert
		pStmt = preparedStatements[STMT_INSERT_SOLUTION_HEADER].stmtPrepared;
		sqlite3_reset(pStmt); // @suppress("Invalid arguments")
		sqlite3_clear_bindings(pStmt); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int64(pStmt, 1, solutionId); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int64(pStmt, 2, originSolution); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int64(pStmt, 3, numMoves); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_text(pStmt, 4, strUTCTime, -1, SQLITE_STATIC); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int64(pStmt, 5, timeTaken); // @suppress("Invalid arguments")
		errCode = sqlite3_step(pStmt); // @suppress("Invalid arguments")
		if ( SQLITE_DONE != errCode)
			throw(errCode);
		lastErrorCode = 0x0;

	}

	catch (int e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::createSolutionHeader -> sqlite3 reports %s\n", sqlite3_errstr( e));
#endif
		lastErrorCode = e;
		return (-1 * e);
	}

	catch (const char *e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::createSolutionHeader -> %s\n", e);
#endif
		lastErrorCode = 0;
		return (-3);
	}

	// And done
	return (solutionId);

}

// Create a new board
BOARDID_T SolitaireDB::createNewBoard(CSolitaireBoard *pBoard) {

	// If not init, return an error
	if (!isInit())
		return (-1);

	// Constants
	const size_t CONVERSION_BUFFER_SIZE = 200;

	// Variables
	char converted[CONVERSION_BUFFER_SIZE];
	int errCode;
	BOARDID_T boardId = -2;
	sqlite3_stmt *pStmt = (sqlite3_stmt*) 0x0;

	// Wrap it
	try {

		// Compress the board
		COMP_BOARD compBoard;
		if (!pBoard->CompressBoard(&compBoard)) {
			throw("Failure to compress original board");
		}

		// Convert the board to readable text
		memset(converted, 0x0, CONVERSION_BUFFER_SIZE);
		int convertSize = convertToText(compBoard.uacData, sizeof(compBoard.uacData), converted,
				CONVERSION_BUFFER_SIZE);

		// Get the highest board id
		pStmt = preparedStatements[STMT_GET_HIGHEST_BOARD_ID].stmtPrepared;
		sqlite3_reset(pStmt); // @suppress("Invalid arguments")
		errCode = sqlite3_step(pStmt); // @suppress("Invalid arguments")
		switch (errCode) {

		case SQLITE_DONE:
			// Wasn't found
			boardId = 0x0;
			break;

		case SQLITE_ROW:
			// Found a value?
			if ( SQLITE_NULL == sqlite3_column_type(pStmt, 0)) // @suppress("Invalid arguments")
				boardId = 0x0;
			else
				boardId = sqlite3_column_int64(pStmt, 0); // @suppress("Invalid arguments")
			break;

		default:
			throw(errCode);

		}

		// Add one
		++boardId;

		// And insert
		pStmt = preparedStatements[STMT_INSERT_NEW_BOARD].stmtPrepared;
		sqlite3_reset(pStmt); // @suppress("Invalid arguments")
		sqlite3_clear_bindings(pStmt); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int64(pStmt, 1, boardId); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int(pStmt, 2, pBoard->GetSolType()); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_text(pStmt, 3, converted, convertSize, SQLITE_STATIC); // @suppress("Invalid arguments")
		errCode = sqlite3_step(pStmt); // @suppress("Invalid arguments")
		if ( SQLITE_DONE != errCode)
			throw(errCode);
		lastErrorCode = 0x0;

	}

	catch (int e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::createNewBoard -> sqlite3 reports %s\n", sqlite3_errstr( e));
#endif
		lastErrorCode = e;
		boardId = -1 * e;
	}

	catch (const char *e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::createNewBoard -> %s\n", e);
#endif
		lastErrorCode = 0;
		boardId = -3;
	}

	// And done
	return (boardId);

}

// Attach a board and solution
bool SolitaireDB::attachBoardAndSolution(BOARDID_T boardId, SOLUTIONID_T solutionId) {

	// If not init, return an error
	if (!isInit())
		return (-1);

	// Variables
	bool retValue = false;
	int errCode;
	sqlite3_stmt *pStmt = preparedStatements[STMT_INSERT_BOARD_SOLUTION].stmtPrepared;

	// Wrap it
	try {

		// And insert
		sqlite3_reset(pStmt); // @suppress("Invalid arguments")
		sqlite3_clear_bindings(pStmt); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int64(pStmt, 1, boardId); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int64(pStmt, 2, solutionId); // @suppress("Invalid arguments")
		errCode = sqlite3_step(pStmt); // @suppress("Invalid arguments")
		if ( SQLITE_DONE != errCode)
			throw(errCode);

		// All is well
		lastErrorCode = 0x0;
		retValue = true;

	}

	catch (int e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::attachBoardAndSolution -> sqlite3 reports %s\n", sqlite3_errstr( e));
#endif
		lastErrorCode = e;
		retValue = false;
	}

	catch (const char *e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::attachBoardAndSolution -> %s\n", e);
#endif
		lastErrorCode = 0;
		retValue = false;
	}

	// And done
	return (retValue);

}

// Convert text
int SolitaireDB::convertToText(const unsigned char *inpBytes, const size_t inpSize, char *outBuffer,
		const size_t outSize) {

	// Constants
	const char CONVERT_VALUE[] = "0123456789ABCDEF";

	// Size must be double input size plus one
	size_t reqSize = (2 * inpSize) + 1;
	if (outSize < reqSize)
		return (-1);

	// Convert
	for (size_t nPos = 0; inpSize > nPos; ++nPos) {
		outBuffer[(2 * nPos)] = CONVERT_VALUE[((inpBytes[nPos] % 0xf0) >> 4)];
		outBuffer[(2 * nPos) + 1] = CONVERT_VALUE[(inpBytes[nPos] % 0x0f)];
	}
	outBuffer[reqSize] = 0x0;

	// And done
	return (reqSize);

}

// Get the solution for a board
SOLUTIONID_T SolitaireDB::getSolutionForBoard(BOARDID_T boardId) {

	// If not init, return an error
	if (!isInit())
		return (false);

	// Variables
	int errCode;
	SOLUTIONID_T retValue = -1;
	sqlite3_stmt *pStmt = preparedStatements[STMT_GET_SOLUTION_ID].stmtPrepared;

	// Wrap it
	try {

		// Search through the primary
		pStmt = preparedStatements[STMT_LOOKUP_ALTERNATE_BOARD].stmtPrepared;
		sqlite3_reset(pStmt); // @suppress("Invalid arguments")
		sqlite3_clear_bindings(pStmt); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int64(pStmt, 1, boardId); // @suppress("Invalid arguments")
		errCode = sqlite3_step(pStmt); // @suppress("Invalid arguments")
		switch (errCode) {

		case SQLITE_DONE:
			// Wasn't found
			retValue = 0x0;
			break;

		case SQLITE_ROW:
			// Found a value?
			if ( SQLITE_NULL == sqlite3_column_type(pStmt, 0)) // @suppress("Invalid arguments")
				retValue = 0x0;
			else {
				retValue = sqlite3_column_int64(pStmt, 0); // @suppress("Invalid arguments")
			}
			break;

		default:
			throw(errCode);

		}

		// And all is well, SQL wise
		lastErrorCode = 0x0;

	}

	catch (int e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::getSolutionForBoard -> sqlite3 reports %s\n", sqlite3_errstr( e));
#endif
		lastErrorCode = e;
	}

	catch (const char *e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::getSolutionForBoard -> %s\n", e);
#endif
		lastErrorCode = 0;
	}

	// And done
	return (retValue);

}

// Pull an alternate board
bool SolitaireDB::getAlternateBoard(CSolitaireBoard *pBoard, S_ALTERNATE_BOARD &altBoard) {

	// If not init, return an error
	if ((!isInit()) || ((CSolitaireBoard*) 0x0 == pBoard))
		return (false);

	// Constants
	const size_t CONVERSION_BUFFER_SIZE = 200;

	// Variables
	bool retValue = false;
	char converted[CONVERSION_BUFFER_SIZE];
	int errCode;
	sqlite3_stmt *pStmt = (sqlite3_stmt*) 0x0;

	// Wrap it
	try {

		// Compress the board
		COMP_BOARD compBoard;
		if (!pBoard->CompressBoard(&compBoard)) {
			throw("Failure to compress original board");
		}

		// Convert the board to readable text
		memset(converted, 0x0, CONVERSION_BUFFER_SIZE);
		int convertSize = convertToText(compBoard.uacData, sizeof(compBoard.uacData), converted,
				CONVERSION_BUFFER_SIZE);

		// Search through the primary
		pStmt = preparedStatements[STMT_LOOKUP_ALTERNATE_BOARD].stmtPrepared;
		sqlite3_reset(pStmt); // @suppress("Invalid arguments")
		sqlite3_clear_bindings(pStmt); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_text(pStmt, 1, converted, convertSize, SQLITE_STATIC); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int(pStmt, 2, pBoard->GetSolType()); // @suppress("Invalid arguments")
		errCode = sqlite3_step(pStmt); // @suppress("Invalid arguments")
		switch (errCode) {

		case SQLITE_DONE:
			// Wasn't found
			retValue = false;
			break;

		case SQLITE_ROW:
			// Found a value?
			if ( SQLITE_NULL == sqlite3_column_type(pStmt, 0)) // @suppress("Invalid arguments")
				retValue = false;
			else {
				altBoard.mapOfTo[CS_CLUBS] = (CARDSUITS_T) sqlite3_column_int(pStmt, 2); // @suppress("Invalid arguments")
				altBoard.mapOfTo[CS_DIAMONDS] = (CARDSUITS_T) sqlite3_column_int(pStmt, 3); // @suppress("Invalid arguments")
				altBoard.mapOfTo[CS_HEARTS] = (CARDSUITS_T) sqlite3_column_int(pStmt, 4); // @suppress("Invalid arguments")
				altBoard.mapOfTo[CS_SPADES] = (CARDSUITS_T) sqlite3_column_int(pStmt, 5); // @suppress("Invalid arguments")
				altBoard.pBoard = pBoard;
				retValue = true;
			}
			break;

		default:
			throw(errCode);

		}

		// And all is well, SQL wise
		lastErrorCode = 0x0;

	}

	catch (int e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::getAlternateBoard -> sqlite3 reports %s\n", sqlite3_errstr( e));
#endif
		lastErrorCode = e;
	}

	catch (const char *e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::getAlternateBoard -> %s\n", e);
#endif
		lastErrorCode = 0;
	}

	// And done
	return (retValue);

}

// Lookup primary board id
BOARDID_T SolitaireDB::getPrimaryBoardId(CSolitaireBoard *pBoard, bool *isAlt) {

	// If not init, return an error
	if (!isInit())
		return (-1);

	// Constants
	const size_t CONVERSION_BUFFER_SIZE = 200;

	// Variables
	char converted[CONVERSION_BUFFER_SIZE];
	int errCode;
	BOARDID_T boardId = -2;
	sqlite3_stmt *pStmt = (sqlite3_stmt*) 0x0;

	// Wrap it
	try {

		// Not an alt board (not yet at least)
		if ((bool*) 0x0 != isAlt)
			*isAlt = false;

		// Compress the board
		COMP_BOARD compBoard;
		if (!pBoard->CompressBoard(&compBoard)) {
			throw("Failure to compress original board");
		}

		// Convert the board to readable text
		memset(converted, 0x0, CONVERSION_BUFFER_SIZE);
		int convertSize = convertToText(compBoard.uacData, sizeof(compBoard.uacData), converted,
				CONVERSION_BUFFER_SIZE);

		// Search through the primary
		pStmt = preparedStatements[STMT_LOOKUP_PRIMARY_BOARD].stmtPrepared;
		sqlite3_reset(pStmt); // @suppress("Invalid arguments")
		sqlite3_clear_bindings(pStmt); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_text(pStmt, 1, converted, convertSize, SQLITE_STATIC); // @suppress("Invalid arguments")
		errCode = sqlite3_bind_int(pStmt, 2, pBoard->GetSolType()); // @suppress("Invalid arguments")
		errCode = sqlite3_step(pStmt); // @suppress("Invalid arguments")
		switch (errCode) {

		case SQLITE_DONE:
			// Wasn't found
			boardId = 0x0;
			break;

		case SQLITE_ROW:
			// Found a value?
			if ( SQLITE_NULL == sqlite3_column_type(pStmt, 0)) // @suppress("Invalid arguments")
				boardId = 0x0;
			else
				boardId = sqlite3_column_int64(pStmt, 0); // @suppress("Invalid arguments")
			break;

		default:
			throw(errCode);

		}

		// Need to search the alternate?
		if (0x0 == boardId) {

			// Search through the primary
			pStmt = preparedStatements[STMT_LOOKUP_ALTERNATE_BOARD].stmtPrepared;
			sqlite3_reset(pStmt); // @suppress("Invalid arguments")
			sqlite3_clear_bindings(pStmt); // @suppress("Invalid arguments")
			errCode = sqlite3_bind_text(pStmt, 1, converted, convertSize, SQLITE_STATIC); // @suppress("Invalid arguments")
			errCode = sqlite3_bind_int(pStmt, 2, pBoard->GetSolType()); // @suppress("Invalid arguments")
			errCode = sqlite3_step(pStmt); // @suppress("Invalid arguments")
			switch (errCode) {

			case SQLITE_DONE:
				// Wasn't found
				boardId = 0x0;
				break;

			case SQLITE_ROW:
				// Found a value?
				if ( SQLITE_NULL == sqlite3_column_type(pStmt, 0)) // @suppress("Invalid arguments")
					boardId = 0x0;
				else {
					boardId = sqlite3_column_int64(pStmt, 0); // @suppress("Invalid arguments")
					if ((bool*) 0x0 != isAlt)
						*isAlt = true;
				}
				break;

			default:
				throw(errCode);

			}

		} // endif search alternate

	}

	catch (int e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::getPrimaryBoardId -> sqlite3 reports %s\n", sqlite3_errstr( e));
#endif
		lastErrorCode = e;
		boardId = -1 * e;
	}

	catch (const char *e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during SolitaireDB::getPrimaryBoardId -> %s\n", e);
#endif
		lastErrorCode = 0;
		boardId = -3;
	}

	// And done
	return (boardId);

}

//
// Save moves to the database
//
// The board, and all of its alternates, and all of the moves for the board,
// will be stored to the database.
//
// The solution will STOP being written when all remaining moves are to
// the foundation - as such a board solution from that point is
// trivially trivial.
//
// @param pBoard The originating board for the solution
// @param lstMoves The moves used to solve the board
// @param timeTaken The number of milliseconds required to solve the board
// @return TRUE if everything was saved, FALSE otherwise

bool saveMovesToDB(const CSolitaireBoard *pBoard, const LST_MOVES &lstMoves, unsigned long timeTaken) {

	// Variables
	bool bSaved = false;
	BOARDID_T boardId;
	SOLUTIONID_T topSolutionId = 0;
	size_t movesSoFar = 0x0;

	try {

		// Allocate a DB object
		SolitaireDB sDB;
		if (!sDB.initDB())
			throw("Failure to init db");

		// Start a loop over all of the moves
		CSolitaireBoard *pCurBoard = CSolitaireBoard::CreateBoardCopy(pBoard);
		CITR_MOVES itrMove = lstMoves.cbegin();
		for (; lstMoves.cend() != itrMove; ++itrMove, ++movesSoFar) {

			// Check to see if the board already exists
			boardId = sDB.getPrimaryBoardId(pCurBoard, (bool*) 0x0);
			if (0x0 > boardId) {
				char errMsg[1024 + 1];
				sprintf(errMsg, "SQL error: %s", sDB.getLastDBError());
				throw(errMsg);
			} else if (0x0 < boardId) {

#ifdef	ACTIVE_LOGGING
				fprintf( stderr, "At movesSoFar %lu, current board matches existing board %lld\n", movesSoFar, boardId);
#endif

				// Apply the move and continue
				const MOVE_T &curMove = *itrMove;
				pCurBoard->ApplyMove(curMove);
				continue;

			}

			// Create the solution header
			SOLUTIONID_T solutionId = sDB.createSolutionHeader(topSolutionId, (lstMoves.size() - movesSoFar),
					timeTaken);
			if (0 == topSolutionId) {
				topSolutionId = solutionId;
			}

			// Allocate a board and associate the solution
			boardId = sDB.createNewBoard(pCurBoard);
			sDB.attachBoardAndSolution(boardId, solutionId);

			// Produce and save all of the alternate boards
			LST_ALTERNATE_BOARD lstBoards = findAlternateBoards(pCurBoard);
			ITR_ALTERNATE_BOARD itrBoards = lstBoards.begin();
			for (; lstBoards.end() != itrBoards; ++itrBoards) {

				// Ensure the board is not already recorded
				S_ALTERNATE_BOARD &altBoard = *itrBoards;
				BOARDID_T lookupId = sDB.getPrimaryBoardId(altBoard.pBoard, (bool*) 0x0);
				if ((0 == lookupId) && (!sDB.insertAlternateBoard(boardId, altBoard))) {
					throw("Could not save alternate board");
				};
				if (pCurBoard != altBoard.pBoard) {
					delete altBoard.pBoard;
					altBoard.pBoard = (CSolitaireBoard*) 0x0;
				}
			}
			lstBoards.clear();

			// Record this move and all forward
			CITR_MOVES itrStart = itrMove;
			int nMoveNum = 1;
			for (; lstMoves.cend() != itrStart; ++itrStart, ++nMoveNum) {
				const MOVE_T &move = *itrStart;
				if (!sDB.insertSolutionDetail(solutionId, nMoveNum, move)) {
					throw("Could not save solution detail");
				}
			}

			// Apply the move
			const MOVE_T &curMove = *itrMove;
			pCurBoard->ApplyMove(curMove);

		}

		// And all is well
		bSaved = true;

	}

	catch (const char *e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Error during saveMovesToDB -> %s\n", e);
#endif
	}

	// And done
	return (bSaved);

}
