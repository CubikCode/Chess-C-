#pragma once
class CurrMove
{
private:
	int firstBut; //Holds first button pressed
	int secondBut; //Holds second button pressed
	int firstRow; //Holds current Row
	int secondRow; //Holds Row destination
	int firstCol; //Holds current Column
	int secondCol; //Holds Column Destination
	char firstPiece; //Holds Character of piece to move
	char secondPiece; //Holds character of piece possibly taken or * if there is no piece taken.
	int firstColor; //Holds the color of the first piece
	int secondColor; //Holds the color of the second piece if any.
public:
	CurrMove()
	{
		firstBut = -1;
		secondBut = -1;
		firstRow = -1;
		secondRow = -1;
		firstCol = -1;
		secondCol= -1;
		firstPiece = '*';
		secondPiece = '*';
		firstColor = 2;
		secondColor = 2;
	}

	int getFirstBut() { return firstBut;	}
	int getSecondBut() { return secondBut; }
	int getFirstRow() { return firstRow; }
	int getSecondRow() { return secondRow; }
	int getFirstCol() { return firstCol; }
	int getSecondCol() { return secondCol; }
	char getFirstPiece() { return firstPiece; }
	char getSecondPiece() { return secondPiece; }
	int getFirstColor() { return firstColor; }
	int getSecondColor() { return secondColor; }

	void setFirstBut(int b) { firstBut = b; }
	void setSecondBut(int b) { secondBut = b; }
	void setFirstRow(int r) { firstRow = r; }
	void setSecondRow(int r) { secondRow = r; }
	void setFirstCol(int c) { firstCol = c; }
	void setSecondCol(int c) { secondCol = c; }
	void setFirstPiece(char p) {  firstPiece = p; }
	void setSecondPiece(char p) {  secondPiece = p; }
	void setFirstColor(int c) {  firstColor = c; }
	void setSecondColor(int c) {  secondColor = c; }

};