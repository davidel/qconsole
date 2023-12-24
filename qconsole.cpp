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


#include "qinclude.h"




#ifndef SYMBIAN9
#define QCONSOLE_PASSWD_FILE "c:\\system\\data\\qconsole.txt"
#else
#define QCONSOLE_PASSWD_FILE "c:\\shared\\qconsole.txt"
#endif
#define BT_SERIAL_ID 0x1101



static void BuildProtocolDescriptionL(CSdpAttrValueDES *pDES, int iChan)
{
	TBuf8<1> channel;

	channel.Append((TChar) iChan);

	pDES->StartListL()->BuildDESL()->StartListL()
		->BuildUUIDL(KL2CAP)->EndListL()->BuildDESL()
			->StartListL()
				->BuildUUIDL(KRFCOMM)
					->BuildUintL(channel)
						->EndListL()
							->EndListL();
}

static CSdpAttrValueDES *GetDescriptor(TInt Chan)
{
	CSdpAttrValueDES *pDES = CSdpAttrValueDES::NewDESL(NULL);

	CleanupStack::PushL(pDES);
	BuildProtocolDescriptionL(pDES, Chan);

	CleanupStack::Pop(pDES);

	return pDES;
}

int QConsEnv::SetupSvrSock(TUint32 uUID)
{
	TInt r;
	TUid SUid;
	TRequestStatus RS;

	r = SkSvr.Open(SS, MKDES16("RFCOMM"));
	User::LeaveIfError(r);

	r = SkSvr.GetOpt(KRFCOMMGetAvailableServerChannel, KSolBtRFCOMM, iSvrChannel);
	User::LeaveIfError(r);

	Addr.SetPort(iSvrChannel);
	r = SkSvr.Bind(Addr);
	User::LeaveIfError(r);

	r = SkSvr.Listen(5);
	User::LeaveIfError(r);

	r = SkCli.Open(SS);
	User::LeaveIfError(r);

	r = Mgr.Connect();
	User::LeaveIfError(r);

#ifndef SYMBIAN9
	r = SDb.Open(Mgr);
	User::LeaveIfError(r);

	SUid.iUid = RProcess().Type()[1].iUid;
	Sec.SetUid(SUid);
	Sec.SetChannelID(iSvrChannel);
	Sec.SetProtocolID(KSolBtRFCOMM);
	Sec.SetAuthentication(EFalse);
	Sec.SetAuthorisation(EFalse);
	Sec.SetEncryption(EFalse);

	SDb.RegisterService(Sec, RS);
	User::WaitForRequest(RS);
	User::LeaveIfError(RS.Int());
#endif

	SdpS.Connect();
	r = SdpDB.Open(SdpS);
	User::LeaveIfError(r);

	SdpDB.CreateServiceRecordL(uUID, SdpH);

	CSdpAttrValueDES *pDES = GetDescriptor(iSvrChannel);

	SdpDB.UpdateAttributeL(SdpH, KSdpAttrIdProtocolDescriptorList, *pDES);

	delete pDES;


	SdpDB.UpdateAttributeL(SdpH,
			       KSdpAttrIdServiceID,
			       SUid.iUid);
	SdpDB.UpdateAttributeL(SdpH,
			       KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetServiceName,
			       MKDES16("QConsole"));
	SdpDB.UpdateAttributeL(SdpH,
			       KSdpAttrIdBasePrimaryLanguage + KSdpAttrIdOffsetServiceDescription,
			       MKDES16("QConsole Terminal"));

	return 0;
}

int QConsEnv::Restart(void)
{
	TInt r;

	SkCli.Close();

	SdpDB.UpdateAttributeL(SdpH, KSdpAttrIdServiceAvailability, 0xff);

	r = SkCli.Open(SS);
	User::LeaveIfError(r);

	return 0;
}

int QConsEnv::Accept(void)
{
	TRequestStatus RS;

	DebugOutput(this, 10, "Accepting ...\n");
	SkSvr.Accept(SkCli, RS);
	User::WaitForRequest(RS);
	DebugOutput(this, 10, "Done %d\n", RS.Int());

	if (RS == KErrNone)
		SdpDB.UpdateAttributeL(SdpH, KSdpAttrIdServiceAvailability, 0x00);

	return RS == KErrNone ? 0: -1;
}

QConsEnv::QConsEnv(TAny *pData)
{
	TInt r;

	iDbgLevel = 10;
	pCon = Console::NewL(MKDES16("QConsole"), TSize(KConsFullScreen, KConsFullScreen));
#ifndef __WINS__
	DebugOutput(this, 10, "Starting comms server\n");
	r = StartC32();
	if (r != KErrNone && r != KErrAlreadyExists)
		User::Leave(r);
#endif

	FS.Connect();
	SS.Connect();

	SetupSvrSock(BT_SERIAL_ID);

	strcpy(szCurPath, "c:");
}

QConsEnv::~QConsEnv(void)
{
	SkCli.Close();
	SkSvr.Close();

#ifndef SYMBIAN9
	TRequestStatus RS;
	SDb.UnregisterService(Sec, RS);
	User::WaitForRequest(RS);
#endif
	SdpDB.DeleteRecordL(SdpH);
#ifndef SYMBIAN9
	SDb.Close();
#endif
	Mgr.Close();
	SdpDB.Close();
	SS.Close();
	FS.Close();
	delete pCon;
}

char *ConvU2C(char *pDest, int iDestSize, TUint16 const *pSrc, int iSrcSize)
{
	int i;

	if (iSrcSize < 0)
		iSrcSize = User::StringLength(pSrc);
	if (iSrcSize >= iDestSize)
		iSrcSize = iDestSize - 1;
	for (i = 0; i < iSrcSize; i++)
		pDest[i] = (char) pSrc[i];
	pDest[i] = 0;
	return pDest;
}

TUint16 *ConvC2U(TUint16 *pDest, int iDestSize, char const *pSrc, int iSrcSize)
{
	int i;

	if (iSrcSize < 0)
		iSrcSize = strlen(pSrc);
	if (iSrcSize >= iDestSize)
		iSrcSize = iDestSize - 1;
	for (i = 0; i < iSrcSize; i++)
		pDest[i] = (TUint16) pSrc[i];
	pDest[i] = 0;
	return pDest;
}

char *DupConvU2C(TUint16 const *pSrc, int iSrcSize, int iAllocExtra)
{
	int i;
	char *pDest;

	if (iSrcSize < 0)
		iSrcSize = User::StringLength(pSrc);
	if (!(pDest = (char *) User::Alloc(iSrcSize + iAllocExtra + 1)))
		return NULL;
	for (i = 0; i < iSrcSize; i++)
		pDest[i] = (char) pSrc[i];
	pDest[i] = 0;
	return pDest;
}

TUint16 *DupConvC2U(char const *pSrc, int iSrcSize, int iAllocExtra)
{
	int i;
	TUint16 *pDest;

	if (iSrcSize < 0)
		iSrcSize = strlen(pSrc);
	if (!(pDest = (TUint16 *) User::Alloc((iSrcSize + iAllocExtra + 1) * sizeof(TUint16))))
		return NULL;
	for (i = 0; i < iSrcSize; i++)
		pDest[i] = (TUint16) pSrc[i];
	pDest[i] = 0;
	return pDest;
}

void *MemDup(void const *pData, int iSize, int iExtra)
{
	void *pDData;

	if ((pDData = User::Alloc(iSize + iExtra)) == NULL)
		return NULL;
	memcpy(pDData, pData, iSize);

	return pDData;
}

int DebugOutput(QConsEnv *pCE, int iLevel, char const *pszFmt, ...)
{
	int iSize = 256, iLength = -1;
	char *pszBuf;
	TUint16 *pUBuf;
	VA_LIST Args;

	if (pCE->pCon == NULL || iLevel > pCE->iDbgLevel)
		return 0;
	for (;;)
	{
		if ((pszBuf = (char *) User::Alloc(iSize + 1)) == NULL)
			return -1;

		TPtr8 hDest((unsigned char *) pszBuf, iSize);

		VA_START(Args, pszFmt);
		TRAPD(iError, hDest.FormatList(TPtrC8((TUint8 const *) pszFmt), Args));
		VA_END(Args);
		if (iError == KErrNone)
		{
			hDest.ZeroTerminate();
			iLength = hDest.Length();
			break;
		}
		User::Free(pszBuf);
		iSize *= 2;
	}
	pUBuf = DupConvC2U(pszBuf, iLength, 0);
	User::Free(pszBuf);
	if (pUBuf == NULL)
		return -1;
	pCE->pCon->Printf(_L("%s"), pUBuf);
	User::Free(pUBuf);

	return iLength;
}

int SendPkt(QConsEnv *pCE, TUint8 const *pData, int iSize)
{
	TRequestStatus RS;
	char PktBuf[8];

	PktBuf[0] = 0;
	PUT_LE16((unsigned int) iSize, PktBuf + 1);
	pCE->SkCli.Write(TPtrC8((TUint8 const *) PktBuf, 3), RS);
	User::WaitForRequest(RS);

	if (iSize > 0)
	{
		pCE->SkCli.Write(TPtrC8(pData, iSize), RS);
		User::WaitForRequest(RS);

		if (RS != KErrNone)
			return -1;
	}

	return 0;
}

static int RealRecv(QConsEnv *pCE, TUint8 *pBuf, int iSize)
{
	int iRecvd;
	TRequestStatus RS;

	for (iRecvd = 0; iRecvd < iSize;)
	{
		TPtr8 hBuf(pBuf + iRecvd, 0, iSize - iRecvd);

		pCE->SkCli.Read(hBuf, RS);
		User::WaitForRequest(RS);

		if (RS != KErrNone)
			return -1;

		iRecvd += hBuf.Length();
	}

	return iRecvd;
}

int RecvPkt(QConsEnv *pCE, TUint8 **ppData, int *piSize)
{
	TUint32 uSize;
	char PktBuf[8];

	if (RealRecv(pCE, (TUint8 *) PktBuf, 3) != 3)
		return -1;
	GET_LE16(uSize, PktBuf + 1);
	if ((*ppData = (TUint8 *) User::Alloc(uSize + 1)) == NULL)
		return -1;

	if (RealRecv(pCE, *ppData, uSize) != (int) uSize)
	{
		User::Free(*ppData);
		return -1;
	}
	(*ppData)[uSize] = 0;
	*piSize = (int) uSize;

	return 0;
}

void FreePktData(TUint8 *pData)
{

	User::Free(pData);
}

static int VerifyLogin(TUint8 const *pszPwdSha, TUint8 const *pPasswd, int iPasswdSize,
		       char const *pszRndStr) {
	int i;
	unsigned int uXVal;
	sha1_ctx_t ShCtx;
	unsigned char ShaDgst[SHA1_DIGEST_SIZE];

	sha1_init(&ShCtx);
	sha1_update(&ShCtx, (unsigned char const *) pszRndStr, strlen(pszRndStr));
	sha1_update(&ShCtx, (unsigned char const *) ",", 1);
	sha1_update(&ShCtx, (unsigned char const *) pPasswd, iPasswdSize);
	sha1_final(ShaDgst, &ShCtx);
	for (i = 0; i < (int) sizeof(ShaDgst); i++)
		if (sscanf((char const *) pszPwdSha + 2 * i, "%2x", &uXVal) != 1 ||
		    uXVal != (unsigned int) ShaDgst[i])
			return -1;

	return 0;
}

static void GenRndLoginString(QConsEnv *pCE)
{
	int i;

	srand(time(NULL));
	for (i = 0; i < 8; i++)
		sprintf(pCE->szRndStr + 2 * i, "%02x", rand() & 0xff);
}

static int QConsoleLogin(QConsEnv *pCE)
{
	int iUserSize, iPassSize;
	TUint8 *pUserData, *pPassData;

	if (RecvPkt(pCE, &pUserData, &iUserSize) < 0)
		return -1;
	if (RecvPkt(pCE, &pPassData, &iPassSize) < 0)
	{
		FreePktData(pUserData);
		return -1;
	}
	DebugOutput(pCE, 10, "User='%s' Pass='%s'\n", pUserData, pPassData);
	if (iPassSize != 2 * SHA1_DIGEST_SIZE)
	{
		DebugOutput(pCE, 1, "Bad login syntax\n");
		FreePktData(pPassData);
		FreePktData(pUserData);
		SendPkt(pCE, (TUint8 const *) "Bad login syntax\n",
			STRSIZE("Bad login syntax\n"));
		return -1;
	}

	RFile File;

	if (File.Open(pCE->FS, MKDES16(QCONSOLE_PASSWD_FILE), EFileRead) != KErrNone)
	{
		DebugOutput(pCE, 1, "Unable to open password file: %s\n", QCONSOLE_PASSWD_FILE);
		FreePktData(pPassData);
		FreePktData(pUserData);
		SendPkt(pCE, (TUint8 const *) "Unable to open password file\n",
			STRSIZE("Unable to open password file\n"));
		return -1;
	}

	TInt iSize = 0;

	File.Size(iSize);

	TUint8 *pFBuf = (TUint8 *) User::Alloc(iSize + 1);

	if (!pFBuf)
	{
		File.Close();
		FreePktData(pPassData);
		FreePktData(pUserData);
		SendPkt(pCE, (TUint8 const *) "Unable to parse password file\n",
			STRSIZE("Unable to parse password file\n"));
		return -1;
	}

	TPtr8 hFBuf(pFBuf, iSize + 1);

	TInt iResult = File.Read(hFBuf, iSize);

	File.Close();

	if (iResult != KErrNone)
	{
		User::Free(pFBuf);
		FreePktData(pPassData);
		FreePktData(pUserData);
		SendPkt(pCE, (TUint8 const *) "Unable to parse password file\n",
			STRSIZE("Unable to parse password file\n"));
		return -1;
	}

	TBool bMatch = EFalse;
	TLex8 Lex(TPtrC8(pFBuf, iSize));

	for (; !bMatch && !Lex.Eos();)
	{
		TPtrC8 Usr = Lex.NextToken();

		Lex.SkipCharacters();

		TPtrC8 Pwd = Lex.NextToken();

		Lex.SkipCharacters();

		if (Pwd.Length() == 0)
			break;

		if (Usr.Compare(TPtrC8(pUserData, iUserSize)) == 0)
		{
			bMatch = VerifyLogin(pPassData, Pwd.Ptr(), Pwd.Length(),
					     pCE->szRndStr) == 0;
			break;
		}
	}

	User::Free(pFBuf);
	FreePktData(pPassData);
	FreePktData(pUserData);

	if (!bMatch)
	{
		SendPkt(pCE, (TUint8 const *) "Unable to authenticate\n",
			STRSIZE("Unable to authenticate\n"));
		return -1;
	}

	return SendPkt(pCE, (TUint8 *) "", 0);
}

static int SendWelcome(QConsEnv *pCE)
{
	char szWelcome[512];

	sprintf(szWelcome, "Welcome to QConsole %s <%s>\n",
		QCONSOLE_VERSION, pCE->szRndStr);

	return SendPkt(pCE, (TUint8 *) szWelcome, strlen(szWelcome));
}

static int QConsoleMain(TAny *pData)
{
	int iResult = 0;
	QConsEnv *pCE = new QConsEnv(pData);

	CleanupStack::PushL(pCE);

#ifndef SYMBIAN9
	RProcess().Rename(MKDES16("QConsole"));
#endif

	DebugOutput(pCE, 1, "QConsole ready on channel %d\n", pCE->iSvrChannel);
	for (;;)
	{
		DebugOutput(pCE, 10, "Waiting connections ...\n");
		while (pCE->Accept() < 0)
			User::After(1000 * 1000);
		DebugOutput(pCE, 10, "Got it\n");
		GenRndLoginString(pCE);
		SendWelcome(pCE);

		if (QConsoleLogin(pCE) < 0)
		{
			pCE->Restart();
			continue;
		}
		for (;;)
		{
			int iSize;
			TUint8 *pData;

			if (RecvPkt(pCE, &pData, &iSize) == 0)
			{
				iResult = ProcessInput(pCE, pData, iSize);

				FreePktData(pData);
				if (iResult < 0)
					break;
			}
		}
		if (iResult < QCRES_EXIT)
			break;
		pCE->Restart();
	}

	DebugOutput(pCE, 10, "Closing server connection\n");
	CleanupStack::Pop(pCE);
	delete pCE;

	return iResult;
}

TInt QConsoleThread(TAny *pData)
{
	int iResult = -1;
	CTrapCleanup *pTC = CTrapCleanup::New();

	CActiveScheduler *pAS = new CActiveScheduler();

	CActiveScheduler::Install(pAS);

	_LIT(KQConsole, "QConsole");

	TRAPD(iError, iResult = QConsoleMain(pData));
	__ASSERT_ALWAYS(!iError, User::Panic(KQConsole, iError));

	CActiveScheduler::Install(NULL);
	delete pAS;
	delete pTC;

	if (iResult == QCRES_REBOOT)
		UserSvr::ResetMachine(EStartupWarmReset);

	return KErrNone;
}

