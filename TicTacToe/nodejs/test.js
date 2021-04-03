const addon = require( '../build/Release/addon.node');
try {
	data = "IHsgImJvYXJkIiA6ICItLS0tLS0tLS0iIH0g";
	buff = new Buffer.from( data, 'base64');
	text = buff.toString('ascii');
	console.log( text);
}
catch( e) {
	console.log( "Unable to atob");
	console.log( e);
}
console.log( addon.BF_TicTacToe("---------"));
