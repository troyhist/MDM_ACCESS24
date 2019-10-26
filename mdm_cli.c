/* =======================================================================
// MDM_CLI ver. 1.0 (pure C)
// "common medium" emulator - CLIENT PART
// -----------------------------------------------------------------------
// MD: 19.08.2003
// LE: 19.08.2003
// -----------------------------------------------------------------------
//   Compiler: MS VC 6.0
// Aplication: ANSI, DOS16, WIN32
//  Edited by: Serguei Kolevatov
// =======================================================================
// mdm_cli.c: implementation of service functions
// ======================================================================= */
#include <stdio.h>
#include <string.h>		/* strcpy (19.08.2003) */
#include <stdlib.h>		/* srand */
#include <time.h>		/* clock */
#include "mdm_access.h"
#include "mdm_cli.h"
#include "..\\TCILIB\\FUNC\\GETVAL\\strgetval.h"	/* StrGetVal (19.08.2003) */
#include "AutoBuild.h"	/* AUTO_BUILD_NUMBER (28.08.2003) */
#include "LastBackup.h"	/* LAST_BACKUP_NUMBER (19.08.2003) */
/* ======================================================================= */
/* global data */
/* ======================================================================= */
char glMCGD[256];		/* MDM SRV Global Data */
/* ======================================================================= */
/* private macros */
/* ======================================================================= */
#define pER(_n_)		(MAFRT_ERROR==_n_)?"ERR":"ok"
/* ======================================================================= */
void CMDM_CLISanity(tagCMDM_CLI* pMS, char* sFuncName);
void IntErrExit(char* sFuncName, char* sErr);
/* ======================================================================= */
/* CLI public functions (09.09.2003) */
/* ======================================================================= */
int CMDM_CLIIterate			(tagCMDM_CLI* pMS, int nCmd, int *pbActiveRx,
							 tagMDM_SAMPLES* pData)
{
/* -----------------------------------------------------------------------
// Description: performs one POLL-UPD iteration
//   Parametrs: ...
//      Return: 0 - ok, (-1) - error
//              1 - syncronized, possible data has not been sent
// -----------------------------------------------------------------------
//   MD: 09.09.2003
//   LE: 19.09.2003
// TODO: test it
// ----------------------------------------------------------------------- */
CMDM_CLISanity(pMS, "CMDM_CLIIterate(..)");
switch(nCmd)
	{
	/* ------------------------------------------------------------------- */
	/* initially triyng to syncronize ourself */
	/* ------------------------------------------------------------------- */
	case MDM_CLI_ITER_SYNC:
	if(0==pMS->m_tST.bSyncr)
		{
		do	{
			CMDM_CLIResponse (pMS);
			} while (0==pMS->m_tST.bSyncr);
		return(1);
		}
	else
		{ CMDMErrExit("CMDM_CLIIterate()",
		"CMD = SYNC, but CLI is already syncronised");}
	break;
	/* ------------------------------------------------------------------- */
	/* waiting for POLL command */
	/* ------------------------------------------------------------------- */
	case MDM_CLI_ITER_UPD_STATE:
	if(0==pMS->m_tST.bPolled)
		{
		/* --------------------------------------------------------------- */
		/* check, if CLI has data to tx */
		/* --------------------------------------------------------------- */
		if (pData)
			{
			pMS->m_tST.bActiveTx=1;
			pMS->m_tST.tMDM=*pData;
			}
		/* --------------------------------------------------------------- */
		do	{
			CMDM_CLIResponse (pMS);
			} while (0==pMS->m_tST.bPolled);
		/* --------------------------------------------------------------- */
		}
	else
		{ CMDMErrExit("CMDM_CLIIterate()", 
		"CMD = UPD_STATE, but CLI is already polled");}
	break;
	/* ------------------------------------------------------------------- */
	/* waiting for POLL command */
	/* ------------------------------------------------------------------- */
	case MDM_CLI_ITER_SKIP_POLL:
	if(0==pMS->m_tST.bPolled)
		{
		pMS->m_tST.bActiveTx=0; /* (15.09.2003) */
		/* --------------------------------------------------------------- */
		do	{
			CMDM_CLIResponse (pMS);
			} while (0==pMS->m_tST.bPolled);
		}
	else
		{ CMDMErrExit("CMDM_CLIIterate()", 
		"CMD = SKIP_POLL, but CLI is already polled");}
	break;
	/* ------------------------------------------------------------------- */
	/* waiting for UPDATE command */
	/* ------------------------------------------------------------------- */
	case MDM_CLI_ITER_GET_STATE:
	if(0==pMS->m_tST.bUpdated)
		{
		/* --------------------------------------------------------------- */
		do	{
			CMDM_CLIResponse (pMS);
			} while (0==pMS->m_tST.bUpdated);
		/* --------------------------------------------------------------- */
		/* check, if CLI has received data */
		/* --------------------------------------------------------------- */
		if(pMS->m_tST.bActiveRx)
			{
			if(pbActiveRx)*pbActiveRx=1;
			if(pData) *pData=pMS->m_tST.tMDM;
			}
		else
			{ if(pbActiveRx)*pbActiveRx=0; }	/* (19.09.2003) */
		}
	else
		{ CMDMErrExit("CMDM_CLIIterate()", 
		"CMD = GET_STATE, but CLI is already updated");}
	break;
	/* ------------------------------------------------------------------- */
	}
return(0);
}
/* ======================================================================= */
/* private functions */
/* ======================================================================= */
int	CMDM_CLIIsInited(tagCMDM_CLI* pMS)
	{ return(pMS->m_bInited); }
/* ----------------------------------------------------------------------- */
void CMDM_CLISanity(tagCMDM_CLI* pMS, char* sFuncName)
	{ if(!CMDM_CLIIsInited(pMS)) 
	IntErrExit(sFuncName, "sanity check failed"); }
/* ======================================================================= */
/* service functions */
/* ======================================================================= */
void CMDM_CLI(tagCMDM_CLI* pMS)
	{ /* constructor */ pMS->m_bInited=0; }
/* ----------------------------------------------------------------------- */
void CMDM_CLI_				(tagCMDM_CLI* pMS)
	{ /* destructor (24.09.2003) */
	if(!CMDM_CLIIsInited(pMS)) return;
	pMS->m_bInited=0; }
/* ======================================================================= */
int CMDM_CLIInit(tagCMDM_CLI* pMS, char* sCFGFile, int nCurTime)
{
/* -----------------------------------------------------------------------
// Description: inits medium client
//   Parametrs: ...
//      Return: 0 - ok, (-1) - error
// -----------------------------------------------------------------------
//   MD: 19.08.2003
//   LE: 22.08.2003
// TODO: test it
// ----------------------------------------------------------------------- */
char sBuf[4096];	// BUFFER HAS TO BE LARGE ENOUGH TO READ ALL FILE
char sFLNMS[3][256];
FILE* fp;
int nAddr;
int nClock;
time_t t;
/* ----------------------------------------------------------------------- */
CMDM_Delay(MA_MDM_CHK_INT);
time(&t); nClock=clock()+t;
/* ----------------------------------------------------------------------- */
srand(nClock*CLOCKS_PER_SEC);
nAddr=rand();			// address of the station (random no.)
/* ----------------------------------------------------------------------- */
fp=fopen(sCFGFile, "rt");
if (fp==NULL)
	CMDMErrExit("CMDM_CLIInit(..)", "cann't open cfg-file %s", sCFGFile);
/* ----------------------------------------------------------------------- */
fread(sBuf, 1, sizeof(sBuf), fp); fclose(fp);
/* ----------------------------------------------------------------------- */
#define CFG_PAREE(_n_)	CMDMErrExit("CMDM_CLIInit(..)", "parameter %s not found", _n_)
#define CFG_GV(_n_)		StrGetValue(sBuf, _n_, glMCGD)
#define CFG_PARSE(_n_)	if(!CFG_GV(_n_)) CFG_PAREE(_n_)
/* ----------------------------------------------------------------------- */
/* process MAC cfg */
CFG_PARSE("PROT_EXCH_CMDS");	else { sscanf(glMCGD, "%s", &sFLNMS[0]); }
CFG_PARSE("PROT_EXCH_ANSWRS");	else { sscanf(glMCGD, "%s", &sFLNMS[1]); }
CFG_PARSE("PROT_EXCH_FLAG");	else { sscanf(glMCGD, "%s", &sFLNMS[2]); }
/* ----------------------------------------------------------------------- */
CFG_PARSE("DEBUG_MSG");			else { sscanf(glMCGD, "%d", &pMS->m_bShowDebugMsg); }
/* ----------------------------------------------------------------------- */
/* inits internal data - CFG */
pMS->m_bInited=1;				// flag "inited"
CMDMInit(&pMS->m_tProt, nAddr, sFLNMS[2], sFLNMS[0], sFLNMS[1]);
/* ----------------------------------------------------------------------- */
/* inits STATE */
pMS->m_tST.bRegistred=0;		// flag "STA is registred"
pMS->m_tST.bSyncr=0;			// flag "STA is syncronised"
pMS->m_tST.bPolled=0;			// flag "STA is polled"
pMS->m_tST.bActiveTx=0;			// flag "STA has data to Tx"
pMS->m_tST.bActiveRx=0;			// flag "STA has data to Rx"
pMS->m_tST.nRetries=0;			// no. of retries
pMS->m_tST.nCurTime=nCurTime;	// current emulation time
pMS->m_tST.nServTime=0;			// current emulation time on SERVER
/* ----------------------------------------------------------------------- */
/* 09.09.2003 */
CMDMLog(&pMS->m_tProt, "inited (nRes=%d), nAddr=%#08x, protfile=%s", 
		0, pMS->m_tProt.m_nAddr, pMS->m_tProt.m_sSRV_CMDS);
/* ----------------------------------------------------------------------- */
return(0);
#undef CFG_PAREE
#undef CFG_GV
#undef CFG_PARSE
}
/* ======================================================================= */
/* scenarios */
/* ======================================================================= */
int CMDM_CLIAnswPoll (tagCMDM_CLI* pMS, tagMAFR* pCMD)
	{
	tagMAFR tFR_ANS;
	tagPRT_MDM* pPRM=&pMS->m_tProt;
	/* ------------------------------------------------------------------- */
	/* answers on a POLL frame (21.08.2003, 11.09.2003) */
	/* ------------------------------------------------------------------- */
	CMDMLog(pPRM, "--> POLL,\tsrv time = %d units", pCMD->tTime.m_nTimeStamp);
	/* ------------------------------------------------------------------- */
	if(0==pMS->m_tST.bUpdated)
		{ CMDMLog(pPRM, "... ignored, not UPD"); return(0); }
	if(1==pMS->m_tST.bPolled)
		{ CMDMLog(pPRM, "... ignored"); return(0); }
	/* ------------------------------------------------------------------- */
	CMDM_CreateFrame(&tFR_ANS, MA_FR_VALID, MAFRT_ACK, 
		pMS->m_tST.bActiveTx?MAINFT_MDM_ST:MAINFT_NONE,
		MA_SRV_ADDR, pMS->m_tProt.m_nAddr, pCMD->m_nFRID, 
		NULL, pMS->m_tST.bActiveTx?&pMS->m_tST.tMDM:NULL);
	CMDM_SaveMsg(pPRM, &tFR_ANS);
	CMDMLog(pPRM, "<-- ACK %s", pMS->m_tST.bActiveTx?"+ DATA":"");
	/* ------------------------------------------------------------------- */
	pMS->m_tST.bPolled=1;
	pMS->m_tST.bUpdated=0;
	/* ------------------------------------------------------------------- */
	return(0);
	}
/* ======================================================================= */
int CMDM_CLIAnswUpd	(tagCMDM_CLI* pMS, tagMAFR* pCMD)
	{
	tagMAFR tFR_ANS;
	tagPRT_MDM* pPRM=&pMS->m_tProt;
	/* ------------------------------------------------------------------- */
	/* answers on a UPD frame (21.08.2003, 15.09.2003) */
	/* ------------------------------------------------------------------- */
	CMDMLog(pPRM, "--> UPD,\tsrv time = %d units, mdm = %s", 
		pCMD->tTime.m_nTimeStamp,
		(MAINFT_TIME==pCMD->m_nInfType)?"IDLE":"BUSY");
	/* ------------------------------------------------------------------- */
	if(0==pMS->m_tST.bSyncr)
		{ CMDMLog(pPRM, "... ignored, not SYNC"); return(0); }
	if(1==pMS->m_tST.bUpdated)
		{ CMDMLog(pPRM, "... ignored"); return(0); }
	/* ------------------------------------------------------------------- */
	pMS->m_tST.nServTime=pCMD->tTime.m_nTimeStamp;	/* (15.09.2003) */
	pMS->m_tST.bActiveRx=0;	/* (11.09.2003) */
	/* ------------------------------------------------------------------- */
	/* receive new MDM state (09.09.2003) */
	if(MAINFT_MDM_ST==pCMD->m_nInfType || MAINFT_TIME_MDM_ST==pCMD->m_nInfType)
		{
		pMS->m_tST.bActiveRx=1;
		pMS->m_tST.tMDM=pCMD->tMdm.m_tMDM;
		}
	/* ------------------------------------------------------------------- */
	CMDM_CreateFrame(&tFR_ANS, MA_FR_VALID, MAFRT_ACK, MAINFT_NONE,
		MA_SRV_ADDR, pMS->m_tProt.m_nAddr, pCMD->m_nFRID, 
		NULL, NULL);
	CMDM_SaveMsg(pPRM, &tFR_ANS);
	CMDMLog(pPRM, "<-- ACK");
	/* ------------------------------------------------------------------- */
	pMS->m_tST.bUpdated=1;
	pMS->m_tST.bPolled=0;
	/* ------------------------------------------------------------------- */
	return(0);
	}
/* ======================================================================= */
int CMDM_CLIAnswSync (tagCMDM_CLI* pMS, tagMAFR* pCMD)
	{
	tagMAFR tFR_ANS;
	tagPRT_MDM* pPRM=&pMS->m_tProt;
	/* ------------------------------------------------------------------- */
	/* answers on a SYNC frame (21.08.2003, 15.09.2003) */
	/* ------------------------------------------------------------------- */
	CMDMLog(pPRM, "--> %sSYNC,\tsrv time = %d units", 
		(pMS->m_tST.bSyncr)?"re ":"", pCMD->tTime.m_nTimeStamp);
	/* ------------------------------------------------------------------- */
	CMDM_CreateFrame(&tFR_ANS, MA_FR_VALID, MAFRT_ACK, MAINFT_NONE,
		MA_SRV_ADDR, pMS->m_tProt.m_nAddr, pCMD->m_nFRID, 
		NULL, NULL);
	CMDM_SaveMsg(pPRM, &tFR_ANS);
	CMDMLog(pPRM, "<-- ACK");
	/* ------------------------------------------------------------------- */
	pMS->m_tST.nServTime=pCMD->tTime.m_nTimeStamp;	/* (15.09.2003) */
	/* ------------------------------------------------------------------- */
	pMS->m_tST.bSyncr=1;
	pMS->m_tST.bUpdated=1;
	pMS->m_tST.bPolled=0;
	/* ------------------------------------------------------------------- */
	return(0);
	}
/* ======================================================================= */
/* functionality */
/* ======================================================================= */
int CMDM_CLIRegister (tagCMDM_CLI* pMS)
{
/* -----------------------------------------------------------------------
// Description: checks incoming messages
//   Parametrs: ...
//      Return: 0 - ok, (-1) - error
// -----------------------------------------------------------------------
//   MD: 18.08.2003
//   LE: 22.08.2003
// TODO: complete it
// ----------------------------------------------------------------------- */
int nRes;
int nOccupied=0;
int nTriesMDM=0, nCheckPoll=0;
tagMAFR tFR_CMD;
tagMAFR tFR_ANS;
tagPRT_MDM* pPRM=&pMS->m_tProt;
/* ----------------------------------------------------------------------- */
CMDM_CLISanity(pMS, "CMDM_CLIRegister()");
/* ----------------------------------------------------------------------- */
CMDM_CreateFrame(&tFR_ANS, MA_FR_VALID, MAFRT_REGREQ, MAINFT_NONE,
				 MA_SRV_ADDR, pMS->m_tProt.m_nAddr, -1,
				 NULL, NULL);
/* ----------------------------------------------------------------------- */
/* waits for a broadcast frame */
CMDMLog(pPRM, "trying to access the MDM ...");
do	{
	nRes=CMDM_TryToLoadAFrame(pPRM, &tFR_CMD);
	/* ----------------------------------------------------------------------- */
	if (nRes)
		{
		if(CMDM_HaveRecvBrdcstPOLL (pPRM, &tFR_CMD))
			{
			/* ----------------------------------------------------------- */
			CMDMLog(pPRM, "--> BRDCST POLL frame");
			/* ----------------------------------------------------------- */
			nCheckPoll=0;
			/* ----------------------------------------------------------- */
			if(CMDM_IsMdmFree(pPRM))
				{
				CMDM_MdmOccupy(pPRM);
				nOccupied=1;
				break;
				}
			else
				{
				nTriesMDM++;
				CMDMLog(pPRM, "--X");
				CMDM_Delay(rand()%MA_RAND_WAIT);
				}
			}
		else
			{
			CMDMLog(pPRM, "--X not a BPOLL (nDA=%#08x, nSA=%#08x, nType=%d)",
				tFR_CMD.m_nDA, tFR_CMD.m_nSA, tFR_CMD.m_nCMD);
			}
		}
	/* ----------------------------------------------------------------------- */
	else
		{
		if(tFR_CMD.m_bValid)
		CMDMLog(pPRM, "--X not a BPOLL (nDA=%#08x, nSA=%#08x, nType=%d)",
			tFR_CMD.m_nDA, tFR_CMD.m_nSA, tFR_CMD.m_nCMD);
		else
		CMDMLog(pPRM, "--X no response from (nAddr=%#08x)\n", MA_SRV_ADDR);
		}
	/* ----------------------------------------------------------------------- */
	nCheckPoll++;
	if (MA_MAX_ACCESS_TRIES==nTriesMDM ||
		MA_MAX_W_BPOLL==nCheckPoll)
		{
		CMDMLog(pPRM, "cann't access SRV (nCheckPoll=%d, nTriesMDM=%d)",
			nCheckPoll, nTriesMDM);
		return(MAFRT_ERROR);
		}
	/* ----------------------------------------------------------------------- */
	CMDM_Delay(MA_MDM_CHK_INT);
	} while (nOccupied==0);
/* ----------------------------------------------------------------------- */
/* sends REGREQ */
CMDM_ClearCMD(pPRM);
CMDM_SaveMsg(pPRM, &tFR_ANS);
CMDMLog(pPRM, "<-- REGREQ");
/* ----------------------------------------------------------------------- */
/* waits for REG_ALWD */
if(MAFRT_ERROR==CMDM_WaitForFrameTypeFromSta(pPRM, &tFR_CMD, MAFRT_REG_ALWD,
   MA_SRV_ADDR, MA_W_REG_ALWD, 0, pMS->m_bShowDebugMsg))
	{
	if(tFR_CMD.m_bValid)
		CMDMLog(pPRM, "--X not a REG_ALWD (nDA=%#08x, nSA=%#08x, nType=%d)",
		tFR_CMD.m_nDA, tFR_CMD.m_nSA, tFR_CMD.m_nCMD);
	else
		CMDMLog(pPRM, "--X no response from (nAddr=%#08x)\n", MA_SRV_ADDR);
	return(MAFRT_ERROR); }
/* ----------------------------------------------------------------------- */
CMDMLog(pPRM, "--> REG_ALWD");
/* ----------------------------------------------------------------------- */
/* sends REGINFO */
CMDM_CreateFrame(&tFR_ANS, MA_FR_VALID, MAFRT_REGINFO, MAINFT_REG_INFO,
				 MA_SRV_ADDR, pMS->m_tProt.m_nAddr, tFR_CMD.m_nFRID,
				 NULL, NULL);
CMDM_ClearCMD(pPRM);
CMDM_SaveMsg(pPRM, &tFR_ANS);
CMDMLog(pPRM, "<-- REGINFO");
/* ----------------------------------------------------------------------- */
/* waits for REG */
if(MAFRT_ERROR==CMDM_WaitForFrameTypeFromSta(pPRM, &tFR_CMD, MAFRT_REG,
   MA_SRV_ADDR, MA_MAX_POLLS, 0, pMS->m_bShowDebugMsg))
	{
	if(tFR_CMD.m_bValid)
		CMDMLog(pPRM, "--X not a REG (nDA=%#08x, nSA=%#08x, nType=%d)",
		tFR_CMD.m_nDA, tFR_CMD.m_nSA, tFR_CMD.m_nCMD);
	else
		CMDMLog(pPRM, "--X no response from (nAddr=%#08x)\n", MA_SRV_ADDR);
	return(MAFRT_ERROR); }
/* ----------------------------------------------------------------------- */
CMDMLog(pPRM, "--> REG");
/* ----------------------------------------------------------------------- */
/* sends ACK */
CMDM_CreateFrame(&tFR_ANS, MA_FR_VALID, MAFRT_ACK, MAINFT_NONE,
				 MA_SRV_ADDR, pMS->m_tProt.m_nAddr, tFR_CMD.m_nFRID,
				 NULL, NULL);
CMDM_ClearCMD(pPRM);
CMDM_SaveMsg(pPRM, &tFR_ANS);
CMDMLog(pPRM, "<-- ACK");
/* ----------------------------------------------------------------------- */
pMS->m_tST.bRegistred=1;
/* ----------------------------------------------------------------------- */
return(0);
}
/* ======================================================================= */
int CMDM_CLIResponse (tagCMDM_CLI* pMS)
{
/* ----------------------------------------------------------------------- */
/* responses on SRV commands (21.08.2003) */
/* ----------------------------------------------------------------------- */
int nRes;
tagMAFR tFR_CMD;
tagPRT_MDM* pPRM=&pMS->m_tProt;
/* ----------------------------------------------------------------------- */
CMDM_CLISanity(pMS, "CMDM_CLIResponse()");
/* ----------------------------------------------------------------------- */
CMDM_Delay (MA_MDM_CHK_INT);
/* ----------------------------------------------------------------------- */
if(0==CMDM_TryToLoadAFrame(pPRM, &tFR_CMD))
	return(0);
/* ----------------------------------------------------------------------- */
if(0==CMDM_IsFrameForSTA(pPRM, &tFR_CMD, pPRM->m_nAddr))
	return(0);
/* ----------------------------------------------------------------------- */
if(tFR_CMD.m_nFRID==pMS->m_nLastFRID)
	return(0);
/* ----------------------------------------------------------------------- */
pMS->m_nLastFRID=tFR_CMD.m_nFRID;
/* ----------------------------------------------------------------------- */
/* analize frame type */
/* ----------------------------------------------------------------------- */
switch(tFR_CMD.m_nCMD)
	{
	case MAFRT_POLL:
		nRes=CMDM_CLIAnswPoll (pMS, &tFR_CMD);
		CMDMLog(pPRM, "poll %s", pER(nRes));
		break;
	case MAFRT_UPD:
		nRes=CMDM_CLIAnswUpd (pMS, &tFR_CMD);
		CMDMLog(pPRM, "update %s", pER(nRes));
		break;
	case MAFRT_SYNC:
		nRes=CMDM_CLIAnswSync (pMS, &tFR_CMD);
		CMDMLog(pPRM, "syncronisation %s", pER(nRes));
		break;
	default:
		CMDMLog(pPRM, "ignore a frame (nDA=%#08x, nSA=%#08x, nType=%d)",
			tFR_CMD.m_nDA, tFR_CMD.m_nSA, tFR_CMD.m_nCMD);
		break;
	};
/* ----------------------------------------------------------------------- */
return(0);
}
/* ======================================================================= */
int CMDM_CLIStart (tagCMDM_CLI* pMS)
	{
	/* ------------------------------------------------------------------- */
	/* client 21.08.2003 */
	/* ------------------------------------------------------------------- */
	int nRegTries=0;
	int nRes;
	//int bNotDone;
	tagPRT_MDM* pPRT=&pMS->m_tProt;
	/* ------------------------------------------------------------------- */
	CMDM_CLISanity(pMS, "CMDM_CLIStart()");
	/* ------------------------------------------------------------------- */
	/* registration */
	/* ------------------------------------------------------------------- */
	CMDMLog(pPRT, "registration");
	do	{
		CMDMLog(pPRT, "try %d/%d", nRegTries+1, MA_MAX_REGRQ);
		nRes=CMDM_CLIRegister(pMS);
		if(0==nRes) break;
		nRegTries++;
		if (MA_MAX_REGRQ==nRegTries)
			{
			CMDMLog(pPRT, "registration failed, too many tries (%d) !!!",
				nRegTries);
			return(MAFRT_ERROR);
			}
		}
	while(nRes==MAFRT_ERROR);
	CMDMLog(pPRT, "registration OK");
	/* ------------------------------------------------------------------- */
	return(0);
	}
/* ======================================================================= */
#define EXAMPL_SRV_CFG_FILE "C:\\Kolevatov\\EMU\\sta_cfg02.txt"
/* ======================================================================= */
#ifdef MDM_CLI_MAIN
/* ======================================================================= */
/* Global Function */
/* ======================================================================= */
void IntErrExit(char* sFuncName, char* sErr)
{
fprintf(stderr, "ERROR: %s, %s (stderr)\n", sFuncName, sErr);
fprintf(stdout, "ERROR: %s, %s (stdout)\n", sFuncName, sErr);
exit(0);
}
/* ======================================================================= */
int main(int argc, char* argv[])
	{
	/* ------------------------------------------------------------------- */
	int nRes;
	int bRx;
	int bNotDone;
	tagCMDM_CLI tMS;
	tagMDM_SAMPLES tDataRx;
	/* ------------------------------------------------------------------- */
	CMDM_CLI(&tMS);
	/* ------------------------------------------------------------------- */
	printf("\n"
		"MDM_CLI TEST ver. 0.1.%d (build # %d)\n"
		"common medium emulation - CLIENT application\n\n",
		LAST_BACKUP_NUMBER, AUTO_BUILD_NUMBER);
	/* ------------------------------------------------------------------- */
	if(2!=argc)
		{
		printf("\n"
		"  USAGE: MDM_CLI <client_cfg_file>\n"
		"EXAMPLE: mdm_cli %s\n\n", EXAMPL_SRV_CFG_FILE);
		return(0);
		}
	/* ------------------------------------------------------------------- */
	printf("MDM_CLI: initialization, cfg file = %s\n", argv[1]);
	nRes=CMDM_CLIInit(&tMS, argv[1], 0);
	/* ------------------------------------------------------------------- */
	CMDM_CLIStart (&tMS);
	/* ------------------------------------------------------------------- */
	/* syncronisation */
	bNotDone=1;
	/* ------------------------------------------------------------------- */
	while(bNotDone)
		{
		CMDM_CLIIterate	(&tMS, 0, &bRx, &tDataRx);
		}
	/* ------------------------------------------------------------------- */
	return(0);
	}
/* ======================================================================= */
#endif /* MDM_CLI_MAIN */
/* ======================================================================= */