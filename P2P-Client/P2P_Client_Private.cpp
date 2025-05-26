#include "P2P_Client_Public.h"
#include "P2P_Client_Private.h"

VOID P2PClient::ClrAndReleaseResource(MYDEF::BOOL bClrUserSet)
{
	ReleaseThread();
	ReleaseSocket();
	ReleaseUserList();
	ClrStatus();
	ClrUserInfo();
	if (bClrUserSet)//�߳̽�������ܶ��û����õĻص��������в��������������
	{
		ClrUserSet();
	}
}

MYDEF::BOOL P2PClient::InitSocket(VOID)
{
	if (m_soClient == INVALID_SOCKET)
	{
		m_soClient = socket(AF_INET, SOCK_STREAM, NULL);//ʧ���򷵻� INVALID_SOCKET
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
		closesocket((SOCKET)m_soClient);//�ر��׽���
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
	//����߳��Ƿ����
	if (m_hRecvThread != NULL)
	{
		return true;
	}

	//����¼������Ƿ����
	if (m_hThreadRecvComplete != NULL)
	{
		CloseHandle(m_hThreadRecvComplete);//�ر��¼�������
		m_hThreadRecvComplete = NULL;
	}

	//�����¼�����
	m_hThreadRecvComplete = CreateEventW(NULL, TRUE, FALSE, NULL);//�ֶ��¼�����,��ʼ״̬Ϊ���ź�,ʧ�ܷ���NULL
	if (m_hThreadRecvComplete == NULL)
	{
		return false;
	}
	
	//����״̬��Ա
	{
		m_errRecvThreadLastErrorCode = NO_ERR;//�������״̬
		m_bRecvThreadErrorReturn = false;//�������״̬
		m_bRecvThreadReturn = false;//��Ҫ����
	}

	//�����߳�
	m_hRecvThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(RecvThreadProc), this, CREATE_SUSPENDED, NULL);//ʧ�ܷ���NULL �̴߳�������ͣ����ڵ�
	if (m_hRecvThread == NULL)
	{
		return false;
	}

	//�����ɹ�,��ʼ����
	ResumeThread(m_hRecvThread);
	return true;
}

VOID P2PClient::ReleaseThread(VOID)
{
	//���̷߳���
	if (m_hRecvThread != NULL)
	{
		m_bRecvThreadReturn = true;//�����̷߳���
		WaitForSingleObject(m_hRecvThread, INFINITE);//�ȴ��߳̽���

		CloseHandle(m_hRecvThread);//�ر��߳̾��
		m_hRecvThread = NULL;
	}

	//�ر��¼�������
	if (m_hThreadRecvComplete != NULL)
	{
		CloseHandle(m_hThreadRecvComplete);//�ر��¼�������
		m_hThreadRecvComplete = NULL;
	}

	//����״̬��Ա
	{
		m_errRecvThreadLastErrorCode = NO_ERR;//�������״̬
		m_bRecvThreadErrorReturn = false;//�������״̬
		m_bRecvThreadReturn = false;//��Ҫ����
	}
}

VOID P2PClient::SetThreadErrorReturn(VOID)
{
	m_bRecvThreadErrorReturn = true;//���ô���״̬
}

MYDEF::BOOL P2PClient::IsThreadErrorReturn(VOID)
{
	return m_bRecvThreadErrorReturn;
}

MYDEF::BOOL P2PClient::InitUserList(MYDEF::COUNT ctUserCount)
{
	//��С�뵱ǰ��С��ͬ,ֱ�ӷ���
	if (ctUserCount == m_ctUserCount)
	{
		return true;
	}
	m_ctUserCount = ctUserCount;//��ֵ

	if (m_ctUserCount == 0)//Ϊ0���ͷ�
	{
		ReleaseUserList();
		return true;
	}

	if (m_stUserList != NULL)//�����б����ͷ�,���ں������·���
	{
		ReleaseUserList();
	}

	m_stUserList = (UserInfo*)malloc(sizeof(UserInfo) * (size_t)m_ctUserCount);//����һ��ָ������
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
		m_ctUserCount = 0;//�û�����Ϊ0
		free(m_stUserList);//�ͷ�����ָ������
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
	m_cUserName[MAXUSERNAME - 1] = '\0';//��������ΪMAXUSERNAME - 1ʱ���ַ����ڴ��ڲ��������һ��\0���ֶ���ӣ�����strncpy���Զ���\0���ʣ���ֽ�
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

	//�鿴���
	sdwRecvLen = recv((SOCKET)m_soClient, (MYDEF::PCHAR)(&szPacketSize), sizeof(szPacketSize), MSG_PEEK);//�鿴�����С
	*psdwRecvLen = sdwRecvLen;

	if (sdwRecvLen < 0)//ʧ��
	{
		RECVERRRETURN(RECV_FAIL, false);
	}
	if (sdwRecvLen == 0)//�Ͽ�����
	{
		RECVERRRETURN(SERVER_BROKEN_LINK, false);
	}

	if (szPacketSize > szDataSize)//У�����򲻰��չ涨����,ֱ�ӶϿ�����
	{
		RECVERRRETURN(PACKET_CHECK_FAIL, false);
	}

	//��ʽ���շ��
	sdwRecvLen = recv((SOCKET)m_soClient, (MYDEF::PCHAR)(pbyRecvData), (int)szPacketSize, 0);
	*psdwRecvLen = sdwRecvLen;

	if (sdwRecvLen < 0)//ʧ��
	{
		RECVERRRETURN(RECV_FAIL, false);
	}
	if (sdwRecvLen == 0)//�Ͽ�����
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
		Sleep(0);//������ʣ��ʱ��

		//����һ�����
		if (!pClass->RecvPacket(byRecvBuffer, sizeof(byRecvBuffer), &sdwRecvLen))
		{
			pClass->SetThreadErrorReturn();//��������λ
			return -1;
		}

		//�жϲ�������
		pClass->SetRecvHandling();//��ʼ����
		if (!pClass->HandleIO(byRecvBuffer, sizeof(byRecvBuffer)))//�˴������ô�����//ʧ�ܴ�������������,ֱ�ӶϿ�����
		{
			pClass->SetThreadErrorReturn();//��������λ
			pClass->SetRecvComplete();//�������
			return -1;
		}
		pClass->SetRecvComplete();//�������
	}
	return 0;
}

MYDEF::BOOL P2PClient::HandleIO(MYDEF::PBYTE byData, MYDEF::SIZE szDataSize)
{
	//������ܵ�����Ϣ
	switch (GETMESSAGE(byData))
	{
	case LOGINSUC://��¼�ɹ�
	{
		LoginSuc* stLoginInfo = (LoginSuc*)byData;
		if (stLoginInfo->szMsgSize < sizeof(LoginSuc))//����������İ汾���ܻ�������ֻҪ��С�ڣ�ʣ�����ݿ��Բ�����
		{
			RECVERRRETURN(PACKET_CHECK_FAIL, false);
		}


	}
	break;
	case LOGINERR://��¼ʧ��
	{

	}
	break;
	case LISTSIZEINFO://�û�����
	{

	}
	break;
	case LISTUSERINFO://�û���Ϣ
	{

	}
	break;
	case USERCONNECT://�Է�����������
	{
		UserConnect* stConnectPeer = (UserConnect*)byData;
		if (stConnectPeer->szMsgSize < sizeof(UserConnect))
		{
			RECVERRRETURN(PACKET_CHECK_FAIL, false);
		}

	}
	break;
	case USERPERMITINFO://�Է�ͬ������
	{

	}
	break;
	case USERREJECTINFO://�Է��ܾ�����
	{

	}
	break;
	default:
	{
		//������󷵻�false,�Ͽ�����,��ͣ�߳�����
		//RECVERRRETURN(PACKET_CHECK_FAIL, false);//����������İ汾���ܻ�ʹ�������ṹ��Ϊ��汾���ǣ��˴����Է��
	}
	break;
	}

	return true;
}