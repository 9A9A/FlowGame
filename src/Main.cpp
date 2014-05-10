#include "Flow.h"

void WriteFileHeader() // √енераци€ игрового пол€ (запись в файл)
{
	TCHAR buffer[BUFFSIZE];
	TCHAR stdPath[BUFFSIZE] = TEXT("MapFile.map");
	MapFileHeader mpfHeader;
	FlowFileArray *mpfArray;
	mpfHeader.f_offset = sizeof(MapFileHeader);
	DWORD CurrentDirectoryBuffer;
	DWORD Header_offset;
	CurrentDirectoryBuffer = GetCurrentDirectory(256,buffer);
	HANDLE hFile;
	CharToOem(buffer,buffer);
	cout << "\nEnter size of game field in x : ";
	cin >> mpfHeader.g_Width;
	cout << "\nEnter size of game field in y : ";
	cin >> mpfHeader.g_Height;
	cout << "\nEnter count of starting positions : ";
	cin >> mpfHeader.g_StartPosCount;
	mpfArray = (FlowFileArray *)malloc(sizeof(FlowFileArray)*mpfHeader.g_StartPosCount);
	for(int i=0;i<mpfHeader.g_StartPosCount;i++)
	{
		cout << "\nEnter x coord of startpos: ";;
		cin >> mpfArray[i].g_StartPos_x;
		cout << "\nEnter y coord of startpos: ";
		cin >> mpfArray[i].g_StartPos_y;		
		cout << "\nEnter x coord of endpos: ";
		cin >> mpfArray[i].g_EndPos_x;
		cout << "\nEnter y coord of endpos: ";
		cin >> mpfArray[i].g_EndPos_y;
		//cout << "\nEnter Keyvalue for this line:";
		//cin >> mpfArray[i].g_idPos;
		mpfArray[i].g_PosKeyValue = i+1;
	}
	hFile = CreateFile(stdPath,GENERIC_WRITE, 0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	WriteFile(hFile,(LPVOID)&mpfHeader,sizeof(mpfHeader),&Header_offset,NULL);
	for(int i=0;i<mpfHeader.g_StartPosCount;i++)
	{
		WriteFile(hFile,(LPVOID)&mpfArray[i],sizeof(mpfArray[i]),&Header_offset,NULL);
	}

	free(mpfArray);
	CloseHandle(hFile);
}
void ReadFileHeader() // ќткрытие игрового пол€
{
	TCHAR stdPath[BUFFSIZE] = TEXT("MapFile.map");
	static MapFileHeader mpfHeader;
	HANDLE hFile;
	DWORD Header_offset;
	static FlowFileArray *mpfArray;
	DWORD Data_offset;
	hFile = CreateFile(stdPath,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	ReadFile(hFile,(LPVOID)&mpfHeader,sizeof(MapFileHeader),&Header_offset,NULL);
	TCHAR **GameArray;
	GameArray = (TCHAR **)malloc(sizeof(TCHAR *)*mpfHeader.g_Height);
	for(int i=0;i<mpfHeader.g_Height;i++)
	{
		GameArray[i] = (TCHAR *)malloc(sizeof(TCHAR *)*mpfHeader.g_Width);
	}
	mpfArray = (FlowFileArray *)malloc(sizeof(FlowFileArray)*mpfHeader.g_StartPosCount);
	SetFilePointer(hFile,mpfHeader.f_offset,NULL,FILE_BEGIN);
	printf("%d %d %d \n",mpfHeader.g_Height,mpfHeader.g_Width,mpfHeader.f_offset);
	for(int i=0;i<mpfHeader.g_StartPosCount;i++)
	{
		printf("Id line is : %d\n",i);
		ReadFile(hFile,(LPVOID)&mpfArray[i],sizeof(mpfArray[i]),&Data_offset,NULL);
		printf("%d \n",mpfArray[i].g_StartPos_x);
		printf("%d \n",mpfArray[i].g_StartPos_y);
		printf("%d \n",mpfArray[i].g_EndPos_x);
		printf("%d \n",mpfArray[i].g_EndPos_y);
		printf("%d \n",mpfArray[i].g_PosKeyValue);
		printf("%d \n\n",mpfArray[i].g_idPos);
	}
	BuildGameMatrix(mpfArray,mpfHeader);
	CloseHandle(hFile);
}
cell **AllocateMemory(MapFileHeader G_GameRule) // выделение пам€ти под игровую матрицу
{
	cell **g_GameField = (cell **)malloc(sizeof(cell *)*G_GameRule.g_Height);
	for(int i=0;i<G_GameRule.g_Height;i++)
	{
		g_GameField[i]=(cell *)malloc(sizeof(cell)*G_GameRule.g_Width);
	}
	for(int i=0;i<G_GameRule.g_Height;i++)
	{
		for(int j=0;j<G_GameRule.g_Width;j++)
		{
			g_GameField[i][j].walkable = 1;
			g_GameField[i][j].g_PosKeyValue = FREE_BLOCK;
		}
	}
	return g_GameField;
}
DWORD *BuildListId(MapFileHeader G_GameRule,cell **g_GameField) // построение списка idList
{
	DWORD *g_ListId;
	g_ListId = (DWORD *)malloc(sizeof(DWORD) * (G_GameRule.g_Height * G_GameRule.g_Width));
	int counter = 0;
	for(int i=0;i<G_GameRule.g_Height;i++)
	{
		for(int j=0;j<G_GameRule.g_Width;j++)
		{
			g_ListId[counter] = g_GameField[i][j].g_idPos;
			counter++;
		}
	}
	return g_ListId;
}
void DisplayGameMatrix(MapFileHeader G_GameRule,cell **g_GameField, DWORD *g_IdList,FlowFileArray *G_GameMatrix,FlowLine *g_FlowLine,short lines_done) // вывод на экран матрицы
{
	HANDLE text_color = GetStdHandle(STD_OUTPUT_HANDLE);
	for(int i=0;i<G_GameRule.g_Height;i++)
	{
		for(int j=0;j<G_GameRule.g_Width;j++)
		{
			SetConsoleTextAttribute(text_color,g_GameField[i][j].g_PosKeyValue);
			if(g_GameField[i][j].g_PosKeyValue == FREE_BLOCK)
			{
				printf("%d\t",g_GameField[i][j].g_PosKeyValue);
			}
			else
			{
				SetConsoleTextAttribute(text_color,g_GameField[i][j].g_PosKeyValue*20);
				printf("%d\t",g_GameField[i][j].g_PosKeyValue);
			}
		}
		printf("\n");
	}
	SetConsoleTextAttribute(text_color, 15);
	printf("\nUsed Cells : %d\tIn Percentage : %d%%\t",UsedCells(g_GameField,G_GameRule,G_GameMatrix),UsedCellsInPercentage(g_GameField,G_GameRule,G_GameMatrix));
	for(int i=0;i<G_GameRule.g_StartPosCount;i++)
	{
		printf("\nListing %d :",i);
		for(int j=0;j<(G_GameRule.g_Height + G_GameRule.g_Width)*2;j++)
		{
			printf("%d ",g_FlowLine[i].g_PointListing[j]);
		}
		printf("END: %d",g_FlowLine[i].done);
	}
	printf("\n");
	printf("Lines done: %d\n",lines_done);
}
short FlowLineId(short keyvalue,MapFileHeader G_GameRule,FlowFileArray *G_GameMatrix)
{
	short counter;
	for(int i=0;i<G_GameRule.g_StartPosCount;i++)
	{
		if(keyvalue == G_GameMatrix[i].g_PosKeyValue)
		{
			counter = i;
		}
	}
	return counter;
}
void UpdateGameField(FlowFileArray *G_GameMatrix,MapFileHeader G_GameRule,cell **g_GameField,DWORD *g_IdList,FlowLine *g_FlowLine)
{
	short x,y,keyvalue,flag,temp_id,temp_value,temp_lineid,cell_id,lines_done=0,temp_listing_id;
	short *n;
	bool breakout;
	while(true)
	{
		system("cls");
		flag = 0;
		DisplayGameMatrix(G_GameRule,g_GameField,g_IdList,G_GameMatrix,g_FlowLine,lines_done);
		lines_done = 0;
		scanf("%d %d %d",&x,&y,&keyvalue);
		if((0 <= y < G_GameRule.g_Height) && (0 <= x < G_GameRule.g_Width) && (g_GameField[y][x].walkable != 0))
		{
			for(int i=0;i<G_GameRule.g_StartPosCount;i++)
			{
				if(keyvalue==G_GameMatrix[i].g_PosKeyValue)
				{
					flag = 1;
				}
			}
			cell_id = ConvertCoordinateToId(x,y,G_GameRule);
			temp_value = keyvalue;
			if((g_GameField[y][x].g_PosKeyValue != keyvalue) && (g_GameField[y][x].g_PosKeyValue != FREE_BLOCK))
			{
				coordinate buffer;
				temp_lineid = FlowLineId(g_GameField[y][x].g_PosKeyValue,G_GameRule,G_GameMatrix);
				for(int i=0;i<(G_GameRule.g_Height + G_GameRule.g_Width)*2;i++)
				{
					if(g_FlowLine[temp_lineid].g_PointListing[i] == cell_id)
					{
						temp_listing_id = i;
					}
				}
				g_FlowLine[temp_lineid].current = temp_listing_id;
				for(int i=temp_listing_id;i<((G_GameRule.g_Height + G_GameRule.g_Width)*2)-1;i++)
				{
					buffer = ConvertIdToCoordinate(g_FlowLine[temp_lineid].g_PointListing[i],G_GameRule);
					g_FlowLine[temp_lineid].g_PointListing[i] = -1;
					g_GameField[buffer.g_Pos_y][buffer.g_Pos_x].g_PosKeyValue = 0;
				}
				g_FlowLine[temp_lineid].g_PointListing[((G_GameRule.g_Height + G_GameRule.g_Width)*2)-1] = g_FlowLine[temp_lineid].g_idEndPos;
				buffer = ConvertIdToCoordinate(g_FlowLine[temp_lineid].g_idEndPos,G_GameRule);
				g_GameField[buffer.g_Pos_y][buffer.g_Pos_x].g_PosKeyValue = g_FlowLine[temp_lineid].g_PosKeyValue;
				if(g_FlowLine[temp_lineid].done == _TRUE)
				{
					g_FlowLine[temp_lineid].done = _FALSE;
				}
			}
			temp_lineid = FlowLineId(keyvalue,G_GameRule,G_GameMatrix);
			if((g_FlowLine[temp_lineid].g_PosKeyValue == keyvalue)&&(g_FlowLine[temp_lineid].done!=_TRUE))
			{
				coordinate temp;
				temp = ConvertIdToCoordinate(g_FlowLine[temp_lineid].g_PointListing[g_FlowLine[temp_lineid].current-1],G_GameRule);
				n = NCells(g_GameField,G_GameMatrix,G_GameRule,temp.g_Pos_x,temp.g_Pos_y);
				for(int h=0;h<sizeof(g_FlowLine[temp_lineid].g_PointListing);h++)
				{
					if(g_FlowLine[temp_lineid].g_PointListing[h]==ConvertCoordinateToId(x,y,G_GameRule))
					{
						breakout = _TRUE;
					}
				}
				if(breakout == _FALSE)
				{
					for(int i=0;i<SIDE_MAX_COUNT;i++)
					{
						if(n[i]==cell_id)
						{
							if(flag == FREE_BLOCK)
							{
								keyvalue = _FALSE;
							}
							if(g_GameField[y][x].walkable == _FALSE)
							{
								keyvalue = temp_value;
							}
							else
							{
								g_GameField[y][x].g_PosKeyValue = keyvalue;
								temp = ConvertIdToCoordinate(cell_id,G_GameRule);
								n = NCells(g_GameField,G_GameMatrix,G_GameRule,temp.g_Pos_x,temp.g_Pos_y);
								for(int i=0;i<SIDE_MAX_COUNT;i++)
								{
									if(n[i]==g_FlowLine[temp_lineid].g_idEndPos)
									{
										g_FlowLine[temp_lineid].done = true;
										g_FlowLine[temp_lineid].g_PointListing[g_FlowLine[temp_lineid].current+1] = g_FlowLine[temp_lineid].g_idEndPos;
										g_FlowLine[temp_lineid].g_PointListing[(G_GameRule.g_Height + G_GameRule.g_Width)*2-1] = -1;
									}
								}
								g_FlowLine[temp_lineid].g_PointListing[g_FlowLine[temp_lineid].current] = cell_id;
								g_FlowLine[temp_lineid].current++;
							}
							temp_value = keyvalue;
							temp_id = g_GameField[y][x].g_idPos;
						}
					}
				}
			}
			breakout = _FALSE;
			for(int i=0;i<G_GameRule.g_StartPosCount;i++)
			{
				if(g_FlowLine[i].done == _TRUE)
				{
					lines_done++;
				}
			}
			if((lines_done == G_GameRule.g_StartPosCount) && (UsedCellsInPercentage(g_GameField,G_GameRule,G_GameMatrix) == COMPLETE))
			{
				system("cls");
				DisplayGameMatrix(G_GameRule,g_GameField,g_IdList,G_GameMatrix,g_FlowLine,lines_done);
				EndGame();
			}
		}
	}
}
void EndGame()
{
	printf("\nWinner!\n");
	system("pause");
	exit(1);
}
void BuildGameMatrix(FlowFileArray *G_GameMatrix,MapFileHeader G_GameRule)
{
	cell **g_GameField = AllocateMemory(G_GameRule);
	DWORD *g_IdList = BuildListId(G_GameRule,g_GameField);
	FlowLine *g_FlowLine = FlowLinesArrays(G_GameRule,g_GameField,G_GameMatrix);
	//======================================================
	/*
	H = G_GameRule.g_Height;
	W = G_GameRule.g_Width;
	px = (int *)malloc(sizeof(int)*H*W);
	py = (int *)malloc(sizeof(int)*H*W);
	grid = (int **)malloc(sizeof(int *)*G_GameRule.g_Height);
	for(int i=0;i<G_GameRule.g_Height;i++)
	{
		grid[i]=(int *)malloc(sizeof(int)*G_GameRule.g_Width);
	}
	for(int g = 0; g<G_GameRule.g_Height;g++)
	{
		for(int h = 0; h<G_GameRule.g_Width;h++)
		{
			grid[g][h] = BLANK;
		}
	}*/
	//======================================================
	for(int i=0;i<G_GameRule.g_StartPosCount;i++)
	{
		short g_TempStart_x,g_TempStart_y,g_TempEnd_x,g_TempEnd_y;
		g_TempStart_x = G_GameMatrix[i].g_StartPos_x;
		g_TempStart_y = G_GameMatrix[i].g_StartPos_y;
		g_TempEnd_x = G_GameMatrix[i].g_EndPos_x;
		g_TempEnd_y = G_GameMatrix[i].g_EndPos_y;
		g_GameField[g_TempStart_y][g_TempStart_x].g_PosKeyValue = G_GameMatrix[i].g_PosKeyValue;
		g_GameField[g_TempEnd_y][g_TempEnd_x].g_PosKeyValue = G_GameMatrix[i].g_PosKeyValue;
		g_GameField[g_TempStart_y][g_TempStart_x].walkable = 0;
		g_GameField[g_TempEnd_y][g_TempEnd_x].walkable = 0;
		//=========================================
		//grid[g_TempStart_y][g_TempStart_x] = WALL;
		//grid[g_TempEnd_y][g_TempEnd_x] = WALL;
		//=========================================
	}
	for(int i=0;i<G_GameRule.g_Height;i++)
	{
		for(int j=0;j<G_GameRule.g_Width;j++)
		{
			g_GameField[i][j].g_idPos = i*G_GameRule.g_Height + j;
		}
	}
	system("cls");
	pathVariant *variantArray = (pathVariant *)malloc(sizeof(pathVariant)*G_GameRule.g_StartPosCount);
	for(int i=0;i<G_GameRule.g_StartPosCount;i++)
	{
		variantArray[i].entry(G_GameRule,G_GameMatrix,i);
		variantArray[i].Output();
	}
	//AutoSolve(G_GameRule,G_GameMatrix);
	//UpdateGameField(G_GameMatrix,G_GameRule,g_GameField,g_IdList,g_FlowLine);
}
short *NCells(cell **g_GameField,FlowFileArray *G_GameMatrix,MapFileHeader G_GameRule,short x, short y) // (¬ычисл€ет окрестность фон неймана вокруг заданной точки с размерностью 1 )возвращает массив с id_pos соседей указанной точки (short) by default 
{
	short *g_NCellsArray;
	int i=0; 
	g_NCellsArray = (short *)malloc(sizeof(short)*SIDE_MAX_COUNT);
	for(int j=0;j<SIDE_MAX_COUNT;j++)
	{
		g_NCellsArray[j] = -1;
	}
	if ((y+1)<G_GameRule.g_Height)
	{
		g_NCellsArray[i++]=g_GameField[y+1][x].g_idPos;
	}
	if ((y-1)>=0)
	{
		g_NCellsArray[i++]=g_GameField[y-1][x].g_idPos;
	}
	if ((x+1)<G_GameRule.g_Width)
	{
		g_NCellsArray[i++]=g_GameField[y][x+1].g_idPos;
	}
	if ((x-1)>=0)
	{
		g_NCellsArray[i++]=g_GameField[y][x-1].g_idPos;
	}
	return g_NCellsArray;
}
coordinate *VonNeumannNeighborhood(int *m_EntryArray,int m_Size_x,int m_Size_y,int x, int y) // ¬ычисление окрестности фон неймана m_EntryArray - входной массив / m_Size_x/y - размеры массива   x/y - координаты точки 
{
	int index=0;
	coordinate *m_output = (coordinate *)malloc(sizeof(coordinate)*SIDE_MAX_COUNT);
	for(int i=0;i<SIDE_MAX_COUNT;i++)
	{
		m_output[i].g_Pos_x = -1;
		m_output[i].g_Pos_y = -1;
	}
	if ((y+1)<m_Size_y)
	{
		m_output[index].g_Pos_x = x;
		m_output[index++].g_Pos_y = y+1;
	}
	if ((y-1)>=0)
	{
		m_output[index].g_Pos_x = x;
		m_output[index++].g_Pos_y = y-1;
	}
	if ((x+1)<m_Size_x)
	{
		m_output[index].g_Pos_x = x+1;
		m_output[index++].g_Pos_y = y;
	}
	if ((x-1)>=0)
	{
		m_output[index].g_Pos_x = x-1;
		m_output[index++].g_Pos_y = y;
	}
	return m_output;
}
uInt UsedCells(cell **g_GameField,MapFileHeader G_GameRule,FlowFileArray *G_GameMatrix) // возвращает число зан€тых клеток
{
	unsigned int g_Cells = 0;
	unsigned int g_ReservedCells = G_GameRule.g_StartPosCount*2;
	cell **g_CheckField;
	g_CheckField = (cell **)malloc(sizeof(cell *)*G_GameRule.g_Height);
	for(int i=0;i<G_GameRule.g_Height;i++)
	{
		g_CheckField[i]=(cell *)malloc(sizeof(cell)*G_GameRule.g_Width);
	}
	g_CheckField = g_GameField;
	for(int i=0;i<G_GameRule.g_Height;i++)
	{
		for(int j=0;j<G_GameRule.g_Width;j++)
		{
			if(g_CheckField[i][j].g_PosKeyValue!=0)
			{
				g_Cells++;
			}
		}
	}
	return g_Cells - g_ReservedCells;
}
coordinate ConvertIdToCoordinate(short g_Pos_id,MapFileHeader G_GameRule)
{
	coordinate Value;
	Value.g_Pos_x = g_Pos_id%G_GameRule.g_Height;
	Value.g_Pos_y = (short)g_Pos_id/G_GameRule.g_Height;
	return Value;
}
short ConvertCoordinateToId(short x, short y,MapFileHeader G_GameRule)
{
	short g_PosId = (y * G_GameRule.g_Height) + x;
	return g_PosId;
}
uInt UsedCellsInPercentage(cell **g_GameField,MapFileHeader G_GameRule,FlowFileArray *G_GameMatrix) // возвращает в % число зан€тых клеток
{
	float g_UsedCells = (float)UsedCells(g_GameField,G_GameRule,G_GameMatrix);
	float g_ReservedCells = G_GameRule.g_StartPosCount*2;
	float g_TotalCells = G_GameRule.g_Height * G_GameRule.g_Width;
	float  total = ((g_UsedCells)/(g_TotalCells-g_ReservedCells)) * 100;
	if((total - (int)total)>=0.5)
	{
		total = (int)total + 1;
	}
	return total;
}
FlowLine *FlowLinesArrays(MapFileHeader G_GameRule,cell **g_GameField,FlowFileArray *mpfArray)
{
	FlowLine *g_FlowLine;
	g_FlowLine = (FlowLine *)malloc(sizeof(FlowLine)*G_GameRule.g_StartPosCount);
	for(int i=0;i<G_GameRule.g_StartPosCount;i++)
	{
		g_FlowLine[i].g_PointListing = (short *)malloc(sizeof(short)*((G_GameRule.g_Height + G_GameRule.g_Width)*2));
		for(int j=0;j<(G_GameRule.g_Height + G_GameRule.g_Width)*2;j++)
		{
			g_FlowLine[i].g_PointListing[j] = -1;
		}
		g_FlowLine[i].g_idStartPos = ConvertCoordinateToId(mpfArray[i].g_StartPos_x,mpfArray[i].g_StartPos_y,G_GameRule);
		g_FlowLine[i].g_idEndPos = ConvertCoordinateToId(mpfArray[i].g_EndPos_x,mpfArray[i].g_EndPos_y,G_GameRule);
		g_FlowLine[i].g_PointListing[0] = g_FlowLine[i].g_idStartPos;
		g_FlowLine[i].g_PointListing[(G_GameRule.g_Height + G_GameRule.g_Width)*2-1] = g_FlowLine[i].g_idEndPos;
		g_FlowLine[i].g_PosKeyValue = mpfArray[i].g_PosKeyValue;
		g_FlowLine[i].current = 1;
		g_FlowLine[i].done = false;
	}
	return g_FlowLine;
}
int main()
{
	/*
	int e[10][10];
	coordinate *fuck = VonNeumannNeighborhood(*e,10,10,5,5);
	for(int i=0;i<SIDE_MAX_COUNT;i++)
	{
		printf("x:%d  y:%d\n",fuck[i].g_Pos_x,fuck[i].g_Pos_y);
	}
	/*
	string str;
	getline(cin, str);
	int a[3];
	int quantity = 0;
	stringstream ss(str);
	if (!(ss >> a[0])) { printf("0\n"); system("pause"); }
	++quantity;
	if (!(ss >> a[1])) { printf("1\n"); system("pause"); }
	if (!(ss >> a[2])) { printf("3\n"); system("pause"); }
	*/
	GameMenu();
	system("pause");
}
void GameMenu()
{
	int entry;
	printf("1. Create game level.\n");
	printf("2. Start the game.\n");
	printf("3. Exit.\n");
	scanf("%d",&entry);
	switch(entry)
	{
	case 1:
		WriteFileHeader();
	case 2:
		ReadFileHeader();
	case 3:
		exit(1);
	}
	system("pause");
}

/*
void AutoSolve(MapFileHeader G_GameRule, FlowFileArray *G_GameMatrix,int x1, int y1, int x2, int y2)
{
	system("cls");
	H = G_GameRule.g_Height+2;
	W = G_GameRule.g_Width+2;
	px = (int *)malloc(sizeof(int)*H*W);
	py = (int *)malloc(sizeof(int)*H*W);
	grid = (int **)malloc(sizeof(int *)*H);
	for(int i=0;i<H;i++)
	{
		grid[i]=(int *)malloc(sizeof(int)*W);
	}
	//int x1,y1,x2,y2;
	scanf("%d%d%d%d", &x1,&y1,&x2,&y2);
	x1=x1+1;
	x2=x2+1;
	y1=y1+1;
	y2=y2+1;
	for(int i=0;i<H*W;i++)
	{
		px[i]=-1;
		py[i]=-1;
	}
	for(int g = 0; g<H;g++)
	{
		for(int h = 0; h<W;h++)
		{
			if((g == 0 || h == 0) || (g == H-1 || h == W-1))
			{
				grid[g][h] = WALL;
			}
			else
			{
				grid[g][h] = BLANK;
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
		grid[g_TempStart_y][g_TempStart_x] = WALL;
		grid[g_TempEnd_y][g_TempEnd_x] = WALL;
	}
	for(int i=0;i<H;i++)
	{
		printf("\n");
		for(int j=0;j<W;j++)
		{
				printf("%d\t",grid[i][j]);
		}
	}
	printf("\n");
	grid[y1][x1] = BLANK;
	grid[y2][x2] = BLANK;
	for(int i=0;i<H;i++)
	{
		printf("\n");
		for(int j=0;j<W;j++)
		{
				printf("%d\t",grid[i][j]);
		}
	}
	printf("\n");
	//grid[w][q] = BLANK;
	//grid[r][e] = BLANK;
	AlgorithmLee(x1,y1,x2,y2);
	for(int i=0;i<H;i++)
	{
		printf("\n");
		for(int j=0;j<W;j++)
		{
				printf("%d\t",grid[i][j]);
		}
	}
	for(int T=0;T<W*H;T++)
	{
		printf("x:%d  y:%d\n",px[T],py[T]);
	}
	system("pause");
}*/
int ManhattanDistance(int x1, int x2, int y1, int y2)
{
	return abs(x1-x2)+abs(y1-y2);
}
/*
bool AlgorithmLee(int ax, int ay, int bx, int by)   // поиск пути из €чейки (ax, ay) в €чейку (bx, by)
{
	int dx[4] = {1, 0, -1, 0};   // смещени€, соответствующие сосед€м €чейки
	int dy[4] = {0, 1, 0, -1};   // справа, снизу, слева и сверху
	int d, x, y, k;
	bool stop;
	// распространение волны
	d = 0;
	grid[ay][ax] = 0;            // стартова€ €чейка помечена 0
	do 
	{
		stop = true;               // предполагаем, что все свободные клетки уже помечены
		for( y = 0; y < H; ++y )
		{
			for( x = 0; x < W; ++x )
			{
				if ( grid[y][x] == d )                         // €чейка (x, y) помечена числом d
				{
					for ( k = 0; k < 4; ++k )                    // проходим по всем непомеченным сосед€м
					{
						if ( grid[y + dy[k]][x + dx[k]] == BLANK )
						{
							stop = false;                            // найдены непомеченные клетки
							grid[y + dy[k]][x + dx[k]] = d + 1;      // распростран€ем волну
						}
					}
				}
			}
		}
		d++;
	} 
	while ( !stop && grid[by][bx] == BLANK );
	if (grid[by][bx] == BLANK) return false;  // путь не найден
	// восстановление пути
	len = grid[by][bx];            // длина кратчайшего пути из (ax, ay) в (bx, by)
	x = bx;
	y = by;
	d = len;
	while ( d > 0 )
	{
		px[d] = x;
		py[d] = y;                   // записываем €чейку (x, y) в путь
		d--;
		for (k = 0; k < 4; ++k)
		{
			if (grid[y + dy[k]][x + dx[k]] == d)
			{
				x = x + dx[k];
				y = y + dy[k];           // переходим в €чейку, котора€ на 1 ближе к старту
				break;
			}
		}
	}
	px[0] = ax;
	py[0] = ay;                    // теперь px[0..len] и py[0..len] - координаты €чеек пути
	return true;
}
*/