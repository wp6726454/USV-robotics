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
/*  F i l e               &F: pnd_iob_core.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
#ifndef PND_IOB_CORE_H                     /* ----- reinclude-protection ----- */
#define PND_IOB_CORE_H

#include "pniobase.h"
#include "servusrx.h"
#include "pniobase.h"
#include "pniousrx.h"
#include "pnd_iodu.h"
#include "pnd_pnio_user_core.h"


#define PND_INVALID_HANDLE  0xFFFF
#define MAX_DEVICE_ACTIVATION_SIZE 8
/*****************************************************************************
Class for controller Instances
*****************************************************************************/

typedef enum {
    IDLE         = 0,
    DEACT        = 1,
    TAILOR       = 2,
    TAILOR_WRITE = 3,
    TAILOR_ACT   = 4 
} IOSYSTEM_RECON_ENUM;




enum pnd_iob_controller_status
{
    pnd_controller_closed = 0,
    pnd_controller_opened
};

enum pnd_iob_interface_status
{
    pnd_interface_closed = 0,
    pnd_interface_opened
};

typedef vector<PNIO_UINT32> LogAddrVector; // PendingActivationList, InProgressActivationList

class IController {

    /* misc bits */
    PNIO_CBF m_pUserCbf_Mode;
    PNIO_CBF m_pUserCbf_DevAct;
    PNIO_CBF m_pUserCbf_Alarm;
    PNIO_CBF m_pUserCbf_DR_Read;
    PNIO_CBF m_pUserCbf_DR_Write;
    PNIO_CBF m_pCbf_AlarmInd;
	PNIO_CBF m_pUserCbf_DiagReq;
	PNIO_CBF m_pUserCbf_IosytemReconfig;
    PNIO_UINT32 m_ExtPar;
    PNIO_UINT32 m_CpIndex;
    PNIO_MODE_TYPE  m_Mode;      /* Actual mode */
	PNIO_IOS_RECONFIG_MODE m_iOSystemReconfigMode;
	PNIO_UINT32 m_pnioErrorCodeOfLastFailedDevice;
	PNIO_ADDR m_laddrOfLastFailedDevice;

    public:

    IController(PNIO_UINT32);
    ~IController(void);

    pnd_iob_controller_status m_status;

    pnd_iob_controller_status m_get_status(PNIO_VOID);
    PNIO_VOID                  m_set_status(pnd_iob_controller_status status);

    static PNIO_VOID remove_instance(PNIO_UINT16);
    static IController *get_instance(PNIO_UINT32 CpIndex);
    static CIODU *get_IODU(PNIO_UINT32 CpIndex);
    static PNIO_UINT32 get_handle_by_index(PNIO_UINT32 CpIndex);
    static PNIO_UINT32 get_handle( IController * pThis);
    
    PNIO_UINT32     register_cbf(PNIO_CBE_TYPE CbeType, PNIO_CBF cbf);
    PNIO_VOID        de_register_all_cbf();

    PND_RQB_PTR_TYPE pnd_lower_rqb_alloc( LSA_OPCODE_TYPE opcode, LSA_HANDLE_TYPE handle, PNIO_VOID_PTR_TYPE pInst,PNIO_VOID(*func)(PNIO_VOID*_this, PNIO_VOID*_rqb) );
    PNIO_VOID         pnd_lower_rqb_free( PNIO_VOID_PTR_TYPE pRQB );

    PNIO_UINT32    pnd_test_rqb_queue( PND_RQB_PTR_TYPE  *rqb_queue,  PND_RQB_PTR_TYPE  rqb );
    PNIO_UINT16    pnd_add_to_rqb_queue( PND_RQB_PTR_TYPE  *rqb_queue, PND_RQB_PTR_TYPE  rqb );
    PNIO_UINT16    pnd_get_from_rqb_queue( PND_RQB_PTR_TYPE  *rqb_queue, PND_RQB_PTR_TYPE  *rqb );
    
    static PNIO_UINT32 controller_open(PNIO_UINT32 CpIndex,
        PNIO_UINT32 ExtPar,
        PNIO_CBF cbf_RecReadConf,
        PNIO_CBF cbf_RecWriteConf,
        PNIO_CBF cbf_AlarmInd,
        PNIO_UINT32 *UserHandle);

    static PNIO_UINT32 controller_close(PNIO_UINT32 UserHandle);
    
    PNIO_UINT32 set_mode( PNIO_MODE_TYPE Mode);
    PNIO_UINT32 device_activate(const PNIO_ADDR * pAddr, PNIO_DEV_ACT_TYPE mode);
    PNIO_UINT32 record_read(PNIO_ADDR * pAddr, PNIO_REF ReqRef, PNIO_UINT32 RecordIndex, PNIO_UINT32 Length);
    PNIO_UINT32 record_write(PNIO_ADDR * pAddr, PNIO_REF ReqRef, PNIO_UINT32 RecordIndex, PNIO_UINT32 Length, PNIO_UINT8 * pBuffer);
    PNIO_UINT32 check_log_adress(PNIO_ADDR * pAddr);
	PNIO_UINT32	ctrl_diag_request(LSA_HANDLE_TYPE handle, PNIO_CTRL_DIAG * pDiagInfo);
	PNIO_UINT32 iosystem_reconfig(PNIO_UINT32 handle, PNIO_IOS_RECONFIG_MODE Mode, PNIO_UINT32 DeviceCnt, PNIO_ADDR * DeviceList, PNIO_UINT32 PortInterconnectionCnt, PNIO_ADDR * PortInterconnectionList);
    PNIO_VOID    iosystem_reconfig_sm(PNIO_VOID_PTR_TYPE  pRqb);
    
    static PNIO_UINT32 alarm_provide(PNIO_VOID_PTR_TYPE pInst, PNIO_UINT16 nrOfAlarmResource);
    static PNIO_VOID   alarm_indication(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb);
    static PNIO_VOID   alarm_response(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb);

	static PNIO_VOID write_snmp_record(PNIO_VOID_PTR_TYPE pInst);

    static PNIO_VOID controller_open_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE rqb_ptr);
    static PNIO_VOID controller_close_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE rqb_ptr);
    static PNIO_VOID set_mode_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE pRqb);
    static PNIO_VOID device_activate_done(PNIO_VOID_PTR_TYPE  pInst, PNIO_VOID_PTR_TYPE  pRqb);
    static PNIO_VOID read_write_record_done(PNIO_VOID_PTR_TYPE  pInst, PNIO_VOID_PTR_TYPE  pRqb);
	static PNIO_VOID ctrl_diag_req_done(PNIO_VOID_PTR_TYPE  pInst, PNIO_VOID_PTR_TYPE  pRqb);
	static PNIO_VOID iosystem_reconfig_done(PNIO_VOID_PTR_TYPE  pInst, PNIO_VOID_PTR_TYPE  pRqb);

    protected:

    PNIO_UINT16   m_hInstanceHandle;
    PNIO_UINT16   m_pnd_ioc_wait_event;  // Wait event
    PNIO_UINT16   m_nr_of_alarm_res;
    PNIO_UINT16    m_pndDevNumberList[PND_LADDR_MAX];     // Device number list for each HD/IF
	PNIO_UINT16    m_pndSlotNumberList[PND_LADDR_MAX];    // Slot number list for each HD/IF
	PNIO_UINT16    m_pndSubslotNumberList[PND_LADDR_MAX]; // Subslot number list for each HD/IF
    PNIO_BOOL      m_set_mode_req_running;
    
    private:

	LogAddrVector              m_MandatoryDeviceList;
	LogAddrVector              m_OptionalDeviceList;
	LogAddrVector              m_PendingActivationList;
	LogAddrVector              m_InProgressActivationList;

    PNIO_IOS_RECONFIG_PTR_TYPE m_IosReconfigParams;
    IOSYSTEM_RECON_ENUM        m_state_iosystem_reconfig;
    PNIO_BOOL                   m_iosys_reconfig_running;

    PNIO_VOID   IOSysRcfg_PrepareDeactPendingList( LogAddrVector& pendingList );
    PNIO_VOID   IOSysRcfg_PrepareDeActRequest( PNIO_ADDR &laddr, PNIO_DEV_ACT_TYPE actType, PNIO_VOID *rqb );
    PNIO_VOID   IOSysRcfg_PrepareReadReq( PNIO_ADDR& laddr, PNIO_VOID *rqb );
    PNIO_UINT32 IOSysRcfg_EvaluateReadReq( PNIO_VOID *rqb );
    PNIO_VOID   IOSysRcfg_PrepareWriteReq( PNIO_ADDR& laddr, PNIO_VOID *rqb );
    PNIO_UINT32 IOSysRcfg_EvaluateWriteReq( PNIO_VOID *rqb );
    PNIO_VOID   IOSysRcfg_PrepareActPendingList( LogAddrVector& pendingList );
    PNIO_UINT32 IOSysRcfg_StoreUserParams( PNIO_UINT32 DeviceCnt, PNIO_ADDR * DeviceList, 
								           PNIO_UINT32 PortInterconnectionCnt, PNIO_ADDR * PortInterconnectionList );
    PNIO_VOID   IOSysRcfg_FreeUserParams();
    PNIO_UINT32 IOSysRcfg_MapCmErrToPnioErr(PNIO_ERR_STAT err);
    PNIO_VOID   IOSysRcfg_CreateMandatoryAndOptionalDeviceList( PNIO_VOID_PTR_TYPE record );
    PNIO_VOID   IOSysRcfg_CreatePDInstanceTailorData( PNIO_VOID_PTR_TYPE *ppPDInstanceTailorDataBuffer, 
                                         PNIO_UINT32 *pPDInstanceTailorDataBufferLen );

    PNIO_UINT32 GetLogAddrOfInterfaceSubmodule();
    PNIO_UINT32 GetNextDeviceFromPendingList( PNIO_ADDR *laddr, LogAddrVector& pendingList, 
														        LogAddrVector& inProgressList );
    PNIO_VOID   RemoveDeviceFromInProgressList( PNIO_ADDR& laddr, LogAddrVector& inProgressList );
    PNIO_UINT32 IsInProgressListNotFull( LogAddrVector& inProgressList );
    PNIO_VOID   AddDeviceToInProgressList( PNIO_UINT32 laddr, LogAddrVector& inProgressList );
};

class IIOInterface {

    /* misc bits */
    PNIO_CBF m_pUserCbf_Alarm;
    PNIO_CBF m_pUserCbf_DR_Read;
	PNIO_CBF m_pUserCbf_SetIpAndNos;
    PNIO_CBF m_pUserCbf_RemaRead;
    PNIO_UINT32 m_CpIndex;
	PNIO_BOOL m_set_ip_and_nos_running;

    public:

    IIOInterface(PNIO_UINT32);
    ~IIOInterface(void);

	pnd_iob_interface_status m_status;

    pnd_iob_interface_status m_get_status(PNIO_VOID);
    PNIO_VOID m_set_status(pnd_iob_interface_status status);

    static PNIO_VOID remove_instance(PNIO_UINT16);
	static IIOInterface *get_instance(PNIO_UINT32 CpIndex);  //IIOInterface
    static PNIO_UINT32 get_handle_by_index(PNIO_UINT32 CpIndex);
	static PNIO_UINT32 get_handle( IIOInterface * pThis);
    
    PNIO_UINT32 register_cbf(PNIO_CBE_TYPE CbeType, PNIO_CBF cbf);
    PNIO_VOID de_register_all_cbf();
    
    PND_RQB_PTR_TYPE pnd_lower_rqb_alloc( LSA_OPCODE_TYPE opcode, LSA_HANDLE_TYPE handle, PNIO_VOID_PTR_TYPE pInst,PNIO_VOID(*func)(PNIO_VOID*_this, PNIO_VOID*_rqb) );
    PNIO_VOID        pnd_lower_rqb_free( PNIO_VOID_PTR_TYPE pRQB );

	static PNIO_UINT32 interface_open(PNIO_UINT32 CpIndex,
        PNIO_CBF cbf_RecReadConf,
        PNIO_CBF cbf_AlarmInd,
        PNIO_UINT32 *UserHandle);

    static PNIO_UINT32 interface_close(PNIO_UINT32 UserHandle);
    
   
    PNIO_UINT32 record_read(PNIO_ADDR * pAddr, PNIO_REF ReqRef, PNIO_UINT32 RecordIndex, PNIO_UINT32 Length);
    PNIO_UINT32 check_log_adress(PNIO_ADDR * pAddr);
	PNIO_UINT32	set_ip_and_nos(PNIO_UINT32 handle, PNIO_SET_IP_NOS_MODE_TYPE Mode, PNIO_IPv4 IpV4, PNIO_NOS NoS);

    static PNIO_UINT32 alarm_provide(PNIO_VOID_PTR_TYPE pInst, PNIO_UINT16 nrOfAlarmResource);
    static PNIO_VOID   alarm_indication(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb);
    static PNIO_VOID   alarm_response(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb);

    static PNIO_UINT32 rema_provide(PNIO_VOID_PTR_TYPE pInst, PNIO_UINT16 nrOfRemaResource);
	static PNIO_VOID rema_response(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  rqb_ptr);

    static PNIO_VOID interface_open_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE rqb_ptr);
    static PNIO_VOID interface_close_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE rqb_ptr);
    static PNIO_VOID record_read_done(PNIO_VOID_PTR_TYPE  pInst, PNIO_VOID_PTR_TYPE  pRqb);
	static PNIO_VOID set_ip_and_nos_done(PNIO_VOID_PTR_TYPE  pInst, PNIO_VOID_PTR_TYPE  pRqb);
    static PNIO_VOID ifc_rema_done(PNIO_VOID_PTR_TYPE  pInst, PNIO_VOID_PTR_TYPE  pRqb);

    protected:

    PNIO_UINT16   m_hInstanceHandle;
    PNIO_UINT16   m_pnd_pd_wait_event;  // Wait event
    PNIO_UINT16   m_nr_of_alarm_res;
	PNIO_UINT16   m_nr_of_rema_res;
    
};

/*---------------------------------------------------------------------*/

void pnd_iobase_core_init(void);
void pnd_iobase_core_undo_init(void);

void pnd_iobase_core_startup(PNIO_VOID_PTR_TYPE rqb_ptr);

void pnd_iobase_core_usr_enter(void);
void pnd_iobase_core_usr_exit(void);

PNIO_UINT32 pnd_iob_core_get_handle(PNIO_UINT32 CpIndex);
IController* pnd_iob_core_get_instance(PNIO_UINT32 CpIndex);

PNIO_VOID pnd_iob_core_request( PND_RQB_PTR_TYPE  rqb_ptr );
PNIO_VOID pnd_iob_core_request_done( PND_RQB_PTR_TYPE  rqb_ptr );

/*---------------------------------------------------------------------*/
/*macros */
/*---------------------------------------------------------------------*/

#define PND_IOB_USR_ENTER pnd_iobase_core_usr_enter();
#define PND_IOB_USR_EXIT  pnd_iobase_core_usr_exit();

#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/