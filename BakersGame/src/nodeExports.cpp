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

// Project includes
#include <SolitaireBoard.h>
#include <nodeSupport.h>
#include <solverSupport.h>
#include <v8Support.h>

// Global constants
static const char *ERROR_MESSAGES[] = { "Not an error", // Value = 0
		"Unable to create board",						// Value = 1
		"Invalid board size",							// Value = 2
		"Unable to convert to JSON",					// Value = 3
		"No arguments provided",						// Value = 4
		"Invalid arguments provided",					// Value = 5
		"Invalid FOUNDATION arguments provided",		// Value = 6
		"Invalid RESERVE arguments provided",			// Value = 7
		"Invalid TABELAU argument provided",			// Value = 8
		"Cannot convert to game board",					// Value = 9
		"Impossible game board",						// Value = 10
		"Unable to convert moves",						// Value = 11
		"Timeout attempting to solve"					// Value = 12
		};
static const char MEMORY_ALLOCATION_FAILURE[] = "{ 'errorcode' : 255, 'errormsg' : 'Memory Allocation Failure' }";
static const int OUTPUT_BUFFER_ALLOCATION = 90000;

// Node includes
#include <node.h>

namespace Lambda_BakersGame {

// Using helpfuls
using v8::FunctionCallbackInfo;
using v8::Array;
using v8::Isolate;
using v8::Local;
using v8::MaybeLocal;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

void Method_BF_BakersGameEasy_GetBoard(const FunctionCallbackInfo<Value> &args) {

	// Get the program arguments
	Isolate *isolate = args.GetIsolate();

	// Attempt to allocate space for the return message
	char *pBuffer = new char[OUTPUT_BUFFER_ALLOCATION];
	if ((char*) 0x0 == pBuffer) {
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, (char*) MEMORY_ALLOCATION_FAILURE).ToLocalChecked());
		return;
	}

	// Wrap it
	try {

		// Was a seed value provided?
		SEED_T seedVal = 0x0;
		if ((args.Length() >= 1) && (args[0]->IsNumber())) {
			double passedNum = args[0].As<Number>()->Value();
			while (passedNum > UINT32_MAX)
				passedNum -= UINT32_MAX;
			while (0 > passedNum)
				passedNum += UINT32_MAX;
			seedVal = (SEED_T) passedNum;
		}

		// Make sure the seed value is valid - or pick a random one from the list
		seedVal = validateSeedValue(ST_BAKERS_GAME_EASY, seedVal);

		// Create the board
		CSB_BakersGame *pBoard = (CSB_BakersGame*) CSolitaireBoard::CreateBoard(ST_BAKERS_GAME_EASY);
		if ((CSB_BakersGame*) 0x0 == pBoard) {
			throw(1);
		}
		srand(seedVal);
		pBoard->InitRandomBoard();

		// Convert to a JSON representation
		if (0 >= convertBoardToJSON(seedVal, pBoard, pBuffer, OUTPUT_BUFFER_ALLOCATION)) {
			throw(3);
		}

	}

	catch (const int e) {
		memset(pBuffer, 0x0, OUTPUT_BUFFER_ALLOCATION);
		sprintf(pBuffer, "{ \"errorcode\" : %d, \"errormsg\" : \"%s\" }", e, ERROR_MESSAGES[e]);
	}

	// Set the return value
	args.GetReturnValue().Set(String::NewFromUtf8(isolate, (char*) pBuffer).ToLocalChecked());

	// Free resources
	delete[] pBuffer;
	pBuffer = (char*) 0x0;

}

void Method_BF_BakersGame_SolveBoard(const FunctionCallbackInfo<Value> &args) {

	// Get the program arguments
	Isolate *isolate = args.GetIsolate();
	Local<v8::Context> context = v8::Context::New(isolate);

	// Other variables
	CSolitaireBoard *pBoard = (CSolitaireBoard*) 0x0;
	v8::Local<v8::Value> emptyValue;

	// Attempt to allocate space for the return message
	char *pBuffer = new char[OUTPUT_BUFFER_ALLOCATION];
	if ((char*) 0x0 == pBuffer) {
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, (char*) MEMORY_ALLOCATION_FAILURE).ToLocalChecked());
		return;
	}

	// Wrap it
	try {

		// Have enough arguments been provided?
		if (args.Length() < 4)
			throw(5);

		// Has a game type been provided?
		unsigned char inputType[100 + 1];
		memset(inputType, 0x0, sizeof(inputType));
		Local<v8::String> argGameType = args[0].As<String>();
		argGameType->WriteOneByte(isolate, inputType, 0, 100);
		SOLITAIRE_T gameType = convertTextToGameType((const char*) inputType);

		// Has a foundation been provided?
		std::vector<std::string> vctFoundation;
		if (!args[1]->IsArray())
			throw(5);
		Local<Array> arrFoundation = Local<Array>::Cast(args[1]);
		if (arrFoundation->Length() != 4)
			throw(6);
		for (int nFnd = 0; 4 > nFnd; ++nFnd) {
			std::string value = extractStringFromV8Array(isolate, context, arrFoundation, nFnd);
			vctFoundation.push_back(value);
		}

		// Has reserve been provided?
		std::vector<std::string> vctReserve;
		if (!args[2]->IsArray())
			throw(5);
		Local<Array> arrReserve = Local<Array>::Cast(args[2]);
		if (arrReserve->Length() != 4)
			throw(7);
		for (int nRsv = 0; 4 > nRsv; ++nRsv) {
			std::string value = extractStringFromV8Array(isolate, context, arrReserve, nRsv);
			vctReserve.push_back(value);
		}

		// Has tableau been provided?
		std::vector<std::vector<std::string>> vctTableau;
		if (!args[3]->IsArray())
			throw(5);
		Local<Array> arrTableau = Local<Array>::Cast(args[3]);
		for (unsigned int nRow = 0; arrTableau->Length() > nRow; ++nRow) {

			// This value must be an array with 8 items
			v8::MaybeLocal<v8::Value> maybeRow = arrTableau->Get(context, nRow); // @suppress("Invalid arguments")
			if (maybeRow.IsEmpty())
				throw(8);
			v8::Local<v8::Value> localRow = maybeRow.FromMaybe(emptyValue);
			if (!localRow->IsArray())
				throw(8);
			v8::Local<v8::Array> aRow = v8::Local<v8::Array>::Cast(localRow);
			if (aRow->Length() != 8)
				throw(8);

			// Go through the array and make a vector
			std::vector<std::string> vctRow;
			for (int nCol = 0; 8 > nCol; ++nCol) {
				std::string value = extractStringFromV8Array(isolate, context, aRow, nCol);
				vctRow.push_back(value);
			}
			vctTableau.push_back(vctRow);

		} // endfor loop over tableau

		// Now make that board
		pBoard = convertStringsToBoard(gameType, vctFoundation, vctReserve, vctTableau);
		if ((CSolitaireBoard*) 0x0 == pBoard)
			throw(9);

		// Check the board validity
		if( !validateCardsAndBoard( pBoard)) {
			throw(10);
		}


		// Call for a solution
		bool timeout = false;
		LST_MOVES lstMoves = findOrSolveBoard(pBoard, &timeout);
		if( timeout) {
			throw(12);
		}
		else if (0 >= convertMovesToJSON(lstMoves, pBuffer, OUTPUT_BUFFER_ALLOCATION)) {
			throw(11);
		}

	}

	catch (const int e) {
		memset(pBuffer, 0x0, OUTPUT_BUFFER_ALLOCATION);
		sprintf(pBuffer, "{ \"errorcode\" : %d, \"errormsg\" : \"%s\" }", e, ERROR_MESSAGES[e]);
	}

	// Set the return value
	args.GetReturnValue().Set(String::NewFromUtf8(isolate, (char*) pBuffer).ToLocalChecked());

	// Free resources
	delete[] pBuffer;
	pBuffer = (char*) 0x0;
	if ((CSolitaireBoard*) 0x0 != pBoard) {
		delete pBoard;
		pBoard = (CSolitaireBoard*) 0x0;
	}

}

void Initialize(Local<Object> exports) {
	NODE_SET_METHOD(exports, "Method_BF_BakersGameEasy_GetBoard", Method_BF_BakersGameEasy_GetBoard);
	NODE_SET_METHOD(exports, "Method_BF_BakersGame_SolveBoard", Method_BF_BakersGame_SolveBoard);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)

}  // namespace Lambda_BakersGame
