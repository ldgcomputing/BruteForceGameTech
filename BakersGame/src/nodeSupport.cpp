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
static SET_SEEDS g_allValidSeedsEasy;
static SET_SEEDS g_allValidSeedsStnd;

//
// Validate that every card is present on the board
//
// @param pBoard The board to analyze
// @return TRUE if every card is accounted for, FALSE otherwise
//

bool validateCardsAndBoard(const CSolitaireBoard *pBoard) {

	// Assume all is well
	bool retValue = true;

	// Create a set to ensure all cards have eventually been seen
	std::set<CARD_T> setCards;
	for (int ics = CS_CLUBS; CS_INVALID > ics; ++ics) {
		for (int icv = CV_ACE; CV_INVALID > icv; ++icv) {
			CARDSUITS_T cs = (CARDSUITS_T) ics;
			CARDVALUES_T cv = (CARDVALUES_T) icv;
			CARD_T card(cs, cv);
			setCards.insert(card);
		}
	}

	//
	// Check the validity of that board - every card must be present and only present once
	//

	CARD_T emptyCard;

	// Check foundation
	for (int nSuit = 0; retValue && (CS_INVALID > nSuit); ++nSuit) {
		CARDSUITS_T cs = (CARDSUITS_T) nSuit;
		CARDVALUES_T cv = pBoard->GetFoundationCardValue(cs);
		for (int nValue = CV_ACE; cv > nValue; ++nValue) {
			CARDVALUES_T intCV = (CARDVALUES_T) nValue;
			CARD_T fndCard(cs, intCV);
			if ((emptyCard.eSuit == fndCard.eSuit) && (emptyCard.eValue == fndCard.eValue))
				continue;
			std::set<CARD_T>::iterator itrCard = setCards.find(fndCard);
			if (setCards.end() == itrCard) {
#ifdef	ACTIVE_LOGGING
				char cardValue[100];
				if (!convertCardToText(fndCard, cardValue, 99))
					strcpy(cardValue, "**INVALID**");
				fprintf( stderr, "Tried to delete card in foundation %s\n", cardValue);
#endif
				retValue = false;
			} else
				setCards.erase(itrCard);
		}
	}

	// Go through the reserve
	for (int nRsv = 0; retValue && (pBoard->NumReserveSpaces() > nRsv); ++nRsv) {
		CARD_T rsvCard = pBoard->GetReserveCard(nRsv);
		if ((emptyCard.eSuit == rsvCard.eSuit) && (emptyCard.eValue == rsvCard.eValue))
			continue;
		std::set<CARD_T>::iterator itrCard = setCards.find(rsvCard);
		if (setCards.end() == itrCard) {
#ifdef	ACTIVE_LOGGING
			char cardValue[100];
			if (!convertCardToText(rsvCard, cardValue, 99))
				strcpy(cardValue, "**INVALID**");
			fprintf( stderr, "Tried to delete card in reserve %s\n", cardValue);
#endif
			retValue = false;
		} else
			setCards.erase(itrCard);
	}

	// Go through the tableau
	for (int nRow = 0; retValue && (pBoard->NumRows() > nRow); ++nRow) {
		for (int nCol = 0; retValue && (pBoard->NumCols() > nCol); ++nCol) {
			CARD_T tblCard = pBoard->GetTableauCardAt(nCol, nRow);
			if ((emptyCard.eSuit != tblCard.eSuit) && (emptyCard.eValue != tblCard.eValue)) {
				std::set<CARD_T>::iterator itrCard = setCards.find(tblCard);
				if (setCards.end() == itrCard) {
#ifdef	ACTIVE_LOGGING
					char cardValue[100];
					if (!convertCardToText(tblCard, cardValue, 99))
						strcpy(cardValue, "**INVALID**");
					fprintf( stderr, "Tried to delete card in tableau %s at row %d col %d\n", cardValue, nRow,
							nCol);
#endif
					retValue = false;
				} else
					setCards.erase(itrCard);
			}
		} // endfor col
	} // endfor row

	// Should be no cards left
	if (!setCards.empty()) {
#ifdef ACTIVE_LOGGING
		fprintf( stderr, "There are %lu cards remaining in the set\n", setCards.size());
#endif
		retValue = false;
	}

	// And done
	return (retValue);

}

//
// Load the list of valid seed values into memory.
//

void loadValidSeedValues() {

	// The maximum size of an input line
	const int MAX_LINE_SIZE = 100;

	// Variables
	char inpLine[MAX_LINE_SIZE + 1];

	// Open the standard file for processing
	FILE *fInput = fopen("resources/validSeedsBakersGame.txt", "r");
	if ((FILE*) 0x0 != fInput) {

		// While not at the EOF
		while (!feof(fInput)) {

			// Read a line and convert it
			memset(inpLine, 0x0, sizeof(inpLine));
			if ((char*) 0x0 != fgets(inpLine, MAX_LINE_SIZE, fInput)) {
				SEED_T value;
				if (1 == sscanf(inpLine, "%u", &value)) {
					g_allValidSeedsEasy.insert(value);
					g_allValidSeedsStnd.insert(value);
				}
			} else {
				break; // caught an error
			}

		} // endwhile not EOF

		// And close
		fclose(fInput);
		fInput = (FILE*) 0x0;

	}

	// Open the easy file for processing
	fInput = fopen("resources/validSeedsBakersGameEasy.txt", "r");
	if ((FILE*) 0x0 != fInput) {

		// While not at the EOF
		while (!feof(fInput)) {

			// Read a line and convert it
			memset(inpLine, 0x0, sizeof(inpLine));
			if ((char*) 0x0 != fgets(inpLine, MAX_LINE_SIZE, fInput)) {
				SEED_T value;
				if (1 == sscanf(inpLine, "%u", &value)) {
					g_allValidSeedsEasy.insert(value);
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
// @param gameType The type of game to check.
// @param seedValue The value to check.
// @return	A valid seed value (may not be the original parameter), or zero for
//			a bad game type.
//

SEED_T validateSeedValue(SOLITAIRE_T gameType, SEED_T seedValue) {

	// Has the set been loaded?
	if (g_allValidSeedsEasy.empty())
		loadValidSeedValues();

	// Check to see if its in the set
	SEED_T retValue = seedValue;
	switch (gameType) {

	case ST_BAKERS_GAME:

		if (1 != g_allValidSeedsStnd.count(seedValue)) {

			// Pick one at random then
			srand(seedValue);
			const int select = (rand() % g_allValidSeedsStnd.size());
			CITR_SEEDS citrSeed = g_allValidSeedsStnd.cbegin();
			for (int pos = 0; select > pos; ++pos, ++citrSeed)
				;
			retValue = *citrSeed;

		}
		break;

	case ST_BAKERS_GAME_EASY:

		if (1 != g_allValidSeedsEasy.count(seedValue)) {

			// Pick one at random then
			srand(seedValue);
			const int select = (rand() % g_allValidSeedsEasy.size());
			CITR_SEEDS citrSeed = g_allValidSeedsEasy.cbegin();
			for (int pos = 0; select > pos; ++pos, ++citrSeed)
				;
			retValue = *citrSeed;

		}
		break;

	default:
		retValue = 0; // no good seed
		break;

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

int convertMovesToJSON(LST_MOVES &lstMoves, char *pBuffer, const size_t bufSize) {

	// Constants
	const int TRANSLATE_BUF_SIZE = 100;

	// Going to use a std::string to hold the JSON details until final conversion
	char translateBuffer[TRANSLATE_BUF_SIZE + 1];
	std::string sOutputJSON;

	// Make the JSON - init
	sOutputJSON.append("{ \"Moves\" : [");

	// Loop over the moves
	for (CITR_MOVES itrMoves = lstMoves.begin(); lstMoves.end() != itrMoves; ++itrMoves) {

		const MOVE_T &curMove = *itrMoves;

		if (lstMoves.begin() != itrMoves) {
			sOutputJSON.append(",");
		}
		sOutputJSON.append("\n\t{ ");

		// Move from
		sOutputJSON.append(" \"movefrom\" : \"");
		sOutputJSON.append(MOVE_VALUES[curMove.eMoveFrom]);
		sOutputJSON.append("\", ");

		// Column & row
		sOutputJSON.append("\"fromcol\" : ");
		sprintf(translateBuffer, "%d", curMove.nFromCol);
		sOutputJSON.append(translateBuffer);
		sOutputJSON.append(", \"fromrow\" : ");
		sprintf(translateBuffer, "%d", curMove.nFromRow);
		sOutputJSON.append(translateBuffer);
		sOutputJSON.append(", ");

		// Move to
		sOutputJSON.append(" \"moveto\" : \"");
		sOutputJSON.append(MOVE_VALUES[curMove.eMoveTo]);
		sOutputJSON.append("\", ");

		// Column & row
		sOutputJSON.append("\"tocol\" : ");
		sprintf(translateBuffer, "%d", curMove.nToCol);
		sOutputJSON.append(translateBuffer);
		sOutputJSON.append(", \"torow\" : ");
		sprintf(translateBuffer, "%d", curMove.nToRow);
		sOutputJSON.append(translateBuffer);
		sOutputJSON.append(" }");

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

