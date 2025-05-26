#include "P2P_Client_Public.h"
#include "P2P_Client_Private.h"



VOID P2PClient::DftHandleConnectMsgFunc(MYDEF::PCCHAR pcUserName, MYDEF::PCCHAR pcUserConnectMsg)
{
	return;
}

//VOID P2PClient::DftHandleLinkResultFunc(MYDEF::SOCKET sPeerSocket)
//{
//	if (sPeerSocket != INVALID_SOCKET)
//		closesocket(sPeerSocket);
//}

P2PClient::P2PClient(MYDEF::PORT LanPort, HandleConnectMsgFunc pConnectMsgFunc)//, HandleLinkResultFunc pLinkResultFunc
{
	m_soClient = INVALID_SOCKET;

	m_bLoginServer = false;
	m_errLastErrorCode = NO_ERR;

	assert(pConnectMsgFunc != NULL);
	//assert(pLinkResultFunc != NULL);
	m_pConnectMsgFunc = pConnectMsgFunc;
	//m_pLinkResultFunc = pLinkResultFunc;
	m_LanPort = LanPort;
	
	m_hRecvThread = NULL;
	m_hThreadRecvComplete = NULL;
	m_errRecvThreadLastErrorCode = NO_ERR;
	m_bRecvThreadReturn = false;
	m_bRecvThreadErrorReturn = false;

	m_ctUserCount = 0;
	m_stUserList = NULL;
	
	m_cUserName[0] = '\0';
	m_tTime = 0;
	m_stUserNet = { 0 };
}


P2PClient::~P2PClient(VOID)
{
	LogoutServer();//下线,并关闭\重置类所有成员
}


MYDEF::BOOL P2PClient::LoginServer(IpAddr uniServerIp, MYDEF::PORT wServerPort, MYDEF::PCHAR pcUserName)
{
	//函数入参检查
	{
		if (IsLogin())//检查登录状态
		{
			return true;
		}

		if (pcUserName == NULL)//检查传入参数
		{
			ERRORRETURN(NULL_PTR, false);
		}

		if (pcUserName[0] == '\0')//字符串长度检查
		{
			ERRORRETURN(BUFF_SIZE_ZERO, false);
		}
	}

	//初始化socket
	if (!InitSocket())
	{
		ERRORRETURN(SOCKET_INIT_FAIL, false);
	}

	//链接服务器
	{
		SOCKADDR_IN addrSrv;//初始化结构
		addrSrv.sin_addr.S_un.S_addr = uniServerIp.addr;
		addrSrv.sin_family = AF_INET;
		addrSrv.sin_port = htons(wServerPort);

		if (connect((SOCKET)m_soClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) != SOCKET_ERROR)
		{
			ERRORRETURN(CONNECT_ERROR, false);
		}

		ResumeThread(m_hRecvThread);//链接成功,恢复线程执行
	}


	//创建线程
	if (!InitThread())
	{
		ERRORRETURN(RECV_THREAD_INIT_FAIL, false);
	}

	//准备封包结构体
	LoginInfo stLoginInfo;
	{
		//计算包体长度
		stLoginInfo.szMsgSize = sizeof(LoginInfo);
		//写入请求头
		stLoginInfo.mMsgHead = LOGININFO;
		//写入用户名
		strncpy(stLoginInfo.cUserName, pcUserName, MAXUSERNAME - 1);//超出截断
		stLoginInfo.cUserName[MAXUSERNAME - 1] = '\0';//强制赋值0字节，如果pcUserName长度大等于MAXUSERNAME-1，则strncpy不会给最后一个字节填充0
		//获得本地私有终端
		MYDEF::PCHAR* LanIpStrArr;
		{
			hostent* pHost;
			MYDEF::CHAR szHost[256];//限制作用域
			gethostname(szHost, 256);
			pHost = gethostbyname(szHost);//这家伙返回的是静态全局数据,不用释放
			if (pHost == NULL)//检查是否获得成功
			{
				ERRORRETURN(GET_USER_LAN_FAIL, false);
			}
			LanIpStrArr = pHost->h_addr_list;//私有终端列表
		}
		//写入私有终端
		{
			IpPort* pstUserLan = stLoginInfo.stUserLan;

			for (MYDEF::SIZE i = 0; i < MAXLANCOUNT; ++i)//MAXLANCOUNT是私有终端个数,超出截断
			{
				++stLoginInfo.dwLanCount;
				pstUserLan[i].Ip.addr = inet_addr(LanIpStrArr[i]);
				pstUserLan[i].Port = m_LanPort;
			}
		}
	}
	

	//结构体初始化完毕,开始发送
	{
		if (!SendMsgStruct(&stLoginInfo))
		{
			ClrAndReleaseResource(false);//发送错误就直接断开链接清理资源
			return false;//上面SendMsgStruct函数会设置最后一个错误码
		}

		//等待服务器返回信息
		for (MYDEF::SIZE i = 0; i < MAXWAITLOOPCOUNT; ++i)
		{
			if (IsLogin())
			{
				break;
			}
			Sleep(WAITSLEEPTIMEMS);
		}

		WaitForRecvComplete();//等待线程处理完毕
		if (IsThreadErrorReturn())//线程运行出错
		{
			MYDEF::ERRCODE errRecvThreadErrorCode = GetRecvLastErrorCode();
			ClrAndReleaseResource(false);//释放资源关闭句柄会清理错误码所以在上面保存
			ERRORRETURN(errRecvThreadErrorCode, false);
		}
	}


	if (IsLogin())
	{
		SetUserName(pcUserName);//保存用户名
	}
	else
	{
		//登录失败,服务器未响应
		ClrAndReleaseResource(false);
		ERRORRETURN(SERVER_NO_RESPONSE, false);
	}

	return true;
}

MYDEF::BOOL P2PClient::LoginServer(MYDEF::PCHAR pcIp, MYDEF::PCHAR pcPort, MYDEF::PCHAR pcUserName)
{
	return LoginServer(IpAddr{ inet_addr(pcIp) }, atoi(pcPort), pcUserName);
}

VOID P2PClient::LogoutServer(VOID)
{
	//登出
	//下线,并关闭\重置类所有成员
	if (!IsLogin())
	{
		return;
	}

	SetLogOut();//设置为下线

	LogoutInfo stLogoutInfo = { 0 };
	stLogoutInfo.szMsgSize = sizeof(LogoutInfo);
	stLogoutInfo.mMsgHead = LGOUTINFO;

	send((SOCKET)m_soClient, (MYDEF::PCHAR)(&stLogoutInfo), (int)stLogoutInfo.szMsgSize, 0);
	//不检查返回值了

	ClrAndReleaseResource(true);//释放掉一切资源

	return;
}

MYDEF::BOOL P2PClient::IsLogin(VOID)
{
	return m_bLoginServer;
}

MYDEF::BOOL P2PClient::GetUserListFromServer(VOID)
{
	if (!IsLogin())//检查登录状态
	{
		ERRORRETURN(CANOT_LOGIN, false);
	}

	//先获得用户数目
	GetListSize stGetListSize;
	{
		stGetListSize.szMsgSize = sizeof(GetListSize);
		stGetListSize.mMsgHead = GETLISTSIZE;
	}

	//结构体初始化完毕,开始发送
	{
		if (!SendMsgStruct(&stGetListSize))
		{
			ClrAndReleaseResource(false);//发送错误就直接断开链接清理资源
			return false;//上面函数会设置最后一个错误码
		}

		ReleaseUserList();//清理用户列表
		//等待服务器返回信息
		for (MYDEF::SIZE i = 0; i < MAXWAITLOOPCOUNT; ++i)
		{
			if (!IsOtherUserListEmpty())//如果还是空的,就代表服务器还没返回
			{
				break;
			}
			Sleep(WAITSLEEPTIMEMS);
		}

		WaitForRecvComplete();//等待线程处理完毕
		if (IsThreadErrorReturn())//线程运行出错
		{
			MYDEF::ERRCODE errRecvThreadErrorCode = GetRecvLastErrorCode();
			ClrAndReleaseResource(false);//释放资源关闭句柄会清理错误码所以在上面保存
			ERRORRETURN(errRecvThreadErrorCode, false);
		}
	}


	

	GetListUser stGetListUser;
	{
		stGetListUser.szMsgSize = sizeof(GetListUser);
		stGetListUser.mMsgHead = GETLISTUSER;
	}
	//结构体初始化完毕,开始发送
	{
		if (!SendMsgStruct(&stGetListUser))
		{
			ClrAndReleaseResource(false);//发送错误就直接断开链接清理资源
			return false;//上面函数会设置最后一个错误码
		}

		//等待服务器返回信息
		for (MYDEF::SIZE i = 0; i < MAXWAITLOOPCOUNT; ++i)
		{
			//if (!IsOtherUserListEmpty())//如果还是空的,就代表服务器还没返回
			{
				break;//如何判断?
			}
			Sleep(WAITSLEEPTIMEMS);
		}

		WaitForRecvComplete();//等待线程处理完毕
		if (IsThreadErrorReturn())//线程运行出错
		{
			MYDEF::ERRCODE errRecvThreadErrorCode = GetRecvLastErrorCode();
			ClrAndReleaseResource(false);//释放资源关闭句柄会清理错误码所以在上面保存
			ERRORRETURN(errRecvThreadErrorCode, false);
		}
	}






















	return true;
}

MYDEF::COUNT P2PClient::GetUserCount(VOID)
{
	return m_ctUserCount;//未登录或未获得用户列表情况下m_ctUserInfoCount为0
}

CONST P2PClient::UserInfo* P2PClient::GetUserInfo(MYDEF::COUNT ctIndex)
{
	if (ctIndex >= m_ctUserCount)//未登录或未获得用户列表情况下m_ctUserInfoCount为0直接错误返回,不需要判断是否login
	{
		ERRORRETURN(INDEX_OUT_OF_RANGE, NULL);
	}
	return &m_stUserList[ctIndex];
}

CONST P2PClient::UserInfo* P2PClient::operator[](MYDEF::COUNT ctIndex)
{
	return GetUserInfo(ctIndex);
}


MYDEF::BOOL P2PClient::SetHandleConnectMsgFunc(HandleConnectMsgFunc pConnectMsgFunc)//, HandleLinkResultFunc pLinkResultFunc
{
	if (pConnectMsgFunc == NULL)// || pLinkResultFunc == NULL
	{
		ERRORRETURN(NULL_PTR, false);
	}

	if (IsLogin())//登陆后不可改变,因为该函数已被注册入线程,修改可能带来风险
	{
		ERRORRETURN(CANOT_MODIFY_AFTER_LOGIN, false);
	}

	m_pConnectMsgFunc = pConnectMsgFunc;
	//m_pLinkResultFunc = pLinkResultFunc;

	return true;
}


VOID P2PClient::SetLastErrorCode(MYDEF::ERRCODE errErrorCode)
{
	m_errLastErrorCode = errErrorCode;
}

MYDEF::ERRCODE P2PClient::GetLastErrorCode(VOID)
{
	return m_errLastErrorCode;
}

MYDEF::PCCHAR P2PClient::GetErrorReason(MYDEF::ERRCODE errErrorCode)
{
	if (errErrorCode < ERROR_CODE_START || errErrorCode >= ERROR_CODE_END)
	{
		return "未知错误码!";
	}
	return ErrReason[errErrorCode];
}