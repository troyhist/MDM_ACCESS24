/* =======================================================================
// MDM_SRV ver. 1.0 (pure C)
// "common medium" emulator - SERVER PART
// -----------------------------------------------------------------------
// MD: 18.08.2003
// LE: 16.10.2003
// -----------------------------------------------------------------------
//   Compiler: MS VC 6.0
// Aplication: ANSI, DOS16, WIN32
//  Edited by: Serguei Kolevatov
// =======================================================================
// mdm_srv.c: implementation of service functions
// ======================================================================= */
/* ======================================================================= */
#include <stdio.h>
#include <string.h>			/* strcpy (19.08.2003) */
#include <conio.h>			/* kbhit, getch (16.10.2003) */
/* ----------------------------------------------------------------------- */
#ifdef __STDC__
#include <stdarg.h>			/* variable argument list */
#elif _WIN32
#include <stdarg.h>			/* variable argument list */
#else
#include <varargs.h>		/* Required for UNIX V compatibility */
#endif
/* ----------------------------------------------------------------------- */
#include "mdm_access.h"
#include "mdm_srv.h"
#include "..\\STACK802_11\\c_vector.h"	/* VectorDumpCMPLX (18.09.2003) */
#include "..\\TCILIB\\FUNC\\GETVAL\\strgetval.h"	/* StrGetVal (19.08.2003) */
#include "AutoBuild.h"	/* AUTO_BUILD_NUMBER (28.08.2003) */
#include "LastBackup.h"	/* LAST_BACKUP_NUMBER (19.08.2003) */
/* ======================================================================= */
/* global data */
/* ======================================================================= */
char glMSGD[256];		/* MDM SRV Global Data */
char glMSGD8K[8192];	/* MDM SRV Global Data */
/* ======================================================================= */
/* private macros */
/* ======================================================================= */
#define pER(_n_)		(MAFRT_ERROR==_n_)?"ERR":"ok"
/* ======================================================================= */
/* private functions */
/* ======================================================================= */
int	CMDM_SRVIsInited	(tagCMDM_SRV* pMS)
	{ return(pMS->m_bInited); }
/* ----------------------------------------------------------------------- */
void CMDM_SRVSanity		(tagCMDM_SRV* pMS, char* sFuncName)
	{ if(!CMDM_SRVIsInited(pMS)) 
	CMDMErrExit(sFuncName, "sanity check failed"); }
/* ======================================================================= */
/* service functions */
/* ======================================================================= */
void CMDM_SRV			(tagCMDM_SRV* pMS)
	{ /* constructor */ CLOGR(&pMS->m_tLOGR); pMS->m_bInited=0; }
/* ----------------------------------------------------------------------- */
void CMDM_SRV_			(tagCMDM_SRV* pMS)
	{ /* (25.09.2003) */ CLOGR_(&pMS->m_tLOGR); pMS->m_bInited=0; }
/* ======================================================================= */
int CMDM_SRVInit(tagCMDM_SRV* pMS, char* sCFGFile)
{
/* -----------------------------------------------------------------------
// Description: inits medium server
//   Parametrs: ...
//      Return: 0 - ok, (-1) - error
// -----------------------------------------------------------------------
//   MD: 18.08.2003
//   LE: 25.09.2003
// TODO: complete it
// ----------------------------------------------------------------------- */
char sBuf[2048];
char sFLNMS[3][256];
int i;
FILE* fp;
/* ----------------------------------------------------------------------- */
srand(clock());
/* ----------------------------------------------------------------------- */
fp=fopen(sCFGFile, "rt");
if (NULL==fp)
	CMDMErrExit("CMDM_SRVInit(..)", "cann't open cfg-file %s", sCFGFile);
/* ----------------------------------------------------------------------- */
fread(sBuf, 1, sizeof(sBuf), fp); fclose(fp);
/* ----------------------------------------------------------------------- */
#define CFG_PAREE(_n_)	CMDMErrExit("CMDM_SRVInit(..)", "parameter %s not found", _n_)
#define CFG_GV(_n_)		StrGetValue(sBuf, _n_, glMSGD)
#define CFG_PARSE(_n_)	if(!CFG_GV(_n_)) CFG_PAREE(_n_)
/* ----------------------------------------------------------------------- */
/* process MAC cfg */
CFG_PARSE("PROT_EXCH_CMDS");	else { sscanf(glMSGD, "%s", &sFLNMS[0]); }
CFG_PARSE("PROT_EXCH_ANSWRS");	else { sscanf(glMSGD, "%s", &sFLNMS[1]); }
CFG_PARSE("PROT_EXCH_FLAG");	else { sscanf(glMSGD, "%s", &sFLNMS[2]); }
/* ----------------------------------------------------------------------- */
/* SRV cfg */
CFG_PARSE("DEBUG_MSG");		else { sscanf(glMSGD, "%d", &pMS->m_tCFG.bShowDebugMsg); }
CFG_PARSE("DEBUG_MSG_MSD");	else { sscanf(glMSGD, "%d", &pMS->m_tCFG.bShowDebugMsgMsd); }
CFG_PARSE("MAX_CLI");		else { sscanf(glMSGD, "%d", &pMS->m_tCFG.nStasMax); }
CFG_PARSE("SDL_SIM_SUPPORT");else{ sscanf(glMSGD, "%d", &pMS->m_tCFG.bSuppSDLSIM); }
CFG_PARSE("SAVE_LOG");		else{ sscanf(glMSGD, "%d", &pMS->m_tCFG.bSaveLog); }
CFG_PARSE("SRV_LOG");		else{ sscanf(glMSGD, "%s", &pMS->m_tCFG.sLogger); }
/* ----------------------------------------------------------------------- */
/* inits internal data */
pMS->m_bInited		=1;				// flag "inited"
/* ----------------------------------------------------------------------- */
/* inits STATE */
pMS->m_tST.nCurTime		=0;		// current emulation time
pMS->m_tST.nLastFrID	=0;		// last frame ID (has sence only for SRV)
pMS->m_tST.nStas		=0;		// no. of the registred STAs
pMS->m_tST.nTaktErrors	=0;		// total # of takt errors
pMS->m_tST.nStasActive	=0;		// current # of STAs in Tx mode
/* ----------------------------------------------------------------------- */
if(pMS->m_tCFG.bSaveLog)		// (25.09.2003)
CLOGRInit(&pMS->m_tLOGR, NULL, pMS->m_tCFG.sLogger, "MDM SERVER");
/* ----------------------------------------------------------------------- */
CMDMInit(&pMS->m_tCFG.tProt, MA_SRV_ADDR, sFLNMS[2], sFLNMS[0], sFLNMS[1]);
CMDM_MdmOccupy(&pMS->m_tCFG.tProt);
/* ----------------------------------------------------------------------- */
if (pMS->m_tCFG.nStasMax>MDMSRV_MAXSTA) pMS->m_tCFG.nStasMax=MDMSRV_MAXSTA;
/* ----------------------------------------------------------------------- */
/* clears valididty flag */
for (i=0; i<MDMSRV_MAXSTA; i++)
	pMS->m_pStas[i].m_bValid=0;
/* ----------------------------------------------------------------------- */
return(0);
#undef CFG_PAREE
#undef CFG_GV
#undef CFG_PARSE
}
/* ======================================================================= */
int  CMSLog	(tagCMDM_SRV* pMS, int nLogMsgType, const char* sMsgFormat, ...)
	{
	/* ------------------------------------------------------------------- */
	/* updates log file (18.09.2003, 16.10.2003) */
	/* nLogMsgType - type of the log msg: normal, debug, MSE diagram */
	/* ------------------------------------------------------------------- */
	char sMsg[8192];		/* fixed buf. size, may be changed to var. */
	int bProcess;
	va_list arglist;
	va_start(arglist, sMsgFormat);
	/* ------------------------------------------------------------------- */
	CMDMSanity(&pMS->m_tCFG.tProt, "CMSLog()");
	/* ------------------------------------------------------------------- */
	bProcess=0;
	/* ------------------------------------------------------------------- */
	switch(nLogMsgType)
		{
		case MDMS_LNORM:	bProcess=1; break;
		case MDMS_LDEB:		if(pMS->m_tCFG.bShowDebugMsg) bProcess=1; break;
		case MDMS_LDEBMSD:	if(pMS->m_tCFG.bShowDebugMsgMsd) bProcess=1; break;
		default: bProcess=0; break;
		};
	/* ------------------------------------------------------------------- */
	if(0==bProcess) return(0);
	/* ------------------------------------------------------------------- */
	vsprintf (sMsg, sMsgFormat, arglist);
	CMDMLog(&pMS->m_tCFG.tProt, sMsg);
	if(pMS->m_tCFG.bSaveLog)
	CLOGRAddDirect(&pMS->m_tLOGR, "%4d.0 : %s", pMS->m_tST.nCurTime, sMsg);
	/* ------------------------------------------------------------------- */
	return(0);	
	}
/* ======================================================================= */
/* private functions */
/* ======================================================================= */
int CMDM_SRVAddStaToList(tagCMDM_SRV* pMS, int nStaAddr)
	{
	/* ------------------------------------------------------------------- */
	/* adds a STA to the POLL list (18.08.2003, 26.08.2003, 09.09.2003) */
	/* TODO: complete it */
	/* ------------------------------------------------------------------- */
	int nIdx;
	/* ------------------------------------------------------------------- */
	CMDM_SRVSanity(pMS, "CMDM_SRVAddSTAToList()");
	/* ------------------------------------------------------------------- */
	/* check if STA already exists in the list */
	if (CMDM_SRVIsStaRegistred(pMS, nStaAddr))
		return(1);
	/* ------------------------------------------------------------------- */
	/* searchs for empty record */
	nIdx=CMDM_SRVGetEmptySlotIdx(pMS);
	if(MAFRT_ERROR==nIdx)
		return(MAFRT_ERROR);
	/* ------------------------------------------------------------------- */
	pMS->m_pStas[nIdx].m_bValid=1;			// flag "record is valid"
	pMS->m_pStas[nIdx].m_bTaktError=0;		// ...
	pMS->m_pStas[nIdx].m_bPolled=0;			// flag "STA is already polled"
	pMS->m_pStas[nIdx].m_bUpdated=0;		// ...
	pMS->m_pStas[nIdx].m_bSyncr=0;			// flag "STA is syncronized" // TODO: do we really need it ?
	pMS->m_pStas[nIdx].m_bActive=0;			// flag "STA is active" (09.09.2003)
	pMS->m_pStas[nIdx].m_nAddr=nStaAddr;	// address of the station
	pMS->m_pStas[nIdx].m_nLastFRID=MAFRT_ERROR;
	/* ------------------------------------------------------------------- */
	pMS->m_tST.nStas++;
	/* ------------------------------------------------------------------- */
	return(0);
	}
/* ======================================================================= */
int CMDM_SRVGetEmptySlotIdx(tagCMDM_SRV* pMS)
	{
	/* ------------------------------------------------------------------- */
	/* returns idx of the empty slot in the list or '-1' (19.08.2003) */
	/* ------------------------------------------------------------------- */
	int i;
	/* ------------------------------------------------------------------- */
	CMDM_SRVSanity(pMS, "CMDM_SRVGetEmptySlotIdx()");
	/* ------------------------------------------------------------------- */
	for(i=0; i<MDMSRV_MAXSTA; i++)
		if(0==pMS->m_pStas[i].m_bValid)
			return(i);
	return(MAFRT_ERROR);
	}
/* ======================================================================= */
int CMDM_SRVGetStaIdxByAddr(tagCMDM_SRV* pMS, int nStaAddr)
	{
	/* ------------------------------------------------------------------- */
	/* returns idx of the STA or '-1' (19.08.2003) */
	/* ------------------------------------------------------------------- */
	int i;
	/* ------------------------------------------------------------------- */
	CMDM_SRVSanity(pMS, "CMDM_SRVGetStaIdxByAddr(..)");
	/* ------------------------------------------------------------------- */
	for(i=0; i<MDMSRV_MAXSTA; i++)
		if (pMS->m_pStas[i].m_bValid && 
			pMS->m_pStas[i].m_nAddr==nStaAddr)
			return(i);
	return(MAFRT_ERROR);
	/* ------------------------------------------------------------------- */
	}
/* ======================================================================= */
int CMDM_SRVIsStaRegistred(tagCMDM_SRV* pMS, int nStaAddr)
	{
	int nRes;
	/* ------------------------------------------------------------------- */
	/* returns '1' if STA is registred (19.08.2003) */
	/* ------------------------------------------------------------------- */
	CMDM_SRVSanity(pMS, "CMDM_SRVIsSTARegistred(..)");
	nRes=CMDM_SRVGetStaIdxByAddr(pMS, nStaAddr);
	return((nRes==MAFRT_ERROR)?0:1);
	/* ------------------------------------------------------------------- */
	}
/* ======================================================================= */
int CMDM_SRVUnregSta(tagCMDM_SRV* pMS, int nIdx)
	{
	/* ------------------------------------------------------------------- */
	/* unregisters given STA & decreases no. of STAs (21.08.2003) */
	/* ------------------------------------------------------------------- */
	CMDM_SRVSanity(pMS, "CMDM_SRVUnregSta(..)");
	CMSLog(pMS, MDMS_LNORM, "unregister STA no. %d, ADDR = %#08x",
		nIdx+1, pMS->m_pStas[nIdx].m_nAddr);
	pMS->m_pStas[nIdx].m_bValid=0;
	pMS->m_tST.nStas--;
	return(0);
	}
/* ======================================================================= */
int CMDM_SRVUnregStaByAddr(tagCMDM_SRV* pMS, int nAddr)
	{
	/* ------------------------------------------------------------------- */
	/* unregisters given STA by STA's address (25.08.2003) */
	/* ------------------------------------------------------------------- */
	int nIdx;
	CMDM_SRVSanity(pMS, "CMDM_SRVUnregStaByAddr(..)");
	nIdx=CMDM_SRVGetStaIdxByAddr(pMS, nAddr);
	return(CMDM_SRVUnregSta(pMS, nIdx));
	}
/* ======================================================================= */
int CMDM_SRVUpdFRIDForStaByIdx(tagCMDM_SRV* pMS, int nIdx, int nFRID)
	{
	/* ------------------------------------------------------------------- */
	/* updates last FRID field of given STA (26.08.2003) */
	/* ------------------------------------------------------------------- */
	CMDM_SRVSanity(pMS, "CMDM_SRVUpdFRIDForStaByIdx(..)");
	/* ------------------------------------------------------------------- */
	pMS->m_pStas[nIdx].m_nLastFRID=nFRID;
	/* ------------------------------------------------------------------- */
	return(0);
	}
/* ======================================================================= */
/* scenarios */
/* ======================================================================= */
int CMDM_SRVPerformReg(tagCMDM_SRV* pMS, int nStaAddr)
{
/* -----------------------------------------------------------------------
// Description: tries to register a STA
//   Parametrs: ...
//      Return: 0 - ok, (-1) - error
// -----------------------------------------------------------------------
//   MD: 18.08.2003
//   LE: 25.08.2003
// TODO: none
// ----------------------------------------------------------------------- */
tagMAFR tFR_ANS, tFR_ALWD, tFR_REG;
tagPRT_MDM* pPRM=&pMS->m_tCFG.tProt;
/* ----------------------------------------------------------------------- */
CMDM_SRVSanity(pMS, "CMDM_SRVPerformReg()");
/* ----------------------------------------------------------------------- */
CMSLog(pMS, MDMS_LDEBMSD, "--> REGREQ from STA, ADDR = %#08x", nStaAddr);
/* ----------------------------------------------------------------------- */
/* checks, if registration is allowed */
/* ----------------------------------------------------------------------- */
if(CMDM_SRVGetEmptySlotIdx(pMS)==MAFRT_ERROR)
	{ CMSLog(pMS, MDMS_LNORM, "registration is not allowed, too many STAs (%d)\n",
	MDMSRV_MAXSTA);	return(0);}
/* ----------------------------------------------------------------------- */
/* creates REG_ALWD frame */
/* ----------------------------------------------------------------------- */
CMDM_CreateFrame(&tFR_ALWD, MA_FR_VALID, MAFRT_REG_ALWD, MAINFT_NONE,
				 nStaAddr, MA_SRV_ADDR, ++pMS->m_tST.nLastFrID,
				 NULL, NULL);
/* ----------------------------------------------------------------------- */
/* checks if the STA is alredy in the POLL list */
/* ----------------------------------------------------------------------- */
if (CMDM_SRVIsStaRegistred(pMS, nStaAddr))
	{ CMSLog(pMS, MDMS_LNORM, "STA is already registered (nAddr=%#08x)", nStaAddr);
	return(MAFRT_ERROR); }
/* ----------------------------------------------------------------------- */
/* sends a allowance of REG */
CMDM_SaveMsg(pPRM, &tFR_ALWD);
CMSLog(pMS, MDMS_LDEBMSD, "<-- REG_ALWD, ADDR = %#08x", nStaAddr);
/* ----------------------------------------------------------------------- */
if(MAFRT_ERROR==CMDM_WaitForFrameTypeFromStaFRID(pPRM, &tFR_ANS, MAFRT_REGINFO,
   nStaAddr, -1, MA_MAX_W_REGINFO, pMS->m_tCFG.bSuppSDLSIM,
   pMS->m_tCFG.bShowDebugMsg))
	{
	if(tFR_ANS.m_bValid)
		CMSLog(pMS, MDMS_LDEBMSD,
		"--X not an REGINFO (nDA=%#08x, nSA=%#08x, nType=%d)",
		tFR_ANS.m_nDA, tFR_ANS.m_nSA, tFR_ANS.m_nCMD);
	else
		CMSLog(pMS, MDMS_LDEBMSD, "--X no response from (nAddr=%#08x)\n", nStaAddr);
	return(MAFRT_ERROR);
	}
/* ----------------------------------------------------------------------- */
CMSLog(pMS, MDMS_LDEBMSD, "--> REGINFO, ADDR = %#08x", nStaAddr);
/* ----------------------------------------------------------------------- */
/* adds STA into the list */
if (CMDM_SRVAddStaToList(pMS, nStaAddr)==MAFRT_ERROR)
	{ CMSLog(pMS, MDMS_LNORM, "Cann't register a STA (nAddr=%#08x)\n", nStaAddr);
	return(MAFRT_ERROR); }
CMSLog(pMS, MDMS_LNORM, "STA has been registered, ADDR = %#08x", nStaAddr);
/* ----------------------------------------------------------------------- */
/* creates REG frame */
CMDM_CreateFrame(&tFR_REG, MA_FR_VALID, MAFRT_REG, MAINFT_NONE,
				 nStaAddr, MA_SRV_ADDR, -1,
				 NULL, NULL);
/* ----------------------------------------------------------------------- */
/* sends REG */
CMDM_SaveMsg(pPRM, &tFR_REG);
CMSLog(pMS, MDMS_LDEBMSD, "<-- REG, ADDR = %#08x", nStaAddr);
/* ----------------------------------------------------------------------- */
/* waits for ACK */
if(MAFRT_ERROR==CMDM_WaitForFrameTypeFromStaFRID(pPRM, &tFR_ANS, MAFRT_ACK,
   nStaAddr, -1, MA_MAX_W_REGACK, pMS->m_tCFG.bSuppSDLSIM,
   pMS->m_tCFG.bShowDebugMsg))
	{
	if(tFR_ANS.m_bValid)
		CMSLog(pMS, MDMS_LDEBMSD, "--X not an ACK (nDA=%#08x, nSA=%#08x, nType=%d)",
		tFR_ANS.m_nDA, tFR_ANS.m_nSA, tFR_ANS.m_nCMD);
	else
		CMSLog(pMS, MDMS_LDEBMSD, "--X no response from (nAddr=%#08x)\n", nStaAddr);
	CMDM_SRVUnregStaByAddr(pMS, nStaAddr);
	return(MAFRT_ERROR);
	}
/* ----------------------------------------------------------------------- */
CMSLog(pMS, MDMS_LDEBMSD, "--> ACK, ADDR = %#08x", nStaAddr);
/* ----------------------------------------------------------------------- */
return(0);
}
/* ======================================================================= */
int CMDM_SRVPerformSYNC(tagCMDM_SRV* pMS, int nIdx)
{
/* -----------------------------------------------------------------------
// Description: performs syncronization
//   Parametrs: ...
//      Return: 0 - ok, (-1) - error
// -----------------------------------------------------------------------
//   MD: 21.08.2003
//   LE: 26.08.2003
// TODO: test it
// ----------------------------------------------------------------------- */
int nStaAddr;
int nLFRID;
int nErrDelay=MA_ERROR_DELAY + MA_ERR_INCR*pMS->m_tST.nTaktErrors;
tagMAFR tFR_CMD, tFR_ANS;
tagPRT_MDM* pPRM=&pMS->m_tCFG.tProt;
/* ----------------------------------------------------------------------- */
CMDM_SRVSanity(pMS, "CMDM_SRVPerformSYNC()");
/* ----------------------------------------------------------------------- */
if(0==pMS->m_pStas[nIdx].m_bSyncr && pMS->m_pStas[nIdx].m_bTaktError)
	{ CMSLog(pMS, MDMS_LNORM, "STA %d, takt error ... wait %d ms, re SYNC", nIdx+1, nErrDelay);
	CMDM_Delay(nErrDelay); }
/* ----------------------------------------------------------------------- */
nStaAddr=pMS->m_pStas[nIdx].m_nAddr;
nLFRID=pMS->m_pStas[nIdx].m_nLastFRID;
/* ----------------------------------------------------------------------- */
/* creates a SYNC frame */
CMDM_CreateFrame(&tFR_CMD, MA_FR_VALID, MAFRT_SYNC, MAINFT_TIME,
				 nStaAddr, MA_SRV_ADDR, -1,
				 &pMS->m_tST.nCurTime, NULL);
/* ----------------------------------------------------------------------- */
/* sends a UPD frame */
CMDM_SaveMsg(pPRM, &tFR_CMD);
CMSLog(pMS, MDMS_LDEBMSD, "<-- SYNC no. %d, ADDR = %#08x", nIdx+1, nStaAddr);
/* ----------------------------------------------------------------------- */
/* waits for ACK */
if(MAFRT_ERROR==CMDM_WaitForFrameTypeFromStaFRID(pPRM, &tFR_ANS, MAFRT_ACK,
   nStaAddr, nLFRID, MA_MAX_SYNCS, pMS->m_tCFG.bSuppSDLSIM,
   pMS->m_tCFG.bShowDebugMsg))
	{
	if(tFR_ANS.m_bValid)
		CMSLog(pMS, MDMS_LDEBMSD, "--X not an ACK (nDA=%#08x, nSA=%#08x, nType=%d)",
		tFR_ANS.m_nDA, tFR_ANS.m_nSA, tFR_ANS.m_nCMD);
	else
		CMSLog(pMS, MDMS_LDEBMSD, "--X no response from (nAddr=%#08x)\n", nStaAddr);
	return(MAFRT_ERROR);
	}
/* ----------------------------------------------------------------------- */
CMDM_SRVUpdFRIDForStaByIdx(pMS, nIdx, tFR_ANS.m_nFRID);
CMSLog(pMS, MDMS_LDEBMSD, "--> ACK, ADDR = %#08x", nStaAddr);
/* ----------------------------------------------------------------------- */
pMS->m_pStas[nIdx].m_bSyncr=1;
pMS->m_pStas[nIdx].m_bUpdated=1;
pMS->m_pStas[nIdx].m_bPolled=0;
pMS->m_pStas[nIdx].m_bTaktError=0;
/* ----------------------------------------------------------------------- */
return(0);
}
/* ======================================================================= */
int CMDM_SRVPerformUPD (tagCMDM_SRV* pMS, int nIdx)
{
/* -----------------------------------------------------------------------
// Description: performs MDM Update command
//   Parametrs: ...
//      Return: 0 - ok, (-1) - error
// -----------------------------------------------------------------------
//   MD: 21.08.2003
//   LE: 26.08.2003, 09.09.2003
// TODO: test it
// ----------------------------------------------------------------------- */
int nStaAddr;
int nLFRID;
int nErrDelay=MA_ERROR_DELAY + MA_ERR_INCR*pMS->m_tST.nTaktErrors;
tagMAFR tFR_CMD, tFR_ANS;
tagPRT_MDM* pPRM=&pMS->m_tCFG.tProt;
/* ----------------------------------------------------------------------- */
CMDM_SRVSanity(pMS, "CMDM_SRVPerformUPD()");
/* ----------------------------------------------------------------------- */
if(0==pMS->m_pStas[nIdx].m_bSyncr) return(0);
if(0==pMS->m_pStas[nIdx].m_bPolled) return(0);
/* ----------------------------------------------------------------------- */
nStaAddr=pMS->m_pStas[nIdx].m_nAddr;
nLFRID=pMS->m_pStas[nIdx].m_nLastFRID;
/* ----------------------------------------------------------------------- */
/* creates a UPD frame */
CMDM_CreateFrame(&tFR_CMD, MA_FR_VALID, MAFRT_UPD,
				 (pMS->m_tST.nStasActive>0)?MAINFT_TIME_MDM_ST:MAINFT_TIME,
				 nStaAddr, MA_SRV_ADDR, ++pMS->m_tST.nLastFrID,
				 &pMS->m_tST.nCurTime, &pMS->m_tMDM);
/* ----------------------------------------------------------------------- */
/* sends a UPD frame */
CMDM_SaveMsg(pPRM, &tFR_CMD);
CMSLog(pMS, MDMS_LDEBMSD, "<-- UPD no. %d, ADDR = %#08x", nIdx+1, nStaAddr);
/* ----------------------------------------------------------------------- */
/* waits for ACK */
if(MAFRT_ERROR==CMDM_WaitForFrameTypeFromStaFRID(pPRM, &tFR_ANS, MAFRT_ACK,
   nStaAddr, nLFRID, MA_MAX_UPDS, pMS->m_tCFG.bSuppSDLSIM,
   pMS->m_tCFG.bShowDebugMsg))
	{
	if(tFR_ANS.m_bValid)
		CMSLog(pMS, MDMS_LDEBMSD, "--X not an ACK (nDA=%#08x, nSA=%#08x, nType=%d)",
		tFR_ANS.m_nDA, tFR_ANS.m_nSA, tFR_ANS.m_nCMD);
	else
		CMSLog(pMS, MDMS_LDEBMSD, "--X no response from (nAddr=%#08x)", nStaAddr);
	if(pMS->m_pStas[nIdx].m_bTaktError) return(MAFRT_ERROR);
	CMSLog(pMS, MDMS_LNORM, "STA %d lost SYNC ... wait %d ms", nIdx+1, nErrDelay);
	CMDM_Delay(nErrDelay);
	pMS->m_pStas[nIdx].m_bTaktError=1;
	pMS->m_pStas[nIdx].m_bSyncr=0;
	pMS->m_tST.nTaktErrors++;
	return(0);
	}
/* ----------------------------------------------------------------------- */
CMDM_SRVUpdFRIDForStaByIdx(pMS, nIdx, tFR_ANS.m_nFRID);
CMSLog(pMS, MDMS_LDEBMSD, "--> ACK, ADDR = %#08x", nStaAddr);
/* ----------------------------------------------------------------------- */
pMS->m_pStas[nIdx].m_bPolled=0;
pMS->m_pStas[nIdx].m_bUpdated=1;
pMS->m_pStas[nIdx].m_bTaktError=0;
/* ----------------------------------------------------------------------- */
return(0);
}
/* ======================================================================= */
int CMDM_SRVPollSTA (tagCMDM_SRV* pMS, int nIdx)
	{
/* -----------------------------------------------------------------------
// Description: polls all the STAs
//   Parametrs: ...
//      Return: 0 - ok, (-1) - error
// -----------------------------------------------------------------------
//   MD: 21.08.2003
//   LE: 12.09.2003
// TODO: test it
// ----------------------------------------------------------------------- */
int nStaAddr;
int nLFRID;
int nErrDelay=MA_ERROR_DELAY + MA_ERR_INCR*pMS->m_tST.nTaktErrors;
tagMAFR tFR_ANS, tFR_POLL;
tagPRT_MDM* pPRM=&pMS->m_tCFG.tProt;
/* ----------------------------------------------------------------------- */
CMDM_SRVSanity(pMS, "CMDM_SRVPollSTA()");
/* ----------------------------------------------------------------------- */
if(0==pMS->m_pStas[nIdx].m_bSyncr) return(0);
if(0==pMS->m_pStas[nIdx].m_bUpdated) return(0);
/* ----------------------------------------------------------------------- */
nStaAddr=pMS->m_pStas[nIdx].m_nAddr;
nLFRID=pMS->m_pStas[nIdx].m_nLastFRID;
/* ----------------------------------------------------------------------- */
/* creates a POLL frame */
CMDM_CreateFrame(&tFR_POLL, MA_FR_VALID, MAFRT_POLL, MAINFT_TIME, 
				 nStaAddr, MA_SRV_ADDR, ++pMS->m_tST.nLastFrID,
				 &pMS->m_tST.nCurTime, NULL);
/* ----------------------------------------------------------------------- */
/* sends a POLL frame */
CMDM_SaveMsg(pPRM, &tFR_POLL);
CMSLog(pMS, MDMS_LDEBMSD, "<-- POLL no. %d, ADDR = %#08x", nIdx+1, nStaAddr);
/* ----------------------------------------------------------------------- */
/* waits for ACK */
if(MAFRT_ERROR==CMDM_WaitForFrameTypeFromStaFRID(pPRM, &tFR_ANS, MAFRT_ACK,
   nStaAddr, nLFRID, MA_MAX_POLLS, pMS->m_tCFG.bSuppSDLSIM,
   pMS->m_tCFG.bShowDebugMsg))
	{
	if(tFR_ANS.m_bValid)
		CMSLog(pMS, MDMS_LDEBMSD, "--X not an ACK (nDA=%#08x, nSA=%#08x, nType=%d)",
		tFR_ANS.m_nDA, tFR_ANS.m_nSA, tFR_ANS.m_nCMD);
	else
		CMSLog(pMS, MDMS_LDEBMSD, "--X no response from (nAddr=%#08x)\n", nStaAddr);
	if(pMS->m_pStas[nIdx].m_bTaktError) return(MAFRT_ERROR);
	CMSLog(pMS, MDMS_LNORM, "STA %d lost SYNC ... wait %d ms", nIdx+1, nErrDelay);
	CMDM_Delay(nErrDelay);
	pMS->m_pStas[nIdx].m_bTaktError=1;
	pMS->m_pStas[nIdx].m_bSyncr=0;
	pMS->m_tST.nTaktErrors++;
	return(0);
	}
/* ----------------------------------------------------------------------- */
/* recv ACK */
CMDM_SRVUpdFRIDForStaByIdx(pMS, nIdx, tFR_ANS.m_nFRID);
/* ----------------------------------------------------------------------- */
if (MAINFT_MDM_ST==tFR_ANS.m_nInfType)
	{
	pMS->m_pStas[nIdx].m_bActive=1;
	pMS->m_pStas[nIdx].m_tMDM=tFR_ANS.tMdm.m_tMDM;	/* (12.09.2003) */
	}
else
	{ pMS->m_pStas[nIdx].m_bActive=0; }				/* (12.09.2003) */
/* ----------------------------------------------------------------------- */
CMSLog(pMS, MDMS_LDEBMSD, "--> ACK, ADDR = %#08x, state = %s",
	nStaAddr, pMS->m_pStas[nIdx].m_bActive?"TX":"IDLE");
/* ----------------------------------------------------------------------- */
pMS->m_pStas[nIdx].m_bPolled=1;
pMS->m_pStas[nIdx].m_bUpdated=0;
pMS->m_pStas[nIdx].m_bTaktError=0;
/* ----------------------------------------------------------------------- */
return(0);
}
/* ======================================================================= */
/* functionality */
/* ======================================================================= */
int CMDM_SRVChkProtocol	(tagCMDM_SRV* pMS)
{
/* -----------------------------------------------------------------------
// Description: checks incoming messages
//   Parametrs: ...
//      Return: 0 - ok, (-1) - error
// -----------------------------------------------------------------------
//   MD: 18.08.2003
//   LE: 26.08.2003
// TODO: test it
// ----------------------------------------------------------------------- */
int nRes;
tagMAFR tFR_CMD;
tagMAFR tFR_ANS;
tagPRT_MDM* pPRM=&pMS->m_tCFG.tProt;
/* ----------------------------------------------------------------------- */
CMDM_SRVSanity(pMS, "CMDM_SRVChkProtocol()");
/* ----------------------------------------------------------------------- */
/* checks, if REGs are still possible */
if (pMS->m_tST.nStas==pMS->m_tCFG.nStasMax)
	{ return(0); }
/* ----------------------------------------------------------------------- */
/* creates BRDCST POLL frame */
CMDM_CreateFrame(&tFR_CMD, MA_FR_VALID, MAFRT_POLL, MAINFT_TIME,
				 MA_BRDCST_ADDR, MA_SRV_ADDR, -1,
				 &pMS->m_tST.nCurTime, NULL);
/* ----------------------------------------------------------------------- */
/* put BRDCST POLL frame on the MDM & set FREE flag */
CMDM_MdmRelease	(pPRM);
CMDM_SaveMsg	(pPRM, &tFR_CMD);
CMSLog			(pMS, MDMS_LDEBMSD,
				 "<-- BRDCST POLL,\tsrv time = %d units", pMS->m_tST.nCurTime);
/* ----------------------------------------------------------------------- */
/* checks if SRV has received smth. */
if (MAFRT_ERROR==CMDM_WaitForMDM(pPRM, &tFR_ANS, MA_MAX_W_REGREQ, 
	pMS->m_tCFG.bShowDebugMsg))
	{ CMDM_MdmOccupy(&pMS->m_tCFG.tProt); return (0); }
/* ----------------------------------------------------------------------- */
/* checks if received frame is for this STA */
if(CMDM_IsFrameForSTA(pPRM, &tFR_ANS, MA_SRV_ADDR))
	{
	/* ------------------------------------------------------------------- */
	/* analize frame type */
	/* ------------------------------------------------------------------- */
	switch(tFR_ANS.m_nCMD)
		{
		case MAFRT_REGREQ:
			nRes=CMDM_SRVPerformReg(pMS, tFR_ANS.m_nSA);
			if(MAFRT_ERROR==nRes)
				CMSLog(pMS, MDMS_LNORM, "registration failed");
			CMSLog(pMS, MDMS_LNORM, "ok");
			break;
		default:
			CMSLog(pMS, MDMS_LNORM,
				"ignore a frame (nDA=%#08x, nSA=%#08x, nType=%d)",
				tFR_ANS.m_nDA, tFR_ANS.m_nSA, tFR_ANS.m_nCMD);
			break;
		};
	/* ------------------------------------------------------------------- */
	CMDM_ClearANS (pPRM);
	/* ------------------------------------------------------------------- */
	}
/* ----------------------------------------------------------------------- */
return(0);
}
/* ======================================================================= */
int CMDM_SRVInteract (tagCMDM_SRV* pMS, int* pbNotDone)
{
/* -----------------------------------------------------------------------
// Description: checks if user want to stop simulation
//   Parametrs: ...
//      Return: 0 - ok, (-1) - error
// -----------------------------------------------------------------------
//   MD: 16.10.2003
//   LE: 16.10.2003
// TODO: test it
// ----------------------------------------------------------------------- */
tagPRT_MDM* pPRM=&pMS->m_tCFG.tProt;
/* ----------------------------------------------------------------------- */
CMDM_SRVSanity(pMS, "CMDM_SRVInteract()");
/* ----------------------------------------------------------------------- */
if(NULL==pbNotDone) return(0);
if(kbhit()) if (0x1b==getch())
	{ *pbNotDone=0; CMDM_MdmOccupy(pPRM); CMDM_ClearANS (pPRM); }
/* ----------------------------------------------------------------------- */
return(0);
}
/* ======================================================================= */
int CMDM_SRVMixMedium		(tagCMDM_SRV* pMS)
	{
	/* ------------------------------------------------------------------- */
	/* mixes MDM of active stations (08.09.2003, 11.09.2003) */
	/* ------------------------------------------------------------------- */
	int i, j;
	//tagPRT_MDM* pPRM=&pMS->m_tCFG.tProt;
	/* ------------------------------------------------------------------- */
	CMDM_SRVSanity(pMS, "CMDM_SRVMixMedium()");
	/* ------------------------------------------------------------------- */
	/* loads MDM of active stations */
	/* ------------------------------------------------------------------- */
	for (i=0; i<MDMSRV_MAXSTA; i++)
		{
		if (pMS->m_pStas[i].m_bValid && 
			pMS->m_pStas[i].m_bPolled &&
			pMS->m_pStas[i].m_bActive)
			{
			for (j=0; j<20; j++)
				{
				pMS->m_tMDM.m_pData[j].R+=pMS->m_pStas[i].m_tMDM.m_pData[j].R;
				pMS->m_tMDM.m_pData[j].I+=pMS->m_pStas[i].m_tMDM.m_pData[j].I;
				}
			pMS->m_tST.nStasActive++;
			}
		}
	/* ------------------------------------------------------------------- */
	return(0);
	}
/* ======================================================================= */
int CMDM_SRVPoll (tagCMDM_SRV* pMS)
	{
	/* ------------------------------------------------------------------- */
	/* polls all the STAs (21.08.2003) */
	/* ------------------------------------------------------------------- */
	int i;
	int nRes;
	tagPRT_MDM* pPRM=&pMS->m_tCFG.tProt;
	/* ------------------------------------------------------------------- */
	CMDM_SRVSanity(pMS, "CMDM_SRVPoll()");
	/* ------------------------------------------------------------------- */
	if(pMS->m_tST.nStas)
		{
		CMSLog(pMS, MDMS_LNORM, "...");
		CMSLog(pMS, MDMS_LNORM, "POLL of %d STA(s)", pMS->m_tST.nStas);
		}
	/* ------------------------------------------------------------------- */
	for (i=0; i<MDMSRV_MAXSTA; i++)
		if(pMS->m_pStas[i].m_bValid)
			if(pMS->m_pStas[i].m_bSyncr)
				{
				nRes=CMDM_SRVPollSTA(pMS, i);
				if(MAFRT_ERROR==nRes)
					CMDM_SRVUnregSta(pMS, i);
				CMDM_ClearANS (pPRM);
				CMSLog(pMS, MDMS_LNORM, "polled no. %d ... %s", i+1, pER(nRes));
				}
	/* ------------------------------------------------------------------- */
	if(pMS->m_tST.nStas)
		CMSLog(pMS, MDMS_LNORM, "ok");
	/* ------------------------------------------------------------------- */
	return(0);
	}
/* ======================================================================= */
int CMDM_SRVResetMDMState (tagCMDM_SRV* pMS)
	{
	/* ------------------------------------------------------------------- */
	/* Resets MDM state (26.08.2003, 09.09.2003) */
	/* ------------------------------------------------------------------- */
	int j;
	/* ------------------------------------------------------------------- */
	CMDM_SRVSanity(pMS, "CMDM_SRVResetMDMState()");
	/* ------------------------------------------------------------------- */
	pMS->m_tST.nStasActive=0;
	/* ------------------------------------------------------------------- */
	/* clears MDM */
	/* ------------------------------------------------------------------- */
	for (j=0; j<20; j++)
		{
		pMS->m_tMDM.m_pData[j].R=0;
		pMS->m_tMDM.m_pData[j].I=0;
		}
	/* ------------------------------------------------------------------- */
	return(0);
	}
/* ======================================================================= */
int CMDM_SRVStatistics (tagCMDM_SRV* pMS)
	{
	/* ------------------------------------------------------------------- */
	/* prints staticstics (26.08.2003) */
	/* ------------------------------------------------------------------- */
	int i;
	static int nOldStas=0;
	/* ------------------------------------------------------------------- */
	tagPRT_MDM* pPRM=&pMS->m_tCFG.tProt;
	/* ------------------------------------------------------------------- */
	CMDM_SRVSanity(pMS, "CMDM_SRVStatistics()");
	/* ------------------------------------------------------------------- */
	if(0==pMS->m_tST.nStas && 0==nOldStas)
		{
		if (!(pMS->m_tST.nCurTime%10))
			CMSLog(pMS, MDMS_LNORM, "... waiting, nCurTime = %d units",
			pMS->m_tST.nCurTime);
		return(0);
		}
	/* ------------------------------------------------------------------- */
	CMSLog(pMS, MDMS_LNORM, "...");
	CMSLog(pMS, MDMS_LNORM, "Statistics:");
	CMSLog(pMS, MDMS_LNORM, "nCurTime   = %d units", pMS->m_tST.nCurTime);
	CMSLog(pMS, MDMS_LNORM, "polled STA = %d (MAX %d)", pMS->m_tST.nStas, pMS->m_tCFG.nStasMax);
	CMSLog(pMS, MDMS_LNORM, "takt ERRs  = %d", pMS->m_tST.nTaktErrors);
	CMSLog(pMS, MDMS_LNORM, "...");
	CMSLog(pMS, MDMS_LNORM, "STA in TxM = %d", pMS->m_tST.nStasActive);
	CMSLog(pMS, MDMS_LNORM, "MDM state  = %s", (pMS->m_tST.nStasActive>0)?"ACTIVE":"IDLE");
	CMSLog(pMS, MDMS_LNORM, "...");
	/* ------------------------------------------------------------------- */
	for (i=0; i<MDMSRV_MAXSTA; i++)
		if(pMS->m_pStas[i].m_bValid)
			if(pMS->m_pStas[i].m_bActive)
				CMSLog(pMS, MDMS_LNORM, "STA %d - ACTIVE (%d), MDM={%s}", i+1,
					pMS->m_pStas[i].m_nAddr,
					VectorDumpCMPLX(glMSGD8K,
					pMS->m_pStas[i].m_tMDM.m_pData, 20));
	/* ------------------------------------------------------------------- */
	nOldStas=pMS->m_tST.nStas;
	/* ------------------------------------------------------------------- */
	return(0);
	}
/* ======================================================================= */
int CMDM_SRVUpdateCounter (tagCMDM_SRV* pMS)
	{
	/* ------------------------------------------------------------------- */
	CMDM_SRVSanity(pMS, "CMDM_SRVUpdateCounter()");
	/* ------------------------------------------------------------------- */
	pMS->m_tST.nCurTime++;
	return(0);
	}
/* ======================================================================= */
int CMDM_SRVUpdateMDM (tagCMDM_SRV* pMS)
	{
	/* ------------------------------------------------------------------- */
	/* updates MDM for all STA & Syncronize unsyncronised (21.08.2003) */
	/* ------------------------------------------------------------------- */
	int i;
	int nRes;
	tagPRT_MDM* pPRM=&pMS->m_tCFG.tProt;
	/* ------------------------------------------------------------------- */
	CMDM_SRVSanity(pMS, "CMDM_SRVUpdateMDM()");
	/* ------------------------------------------------------------------- */
	if(pMS->m_tST.nStas)
		{
		CMSLog(pMS, MDMS_LNORM, "...");
		CMSLog(pMS, MDMS_LNORM, "UPD MDM of %d STA(s)", pMS->m_tST.nStas);
		}
	/* ------------------------------------------------------------------- */
	for (i=0; i<MDMSRV_MAXSTA; i++)
		if(pMS->m_pStas[i].m_bValid)
			{
			if(pMS->m_pStas[i].m_bSyncr)
				{
				nRes=CMDM_SRVPerformUPD(pMS, i);
				}
			else
				{
				nRes=CMDM_SRVPerformSYNC(pMS, i);
				}
			CMDM_ClearANS (pPRM);
			if(MAFRT_ERROR==nRes) CMDM_SRVUnregSta(pMS, i);
			CMSLog(pMS, MDMS_LNORM, "updated no. %d ... %s", i+1, pER(nRes));
			}
	/* ------------------------------------------------------------------- */
	if(pMS->m_tST.nStas)
		CMSLog(pMS, MDMS_LNORM, "ok");
	/* ------------------------------------------------------------------- */
	return(0);
	}
/* ======================================================================= */
#define EXAMPL_SRV_CFG_FILE "C:\\Kolevatov\\EMU\\cfg_srv.txt"
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
	int bNotDone;
	tagCMDM_SRV tMS, *pMS=&tMS;
	/* ------------------------------------------------------------------- */
	CMDM_SRV(pMS);
	/* ------------------------------------------------------------------- */
	printf("\n"
		"MDM_SRV ver. 0.1.%d (build # %d)\n"
		"common medium emulation - server application\n\n",
		LAST_BACKUP_NUMBER, AUTO_BUILD_NUMBER);
	/* ------------------------------------------------------------------- */
	if(2!=argc)
		{
		printf("\n"
		"  USAGE: MDM_SRV <server_cfg_file>\n"
		"EXAMPLE: mdm_srv %s\n\n", EXAMPL_SRV_CFG_FILE);
		return(0);
		}
	/* ------------------------------------------------------------------- */
	nRes=CMDM_SRVInit(pMS, argv[1]);
	printf(
		"         server cfg-file = %s\n"
		"MDM_SRV: inited (nRes = %d)\n"
		"         max CLIs     = %d (MDMSRV_MAXSTA=%d)\n"
		"         protfile SRV = %s\n"
		"         protfile CLI = %s\n"
		"         protfile FLG = %s\n\n"
		"MDM_SRV: press 'ESC' to stop\n\n"
		, argv[1], nRes,
		tMS.m_tCFG.nStasMax, MDMSRV_MAXSTA,
		tMS.m_tCFG.tProt.m_sSRV_CMDS,
		tMS.m_tCFG.tProt.m_sCLI_ANSWRS,
		tMS.m_tCFG.tProt.m_sMdmFreeFlag);
	/* ------------------------------------------------------------------- */
	CMSLog(pMS, MDMS_LNORM, "... ok");
	/* ------------------------------------------------------------------- */
	bNotDone=1;
	/* ------------------------------------------------------------------- */
	while(bNotDone)
		{
		CMDM_SRVResetMDMState(pMS);
		CMDM_SRVChkProtocol(pMS);
		CMDM_SRVPoll(pMS);
		CMDM_SRVMixMedium(pMS);
		CMDM_SRVUpdateCounter(pMS);	/* placed between POLL and UPD */
		CMDM_SRVUpdateMDM(pMS);
		CMDM_SRVStatistics(pMS);
		CMDM_SRVInteract(pMS, &bNotDone);		/* (16.10.2003) */
		}
	/* ------------------------------------------------------------------- */
	printf("MDM_SRV: aborted by user\n");
	CMDM_SRV_(pMS);
	return(0);
	}
/* ======================================================================= */
