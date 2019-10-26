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
// mdm_srv.h: implementation of service functions
// ======================================================================= */
#ifndef __MDM_SRV__
#define __MDM_SRV__
/* ======================================================================= */
#include "..\\TCILIB\\CLASS_C\\C_LOGGER\\c_logger.h"
/* ======================================================================= */
/* constants - may be changed manually */
/* ======================================================================= */
#define MDMSRV_MAXSTA	(10)	/* maximal number of the supported stations */
/* ======================================================================= */
/* constants ... DO NOT CHANGE (16.10.2003) */
/* ======================================================================= */
#define MDMS_LNORM		(0)		// normal log message
#define MDMS_LDEB		(1)		// debug message
#define MDMS_LDEBMSD	(2)		// debug message sequence diagram
/* ======================================================================= */
/* station record */
/* ======================================================================= */
typedef struct
	{
	int m_bValid;				// flag "record is valid"
	int m_bTaktError;			// flag "STA is has skipped a takt"
	int m_bPolled;				// flag "STA is already polled"
	int m_bUpdated;				// flag "STA is already updated"
	int m_bSyncr;				// flag "STA is syncronized" // TODO: do we really need it ?
	int m_bActive;				// flag "STA is active" (09.09.2003)
	int m_nAddr;				// address of the station
	int m_nLastFRID;			// last FRID
	tagMDM_SAMPLES m_tMDM;		// CLI medium state (09.09.2003)
	} tagMDM_STAINF;
/* ======================================================================= */
/* server struct */
/* ======================================================================= */
typedef struct
	{
	int		m_bInited;				// flag "inited"
	/* ------------------------------------------------------------------- */
	struct	{
		int	nCurTime;				// current emulation time
		int	nLastFrID;				// last sent frame ID (has sence only for SRV)
		int	nTaktErrors;			// total number of takt error
		int	nStasActive;			// current # of STAs in Tx mode
		int	nStas;					// no. of the registred STAs		
		}	m_tST;					// STATE
	/* ------------------------------------------------------------------- */
	struct	{
		int	nStasMax;				// MAX # of registred STAs
		int	bShowDebugMsg;			// show debug messages
		int	bShowDebugMsgMsd;		// show debug msg: message sequence diagram
		int	bSuppSDLSIM;			// SDL Simulation support (11.09.2003)
		int bSaveLog;				// Save log file (25.09.2003)
		char sLogger[256];			// logger file name (18.09.2003)
		tagPRT_MDM tProt;			// protocol exchange
		}	m_tCFG;					// CONFIG
	/* ------------------------------------------------------------------- */
	tagCLOGR m_tLOGR;				// logger (18.09.2003)
	/* ------------------------------------------------------------------- */
	tagMDM_STAINF m_pStas[MDMSRV_MAXSTA];	// information
	tagMDM_SAMPLES m_tMDM;			// medium (08.09.2003)
	} tagCMDM_SRV;
/* ======================================================================= */
/* "class" functions */
/* ======================================================================= */
void CMDM_SRV				(tagCMDM_SRV* pMS);
void CMDM_SRV_				(tagCMDM_SRV* pMS);
int  CMDM_SRVIsInited		(tagCMDM_SRV* pMS);
int  CMDM_SRVInit			(tagCMDM_SRV* pMS, char* sCFGFile);
/* ======================================================================= */
int  CMSLog					(tagCMDM_SRV* pMS, int nLogMsgType, const char* sMsgFormat, ...);	/* (18.09.2003) */
/* ======================================================================= */
/* private functions */
/* ======================================================================= */
int CMDM_SRVAddStaToList	(tagCMDM_SRV* pMS, int nStaAddr);
int CMDM_SRVGetEmptySlotIdx	(tagCMDM_SRV* pMS);
int CMDM_SRVGetStaIdxByAddr	(tagCMDM_SRV* pMS, int nStaAddr);
int CMDM_SRVIncFRID			(tagCMDM_SRV* pMS);
int CMDM_SRVIsStaRegistred	(tagCMDM_SRV* pMS, int nStaAddr);
int CMDM_SRVUnregSta		(tagCMDM_SRV* pMS, int nIdx);
int CMDM_SRVUpdFRIDForStaByIdx(tagCMDM_SRV* pMS, int nIdx, int nFRID);
/* ======================================================================= */
/* scenarios */
/* ======================================================================= */
int CMDM_SRVPerformReg		(tagCMDM_SRV* pMS, int nStaAddr);
int CMDM_SRVPerformSYNC		(tagCMDM_SRV* pMS, int nIdx);
int CMDM_SRVPerformUPD		(tagCMDM_SRV* pMS, int nIdx);
int CMDM_SRVPollSTA			(tagCMDM_SRV* pMS, int nIdx);
/* ======================================================================= */
/* functionality */
/* ======================================================================= */
int CMDM_SRVChkProtocol		(tagCMDM_SRV* pMS);
int CMDM_SRVInteract		(tagCMDM_SRV* pMS, int* pbNotDone); /* user interactivity (16.10.2003) */
int CMDM_SRVMixMedium		(tagCMDM_SRV* pMS);	/* 08.09.2003 */
int CMDM_SRVPoll			(tagCMDM_SRV* pMS);
int CMDM_SRVResetMDMState	(tagCMDM_SRV* pMS);
int CMDM_SRVStatistics		(tagCMDM_SRV* pMS);
int CMDM_SRVUpdateCounter	(tagCMDM_SRV* pMS);
int CMDM_SRVUpdateMDM		(tagCMDM_SRV* pMS);
/* ======================================================================= */
#endif /* __MDM_SRV__ */
/* ======================================================================= */