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
	LogoutServer();//����,���ر�\���������г�Ա
}


MYDEF::BOOL P2PClient::LoginServer(IpAddr uniServerIp, MYDEF::PORT wServerPort, MYDEF::PCHAR pcUserName)
{
	//������μ��
	{
		if (IsLogin())//����¼״̬
		{
			return true;
		}

		if (pcUserName == NULL)//��鴫�����
		{
			ERRORRETURN(NULL_PTR, false);
		}

		if (pcUserName[0] == '\0')//�ַ������ȼ��
		{
			ERRORRETURN(BUFF_SIZE_ZERO, false);
		}
	}

	//��ʼ��socket
	if (!InitSocket())
	{
		ERRORRETURN(SOCKET_INIT_FAIL, false);
	}

	//���ӷ�����
	{
		SOCKADDR_IN addrSrv;//��ʼ���ṹ
		addrSrv.sin_addr.S_un.S_addr = uniServerIp.addr;
		addrSrv.sin_family = AF_INET;
		addrSrv.sin_port = htons(wServerPort);

		if (connect((SOCKET)m_soClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)) != SOCKET_ERROR)
		{
			ERRORRETURN(CONNECT_ERROR, false);
		}

		ResumeThread(m_hRecvThread);//���ӳɹ�,�ָ��߳�ִ��
	}


	//�����߳�
	if (!InitThread())
	{
		ERRORRETURN(RECV_THREAD_INIT_FAIL, false);
	}

	//׼������ṹ��
	LoginInfo stLoginInfo;
	{
		//������峤��
		stLoginInfo.szMsgSize = sizeof(LoginInfo);
		//д������ͷ
		stLoginInfo.mMsgHead = LOGININFO;
		//д���û���
		strncpy(stLoginInfo.cUserName, pcUserName, MAXUSERNAME - 1);//�����ض�
		stLoginInfo.cUserName[MAXUSERNAME - 1] = '\0';//ǿ�Ƹ�ֵ0�ֽڣ����pcUserName���ȴ����MAXUSERNAME-1����strncpy��������һ���ֽ����0
		//��ñ���˽���ն�
		MYDEF::PCHAR* LanIpStrArr;
		{
			hostent* pHost;
			MYDEF::CHAR szHost[256];//����������
			gethostname(szHost, 256);
			pHost = gethostbyname(szHost);//��һﷵ�ص��Ǿ�̬ȫ������,�����ͷ�
			if (pHost == NULL)//����Ƿ��óɹ�
			{
				ERRORRETURN(GET_USER_LAN_FAIL, false);
			}
			LanIpStrArr = pHost->h_addr_list;//˽���ն��б�
		}
		//д��˽���ն�
		{
			IpPort* pstUserLan = stLoginInfo.stUserLan;

			for (MYDEF::SIZE i = 0; i < MAXLANCOUNT; ++i)//MAXLANCOUNT��˽���ն˸���,�����ض�
			{
				++stLoginInfo.dwLanCount;
				pstUserLan[i].Ip.addr = inet_addr(LanIpStrArr[i]);
				pstUserLan[i].Port = m_LanPort;
			}
		}
	}
	

	//�ṹ���ʼ�����,��ʼ����
	{
		if (!SendMsgStruct(&stLoginInfo))
		{
			ClrAndReleaseResource(false);//���ʹ����ֱ�ӶϿ�����������Դ
			return false;//����SendMsgStruct�������������һ��������
		}

		//�ȴ�������������Ϣ
		for (MYDEF::SIZE i = 0; i < MAXWAITLOOPCOUNT; ++i)
		{
			if (IsLogin())
			{
				break;
			}
			Sleep(WAITSLEEPTIMEMS);
		}

		WaitForRecvComplete();//�ȴ��̴߳������
		if (IsThreadErrorReturn())//�߳����г���
		{
			MYDEF::ERRCODE errRecvThreadErrorCode = GetRecvLastErrorCode();
			ClrAndReleaseResource(false);//�ͷ���Դ�رվ����������������������汣��
			ERRORRETURN(errRecvThreadErrorCode, false);
		}
	}


	if (IsLogin())
	{
		SetUserName(pcUserName);//�����û���
	}
	else
	{
		//��¼ʧ��,������δ��Ӧ
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
	//�ǳ�
	//����,���ر�\���������г�Ա
	if (!IsLogin())
	{
		return;
	}

	SetLogOut();//����Ϊ����

	LogoutInfo stLogoutInfo = { 0 };
	stLogoutInfo.szMsgSize = sizeof(LogoutInfo);
	stLogoutInfo.mMsgHead = LGOUTINFO;

	send((SOCKET)m_soClient, (MYDEF::PCHAR)(&stLogoutInfo), (int)stLogoutInfo.szMsgSize, 0);
	//����鷵��ֵ��

	ClrAndReleaseResource(true);//�ͷŵ�һ����Դ

	return;
}

MYDEF::BOOL P2PClient::IsLogin(VOID)
{
	return m_bLoginServer;
}

MYDEF::BOOL P2PClient::GetUserListFromServer(VOID)
{
	if (!IsLogin())//����¼״̬
	{
		ERRORRETURN(CANOT_LOGIN, false);
	}

	//�Ȼ���û���Ŀ
	GetListSize stGetListSize;
	{
		stGetListSize.szMsgSize = sizeof(GetListSize);
		stGetListSize.mMsgHead = GETLISTSIZE;
	}

	//�ṹ���ʼ�����,��ʼ����
	{
		if (!SendMsgStruct(&stGetListSize))
		{
			ClrAndReleaseResource(false);//���ʹ����ֱ�ӶϿ�����������Դ
			return false;//���溯�����������һ��������
		}

		ReleaseUserList();//�����û��б�
		//�ȴ�������������Ϣ
		for (MYDEF::SIZE i = 0; i < MAXWAITLOOPCOUNT; ++i)
		{
			if (!IsOtherUserListEmpty())//������ǿյ�,�ʹ����������û����
			{
				break;
			}
			Sleep(WAITSLEEPTIMEMS);
		}

		WaitForRecvComplete();//�ȴ��̴߳������
		if (IsThreadErrorReturn())//�߳����г���
		{
			MYDEF::ERRCODE errRecvThreadErrorCode = GetRecvLastErrorCode();
			ClrAndReleaseResource(false);//�ͷ���Դ�رվ����������������������汣��
			ERRORRETURN(errRecvThreadErrorCode, false);
		}
	}


	

	GetListUser stGetListUser;
	{
		stGetListUser.szMsgSize = sizeof(GetListUser);
		stGetListUser.mMsgHead = GETLISTUSER;
	}
	//�ṹ���ʼ�����,��ʼ����
	{
		if (!SendMsgStruct(&stGetListUser))
		{
			ClrAndReleaseResource(false);//���ʹ����ֱ�ӶϿ�����������Դ
			return false;//���溯�����������һ��������
		}

		//�ȴ�������������Ϣ
		for (MYDEF::SIZE i = 0; i < MAXWAITLOOPCOUNT; ++i)
		{
			//if (!IsOtherUserListEmpty())//������ǿյ�,�ʹ����������û����
			{
				break;//����ж�?
			}
			Sleep(WAITSLEEPTIMEMS);
		}

		WaitForRecvComplete();//�ȴ��̴߳������
		if (IsThreadErrorReturn())//�߳����г���
		{
			MYDEF::ERRCODE errRecvThreadErrorCode = GetRecvLastErrorCode();
			ClrAndReleaseResource(false);//�ͷ���Դ�رվ����������������������汣��
			ERRORRETURN(errRecvThreadErrorCode, false);
		}
	}






















	return true;
}

MYDEF::COUNT P2PClient::GetUserCount(VOID)
{
	return m_ctUserCount;//δ��¼��δ����û��б������m_ctUserInfoCountΪ0
}

CONST P2PClient::UserInfo* P2PClient::GetUserInfo(MYDEF::COUNT ctIndex)
{
	if (ctIndex >= m_ctUserCount)//δ��¼��δ����û��б������m_ctUserInfoCountΪ0ֱ�Ӵ��󷵻�,����Ҫ�ж��Ƿ�login
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

	if (IsLogin())//��½�󲻿ɸı�,��Ϊ�ú����ѱ�ע�����߳�,�޸Ŀ��ܴ�������
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
		return "δ֪������!";
	}
	return ErrReason[errErrorCode];
}