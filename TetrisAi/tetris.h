/******************************************************************************************
Copyright           : 2000-2012, Huawei Tech. Co., Ltd.
FileName            : 俄罗斯方块对战C/C++客户端程序头文件
Description         : 
Project             : TetrixClient
Author              : geshijun 165756
Date Of Creation    : 2012-03-05
Version             : 1.0
Modification History: 
1. Date         : 
Author       : 
Modification : 
******************************************************************************************/
#pragma once

#include <vector>
typedef unsigned int uint;

const uint MAX_COL    = 10; // 0 - 9 
const uint MAX_ROW    = 20; // 0 - 19
const uint MAX_ROTATE =  4; // 0 - 3
const uint MAX_BLOCK  =  7; // 0 - 6

//--------------------砖块定义----------------------------
#define BLOCK_O 0 // rotate=0 rotate=1 rotate=2 rotate=3    
//   |**  |   |**  |   |**  |   |**  |
//   |**  |   |**  |   |**  |   |**  |
//   |    |   |    |   |    |   |    |
//   |    |   |    |   |    |   |    |

#define BLOCK_I 1 // rotate=0 rotate=1 rotate=2 rotate=3    
//   |*   |   |****|   |*   |   |****|
//   |*   |   |    |   |*   |   |    |
//   |*   |   |    |   |*   |   |    |
//   |*   |   |    |   |*   |   |    |

#define BLOCK_T 2 // rotate=0 rotate=1 rotate=2 rotate=3    
//   |*** |   | *  |   | *  |   |*   |
//   | *  |   |**  |   |*** |   |**  |
//   |    |   | *  |   |    |   |*   |
//   |    |   |    |   |    |   |    |

#define BLOCK_L 3 // rotate=0 rotate=1 rotate=2 rotate=3    
//   |*   |   |*** |   |**  |   |  * |
//   |*   |   |*   |   | *  |   |*** |
//   |**  |   |    |   | *  |   |    |
//   |    |   |    |   |    |   |    |

#define BLOCK_J 4 // rotate=0 rotate=1 rotate=2 rotate=3    
//   | *  |   |*   |   |**  |   |*** |
//   | *  |   |*** |   |*   |   |  * |
//   |**  |   |    |   |*   |   |    |
//   |    |   |    |   |    |   |    |

#define BLOCK_Z 5 // rotate=0 rotate=1 rotate=2 rotate=3    
//   |**  |   | *  |   |**  |   | *  |
//   | ** |   |**  |   | ** |   |**  |
//   |    |   |*   |   |    |   |*   |
//   |    |   |    |   |    |   |    |

#define BLOCK_S 6 // rotate=0 rotate=1 rotate=2 rotate=3
//   | ** |   |*   |   | ** |   |*   |
//   |**  |   |**  |   |**  |   |**  |
//   |    |   | *  |   |    |   | *  |
//   |    |   |    |   |    |   |    |

struct TACTION
{
    uint rotate;  // 0 - 3
    uint col;     // 0 - 9 （从左到右，以方块的最左边的列数为准）
    uint down;    // 0 - 20 (0表示直接落底，1 - 20表示下降几格)
};

struct TINPUT
{
	/*存放服务器返回的本方当前方块的分布（0表示空，1表示有方块）
	自下而上分布，matrix_self表示最左下方的方格，matrix_self[19][9]
	表示最右上方的方格*/
    char matrix_self[MAX_ROW][MAX_COL];

	/*存放对方当前方块的分布，可以不使用，也可用来在适当时机调整
	本方战术,分布方式同matrix_self[][]*/
    char matrix_rival[MAX_ROW][MAX_COL];

	//当前的方块形状（本方和对方的方块形状相同）
    uint curBlock;

	//下一个方块形状
    uint nextBlock;
};

extern void play(const TINPUT& input, std::vector<TACTION>& actions);
