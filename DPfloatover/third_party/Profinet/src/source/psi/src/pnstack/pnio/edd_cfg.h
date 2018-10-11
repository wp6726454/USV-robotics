#ifndef EDD_CFG_H                        /* ----- reinclude-protection ----- */
#define EDD_CFG_H

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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: edd_cfg.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Configuration for EDD:                                                   */
/*  Defines constants, types and macros for edd.                             */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some definitions depend on the different system, compiler or             */
/*  operating system.                                                        */ 
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDD_MESSAGE
/*  13.08.08    JS    initial version.                                       */
/*  27.09.10    JS    added parameter EDD_CFG_MAX_INTERFACE_CNT              */
/*  17.01.11    JS    removed revisions from header                          */
/*                    added EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE               */
#endif
/*****************************************************************************/



/*---------------------------------------------------------------------------*/
/*  PSI adaption function for all EDDx                                       */
/*---------------------------------------------------------------------------*/

typedef LSA_VOID (*EDD_REQUEST_FCT)( LSA_VOID_PTR_TYPE );

/* export some EDDx common functions used by upper layers { */
/* marked for DELETION */
LSA_VOID EDD_ALLOC_DEV_MEM(
	LSA_VOID_PTR_TYPE*  lower_mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_UINT16          lsa_comp_id
);

/* marked for DELETION */
LSA_VOID EDD_FREE_DEV_MEM(
	LSA_UINT16          *ret_val_ptr,
	LSA_VOID_PTR_TYPE    lower_mem_ptr,
	LSA_SYS_PTR_TYPE     pSys,
	LSA_UINT16           lsa_comp_id
); 

/*---------------------------------------------------------------------------*/

LSA_VOID psi_edd_alloc_nrt_tx_mem(
	LSA_VOID_PTR_TYPE*  lower_mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_UINT16          lsa_comp_id
);

LSA_VOID psi_edd_alloc_nrt_rx_mem(
	LSA_VOID_PTR_TYPE*  lower_mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_UINT16          lsa_comp_id
);

LSA_VOID psi_edd_free_nrt_tx_mem(
	LSA_UINT16          *ret_val_ptr,
	LSA_VOID_PTR_TYPE    lower_mem_ptr,
	LSA_SYS_PTR_TYPE     pSys,
	LSA_UINT16           lsa_comp_id
); 

LSA_VOID psi_edd_free_nrt_rx_mem(
	LSA_UINT16          *ret_val_ptr,
	LSA_VOID_PTR_TYPE    lower_mem_ptr,
	LSA_SYS_PTR_TYPE     pSys,
	LSA_UINT16           lsa_comp_id
); 

/*---------------------------------------------------------------------------*/

LSA_UINT16 edd_get_mailbox_id( // Lookup for Mailbox ID (dependend on EDDx)
	LSA_VOID_PTR_TYPE rqb_ptr, 
	LSA_UINT16        comp_id_upper,
	LSA_SYS_PTR_TYPE  sys_ptr 
);

EDD_REQUEST_FCT edd_get_open_fct( // Lookup for open channel function
	LSA_UINT16 comp_id_edd
);

EDD_REQUEST_FCT edd_get_close_fct( // Lookup for close channel function
	LSA_UINT16 comp_id_edd
);

EDD_REQUEST_FCT edd_get_request_fct( // Lookup for request function
	LSA_UINT16 comp_id_edd
);

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


/*===========================================================================*/
/*                              defines                                      */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Maximum number of interfaces of all EDDs integrated                       */
/* Note: This parameter is intended to be used by upper components to limit  */
/*       the number of supported EDD interfaces. This parameter is not used  */
/*       by the EDD.                                                         */
/*---------------------------------------------------------------------------*/

#ifndef EDD_CFG_MAX_INTERFACE_CNT
# error "EDD_CFG_MAX_INTERFACE_CNT not defined"
#endif

/*---------------------------------------------------------------------------*/
/* Maximum number of ports supported by EDD API services                     */
/* Note: This must be the maximum port count possbile of a EDD device!       */
/*       This define is used to define the EDD User API array sizes.         */
/*       If one EDD supports more ports, the EDD will signal an runtime      */
/*       fatal error!                                                        */
/* Note: With EDDP we can have up to 4 ports per Device.                     */
/*---------------------------------------------------------------------------*/

#ifndef EDD_CFG_MAX_PORT_CNT
# error "EDD_CFG_MAX_PORT_CNT not defined"
#endif

/*---------------------------------------------------------------------------*/
/* Maximum number of ports over all interfaces of all EDDs                   */
/* Maximum: EDD_CFG_MAX_INTERFACE_CNT * EDD_CFG_MAX_PORT_CNT                 */
/* Note: If the maximum number of all ports over all interfaces is known this*/
/*       define can be used to define the limit. Applications can use this   */
/*       define to limit local memory management.                            */
/* Note: The EDD does not use/validate this define!                          */
/*---------------------------------------------------------------------------*/

#ifndef EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE
# error "EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE not defined"
#endif

/*---------------------------------------------------------------------------*/
/* Maximum number of MRP instances supported with a EDD interface            */
/*---------------------------------------------------------------------------*/

#ifndef EDD_CFG_MAX_MRP_INSTANCE_CNT
# error "EDD_CFG_MAX_MRP_INSTANCE_CNT not defined"
#endif

/*---------------------------------------------------------------------------*/
/* Because of the zero-copy - interface we must take care of receive buffer  */
/* size requirements from the ethernetcontroller within application.         */
/*                                                                           */
/* EDD_CFG_RX_FRAME_BUFFER_LENGTH defines the maximum receive framebuffer    */
/* requirement of all Ethernetcontroller within the system. This define is   */
/* set to the EDD define EDD_FRAME_BUFFER_LENGTH which shall be used to      */
/* allocate receive buffers by any EDD application.                          */
/*                                                                           */
/* Current requirements                                                      */
/*                                                                           */
/* EDDP:         1536                                                        */
/* EDDI:         1536                                                        */
/* EDDS:         LLIF/MAC dependend                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#if ( PSI_CFG_USE_EDDS == 1 )
#define EDD_CFG_RX_FRAME_BUFFER_LENGTH      2048 /* EDDS - Intel LLIF */
#else
#define EDD_CFG_RX_FRAME_BUFFER_LENGTH      1536
#endif

/*---------------------------------------------------------------------------*/
/* Max number of entrys in one indication resource for cyclic SRT.           */
/* (see service EDD_SRV_SRT_IND_PROVIDE)                                     */
/* (1..0xFFFF)                                                               */
/*---------------------------------------------------------------------------*/

#define EDD_CFG_CSRT_MAX_INDICATIONS    64

/*---------------------------------------------------------------------------*/
/* Max number of provider groups supported.                                  */
/* (1..0xFFFF)                                                               */
/*---------------------------------------------------------------------------*/

#define EDD_CFG_CSRT_MAX_PROVIDER_GROUP 8

/*---------------------------------------------------------------------------*/
/* Max number of providers per interface supported.                          */
/* (1..0x7FFF)                                                               */
/*---------------------------------------------------------------------------*/

#define EDD_CFG_MAX_NR_PROVIDERS  1024

/*---------------------------------------------------------------------------*/
/* Max number of ARSets per interface supported.                             */
/* (1..0xFF)                                                                 */
/*---------------------------------------------------------------------------*/

#define EDD_CFG_MAX_NR_ARSETS  4

/*===========================================================================*/
/*                              basic attributes                             */
/*===========================================================================*/

#define EDD_ATTR_NEAR
#define EDD_ATTR_FAR
#define EDD_ATTR_SHUGE
#define EDD_ATTR_HUGE

/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/

/*=====  code attributes  =====*/

#define EDD_UPPER_IN_FCT_ATTR    EDD_ATTR_HUGE  /* prefix-user to prefix */
#define EDD_UPPER_OUT_FCT_ATTR   EDD_ATTR_HUGE  /* prefix to prefix-user */

/*=====  data attributes  =====*/

#define EDD_UPPER_RQB_ATTR       EDD_ATTR_HUGE  /* upper-rqb data */
#define EDD_UPPER_MEM_ATTR       EDD_ATTR_HUGE  /* upper-memory data */
#define EDD_COMMON_MEM_ATTR      EDD_ATTR_HUGE  /* greatest common data   */
                                                /* attribute (matches all)*/

/*---------------------------------------------------------------------------*/
/* Definition of RQB-HEADER:                                                 */
/*---------------------------------------------------------------------------*/

typedef LSA_VOID(*PSI_EDD_REQUEST_DONE_CBF_TYPE)(LSA_VOID*);

#define EDD_RQB_HEADER LSA_RQB_HEADER(EDD_UPPER_RQB_PTR_TYPE) \
                       PSI_EDD_REQUEST_DONE_CBF_TYPE psi_cbf; \
                       LSA_VOID*  psi_sys_ptr;

/*---------------------------------------------------------------------------*/
/* Definition of RQB-TRAILER                                                 */
/*                                                                           */
/* If necessary during system integration, additional parameters can be      */
/* added!                                                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define EDD_RQB_TRAILER LSA_RQB_TRAILER 

/*---------------------------------------------------------------------------*/
/* Set and get parameter of the rqb-header                                   */
/*---------------------------------------------------------------------------*/

#define EDD_RQB_SET_NEXT_RQB_PTR(_pRQB, _Value)  LSA_RQB_SET_NEXT_RQB_PTR(_pRQB,_Value)
#define EDD_RQB_GET_NEXT_RQB_PTR(_pRQB)          LSA_RQB_GET_NEXT_RQB_PTR(_pRQB)
#define EDD_RQB_SET_PREV_RQB_PTR(_pRQB, _Value)  LSA_RQB_SET_PREV_RQB_PTR(_pRQB,_Value)
#define EDD_RQB_GET_PREV_RQB_PTR(_pRQB)          LSA_RQB_GET_PREV_RQB_PTR(_pRQB)
#define EDD_RQB_SET_OPCODE(_pRQB, _Value)        LSA_RQB_SET_OPCODE(_pRQB,_Value)
#define EDD_RQB_GET_OPCODE(_pRQB)                LSA_RQB_GET_OPCODE(_pRQB)
#define EDD_RQB_SET_HANDLE(_pRQB, _Value)        LSA_RQB_SET_HANDLE(_pRQB,_Value)
#define EDD_RQB_GET_HANDLE(_pRQB)                LSA_RQB_GET_HANDLE(_pRQB)
#define EDD_RQB_SET_USERID_UVAR32(_pRQB, _Value) LSA_RQB_SET_USER_ID_UVAR32(_pRQB, _Value)
#define EDD_RQB_GET_USERID_UVAR32(_pRQB)         LSA_RQB_GET_USER_ID_UVAR32(_pRQB)
#define EDD_RQB_GET_USERID(_pRQB)                LSA_RQB_GET_USER_ID(_pRQB)
#define EDD_RQB_SET_USERID_PTR(_pRQB, _Value)    LSA_RQB_SET_USER_ID_PTR(_pRQB,_Value)
#define EDD_RQB_GET_USERID_PTR(_pRQB)            LSA_RQB_GET_USER_ID_PTR(_pRQB)
#define EDD_RQB_SET_RESPONSE(_pRQB, _Value)      LSA_RQB_SET_RESPONSE(_pRQB,_Value)
#define EDD_RQB_GET_RESPONSE(_pRQB)              LSA_RQB_GET_RESPONSE(_pRQB)

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*****************************************************************************/
/*  end of file EDD_CFG.H                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDD_CFG_H */
