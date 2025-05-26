#pragma once
/**********************���ͱ���**********************/
namespace MYDEF
{
	typedef void* PVOID;
	typedef const void* PCVOID;

	typedef bool BOOL, * PBOOL;
	typedef const bool CBOOL, * PCBOOL;
	typedef char CHAR, * PCHAR;
	typedef const char CCHAR, * PCCHAR;
	typedef unsigned char UCHAR, PUCHAR;
	typedef const unsigned char CUCHAR, PCUCHAR;

	typedef unsigned __int8 BYTE, * PBYTE;
	typedef const unsigned __int8 CBYTE, * PCBYTE;
	typedef unsigned __int16 WORD, * PWORD;
	typedef const unsigned __int16 CWORD, * PCWORD;
	typedef unsigned __int32 DWORD, * PDWORD;
	typedef const unsigned __int32 CDWORD, * PCDWORD;
	typedef unsigned __int64 QWORD, * PQWORD;
	typedef const unsigned __int64 CQWORD, * PCQWORD;

	typedef signed __int8 SBYTE, * PSBYTE;
	typedef const signed __int8 CSBYTE, * PCSBYTE;
	typedef signed __int16 SWORD, * PSWORD;
	typedef const signed __int16 CSWORD, * PCSWORD;
	typedef signed __int32 SDWORD, * PSDWORD;
	typedef const signed __int32 CSDWORD, * PCSDWORD;
	typedef signed __int64 SQWORD, * PSQWORD;
	typedef const signed __int64 CSQWORD, * PCSQWORD;

	typedef void* HANDLE;//Windows���
	typedef QWORD TIMESTAMP, * PTIMESTAMP;//ʱ���
	typedef DWORD IPADDR, * PIPADDR;//ip��ַ
	typedef BYTE IPFIELD;//ip�ֶ�
	typedef WORD PORT, * PPORT;//�˿�
	typedef QWORD MESSAGE, * PMESSAGE;//������Ϣ
	typedef QWORD SOCKET, * PSOCKET;//socket���
	typedef QWORD SIZE, * PSIZE;//��С
	typedef QWORD COUNT, * PCOUNT;//����
	typedef QWORD ERRCODE, * PERRCODE;//������
}
/**********************���ͱ���**********************/


/**********************�궨��**********************/
//��Ǻ�
#define C2S	//�ͻ���->������
#define S2C	//������->�ͻ���
#define CWS	//�ͻ���<>������

//����ǳ�������
#define LOGININFO			C2S	((MYDEF::MESSAGE)256)
#define LGOUTINFO			C2S	((MYDEF::MESSAGE)257)
//��÷������ѵ�¼�û��б�
#define GETLISTSIZE			C2S	((MYDEF::MESSAGE)258)
#define GETLISTUSER			C2S	((MYDEF::MESSAGE)259)
//�������ӶԷ�
#define CONNECTUSER			C2S	((MYDEF::MESSAGE)260)
//ͬ��\�ܾ��Է�����������
#define USERPERMITCONNECT	C2S	((MYDEF::MESSAGE)261)
#define USERREJECTCONNECT	C2S	((MYDEF::MESSAGE)262)


//����ǳ��������Ļ�Ӧ
#define LOGINSUC			S2C	((MYDEF::MESSAGE)512)
#define LOGINERR			S2C	((MYDEF::MESSAGE)513)
//����û��б�������Ļ�Ӧ
#define LISTSIZEINFO		S2C	((MYDEF::MESSAGE)514)
#define LISTUSERINFO		S2C	((MYDEF::MESSAGE)515)
//�Է�����������
#define USERCONNECT			S2C	((MYDEF::MESSAGE)516)
//P2Pͬ����������˫�����͵ĶԷ���Ϣ
#define USERPERMITINFO		S2C	((MYDEF::MESSAGE)517)
#define USERREJECTINFO		S2C	((MYDEF::MESSAGE)518)


//Լ������
#define MAXUSERNAME			((MYDEF::SIZE)(64))
#define MAXLANCOUNT			((MYDEF::SIZE)(8))
#define MAXPACKETSIZE		((MYDEF::SIZE)(1024))
#define MAXUSERCNTMSG		((MYDEF::SIZE)(255))
//���ٲ���
#define GETMESSAGE(pMem)	(*((MYDEF::PMESSAGE)(((MYDEF::PBYTE)pMem)+(sizeof(MYDEF::SIZE)))))
#define GETMSGSIZE(pMem)	(*((MYDEF::PSIZE)pMem))
/**********************�궨��**********************/

//���Ƕ�汾��ʹ�ÿ��ɼ�����������ϸ��飩
//����汾֮��ṹ��ǰ������Ҫ��ȫ��ͬ����������ӳ�Ա��ͨ���ṹ���һ��size�ֶδ�С�����ְ汾