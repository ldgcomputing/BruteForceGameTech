// Required includes
#include <stdlib.h>
#include <stdint.h>
#include <memory.h>
#include <string.h>
#include <time.h>

// Project includes
#include <CommonElements.h>
#include <BoardFunctions.h>

// Global constants
static const char *ERROR_MESSAGES [] = {
	"Not an error",
	"Invalid board size",
	"Unable to convert to JSON",
	"No arguments provided"
};
static const char MEMORY_ALLOCATION_FAILURE [] = "{ 'errorcode' : 255, 'errormsg' : 'Memory Allocation Failure' }";
static const int OUTPUT_BUFFER_ALLOCATION = 90000;

// Don't compile for Eclipse
#ifndef	COMPILING_FOR_NODE

// Node includes
#include <node.h>

namespace Lambda_TicTacToe {

// Using helpfuls
using tictactoe::S_TTT_BOARD;
using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::String;
using v8::Value;

void Method_BF_TicTacToe(const FunctionCallbackInfo<Value> &args) {

	// Get the program arguments
	Isolate *isolate = args.GetIsolate();

	// Attempt to allocate space for the return message
	char *pBuffer = new char [OUTPUT_BUFFER_ALLOCATION];
	if( (char *) 0x0 == pBuffer) {
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, (char*) MEMORY_ALLOCATION_FAILURE).ToLocalChecked());
		return;
	}

	// Wrap it
	try {

		// Was an argument provided
		if ((args.Length() >= 1) && (args[0]->IsString())) {

			// Extract the board
			unsigned char inputBoard[100 + 1];
			memset(inputBoard, 0x0, sizeof(inputBoard));
			Local<v8::String> argParamBoard = args[0].As<String>();
			int boardSize = argParamBoard->WriteOneByte(isolate, inputBoard, 0, 100);
			if (boardSize != 9) {
				throw(1);
			}

			// Convert the board into a starting board
			S_TTT_BOARD startingBoard;
			tictactoe::initializeBoardFromString( startingBoard, (const char *) inputBoard);

			// Now process that board
			uint8_t nextPlayer = tictactoe::getNextPlayer( startingBoard);
			tictactoe::processBoard( startingBoard, nextPlayer);

			// Convert to JSON output
			int nSize = convertBoardToJSON( startingBoard, pBuffer, OUTPUT_BUFFER_ALLOCATION);
			if( 0 >= nSize) throw(2);

		}

		else {
			throw(3);
		}

	}

	catch (const int e) {
		memset(pBuffer, 0x0, OUTPUT_BUFFER_ALLOCATION);
		sprintf( pBuffer, "{ \"errorcode\" : %d, \"errormsg\" : \"%s\" }", e, ERROR_MESSAGES[e]);
	}

	// Set the return value
	args.GetReturnValue().Set(String::NewFromUtf8(isolate, (char*) pBuffer).ToLocalChecked());

	// Free resources
	delete [] pBuffer;
	pBuffer = (char *) 0x0;

}

void Initialize(Local<Object> exports) {
	NODE_SET_METHOD(exports, "BF_TicTacToe", Method_BF_TicTacToe);
}

NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize)

}  // namespace Lambda_TicTacToe

#endif
