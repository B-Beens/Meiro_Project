/*====================================================================*/
/*	color.h	                                                   		  */
/*====================================================================*/
#include <stdio.h>
#include <conio.h>
#include <windows.h>

// 画面消去の定数
#define CLS_SPACE	((WORD)(0x20))
#define CLS_COLOR	((WORD)(0x0F))

/*====================================================================*/
/*	cursor	カーソル表示／非表示の切り替え							  */
/*====================================================================*/
void cursor(int flag)
{
	HANDLE hdt;	/* 標準出力デバイスハンドル */
	CONSOLE_CURSOR_INFO cci;
	hdt = GetStdHandle(STD_OUTPUT_HANDLE);	/* 標準出力デバイスハンドル取得 */
	GetConsoleCursorInfo(hdt, &cci);		// CONSOLE_CURSOR_INFO構造体の現在の状態を取得
	/* メンバ変数であるbVisibleがカーソルの表示・非表示を
	   制御する変数なので、これをFALSEにする事でカーソルを
	   非表示にできる */
	if (flag == FALSE)
		cci.bVisible = FALSE;
	else
		cci.bVisible = TRUE;

	// 変更した構造体情報をコンソールWindowにセットする
	SetConsoleCursorInfo(hdt, &cci);
}
/*====================================================================*/
/*	locate	座標設定処理											  */
/*		引数１：Ｘ軸　　引数２：Ｙ軸　　戻り値：なし				  */
/*		座標の原点は左上が（0,0)とする								  */
/*====================================================================*/
void locate(int x, int y)
{
	HANDLE hdt;	/* 標準出力デバイスハンドル */
	COORD pos;	/* 座標(X,Y) */

	hdt = GetStdHandle(STD_OUTPUT_HANDLE);	/* 標準出力デバイスハンドル取得 */

	pos.X = (SHORT)x;	/* Ｘ座標設定 */
	pos.Y = (SHORT)y;	/* Ｙ座標設定 */

	SetConsoleCursorPosition(hdt, pos);	/* 標準出力にカーソル位置設定 */
}

/*====================================================================*/
/*	color	文字色設定処理											  */
/*		引数１：前景色　　戻り値：なし				  				  */
/*====================================================================*/
void color(int col)
{
	HANDLE hdt;		/* 標準出力デバイスハンドル */
	WORD wcol = 0;	/* テキスト色(前景色) */

	hdt = GetStdHandle(STD_OUTPUT_HANDLE);	/* 標準出力デバイスハンドル取得 */

	switch (col)
	{
	case 1:/* 赤 */
		wcol = FOREGROUND_RED;
		break;
	case 2:/* 緑 */
		wcol = FOREGROUND_GREEN;
		break;
	case 3:/* 橙 */
		wcol = FOREGROUND_RED | FOREGROUND_GREEN;
		break;
	case 4:/* 青 */
		wcol = FOREGROUND_BLUE;
		break;
	case 5:/* 紫 */
		wcol = FOREGROUND_RED | FOREGROUND_BLUE;
		break;
	case 6:/* 水色 */
		wcol = FOREGROUND_GREEN | FOREGROUND_BLUE;
		break;
	case 7:/* 白 */
		wcol = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;
		break;
		/* ここから後は明るい色(INTENSITY 強調) */
	case 11:/* 赤 */
		wcol = FOREGROUND_RED | FOREGROUND_INTENSITY;
		break;
	case 12:/* 緑 */
		wcol = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		break;
	case 13:/* 黄 */
		wcol = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		break;
	case 14:/* 青 */
		wcol = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		break;
	case 15:/* 紫 */
		wcol = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		break;
	case 16:/* 水色 */
		wcol = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		break;
	case 17:/* 白 */
		wcol = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		break;
	default:/* 白 */
		wcol = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN;
		break;
	}
	SetConsoleTextAttribute(hdt, wcol);	/* 標準出力に文字色を設定 */
}

// すべての画面消去
void cls(void)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi = { 0 };
	COORD pos = { 0 };
	HANDLE hStderr;

	hStderr = GetStdHandle(STD_ERROR_HANDLE);
	GetConsoleScreenBufferInfo(hStderr, &csbi);
	SetConsoleCursorPosition(hStderr, pos);
	FillConsoleOutputCharacter(hStderr, CLS_SPACE, (csbi.dwSize.X * csbi.dwSize.Y), pos, NULL);
	FillConsoleOutputAttribute(hStderr, CLS_COLOR, (csbi.dwSize.X * csbi.dwSize.Y), pos, NULL);
}
