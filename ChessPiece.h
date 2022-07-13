#pragma once
class ChessPiece {
private:

    //Name of piece
    char piece;

    //Holds the color
    int color;

    //Holds the actual position of the piece on the board
    //if both are -1 then the piece is out of the board
    int posRow;
    int posColumn;

    //Checks if it was previously moved, this is specifically for king and rooks
    bool firstMove;

public:
    ChessPiece() {
        piece = '*';
        color = 2;
        posRow = -1;
        posColumn = -1;
        firstMove = false;
    }
    ChessPiece(char p, int c) {
        piece = p;
        color = c;
        posRow = -1;
        posColumn = -1;
        firstMove = false;
    }

    char getPiece() { return piece; }
    int getColor() { return color; }
    int getPosRow() { return posRow; }
    int getPosColumn() { return posColumn; }
    bool getFirstMove() { return firstMove; }

    void setPiece(char p) { piece = p; }
    void setColor(int c) { color = c; }
    void setPosRow(int x) { posRow = x; }
    void setPosColumn(int y) { posColumn = y; }
    void setFirstMove(bool f) { firstMove = f; }


};

