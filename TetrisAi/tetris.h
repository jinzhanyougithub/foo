/******************************************************************************************
Copyright           : 2000-2012, Huawei Tech. Co., Ltd.
FileName            : ����˹�����սC/C++�ͻ��˳���ͷ�ļ�
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

//--------------------ש�鶨��----------------------------
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
    uint col;     // 0 - 9 �������ң��Է��������ߵ�����Ϊ׼��
    uint down;    // 0 - 20 (0��ʾֱ����ף�1 - 20��ʾ�½�����)
};

struct TINPUT
{
	/*��ŷ��������صı�����ǰ����ķֲ���0��ʾ�գ�1��ʾ�з��飩
	���¶��Ϸֲ���matrix_self��ʾ�����·��ķ���matrix_self[19][9]
	��ʾ�����Ϸ��ķ���*/
    char matrix_self[MAX_ROW][MAX_COL];

	/*��ŶԷ���ǰ����ķֲ������Բ�ʹ�ã�Ҳ���������ʵ�ʱ������
	����ս��,�ֲ���ʽͬmatrix_self[][]*/
    char matrix_rival[MAX_ROW][MAX_COL];

	//��ǰ�ķ�����״�������ͶԷ��ķ�����״��ͬ��
    uint curBlock;

	//��һ��������״
    uint nextBlock;
};

extern void play(const TINPUT& input, std::vector<TACTION>& actions);
