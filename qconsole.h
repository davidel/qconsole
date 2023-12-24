//    Copyright 2023 Davide Libenzi
// 
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
// 
//        http://www.apache.org/licenses/LICENSE-2.0
// 
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
// 


#if !defined(_QCONSOLE_H)
#define _QCONSOLE_H


#define COUNT_OF(a) (sizeof(a) / sizeof(a[0]))
#define STRSIZE(s) (sizeof(s) - 1)
#define MKDES16(s) TPtrC16(_S(s), STRSIZE(s))

#define QCONSOLE_VERSION "1.61"


class QConsEnv: public CBase
{
public:
	int Restart(void);
	int Accept(void);
	QConsEnv(TAny *pData);
	~QConsEnv(void);

	int iDbgLevel;
	CConsoleBase *pCon;
	RSocketServ SS;
	RSdpDatabase SdpDB;
	RSdp SdpS;
	TSdpServRecordHandle SdpH;
	RBTMan Mgr;
#ifndef SYMBIAN9
	RBTSecuritySettings SDb;
	TBTServiceSecurity Sec;
#endif
	TBTSockAddr Addr;
	TInt iSvrChannel;
	RSocket SkSvr;
	RSocket SkCli;
	RFs FS;
	char szCurPath[256];
	char szRndStr[256];

private:
	int SetupSvrSock(TUint32 uUID);
};


char *ConvU2C(char *pDest, int iDestSize, TUint16 const *pSrc, int iSrcSize = -1);
TUint16 *ConvC2U(TUint16 *pDest, int iDestSize, char const *pSrc, int iSrcSize = -1);
char *DupConvU2C(TUint16 const *pSrc, int iSrcSize, int iAllocExtra = 0);
TUint16 *DupConvC2U(char const *pSrc, int iSrcSize, int iAllocExtra = 0);
void *MemDup(void const *pData, int iSize, int iExtra = 0);
int DebugOutput(QConsEnv *pCE, int iLevel, char const *pszFmt, ...);
int SendPkt(QConsEnv *pCE, TUint8 const *pData, int iSize);
int RecvPkt(QConsEnv *pCE, TUint8 **ppData, int *piSize);
void FreePktData(TUint8 *pData);
TInt QConsoleThread(TAny *pData);


#endif

