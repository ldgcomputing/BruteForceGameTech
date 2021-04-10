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

// Required includes
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <string.h>
#include <time.h>

// STL includes
#include <set>
#include <string>
#include <utility>

// Project includes
#include <nodeSupport.h>
#include <SolitaireBoard.h>

// Helpful typedefs
typedef std::set<SEED_T> SET_SEEDS;
typedef SET_SEEDS::const_iterator CITR_SEEDS;
typedef SET_SEEDS::iterator ITR_SEEDS;
typedef std::pair<ITR_SEEDS, bool> INS_SEEDS;

// Global variable - the set of all valid seed values
static SET_SEEDS g_allValidSeeds;

//
// A Bakers Game board that can be modified.
//

class CSB_BakersGameModifiable: public CSB_BakersGame {

public:

	// Construction
	CSB_BakersGameModifiable(const bool bModeEasy) :
			CSB_BakersGame(bModeEasy) {
		ClearBoard();
		SetSolType( bModeEasy ? ST_BAKERS_GAME_EASY : ST_BAKERS_GAME);
	}

	// Destruction
	virtual ~CSB_BakersGameModifiable() {

	}

	// Setters
	void setFoundation(CARDSUITS_T suit, CARDVALUES_T value) {
		m_acFoundationHighest[suit] = value;
	}
	void setReserveSpace(int space, CARD_T value) {
		if ((0 <= space) && (CSB_BakersGame::NUM_RESV_ALLOCATED > space)) {
			m_acReserve[space] = value;
		}
	}
	void setTableau(int col, int row, CARD_T value) {
		if ((0 <= col) && (CSB_BakersGame::NUM_COLS_ALLOCATED > col) && (0 <= row)
				&& (CSB_BakersGame::NUM_ROWS_ALLOCATED > row)) {
			m_acTableau[col][row] = value;
		}
	}
	void setPlayable(const bool bPlayable) {
		m_bIsPlayable = bPlayable;
	}

};

//
// Load the list of valid seed values into memory.
//

void loadValidSeedValues() {

	// The maximum size of an input line
	const int MAX_LINE_SIZE = 100;

	// Variables
	char inpLine[MAX_LINE_SIZE + 1];

	// Open the file for processing
	FILE *fInput = fopen("resources/validSeeds.txt", "r");
	if ((FILE*) 0x0 != fInput) {

		// While not at the EOF
		while (!feof(fInput)) {

			// Read a line and convert it
			memset(inpLine, 0x0, sizeof(inpLine));
			if ((char*) 0x0 != fgets(inpLine, MAX_LINE_SIZE, fInput)) {
				SEED_T value;
				if (1 == sscanf(inpLine, "%u", &value)) {
					g_allValidSeeds.insert(value);
				}
			} else {
				break; // caught an error
			}

		} // endwhile not EOF

		// And close
		fclose(fInput);
		fInput = (FILE*) 0x0;

	}

}

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

SEED_T validateSeedValue(SEED_T seedValue) {

	// Has the set been loaded?
	if (0x0 == g_allValidSeeds.size())
		loadValidSeedValues();

	// Check to see if its in the set
	SEED_T retValue = seedValue;
	if (1 != g_allValidSeeds.count(seedValue)) {

		// Pick one at random then
		srand(seedValue);
		const int select = (rand() % g_allValidSeeds.size());
		CITR_SEEDS citrSeed = g_allValidSeeds.cbegin();
		for (int pos = 0; select > pos; ++pos, ++citrSeed)
			;
		retValue = *citrSeed;

	}

	// And done
	return (retValue);

}

//
// Convert a list of solution moves to a JSON representation
//
// See usage.txt for more details.
//
// @param lstMoves The moves to report
// @param pBuffer The output buffer to write the JSON to.
// @param bufSize The size of the output buffer.
// @return The number of bytes written to the buffer or, if negative, an error code.
//

int convertMovesToJSON( LST_MOVES &lstMoves, char *pBuffer, const size_t bufSize) {

	// Constants
	const int TRANSLATE_BUF_SIZE = 100;

	// Going to use a std::string to hold the JSON details until final conversion
	char translateBuffer[TRANSLATE_BUF_SIZE + 1];
	std::string sOutputJSON;

	// Make the JSON - init
	sOutputJSON.append("{ \"Moves\" : [");

	// Loop over the moves
	for( CITR_MOVES itrMoves = lstMoves.begin(); lstMoves.end() != itrMoves; ++ itrMoves) {

		const MOVE_T &curMove = *itrMoves;

		if( lstMoves.begin() != itrMoves) {
			sOutputJSON.append( ",");
		}
		sOutputJSON.append( "\n\t{ ");

		// Move from
		sOutputJSON.append( " \"movefrom\" : \"");
		sOutputJSON.append( MOVE_VALUES[curMove.eMoveFrom]);
		sOutputJSON.append( "\", ");

		// Column & row
		sOutputJSON.append( "\"fromcol\" : ");
		sprintf( translateBuffer, "%d", curMove.nFromCol);
		sOutputJSON.append( translateBuffer);
		sOutputJSON.append( ", \"fromrow\" : ");
		sprintf( translateBuffer, "%d", curMove.nFromRow);
		sOutputJSON.append( translateBuffer);
		sOutputJSON.append( ", ");

		// Move to
		sOutputJSON.append( " \"moveto\" : \"");
		sOutputJSON.append( MOVE_VALUES[curMove.eMoveTo]);
		sOutputJSON.append( "\", ");

		// Column & row
		sOutputJSON.append( "\"tocol\" : ");
		sprintf( translateBuffer, "%d", curMove.nToCol);
		sOutputJSON.append( translateBuffer);
		sOutputJSON.append( ", \"torow\" : ");
		sprintf( translateBuffer, "%d", curMove.nToRow);
		sOutputJSON.append( translateBuffer);
		sOutputJSON.append( " }");

	}

	// Make the JSON - complete
	sOutputJSON.append("\n]\n}\n");

	// If there sufficient space to do the conversion?
	int retValue = 0x0;
	if (sOutputJSON.length() < (bufSize - 1)) {
		// Convert and set
		memset(pBuffer, 0x0, bufSize);
		strncpy(pBuffer, sOutputJSON.c_str(), bufSize - 1);
		retValue = sOutputJSON.length();
	} else {
		retValue = -1;
	}

	// And done
	return (retValue);

}

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

int convertBoardToJSON(SEED_T seedVal, CSolitaireBoard *pBoard, char *pBuffer, const size_t bufSize) {

	// Constants
	const int TRANSLATE_BUF_SIZE = 100;

	// Going to use a std::string to hold the JSON details until final conversion
	char translateBuffer[TRANSLATE_BUF_SIZE + 1];
	std::string sOutputJSON;

	// Make the JSON - init
	sOutputJSON.append("{ \n");

	// Make the JSON - seed value
	sOutputJSON.append("\"seed_value\" : ");
	sprintf(translateBuffer, "%u", seedVal);
	sOutputJSON.append(translateBuffer);
	sOutputJSON.append(",\n");

	// Make the JSON - foundations
	sOutputJSON.append("\"foundations\" : [\n");
	sOutputJSON.append(" { \"suit\" : \"clubs\" , \"value\" : \"");
	memset(translateBuffer, 0x0, sizeof(translateBuffer));
	convertCardValueToText(pBoard->GetFoundationCardValue(CS_CLUBS), translateBuffer, TRANSLATE_BUF_SIZE);
	sOutputJSON.append(translateBuffer);
	sOutputJSON.append("\" },\n");
	sOutputJSON.append(" { \"suit\" : \"diamonds\" , \"value\" : \"");
	memset(translateBuffer, 0x0, sizeof(translateBuffer));
	convertCardValueToText(pBoard->GetFoundationCardValue(CS_DIAMONDS), translateBuffer, TRANSLATE_BUF_SIZE);
	sOutputJSON.append(translateBuffer);
	sOutputJSON.append("\" },\n");
	sOutputJSON.append(" { \"suit\" : \"hearts\" , \"value\" : \"");
	memset(translateBuffer, 0x0, sizeof(translateBuffer));
	convertCardValueToText(pBoard->GetFoundationCardValue(CS_HEARTS), translateBuffer, TRANSLATE_BUF_SIZE);
	sOutputJSON.append(translateBuffer);
	sOutputJSON.append("\" },\n");
	sOutputJSON.append(" { \"suit\" : \"spades\" , \"value\" : \"");
	memset(translateBuffer, 0x0, sizeof(translateBuffer));
	convertCardValueToText(pBoard->GetFoundationCardValue(CS_SPADES), translateBuffer, TRANSLATE_BUF_SIZE);
	sOutputJSON.append(translateBuffer);
	sOutputJSON.append("\" }\n");
	sOutputJSON.append("],\n");

	// Make the JSON - reserve
	sOutputJSON.append("\"reserve\" : [ ");
	for (int nCol = 0; nCol < pBoard->NumReserveSpaces(); ++nCol) {
		memset(translateBuffer, 0x0, sizeof(translateBuffer));
		convertCardToText(pBoard->GetReserveCard(nCol), translateBuffer, TRANSLATE_BUF_SIZE);
		if (0x0 != nCol) {
			sOutputJSON.append(", ");
		}
		sOutputJSON.append("\"");
		sOutputJSON.append(translateBuffer);
		sOutputJSON.append("\"");
	}
	sOutputJSON.append(" ],\n");

	// Make the JSON - tableau
	sOutputJSON.append("\"tableau\" : [ \n");
	for (int nRow = 0; nRow < pBoard->NumRows(); ++nRow) {

		bool dataToAppend = false;
		std::string rowOutput;

		if (0x0 != nRow) {
			rowOutput.append(",\n");
		}
		rowOutput.append("[ ");
		for (int nCol = 0; nCol < pBoard->NumCols(); ++nCol) {

			// Get the card
			CARD_T card = pBoard->GetTableauCardAt(nCol, nRow);
			memset(translateBuffer, 0x0, sizeof(translateBuffer));
			convertCardToText(card, translateBuffer, TRANSLATE_BUF_SIZE);
			dataToAppend |= (0x0 != translateBuffer[0]);

			// Write it
			if (0x0 != nCol) {
				rowOutput.append(", ");
			}
			rowOutput.append("\"");
			rowOutput.append(translateBuffer);
			rowOutput.append("\"");

		} // endfor columns
		rowOutput.append(" ]");

		// Append it?
		if (dataToAppend) {
			sOutputJSON.append(rowOutput);
		}

	} // endfor rows
	sOutputJSON.append("\n]\n");

	// Make the JSON - complete
	sOutputJSON.append("}\n");

	// If there sufficient space to do the conversion?
	int retValue = 0x0;
	if (sOutputJSON.length() < (bufSize - 1)) {
		// Convert and set
		memset(pBuffer, 0x0, bufSize);
		strncpy(pBuffer, sOutputJSON.c_str(), bufSize - 1);
		retValue = sOutputJSON.length();
	} else {
		retValue = -1;
	}

	// And done
	return (retValue);

}

//
// Convert a board representation to an actual board
//
// See usage.txt for more details.
//
// WARNING!  No data validation is provided by this function - it
// is quite possible to corrupt
//
// See usage.txt for more details.
//
// @param boardType The type of board to create;
// @param lstFoundation A list of strings representing the highest foundation value, or blank for empty.
// @param lstReserve A list of strings for each reserve space holding the string for the card, or blank for empty.
// @param lstRows A list of lists of string data for each card, or blank for empty.
// @return A constructed board.
//

CSolitaireBoard* convertStringsToBoard(const int boardType, std::vector<std::string> lstFoundation,
		std::vector<std::string> lstReserve, std::vector<std::vector<std::string>> lstRows) {

	typedef std::vector<std::vector<std::string>> VCT_ROWS;
	typedef std::vector<std::string> VCT_ROW;
	typedef VCT_ROWS::const_iterator CITR_ROWS;
	typedef VCT_ROW::const_iterator CITR_ROW;

	CSolitaireBoard *retValue = ((CSolitaireBoard*) 0x0);

	// Wrap it all
	try {

		// Is it Bakers Game?
		bool isEasy = false;
		switch (boardType) {
		case ST_BAKERS_GAME_EASY:
			isEasy = true;
			break;
		case ST_BAKERS_GAME:
			break;
		default:
			throw("Unknown game type");
		}

		// Create a board
		CSB_BakersGameModifiable board(isEasy);

		// Validate input
		if (lstFoundation.size() != CS_INVALID)
			throw("Invalid foundation size");
		if (lstReserve.size() != (size_t) board.NumReserveSpaces())
			throw("Invalid reserve size");

#ifdef ACTIVE_LOGGING
		fprintf( stderr, "CS_CLUBS = %s\n", lstFoundation.at(CS_CLUBS).c_str());
		fprintf( stderr, "CS_DIAMONDS = %s\n", lstFoundation.at(CS_DIAMONDS).c_str());
		fprintf( stderr, "CS_HEARTS = %s\n", lstFoundation.at(CS_HEARTS).c_str());
		fprintf( stderr, "CS_SPADES = %s\n", lstFoundation.at(CS_SPADES).c_str());
#endif

		// Set the foundations
		board.setFoundation(CS_CLUBS, convertTextToCardValue(lstFoundation.at(CS_CLUBS).c_str()));
		board.setFoundation(CS_DIAMONDS, convertTextToCardValue(lstFoundation.at(CS_DIAMONDS).c_str()));
		board.setFoundation(CS_HEARTS, convertTextToCardValue(lstFoundation.at(CS_HEARTS).c_str()));
		board.setFoundation(CS_SPADES, convertTextToCardValue(lstFoundation.at(CS_SPADES).c_str()));

#ifdef ACTIVE_LOGGING
		char cardValue[100];
		if( !convertCardValueToText( board.GetFoundationCardValue(CS_CLUBS), cardValue, 100))
			strcpy( cardValue, "Unknown");
		fprintf( stderr, "CS_CLUBS on convert = %s\n", cardValue);
		if( !convertCardValueToText( board.GetFoundationCardValue(CS_DIAMONDS), cardValue, 100))
			strcpy( cardValue, "Unknown");
		fprintf( stderr, "CS_DIAMONDS on convert = %s\n", cardValue);
		if( !convertCardValueToText( board.GetFoundationCardValue(CS_HEARTS), cardValue, 100))
			strcpy( cardValue, "Unknown");
		fprintf( stderr, "CS_HEARTS on convert = %s\n", cardValue);
		if( !convertCardValueToText( board.GetFoundationCardValue(CS_SPADES), cardValue, 100))
			strcpy( cardValue, "Unknown");
		fprintf( stderr, "CS_SPADES on convert = %s\n", cardValue);
#endif

		// Set the reserve
		for (int nPos = 0; board.NumReserveSpaces() > nPos; ++nPos) {
			board.setReserveSpace(nPos, convertTextToCard(lstReserve.at(nPos).c_str()));
		}

		// Set the tableau
		int nRow = 0;
		CITR_ROWS citrRows = lstRows.cbegin();
		for (; lstRows.cend() != citrRows; ++citrRows, ++nRow) {

			// Extract the row and validate it
			const VCT_ROW &row = *citrRows;
			if (row.size() != (size_t) board.NumCols())
				throw("Invalid number of columns on row");

			// Loop over columns
			int nCol = 0;
			CITR_ROW citrRow = row.cbegin();
			for (; row.cend() != citrRow; ++citrRow, ++nCol) {

				// Fill it in
				const std::string &cell = *citrRow;
				board.setTableau(nCol, nRow, convertTextToCard(cell.c_str()));

			} // endfor loop over columns

		} // endfor loop over rows

		// Make a good copy
		retValue = CSolitaireBoard::CreateBoardCopy(&board);

	}

	catch (const char *e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Caught an exception: %s\n", e);
#endif
		retValue = ((CSolitaireBoard*) 0x0);
	}

	// And return the board
	return (retValue);

}

//
// Extract a string from an array
//
// @param isolate The V8 isolation object.
// @param context The V8 context object.
// @param arr The V8 array from which to extract the string
// @param pos The position to extract
// @return An std::string object of the value, or a blank string
//

std::string extractStringFromV8Array(v8::Isolate *isolate, v8::Local<v8::Context> context, v8::Local<v8::Array> arr,
		int pos) {

	// Empty value
	std::string retValue("");  // @suppress("Invalid arguments")

	// Convert
	v8::Local<v8::Value> emptyValue;
	v8::MaybeLocal<v8::Value> mlv = arr->Get(context, pos); // @suppress("Invalid arguments")
	if (!mlv.IsEmpty()) {
		v8::Local<v8::Value> lv = mlv.FromMaybe(emptyValue);
		v8::Local<v8::String> s = v8::Local<v8::String>::Cast(lv);
		unsigned char value[100];
		memset(value, 0x0, sizeof(value));
		s->WriteOneByte(isolate, value, 0, 99, v8::String::NO_OPTIONS);
		retValue = (const char *) value;
	}

	// And done
	return (retValue);

}
