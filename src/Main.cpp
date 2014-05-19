#include "Flow.h"
short iGlobalDone;



void WriteFileHeader() // √енераци€ игрового пол€ (запись в файл)
{
	TCHAR buffer[BUFFSIZE];
	TCHAR stdPath[BUFFSIZE] = TEXT("MapFile.map");
	MapFileHeader mpfHeader;
	FlowFileArray *mpfArray;
	mpfHeader.f_offset = sizeof(MapFileHeader);
	DWORD CurrentDirectoryBuffer;
	DWORD Header_offset;
	CurrentDirectoryBuffer = GetCurrentDirectory(BUFFSIZE,buffer);
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
			g_GameField[i][j].walkable = true;
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
#ifdef _DEBUG
	for(int i=0;i<G_GameRule.g_StartPosCount;i++)
	{
		printf("\nListing %d :",i);
		for(int j=0;j<(G_GameRule.g_Height + G_GameRule.g_Width)*2;j++)
		{
			printf("%d ",g_FlowLine[i].g_PointListing[j]);
		}
		printf("END: %d",g_FlowLine[i].done);
	}
#endif
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
#ifndef _DEBUG
		system("cls");
#endif
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
				if(g_FlowLine[temp_lineid].done == true)
				{
					g_FlowLine[temp_lineid].done = false;
				}
			}
			temp_lineid = FlowLineId(keyvalue,G_GameRule,G_GameMatrix);
			if((g_FlowLine[temp_lineid].g_PosKeyValue == keyvalue)&&(g_FlowLine[temp_lineid].done!= true))
			{
				coordinate temp;
				temp = ConvertIdToCoordinate(g_FlowLine[temp_lineid].g_PointListing[g_FlowLine[temp_lineid].current-1],G_GameRule);
				n = NCells(g_GameField,G_GameMatrix,G_GameRule,temp.g_Pos_x,temp.g_Pos_y);
				for(int h=0;h<sizeof(g_FlowLine[temp_lineid].g_PointListing);h++)
				{
					if(g_FlowLine[temp_lineid].g_PointListing[h]==ConvertCoordinateToId(x,y,G_GameRule))
					{
						breakout = true;
					}
				}
				if(breakout == false)
				{
					for(int i=0;i<SIDE_MAX_COUNT;i++)
					{
						if(n[i]==cell_id)
						{
							if(flag == FREE_BLOCK)
							{
								keyvalue = false;
							}
							if(g_GameField[y][x].walkable == false)
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
										g_FlowLine[temp_lineid].g_PointListing[(G_GameRule.g_Height + G_GameRule.g_Width)*2-1] = EMPTY_CELL;
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
			breakout = false;
			for(int i=0;i<G_GameRule.g_StartPosCount;i++)
			{
				if(g_FlowLine[i].done == true)
				{
					lines_done++;
				}
			}
			iGlobalDone = lines_done;
			if((lines_done == G_GameRule.g_StartPosCount) && (UsedCellsInPercentage(g_GameField,G_GameRule,G_GameMatrix) == COMPLETE))
			{
				system("cls");
				DisplayGameMatrix(G_GameRule,g_GameField,g_IdList,G_GameMatrix,g_FlowLine,lines_done);
				EndGame();
			}
		}
	}
}
void UpdateGameField(FlowFileArray *G_GameMatrix,MapFileHeader G_GameRule,cell **g_GameField,DWORD *g_IdList,FlowLine *g_FlowLine, int sendto_x, int sendto_y, int sendto_keyvalue) // overloaded version of UpdateGameField
{
	static short x,y,keyvalue,flag,temp_id,temp_value,temp_lineid,cell_id,lines_done=0,temp_listing_id;
	static short *n;
	static bool breakout;
#ifndef _DEBUG
		system("cls");
#endif
		flag = 0;
		//DisplayGameMatrix(G_GameRule,g_GameField,g_IdList,G_GameMatrix,g_FlowLine,lines_done);
		lines_done = 0;
		//scanf("%d %d %d",&x,&y,&keyvalue);
		x = sendto_x;
		y = sendto_y;
		keyvalue  = sendto_keyvalue;
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
				if(g_FlowLine[temp_lineid].done == true)
				{
					g_FlowLine[temp_lineid].done = false;
				}
			}
			temp_lineid = FlowLineId(keyvalue,G_GameRule,G_GameMatrix);
			if((g_FlowLine[temp_lineid].g_PosKeyValue == keyvalue)&&(g_FlowLine[temp_lineid].done!= true))
			{
				coordinate temp;
				temp = ConvertIdToCoordinate(g_FlowLine[temp_lineid].g_PointListing[g_FlowLine[temp_lineid].current-1],G_GameRule);
				n = NCells(g_GameField,G_GameMatrix,G_GameRule,temp.g_Pos_x,temp.g_Pos_y);
				for(int h=0;h<sizeof(g_FlowLine[temp_lineid].g_PointListing);h++)
				{
					if(g_FlowLine[temp_lineid].g_PointListing[h]==ConvertCoordinateToId(x,y,G_GameRule))
					{
						breakout = true;
					}
				}
				if(breakout == false)
				{
					for(int i=0;i<SIDE_MAX_COUNT;i++)
					{
						if(n[i]==cell_id)
						{
							if(flag == FREE_BLOCK)
							{
								keyvalue = false;
							}
							if(g_GameField[y][x].walkable == false)
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
										g_FlowLine[temp_lineid].g_PointListing[(G_GameRule.g_Height + G_GameRule.g_Width)*2-1] = EMPTY_CELL;
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
			breakout = false;
			for(int i=0;i<G_GameRule.g_StartPosCount;i++)
			{
				if(g_FlowLine[i].done == true)
				{
					lines_done++;
				}
			}
			DisplayGameMatrix(G_GameRule,g_GameField,g_IdList,G_GameMatrix,g_FlowLine,lines_done);
			iGlobalDone = lines_done;
			if((lines_done == G_GameRule.g_StartPosCount) && (UsedCellsInPercentage(g_GameField,G_GameRule,G_GameMatrix) == COMPLETE))
			{
				system("cls");
				DisplayGameMatrix(G_GameRule,g_GameField,g_IdList,G_GameMatrix,g_FlowLine,lines_done);
				EndGame();
			}
		}
}
void EndGame()
{
	printf("\nWinner!\n");
	system("pause");
	exit(0);
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
	int lines_done = 0;
	iGlobalDone = lines_done;
	DisplayGameMatrix(G_GameRule,g_GameField,g_IdList,G_GameMatrix,g_FlowLine,lines_done);
	while(true)
	{
		std::string buffer;
		std::stringstream ss;
		int numvals[] = { 0, 0, 0 };
		std::cin >> buffer;
		//std::cout << "Buffer contains: \"" << buffer << '\"' << std::endl;
		ss.clear();
		ss.str(buffer);
		if (!(ss >> numvals[0]))
			throw std::runtime_error("Wrong argument");
		std::cin >> buffer;
		//std::cout << "Buffer contains: \"" << buffer << '\"' << std::endl;
		ss.clear();
		ss.str(buffer);
		if (ss >> numvals[1]) 
		{
			std::cin >> buffer;
			//std::cout << "Buffer contains: \"" << buffer << '\"' << std::endl;
			ss.clear();
			ss.str(buffer);
			if (!(ss >> numvals[2]))
			{
				throw std::runtime_error("Wrong argument");
			}
			UpdateGameField(G_GameMatrix,G_GameRule,g_GameField,g_IdList,g_FlowLine,numvals[0],numvals[1],numvals[2]);
		}
		else
		{
			if (buffer != "auto")
				throw std::runtime_error("Keyword \"" + buffer + "\" is unknown");
			pathVariant autoSolve(G_GameRule,G_GameMatrix,numvals[0]-1);
			for(int p = 0; p < autoSolve.m_pathLength; p++)
			{
				//cout<< autoSolve.path[p].g_Pos_x-1 << autoSolve.path[p].g_Pos_y-1 << "\n";
				UpdateGameField(G_GameMatrix,G_GameRule,g_GameField,g_IdList,g_FlowLine,autoSolve.path[p].g_Pos_x-1,autoSolve.path[p].g_Pos_y-1,numvals[0]);
			}
		}
		system("cls");
		DisplayGameMatrix(G_GameRule,g_GameField,g_IdList,0,0,iGlobalDone);

		
        //doNumberAndString(numericvals[0]);

        
		//UpdateGameField(G_GameMatrix,G_GameRule,g_GameField,g_IdList,g_FlowLine,x,y,keyvalue);
	}
	 
	/*
	vector<pathVariant> variantArray;
	//pathVariant *variantArray = (pathVariant *)malloc(sizeof(pathVariant)*G_GameRule.g_StartPosCount);
	for(int i=0;i<G_GameRule.g_StartPosCount;i++)
	{
		variantArray.push_back(pathVariant(G_GameRule,G_GameMatrix,i));
		//variantArray[i].initialize(G_GameRule,G_GameMatrix,i);
#ifdef _DEBUG
		variantArray[i].Output();
#endif
	}
	// NEW ADDED START
	fieldComparsion **compField = (fieldComparsion**)malloc(sizeof(fieldComparsion*)*G_GameRule.g_Height);
	for(int i=0;i<G_GameRule.g_Height;i++)
	{
		compField[i] = (fieldComparsion*)malloc(sizeof(fieldComparsion)*G_GameRule.g_Width);
	}
	for(int i=0;i<G_GameRule.g_Height;i++)
	{
		for(int j=0;j<G_GameRule.g_Width;j++)
		{
			compField[i][j].initialize(G_GameRule);
		}
	}
	for(int i=0;i<G_GameRule.g_StartPosCount;i++)
	{
		//compField[G_GameMatrix[i].g_EndPos_y][G_GameMatrix[i].g_EndPos_x].m_editable = false;
		//compField[G_GameMatrix[i].g_StartPos_y][G_GameMatrix[i].g_StartPos_x].m_editable = false;
		int x,y;
		for(int j=0;j<variantArray[i].m_pathLength;j++)
		{
			if(variantArray[i].path[j].g_Pos_x != EMPTY_CELL)
			{
				x = variantArray[i].path[j].g_Pos_x-1;
				y = variantArray[i].path[j].g_Pos_y-1;
				if(compField[y][x].m_editable == true)
				{
					compField[y][x].m_idArray[compField[y][x].m_counter] = i;
					compField[y][x].m_counter++;	
					//m_inCounter++;
				}
			}
		}
	}
	printf("\n");
	int s_overloadedCells;
	int s_freeCells=0;
	struct coord
	{
		short g_Pos_x;
		short g_Pos_y;
		coord(short x, short y) : g_Pos_x(x), g_Pos_y(y) {}
	};
	vector<coord> overloadedCoordinates;
	for(int i=0;i<G_GameRule.g_Height;i++)
	{
		for(int j=0;j<G_GameRule.g_Width;j++)
		{
			compField[i][j].Output();
			if(compField[i][j].m_counter > 1)
			{
				overloadedCoordinates.push_back(coord(j,i));
			}
			if(compField[i][j].m_counter == 0)
			{
				s_freeCells++;
			}
		}
		s_overloadedCells = overloadedCoordinates.size();
		printf("\n");
	}
	if(s_overloadedCells > 0)
	{
		for(int i=0;i<s_overloadedCells;i++) // все линии которые пересеклись - помечаем inCollision -> TRUE
		{
			int x,y;
			x = overloadedCoordinates[i].g_Pos_x;
			y = overloadedCoordinates[i].g_Pos_y;
			for(int j=0;j<compField[y][x].m_counter;j++)
			{
				if(variantArray[compField[y][x].m_idArray[j]].inCollision == false)
				{
				
					variantArray[compField[y][x].m_idArray[j]].inCollision = true;
				}
			}
		}
		struct m_distID
		{
			int id;
			int m_distance;
			m_distID(int A, int B) : id(A), m_distance(B) {}
		};
		vector<m_distID> priorityExecution;
		for(int i=0;i<G_GameRule.g_StartPosCount;i++)
		{
			if(variantArray[i].inCollision == true)
			{
				priorityExecution.push_back(m_distID(i,variantArray[i].m_Distance));
			}
		}
		sort(priorityExecution.begin(), priorityExecution.end(), [](const m_distID &a, const m_distID &b) { return a.m_distance < b.m_distance; });


	#ifdef _DEBUG
		for(int i=0;i<priorityExecution.size();i++)
		{
			printf("\nDISTANCE: %d  ID: %d",priorityExecution[i].m_distance,priorityExecution[i].id);
		}
		for(int i=0;i<G_GameRule.g_StartPosCount;i++)
		{
			printf("Line : %d inCollision : %d\n",i,variantArray[i].inCollision);
		}

		printf("\noverloaded cells: %d   free cells : %d\n",s_overloadedCells,s_freeCells);
		for(int i=0;i<s_overloadedCells;i++)
		{
			printf("\nx: %d y: %d\n",overloadedCoordinates[i].g_Pos_x,overloadedCoordinates[i].g_Pos_y);
		}
	#endif
		int **BlockArray = (int **)malloc(sizeof(int *)*G_GameRule.g_Height);
		for(int i=0;i<G_GameRule.g_Height;i++)
		{
			BlockArray[i] = (int *)malloc(sizeof(int) *G_GameRule.g_Width);
		}
		nullArray(G_GameRule.g_Width,G_GameRule.g_Height,BlockArray);
		for(int i=0;i<G_GameRule.g_StartPosCount;i++)
		{
			if(variantArray[i].inCollision != true)
			{
				for(int h=0;h<variantArray[i].m_pathLength;h++)
				{
					int x,y;
					x = variantArray[i].path[h].g_Pos_x-1;
					y = variantArray[i].path[h].g_Pos_y-1;
					BlockArray[y][x] = WALL;
				}
			}
		}
		vector<pathVariant> varArray;
		for(int i=0;i<priorityExecution.size();i++)
		{
			varArray.push_back(pathVariant(G_GameRule,G_GameMatrix,priorityExecution[i].id,BlockArray));
			UpdateBlockArray(varArray[i].path,BlockArray,varArray[i].m_pathLength);
			varArray[i].Output();
		}
	#ifdef _DEBUG
		for(int i=0;i<G_GameRule.g_Height;i++)
		{
			for(int j=0;j<G_GameRule.g_Width;j++)
			{
				printf("%d\t",BlockArray[i][j]);
			}
			printf("\n");
		}
	#endif
		//pathVariant check(G_GameRule,G_GameMatrix,0,BlockArray);
		//check.Output();
		// END
		//AutoSolve(G_GameRule,G_GameMatrix);
	}
	//UpdateGameField(G_GameMatrix,G_GameRule,g_GameField,g_IdList,g_FlowLine);
*/
}
void UpdateBlockArray(coordinate *input, int **output, int i_size)
{
	for(int i=0;i<i_size;i++)
	{
		int x,y;
		x = input[i].g_Pos_x-1;
		y = input[i].g_Pos_y-1;
		output[y][x] = WALL;
	}
}



short *NCells(cell **g_GameField,FlowFileArray *G_GameMatrix,MapFileHeader G_GameRule,short x, short y) // (¬ычисл€ет окрестность фон неймана вокруг заданной точки с размерностью 1 )возвращает массив с id_pos соседей указанной точки (short) by default 
{
	short *g_NCellsArray;
	int i=0; 
	g_NCellsArray = (short *)malloc(sizeof(short)*SIDE_MAX_COUNT);
	for(int j=0;j<SIDE_MAX_COUNT;j++)
	{
		g_NCellsArray[j] = WALL;
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
void nullArray(int size_x,int size_y, int **input)
{
	for(int i=0;i<size_y;i++)
	{
		for(int j=0;j<size_x;j++)
		{
			input[i][j] = FREE_BLOCK;
		}
	}
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
	//WriteFileHeader();
	//ReadFileHeader();
	//GameMenu();
	GameMenu();
	//system("pause");
}
void GameMenu()
{
	char entry;
	printf("1. Create game level.\n");
	printf("2. Start the game.\n");
	printf("3. Exit.\n");
	entry = _getch();
	switch(entry)
	{
	case '1':
		WriteFileHeader();
	case '2':
		ReadFileHeader();
	case '3':
		exit(0);
	}
}
int ManhattanDistance(int x1, int x2, int y1, int y2)
{
	return abs(x1-x2)+abs(y1-y2);
}
