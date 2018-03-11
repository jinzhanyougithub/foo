/******************************************************************************************
Copyright           : 2000-2012, Huawei Tech. Co., Ltd.
FileName            : ����˹�����սC/C++�ͻ��˳�������㷨����
Description         : 
Project             : TetrixClient
Author              : 
Date Of Creation    : 
Version             : 
Note                : ��������Ѵ�ã���������Ҫ�Լ�����play.cpp, 
Modification History: 
1. Date         : 
Author       : 
Modification : 
******************************************************************************************/
#include <time.h>
#include "tetris.h"

/*play()��������������״��λ�õĺ����㷨������������Ҫ�����޸�

���룺struct TINPUT���ýṹ���а���������ǰ����ֲ����Է�����ֲ�����ǰ�����Լ���һ�����飬
      ��ǰ����ĳ�ʼ��̬����Ҫ�ṩ����Ϊ������ѡ��������ת�����庬����ο�struct TINPUT����
	  ����ʱע��

�����std::vector<TACTION>, �ýṹ�а�����ǰ�������ת��ʽ(rotate)������һ������(col���Ե�ǰ������״��
      ������ڵ���Ϊ׼)���Լ�������ٸ�(down)�����庬����ο�struct TINPUT��������ʱע��

��ע�� (1) TACTION�ṹ���rotate, col ��down��ֵ����Խ�磬rotate��colԽ����ȡ���ֵ��down
           Խ����ȡ0.

       (2) ���ڷ�����������ƣ����糤����״�ķ�����ŷţ�ֻ�ܷ���0-6�У�������7,8,9�з��£���
	       ��Ҫplay�����Լ�����֤������ȷ���������ֵԽ�磬������������������������������߿���

	   (3) ������IP�˿��Լ��û�������Ҫ��config.ini�ļ������ã�config.ini�ļ��������ڵ�ǰ����
	       Ŀ¼�»���exe�ļ���ͬĿ¼��
*/
void play(const TINPUT &input, std::vector<TACTION>& actions)
{
	TACTION sAct;

	srand((uint)time(NULL));
	//�����д��ת
	sAct.rotate = rand() % 3;
	srand((uint)time(NULL));
	//�����д����
	sAct.col = rand() % 9;
	//ֱ���������ײ�
	sAct.down = 0;
	//cout << "Rotate: " <<  sAct.rotate << "   MAX_COL: " << sAct.col << "    Down: " << sAct.down << endl;

	actions.push_back(sAct);
}