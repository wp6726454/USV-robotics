#ifndef TCIP_USR_H
#define TCIP_USR_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  C o m p o n e n t     &C: TCPIP (TCIP for Interniche Stack)         :C&  */
/*                                                                           */
/*  F i l e               &F: tcip_usr.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  User interface                                                           */
/*                                                                           */
/*****************************************************************************/


/*------------------------------------------------------------------------------
//	OPCODES
//	TCIP_RQB_TYPE::opcode
//----------------------------------------------------------------------------*/

#define	TCIP_OPC_OPEN_CHANNEL   1
#define TCIP_OPC_CLOSE_CHANNEL  2
#define	TCIP_OPC_TIMER			3			


/*------------------------------------------------------------------------------
//	ERROR_CODES
//	TCIP_RQB_TYPE::response
//----------------------------------------------------------------------------*/

#define TCIP_OK                   LSA_RSP_OK /* (usually 0x01 but configurable) */
#define TCIP_OK_PENDING           (LSA_RSP_OK_OFFSET + 0) /* (0x40) internally used only! */

#define TCIP_ERR_RESOURCE         LSA_RSP_ERR_RESOURCE /* (0x84) */
#define TCIP_ERR_PARAM            LSA_RSP_ERR_PARAM    /* (0x86) */
#define TCIP_ERR_SEQUENCE         LSA_RSP_ERR_SEQUENCE /* (0x87) */
#define TCIP_ERR_SYS_PATH         LSA_RSP_ERR_SYS_PATH /* (0x91) */

#define TCIP_ERR_LOWER_LAYER      (LSA_RSP_ERR_OFFSET + 0) /* (0xc0) */ 


/*-----------------------------------------------------------------------------
//---------------------------------------------------------------------------*/

typedef struct tcip_rqb_tag TCIP_UPPER_RQB_ATTR * TCIP_UPPER_RQB_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: TCIP_OPC_OPEN_CHANNEL
//---------------------------------------------------------------------------*/

#define TCIP_INVALID_HANDLE  ((LSA_HANDLE_TYPE)-1) /* NOTE: do not check with #if, CPP sees type-cast as an undefined symbol and assumes zero... */

typedef LSA_VOID  LSA_FCT_PTR(TCIP_UPPER_OUT_FCT_ATTR, TCIP_UPPER_CALLBACK_FCT_PTR_TYPE)(TCIP_UPPER_RQB_PTR_TYPE rb);

typedef struct tcip_open_channel_tag {
	LSA_HANDLE_TYPE		handle;						/* cnf: channel-handle of TCIP */
	LSA_HANDLE_TYPE		handle_upper;				/* req: channel-handle of user, every confirmation of an request set this handle to rqb.handle */
	LSA_SYS_PATH_TYPE	sys_path;					/* req: sys_path of channel */
	TCIP_UPPER_CALLBACK_FCT_PTR_TYPE
						tcip_request_upper_done_ptr;	/* req: pointer to the callback-function */
} TCIP_OPEN_CHANNEL_TYPE;

typedef TCIP_OPEN_CHANNEL_TYPE TCIP_UPPER_MEM_ATTR * TCIP_UPPER_OPEN_CHANNEL_PTR_TYPE;


/*-----------------------------------------------------------------------------
// OPCODE: TCIP_OPC_CLOSE_CHANNEL
//---------------------------------------------------------------------------*/

#if 0
typedef struct tcip_close_channel_tag {
	no parameters
} TCIP_CLOSE_CHANNEL_TYPE;
#endif


/*-----------------------------------------------------------------------------
// OPCODE: TCIP_OPC_TIMER
//---------------------------------------------------------------------------*/

typedef struct tcip_timer_tag {

	volatile long in_use; /* note: native long, see TCIP_EXCHANGE_LONG() */

} TCIP_TIMER_TYPE;

typedef TCIP_TIMER_TYPE TCIP_UPPER_MEM_ATTR * TCIP_UPPER_TIMER_PTR_TYPE;


/*------------------------------------------------------------------------------
// TCIP_RQB_TYPE .. ReQuestBlock
//----------------------------------------------------------------------------*/

typedef struct tcip_rqb_tag {
	TCIP_RQB_HEADER  /* first in struct, see tcip_cfg.h */

	union {
		TCIP_OPEN_CHANNEL_TYPE   open;
		/* TCIP_CLOSE_CHANNEL_TYPE  close; -- no parameters, no type */
		TCIP_TIMER_TYPE          timer;
	} args;

	TCIP_RQB_TRAILER /* last in struct, see tcip_cfg.h */
} TCIP_RQB_TYPE;


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  tcip_open_channel
 *
 * function:       open a communication channel
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_VOID  TCIP_UPPER_IN_FCT_ATTR
tcip_open_channel(
    TCIP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);


/*=============================================================================
 * function name:  tcip_close_channel
 *
 * function:       close a communication channel
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_VOID  TCIP_UPPER_IN_FCT_ATTR
tcip_close_channel(
    TCIP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);


/*=============================================================================
 * function name:  tcip_request
 *
 * function:       send a request to a communication channel
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_VOID  TCIP_UPPER_IN_FCT_ATTR
tcip_request(
    TCIP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);


/*=============================================================================
 * function name:  tcip_set_ip_suite
 *
 * function:       sets the ip address for a network interface
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_BOOL  TCIP_UPPER_IN_FCT_ATTR
tcip_set_ip_suite(
	LSA_UINT32  interface_id,
	LSA_UINT32	ip_address,
	LSA_UINT32	subnet_mask,
	LSA_UINT32	gateway
);

/*=============================================================================
 * function name:  tcip_get_any_interface_mac_addr
 *
 * function:       returns the mac address for an existing network interface
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_BOOL  TCIP_UPPER_IN_FCT_ATTR
tcip_get_any_interface_mac_addr(
    LSA_UINT32* p_interface_id,
	LSA_UINT8*	p_mac_address,
	LSA_UINT16	mac_address_length
);

/*=============================================================================
 * function name:  tcip_get_timeparams
 *
 * function:       returns the TODO
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

LSA_VOID TCIP_UPPER_IN_FCT_ATTR
tcip_get_timeparams(
	LSA_UINT32 *p_keepalivetime,
	LSA_UINT32 *p_connectiontimeout,
	LSA_UINT32 *p_retransmittimeout,
	LSA_UINT32 *p_retransmitcount
);

/*=============================================================================
 * function name:  tcip_set_timeparams
 *
 * function:       returns the TODO
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

LSA_UINT16 TCIP_UPPER_IN_FCT_ATTR
tcip_set_timeparams(
	LSA_UINT32 keepalivetime,
	LSA_UINT32 connectiontimeout,
	LSA_UINT32 retransmittimeout,
	LSA_UINT32 retransmitcount,
	LSA_UINT32 no_change_value
);

/*=============================================================================
 * function name:  tcip_get_mib2_ifindex
 *
 * function:       returns interface index
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_BOOL  TCIP_UPPER_IN_FCT_ATTR
tcip_get_mib2_ifindex(
	LSA_UINT32  interface_id,
	LSA_UINT16  port_id,
	LSA_UINT32* IfIndex
);


/*=============================================================================
 * function name:  tcip_set_mib2_ifgroup_data
 *
 * function:       sets interface description string for one interface
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_BOOL TCIP_UPPER_IN_FCT_ATTR 
tcip_set_mib2_ifgroup_data(
	LSA_UINT32 interface_id,
	LSA_UINT16 port_id,
	LSA_UINT8* p_ifDescr,
	LSA_UINT16 ifDescr_length
);

/*=============================================================================
 * function name:  tcip_set_mib2_sysgroup_data
 *
 * function:       sets parameters of mib2-system-group
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_BOOL TCIP_UPPER_IN_FCT_ATTR 
tcip_set_mib2_sysgroup_data(
	LSA_UINT8* p_sysName,
	LSA_UINT16 sysName_length,
	LSA_UINT8* p_sysDescr,
	LSA_UINT16 sysDescr_length,
	LSA_UINT8* p_sysContact,
	LSA_UINT16 sysContact_length,
	LSA_UINT8* p_sysLocation,
	LSA_UINT16 sysLocation_length
);

/*=============================================================================
 * function name:  tcip_mib2_statistics_update
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_BOOL TCIP_UPPER_IN_FCT_ATTR 
tcip_mib2_statistics_update(
	LSA_VOID
);

/*====  out functions  =====*/

/*=============================================================================
 * function name:  TCIP_REQUEST_UPPER_DONE
 *
 * function:       returns a finished request to the upper layer
 *
 * parameters:     TCIP_UPPER_CALLBACK_FCT_PTR_TYPE
 *                     tcip_request_upper_done_ptr:
 *                     pointer to tcip_request_upper_done_ptr, given by
 *                     tcip_open_channel()
 *
 *                 TCIP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-RQB
 *                 LSA_SYS_PTR_TYPE       sys_ptr:        system-pointer
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
#ifndef TCIP_REQUEST_UPPER_DONE
LSA_VOID  TCIP_UPPER_OUT_FCT_ATTR  TCIP_REQUEST_UPPER_DONE(
    TCIP_UPPER_CALLBACK_FCT_PTR_TYPE  tcip_request_upper_done_ptr,
    TCIP_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
    LSA_SYS_PTR_TYPE				  sys_ptr
);
#endif


/*=============================================================================
 * function name:  TCIP_ALLOC_UPPER_RQB_LOCAL
 *
 * function:       allocate an upper rqb for usage in tcip
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_ALLOC_UPPER_RQB_LOCAL
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR TCIP_ALLOC_UPPER_RQB_LOCAL(
    TCIP_UPPER_RQB_PTR_TYPE  TCIP_LOCAL_MEM_ATTR * upper_rqb_ptr_ptr,
    LSA_UINT16									   length
);
#endif

/*=============================================================================
 * function name:  TCIP_FREE_UPPER_RQB_LOCAL
 *
 * function:       allocate an upper rqb for usage in tcip
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_FREE_UPPER_RQB_LOCAL
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR TCIP_FREE_UPPER_RQB_LOCAL(
    LSA_UINT16                 TCIP_LOCAL_MEM_ATTR * ret_val_ptr,
    TCIP_UPPER_RQB_PTR_TYPE                          upper_rqb_ptr
);
#endif


/*=============================================================================
 * function name:  TCIP_ALLOC_UPPER_RQB
 *
 * function:       allocate an upper-RQB
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_ALLOC_UPPER_RQB
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_ALLOC_UPPER_RQB(
    TCIP_UPPER_RQB_PTR_TYPE  TCIP_LOCAL_MEM_ATTR   *  upper_rqb_ptr_ptr,
    LSA_USER_ID_TYPE                              user_id,
    LSA_UINT16                                    length,
    LSA_SYS_PTR_TYPE                              sys_ptr
);
#endif


/*=============================================================================
 * function name:  TCIP_FREE_UPPER_RQB
 *
 * function:       free an upper-RQB
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#ifndef TCIP_FREE_UPPER_RQB
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_FREE_UPPER_RQB(
    LSA_UINT16                 TCIP_LOCAL_MEM_ATTR   *  ret_val_ptr,
    TCIP_UPPER_RQB_PTR_TYPE                             upper_rqb_ptr,
    LSA_SYS_PTR_TYPE                                  sys_ptr
);
#endif


/*=============================================================================
 * function name:  TCIP_REQUEST_LOCAL
 *
 * function:       send a request to "self"
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 * maybe this should go into tcip_sys.h, but there is no upper-rqb-type...
 *
 *===========================================================================*/

#ifndef TCIP_REQUEST_LOCAL
LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_REQUEST_LOCAL(
    TCIP_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);
#endif


/*=============================================================================
 * function name:  TCIP_TRIGGER_SELECT
 *
 * function:       usage see SOCK
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

LSA_VOID  TCIP_SYSTEM_OUT_FCT_ATTR
TCIP_TRIGGER_SELECT(
	LSA_VOID
);


/*=============================================================================
 * function name:  TCIP_MIB2_SYSGROUP_DATA_WRITTEN
 *
 * function:       usage see SOCK
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/

LSA_VOID TCIP_UPPER_OUT_FCT_ATTR 
TCIP_MIB2_SYSGROUP_DATA_WRITTEN(
	LSA_CHAR LSA_COMMON_MEM_ATTR * ptr_sysName,
	LSA_CHAR LSA_COMMON_MEM_ATTR * ptr_sysContact,
	LSA_CHAR LSA_COMMON_MEM_ATTR * ptr_sysLocation
);


/*=============================================================================
 * function name:  TCIP_MIB2_STATISTICS_UPDATE_DONE
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
LSA_VOID TCIP_UPPER_OUT_FCT_ATTR 
TCIP_MIB2_STATISTICS_UPDATE_DONE(
	LSA_VOID
);


/*=============================================================================
 * function name:  tcip_srv_multicast, TCIP_SRV_MULTICAST_DONE
 *
 * documentation:  LSA_TCIP_Detailspec.doc
 *
 *===========================================================================*/
#if TCIP_CFG_ENABLE_MULTICAST

#define TCIP_SRV_MULTICAST_ENABLE  1
#define TCIP_SRV_MULTICAST_DISABLE 2

LSA_RESPONSE_TYPE TCIP_UPPER_IN_FCT_ATTR
tcip_srv_multicast(
	LSA_INT32 dev_handle,
	LSA_UINT32 interface_id,
	LSA_UINT32 mc_address, /* (!) network byte order */
	LSA_UINT8  mode
);

LSA_VOID TCIP_UPPER_OUT_FCT_ATTR 
TCIP_SRV_MULTICAST_DONE(
	LSA_RESPONSE_TYPE result,
	LSA_INT32 dev_handle,
	LSA_UINT8 mode
);

#endif

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of TCIP_USR_H */
