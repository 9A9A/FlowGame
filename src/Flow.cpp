#include "Flow.h"
//////////////////////////////////////////////////////////////////////////////
/// Class pathVariant
//////////////////////////////////////////////////////////////////////////////
pathVariant::pathVariant()
{

}
void pathVariant::Output()
{
	printf("\n=======================================\n");
	printf("Line id : %d  Manhattan Distance: %d \n",globalId,m_Distance);
	printf("\n=======================================\n");
	for(int i=0;i<H*W;i++)
	{
		if(path[i].g_Pos_x!= EMPTY_CELL )
		{
			printf("\nx:%d || y:%d",path[i].g_Pos_x,path[i].g_Pos_y);
		}
	}
	printf("\n=======================================\n");
	for(int g = 0; g<H;g++)
	{
		printf("\n");
		for(int h = 0; h<W;h++)
		{
			printf("%d\t",gridArray[g][h]);
		}
	}
}
void pathVariant::clearPath()
{
	for(int i=0;i<H*W;i++)
	{
		path[i].g_Pos_x = EMPTY_CELL;
		path[i].g_Pos_y = EMPTY_CELL;
	}
}
void pathVariant::entry(MapFileHeader G_GameRule, FlowFileArray *G_GameMatrix,int FlowId)
{
	pathRecognize(G_GameRule,G_GameMatrix,FlowId);
	m_offset = M_OFFSET;
	globalId = FlowId;
	loadGrid(G_GameRule,G_GameMatrix);
	traceGrid(G_GameRule,G_GameMatrix);
	m_Distance = ManhattanDistance(x1,x2,y1,y2);
}
void pathVariant::pathRecognize(MapFileHeader G_GameRule,FlowFileArray *G_GameMatrix,int id)
{
	x1 = G_GameMatrix[id].g_StartPos_x+1;
	x2 = G_GameMatrix[id].g_EndPos_x+1;
	y1 = G_GameMatrix[id].g_StartPos_y+1;
	y2 = G_GameMatrix[id].g_EndPos_y+1;
}
void pathVariant::loadGrid(MapFileHeader G_GameRule,FlowFileArray *G_GameMatrix)
{
	H = G_GameRule.g_Height + m_offset;
	W = G_GameRule.g_Width + m_offset;
	gridArray = (int **)malloc(sizeof(int *)*H);
	for(int i=0;i<W;i++)
	{
		gridArray[i] = (int *)malloc(sizeof(int)*W);
	}
	path =(coordinate *)malloc(sizeof(coordinate)*H*W);
	clearPath();
	for(int g = 0; g<H;g++)
	{
		for(int h = 0; h<W;h++)
		{
			if((g == 0 || h == 0) || (g == H-1 || h == W-1))
			{
				gridArray[g][h] = WALL;
			}
			else
			{
				gridArray[g][h] = BLANK;
			}
		}
	}
	for(int i=0;i<G_GameRule.g_StartPosCount;i++)
	{
		short g_TempStart_x,g_TempStart_y,g_TempEnd_x,g_TempEnd_y;
		g_TempStart_x = G_GameMatrix[i].g_StartPos_x+1;
		g_TempStart_y = G_GameMatrix[i].g_StartPos_y+1;
		g_TempEnd_x = G_GameMatrix[i].g_EndPos_x+1;
		g_TempEnd_y = G_GameMatrix[i].g_EndPos_y+1;
		gridArray[g_TempStart_y][g_TempStart_x] = WALL;
		gridArray[g_TempEnd_y][g_TempEnd_x] = WALL;
	}
	gridArray[y1][x1] = BLANK;
	gridArray[y2][x2] = BLANK;
}
bool pathVariant::AlgorithmLee(int ax, int ay, int bx, int by)   // поиск пути из €чейки (ax, ay) в €чейку (bx, by)
{
	int dx[4] = {1, 0, -1, 0};   // смещени€, соответствующие сосед€м €чейки
	int dy[4] = {0, 1, 0, -1};   // справа, снизу, слева и сверху
	int d, x, y, k;
	bool stop;
	// распространение волны
	d = 0;
	gridArray[ay][ax] = 0;            // стартова€ €чейка помечена 0
	do 
	{
		stop = true;               // предполагаем, что все свободные клетки уже помечены
		for( y = 0; y < H; ++y )
		{
			for( x = 0; x < W; ++x )
			{
				if ( gridArray[y][x] == d )                         // €чейка (x, y) помечена числом d
				{
					for ( k = 0; k < 4; ++k )                    // проходим по всем непомеченным сосед€м
					{
						if ( gridArray[y + dy[k]][x + dx[k]] == BLANK )
						{
							stop = false;                            // найдены непомеченные клетки
							gridArray[y + dy[k]][x + dx[k]] = d + 1;      // распростран€ем волну
						}
					}
				}
			}
		}
		d++;
	} 
	while ( !stop && gridArray[by][bx] == BLANK );
	if (gridArray[by][bx] == BLANK) return false;  // путь не найден
	// восстановление пути
	len = gridArray[by][bx];            // длина кратчайшего пути из (ax, ay) в (bx, by)
	x = bx;
	y = by;
	d = len;
	while ( d > 0 )
	{
		path[d].g_Pos_x = x;
		path[d].g_Pos_y = y;                   // записываем €чейку (x, y) в путь
		d--;
		for (k = 0; k < 4; ++k)
		{
			if (gridArray[y + dy[k]][x + dx[k]] == d)
			{
				x = x + dx[k];
				y = y + dy[k];           // переходим в €чейку, котора€ на 1 ближе к старту
				break;
			}
		}
	}
	path[0].g_Pos_x = ax;
	path[0].g_Pos_y = ay;                    // теперь px[0..len] и py[0..len] - координаты €чеек пути
	return true;
}
void pathVariant::traceGrid(MapFileHeader G_GameRule,FlowFileArray *G_GameMatrix)
{
	AlgorithmLee(x1,y1,x2,y2);
}
//////////////////////////////////////////////////////////////////////////////
/// Class fieldComparsion
//////////////////////////////////////////////////////////////////////////////
void fieldComparsion::initialize(MapFileHeader G_GameRule)
{
	m_editable = _TRUE;
	m_counter = NULL;
	m_idArray = m_idArraySize(m_idArray,G_GameRule);
}
int *fieldComparsion::m_idArraySize(int *i_array,MapFileHeader G_GameRule)
{
	i_array = (int *)malloc(sizeof(i_array)*G_GameRule.g_StartPosCount);
	for(int i=0;i<G_GameRule.g_StartPosCount;i++)
	{
		i_array[i] = EMPTY_CELL;
	}
}

