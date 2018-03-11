//#include "stdafx.h"
//#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include <iostream>
#include <signal.h>
#include <sstream>

#include "TCPClient.hpp"

TCPClient tcp;

typedef unsigned int uint;

const uint LINE=20;
const uint WIDTH=10;

struct TACTION
{
    uint rotate;  // 0 - 3
    uint col;     // 0 - 9
    uint down;    // 0 - 20
};

struct TINPUT
{
    char matrix_self[LINE][WIDTH];
    char matrix_rival[LINE][WIDTH];
    uint curBlock;
    uint nextBlock;
};

int HIGHLIMIT=6;
int SCORE;

typedef struct Status{
    int xyOffset[4][2]; // block body based on base point x,y
    int curDrc; //direction
    int side; // the leftest point x
    Status *next;
}Status;

typedef struct Factor{
    Status *status; //status of Block
    
    int faces;
    int fullLine;
    int emptyCells;
    int hight;
    
    int realX; //coordinate of Block
    int realY; //coordinate of Block
}Factor;

int Board[LINE][WIDTH];// Game board 17line*10width
int TempBoard[LINE][WIDTH];//Used to get board Status
Status * Block[8]; // 7 blocks,block list

//advanced array
int const AdvLength=20;// length of the array
int AdvBlocks[AdvLength]; //advanced blocks
int CurIndex; //current index of in the array

int col;
int drc;

void PrintTempBoard()
{
    //int minLine=GetTempBoard();
    
    for(int i=0;i<LINE;i++,printf("\n"))
        for(int j=0;j<WIDTH;j++)
            printf("%c",TempBoard[i][j]?'x':'-');
    printf("\n");
}

void PrintBoard()
{
    for(int y=0;y<LINE;y++,printf("\n"))
        for(int x=0;x<WIDTH;x++)
            printf("%c",Board[LINE-y-1][x]?'X':'-');
}

void PrintBlock(Status block)
{
    int xy[6][6]={0};
    for(int i=0;i<4;i++)
    {
        printf("(%d,%d)\t",block.xyOffset[i][0],block.xyOffset[i][1]);
        
        int x=block.xyOffset[i][0]+2;
        int y=block.xyOffset[i][1]+2;
        xy[x][y]=1;
    }
    
    printf("\n");
    
    for(int i=5;i>=0;i--,printf("\n"))
        for(int j=0;j<6;j++)
        {
            printf("%c",xy[j][i]?'X':'-');
        }
    printf("\n\n");
}

/*
 init the AdvBlocks array
 */

int containBlock(int block)
{
    for(int i=0;i<AdvLength;i++)
        if(AdvBlocks[i]==block)    return 1;
    return 0;
}

void initAdvBlocks()
{
    srand(time(NULL));
    for(int i=0;i<AdvLength;i++)
        AdvBlocks[i]=rand()%7+1;
}

/*
 get a new block
 */
int getNextBlock()
{
    
    int temp=AdvBlocks[CurIndex];
    
    //AdvBlocks[CurIndex]= containBlock(2)?rand()%7+1:2;
    AdvBlocks[CurIndex]= rand()%7+1;
    
    CurIndex=(CurIndex+1)%AdvLength;
    
    //return rand()%2?5:7;
    
    return temp;
}

/*
 preview the next block
 */
int viewNextBlock()
{
    return AdvBlocks[(CurIndex+1)%AdvLength];
}

/*
 determine whether a cell is inside the board (Board / TempBoard)
 */

int insideBoard(int y,int x)
{
    return  (y>=0 && y<LINE) && (x>=0 && x<WIDTH);
}


/*
 Get the TempBoard which is used to specify the empty space where the block can be put.
 */
void GetTempBoard()
{
    memset(TempBoard,1,sizeof(TempBoard));
    
    for(int x=0;x<WIDTH;x++)
    {
        for(int y=LINE-1;y>=0;y--)
        {
            if(Board[y][x])    break;
            TempBoard[y][x]=0;
            
        }
    }
    
    //PrintTempBoard();
}

/*
 Determine whether the block is fit into the TempBoard.
 */
int fitIn(Status * block , int x, int y)
{
    for(int i=0;i<4;i++)
    {
        int realX=x+block->xyOffset[i][0];
        int realY=y+block->xyOffset[i][1];
        
        if(!insideBoard(realY,realX)||TempBoard[realY][realX])    return 0;
    }
    
    return 1;
}

/*
 put a block in the board ( Board / TempBoard )
 */

void putBlock(Status *block , int x , int y ,int BOARD[][10])
{
    if(!block)    return;
    
    for(int i=0;i<4;i++)
    {
        int realX=x+block->xyOffset[i][0];
        int realY=y+block->xyOffset[i][1];
        
        BOARD[realY][realX]=1;
    }
}

/*
 remove a block from a board (Board / TempBoard)
 */
void removeBlock(Status *block ,int x , int y,int BOARD[][10])
{
    if(!block) return;
    
    for(int i=0;i<4;i++)
    {
        int realX=x+block->xyOffset[i][0];
        int realY=y+block->xyOffset[i][1];
        
        BOARD[realY][realX]=0;
    }
}

/*
 Determine it is a full line
 */
int fullLine(int line)
{
    for(int i=0;i<WIDTH;i++)
        if(Board[line][i]==0)    return 0;
    return 1;
}

/*
 delete a line .
 line:0 - LINE
 */
void deleteLineAt(int line)
{
    for(int y=line;y<LINE;y++)
        for(int x=0;x<WIDTH;x++)
            Board[y][x]=Board[y+1][x];
}

/*
 Delete all full lines.
 */

int removeFullLines()
{
    int count=0;
    for(int line=LINE-1;line>=0;line--)
    {
        if(fullLine(line))
        {
            count++;
            deleteLineAt(line);
        }
    }
    return count;
}


/*
 determine whether a cell is belong to a block
 */

int belongToBlock(Status * stus, int offsetX, int offsetY)
{
    for(int i=0;i<4;i++)
    {
        int x=stus->xyOffset[i][0];
        int y=stus->xyOffset[i][1];
        
        if(offsetX==x && offsetY==y)    return 1;
    }
    
    return 0;
}

/*
 get the faces in touch with a block
 */

int getFaces(Status *stus,int x, int y)
{
    int count=0;
    
    for(int i=0;i<4;i++)
    {
        int realX=x+stus->xyOffset[i][0];
        int realY=y+stus->xyOffset[i][1];
        
        int add =  (i!=0)?1:1;
        
        if( !insideBoard(realY,realX-1) || Board[realY][realX-1] )    count+=add;
        if( !insideBoard(realY,realX+1) || Board[realY][realX+1] )    count+=add;
        if( !insideBoard(realY-1,realX) || Board[realY-1][realX] )    count+=add;
        if( !insideBoard(realY+1,realX) || Board[realY+1][realX] )    count+=add;
    }
    
    //printf("faces=%d\n",count);
    return count;
}

/*
 get the empty cells blow a block when this block is put some where
 */
int getEmptyCells(Status * block,int x, int y)
{
    putBlock(block,x,y,TempBoard);
    
    int count=0;
    for(int i=0;i<4;i++)
    {
        int realX=x+block->xyOffset[i][0];
        int realY=y+block->xyOffset[i][1];
        
        while(insideBoard(realY-1,realX) && TempBoard[realY-1][realX]==0)
        {
            count++;
            realY--;
        }
    }
    
    //printf("EmptyCells=%d\n",count);
    
    removeBlock(block,x,y,TempBoard);
    
    return count;
}

/*
 get the full lines when a block is put some where
 */

int getFullLines()
{
    int count=0;
    
    for(int i=0;i<LINE;i++)
        if(fullLine(i))    count++;
    
    return count;
}

/*
 get the hight of a block when it is put somewhere
 */

int getHight(Status * stus, int x ,int y)
{
    return stus->xyOffset[3][1]+y;
}

/*
 get the highest solid cell position  from Board
 */

int getHighestLineNum()
{
    for(int line=LINE-1;line>=0;line--)
        for(int i=0;i<WIDTH;i++)
            if(Board[line][i])    return line;
    return 0;
}

/*
 get the lowest empty cell position  from TempBoard
 */

int getLowestLine()
{
    for(int y=0;y<LINE;y++)
        for(int x=0;x<WIDTH;x++)
            if(!TempBoard[y][x])    return y;
    
    return LINE;
}

/*
 determine whether any solid cells holp up this block in Board
 */
int canStay(Status *stus, int x, int y)
{
    for(int i=0;i<4;i++)
    {
        int offsetX=stus->xyOffset[i][0];
        int offsetY=stus->xyOffset[i][1]-1;
        if(!insideBoard(offsetY+y, offsetX+x) || (!belongToBlock(stus,offsetX,offsetY)&&Board[y+offsetY][x+offsetX]) )    return 1;
    }
    
    return 0;
}


/*
 When highest line is lower than 10 , we use this factor.
 */

Factor cmp1(Factor min, Factor temp)
{
    
    HIGHLIMIT = ( getHighestLineNum()>= HIGHLIMIT ? 3: HIGHLIMIT);
    
    int factMin,factTemp;
    
    float score[]={0,-0.5,2,3,4};
    
    factMin=4*score[min.fullLine]+ 2*min.faces-6*min.emptyCells-1*min.hight;//best 4 2 -6 -1
    factTemp=4*score[temp.fullLine]+ 2*temp.faces-6*temp.emptyCells-1*temp.hight;
    
    return factMin>factTemp?min:temp;
}

/*
 When highest line is higher than 10 , we use this factor.
 */
Factor cmp2(Factor min, Factor temp)
{
    HIGHLIMIT = ( getHighestLineNum()<= HIGHLIMIT ? 7 : HIGHLIMIT );
    
    int factMin,factTemp;
    
    float score[]={0,0,2,4,8};
    
    factMin=100*score[min.fullLine]+ 2*min.faces-4*min.emptyCells-1*min.hight;//best 100 2 -4 -1
    factTemp=100*score[temp.fullLine]+ 2*temp.faces-4*temp.emptyCells-1*temp.hight;
    
    return factMin>factTemp?min:temp;
    
}

/*
 Determine which Status of the block can be put into the space(TempBoard & Board).
 */
Factor getBestStatus(int block,int x, int y)
{
    Factor temp , minFact={NULL,0,-1,LINE*WIDTH,LINE,0,0};
    Status *cur=Block[block];
    while(cur)
    {
        if(canStay(cur,x,y) && fitIn(cur,x,y))
        {
            putBlock(cur,x,y,Board);
            temp.status=cur;
            temp.faces=getFaces(cur,x,y);
            temp.fullLine=getFullLines();
            temp.emptyCells=getEmptyCells(cur,x,y);
            temp.hight=getHight(cur,x,y);
            temp.realX=x;
            temp.realY=y;
            removeBlock(cur,x,y,Board);
            
            //when the hight of blocks is higher than hightLimit , change compare function
            int highestLine=getHighestLineNum();
            minFact= highestLine<HIGHLIMIT?cmp1(minFact,temp):cmp2(minFact,temp);
        }
        cur=cur->next;
    }
    
    return minFact;
    
}

/*
 Test whether the postion is a proper place to put block.
 Put it when proper.
 */
Factor TryAndPutBlock(int block)
{
    
    Factor minFact={NULL,0,-1,LINE*WIDTH,LINE,0,0};
    Factor temp;
    
    int score[]={0,0,2,4,8};
    int lines;
    
    for(int y=0;y<LINE;y++)
        for(int x=0;x<WIDTH;x++)
        {
            if(TempBoard[y][x]==0)
            {
                temp=getBestStatus(block,x,y);
                
                int highestLine=getHighestLineNum();
                minFact= highestLine< HIGHLIMIT?cmp1(minFact,temp):cmp2(minFact,temp);
            }
        }
    
    putBlock(minFact.status,minFact.realX,minFact.realY,Board);
    
    lines=removeFullLines();
    SCORE=SCORE+score[lines];
    
    //PrintBoard();
    
    return minFact;
    
}

void Begin(uint block)
{
    //int block;
    //int count=0;
    
    //while(scanf("%d",&block)!=EOF)
    
    //initAdvBlocks();
    
    //while(1)
    //{
        //block=getNextBlock();
        GetTempBoard();
        //PrintTempBoard();
        Factor pos=TryAndPutBlock(block);
        if(pos.status==NULL){
            //printf("count=%d\n",count);
            return;
        }
        drc = pos.status->curDrc;
        col = pos.status->side+pos.realX+1;
        printf("%d\t%d\t%d\n",block,pos.status->curDrc,pos.status->side+pos.realX+1);
        printf("SCORE=%d\n",SCORE);
        //system("pause");
        //count++;
    //}
}

void TravelBlockList()
{
    Status * cur;
    for(int i=0;i<8;i++)
    {
        cur=Block[i];
        while(cur)
        {
            PrintBlock(*cur);
            cur=cur->next;
        }
        
    }
}

void EndGame()
{
    Status *cur ,*temp;
    
    for(int i=0;i<8;i++)
    {
        cur=Block[i];
        while(cur)
        {
            temp=cur->next;
            free(cur);
            cur=temp;
        }
    }
}

void InitBlock(Status *block, int offset[][2] , int drc,int side)
{
    for(int i=0;i<4;i++)
    {
        block->xyOffset[i][0]=offset[i][0];
        block->xyOffset[i][1]=offset[i][1];
    }
    
    block->curDrc=drc;
    block->side=side;
    
    //PrintBlock(*block);
}

void InitGame()
{
    Status *B1_S1=(Status*)malloc(sizeof(Status));
    Status *B2_S1=(Status*)malloc(sizeof(Status)),*B2_S2=(Status*)malloc(sizeof(Status));
    Status *B3_S1=(Status*)malloc(sizeof(Status)),*B3_S2=(Status*)malloc(sizeof(Status)),*B3_S3=(Status*)malloc(sizeof(Status)),*B3_S4=(Status*)malloc(sizeof(Status));
    Status *B4_S1=(Status*)malloc(sizeof(Status)),*B4_S2=(Status*)malloc(sizeof(Status)),*B4_S3=(Status*)malloc(sizeof(Status)),*B4_S4=(Status*)malloc(sizeof(Status));
    Status *B5_S1=(Status*)malloc(sizeof(Status)),*B5_S2=(Status*)malloc(sizeof(Status)),*B5_S3=(Status*)malloc(sizeof(Status)),*B5_S4=(Status*)malloc(sizeof(Status));
    Status *B6_S1=(Status*)malloc(sizeof(Status)),*B6_S2=(Status*)malloc(sizeof(Status));
    Status *B7_S1=(Status*)malloc(sizeof(Status)),*B7_S2=(Status*)malloc(sizeof(Status));
    Block[1]=B1_S1;    B1_S1->next=NULL;
    Block[2]=B2_S1; B2_S1->next=B2_S2; B2_S2->next=NULL;
    Block[3]=B3_S1; B3_S1->next=B3_S2; B3_S2->next=B3_S3; B3_S3->next=B3_S4; B3_S4->next=NULL;
    Block[4]=B4_S1; B4_S1->next=B4_S2; B4_S2->next=B4_S3; B4_S3->next=B4_S4; B4_S4->next=NULL;
    Block[5]=B5_S1; B5_S1->next=B5_S2; B5_S2->next=B5_S3; B5_S3->next=B5_S4; B5_S4->next=NULL;
    Block[6]=B6_S1; B6_S1->next=B6_S2; B6_S2->next=NULL;
    Block[7]=B7_S1; B7_S1->next=B7_S2; B7_S2->next=NULL;
    //xx
    //xx
    int offsetB1S1[4][2]={{0,0},{1,0},{0,1},{1,1}};        InitBlock(B1_S1,offsetB1S1,0,0);
    //x
    //x
    //x
    //x
    int offsetB2S1[4][2]={{0,0},{0,1},{0,2},{0,3}};        InitBlock(B2_S1,offsetB2S1,1,0);
    //xxxx
    int offsetB2S2[4][2]={{0,0},{1,0},{2,0},{3,0}};        InitBlock(B2_S2,offsetB2S2,0,0);
    //xxx
    // x
    int offsetB3S1[4][2]={{0,0},{-1,1},{0,1},{1,1}};    InitBlock(B3_S1,offsetB3S1,2,-1);
    // x
    //xx
    // x
    int offsetB3S2[4][2]={{0,0},{-1,1},{0,1},{0,2}};    InitBlock(B3_S2,offsetB3S2,3,-1);
    // x
    //xxx
    int offsetB3S3[4][2]={{0,0},{1,0},{2,0},{1,1}};        InitBlock(B3_S3,offsetB3S3,0,0);
    //x
    //xx
    //x
    int offsetB3S4[4][2]={{0,0},{0,1},{1,1},{0,2}};        InitBlock(B3_S4,offsetB3S4,1,0);
    //x
    //x
    //xx
    int offsetB4S1[4][2]={{0,0},{1,0},{0,1},{0,2}};        InitBlock(B4_S1,offsetB4S1,3,0);
    //xxx
    //x
    int offsetB4S2[4][2]={{0,0},{0,1},{1,1},{2,1}};        InitBlock(B4_S2,offsetB4S2,0,0);
    //xx
    // x
    // x
    int offsetB4S3[4][2]={{0,0},{0,1},{0,2},{-1,2}};       InitBlock(B4_S3,offsetB4S3,1,-1);
    //  x
    //xxx
    int  offsetB4S4[4][2]={{0,0},{1,0},{2,0},{2,1}};        InitBlock(B4_S4,offsetB4S4,2,0);
    // x
    // x
    //xx
    int offsetB5S1[4][2]={{0,0},{1,0},{1,1},{1,2}};        InitBlock(B5_S1,offsetB5S1,1,0);
    //x
    //xxx
    int offsetB5S2[4][2]={{0,0},{1,0},{2,0},{0,1}};        InitBlock(B5_S2,offsetB5S2,2,0);
    //xx
    //x
    //x
    int offsetB5S3[4][2]={{0,0},{0,1},{0,2},{1,2}};        InitBlock(B5_S3,offsetB5S3,3,0);
    //xxx
    //  x
    int offsetB5S4[4][2]={{0,0},{0,1},{-1,1},{-2,1}};    InitBlock(B5_S4,offsetB5S4,0,-1);
    //xx
    // xx
    int offsetB6S1[4][2]={{0,0},{1,0},{0,1},{-1,1}};    InitBlock(B6_S1,offsetB6S1,0,-1);
    // x
    //xx
    //x
    int offsetB6S2[4][2]={{0,0},{0,1},{1,1},{1,2}};        InitBlock(B6_S2,offsetB6S2,1,0);
    // xx
    //xx
    int offsetB7S1[4][2]={{0,0},{1,0},{1,1},{2,1}};        InitBlock(B7_S1,offsetB7S1,0,0);
    //x
    //xx
    // x
    int offsetB7S2[4][2]={{0,0},{0,1},{-1,1},{-1,2}};    InitBlock(B7_S2,offsetB7S2,1,-1);
}

void sig_exit(int s)
{
    tcp.exit();
    exit(0);
}

void decode(const char* pBuf, uint nLen, TINPUT& output)
{
    if (nLen != 404 || NULL == pBuf)
    {
        return;
    }
    
    uint k = 0;
    
    memset(output.matrix_self, 0 ,sizeof(output.matrix_self));
    for (uint i = 0; i < LINE; ++i)
    {
        for (uint j = 0; j < WIDTH; ++j)
        {
            if (k < (LINE*WIDTH))
            {
                output.matrix_self[i][j] = pBuf[k++] - 0x30;
            }
            else
            {
                cout << "k is out of range, exit!" << endl;
                return;
            }
        }
    }
    
    if (k != (LINE*WIDTH))
    {
        cout << " k != 200, exit! k = " << k <<endl;
        return;
    }
    
    memset(output.matrix_rival, 0 ,sizeof(output.matrix_rival));
    for (uint l = 0; l < LINE; ++l)
    {
        for (uint m = 0; m < WIDTH; ++m)
        {
            if (k < (LINE * WIDTH * 2))
            {
                output.matrix_rival[l][m] = pBuf[k++] - 0x30;
            }
            else
            {
                cout << "k is out of range, exit!" << endl;
                return;
            }
        }
    }
    
    if (k != (LINE*WIDTH*2))
    {
        cout << " k != 400, exit! k = " << k <<endl;
        return;
    }
    
    output.curBlock = pBuf[k++] - 0x30;
    output.nextBlock = pBuf[k] - 0x30;
    cout << endl << "Block is: " << output.curBlock << "  " << output.nextBlock << endl;
    cout <<"Decode data succeed! " << endl;
}

bool encode(std::vector<TACTION>& vecAction, string& strData)
{
    if (vecAction.empty() )
    {
        cout << "encode parameter is error, exit!" << endl;
        return false;
    }
    
    uint nTotalDown = 0;
    string strTmp;
    stringstream ss;
    
    std::vector<TACTION>::iterator it = vecAction.begin();
    
    for (;it != vecAction.end(); ++it)
    {
        if (it->rotate >= 4)
        {
            cout <<"Warning: roate value: " << it->rotate << " is out of range, use 3 instead." << endl;
            it->rotate = (4 - 1);
        }
        
        if (it->col >= WIDTH)
        {
            cout <<"Warning: MAX_COL value: " << it->col << " is out of range, use 9 instead." << endl;
            it->col = (WIDTH - 1);
        }
        
        if (it->down > LINE)
        {
            cout <<"Warning: down value: " << it->down << " is out of range, use 0 instead." << endl;
            it->down = 0;
        }
        
        nTotalDown += it->down;
        
        if (nTotalDown > LINE)
        {
            cout << "Warning: total down value: " << nTotalDown << " is out of range, current down uses 0 instead." << endl;
            it->down = 0;
        }
        
        string strRotate ;
        string strCol ;
        string strDown;
        ss.clear();
        ss << it->rotate;
        ss >> strRotate;
        
        ss.clear();
        ss << it->col;
        ss >> strCol;
        
        ss.clear();
        ss << it->down;
        ss >> strDown;
        if (it->down < 10)
        {
            strDown.insert(0, "0");
        }
        
        strTmp += strRotate;
        strTmp += strCol;
        strTmp += strDown;
    }
    
    ss.clear();
    ss << vecAction.size();
    ss >> strData;
    
    strData += strTmp;
    cout << "encode data succeed! data is: " << strData << endl;
    return true;
}

int main()
{
    InitGame();
    
    signal(SIGINT, sig_exit);
    tcp.setup( "127.0.0.1", 12345 );
    while(1)
    {
        tcp.Send("Jinzhanyou\r\n");
        string rec = tcp.receive();
        if ( rec != "" )
        {
            TINPUT input;
            const char *buffer;
            buffer = rec.c_str();
            decode(buffer, 404, input);
            uint block = input.curBlock;
            
            for ( int x = LINE - 1; x >= 0; x-- )
            {
                for ( int y = WIDTH - 1; y >= 0; y-- )
                {
                    Board[x][y] = input.matrix_self[x][y];
                }
            }
            //Board = input.matrix_self;
//            for ( int i = 0; i < LINE; i++ )
//            {
//                for ( int j = 0; j < WIDTH; j++ )
//                {
//                    Board[i][j] = input.matrix_self[i][j];
//                }
//            }
            Status *cur=Block[block];
            if ( cur != nullptr )
            {
                cout << "cur block" << endl;
                PrintBlock(*cur);
            } 
            Begin(block);

            sleep(3);
            TACTION act;
            act.rotate = drc;
            act.col = col;
            act.down = 0;

            string strSendData;
            std::vector<TACTION> actions;
            actions.push_back(act);
            encode(actions, strSendData);
            tcp.Send(strSendData.c_str());
        }
    }
    EndGame();
    return 0;
}
