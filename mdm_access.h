/* =======================================================================
// MDM_ACCESS ver. 1.0 (pure C)
// "common medium" access function library
// -----------------------------------------------------------------------
// MD: 18.08.2003
// LE: 08.09.2003
// -----------------------------------------------------------------------
//   Compiler: MS VC 6.0
// Aplication: ANSI, DOS16, WIN32
//  Edited by: Serguei Kolevatov
// =======================================================================
// mdm_access.c: implementation of service functions
// ======================================================================= */
#ifndef __MDM_ACCESS__
#define __MDM_ACCESS__
/* ======================================================================= */
#include <stdlib.h>		/* srand */
#include <time.h>		/* clock */
/* ======================================================================= */
#include "..\\STACK802_11\\proc_phy_exp.h"	/* medium format (08.09.2003) */
/* ======================================================================= */
/* errors */
/* ======================================================================= */
#define MAFRT_ERROR		(-1)	/* global error */
/* ----------------------------------------------------------------------- */
/* waiting times, ms */
/* ----------------------------------------------------------------------- */
#define MA_ERROR_DELAY	(1000)	/* error delay (in ms) */
#define MA_ERR_INCR		(100)	/* error delay increase (in ms) */
#define MA_MDM_CHK_INT	(70)	/* MDM check interval (in ms) */
#define MA_WAIT_INCR	(2)		/* wait increase each tact (in ms) */
#define MA_RAND_WAIT	(50)	/* max. random wait time (in ms) */
/* ----------------------------------------------------------------------- */
/* SRV: max # of attempts to ... */
/* ----------------------------------------------------------------------- */
#define MA_MAX_W_REGREQ		(10)	/* ... to receive an REGREQ after sending a broadcast poll */
#define MA_MAX_W_REGINFO	(30)	/* ... to receive an REGINFO after sending a REG_ALWD */
#define MA_MAX_W_REGACK		(30)	/* ... to receive an ACK after sending a ACK */
/* ----------------------------------------------------------------------- */
#define MA_MAX_POLLS		(25)	/* ... to receive an ACK after polling a STA */
#define MA_MAX_SYNCS		(25)	/* ... to receive an ACK after sending a SYNC frame */
#define MA_MAX_UPDS			(50)	/* ... to receive an ACK after sending an UPD frame */
/* ----------------------------------------------------------------------- */
/* CLI: max # of attempts to ... */
/* ----------------------------------------------------------------------- */
#define MA_MAX_W_BPOLL		(50)	/* ... to receive a BPOLL frame */
#define MA_MAX_ACCESS_TRIES	(30)	/* ... to access the MDM after recv. a BPOLL frame */
#define MA_W_REG_ALWD		(30)	/* ... to recv. REG_ALWD frame */
#define MA_MAX_REGRQ		(5)		/* max # of registration attempts */
/* ======================================================================= */
/* SERVER commands */
/* ======================================================================= */
#define MAFRT_POLL		(4)	/* poll */
#define MAFRT_UPD		(0)	/* cmd: update */
#define MAFRT_SYNC		(1)	/* cmd: update + syncronisation */
#define MAFRT_REG_ALWD	(2)	/* registration allowed */
#define MAFRT_REG		(3)	/* registration accepted */
/* ======================================================================= */
/* CLIENT commands */
/* ======================================================================= */
#define MAFRT_ACK			(5)	/* ACK */
#define MAFRT_REGREQ		(8)	/* registration request */
#define MAFRT_REGINFO		(9)	/* registration request */
/* ======================================================================= */
/* data flags (21.08.2003) */
/* ======================================================================= */
#define MAINFT_NONE			(0)	/* none */
#define MAINFT_REG_INFO		(1)	/* registration info */
#define MAINFT_TIME			(2)	/* server time */
#define MAINFT_TIME_MDM_ST	(3)	/* MDM info */
#define MAINFT_MDM_ST		(4)	/* STA state */
/* ======================================================================= */
/* ----------------------------------------------------------------------- */
/* DO NOT CHANGE */
/* ----------------------------------------------------------------------- */
#define MA_BRDCST_ADDR		(-1)	/* broadcast address */
#define MA_SRV_ADDR			(1)	/* server's address */
#define MA_FR_VALID			(1)	/* frame is valid */
/* ======================================================================= */
/* PHYSICAL representation (21.08.2003) */
/* ======================================================================= */
typedef struct
	{
	unsigned char m_bValid;		// flag "frame is IDLE"
	unsigned char m_cCMD;		// type of the frame
	unsigned char m_cInfType;	// type of the frame
	unsigned char m_cDummy;		// type of the frame
	int m_nDA;					// Dest. address
	int m_nSA;					// Source address
	int m_nFRID;				// Frame ID
	unsigned int m_nFrameCheck;	// CRC
	} tagMAFR_PHY;
/* ======================================================================= */
/* time information (26.08.2003) */
/* ======================================================================= */
typedef struct
	{
	int m_nTimeStamp;		// Time stamp
	} tagMAINFT_TIME;
/* ======================================================================= */
/* MDM information frame (20.08.2003) */
/* ======================================================================= */
typedef struct
	{
	tagMDM_SAMPLES m_tMDM;	// MDM state (09.09.2003)
	} tagMAINFT_MDM;
/* ======================================================================= */
/* REGINFO frame (20.08.2003) */
/* ======================================================================= */
typedef struct
	{
	//char m_sMDMFilename[256];
	int nDummy;
	} tagMAINFT_REGINFO;
/* ======================================================================= */
/* protocol frame format (18.08.2003) */
/* ======================================================================= */
typedef struct
	{
	int m_bValid;			// flag "frame is IDLE"
	int m_nCMD;				// type of the frame
	int m_nInfType;			// data type
	int m_nDA;				// Dest. address
	int m_nSA;				// Source address
	int m_nFRID;			// Frame ID
	tagMAINFT_TIME		tTime;
	tagMAINFT_MDM		tMdm;
	tagMAINFT_REGINFO	tRegInfo;
	} tagMAFR;
/* ======================================================================= */
/* protocol MDM */
/* ======================================================================= */
typedef struct
	{
	int m_bInited;				// ...
	int m_bSRV;					// flag "I'm a SERVER"
	int m_nAddr;				// address of the STA
	char m_sMdmFreeFlag[256];	// flag "server is free"
	char m_sSRV_CMDS[256];		// file w. commands from SRV
	char m_sCLI_ANSWRS[256];	// file w. answers from CLI
	} tagPRT_MDM;
/* ======================================================================= */
void CMDMErrExit(char* sFuncName, const char* sErrFormat, ...);
int CMDMLog(tagPRT_MDM* pPRM, const char* sMsgFormat, ...);
/* ----------------------------------------------------------------------- */
/* frame creation */
/* ----------------------------------------------------------------------- */
int CMDM_CreateFrame(tagMAFR* pFr,
					 int bValid,
					 int nCMD,
					 int nInfType,
					 int nDA,
					 int nSA,
					 int nFRID,
					 int* pTimeStamp,
					 tagMDM_SAMPLES *pMDM
					);	/* creates a frame (19.08.2003) */
/* ----------------------------------------------------------------------- */
/* init & sanity */
/* ----------------------------------------------------------------------- */
void CMDMSanity	(tagPRT_MDM* pPRM, char* sFuncName);				/* sanity check */
int CMDMInit	(tagPRT_MDM* pPRM, int nAddr, char* sMdmFreeFlag, 
				 char* sSRV_CMDS, char* sCLI_ANSWRS);	/* init */
/* ----------------------------------------------------------------------- */
/* primitive access functions */
/* ----------------------------------------------------------------------- */
int CMDM_ClearANS	(tagPRT_MDM* pPRM);					/* clears CLI's ANS medium (21.08.2003) */
int CMDM_ClearCMD	(tagPRT_MDM* pPRM);					/* clears SRV's CMD medium (22.08.2003) */
int CMDM_MdmOccupy	(tagPRT_MDM* pPRM);					/* clears "free access" flag */
int CMDM_MdmRelease	(tagPRT_MDM* pPRM);					/* creates "free access" flag */
/* ----------------------------------------------------------------------- */
int CMDM_LoadMsg(tagPRT_MDM* pPRM, tagMAFR* pFR);		/* loads frame from CLI */
int CMDM_SaveMsg(tagPRT_MDM* pPRM, tagMAFR* pFR);		/* saves frame from SRV */
int CMDM_IsMdmFree	(tagPRT_MDM* pPRM);					/* returns '1' if MDM is free */
/* ----------------------------------------------------------------------- */
/* public service function */
/* ----------------------------------------------------------------------- */
int CMDM_IsFrameForSTA		(tagPRT_MDM* pPRM, tagMAFR* pFR, int nAddr);
int CMDM_TryToLoadAFrame	(tagPRT_MDM* pPRM, tagMAFR* pFR);	/* returns '1' if a STA Address has received a valid frame */
int CMDM_WaitForFrameType			(tagPRT_MDM* pPRM, tagMAFR* pFR_ANS, int nType, int nMaxRetries, int bSupSDLSim, int bShowMsg);	/* returns '0' if STA has received a frame of given type (22.08.2003) */
int CMDM_WaitForFrameTypeFromSta	(tagPRT_MDM* pPRM, tagMAFR* pFR_ANS, int nType, int nAddr, int nMaxRetries, int bSupSDLSim, int bShowMsg);
int CMDM_WaitForFrameTypeFromStaFRID(tagPRT_MDM* pPRM, tagMAFR* pFR_ANS, int nType, int nAddr, int nFRID, int nMaxRetries, int bSupSDLSim, int bShowMsg);
int CMDM_WaitForMDM			(tagPRT_MDM* pPRM, tagMAFR* pFR_ANS, int nMaxRetries, int bShowMsg);	/* returns '0' if MDM has a valid frame (21.08.2003) */
/* ----------------------------------------------------------------------- */
/* functions, used by SRV */
/* ----------------------------------------------------------------------- */
int CMDM_HaveRecvACK		(tagPRT_MDM* pPRM, tagMAFR* pFR, int nSA);	/* returns '1' if STA has received an ACK from STA w. SA (19.08.2003) */
int CMDM_HaveRecvREGREQ		(tagPRT_MDM* pPRM, tagMAFR* pFR);				/* returns '1' if STA has received an REGREQ (20.08.2003) */
int CMDM_HaveRecvREGINFO	(tagPRT_MDM* pPRM, tagMAFR* pFR, int nSA);	/* returns '1' if STA has received an REGINFO from STA w. SA (20.08.2003) */
/* ----------------------------------------------------------------------- */
/* functions, used by CLI */
/* ----------------------------------------------------------------------- */
int CMDM_HaveRecvPOLL		(tagPRT_MDM* pPRM, tagMAFR* pFR);		/* returns '1' if STA has received a POLL from SRV (20.08.2003) */
int CMDM_HaveRecvBrdcstPOLL	(tagPRT_MDM* pPRM, tagMAFR* pFR);		/* returns '1' if STA has received a BROADCAST POLL SRV (20.08.2003) */
int CMDM_HaveRecvREG_ALWD	(tagPRT_MDM* pPRM, tagMAFR* pFR);		/* returns '1' if STA has received a REG_ALWD from SRV (20.08.2003) */
int CMDM_HaveRecvREG		(tagPRT_MDM* pPRM, tagMAFR* pFR);		/* returns '1' if STA has received a REG from SRV (19.08.2003) */
int CMDM_HaveRecvUPD		(tagPRT_MDM* pPRM, tagMAFR* pFR);		/* returns '1' if STA has received a DATA POLL from SRV (20.08.2003) */
int CMDM_HaveRecvSYNC		(tagPRT_MDM* pPRM, tagMAFR* pFR);		/* returns '1' if STA has received a DATA+SYNC POLL from SRV (20.08.2003) */
/* ----------------------------------------------------------------------- */
/* delay functions */
/* ----------------------------------------------------------------------- */
int CMDM_Rand			(void);								/* random number of ms */
int CMDM_Delay			(int nmS);							/* delay */
/* ======================================================================= */
#endif /* __MDM_ACCESS__ */
/* ======================================================================= */
