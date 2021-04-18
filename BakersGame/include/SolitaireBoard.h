/*
 *  SolitaireBoard.h
 *  Solitaire Test
 *
 *  Created by Louis Gehrig on 11/8/10.
 *  Copyright 2010 Louis Gehrig. All rights reserved.
 *
 */

/*

 The CSolitaireBoard is an abstract class that provides all of the methods necessary to represent the board.  Specific
 instantiations of the class will have additional methods to support problem solving, etc.

 */

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

#ifndef	INCLUDE_SolitaireBoard_H
#define	INCLUDE_SolitaireBoard_H

// Necessary includes
#include <list>

//
// The card suits and values
//

enum e_card_suits {

	CS_CLUBS = 0, CS_DIAMONDS, CS_HEARTS, CS_SPADES, CS_INVALID

};
typedef enum e_card_suits CARDSUITS_T;

enum e_card_values {

	CV_EMPTY = 0,
	CV_ACE,
	CV_TWO,
	CV_THREE,
	CV_FOUR,
	CV_FIVE,
	CV_SIX,
	CV_SEVEN,
	CV_EIGHT,
	CV_NINE,
	CV_TEN,
	CV_JACK,
	CV_QUEEN,
	CV_KING,
	CV_INVALID

};
typedef enum e_card_values CARDVALUES_T;

struct s_card {

	// Members
	CARDSUITS_T eSuit;
	CARDVALUES_T eValue;

	// Constructors
	s_card() {
		eSuit = CS_INVALID;
		eValue = CV_INVALID;
	}
	s_card(const s_card &copy) :
			eSuit(copy.eSuit), eValue(copy.eValue) {
	}
	s_card(const CARDSUITS_T eS, CARDVALUES_T eV) {
		eSuit = eS;
		eValue = eV;
	}

	// Operators
	bool operator<(const struct s_card &right) const {
		if (this->eSuit > right.eSuit)
			return (false);
		if (this->eSuit < right.eSuit)
			return (true);
		return (this->eValue < right.eValue);
	}
	bool operator!=(const struct s_card &right) const {
		return ((this->eSuit != right.eSuit) || (this->eValue != right.eValue));
	}
	bool operator==(const struct s_card &right) const {
		return ((this->eSuit == right.eSuit) && (this->eValue == right.eValue));
	}

};
typedef s_card CARD_T;

// A structure to represent a compressed board
struct s_comp_board {
	unsigned char uacData[64];
};
typedef struct s_comp_board COMP_BOARD;
bool operator<(const COMP_BOARD &left, const COMP_BOARD &right);

enum e_move_position {

	MP_TABLEAU, MP_WASTEPILE, MP_STOCKPILE, MP_FOUNDATION, MP_RESERVE, MP_INVALID

};
typedef enum e_move_position MOVEPOS_T;
extern const char *MOVE_VALUES[];

struct s_move {

	MOVEPOS_T eMoveFrom;
	int nFromCol;
	int nFromRow;
	MOVEPOS_T eMoveTo;
	int nToCol;
	int nToRow;

};
typedef s_move MOVE_T;
typedef std::list<MOVE_T> LST_MOVES;
typedef LST_MOVES::const_iterator CITR_MOVES;
typedef LST_MOVES::iterator ITR_MOVES;

//
// The solitaire types
//

enum e_solitaire_types {

	ST_KLONDIKE_1,
	ST_KLONDIKE_3,
	ST_BAKERS_GAME,
	ST_BAKERS_GAME_TEST,
	ST_BAKERS_GAME_EASY,
	ST_BAKERS_GAME_EASY_TEST,
	ST_INVALID

};
typedef enum e_solitaire_types SOLITAIRE_T;

//
// Helpful functions
//

bool convertCardToText(CARD_T card, char *pBuffer, const size_t bufSize);
bool convertCardValueToText(CARDVALUES_T cardValue, char *pBuffer, const size_t bufSize);
CARD_T convertTextToCard(const char *pBuffer);
CARDVALUES_T convertTextToCardValue(const char *pBuffer);
void PrintMove(FILE *pOutput, const MOVE_T cTheMove);
SOLITAIRE_T convertTextToGameType(const char *gameTypeString);

//
// The CSolitaireBoard class
//

class CSolitaireBoard {

public:

	// Destructions
	virtual ~CSolitaireBoard();

	// Create a new board
	static CSolitaireBoard* CreateBoard(const SOLITAIRE_T eType);
	static CSolitaireBoard* CreateBoardCopy(const CSolitaireBoard *pBoard);

	// Get the type
	SOLITAIRE_T GetSolType() const;

	// Information about the board itself
	virtual bool HasFoundationPiles() const = 0;
	virtual bool HasStockPile() const = 0;
	virtual bool HasWastePile() const = 0;
	virtual bool IsPlayable() const = 0;
	virtual bool IsWinner() const = 0;
	virtual int NumCols() const = 0;
	virtual int NumReserveSpaces() const = 0;
	virtual int NumRows() const = 0;
	virtual CARD_T GetReserveCard(const int nCol) const = 0;
	virtual CARD_T GetTableauCardAt(const int nCol, const int nRow) const = 0;
	virtual CARDVALUES_T GetFoundationCardValue(const CARDSUITS_T eSuit) const = 0;

	// Clear a board
	virtual void ClearBoard() = 0;

	// Create a new random board
	virtual bool InitRandomBoard() = 0;

	// Copy an existing board - derived classes MUST call the base function first!
	// Only boards of the same type can be copied!
	virtual bool CopyBoard(const CSolitaireBoard *pBase);

	// User hits "Auto".
	// Auto should be recursively called until it returns an MP_INVALID from
	// Each invokation of AutoComplete returns only one move
	virtual MOVE_T AutoComplete() const = 0;

	// Apply a move
	virtual bool ApplyMove(const MOVE_T cMove) = 0;

	// Run a cheat
	virtual MOVE_T ApplyCheatAny() = 0;

	//
	// Board compression
	//
	// Board compression is meant to permit solvers & I/O classes to represent the
	// board as a simple memory object.  At present, boards must be able to be
	// compressed to within a 64 byte space.
	//
	// The RotateAndCompress functions should be used to ensure that a mere
	// rotation generates the same compressed board image.
	//

	virtual bool CompressBoard(COMP_BOARD *pData) const = 0;
	virtual bool RotateAndCompressBoard(COMP_BOARD *pData) const = 0;
	virtual bool ExpandBoard(const COMP_BOARD *pData) = 0;

	// Debugging aids
	virtual void DumpBoardToDebug() = 0;

protected:

	// Construction
	CSolitaireBoard();

	// Setters
	void SetSolType(const SOLITAIRE_T eType);

	// Member variables
	SOLITAIRE_T m_eSolType;

};

//
// The completely invalid board
//

class CSB_InvalidBoard: public CSolitaireBoard {

public:

	// Destruction
	virtual ~CSB_InvalidBoard() {
	}
	;

	// Information about the board
	virtual bool HasFoundationPiles() const {
		return (false);
	}
	;
	virtual bool HasStockPile() const {
		return (false);
	}
	;
	virtual bool HasWastePile() const {
		return (false);
	}
	;
	virtual bool IsPlayable() const {
		return (false);
	}
	;
	virtual bool IsWinner() const {
		return (false);
	}
	;
	virtual int NumCols() const {
		return (0);
	}
	;
	virtual int NumReserveSpaces() const {
		return (0);
	}
	;
	virtual int NumRows() const {
		return (0);
	}
	;
	virtual CARD_T GetReserveCard(const int nCol) const {
		CARD_T cInvalidCard;
		return (cInvalidCard);
	}
	;
	virtual CARD_T GetTableauCardAt(const int nCol, const int nRow) const {
		CARD_T cInvalidCard;
		return (cInvalidCard);
	}
	;
	virtual CARDVALUES_T GetFoundationCardValue(const CARDSUITS_T eSuit) const {
		return (CV_INVALID);
	}
	;

	// Create a new board
	virtual bool InitRandomBoard() {
		return (false);
	}
	;

	// Clear a board
	virtual void ClearBoard() {
	}
	;

	// Copy an existing board
	virtual bool CopyBoard(const CSolitaireBoard *pBase) {
		return (false);
	}
	;

	// Auto complete
	virtual MOVE_T AutoComplete() const;

	// Apply a move
	virtual bool ApplyMove(const MOVE_T cMove) {
		return (false);
	}
	;

	// Run a cheat
	virtual MOVE_T ApplyCheatAny() {
		MOVE_T cInvalidMove;
		cInvalidMove.eMoveFrom = cInvalidMove.eMoveTo = MP_INVALID;
		return (cInvalidMove);
	}
	;

	// Debugging aids
	virtual void DumpBoardToDebug() {
	}
	;

	// Compress board
	virtual bool CompressBoard(COMP_BOARD *pData) const {
		return (false);
	}
	;
	virtual bool RotateAndCompressBoard(COMP_BOARD *pData) const {
		return (false);
	}
	;
	virtual bool ExpandBoard(const COMP_BOARD *pData) {
		return (false);
	}
	;

protected:

	// Construction
	CSB_InvalidBoard() {
	}
	;
	friend class CSolitaireBoard;

};

//
// The Bakers Game board
//

class CSB_BakersGame: public CSolitaireBoard {

public:

	// Destruction
	virtual ~CSB_BakersGame();

	// Information about the board
	virtual bool HasFoundationPiles() const {
		return (true);
	}

	virtual bool HasStockPile() const {
		return (false);
	}

	virtual bool HasWastePile() const {
		return (false);
	}

	virtual bool IsPlayable() const {
		return (m_bIsPlayable);
	}

	bool isEasyMode() const {
		return (m_bModeEasy);
	}

	virtual bool IsWinner() const;
	virtual int NumCols() const {
		return (NUM_COLS_ALLOCATED);
	}

	virtual int NumReserveSpaces() const {
		return (4);
	}

	virtual int NumRows() const {
		return (NUM_ROWS_ALLOCATED - 1);
	}

	virtual CARD_T GetReserveCard(const int nCol) const;
	virtual CARD_T GetTableauCardAt(const int nCol, const int nRow) const;
	virtual CARDVALUES_T GetFoundationCardValue(const CARDSUITS_T eSuit) const;

	// Create a new board
	virtual bool InitRandomBoard();

	// Clear a board
	virtual void ClearBoard();

	// Copy an existing board
	virtual bool CopyBoard(const CSolitaireBoard *pBase);

	// Auto complete
	virtual MOVE_T AutoComplete() const;

	// Apply a move
	virtual bool ApplyMove(const MOVE_T cMove);

	// Run a cheat
	virtual MOVE_T ApplyCheatAny();

	// Debugging aids
	virtual void DumpBoardToDebug();

	// Board compression
	virtual bool CompressBoard(COMP_BOARD *pData) const;
	virtual bool RotateAndCompressBoard(COMP_BOARD *pData) const;
	virtual bool ExpandBoard(const COMP_BOARD *pData);

protected:

	// Construction
	CSB_BakersGame(const bool bModeEasy);
	friend class CSolitaireBoard;

	// Important constants
	static const int NUM_COLS_ALLOCATED = 8;
	static const int NUM_RESV_ALLOCATED = 4;
	static const int NUM_ROWS_ALLOCATED = 31;

	// The actual board itself
	bool m_bIsPlayable;
	bool m_bModeEasy;
	CARD_T m_acReserve[NUM_RESV_ALLOCATED];
	CARD_T m_acTableau[NUM_COLS_ALLOCATED][NUM_ROWS_ALLOCATED];
	CARDVALUES_T m_acFoundationHighest[CS_INVALID];

private:

	// Keep an invalid card available to prevent constant allocation of one
	CARD_T m_cPrivateInvalidCard;

};

//
// A Bakers Game board that can be modified.
//

class CSB_BakersGameModifiable: public CSB_BakersGame {

public:

	// Construction
	CSB_BakersGameModifiable(const bool bModeEasy) :
			CSB_BakersGame(bModeEasy) {
		ClearBoard();
		SetSolType(bModeEasy ? ST_BAKERS_GAME_EASY : ST_BAKERS_GAME);
	}

	// Destruction
	virtual ~CSB_BakersGameModifiable() {

	}

	// Setters
	void setFoundation(CARDSUITS_T suit, CARDVALUES_T value) {
		m_acFoundationHighest[suit] = value;
	}
	void setReserveSpace(int space, CARD_T value) {
		if ((0 <= space) && (CSB_BakersGame::NUM_RESV_ALLOCATED > space)) {
			m_acReserve[space] = value;
		}
	}
	void setTableau(int col, int row, CARD_T value) {
		if ((0 <= col) && (CSB_BakersGame::NUM_COLS_ALLOCATED > col) && (0 <= row)
				&& (CSB_BakersGame::NUM_ROWS_ALLOCATED > row)) {
			m_acTableau[col][row] = value;
		}
	}
	void setPlayable(const bool bPlayable) {
		m_bIsPlayable = bPlayable;
	}

};

#endif

