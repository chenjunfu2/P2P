#pragma once
#include "Client_Server_MsgDefine.h"

/**********************�����ṹ��**********************/
//ip��ַ
union IpAddr
{
	MYDEF::IPADDR addr;//1����ַ
	struct { MYDEF::IPFIELD b1, b2, b3, b4; };//4���ֶ�
};

//ip��ַ+�˿ں�
struct IpPort
{
	IpAddr Ip;//ip
	MYDEF::PORT Port;//�˿ں�
};
/**********************�����ṹ��**********************/


/**********************ͨ�Žṹ��**********************/
//�����������
struct LoginInfo C2S
{
	MYDEF::SIZE szMsgSize;//��Ϣ�ṹ���С
	MYDEF::MESSAGE mMsgHead;//��Ϣͷ
	MYDEF::CHAR cUserName[MAXUSERNAME];//�û���,��'\0'��β
	MYDEF::COUNT dwLanCount;//˽���ն˸���
	IpPort stUserLan[MAXLANCOUNT];//˽���ն�
};
static_assert(sizeof(LoginInfo) <= MAXPACKETSIZE);//����,������Ϣ�ṹ��С����С���������С

//�ǳ���������
struct LogoutInfo C2S
{
	MYDEF::SIZE szMsgSize;//��Ϣ�ṹ���С
	MYDEF::MESSAGE mMsgHead;//��Ϣͷ
};
static_assert(sizeof(LogoutInfo) <= MAXPACKETSIZE);


//��ȡ�û���
struct GetListSize C2S
{
	MYDEF::SIZE szMsgSize;//��Ϣ�ṹ���С
	MYDEF::MESSAGE mMsgHead;//��Ϣͷ
};
static_assert(sizeof(GetListSize) <= MAXPACKETSIZE);

//��ȡ�û��б�
struct GetListUser C2S
{
	MYDEF::SIZE szMsgSize;//��Ϣ�ṹ���С
	MYDEF::MESSAGE mMsgHead;//��Ϣͷ
};
static_assert(sizeof(GetListUser) <= MAXPACKETSIZE);

//������������û�
struct ConnectUser C2S
{
	MYDEF::SIZE szMsgSize;//��Ϣ�ṹ���С
	MYDEF::MESSAGE mMsgHead;//��Ϣͷ
	MYDEF::CHAR cUserName[MAXUSERNAME];//�û���,��'\0'��β
	MYDEF::CHAR cUserConnectMsg[MAXUSERCNTMSG];//��������ʱ��������Ϣ
};
static_assert(sizeof(ConnectUser) <= MAXPACKETSIZE);

//ͬ����û�������
struct UserPermitConnect C2S
{
	MYDEF::SIZE szMsgSize;//��Ϣ�ṹ���С
	MYDEF::MESSAGE mMsgHead;//��Ϣͷ
	MYDEF::CHAR cUserName[MAXUSERNAME];//�û���,��'\0'��β
};
static_assert(sizeof(UserPermitConnect) <= MAXPACKETSIZE);

//�ܾ����û�������
struct UserRejectConnect C2S
{
	MYDEF::SIZE szMsgSize;//��Ϣ�ṹ���С
	MYDEF::MESSAGE mMsgHead;//��Ϣͷ
	MYDEF::CHAR cUserName[MAXUSERNAME];//�û���,��'\0'��β
};
static_assert(sizeof(UserRejectConnect) <= MAXPACKETSIZE);



//��¼�ɹ�
struct LoginSuc S2C
{
	MYDEF::SIZE szMsgSize;//��Ϣ�ṹ���С
	MYDEF::MESSAGE mMsgHead;//��Ϣͷ
	MYDEF::TIMESTAMP tLoginTime;//��½ʱ���
	IpPort stUserNet;//�ͻ����Լ��Ĺ���ip���˿�
};
static_assert(sizeof(LoginSuc) <= MAXPACKETSIZE);

//��¼ʧ��
struct LoginErr S2C
{
	MYDEF::SIZE szMsgSize;//��Ϣ�ṹ���С
	MYDEF::MESSAGE mMsgHead;//��Ϣͷ
	MYDEF::MESSAGE mErrCode;//������
};
static_assert(sizeof(LoginErr) <= MAXPACKETSIZE);


//���������ص�ǰ�ѵ�¼�û�����
struct ListSizeInfo S2C
{
	MYDEF::SIZE szMsgSize;//��Ϣ�ṹ���С
	MYDEF::MESSAGE mMsgHead;//��Ϣͷ
	MYDEF::COUNT dwUserCount;//�û���
};
static_assert(sizeof(ListSizeInfo) <= MAXPACKETSIZE);

//���������������û���Ϣ
struct ListUserInfo S2C
{
	MYDEF::SIZE szMsgSize;//��Ϣ�ṹ���С
	MYDEF::MESSAGE mMsgHead;//��Ϣͷ
	MYDEF::CHAR cUserName[MAXUSERNAME];//�û���,��'\0'��β
	MYDEF::TIMESTAMP tLoginTime;//���û���¼��������ʱ��
};
static_assert(sizeof(ListUserInfo) <= MAXPACKETSIZE);

//����û�����������
struct UserConnect S2C
{
	MYDEF::SIZE szMsgSize;//��Ϣ�ṹ���С
	MYDEF::MESSAGE mMsgHead;//��Ϣͷ
	MYDEF::CHAR cUserName[MAXUSERNAME];//�û���,��'\0'��β
	MYDEF::CHAR cUserConnectMsg[MAXUSERCNTMSG];//��������ʱ��������Ϣ
};
static_assert(sizeof(UserConnect) <= MAXPACKETSIZE);

//��ͬ�������Է�ͬ������ʱ�����������ĶԷ���Ϣ(���ʱ��ʼ��ʼ������P2P����)
struct UserPermitInfo S2C
{
	MYDEF::SIZE szMsgSize;//��Ϣ�ṹ���С
	MYDEF::MESSAGE mMsgHead;//��Ϣͷ
	MYDEF::CHAR cUserName[MAXUSERNAME];//�û���,��'\0'��β
	IpPort stUserNet;//�û��Ĺ����ն�
	MYDEF::COUNT dwLanCount;//˽���ն˸���
	IpPort stUserLan[MAXLANCOUNT];//˽���ն�
};
static_assert(sizeof(UserPermitInfo) <= MAXPACKETSIZE);

//�û��ܾ��������������
struct UserRejectInfo S2C
{
	MYDEF::SIZE szMsgSize;//��Ϣ�ṹ���С
	MYDEF::MESSAGE mMsgHead;//��Ϣͷ
	MYDEF::MESSAGE mErrCode;//������(���������û��ܾ����û���ʱδ��Ӧ���û�������(�����ߵ�)
};
static_assert(sizeof(UserRejectInfo) <= MAXPACKETSIZE);
/**********************ͨ�Žṹ��**********************/