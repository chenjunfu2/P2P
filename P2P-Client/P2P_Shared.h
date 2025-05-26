#pragma once

#include "Client_Server_Shared.h"

/**********************宏定义**********************/
//标记宏
#define P2P//节点<>节点

//验证对方信息
#define PEERCONNECTPEERSYN		P2P 128
#define PEERCONNECTPEERACK		P2P 129
/**********************宏定义**********************/


/**********************结构体**********************/
struct PeerConnectPeer P2P//链接对方时的验证
{
	MYDEF::SIZE szMsgSize;//信息结构体大小
	MYDEF::MESSAGE mMsgHead;//信息头
	MYDEF::CHAR cUserName[MAXUSERNAME];//自己的用户名称
};
/**********************结构体**********************/






