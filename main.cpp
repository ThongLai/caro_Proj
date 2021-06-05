#include <iostream>
#include <conio.h>
#include <Windows.h>
using namespace std;

//Hằng số
#define BOARD_SIZE 12 // Kích thức ma trận bàn cờ
#define LEFT 3 // Tọa độ trái màn hình bàn cờ
#define TOP 1 // Tọa độ trên màn hình bàn cờ
// Khai báo kiểu dữ liệu
struct _POINT { int x, y, c; }; // x: tọa độ dòng, y: tọa độ cột, c: đánh dấu
_POINT _A[BOARD_SIZE][BOARD_SIZE]; //Ma trận bàn cờ
bool _TURN; //true là lượt người thứ nhất và false là lượt người thứ hai
int _COMMAND; // Biến nhận giá trị phím người dùng nhập
int _X, _Y; //Tọa độ hiện hành trên màn hình bàn cờ
int _TURN_COUNT;

void FixConsoleWindow();
void GotoXY(int x, int y);
void ResetData();
void DrawBoard(int pSize);
void StartGame();
void GabageCollect();
void ExitGame();
int ProcessFinish(int pWhoWin);
int AskContinue();
int TestBoard();
int CheckBoard(int pX, int pY);
void MoveRight();
void MoveLeft();
void MoveDown();
void MoveUp();

bool isWin();

void main()
{
	FixConsoleWindow();
	StartGame();
	bool validEnter = true;
	while (1)
	{
		_COMMAND = toupper(_getch());
		if (_COMMAND == 27)
		{
			ExitGame();
			return;
		}
		else {
			if (_COMMAND == 'A') MoveLeft();
			else if (_COMMAND == 'W') MoveUp();
			else if (_COMMAND == 'S') MoveDown();
			else if (_COMMAND == 'D') MoveRight();
			else if (_COMMAND == 13) {// Người dùng đánh dấu trên màn hình bàn cờ
				_TURN_COUNT++;
				switch (CheckBoard(_X, _Y)) {
				case -1:
					cout << "X"; break;
				case 1:
					cout << "O"; break;
				case 0: validEnter = false; // Khi đánh vào ô đã đánh rồi
				}
				// Tiếp theo là kiểm tra và xử lý thắng/thua/hòa/tiếp tục
				if (validEnter == true) {
					switch (ProcessFinish(TestBoard())) {
					case -1: case 1: case 0:
						if (AskContinue() == 'N') {
							ExitGame(); return;
						}
						else
							StartGame();
					}
				}
				validEnter = true; // Mở khóa 
			}
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
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
void DrawBoard(int pSize) {
	for (int i = 0; i <= pSize; i++)
		for (int j = 0; j <= pSize; j++) {
			GotoXY(LEFT + 4 * i, TOP + 2 * j);
			cout << ".";
		}
}
int ProcessFinish(int pWhoWin) {
	GotoXY(0, _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y + 2); // Nhảy tới vị trí thích hợp để in chuỗi thắng/thua/hòa
	switch (pWhoWin) {
	case -1:
		cout << "Nguoi choi " << true << " da thang va nguoi choi " << false << " da thua\n";
		break;
	case 1:
		cout << "Nguoi choi " << false << " da thang va nguoi choi " << true << " da thua\n";
		break;
	case 0:
		cout << "Nguoi choi " << true << " va nguoi choi " << false << " da hoa\n";
		break;
	case 2:
		_TURN = !_TURN; // Đổi lượt nếu không có gì xảy ra
	}
	GotoXY(_X, _Y); // Trả về vị trí hiện hành của con trỏ màn hình bàn cờ
	return pWhoWin;
}
int AskContinue() {
	GotoXY(0, _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y + 4);
	cout << "Nhan 'y/n' de tiep tuc/dung: ";
	return toupper(_getch());
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
	_TURN_COUNT = 0;
}
void GabageCollect()
{
	// Dọn dẹp tài nguyên nếu có khai báo con trỏ
}
int TestBoard()
{
	if (_TURN_COUNT == BOARD_SIZE * BOARD_SIZE) return 0; // Hòa
	else {
		if (isWin())
			return (_TURN == true ? -1 : 1); // -1 nghĩa là lượt ‘true’ thắng
		else
			return 2; // 2 nghĩa là chưa ai thắng
	}
}
int CheckBoard(int pX, int pY) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (_A[i][j].x == pX && _A[i][j].y == pY && _A[i][j].c == 0) {
				if (_TURN == true) _A[i][j].c = -1; // Nếu lượt hiện hành là true thì c = -1
				else _A[i][j].c = 1; // Nếu lượt hiện hành là false thì c = 1
				return _A[i][j].c;
			}
		}
	}
	return 0;
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
void StartGame() {
	system("cls");
	ResetData(); // Khởi tạo dữ liệu gốc
	DrawBoard(BOARD_SIZE); // Vẽ màn hình game
}
void ExitGame() {
	system("cls");
	GabageCollect();
	//Có thể lưu game trước khi exit
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