#ifndef PND_PD_EVENT_HANDLER_H
#define PND_PD_EVENT_HANDLER_H
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_PDEventHandler.h                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Definition of PD Event Handler classes                                   */
/*  (Listener and Handler for CM PD Events)                                  */
/*                                                                           */
/*****************************************************************************/

class CPDUser;

/////////////////////////////////////////////////////////////////////////////
// PND Defines                                                               
/////////////////////////////////////////////////////////////////////////////

#define PNIO_AL_PROFILE_TYPE      0x08
#define PNIO_AL_AINFO_TYPE        0x00
#define PNIO_AL_CTRL_FLAGS_PD     0x01

typedef enum {
    PNIO_AOFF_BLKTYPE                        = 0,
    PNIO_AOFF_BLKLEN                         = 2,
    PNIO_AOFF_BLKVERSION                     = 4,
    PNIO_AOFF_ALARMTYPE                      = 6,
    PNIO_AOFF_APPLICATIONPROCESSIDENTIFIER   = 8,
    PNIO_AOFF_SLOT                           = 12,
    PNIO_AOFF_SUBSLOT                        = 14,
    PNIO_AOFF_MODULIDENTIFICATION            = 16,
    PNIO_AOFF_SUBMODULIDENTIFICATION         = 20,
    PNIO_AOFF_ALARMSPECIFIER                 = 24,
    PNIO_AOFF_USERSTRUCTUREIDENTIFIER        = 26,
    PNIO_ALARM_NORM_HEADER_OFFSETS_LEN       = 28
	                                              
} PNIO_PD_ALARM_NORM_HEADER_OFFSETS;

/////////////////////////////////////////////////////////////////////////////
// CPDEventHandler
/////////////////////////////////////////////////////////////////////////////

class CPDEventHandler
{
	// Attributes
private:
	LSA_BOOL        m_isStopping;  // CM channel close requested
	PND_HANDLE_TYPE m_pndHandle;   // PND handle
	CPDUser         *m_pUser;      // Link to PD User (for IND handling)
	LSA_VOID (CPDEventHandler::*m_handler[CM_OPC_MAX])(CM_UPPER_RQB_PTR_TYPE p_rb );  // Pointer to response handler

	// Construction/ Destruction
public:
	CPDEventHandler( PND_HANDLE_PTR_TYPE pHandle, CPDUser *pUser );
	virtual ~CPDEventHandler();

	// Accessors

	// EventHandling methode (CM-Indication)
	LSA_VOID ErrorEventHandling( CM_UPPER_RQB_PTR_TYPE pRQB );
	LSA_VOID AlarmIndication( CM_UPPER_RQB_PTR_TYPE pRQB );
	LSA_VOID ApplReadyIndication( CM_UPPER_RQB_PTR_TYPE pRQB );
	LSA_VOID SyncLedIndication( CM_UPPER_RQB_PTR_TYPE pRQB );
	LSA_VOID LinkStatusIndication( CM_UPPER_RQB_PTR_TYPE pRQB );

	// Requests
	LSA_VOID StartListening( LSA_UINT16 nrOfEvents, LSA_UINT16 nrOfSub );
	LSA_VOID StopListening();
	LSA_VOID HandleEvent( LSA_OPCODE_TYPE opcode, CM_UPPER_RQB_PTR_TYPE pRQB );

	// Class methodes ( Used for callbacks )
	static LSA_VOID EventReceived(LSA_VOID* p_inst, LSA_VOID* p_rb);
	LSA_VOID AlarmIndicationDone( LSA_VOID* p_rb );

private:
	LSA_VOID initMap();
	LSA_VOID reprovide( CM_UPPER_RQB_PTR_TYPE pRQB );

};

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif // !defined(PND_PD_EVENT_HANDLER_H)
