/*
 *  SolitaireBoard.cpp
 *  Solitaire Test
 *
 *  Created by Louis Gehrig on 11/8/10.
 *  Copyright 2010 Louis Gehrig. All rights reserved.
 *
 */

// Standard includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// STL includes
#include <algorithm>
#include <vector>

// Project includes
#include "SolitaireBoard.h"

// Sorting operators
bool operator<(const struct s_card &left, const struct s_card &right) {
	if (left.eSuit > right.eSuit)
		return (false);
	if (left.eSuit < right.eSuit)
		return (true);
	return (left.eValue < right.eValue);
}
bool operator<(const COMP_BOARD &left, const COMP_BOARD &right) {
	return (memcmp(&left, &right, sizeof(COMP_BOARD)));
}

//
// Helpful functions
//

void PrintMove(FILE *pOutput, const MOVE_T cTheMove) {

	// Constants
	const char *MOVE_VALUES[] = { "Tableau", "Wastepile", "Stockpile", "Foundation", "Reserve", "Invalid" };

	// Validate input
	if ((FILE*) 0x0 == pOutput)
		return;

	// Print the move
	fprintf(pOutput, "%s ", MOVE_VALUES[cTheMove.eMoveFrom]);
	fprintf(pOutput, "col %d row %d .... TO .... ", cTheMove.nFromCol, cTheMove.nFromRow);
	fprintf(pOutput, "%s ", MOVE_VALUES[cTheMove.eMoveTo]);
	fprintf(pOutput, "col %d row %d", cTheMove.nToCol, cTheMove.nToRow);

}

bool convertCardToText(CARD_T card, char *pBuffer, const size_t bufSize) {
	if ((char*) 0x0 == pBuffer)
		return (false);
	if (3 > bufSize)
		return (false);
	switch (card.eSuit) {
	case CS_CLUBS:
		pBuffer[0] = 'C';
		break;
	case CS_DIAMONDS:
		pBuffer[0] = 'D';
		break;
	case CS_HEARTS:
		pBuffer[0] = 'H';
		break;
	case CS_SPADES:
		pBuffer[0] = 'S';
		break;
	default:
		return (false);
	}
	switch (card.eValue) {
	case CV_ACE:
		pBuffer[1] = 'A';
		break;
	case CV_TWO:
		pBuffer[1] = '2';
		break;
	case CV_THREE:
		pBuffer[1] = '3';
		break;
	case CV_FOUR:
		pBuffer[1] = '4';
		break;
	case CV_FIVE:
		pBuffer[1] = '5';
		break;
	case CV_SIX:
		pBuffer[1] = '6';
		break;
	case CV_SEVEN:
		pBuffer[1] = '7';
		break;
	case CV_EIGHT:
		pBuffer[1] = '8';
		break;
	case CV_NINE:
		pBuffer[1] = '9';
		break;
	case CV_TEN:
		pBuffer[1] = '0';
		break;
	case CV_JACK:
		pBuffer[1] = 'J';
		break;
	case CV_QUEEN:
		pBuffer[1] = 'Q';
		break;
	case CV_KING:
		pBuffer[1] = 'K';
		break;
	default:
		return (false);
	}
	pBuffer[2] = 0x0;
	return (true);
}

bool convertCardValueToText(CARDVALUES_T cardValue, char *pBuffer, const size_t bufSize) {
	if ((char*) 0x0 == pBuffer)
		return (false);
	if (2 > bufSize)
		return (false);
	switch (cardValue) {
	case CV_ACE:
		pBuffer[0] = 'A';
		break;
	case CV_TWO:
		pBuffer[0] = '2';
		break;
	case CV_THREE:
		pBuffer[0] = '3';
		break;
	case CV_FOUR:
		pBuffer[0] = '4';
		break;
	case CV_FIVE:
		pBuffer[0] = '5';
		break;
	case CV_SIX:
		pBuffer[0] = '6';
		break;
	case CV_SEVEN:
		pBuffer[0] = '7';
		break;
	case CV_EIGHT:
		pBuffer[0] = '8';
		break;
	case CV_NINE:
		pBuffer[0] = '9';
		break;
	case CV_TEN:
		pBuffer[0] = '0';
		break;
	case CV_JACK:
		pBuffer[0] = 'J';
		break;
	case CV_QUEEN:
		pBuffer[0] = 'Q';
		break;
	case CV_KING:
		pBuffer[0] = 'K';
		break;
	default:
		return (false);
	}
	pBuffer[1] = 0x0;
	return (true);

}

//
// Class CSolitaireBoard
//

// Construction
CSolitaireBoard::CSolitaireBoard() {

	m_eSolType = ST_INVALID;

}

// Destruction
CSolitaireBoard::~CSolitaireBoard() {

}

// Accessors
SOLITAIRE_T CSolitaireBoard::GetSolType() const {
	return (m_eSolType);
}

void CSolitaireBoard::SetSolType(const SOLITAIRE_T eType) {
	m_eSolType = eType;
}

// Create a board
CSolitaireBoard* CSolitaireBoard::CreateBoard(const SOLITAIRE_T eType) {

	// Variables
	CSolitaireBoard *pRetValue = (CSolitaireBoard*) 0x0;

	// Dispatch
	switch (eType) {

	case ST_BAKERS_GAME:
	case ST_BAKERS_GAME_TEST:
		pRetValue = new CSB_BakersGame(false);
		break;

	case ST_BAKERS_GAME_EASY:
	case ST_BAKERS_GAME_EASY_TEST:
		pRetValue = new CSB_BakersGame(true);
		break;

	case ST_INVALID:
		pRetValue = new CSB_InvalidBoard();
		break;

	default:
		// Hmmm ... how did we get here?
	{
		char strMessage[250 + 1];
		sprintf(strMessage, "Encountered solitaire type %d", eType);
		// DebugStr( (const unsigned char *) strMessage) ;
		pRetValue = (CSolitaireBoard*) 0x0;
	}
		break;

	}

	// Set necessary type and initialize it
	if ((CSolitaireBoard*) 0x0 != pRetValue) {

		pRetValue->SetSolType(eType);
		switch (eType) {

		case ST_BAKERS_GAME_TEST: {
			CSB_BakersGame *pBGET = (CSB_BakersGame*) pRetValue;
			pBGET->ClearBoard();

			/*
			 pBGET -> m_acTableau [0][0] =	CARD_T( CS_DIAMONDS,	CV_THREE) ;
			 pBGET -> m_acTableau [1][0] =	CARD_T( CS_HEARTS,		CV_KING) ;
			 pBGET -> m_acTableau [2][0] =	CARD_T( CS_SPADES,		CV_TWO) ;
			 pBGET -> m_acTableau [3][0] =	CARD_T( CS_SPADES,		CV_JACK) ;
			 pBGET -> m_acTableau [4][0] =	CARD_T( CS_DIAMONDS,	CV_TEN) ;
			 pBGET -> m_acTableau [5][0] =	CARD_T( CS_CLUBS,		CV_FOUR) ;
			 pBGET -> m_acTableau [6][0] =	CARD_T( CS_CLUBS,		CV_TWO) ;
			 pBGET -> m_acTableau [7][0] =	CARD_T( CS_CLUBS,		CV_THREE) ;

			 pBGET -> m_acTableau [0][1] =	CARD_T( CS_HEARTS,		CV_TWO) ;
			 pBGET -> m_acTableau [1][1] =	CARD_T( CS_CLUBS,		CV_TEN) ;
			 pBGET -> m_acTableau [2][1] =	CARD_T( CS_HEARTS,		CV_SEVEN) ;
			 pBGET -> m_acTableau [3][1] =	CARD_T( CS_SPADES,		CV_ACE) ;
			 pBGET -> m_acTableau [4][1] =	CARD_T( CS_CLUBS,		CV_FIVE) ;
			 pBGET -> m_acTableau [5][1] =	CARD_T( CS_CLUBS,		CV_JACK) ;
			 pBGET -> m_acTableau [6][1] =	CARD_T( CS_HEARTS,		CV_JACK) ;
			 pBGET -> m_acTableau [7][1] =	CARD_T( CS_DIAMONDS,	CV_SIX) ;

			 pBGET -> m_acTableau [0][2] =	CARD_T( CS_DIAMONDS,	CV_SEVEN) ;
			 pBGET -> m_acTableau [1][2] =	CARD_T( CS_SPADES,		CV_QUEEN) ;
			 pBGET -> m_acTableau [2][2] =	CARD_T( CS_CLUBS,		CV_SIX) ;
			 pBGET -> m_acTableau [3][2] =	CARD_T( CS_DIAMONDS,	CV_QUEEN) ;
			 pBGET -> m_acTableau [4][2] =	CARD_T( CS_SPADES,		CV_KING) ;
			 pBGET -> m_acTableau [5][2] =	CARD_T( CS_CLUBS,		CV_NINE) ;
			 pBGET -> m_acTableau [6][2] =	CARD_T( CS_HEARTS,		CV_TEN) ;
			 pBGET -> m_acTableau [7][2] =	CARD_T( CS_HEARTS,		CV_ACE) ;

			 pBGET -> m_acTableau [0][3] =	CARD_T( CS_HEARTS,		CV_THREE) ;
			 pBGET -> m_acTableau [1][3] =	CARD_T( CS_DIAMONDS,	CV_KING) ;
			 pBGET -> m_acTableau [2][3] =	CARD_T( CS_HEARTS,		CV_NINE) ;
			 pBGET -> m_acTableau [3][3] =	CARD_T( CS_HEARTS,		CV_SIX) ;
			 pBGET -> m_acTableau [4][3] =	CARD_T( CS_SPADES,		CV_FIVE) ;
			 pBGET -> m_acTableau [5][3] =	CARD_T( CS_CLUBS,		CV_ACE) ;
			 pBGET -> m_acTableau [6][3] =	CARD_T( CS_CLUBS,		CV_QUEEN) ;
			 pBGET -> m_acTableau [7][3] =	CARD_T( CS_HEARTS,		CV_QUEEN) ;

			 pBGET -> m_acTableau [0][4] =	CARD_T( CS_DIAMONDS,	CV_ACE) ;
			 pBGET -> m_acTableau [1][4] =	CARD_T( CS_SPADES,		CV_NINE) ;
			 pBGET -> m_acTableau [2][4] =	CARD_T( CS_DIAMONDS,	CV_FOUR) ;
			 pBGET -> m_acTableau [3][4] =	CARD_T( CS_CLUBS,		CV_EIGHT) ;
			 pBGET -> m_acTableau [4][4] =	CARD_T( CS_SPADES,		CV_FOUR) ;
			 pBGET -> m_acTableau [5][4] =	CARD_T( CS_SPADES,		CV_TEN) ;
			 pBGET -> m_acTableau [6][4] =	CARD_T( CS_SPADES,		CV_SEVEN) ;
			 pBGET -> m_acTableau [7][4] =	CARD_T( CS_CLUBS,		CV_SEVEN) ;

			 pBGET -> m_acTableau [0][5] =	CARD_T( CS_DIAMONDS,	CV_FIVE) ;
			 pBGET -> m_acTableau [1][5] =	CARD_T( CS_SPADES,		CV_EIGHT) ;
			 pBGET -> m_acTableau [2][5] =	CARD_T( CS_SPADES,		CV_SIX) ;
			 pBGET -> m_acTableau [3][5] =	CARD_T( CS_HEARTS,		CV_FIVE) ;
			 pBGET -> m_acTableau [4][5] =	CARD_T( CS_DIAMONDS,	CV_EIGHT) ;
			 pBGET -> m_acTableau [5][5] =	CARD_T( CS_HEARTS,		CV_EIGHT) ;
			 pBGET -> m_acTableau [6][5] =	CARD_T( CS_DIAMONDS,	CV_TWO) ;
			 pBGET -> m_acTableau [7][5] =	CARD_T( CS_HEARTS,		CV_FOUR) ;

			 pBGET -> m_acTableau [0][6] =	CARD_T( CS_SPADES,		CV_THREE) ;
			 pBGET -> m_acTableau [1][6] =	CARD_T( CS_CLUBS,		CV_KING) ;
			 pBGET -> m_acTableau [2][6] =	CARD_T( CS_DIAMONDS,	CV_NINE) ;
			 pBGET -> m_acTableau [3][6] =	CARD_T( CS_DIAMONDS,	CV_JACK) ;
			 */

			pBGET->m_acTableau[0][0] = CARD_T(CS_DIAMONDS, CV_EIGHT);
			pBGET->m_acTableau[1][0] = CARD_T(CS_HEARTS, CV_KING);
			pBGET->m_acTableau[2][0] = CARD_T(CS_SPADES, CV_TWO);
			pBGET->m_acTableau[3][0] = CARD_T(CS_SPADES, CV_JACK);
			pBGET->m_acTableau[4][0] = CARD_T(CS_DIAMONDS, CV_TEN);
			pBGET->m_acTableau[5][0] = CARD_T(CS_CLUBS, CV_FOUR);
			pBGET->m_acTableau[6][0] = CARD_T(CS_CLUBS, CV_TWO);
			pBGET->m_acTableau[7][0] = CARD_T(CS_CLUBS, CV_THREE);

			pBGET->m_acTableau[0][1] = CARD_T(CS_DIAMONDS, CV_SEVEN);
			pBGET->m_acTableau[1][1] = CARD_T(CS_CLUBS, CV_TEN);
			pBGET->m_acTableau[2][1] = CARD_T(CS_HEARTS, CV_SEVEN);
			pBGET->m_acTableau[3][1] = CARD_T(CS_SPADES, CV_ACE);
			pBGET->m_acTableau[4][1] = CARD_T(CS_CLUBS, CV_FIVE);
			pBGET->m_acTableau[5][1] = CARD_T(CS_CLUBS, CV_JACK);
			pBGET->m_acTableau[6][1] = CARD_T(CS_HEARTS, CV_JACK);
			pBGET->m_acTableau[7][1] = CARD_T(CS_DIAMONDS, CV_SIX);

			//pBGET -> m_acTableau [0][2] =	CARD_T( CS_DIAMONDS,	CV_SEVEN) ;
			pBGET->m_acTableau[1][2] = CARD_T(CS_SPADES, CV_QUEEN);
			pBGET->m_acTableau[2][2] = CARD_T(CS_CLUBS, CV_SIX);
			pBGET->m_acTableau[3][2] = CARD_T(CS_DIAMONDS, CV_QUEEN);
			pBGET->m_acTableau[4][2] = CARD_T(CS_SPADES, CV_KING);
			pBGET->m_acTableau[5][2] = CARD_T(CS_CLUBS, CV_NINE);
			pBGET->m_acTableau[6][2] = CARD_T(CS_HEARTS, CV_TEN);
			//pBGET -> m_acTableau [7][2] =	CARD_T( CS_HEARTS,		CV_ACE) ;

			//pBGET -> m_acTableau [0][3] =	CARD_T( CS_HEARTS,		CV_THREE) ;
			pBGET->m_acTableau[1][3] = CARD_T(CS_DIAMONDS, CV_KING);
			pBGET->m_acTableau[2][3] = CARD_T(CS_HEARTS, CV_NINE);
			pBGET->m_acTableau[3][3] = CARD_T(CS_HEARTS, CV_SIX);
			pBGET->m_acTableau[4][3] = CARD_T(CS_SPADES, CV_FIVE);
			pBGET->m_acTableau[5][3] = CARD_T(CS_CLUBS, CV_ACE);
			pBGET->m_acTableau[6][3] = CARD_T(CS_CLUBS, CV_QUEEN);
			//pBGET -> m_acTableau [7][3] =	CARD_T( CS_HEARTS,		CV_QUEEN) ;

			//pBGET -> m_acTableau [0][4] =	CARD_T( CS_DIAMONDS,	CV_ACE) ;
			pBGET->m_acTableau[1][4] = CARD_T(CS_SPADES, CV_NINE);
			//pBGET -> m_acTableau [2][4] =	CARD_T( CS_DIAMONDS,	CV_FOUR) ;
			pBGET->m_acTableau[3][4] = CARD_T(CS_CLUBS, CV_EIGHT);
			pBGET->m_acTableau[4][4] = CARD_T(CS_SPADES, CV_FOUR);
			pBGET->m_acTableau[5][4] = CARD_T(CS_SPADES, CV_TEN);
			pBGET->m_acTableau[6][4] = CARD_T(CS_SPADES, CV_SEVEN);
			//pBGET -> m_acTableau [7][4] =	CARD_T( CS_CLUBS,		CV_SEVEN) ;

			//pBGET -> m_acTableau [0][5] =	CARD_T( CS_DIAMONDS,	CV_FIVE) ;
			pBGET->m_acTableau[1][5] = CARD_T(CS_SPADES, CV_EIGHT);
			//pBGET -> m_acTableau [2][5] =	CARD_T( CS_SPADES,		CV_SIX) ;
			pBGET->m_acTableau[3][5] = CARD_T(CS_HEARTS, CV_FIVE);
			pBGET->m_acTableau[4][5] = CARD_T(CS_SPADES, CV_THREE);
			pBGET->m_acTableau[5][5] = CARD_T(CS_HEARTS, CV_EIGHT);
			pBGET->m_acTableau[6][5] = CARD_T(CS_SPADES, CV_SIX);
			//pBGET -> m_acTableau [7][5] =	CARD_T( CS_HEARTS,		CV_FOUR) ;

			//pBGET -> m_acTableau [0][6] =	CARD_T( CS_SPADES,		CV_THREE) ;
			pBGET->m_acTableau[1][6] = CARD_T(CS_CLUBS, CV_KING);
			//pBGET -> m_acTableau [2][6] =	CARD_T( CS_DIAMONDS,	CV_NINE) ;
			pBGET->m_acTableau[3][6] = CARD_T(CS_DIAMONDS, CV_JACK);

			pBGET->m_acReserve[0] = CARD_T(CS_DIAMONDS, CV_FIVE);
			pBGET->m_acReserve[1] = CARD_T(CS_CLUBS, CV_SEVEN);
			pBGET->m_acReserve[2] = CARD_T(CS_HEARTS, CV_QUEEN);
			pBGET->m_acReserve[3] = CARD_T(CS_DIAMONDS, CV_NINE);

			pBGET->m_acFoundationHighest[CS_CLUBS] = CV_EMPTY;
			pBGET->m_acFoundationHighest[CS_DIAMONDS] = CV_FOUR;
			pBGET->m_acFoundationHighest[CS_HEARTS] = CV_FOUR;
			pBGET->m_acFoundationHighest[CS_SPADES] = CV_EMPTY;

			pBGET->m_bIsPlayable = true;
			pBGET->DumpBoardToDebug();
		}
			break;

		case ST_BAKERS_GAME_EASY_TEST: {
			CSB_BakersGame *pBGET = (CSB_BakersGame*) pRetValue;
			pBGET->ClearBoard();
			/*
			 pBGET -> m_acTableau [0][0] = CARD_T( CS_CLUBS, CV_KING) ;
			 pBGET -> m_acTableau [0][1] = CARD_T( CS_CLUBS, CV_QUEEN) ;
			 pBGET -> m_acTableau [0][2] = CARD_T( CS_CLUBS, CV_JACK) ;
			 pBGET -> m_acTableau [0][3] = CARD_T( CS_CLUBS, CV_NINE) ;
			 pBGET -> m_acTableau [0][4] = CARD_T( CS_CLUBS, CV_EIGHT) ;
			 pBGET -> m_acTableau [0][5] = CARD_T( CS_CLUBS, CV_SEVEN) ;
			 pBGET -> m_acTableau [0][6] = CARD_T( CS_CLUBS, CV_SIX) ;
			 pBGET -> m_acTableau [1][0] = CARD_T( CS_CLUBS, CV_FIVE) ;
			 pBGET -> m_acTableau [1][1] = CARD_T( CS_CLUBS, CV_FOUR) ;
			 pBGET -> m_acTableau [1][2] = CARD_T( CS_CLUBS, CV_THREE) ;
			 pBGET -> m_acTableau [1][3] = CARD_T( CS_CLUBS, CV_TWO) ;
			 pBGET -> m_acTableau [1][4] = CARD_T( CS_CLUBS, CV_ACE) ;
			 pBGET -> m_acTableau [1][5] = CARD_T( CS_CLUBS, CV_TEN) ;
			 pBGET -> m_acTableau [1][6] = CARD_T( CS_HEARTS, CV_QUEEN) ;
			 pBGET -> m_acTableau [1][7] = CARD_T( CS_HEARTS, CV_KING) ;
			 pBGET -> m_acFoundationHighest [CS_DIAMONDS] = CV_KING ;
			 pBGET -> m_acFoundationHighest [CS_HEARTS] = CV_JACK ;
			 pBGET -> m_acFoundationHighest [CS_SPADES] = CV_KING ;
			 */
			/*
			 pBGET -> m_acTableau [0][0] =	CARD_T( CS_CLUBS, CV_KING) ;
			 pBGET -> m_acTableau [1][0] =	CARD_T( CS_HEARTS, CV_FOUR) ;
			 pBGET -> m_acTableau [2][0] =	CARD_T( CS_CLUBS, CV_SEVEN) ;
			 pBGET -> m_acTableau [3][0] =	CARD_T( CS_SPADES, CV_THREE) ;
			 pBGET -> m_acTableau [4][0] =	CARD_T( CS_SPADES, CV_NINE) ;
			 pBGET -> m_acTableau [5][0] =	CARD_T( CS_DIAMONDS, CV_QUEEN) ;
			 pBGET -> m_acTableau [6][0] =	CARD_T( CS_DIAMONDS, CV_THREE) ;
			 pBGET -> m_acTableau [7][0] =	CARD_T( CS_CLUBS, CV_SIX) ;
			 pBGET -> m_acTableau [0][1] =	CARD_T( CS_CLUBS, CV_NINE) ;
			 pBGET -> m_acTableau [1][1] =	CARD_T( CS_SPADES, CV_QUEEN) ;
			 pBGET -> m_acTableau [2][1] =	CARD_T( CS_SPADES, CV_TEN) ;
			 pBGET -> m_acTableau [3][1] =	CARD_T( CS_HEARTS, CV_NINE) ;
			 pBGET -> m_acTableau [4][1] =	CARD_T( CS_SPADES, CV_JACK) ;
			 pBGET -> m_acTableau [5][1] =	CARD_T( CS_CLUBS, CV_TEN) ;
			 pBGET -> m_acTableau [6][1] =	CARD_T( CS_CLUBS, CV_THREE) ;
			 pBGET -> m_acTableau [7][1] =	CARD_T( CS_HEARTS, CV_EIGHT) ;
			 pBGET -> m_acTableau [0][2] =	CARD_T( CS_SPADES, CV_FOUR) ;
			 pBGET -> m_acTableau [1][2] =	CARD_T( CS_HEARTS, CV_THREE) ;
			 pBGET -> m_acTableau [2][2] =	CARD_T( CS_CLUBS, CV_TWO) ;
			 pBGET -> m_acTableau [3][2] =	CARD_T( CS_CLUBS, CV_EIGHT) ;
			 pBGET -> m_acTableau [4][2] =	CARD_T( CS_HEARTS, CV_FIVE) ;
			 pBGET -> m_acTableau [5][2] =	CARD_T( CS_DIAMONDS, CV_ACE) ;
			 pBGET -> m_acTableau [6][2] =	CARD_T( CS_DIAMONDS, CV_JACK) ;
			 pBGET -> m_acTableau [7][2] =	CARD_T( CS_SPADES, CV_SIX) ;
			 pBGET -> m_acTableau [0][3] =	CARD_T( CS_DIAMONDS, CV_EIGHT) ;
			 pBGET -> m_acTableau [1][3] =	CARD_T( CS_SPADES, CV_FIVE) ;
			 pBGET -> m_acTableau [2][3] =	CARD_T( CS_DIAMONDS, CV_NINE) ;
			 pBGET -> m_acTableau [3][3] =	CARD_T( CS_SPADES, CV_EIGHT) ;
			 pBGET -> m_acTableau [4][3] =	CARD_T( CS_HEARTS, CV_SEVEN) ;
			 pBGET -> m_acTableau [5][3] =	CARD_T( CS_SPADES, CV_TWO) ;
			 pBGET -> m_acTableau [6][3] =	CARD_T( CS_DIAMONDS, CV_TWO) ;
			 pBGET -> m_acTableau [7][3] =	CARD_T( CS_HEARTS, CV_TEN) ;
			 pBGET -> m_acTableau [0][4] =	CARD_T( CS_HEARTS, CV_QUEEN) ;
			 pBGET -> m_acTableau [1][4] =	CARD_T( CS_CLUBS, CV_ACE) ;
			 pBGET -> m_acTableau [2][4] =	CARD_T( CS_SPADES, CV_SEVEN) ;
			 pBGET -> m_acTableau [3][4] =	CARD_T( CS_DIAMONDS, CV_SEVEN) ;
			 pBGET -> m_acTableau [4][4] =	CARD_T( CS_DIAMONDS, CV_FIVE) ;
			 pBGET -> m_acTableau [5][4] =	CARD_T( CS_HEARTS, CV_TWO) ;
			 pBGET -> m_acTableau [6][4] =	CARD_T( CS_HEARTS, CV_SIX) ;
			 pBGET -> m_acTableau [7][4] =	CARD_T( CS_CLUBS, CV_FOUR) ;
			 pBGET -> m_acTableau [0][5] =	CARD_T( CS_DIAMONDS, CV_SIX) ;
			 pBGET -> m_acTableau [1][5] =	CARD_T( CS_DIAMONDS, CV_FOUR) ;
			 pBGET -> m_acTableau [2][5] =	CARD_T( CS_HEARTS, CV_JACK) ;
			 pBGET -> m_acTableau [3][5] =	CARD_T( CS_CLUBS, CV_QUEEN) ;
			 pBGET -> m_acTableau [4][5] =	CARD_T( CS_HEARTS, CV_ACE) ;
			 pBGET -> m_acTableau [5][5] =	CARD_T( CS_DIAMONDS, CV_TEN) ;
			 pBGET -> m_acTableau [6][5] =	CARD_T( CS_HEARTS, CV_KING) ;
			 pBGET -> m_acTableau [7][5] =	CARD_T( CS_SPADES, CV_KING) ;
			 pBGET -> m_acTableau [0][6] =	CARD_T( CS_CLUBS, CV_FIVE) ;
			 pBGET -> m_acTableau [1][6] =	CARD_T( CS_DIAMONDS, CV_KING) ;
			 pBGET -> m_acTableau [2][6] =	CARD_T( CS_CLUBS, CV_JACK) ;
			 pBGET -> m_acTableau [3][6] =	CARD_T( CS_SPADES, CV_ACE) ;
			 */
			//pBGET -> m_acTableau [0][0] =	CARD_T( CS_DIAMONDS,	CV_EIGHT) ;
			pBGET->m_acTableau[1][0] = CARD_T(CS_DIAMONDS, CV_EIGHT);
			pBGET->m_acTableau[2][0] = CARD_T(CS_SPADES, CV_FIVE);
			pBGET->m_acTableau[3][0] = CARD_T(CS_SPADES, CV_SIX);
			pBGET->m_acTableau[4][0] = CARD_T(CS_DIAMONDS, CV_SIX);
			pBGET->m_acTableau[5][0] = CARD_T(CS_HEARTS, CV_TEN);
			pBGET->m_acTableau[6][0] = CARD_T(CS_DIAMONDS, CV_TEN);
			pBGET->m_acTableau[7][0] = CARD_T(CS_CLUBS, CV_TEN);

			//pBGET -> m_acTableau [0][1] =	CARD_T( CS_CLUBS,		CV_SEVEN) ;
			pBGET->m_acTableau[1][1] = CARD_T(CS_CLUBS, CV_FOUR);
			pBGET->m_acTableau[2][1] = CARD_T(CS_DIAMONDS, CV_TWO);
			pBGET->m_acTableau[3][1] = CARD_T(CS_CLUBS, CV_QUEEN);
			pBGET->m_acTableau[4][1] = CARD_T(CS_HEARTS, CV_NINE);
			pBGET->m_acTableau[5][1] = CARD_T(CS_SPADES, CV_EIGHT);
			pBGET->m_acTableau[6][1] = CARD_T(CS_DIAMONDS, CV_SEVEN);
			pBGET->m_acTableau[7][1] = CARD_T(CS_SPADES, CV_SEVEN);

			//pBGET -> m_acTableau [0][2] =	CARD_T( CS_DIAMONDS,	CV_SEVEN) ;
			pBGET->m_acTableau[1][2] = CARD_T(CS_HEARTS, CV_THREE);
			pBGET->m_acTableau[2][2] = CARD_T(CS_CLUBS, CV_SIX);
			pBGET->m_acTableau[3][2] = CARD_T(CS_HEARTS, CV_KING);
			pBGET->m_acTableau[4][2] = CARD_T(CS_HEARTS, CV_JACK);
			pBGET->m_acTableau[5][2] = CARD_T(CS_CLUBS, CV_KING);
			pBGET->m_acTableau[6][2] = CARD_T(CS_HEARTS, CV_EIGHT);
			pBGET->m_acTableau[7][2] = CARD_T(CS_HEARTS, CV_SEVEN);

			//pBGET -> m_acTableau [0][3] =	CARD_T( CS_HEARTS,		CV_THREE) ;
			pBGET->m_acTableau[1][3] = CARD_T(CS_SPADES, CV_QUEEN);
			pBGET->m_acTableau[2][3] = CARD_T(CS_SPADES, CV_THREE);
			pBGET->m_acTableau[3][3] = CARD_T(CS_CLUBS, CV_JACK);
			pBGET->m_acTableau[4][3] = CARD_T(CS_DIAMONDS, CV_NINE);
			pBGET->m_acTableau[5][3] = CARD_T(CS_SPADES, CV_FOUR);
			//pBGET -> m_acTableau [6][3] =	CARD_T( CS_CLUBS,		CV_QUEEN) ;
			pBGET->m_acTableau[7][3] = CARD_T(CS_HEARTS, CV_SIX);

			//pBGET -> m_acTableau [0][4] =	CARD_T( CS_DIAMONDS,	CV_ACE) ;
			pBGET->m_acTableau[1][4] = CARD_T(CS_HEARTS, CV_QUEEN);
			pBGET->m_acTableau[2][4] = CARD_T(CS_SPADES, CV_NINE);
			pBGET->m_acTableau[3][4] = CARD_T(CS_SPADES, CV_TWO);
			pBGET->m_acTableau[4][4] = CARD_T(CS_DIAMONDS, CV_KING);
			pBGET->m_acTableau[5][4] = CARD_T(CS_DIAMONDS, CV_FIVE);
			//pBGET -> m_acTableau [6][4] =	CARD_T( CS_SPADES,		CV_SEVEN) ;
			pBGET->m_acTableau[7][4] = CARD_T(CS_HEARTS, CV_FIVE);

			//pBGET -> m_acTableau [0][5] =	CARD_T( CS_DIAMONDS,	CV_FIVE) ;
			pBGET->m_acTableau[1][5] = CARD_T(CS_HEARTS, CV_FOUR);
			//pBGET -> m_acTableau [2][5] =	CARD_T( CS_SPADES,		CV_SIX) ;
			pBGET->m_acTableau[3][5] = CARD_T(CS_CLUBS, CV_NINE);
			pBGET->m_acTableau[4][5] = CARD_T(CS_DIAMONDS, CV_QUEEN);
			pBGET->m_acTableau[5][5] = CARD_T(CS_DIAMONDS, CV_FOUR);
			//pBGET -> m_acTableau [6][5] =	CARD_T( CS_SPADES,		CV_SIX) ;
			//pBGET -> m_acTableau [7][5] =	CARD_T( CS_HEARTS,		CV_FOUR) ;

			//pBGET -> m_acTableau [0][6] =	CARD_T( CS_SPADES,		CV_THREE) ;
			pBGET->m_acTableau[1][6] = CARD_T(CS_SPADES, CV_TEN);
			//pBGET -> m_acTableau [2][6] =	CARD_T( CS_DIAMONDS,	CV_NINE) ;
			pBGET->m_acTableau[3][6] = CARD_T(CS_CLUBS, CV_EIGHT);
			pBGET->m_acTableau[4][6] = CARD_T(CS_DIAMONDS, CV_JACK);
			pBGET->m_acTableau[5][6] = CARD_T(CS_DIAMONDS, CV_THREE);
			//pBGET -> m_acTableau [6][6] =	CARD_T( CS_SPADES,		CV_SIX) ;
			//pBGET -> m_acTableau [7][6] =	CARD_T( CS_HEARTS,		CV_FOUR) ;

			//pBGET -> m_acTableau [0][7] =	CARD_T( CS_SPADES,		CV_THREE) ;
			//pBGET -> m_acTableau [1][7] =	CARD_T( CS_SPADES,		CV_TEN) ;
			//pBGET -> m_acTableau [2][7] =	CARD_T( CS_DIAMONDS,	CV_NINE) ;
			pBGET->m_acTableau[3][7] = CARD_T(CS_CLUBS, CV_SEVEN);
			//pBGET -> m_acTableau [4][7] =	CARD_T( CS_DIAMONDS,	CV_JACK) ;
			//pBGET -> m_acTableau [5][7] =	CARD_T( CS_DIAMONDS,	CV_THREE) ;
			//pBGET -> m_acTableau [6][7] =	CARD_T( CS_SPADES,		CV_SIX) ;
			//pBGET -> m_acTableau [7][7] =	CARD_T( CS_HEARTS,		CV_FOUR) ;

			pBGET->m_acReserve[0] = CARD_T(CS_CLUBS, CV_FIVE);
			pBGET->m_acReserve[1] = CARD_T(CS_SPADES, CV_KING);
			pBGET->m_acReserve[2] = CARD_T(CS_SPADES, CV_JACK);
			//pBGET -> m_acReserve [3] =	CARD_T( CS_DIAMONDS,	CV_NINE) ;

			pBGET->m_acFoundationHighest[CS_CLUBS] = CV_THREE;
			pBGET->m_acFoundationHighest[CS_DIAMONDS] = CV_ACE;
			pBGET->m_acFoundationHighest[CS_HEARTS] = CV_TWO;
			pBGET->m_acFoundationHighest[CS_SPADES] = CV_ACE;

			pBGET->m_bIsPlayable = true;
			pBGET->DumpBoardToDebug();

		}
			break;

		default:
			pRetValue->InitRandomBoard();
			break;
		}

	}

	// And return
	return (pRetValue);

}

// Create a board from a copy
CSolitaireBoard* CSolitaireBoard::CreateBoardCopy(const CSolitaireBoard *pBoard) {

	// Variables
	CSolitaireBoard *pRetValue = (CSolitaireBoard*) 0x0;

	// Validate input
	if ((const CSolitaireBoard*) 0x0 == pBoard)
		return ((CSolitaireBoard*) 0x0);

	// Dispatch
	switch (pBoard->GetSolType()) {

	case ST_BAKERS_GAME:
	case ST_BAKERS_GAME_TEST:
		pRetValue = new CSB_BakersGame(false);
		break;

	case ST_BAKERS_GAME_EASY:
	case ST_BAKERS_GAME_EASY_TEST:
		pRetValue = new CSB_BakersGame(true);
		break;

	case ST_INVALID:
		pRetValue = new CSB_InvalidBoard();
		break;

	default:
		// Hmmm ... how did we get here?
	{
		char strMessage[250 + 1];
		sprintf(strMessage, "Encountered solitaire type %d", pBoard->GetSolType());
		// DebugStr( (const unsigned char *) strMessage) ;
		pRetValue = (CSolitaireBoard*) 0x0;
	}
		break;

	}

	// Set necessary type and initialize it
	if ((CSolitaireBoard*) 0x0 != pRetValue) {

		pRetValue->SetSolType(pBoard->GetSolType());
		pRetValue->CopyBoard(pBoard);

	}

	// And return
	return (pRetValue);

}

// Copy the board
bool CSolitaireBoard::CopyBoard(const CSolitaireBoard *pBase) {

	// Validate input
	if ((const CSolitaireBoard*) 0x0 == pBase)
		return (false);
	if (pBase == this)
		return (false);

	// Verify the boards are the same type
	if (GetSolType() != pBase->GetSolType())
		return (false);

	// Copy the information, then return
	return (true);

}

//
// The Invalid board
//

// Auto complete
MOVE_T CSB_InvalidBoard::AutoComplete() const {

	// Variables
	MOVE_T cInvalidMove;

	// Fill it in
	cInvalidMove.eMoveFrom = cInvalidMove.eMoveTo = MP_INVALID;
	cInvalidMove.nFromCol = cInvalidMove.nFromRow = cInvalidMove.nToCol = cInvalidMove.nToRow = 0x0;

	// And done
	return (cInvalidMove);

}

//
// The Bakers Game board
//

// Construction
CSB_BakersGame::CSB_BakersGame(const bool bModeEasy) :
		m_bIsPlayable(false), m_bModeEasy(bModeEasy) {

}

// Destruction
CSB_BakersGame::~CSB_BakersGame() {

}

// Debugging aid
void CSB_BakersGame::DumpBoardToDebug() {

	// Constants
	const char CARD_SUIT_CHAR[] = "CDHS ";
	const char CARD_VALUE_CHAR[] = " A234567890JQK ";

	// Variables
	bool bNotInvalid = true;
	CARD_T cCurCard;
	char strOutputLine[500 + 1];
	int nLoop, nRow;

	// Dump foundation info
	sprintf(strOutputLine, "Foundation: HEARTS %c, SPADES %c, CLUBS %c, DIAMONDS %c",
			CARD_VALUE_CHAR[GetFoundationCardValue(CS_HEARTS)], CARD_VALUE_CHAR[GetFoundationCardValue(CS_SPADES)],
			CARD_VALUE_CHAR[GetFoundationCardValue(CS_CLUBS)], CARD_VALUE_CHAR[GetFoundationCardValue(CS_DIAMONDS)]);
#ifdef	ACTIVE_LOGGING
	fprintf( stderr, "%s\n", strOutputLine) ;
#endif

	// Dump the reserve info
	for (nLoop = NumReserveSpaces() - 1; 0 <= nLoop; --nLoop) {
		cCurCard = GetReserveCard(nLoop);
		sprintf(strOutputLine, "Reserve %d, %c%c", nLoop, CARD_SUIT_CHAR[cCurCard.eSuit],
				CARD_VALUE_CHAR[cCurCard.eValue]);
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "%s\n", strOutputLine) ;
#endif
	}

	// Dump the rows
	for (nRow = 0; bNotInvalid && (nRow < NumRows()); ++nRow) {
		memset(strOutputLine, 0x0, sizeof(strOutputLine));
		for (bNotInvalid = false, nLoop = 0; nLoop < NumCols(); ++nLoop) {
			cCurCard = GetTableauCardAt(nLoop, nRow);
			if (CS_INVALID == cCurCard.eSuit) {
				strncat(strOutputLine, "   ", 3);
			} else {
				strncat(strOutputLine, CARD_SUIT_CHAR + cCurCard.eSuit, 1);
				strncat(strOutputLine, CARD_VALUE_CHAR + cCurCard.eValue, 1);
				strncat(strOutputLine, " ", 1);
				bNotInvalid = true;
			}
		}
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "%s\n", strOutputLine) ;
#endif
	}

}

// Get a card at a particular position
CARD_T CSB_BakersGame::GetTableauCardAt(const int nCol, const int nRow) const {

	// Validate input
	/* Input validation reset to use constants to attempt to improve speed
	 if( nCol >= NumCols( )) return( m_cPrivateInvalidCard) ;
	 if( nRow >= NumRows( )) return( m_cPrivateInvalidCard) ;
	 */
	if (nCol >= NUM_COLS_ALLOCATED)
		return (m_cPrivateInvalidCard);
	if (nRow >= NUM_ROWS_ALLOCATED)
		return (m_cPrivateInvalidCard);
	if (0 > nCol)
		return (m_cPrivateInvalidCard);
	if (0 > nRow)
		return (m_cPrivateInvalidCard);

	// Okay to return
	return (m_acTableau[nCol][nRow]);

}

CARD_T CSB_BakersGame::GetReserveCard(const int nCol) const {

	// Validate input
	if (nCol >= NumReserveSpaces())
		return (m_cPrivateInvalidCard);
	if (0 > nCol)
		return (m_cPrivateInvalidCard);

	// Pull the card
	return (m_acReserve[nCol]);

}

CARDVALUES_T CSB_BakersGame::GetFoundationCardValue(const CARDSUITS_T eSuit) const {
	if (eSuit == CS_INVALID)
		return (CV_INVALID);
	return (m_acFoundationHighest[eSuit]);
}

// Copy the board
bool CSB_BakersGame::CopyBoard(const CSolitaireBoard *pBase) {

	// Variables
	CSB_BakersGame *pcBoard = (CSB_BakersGame*) pBase;

	// Check the base class
	if (false == CSolitaireBoard::CopyBoard(pBase))
		return (false);

	// Now copy the elements
	memcpy(m_acReserve, pcBoard->m_acReserve, sizeof(m_acReserve));
	memcpy(m_acTableau, pcBoard->m_acTableau, sizeof(m_acTableau));
	memcpy(m_acFoundationHighest, pcBoard->m_acFoundationHighest, sizeof(m_acFoundationHighest));
	m_bModeEasy = pcBoard->m_bModeEasy;
	m_bIsPlayable = pcBoard->m_bIsPlayable;

	// All is well
	return (true);

}

// Create a new board
bool CSB_BakersGame::InitRandomBoard() {

	// Constants
	const int MAX_ALLOWED_STUCK = 15;

	// Variables
	bool bRetValue = false;
	CARD_T cCurCard;
	int nStuckCnt = 0x0;
	long nCurSuit, nCurValue;
	long nCol, nLoop, nRow;
	long nRandValue;

	// Wrap it all
	try {

		// Initialize it all
		ClearBoard();

		// Start the loop
		for (nCurSuit = nStuckCnt = 0; CS_INVALID != nCurSuit; ++nCurSuit) {

			for (nCurValue = CV_ACE; CV_INVALID != nCurValue;) {

				// Come up with a random place to put this
				nRandValue = random() % 52;
				nCol = nRandValue % 8;
				nRow = nRandValue / 8;

				// Is this spot already taken?
				cCurCard = m_acTableau[nCol][nRow];
				if (m_cPrivateInvalidCard != m_acTableau[nCol][nRow]) {
					++nStuckCnt;
					if (MAX_ALLOWED_STUCK > nStuckCnt)
						continue;
					for (nLoop = 0; ((nLoop < 52) && (0x0 != nStuckCnt)); ++nLoop) {
						nCol = nLoop % 8;
						nRow = nLoop / 8;
						if (m_cPrivateInvalidCard == m_acTableau[nCol][nRow]) {
							nStuckCnt = 0;
							break;
						}
					}
				}

				// Okay to update
				cCurCard.eSuit = (CARDSUITS_T) nCurSuit;
				cCurCard.eValue = (CARDVALUES_T) nCurValue;
				m_acTableau[nCol][nRow] = cCurCard;

				// Adavance to next value
				++nCurValue;
				nStuckCnt = 0;

			}

		}

		// Everything has been assigned
		m_bIsPlayable = true;
		bRetValue = true;

	}

	catch (const char *e) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "%s\n", e) ;
#endif
		bRetValue = false;
	}

	catch (...) {
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Unknwon exception caught from CSB_BakersGame::InitRandomBoard\n") ;
#endif
		bRetValue = false;
	}

	// And return
	return (bRetValue);

}

// Is it a winner?
bool CSB_BakersGame::IsWinner() const {

	// Variables
	bool bRetValue = false;

	// All the foundation piles must be the KINGS
	bRetValue = (CV_KING == GetFoundationCardValue(CS_HEARTS)) && (CV_KING == GetFoundationCardValue(CS_SPADES))
			&& (CV_KING == GetFoundationCardValue(CS_CLUBS)) && (CV_KING == GetFoundationCardValue(CS_DIAMONDS));

	// And return!
	return (bRetValue);

}

// Apply a cheat
MOVE_T CSB_BakersGame::ApplyCheatAny() {

	// Constants
	const int NUM_COLS = NumCols();
	const int NUM_ROWS = NumRows();

	// Variables
	CARD_T cCurCard, cSearchCard;
	CARDSUITS_T eSuit = (CARDSUITS_T) (random() % CS_INVALID);
	CARDVALUES_T eValue;
	int nCol, nLoop, nRow;
	MOVE_T cRetMove;

	// Setup
	cRetMove.eMoveFrom = cRetMove.eMoveTo = MP_INVALID;

	// Find a suit to move
	if (CV_KING == GetFoundationCardValue(eSuit)) {
		for (nLoop = 0; CS_INVALID > nLoop; ++nLoop) {
			eSuit = (CARDSUITS_T) nLoop;
			if (CV_KING > GetFoundationCardValue(eSuit))
				break;
			eSuit = CS_INVALID;
		}
		if (CS_INVALID == eSuit)
			return (cRetMove);
	}

	// What's the highest value for that suit?
	eValue = GetFoundationCardValue(eSuit);

	// Increment it - that's the card to search for
	cSearchCard.eSuit = eSuit;
	cSearchCard.eValue = (CARDVALUES_T) (eValue + 1);
	for (nCol = 0; NUM_COLS > nCol; ++nCol) {
		for (nRow = 0; NUM_ROWS > nRow; ++nRow) {
			cCurCard = GetTableauCardAt(nCol, nRow);
			if (cCurCard == cSearchCard)
				break;
		} // endfor search rows
		if (cCurCard == cSearchCard)
			break;
	} // endfor search columns

	// The card was not found?
	if (cCurCard != cSearchCard)
		return (cRetMove);

	// Setup the move
	cRetMove.eMoveFrom = MP_TABLEAU;
	cRetMove.nFromCol = nCol;
	cRetMove.nFromRow = nRow;
	cRetMove.eMoveTo = MP_FOUNDATION;
	cRetMove.nToCol = cSearchCard.eSuit;
	cRetMove.nToRow = cSearchCard.eValue;

	// Adjust the foundation
	m_acFoundationHighest[eSuit] = cSearchCard.eValue;

	// Adjust the tableau
	for (; NUM_ROWS > nRow; ++nRow) {
		m_acTableau[nCol][nRow] = m_acTableau[nCol][nRow + 1];
	}
	m_acTableau[nCol][NUM_ROWS] = m_cPrivateInvalidCard;

	// And return
	return (cRetMove);

}

// Auto complete
MOVE_T CSB_BakersGame::AutoComplete() const {

	// Variables
	CARD_T cCurCard;
	MOVE_T cCompleteMove;
	int nCol, nRow, nCardValue, nFndValue;

	// Assume no good move
	cCompleteMove.eMoveFrom = cCompleteMove.eMoveTo = MP_INVALID;
	cCompleteMove.nFromCol = cCompleteMove.nFromRow = cCompleteMove.nToCol = cCompleteMove.nToRow = 0x0;

	// Check the reserve cards
	for (nCol = NumReserveSpaces() - 1; ((0 <= nCol) && (MP_INVALID == cCompleteMove.eMoveFrom)); --nCol) {

		// Pull the card
		cCurCard = GetReserveCard(nCol);
		if (CS_INVALID != cCurCard.eSuit) {

			// Pull card value & foundation values
			nCardValue = (int) cCurCard.eValue;
			nFndValue = (int) GetFoundationCardValue(cCurCard.eSuit);

			// Valid to move?
			if ((nFndValue + 1) == nCardValue) {
				cCompleteMove.eMoveFrom = MP_RESERVE;
				cCompleteMove.nFromCol = nCol;
				cCompleteMove.nFromRow = 0x0;
				cCompleteMove.eMoveTo = MP_FOUNDATION;
				cCompleteMove.nToCol = cCurCard.eSuit;
				cCompleteMove.nToRow = nCardValue;
				return (cCompleteMove);
			}

		}

	} // endfor check reserve

	// Loop over each column
	for (nCol = NumCols() - 1; ((0 <= nCol) && (MP_INVALID == cCompleteMove.eMoveFrom)); --nCol) {

		// Look for the bottom card
		for (nRow = NumRows() - 1; 0 <= nRow; --nRow) {

			// Pull the card
			cCurCard = GetTableauCardAt(nCol, nRow);
			if (CS_INVALID != cCurCard.eSuit) {

				// Pull card value & foundation values
				nCardValue = (int) cCurCard.eValue;
				nFndValue = (int) GetFoundationCardValue(cCurCard.eSuit);

				// Valid to move?
				if ((nFndValue + 1) == nCardValue) {
					cCompleteMove.eMoveFrom = MP_TABLEAU;
					cCompleteMove.nFromCol = nCol;
					cCompleteMove.nFromRow = nRow;
					cCompleteMove.eMoveTo = MP_FOUNDATION;
					cCompleteMove.nToCol = cCurCard.eSuit;
					cCompleteMove.nToRow = nCardValue;
				}

				// Move to next column
				break;

			}

		} // endfor rows
	} // endfor cols

	// And done
	return (cCompleteMove);

}

// Apply a move
bool CSB_BakersGame::ApplyMove(const MOVE_T cMove) {

	// Constants
	const int NUM_COLS = NumCols();
	const int NUM_RESV = NumReserveSpaces();
	const int NUM_ROWS = NumRows();

	// Variables
	bool bIsOk = false;
	bool bRetValue = false;
	CARD_T cFromCard, cNextCard, cToCard;
	int nCardValue, nFndValue, nTblValue;
	int nCardsToMove = 0;
	int nFreeSlots = 0;
	int nOpenCols = 0;
	int nFromRow, nToRow;
	int nLoop, nRow;
	int nSpaceToMove = -1;
	int nStartingRow;

	// Wrap it all
	try {

		// Quick validate input
		if (0 > cMove.nFromCol)
			throw("Invalid from column");
		if (0 > cMove.nFromRow)
			throw("Invalid from row");
		if (0 > cMove.nToCol)
			throw("Invalid to column");
		if (0 > cMove.nToRow)
			throw("Invalid to row");
		if (NumCols() <= cMove.nFromCol)
			throw("Invalid from column");
		if (NumCols() <= cMove.nToCol)
			throw("Invalid to column");
		if (NUM_ROWS <= cMove.nFromRow)
			throw("Invalid from row");
		if (NUM_ROWS <= cMove.nToRow)
			throw("Invalid to row");

		// Important enough to count free slots?
		if ((MP_TABLEAU == cMove.eMoveFrom) && (MP_TABLEAU == cMove.eMoveTo)) {
			for (nLoop = 0; NUM_RESV > nLoop; ++nLoop) {
				cNextCard = GetReserveCard(nLoop);
				if (CS_INVALID == cNextCard.eSuit)
					++nFreeSlots;
			}
			for (nLoop = 0; NUM_COLS > nLoop; ++nLoop) {
				cNextCard = GetTableauCardAt(nLoop, 0);
				if (CS_INVALID == cNextCard.eSuit)
					++nOpenCols;
			}
		}

		// Pull the moving card
		switch (cMove.eMoveFrom) {

		// Acceptable froms ...
		case MP_TABLEAU:
			cFromCard = GetTableauCardAt(cMove.nFromCol, cMove.nFromRow);
			break;
		case MP_RESERVE:
			cFromCard = GetReserveCard(cMove.nFromCol);
			break;

			// Unacceptable froms ...
		case MP_WASTEPILE:
		case MP_STOCKPILE:
		case MP_FOUNDATION:
		case MP_INVALID:
		default:
			throw("Invalid from type");
			break;

		}

		// Check that the from card is valid
		if (CS_INVALID == cFromCard.eSuit)
			throw("Invalid card suit in from");

		// Place the card
		switch (cMove.eMoveTo) {

		// Tableau case
		case MP_TABLEAU:

			// Where is it going?  Pessimist assumption invalid
			cToCard = GetTableauCardAt(cMove.nToCol, cMove.nToRow);
			bIsOk = false;

			// Is it going to an empty column?
			if ((m_cPrivateInvalidCard == cToCard) && (0 == cMove.nToRow)) {
				nStartingRow = 0;
				bIsOk = true;
			}

			// Is it going to fill out a stack?
			if (cFromCard.eSuit == cToCard.eSuit) {

				// Pull values
				nCardValue = cFromCard.eValue;
				nTblValue = cToCard.eValue;

				// Will it stack?
				bIsOk = ((nCardValue + 1) == nTblValue);

				// Is there space underneath?
				cNextCard = GetTableauCardAt(cMove.nToCol, cMove.nToRow + 1);
				bIsOk &= (CS_INVALID == cNextCard.eSuit);
				nStartingRow = cMove.nToRow + 1;

			}

			// Not okay?
			if (!bIsOk) {
#ifdef	_DEBUG
					char	strMessage [1024 + 1] ;
					DumpBoardToDebug( ) ;
					sprintf(
							strMessage, "From col: %d, From row: %d ===> To col: %d, To row: %d",
							cMove.nFromCol, cMove.nFromRow, cMove.nToCol, cMove.nToRow) ;
#ifdef	ACTIVE_LOGGING
					fprintf( stderr, "%s\n", strMessage) ;
#endif
#endif
				throw("Not able to stack this card/stack at the to location");
			}

			// Moving a stack?
			if (MP_TABLEAU == cMove.eMoveFrom) {

				nCardsToMove = 0;	// extra one from the loop + always can move one card anyway
				cNextCard = cFromCard;
				nCardValue = cNextCard.eValue - 1;
				for (nRow = cMove.nFromRow + 1; ((CS_INVALID != cNextCard.eSuit) && (NUM_ROWS > nRow));
						++nRow, --nCardValue) {
					++nCardsToMove;
					cNextCard = GetTableauCardAt(cMove.nFromCol, nRow);
					if ((CS_INVALID != cNextCard.eSuit) && (cNextCard.eSuit != cFromCard.eSuit))
						throw("Non-matching suit!");
					if ((CS_INVALID != cNextCard.eSuit) && (cNextCard.eValue != nCardValue))
						throw("Trying to move non-descending stack");
				}

				// Not an easy game?
				if (!m_bModeEasy) {
					nSpaceToMove = -1;
					cNextCard = GetTableauCardAt(cMove.nToCol, cMove.nToRow);
					if ((0x0 == cMove.nToRow) && (CS_INVALID == cNextCard.eSuit)) {
						if (0x0 == nFreeSlots)
							nSpaceToMove = nOpenCols;
						else if (1 >= nOpenCols)
							nSpaceToMove = nFreeSlots + 1;
						else
							nSpaceToMove = (nFreeSlots + 1) * (nOpenCols - 1);
					} else {
						if (0x0 == nFreeSlots)
							nSpaceToMove = 2 * nOpenCols;
						else
							nSpaceToMove = nFreeSlots * (nOpenCols + 1) + 1;
					}
					if ((1 < nCardsToMove) && (nCardsToMove > nSpaceToMove)) {
#ifdef	_DEBUG
							char	strDebugMsg [1024 + 1] ;
							DumpBoardToDebug( ) ;
							sprintf( strDebugMsg, "Tableau to tableau move: col %d row %d -> col %d row %d\n", cMove.nFromCol, cMove.nFromRow, cMove.nToCol, cMove.nToRow) ;
#ifdef	ACTIVE_LOGGING
							fprintf( stderr, "%s\n", strDebugMsg) ;
#endif
#endif
						throw("Insufficient free space to do the move");
					}
				}

				// Okay to move!
				for (nToRow = nStartingRow, nFromRow = cMove.nFromRow, cNextCard = cFromCard;
						((CS_INVALID != cNextCard.eSuit) && (NUM_ROWS > nRow)); ++nToRow, ++nFromRow) {
					cNextCard = GetTableauCardAt(cMove.nFromCol, nFromRow);
					m_acTableau[cMove.nToCol][nToRow] = cNextCard;
					m_acTableau[cMove.nFromCol][nFromRow] = m_cPrivateInvalidCard;
				}

			}

			// Moving from reserve?
			else if (MP_RESERVE == cMove.eMoveFrom) {
				m_acTableau[cMove.nToCol][nStartingRow] = cFromCard;
				m_acReserve[cMove.nFromCol] = m_cPrivateInvalidCard;
			}

			// All is well
			bRetValue = true;
			break;

		case MP_RESERVE:

			// Verify the reserve spot is empty
			cToCard = GetReserveCard(cMove.nToCol);
			if (CS_INVALID != cToCard.eSuit)
				throw("Reserve spot not empty");

			// Verify the source is valid
			bIsOk = false;	// pessimistic
			switch (cMove.eMoveFrom) {
			case MP_RESERVE:
				// Always good
				bIsOk = true;
				break;
			case MP_TABLEAU:
				cNextCard = GetTableauCardAt(cMove.nFromCol, cMove.nFromRow + 1);
				bIsOk = (CS_INVALID == cNextCard.eSuit);
				break;
			default:
				bIsOk = false;
				break;
			}
			if (!bIsOk)
				throw("Not a valid from location - maybe cards beneath it - RESERVE");

			// Make the move
			switch (cMove.eMoveFrom) {
			case MP_RESERVE:
				m_acReserve[cMove.nToCol] = m_acReserve[cMove.nFromCol];
				m_acReserve[cMove.nFromCol] = m_cPrivateInvalidCard;
				break;
			case MP_TABLEAU:
				m_acReserve[cMove.nToCol] = m_acTableau[cMove.nFromCol][cMove.nFromRow];
				m_acTableau[cMove.nFromCol][cMove.nFromRow] = m_cPrivateInvalidCard;
				break;
			default:
				// Do nothing
				break;
			}

			// All is well
			bRetValue = true;
			break;

			// Foundation case
		case MP_FOUNDATION:

			// Verify the suits are the same
			if (cFromCard.eSuit != cMove.nToCol)
				throw("Foundation move suit does not match");

			// Check values are appropriate
			nCardValue = cFromCard.eValue;
			nFndValue = GetFoundationCardValue(cFromCard.eSuit);
			if ((nFndValue + 1) != nCardValue)
				throw("Foundation move not next card");

			// Verify the source is valid
			bIsOk = false;	// pessimistic
			switch (cMove.eMoveFrom) {
			case MP_RESERVE:
				// Always good
				bIsOk = true;
				break;
			case MP_TABLEAU:
				cNextCard = GetTableauCardAt(cMove.nFromCol, cMove.nFromRow + 1);
				bIsOk = (CS_INVALID == cNextCard.eSuit);
				break;
			default:
				bIsOk = false;
				break;
			}
			if (!bIsOk)
				throw("Not a valid from location - maybe cards beneath it - FOUNDATION");

			// Place it
			m_acFoundationHighest[cFromCard.eSuit] = (CARDVALUES_T) nCardValue;
			switch (cMove.eMoveFrom) {
			case MP_RESERVE:
				m_acReserve[cMove.nFromCol] = m_cPrivateInvalidCard;
				break;
			case MP_TABLEAU:
				m_acTableau[cMove.nFromCol][cMove.nFromRow] = m_cPrivateInvalidCard;
				break;
			default:
				// Do nothing
				break;
			}

			// All is well
			bRetValue = true;
			break;

			// Unacceptable tos ...
		case MP_WASTEPILE:
		case MP_STOCKPILE:
		case MP_INVALID:
		default:
			throw("Invalid to location");
			break;
		}

	}

	catch (const char *e) {
#ifdef	_DEBUG
		char	strMessage [1024 + 50 + 1] ;
		strncpy( strMessage, "CSB_BakersGame::ApplyMove -> ", 50) ;
		strncat( strMessage, e, 1024) ;
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "%s\n", strMessage) ;
#endif
		usleep( 50) ;
#endif
		bRetValue = false;
	}

	catch (...) {
#ifdef	_DEBUG
#ifdef	ACTIVE_LOGGING
		fprintf( stderr, "Unknown exception caught from CSB_BakersGame::ApplyMove\n") ;
#endif
#endif
		bRetValue = false;
	}

	// And return
	return (bRetValue);

}

// Clear a board
void CSB_BakersGame::ClearBoard() {

	// Variables
	int nCol, nRow;

	// Initialize the tableau
	for (nCol = NumCols() - 1; 0 <= nCol; --nCol) {
		for (nRow = NumRows() - 1; 0 <= nRow; --nRow) {
			m_acTableau[nCol][nRow] = m_cPrivateInvalidCard;
		}
	}

	// Clear the foundation piles
	for (nCol = 0; nCol < CS_INVALID; ++nCol) {
		m_acFoundationHighest[nCol] = CV_EMPTY;
	}

	// Clear the reserve cards
	for (nCol = NumReserveSpaces() - 1; 0 <= nCol; --nCol) {
		m_acReserve[nCol] = m_cPrivateInvalidCard;
	}

	// And board is not playable now
	m_bIsPlayable = false;

}

// Board compression
bool CSB_BakersGame::RotateAndCompressBoard(COMP_BOARD *pData) const {

	// Helpful typedefs
	typedef std::vector<CARD_T> CNT_CARDS;
	typedef CNT_CARDS::iterator ITR_CARDS;

	// Constants
	const int NUM_COLS = NumCols();
	const int NUM_RESV = NumReserveSpaces();
	const int NUM_ROWS = NumRows();

	// Variables
	bool bSavedCol[NUM_COLS + 1];
	CARD_T cCurCard;
	CARD_T *pcCurCard;
	CNT_CARDS cntCards;
	int nCardPos;
	int nDataVal;
	int nCol, nPos, nRow;
	ITR_CARDS itrCards;
	size_t nLoop;

	// Validate input
	if ((COMP_BOARD*) 0x0 == pData)
		return (false);
	if (!IsPlayable())
		return (false);

	// Clear it all
	for (nLoop = 0; nLoop < sizeof(pData->uacData); ++nLoop)
		pData->uacData[nLoop] = 0xff;
	for (nLoop = 0; nLoop <= (size_t) NUM_COLS; ++nLoop)
		bSavedCol[nLoop] = false;

	// Save the foundation cards
	for (nLoop = nPos = 0; nLoop < CS_INVALID; ++nLoop, ++nPos)
		pData->uacData[nPos] = m_acFoundationHighest[nLoop];

	// Sort the reserve row and then save them
	cntCards.reserve(NUM_RESV + 1);
	for (nLoop = 0; (size_t) NUM_RESV > nLoop; ++nLoop)
		cntCards.push_back(m_acReserve[nLoop]);
	std::sort(cntCards.begin(), cntCards.end());
	for (nLoop = 0, itrCards = cntCards.begin(); cntCards.end() != itrCards; ++nLoop, ++nPos, ++itrCards) {
		pcCurCard = &(*itrCards);
		pData->uacData[nPos] = (((int) (pcCurCard->eSuit)) << 4) + pcCurCard->eValue;
	}

	// Sort the tableau cards by the topmost card in each stack
	cntCards.clear();
	cntCards.reserve(NUM_COLS + 1);
	for (nCol = 0; NUM_COLS > nCol; ++nCol)
		cntCards.push_back(m_acTableau[nCol][0]);
	std::sort(cntCards.begin(), cntCards.end());

	// Save out the columns
	for (nLoop = 0, itrCards = cntCards.begin(); cntCards.end() != itrCards; ++itrCards, ++nLoop) {

		// Which column is it?
		pcCurCard = &(*itrCards);
		for (nCol = 0; NUM_COLS > nCol; ++nCol) {
			if ((m_acTableau[nCol][0] == *pcCurCard) && (!bSavedCol[nCol]))
				break;
		}
		if (NUM_COLS == nCol)
			return (false);

		// And loop through the rows
		for (nRow = 0; nRow < NUM_ROWS; ++nRow) {
			cCurCard = m_acTableau[nCol][nRow];
			if (CS_INVALID != cCurCard.eSuit) {
				if (32 <= nRow)
					return (false);	// Can't record really deep rows
				nCardPos = cCurCard.eSuit * 13 + (cCurCard.eValue - 1);
				nDataVal = (nLoop << 5) + nRow;
				pData->uacData[nPos + nCardPos] = nDataVal;
			}
		}

	} // endfor loop over sorted columns

	// And return
	return (true);

}

bool CSB_BakersGame::CompressBoard(COMP_BOARD *pData) const {

	// Constants
	const int NUM_COLS = NumCols();
	const int NUM_RESV = NumReserveSpaces();
	const int NUM_ROWS = NumRows();

	// Variables
	CARD_T cCurCard;
	int nCardPos;
	int nDataVal;
	int nCol, nPos, nRow;
	size_t nLoop;

	// Validate input
	if ((COMP_BOARD*) 0x0 == pData)
		return (false);
	if (!IsPlayable())
		return (false);

	// Clear it all
	for (nLoop = 0; nLoop < sizeof(pData->uacData); ++nLoop)
		pData->uacData[nLoop] = 0xff;

	// Save the flags
	pData->uacData[0] |= (m_bIsPlayable ? 1 : 0);
	pData->uacData[0] |= (m_bModeEasy ? 2 : 0);

	// Save the foundation cards
	for (nLoop = 0, nPos = 1; nLoop < CS_INVALID; ++nLoop, ++nPos)
		pData->uacData[nPos] = m_acFoundationHighest[nLoop];

	// Save the reserve cards
	for (nLoop = 0; nLoop < (size_t) NUM_RESV; ++nLoop, ++nPos) {
		pData->uacData[nPos] = (((int) (m_acReserve[nLoop].eSuit)) << 4) + m_acReserve[nLoop].eValue;
	}

	// Save the filled in tableau cards
	for (nCol = 0; nCol < NUM_COLS; ++nCol) {
		for (nRow = 0; nRow < NUM_ROWS; ++nRow) {
			cCurCard = GetTableauCardAt(nCol, nRow);
			if (CS_INVALID != cCurCard.eSuit) {
				if (32 <= nRow)
					return (false);	// Can't record really deep rows
				nCardPos = cCurCard.eSuit * 13 + (cCurCard.eValue - 1);
				nDataVal = (nCol << 5) + nRow;
				pData->uacData[nPos + nCardPos] = nDataVal;
			}
		}
	}

	// And all is well
	return (true);

}

bool CSB_BakersGame::ExpandBoard(const COMP_BOARD *pData) {

	// Constants
	const int NUM_RESV = NumReserveSpaces();

	// Variables
	CARD_T cCurCard;
	int nCol, nLoop, nPos, nRow;

	// Validate input
	if ((const COMP_BOARD*) 0x0 == pData)
		return (false);

	// First, clear the board
	ClearBoard();

	// Read the flags
	m_bIsPlayable = (pData->uacData[0] & 1);
	m_bModeEasy = (pData->uacData[0] & 2);

	// Write the foundation cards
	for (nLoop = 0, nPos = 1; nLoop < CS_INVALID; ++nLoop, ++nPos)
		m_acFoundationHighest[nLoop] = (CARDVALUES_T) pData->uacData[nPos];

	// Write the reserve cards
	for (nLoop = 0; nLoop < NUM_RESV; ++nLoop, ++nPos) {
		cCurCard.eSuit = (CARDSUITS_T) (pData->uacData[nPos] >> 4);
		cCurCard.eValue = (CARDVALUES_T) (pData->uacData[nPos] & 0x0f);
		m_acReserve[nLoop] = cCurCard;
	}

	// Write the rest of it
	for (nLoop = 0; nLoop < 52; ++nLoop, ++nPos) {

		// Card already somewhere else?
		if (0xff == pData->uacData[nPos])
			continue;

		// Which card is it?  And where is it?
		cCurCard.eSuit = (CARDSUITS_T) (nLoop / 13);
		cCurCard.eValue = (CARDVALUES_T) ((nLoop % 13) + 1);
		nCol = pData->uacData[nPos] >> 5;
		nRow = pData->uacData[nPos] & 0x1f;
		m_acTableau[nCol][nRow] = cCurCard;

	}

	// And assume it's all good
	m_bIsPlayable = true;

	// All is well
	return (true);

}
