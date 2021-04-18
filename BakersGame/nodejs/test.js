const addon = require( '../build/Release/addon.node');

// Create a sample board
console.log(addon.Method_BF_BakersGameEasy_GetBoard(100));
testBoard = JSON.parse( addon.Method_BF_BakersGameEasy_GetBoard(100));
console.log( testBoard);

// Make foundation arguments
foundations = [ "", "", "", "" ]
for( fndPos = 0 ; testBoard.foundations.length > fndPos; ++ fndPos) {
	if( testBoard.foundations[fndPos].suit == "clubs")
		foundations[0] = testBoard.foundations[fndPos].value;
	else if( testBoard.foundations[fndPos].suit == "diamonds")
		foundations[1] = testBoard.foundations[fndPos].value;
	else if( testBoard.foundations[fndPos].suit == "hearts")
		foundations[2] = testBoard.foundations[fndPos].value;
	else if( testBoard.foundations[fndPos].suit == "spades")
		foundations[3] = testBoard.foundations[fndPos].value;
};

// Solve the board
console.log( "----------");
console.log( addon.Method_BF_BakersGame_SolveBoard( "ST_BAKERS_GAME_EASY", foundations, testBoard.reserve, testBoard.tableau));

// Ask to solve an unsolvable board
console.log( "----------");
foundations = [ "", "", "", "" ]
testBoard = { 
"seed_value" : 100,
"foundations" : [
 { "suit" : "clubs" , "value" : "" },
 { "suit" : "diamonds" , "value" : "" },
 { "suit" : "hearts" , "value" : "" },
 { "suit" : "spades" , "value" : "" }
],
"reserve" : [ "", "", "", "" ],
"tableau" : [ 
[ "S8", "D7", "DA", "CA", "C6", "HJ", "S9", "D3" ],
[ "DK", "H5", "S5", "C7", "C8", "HK", "H4", "D4" ],
[ "SA", "D5", "C3", "HA", "DQ", "H7", "H9", "H0" ],
[ "S0", "H3", "SQ", "S7", "HQ", "CJ", "C4", "C9" ],
[ "H2", "SK", "C2", "D8", "DJ", "CK", "S4", "D9" ],
[ "D6", "C5", "D0", "H8", "CQ", "D2", "S2", "S6" ],
[ "C0", "S3", "SJ", "H6", "", "", "", "" ]
]
};
console.log( addon.Method_BF_BakersGame_SolveBoard( "ST_BAKERS_GAME_EASY", foundations, testBoard.reserve, testBoard.tableau));
