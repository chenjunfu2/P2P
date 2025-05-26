#pragma once
/**********************类型别名**********************/
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

	typedef void* HANDLE;//Windows句柄
	typedef QWORD TIMESTAMP, * PTIMESTAMP;//时间戳
	typedef DWORD IPADDR, * PIPADDR;//ip地址
	typedef BYTE IPFIELD;//ip字段
	typedef WORD PORT, * PPORT;//端口
	typedef QWORD MESSAGE, * PMESSAGE;//附加消息
	typedef QWORD SOCKET, * PSOCKET;//socket句柄
	typedef QWORD SIZE, * PSIZE;//大小
	typedef QWORD COUNT, * PCOUNT;//计数
	typedef QWORD ERRCODE, * PERRCODE;//错误码
}
/**********************类型别名**********************/


/**********************宏定义**********************/
//标记宏
#define C2S	//客户端->服务器
#define S2C	//服务器->客户端
#define CWS	//客户端<>服务器

//登入登出服务器
#define LOGININFO			C2S	((MYDEF::MESSAGE)256)
#define LGOUTINFO			C2S	((MYDEF::MESSAGE)257)
//获得服务器已登录用户列表
#define GETLISTSIZE			C2S	((MYDEF::MESSAGE)258)
#define GETLISTUSER			C2S	((MYDEF::MESSAGE)259)
//请求链接对方
#define CONNECTUSER			C2S	((MYDEF::MESSAGE)260)
//同意\拒绝对方的链接请求
#define USERPERMITCONNECT	C2S	((MYDEF::MESSAGE)261)
#define USERREJECTCONNECT	C2S	((MYDEF::MESSAGE)262)


//登入登出服务器的回应
#define LOGINSUC			S2C	((MYDEF::MESSAGE)512)
#define LOGINERR			S2C	((MYDEF::MESSAGE)513)
//获得用户列表服务器的回应
#define LISTSIZEINFO		S2C	((MYDEF::MESSAGE)514)
#define LISTUSERINFO		S2C	((MYDEF::MESSAGE)515)
//对方请求链接你
#define USERCONNECT			S2C	((MYDEF::MESSAGE)516)
//P2P同意后服务器向双方发送的对方信息
#define USERPERMITINFO		S2C	((MYDEF::MESSAGE)517)
#define USERREJECTINFO		S2C	((MYDEF::MESSAGE)518)


//约定常量
#define MAXUSERNAME			((MYDEF::SIZE)(64))
#define MAXLANCOUNT			((MYDEF::SIZE)(8))
#define MAXPACKETSIZE		((MYDEF::SIZE)(1024))
#define MAXUSERCNTMSG		((MYDEF::SIZE)(255))
//快速操作
#define GETMESSAGE(pMem)	(*((MYDEF::PMESSAGE)(((MYDEF::PBYTE)pMem)+(sizeof(MYDEF::SIZE)))))
#define GETMSGSIZE(pMem)	(*((MYDEF::PSIZE)pMem))
/**********************宏定义**********************/

//考虑多版本（使用宽松检查封包而不是严格检查）
//多个版本之间结构体前面内容要完全相同后面可以增加成员，通过结构体第一个size字段大小来区分版本