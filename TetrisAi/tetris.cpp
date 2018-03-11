/******************************************************************************************
Copyright           : 2000-2012, Huawei Tech. Co., Ltd.
FileName            : 俄罗斯方块对战C/C++客户端程序核心算法代码
Description         : 
Project             : TetrixClient
Author              : 
Date Of Creation    : 
Version             : 
Note                : 基础框架已搭好，参赛者需要自己完善play.cpp, 
Modification History: 
1. Date         : 
Author       : 
Modification : 
******************************************************************************************/
#include <time.h>
#include "tetris.h"

/*play()决定方块下落形状和位置的核心算法函数，根据需要自行修改

输入：struct TINPUT，该结构体中包含本方当前方块分布、对方方块分布、当前方块以及下一个方块，
      当前方块的初始形态不需要提供，因为可以由选手随意旋转。具体含义请参考struct TINPUT变量
	  定义时注释

输出：std::vector<TACTION>, 该结构中包含当前方块的旋转方式(rotate)、在哪一列下落(col，以当前方块形状最
      左边所在的列为准)，以及下落多少格(down)，具体含义请参考struct TINPUT变量定义时注释

备注： (1) TACTION结构里的rotate, col 和down数值不可越界，rotate和col越界则取最大值，down
           越界则取0.

       (2) 对于方块的列数限制（例如长条形状的方块横着放，只能放在0-6列，不能再7,8,9列放下），
	       需要play方法自己来保证列数正确，如果列数值越界，服务器会检查出来，并尽量将方块贴边靠。

	   (3) 服务器IP端口以及用户姓名需要在config.ini文件中配置，config.ini文件必须置于当前工程
	       目录下或者exe文件相同目录下
*/
void play(const TINPUT &input, std::vector<TACTION>& actions)
{
	TACTION sAct;

	srand((uint)time(NULL));
	//随机填写旋转
	sAct.rotate = rand() % 3;
	srand((uint)time(NULL));
	//随机填写列数
	sAct.col = rand() % 9;
	//直接下落至底部
	sAct.down = 0;
	//cout << "Rotate: " <<  sAct.rotate << "   MAX_COL: " << sAct.col << "    Down: " << sAct.down << endl;

	actions.push_back(sAct);
}