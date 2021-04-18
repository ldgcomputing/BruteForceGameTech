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
