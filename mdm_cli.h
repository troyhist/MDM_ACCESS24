/* =======================================================================
// MDM_CLI ver. 1.0 (pure C)
// "common medium" emulator - CLIENT PART
// -----------------------------------------------------------------------
// MD: 19.08.2003
// LE: 10.09.2003
// -----------------------------------------------------------------------
//   Compiler: MS VC 6.0
// Aplication: ANSI, DOS16, WIN32
//  Edited by: Serguei Kolevatov
// =======================================================================
// mdm_cli.h: implementation of service functions
// ======================================================================= */
#ifndef __MDM_CLI__
#define __MDM_CLI__
/* ======================================================================= */
#include "mdm_access.h"				/* 09.09.2003 */
#include "..\\STACK802_11\\proc_phy_exp.h"
/* ======================================================================= */
/* constants - may be changed manually */
/* ======================================================================= */
/* ----------------------------------------------------------------------- */
/* DO NOT CHANGE */
/* ----------------------------------------------------------------------- */
#define MDM_ST_IDLE			(0)		// idle
#define MDM_ST_RxSignThres	(1)		// signal threshold
#define MDM_ST_RxPwrFall	(2)		// pwr fall
/* ----------------------------------------------------------------------- */
/* CLI iterator's commands (17.09.2003) */
/* ----------------------------------------------------------------------- */
#define MDM_CLI_ITER_SYNC		(0)		// syncronisation
#define MDM_CLI_ITER_GET_STATE	(1)		// answer on UPD
#define MDM_CLI_ITER_UPD_STATE	(2)		// answer on POLL w. new MDM state
#define MDM_CLI_ITER_SKIP_POLL	(3)		// answer on POLL w. no data
/* ======================================================================= */
/* server struct */
/* ======================================================================= */
typedef struct
	{
	int		m_bInited;			// flag "inited"
	int		m_nLastFRID;		// last received FRID
	int		m_bShowDebugMsg;	// show debug messages
	tagPRT_MDM m_tProt;			// protocol file
	struct {
		int	bRegistred;			// flag "STA is registred"
		int	bSyncr;				// flag "STA is syncronised"
		int	bPolled;			// flag "STA is polled"
		int	bUpdated;			// flag "STA is updated"
		int	bActiveTx;			// flag "STA has data to Tx"
		int	bActiveRx;			// flag "STA has data on MDM to Rx"	(12.09.2003)
		int	nRetries;			// no. of retries
		int	nCurTime;			// current emulation time
		int	nServTime;			// Server Time
		tagMDM_SAMPLES tMDM;	// medium (08.09.2003)
		}  m_tST;
	} tagCMDM_CLI;
/* ======================================================================= */
/* CLI public functions (09.09.2003) */
/* ======================================================================= */
int CMDM_CLIIterate			(tagCMDM_CLI* pMS, int nCmd, int *pbActiveRx, tagMDM_SAMPLES* pData); /* (09.09.2003, 17.09.2003) */
/* ======================================================================= */
/* "class" functions (09.09.2003) */
/* ======================================================================= */
void CMDM_CLI				(tagCMDM_CLI* pMS);
void CMDM_CLI_				(tagCMDM_CLI* pMS);
int  CMDM_CLIIsInited		(tagCMDM_CLI* pMS);
int  CMDM_CLIInit			(tagCMDM_CLI* pMS, char* sCFGFile, int nCurTime);
/* ======================================================================= */
/* functions for PHY layer of SYS */
/* ======================================================================= */
int CMDM_CLIResponse		(tagCMDM_CLI* pMS);		/* 21.08.2003 */
int CMDM_CLIStart			(tagCMDM_CLI* pMS);		/* 21.08.2003 */
int CMDM_CLIGetMDMState		(tagCMDM_CLI* pMS);		/* 26.08.2003 */
/* ======================================================================= */
/* scenarios */
/* ======================================================================= */
int CMDM_CLIAnswPoll	(tagCMDM_CLI* pMS, tagMAFR* pCMD);
int CMDM_CLIAnswUpd		(tagCMDM_CLI* pMS, tagMAFR* pCMD);
int CMDM_CLIAnswSync	(tagCMDM_CLI* pMS, tagMAFR* pCMD);
/* ======================================================================= */
/* functionality */
/* ======================================================================= */
int CMDM_CLIRegister		(tagCMDM_CLI* pMS);		/* 18.08.2003 */
/* ======================================================================= */
#endif /* __MDM_CLI__ */
/* ======================================================================= */
