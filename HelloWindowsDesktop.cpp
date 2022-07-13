#include <windows.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <stack>

#include "ChessPiece.h"
#include "CurrMove.h"

//For changing from char to wchar_t
#include <cstdlib>
#include <cstring>
#include <string>

#define FILE_MENU_NEW 65
#define FILE_MENU_OPEN 66
#define FILE_MENU_EXIT 67
#define ROOK_UPGRADE 68
#define HORSE_UPGRADE 69
#define BISHOP_UPGRADE 70
#define QUEEN_UPGRADE 71

//Variables
const int SQUARES = 64;
//char pawnUpgrade = 'Q';
int pawnUpgradeColor;
int pawnUpgradeID = -1;
std::stack<int, std::vector<int>> buttonPushed;
	//HWND hName, hAge, hOut, hLogo;
HMENU hMenu;
HWND hMain;
HBITMAP hPieces[13];
HWND hBut[SQUARES];
int index = 0;
CurrMove currMove; //Holds the values of the pushedButtons, their respective rows and columns and the pieces to move.


//Chess Constants
const int ARRSIZE = 8;
const int WHITE = 0;
const int BLACK = 1;

//Chess Variables
ChessPiece chessBoard[ARRSIZE][ARRSIZE];
int playersTurn = WHITE;
// PrevMove holds the previous move:
// Piece, atRow, atColumn, toRow, toColumn, Color
int prevMove[6] = { -1, -1, -1, -1, -1, -1 };

ChessPiece white[16]{
       ChessPiece('P', 0),
       ChessPiece('P', 0),
       ChessPiece('P', 0),
       ChessPiece('P', 0),
       ChessPiece('P', 0),
       ChessPiece('P', 0),
       ChessPiece('P', 0),
       ChessPiece('P', 0),
       ChessPiece('R', 0),
       ChessPiece('H', 0),
       ChessPiece('B', 0),
       ChessPiece('K', 0), // 11
       ChessPiece('Q', 0),
       ChessPiece('B', 0),
       ChessPiece('H', 0),
       ChessPiece('R', 0),
};
ChessPiece black[16]{
        ChessPiece('P', 1),
        ChessPiece('P', 1),
        ChessPiece('P', 1),
        ChessPiece('P', 1),
        ChessPiece('P', 1),
        ChessPiece('P', 1),
        ChessPiece('P', 1),
        ChessPiece('P', 1),
        ChessPiece('R', 1),
        ChessPiece('H', 1),
        ChessPiece('B', 1),
        ChessPiece('Q', 1),
        ChessPiece('K', 1), //12
        ChessPiece('B', 1),
        ChessPiece('H', 1),
        ChessPiece('R', 1),
};




//Function declarations
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK DialogProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

void AddMenus(HWND);

void AddControls(HWND);

void addChessGame(); // Initializes the chessBoard Array.

void translateMove(); //Translate the button ID to the respective row and column to use in the chessBoard.

int translatePostoButton(int r, int c); //Translate the given row and column to a button.

void passAndClear(); //Pass the content of the pushedButtons to currMove and then empties the pushedButtons.

void loadImages();

int getImageID();

void registerDialogClass(HINSTANCE hInst);

void displayDialog(HWND hWnd, int color);

void newGame();





		//Chess Game Functions
		

//A function that updates the ChessBoard when a move occurs. Returns true if the move was applied.
bool movePiece(char p, int atRow, int atColumn, int toRow, int toColumn, ChessPiece arr[][ARRSIZE], ChessPiece white[], ChessPiece black[], int previousMove[], int _color);

//A function that verifies if the requested move is valid. Returns true if valid, false if not.
//Here lies most of the logic of chess.
bool validMove(char p, int atRow, int atColumn, int toRow, int toColumn, ChessPiece arr[][ARRSIZE], bool isCalledByCheck, ChessPiece white[], ChessPiece black[], int previousMove[], int _color);

//This function loops through all the pieces of the opposite color given and check if at least one piece can reach the given square with a valid move.
//If so, then a king of color color in a given square would be on check, therefore The function would return true. otherwise it will return false.
bool isOnCheck(int color, int atRow, int atColumn, ChessPiece board[][ARRSIZE], ChessPiece white[], ChessPiece black[], int previousMove[]);







int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow)
{

	WNDCLASSW wc = { 0 };

	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = hInst;
	wc.lpszClassName = L"myWindowClass";
	wc.lpfnWndProc = WindowProcedure;

	if (!RegisterClassW(&wc))
		return -1;

    registerDialogClass(hInst);

	hMain = CreateWindowW(L"myWindowClass", L"My Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 100, 100, 500, 500, NULL, NULL, NULL, NULL);

	MSG msg = { 0 };

	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_COMMAND:

		switch (wp)
		{

		case FILE_MENU_EXIT:
			DestroyWindow(hWnd);
			break;

		case FILE_MENU_NEW:
			MessageBeep(MB_ICONINFORMATION);
            newGame();
			break;

		default:
			if (wp >= 0 && wp < 64)
			{
                buttonPushed.push(wp);
                if (buttonPushed.size() == 2)
                {
                    passAndClear();
                    translateMove();

                    if (playersTurn == currMove.getFirstColor() && movePiece(currMove.getFirstPiece(), currMove.getFirstRow(), currMove.getFirstCol(), currMove.getSecondRow(), currMove.getSecondCol(), chessBoard, white, black, prevMove, currMove.getFirstColor()))
                    {
                        SendMessageW(hBut[currMove.getFirstBut()], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[12]);
                        int imageID;

                        imageID = getImageID();
                        SendMessageW(hBut[currMove.getSecondBut()], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[imageID]);

                        //Changes the turn of player.
                        playersTurn++;
                        playersTurn %= 2;
                    }
                }
			}
	

		}


		break;
	case WM_CREATE:
		loadImages();
		AddMenus(hWnd);
		AddControls(hWnd);
        addChessGame();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProcW(hWnd, msg, wp, lp);
	}

}

void AddMenus(HWND hWnd)
{
	hMenu = CreateMenu();

	HMENU hFileMenu = CreateMenu();
	HMENU hSubMenu = CreateMenu();

	//AppendMenu(hSubMenu, MF_STRING, NULL, L"SubMenu Item");



	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_NEW, L"New Game");
	//AppendMenu(hFileMenu, MF_POPUP, (UINT_PTR)hSubMenu, L"Load Game");
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_EXIT, L"Exit");


	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
	//AppendMenu(hMenu, MF_STRING, NULL, L"Help");

	SetMenu(hWnd, hMenu);
}

void AddControls(HWND hWnd)
{
	

	for (int i = 0; i < SQUARES; i++)
	{
		hBut[i] = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, (10 + ((i % 8) * 50)), (10 + ((i / 8) * 50)), 48, 48, hWnd, (HMENU)i, NULL, NULL);
		SendMessageW(hBut[i], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[12]);
	}

	//Setting the Pieces on Place:


	SendMessageW(hBut[0], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[6]);
	SendMessageW(hBut[1], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[7]);
	SendMessageW(hBut[2], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[8]);
	SendMessageW(hBut[3], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[9]);
	SendMessageW(hBut[4], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[10]);
	SendMessageW(hBut[5], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[8]);
	SendMessageW(hBut[6], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[7]);
	SendMessageW(hBut[7], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[6]);

	//Black Pawns
	SendMessageW(hBut[8], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[11]);
	SendMessageW(hBut[9], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[11]);
	SendMessageW(hBut[10], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[11]);
	SendMessageW(hBut[11], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[11]);
	SendMessageW(hBut[12], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[11]);
	SendMessageW(hBut[13], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[11]);
	SendMessageW(hBut[14], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[11]);
	SendMessageW(hBut[15], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[11]);

	
		//White Pawns
	SendMessageW(hBut[48], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[5]);
	SendMessageW(hBut[49], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[5]);
	SendMessageW(hBut[50], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[5]);
	SendMessageW(hBut[51], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[5]);
	SendMessageW(hBut[52], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[5]);
	SendMessageW(hBut[53], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[5]);
	SendMessageW(hBut[54], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[5]);
	SendMessageW(hBut[55], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[5]);

	//White Pieces
	SendMessageW(hBut[56], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[0]);
	SendMessageW(hBut[57], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[1]);
	SendMessageW(hBut[58], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[2]);
	SendMessageW(hBut[59], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[3]);
	SendMessageW(hBut[60], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[4]);
	SendMessageW(hBut[61], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[2]);
	SendMessageW(hBut[62], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[1]);
	SendMessageW(hBut[63], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[0]);


}

void loadImages()
{

	//hLogoImage = (HBITMAP)LoadImageW(NULL, L"White Pieces//WHorse.bmp", IMAGE_BITMAP, 100, 100, LR_LOADFROMFILE);
	hPieces[0] = (HBITMAP)LoadImageW(NULL, L"White Pieces//WRook.bmp", IMAGE_BITMAP, 48, 48, LR_LOADFROMFILE);
	hPieces[1] = (HBITMAP)LoadImageW(NULL, L"White Pieces//WHorse.bmp", IMAGE_BITMAP, 48, 48, LR_LOADFROMFILE);
	hPieces[2] = (HBITMAP)LoadImageW(NULL, L"White Pieces//WBishop.bmp", IMAGE_BITMAP, 48, 48, LR_LOADFROMFILE);
	hPieces[3] = (HBITMAP)LoadImageW(NULL, L"White Pieces//WQueen.bmp", IMAGE_BITMAP, 48, 48, LR_LOADFROMFILE);
	hPieces[4] = (HBITMAP)LoadImageW(NULL, L"White Pieces//WKing.bmp", IMAGE_BITMAP, 48, 48, LR_LOADFROMFILE);
	hPieces[5] = (HBITMAP)LoadImageW(NULL, L"White Pieces//WPawn.bmp", IMAGE_BITMAP, 48, 48, LR_LOADFROMFILE);
	hPieces[6] = (HBITMAP)LoadImageW(NULL, L"Black Pieces//BRook.bmp", IMAGE_BITMAP, 48, 48, LR_LOADFROMFILE);
	hPieces[7] = (HBITMAP)LoadImageW(NULL, L"Black Pieces//BHorse.bmp", IMAGE_BITMAP, 48, 48, LR_LOADFROMFILE);
	hPieces[8] = (HBITMAP)LoadImageW(NULL, L"Black Pieces//BBishop.bmp", IMAGE_BITMAP, 48, 48, LR_LOADFROMFILE);
	hPieces[9] = (HBITMAP)LoadImageW(NULL, L"Black Pieces//BQueen.bmp", IMAGE_BITMAP, 48, 48, LR_LOADFROMFILE);
	hPieces[10] = (HBITMAP)LoadImageW(NULL, L"Black Pieces//BKing.bmp", IMAGE_BITMAP, 48, 48, LR_LOADFROMFILE);
	hPieces[11] = (HBITMAP)LoadImageW(NULL, L"Black Pieces//BPawn.bmp", IMAGE_BITMAP, 48, 48, LR_LOADFROMFILE);
	hPieces[12] = (HBITMAP)LoadImageW(NULL, L"Blank Square//Square.bmp", IMAGE_BITMAP, 48, 48, LR_LOADFROMFILE);
}

void addChessGame() {

    //Initiate the chessboard with the pieces
    for (int i = 0; i < ARRSIZE; i++) {
        for (int j = 0; j < ARRSIZE; j++) {
            if (i == 0 || i == 1) {
                white[15 - (j + (i * 8))].setPosRow(i);
                white[15 - (j + (i * 8))].setPosColumn(j);
                chessBoard[i][j] = white[15 - (j + (i * 8))];

            }
            else if (i == 6 || i == 7) {
                black[j + (8 * (i - 6))].setPosRow(i);
                black[j + (8 * (i - 6))].setPosColumn(j);
                chessBoard[i][j] = black[j + (8 * (i - 6))];
            }
        }
    }

}

void translateMove() {

    currMove.setFirstRow(7 - (currMove.getFirstBut() / 8));
    currMove.setFirstCol(currMove.getFirstBut() % 8);
    currMove.setSecondRow(7 - (currMove.getSecondBut() / 8));
    currMove.setSecondCol(currMove.getSecondBut() % 8);
    currMove.setFirstPiece(chessBoard[currMove.getFirstRow()][currMove.getFirstCol()].getPiece());
    currMove.setFirstColor(chessBoard[currMove.getFirstRow()][currMove.getFirstCol()].getColor());

}

int translatePostoButton(int r, int c)
{
    return (8 * (7 - r)) + c;
}

void passAndClear()
{
    currMove.setSecondBut(buttonPushed.top());
    buttonPushed.pop();
    currMove.setFirstBut(buttonPushed.top());
    buttonPushed.pop();
    buttonPushed.size();

}

int getImageID()
{
    int id, p, c;

    p = chessBoard[currMove.getSecondRow()][currMove.getSecondCol()].getPiece();
    c = currMove.getFirstColor();

    switch (c)
    {
    case WHITE:
        switch (p)
        {
        case 'P':
            id = 5;
            break;
        case 'R':
            id = 0;
            break;
        case 'H':
            id = 1;
            break;
        case 'B':
            id = 2;
            break;
        case 'Q':
            id = 3;
            break;
        case 'K':
            id = 4;
            break;
        default:
            id = 12;
            break;
        }
        return id;
        break;
    case BLACK:
        switch (p)
        {
        case 'P':
            id = 11;
            break;
        case 'R':
            id = 6;
            break;
        case 'H':
            id = 7;
            break;
        case 'B':
            id = 8;
            break;
        case 'Q':
            id = 9;
            break;
        case 'K':
            id = 10;
            break;
        default:
            break;
        }
        return id;
        break;
    default:
        id = 12;
        return id;
        break;
    }
}

LRESULT CALLBACK DialogProcedure(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_CLOSE:
        EnableWindow(hMain, true);
        DestroyWindow(hWnd);
        break;
    case WM_COMMAND:
        char pUpgrade;
        int imageIndex;
        switch (wp)
        {
        case ROOK_UPGRADE:
            // pawnUpgrade = 'R';
            pUpgrade = 'R';
            imageIndex = 0;
            break;
        case HORSE_UPGRADE:
            pUpgrade = 'H';
            imageIndex = 1;
            //pawnUpgrade = 'H';
            break;
        case BISHOP_UPGRADE:
            pUpgrade = 'B';
            imageIndex = 2;
           // pawnUpgrade = 'B';
            break;
        case QUEEN_UPGRADE:
            pUpgrade = 'Q';
            imageIndex = 3;
           // pawnUpgrade = 'Q';
        default:
            break;
        }
        if (prevMove[5] == WHITE)
        {
            white[pawnUpgradeID].setPiece(pUpgrade);
            chessBoard[prevMove[3]][prevMove[4]].setPiece(pUpgrade);
            SendMessageW(hBut[translatePostoButton(prevMove[3], prevMove[4])], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[imageIndex]);
        }
        else 
        {
            imageIndex += 6;
            black[pawnUpgradeID].setPiece(pUpgrade);
            chessBoard[prevMove[3]][prevMove[4]].setPiece(pUpgrade);
            SendMessageW(hBut[translatePostoButton(prevMove[3], prevMove[4])], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[imageIndex]);
        }
        EnableWindow(hMain, true);
        DestroyWindow(hWnd);
        break;
    default:
        return DefWindowProcW(hWnd, msg, wp, lp);
        break;
    }
}


void registerDialogClass(HINSTANCE hInst) 
{
    WNDCLASSW dialog = { 0 };

    dialog.hbrBackground = (HBRUSH)COLOR_WINDOW;
    dialog.hCursor = LoadCursor(NULL, IDC_ARROW);
    dialog.hInstance = hInst;
    dialog.lpszClassName = L"myDialogClass";
    dialog.lpfnWndProc = DialogProcedure;

    RegisterClassW(&dialog);


}

void displayDialog(HWND hWnd, int color)
{
    HWND hDialog;
    hDialog = CreateWindowW(L"myDialogClass", L"Pawn Upgrade", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 400, 400, 200, 200, hWnd, NULL, NULL, NULL);

    HWND hRook, hHorse, hBishop, hQueen;
     hRook = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, (10), (10), 48, 48, hDialog, (HMENU)ROOK_UPGRADE, NULL, NULL);
     hHorse = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, (60), (10), 48, 48, hDialog, (HMENU)HORSE_UPGRADE, NULL, NULL);
     hBishop = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, (10), (60), 48, 48, hDialog, (HMENU)BISHOP_UPGRADE, NULL, NULL);
     hQueen = CreateWindowW(L"Button", NULL, WS_VISIBLE | WS_CHILD | BS_BITMAP, (60), (60), 48, 48, hDialog, (HMENU)QUEEN_UPGRADE, NULL, NULL);

     switch (color)
     {
     case WHITE:
         SendMessageW(hRook, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[0]);
         SendMessageW(hHorse, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[1]);
         SendMessageW(hBishop, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[2]);
         SendMessageW(hQueen, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[3]);
         break;
     case BLACK:
         SendMessageW(hRook, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[6]);
         SendMessageW(hHorse, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[7]);
         SendMessageW(hBishop, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[8]);
         SendMessageW(hQueen, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[9]);
         break;
     default:
         break;
     }

     EnableWindow(hMain, false);
}

//Chess Funtions Definitions


bool movePiece(char p, int atRow, int atColumn, int toRow, int toColumn, ChessPiece arr[][ARRSIZE], ChessPiece white[], ChessPiece black[], int previousMove[], int _color) {
    if (validMove(p, atRow, atColumn, toRow, toColumn, arr, false, white, black, previousMove, _color)) {

        //Confirms what pieces are on the two squares selected.
        int idWhite = -1, idBlack = -1;


        //The if and else structure updates accordingly the two pieces that were on the two squares. Updating the position of the piece that was capture to (-1, -1) if any.
        if (arr[atRow][atColumn].getColor() == 0) {

            for (int i = 0; i < 16; i++) {
                if (white[i].getPiece() == p && white[i].getPosRow() == atRow && white[i].getPosColumn() == atColumn) {
                    idWhite = i;
                }
                if (black[i].getPiece() == arr[toRow][toColumn].getPiece() && black[i].getPosRow() == toRow &&
                    black[i].getPosColumn() == toColumn) {

                    idBlack = i;
                }
            }

            //En Passant Captured piece update
            if (p == 'P' && toColumn != atColumn && arr[toRow][toColumn].getPiece() == '*') {
                for (int i = 0; i < 8; i++) {
                    if (black[i].getPosColumn() == toColumn && black[i].getPosRow() == atRow) {
                        idBlack = i;
                    }
                }
                arr[atRow][toColumn] = ChessPiece();

                //Add here the clear pawn from board
                SendMessageW(hBut[translatePostoButton(atRow, toColumn)], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[12]);
            }

            //std::cout << "Updating white piece " << white[idWhite].getPiece() << " position to: " << toRow << ", "
            //    << toColumn << std::endl;
            white[idWhite].setPosRow(toRow);
            white[idWhite].setPosColumn(toColumn);
            //std::cout << "Now the white piece " << white[idWhite].getPiece() << " position is: "
           //     << white[idWhite].getPosRow() << ", " << white[idWhite].getPosColumn() << std::endl;
            if (!white[idWhite].getFirstMove()) white[idWhite].setFirstMove(true);
            if (idBlack != -1) {
               // std::cout << "Killing the black piece\n\n\n";
                black[idBlack].setPosRow(-1);
                black[idBlack].setPosColumn(-1);
            }


        }
        else {

            for (int i = 0; i < 16; i++) {
                if (black[i].getPiece() == p && black[i].getPosRow() == atRow &&
                    black[i].getPosColumn() == atColumn)
                    idBlack = i;
                if (white[i].getPiece() == arr[toRow][toColumn].getPiece() && white[i].getPosRow() == toRow &&
                    white[i].getPosColumn() == toColumn)
                    idWhite = i;

            }

            //En Passant Captured piece update
            if (p == 'P' && toColumn != atColumn && arr[toRow][toColumn].getPiece() == '*') {
                for (int i = 0; i < 8; i++) {
                    if (white[i].getPosColumn() == toColumn && white[i].getPosRow() == atRow) {
                        idWhite = i;
                    }
                }
                arr[atRow][toColumn] = ChessPiece();
                //Add here the clear pawn from board
                SendMessageW(hBut[translatePostoButton(atRow, toColumn)], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[12]);
            }

            //std::cout << "Updating black piece position to: " << toRow << ", " << toColumn << std::endl;
            black[idBlack].setPosRow(toRow);
            black[idBlack].setPosColumn(toColumn);
            if (!black[idBlack].getFirstMove()) black[idBlack].setFirstMove(true);
            if (idWhite != -1) {

               // std::cout << "Killing the white piece\n\n\n";
                white[idWhite].setPosRow(-1);
                white[idWhite].setPosColumn(-1);
            }

        }
        //
        //
        //Code Block for Castle approved:
        if (p == 'K') {
            //White short Castle
            if (atRow == 0 && atColumn == 4 && toRow == 0 && toColumn == 6) {
                //Updating Rook position
                white[8].setFirstMove(true);
                white[8].setPosRow(0);
                white[8].setPosColumn(5);
                arr[0][5] = arr[0][7];
                SendMessageW(hBut[translatePostoButton(0, 5)], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[0]);
                arr[0][7] = ChessPiece();
                SendMessageW(hBut[translatePostoButton(0, 7)], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[12]);


            }

            //White long Castle
            else if (atRow == 0 && atColumn == 4 && toRow == 0 && toColumn == 2) {
                //Updating Rook position
                white[15].setFirstMove(true);
                white[15].setPosRow(0);
                white[15].setPosColumn(3);
                arr[0][3] = arr[0][0];
                SendMessageW(hBut[translatePostoButton(0, 3)], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[0]);

                arr[0][0] = ChessPiece();
                SendMessageW(hBut[translatePostoButton(0, 0)], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[12]);

            }

            //Black short Castle
            else if (atRow == 7 && atColumn == 4 && toRow == 7 && toColumn == 6) {
                //Updating Rook position
                black[15].setFirstMove(true);
                black[15].setPosRow(7);
                black[15].setPosColumn(5);
                arr[7][5] = arr[7][7];
                SendMessageW(hBut[translatePostoButton(7, 5)], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[6]);
                arr[7][7] = ChessPiece();
                SendMessageW(hBut[translatePostoButton(7, 7)], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[12]);


            }

            //Black long Castle
            else if (atRow == 7 && atColumn == 4 && toRow == 7 && toColumn == 2) {
                //Updating Rook position
                black[8].setFirstMove(true);
                black[8].setPosRow(7);
                black[8].setPosColumn(3);
                arr[7][3] = arr[7][0];
                SendMessageW(hBut[translatePostoButton(7, 3)], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[6]);

                arr[7][0] = ChessPiece();
                SendMessageW(hBut[translatePostoButton(7, 0)], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[12]);


            }
        }

        //
        //

        //Updates the Chessboard with the move.
        arr[toRow][toColumn] = arr[atRow][atColumn];
        arr[atRow][atColumn] = ChessPiece();

        //Pawn Upgrade
        if (p == 'P') {
            if (toRow == 7) {
                int pawnID;
                for (int i = 0; i < 8; i++) {
                    if (white[i].getPosRow() == toRow && white[i].getPosColumn() == toColumn) {
                        pawnID = i;
                    }
                }
                pawnUpgradeID = pawnID;
                //pawnUpgradeColor = WHITE;
                displayDialog(hMain, WHITE);


                white[pawnID].setPiece('P');
                arr[toRow][toColumn] = white[pawnID];
            }
            else if (toRow == 0) {
                int pawnID;
                for (int i = 0; i < 8; i++) {
                    if (black[i].getPosRow() == toRow && black[i].getPosColumn() == toColumn) {
                        pawnID = i;
                    }
                }
                pawnUpgradeID = pawnID;
                //pawnUpgradeColor = BLACK;
                displayDialog(hMain, BLACK);

                black[pawnID].setPiece('P');
                arr[toRow][toColumn] = black[pawnID];
            }
        }

        //Updating previousMove
        previousMove[0] = p;
        previousMove[1] = atRow;
        previousMove[2] = atColumn;
        previousMove[3] = toRow;
        previousMove[4] = toColumn;
        previousMove[5] = arr[toRow][toColumn].getColor();

        return true;
    }
    else {
        //std::cout << "Invalid move." << std::endl << std::endl;
        return false;
    }

}

//This function verifies if the chosen piece can actually move to the square.
bool validMove(char p, int atRow, int atColumn, int toRow, int toColumn, ChessPiece arr[][ARRSIZE], bool isCalledByCheck, ChessPiece white[], ChessPiece black[], int previousMove[], int _color) {
   /* std::cout << "Move test: \n" << "Piece: " << arr[atRow][atColumn].getPiece() << std::endl;
    std::cout << "Color: " << arr[atRow][atColumn].getColor() << std::endl;
    std::cout << "From: " << atRow << ", " << atColumn << std::endl;
    std::cout << "To: " << toRow << ", " << toColumn << std::endl << std::endl;*/


    if (arr[atRow][atColumn].getPiece() == p && atRow >= 0 && atRow < ARRSIZE && toRow >= 0 && toRow < ARRSIZE &&
        atColumn >= 0 && atColumn < ARRSIZE && toColumn >= 0 && toColumn < ARRSIZE && arr[atRow][atColumn].getColor() == _color) {

        int distRow, distColumn;
        distRow = toRow - atRow;
        distColumn = toColumn - atColumn;

        //Check that the move is not to stay on the same square
        if (distRow == 0 && distColumn == 0) {
            //std::cout << "The pieces wants to move to the square he is at.\n\n\n";
            return false;
        }


        switch (p) {
        case 'P':

            if (arr[atRow][atColumn].getColor() == 0 && distRow < 0) {
                //std::cout << "White pawn is trying to move backwards.\n\n";
                return false;
            }
            else if (arr[atRow][atColumn].getColor() == 1 && distRow > 0) {
                //std::cout << "Black Pawn is trying to move forward.\n\n";
                return false;
            }

            //Double square first move of pawn.
            if (abs(distRow) == 2) {
                //This pawn move doesn't put the king on check so it should not be valid when is called by the check function.
                if (isCalledByCheck) {
                    return false;
                }
                if (distColumn != 0) {
                    //std::cout << "Fails since the pawn is trying to move two forward and diagonally.\n\n";
                    return false;
                }

                else if (atRow != 1 && atRow != 6) {
                    //std::cout << "Fails to double jump since it is not on the spawn point.\n\n";
                    return false;
                }

                else if (arr[atRow][atColumn].getColor() == 0) {
                    if (arr[atRow + 1][atColumn].getPiece() != '*') {
                        //std::cout << "There is an obstacle in the path.\n\n";
                        return false;
                    }
                }
                else {
                    if (arr[atRow - 1][atColumn].getPiece() != '*') {
                        //std::cout << "There is an obstacle in the path. \n\n";
                        return false;
                    }
                }
            }
            else {
                if (abs(distRow) != 1) {
                    //std::cout << "Fails to move one row forward.\n\n";
                    return false;
                }
                else if (abs(distColumn) > 1) {
                    //std::cout << "Fails to move to the sides one or none \n\n";
                    return false;
                }

                //Checks for obstacles in path for forward movement.
                if (distColumn == 0) {
                    //Pawn doesn't put the King on check by moving foward so we return false if this function iscAlledByCheck.
                    if (isCalledByCheck) return false;
                    if (arr[toRow][toColumn].getPiece() != '*') {
                        //std::cout << "Pawn can't move forward when there is a piece on the path. \n\n";
                        return false;
                    }
                }



                //If the move is diagonal forward then it can only be applied if there is a piece of the opposite color on the destination.
                else if (arr[toRow][toColumn].getColor() == 2) {
                    /*
             Code for En Passant
              - It should Ignore this elseif if it has the requirements for En Passant
             */
                    if (previousMove[0] != 'P') {
                        //std::cout << "Last move wasn't a Pawn move.\n\n";
                        return false;
                    }
                    else if (abs(previousMove[3] - previousMove[1]) != 2) {
                        //std::cout << "Last move was not a double jump.\n\n";
                        return false;
                    }
                    else if (atRow != previousMove[3]) {
                        //std::cout << "The Pawns are not on the same row.\n\n";
                        return false;
                    }
                    else if (toColumn != previousMove[4]) {
                        //std::cout << "Pawns don't end up in the same column.\n\n";
                        return false;
                    }
                    //This statement is only temporadily since we can move pieces of the same color consecutively.
                    else if (arr[atRow][atColumn].getColor() == previousMove[5]) {
                        //std::cout << "The pawns have the same color \n\n ";
                        return false;
                    }
                }
            }
            break;

        case 'R':
            if (atRow == toRow) {
                if (atColumn == toColumn) return false;
            }
            else if (atColumn != toColumn) { return false; }

            //Verifies that there is no obstacle in the path that the piece has to move.
            if (distRow > 0) {
                for (int i = atRow + 1; i < toRow; i++) {
                    if (arr[i][atColumn].getPiece() != '*') {
                        //std::cout << "There is a piece on the path. \n \n \n";
                        return false;
                    }
                }
            }
            else if (distRow < 0) {
                for (int i = atRow - 1; i > toRow; i--) {
                    if (arr[i][atColumn].getPiece() != '*') {
                        //std::cout << "There is a piece on the path. \n \n \n";
                        return false;
                    }
                }
            }
            else {
                if (distColumn > 0) {
                    for (int i = atColumn + 1; i < toColumn; i++) {
                        if (arr[atRow][i].getPiece() != '*') {
                            //std::cout << "There is a piece on the path. \n \n \n";
                            return false;
                        }
                    }
                }
                else if (distColumn < 0) {
                    for (int i = atColumn - 1; i > toColumn; i--) {
                        if (arr[atRow][i].getPiece() != '*') {
                            //std::cout << "There is a piece on the path. \n \n \n";
                            return false;
                        }
                    }
                }
            }
            //Ends the verification of obstacle

            break;
        case 'H':
            if (atRow == toRow || atColumn == toColumn) {
                //std::cout << "The row or the column is the same and the horse doesn't move like that.\n\n\n";
                return false;

            }
            else if (abs(distRow) + abs(distColumn) != 3) {
                //std::cout << "The sum of the absolute distants is not 3.\n\n\n ";
                return false;
            }
            break;
        case 'B':
            //Verifiy if bishop is moving properly
            if (abs(distRow) != abs(distColumn)) return false;
            //Verifies if there is no obstacle on path
            else if (distRow > 0) {
                if (distColumn > 0) {
                    for (int i = 1; i < (distRow); i++) {
                        if (arr[atRow + i][atColumn + i].getColor() != 2) return false;
                    }
                }
                else {
                    for (int i = 1; i < (distRow); i++) {
                        if (arr[atRow + i][atColumn - i].getColor() != 2) return false;
                    }
                }
            }
            else {
                if (distColumn > 0) {
                    for (int i = 1; i < (distColumn); i++) {
                        if (arr[atRow - i][atColumn + i].getColor() != 2) return false;
                    }
                }
                else {
                    for (int i = 1; i < (abs(distColumn)); i++) {
                        if (arr[atRow - i][atColumn - i].getColor() != 2) return false;
                    }
                }
            }
            break;
        case 'Q':

            //Queen acting as a Rook
            if (distRow == 0 || distColumn == 0) {

                //Verifies that there is no obstacle in the path that the piece has to move.
                if (distRow > 0) {
                    for (int i = atRow + 1; i < toRow; i++) {
                        if (arr[i][atColumn].getPiece() != '*') {
                            //std::cout << "There is a piece on the path. \n \n \n";
                            return false;
                        }
                    }
                }
                else if (distRow < 0) {
                    for (int i = atRow - 1; i > toRow; i--) {
                        if (arr[i][atColumn].getPiece() != '*') {
                            //std::cout << "There is a piece on the path. \n \n \n";
                            return false;
                        }
                    }
                }
                else {
                    if (distColumn > 0) {
                        for (int i = atColumn + 1; i < toColumn; i++) {
                            if (arr[atRow][i].getPiece() != '*') {
                                //std::cout << "There is a piece on the path. \n \n \n";
                                return false;
                            }
                        }
                    }
                    else if (distColumn < 0) {
                        for (int i = atColumn - 1; i > toColumn; i--) {
                            if (arr[atRow][i].getPiece() != '*') {
                                //std::cout << "There is a piece on the path. \n \n \n";
                                return false;
                            }
                        }
                    }
                }
                //Ends the verification of obstacle


            }

            //Queen acting as a Bishop:

            else if (abs(distRow) != abs(distColumn)) return false;
            //Verifies if there is no obstacle on path
            else if (distRow > 0) {
                if (distColumn > 0) {
                    for (int i = 1; i < (distRow); i++) {
                        if (arr[atRow + i][atColumn + i].getColor() != 2) return false;
                    }
                }
                else {
                    for (int i = 1; i < (distRow); i++) {
                        if (arr[atRow + i][atColumn - i].getColor() != 2) return false;
                    }
                }
            }
            else {
                if (distColumn > 0) {
                    for (int i = 1; i < (distColumn); i++) {
                        if (arr[atRow - i][atColumn + i].getColor() != 2) return false;
                    }
                }
                else {
                    for (int i = 1; i < (abs(distColumn)); i++) {
                        if (arr[atRow - i][atColumn - i].getColor() != 2) return false;
                    }
                }
            }

            break;
        case 'K':
            if (arr[atRow][atColumn].getColor() == 0) {
                //White long castle:
                if (atRow == 0 && toRow == 0 && atColumn == 4 && toColumn == 2) {
                    //
                    //Code for verifying that the rook and king haven't moved yet.
                    //
                    if (white[15].getFirstMove() != false || white[15].getPosRow() == -1 || white[11].getFirstMove() != false) return false;
                    //
                    //Code for obstacles on path
                    //
                    if (arr[0][3].getPiece() != '*' || arr[0][2].getPiece() != '*' || arr[0][1].getPiece() != '*') return false;
                    //
                    //Code for verifying checks
                    //

                    if (isOnCheck(WHITE, 0, 4, arr, white, black, previousMove)) {
                        //std::cout << "The King is on check so he can't castle.\n\n";
                        return false;
                    }
                    else if (isOnCheck(WHITE, 0, 3, arr, white, black, previousMove)) {
                        //std::cout << "The King would pass through check if he does that castle.\n";
                        return false;
                    }
                }
                //White short castle:
                else if (atRow == 0 && toRow == 0 && atColumn == 4 && toColumn == 6) {
                    //
                    //Code for verifying that the rook and king haven't moved yet.
                    //
                    if (white[8].getFirstMove() != false || white[8].getPosRow() == -1 || white[11].getFirstMove() != false) return false;
                    //
                    //Code for obstacles on path
                    //
                    if (arr[0][5].getPiece() != '*' || arr[0][6].getPiece() != '*') return false;
                    //
                    //Code for verifying checks
                    //
                    if (isOnCheck(WHITE, 0, 4, arr, white, black, previousMove)) {
                        //std::cout << "The King is on check so he can't castle.\n\n";
                        return false;
                    }
                    else if (isOnCheck(WHITE, 0, 5, arr, white, black, previousMove)) {
                        //std::cout << "The King would pass through check if he does that castle.\n";
                        return false;
                    }
                }
                else if (abs(distRow) > 1 || abs(distColumn) > 1) {
                    //std::cout << "We verified that it was not a castle, and this move is not valid for a king.\n\n";
                    return false;
                }
            }
            else {
                //Black long castle:
                if (atRow == 7 && toRow == 7 && atColumn == 4 && toColumn == 2) {
                    //
                    //Code for verifying that the rook and king haven't moved yet.
                    //
                    if (black[8].getFirstMove() != false || black[8].getPosRow() == -1 || black[12].getFirstMove() != false) return false;
                    //
                    //Code for obstacles on path
                    //
                    if (arr[7][3].getPiece() != '*' || arr[7][2].getPiece() != '*' || arr[7][1].getPiece() != '*') return false;
                    //
                    //Code for verifying checks
                    //

                    if (isOnCheck(BLACK, 7, 4, arr, white, black, previousMove)) {
                        //std::cout << "The King is on check so he can't castle.\n\n";
                        return false;
                    }
                    else if (isOnCheck(BLACK, 7, 3, arr, white, black, previousMove)) {
                        //std::cout << "The King would pass through check if he does that castle.\n";
                        return false;
                    }
                }
                //White short castle:
                else if (atRow == 7 && toRow == 7 && atColumn == 4 && toColumn == 6) {
                    //
                    //Code for verifying that the rook and king haven't moved yet.
                    //
                    if (black[15].getFirstMove() != false || black[15].getPosRow() == -1 || black[12].getFirstMove() != false) return false;
                    //
                    //Code for obstacles on path
                    //
                    if (arr[7][5].getPiece() != '*' || arr[7][6].getPiece() != '*') return false;
                    //
                    //Code for verifying checks
                    //

                    if (isOnCheck(BLACK, 7, 4, arr, white, black, previousMove)) {
                        //std::cout << "The King is on check so he can't castle.\n\n";
                        return false;
                    }
                    else if (isOnCheck(BLACK, 7, 5, arr, white, black, previousMove)) {
                        //std::cout << "The King would pass through check if he does that castle.\n";
                        return false;
                    }
                }
                else if (abs(distRow) > 1 || abs(distColumn) > 1) {
                    //std::cout << "We verified that it was not a castle, and this move is not valid for a king.\n\n";
                    return false;
                }
            }

            break;
        default:
            return false;
        }

        //Verifies that the destination of the piece doesn't have a piece of same color.
        if (!isCalledByCheck && arr[atRow][atColumn].getColor() == arr[toRow][toColumn].getColor()) return false;

        //If the piece is a King, then we have to verify for checks on the new position.
        if (arr[atRow][atColumn].getPiece() == 'K') {
            if (!isCalledByCheck && isOnCheck(arr[atRow][atColumn].getColor(), toRow, toColumn, arr, white, black, previousMove)) {
                //std::cout << "The king would be in check so we are returning as invalid move.\n\n";
                return false;
            }
        }
        if (isCalledByCheck != true) {
            ChessPiece temp[ARRSIZE][ARRSIZE];

            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    temp[i][j] = arr[i][j];
                }
            }

            temp[toRow][toColumn] = temp[atRow][atColumn];
            temp[atRow][atColumn] = ChessPiece();

            if (temp[toRow][toColumn].getColor() == WHITE) {
                if (temp[toRow][toColumn].getPiece() == 'K')
                {
                    if (isOnCheck(WHITE, toRow, toColumn,
                        temp, white, black, previousMove)) return false;
                }
                else if (isOnCheck(WHITE, white[11].getPosRow(), white[11].getPosColumn(),
                    temp, white, black, previousMove)) {
                    //std::cout << "The move would put the King on discovered check.\n\n";
                    return false;
                }

            }
            else if (temp[toRow][toColumn].getColor() == BLACK) {
                if (temp[toRow][toColumn].getPiece() == 'K')
                {
                    if (isOnCheck(BLACK, toRow, toColumn,
                        temp, white, black, previousMove)) return false;
                }
                else if (isOnCheck(BLACK, black[12].getPosRow(), black[12].getPosColumn(),
                    temp, white, black, previousMove)) {
                    //std::cout << "The move would put the King on discovered check.\n\n";
                    return false;
                }
            }
        }

        return true;
    }
    else return false;
}

bool isOnCheck(int color, int atRow, int atColumn, ChessPiece board[][ARRSIZE], ChessPiece white[], ChessPiece black[], int previousMove[]) {
    switch (color) {
    case 0:
        for (int i = 0; i < 16; i++) {
            if (black[i].getPosRow() != -1) {
                if (validMove(black[i].getPiece(), black[i].getPosRow(), black[i].getPosColumn(), atRow, atColumn, board, true, white, black, previousMove, BLACK)) {
                    //std::cout << "The black " << black[i].getPiece() << " at position (" << black[i].getPosRow() << ", " << black[i].getPosColumn()
                        //<< ") Can move to position (" << atRow << ", " << atColumn << ") and attack the White King.\n\n\n";
                    return true;
                }
            }
        }
        break;
    case 1:
        for (int i = 0; i < 16; i++) {
            if (white[i].getPosRow() != -1) {
                if (validMove(white[i].getPiece(), white[i].getPosRow(), white[i].getPosColumn(), atRow, atColumn,
                    board, true, white, black, previousMove, WHITE)) {
                    //std::cout << "The white " << white[i].getPiece() << " at position (" << white[i].getPosRow() << ", " << white[i].getPosColumn()
                        //<< ") Can move to position (" << atRow << ", " << atColumn << ") and attack the Black King.\n\n\n";
                    return true;
                }

            }

        }
        break;
    default:
        break;
    }
    return false;
}

void newGame()
{

    playersTurn = 0;

    while (!buttonPushed.empty()) {
        buttonPushed.pop();
    }

        white[0] = ChessPiece('P', 0);
        white[1] = ChessPiece('P', 0);
        white[2] = ChessPiece('P', 0);
        white[3] = ChessPiece('P', 0);
        white[4] = ChessPiece('P', 0);
        white[5] = ChessPiece('P', 0);
        white[6] = ChessPiece('P', 0);
        white[7] = ChessPiece('P', 0);
        white[8] = ChessPiece('R', 0);
        white[9] = ChessPiece('H', 0);
        white[10] = ChessPiece('B', 0);
        white[11] = ChessPiece('K', 0); // 11
        white[12] = ChessPiece('Q', 0);
        white[13] = ChessPiece('B', 0);
        white[14] = ChessPiece('H', 0);
        white[15] = ChessPiece('R', 0);

        black[0] = ChessPiece('P', 1);
        black[1] = ChessPiece('P', 1);
        black[2] = ChessPiece('P', 1);
        black[3] = ChessPiece('P', 1);
        black[4] = ChessPiece('P', 1);
        black[5] = ChessPiece('P', 1);
        black[6] = ChessPiece('P', 1);
        black[7] = ChessPiece('P', 1);
        black[8] = ChessPiece('R', 1);
        black[9] = ChessPiece('H', 1);
        black[10] = ChessPiece('B', 1);
        black[11] = ChessPiece('Q', 1);
        black[12] = ChessPiece('K', 1); //12
        black[13] = ChessPiece('B', 1);
        black[14] = ChessPiece('H', 1);
        black[15] = ChessPiece('R', 1);

        

        for (int i = 0; i < 64; i++)
        {
            chessBoard[i / 8][i % 8] = ChessPiece();
            SendMessageW(hBut[i], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[12]);
        }

        addChessGame();

        SendMessageW(hBut[0], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[6]);
        SendMessageW(hBut[1], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[7]);
        SendMessageW(hBut[2], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[8]);
        SendMessageW(hBut[3], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[9]);
        SendMessageW(hBut[4], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[10]);
        SendMessageW(hBut[5], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[8]);
        SendMessageW(hBut[6], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[7]);
        SendMessageW(hBut[7], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[6]);

        //Black Pawns
        SendMessageW(hBut[8], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[11]);
        SendMessageW(hBut[9], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[11]);
        SendMessageW(hBut[10], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[11]);
        SendMessageW(hBut[11], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[11]);
        SendMessageW(hBut[12], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[11]);
        SendMessageW(hBut[13], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[11]);
        SendMessageW(hBut[14], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[11]);
        SendMessageW(hBut[15], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[11]);


        //White Pawns
        SendMessageW(hBut[48], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[5]);
        SendMessageW(hBut[49], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[5]);
        SendMessageW(hBut[50], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[5]);
        SendMessageW(hBut[51], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[5]);
        SendMessageW(hBut[52], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[5]);
        SendMessageW(hBut[53], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[5]);
        SendMessageW(hBut[54], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[5]);
        SendMessageW(hBut[55], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[5]);

        //White Pieces
        SendMessageW(hBut[56], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[0]);
        SendMessageW(hBut[57], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[1]);
        SendMessageW(hBut[58], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[2]);
        SendMessageW(hBut[59], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[3]);
        SendMessageW(hBut[60], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[4]);
        SendMessageW(hBut[61], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[2]);
        SendMessageW(hBut[62], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[1]);
        SendMessageW(hBut[63], BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hPieces[0]);

}