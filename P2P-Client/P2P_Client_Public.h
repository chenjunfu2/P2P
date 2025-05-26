#pragma once

#include"Client_Server_Shared.h"
#include "P2P_Shared.h"

//定义宏
#define CONST				const
#define VOID				void
#define NULL				0

class P2PClient//请不要继承该类,不要声明或传递该类的Const实例,更不要复制该类或从该类的一个实例创建另一个该类的实例
{
private://私有的默认处理函数
	static VOID DftHandleConnectMsgFunc(MYDEF::PCCHAR pcUserName, MYDEF::PCCHAR pcUserConnectMsg);//默认的链接消息处理函数(该函数应快速处理后立即返回,否则会阻塞接收线程)
	//static VOID DftHandleLinkResultFunc(MYDEF::SOCKET sPeerSocket);//默认的链接结果处理函数,直接关闭链接句柄,断开链接(该函数应快速处理后立即返回,否则会阻塞接收线程)

public://公开成员和定义
	typedef VOID(*HandleConnectMsgFunc)(MYDEF::PCCHAR pcUserName, MYDEF::PCCHAR pcUserConnectMsg);//函数指针宏定义
	//typedef VOID(*HandleLinkResultFunc)(MYDEF::SOCKET sPeerSocket);//函数指针宏定义

	struct UserInfo
	{
		MYDEF::CHAR cUserName[MAXUSERNAME];//用户名,以'\0'结尾
		MYDEF::TIMESTAMP tTime;//该用户登录服务器的时间
	};

	P2PClient(MYDEF::PORT Port = 0,
		HandleConnectMsgFunc pConnectMsgFunc = DftHandleConnectMsgFunc);
		//HandleLinkResultFunc pLinkResultFunc = DftHandleLinkResultFunc);//需要本地对外链接使用的端口号和接受对方p2p请求的处理回调函数
	~P2PClient(VOID);

	MYDEF::BOOL LoginServer(IpAddr uniServerIp, MYDEF::PORT wServerPort, MYDEF::PCHAR UserName);//登录服务器
	MYDEF::BOOL LoginServer(MYDEF::PCHAR pcIp, MYDEF::PCHAR pcPort, MYDEF::PCHAR pcUserName);

	VOID LogoutServer(VOID);//登出服务器,并清理所有资源
	MYDEF::BOOL IsLogin(VOID);//检查是否登录

	MYDEF::BOOL GetUserListFromServer(VOID);//从服务器得到用户列表

	MYDEF::COUNT GetUserCount(VOID);//获得用户列表大小
	CONST UserInfo* GetUserInfo(MYDEF::COUNT ctIndex);//用于访问单个用户信息
	CONST UserInfo* operator[](MYDEF::COUNT ctIndex);//重载运算符,用于访问单个用户信息

	MYDEF::SOCKET ConnectAPeer(CONST UserInfo& stUserInfo);//P2P链接一个用户
	MYDEF::BOOL DisConnectAPeer(MYDEF::SOCKET soPeerSocket);//与一个P2P用户断开链接

	MYDEF::SOCKET AgreePeerLink(MYDEF::PCCHAR pcUserName);//接受请求链接的某个用户(需传入用户名,防止同时多个请求导致混乱)
	VOID RejectPeerLink(MYDEF::PCCHAR pcUserName);//拒绝请求链接的某个用户(需传入用户名,防止同时多个请求导致混乱)

	MYDEF::BOOL SetHandleConnectMsgFunc(HandleConnectMsgFunc pConnectMsgFunc = DftHandleConnectMsgFunc);
										//HandleLinkResultFunc pLinkResultFunc = DftHandleLinkResultFunc);//重新设置函数(不带参数调用则设置为默认)

	VOID SetLastErrorCode(MYDEF::ERRCODE errErrorCode);//设置最后一个错误码
	MYDEF::ERRCODE GetLastErrorCode(VOID);//获得最后一个错误的错误码
	MYDEF::PCCHAR GetErrorReason(MYDEF::ERRCODE errErrorCode);//通过错误码获得错误原因

public://屏蔽的成员函数
	P2PClient(CONST P2PClient&) = delete;
	P2PClient(CONST P2PClient*) = delete;

	P2PClient& operator=(CONST P2PClient&) = delete;
	P2PClient& operator=(CONST P2PClient*) = delete;
	P2PClient& operator=(CONST P2PClient) = delete;
private:
	//私有成员函数（关联多个私有成员的函数）
	VOID ClrAndReleaseResource(MYDEF::BOOL bClrUserSet);

	
	MYDEF::BOOL BuildPeerLink(VOID);//主动建立链接
private:
	//私有数据和关联的成员函数
	MYDEF::SOCKET m_soClient;//套接字描述符

	MYDEF::BOOL InitSocket(VOID);
	VOID ReleaseSocket(VOID);//破坏链接关闭句柄并重置
	MYDEF::BOOL SendMsgStruct(MYDEF::PCVOID pSendStruct);

private:
	//StatusInfo
	MYDEF::BOOL m_bLoginServer;//登录状态
	MYDEF::ERRCODE m_errLastErrorCode;//错误码

	VOID SetLogin(VOID);//设置为登录状态
	VOID SetLogOut(VOID);//设置为下线状态
	VOID ClrStatus(VOID);//清除状态
private:
	//UserSetInfo
	HandleConnectMsgFunc m_pConnectMsgFunc;//p2p对方链接请求的回调函数
	//HandleLinkResultFunc m_pLinkResultFunc;//p2p链接建立后通知程序的函数
	MYDEF::PORT m_LanPort;//本地lan端口
	VOID ClrUserSet(VOID);//清除设置(重置)
private:
	//ThreadInfo
	MYDEF::HANDLE m_hRecvThread;//线程句柄
	MYDEF::HANDLE m_hThreadRecvComplete;//线程处理事件完毕
	MYDEF::ERRCODE m_errRecvThreadLastErrorCode;//接收线程错误码
	MYDEF::BOOL m_bRecvThreadErrorReturn;//线程因为错误返回
	MYDEF::BOOL m_bRecvThreadReturn;//是否要线程结束运行

	static MYDEF::DWORD _stdcall RecvThreadProc(P2PClient* pClass);//接收线程入口函数

	MYDEF::BOOL InitThread(VOID);//负责清理错误、重置、启动线程
	VOID ReleaseThread(VOID);//负责关闭线程、重置、释放句柄

	VOID SetThreadErrorReturn(VOID);//设置:线程遇到错误返回(接收线程调用)
	MYDEF::BOOL IsThreadErrorReturn(VOID);//获得线程是否因为错误返回(主函数调用)

	VOID SetRecvLastErrorCode(MYDEF::ERRCODE errErrorCode);//设置接收线程的最后一个错误码(接收线程调用)
	MYDEF::ERRCODE GetRecvLastErrorCode(VOID);//获得接收线程的最后一个错误码(主函数调用)

	MYDEF::BOOL SetRecvHandling(VOID);//设置线程为正在处理状态(接收线程调用)
	MYDEF::BOOL SetRecvComplete(VOID);//设置线程为处理完毕状态(接收线程调用)
	MYDEF::BOOL WaitForRecvComplete(VOID);//等待线程处理完毕(主函数调用)

	MYDEF::BOOL HandleIO(MYDEF::PBYTE byData, MYDEF::SIZE szDataSize);//接收线程处理封包的函数
	MYDEF::BOOL RecvPacket(MYDEF::PBYTE pbyRecvData, MYDEF::SIZE szDataSize, MYDEF::PSDWORD psdwRecvLen);//接收线程用于接收单个封包的函数
private:
	//OtherUserList
	MYDEF::COUNT m_ctUserCount;//用户数
	UserInfo* m_stUserList;//用户信息指针数组(一维数组)

	MYDEF::BOOL InitUserList(MYDEF::COUNT ctUserCount);//初始化并分配用户列表
	VOID ReleaseUserList(VOID);//释放用户列表
	MYDEF::BOOL IsOtherUserListEmpty(VOID);//判断用户列表是否为空

private:
	//OwnUserInfo
	MYDEF::CHAR m_cUserName[MAXUSERNAME];//用户名,以'\0'结尾(登陆后才有)
	MYDEF::TIMESTAMP m_tTime;//登陆时间戳(登陆后才有)
	IpPort m_stUserNet;//自己的公网ip、端口(登陆后才有)

	VOID SetUserName(MYDEF::PCCHAR pcUserName);//设置用户名
	VOID ClrUserName(VOID);//清除用户名
	VOID ClrUserInfo(VOID);//清除所有用户登录信息
public:
	enum ErrCode
	{
		ERROR_CODE_START = 0,
		NO_ERR = 0,
		NULL_PTR,//传入空指针
		PACKET_CHECK_FAIL,//封包校验失败
		CANOT_LOGIN,//未登录
		CANOT_MODIFY_AFTER_LOGIN,//登录后不可修改
		BUFF_SIZE_ZERO,//传入空间为0
		INDEX_OUT_OF_RANGE,//访问索引超出范围
		RUN_OUT_OF_MEMORY,//内存用完(分配失败)
		SERVER_NO_RESPONSE,//服务器未响应\不可达
		SERVER_BROKEN_LINK,//服务器断开链接
		GET_USER_LAN_FAIL,//获得用户私有端失败
		PEER_NO_RESPONSE,//用户未响应\不可达
		CONNECT_ERROR,//链接出错
		SOCKET_INIT_FAIL,//socket初始化失败
		RECV_THREAD_INIT_FAIL,//接收线程初始化失败
		RECV_FAIL,
		SEND_FAIL,

		ERROR_CODE_END,
	};
};

#undef NULL
#undef VOID
#undef CONST