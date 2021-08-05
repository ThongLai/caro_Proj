#include <iostream>
#include <conio.h>
#include <windows.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <iomanip>
using namespace std;


//Hằng số
#define SCREEN_WIDTH 120 //Chiều dài màn hình
#define SCREEN_HEIGHT 30 //Chiều rộng màn hình
#define BOARD_SIZE 12 // Kích thức ma trận bàn cờ
#define SQUARE_W 4 // Chiều dài 1 ô
#define SQUARE_H 2 // Chiều rộng 1 ô
#define LEFT (SCREEN_WIDTH - (BOARD_SIZE*SQUARE_W + 1)) / 2 // Tọa độ trái màn hình bàn cờ
#define TOP (SCREEN_HEIGHT - (BOARD_SIZE*SQUARE_H + 1)) / 3 // Tọa độ trên màn hình bàn cờ

// Khai báo kiểu dữ liệu
struct _POINT { int x, y, c; }; // x: tọa độ dòng, y: tọa độ cột, c: đánh dấu
_POINT _A[BOARD_SIZE][BOARD_SIZE]; //Ma trận bàn cờ
string MENU[] = {
	"CARO GAME",
	"Enter - Play.",
	"C - Continue.",
	"L - Load Game.",
	"I - Instruction.",
	"Esc - Exit.",
};
string FINISH[] = { "Player X wins!", "It's a tie!", "Player O wins!" };
vector <string> GAMESAVES;
bool _TURN; //true là lượt người thứ nhất và false là lượt người thứ hai
int _COMMAND; // Biến nhận giá trị phím người dùng nhập
int _X, _Y; //Tọa độ hiện hành trên màn hình bàn cờ
int _TURN_COUNT;
bool LOAD;
bool AUTOSAVE;
clock_t START_TIME;
clock_t TIME;


//VIEW FUNCTIONS
void FixConsoleWindow();
void GotoXY(int x, int y);
void DrawMenu();
void DrawBoard();
void DrawDisplay();
int ProcessFinish(int pWhoWin);
int AskContinue();


//MODEL FUNCTIONS
void ResetData();
void SetUpGameSaves();
void AutoSaveGame();
void DeleteSave();
void GabageCollect();
int TestBoard();
int CheckBoard(int pX, int pY);
bool isWin();
void outputTime();

//CONTROL FUNCTIONS
void PlayGame();
void Continue();
void LoadGame();
void Instruction();
void ExitGame();

void StartGame();
void SaveGame();
void PauseGame();
void MoveRight();
void MoveLeft();
void MoveDown();
void MoveUp();


void main()
{
	//FixConsoleWindow();
	SetUpGameSaves();

	while (1) {
		system("cls");
		DrawMenu();
		do { _COMMAND = toupper(_getch()); } 		
		while (!(_COMMAND == 13 || _COMMAND == 'C' || _COMMAND == 'L' || _COMMAND == 'I' || _COMMAND == 27));

		switch (_COMMAND)
		{
		case 13: PlayGame(); break;
		case 'C': Continue(); break;
		case 'L': LoadGame(); break;
		case 'I': Instruction(); break;
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
	GotoXY((SCREEN_WIDTH - MENU[0].size()) / 2, (SCREEN_HEIGHT) / 4 - 3);
	cout << MENU[0];

	int n = sizeof(MENU) / sizeof(string);
	int j = 0;
	for (int i = 1; i < n; i++) {
		if (i == 2 && !AUTOSAVE)
			continue;
		GotoXY((SCREEN_WIDTH - MENU[i].size()) / 2, (SCREEN_HEIGHT) / 4 + j++ * 2 + 1);
		cout << MENU[i];
	}
	GotoXY(0, 0);
}
void DrawBoard() {
	system("cls");
	for (int i = 0; i <= BOARD_SIZE; i++)
		for (int j = 0; j <= BOARD_SIZE; j++) {
			GotoXY(LEFT + SQUARE_W * i, TOP + SQUARE_H * j);
			cout << ".";
			if (_A[i][j].c && j < BOARD_SIZE && i < BOARD_SIZE) {
				GotoXY(_A[i][j].x, _A[i][j].y);
				cout << (_A[i][j].c == -1 ? "X" : "O");
			}
		}

	// Nhảy tới vị trí thích hợp để in chuỗi chú thích
	int X = _A[BOARD_SIZE / 10][BOARD_SIZE - 1].x + 5, Y = _A[BOARD_SIZE / 10][BOARD_SIZE - 1].y;
	
	GotoXY(X, Y);
	cout << "TIME: ";
	GotoXY(X, Y + SQUARE_H);
	cout << "TURNS: ";
	GotoXY(X, Y + SQUARE_H * 2);
	cout << "Waiting: ";

	GotoXY(X, Y + SQUARE_H * 5);
	cout << "P - Pause.";
	GotoXY(X, Y + SQUARE_H * 6);
	cout << "T - Save Game.";
	GotoXY(X, Y + SQUARE_H * 7);
	cout << "Esc - Back to Menu.";
	DrawDisplay();
	GotoXY(_X, _Y);
}
void DrawDisplay()
{
	// Nhảy tới vị trí thích hợp để hiện thị chỉ số cho người chơi.
	GotoXY(_A[BOARD_SIZE / 10][BOARD_SIZE - 1].x + 11, _A[BOARD_SIZE / 10][BOARD_SIZE - 1].y);
	cout << setfill('0') << setw(2) << TIME / 3600 << ":" << setfill('0') << setw(2) << (TIME / 60) % 60 << ":" << setfill('0') << setw(2) << TIME % 60 << endl;
	GotoXY(_A[BOARD_SIZE / 10 + 1][BOARD_SIZE - 1].x + 12, _A[BOARD_SIZE / 10 + 1][BOARD_SIZE - 1].y);
	cout << _TURN_COUNT;
	GotoXY(_A[BOARD_SIZE / 10 + 2][BOARD_SIZE - 1].x + 14, _A[BOARD_SIZE / 10 + 2][BOARD_SIZE - 1].y);
	cout << (_TURN ? "X" : "O");
	GotoXY(_X, _Y);
}
int ProcessFinish(int pWhoWin) {
	if (pWhoWin != 2) {
		if (TOP >= 2)
			GotoXY((SCREEN_WIDTH - FINISH[1 + pWhoWin].size()) / 2, TOP - 2); // Nhảy tới vị trí thích hợp để in chuỗi thắng/thua/hòa
		else
			GotoXY((SCREEN_WIDTH - FINISH[1 + pWhoWin].size()) / 2, SCREEN_HEIGHT / 2 - 2); // Nhảy tới vị trí thích hợp để in chuỗi thắng/thua/hòa
		cout << FINISH[1 + pWhoWin];
		GotoXY(_X, _Y); // Trả về vị trí hiện hành của con trỏ màn hình bàn cờ
	}
	else
		_TURN = !_TURN; // Đổi lượt nếu không có gì xảy ra

	return pWhoWin;
}
int AskContinue() {
	if(TOP + BOARD_SIZE * SQUARE_H + 4 < SCREEN_HEIGHT)
		GotoXY((SCREEN_WIDTH - 34) / 2, TOP + BOARD_SIZE * SQUARE_H + 2); // Nhảy tới vị trí thích hợp để in chuỗi thắng/thua/hòa
	else
		GotoXY((SCREEN_WIDTH - 34) / 2, SCREEN_HEIGHT / 2 + 2); // Nhảy tới vị trí thích hợp để in chuỗi thắng/thua/hòa
	cout << "Play again? - Y / Back to Menu - Esc";

	do { _COMMAND = toupper(_getch()); } while (!(_COMMAND == 'Y' || _COMMAND == 27));
	return _COMMAND;
}


//MODEL FUNCTIONS
void ResetData() {

	if (LOAD) { LOAD = false; return; }
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			_A[i][j].x = SQUARE_W * j + LEFT + SQUARE_W / 2; // Trùng với hoành độ màn hình bàn cờ
			_A[i][j].y = SQUARE_H * i + TOP + SQUARE_H / 2; // Trùng với tung độ màn hình bàn cờ
			_A[i][j].c = 0; // 0 nghĩa là chưa ai đánh dấu, nếu đánh dấu phải theo quy 
			//định như sau: -1 là lượt true đánh, 1 là lượt false đánh					
		}
	}

	_TURN = true; _COMMAND = -1; // Gán lượt và phím mặc định
	_X = _A[0][0].x; _Y = _A[0][0].y; // Thiết lập lại tọa độ hiện hành ban đầu
	GotoXY(_X, _Y);
	_TURN_COUNT = 0;
	START_TIME = clock() / CLOCKS_PER_SEC;
	TIME = 0;
}
void SetUpGameSaves()
{
	ifstream fin;
	string file_name = "SAVE .txt";
	stringstream ss;

	while (1) {
		ss << GAMESAVES.size() + 1;
		fin.open(file_name.substr().insert(5, ss.str()), ios::in);
		if (!fin) break;
		fin.close();
		GAMESAVES.push_back(file_name.substr().insert(5, ss.str()));
		ss.str("");
	}
	LOAD = false;
	fin.open("SAVE AUTO.txt", ios::in);
	if (fin) { AUTOSAVE = true;	fin.close(); }
	else AUTOSAVE = false;
}
void AutoSaveGame()
{
	ofstream fout("SAVE AUTO.txt", ios::out);

	fout << TIME << endl;
	fout << _TURN_COUNT << endl;
	fout << _TURN << endl;
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
			if (_A[i][j].c)
				fout << _A[i][j].x << " " << _A[i][j].y << " " << _A[i][j].c << " ";
	fout.close();
	AUTOSAVE = true;
}
void DeleteSave()
{
	system("cls");
	GotoXY((SCREEN_WIDTH - 23) / 2, (SCREEN_HEIGHT) / 2 - 10);
	cout << "Select a file to delete";

	if (!GAMESAVES.size()) {
		GotoXY((SCREEN_WIDTH - 6) / 2, (SCREEN_HEIGHT) / 2 - 6);
		cout << "EMPTY!";
	}
	for (int i = 0; i < GAMESAVES.size(); i++) {
		GotoXY((SCREEN_WIDTH - 10) / 2, (SCREEN_HEIGHT) / 2 - 6 + i * 2);
		cout << i + 1 << " - " << GAMESAVES[i].substr(0, 6);
	}
	GotoXY((SCREEN_WIDTH - 10) / 2, (SCREEN_HEIGHT) / 2 - 6 + (GAMESAVES.size() + 2) * 2);
	cout << "Esc - Back";
	GotoXY(0, 0);
	do { _COMMAND = toupper(_getch()); } while (!((_COMMAND - '0' >= 1 && _COMMAND - '0' <= GAMESAVES.size()) || _COMMAND == 27));
	if (_COMMAND == 27)
		return;

	int index = _COMMAND - '0';
	system("cls");
	GotoXY((SCREEN_WIDTH - 39) / 2, (SCREEN_HEIGHT) / 2 - 10);
	cout << "Are you sure you want to delete " << GAMESAVES[_COMMAND - '0' - 1].substr(0, 6) << "?";
	GotoXY((SCREEN_WIDTH - 18) / 2, (SCREEN_HEIGHT) / 2);
	cout << "(Y - Yes / N - No)";
	GotoXY(0, 0);
	do { _COMMAND = toupper(_getch()); } while (!(_COMMAND == 'Y' || _COMMAND == 'N'));
	if (_COMMAND == 'N')
		return;

	system("cls");
	remove(GAMESAVES[index - 1].c_str());
	for (int i = index; i < GAMESAVES.size(); i++) {
		rename(GAMESAVES[i].c_str(), GAMESAVES[i - 1].c_str());
	}
	GAMESAVES.pop_back();
	GotoXY((SCREEN_WIDTH - 8) / 2, (SCREEN_HEIGHT) / 2 - 10);
	cout << "DELETED!";
	GotoXY(0, 0);
	Sleep(1000);
	return;
}
void GabageCollect()
{

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
	int i = (pY - TOP - SQUARE_H / 2) / SQUARE_H, j = (pX - LEFT - SQUARE_W / 2) / SQUARE_W;
	if (_A[i][j].c != 0)
		return 0;

	if (_TURN == true) _A[i][j].c = -1; // Nếu lượt hiện hành là true thì c = -1
	else _A[i][j].c = 1; // Nếu lượt hiện hành là false thì c = 1
	return _A[i][j].c;
}
bool isWin() {
	int y_cur = (_X - LEFT - SQUARE_W / 2) / SQUARE_W, x_cur = (_Y - TOP - SQUARE_H / 2) / SQUARE_H;
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
void outputTime()
{
	if (clock() / CLOCKS_PER_SEC - START_TIME - TIME < 1)
		return;
	TIME = clock() / CLOCKS_PER_SEC - START_TIME;
	GotoXY(_A[BOARD_SIZE / 10][BOARD_SIZE - 1].x + 11, _A[BOARD_SIZE / 10][BOARD_SIZE - 1].y);
	cout << setfill('0') << setw(2) << TIME / 3600 << ":" << setfill('0') << setw(2) << (TIME / 60) % 60 << ":" << setfill('0') << setw(2) << TIME % 60 << endl;
	GotoXY(_X, _Y);
}


//CONTROL FUNCTIONS
void PlayGame()
{
	StartGame();
	bool validEnter = true;
	while (1)
	{
		do { 
			while (!_kbhit())
				outputTime();
			_COMMAND = toupper(_getch());
		}
		while (!(_COMMAND == 27 || _COMMAND == 'A' || _COMMAND == 'W' || _COMMAND == 'S' || _COMMAND == 'D' || _COMMAND == 13 || _COMMAND == 'P' || _COMMAND == 'T'));

		switch (_COMMAND)
		{
		case 27: AutoSaveGame(); return;
		case 'A': MoveLeft(); break;
		case 'W': MoveUp(); break;
		case 'S': MoveDown(); break;
		case 'D': MoveRight(); break;
		case 'T': SaveGame(); break;
		case 'P': PauseGame(); break;
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
			DrawDisplay();
		}
	}
}
void Continue()
{
	if (!AUTOSAVE) return;
	system("cls");
	GotoXY((SCREEN_WIDTH - 14) / 2, (SCREEN_HEIGHT) / 2 - 10);
	cout << "Loading game...";

	ifstream fin("SAVE AUTO.txt", ios::in);
	if (!fin) return;

	ResetData();
	fin >> TIME >> _TURN_COUNT >> _TURN;
	START_TIME = clock() / CLOCKS_PER_SEC - TIME;
	int i{ 0 }, j{ 0 };
	_POINT p{ 0,0,0 };
	while (fin >> p.x >> p.y >> p.c) {
		i = (p.y - TOP - SQUARE_H / 2) / SQUARE_H;
		j = (p.x - LEFT - SQUARE_W / 2) / SQUARE_W;
		_A[i][j].c = p.c;
	}
	fin.close();
	AUTOSAVE = false;
	LOAD = true;

	Sleep(1000);
	PlayGame();
}
void LoadGame() {
	while (1) {
		system("cls");
		GotoXY((SCREEN_WIDTH - 9) / 2, (SCREEN_HEIGHT) / 2 - 10);
		cout << "Load Game";

		if (!GAMESAVES.size()) {
			GotoXY((SCREEN_WIDTH - 6) / 2, (SCREEN_HEIGHT) / 2);
			cout << "EMPTY!";
			Sleep(1000);
			return;
		}
		for (int i = 0; i < GAMESAVES.size(); i++) {
			GotoXY((SCREEN_WIDTH - 10) / 2, (SCREEN_HEIGHT) / 2 - 6 + i * 2);
			cout << i + 1 << " - " << GAMESAVES[i].substr(0, 6);
		}
		GotoXY((SCREEN_WIDTH - 15) / 2, (SCREEN_HEIGHT) / 2 - 6 + (GAMESAVES.size() + 2) * 2);
		cout << "D - Delete Save";
		GotoXY((SCREEN_WIDTH - 18) / 2, (SCREEN_HEIGHT) / 2 - 6 + (GAMESAVES.size() + 3) * 2);
		cout << "Esc - Back to Menu";
		GotoXY(0, 0);
		do { _COMMAND = toupper(_getch()); } while (!((_COMMAND - '0' >= 1 && _COMMAND - '0' <= GAMESAVES.size()) || _COMMAND == 27 || _COMMAND == 'D'));
		if (_COMMAND == 27)
			return;
		if (_COMMAND == 'D') {
			DeleteSave();
			continue;
		}

		system("cls");
		GotoXY((SCREEN_WIDTH - 14) / 2, (SCREEN_HEIGHT) / 2 - 10);
		cout << "Loading game...";

		ifstream fin(GAMESAVES[_COMMAND - '0' - 1], ios::in);
		if (!fin) {
			GotoXY((SCREEN_WIDTH - 6) / 2, (SCREEN_HEIGHT) / 2);
			cout << "ERROR!";
			GotoXY((SCREEN_WIDTH - 20) / 2, (SCREEN_HEIGHT) / 2 + 2);
			cout << "Please try again :-(";
			Sleep(1000);
			system("cls");
			continue;
		}

		ResetData();
		fin >> TIME >> _TURN_COUNT >> _TURN;
		START_TIME = clock() / CLOCKS_PER_SEC - TIME;
		int i{ 0 }, j{ 0 };
		_POINT p{ 0,0,0 };
		while (fin >> p.x >> p.y >> p.c) {
			i = (p.y - TOP - SQUARE_H / 2) / SQUARE_H;
			j = (p.x - LEFT - SQUARE_W / 2) / SQUARE_W;
			_A[i][j].c = p.c;
		}
		fin.close();
		LOAD = true;

		Sleep(1000);
		PlayGame();
		return;
	}
}
void Instruction()
{
	system("cls");
	GotoXY((SCREEN_WIDTH - 11) / 2, (SCREEN_HEIGHT) / 4 - 3);
	cout << "HOW TO PLAY";


	GotoXY((SCREEN_WIDTH - 83) / 2, (SCREEN_HEIGHT) / 2 - 6);
	cout << "- Use <W / A / S / D> to move the cursor up / left / down / right around the board.";

	GotoXY((SCREEN_WIDTH - 31) / 2, (SCREEN_HEIGHT) / 2 - 4);
	cout << "- Press <Enter> to make a move.";

	GotoXY((SCREEN_WIDTH - 32) / 2, (SCREEN_HEIGHT) / 2 - 2);
	cout << "- Press <T> to save the process.";

	GotoXY((SCREEN_WIDTH - 10) / 2, (SCREEN_HEIGHT) / 2 + 2);
	cout << "Esc - Back";
	GotoXY(0, 0);
	do { _COMMAND = toupper(_getch()); } while (!(_COMMAND == 27));
}
void ExitGame() {
	system("cls");
	//Có thể lưu game trước khi exit
	GabageCollect();

	GotoXY((SCREEN_WIDTH - 22) / 2, (SCREEN_HEIGHT) / 2 - 2);
	cout << "THANKS FOR PLAYING :-)";
	Sleep(1000);
	system("cls");
	exit(0);
}

void StartGame() {
	system("cls");
	ResetData(); // Khởi tạo dữ liệu gốc
	DrawBoard(); // Vẽ màn hình game
}
void SaveGame() {
	int pause_time = clock() / CLOCKS_PER_SEC;
	while (1) {
		system("cls");
		GotoXY((SCREEN_WIDTH - 9) / 2, (SCREEN_HEIGHT) / 2 - 10);
		cout << "Save Game";

		if (!GAMESAVES.size()) {
			GotoXY((SCREEN_WIDTH - 6) / 2, (SCREEN_HEIGHT) / 2 - 6);
			cout << "EMPTY!";
		}
		for (int i = 0; i < GAMESAVES.size(); i++) {
			GotoXY((SCREEN_WIDTH - 10) / 2, (SCREEN_HEIGHT) / 2 - 6 + i * 2);
			cout << i + 1 << " - " << GAMESAVES[i].substr(0, 6);
		}
		GotoXY((SCREEN_WIDTH - 12) / 2, (SCREEN_HEIGHT) / 2 - 6 + (GAMESAVES.size() + 2) * 2);
		cout << "N - New Save";
		GotoXY((SCREEN_WIDTH - 15) / 2, (SCREEN_HEIGHT) / 2 - 6 + (GAMESAVES.size() + 3) * 2);
		cout << "D - Delete Save.";
		GotoXY((SCREEN_WIDTH - 10) / 2, (SCREEN_HEIGHT) / 2 - 6 + (GAMESAVES.size() + 4) * 2);
		cout << "Esc - Back";
		GotoXY(0, 0);
		do { _COMMAND = toupper(_getch()); } 		
		while (!((_COMMAND - '0' >= 1 && _COMMAND - '0' <= GAMESAVES.size()) || _COMMAND == 27 || _COMMAND == 'N' || _COMMAND == 'D'));
		if (_COMMAND == 27)
			break;
		if (_COMMAND == 'D') {
			DeleteSave();
			continue;
		}

		system("cls");
		GotoXY((SCREEN_WIDTH - 17) / 2, (SCREEN_HEIGHT) / 2 - 10);
		cout << "Saving process...";

		ofstream fout;
		if (_COMMAND != 'N') {
			fout.open(GAMESAVES[_COMMAND - '0' - 1], ios::out);
		}
		else {
			stringstream ss;
			ss << GAMESAVES.size() + 1;
			fout.open("SAVE " + ss.str() + ".txt", ios::out);
			GAMESAVES.push_back("SAVE " + ss.str() + ".txt");
		}
		if (!fout) {
			GotoXY((SCREEN_WIDTH - 6) / 2, (SCREEN_HEIGHT) / 2);
			cout << "ERROR!";
			GotoXY((SCREEN_WIDTH - 20) / 2, (SCREEN_HEIGHT) / 2 + 2);
			cout << "Please try again :-(";
			GAMESAVES.pop_back();
			Sleep(1000);
			system("cls");
			continue;
		}

		fout << TIME << endl;
		fout << _TURN_COUNT << endl;
		fout << _TURN << endl;
		for (int i = 0; i < BOARD_SIZE; i++)
			for (int j = 0; j < BOARD_SIZE; j++)
				if (_A[i][j].c)
					fout << _A[i][j].x << " " << _A[i][j].y << " " << _A[i][j].c << " ";
		fout.close();

		GotoXY((SCREEN_WIDTH - 8) / 2, (SCREEN_HEIGHT) / 2);
		cout << "SUCCEED!";
		GotoXY(0, 0);
		Sleep(1000);
		system("cls");
		break;
	}
	START_TIME += clock() / CLOCKS_PER_SEC - pause_time;
	DrawBoard();
}
void PauseGame()
{
	int pause_time = clock() / CLOCKS_PER_SEC;
	system("cls");
	GotoXY((SCREEN_WIDTH - 5) / 2, (SCREEN_HEIGHT) / 4 - 3);
	cout << "PAUSE";

	GotoXY((SCREEN_WIDTH - 20) / 2, (SCREEN_HEIGHT) / 2 - 4);
	cout << "P - Back to the game";

	GotoXY(0, 0);
	do { _COMMAND = toupper(_getch()); } while (!(_COMMAND == 'P'));
	START_TIME += clock() / CLOCKS_PER_SEC - pause_time;
	GotoXY(_X, _Y);
	DrawBoard();
}
void MoveRight() {
	if (_X < _A[BOARD_SIZE - 1][BOARD_SIZE - 1].x)
	{
		_X += SQUARE_W;
		GotoXY(_X, _Y);
	}
}
void MoveLeft() {
	if (_X > _A[0][0].x) {
		_X -= SQUARE_W;
		GotoXY(_X, _Y);
	}
}
void MoveDown() {
	if (_Y < _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y)
	{
		_Y += SQUARE_H;
		GotoXY(_X, _Y);
	}
}
void MoveUp() {
	if (_Y > _A[0][0].y) {
		_Y -= SQUARE_H;
		GotoXY(_X, _Y);

	}
}