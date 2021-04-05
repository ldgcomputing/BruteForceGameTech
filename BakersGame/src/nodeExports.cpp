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

// Project includes
#include <nodeSupport.h>
#include <SolitaireBoard.h>

// Global constants
static const char *ERROR_MESSAGES[] = { "Not an error", // Value = 0
		"Unable to create board",		// Value = 1
		"Invalid board size",			// Value = 2
		"Unable to convert to JSON",	// Value = 3
		"No arguments provided"			// Value = 4
		};
static const char MEMORY_ALLOCATION_FAILURE[] = "{ 'errorcode' : 255, 'errormsg' : 'Memory Allocation Failure' }";
static const int OUTPUT_BUFFER_ALLOCATION = 90000;

// Node includes
#include <node.h>

namespace Lambda_BakersGame {

// Using helpfuls
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
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
		seedVal = validateSeedValue(seedVal);

		// Create the board
		CSB_BakersGame *pBoard = (CSB_BakersGame*) CSolitaireBoard::CreateBoard(ST_BAKERS_GAME_EASY);
		if ((CSB_BakersGame*) 0x0 == pBoard) {
			throw(1);
		}
		srand( seedVal);
		pBoard -> InitRandomBoard();

		// Convert to a JSON representation
		if( ! convertToJSON( seedVal, pBoard, pBuffer, OUTPUT_BUFFER_ALLOCATION)) {
			throw( 3);
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

void Initialize(Local<Object> exports) {
	NODE_SET_METHOD(exports, "Method_BF_BakersGameEasy_GetBoard", Method_BF_BakersGameEasy_GetBoard);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)

}  // namespace Lambda_BakersGame
