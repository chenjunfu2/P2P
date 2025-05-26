#include <WinSock2.h>
#pragma comment(lib ,"ws2_32.lib")
#pragma warning(disable : 4996)

#include <string.h>
#include <assert.h>


#define ERRORRETURN(errcode,ret) {SetLastErrorCode(errcode);return ret;}
#define RECVERRRETURN(errcode,ret) {SetRecvLastErrorCode(errcode);return ret;}

#define MAXWAITLOOPCOUNT 100
#define WAITSLEEPTIMEMS 100

//定义宏
#define CONST				const
#define VOID				void
#define NULL				0

//错误原因字符串
static inline MYDEF::PCCHAR ErrReason[P2PClient::ERROR_CODE_END] =
{
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
};