/*
 *  QConsole by Davide Libenzi ( Symbian console server )
 *  Copyright (C) 2004  Davide Libenzi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Davide Libenzi <davidel@xmailserver.org>
 *
 */

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

