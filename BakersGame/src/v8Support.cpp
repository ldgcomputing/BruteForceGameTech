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
#include <string>

// Project includes
#include <v8Support.h>

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
