#include <windows.h>
#include <conio.h>
using namespace std;

//设置编辑器的能容纳的文本大小，最多10行80列
const int MAX_ROWS = 10;
const int MAX_COLS = 80;

CHAR_INFO buffer[MAX_ROWS][MAX_COLS];//二维字符数组，用来构成文本
COORD cursorPos;//专门坐标位置的数据结构
CONSOLE_SCREEN_BUFFER_INFO csbi;//控制台屏幕缓冲区信息

//初始化二维字符数组
void initBuffer() {
	for (int i = 0; i < MAX_ROWS; i++) {
		for (int j = 0; j < MAX_COLS; j++) {
			buffer[i][j].Char.AsciiChar = ' ';
			buffer[i][j].Attributes = FOREGROUND_BLUE;
		}
	}
}

//初始化坐标数据结构
void initCursor() {
	cursorPos.X = 0;
	cursorPos.Y = 0;
}

//把构成文本的二维数组打印到控制台
void drawBuffer(HANDLE console) {
	SMALL_RECT rect;
	rect.Top = 0;
	rect.Left = 0;
	rect.Bottom = MAX_ROWS - 1;
	rect.Right = MAX_COLS - 1;

	WriteConsoleOutputA(console, (CHAR_INFO*)buffer, { MAX_COLS, MAX_ROWS }, { 0, 0 }, &rect);
}

//处理键盘按键
void ProcessKeys(int key, HANDLE console) {
	if (key == 0 || key == 224) {  //如果是方向键和删除键
		key = _getch();
		switch (key) {
		case 72: // 上
			if (cursorPos.Y != 0)
				cursorPos.Y--;
			SetConsoleCursorPosition(console, cursorPos);//移动光标到指定位置
			break;
		case 80: // 下
			cursorPos.Y++;
			SetConsoleCursorPosition(console, cursorPos);//移动光标到指定位置
			break;
		case 75: // 左
			if (cursorPos.X != 0)
				cursorPos.X--;
			if (cursorPos.Y != 0 && cursorPos.X == 0)
				cursorPos.Y--;
			SetConsoleCursorPosition(console, cursorPos);
			break;
		case 77: // 右
			if (cursorPos.X != MAX_COLS)//如果一行的字符数没超过最大列数，那么光标就右移一位
				cursorPos.X++;
			else
			{
				cursorPos.X = 0;//如果超过了，就换行
				cursorPos.Y++;
			}
			SetConsoleCursorPosition(console, cursorPos);
			break;
		case 83: //删除键
			buffer[cursorPos.Y][cursorPos.X - 1].Char.AsciiChar = ' ';
			buffer[cursorPos.Y][cursorPos.X - 1].Attributes = FOREGROUND_BLUE;
			if (cursorPos.X != 0)
				cursorPos.X--;
			if (cursorPos.Y != 0 && cursorPos.X == 0)
				cursorPos.Y--;
			SetConsoleCursorPosition(console, cursorPos);
			drawBuffer(console);
			break;
		}
	}
	else if (key == 13) {//如果是换行键
		cursorPos.X = 0;
		cursorPos.Y++;
		SetConsoleCursorPosition(console, cursorPos);	//在输出设备上定位光标
	}
	else if (key == 8) {  // 如果是回退键（Backspace键）
		buffer[cursorPos.Y][cursorPos.X - 1].Char.AsciiChar = ' ';
		buffer[cursorPos.Y][cursorPos.X - 1].Attributes = FOREGROUND_BLUE;
		if (cursorPos.X != 0)
			cursorPos.X--;
		if (cursorPos.Y != 0 && cursorPos.X == 0)
			cursorPos.Y--;
		SetConsoleCursorPosition(console, cursorPos);
		drawBuffer(console);
	}
	else {//如果是输入文本
		buffer[cursorPos.Y][cursorPos.X].Char.AsciiChar = (char)key;
		buffer[cursorPos.Y][cursorPos.X].Attributes = FOREGROUND_BLUE;
		drawBuffer(console);
		cursorPos.X++;
		SetConsoleCursorPosition(console, cursorPos);
		drawBuffer(console);
	}
}

void input(HANDLE console) {

	while (true)
	{
		GetConsoleScreenBufferInfo(console, &csbi);	//获取屏幕缓冲的光标位置,然后把当前光标位置赋给COORD
		cursorPos.X = csbi.dwCursorPosition.X;			//得到坐标x的值
		cursorPos.Y = csbi.dwCursorPosition.Y;			//y值

		if (_kbhit()) {
			int key = _getch();
			ProcessKeys(key, console);
		}
	}
}

int main() {
	initBuffer();
	initCursor();
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	input(console);
}
