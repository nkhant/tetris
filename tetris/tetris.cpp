// tetris.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

using namespace std;

#include <windows.h>
#include <thread>
#include <vector>

wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char *pField = nullptr; // 0  is empty, 1 part of shape and walls

int nScreenWidth = 80;
int nScreenHeight = 30;

// r is 0, 1, 2, 3 considering to degrees and returns an index
int Rotate(int px, int py, int r)
{
	switch (r % 4)
	{
	case 0: return py * 4 + px; // 0
	case 1: return 12 + py - (px * 4); // 90
	case 2: return 15 - (4 * py) - px; // 180
	case 3: return 3 - py + (4 * px); // 270

	}
	return 0;
}

bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{


	//only will change if something causes to
	//no point to check lots of locations
	//if one fails, immdeiatly return
	for (int px = 0; px < 4; px++)
	{ 
		for (int py = 0; py < 4; py++)
		{
			//get index into piece of testronimo
			int pi = Rotate(px, py, nRotation);

			//get index into field based on current playing locationo
			//posy and posx are top left
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			//because we in c++, need to remember not to go out of bounds for checks
			//otherwise accessing memories that isnt there
			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
			{
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight)
				{
					//there was a bug where "X" made it constant and '' not
					//
					if (tetromino[nTetromino][pi] == L'X' && pField[fi] != 0)
					{
						return false;//fail on first hit
					}
				}
			}
		}
	}
	return true;
}

int main()
{
	//Create Assets-------------------
	// Dot is emtpy space, X is part of the shape
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	tetromino[2].append(L".X.");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	tetromino[6].append(L"....");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L".X..");
	tetromino[6].append(L".X..");

	pField = new unsigned char[nFieldHeight*nFieldWidth]; //creates the walls
	for (int x = 0; x < nFieldWidth; x++)
	{ //board boundry
		for (int y = 0; y < nFieldHeight; y++)
		{
			//set the pField to 0 if empty and 9 for walls
			//using a single dimension arary and changing index via math of (y * width) + x 
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
		}
	}
	wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];
	for (int i = 0; i < nScreenHeight*nScreenWidth; i++) {
		screen[i] = L' ';
	}
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;


	

	
	
	bool bGameOver = false;

	int nCurrentPiece = 0;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;


	bool bKey[4];
	bool bRotateHold = false; // if user holding

	int nSpeed = 20; //diffuclty or current lvl of game
	int nSpeedCounter = 0; //counts until == nSpeed which will then force speed down
	bool bForceDown = false;
	int nPieceCount = 0;
	int nScore = 0;

	vector<int> vLines;

	while (!bGameOver)
	{
		// GAME TIMING ---------------------------------------------------
		this_thread::sleep_for(50ms);
		nSpeedCounter++;
		bForceDown = (nSpeedCounter == nSpeed);
		// USER INPUT ----------------------------------------------------
		//get asyn keys
		//goes thrgh the array and gets the async keys to see of key is pressed, returns true if so
		//hexidecimal of they keys
		//end up with array of true or false
		for (int k = 0; k < 4; k++)								// R   L   D Z
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

		// GAME LOGIC (shapes falling and collision, etc) -----------------------------------
		if (bKey[0])
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) {
				nCurrentX = nCurrentX + 1;
			}
		}
		if (bKey[1])
		{
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) {
				nCurrentX = nCurrentX - 1;
			}
		}

		//if you see nested if, probably can always just add an && to make it one if like so:
		if (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY +1 )) {
			nCurrentY = nCurrentY + 1;
		}
		//OR YOU CAN ALSO DO THIS TO OPTIMIZE for more simplicity TO BE ON ONE LINE ONLY such as shown below
		//nCurrentRotation += (bKey[3] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY ) ? 1 : 0);
		if (bKey[3])
		{
			nCurrentRotation += (!bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY) ? 1 : 0);
			bRotateHold = true;
		}
		else
		{
			bRotateHold = false;
		}
		
		if (bForceDown) {
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) {
				nCurrentY++;
			}
			else
			{
				//lock current piece in the field
				for (int px = 0; px < 4; px++)
				{
					for (int py = 0; py < 4; py++)
					{
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X') {
							//empty and shape are both 0 hence + 1
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;
						}
					}
				}

				nPieceCount++;
				if (nPieceCount % 10 == 0)
				{
					if (nSpeed >= 10) nSpeed--; //nSpeed == game ticks so smaller is faster
				}
				//check have got any lines
				//dont need to check whole field, only last tetromino piece
				//only need check 4 rows
				for (int py = 0; py < 4; py++)
				{
					if (nCurrentY + py < nFieldHeight - 1) //boundry check
					{
						bool bLine = true; //assuming there is going to be a line
						//doesnt check fist and last cells cause they are boundries
						for (int px = 1; px < nFieldWidth - 1; px++)
						{
							// if empty space, then false, checks vi indexting
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0; 
						}

						if (bLine)
						{
							//remove Line and replace to =
							for (int px = 1; px < nFieldWidth - 1; px++) 
							{
								pField[(nCurrentY + py) * nFieldWidth + px] = 8; //8 is equal signal
							}
							vLines.push_back(nCurrentY + py);
						}
					}
				}

				nScore += 25;
				if (!vLines.empty()) 
				{
					nScore += (1 << vLines.size()) * 100; //more reward for bigger risk
				}
				//chose the next piece
				nCurrentPiece = rand() % 7;
				nCurrentRotation = 0;
				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;

				//if piece does not fit
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
		
			}
			nSpeedCounter = 0;

		}
		// RENDER OUTPUT ---------------------------------------------
		
		//Draw field
		for (int x = 0; x < nFieldWidth; x++)
		{ //board boundry
			for (int y = 0; y < nFieldHeight; y++)

			{
				//iterates through x and y
				//offset by 2 so it isnt right in the corner of the screen 
				//setting output depending on field array. if 0 it will be empty space, 1-7 is tetris shapes, = for 8 to make line, " for border
				//use the 
				screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y*nFieldWidth + x]];
			}
		}


		//draw current piece
		for (int px = 0; px < 4; px++)
		{
			for (int py = 0; py < 4; py++)
			{
				if(tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
				{
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65; //equals to asies table ABC etc
                }
			}
		}

		//Draw score
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);

		if (!vLines.empty())
		{
			//Display Frame
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms); //delay to see equal signs;

			//removes the lines and move down all the shapes above the line
			for (auto &v : vLines) //gives us the row
			{
				for (int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
					{
						//update and pushed stuff above down
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
					}
					pField[px] = 0;
				}
			}
			vLines.clear();
		}

		//Display Frame
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
		
	}

	//cant use cout when using screen buffer so cleans out console
	CloseHandle(hConsole);
	cout << "Game Over!! Score: " << nScore << endl;
	system("pause");
	
	
	return 0;
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
