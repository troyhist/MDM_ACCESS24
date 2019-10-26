/* =======================================================================
// MDM_ACCESS ver. 1.0 (pure C)
// "common medium" access function library
// -----------------------------------------------------------------------
// MD: 18.08.2003
// LE: 18.08.2003
// -----------------------------------------------------------------------
//   Compiler: MS VC 6.0
// Aplication: ANSI, DOS16, WIN32
//  Edited by: Serguei Kolevatov
// =======================================================================
// mdm_access.c: implementation of service functions
// ======================================================================= */
#include <stdio.h>
#include <stdlib.h>			/* exit */
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
/* ======================================================================= */
/* global function */
/* ======================================================================= */
void IntErrExit(char* sFuncName, char* sErr);
/* ======================================================================= */
void CMDMErrExit(char* sFuncName, const char* sErrFormat, ...)
{
/* ----------------------------------------------------------------------- */
char sErr[8192];		/* fixed buf. size, may be changed to var. */
char sBuf[8192];
va_list arglist;
va_start(arglist, sErrFormat);
/* ----------------------------------------------------------------------- */
vsprintf (sErr, sErrFormat, arglist);
sprintf(sBuf, "func. %s: %s", sFuncName, sErr);
/* ----------------------------------------------------------------------- */
IntErrExit("CMDMErrExit(..)", sBuf);
}
/* ======================================================================= */
int CMDMLog(tagPRT_MDM* pPRM, const char* sMsgFormat, ...)
	{
	/* ------------------------------------------------------------------- */
	/* prints text string on a screen (21.08.2003) */
	/* ------------------------------------------------------------------- */
	char sMsg[8192];		/* fixed buf. size, may be changed to var. */
	va_list arglist;
	va_start(arglist, sMsgFormat);
	/* ------------------------------------------------------------------- */
	CMDMSanity(pPRM, "CMDMLog()");
	/* ------------------------------------------------------------------- */
	vsprintf (sMsg, sMsgFormat, arglist);
	printf("%s: %s\n", pPRM->m_bSRV?"MDM_SRV":"MDM_CLI", sMsg);
	/* ------------------------------------------------------------------- */
	return(0);	
	}
/* ======================================================================= */
/* frame creation */
/* ======================================================================= */
int CMDM_CreateFrame(tagMAFR* pFr,
					 int bValid,
					 int nCMD,
					 int nInfType,
					 int nDA,
					 int nSA,
					 int nFRID,
					 int* pTimeStamp,
					 tagMDM_SAMPLES *pMDM)
{
/* creates a frame (19.08.2003, 26.08.2003, 09.09.2003) */
if (pFr==NULL) return(MAFRT_ERROR);
pFr->m_bValid		=bValid;		// flag "server is IDLE"
pFr->m_nCMD			=nCMD;			// type of the frame
pFr->m_nInfType		=nInfType;		// information type
pFr->m_nDA			=nDA;			// Dest. address
pFr->m_nSA			=nSA;			// Source address
pFr->m_nFRID		=nFRID;
if(pTimeStamp)		pFr->tTime.m_nTimeStamp=*pTimeStamp;
if(pMDM)			pFr->tMdm.m_tMDM=*pMDM;
return(0);
}
/* ----------------------------------------------------------------------- */
int CMDMConvFrToPhyFr(tagMAFR* pFr, tagMAFR_PHY* pFrPhy,
					  tagMAINFT_TIME*		pTime,
					  tagMAINFT_MDM*		pMdm, 
					  tagMAINFT_REGINFO*	pRegInfo)
	{
	/* converts frame to PHY representation (21.08.2003) */
	if (pFr==NULL) return(MAFRT_ERROR);
	if (pFrPhy==NULL) return(MAFRT_ERROR);
	pFrPhy->m_bValid	=(unsigned char)pFr->m_bValid;		// flag "server is IDLE"
	pFrPhy->m_cCMD		=(unsigned char)pFr->m_nCMD;		// type of the frame
	pFrPhy->m_cInfType	=(unsigned char)pFr->m_nInfType;	// information type
	pFrPhy->m_cDummy	=0;
	pFrPhy->m_nDA		=pFr->m_nDA;			// Dest. address
	pFrPhy->m_nSA		=pFr->m_nSA;			// Source address
	pFrPhy->m_nFRID		=pFr->m_nFRID;
	if(pTime)	*pTime		=pFr->tTime;
	if(pMdm)	*pMdm		=pFr->tMdm;
	if(pRegInfo)*pRegInfo	=pFr->tRegInfo;
	return(0);
	}
/* ----------------------------------------------------------------------- */
int CMDMConvPhyFrToFr(tagMAFR* pFr, tagMAFR_PHY* pFrPhy,
					  tagMAINFT_TIME*		pTime,
					  tagMAINFT_MDM*		pMdm, 
					  tagMAINFT_REGINFO*	pRegInfo)
	{
	/* converts frame to PHY representation (21.08.2003) */
	if (pFr==NULL) return(MAFRT_ERROR);
	if (pFrPhy==NULL) return(MAFRT_ERROR);
	pFr->m_bValid	=pFrPhy->m_bValid;		// flag "server is IDLE"
	pFr->m_nCMD		=pFrPhy->m_cCMD;		// type of the frame
	pFr->m_nInfType	=pFrPhy->m_cInfType;	// information type
	pFr->m_nDA		=pFrPhy->m_nDA;			// Dest. address
	pFr->m_nSA		=pFrPhy->m_nSA;			// Source address
	pFr->m_nFRID	=pFrPhy->m_nFRID;
	if(pTime)		pFr->tTime		=*pTime;
	if(pMdm)		pFr->tMdm		=*pMdm;
	if(pRegInfo)	pFr->tRegInfo	=*pRegInfo;
	return(0);
	}
/* ======================================================================= */
/* init & sanity */
/* ======================================================================= */
void CMDMSanity	(tagPRT_MDM* pPRM, char* sFuncName)
	{
	/* sanity check (20.08.2003) */
	if(pPRM==NULL) CMDMErrExit(sFuncName, "sanity check failed");
	if(0==pPRM->m_bInited) CMDMErrExit(sFuncName, "sanity check failed");
	}
/* ----------------------------------------------------------------------- */
int CMDMInit (tagPRT_MDM* pPRM, int nAddr, char* sMdmFreeFlag, 
			  char* sSRV_CMDS, char* sCLI_ANSWRS)
	{
	/* init (20.08.2003) */
	if(pPRM==NULL) CMDMErrExit("CMDMInit(..)", "pPRM is NULL");
	pPRM->m_bInited=1;
	pPRM->m_bSRV=(nAddr==MA_SRV_ADDR)?1:0;
	pPRM->m_nAddr=nAddr;
	sprintf(pPRM->m_sMdmFreeFlag,	"%s", sMdmFreeFlag);
	sprintf(pPRM->m_sSRV_CMDS,		"%s", sSRV_CMDS);
	sprintf(pPRM->m_sCLI_ANSWRS,	"%s", sCLI_ANSWRS);
	return(0);
	}
/* ======================================================================= */
/* PHY frame check */
/* ======================================================================= */
unsigned int CMDMCalcFrameCheck(tagMAFR_PHY* pFrPhy)
	{
	/* calculates CRC (21.08.2003) */
	#define MAX_CRC_BYTES		(4)
	unsigned int nFrameCheck=0;
	unsigned int nMSB;
	int i, nValues[MAX_CRC_BYTES];
	if(pFrPhy==NULL) CMDMErrExit("CMDMCalcFrameCheck(..)", "pFrPhy is NULL");
	nValues[0]=pFrPhy->m_nDA;
	nValues[1]=pFrPhy->m_nSA;
	nValues[2]=pFrPhy->m_nFRID;
	nValues[3]=pFrPhy->m_cCMD;
	for (i=0; i<MAX_CRC_BYTES; i++)
		{
		nFrameCheck=nFrameCheck^nValues[i];
		nMSB=(nFrameCheck>>24)&0xff;
		nFrameCheck=nFrameCheck<<8;
		nFrameCheck=nFrameCheck|nMSB;
		}
	return(nFrameCheck);
	}
/* ======================================================================= */
int CMDMIsValid (tagMAFR_PHY* pFrPhy)
	{
	/* checks CRC (21.08.2003) */
	unsigned int nFrameCheck;
	if(pFrPhy==NULL) CMDMErrExit("CMDMIsValid(..)", "pFrPhy is NULL");
	nFrameCheck=CMDMCalcFrameCheck(pFrPhy);
	if (pFrPhy->m_nFrameCheck==nFrameCheck)
		return(1);
	return(0);
	}
/* ======================================================================= */
/* file function */
/* ======================================================================= */
int fexist(char* sFileName)
	{
	FILE* fp;
	if((fp=fopen(sFileName, "rb"))==NULL)
		return(0);
	fclose(fp); return(1);
	}
/* ======================================================================= */
/* primitive access functions */
/* ======================================================================= */
int CMDM_ClearANS (tagPRT_MDM* pPRM)
	{
	/* ------------------------------------------------------------------- */
	/* clears CLI's ANS medium (21.08.2003) */
	/* ------------------------------------------------------------------- */
	CMDMSanity(pPRM, "CMDM_ClearANS()");
	/* ------------------------------------------------------------------- */
	if(fexist(pPRM->m_sCLI_ANSWRS)==0) return(MAFRT_ERROR);
	remove(pPRM->m_sCLI_ANSWRS);
	return(0);
	/* ------------------------------------------------------------------- */
	}
/* ======================================================================= */
int CMDM_ClearCMD (tagPRT_MDM* pPRM)
	{
	/* ------------------------------------------------------------------- */
	/* clears SRV's CMD medium (22.08.2003) */
	/* ------------------------------------------------------------------- */
	CMDMSanity(pPRM, "CMDM_ClearCMD()");
	/* ------------------------------------------------------------------- */
	if(fexist(pPRM->m_sSRV_CMDS)==0) return(MAFRT_ERROR);
	remove(pPRM->m_sSRV_CMDS);
	return(0);
	/* ------------------------------------------------------------------- */
	}
/* ======================================================================= */
int CMDM_MdmOccupy(tagPRT_MDM* pPRM)
	{
	/* ------------------------------------------------------------------- */
	/* clears "free access" flag (20.08.2003) */
	/* ------------------------------------------------------------------- */
	CMDMSanity(pPRM, "CMDM_MdmOccupy()");
	/* ------------------------------------------------------------------- */
	if(fexist(pPRM->m_sMdmFreeFlag)==0) return(MAFRT_ERROR);
	remove(pPRM->m_sMdmFreeFlag);
	return(0);
	/* ------------------------------------------------------------------- */
	}
/* ======================================================================= */
int CMDM_MdmRelease(tagPRT_MDM* pPRM)
	{
	/* ------------------------------------------------------------------- */
	/* creates "free access" flag (20.08.2003) */
	/* ------------------------------------------------------------------- */
	FILE* fp;
	/* ------------------------------------------------------------------- */
	CMDMSanity(pPRM, "CMDM_MdmRelease()");
	/* ------------------------------------------------------------------- */
	if(fexist(pPRM->m_sMdmFreeFlag)==1) return(0);
	if((fp=fopen(pPRM->m_sMdmFreeFlag, "wb"))==NULL)
		CMDMErrExit("CMDM_MdmRelease()", "can't open 'free access' file %s",
		pPRM->m_sMdmFreeFlag);
	fclose(fp);
	return(0);
	/* ------------------------------------------------------------------- */
	}
/* ======================================================================= */
int CMDM_LoadMsg(tagPRT_MDM* pPRM, tagMAFR* pFR)
	{
/* -----------------------------------------------------------------------
// Description: loads protocol frame
//   Parametrs: ...
//      Return: 0 - ok, (-1) - error
// -----------------------------------------------------------------------
//   MD: 18.08.2003
//   LE: 25.08.2003
// TODO: test it
// ----------------------------------------------------------------------- */
FILE* fp;
int nReadBytes;
char* pFilename;
tagMAFR_PHY tFRP;
/* ----------------------------------------------------------------------- */
CMDMSanity(pPRM, "CMDM_LoadMsg()");
/* ------------------------------------------------------------------- */
if (pFR==NULL) CMDMErrExit("CMDM_LoadMsg(..)", "pFR==NULL");
/* ----------------------------------------------------------------------- */
/* decission */
tFRP.m_cDummy=0;
pFilename=pPRM->m_bSRV?pPRM->m_sCLI_ANSWRS:pPRM->m_sSRV_CMDS;
/* ----------------------------------------------------------------------- */
if((fp=fopen(pFilename, "rb"))==NULL)
	{ pFR->m_bValid=0; return(0); }
/* ----------------------------------------------------------------------- */
nReadBytes=fread(&tFRP, 1, sizeof(tagMAFR_PHY), fp);
/* ----------------------------------------------------------------------- */
if(sizeof(tagMAFR_PHY)!=nReadBytes)
	{ fclose(fp); pFR->m_bValid=0; return(0); }
/* ----------------------------------------------------------------------- */
if(0==tFRP.m_bValid)
	{ fclose(fp); pFR->m_bValid=0; return(0); }
/* ----------------------------------------------------------------------- */
if (CMDMCalcFrameCheck(&tFRP)!=tFRP.m_nFrameCheck)
	{ fclose(fp); pFR->m_bValid=0; return(0); }
/* ----------------------------------------------------------------------- */
switch(tFRP.m_cInfType)
	{
	case MAINFT_REG_INFO:
		fread(&pFR->tRegInfo,	1, sizeof(tagMAINFT_REGINFO), fp);	break;
	case MAINFT_TIME:
		fread(&pFR->tTime,		1, sizeof(tagMAINFT_TIME), fp);	break;
	case MAINFT_TIME_MDM_ST:
		fread(&pFR->tTime,		1, sizeof(tagMAINFT_TIME), fp);
		fread(&pFR->tMdm,		1, sizeof(tagMAINFT_MDM), fp);	break;
	case MAINFT_MDM_ST:
		fread(&pFR->tMdm,		1, sizeof(tagMAINFT_MDM), fp);	break;
	default: break;
	}
/* ----------------------------------------------------------------------- */
CMDMConvPhyFrToFr(pFR, &tFRP, &pFR->tTime, &pFR->tMdm, &pFR->tRegInfo);
/* ----------------------------------------------------------------------- */
fclose(fp); return(0);
/* ----------------------------------------------------------------------- */
}
/* ======================================================================= */
int CMDM_SaveMsg(tagPRT_MDM* pPRM, tagMAFR* pFR)
	{
/* -----------------------------------------------------------------------
// Description: saves protocol frame
//   Parametrs: ...
//      Return: 0 - ok, (-1) - error
// -----------------------------------------------------------------------
//   MD: 18.08.2003
//   LE: 25.08.2003
// TODO: test it
// ----------------------------------------------------------------------- */
FILE* fp;
char* pFilename;
tagMAFR_PHY tFRP;
/* ----------------------------------------------------------------------- */
CMDMSanity(pPRM, "CMDM_SaveMsg()");
/* ----------------------------------------------------------------------- */
if (pFR==NULL) CMDMErrExit("CMDM_SaveMsg(..)", "pFR==NULL");
/* ----------------------------------------------------------------------- */
/* decission */
/* ----------------------------------------------------------------------- */
pFilename=pPRM->m_bSRV?pPRM->m_sSRV_CMDS:pPRM->m_sCLI_ANSWRS;
/* ----------------------------------------------------------------------- */
if((fp=fopen(pFilename, "wb"))==NULL)
	CMDMErrExit("CMDM_SaveMsg(..)", "can't write into file %s", pFilename);
/* ----------------------------------------------------------------------- */
CMDMConvFrToPhyFr(pFR, &tFRP, &pFR->tTime, &pFR->tMdm, &pFR->tRegInfo);
/* ----------------------------------------------------------------------- */
tFRP.m_nFrameCheck=CMDMCalcFrameCheck(&tFRP);
fwrite(&tFRP, 1, sizeof(tagMAFR_PHY), fp);
/* ----------------------------------------------------------------------- */
switch(tFRP.m_cInfType)
	{
	case MAINFT_REG_INFO:
		fwrite(&pFR->tRegInfo,	1, sizeof(tagMAINFT_REGINFO),	fp); break;
	case MAINFT_TIME:
		fwrite(&pFR->tTime,		1, sizeof(tagMAINFT_TIME),	fp); break;
	case MAINFT_TIME_MDM_ST:
		fwrite(&pFR->tTime,		1, sizeof(tagMAINFT_TIME),	fp);
		fwrite(&pFR->tMdm,		1, sizeof(tagMAINFT_MDM),	fp); break;
	case MAINFT_MDM_ST:
		fwrite(&pFR->tMdm,		1, sizeof(tagMAINFT_MDM),	fp); break;
	default: break;
	}
/* ----------------------------------------------------------------------- */
fflush(fp); fclose(fp);
return(0);
/* ----------------------------------------------------------------------- */
}
/* ======================================================================= */
int CMDM_IsMdmFree(tagPRT_MDM* pPRM)
	{
	/* ------------------------------------------------------------------- */
	/* returns '1' if MDM is free (19.08.2003) */
	/* ------------------------------------------------------------------- */
	CMDMSanity(pPRM, "CMDM_IsFree()");
	return(fexist(pPRM->m_sMdmFreeFlag));
	/* ------------------------------------------------------------------- */
	}
/* ======================================================================= */
/* unversal functions */
/* ======================================================================= */
int CMDM_IsFrameForSTA(tagPRT_MDM* pPRM, tagMAFR* pFR, int nAddr)
	{
	/* ------------------------------------------------------------------- */
	/* returns '1' if a STA has received a frame intended for it */
	/* ------------------------------------------------------------------- */
	CMDMSanity(pPRM, "CMDM_IsFrameForSTA(..)");
	/* ------------------------------------------------------------------- */
	if (pFR==NULL) CMDMErrExit("CMDM_IsFrameForSTA(..)", "pFR==NULL");
	/* ------------------------------------------------------------------- */
	if(pFR->m_nDA==nAddr)
		return(1);
	return(0);
	}
/* ======================================================================= */
/* public service function */
/* ======================================================================= */
int CMDM_TryToLoadAFrame(tagPRT_MDM* pPRM, tagMAFR* pFR)
	{
	/* ------------------------------------------------------------------- */
	/* returns '1' if a STA w. given Address has received a frame */
	/* ------------------------------------------------------------------- */
	CMDMSanity(pPRM, "CMDM_TryToLoadAFrame(..)");
	/* ------------------------------------------------------------------- */
	CMDM_LoadMsg(pPRM, pFR);
	/* ------------------------------------------------------------------- */
	if(pFR->m_bValid==1)
		return(1);
	return(0);
	}
/* ======================================================================= */
int CMDM_WaitForFrameType (tagPRT_MDM* pPRM, tagMAFR* pFR, int nType,
						   int nMaxRetries, int bSupSDLSim, int bShowMsg)
	{
	/* ------------------------------------------------------------------- */
	/* returns '0' if STA has received a frame of given type */
	/* bSupSDLSim - if '=1' then waits unlimited time */
	/* (22.08.2003, 11.09.2003) */
	/* ------------------------------------------------------------------- */
	int nRetries=0;
	int nRes;
	do	{
		if(bShowMsg)
			CMDMLog(pPRM, "... try %d/%d, wait %d ms", nRetries+1, nMaxRetries,
			MA_MDM_CHK_INT + MA_WAIT_INCR*nRetries);
		nRes=CMDM_TryToLoadAFrame(pPRM, pFR);
		if (1==nRes)
			{
			if(pFR->m_nCMD==nType)
				return(0);
			CMDMLog(pPRM, "--X not a type %d (nDA=%#08x, nSA=%#08x, nType=%d)",
				nType, pFR->m_nDA, pFR->m_nSA, pFR->m_nCMD);
			}
		nRetries++;
		CMDM_Delay(MA_MDM_CHK_INT + MA_WAIT_INCR*nRetries);
		} while(bSupSDLSim || (nRetries<nMaxRetries));
	return(MAFRT_ERROR);
	}
/* ======================================================================= */
int CMDM_WaitForFrameTypeFromSta (tagPRT_MDM* pPRM, tagMAFR* pFR, int nType,
						   int nAddr, int nMaxRetries, int bSupSDLSim, int bShowMsg)
	{
	/* ------------------------------------------------------------------- */
	/* returns '0' if STA has received a frame of given type, */
	/* from given STA (22.08.2003, 11.09.2003) */
	/* ------------------------------------------------------------------- */
	int nErr;
	nErr=CMDM_WaitForFrameType (pPRM, pFR, nType, nMaxRetries, bSupSDLSim, bShowMsg);
	if (0==nErr)
		if(pFR->m_nSA==nAddr)
			return(0);
	return(MAFRT_ERROR);
	}
/* ======================================================================= */
int CMDM_WaitForFrameTypeFromStaFRID (tagPRT_MDM* pPRM, tagMAFR* pFR, int nType,
						   int nAddr, int nLastFRID, int nMaxRetries, int bSupSDLSim, int bShowMsg)
	{
	/* ------------------------------------------------------------------- */
	/* returns '0' if STA has received a frame of given type, */
	/* from given STA (22.08.2003, 11.09.2003) */
	/* ------------------------------------------------------------------- */
	int nErr;
	nErr=CMDM_WaitForFrameTypeFromSta (pPRM, pFR, nType, nAddr,
		nMaxRetries, bSupSDLSim, bShowMsg);
	if (0==nErr)
		if (MAFRT_ERROR==nLastFRID || pFR->m_nFRID!=nLastFRID)
			return(0);
	return(MAFRT_ERROR);
	}
/* ======================================================================= */
int CMDM_WaitForMDM(tagPRT_MDM* pPRM, tagMAFR* pFR_ANS,
					int nMaxRetries, int bShowMsg)
	{
	/* ------------------------------------------------------------------- */
	/* (21.08.2003) */
	/* ------------------------------------------------------------------- */
	int nRetries=0;
	int nRes;
	do	{
		if(bShowMsg)
			CMDMLog(pPRM, "... try %d/%d, wait %d ms", nRetries+1, nMaxRetries,
			MA_MDM_CHK_INT + MA_WAIT_INCR*nRetries);
		nRes=CMDM_TryToLoadAFrame(pPRM, pFR_ANS);
		if (1==nRes)
			return(0);
		nRetries++;
		CMDM_Delay(MA_MDM_CHK_INT + MA_WAIT_INCR*nRetries);
		} while(nRetries<nMaxRetries);
	return(MAFRT_ERROR);
	}
/* ======================================================================= */
/* functions, used by CLI */
/* ======================================================================= */
int CMDM_HaveRecvBrdcstPOLL	(tagPRT_MDM* pPRM, tagMAFR* pFR)
	{		/* returns '1' if STA has received a BROADCAST POLL SRV (20.08.2003) */
	if(CMDM_IsFrameForSTA(pPRM, pFR, MA_BRDCST_ADDR))
		if(pFR->m_nCMD==(unsigned char)MAFRT_POLL)
			return(1);
	return(0);
	}
/* ======================================================================= */
/* delay functions */
/* ======================================================================= */
int CMDM_Rand(void)
	{
	/* ------------------------------------------------------------------- */
	/* random number of ms (18.08.2003) */
	/* ------------------------------------------------------------------- */
	int nRand=rand();
	/* ------------------------------------------------------------------- */
	return(nRand);
	}
/* ======================================================================= */
int CMDM_Delay(int nuS)
	{
	/* ------------------------------------------------------------------- */
	/* delay of nuS (microsecond) (20.08.2003) */
	/* ------------------------------------------------------------------- */
	int nStopTime;
	int nClocks;
	/* ------------------------------------------------------------------- */
	nClocks=(CLOCKS_PER_SEC*nuS)/1000;
	nStopTime=clock()+nClocks;
	while(clock()<nStopTime);
	/* ------------------------------------------------------------------- */
	return(0);
	}
/* ======================================================================= */
