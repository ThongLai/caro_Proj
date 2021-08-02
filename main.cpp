#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <stdio.h>
using namespace std;


//Hằng số
#define SCREEN_WIDTH 120 //Chiều dài màn hình
#define SCREEN_HEIGHT 30 //Chiều rộng màn hình
#define BOARD_SIZE 5 // Kích thức ma trận bàn cờ
#define LEFT (SCREEN_WIDTH - (BOARD_SIZE*4 + 1)) / 2 // Tọa độ trái màn hình bàn cờ
#define TOP (SCREEN_HEIGHT - (BOARD_SIZE*2 + 1)) / 3 // Tọa độ trên màn hình bàn cờ


// Khai báo kiểu dữ liệu
struct _POINT { int x, y, c; }; // x: tọa độ dòng, y: tọa độ cột, c: đánh dấu
_POINT _A[BOARD_SIZE][BOARD_SIZE]; //Ma trận bàn cờ
bool _TURN; //true là lượt người thứ nhất và false là lượt người thứ hai
int _COMMAND; // Biến nhận giá trị phím người dùng nhập
int _X, _Y; //Tọa độ hiện hành trên màn hình bàn cờ
int _TURN_COUNT;
string MENU[] = {
	"CARO",
	"Enter - Play.",
	"C - Continue.",
	"I - Instruction.",
	"S - Settings.",
	"Esc - Exit.",
};
string FINISH[] = { "Player X wins!", "It's a tie!", "Player O wins!" };


//VIEW FUNCTIONS
void FixConsoleWindow();
void GotoXY(int x, int y);
void DrawMenu();
void DrawBoard(int pSize);
void DrawDisplay();
int ProcessFinish(int pWhoWin);
int AskContinue();


//MODEL FUNCTIONS
void ResetData();
void GabageCollect();
int TestBoard();
int CheckBoard(int pX, int pY);
bool isWin();


//CONTROL FUNCTIONS
void PlayGame();
void StartGame();
void SaveGame();
void ExitGame();

void MoveRight();
void MoveLeft();
void MoveDown();
void MoveUp();


void main()
{
	FixConsoleWindow();

	while (1) {
		system("cls");
		DrawMenu();
		do { _COMMAND = toupper(_getch()); } 
		while (!(_COMMAND == 13 || _COMMAND == 'C' || _COMMAND == 'S' || _COMMAND == 27));

		switch (_COMMAND)
		{
		case 13: PlayGame(); break;
		case 'C': break;
		case 'S': SaveGame(); break;
		case 27: ExitGame(); return;
		}
	}
}


//VIEW FUNCTIONS
void FixConsoleWindow() {
	HWND consoleWindow = GetConsoleWindow();
	LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
	SetWindowLong(consoleWindow, GWL_STYLE, style);
}
void GotoXY(int x, int y) {
	COORD coord = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void DrawMenu()
{
	GotoXY((SCREEN_WIDTH - MENU[0].size()) / 2, (SCREEN_HEIGHT - 8) / 4 - 3);
	cout << MENU[0];

	int n = sizeof(MENU) / sizeof(string);
	for (int i = 1; i < n; i++) {
		GotoXY((SCREEN_WIDTH - MENU[i].size()) / 2, (SCREEN_HEIGHT - 8) / 4 + i * 2 - 1);
		cout << MENU[i];
	}
	GotoXY(_X, _Y);
}
void DrawBoard(int pSize) {
	for (int i = 0; i <= pSize; i++)
		for (int j = 0; j <= pSize; j++) {
			GotoXY(LEFT + 4 * i, TOP + 2 * j);
			cout << ".";
		}

	// Nhảy tới vị trí thích hợp để in chuỗi chú thích
	GotoXY(_A[BOARD_SIZE / 10][BOARD_SIZE - 1].x + 5, _A[BOARD_SIZE / 10][BOARD_SIZE - 1].y);
	cout << "TURNS: ";
	GotoXY(_A[BOARD_SIZE / 10 + 1][BOARD_SIZE - 1].x + 5, _A[BOARD_SIZE / 10 + 1][BOARD_SIZE - 1].y);
	cout << "Waiting: ";
	GotoXY(_A[BOARD_SIZE / 10 + 2][BOARD_SIZE - 1].x + 5, _A[BOARD_SIZE / 10 + 3][BOARD_SIZE - 1].y);
	cout << "P - Pause.";
	GotoXY(_A[BOARD_SIZE / 10 + 3][BOARD_SIZE - 1].x + 5, _A[BOARD_SIZE / 10 + 4][BOARD_SIZE - 1].y);
	cout << "Esc - Exit.";
	GotoXY(_X, _Y);
}
void DrawDisplay()
{
	// Nhảy tới vị trí thích hợp để hiện thị chỉ số cho người chơi.
	GotoXY(_A[BOARD_SIZE / 10][BOARD_SIZE - 1].x + 12, _A[BOARD_SIZE / 10][BOARD_SIZE - 1].y);
	cout << _TURN_COUNT;
	GotoXY(_A[BOARD_SIZE / 10 + 1][BOARD_SIZE - 1].x + 14, _A[BOARD_SIZE / 10 + 1][BOARD_SIZE - 1].y);
	cout << (_TURN ? "X" : "O");
	GotoXY(_X, _Y);
}
int ProcessFinish(int pWhoWin) {
	if (pWhoWin != 2) {
		GotoXY((SCREEN_WIDTH - FINISH[1 + pWhoWin].size()) / 2, TOP - 2); // Nhảy tới vị trí thích hợp để in chuỗi thắng/thua/hòa
		cout << FINISH[1 + pWhoWin];
		GotoXY(_X, _Y); // Trả về vị trí hiện hành của con trỏ màn hình bàn cờ
	}
	else
		_TURN = !_TURN; // Đổi lượt nếu không có gì xảy ra

	return pWhoWin;
}
int AskContinue() {
	GotoXY((SCREEN_WIDTH - 34) / 2, TOP + BOARD_SIZE * 2 + 4); // Nhảy tới vị trí thích hợp để in chuỗi thắng/thua/hòa
	cout << "Play again? - Y / Back to Menu - Esc";

	do { _COMMAND = toupper(_getch()); } while (!(_COMMAND == 'Y' || _COMMAND == 27));
	return _COMMAND;
}


//MODEL FUNCTIONS
void ResetData() {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			_A[i][j].x = 4 * j + LEFT + 2; // Trùng với hoành độ màn hình bàn cờ
			_A[i][j].y = 2 * i + TOP + 1; // Trùng với tung độ màn hình bàn cờ
			_A[i][j].c = 0; // 0 nghĩa là chưa ai đánh dấu, nếu đánh dấu phải theo quy 
			//định như sau: -1 là lượt true đánh, 1 là lượt false đánh					
		}
	}
	_TURN = true; _COMMAND = -1; // Gán lượt và phím mặc định
	_X = _A[0][0].x; _Y = _A[0][0].y; // Thiết lập lại tọa độ hiện hành ban đầu
	GotoXY(_X, _Y);
	_TURN_COUNT = 0;
}
void GabageCollect()
{
	// Dọn dẹp tài nguyên nếu có khai báo con trỏ
}
int TestBoard()
{
		if (isWin())
			return (_TURN == true ? -1 : 1); // -1 nghĩa là lượt ‘true’ thắng
		else if (_TURN_COUNT == BOARD_SIZE * BOARD_SIZE)
			return 0; // Hòa
		else
			return 2; // 2 nghĩa là chưa ai thắng
		
}
int CheckBoard(int pX, int pY) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (_A[i][j].x == pX && _A[i][j].y == pY) {
				if (_A[i][j].c != 0)
					return 0;

				if (_TURN == true) _A[i][j].c = -1; // Nếu lượt hiện hành là true thì c = -1
				else _A[i][j].c = 1; // Nếu lượt hiện hành là false thì c = 1
				return _A[i][j].c;
			}
		}
	}
}
bool isWin() {
	int y_cur = (_X - LEFT - 2) / 4, x_cur = (_Y - TOP - 1) / 2;
	int x = x_cur, y = y_cur;
	int count = 1;
	while (y + 1 < BOARD_SIZE && _A[x_cur][y_cur].c == _A[x][y + 1].c) {
		count++;
		y++;
	}
	x = x_cur, y = y_cur;
	while (y - 1 >= 0 && _A[x_cur][y_cur].c == _A[x][y - 1].c) {
		count++;
		y--;
	}
	if (count >= 5)
		return true;

	count = 1;
	while (x + 1 < BOARD_SIZE && _A[x_cur][y_cur].c == _A[x + 1][y].c) {
		count++;
		x++;
	}
	x = x_cur, y = y_cur;
	while (x - 1 >= 0 && _A[x_cur][y_cur].c == _A[x - 1][y].c) {
		count++;
		x--;
	}
	if (count >= 5)
		return true;

	count = 1;
	while (x + 1 < BOARD_SIZE && y - 1 >= 0 && _A[x_cur][y_cur].c == _A[x + 1][y - 1].c) {
		count++;
		x++;
		y--;
	}
	x = x_cur, y = y_cur;
	while (x - 1 >= 0 && y + 1 < BOARD_SIZE && _A[x_cur][y_cur].c == _A[x - 1][y + 1].c) {
		count++;
		x--;
		y++;
	}
	if (count >= 5)
		return true;

	count = 1;
	while (x + 1 < BOARD_SIZE && y + 1 < BOARD_SIZE && _A[x_cur][y_cur].c == _A[x + 1][y + 1].c) {
		count++;
		x++;
		y++;
	}
	x = x_cur, y = y_cur;
	while (x - 1 >= 0 && y - 1 >= 0 && _A[x_cur][y_cur].c == _A[x - 1][y - 1].c) {
		count++;
		x--;
		y--;
	}
	if (count >= 5)
		return true;

	return false;
}


//CONTROL FUNCTIONS
void PlayGame()
{
	StartGame();
	bool validEnter = true;
	while (1)
	{
		DrawDisplay();
		do { _COMMAND = toupper(_getch()); } 		
		while (!(_COMMAND == 27 || _COMMAND == 'A' || _COMMAND == 'W' || _COMMAND == 'S' || _COMMAND == 'D' || _COMMAND == 13));

		switch (_COMMAND)
		{
		case 27: SaveGame(); return;
		case 'A': MoveLeft(); break;
		case 'W': MoveUp(); break;
		case 'S': MoveDown(); break;
		case 'D': MoveRight(); break;
		case 13:
			_TURN_COUNT++;
			switch (CheckBoard(_X, _Y)) {
			case -1:
				cout << "X"; break;
			case 1:
				cout << "O"; break;
			case 0: {validEnter = false; _TURN_COUNT--; }// Khi đánh vào ô đã đánh rồi
			}
			// Tiếp theo là kiểm tra và xử lý thắng/thua/hòa/tiếp tục
			if (validEnter == true) {
				if (abs(ProcessFinish(TestBoard())) <= 1) {
					if (AskContinue() == 27)
						return;
					else
						StartGame();
				}
			}
			validEnter = true; // Mở khóa 
		}
	}
}
void StartGame() {
	system("cls");
	ResetData(); // Khởi tạo dữ liệu gốc
	DrawBoard(BOARD_SIZE); // Vẽ màn hình game
}
void SaveGame() {
	system("cls");
	GotoXY((SCREEN_WIDTH - 17) / 2, (SCREEN_HEIGHT) / 2 - 10);
	cout << "Saving process...";
	Sleep(1000);
}
void ExitGame()
{
	system("cls");
	//Có thể lưu game trước khi exit
	SaveGame();
	GabageCollect();

	GotoXY((SCREEN_WIDTH - 22) / 2, (SCREEN_HEIGHT) / 2);
	cout << "THANKS FOR PLAYING :-)";
	Sleep(1000);
	system("cls");
	exit(0);
}
void MoveRight() {
	if (_X < _A[BOARD_SIZE - 1][BOARD_SIZE - 1].x)
	{
		_X += 4;
		GotoXY(_X, _Y);
	}
}
void MoveLeft() {
	if (_X > _A[0][0].x) {
		_X -= 4;
		GotoXY(_X, _Y);
	}
}
void MoveDown() {
	if (_Y < _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y)
	{
		_Y += 2;
		GotoXY(_X, _Y);
	}
}
void MoveUp() {
	if (_Y > _A[0][0].y) {
		_Y -= 2;
		GotoXY(_X, _Y);

	}
}