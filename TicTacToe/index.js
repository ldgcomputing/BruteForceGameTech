const addon = require( './build/Release/addon.node');

exports.handler =  function(event, context, callback) {

	// Start with a blank board value
	boardVal = "";
	body = "";

	// Extract the board value
	if( event.routeKey == "POST /TicTacToe") {
		try {
			console.log( "event.body = '" + event.body + "'");
			buff = new Buffer.from( event.body, 'base64');
			body = buff.toString('ascii');
			bodyObj = JSON.parse( body);
			boardVal = bodyObj.board;
		}
		catch( e) {
			console.log( "Failure to decode body: " + body);
			console.log( e);
		}
	}
	else {
		console.log( "Unknown routeKey value: " + event.routeKey);
		console.log( e);
	}

	console.log( "Decoded board value: " + boardVal);

	// Call the function
	output = addon.BF_TicTacToe(boardVal);
	outputJSON = { "errorcode" : 254, "errormsg" : "Unknown instantiation error" }
	try {
		outputJSON = JSON.parse( output);
	}
	catch( e) {
		outputJSON = { "errorcode" : 253, "errormsg" : "JSON parsing error", "detail" : e }
		console.log( e);
	}

	// Send the output
	callback( null, outputJSON);
	// return output;

}
