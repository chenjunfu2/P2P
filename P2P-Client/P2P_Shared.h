#pragma once

#include "Client_Server_Shared.h"

/**********************�궨��**********************/
//��Ǻ�
#define P2P//�ڵ�<>�ڵ�

//��֤�Է���Ϣ
#define PEERCONNECTPEERSYN		P2P 128
#define PEERCONNECTPEERACK		P2P 129
/**********************�궨��**********************/


/**********************�ṹ��**********************/
struct PeerConnectPeer P2P//���ӶԷ�ʱ����֤
{
	MYDEF::SIZE szMsgSize;//��Ϣ�ṹ���С
	MYDEF::MESSAGE mMsgHead;//��Ϣͷ
	MYDEF::CHAR cUserName[MAXUSERNAME];//�Լ����û�����
};
/**********************�ṹ��**********************/






