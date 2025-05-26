#include "P2P_Client_Public.h"
#include "P2P_Client_Private.h"

VOID P2PClient::ClrAndReleaseResource(MYDEF::BOOL bClrUserSet)
{
	ReleaseThread();
	ReleaseSocket();
	ReleaseUserList();
	ClrStatus();
	ClrUserInfo();
	if (bClrUserSet)//线程结束后才能对用户设置的回调函数进行操作，以免出问题
	{
		ClrUserSet();
	}
}

MYDEF::BOOL P2PClient::InitSocket(VOID)
{
	if (m_soClient == INVALID_SOCKET)
	{
		m_soClient = socket(AF_INET, SOCK_STREAM, NULL);//失败则返回 INVALID_SOCKET
		if (m_soClient == INVALID_SOCKET)
		{
			return false;
		}
	}
	return true;
}

VOID P2PClient::ReleaseSocket(VOID)
{
	if (m_soClient != INVALID_SOCKET)
	{
		closesocket((SOCKET)m_soClient);//关闭套接字
		m_soClient = INVALID_SOCKET;
	}
}

MYDEF::BOOL P2PClient::SendMsgStruct(MYDEF::PCVOID pSendStruct)
{
	MYDEF::SDWORD sdwSendLen = send((SOCKET)m_soClient, (MYDEF::PCCHAR)pSendStruct, (int)GETMSGSIZE(pSendStruct), 0);
	if (sdwSendLen < 0)
	{
		ERRORRETURN(SEND_FAIL, false);
	}
	if (sdwSendLen == 0)
	{
		ERRORRETURN(SERVER_BROKEN_LINK, false);
	}
	return true;
}

VOID P2PClient::SetLogin(VOID)
{
	m_bLoginServer = true;
}

VOID P2PClient::SetLogOut(VOID)
{
	m_bLoginServer = false;
}

VOID P2PClient::ClrStatus(VOID)
{
	SetLogOut();
	m_errLastErrorCode = NO_ERR;
}

VOID P2PClient::ClrUserSet(VOID)
{
	m_pConnectMsgFunc = DftHandleConnectMsgFunc;
	//m_pLinkResultFunc = DftHandleLinkResultFunc;
	m_LanPort = 0;
}

MYDEF::BOOL P2PClient::InitThread(VOID)
{
	//检查线程是否存在
	if (m_hRecvThread != NULL)
	{
		return true;
	}

	//检查事件对象是否存在
	if (m_hThreadRecvComplete != NULL)
	{
		CloseHandle(m_hThreadRecvComplete);//关闭事件对象句柄
		m_hThreadRecvComplete = NULL;
	}

	//创建事件对象
	m_hThreadRecvComplete = CreateEventW(NULL, TRUE, FALSE, NULL);//手动事件对象,初始状态为无信号,失败返回NULL
	if (m_hThreadRecvComplete == NULL)
	{
		return false;
	}
	
	//重置状态成员
	{
		m_errRecvThreadLastErrorCode = NO_ERR;//清除错误状态
		m_bRecvThreadErrorReturn = false;//清除错误状态
		m_bRecvThreadReturn = false;//不要返回
	}

	//创建线程
	m_hRecvThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(RecvThreadProc), this, CREATE_SUSPENDED, NULL);//失败返回NULL 线程创建后暂停在入口点
	if (m_hRecvThread == NULL)
	{
		return false;
	}

	//创建成功,开始运行
	ResumeThread(m_hRecvThread);
	return true;
}

VOID P2PClient::ReleaseThread(VOID)
{
	//让线程返回
	if (m_hRecvThread != NULL)
	{
		m_bRecvThreadReturn = true;//接收线程返回
		WaitForSingleObject(m_hRecvThread, INFINITE);//等待线程结束

		CloseHandle(m_hRecvThread);//关闭线程句柄
		m_hRecvThread = NULL;
	}

	//关闭事件对象句柄
	if (m_hThreadRecvComplete != NULL)
	{
		CloseHandle(m_hThreadRecvComplete);//关闭事件对象句柄
		m_hThreadRecvComplete = NULL;
	}

	//重置状态成员
	{
		m_errRecvThreadLastErrorCode = NO_ERR;//清除错误状态
		m_bRecvThreadErrorReturn = false;//清除错误状态
		m_bRecvThreadReturn = false;//不要返回
	}
}

VOID P2PClient::SetThreadErrorReturn(VOID)
{
	m_bRecvThreadErrorReturn = true;//设置错误状态
}

MYDEF::BOOL P2PClient::IsThreadErrorReturn(VOID)
{
	return m_bRecvThreadErrorReturn;
}

MYDEF::BOOL P2PClient::InitUserList(MYDEF::COUNT ctUserCount)
{
	//大小与当前大小相同,直接返回
	if (ctUserCount == m_ctUserCount)
	{
		return true;
	}
	m_ctUserCount = ctUserCount;//赋值

	if (m_ctUserCount == 0)//为0则释放
	{
		ReleaseUserList();
		return true;
	}

	if (m_stUserList != NULL)//存在列表则释放,并在后面重新分配
	{
		ReleaseUserList();
	}

	m_stUserList = (UserInfo*)malloc(sizeof(UserInfo) * (size_t)m_ctUserCount);//分配一个指针数组
	if (m_stUserList == NULL)
	{
		return false;
	}

	return true;
}

VOID P2PClient::ReleaseUserList(VOID)
{
	if (m_stUserList != NULL)
	{
		m_ctUserCount = 0;//用户数设为0
		free(m_stUserList);//释放整个指针数组
		m_stUserList = NULL;
	}
}

MYDEF::BOOL P2PClient::IsOtherUserListEmpty(VOID)
{
	return m_ctUserCount == 0;
}


VOID P2PClient::SetRecvLastErrorCode(MYDEF::ERRCODE errErrorCode)
{
	m_errRecvThreadLastErrorCode = errErrorCode;
}

MYDEF::ERRCODE P2PClient::GetRecvLastErrorCode(VOID)
{
	return m_errRecvThreadLastErrorCode;
}

MYDEF::BOOL P2PClient::SetRecvHandling(VOID)
{
	return ResetEvent(m_hThreadRecvComplete);
}

MYDEF::BOOL P2PClient::SetRecvComplete(VOID)
{
	return SetEvent(m_hThreadRecvComplete);
}

MYDEF::BOOL P2PClient::WaitForRecvComplete(VOID)
{
	return WaitForSingleObject(m_hThreadRecvComplete, INFINITE) == WAIT_OBJECT_0;
}

VOID P2PClient::SetUserName(MYDEF::PCCHAR pcUserName)
{
	strncpy(m_cUserName, pcUserName, MAXUSERNAME - 1);
	m_cUserName[MAXUSERNAME - 1] = '\0';//拷贝长度为MAXUSERNAME - 1时，字符串内存内不包含最后一个\0，手动添加，否则strncpy会自动用\0填充剩余字节
}


VOID P2PClient::ClrUserName(VOID)
{
	m_cUserName[0] = '\0';
}

VOID P2PClient::ClrUserInfo(VOID)
{
	ClrUserName();
	m_tTime = 0;
	m_stUserNet = { 0 };
}

MYDEF::BOOL P2PClient::RecvPacket(MYDEF::PBYTE pbyRecvData, MYDEF::SIZE szDataSize, MYDEF::PSDWORD psdwRecvLen)
{
	MYDEF::SDWORD sdwRecvLen;
	MYDEF::SIZE szPacketSize;

	//查看封包
	sdwRecvLen = recv((SOCKET)m_soClient, (MYDEF::PCHAR)(&szPacketSize), sizeof(szPacketSize), MSG_PEEK);//查看封包大小
	*psdwRecvLen = sdwRecvLen;

	if (sdwRecvLen < 0)//失败
	{
		RECVERRRETURN(RECV_FAIL, false);
	}
	if (sdwRecvLen == 0)//断开链接
	{
		RECVERRRETURN(SERVER_BROKEN_LINK, false);
	}

	if (szPacketSize > szDataSize)//校验出错或不按照规定发包,直接断开链接
	{
		RECVERRRETURN(PACKET_CHECK_FAIL, false);
	}

	//正式接收封包
	sdwRecvLen = recv((SOCKET)m_soClient, (MYDEF::PCHAR)(pbyRecvData), (int)szPacketSize, 0);
	*psdwRecvLen = sdwRecvLen;

	if (sdwRecvLen < 0)//失败
	{
		RECVERRRETURN(RECV_FAIL, false);
	}
	if (sdwRecvLen == 0)//断开链接
	{
		RECVERRRETURN(SERVER_BROKEN_LINK, false);
	}

	return true;
}

MYDEF::DWORD _stdcall P2PClient::RecvThreadProc(P2PClient* pClass)
{
	MYDEF::BYTE byRecvBuffer[MAXPACKETSIZE];
	MYDEF::SDWORD sdwRecvLen;
	while (!pClass->m_bRecvThreadReturn)
	{
		Sleep(0);//放弃掉剩余时间

		//接收一个封包
		if (!pClass->RecvPacket(byRecvBuffer, sizeof(byRecvBuffer), &sdwRecvLen))
		{
			pClass->SetThreadErrorReturn();//出错返回置位
			return -1;
		}

		//判断并处理封包
		pClass->SetRecvHandling();//开始处理
		if (!pClass->HandleIO(byRecvBuffer, sizeof(byRecvBuffer)))//此处会设置错误码//失败代表数据有问题,直接断开链接
		{
			pClass->SetThreadErrorReturn();//出错返回置位
			pClass->SetRecvComplete();//处理完毕
			return -1;
		}
		pClass->SetRecvComplete();//处理完毕
	}
	return 0;
}

MYDEF::BOOL P2PClient::HandleIO(MYDEF::PBYTE byData, MYDEF::SIZE szDataSize)
{
	//处理接受到的消息
	switch (GETMESSAGE(byData))
	{
	case LOGINSUC://登录成功
	{
		LoginSuc* stLoginInfo = (LoginSuc*)byData;
		if (stLoginInfo->szMsgSize < sizeof(LoginSuc))//服务器后面的版本可能会比这个大，只要不小于，剩下数据可以不处理
		{
			RECVERRRETURN(PACKET_CHECK_FAIL, false);
		}


	}
	break;
	case LOGINERR://登录失败
	{

	}
	break;
	case LISTSIZEINFO://用户个数
	{

	}
	break;
	case LISTUSERINFO://用户信息
	{

	}
	break;
	case USERCONNECT://对方请求链接你
	{
		UserConnect* stConnectPeer = (UserConnect*)byData;
		if (stConnectPeer->szMsgSize < sizeof(UserConnect))
		{
			RECVERRRETURN(PACKET_CHECK_FAIL, false);
		}

	}
	break;
	case USERPERMITINFO://对方同意链接
	{

	}
	break;
	case USERREJECTINFO://对方拒绝链接
	{

	}
	break;
	default:
	{
		//封包错误返回false,断开链接,暂停线程运行
		//RECVERRRETURN(PACKET_CHECK_FAIL, false);//服务器后面的版本可能会使用其它结构，为多版本考虑，此处忽略封包
	}
	break;
	}

	return true;
}