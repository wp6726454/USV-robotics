#ifndef PND_PD_SUBSLOT_H
#define PND_PD_SUBSLOT_H
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
/*  F i l e               &F: pnd_PDSubslot.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Definition of PDEV Subslot class                                         */
/*                                                                           */
/*****************************************************************************/

class CUserNode;

/////////////////////////////////////////////////////////////////////////////
// CPDSubslot PD subslot storage class to hold PDEV subslot specific data
/////////////////////////////////////////////////////////////////////////////

class CPDSubslot : public CUserNode
{
	// Attributes
private:
	// PNIO key numbers
	LSA_UINT16  m_portID;             // Port-ID  (key value)
	LSA_UINT16  m_laddr;              // Logical address of PDev submod
	LSA_UINT16  m_slotNr;             // Mapped slot number
	LSA_UINT16  m_subslotNr;          // Mapped subslot number
	LSA_UINT32  m_slotIdent;          // Ident of slot
	LSA_UINT32  m_subslotIdent;       // Ident of subslot

	// Sync information 
	LSA_UINT16 m_syncState;           // see CM_PD_SYNC_LED_INFO_MAX
	LSA_BOOL   m_isRateValid;
	LSA_UINT8  m_rcvSyncPrio;
	LSA_UINT8  m_localPrio;
	LSA_UINT8  m_pllState;

	// MAU-Type information
	LSA_UINT8  m_mauStatus;
	LSA_UINT8  m_mauSpeed;
	LSA_UINT8  m_mauMode;

	// Construction/ Destruction
public:
	CPDSubslot( LSA_UINT16 portID, PND_HANDLE_PTR_TYPE pHandle, CUserNode *pParent );
	virtual ~CPDSubslot();
	void Create( LSA_UINT16 slotNr, LSA_UINT16 subslotNr, LSA_UINT32 modIdent, LSA_UINT32 subIdent, LSA_UINT16 subLogAddr );

	// Accessors Read
	static string GetKey( LSA_UINT16 portID );

	LSA_UINT16 GetSlotNr();
	LSA_UINT16 GetSubslotNr();
	LSA_UINT16 GetSubslotLogAddr();
	LSA_UINT32 GetSlotIdent();
	LSA_UINT32 GetSubslotIdent();

	PND_SYNC_STATE_TYPE GetSyncState();

	// Accessors Write
	void Map( LSA_UINT16 slotNr, LSA_UINT16 subslotNr, LSA_UINT32 modIdent, LSA_UINT32 subIdent );
	void Unmap();
	void SetMauState( LSA_UINT8 status, LSA_UINT8 speed, LSA_UINT8 mode );
	void SetSyncState( LSA_UINT16 syncLedInfo, LSA_BOOL isRateValid, LSA_UINT8 rcvSyncPrio, LSA_UINT8 localPrio, LSA_UINT8 pllState );
	void UpdateFromAlarm( LSA_BOOL hasDiag, LSA_UINT32 maintenanceStatus );
	void SetState( LSA_UINT16 state );
	void SetARP();

	// Accessors to get state information for PDEV subslots
	LSA_BOOL IsMapped();
	LSA_BOOL IsLinkUp();

	// Overrides to implement UserNode methodes
	virtual string          GetKey();
	virtual LSA_UINT        GetNumber();
	virtual LSA_HANDLE_TYPE GetPnioHandle();

	// Overrides to implement UserNode PND request methodes (CM-Requests)
    virtual LSA_VOID RecordRead( PND_RQB_PTR_TYPE p_rb );
    virtual LSA_VOID RecordWrite( PND_RQB_PTR_TYPE p_rb );

	// Requests (to CM-PD)
	void ReadRecord( PND_RQB_PTR_TYPE p_rb );
	void WriteRecord( PND_RQB_PTR_TYPE p_rb );

protected:
	// Class methodes ( Used for callbacks from PNIO )
	static void ReadRecordDone( void* p_inst, void* p_rb );
	static void WriteRecordDone( void* p_inst, void* p_rb );
};

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  // !defined(PND_PD_SUBSLOT_H)
