/******************************************************************************************
Copyright           : 2000-2012, Huawei Tech. Co., Ltd.
FileName            : ����˹�����սC/C++�ͻ��˳����ܳ���
Description         : 
Project             : TetrixClient
Author              : geshijun 165756
Date Of Creation    : 2012-03-05
Version             : 1.0
Note                : ��������Ѵ�ã���������Ҫ�Լ�����play.cpp, ������IP�˿��Լ��û�������Ҫ��config.ini�ļ�
                      �����ã�config.ini�ļ��������ڵ�ǰ����Ŀ¼��
Modification History: 
1. Date         : 
Author       : 
Modification : 
******************************************************************************************/

#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>  
#include <iostream>
#include "tetris.h"
using namespace std;

#pragma comment(lib, "WS2_32.lib")

//�����������������ݳ���
const uint DATA_LEN = 404;
const uint MAX_BUFFER_SIZE = 800;

SOCKET g_sock;


//��ȡ�����ļ�

void readCfgFile(string& strName, string& strServerIP, uint& nPort)
{
	char szName[MAX_BUFFER_SIZE] = {0};
	char szServerIP[MAX_BUFFER_SIZE] = {0};
	uint nTmp = 12345;

	GetPrivateProfileString("config", "UserName", "Player XXX", szName, MAX_BUFFER_SIZE, ".\\config.ini");
	GetPrivateProfileString("config", "ServerIP", "127.0.0.1", szServerIP, MAX_BUFFER_SIZE, ".\\config.ini");
	nTmp = GetPrivateProfileInt("config", "ServerPort", 12345, ".\\config.ini");
	
	strName = szName;
	strServerIP = szServerIP;
	nPort = nTmp;
	cout << "UserName: " << strName << "    " << "ServerIP: " << strServerIP << "    " << "ServerPort: " << nPort << endl;
};

//ͨ��socket�������ݣ���������������ݽ�β��"\r\n"��
bool sendData(const char* szMsg)
{
	char szData[MAX_BUFFER_SIZE] = {0};
	//�ӽ�����"\r\n"
	sprintf(szData, "%s\r\n", szMsg);

	uint nResult = send(g_sock, szData, strlen(szData), 0);
	if(SOCKET_ERROR == nResult)
	{
		cout << "Send data error, exit! error id = " << WSAGetLastError() << endl;
		return false;
	}
	else
	{
		cout<<"Send data succeed, data length is: " << strlen(szData) << endl;
		return true;
	}
}

//���ӷ�����
bool connectServer(const string& strServerIP, uint nPort)
{
	//����SOCKET�⣬�汾Ϊ2.0
	WORD wVersionRequested;
	WSADATA wsaData;
	int err = 0;	
	wVersionRequested = MAKEWORD(2, 0);	
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) 
	{
		cout << "Socket2.0 init failed��exit!" << endl;
		return false;
	}	
	if (LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0)
	{
		WSACleanup();
		return false; 
	}

	//�����׽���
	g_sock = socket(AF_INET,SOCK_STREAM,0);
	if (g_sock == INVALID_SOCKET)
	{
		cout << " Create socket failed, exit!" << endl;
		return false;
	}

	//�����ַ
	sockaddr_in myaddr; 
	memset(&myaddr,0,sizeof(myaddr));
	myaddr.sin_family=AF_INET;
	myaddr.sin_addr.s_addr = inet_addr(strServerIP.c_str());  
	myaddr.sin_port=htons(nPort);

	//connect
	if (SOCKET_ERROR == connect(g_sock, (sockaddr*)&myaddr, sizeof(myaddr)))
	{
		cout << "Connect server failed, exit! " << endl;
		closesocket(g_sock);
		WSACleanup();
	    return false;
	}
	else
	{
		cout<<"Connect server succeed!" << endl;
		return true;
	}
}

//�ر�socket
void closeSocket()
{
	if (!closesocket(g_sock)) 
	{
		WSAGetLastError();
		return;
	}
	cout << "Close socket succeed! " << endl;

	if (!WSACleanup())
	{
		WSAGetLastError();
		return;
	}
}

//�������������������룬�������g_mine[][], g_rival[][], g_nextBlock��g_curBlock��
void decode(const char* pBuf, uint nLen, TINPUT& output)
{
	if (nLen != DATA_LEN || NULL == pBuf)
	{
		return;
	}

	uint k = 0;

	memset(output.matrix_self, 0 ,sizeof(output.matrix_self));
	for (uint i = 0; i < MAX_ROW; ++i)
	{
		for (uint j = 0; j < MAX_COL; ++j)
		{
			if (k < (MAX_ROW*MAX_COL))
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

	if (k != (MAX_ROW*MAX_COL))
	{
		cout << " k != 200, exit! k = " << k <<endl;
		return;
	}

	memset(output.matrix_rival, 0 ,sizeof(output.matrix_rival));
	for (uint l = 0; l < MAX_ROW; ++l)
	{
		for (uint m = 0; m < MAX_COL; ++m)
		{
			if (k < (MAX_ROW * MAX_COL * 2))
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

	if (k != (MAX_ROW*MAX_COL*2))
	{
		cout << " k != 400, exit! k = " << k <<endl;
		return;
	}

	if (pBuf[k] > BLOCK_S+0x30 || pBuf[k] < BLOCK_O+0x30)
	{
		cout << "Current block is out of range, exit! pBuf[k] = " << pBuf[k] << endl;
		return;
	}

	output.curBlock = pBuf[k++] - 0x30;

	if (pBuf[k] > BLOCK_S+0x30 || pBuf[k] < BLOCK_O+0x30)
	{
		cout << "Next block is out of range, exit! pBuf[k] = " << pBuf[k] << endl;
		return;
	}

	output.nextBlock = pBuf[k] - 0x30;
	cout << endl << "Block is: " << output.curBlock << "  " << output.nextBlock << endl;
	cout <<"Decode data succeed! " << endl;
}

bool recvData(TINPUT& input)
{
	char szRecvData[MAX_BUFFER_SIZE] = {0};
	//��TCP���ӽ�������
	int nLen = recv(g_sock, szRecvData, sizeof(szRecvData), 0);
	if (nLen > 0)
	{
		//����Լ�������������͵����ݳ��ȱ�����404�ֽ�
		if (strlen(szRecvData) != DATA_LEN)
		{
			cout << "Recv data length is not 404, exit!  lenth is: " << strlen(szRecvData) << endl;
			return false;
		}

		cout << "Recv 404 bytes data succeed: " << szRecvData << endl;
		
		//���������������������
		decode(szRecvData, strlen(szRecvData), input);
		return true;
	}
	else
	{
		cout << "Recv data failed, exit!" << endl;
		return false;	
	}
}

//��vector��������ֵ����У�飬У��ɹ���vecAction�е����ݱ�����������Э�̵����ݸ�ʽ
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
		//Խ����ȡ���ֵ
		if (it->rotate >= MAX_ROTATE)
		{
			cout <<"Warning: roate value: " << it->rotate << " is out of range, use 3 instead." << endl;
			it->rotate = (MAX_ROTATE - 1);
		}

		if (it->col >= MAX_COL)
		{
			cout <<"Warning: MAX_COL value: " << it->col << " is out of range, use 9 instead." << endl;
			it->col = (MAX_COL - 1);
		}

		if (it->down > MAX_ROW)
		{
			cout <<"Warning: down value: " << it->down << " is out of range, use 0 instead." << endl;
			it->down = 0;
		}

		nTotalDown += it->down;

		if (nTotalDown > MAX_ROW)
		{
			cout << "Warning: total down value: " << nTotalDown << " is out of range, current down uses 0 instead." << endl;
			it->down = 0;
		}

		string strRotate ;
		string strCol ;
		string strDown;	

		//����ת����string
		ss.clear();
		ss << it->rotate;
		ss >> strRotate;

		ss.clear();
		ss << it->col;
		ss >> strCol;

		ss.clear();
		ss << it->down;
		ss >> strDown;

		//10���ڵ�����ǰ���0
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
	string strUserName;
	string strServerIP;
	uint nPort = 8888;

	//�������ļ���ȡ�û�����������IP�Ͷ˿�
	/*if (!readCfgFile(strUserName, strServerIP, nPort))
	{
		system("pause");
		return -1;
	}*/
	readCfgFile(strUserName, strServerIP, nPort);

    //���ӷ�����
	if (!connectServer(strServerIP, nPort))
	{
		system("pause");
		return -1;
	}

	//���ӽ���֮�����û�����������
	sendData(strUserName.c_str());
	
	while (true)
	{
		TINPUT sInput;

		//��TCP�������ݲ�����
		if (!recvData(sInput))
		{
			break;
		}

		uint nDataLen = 0;
		
		//��ŷ��ظ�������������
		string strSendData;
		std::vector<TACTION> vecAction;

		play(sInput, vecAction);
		if (encode(vecAction, strSendData))
		{
			sendData(strSendData.c_str());
		}
		else
		{
			break;
		}
	}

	closeSocket();
	system("pause");
	return 0;
}