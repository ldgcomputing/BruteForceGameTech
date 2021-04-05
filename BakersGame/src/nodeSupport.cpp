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

int convertToJSON(SEED_T seedVal, CSolitaireBoard *pBoard, char *pBuffer, const size_t bufSize) {

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
	sOutputJSON.append(" { \"suit\" : \"club\" , \"value\" : \"");
	memset( translateBuffer, 0x0, sizeof( translateBuffer));
	convertCardValueToText(pBoard->GetFoundationCardValue(CS_CLUBS), translateBuffer, TRANSLATE_BUF_SIZE);
	sOutputJSON.append(translateBuffer);
	sOutputJSON.append("\" },\n");
	sOutputJSON.append(" { \"suit\" : \"diamond\" , \"value\" : \"");
	memset( translateBuffer, 0x0, sizeof( translateBuffer));
	convertCardValueToText(pBoard->GetFoundationCardValue(CS_DIAMONDS), translateBuffer, TRANSLATE_BUF_SIZE);
	sOutputJSON.append(translateBuffer);
	sOutputJSON.append("\" },\n");
	sOutputJSON.append(" { \"suit\" : \"heart\" , \"value\" : \"");
	memset( translateBuffer, 0x0, sizeof( translateBuffer));
	convertCardValueToText(pBoard->GetFoundationCardValue(CS_HEARTS), translateBuffer, TRANSLATE_BUF_SIZE);
	sOutputJSON.append(translateBuffer);
	sOutputJSON.append("\" },\n");
	sOutputJSON.append(" { \"suit\" : \"spade\" , \"value\" : \"");
	memset( translateBuffer, 0x0, sizeof( translateBuffer));
	convertCardValueToText(pBoard->GetFoundationCardValue(CS_SPADES), translateBuffer, TRANSLATE_BUF_SIZE);
	sOutputJSON.append(translateBuffer);
	sOutputJSON.append("\" },\n");
	sOutputJSON.append("],\n");

	// Make the JSON - reserve
	sOutputJSON.append("\"reserve\" : [ ");
	for (int nCol = 0; nCol < pBoard->NumReserveSpaces(); ++nCol) {
		memset( translateBuffer, 0x0, sizeof( translateBuffer));
		convertCardToText(pBoard->GetReserveCard(nCol), translateBuffer, TRANSLATE_BUF_SIZE);
		if( 0x0 != nCol) {
			sOutputJSON.append(", ");
		}
		sOutputJSON.append("\"");
		sOutputJSON.append(translateBuffer);
		sOutputJSON.append("\"");
	}
	sOutputJSON.append(" ],\n");

	// Make the JSON - tableau
	sOutputJSON.append("\"tableau\" : [ \n");
	for( int nRow = 0; nRow < pBoard->NumRows(); ++ nRow) {

		bool dataToAppend = false;
		std::string rowOutput;

		if( 0x0 != nRow) {
			rowOutput.append( ",\n");
		}
		rowOutput.append("[ ");
		for( int nCol = 0; nCol < pBoard->NumCols(); ++ nCol) {

			// Get the card
			CARD_T card = pBoard->GetTableauCardAt( nCol, nRow);
			memset( translateBuffer, 0x0, sizeof( translateBuffer));
			convertCardToText( card, translateBuffer, TRANSLATE_BUF_SIZE);
			dataToAppend |= (0x0 != translateBuffer[0]);

			// Write it
			if( 0x0 != nCol) {
				rowOutput.append( ", ");
			}
			rowOutput.append("\"");
			rowOutput.append(translateBuffer);
			rowOutput.append("\"");

		} // endfor columns
		rowOutput.append(" ]");

		// Append it?
		if(dataToAppend) {
			sOutputJSON.append( rowOutput);
		}

	} // endfor rows
	sOutputJSON.append( "\n]\n");

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
