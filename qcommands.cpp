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

#include "qinclude.h"



#define CHUNK_PREFIX "$chk."
#define QC_BUFF_SIZE (1024 * 4)
#define QC_IOBUFF_SIZE (1024 * 1)
#define ISCMD(p, n, s) ((n) >= (int) STRSIZE(s) && memcmp(p, s, STRSIZE(s)) == 0 && \
				((p)[STRSIZE(s)] == 0 || (p)[STRSIZE(s)] == ' '))




struct HalDump
{
	TInt iAttr;
	char const *pszFmt;
	int (*pTransAttr)(TInt, char *, int);
};


static int TransInt(TInt iVal, char *pszBuf, int iSize);
static int TransHex(TInt iVal, char *pszBuf, int iSize);


static HalDump const HalAttr[] =
{
	{
		HALData::EManufacturer,
			"Manufacturer           : %s\n",
			TransInt
	},
	{
		HALData::EManufacturerHardwareRev,
			"Manufacturer HW Rev    : %s\n",
			TransHex
	},
	{
		HALData::EManufacturerSoftwareRev,
			"Manufacturer SW Rev    : %s\n",
			TransHex
	},
	{
		HALData::EManufacturerSoftwareBuild,
			"Manufacturer SW Bld    : %s\n",
			TransHex
	},
	{
		HALData::EModel,
			"Model                  : %s\n",
			TransHex
	},
	{
		HALData::EMachineUid,
			"Machine UID            : %s\n",
			TransHex
	},
	{
		HALData::EDeviceFamily,
			"Device Family          : %s\n",
			TransInt
	},
	{
		HALData::EDeviceFamilyRev,
			"Device Family Rev      : %s\n",
			TransInt
	},
	{
		HALData::ECPU,
			"CPU                    : %s\n",
			TransInt
	},
	{
		HALData::ECPUArch,
			"CPU Arch               : %s\n",
			TransInt
	},
	{
		HALData::ECPUABI,
			"CPU ABI                : %s\n",
			TransInt
	},
	{
		HALData::ECPUSpeed,
			"CPU Speed              : %s KHz\n",
			TransInt
	},
	{
		HALData::ESystemStartupReason,
			"System Startup Reason  : %s\n",
			TransInt
	},
	{
		HALData::ESystemException,
			"System Exception       : %s\n",
			TransInt
	},
	{
		HALData::ESystemTickPeriod,
			"System Tick Period     : %s\n",
			TransInt
	},
	{
		HALData::EMemoryRAM,
			"RAM                    : %s bytes\n",
			TransInt
	},
	{
		HALData::EMemoryRAMFree,
			"RAM free               : %s bytes\n",
			TransInt
	},
	{
		HALData::EMemoryROM,
			"ROM                    : %s bytes\n",
			TransInt
	},
	{
		HALData::EMemoryPageSize,
			"Page Size              : %s bytes\n",
			TransInt
	},
	{
		HALData::EPowerGood,
			"Power Good             : %s\n",
			TransInt
	},
	{
		HALData::EPowerBatteryStatus,
			"Power Battery Status   : %s\n",
			TransInt
	},
	{
		HALData::EPowerBackup,
			"Power Backup           : %s\n",
			TransInt
	},
	{
		HALData::EPowerBackupStatus,
			"Backup Battery Status  : %s\n",
			TransInt
	},
	{
		HALData::EPowerExternal,
			"Power External         : %s\n",
			TransInt
	},
	{
		HALData::EKeyboard,
			"Keyboard               : %s\n",
			TransInt
	},
	{
		HALData::EKeyboardDeviceKeys,
			"Keyboard Device Keys   : %s\n",
			TransInt
	},
	{
		HALData::EKeyboardAppKeys,
			"Keyboard App Keys      : %s\n",
			TransInt
	},
	{
		HALData::EKeyboardClick,
			"Keyboard Click         : %s\n",
			TransInt
	},
	{
		HALData::EKeyboardClickState,
			"Keyboard Click State   : %s\n",
			TransInt
	},
	{
		HALData::EKeyboardClickVolume,
			"Keyboard Click Vol     : %s\n",
			TransInt
	},
	{
		HALData::EKeyboardClickVolumeMax,
			"Keyboard Click Vol Max : %s\n",
			TransInt
	},
	{
		HALData::EDisplayXPixels,
			"Display X Pixels       : %s\n",
			TransInt
	},
	{
		HALData::EDisplayYPixels,
			"Display Y Pixels       : %s\n",
			TransInt
	},
	{
		HALData::EDisplayXTwips,
			"Display X Twips        : %s\n",
			TransInt
	},
	{
		HALData::EDisplayYTwips,
			"Display Y Twips        : %s\n",
			TransInt
	},
	{
		HALData::EDisplayColors,
			"Display Colors         : %s\n",
			TransInt
	},
	{
		HALData::EDisplayState,
			"Display State          : %s\n",
			TransInt
	},
	{
		HALData::EDisplayContrast,
			"Display Contrast       : %s\n",
			TransInt
	},
	{
		HALData::EDisplayContrastMax,
			"Display Contrast Max   : %s\n",
			TransInt
	},
	{
		HALData::EBacklight,
			"Backlight              : %s\n",
			TransInt
	},
	{
		HALData::EBacklightState,
			"Backlight State        : %s\n",
			TransInt
	},
	{
		HALData::EPen,
			"Pen                    : %s\n",
			TransInt
	},
	{
		HALData::EPenX,
			"Pen X                  : %s\n",
			TransInt
	},
	{
		HALData::EPenY,
			"Pen Y                  : %s\n",
			TransInt
	},
	{
		HALData::EPenDisplayOn,
			"Pen Display On         : %s\n",
			TransInt
	},
	{
		HALData::EPenClick,
			"Pen Click              : %s\n",
			TransInt
	},
	{
		HALData::EPenClickState,
			"Pen Click State        : %s\n",
			TransInt
	},
	{
		HALData::EPenClickVolume,
			"Pen Click Vol          : %s\n",
			TransInt
	},
	{
		HALData::EPenClickVolumeMax,
			"Pen Click Vol Max      : %s\n",
			TransInt
	},
	{
		HALData::EMouse,
			"Mouse                  : %s\n",
			TransInt
	},
	{
		HALData::EMouseX,
			"Mouse X                : %s\n",
			TransInt
	},
	{
		HALData::EMouseY,
			"Mouse Y                : %s\n",
			TransInt
	},
	{
		HALData::EMouseState,
			"Mouse State            : %s\n",
			TransInt
	},
	{
		HALData::EMouseAcceleration,
			"Mouse Accel            : %s\n",
			TransInt
	},
	{
		HALData::EMouseButtons,
			"Mouse Buttons          : %s\n",
			TransInt
	},
	{
		HALData::EMouseButtonState,
			"Mouse Button State     : %s\n",
			TransInt
	},
	{
		HALData::ECaseState,
			"Case State             : %s\n",
			TransInt
	},
	{
		HALData::ECaseSwitch,
			"Case Switch            : %s\n",
			TransInt
	},
	{
		HALData::ECaseSwitchDisplayOn,
			"Case State Display On  : %s\n",
			TransInt
	},
	{
		HALData::ECaseSwitchDisplayOff,
			"Case State Display Off : %s\n",
			TransInt
	},
	{
		HALData::ELEDs,
			"LEDs                   : %s\n",
			TransInt
	},
	{
		HALData::ELEDmask,
			"LED Mask               : %s\n",
			TransHex
	},
	{
		HALData::EIntegratedPhone,
			"Integrated Phone       : %s\n",
			TransInt
	},
	{
		HALData::EDisplayBrightness,
			"Display Brightness     : %s\n",
			TransInt
	},
	{
		HALData::EDisplayBrightnessMax,
			"Display Brightness Max : %s\n",
			TransInt
	},
	{
		HALData::EKeyboardBacklightState,
			"Keyboard Bklight State : %s\n",
			TransInt
	},
	{
		HALData::EAccessoryPower,
			"Accessory Power        : %s\n",
			TransInt
	},
	{
		HALData::ELanguageIndex,
			"Language Index         : %s\n",
			TransInt
	},
	{
		HALData::EKeyboardIndex,
			"Keyboard Index         : %s\n",
			TransInt
	},
	{
		HALData::EMaxRAMDriveSize,
			"Max RAM Drive Size     : %s bytes\n",
			TransInt
	},
	{
		HALData::EKeyboardState,
			"Keyboard State         : %s\n",
			TransInt
	},
	{
		HALData::ESystemDrive,
			"System Drive           : %s\n",
			TransInt
	},
	{
		HALData::EPenState,
			"Pen State              : %s\n",
			TransInt
	},
	{
		HALData::EDisplayIsMono,
			"Display Is Mono        : %s\n",
			TransInt
	},
	{
		HALData::EDisplayIsPalettized,
			"Display Is Palettized  : %s\n",
			TransInt
	},
	{
		HALData::EDisplayBitsPerPixel,
			"Display Bits Per Pixel : %s\n",
			TransInt
	},
	{
		HALData::EDisplayNumModes,
			"Display Num Modes      : %s\n",
			TransInt
	},
	{
		HALData::EDisplayMemoryAddress,
			"Display Memory Address : %s\n",
			TransHex
	},
	{
		HALData::EDisplayOffsetToFirstPixel,
			"Display Off 1st Pixel  : %s\n",
			TransInt
	},
	{
		HALData::EDisplayOffsetBetweenLines,
			"Display Off From Pixel : %s\n",
			TransInt
	},
	{
		HALData::EDisplayPaletteEntry,
			"Display Palette Entry  : %s\n",
			TransInt
	},
	{
		HALData::EDisplayIsPixelOrderRGB,
			"Display Pixel Ord RGB  : %s\n",
			TransInt
	},
	{
		HALData::EDisplayIsPixelOrderLandscape,
			"Display Pixel Ord Land : %s\n",
			TransInt
	},
	{
		HALData::EDisplayMode,
			"Display Mode           : %s\n",
			TransInt
	},
	{
		HALData::ESwitches,
			"Switches               : %s\n",
			TransHex
	},
	{
		HALData::EDebugPort,
			"Debug Port             : %s\n",
			TransInt
	},
	{
		HALData::ELocaleLoaded,
			"Locale Loaded          : %s\n",
			TransInt
	},
	{
		HALData::EClipboardDrive,
			"Clipboard Drive        : %s\n",
			TransInt
	},
};

const char * const pszHelpStr = "QConsole supported commands:\n\n"
	"help                  = Prints this help screen\n"
	"pwd                   = Returns the current default directory\n"
	"exit                  = Quit the QConsole session\n"
	"shutdown              = Quit the QConsole session and shuts down the QConsole server\n"
	"reboot                = Quit the QConsole session and reboot the remote device\n"
	"cd DIR                = Change the current directory to DIR (.. equal up one level)\n"
	"ls [DIR/FILE]         = Lists the current directory or the specified path\n"
	"ps                    = Lists currently running processes\n"
	"kill PID              = Terminates the process identified by PID\n"
#ifndef SYMBIAN9
	"prio PID PRIO         = Adjust process PID priority to PRIO (L=low B=back F=fore H=high)\n"
	"prot PID PROT         = Set/unset process PID protected state (P=protected U=unprotected)\n"
	"pren PID NAM          = Change process PID name to NAME\n"
#endif
	"pinf PID              = Display process PID informations\n"
	"run APP/EXE ...       = Starts the application or exe with the specified parameters\n"
	"rundoc FILE           = Starts an application which can handle FILE\n"
	"drives                = Lists available drives and associated information\n"
	"mkdir DIR             = Creates the directory specified by DIR\n"
	"rmdir DIR             = Removes the directory specified by DIR\n"
	"rmtree DIR            = Removes the whole tree underneath DIR (dangerous)\n"
	"rm FILE               = Removes the file specified by FILE\n"
	"chmod MODE FILE       = Change the access mode of FILE applying MODE changes\n"
	"                        ([+-][hws]+ | h=hidden w=write s=system)\n"
	"mv OLD NEW            = Renames files or directories from OLD to NEW (must be on same drive)\n"
	"cp SRC DST            = Copy SRC file onto DST file\n"
	"cat FILE              = Dump the content of remote file FILE on screen\n"
	"lsthr [PID]           = Lists threads of process PID (all existing threads if PID is not specified)\n"
	"lssem                 = Lists all global semaphores available on the system\n"
	"lsrchk                = Lists all global memory chunks available on the system\n"
	"lspdd                 = Lists all physical devices available on the system\n"
	"lsldd                 = Lists all logical devices available on the system\n"
	"lsdll                 = Lists all DLLs currently loaded on the system\n"
	"hdump                 = Dump device information\n"
	"find [-1s] PATH MATCH = Recursively searches PATH for MATCH file or directory name\n"
	"                        (wildcards allowed in MATCH). Allowed flags for the command are:\n"
	"                          1 : Do not recurse inside subdirectories\n"
	"                          s : Print only the file path\n"
	"getchk CHK LOC        = Dumps the content of the remote chunk (on device) CHK to local file LOC\n"
	"get [-R] REM LOC      = Copies the remote file(s) (on device) REM to local file/path LOC.\n"
	"                        Wildcards allowed in REM. Allowed flags for the command are:\n"
	"                          R : Recurse to subdirectories\n"
	"put [-Rf] REM LOC     = Copies the local file(s) LOC to the remote file/path (on device) REM.\n"
	"                        Wildcards allowed in LOC. Allowed flags for the command are:\n"
	"                          R : Recurse to subdirectories\n"
	"                          f : Forces the missing subdirectories creation on the device\n\n";




static int TransInt(TInt iVal, char *pszBuf, int iSize)
{
	sprintf(pszBuf, "%d", iVal);
	return 0;
}

static int TransHex(TInt iVal, char *pszBuf, int iSize)
{
	sprintf(pszBuf, "%08x", iVal);
	return 0;
}

static int Do_hdump(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	char *pszBuf = (char *) User::Alloc(QC_BUFF_SIZE);

	if (pszBuf == NULL)
		return 1;

	for (int i = 0; i < (int) COUNT_OF(HalAttr); i++)
	{
		TInt iVal;
		char szTrans[128];

		if (HAL::Get((HAL::TAttribute) HalAttr[i].iAttr, iVal) != KErrNone)
			continue;
		HalAttr[i].pTransAttr(iVal, szTrans, sizeof(szTrans) - 1);
		sprintf(pszBuf, HalAttr[i].pszFmt, szTrans);

		if (SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf)) < 0)
		{
			User::Free(pszBuf);
			return -1;
		}
	}
	User::Free(pszBuf);

	return 0;
}

static int Do_help(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	char *pszBuf = (char *) User::Alloc(QC_BUFF_SIZE);

	if (pszBuf == NULL)
		return 1;

	sprintf(pszBuf,
		"QConsole v%s - Symbian Console Server\n"
		"Copyright (C) 2004  Davide Libenzi <davidel@xmailserver.org>\n\n",
		QCONSOLE_VERSION);

	if (SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf)) < 0)
	{
		User::Free(pszBuf);
		return -1;
	}
	User::Free(pszBuf);

	if (SendPkt(pCE, (TUint8 const *) pszHelpStr, strlen(pszHelpStr)) < 0)
		return -1;

	return 0;
}

static int Do_pwd(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	char szOut[512];

	sprintf(szOut, "%s\n", pCE->szCurPath);
	if (SendPkt(pCE, (TUint8 const *) szOut, strlen(szOut)) < 0)
		return -1;

	return 0;
}

static int Do_exit(QConsEnv *pCE, TUint8 *pData, int iSize)
{

	SendPkt(pCE, (TUint8 const *) "QConsole closing connection\n",
		STRSIZE("QConsole closing connection\n"));

	return QCRES_EXIT;
}

static int Do_shutdown(QConsEnv *pCE, TUint8 *pData, int iSize)
{

	SendPkt(pCE, (TUint8 const *) "QConsole shutting down\n",
		STRSIZE("QConsole shutting down\n"));

	return QCRES_SHUTDOWN;
}

static int Do_reboot(QConsEnv *pCE, TUint8 *pData, int iSize)
{

	SendPkt(pCE, (TUint8 const *) "QConsole shutting down (system reboot)\n",
		STRSIZE("QConsole shutting down (system reboot)\n"));

	return QCRES_REBOOT;
}

static TInt PreparePath(RFs *pFS, TUint16 const *pPath)
{
	TInt iPos;
	TPtrC hPath(pPath);

	if ((iPos = hPath.LocateReverse((TUint16) '\\')) == KErrNotFound ||
	    hPath.Locate((TUint16) '\\') == iPos)
		return KErrNone;

	return pFS->MkDirAll(TPtrC(pPath, iPos + 1));
}

static int MakePath(char *pszDest, char const *pszCD, TPtrC8 Path)
{
	int iLength = Path.Length();

	if (*(Path.Ptr()) == '\\')
	{
		memcpy(pszDest, pszCD, 2);
		memcpy(pszDest + 2, Path.Ptr(), iLength);
		iLength += 2;
		for (; iLength > 0 && pszDest[iLength - 1] == '\\'; iLength--);
		pszDest[iLength] = 0;
	}
	else if (iLength >= 2 && Path.Ptr()[1] == ':')
	{
		memcpy(pszDest, Path.Ptr(), iLength);
		for (; iLength > 0 && pszDest[iLength - 1] == '\\'; iLength--);
		pszDest[iLength] = 0;
	}
	else if (iLength == 1 && *(Path.Ptr()) == '.')
	{
		iLength = sprintf(pszDest, "%s", pszCD);
	}
	else if (iLength == 2 && memcmp(Path.Ptr(), "..", 2) == 0)
	{
		char *pszBS;

		iLength = sprintf(pszDest, "%s", pszCD);
		if ((pszBS = strrchr(pszDest, '\\')) != NULL)
		{
			*pszBS = 0;
			iLength = pszBS - pszDest;
		}
	}
	else
	{
		iLength = sprintf(pszDest, "%s\\", pszCD);
		memcpy(pszDest + iLength, Path.Ptr(), Path.Length());
		iLength += Path.Length();
		for (; iLength > 0 && pszDest[iLength - 1] == '\\'; iLength--);
		pszDest[iLength] = 0;
	}

	return iLength;
}

static char *GetLsString(TEntry const &Ent, TDesC16 const *pFullName, char *pszLsStr)
{
	TBool bIsExec = EFalse;
	TInt iExt = Ent.iName.FindC(MKDES16(".app"));

	if (iExt != KErrNotFound && iExt == Ent.iName.Length() - 4)
		bIsExec = ETrue;
	else if ((iExt = Ent.iName.FindC(MKDES16(".exe"))) != KErrNotFound &&
		 iExt == Ent.iName.Length() - 4)
		bIsExec = ETrue;

	char const *pszPerm;
	char const *pszDE = Ent.IsDir() ? "d": "-";

	if (bIsExec)
		pszPerm = Ent.IsReadOnly() ? "r-xr-xr-x": "rwxrwxrwx";
	else
		pszPerm = Ent.IsReadOnly() ? "r--r--r--": "rw-rw-rw-";

	char szName[256], szDate[64];

	TDateTime DT = Ent.iModified.DateTime();
	char const *pszMonths[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul",
			"Aug", "Sep", "Oct", "Nov", "Dec" };

	sprintf(szDate, "%s %2d %02d:%02d %4d", pszMonths[DT.Month()], DT.Day() + 1,
		DT.Hour(), DT.Minute(), DT.Year());

	if (pFullName == NULL)
		ConvU2C(szName, sizeof(szName) - 1, TPtrC16(Ent.iName).Ptr(),
			TPtrC16(Ent.iName).Length());
	else
		ConvU2C(szName, sizeof(szName) - 1, pFullName->Ptr(),
			pFullName->Length());
	sprintf(pszLsStr, "%s%s %7d %s %s\n", pszDE, pszPerm, Ent.iSize,
		szDate, szName);

	return pszLsStr;
}

static int Do_cd(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	TPtrC8 Dir = Lex.NextToken();

	char *pszBuf = (char *) User::Alloc(QC_BUFF_SIZE);

	if (pszBuf == NULL)
		return 1;

	int iLength = MakePath(pszBuf, pCE->szCurPath, Dir);
	TUint16 *pDir = (TUint16 *) User::Alloc((iLength + 1) * sizeof(TUint16));

	if (pDir == NULL)
	{
		User::Free(pszBuf);
		return -1;
	}

	ConvC2U(pDir, iLength + 1, pszBuf, iLength);

	TEntry Ent;
	TDriveInfo DI;

	if (iLength <= 2)
	{
		if (pCE->FS.Drive(DI, EDriveA + ((*pszBuf & 0xdf) - 'A')) != KErrNone)
		{
			User::Free(pDir);

			strcat(pszBuf, " : Drive does not exist\n");
			DebugOutput(pCE, 2, "%s", pszBuf);

			int iResult = SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf));
			User::Free(pszBuf);
			return iResult == 0 ? 1: iResult;
		}
	}
	else
	{
		if (pCE->FS.Entry(TPtrC(pDir), Ent) != KErrNone)
		{
			User::Free(pDir);

			strcat(pszBuf, " : Directory does not exist\n");
			DebugOutput(pCE, 2, "%s", pszBuf);

			int iResult = SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf));
			User::Free(pszBuf);
			return iResult == 0 ? 1: iResult;
		}
		User::Free(pDir);
		if (!Ent.IsDir())
		{
			strcat(pszBuf, " : Is a file name\n");
			DebugOutput(pCE, 2, "%s", pszBuf);

			int iResult = SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf));
			User::Free(pszBuf);
			return iResult == 0 ? 1: iResult;
		}
	}

	strcpy(pCE->szCurPath, pszBuf);

	User::Free(pszBuf);

	return 0;
}

static int Do_ls(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	TPtrC8 Dir = Lex.NextToken();

	char *pszBuf = (char *) User::Alloc(QC_BUFF_SIZE);

	if (pszBuf == NULL)
		return 1;

	int iLength = MakePath(pszBuf, pCE->szCurPath, Dir);
	TUint16 *pDir = DupConvC2U(pszBuf, iLength);

	if (pDir == NULL)
	{
		User::Free(pszBuf);
		return -1;
	}

	TEntry Ent;

	if (pCE->FS.Entry(TPtrC(pDir), Ent) == KErrNone &&
	    !Ent.IsDir())
	{
		User::Free(pDir);
		GetLsString(Ent, NULL, pszBuf);

		return SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf));
	}
	User::Free(pDir);

	strcpy(pszBuf + iLength, "\\*");
	iLength += 2;

	if (!(pDir = DupConvC2U(pszBuf, iLength)))
	{
		User::Free(pszBuf);
		return -1;
	}

	CDir *pLst = NULL;

	if (pCE->FS.GetDir(TPtrC(pDir), KEntryAttMatchMask, 0, pLst) != KErrNone)
	{
		User::Free(pDir);

		strcat(pszBuf, " : Directory does not exist\n");
		DebugOutput(pCE, 2, "%s", pszBuf);

		int iResult = SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf));
		User::Free(pszBuf);
		return iResult == 0 ? 1: iResult;
	}

	int iResult = 0;

	for (int i = 0; i < pLst->Count(); i++)
	{
		GetLsString((*pLst)[i], NULL, pszBuf);

		if ((iResult = SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf))) < 0)
			break;
	}

	delete pLst;
	User::Free(pDir);
	User::Free(pszBuf);

	return iResult;
}

static int Do_ps(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	char const *pszHdr = " PID PPID  FLG PRI     UID2     UID3 NAME\n";
	TFindProcess FP;
	TFullName FName;
	char *pszBuf;
	char szName[256];

	if (SendPkt(pCE, (TUint8 const *) pszHdr, strlen(pszHdr)) < 0)
		return -1;

	if ((pszBuf = (char *) User::Alloc(QC_BUFF_SIZE)) == NULL)
		return 1;

	while (FP.Next(FName) == KErrNone)
	{
		RProcess Proc;
		char szFlags[16] = "";

		if (Proc.Open(FP) != KErrNone)
			continue;

		unsigned int uPPid = (unsigned int) Proc.Id();
		RProcess PProc;
#ifndef SYMBIAN9
		if (Proc.Owner(PProc) == KErrNone)
		{
			uPPid = (unsigned int) PProc.Id();
			PProc.Close();
		}
		if (Proc.Protected())
			strcat(szFlags, "P");
		if (!Proc.LoadedFromRam())
			strcat(szFlags, "R");
#endif
		TProcessMemoryInfo PMI;

		Proc.GetMemoryInfo(PMI);

		ConvU2C(szName, sizeof(szName) - 1, TPtrC16(FName).Ptr(),
			TPtrC16(FName).Length());

		TUidType Type = Proc.Type();

		sprintf(pszBuf, "%4u %4u %4s %u %08x:%08x %s\n", (unsigned int) Proc.Id(), uPPid,
			szFlags, (unsigned int) Proc.Priority(), (unsigned int) Type[1].iUid,
			(unsigned int) Type[2].iUid, szName);
		if (SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf)) < 0)
		{
			Proc.Close();
			User::Free(pszBuf);
			return -1;
		}

		Proc.Close();
	}
	User::Free(pszBuf);

	return 0;
}

static int Do_lsthr(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	Lex.SkipSpace();

	TInt Uid;

	if (Lex.Val(Uid) != KErrNone)
		Uid = -1;

	char const *pszHdr = " TID  PID  FLG  PRI   HSIZ  SSIZ   PH   TH NAME\n";
	TFindThread FT;
	TFullName FName;
	char *pszBuf;
	char szName[256];

	if (SendPkt(pCE, (TUint8 const *) pszHdr, strlen(pszHdr)) < 0)
		return -1;

	if ((pszBuf = (char *) User::Alloc(QC_BUFF_SIZE)) == NULL)
		return 1;

	while (FT.Next(FName) == KErrNone)
	{
		RThread Thrd;
		char szFlags[16] = "";

		if (Thrd.Open(FT) != KErrNone)
			continue;

		unsigned int uPid = 0, uTid = (unsigned int) Thrd.Id();
		RProcess Proc;

		if (Thrd.Process(Proc) == KErrNone)
		{
#ifndef SYMBIAN9
			if (!Proc.LoadedFromRam())
				strcat(szFlags, "R");
#endif
			uPid = (unsigned int) Proc.Id();
			Proc.Close();
			if (Uid >= 0 && uPid != (unsigned int) Uid)
			{
				Thrd.Close();
				continue;
			}
		}
#ifndef SYMBIAN9
		if (Thrd.Protected())
			strcat(szFlags, "P");
#endif

		TInt iHeapSize = 0, iStackSize = 0;

#ifndef SYMBIAN9
		Thrd.GetRamSizes(iHeapSize, iStackSize);
#endif

		TInt iProcH = 0, iThreadH = 0;

		Thrd.HandleCount(iProcH, iThreadH);

		ConvU2C(szName, sizeof(szName) - 1, TPtrC16(FName).Ptr(),
			TPtrC16(FName).Length());

		sprintf(pszBuf, "%4u %4u %4s %4d %4dKb %3dKb %4d %4d %s\n", uTid, uPid,
			szFlags, Thrd.Priority(), iHeapSize / 1024, iStackSize / 1024,
			iProcH, iThreadH, szName);
		if (SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf)) < 0)
		{
			Thrd.Close();
			User::Free(pszBuf);
			return -1;
		}

		Thrd.Close();
	}
	User::Free(pszBuf);

	return 0;
}

static int Do_lssem(QConsEnv *pCE, TUint8 *pData, int iSize)
{
#ifndef SYMBIAN9
	char const *pszHdr = " CNT NAME\n";
#else
	char const *pszHdr = "NAME\n";
#endif
	TFindSemaphore FS;
	TFullName FName;
	char *pszBuf;
	char szName[256];

	if (SendPkt(pCE, (TUint8 const *) pszHdr, strlen(pszHdr)) < 0)
		return -1;

	if ((pszBuf = (char *) User::Alloc(QC_BUFF_SIZE)) == NULL)
		return 1;

	while (FS.Next(FName) == KErrNone)
	{
		RSemaphore Sem;

		if (Sem.Open(FS) != KErrNone)
			continue;

		ConvU2C(szName, sizeof(szName) - 1, TPtrC16(FName).Ptr(),
			TPtrC16(FName).Length());

#ifndef SYMBIAN9
		sprintf(pszBuf, "%4d %s\n", Sem.Count(), szName);
#else
		sprintf(pszBuf, "%s\n", szName);
#endif
		if (SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf)) < 0)
		{
			Sem.Close();
			User::Free(pszBuf);
			return -1;
		}

		Sem.Close();
	}
	User::Free(pszBuf);

	return 0;
}

static int Do_lsrchk(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	char const *pszHdr = "    CSIZ     MSIZ  FLG    MADDR NAME\n";
	TFindChunk FC;
	TFullName FName;
	char *pszBuf;
	char szName[256];

	if (SendPkt(pCE, (TUint8 const *) pszHdr, strlen(pszHdr)) < 0)
		return -1;

	if ((pszBuf = (char *) User::Alloc(QC_BUFF_SIZE)) == NULL)
		return 1;

	while (FC.Next(FName) == KErrNone)
	{
		RChunk Chk;

		if (Chk.Open(FC) != KErrNone)
			continue;

		char szFlags[16] = "";

		if (Chk.IsReadable())
			strcat(szFlags, "R");
		if (Chk.IsWritable())
			strcat(szFlags, "W");

		ConvU2C(szName, sizeof(szName) - 1, TPtrC16(FName).Ptr(),
			TPtrC16(FName).Length());

		sprintf(pszBuf, "%6dKb %6dKb %4s %08x %s\n", Chk.Size() / 1024, Chk.MaxSize() / 1024,
			szFlags, (unsigned int) Chk.Base(), szName);
		if (SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf)) < 0)
		{
			Chk.Close();
			User::Free(pszBuf);
			return -1;
		}

		Chk.Close();
	}
	User::Free(pszBuf);

	return 0;
}

static int Do_lsldd(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	char const *pszHdr = "NAME\n";
	TFindLogicalDevice FD;
	TFullName FName;
	char *pszBuf;
	char szName[256];

	if (SendPkt(pCE, (TUint8 const *) pszHdr, strlen(pszHdr)) < 0)
		return -1;

	if ((pszBuf = (char *) User::Alloc(QC_BUFF_SIZE)) == NULL)
		return 1;

	while (FD.Next(FName) == KErrNone)
	{
		ConvU2C(szName, sizeof(szName) - 1, TPtrC16(FName).Ptr(),
			TPtrC16(FName).Length());

		sprintf(pszBuf, "%s\n", szName);
		if (SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf)) < 0)
		{
			User::Free(pszBuf);
			return -1;
		}
	}
	User::Free(pszBuf);

	return 0;
}

static int Do_lspdd(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	char const *pszHdr = "NAME\n";
	TFindPhysicalDevice FD;
	TFullName FName;
	char *pszBuf;
	char szName[256];

	if (SendPkt(pCE, (TUint8 const *) pszHdr, strlen(pszHdr)) < 0)
		return -1;

	if ((pszBuf = (char *) User::Alloc(QC_BUFF_SIZE)) == NULL)
		return 1;

	while (FD.Next(FName) == KErrNone)
	{
		ConvU2C(szName, sizeof(szName) - 1, TPtrC16(FName).Ptr(),
			TPtrC16(FName).Length());

		sprintf(pszBuf, "%s\n", szName);
		if (SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf)) < 0)
		{
			User::Free(pszBuf);
			return -1;
		}
	}
	User::Free(pszBuf);

	return 0;
}

static int Do_lsdll(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	char const *pszHdr = "NAME                          UID2     UID3 FILENAME\n";
	TFindLibrary FL;
	TFullName FName;
	char *pszBuf;
	char szName[128], szFileName[256];

	if (SendPkt(pCE, (TUint8 const *) pszHdr, strlen(pszHdr)) < 0)
		return -1;

	if ((pszBuf = (char *) User::Alloc(QC_BUFF_SIZE)) == NULL)
		return 1;

	while (FL.Next(FName) == KErrNone)
	{
		TInt iSqb = FName.Locate((TChar) '[');
		RLibrary Lib;
		TUidType Type;

		strcpy(szFileName, "");
		if (Lib.Load(TPtrC(FName.Ptr(), iSqb)) == KErrNone)
		{
			TFileName FileName = Lib.FileName();

			ConvU2C(szFileName, sizeof(szFileName) - 1, TPtrC16(FileName).Ptr(),
				TPtrC16(FileName).Length());

			Type = Lib.Type();
			Lib.Close();
		}

		ConvU2C(szName, sizeof(szName) - 1, TPtrC16(FName).Ptr(),
			TPtrC16(FName).Length());

		sprintf(pszBuf, "%-25s %08x:%08x %s\n", szName, (unsigned int) Type[1].iUid,
			(unsigned int) Type[2].iUid, szFileName);
		if (SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf)) < 0)
		{
			User::Free(pszBuf);
			return -1;
		}
	}
	User::Free(pszBuf);

	return 0;
}

static int Do_kill(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	Lex.SkipSpace();

	TInt Uid;
	RProcess Proc;

	if (Lex.Val(Uid) != KErrNone || Proc.Open(Uid) != KErrNone)
	{
		int iResult = SendPkt(pCE, (TUint8 const *) "Invalid PID\n",
				      STRSIZE("Invalid PID\n"));
		return iResult < 0 ? iResult: 1;
	}

#ifndef SYMBIAN9
	Proc.SetProtected(EFalse);
#endif
	Proc.Kill(0);

	Proc.Close();

	return 0;
}

static int Do_pinf(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	Lex.SkipSpace();

	TInt Uid;
	RProcess Proc;

	if (Lex.Val(Uid) != KErrNone || Proc.Open(Uid) != KErrNone)
	{
		int iResult = SendPkt(pCE, (TUint8 const *) "Invalid PID\n",
				      STRSIZE("Invalid PID\n"));
		return iResult < 0 ? iResult: 1;
	}

	char *pszBuf = (char *) User::Alloc(QC_BUFF_SIZE);

	if (pszBuf == NULL)
	{
		Proc.Close();
		return -1;
	}

	int iLength;
	TFileName FName = Proc.FileName();

	strcpy(pszBuf, "Binary File     : ");
	iLength = strlen(pszBuf);
	ConvU2C(pszBuf + iLength, QC_BUFF_SIZE - iLength, FName.PtrZ(), FName.Length());
	strcat(pszBuf, "\n");

	if (SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf)) < 0)
	{
		User::Free(pszBuf);
		Proc.Close();
		return -1;
	}

#ifdef SYMBIAN9
	User::Free(pszBuf);
	Proc.Close();
	return 0;
#else
	TInt iCmdlnLen = Proc.CommandLineLength();
	TUint16 *pCmdLnU = (TUint16 *) User::Alloc((iCmdlnLen + 2) * sizeof(TUint16));

	if (pCmdLnU == NULL)
	{
		User::Free(pszBuf);
		Proc.Close();
		return -1;
	}

	TPtr16 hCmdLn(pCmdLnU, iCmdlnLen + 2);

	Proc.CommandLine(hCmdLn);

	strcpy(pszBuf, "Command Line    : ");
	iLength = strlen(pszBuf);
	ConvU2C(pszBuf + iLength, QC_BUFF_SIZE - iLength, pCmdLnU, iCmdlnLen);
	strcat(pszBuf, "\n");

	User::Free(pCmdLnU);

	if (SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf)) < 0)
	{
		User::Free(pszBuf);
		Proc.Close();
		return -1;
	}

	TProcessMemoryInfo MInf;

	if (Proc.GetMemoryInfo(MInf) != KErrNone)
	{
		User::Free(pszBuf);
		Proc.Close();

		int iResult = SendPkt(pCE, (TUint8 const *) "Unable to get memory info\n",
				      STRSIZE("Unable to get memory info\n"));
		return iResult < 0 ? iResult: 1;
	}

	unsigned int uPPid = (unsigned int) Proc.Id();
	RProcess PProc;

	if (Proc.Owner(PProc) == KErrNone)
	{
		uPPid = (unsigned int) PProc.Id();
		PProc.Close();
	}

	sprintf(pszBuf,
		"Parent          : %u\n"
		"Priority        : %d\n"
		"Loaded From     : %s\n"
		"Protected       : %s\n"
		"Text Base       : 0x%08lx\n"
		"Text Size       : %ld\n"
		"RoData Base     : 0x%08lx\n"
		"RoData Size     : %ld\n"
		"Bss Base        : 0x%08lx\n"
		"Bss Size        : %ld\n"
		"Data Base       : 0x%08lx\n"
		"Data Size       : %ld\n",
		uPPid, Proc.Priority(), Proc.LoadedFromRam() ? "RAM": "ROM",
		Proc.Protected() ? "Yes": "No", MInf.iCodeBase, MInf.iCodeSize,
		MInf.iConstDataBase, MInf.iConstDataSize, MInf.iUninitialisedDataBase,
		MInf.iUninitialisedDataSize, MInf.iInitialisedDataBase, MInf.iInitialisedDataSize);

	if (SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf)) < 0)
	{
		User::Free(pszBuf);
		Proc.Close();
		return -1;
	}

	User::Free(pszBuf);
	Proc.Close();

	return 0;
#endif
}

#ifndef SYMBIAN9

static int Do_prio(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	Lex.SkipSpace();

	TInt Uid;

	if (Lex.Val(Uid) != KErrNone)
	{
		int iResult = SendPkt(pCE, (TUint8 const *) "Invalid PID\n",
				      STRSIZE("Invalid PID\n"));
		return iResult < 0 ? iResult: 1;
	}
	Lex.SkipCharacters();

	TPtrC8 Prio = Lex.NextToken();

	if (Prio.Length() < 1)
	{
		int iResult = SendPkt(pCE, (TUint8 const *) "Invalid PRIO\n",
				      STRSIZE("Invalid PRIO\n"));
		return iResult < 0 ? iResult: 1;
	}

	TProcessPriority KPrio;

	switch (Prio.Ptr()[0] & 0xdf)
	{
	case 'L':
		KPrio = EPriorityLow;
		break;
	case 'B':
		KPrio = EPriorityBackground;
		break;
	case 'F':
		KPrio = EPriorityForeground;
		break;
	case 'H':
		KPrio = EPriorityHigh;
		break;
	default:
		{
			int iResult = SendPkt(pCE, (TUint8 const *) "Invalid PRIO\n",
					      STRSIZE("Invalid PRIO\n"));
			return iResult < 0 ? iResult: 1;
		}
	}

	RProcess Proc;

	if (Proc.Open(Uid) != KErrNone)
	{
		int iResult = SendPkt(pCE, (TUint8 const *) "Invalid PID\n",
				      STRSIZE("Invalid PID\n"));
		return iResult < 0 ? iResult: 1;
	}

	Proc.SetPriority(KPrio);

	Proc.Close();

	return 0;
}

#endif

#ifndef SYMBIAN9

static int Do_prot(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	Lex.SkipSpace();

	TInt Uid;

	if (Lex.Val(Uid) != KErrNone)
	{
		int iResult = SendPkt(pCE, (TUint8 const *) "Invalid PID\n",
				      STRSIZE("Invalid PID\n"));
		return iResult < 0 ? iResult: 1;
	}
	Lex.SkipCharacters();

	TPtrC8 Prio = Lex.NextToken();

	if (Prio.Length() < 1)
	{
		int iResult = SendPkt(pCE, (TUint8 const *) "Invalid PRIO\n",
				      STRSIZE("Invalid PRIO\n"));
		return iResult < 0 ? iResult: 1;
	}

	TBool bProt;

	switch (Prio.Ptr()[0] & 0xdf)
	{
	case 'P':
		bProt = ETrue;
		break;
	case 'U':
		bProt = EFalse;
		break;
	default:
		{
			int iResult = SendPkt(pCE, (TUint8 const *) "Invalid PROT\n",
					      STRSIZE("Invalid PROT\n"));
			return iResult < 0 ? iResult: 1;
		}
	}

	RProcess Proc;

	if (Proc.Open(Uid) != KErrNone)
	{
		int iResult = SendPkt(pCE, (TUint8 const *) "Invalid PID\n",
				      STRSIZE("Invalid PID\n"));
		return iResult < 0 ? iResult: 1;
	}

	Proc.SetProtected(bProt);

	Proc.Close();

	return 0;
}

#endif

#ifndef SYMBIAN9

static int Do_pren(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	Lex.SkipSpace();

	TInt Uid;

	if (Lex.Val(Uid) != KErrNone)
	{
		int iResult = SendPkt(pCE, (TUint8 const *) "Invalid PID\n",
				      STRSIZE("Invalid PID\n"));
		return iResult < 0 ? iResult: 1;
	}
	Lex.SkipCharacters();

	TPtrC8 PName = Lex.NextToken();

	if (PName.Length() < 1)
	{
		int iResult = SendPkt(pCE, (TUint8 const *) "Invalid name\n",
				      STRSIZE("Invalid name\n"));
		return iResult < 0 ? iResult: 1;
	}

	TUint16 *pPName = DupConvC2U((char *) PName.Ptr(), PName.Length());

	if (pPName == NULL)
		return -1;

	RProcess Proc;

	if (Proc.Open(Uid) != KErrNone)
	{
		User::Free(pPName);

		int iResult = SendPkt(pCE, (TUint8 const *) "Invalid PID\n",
				      STRSIZE("Invalid PID\n"));
		return iResult < 0 ? iResult: 1;
	}

	if (Proc.Rename(TPtrC(pPName)) != KErrNone)
	{
		User::Free(pPName);
		Proc.Close();

		int iResult = SendPkt(pCE, (TUint8 const *) "Failed to rename the process\n",
				      STRSIZE("Failed to rename the process\n"));
		return iResult < 0 ? iResult: 1;
	}

	User::Free(pPName);
	Proc.Close();

	return 0;
}

#endif

static int Do_run(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TUint16 *pBinF, *pCmdL;
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	Lex.SkipSpace();
	Lex.Mark();

	TPtrC8 BinF = Lex.NextToken();
	TInt iAppExt = BinF.FindC(TPtrC8((TUint8 const *) ".app"));

	if (iAppExt != KErrNotFound && iAppExt == BinF.Length() - 4)
	{
		TPtrC8 CmdLn = Lex.RemainderFromMark();

		if (!(pBinF = DupConvC2U("apprun.exe", -1)))
			return -1;

		if (!(pCmdL = DupConvC2U((char const *) CmdLn.Ptr(), CmdLn.Length())))
		{
			User::Free(pBinF);
			return -1;
		}
	}
	else
	{
		Lex.SkipCharacters();
		Lex.SkipSpace();

		TPtrC8 CmdL = Lex.Remainder();

		if (!(pBinF = DupConvC2U((char const *) BinF.Ptr(), BinF.Length())))
			return -1;

		if (!(pCmdL = DupConvC2U((char const *) CmdL.Ptr(), CmdL.Length())))
		{
			User::Free(pBinF);
			return -1;
		}
	}

	RProcess Proc;

	if (Proc.Create(TPtrC(pBinF), TPtrC(pCmdL)) != KErrNone)
	{
		User::Free(pBinF);
		User::Free(pCmdL);

		DebugOutput(pCE, 2, "Invalid binary\n");

		int iResult = SendPkt(pCE, (TUint8 const *) "Invalid binary\n",
				      STRSIZE("Invalid binary\n"));
		return iResult < 0 ? iResult: 1;
	}

	Proc.SetPriority(EPriorityLow);
	Proc.Resume();
	Proc.Close();

	User::Free(pBinF);
	User::Free(pCmdL);

	return 0;
}

static int Do_rundoc(QConsEnv *pCE, TUint8 *pData, int iSize)
{
#ifndef UIQ
	RApaLsSession lsSess;
	TThreadId threadId;
	TUint16 *pBinF;
	TLex8 Lex(TPtrC8(pData, iSize));
	TInt res;

	Lex.SkipCharacters();
	Lex.SkipSpace();
	Lex.Mark();

	TPtrC8 BinF = Lex.NextToken();

	if (lsSess.Connect() != KErrNone)
		return -1;

	if (lsSess.GetAllApps() != KErrNone)
	{
		lsSess.Close();
		return -1;
	}

	if (!(pBinF = DupConvC2U((char const *) BinF.Ptr(), BinF.Length())))
	{
		lsSess.Close();
		return -1;
	}

	res = lsSess.StartDocument(TPtrC(pBinF), threadId);

	User::Free(pBinF);
	lsSess.Close();

	if (res != KErrNone)
	{
		DebugOutput(pCE, 2, "Invalid document or no handler\n");

		int iResult = SendPkt(pCE, (TUint8 const *) "Invalid document or no handler\n",
				      STRSIZE("Invalid document or no handler\n"));
		return iResult < 0 ? iResult: 1;
	}
#else
	TLex8 Lex(TPtrC8(pData, iSize));
	TFileName fname;

	Lex.SkipCharacters();
	Lex.SkipSpace();
	Lex.Mark();

	TPtrC8 BinF = Lex.NextToken();
	fname.Copy(BinF);

	// QikFileUtils::StartAppL() crashes if we specify nonexistant file, so we must check if it's there
	RFile File;

	if (File.Open(pCE->FS, fname, EFileRead) != KErrNone)
	{
		DebugOutput(pCE, 2, "Can't access file.\n");

		int iResult = SendPkt(pCE, (TUint8 const *) "Can't access file.\n",
				      STRSIZE("Can't access file.\n"));
		return iResult < 0 ? iResult: 1;
	}
	File.Close();

	QikFileUtils::StartAppL(fname);
#endif
	return 0;
}

static int Do_drives(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TDriveList DL;

	if (pCE->FS.DriveList(DL) != KErrNone)
	{
		int iResult = SendPkt(pCE, (TUint8 const *) "Unable to list drives\n",
				      STRSIZE("Unable to list drives\n"));
		return iResult < 0 ? iResult: 1;
	}

	char const *pszHdr = "DR    FST     SIZE     FREE\n";

	if (SendPkt(pCE, (TUint8 const *) pszHdr, strlen(pszHdr)) < 0)
		return -1;

	char szOut[256];

	for (int i = 0; i < DL.Length(); i++)
	{
		if (!DL.Ptr()[i])
			continue;

		TBuf16<32> FsType;
		char szFs[32] = "?";

		if (pCE->FS.FileSystemName(FsType, i) == KErrNone)
			ConvU2C(szFs, sizeof(szFs) - 1, FsType.Ptr(), FsType.Length());

		TInt iKbSize = 0, iKbFree = 0;
		TVolumeInfo VI;

		if (pCE->FS.Volume(VI, i) == KErrNone)
		{
			TInt64 Val;

#ifdef SYMBIAN9
			Val = VI.iSize;
			Val /= 1024;
			iKbSize = I64LOW(Val);
			Val = VI.iFree;
			Val /= 1024;
			iKbFree = I64LOW(Val);
#else
			TInt64 Rem;
			Val = VI.iSize;
			Val.DivMod(1024, Rem);
			iKbSize = Val.Low();
			Val = VI.iFree;
			Val.DivMod(1024, Rem);
			iKbFree = Val.Low();
#endif
		}

		sprintf(szOut, "%c: %6s %6dKb %6dKb\n", 'A' + i, szFs, iKbSize, iKbFree);
		if (SendPkt(pCE, (TUint8 const *) szOut, strlen(szOut)) < 0)
			return -1;
	}

	return 0;
}

static int Do_mkdir(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	TPtrC8 Dir = Lex.NextToken();

	char *pszBuf = (char *) User::Alloc(QC_BUFF_SIZE);

	if (pszBuf == NULL)
		return 1;

	int iLength = MakePath(pszBuf, pCE->szCurPath, Dir);

	if (iLength > 0 && pszBuf[iLength - 1] != '\\')
		strcpy(pszBuf + iLength, "\\"), iLength++;

	TUint16 *pDir = DupConvC2U(pszBuf, iLength);

	if (pDir == NULL)
	{
		User::Free(pszBuf);
		return -1;
	}

	if (pCE->FS.MkDir(TPtrC(pDir)) != KErrNone)
	{
		User::Free(pDir);

		strcpy(pszBuf + iLength - 1, " : Unable to create directory\n");
		DebugOutput(pCE, 2, "%s", pszBuf);

		int iResult = SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf));
		User::Free(pszBuf);
		return iResult == 0 ? 1: iResult;
	}
	User::Free(pDir);
	User::Free(pszBuf);

	return 0;
}

static int Do_rmdir(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	TPtrC8 Dir = Lex.NextToken();

	char *pszBuf = (char *) User::Alloc(QC_BUFF_SIZE);

	if (pszBuf == NULL)
		return 1;

	int iLength = MakePath(pszBuf, pCE->szCurPath, Dir);

	if (iLength > 0 && pszBuf[iLength - 1] != '\\')
		strcpy(pszBuf + iLength, "\\"), iLength++;

	TUint16 *pDir = DupConvC2U(pszBuf, iLength);

	if (pDir == NULL)
	{
		User::Free(pszBuf);
		return -1;
	}

	if (pCE->FS.RmDir(TPtrC(pDir)) != KErrNone)
	{
		User::Free(pDir);

		strcpy(pszBuf + iLength - 1, " : Unable to remove directory\n");
		DebugOutput(pCE, 2, "%s", pszBuf);

		int iResult = SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf));
		User::Free(pszBuf);
		return iResult == 0 ? 1: iResult;
	}
	User::Free(pDir);
	User::Free(pszBuf);

	return 0;
}

static int Do_rmtree(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	TPtrC8 Dir = Lex.NextToken();

	char *pszBuf = (char *) User::Alloc(QC_BUFF_SIZE);

	if (pszBuf == NULL)
		return 1;

	int iLength = MakePath(pszBuf, pCE->szCurPath, Dir);

	if (iLength > 0 && pszBuf[iLength - 1] != '\\')
		strcpy(pszBuf + iLength, "\\"), iLength++;

	TUint16 *pDir = DupConvC2U(pszBuf, iLength);

	if (pDir == NULL)
	{
		User::Free(pszBuf);
		return -1;
	}

	TInt iResult;
	CFileMan *pCFM = NULL;

	TRAP(iResult, pCFM = CFileMan::NewL(pCE->FS));
	if (iResult != KErrNone)
		return 1;

	iResult = pCFM->RmDir(TPtrC(pDir));

	delete pCFM;

	if (iResult == KErrInUse)
	{
		User::Free(pDir);

		strcpy(pszBuf + iLength - 1, " : Unable to remove directory (in use)\n");
		DebugOutput(pCE, 2, "%s", pszBuf);

		int iResult = SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf));
		User::Free(pszBuf);
		return iResult == 0 ? 1: iResult;
	}
	else if (iResult != KErrNone)
	{
		User::Free(pDir);

		strcpy(pszBuf + iLength - 1, " : Unable to remove directory\n");
		DebugOutput(pCE, 2, "%s", pszBuf);

		int iResult = SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf));
		User::Free(pszBuf);
		return iResult == 0 ? 1: iResult;
	}
	User::Free(pDir);
	User::Free(pszBuf);

	return 0;
}

static int Do_getfile(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));
	TPtrC8 Path = Lex.NextToken();

	int iIoSize = QC_IOBUFF_SIZE;
	char *pszBuf = (char *) User::Alloc(1024 + iIoSize), *pIoBuf;

	if (pszBuf == NULL)
		return 1;
	pIoBuf = pszBuf + 1024;

	int iLength = MakePath(pszBuf, pCE->szCurPath, Path);
	TUint16 *pPath = DupConvC2U(pszBuf, iLength);

	if (pPath == NULL)
	{
		User::Free(pszBuf);
		return -1;
	}

	RFile File;

	if (File.Open(pCE->FS, TPtrC(pPath), EFileRead) != KErrNone)
	{
		User::Free(pPath);

		strcpy(pszBuf + iLength, " : Unable to open file\n");
		DebugOutput(pCE, 2, "%s", pszBuf);

		int iResult = SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf));
		User::Free(pszBuf);
		return iResult == 0 ? 1: iResult;
	}
	User::Free(pPath);

	TInt iFSize;

	File.Size(iFSize);

	TUint32 uSize = iFSize;
	TUint8 SizeBuf[8];

	PUT_LE32(uSize, SizeBuf);
	if (SendPkt(pCE, (TUint8 *) "", 0) < 0 ||
	    SendPkt(pCE, SizeBuf, 4) < 0)
	{
		File.Close();
		User::Free(pszBuf);
		return -1;
	}

	DebugOutput(pCE, 10, "File size %d\n", iFSize);

	TUint32 uSent;
	int iResult = 0;

	for (uSent = 0; uSent < uSize;)
	{
		TPtr8 hData((TUint8 *) pIoBuf, iIoSize);

		if (File.Read(hData) != KErrNone)
		{
			DebugOutput(pCE, 2, "Read error : %s\n", pszBuf);
			iResult = -1;
			break;
		}
		DebugOutput(pCE, 16, "Read %d\n", hData.Length());
		if (hData.Length() == 0)
			break;
		if (SendPkt(pCE, hData.Ptr(), hData.Length()) < 0)
		{
			iResult = -1;
			break;
		}
		uSent += hData.Length();
	}
	File.Close();

	if (uSent != uSize)
	{
		DebugOutput(pCE, 2, "Sent data mismatch : %s", pszBuf);
		iResult = -1;
	}
	User::Free(pszBuf);

	return iResult;
}

static int Do_getchunk(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));
	TPtrC8 Name = Lex.NextToken();

	char *pszBuf = (char *) User::Alloc(1024);

	if (pszBuf == NULL)
		return 1;

	TUint16 *pName = DupConvC2U((char const *) Name.Ptr(), Name.Length());

	if (pName == NULL)
	{
		User::Free(pszBuf);
		return -1;
	}

	RChunk Chk;

	if (Chk.OpenGlobal(TPtrC(pName), ETrue) != KErrNone)
	{
		User::Free(pName);

		sprintf(pszBuf, "%s Unable to open chunk\n", pData);
		DebugOutput(pCE, 2, "%s", pszBuf);

		int iResult = SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf));
		User::Free(pszBuf);
		return iResult == 0 ? 1: iResult;
	}
	User::Free(pName);
	User::Free(pszBuf);

	TUint32 uSize = Chk.Size();
	TUint8 SizeBuf[8];

	PUT_LE32(uSize, SizeBuf);
	if (SendPkt(pCE, (TUint8 *) "", 0) < 0 ||
	    SendPkt(pCE, SizeBuf, 4) < 0)
	{
		Chk.Close();
		return -1;
	}

	DebugOutput(pCE, 10, "Chunk size %d\n", Chk.Size());

	TUint32 uSent, uCurr;
	TUint8 *pChkData = (TUint8 *) Chk.Base();

	for (uSent = 0; uSent < uSize;)
	{
		uCurr = (uSize - uSent) < QC_BUFF_SIZE ? uSize - uSent: QC_BUFF_SIZE;
		if (SendPkt(pCE, pChkData + uSent, uCurr) < 0)
		{
			Chk.Close();
			return -1;
		}
		uSent += uCurr;
	}
	Chk.Close();

	return 0;
}

static int Do_get(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	TPtrC8 Name = Lex.NextToken(), Tok;

	Tok.Set((TUint8 const *) CHUNK_PREFIX);
	if (Name.Length() > Tok.Length() &&
	    TPtrC8(Name.Ptr(), Tok.Length()).CompareC(Tok) == 0)
		return Do_getchunk(pCE, (TUint8 *) Name.Ptr() + Tok.Length(),
				   iSize - (Name.Ptr() - pData) - Tok.Length());

	return Do_getfile(pCE, (TUint8 *) Name.Ptr(),
			  iSize - (Name.Ptr() - pData));
}

static int Do_put(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));
	TPtrC8 Cmd = Lex.NextToken();
	bool bForce = false;

	if (Cmd.CompareC(TPtrC8((TUint8 const *) "putf")) == 0)
		bForce = true;

	Lex.SkipCharacters();
	TPtrC8 Path = Lex.NextToken();

	char *pszBuf = (char *) User::Alloc(QC_BUFF_SIZE);

	if (pszBuf == NULL)
		return 1;

	int iLength = MakePath(pszBuf, pCE->szCurPath, Path);
	TUint16 *pPath = DupConvC2U(pszBuf, iLength);

	if (pPath == NULL)
	{
		User::Free(pszBuf);
		return -1;
	}
	if (bForce)
		PreparePath(&pCE->FS, pPath);

	RFile File;

	if (File.Replace(pCE->FS, TPtrC(pPath), EFileWrite) != KErrNone &&
	    File.Create(pCE->FS, TPtrC(pPath), EFileWrite) != KErrNone)
	{
		User::Free(pPath);

		strcpy(pszBuf + iLength, " : Unable to create file\n");
		DebugOutput(pCE, 2, "%s", pszBuf);

		int iResult = SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf));
		User::Free(pszBuf);
		return iResult == 0 ? 1: iResult;
	}

	int iPktSize;
	TUint8 *pPktData;

	if (SendPkt(pCE, (TUint8 *) "", 0) < 0 ||
	    RecvPkt(pCE, &pPktData, &iPktSize) < 0)
	{
		DebugOutput(pCE, 2, "Protocol error\n");
		File.Close();
		pCE->FS.Delete(TPtrC(pPath));
		User::Free(pPath);
		User::Free(pszBuf);
		return -1;
	}
	if (iPktSize != 4)
	{
		DebugOutput(pCE, 2, "Protocol error\n");
		FreePktData(pPktData);
		File.Close();
		pCE->FS.Delete(TPtrC(pPath));
		User::Free(pPath);
		User::Free(pszBuf);
		return -1;
	}

	unsigned int uFSize;

	GET_LE32(uFSize, pPktData);
	FreePktData(pPktData);

	DebugOutput(pCE, 10, "File size %u\n", uFSize);

	unsigned int uRecvd;
	int iResult = 0;

	for (uRecvd = 0;;)
	{
		if (RecvPkt(pCE, &pPktData, &iPktSize) < 0)
		{
			DebugOutput(pCE, 2, "Protocol error\n");
			iResult = -1;
			break;
		}
		if (iPktSize == 0)
		{
			FreePktData(pPktData);
			break;
		}

		TPtr8 hData(pPktData, iPktSize, iPktSize);

		if (File.Write(hData) != KErrNone)
		{
			DebugOutput(pCE, 2, "Write error : %s\n", pszBuf);
			FreePktData(pPktData);
			iResult = -1;
			break;
		}
		FreePktData(pPktData);

		DebugOutput(pCE, 16, "Write %d\n", iPktSize);

		uRecvd += iPktSize;
	}
	File.Close();

	if (uRecvd != uFSize)
	{
		DebugOutput(pCE, 2, "Received data mismatch : %s", pszBuf);
		pCE->FS.Delete(TPtrC(pPath));
		iResult = -1;
	}
	User::Free(pPath);
	User::Free(pszBuf);

	return iResult;
}

static int Do_rm(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	TPtrC8 Path = Lex.NextToken();

	char *pszBuf = (char *) User::Alloc(QC_BUFF_SIZE);

	if (pszBuf == NULL)
		return 1;

	int iLength = MakePath(pszBuf, pCE->szCurPath, Path);
	TUint16 *pPath = DupConvC2U(pszBuf, iLength);

	if (pPath == NULL)
	{
		User::Free(pszBuf);
		return -1;
	}

	pCE->FS.SetAtt(TPtrC(pPath), 0, KEntryAttReadOnly);

	if (pCE->FS.Delete(TPtrC(pPath)) != KErrNone)
	{
		User::Free(pPath);

		strcpy(pszBuf + iLength, " : Unable to remove file\n");
		DebugOutput(pCE, 2, "%s", pszBuf);

		int iResult = SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf));
		User::Free(pszBuf);
		return iResult == 0 ? 1: iResult;
	}
	User::Free(pPath);
	User::Free(pszBuf);

	return 0;
}

static int DEntryPrint(QConsEnv *pCE, TEntry const *pDEnt, TDesC16 const *pFName,
		       void *pPrivate)
{
	char *pszBuf = (char *) pPrivate;

	GetLsString(*pDEnt, pFName, pszBuf);
	if (SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf)) < 0)
		return -1;

	return 0;
}

static int DEntryPrintName(QConsEnv *pCE, TEntry const *pDEnt, TDesC16 const *pFName,
			   void *pPrivate)
{
	char *pszBuf = (char *) pPrivate;

	ConvU2C(pszBuf, QC_BUFF_SIZE, pFName->Ptr(), pFName->Length());
	strcat(pszBuf, "\n");
	if (SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf)) < 0)
		return -1;

	return 0;
}

static int DoFind(QConsEnv *pCE, const TDesC &hDir, const TDesC &hMatch, bool bRecurse,
		  int (*pFProc)(QConsEnv *, TEntry const *, TDesC16 const *, void *),
		  void *pPrivate)
{
	int iError, iDirLength;
	RDir Dir;
	TEntry *pDEnt;
	TUint16 *pPath;

	if ((iError = Dir.Open(pCE->FS, hDir, KEntryAttNormal | KEntryAttDir |
			       KEntryAttArchive | KEntryAttHidden | KEntryAttSystem |
			       KEntryAttReadOnly)) != KErrNone)
		return iError;
	if ((pDEnt = new TEntry()) == NULL)
	{
		Dir.Close();
		return KErrNoMemory;
	}
	if ((pPath = (TUint16 *) User::Alloc(512 * sizeof(TUint16))) == NULL)
	{
		delete pDEnt;
		Dir.Close();
		return KErrNoMemory;
	}
	iDirLength = hDir.Length();
	Mem::Copy(pPath, hDir.Ptr(), iDirLength * sizeof(TUint16));
	for (;;)
	{
		iError = Dir.Read(*pDEnt);
		if (iError != KErrNone)
		{
			if (iError != KErrEof)
			{
				Dir.Close();
				delete pDEnt;
				User::Free(pPath);
				return KErrGeneral;
			}
			break;
		}
		Mem::Copy(pPath + iDirLength, pDEnt->iName.Ptr(),
			  pDEnt->iName.Length() * sizeof(TUint16));
		pPath[iDirLength + pDEnt->iName.Length()] = 0;
		if (pDEnt->iAtt & KEntryAttDir)
		{
			if (bRecurse)
			{
				pPath[iDirLength + pDEnt->iName.Length()] = (TUint16) '\\';
				pPath[iDirLength + pDEnt->iName.Length() + 1] = 0;

				if ((iError = DoFind(pCE, TPtrC(pPath), hMatch, true,
						     pFProc, pPrivate)) != KErrNone)
				{
					Dir.Close();
					delete pDEnt;
					User::Free(pPath);
					return iError;
				}
			}
		}
		else
		{
			TPtrC hPath(pPath);

			if (pDEnt->iName.MatchC(hMatch) != KErrNotFound &&
			    (*pFProc)(pCE, pDEnt, &hPath, pPrivate) < 0)
			{
				Dir.Close();
				delete pDEnt;
				User::Free(pPath);
				return KErrGeneral;
			}
		}
	}
	User::Free(pPath);
	delete pDEnt;
	Dir.Close();

	return 0;
}

static int Do_find(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	bool bRecurse = true, bSimple = false;
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	TPtrC8 Path = Lex.NextToken();

	if (Path[0] == '-')
	{
		int i;

		for (i = 1; i < Path.Length(); i++)
			switch (Path[i])
			{
			case '1':
				bRecurse = false;
				break;
			case 's':
				bSimple = true;
				break;
			}
		Lex.SkipCharacters();
		Path.Set(Lex.NextToken());
	}

	Lex.SkipCharacters();
	TPtrC8 Match = Lex.NextToken();

	char *pszBuf = (char *) User::Alloc(QC_BUFF_SIZE);

	if (pszBuf == NULL)
		return 1;

	int iPLength = MakePath(pszBuf, pCE->szCurPath, Path);
	TUint16 *pPath = DupConvC2U(pszBuf, iPLength, 2);

	if (pPath == NULL)
	{
		User::Free(pszBuf);
		return -1;
	}

	TPtr hPath(pPath, iPLength, iPLength + 3);

	if (hPath[hPath.Length() - 1] != (TUint16) '\\')
		hPath.Append((TUint16) '\\');

	TUint16 *pMatch = DupConvC2U((char const *) Match.Ptr(), Match.Length());

	if (pMatch == NULL)
	{
		User::Free(pPath);
		User::Free(pszBuf);
		return -1;
	}

	int iResult = DoFind(pCE, hPath, TPtrC(pMatch), bRecurse,
			     bSimple ? DEntryPrintName: DEntryPrint, pszBuf);

	User::Free(pPath);
	User::Free(pMatch);
	User::Free(pszBuf);

	return iResult;
}

static int Do_mv(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	TPtrC8 SPath = Lex.NextToken();
	Lex.SkipCharacters();
	TPtrC8 DPath = Lex.NextToken();

	char *pszBuf = (char *) User::Alloc(QC_BUFF_SIZE), *pszSPath, *pszDPath;

	if (pszBuf == NULL)
		return 1;
	pszSPath = pszBuf;
	pszDPath = pszBuf + 1024;

	int iSLength = MakePath(pszSPath, pCE->szCurPath, SPath);
	int iDLength = MakePath(pszDPath, pCE->szCurPath, DPath);
	TUint16 *pSPath = DupConvC2U(pszSPath, iSLength);

	if (pSPath == NULL)
	{
		User::Free(pszBuf);
		return -1;
	}

	TUint16 *pDPath = DupConvC2U(pszDPath, iDLength);

	if (pDPath == NULL)
	{
		User::Free(pSPath);
		User::Free(pszBuf);
		return -1;
	}

	if (pCE->FS.Rename(TPtrC(pSPath), TPtrC(pDPath)) != KErrNone)
	{
		User::Free(pDPath);
		User::Free(pSPath);

		strcpy(pszSPath + iSLength, " : Unable to rename file\n");
		DebugOutput(pCE, 2, "%s", pszSPath);

		int iResult = SendPkt(pCE, (TUint8 const *) pszSPath, strlen(pszSPath));
		User::Free(pszBuf);
		return iResult == 0 ? 1: iResult;
	}
	User::Free(pDPath);
	User::Free(pSPath);
	User::Free(pszBuf);

	return 0;
}

static int Do_cp(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	TPtrC8 SPath = Lex.NextToken();
	Lex.SkipCharacters();
	TPtrC8 DPath = Lex.NextToken();

	char *pszBuf = (char *) User::Alloc(QC_BUFF_SIZE), *pszSPath, *pszDPath;

	if (pszBuf == NULL)
		return 1;
	pszSPath = pszBuf;
	pszDPath = pszBuf + 1024;

	int iSLength = MakePath(pszSPath, pCE->szCurPath, SPath);
	int iDLength = MakePath(pszDPath, pCE->szCurPath, DPath);
	TUint16 *pSPath = DupConvC2U(pszSPath, iSLength);

	if (pSPath == NULL)
	{
		User::Free(pszBuf);
		return -1;
	}

	TUint16 *pDPath = DupConvC2U(pszDPath, iDLength);

	if (pDPath == NULL)
	{
		User::Free(pSPath);
		User::Free(pszBuf);
		return -1;
	}

	TInt iResult;
	CFileMan *pCFM = NULL;

	TRAP(iResult, pCFM = CFileMan::NewL(pCE->FS));
	if (iResult != KErrNone)
	{
		User::Free(pDPath);
		User::Free(pSPath);
		User::Free(pszBuf);
		return 1;
	}

	iResult = pCFM->Copy(TPtrC(pSPath), TPtrC(pDPath),
			     CFileMan::EOverWrite | CFileMan::ERecurse);

	delete pCFM;
	User::Free(pDPath);
	User::Free(pSPath);

	if (iResult != KErrNone)
	{
		strcpy(pszSPath + iSLength, " : Unable to copy file\n");
		DebugOutput(pCE, 2, "%s", pszSPath);

		int iResult = SendPkt(pCE, (TUint8 const *) pszSPath, strlen(pszSPath));
		User::Free(pszBuf);
		return iResult == 0 ? 1: iResult;
	}
	User::Free(pszBuf);

	return 0;
}

static int Do_chmod(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	TLex8 Lex(TPtrC8(pData, iSize));

	Lex.SkipCharacters();
	TPtrC8 Mode = Lex.NextToken();
	Lex.SkipCharacters();
	TPtrC8 Path = Lex.NextToken();

	TBool bAddMode = ETrue;
	TUint uAddMask = 0, uClearMask = 0;

	for (int i = 0; i < Mode.Length(); i++)
	{
		switch (Mode.Ptr()[i])
		{
		case '+':
			bAddMode = ETrue;
			break;
		case '-':
			bAddMode = EFalse;
			break;
		case 'h':
			if (bAddMode)
				uAddMask |= KEntryAttHidden;
			else
				uClearMask |= KEntryAttHidden;
			break;
		case 's':
			if (bAddMode)
				uAddMask |= KEntryAttSystem;
			else
				uClearMask |= KEntryAttSystem;
			break;
		case 'w':
			if (bAddMode)
				uClearMask |= KEntryAttReadOnly;
			else
				uAddMask |= KEntryAttReadOnly;
			break;
		default:
			{
				int iResult = SendPkt(pCE, (TUint8 const *) "Invalid mode\n",
						      STRSIZE("Invalid mode\n"));
				return iResult < 0 ? iResult: 1;
			}
		}
	}

	char *pszBuf = (char *) User::Alloc(QC_BUFF_SIZE);

	if (pszBuf == NULL)
		return 1;

	int iLength = MakePath(pszBuf, pCE->szCurPath, Path);
	TUint16 *pPath = DupConvC2U(pszBuf, iLength);

	if (pPath == NULL)
	{
		User::Free(pszBuf);
		return -1;
	}

	if (pCE->FS.SetAtt(TPtrC(pPath), uAddMask, uClearMask) != KErrNone)
	{
		User::Free(pPath);

		strcpy(pszBuf + iLength, " : Unable to change file mode\n");
		DebugOutput(pCE, 2, "%s", pszBuf);

		int iResult = SendPkt(pCE, (TUint8 const *) pszBuf, strlen(pszBuf));
		User::Free(pszBuf);
		return iResult == 0 ? 1: iResult;
	}

	User::Free(pPath);
	User::Free(pszBuf);

	return 0;
}

int ProcessInput(QConsEnv *pCE, TUint8 *pData, int iSize)
{
	int iResult = 0;

	DebugOutput(pCE, 10, "GOT %d bytes: '%s'\n", iSize, pData);

	for (; iSize > 0 && strchr("\r\n \t", pData[iSize - 1]); iSize--);

	if (ISCMD(pData, iSize, "pwd"))
		iResult = Do_pwd(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "cd"))
		iResult = Do_cd(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "ls"))
		iResult = Do_ls(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "drives"))
		iResult = Do_drives(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "ps"))
		iResult = Do_ps(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "kill"))
		iResult = Do_kill(pCE, pData, iSize);
#ifndef SYMBIAN9
	else if (ISCMD(pData, iSize, "prio"))
		iResult = Do_prio(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "prot"))
		iResult = Do_prot(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "pren"))
		iResult = Do_pren(pCE, pData, iSize);
#endif
	else if (ISCMD(pData, iSize, "pinf"))
		iResult = Do_pinf(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "run"))
		iResult = Do_run(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "rundoc"))
		iResult = Do_rundoc(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "mkdir"))
		iResult = Do_mkdir(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "rmdir"))
		iResult = Do_rmdir(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "rmtree"))
		iResult = Do_rmtree(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "rm"))
		iResult = Do_rm(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "mv"))
		iResult = Do_mv(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "cp"))
		iResult = Do_cp(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "chmod"))
		iResult = Do_chmod(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "lsthr"))
		iResult = Do_lsthr(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "lssem"))
		iResult = Do_lssem(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "lsrchk"))
		iResult = Do_lsrchk(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "lsldd"))
		iResult = Do_lsldd(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "lspdd"))
		iResult = Do_lspdd(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "lsdll"))
		iResult = Do_lsdll(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "find"))
		iResult = Do_find(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "get"))
		iResult = Do_get(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "put"))
		iResult = Do_put(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "putf"))
		iResult = Do_put(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "hdump"))
		iResult = Do_hdump(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "exit"))
		iResult = Do_exit(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "shutdown"))
		iResult = Do_shutdown(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "reboot"))
		iResult = Do_reboot(pCE, pData, iSize);
	else if (ISCMD(pData, iSize, "help"))
		iResult = Do_help(pCE, pData, iSize);
	else
		SendPkt(pCE, (TUint8 const *) "Invalid command\n",
			STRSIZE("Invalid command\n"));

	SendPkt(pCE, (TUint8 const *) "", 0);

	return iResult;
}

