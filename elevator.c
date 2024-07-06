#include<stdio.h>
#include<stdlib.h> 
#include<time.h>
#include<conio.h>
#include <windows.h>
#include <locale.h>
#define Max_Load 9
#define MAXQSIZE 11

typedef struct passenger
{
	int Aim_Floor;
	int Current_Floor;
	int Weight;
	int Name;
	long Now_Time;
	int ys;
}pg,*pgpt;

typedef struct Queue
{
	pg Wait_p[MAXQSIZE];
	int front;
	int rear;
	int floor;
	int dir;
}Queue,*Queuept;

typedef struct load
{
	pg person[Max_Load];
	int front;
	int rear;
	int Sum_weight;
}load, * loadpt;

typedef struct elevator
{
	loadpt Load;
	int Current_Dir;
	int Current_Floor;
	int Aim_Floor[5];
	int Pre_Dir;
}ele, * elept;

typedef struct state_message
{
	int state;
	struct state_message* next;
}sm, * smpt;

typedef struct passenger_message
{
	pg pas;
	struct passenger_message* next;
}pm, * pmpt;

void CursorJump(int x, int y);
int Queue_Empty(Queuept Q);
int Queue_Full(Queuept Q);
void InQueue(Queuept Q, pg pas);
void OutQueue(Queuept Q);
Queuept InitQueue();
int Rand_Floor();
int Rand_Weight();
void HideCursor();
void color(int c);
void InitPage();
void FirstEle();
void Open_door(int floor);
void Wait_line(Queuept Q);
void Button(Queuept Q);
pg Create_passenger();
void Move_Elevator(elept Ele,int Current_floor,int AimFloor);
loadpt InitLoad();
int Load_Empty(loadpt L);
int Load_Full(loadpt L);
void InLoad(loadpt L, pg pas);
pg Outload(loadpt L);
int Number_Load(loadpt L);
void Print_Load(loadpt L, int Current_Floor, int k);
elept Initele(loadpt L);
void clear();
pg Come_Passenger(Queuept Qu1, Queuept Qu2, Queuept Qd2, Queuept Qu3, Queuept Qd3, Queuept Qu4, Queuept Qd4, Queuept Qd5);
int Sum_rule(int rule[14]);
void Printloadsteady(int floor, loadpt L);
int If_overload(elept E,pg pas);
void Print_sm(smpt S);
void Print_pm(pmpt P);
void Print_rule(int order);

void CursorJump(int x, int y)
{
	COORD pos; //定义光标位置的结构体变量
	pos.X = x; //横坐标
	pos.Y = y; //纵坐标
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); //获取控制台句柄
	SetConsoleCursorPosition(handle, pos); //设置光标位置
}

smpt Init_sm()
{
	smpt S;
	S = malloc(sizeof(sm));
	S->next = NULL;
}

pmpt Init_pm()
{
	pmpt P = malloc(sizeof(pm));
	P->next = NULL;
}

void In_sm(smpt S,int state)
{
	smpt temp = malloc(sizeof(sm));
	temp->state = state;
	temp->next = S->next;
	S->next = temp;
}

void In_pm(pmpt P, pg pas)
{
	pmpt temp = malloc(sizeof(pm));
	temp->pas = pas;
	temp->next = P->next;
	P->next = temp;
}

int Queue_Empty(Queuept Q)
{
	int front = Q->front;
	int rear = Q->rear;
	return front == rear;
}

int Queue_Full(Queuept Q)
{
	int rear = Q->rear;
	return (rear + 1) % MAXQSIZE == Q->front;
}

void InQueue(Queuept Q,pg pas)
{
	int i = Queue_Full(Q);
	if (i==1)
	{
		CursorJump(0, 32);
		printf("第%d楼第%d号乘客来了，但是队列太长Ta又走了",pas.Current_Floor,pas.Name);
	}
	else
	{
		int rear = Q->rear;
		Q->Wait_p[rear] = pas;
		Q->rear = (rear + 1) % MAXQSIZE;
	}
}

void OutQueue(Queuept Q)
{
	int front = Q->front;
	pg pas = Q->Wait_p[front];
	Q->front = (front + 1) % MAXQSIZE;
}

Queuept InitQueue()
{
	Queuept Q = malloc(sizeof(Queue));
	Q->front = Q->rear = 0;
	return Q;
}

int Rand_Floor()
{
	srand(time(0));
	int floor = rand() % 5 + 1;
	return floor;
}

int Rand_Weight()
{
	srand(time(0));
	int weight = rand() % 51 + 20;
	return weight;
}

void HideCursor()
{
	CONSOLE_CURSOR_INFO curInfo; //定义光标信息的结构体变量
	curInfo.dwSize = 1; //如果没赋值的话，光标隐藏无效
	curInfo.bVisible = FALSE; //将光标设置为不可见
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); //获取控制台句柄
	SetConsoleCursorInfo(handle, &curInfo); //设置光标信息
}

void color(int c)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c); //颜色设置
}

void InitPage()
{
	color(6);
	for (int i = 0; i <= 47; i++)
	{
		for (int j = 0; j <= 30; j++)
		{
			if (i == 0 || i == 20 || i == 47)
			{
				CursorJump(i, j);
				printf("■");
			}
			else if (j == 0 || j == 30)
			{
				CursorJump(i, j);
				printf("■");
			}
			else if (21 <= i && i <= 47)
			{
				if (j == 6 || j == 12 || j == 18 || j == 24)
				{
					CursorJump(i, j);
					printf("■");
				}
			}
		}
	}
	for (int i = 5, j = 1; i > 0, j < 30; i--, j += 6)
	{
		CursorJump(21, j);
		printf("%dF", i);
		CursorJump(21, j + 1);
		printf("△");
		CursorJump(21, j + 2);
		printf("▽");
	}
	CursorJump(0, 31);
	printf("实时消息窗：\n");
	CursorJump(49, 0);
	printf("电梯运行规律执行情况：\n");
	CursorJump(49, 1);
	printf("□001 电梯静止停靠——当前层有乘梯请求——》开门\n");
	CursorJump(49, 2);
	printf("□002 电梯静止停靠——上层有乘梯请求——》上行\n");
	CursorJump(49, 3);
	printf("□003 电梯静止停靠——下层有乘梯请求——》下行\n");
	CursorJump(49, 4);
	printf("□004 电梯上行过程——上层有上行乘梯请求——》到达请求楼层-停靠\n");
	CursorJump(49, 5);
	printf("□005 电梯上行过程——下层有上行乘梯请求——》暂不响应\n");
	CursorJump(49, 6);
	printf("□006 电梯上行过程——任意层有下行乘梯请求——》暂不响应\n");
	CursorJump(49, 7);
	printf("□007 电梯关门上行——本层有上行乘梯请求——》暂不响应\n");
	CursorJump(49, 8);
	printf("□008 电梯下行过程——下层有下行乘梯请求——》到达相应楼层-停靠\n");
	CursorJump(49, 9);
	printf("□009 电梯下行过程——上层有下行乘梯请求——》暂不响应\n");
	CursorJump(49, 10);
	printf("□010 电梯下行过程——任意层有上行乘梯请求——》暂不响应\n");
	CursorJump(49, 11);
	printf("□011 电梯关门下行——本层有下行乘梯请求——》暂不响应\n");
	CursorJump(49, 12);
	printf("□012 电梯停靠上客——超载（重量或人数）——》告警-乘客退出-关门\n");
	CursorJump(49, 13);
	printf("□013 电梯停靠上客——超时——》告警-停止上客-关门\n");
	CursorJump(49, 14);
	printf("□014 电梯满载运行——无乘客下梯——》经过楼层不停靠\n");
	CursorJump(49, 15);
	printf("□015 电梯满载运行——有乘客下梯——》经过下梯楼层停靠\n");
	CursorJump(49, 31);
	printf("乘客信息表:\n");
}

void Fix()
{
	for (int i = 0; i <= 47; i++)
	{
		for (int j = 0; j <= 30; j++)
		{
			if (i == 0 || i == 20 || i == 47)
			{
				CursorJump(i, j);
				printf("■");
			}
			else if (j == 0 || j == 30)
			{
				CursorJump(i, j);
				printf("■");
			}
			else if (21 <= i && i <= 47)
			{
				if (j == 6 || j == 12 || j == 18 || j == 24)
				{
					CursorJump(i, j);
					printf("■");
				}
			}
		}
	}
}

loadpt InitLoad()
{
	loadpt L = malloc(sizeof(load));
	L->front = L->rear = 0;
	L->Sum_weight = 0;
	return L;
}

int Load_Empty(loadpt L)
{
	int front = L->front;
	int rear = L->rear;
	return front == rear;
}

int Load_Full(loadpt L)
{
	int rear = L->rear;
	return (rear + 1) % Max_Load == L->front;
}

void InLoad(loadpt L, pg pas)
{
	int Sum_weight = L->Sum_weight;
	L->Sum_weight = pas.Weight + Sum_weight;
	int rear = L->rear;
	L->person[rear] = pas;
	L->rear = (rear + 1) % Max_Load;
}

pg Outload(loadpt L)
{
	if (Load_Empty(L))
	{
		return;
	}
	else
	{
		int front = L->front;
		pg pas = L->person[front];
		L->front = (front + 1) % Max_Load;
		return pas;
	}
}

void FirstEle()
{
	color(6);
	for (int i = 0; i <= 47; i++)
	{
		for (int j = 0; j <= 30; j++)
		{
			if (1 <= i && i <= 20)
			{
				if (j == 24 || j == 30)
				{
					CursorJump(i, j);
					printf("■");
				}
			}
			if (i == 1 || i == 19)
			{
				if (j >= 24 && j <= 30)
				{
					CursorJump(i, j);
					printf("■");
				}
			}
		}
	}
	CursorJump(2, 25);
	printf("1F");
	CursorJump(2, 26);
	printf("人数：0/8人");
	CursorJump(2, 27);
	printf("载荷：0/500KG");
	CursorJump(2, 28);
	printf("1 2 3 4 5");
}

void Open_door(int floor)
{
	Sleep(100);
	for (int j = 0; j <= 30; j++)
	{
		if (j >= (31 - floor * 6) && j <= (35 - floor * 6))
		{
			CursorJump(19, j);
			printf(" ");
			CursorJump(20, j);
			printf(" ");
		}
	}
	Sleep(100);
}

void Close_door(int floor)
{
	Sleep(100);
	for (int j = 0; j <= 30; j++)
	{
		if (j >= (31 - floor * 6) && j <= (35 - floor * 6))
		{
			CursorJump(19, j);
			printf("■");
			CursorJump(20, j);
			printf("■");
		}
	}
	Sleep(100);
}

void Change_number(int number)
{
	switch (number)
	{
	case 1:
		printf("①");
		break;
	case 2:
		printf("②");
		break;
	case 3:
		printf("③");
		break;
	case 4:
		printf("④");
		break;
	case 5:
		printf("⑤");
		break;
	}
}

void Wait_line(Queuept Q)
{
	color(6);
	CursorJump(21, 35 - 6 * Q->floor - Q->dir);
	printf("                    ");
	int a = (Q->rear - Q->front+MAXQSIZE) % MAXQSIZE;
	int j = Q->front;
	for (int i = 0; i < a; i++)
	{
		CursorJump(21+i*2, 35 - 6 * Q->floor - Q->dir);
		Change_number((Q->Wait_p[j]).Aim_Floor);
		printf(" ");
		j = (j + 1) % MAXQSIZE;
	}
}

void Refresh_botton(Queuept Q)
{
	if (Q != NULL)
	{
		if (Queue_Empty(Q))
		{
			if (Q->dir == 1)
			{
				CursorJump(21, 32 - 6 * Q->floor);
				printf("△");
			}
			else
			{
				CursorJump(21, 33 - 6 * Q->floor);
				printf("▽");
			}
		}
	}
}

void Print_Load(loadpt L,int Floor, int k)
{
	int a = Number_Load(L);
	int j = L->front;
	for (int i = 0; i < a; i++)
	{
		CursorJump(2+i*2, 35 - Floor * 6 - k);
		Change_number((L->person[j]).Aim_Floor);
		printf(" ");
		j = (j + 1) % Max_Load;
	}
}

elept Initele(loadpt L)
{
	elept E = malloc(sizeof(ele));
	E->Load = L;
	E->Current_Floor = 1;
	E->Current_Dir = 0;//-1,0,1
	E->Pre_Dir = 0;
	for (int i = 0; i < 5; i++)
	{
		*(E->Aim_Floor + i) = 0;
	}
	return E;
}

void Button(Queuept Q)
{
	if (Queue_Empty(Q))
	{
		return;
	}
	else
	{
		int floor = Q->floor;
		int ud = Q->dir;
		if (ud == 0)
		{
			CursorJump(21, 33 - floor * 6);
			printf("▼");
		}
		else
		{
			CursorJump(21, 32 - floor * 6);
			printf("▲");
		}
	}
}

int Name_Counter=0;

pg Create_passenger()
{
	pg rand_pg;
	clock_t start = clock();
	Name_Counter += 1;
	rand_pg.Name = Name_Counter;
	rand_pg.ys = 1;
	rand_pg.Weight = Rand_Weight();
	rand_pg.Current_Floor = Rand_Floor();
	do {
		rand_pg.Aim_Floor = Rand_Floor();
	} while (rand_pg.Aim_Floor == rand_pg.Current_Floor);
	rand_pg.Now_Time = start;
	return rand_pg;
}

int Number_Load(loadpt L)
{
	return (L->rear - L->front + Max_Load) % Max_Load;
}

void clear()
{
	for (int i = 1; i <= 19; i++)
	{
		for (int j = 1; j <= 29; j++)
		{
			CursorJump(i, j);
			printf(" ");
		}
	}
}

int up_or_down(pg pas)
{
	if (pas.Aim_Floor - pas.Current_Floor > 0)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

int Sum_rule(int rule[14])
{
	int sum = 0;
	for (int i = 0; i < 14; i++)
	{
		sum += rule[i];
	}
	return sum;
}

Queuept Q_pas(pg pas, int ud, Queuept Qu1, Queuept Qu2, Queuept Qd2, Queuept Qu3, Queuept Qd3, Queuept Qu4, Queuept Qd4, Queuept Qd5)
{
	int a = ud * pas.Current_Floor;
	switch (a)
	{
	case 1:
		return Qu1;
	case 2:
		return Qu2;
	case 3:
		return Qu3;
	case 4:
		return Qu4;
	case -2:
		return Qd2;
	case -3:
		return Qd3;
	case -4:
		return Qd4;
	case -5:
		return Qd5;
	}
}

Queuept Q_floor(int Current_floor, int ud, Queuept Qu1, Queuept Qu2, Queuept Qd2, Queuept Qu3, Queuept Qd3, Queuept Qu4, Queuept Qd4, Queuept Qd5,Queuept Q0)
{
	int a = Current_floor * ud;
	switch (a)
	{
	case 1:
		return Qu1;
	case 2:
		return Qu2;
	case 3:
		return Qu3;
	case 4:
		return Qu4;
	case -2:
		return Qd2;
	case -3:
		return Qd3;
	case -4:
		return Qd4;
	case -5:
		return Qd5;
	default:
		return Q0;
	}
}

void Move_Elevator(elept E, int Current_floor, int Aim_Floor,smpt S)
{
	if (Aim_Floor != Current_floor)
	{
		In_sm(S, 2);
		Print_sm(S);
	}
	if (Aim_Floor > Current_floor)
	{
		for (int k = 0; k <=(Aim_Floor - Current_floor) * 6; k++)
		{
			clear();
			for (int i = 0; i <= 47; i++)
			{
				for (int j = 0; j <= 30; j++)
				{
					if (j == 30 - Current_floor * 6 - k || j == 36 - Current_floor * 6 - k)
					{
						if (1 <= i && i <= 19)
						{
							CursorJump(i, j);
							printf("■");
						}
					}
					if (i == 1 || i == 19)
					{
						if (j >= 31-Current_floor*6-k && j <= 35-Current_floor*6-k)
						{
							CursorJump(i, j);
							printf("■");
						}
					}
				}
			}
			CursorJump(2, 31 - Current_floor * 6 - k);
			printf("%dF", Current_floor + k / 6);
			CursorJump(2, 32 - Current_floor * 6 - k);
			int number = Number_Load(E->Load);
			printf("人数：%d/8人", number);
			CursorJump(2, 33 - Current_floor * 6 - k);
			printf("载荷：%d/500KG", E->Load->Sum_weight);
			CursorJump(2, 34 - Current_floor * 6 - k);
			printf("1 2 3 4 5");
			CursorJump(2+(Aim_Floor-1)*2, 34 - Current_floor * 6 - k);
			Change_number(Aim_Floor);
			Print_Load(E->Load, Current_floor, k);//乘客
			Sleep(200);
		}
	}
	else
	{
		for (int k = (Current_floor-Aim_Floor) * 6; k >=0 ; k--)
		{
			clear();
			for (int i = 0; i <= 47; i++)
			{
				for (int j = 0; j <= 30; j++)
				{
					if (j == 30 - Aim_Floor * 6 - k || j == 36 - Aim_Floor * 6 - k)
					{
						if (1 <= i && i <= 19)
						{
							CursorJump(i, j);
							printf("■");
						}
					}
					if (i == 1 || i == 19)
					{
						if (j >= 31 - Aim_Floor * 6 - k && j <= 35 - Aim_Floor * 6 - k)
						{
							CursorJump(i, j);
							printf("■");
						}
					}
				}
			}
			CursorJump(2, 31 - Aim_Floor * 6 - k);
			printf("%dF", Aim_Floor + (k + 5) / 6);
			CursorJump(2, 32 - Aim_Floor * 6 - k);
			printf("人数：%d/8人", Number_Load(E->Load));
			CursorJump(2, 33 - Aim_Floor * 6 - k);
			printf("载荷：%d/500KG", E->Load->Sum_weight);
			CursorJump(2, 34 - Aim_Floor * 6 - k);
			printf("1 2 3 4 5");
			CursorJump(2 + (Aim_Floor - 1) * 2, 34 - Aim_Floor * 6 - k);
			Change_number(Aim_Floor);
			Print_Load(E->Load, Aim_Floor, k);//乘客
			Sleep(200);
		}
	}
	if (Aim_Floor != Current_floor)
	{
		In_sm(S, 3);
		Print_sm(S);
	}
}

pg Come_Passenger(Queuept Qu1,Queuept Qu2,Queuept Qd2,Queuept Qu3,Queuept Qd3,Queuept Qu4,Queuept Qd4,Queuept Qd5)
{
	pg a;
	a.ys = 0;
		pg pas = Create_passenger();
		switch (pas.Current_Floor)
		{
		case 1:
		{
			Qu1->floor = 1;
			Qu1->dir = 1;
			InQueue(Qu1, pas);
			Wait_line(Qu1);
			if (!Queue_Full(Qu1))
			{
				a = pas;
			}
			break;
		}
		case 2:
		{
			Qu2->floor = 2;
			Qd2->floor = 2;
			Qu2->dir = 1;
			Qd2->dir = 0;
			if (pas.Current_Floor < pas.Aim_Floor)
			{
				InQueue(Qu2, pas);
				Wait_line(Qu2);
				if (!Queue_Full(Qu2))
				{
					a = pas;
				}
			}
			else
			{
				InQueue(Qd2, pas);
				Wait_line(Qd2);
				if (!Queue_Full(Qd2))
				{
					a = pas;
				}
			}
			break;
		}
		case 3:
		{
			Qu3->floor = 3;
			Qd3->floor = 3;
			Qu3->dir = 1;
			Qd3->dir = 0;
			if (pas.Current_Floor < pas.Aim_Floor)
			{
				InQueue(Qu3, pas);
				Wait_line(Qu3);
				if (!Queue_Full(Qu3))
				{
					a = pas;
				}
			}
			else
			{
				InQueue(Qd3, pas);
				Wait_line(Qd3);
				if (!Queue_Full(Qd3))
				{
					a = pas;
				}
			}
			break;
		}
		case 4:
		{
			Qu4->floor = 4;
			Qd4->floor = 4;
			Qu4->dir = 1;
			Qd4->dir = 0;
			if (pas.Current_Floor < pas.Aim_Floor)
			{
				InQueue(Qu4, pas);
				Wait_line(Qu4);
				if (!Queue_Full(Qu4))
				{
					a = pas;
				}
			}
			else
			{
				InQueue(Qd4, pas);
				Wait_line(Qd4);
				if (!Queue_Full(Qd4))
				{
					a = pas;
				}
			}
			break;
		}
		case 5:
		{
			Qd5->floor = 5;
			Qd5->dir = 0;
			InQueue(Qd5, pas);
			Wait_line(Qd5);
			if (!Queue_Full(Qd5))
			{
				a = pas;
			}
			break;
		}
		}
		Button(Qu1);
		Button(Qu2);
		Button(Qu3);
		Button(Qu4);
		Button(Qd2);
		Button(Qd3);
		Button(Qd4);
		Button(Qd5);
		Sleep(500);
	return a;
}

int Rank_Q(Queuept Q, pg pas)
{
	int position1=0;
	int front1 = Q->front;
	while (front1 != Q->rear)
	{
		pg temp = Q->Wait_p[front1];
		if (temp.Name == pas.Name)
		{
			position1 = front1;
			break;
		}
		front1 = (front1 + 1) % MAXQSIZE;
	}
	int rank = (position1 - front1 + MAXQSIZE) % MAXQSIZE;
	return rank;
}

int Rank_L(loadpt L, pg pas)
{
	int position1 = 0;
	int front1 = L->front;
	while (front1 != L->rear)
	{
		pg temp = L->person[front1];
		if (temp.Name == pas.Name)
		{
			position1 = front1;
			break;
		}
		front1 = (front1 + 1) % Max_Load;
	}
	int rank = (position1 - front1 + Max_Load) % Max_Load;
	return rank;
}

void In_ele(loadpt L, Queuept Q, pg pas,smpt S)
{
	In_sm(S, 4);
	Print_sm(S);
	int rank1 = Rank_Q(Q, pas);
	int rank2 = Rank_L(L, pas);
	if (up_or_down(pas) == 1)
	{
		CursorJump(21 + 2 * rank1, 34 - pas.Current_Floor * 6);
		printf(" ");
	}
	else
	{
		CursorJump(21 + 2 * rank1, 35 - pas.Current_Floor * 6);
		printf(" ");
	}
	for (int i = 21 + 2 * rank1; i >= 3 + rank2; i--)
	{
		CursorJump(i, 35 - pas.Current_Floor * 6);
		Change_number(pas.Aim_Floor);
		printf("...");
		Sleep(30);
		CursorJump(i, 35 - pas.Current_Floor * 6);
		printf("    ");
	}
}

void Out_ele(loadpt L, pg pas)
{
	int rank = Rank_L(L, pas);
	for (int i = 2 + rank ; i <47; i++)
	{
		CursorJump(i, 35 - pas.Aim_Floor * 6);
		if (i > 3)
		{
			CursorJump(i-3, 35 - pas.Aim_Floor * 6);
			printf("...");
		}
		Change_number(pas.Aim_Floor);
		Sleep(30);
		CursorJump(i, 35 - pas.Aim_Floor * 6);
		printf(" ");
		if (i > 3)
		{
			CursorJump(i - 3, 35 - pas.Aim_Floor * 6);
			printf("   ");
		}
	}
}

void chaozai(int floor)
{
	CursorJump(0, 32);
	printf("电梯超载，在%d层不上客", floor);
	Sleep(500);
}

/*int* Get_upaimfloor(elept E)
{
	int a[5] = { 0 };
	int front = E->Load->front;
	int rear = E->Load->rear;
	int j = 0;
	for (int i = front; i < rear; i++)
	{
		pg pas = E->Load->person[i];
		if (pas.Aim_Floor >= E->Current_Floor)
		{
			a[j] = pas.Aim_Floor;
			j++;
		}
	}
	for (int i = 0; i < 5; i++)
	{
		for (int j = i + 1; j < 5; j++)
		{
			if (a[j] > a[i])
			{
				int temp = a[i];
				a[i] = a[j];
				a[j] = temp;
			}
		}
	}
	return a;
}

int* Get_downaimfloor(elept E)
{
	int a[5] = { 0 };
	int front = E->Load->front;
	int rear = E->Load->rear;
	int j = 0;
	for (int i = front; i < rear; i++)
	{
		pg pas = E->Load->person[i];
		if (pas.Aim_Floor <= E->Current_Floor)
		{
			a[j] = pas.Aim_Floor;
			j++;
		}
	}
	for (int i = 0; i < 5; i++)
	{
		for (int j = i + 1; j < 5; j++)
		{
			if (a[j] > a[i])
			{
				int temp = a[i];
				a[i] = a[j];
				a[j] = temp;
			}
		}
	}
	return a;
}

int* Get_upcurrentfloor(Queuept Qu1, Queuept Qu2, Queuept Qu3, Queuept Qu4)
{
	int a[4] = { 0 };
	if (!Queue_Empty(Qu1))
	{
		a[0] = 1;
	}
	else if (!Queue_Empty(Qu2))
	{
		a[1] = 2;
	}
	else if (!Queue_Empty(Qu3))
	{
		a[2] = 3;
	}
	else if (!Queue_Empty(Qu4))
	{
		a[3] = 4;
	}
	return a;
}

int* Get_downcurrentfloor(Queuept Qd2, Queuept Qd3, Queuept Qd4, Queuept Qd5)
{
	int a[4] = { 0 };
	if (!Queue_Empty(Qd2))
	{
		a[0] = 2;
	}
	else if (!Queue_Empty(Qd3))
	{
		a[1] = 3;
	}
	else if (!Queue_Empty(Qd4))
	{
		a[2] = 4;
	}
	else if (!Queue_Empty(Qd5))
	{
		a[3] = 5;
	}
	return a;
}*/

void Printloadsteady(int floor, loadpt L)
{
	int a = Number_Load(L);
	CursorJump(2, 35 - floor * 6);
	printf("                 ");
	int j = L->front;
	for (int i = 0; i < a; i++)
	{
		CursorJump(2 + i*2, 35 - floor * 6);
		Change_number((L->person[j]).Aim_Floor);
		printf(" ");
		j = (j + 1) % Max_Load;
	}
	Sleep(100);
}

void Init_passenger(Queuept Qu1, Queuept Qu2, Queuept Qd2, Queuept Qu3, Queuept Qd3, Queuept Qu4, Queuept Qd4, Queuept Qd5,int i,smpt S,pmpt P)
{
	int j = 0;
	while (j < i)
	{
		pg temp=Come_Passenger(Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5);
		if (temp.ys == 0)
		{
			In_sm(S, 8);
			Print_sm(S);
		}
		else
		{
			In_pm(P, temp);
			Print_pm(P);
		}
		j++;
	}
	In_sm(S, 1);
}

void Refresh_Q(Queuept Qu1, Queuept Qu2, Queuept Qd2, Queuept Qu3, Queuept Qd3, Queuept Qu4, Queuept Qd4, Queuept Qd5,Queuept Q0,smpt S)
{
	int ys=0;
	for (int i = 01; i <= 5; i++)
	{
		Queuept Q1 = Q_floor(i, 1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
		Queuept Q2 = Q_floor(i, 1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
		clock_t end = clock();
		if (Q1 != NULL)
		{
			if (!Queue_Empty(Q1))
			{
				for (int j = Q1->front; j < Q1->rear; j++)
				{
					pg pas = Q1->Wait_p[j];
					if (end - pas.Now_Time > 15000)
					{
						ys = 1;
						OutQueue(Q1);
					}
				}
			}
		}
		if (Q2 != NULL)
		{
			if (!Queue_Empty(Q2))
			{
				for (int k = Q2->front; k < Q2->rear; k++)
				{
					pg pas = Q2->Wait_p[k];
					if (end - pas.Now_Time > 15000)
					{
						ys = 1;
						OutQueue(Q2);
					}
				}
			}
		}
	}
	if (ys)
	{
		Wait_line(Qu1);
		Wait_line(Qd2);
		Wait_line(Qu2);
		Wait_line(Qd3);
		Wait_line(Qu3);
		Wait_line(Qd4);
		Wait_line(Qu4);
		Wait_line(Qd5);
		Refresh_botton(Qu1);
		Refresh_botton(Qd2);
		Refresh_botton(Qu2);
		Refresh_botton(Qd3);
		Refresh_botton(Qu1);
		Refresh_botton(Qu3);
		Refresh_botton(Qd4);
		Refresh_botton(Qd5);
		In_sm(S, 6);
		Sleep(1000);
	}
}

void Bubble_rank(elept E,int dir)
{
	if (Number_Load(E->Load) > 1)
	{
		switch (dir)
		{
		case -1:
		{
			int i, j;
			pg temp;
			for (i = E->Load->front; (i + 1) % Max_Load != E->Load->rear; i = (i + 1) % Max_Load)
			{
				for (j = i; (j + 1) % Max_Load != E->Load->rear; j = (j + 1) % Max_Load)
				{
					if (E->Load->person[j].Aim_Floor < E->Load->person[(j + 1) % Max_Load].Aim_Floor)
					{
						temp = E->Load->person[j];
						E->Load->person[j] = E->Load->person[(j + 1) % Max_Load];
						E->Load->person[(j + 1) % Max_Load] = temp;
					}
				}
			}
			break;
		}
		case 1:
		{
			int i, j;
			pg temp;
			for (i = E->Load->front; (i + 1) % Max_Load != E->Load->rear; i = (i + 1) % Max_Load)
			{
				for (j = i; (j+1)%Max_Load!= E->Load->rear; j = (j + 1) % Max_Load)
				{
					if (E->Load->person[j].Aim_Floor > E->Load->person[(j + 1) % Max_Load].Aim_Floor)
					{
						temp = E->Load->person[j];
						E->Load->person[j] = E->Load->person[(j + 1) % Max_Load];
						E->Load->person[(j + 1) % Max_Load] = temp;
					}
				}
			}
			break;
		}
		default:
			return;
		}
	}
	else
	{
		return;
	}
}

void While_empty(elept E, Queuept Qu1, Queuept Qu2, Queuept Qd2, Queuept Qu3, Queuept Qd3, Queuept Qu4, Queuept Qd4, Queuept Qd5,Queuept Q0,int step,smpt S)
{
	Refresh_Q(Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0,S);
	int ys = 0;
	if (E->Pre_Dir == -1 || E->Pre_Dir == 0)
	{
		for (int i = 0; i < 5; i++)
		{
			if (E->Current_Floor + i >= 1 && E->Current_Floor + i <= 5)
			{
				Queuept temp = Q_floor(E->Current_Floor + i, 1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
				if (temp != NULL)
				{
					if (!Queue_Empty(temp))
					{
						ys = 1;
						pg pas = temp->Wait_p[temp->front];
						Move_Elevator(E, E->Current_Floor, pas.Current_Floor,S);
						step =abs( pas.Current_Floor - E->Current_Floor);
						E->Current_Floor = pas.Current_Floor;
						Open_door(E->Current_Floor);
						int m = temp->front;
						while(m!=temp->rear)
						{
							if (If_overload(E, pas,S))
							{
								pg pas = temp->Wait_p[m];
								InLoad(E->Load, pas);
								In_ele(E->Load, temp, pas,S);
								OutQueue(temp);
								E->Current_Dir = 1;
								E->Pre_Dir = 1;
								if (i == 0)
								{
									Print_rule(1);
									Print_rule(2);
								}
							}
							m = (m + 1) % MAXQSIZE;
						}
						Bubble_rank(E, E->Current_Dir);
						Printloadsteady(E->Current_Floor, E->Load);
						Wait_line(temp);
						Queuept temp1 = Q_floor(E->Current_Floor, -1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
						if (temp1 != NULL)
						{
							Wait_line(temp1);
						}
						Refresh_botton(temp);
						Close_door(E->Current_Floor);
						break;
					}
				}
			}
			else if (E->Current_Floor - i >= 1 && E->Current_Floor - i <= 5)
			{
				Queuept temp = Q_floor(E->Current_Floor - i, 1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
				if (temp != NULL)
				{
					if (!Queue_Empty(temp))
					{
						ys = 1;
						pg pas = temp->Wait_p[temp->front];
						Move_Elevator(E, E->Current_Floor, pas.Current_Floor,S);
						step = abs(pas.Current_Floor - E->Current_Floor);
						E->Current_Floor = pas.Current_Floor;
						Open_door(E->Current_Floor);
						int m = temp->front;
						while(m!=temp->rear)
						{
							if (If_overload(E, pas,S))
							{
								pg pas = temp->Wait_p[m];
								InLoad(E->Load, pas);
								In_ele(E->Load, temp, pas,S);
								OutQueue(temp);
								E->Current_Dir = 1;
								E->Pre_Dir = 1;
							}
							m = (m + 1) % MAXQSIZE;
						}
						Bubble_rank(E, E->Current_Dir);
						Printloadsteady(E->Current_Floor, E->Load);
						Wait_line(temp);
						Queuept temp1 = Q_floor(E->Current_Floor, -1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
						if (temp1 != NULL)
						{
							Wait_line(temp1);
						}
						Refresh_botton(temp);
						Close_door(E->Current_Floor);
						break;
					}
				}
			}
		}
		if (ys == 0)
		{
			E->Pre_Dir = 1;
			While_empty(E, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0, step,S);
		}
	}
	else
	{
		for (int i = 0; i < 5; i++)
		{
			if (E->Current_Floor + i >= 1 && E->Current_Floor + i <= 5)
			{
				Queuept temp = Q_floor(E->Current_Floor + i, -1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
				if (temp != NULL)
				{
					if (!Queue_Empty(temp))
					{
						ys = 1;
						pg pas = temp->Wait_p[temp->front];
						Move_Elevator(E, E->Current_Floor, pas.Current_Floor,S);
						step = abs(pas.Current_Floor - E->Current_Floor);
						E->Current_Floor = pas.Current_Floor;
						Open_door(E->Current_Floor);
						int m = temp->front;
						while (m != temp->rear)
						{
							if (If_overload(E, pas,S))
							{
								pg pas = temp->Wait_p[m];
								InLoad(E->Load, pas);
								In_ele(E->Load, temp, pas,S);
								OutQueue(temp);
								E->Current_Dir = -1;
								E->Pre_Dir = -1;
								if (i == 0)
								{
									Print_rule(1);
									Print_rule(3);
								}
							}
							m = (m + 1) % MAXQSIZE;
						}
						Bubble_rank(E, E->Current_Dir);
						Printloadsteady(E->Current_Floor, E->Load);
						Wait_line(temp);
						Refresh_botton(temp);
						Close_door(E->Current_Floor);
						break;
					}
				}
			}
			else if (E->Current_Floor - i >= 1 && E->Current_Floor - i <= 5)
			{
				Queuept temp = Q_floor(E->Current_Floor - i, -1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
				if (temp != NULL)
				{
					if (!Queue_Empty(temp))
					{
						ys = 1;
						pg pas = temp->Wait_p[temp->front];
						Move_Elevator(E, E->Current_Floor, pas.Current_Floor,S);
						step = abs(pas.Current_Floor - E->Current_Floor);
						E->Current_Floor = pas.Current_Floor;
						Open_door(E->Current_Floor);
						int m = temp->front;
						while (m != temp->rear)
						{
							if (If_overload(E, pas,S))
							{
								pg pas = temp->Wait_p[m];
								InLoad(E->Load, pas);
								In_ele(E->Load, temp, pas,S);
								OutQueue(temp);
								E->Current_Dir = -1;
								E->Pre_Dir = -1;
							}
							m = (m + 1) % MAXQSIZE;
						}
						Bubble_rank(E, E->Current_Dir);
						Printloadsteady(E->Current_Floor, E->Load);
						Wait_line(temp);
						Refresh_botton(temp);
						Close_door(E->Current_Floor);
						break;
					}
				}
			}
		}
		if (ys == 0)
		{
			E->Pre_Dir = -1;
			While_empty(E, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0, step,S);
		}
	}
}

int If_overload(elept E, pg pas,smpt S)
{
	if (Number_Load(E->Load) + 1 > 8)
	{
		In_sm(S, 7);
		Print_sm(S);
		Print_rule(12);
		return 0;
	}
	if (E->Load->Sum_weight + pas.Weight > 500)
	{
		In_sm(S, 7);
		Print_sm(S);
		Print_rule(12);
		return 0;
	}
	return 1;
}

int If_overtime(elept E, clock_t arrive)
{
	clock_t now = clock();
	if (now - arrive > 10000)
	{
		Sleep(500);
		printf(13);
		return 0;
	}
	else
	{
		return 1;
	}
}

void Go_out(elept E, Queuept Qu1, Queuept Qu2, Queuept Qd2, Queuept Qu3, Queuept Qd3, Queuept Qu4, Queuept Qd4, Queuept Qd5, Queuept Q0,smpt S)
{
	int j = E->Load->front;
	if (j != E->Load->rear)
	{
		In_sm(S, 5);
		Print_sm(S);
	}
	while (j != E->Load->rear)
	{
		pg pas = E->Load->person[j];
		if (pas.Aim_Floor == E->Current_Floor)
		{
			Outload(E->Load);
			int a = E->Load->Sum_weight;
			E->Load->Sum_weight = a - pas.Weight;
			Out_ele(E->Load, pas);
		}
		j = (j + 1) % Max_Load;
	}
}

void After_close(elept E, Queuept Qu1, Queuept Qu2, Queuept Qd2, Queuept Qu3, Queuept Qd3, Queuept Qu4, Queuept Qd4, Queuept Qd5,smpt S)
{
	pg pas = Come_Passenger(Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5);
	if (pas.Current_Floor == E->Current_Floor)
	{
		Sleep(300);
	}
	In_sm(S, 1);
}

void Put_zero(elept E)
{
	for (int i = 0; i < 5; i++)
	{
		*(E->Aim_Floor + i) = 0;
	}
}

void Go_up(elept E, Queuept Qu1, Queuept Qu2, Queuept Qd2, Queuept Qu3, Queuept Qd3, Queuept Qu4, Queuept Qd4, Queuept Qd5, Queuept Q0, int step,smpt S)
{
	Refresh_Q(Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0,S);
	Bubble_rank(E, E->Current_Dir);
	Printloadsteady(E->Current_Floor, E->Load);
	clock_t arrive = clock();
	int j = E->Load->front;
	while (j != E->Load->rear)
	{
		pg pas = E->Load->person[j];
		E->Aim_Floor[pas.Aim_Floor - 1] = pas.Aim_Floor;
		j = (j + 1) % Max_Load;
	}
	for (int i = 1; i <= 4; i++)
	{
		Queuept temp = Q_floor(i, 1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
		if (i > E->Current_Floor)
		{
			if (!Queue_Empty(temp))
			{
				Print_rule(4);
				E->Aim_Floor[i - 1] = i;
			}
		}
		else if (i == E->Current_Floor)
		{
			if (!Queue_Empty(temp))
			{
				Print_rule(7);
			}
		}
		else if (i < E->Current_Floor)
		{
			if (!Queue_Empty(temp))
			{
				Print_rule(5);
				Print_rule(6);
			}
		}
	}
	int in = 0;
	for (int i = 0; i < 5; i++)
	{
		int aimfloor = E->Aim_Floor[i];
		if (aimfloor > 0)
		{
			E->Aim_Floor[i] = 0;
			Move_Elevator(E, E->Current_Floor, aimfloor,S);
			step = abs(aimfloor - E->Current_Floor);
			E->Current_Floor = aimfloor;
			Open_door(E->Current_Floor);
			Go_out(E, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0,S);
			Printloadsteady(E->Current_Floor, E->Load);
			Queuept temp = Q_floor(aimfloor, 1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
			Queuept temp1 = Q_floor(E->Current_Floor, -1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
			if (temp1 != NULL)
			{
				Wait_line(temp1);
			}
			if (temp != NULL)
			{
				if (!Queue_Empty(temp))
				{
					pg pas = temp->Wait_p[temp->front];
					int m = temp->front;
					while(m!=temp->rear)
					{
						if (!If_overtime(E, arrive))
						{
							Printloadsteady(E->Current_Floor, E->Load);
							Wait_line(temp);
							Close_door(E->Current_Floor);
							break;
						}
						if (If_overload(E, pas,S))
						{
							pg pas = temp->Wait_p[m];
							InLoad(E->Load, pas);
							In_ele(E->Load, temp, pas,S);
							OutQueue(temp);
							in = 1;
						}
						else
						{
							break;
						}
						m = (m + 1) % MAXQSIZE;
					}
					Bubble_rank(E, E->Current_Dir);
					Printloadsteady(E->Current_Floor, E->Load);
					Wait_line(temp);
					Refresh_botton(temp);
					Close_door(E->Current_Floor);
					break;
				}
			}
			break;
		}
	}
	int ys = 1;
	for (int i = 0; i < 5; i++)
	{
		if (E->Aim_Floor[i] > 0)
		{
			ys = 0;
			break;
		}
	}
	if (ys == 1&&in==0)
	{
		E->Pre_Dir = 1;
		E->Current_Dir = 0;
		Queuept temp = Q_floor(E->Current_Floor, -1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
		if (temp != NULL)
		{
			Wait_line(temp);
		}
		if (Queue_Empty(temp))
		{
			Close_door(E->Current_Floor);
		}
	}
	else
	{
		Close_door(E->Current_Floor);
		After_close(E, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5,S);
	}
	if (Number_Load(E->Load) == Max_Load - 1)
	{
		Print_rule(14);
		Print_rule(15);
	}
}

void Go_down(elept E, Queuept Qu1, Queuept Qu2, Queuept Qd2, Queuept Qu3, Queuept Qd3, Queuept Qu4, Queuept Qd4, Queuept Qd5, Queuept Q0, int step,smpt S)
{
	Refresh_Q(Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0,S);
	Bubble_rank(E, E->Current_Dir);
	Printloadsteady(E->Current_Floor, E->Load);
	clock_t arrive = clock();
	int j = E->Load->front;
	while (j != E->Load->rear)
	{
		pg pas = E->Load->person[j];
		E->Aim_Floor[5 - pas.Aim_Floor] = pas.Aim_Floor;
		j = (j + 1) % Max_Load;
	}
	for (int i = 2; i <= 5; i++)
	{
		Queuept temp = Q_floor(i, -1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
		if (i < E->Current_Floor)
		{
			if (!Queue_Empty(temp))
			{
				E->Aim_Floor[5-i] = i;
				Print_rule(8);
			}
		}
		else if (i == E->Current_Floor)
		{
			if (!Queue_Empty(temp))
			{
				Print_rule(11);
			}
		}
		else if (i > E->Current_Floor)
		{
			if (!Queue_Empty(temp))
			{
				Print_rule(9);
				Print_rule(10);
			}
		}
	}
	int in = 0;
	for (int i = 0; i < 5; i++)
	{
		int aimfloor = E->Aim_Floor[i];
		if (aimfloor > 0)
		{
			E->Aim_Floor[i] = 0;
			Move_Elevator(E, E->Current_Floor, aimfloor,S);
			step = abs(aimfloor - E->Current_Floor);
			E->Current_Floor = aimfloor;
			Open_door(E->Current_Floor);
			Go_out(E, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0,S);
			Printloadsteady(E->Current_Floor, E->Load);
			Queuept temp = Q_floor(aimfloor, -1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
			if (temp != NULL)
			{
				Wait_line(temp);
			}
			if (temp != NULL)
			{
				if (!Queue_Empty(temp))
				{
					pg pas = temp->Wait_p[temp->front];
					int m = temp->front;
					while(m!=temp->rear)
					{
						if (!If_overtime(E, arrive))
						{
							Bubble_rank(E, E->Current_Dir);
							Printloadsteady(E->Current_Floor, E->Load);
							Wait_line(temp);
							Close_door(E->Current_Floor);
							break;
						}
						if (If_overload(E, pas,S))
						{
							pg pas = temp->Wait_p[m];
							InLoad(E->Load, pas);
							In_ele(E->Load, temp, pas,S);
							OutQueue(temp);
							in = 1;
						}
						else
						{
							break;
						}
						m = (m + 1) % MAXQSIZE;
					}
					Bubble_rank(E, E->Current_Dir);
					Printloadsteady(E->Current_Floor, E->Load);
					Wait_line(temp);
					Refresh_botton(temp);
					Close_door(E->Current_Floor);
					break;
				}
			}
			break;
		}
	}
	int ys = 1;
	for (int i = 0; i < 5; i++)
	{
		if (E->Aim_Floor[i] > 0)
		{
			ys = 0;
			break;
		}
	}
	if (ys == 1&&in==0)
	{
		E->Pre_Dir = -1;
		E->Current_Dir = 0;
		Queuept temp = Q_floor(E->Current_Floor, -1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
		Queuept temp1 = Q_floor(E->Current_Floor, 1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
		if (temp != NULL)
		{
			Wait_line(temp);
		}
		if (Queue_Empty(temp1))
		{
			Close_door(E->Current_Floor);
		}
	}
	else
	{
		Close_door(E->Current_Floor);
		After_close(E, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5,S);
	}
	if (Number_Load(E->Load) == Max_Load - 1)
	{
		Print_rule(14);
		Print_rule(15);
	}
}

void Print_sm(smpt S)
{
	smpt temp = S->next;
	int i = 0;
	do
	{
		int j = temp->state;
		CursorJump(0, 32 + i);
		printf("                               ");
		CursorJump(0, 32 + i);
		switch (j)
		{
		case 1:
			printf("乘客生成");
			break;
		case 2:
			printf("电梯运行中");
			break;
		case 3:
			printf("电梯停靠");
			break;
		case 4:
			printf("乘客进电梯");
			break;
		case 5:
			printf("乘客出电梯");
			break;
		case 6:
			printf("乘客失去耐心离开");
			break;
		case 7:
			printf("超载");
			break;
		case 8:
			printf("队伍太长，乘客走了");
			break;
		}
		temp = temp->next;
		i++;
	} while (temp!=NULL&&i<5);
}

void Print_pm(pmpt P)
{
	pmpt temp = P->next;
	int i = 0;
	do
	{
		pg pas = temp->pas;
		int name = pas.Name;
		int weight = pas.Weight;
		long now_time = pas.Now_Time;
		int cf = pas.Current_Floor;
		CursorJump(49, 32 + i);
		printf("                                           ");
		CursorJump(49, 32 + i);
		printf("在%d楼生成%d号乘客，体重%dKG，生成时间%ld",cf,name,weight,now_time);
		temp = temp->next;
		i++;
	} while (temp!=NULL&&i<5);
}

void Print_rule(int order)
{
	CursorJump(49, order);
	printf("/");
}

int step = 0;

int main()
{
	HideCursor();
	InitPage();
	FirstEle();
	Sleep(500);
	Queuept Qu1 = InitQueue();
	Queuept Qu2 = InitQueue();
	Queuept Qd2 = InitQueue();
	Queuept Qu3 = InitQueue();
	Queuept Qd3 = InitQueue();
	Queuept Qu4 = InitQueue();
	Queuept Qd4 = InitQueue();
	Queuept Qd5 = InitQueue();
	Queuept Q0 = NULL;
	int Pas_Counter = 0;
	int rule[15] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
	loadpt L = InitLoad();
	elept E = Initele(L);
	smpt S = Init_sm();
	pmpt P = Init_pm();
	Init_passenger(Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5,2,S,P);
	While_empty(E, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0, step,S);
	int i = 0;
	while (i<100)
	{
		Init_passenger(Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, 2,S,P);
		Print_sm(S);
		Put_zero(E);
		switch (E->Current_Dir)
		{
		case 1:
			Go_up(E, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0, step,S);
			break;
		case -1:
			Go_down(E, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0, step,S);
			break;
		case 0:
			While_empty(E, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0, step,S);
			break;
		}
		i++;
	}
	//pg group[3];
	//while (Pas_Counter < 3)
	//{
	//	pg pas = Come_Passenger(Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5);
	//	group[Pas_Counter] = pas;
	//	Pas_Counter++;
	//}
	//for (int i = 0; i < 3; i++)//楼层高的在前面
	//{
	//	for (int j = i + 1; j < 3; j++)
	//	{
	//		if (group[j].Current_Floor > group[i].Current_Floor)
	//		{
	//			pg temp = group[i];
	//			group[i] = group[j];
	//			group[j] = temp;
	//		}
	//	}
	//}
	//if (up_or_down(group[2]) == 1)
	//{
	//	Move_Elevator(E, 1, group[2].Current_Floor);
	//	E->Current_Dir = 1;
	//	E->Current_Floor = group[2].Current_Floor;
	//	step = abs(group[2].Current_Floor - 1);
	//	Open_door(group[2].Current_Floor);
	//	Queuept Q = Q_pas(group[2], 1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5);
	//	InLoad(E->Load, group[2]);
	//	In_ele(E->Load, Q, group[2]);
	//	OutQueue(Q);
	//	Printloadsteady(E->Current_Floor, E->Load);
	//	Wait_line(Q);
	//	Close_door(group[2].Current_Floor);
	//}
	//else if (up_or_down(group[0]) == -1 && up_or_down(group[1])==-1 && up_or_down(group[2])==-1)
	//{
	//	Move_Elevator(E, 1, group[2].Current_Floor);
	//	E->Current_Dir = -1;
	//	E->Current_Floor = group[2].Current_Floor;
	//	step = abs(group[2].Current_Floor - 1);
	//	Open_door(group[2].Current_Floor);
	//	Queuept Q = Q_pas(group[2], 1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5);
	//	InLoad(E->Load, group[2]);
	//	In_ele(E->Load, Q, group[2]);
	//	OutQueue(Q);
	//	Printloadsteady(E->Current_Floor, E->Load);
	//	Wait_line(Q);
	//	Close_door(group[2].Current_Floor);
	//}
	//else
	//{
	//	for (int i = 1; i >= 0; i--)
	//	{
	//		if (up_or_down(group[i]) == 1)
	//		{
	//			Move_Elevator(E, 1, group[i].Current_Floor);
	//			E->Current_Dir = 1;
	//			E->Current_Floor = group[i].Current_Floor;
	//			step = abs(group[i].Current_Floor - 1);
	//			Open_door(group[i].Current_Floor);
	//			Queuept Q = Q_pas(group[i], 1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5);
	//			InLoad(E->Load, group[i]);
	//			In_ele(E->Load, Q, group[i]);
	//			OutQueue(Q);
	//			Printloadsteady(E->Current_Floor, E->Load);
	//			Wait_line(Q);
	//			Close_door(group[i].Current_Floor);
	//		}
	//		break;
	//	}
	//}

	//while (Sum_rule(rule) != 0)
	//{
	//	if (E->Current_Dir == 1)
	//	{
	//		if (step > 0)
	//		{

	//			for (int i = 0; i < step; i++)
	//			{
	//				pg pas = Come_Passenger(Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5);
	//			}
	//		}
	//		int* aimfloor1 = malloc(sizeof(int) * 5);
	//		int* aimfloor2 = malloc(sizeof(int) * 4);
	//		aimfloor1 = Get_upaimfloor(E);
	//		aimfloor2 = Get_upcurrentfloor(Qu1, Qu2, Qu3, Qu4);
	//		if (Number_Load(E->Load) == 10)
	//		{
	//			for (int i = 0; i < 4; i++)
	//			{
	//				for (int j = 0; j < 5; j++)
	//				{
	//					if (aimfloor1[j] == aimfloor2[i] && aimfloor1[j] != 0)
	//					{
	//						chaozai(aimfloor1[j]);
	//					}
	//				}
	//			}
	//			int aimfloor2[4] = { 0,0,0,0};
	//		}
	//		int aimfloor3[5] = { 0,0,0,0,0 };
	//		int cf = E->Current_Floor;
	//		for (int i = 0; i < 5; i++)
	//		{
	//			if (aimfloor1[i] >= cf)
	//			{
	//				aimfloor3[aimfloor1[i] - 1] = aimfloor1[i];
	//			}
	//		}
	//		for (int i = 0; i < 4; i++)
	//		{
	//			if (aimfloor2[i] >= cf)
	//			{
	//				aimfloor3[aimfloor2[i] - 1] = aimfloor2[i];
	//			}
	//		}
	//		for (int i = 0; i < 5; i++)
	//		{
	//			if (aimfloor3[i] > 0)
	//			{
	//				Move_Elevator(E, E->Current_Floor, aimfloor3[i]);
	//				step = E->Current_Floor - aimfloor3[i];
	//				E->Current_Floor = aimfloor3[i];
	//				int if_up = 0;
	//				for (int j = i + 1; j < 5; j++)
	//				{
	//					if (aimfloor3[j] > 0)
	//					{
	//						if_up = 1;
	//						break;
	//					}
	//				}
	//				if (if_up == 0)
	//				{
	//					E->Current_Dir = 0;
	//				}
	//				break;
	//			}
	//		}
	//		int front = E->Load->front;
	//		int rear = E->Load->rear;
	//		Open_door(E->Current_Floor);
	//		for (int i = front; i < rear; i++)
	//		{
	//			pg pas = E->Load->person[front];
	//			if (pas.Aim_Floor == E->Current_Floor)
	//			{
	//				Out_ele(E->Load, pas);
	//				Outload(E->Load);
	//			}
	//		}
	//		int ys = 0;
	//		for (int i = 0; i < 4; i++)
	//		{
	//			if (aimfloor2[i] == E->Current_Floor)
	//			{
	//				ys = 1;
	//				break;
	//			}
	//		}
	//		Queuept Q = Q_floor(E->Current_Floor, -1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5,Q0);
	//		if (ys == 1)
	//		{
	//			Queue temp = *Q;
	//			OutQueue(Q);
	//			if (!Queue_Empty(&temp))
	//			{
	//				int front1 = Q->front;
	//				int rear1 = Q->rear;
	//				clock_t chaoshi_start = clock();
	//				for (int i = front; i < rear; i++)
	//				{
	//					pg pas = Q->Wait_p[i];
	//					Sleep(500);
	//					clock_t chaoshi_end = clock();
	//					int judge_weight = InLoad(E->Load, pas);
	//					if (judge_weight == 0)
	//					{
	//						chaozai(E->Current_Floor);
	//						Sleep(500);
	//						break;
	//					}
	//					else if (chaoshi_end - chaoshi_start > 1500)
	//					{
	//						CursorJump(0, 32);
	//						printf("电梯停靠超时，不再上客");
	//						rule[12] = 0;
	//						break;
	//					}
	//					else
	//					{
	//						In_ele(E->Load, &temp, pas);
	//					}
	//				}
	//			}
	//		}
	//		else
	//		{
	//			Queuept temp = Q_floor(E->Current_Floor, -1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
	//			if (!Queue_Empty(temp))
	//			{
	//				int front1 = Q->front;
	//				int rear1 = Q->rear;
	//				for (int i = front; i < rear; i++)
	//				{
	//					pg pas = temp->Wait_p[i];
	//					if (pas.Aim_Floor < pas.Current_Floor)
	//					{
	//						E->Current_Dir = up_or_down(pas);
	//						InLoad(E->Load, pas);
	//						In_ele(E->Load, temp, pas);
	//						OutQueue(temp);
	//					}
	//				}
	//			}
	//		}
	//		Printloadsteady(E->Current_Floor, E->Load);
	//		Wait_line(Q);
	//		Close_door(E->Current_Floor);
	//		pg pas_temp = Come_Passenger(Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5);
	//		if (pas_temp.Current_Floor == E->Current_Floor)
	//		{
	//			CursorJump(0, 32);
	//			printf("已关门，不上客");
	//			Sleep(500);
	//			rule[6] = 0;
	//		}
	//	}
	//	else if (E->Current_Dir == -1)
	//	{
	//		if (step > 0)
	//		{
	//			for (int i = 0; i < step; i++)
	//			{
	//				pg pas = Come_Passenger(Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5);
	//			}
	//		}
	//		int* aimfloor1 = malloc(sizeof(int) * 5);
	//		int* aimfloor2 = malloc(sizeof(int) * 4);
	//		aimfloor1 = Get_downaimfloor(E);
	//		aimfloor2 = Get_downcurrentfloor(Qd2, Qd3, Qd4, Qd5);
	//		if (Number_Load(E->Load) == 10)
	//		{
	//			for (int i = 0; i < 4; i++)
	//			{
	//				for (int j = 0; j < 5; j++)
	//				{
	//					if (aimfloor1[j] == aimfloor2[i] && aimfloor1[j] != 0)
	//					{
	//						chaozai(aimfloor1[j]);
	//					}
	//				}
	//			}
	//			int aimfloor2[4] = { 0 };
	//		}
	//		int aimfloor3[5] = { 0 };
	//		int cf = E->Current_Floor;
	//		for (int i = 0; i < 5; i++)
	//		{
	//			if (aimfloor1[i] <= cf)
	//			{
	//				aimfloor3[5 - aimfloor1[i]] = aimfloor1[i];
	//			}
	//		}
	//		for (int i = 0; i < 4; i++)
	//		{
	//			if (aimfloor2[i] <= cf)
	//			{
	//				aimfloor3[5 - aimfloor2[i]] = aimfloor2[i];
	//			}
	//		}
	//		for (int i = 0; i < 5; i++)
	//		{
	//			if (aimfloor3[i] > 0)
	//			{
	//				Move_Elevator(E, E->Current_Floor, aimfloor3[i]);
	//				step = E->Current_Floor - aimfloor3[i];
	//				E->Current_Floor = aimfloor3[i];
	//				int if_down = 0;
	//				for (int j = i + 1; j < 5; j++)
	//				{
	//					if (aimfloor3[j] > 0)
	//					{
	//						if_down = 1;
	//						break;
	//					}
	//				}
	//				if (if_down == 0)
	//				{
	//					E->Current_Dir = 0;
	//				}
	//				break;
	//			}
	//		}
	//		int front = E->Load->front;
	//		int rear = E->Load->rear;
	//		Open_door(E->Current_Floor);
	//		for (int i = front; i < rear; i++)
	//		{
	//			pg pas = E->Load->person[front];
	//			if (pas.Aim_Floor == E->Current_Floor)
	//			{
	//				Out_ele(E->Load, pas);
	//				Outload(E->Load);
	//			}
	//		}
	//		int ys = 0;
	//		for (int i = 0; i < 4; i++)
	//		{
	//			if (aimfloor2[i] == E->Current_Floor)
	//			{
	//				ys = 1;
	//				break;
	//			}
	//		}
	//		Queuept Q = Q_floor(E->Current_Floor, -1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5,Q0);
	//		if (ys == 1)
	//		{
	//			Queue temp = *Q;
	//			OutQueue(Q);
	//			if (!Queue_Empty(Q))
	//			{
	//				int front1 = Q->front;
	//				int rear1 = Q->rear;
	//				clock_t chaoshi_start = clock();
	//				for (int i = front; i < rear; i++)
	//				{
	//					pg pas = Q->Wait_p[i];
	//					Sleep(500);
	//					clock_t chaoshi_end = clock();
	//					int judge_weight = InLoad(E->Load, pas);
	//					if (judge_weight == 0)
	//					{
	//						chaozai(E->Current_Floor);
	//						Sleep(500);
	//						break;
	//					}
	//					else if (chaoshi_end - chaoshi_start > 1500)
	//					{
	//						CursorJump(0, 32);
	//						printf("电梯停靠超时，不再上客");
	//						rule[12] = 0;
	//						break;
	//					}
	//					else
	//					{
	//						In_ele(E->Load, &temp, pas);
	//					}
	//				}
	//			}
	//		}
	//		else
	//		{
	//			Queuept temp = Q_floor(E->Current_Floor, 1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
	//			if (!Queue_Empty(temp))
	//			{
	//				int front1 = Q->front;
	//				int rear1 = Q->rear;
	//				for (int i = front; i < rear; i++)
	//				{
	//					pg pas = temp->Wait_p[i];
	//					if (pas.Aim_Floor > pas.Current_Floor)
	//					{
	//						E->Current_Dir = up_or_down(pas);
	//						InLoad(E->Load, pas);
	//						In_ele(E->Load, temp, pas);
	//						OutQueue(temp);
	//					}
	//				}
	//			}
	//		}
	//		Printloadsteady(E->Current_Floor, E->Load);
	//		Wait_line(Q);
	//		Close_door(E->Current_Floor);
	//		pg pas_temp = Come_Passenger(Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5);
	//		if (pas_temp.Current_Floor == E->Current_Floor)
	//		{
	//			CursorJump(0, 32);
	//			printf("已关门，不上客");
	//			Sleep(500);
	//			rule[10] = 0;
	//		}
	//	}
	//	else
	//	{
	//		int find = 0;
	//		for (int i = 0; i < 4; i++)
	//		{
	//			if (1 <= E->Current_Floor + i&& E->Current_Floor + i < 5)
	//			{
	//				Queuept Q1 = Q_floor(E->Current_Floor + i, 1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5,Q0);
	//				if (Q1 != NULL)
	//				{
	//					if (!Queue_Empty(Q1))
	//					{
	//						int front = Q1->front;
	//						pg pas = Q1->Wait_p[front];
	//						Move_Elevator(E, E->Current_Floor, pas.Current_Floor);
	//						E->Current_Dir = 1;
	//						step = pas.Current_Floor - E->Current_Floor;
	//						Open_door(E->Current_Floor);
	//						In_ele(E->Load, Q1, pas);
	//						OutQueue(Q1);
	//						find = 1;
	//					}
	//				}
	//			}
	//			if (1<E->Current_Floor+i&& E->Current_Floor + i <=5)
	//			{
	//				if (find == 0)
	//				{
	//					Queuept Q2 = Q_floor(E->Current_Floor + i, -1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
	//					if (Q2 != NULL)
	//					{
	//						if (!Queue_Empty(Q2))
	//						{
	//							int front = Q2->front;
	//							pg pas = Q2->Wait_p[front];
	//							Move_Elevator(E, E->Current_Floor, pas.Current_Floor);
	//							E->Current_Dir = -1;
	//							step = abs(pas.Current_Floor - E->Current_Floor);
	//							Open_door(E->Current_Floor);
	//							In_ele(E->Load,Q2, pas);
	//							OutQueue(Q2);
	//							find = 1;
	//						}
	//					}
	//				}
	//			}
	//			if (i != 0)
	//			{
	//				if (1 <= E->Current_Floor - i &&E->Current_Floor + i < 5)
	//				{
	//					if (find == 0)
	//					{
	//						Queuept Q1 = Q_floor(E->Current_Floor + i, 1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
	//						if (Q1 != NULL)
	//						{
	//							if (!Queue_Empty(Q1))
	//							{
	//								int front = Q1->front;
	//								pg pas = Q1->Wait_p[front];
	//								Move_Elevator(E, E->Current_Floor, pas.Current_Floor);
	//								E->Current_Dir = 1;
	//								step = pas.Current_Floor - E->Current_Floor;
	//								Open_door(E->Current_Floor);
	//								In_ele(E->Load, Q1, pas);
	//								OutQueue(Q1);
	//								find = 1;
	//							}
	//						}
	//					}
	//				}
	//				if ((5 >= E->Current_Floor - i && E->Current_Floor + i > 1) && (find == 0))
	//				{
	//					if (find == 0)
	//					{
	//						Queuept Q2 = Q_floor(E->Current_Floor + i, -1, Qu1, Qu2, Qd2, Qu3, Qd3, Qu4, Qd4, Qd5, Q0);
	//						if (Q2 != NULL)
	//						{
	//							if (!Queue_Empty(Q2))
	//							{
	//								int front = Q2->front;
	//								pg pas = Q2->Wait_p[front];
	//								Move_Elevator(E, E->Current_Floor, pas.Current_Floor);
	//								E->Current_Dir = -1;
	//								step = abs(pas.Current_Floor - E->Current_Floor);
	//								Open_door(E->Current_Floor);
	//								In_ele(E->Load,Q2, pas);
	//								OutQueue(Q2);
	//								find = 1;
	//							}
	//						}
	//					}
	//				}
	//			}
	//			if (find == 1)
	//			{
	//				break;
	//			}
	//		}
	//	}
	//}
	system("pause > nul");
	return 0;
}