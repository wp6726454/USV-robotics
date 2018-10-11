#ifndef PND_OHA_EVENT_HANDLER_H
#define PND_OHA_EVENT_HANDLER_H
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
/*  F i l e               &F: pnd_OHAEventHandler.h                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Definition of OHA Event Handler classes                                  */
/*  (Listener and Handler for OHA Events)                                    */
/*                                                                           */
/*****************************************************************************/


#define PND_OHA_OPC_MAX (OHA_OPC_LLDP_PORT_CONFIG_RESPONSE +1) // see oha_usr.h for last opcode

class COHAUser;

/////////////////////////////////////////////////////////////////////////////
// COHAEventHandler
/////////////////////////////////////////////////////////////////////////////

class COHAEventHandler
{
	// Attributes
private:
	LSA_BOOL         m_isStopping;   // OHA channel close requested
	PND_HANDLE_TYPE  m_pndHandle;    // PND handle
	COHAUser        *m_pUser;        // Link to OHA User
    OHA_UPPER_RQB_PTR_TYPE m_responseRQB; // Stored RQB for Rema Indication
	void (COHAEventHandler::*m_handler[PND_OHA_OPC_MAX])(OHA_UPPER_RQB_PTR_TYPE p_rb );  // Pointer to response handler

	// Construction/ Destruction
public:
	COHAEventHandler( PND_HANDLE_PTR_TYPE pHandle, COHAUser *pUser);
	virtual ~COHAEventHandler();

	// Accessors

	// EventHandling methode (CM-Indication)
	void ErrorEventHandling( OHA_UPPER_RQB_PTR_TYPE pRQB );
	void DcpIndication( OHA_UPPER_RQB_PTR_TYPE pRQB );
	void DcpQueryResponse( OHA_UPPER_RQB_PTR_TYPE pRQB );
	void LldpIndication( OHA_UPPER_RQB_PTR_TYPE pRQB );
	void HelloIndication( OHA_UPPER_RQB_PTR_TYPE pRQB );
	void AddrInfoIndication( OHA_UPPER_RQB_PTR_TYPE pRQB );
	void PrmIndication( OHA_UPPER_RQB_PTR_TYPE pRQB );
	void RemaIndication( OHA_UPPER_RQB_PTR_TYPE pRQB );
	void LldpConfigIndication( OHA_UPPER_RQB_PTR_TYPE pRQB );
	void LldpPortConfigIndication( OHA_UPPER_RQB_PTR_TYPE pRQB );

	// Requests
	void StartListening();
	void StopListening();
	void HandleEvent(LSA_OPCODE_TYPE opcode, OHA_UPPER_RQB_PTR_TYPE pRQB );

	// Class methodes ( Used for callbacks )
	static void EventReceived(void* p_inst, void* p_rb);
	void RemaIndicationDone();
private:
	void initMap();
	void reprovide( OHA_UPPER_RQB_PTR_TYPE pRQB );

};

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif // !defined(PND_OHA_EVENT_HANDLER_H)
