#pragma once
#include "Client_Server_MsgDefine.h"

/**********************辅助结构体**********************/
//ip地址
union IpAddr
{
	MYDEF::IPADDR addr;//1个地址
	struct { MYDEF::IPFIELD b1, b2, b3, b4; };//4个字段
};

//ip地址+端口号
struct IpPort
{
	IpAddr Ip;//ip
	MYDEF::PORT Port;//端口号
};
/**********************辅助结构体**********************/


/**********************通信结构体**********************/
//登入服务器用
struct LoginInfo C2S
{
	MYDEF::SIZE szMsgSize;//信息结构体大小
	MYDEF::MESSAGE mMsgHead;//信息头
	MYDEF::CHAR cUserName[MAXUSERNAME];//用户名,以'\0'结尾
	MYDEF::COUNT dwLanCount;//私有终端个数
	IpPort stUserLan[MAXLANCOUNT];//私有终端
};
static_assert(sizeof(LoginInfo) <= MAXPACKETSIZE);//断言,请求信息结构大小必须小于最大包体大小

//登出服务器用
struct LogoutInfo C2S
{
	MYDEF::SIZE szMsgSize;//信息结构体大小
	MYDEF::MESSAGE mMsgHead;//信息头
};
static_assert(sizeof(LogoutInfo) <= MAXPACKETSIZE);


//获取用户数
struct GetListSize C2S
{
	MYDEF::SIZE szMsgSize;//信息结构体大小
	MYDEF::MESSAGE mMsgHead;//信息头
};
static_assert(sizeof(GetListSize) <= MAXPACKETSIZE);

//获取用户列表
struct GetListUser C2S
{
	MYDEF::SIZE szMsgSize;//信息结构体大小
	MYDEF::MESSAGE mMsgHead;//信息头
};
static_assert(sizeof(GetListUser) <= MAXPACKETSIZE);

//请求链接这个用户
struct ConnectUser C2S
{
	MYDEF::SIZE szMsgSize;//信息结构体大小
	MYDEF::MESSAGE mMsgHead;//信息头
	MYDEF::CHAR cUserName[MAXUSERNAME];//用户名,以'\0'结尾
	MYDEF::CHAR cUserConnectMsg[MAXUSERCNTMSG];//请求链接时附带的消息
};
static_assert(sizeof(ConnectUser) <= MAXPACKETSIZE);

//同意该用户的请求
struct UserPermitConnect C2S
{
	MYDEF::SIZE szMsgSize;//信息结构体大小
	MYDEF::MESSAGE mMsgHead;//信息头
	MYDEF::CHAR cUserName[MAXUSERNAME];//用户名,以'\0'结尾
};
static_assert(sizeof(UserPermitConnect) <= MAXPACKETSIZE);

//拒绝该用户的请求
struct UserRejectConnect C2S
{
	MYDEF::SIZE szMsgSize;//信息结构体大小
	MYDEF::MESSAGE mMsgHead;//信息头
	MYDEF::CHAR cUserName[MAXUSERNAME];//用户名,以'\0'结尾
};
static_assert(sizeof(UserRejectConnect) <= MAXPACKETSIZE);



//登录成功
struct LoginSuc S2C
{
	MYDEF::SIZE szMsgSize;//信息结构体大小
	MYDEF::MESSAGE mMsgHead;//信息头
	MYDEF::TIMESTAMP tLoginTime;//登陆时间戳
	IpPort stUserNet;//客户端自己的公网ip、端口
};
static_assert(sizeof(LoginSuc) <= MAXPACKETSIZE);

//登录失败
struct LoginErr S2C
{
	MYDEF::SIZE szMsgSize;//信息结构体大小
	MYDEF::MESSAGE mMsgHead;//信息头
	MYDEF::MESSAGE mErrCode;//错误码
};
static_assert(sizeof(LoginErr) <= MAXPACKETSIZE);


//服务器返回当前已登录用户个数
struct ListSizeInfo S2C
{
	MYDEF::SIZE szMsgSize;//信息结构体大小
	MYDEF::MESSAGE mMsgHead;//信息头
	MYDEF::COUNT dwUserCount;//用户数
};
static_assert(sizeof(ListSizeInfo) <= MAXPACKETSIZE);

//服务器发送来的用户信息
struct ListUserInfo S2C
{
	MYDEF::SIZE szMsgSize;//信息结构体大小
	MYDEF::MESSAGE mMsgHead;//信息头
	MYDEF::CHAR cUserName[MAXUSERNAME];//用户名,以'\0'结尾
	MYDEF::TIMESTAMP tLoginTime;//该用户登录服务器的时间
};
static_assert(sizeof(ListUserInfo) <= MAXPACKETSIZE);

//这个用户请求链接你
struct UserConnect S2C
{
	MYDEF::SIZE szMsgSize;//信息结构体大小
	MYDEF::MESSAGE mMsgHead;//信息头
	MYDEF::CHAR cUserName[MAXUSERNAME];//用户名,以'\0'结尾
	MYDEF::CHAR cUserConnectMsg[MAXUSERCNTMSG];//请求链接时附带的消息
};
static_assert(sizeof(UserConnect) <= MAXPACKETSIZE);

//你同意请求或对方同意请求时服务器发来的对方信息(这个时候开始初始化对外P2P链接)
struct UserPermitInfo S2C
{
	MYDEF::SIZE szMsgSize;//信息结构体大小
	MYDEF::MESSAGE mMsgHead;//信息头
	MYDEF::CHAR cUserName[MAXUSERNAME];//用户名,以'\0'结尾
	IpPort stUserNet;//用户的公共终端
	MYDEF::COUNT dwLanCount;//私有终端个数
	IpPort stUserLan[MAXLANCOUNT];//私有终端
};
static_assert(sizeof(UserPermitInfo) <= MAXPACKETSIZE);

//用户拒绝了你的链接请求
struct UserRejectInfo S2C
{
	MYDEF::SIZE szMsgSize;//信息结构体大小
	MYDEF::MESSAGE mMsgHead;//信息头
	MYDEF::MESSAGE mErrCode;//错误码(可以区分用户拒绝或用户超时未响应或用户不存在(已下线等)
};
static_assert(sizeof(UserRejectInfo) <= MAXPACKETSIZE);
/**********************通信结构体**********************/