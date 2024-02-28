#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<conio.h>
#include<time.h>

#include"AA.h"
#include"Tool.h"

/* fieldの状態 (field_state) */
#define ROAD    (0)
#define WALL    (1)
#define START_P (8)
#define GOAL_P  (9)

/* fieldの状態 (find) */
#define FOUND		(1)
#define NOT_FOUND	(0)

/* fieldの規模設定 (奇数限定) */
#define FIELD_HEIGHT (31)
#define FIELD_WIDTH  (31)

/* AAの規模設定 */
#define AA_HEIGHT (28)
#define AA_WIDTH  (28)

//キーコード設定
#define ENTER	0x0d
#define UP		0x48
#define RIGHT	0x4d
#define LEFT	0x4b
#define DOWN	0x50
#define KEY_M	0x6d

#define ERR (-1)
#define SAFE (0)

typedef struct FIELD_DATA {
	int field_state;//その位置のステータス
	int find;		//その位置の発見状況
}FIELD_STATE;

FIELD_STATE Field[FIELD_HEIGHT][FIELD_WIDTH] = { WALL };
FIELD_STATE Tenta_Field[FIELD_HEIGHT][FIELD_WIDTH] = { WALL };

//合成した描画用のAA配列
int mix_AA[AA_HEIGHT][AA_WIDTH];

/*ーーーーーーーーーー関数ーーーーーーーーーーー*/
int Random(int Number);
int Create_Wall_Check(int num, int x, int y);

void First_Drawing();
void Drawing_Map();
void Drawing_2D();
void Drawing_3D();

void Shokika();		/* 初期化してマップ再生成 */
void CreateMap();
void CreateMixAA(int type, int i1, int j1, int i2, int j2, int a1, int b1, int a2, int b2);
void Key();			/* 各キー入力の処理 */
void Move();		/* 上にのみ進む */
void TurnMap(int der);	/* 初期化してマップ再生成 */
int Game_State(); /* game終了するか */
/*ーーーーーーーーーーーーーーーーーーーーーーー*/

/* スタート地点とゴール地点の座標 */
int start_x;
int start_y;
int goal_x;
int goal_y;

int goal_cnt = 0;
int Game_Flag = 0;

/* PLAYER位置の座標 */
int Player_X;
int Player_Y;

int game_end = 0;

int main() {

	while (game_end == 0) {

		srand(time(NULL));

		First_Drawing();

		Shokika();

		while (Game_Flag == 0) {
			if (_kbhit()) {
				Key();
			}
			if (goal_cnt == 1) {
				Game_Flag = 1;
			}
		}

		//bs

		if (Game_State() == ERR) {
			game_end = 1;
		}
		else {
			Game_Flag = 0;
			goal_cnt = 0;
		}
	}

	return 0;
}

void Shokika() {

	int i, j;

	//道のみのMAP生成
	for (i = 0; i < FIELD_HEIGHT; i++) {
		for (j = 0; j < FIELD_WIDTH; j++)
		{
			Field[i][j].field_state = ROAD;
			Tenta_Field[i][j].field_state = ROAD;

			//発見状況の初期設定
			Field[i][j].find = NOT_FOUND;
			Tenta_Field[i][j].find = NOT_FOUND;
		}
	}

	//外壁の生成
	for (i = 0; i < FIELD_HEIGHT; i++)
	{
		Field[i][0].field_state = WALL;
		Field[i][FIELD_HEIGHT - 1].field_state = WALL;

		Tenta_Field[i][0].field_state = WALL;
		Tenta_Field[i][FIELD_HEIGHT - 1].field_state = WALL;
	}
	for (i = 0; i < FIELD_WIDTH; i++)
	{
		Field[0][i].field_state = WALL;
		Field[FIELD_WIDTH - 1][i].field_state = WALL;

		Tenta_Field[0][i].field_state = WALL;
		Tenta_Field[FIELD_WIDTH - 1][i].field_state = WALL;
	}

	//倒す棒の生成(棒倒し法)
	for (i = 2; i < FIELD_HEIGHT - 2; i += 2) {
		for (j = 2; j < FIELD_WIDTH - 2; j += 2)
		{
			Field[i][j].field_state = WALL;

			Tenta_Field[i][j].field_state = WALL;
		}
	}
	CreateMap();
	Drawing_3D();
}

void First_Drawing() {

	locate(22, 8);
	printf("～操作説明～");
	locate(22, 9);
	printf("↑：前進する");
	locate(22, 10);
	printf("←：左を向く");
	locate(22, 11);
	printf("→：右を向く");
	locate(22, 12);
	printf("↓：後ろを向く");
	locate(22, 13);
	printf("Ｍ：マップを見る");

	locate(40, 18);
	printf("Enter を押す...");
	_getch();
}

void CreateMap() {

	int i, j;
	int ran, yn;
	int Flag;

	//倒していく棒の位置を探索
	for (i = 2; i < FIELD_HEIGHT - 2; i += 2) {
		for (j = 2; j < FIELD_WIDTH - 2; j += 2)
		{
			Flag = 0;

			//一番上の棒の行以外
			if (i != 2) {
				while (Flag == 0)
				{
					ran = Random(3);	/* 倒す方向 */
					yn = Create_Wall_Check(ran, j, i);	/* その方向に倒せるか */

					//その方向に倒せる場合
					if (yn == SAFE) {
						switch (ran) {
						case 0:		/* 左 */
							Field[i][j - 1].field_state = WALL;
							Flag = 1;
							break;
						case 1:		/* 下 */
							Field[i - 1][j].field_state = WALL;
							Flag = 1;
							break;
						case 2:		/* 右 */
							Field[i][j + 1].field_state = WALL;
							Flag = 1;
							break;
						}
					}
				}
			}
			//一番上の棒の行
			else {
				while (Flag == 0)
				{
					ran = Random(4);	/* 倒す方向 */
					yn = Create_Wall_Check(ran, j, i);	/* その方向に倒せるか */

					//その方向に倒せる場合
					if (yn == SAFE) {
						switch (ran) {
						case 0:		/* 左 */
							Field[i][j - 1].field_state = WALL;
							Flag = 1;
							break;
						case 1:		/* 下 */
							Field[i - 1][j].field_state = WALL;
							Flag = 1;
							break;
						case 2:		/* 右 */
							Field[i][j + 1].field_state = WALL;
							Flag = 1;
							break;
						case 3:		/* 上 */
							Field[i + 1][j].field_state = WALL;
							Flag = 1;
							break;
						}
					}
				}
			}
		}
	}
	Flag = 0;

	//スタート座標の生成
	while (Flag == 0)
	{
		start_x = (rand() % (FIELD_WIDTH - 2)) + 1;
		if (start_x % 2 == 1) {
			break;
		}
	}
	while (Flag == 0)
	{
		start_y = (rand() % (FIELD_HEIGHT - 2)) + 1;
		if (start_y % 2 == 1) {
			break;
		}
	}
	Field[start_y][start_x].field_state = START_P;

	/* PLAYERの初期位置の設定 */
	Player_X = start_x;
	Player_Y = start_y;

	/* PLAYER付近の発見状況の初期設定 */
	for (i = start_y - 1; i <= start_y + 1; i++) {
		for (j = start_x - 1; j <= start_x + 1; j++) {
			Field[i][j].find = FOUND;
		}
	}

	srand(time(NULL));
	//ゴール地点の生成
	while (Flag == 0)
	{
		goal_x = (rand() % (FIELD_WIDTH - 2)) + 1;
		if (goal_x % 2 == 1 && start_x != goal_x) {
			break;
		}
	}
	while (Flag == 0)
	{
		goal_y = (rand() % (FIELD_HEIGHT - 2)) + 1;
		if (goal_y % 2 == 1 && start_y != goal_y) {
			break;
		}
	}
	Field[goal_y][goal_x].field_state = GOAL_P;
}

void Drawing_2D() {

	int i, j;

	for (i = 0; i < FIELD_HEIGHT; i++) {
		locate(0, i);
		for (j = 0; j < FIELD_WIDTH; j++)
		{
			//まだそのFieldが発見されていない時
			if (Field[i][j].find == NOT_FOUND) {
				printf("■");
			}
			//すでにそのFieldが発見されている時
			else
			{
				//そのFieldがPLAYERの座標ではない時
				if (i != Player_Y || j != Player_X) {
					switch (Field[i][j].field_state) {
					case ROAD:
						printf("　");
						break;
					case WALL:
						printf("□");
						break;
					case START_P:
						printf("Ｓ");
						break;
					case GOAL_P:
						printf("Ｇ");
						break;
					default:
						//バグ確認用
						printf("？");
						break;
					}
				}
				//そのFieldがPLAYERの座標である時
				else {
					printf("○");
				}
			}
		}
		//printf("\n");
	}
	
	/*
	//Field確認用field_state
	for (i = 0; i < FIELD_HEIGHT; i++) {
		locate(0, i + 15);
		for (j = 0; j < FIELD_WIDTH; j++) {
			printf(" %d", Field[i][j].field_state);
		}
	}

	//Field確認用find
	for (i = 0; i < FIELD_HEIGHT; i++) {
		locate(0, i + 30);
		for (j = 0; j < FIELD_WIDTH; j++) {
			printf(" %d", Field[i][j].find);
		}
	}
	*/
}

void Drawing_3D() {

	int i, j;
	int Flag = 0;

	//mix_AA配列の初期化
	for (i = 0; i < AA_HEIGHT; i++) {
		for (j = 0; j < AA_WIDTH; j++) {
			mix_AA[i][j] = 0;
		}
	}

	//AA_3Dの[4-0][4-2]field_stateを確認する

	/* [4-0]field_stateを確認する */
	if (Field[Player_Y][Player_X - 1].field_state == ROAD) {
		CreateMixAA(0, 0, 0, 1, 13, 2, 0, 25, 1);
	}
	else {
		CreateMixAA(1, 0, 0, 1, 13, 2, 0, 25, 1);
	}

	/* [4-2]field_stateを確認する */
	if (Field[Player_Y][Player_X + 1].field_state == ROAD) {
		CreateMixAA(2, 0, 14, 1, 27, 2, 26, 25, 27);
	}
	else {
		CreateMixAA(3, 0, 14, 1, 27, 2, 26, 25, 27);
	}

	/* [3-1]field_stateを確認する */
	if (Field[Player_Y -1][Player_X].field_state == WALL) {
		CreateMixAA(4, ERR, ERR, ERR, ERR, 2, 2, 25, 25);
		Flag = 1;
	}
	else {
		/* [3-0]field_stateを確認する */
		if (Field[Player_Y - 1][Player_X - 1].field_state == ROAD) {
			CreateMixAA(5, 2, 2, 6, 13, 7, 2, 20, 6);
		}
		else {
			CreateMixAA(6, 2, 2, 6, 13, 7, 2, 20, 6);
		}
		/* [3-2]field_stateを確認する */
		if (Field[Player_Y - 1][Player_X + 1].field_state == ROAD) {
			CreateMixAA(7, 2, 14, 6, 25, 7, 21, 20, 25);
		}
		else {
			CreateMixAA(8, 2, 14, 6, 25, 7, 21, 20, 25);
		}
	}

	if (Flag != 1) {
		/* [2-1]field_stateを確認する */
		if (Field[Player_Y - 2][Player_X].field_state == WALL) {
			CreateMixAA(9, ERR, ERR, ERR, ERR, 7, 7, 20, 20);
			Flag = 1;
		}
		else {
			/* [2-0]field_stateを確認する */
			if (Field[Player_Y - 2][Player_X - 1].field_state == ROAD) {
				CreateMixAA(10, 7, 7, 9, 13, 10, 7, 17, 9);
			}
			else {
				CreateMixAA(11, 7, 7, 9, 13, 10, 7, 17, 9);
			}
			/* [2-2]field_stateを確認する */
			if (Field[Player_Y - 2][Player_X + 1].field_state == ROAD) {
				CreateMixAA(12, 7, 14, 9, 20, 10, 18, 17, 20);
			}
			else {
				CreateMixAA(13, 7, 14, 9, 20, 10, 18, 17, 20);
			}
		}
	}

	if (Flag != 1) {
		/* [1-1]field_stateを確認する */
		if (Field[Player_Y - 3][Player_X].field_state == WALL) {
			CreateMixAA(14, ERR, ERR, ERR, ERR, 10, 10, 17, 17);
			Flag = 1;
		}
		else {
			/* [1-0]field_stateを確認する */
			if (Field[Player_Y - 3][Player_X - 1].field_state == ROAD) {
				CreateMixAA(15, 10, 10, 11, 13, 12, 10, 15, 11);
			}
			else {
				CreateMixAA(16, 10, 10, 11, 13, 12, 10, 15, 11);
			}
			/* [1-2]field_stateを確認する */
			if (Field[Player_Y - 3][Player_X + 1].field_state == ROAD) {
				CreateMixAA(17, 10, 14, 11, 17, 12, 16, 15, 17);
			}
			else {
				CreateMixAA(18, 10, 14, 11, 17, 12, 16, 15, 17);
			}
		}
	}

	if (Flag != 1) {
		/* [0-1]field_stateを確認する */
		if (Field[Player_Y - 3][Player_X].field_state == WALL) {
			CreateMixAA(19, ERR, ERR, ERR, ERR, 12, 12, 15, 15);
			Flag = 1;
		}
		else {
			CreateMixAA(20, ERR, ERR, ERR, ERR, 12, 12, 15, 15);
		}
	}

	for (i = 0; i < AA_HEIGHT; i++) {
		for (j = 0; j < AA_WIDTH; j++) {
			locate(j * 2, i);
			switch (mix_AA[i][j]) {
				case 0:
					printf("　");
				break;
				case 1:
					printf("・");
				break;
				case 2:
					printf("＼");
				break;
				case 3:
					printf("／");
				break;
				case 4:
					printf("ー");
				break;
				case 5:
					printf("｜");
				break;
			}
		}
	}
}

void Drawing_Map() {

	system("cls");

	Drawing_2D();

	printf( "\n＊ーーーーーーーーーーーーーーーーー＊\n"
			"｜　○：現在地　　　　　　　　　　　｜\n"
			"｜　■：未探索地域　　　　　　　　　｜\n"
			"｜　□：壁　　　　　　　　　　　　　｜\n"
			"｜　Ｓ：初期地点　　　　　　　　　　｜\n"
			"｜　Ｇ：最終地点　　　　　　　　　　｜\n"
			"＊ーーーーーーーーーーーーーーーーー＊");

	_getch();
	system("cls");
}

void CreateMixAA(int type, int i1,int j1,int i2,int j2,int a1, int b1, int a2, int b2) {

	int i, j;

	if (i1 != ERR) {
		for (i = i1; i <= i2; i++) {
			for (j = j1; j <= j2; j++) {
				switch (type) {
				case 0:
					mix_AA[i][j] = ROAD5_1[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = ROAD5_1[AA_HEIGHT - 1 - i][j];
					break;
				case 1:
					mix_AA[i][j] = WALL5_1[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = WALL5_1[AA_HEIGHT - 1 - i][j];
					break;
				case 2:
					mix_AA[i][j] = ROAD5_3[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = ROAD5_3[AA_HEIGHT - 1 - i][j];
					break;
				case 3:
					mix_AA[i][j] = WALL5_3[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = WALL5_3[AA_HEIGHT - 1 - i][j];
					break;
				case 4:
					mix_AA[i][j] = WALL4_2[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = WALL4_2[AA_HEIGHT - 1 - i][j];
					break;
				case 5:
					mix_AA[i][j] = ROAD4_1[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = ROAD4_1[AA_HEIGHT - 1 - i][j];
					break;
				case 6:
					mix_AA[i][j] = WALL4_1[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = WALL4_1[AA_HEIGHT - 1 - i][j];
					break;
				case 7:
					mix_AA[i][j] = ROAD4_3[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = ROAD4_3[AA_HEIGHT - 1 - i][j];
					break;
				case 8:
					mix_AA[i][j] = WALL4_3[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = WALL4_3[AA_HEIGHT - 1 - i][j];
					break;
				case 9:
					mix_AA[i][j] = WALL3_2[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = WALL3_2[AA_HEIGHT - 1 - i][j];
					break;
				case 10:
					mix_AA[i][j] = ROAD3_1[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = ROAD3_1[AA_HEIGHT - 1 - i][j];
					break;
				case 11:
					mix_AA[i][j] = WALL3_1[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = WALL3_1[AA_HEIGHT - 1 - i][j];
					break;
				case 12:
					mix_AA[i][j] = ROAD3_3[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = ROAD3_3[AA_HEIGHT - 1 - i][j];
					break;
				case 13:
					mix_AA[i][j] = WALL3_3[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = WALL3_3[AA_HEIGHT - 1 - i][j];
					break;
				case 14:
					mix_AA[i][j] = WALL2_2[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = WALL2_2[AA_HEIGHT - 1 - i][j];
					break;
				case 15:
					mix_AA[i][j] = ROAD2_1[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = ROAD2_1[AA_HEIGHT - 1 - i][j];
					break;
				case 16:
					mix_AA[i][j] = WALL2_1[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = WALL2_1[AA_HEIGHT - 1 - i][j];
					break;
				case 17:
					mix_AA[i][j] = ROAD2_3[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = ROAD2_3[AA_HEIGHT - 1 - i][j];
					break;
				case 18:
					mix_AA[i][j] = WALL2_3[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = WALL2_3[AA_HEIGHT - 1 - i][j];
					break;
				case 19:
					mix_AA[i][j] = WALL1_2[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = WALL1_2[AA_HEIGHT - 1 - i][j];
					break;
				case 20:
					mix_AA[i][j] = ROAD1_2[i][j];
					mix_AA[AA_HEIGHT - 1 - i][j] = ROAD1_2[AA_HEIGHT - 1 - i][j];
					break;
				}
			}
		}
	}

	for (i = a1; i <= a2; i++) {
		for (j = b1; j <= b2; j++) {
			switch (type) {
			case 0:
				mix_AA[i][j] = ROAD5_1[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = ROAD5_1[AA_HEIGHT - 1 - i][j];
				break;
			case 1:
				mix_AA[i][j] = WALL5_1[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = WALL5_1[AA_HEIGHT - 1 - i][j];
				break;
			case 2:
				mix_AA[i][j] = ROAD5_3[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = ROAD5_3[AA_HEIGHT - 1 - i][j];
				break;
			case 3:
				mix_AA[i][j] = WALL5_3[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = WALL5_3[AA_HEIGHT - 1 - i][j];
				break;
			case 4:
				mix_AA[i][j] = WALL4_2[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = WALL4_2[AA_HEIGHT - 1 - i][j];
				break;
			case 5:
				mix_AA[i][j] = ROAD4_1[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = ROAD4_1[AA_HEIGHT - 1 - i][j];
				break;
			case 6:
				mix_AA[i][j] = WALL4_1[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = WALL4_1[AA_HEIGHT - 1 - i][j];
				break;
			case 7:
				mix_AA[i][j] = ROAD4_3[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = ROAD4_3[AA_HEIGHT - 1 - i][j];
				break;
			case 8:
				mix_AA[i][j] = WALL4_3[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = WALL4_3[AA_HEIGHT - 1 - i][j];
				break;
			case 9:
				mix_AA[i][j] = WALL3_2[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = WALL3_2[AA_HEIGHT - 1 - i][j];
				break;
			case 10:
				mix_AA[i][j] = ROAD3_1[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = ROAD3_1[AA_HEIGHT - 1 - i][j];
				break;
			case 11:
				mix_AA[i][j] = WALL3_1[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = WALL3_1[AA_HEIGHT - 1 - i][j];
				break;
			case 12:
				mix_AA[i][j] = ROAD3_3[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = ROAD3_3[AA_HEIGHT - 1 - i][j];
				break;
			case 13:
				mix_AA[i][j] = WALL3_3[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = WALL3_3[AA_HEIGHT - 1 - i][j];
				break;
			case 14:
				mix_AA[i][j] = WALL2_2[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = WALL2_2[AA_HEIGHT - 1 - i][j];
				break;
			case 15:
				mix_AA[i][j] = ROAD2_1[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = ROAD2_1[AA_HEIGHT - 1 - i][j];
				break;
			case 16:
				mix_AA[i][j] = WALL2_1[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = WALL2_1[AA_HEIGHT - 1 - i][j];
				break;
			case 17:
				mix_AA[i][j] = ROAD2_3[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = ROAD2_3[AA_HEIGHT - 1 - i][j];
				break;
			case 18:
				mix_AA[i][j] = WALL2_3[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = WALL2_3[AA_HEIGHT - 1 - i][j];
				break;
			case 19:
				mix_AA[i][j] = WALL1_2[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = WALL1_2[AA_HEIGHT - 1 - i][j];
				break;
			case 20:
				mix_AA[i][j] = ROAD1_2[i][j];
				mix_AA[AA_HEIGHT - 1 - i][j] = ROAD1_2[AA_HEIGHT - 1 - i][j];
				break;
			}
		}
	}

}

void Key() {

	int keycode = _getch();

	switch (keycode) {
	case UP:	//上キー
		Move();
		break;
	case LEFT:	//左キー
		TurnMap(0);
		break;
	case DOWN:	//下キー
		TurnMap(1);
		break;
	case RIGHT:	//右キー
		TurnMap(2);
		break;
	case KEY_M:	//Ｍキー
		Drawing_Map();
		break;
	}
	Drawing_3D();
}

void Move() {

	if (Field[Player_Y - 1][Player_X].field_state != WALL) {
		Player_Y -= 1;
	}

	//発見状況の上書き
	for (int i = Player_Y - 1; i <= Player_Y; i++) {
		for (int j = Player_X - 1; j <= Player_X + 1; j++)
		{
			Field[i][j].find = FOUND;
		}
	}

	//goal地点かを確認する
	if (Field[Player_Y][Player_X].field_state == GOAL_P) {
		goal_cnt = 1;
	}
}

void TurnMap(int der) {	//PLAYER座標とMAPを変化

	int i, j;
	int x, y;

	//回転済MAPをTentaに格納
	switch (der)
	{
		case 0:	/* 左に向く */
			for (i = 0; i < FIELD_WIDTH; i++) {
				for (j = 0; j < FIELD_HEIGHT; j++)
				{
					Tenta_Field[i][j].field_state = Field[FIELD_HEIGHT - 1 - j][i].field_state;
					Tenta_Field[i][j].find = Field[FIELD_HEIGHT - 1 - j][i].find;

					//回転後のPLAYER座標を記録
					if (i == Player_Y && j == Player_X) {
						y = j;
						x = FIELD_WIDTH - 1 - i;
					}
				}
			}
			break;

		case 1:	/* 下に向く */
			for (i = 0; i < FIELD_HEIGHT; i++) {
				for (j = 0; j < FIELD_WIDTH; j++)
				{
					Tenta_Field[i][j].field_state = Field[FIELD_HEIGHT - 1 - i][FIELD_WIDTH - 1 - j].field_state;
					Tenta_Field[i][j].find = Field[FIELD_HEIGHT - 1 - i][FIELD_WIDTH - 1 - j].find;

					//回転後のPLAYER座標を記録
					if (i == Player_Y && j == Player_X) {
						y = FIELD_HEIGHT - 1 - i;  
						x = FIELD_WIDTH - 1 - j; 
					}
				}
			}
			break;

		case 2:	/* 右に向く */
			for (i = 0; i < FIELD_WIDTH; i++) {
				for (j = 0; j < FIELD_HEIGHT; j++)
				{
					Tenta_Field[i][j].field_state = Field[j][FIELD_WIDTH - 1 - i].field_state;
					Tenta_Field[i][j].find = Field[j][FIELD_WIDTH - 1 - i].find;

					//回転後のPLAYER座標を記録
					if (i == Player_Y && j == Player_X) {
						y = FIELD_HEIGHT - 1 - j;
						x = i;
					}
				}
			}
			break;
	}

	//Player座標書き換え
	Player_X = x;
	Player_Y = y;


	//Tentaに格納した回転済MAPをFieldに上書き
	for (i = 0; i < FIELD_HEIGHT; i++) {
		for (j = 0; j < FIELD_WIDTH; j++) {
			Field[i][j].field_state = Tenta_Field[i][j].field_state;
			Field[i][j].find = Tenta_Field[i][j].find;
		}
	}

	//発見状況の上書き
	for (i = Player_Y - 1; i <= Player_Y; i++) {
		for (j = Player_X - 1; j <= Player_X + 1; j++) {
			Field[i][j].find = FOUND;
		}
	}
}

int Game_State() {
	int i;
	int keycode;
	int command = 0;
	int end = 0;

	char Command_Name[2][8] = {
	" Y E S ",
	" N   O " };

	while (end == 0) {
		system("cls");

		locate(22, 8);
		printf("クリアおめでとう！！");
		locate(22, 10);
		printf("ゲームを続けますか？");

		locate(16, 12);
		printf("　　＋ーーーーーーーーーー＋\n");

		for (i = 0; i < 2; i++) {

			locate(16, 12 + ((i + 1) * 2) - 1);
			printf("　　│　　　　　　        │\n");

			locate(16, 12 + (i + 1) * 2);
			printf("　　│　%s　%s　　%s  │\n"
				, (command == i) ? "◆" : "　 "
				, Command_Name[i]
				, (command == i) ? " " : "");
		}

		locate(16, 17);
		printf("　　│　　　　　　　　　　│\n");
		locate(16, 18);
		printf("　　＋ーーーーーーーーーー＋\n");
		keycode = _getch();

		//Enterが押された時
		if (keycode == ENTER) {
			switch (command) {
			case 0:		//STARTコマンド
				system("cls");
				return SAFE;
				break;
			case 1:
				system("cls");
				return ERR;
				break;
			}
		}
		//Enterが押されなかった時
		else {
			switch (keycode) {
			case UP:	//↑キー
				if (command == 1) {
					command--;
				}
				else if (command == 0) {
					command = 1;
				}
				break;
			case DOWN:	//↓キー
				if (command == 0) {
					command++;
				}
				else if (command == 1) {
					command = 0;
				}
				break;
			default:
				break;
			}
		}
	}
}

int Random(int Number) {

	int r;

	r = rand() % Number;

	return r;
}

int Create_Wall_Check(int num, int x, int y) {

	//その方向がまだ道かどうか
	switch (num) {
	case 0:		/* 左 */
		if (Field[y][x - 1].field_state == ROAD) {
			return SAFE;
		}
		return ERR;
		break;
	case 1:		/* 下 */
		if (Field[y + 1][x].field_state == ROAD) {
			return SAFE;
		}
		return ERR;
		break;
	case 2:		/* 右 */
		if (Field[y][x + 1].field_state == ROAD) {
			return SAFE;
		}
		return ERR;
		break;
	case 3:		/* 上 */
		if (Field[y - 1][x].field_state == ROAD) {
			return SAFE;
		}
		return ERR;
		break;
	}
	return ERR;
}
