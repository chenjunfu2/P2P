#pragma once

#include"Client_Server_Shared.h"
#include "P2P_Shared.h"

//�����
#define CONST				const
#define VOID				void
#define NULL				0

class P2PClient//�벻Ҫ�̳и���,��Ҫ�����򴫵ݸ����Constʵ��,����Ҫ���Ƹ����Ӹ����һ��ʵ��������һ�������ʵ��
{
private://˽�е�Ĭ�ϴ�����
	static VOID DftHandleConnectMsgFunc(MYDEF::PCCHAR pcUserName, MYDEF::PCCHAR pcUserConnectMsg);//Ĭ�ϵ�������Ϣ������(�ú���Ӧ���ٴ������������,��������������߳�)
	//static VOID DftHandleLinkResultFunc(MYDEF::SOCKET sPeerSocket);//Ĭ�ϵ����ӽ��������,ֱ�ӹر����Ӿ��,�Ͽ�����(�ú���Ӧ���ٴ������������,��������������߳�)

public://������Ա�Ͷ���
	typedef VOID(*HandleConnectMsgFunc)(MYDEF::PCCHAR pcUserName, MYDEF::PCCHAR pcUserConnectMsg);//����ָ��궨��
	//typedef VOID(*HandleLinkResultFunc)(MYDEF::SOCKET sPeerSocket);//����ָ��궨��

	struct UserInfo
	{
		MYDEF::CHAR cUserName[MAXUSERNAME];//�û���,��'\0'��β
		MYDEF::TIMESTAMP tTime;//���û���¼��������ʱ��
	};

	P2PClient(MYDEF::PORT Port = 0,
		HandleConnectMsgFunc pConnectMsgFunc = DftHandleConnectMsgFunc);
		//HandleLinkResultFunc pLinkResultFunc = DftHandleLinkResultFunc);//��Ҫ���ض�������ʹ�õĶ˿ںźͽ��ܶԷ�p2p����Ĵ���ص�����
	~P2PClient(VOID);

	MYDEF::BOOL LoginServer(IpAddr uniServerIp, MYDEF::PORT wServerPort, MYDEF::PCHAR UserName);//��¼������
	MYDEF::BOOL LoginServer(MYDEF::PCHAR pcIp, MYDEF::PCHAR pcPort, MYDEF::PCHAR pcUserName);

	VOID LogoutServer(VOID);//�ǳ�������,������������Դ
	MYDEF::BOOL IsLogin(VOID);//����Ƿ��¼

	MYDEF::BOOL GetUserListFromServer(VOID);//�ӷ������õ��û��б�

	MYDEF::COUNT GetUserCount(VOID);//����û��б��С
	CONST UserInfo* GetUserInfo(MYDEF::COUNT ctIndex);//���ڷ��ʵ����û���Ϣ
	CONST UserInfo* operator[](MYDEF::COUNT ctIndex);//���������,���ڷ��ʵ����û���Ϣ

	MYDEF::SOCKET ConnectAPeer(CONST UserInfo& stUserInfo);//P2P����һ���û�
	MYDEF::BOOL DisConnectAPeer(MYDEF::SOCKET soPeerSocket);//��һ��P2P�û��Ͽ�����

	MYDEF::SOCKET AgreePeerLink(MYDEF::PCCHAR pcUserName);//�����������ӵ�ĳ���û�(�贫���û���,��ֹͬʱ��������»���)
	VOID RejectPeerLink(MYDEF::PCCHAR pcUserName);//�ܾ��������ӵ�ĳ���û�(�贫���û���,��ֹͬʱ��������»���)

	MYDEF::BOOL SetHandleConnectMsgFunc(HandleConnectMsgFunc pConnectMsgFunc = DftHandleConnectMsgFunc);
										//HandleLinkResultFunc pLinkResultFunc = DftHandleLinkResultFunc);//�������ú���(������������������ΪĬ��)

	VOID SetLastErrorCode(MYDEF::ERRCODE errErrorCode);//�������һ��������
	MYDEF::ERRCODE GetLastErrorCode(VOID);//������һ������Ĵ�����
	MYDEF::PCCHAR GetErrorReason(MYDEF::ERRCODE errErrorCode);//ͨ���������ô���ԭ��

public://���εĳ�Ա����
	P2PClient(CONST P2PClient&) = delete;
	P2PClient(CONST P2PClient*) = delete;

	P2PClient& operator=(CONST P2PClient&) = delete;
	P2PClient& operator=(CONST P2PClient*) = delete;
	P2PClient& operator=(CONST P2PClient) = delete;
private:
	//˽�г�Ա�������������˽�г�Ա�ĺ�����
	VOID ClrAndReleaseResource(MYDEF::BOOL bClrUserSet);

	
	MYDEF::BOOL BuildPeerLink(VOID);//������������
private:
	//˽�����ݺ͹����ĳ�Ա����
	MYDEF::SOCKET m_soClient;//�׽���������

	MYDEF::BOOL InitSocket(VOID);
	VOID ReleaseSocket(VOID);//�ƻ����ӹرվ��������
	MYDEF::BOOL SendMsgStruct(MYDEF::PCVOID pSendStruct);

private:
	//StatusInfo
	MYDEF::BOOL m_bLoginServer;//��¼״̬
	MYDEF::ERRCODE m_errLastErrorCode;//������

	VOID SetLogin(VOID);//����Ϊ��¼״̬
	VOID SetLogOut(VOID);//����Ϊ����״̬
	VOID ClrStatus(VOID);//���״̬
private:
	//UserSetInfo
	HandleConnectMsgFunc m_pConnectMsgFunc;//p2p�Է���������Ļص�����
	//HandleLinkResultFunc m_pLinkResultFunc;//p2p���ӽ�����֪ͨ����ĺ���
	MYDEF::PORT m_LanPort;//����lan�˿�
	VOID ClrUserSet(VOID);//�������(����)
private:
	//ThreadInfo
	MYDEF::HANDLE m_hRecvThread;//�߳̾��
	MYDEF::HANDLE m_hThreadRecvComplete;//�̴߳����¼����
	MYDEF::ERRCODE m_errRecvThreadLastErrorCode;//�����̴߳�����
	MYDEF::BOOL m_bRecvThreadErrorReturn;//�߳���Ϊ���󷵻�
	MYDEF::BOOL m_bRecvThreadReturn;//�Ƿ�Ҫ�߳̽�������

	static MYDEF::DWORD _stdcall RecvThreadProc(P2PClient* pClass);//�����߳���ں���

	MYDEF::BOOL InitThread(VOID);//��������������á������߳�
	VOID ReleaseThread(VOID);//����ر��̡߳����á��ͷž��

	VOID SetThreadErrorReturn(VOID);//����:�߳��������󷵻�(�����̵߳���)
	MYDEF::BOOL IsThreadErrorReturn(VOID);//����߳��Ƿ���Ϊ���󷵻�(����������)

	VOID SetRecvLastErrorCode(MYDEF::ERRCODE errErrorCode);//���ý����̵߳����һ��������(�����̵߳���)
	MYDEF::ERRCODE GetRecvLastErrorCode(VOID);//��ý����̵߳����һ��������(����������)

	MYDEF::BOOL SetRecvHandling(VOID);//�����߳�Ϊ���ڴ���״̬(�����̵߳���)
	MYDEF::BOOL SetRecvComplete(VOID);//�����߳�Ϊ�������״̬(�����̵߳���)
	MYDEF::BOOL WaitForRecvComplete(VOID);//�ȴ��̴߳������(����������)

	MYDEF::BOOL HandleIO(MYDEF::PBYTE byData, MYDEF::SIZE szDataSize);//�����̴߳������ĺ���
	MYDEF::BOOL RecvPacket(MYDEF::PBYTE pbyRecvData, MYDEF::SIZE szDataSize, MYDEF::PSDWORD psdwRecvLen);//�����߳����ڽ��յ�������ĺ���
private:
	//OtherUserList
	MYDEF::COUNT m_ctUserCount;//�û���
	UserInfo* m_stUserList;//�û���Ϣָ������(һά����)

	MYDEF::BOOL InitUserList(MYDEF::COUNT ctUserCount);//��ʼ���������û��б�
	VOID ReleaseUserList(VOID);//�ͷ��û��б�
	MYDEF::BOOL IsOtherUserListEmpty(VOID);//�ж��û��б��Ƿ�Ϊ��

private:
	//OwnUserInfo
	MYDEF::CHAR m_cUserName[MAXUSERNAME];//�û���,��'\0'��β(��½�����)
	MYDEF::TIMESTAMP m_tTime;//��½ʱ���(��½�����)
	IpPort m_stUserNet;//�Լ��Ĺ���ip���˿�(��½�����)

	VOID SetUserName(MYDEF::PCCHAR pcUserName);//�����û���
	VOID ClrUserName(VOID);//����û���
	VOID ClrUserInfo(VOID);//��������û���¼��Ϣ
public:
	enum ErrCode
	{
		ERROR_CODE_START = 0,
		NO_ERR = 0,
		NULL_PTR,//�����ָ��
		PACKET_CHECK_FAIL,//���У��ʧ��
		CANOT_LOGIN,//δ��¼
		CANOT_MODIFY_AFTER_LOGIN,//��¼�󲻿��޸�
		BUFF_SIZE_ZERO,//����ռ�Ϊ0
		INDEX_OUT_OF_RANGE,//��������������Χ
		RUN_OUT_OF_MEMORY,//�ڴ�����(����ʧ��)
		SERVER_NO_RESPONSE,//������δ��Ӧ\���ɴ�
		SERVER_BROKEN_LINK,//�������Ͽ�����
		GET_USER_LAN_FAIL,//����û�˽�ж�ʧ��
		PEER_NO_RESPONSE,//�û�δ��Ӧ\���ɴ�
		CONNECT_ERROR,//���ӳ���
		SOCKET_INIT_FAIL,//socket��ʼ��ʧ��
		RECV_THREAD_INIT_FAIL,//�����̳߳�ʼ��ʧ��
		RECV_FAIL,
		SEND_FAIL,

		ERROR_CODE_END,
	};
};

#undef NULL
#undef VOID
#undef CONST