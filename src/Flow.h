#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <math.h>
#include <sstream>
#include <Windows.h>
#define FREE_BLOCK 0
#define _TRUE 1
#define _FALSE 0
#define MAX_GAMEFIELD_LENGTH 16
#define MAX_GAMEFIELD_WIDTH 16
#define SIDE_MAX_COUNT 4
#define COMPLETE 100
#define BUFFSIZE 256
#define M_OFFSET 2
#define WALL -1
#define BLANK -2
#define EMPTY_CELL -1
using namespace std;
typedef unsigned int uInt;

typedef struct // Заголовок файла игрового уровня
{
	short g_Height; // размер поля по оси Y
	short g_Width; // размер поля по оси X
	int f_offset; // смещение заголовка относительно массива 
	short g_StartPosCount; // количество стартовых позиций (количество структур FlowFileHeader)
} MapFileHeader;
typedef struct // Элемент игрового поля (количество FlowFileHeader * g_StartPosCount);
{
	short g_PosKeyValue; // id игровой линии
	short g_StartPos_x; // стартовая позиция линии по X (от 0 до g_Width - 1)
	short g_StartPos_y; // стартовая позиция линии по Y (от 0 до g_Height - 1)
	short g_EndPos_x; // конечная позиция линии по X
	short g_EndPos_y; // конечная позиция линии по Y
	short g_idPos; // id игровой точки
} FlowFileArray;
typedef struct
{
	bool walkable; // возможен ли проход (изменение вершины)
	short g_PosKeyValue; // id игровой линии
	short g_idPos; // id игровой точки
} cell;
typedef struct
{
	short g_Pos_x;
	short g_Pos_y;
} coordinate;
typedef struct
{
	short g_PosKeyValue;
	short g_idStartPos;
	short g_idEndPos;
	short *g_PointListing;
	short current;
	bool done;
} FlowLine;
int main();
void GameMenu();
void EndGame();
void WriteFileHeader();
void ReadFileHeader();
int ManhattanDistance(int x1, int x2, int y1, int y2);
void BuildGameMatrix(FlowFileArray *,MapFileHeader);
void DisplayGameMatrix(MapFileHeader G_GameRule,cell **g_GameField, DWORD *g_IdList,FlowFileArray *G_GameMatrix,FlowLine *g_FlowLine,bool end);
short ConvertCoordinateToId(short x, short y,MapFileHeader G_GameRule);
short *NCells(cell **g_GameField,FlowFileArray *G_GameMatrix,MapFileHeader G_GameRule,short x, short y);
unsigned int UsedCells(cell **,MapFileHeader,FlowFileArray *);
unsigned int UsedCellsInPercentage(cell **,MapFileHeader,FlowFileArray *);
FlowLine *FlowLinesArrays(MapFileHeader G_GameRule,cell **g_GameField,FlowFileArray *mpfArray);
coordinate ConvertIdToCoordinate(short g_Pos_id,MapFileHeader G_GameRule);
DWORD *BuildListId(MapFileHeader G_GameRule,cell **g_GameField);
cell **AllocateMemory(MapFileHeader);
void AutoSolve(MapFileHeader G_GameRule, FlowFileArray *G_GameMatrix);
bool AlgorithmLee(int ax, int ay, int bx, int by);
class pathVariant
{
public:
	pathVariant();
	int x1;
	int y1;
	int x2;
	int y2;
	int len;
	int H;
	int W;
	int globalId;
	int **gridArray;
	int m_offset;
	int m_Distance;
	coordinate *path;
	void entry(MapFileHeader G_GameRule, FlowFileArray *G_GameMatrix,int FlowId);
	void Output();
	void clearPath();
private:
	bool AlgorithmLee(int ax, int ay, int bx, int by);
	void pathRecognize(MapFileHeader G_GameRule,FlowFileArray *G_GameMatrix,int id);
	void loadGrid(MapFileHeader G_GameRule,FlowFileArray *G_GameMatrix);
	void traceGrid(MapFileHeader G_GameRule,FlowFileArray *G_GameMatrix);
};

