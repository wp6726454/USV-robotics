#ifndef OHA_CFG_H                        /* ----- reinclude-protection ----- */
#define OHA_CFG_H

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
/*  F i l e               &F: oha_cfg.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Configuration for OHA:                                                   */
/*  Defines constants, types and macros for OHA.                             */
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
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

void oha_psi_startstop( int start );

/*===========================================================================*/
/*                              defines                                      */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/*             Components EDD, DCP, NARE, LLDP, MRP and SOCK                 */
/*                        OHA_CFG_NO_NARE, OHA_CFG_NO_MRP, OHA_CFG_NO_SOCK   */
/*---------------------------------------------------------------------------*/

/* Component NARE not used (no NARE-channel is openend): the request         */
/* OHA_OPC_RESOLVE_ADDRESS is therfore not supported !                       */
/*** #define OHA_CFG_NO_NARE  ***/

/* Component MRP not used (no MRP-channel is openend): the request           */
/* OHA_OPC_CONTROL_MRP_OID is therfore not supported !                       */
/*** #define OHA_CFG_NO_MRP  ***/

/* OHA_SOCK_SNMP_OFF: No AutoActivate/AutoDeactivate of the OHA subagent in SOCK.*/
/* OHA activates its agent in SOCK by OhaSystemOpenChannel and deactivates   */
/* his agent by OhaSystemCloseChannel "automatically". There is no external  */
/* (user-) activation in SOCK necessary, neither an activation of the        */
/* internal agent nor an activation of the (external) OHA agent!             */
/* LSA_SOCK_P04.01.00.00_00.01.04.01 can do AutoActivation.                  */

/* The MRP-MIB (IEC-62439-2-MIB) is not supported.                           */
/*** #define OHA_CFG_NO_MRP_MIB	                                             */

/*---------------------------------------------------------------------------*/
/*                         OHA_CFG_NO_REMA                                   */
/*---------------------------------------------------------------------------*/
/* The PDEV parameterization must also be stored in "remote" parameterization*/
/* and it should be possible to load it during the next restart.             */
/* For this purpose, the Remanence Manager (REMA) is required.               */
/* The use of REMA in OHA can be turned off.                                 */
/*** #define OHA_CFG_NO_REMA  ***/

/*---------------------------------------------------------------------------*/
/* Define if RQB-Priority support should be included.                        */
/* Don't define it not supported by PNIO stack yet                           */
/*---------------------------------------------------------------------------*/

#undef OHA_CFG_USE_RQB_PRIORITY

/*---------------------------------------------------------------------------*/
/* Maximum number of user-channels (max. 128, because of handle-management)  */
/*---------------------------------------------------------------------------*/

#ifndef OHA_CFG_MAX_CHANNELS
# error "OHA_CFG_MAX_CHANNELS not defined" /* max number of user-channels */
#endif


/*---------------------------------------------------------------------------*/
/* Maximum number of NIC's (max. 32, because of handle-management)           */
/*---------------------------------------------------------------------------*/

#define OHA_INVALID_NIC_ID       0xffff       /* for init. the database      */

#ifndef OHA_CFG_MAX_NICS
# error "OHA_CFG_MAX_NICS not defined"
#endif

/*---------------------------------------------------------------------------*/
/* Maximum number of Ports of a Channel                                      */
/* must be >= MaxPortCnt                                                     */
/*---------------------------------------------------------------------------*/

#ifndef OHA_CFG_MAX_PORTS
# error "OHA_CFG_MAX_PORTS not defined"
#endif

/*---------------------------------------------------------------------------*/
/* Configure validity of NameOfStation and PortID.                           */
/*                                                                           */
/* OHA_CFG_ALLOW_UPPER_CASES                                                 */
/*                                                                           */
/*    defined: Upper cases are converted to lower cases before the           */
/*             NameOfStation or the PortID is checked for PNIO standard      */
/*             conventions.                                                  */
/*    undef:   The NameOfStation or the PortID is strictly checked for       */
/*             PNIO standard conventions                                     */
/*                                                                           */
/*    Note:    If the NameOfStation or PortID is valid, the NameOfStation or */
/*             PortID is only stored with (converted) lower cases.           */
/*                                                                           */
/*             If the NameOfStation or PortID is invalid                     */
/*                a) a local Set or remote DCP-Set is rejected,              */
/*                b) a remote station is invalid.                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define OHA_CFG_ALLOW_UPPER_CASES

/*---------------------------------------------------------------------------*/
/* Configure validity of Gateway-IP.                                         */
/*                                                                           */
/* OHA_CFG_NO_GATEWAY_CHECK                                                  */
/*                                                                           */
/*    defined: The Gateway-IP is allowed in another subnet.                  */
/*    undef:   The Gateway-IP is not allowed in another subnet.              */
/*                                                                           */
/*    Note:    By decision of PNIO TT the check is to turn off, if more than */
/*             one interface exists. Only in this case every gateway must be */
/*             able to be set, independent to which subnet it belongs to.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#undef OHA_CFG_NO_GATEWAY_CHECK

/*---------------------------------------------------------------------------*/
/* Additional hello delay by LinkUp in 10ms (20ms default)                   */
/*---------------------------------------------------------------------------*/

/* OHA does a HelloDelay(LinkUp) := "AddDelay" + HelloDelay from the record */

#define OHA_ADDITIONAL_HELLO_DELAY_BY_LINK_UP   2


/*===========================================================================*/
/*                              basic attributes                             */
/*===========================================================================*/

#define OHA_ATTR_NEAR
#define OHA_ATTR_FAR
#define OHA_ATTR_SHUGE
#define OHA_ATTR_HUGE

/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/

/*=====  code attributes  =====*/

#define OHA_UPPER_IN_FCT_ATTR         OHA_ATTR_HUGE  /* OHA-user to OHA  */
#define OHA_UPPER_OUT_FCT_ATTR        OHA_ATTR_HUGE  /* OHA to OHA -user */
#define OHA_EDD_LOWER_IN_FCT_ATTR     OHA_ATTR_HUGE  /* EDD  lower component to OHA */
#define OHA_EDD_LOWER_OUT_FCT_ATTR    OHA_ATTR_HUGE  /* OHA to EDD  lower component */
#define OHA_LLDP_LOWER_IN_FCT_ATTR    OHA_ATTR_HUGE  /* LLDP lower component to OHA */
#define OHA_LLDP_LOWER_OUT_FCT_ATTR   OHA_ATTR_HUGE  /* OHA to LLDP lower component */
#define OHA_NARE_LOWER_IN_FCT_ATTR    OHA_ATTR_HUGE  /* NARE lower component to OHA */
#define OHA_NARE_LOWER_OUT_FCT_ATTR   OHA_ATTR_HUGE  /* OHA to NARE lower component */
#define OHA_MRP_LOWER_IN_FCT_ATTR     OHA_ATTR_HUGE  /* MRP lower component to OHA */
#define OHA_MRP_LOWER_OUT_FCT_ATTR    OHA_ATTR_HUGE  /* OHA to MRP lower component */
#define OHA_DCP_LOWER_IN_FCT_ATTR     OHA_ATTR_HUGE  /* DCP lower component to OHA  */
#define OHA_DCP_LOWER_OUT_FCT_ATTR    OHA_ATTR_HUGE  /* OHA to DCP lower component  */
#define OHA_SOCK_LOWER_IN_FCT_ATTR    OHA_ATTR_HUGE  /* SOCK lower component to OHA */
#define OHA_SNMPX_LOWER_IN_FCT_ATTR   OHA_ATTR_HUGE  /* SNMPX lower component to OHA */
#define OHA_SOCK_LOWER_OUT_FCT_ATTR   OHA_ATTR_HUGE  /* OHA to SOCK lower component */
#define OHA_SNMPX_LOWER_OUT_FCT_ATTR  OHA_ATTR_HUGE  /* OHA to SNMPX lower component */
#define OHA_SYSTEM_IN_FCT_ATTR        OHA_ATTR_FAR   /* system to OHA */
#define OHA_SYSTEM_OUT_FCT_ATTR       OHA_ATTR_FAR   /* OHA to system */
#define OHA_LOCAL_FCT_ATTR            OHA_ATTR_NEAR  /* local function */

/*=====  data attributes  =====*/

#define OHA_UPPER_RQB_ATTR            OHA_ATTR_HUGE  /* upper-rqb data         */
#define OHA_UPPER_MEM_ATTR            OHA_ATTR_HUGE  /* upper-memory data      */
#define OHA_EDD_LOWER_RQB_ATTR        OHA_ATTR_HUGE  /* DCP  lower-rqb data    */
#define OHA_EDD_LOWER_MEM_ATTR        OHA_ATTR_HUGE  /* DCP  lower memory data */
#define OHA_LLDP_LOWER_RQB_ATTR       OHA_ATTR_HUGE  /* LLDP lower-rqb data    */
#define OHA_LLDP_LOWER_MEM_ATTR       OHA_ATTR_HUGE  /* LLDP lower memory data */
#define OHA_NARE_LOWER_RQB_ATTR       OHA_ATTR_HUGE  /* LLDP lower-rqb data    */
#define OHA_NARE_LOWER_MEM_ATTR       OHA_ATTR_HUGE  /* NARE lower memory data */
#define OHA_MRP_LOWER_RQB_ATTR        OHA_ATTR_HUGE  /* NARE lower-rqb data    */
#define OHA_MRP_LOWER_MEM_ATTR        OHA_ATTR_HUGE  /* MRP lower memory data  */
#define OHA_DCP_LOWER_RQB_ATTR        OHA_ATTR_HUGE  /* MRP lower-rqb data     */
#define OHA_DCP_LOWER_MEM_ATTR        OHA_ATTR_HUGE  /* DCP lower memory data  */
#define OHA_SOCK_LOWER_RQB_ATTR       OHA_ATTR_HUGE  /* SOCK lower-rqb data    */
#define OHA_SOCK_LOWER_MEM_ATTR       OHA_ATTR_HUGE  /* SOCK lower memory data */
#define OHA_SNMPX_LOWER_RQB_ATTR      OHA_ATTR_HUGE  /* SNMPX lower-rqb data   */
#define OHA_SNMPX_LOWER_MEM_ATTR      OHA_ATTR_HUGE  /* SNMPX lower memory data */
#define OHA_SYSTEM_MEM_ATTR           OHA_ATTR_HUGE  /* system-memory data     */
#define OHA_LOCAL_MEM_ATTR            OHA_ATTR_HUGE  /* local data             */
#define OHA_COMMON_MEM_ATTR           OHA_ATTR_HUGE  /* greatest common data   */
/* attribute (matches all)*/

/*---------------------------------------------------------------------------*/
/* Definition of RQB-HEADER:                                                 */
/*                                                                           */
/* The order of the parameters can be changed!                               */
/* The length of the RQB-HEADER must be 2/4-byte-aligned according to the    */
/* system!                                                                   */
/* If necessary, dummy-bytes must be added!                                  */
/* If necessary during system integration, additional parameters can be added*/
/*                                                                           */
/* For example:                                                              */
/*   LSA_COMP_ID_TYPE                comp_id; // for lsa-component-id        */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define OHA_RQB_HEADER  LSA_RQB_HEADER(OHA_UPPER_RQB_PTR_TYPE)


/*---------------------------------------------------------------------------*/
/* Definition of RQB-TRAILER                                                 */
/*                                                                           */
/* If necessary during system integration, additional parameters can be      */
/* added!                                                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define OHA_RQB_TRAILER
/* nothing by default */


/*---------------------------------------------------------------------------*/
/* set and get parameter of the rqb-header                                   */
/*---------------------------------------------------------------------------*/

#define OHA_RQB_SET_NEXT_RQB_PTR      LSA_RQB_SET_NEXT_RQB_PTR
#define OHA_RQB_GET_NEXT_RQB_PTR      LSA_RQB_GET_NEXT_RQB_PTR
#define OHA_RQB_SET_PREV_RQB_PTR      LSA_RQB_SET_PREV_RQB_PTR
#define OHA_RQB_GET_PREV_RQB_PTR      LSA_RQB_GET_PREV_RQB_PTR
#define OHA_RQB_SET_OPCODE            LSA_RQB_SET_OPCODE
#define OHA_RQB_GET_OPCODE            LSA_RQB_GET_OPCODE
#define OHA_RQB_SET_HANDLE            LSA_RQB_SET_HANDLE
#define OHA_RQB_GET_HANDLE            LSA_RQB_GET_HANDLE
#define OHA_RQB_SET_USERID_UVAR32     LSA_RQB_SET_USER_ID_UVAR32
#define OHA_RQB_GET_USERID_UVAR32     LSA_RQB_GET_USER_ID_UVAR32
#define OHA_RQB_SET_USERID_UVAR16_1   LSA_RQB_SET_USER_ID_UVAR16_ARRAY_LOW
#define OHA_RQB_GET_USERID_UVAR16_1   LSA_RQB_GET_USER_ID_UVAR16_ARRAY_LOW
#define OHA_RQB_SET_USERID_UVAR16_2   LSA_RQB_SET_USER_ID_UVAR16_ARRAY_HIGH
#define OHA_RQB_GET_USERID_UVAR16_2   LSA_RQB_GET_USER_ID_UVAR16_ARRAY_HIGH
#define OHA_RQB_SET_RESPONSE          LSA_RQB_SET_RESPONSE
#define OHA_RQB_GET_RESPONSE          LSA_RQB_GET_RESPONSE
#define OHA_RQB_SET_USER_ID_PTR       LSA_RQB_SET_USER_ID_PTR
#define OHA_RQB_GET_USER_ID_PTR       LSA_RQB_GET_USER_ID_PTR

#ifdef OHA_CFG_USE_RQB_PRIORITY
#define OHA_RQB_SET_PRIORITY          LSA_RQB_SET_PRIORITY
#define OHA_RQB_GET_PRIORITY          LSA_RQB_GET_PRIORITY
#endif
/*===========================================================================*/
/* LOWER LAYER RQB MACROS to EDD                                             */
/*===========================================================================*/

#define OHA_EDD_RQB_SET_NEXT_RQB_PTR(rb_, val_)   EDD_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define OHA_EDD_RQB_SET_PREV_RQB_PTR(rb_, val_)   EDD_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define OHA_EDD_RQB_SET_OPCODE(rb_, val_)         EDD_RQB_SET_OPCODE(rb_, val_)
#define OHA_EDD_RQB_SET_SERVICE(rb_, val_)        EDD_RQB_SET_SERVICE(rb_, val_)
#define OHA_EDD_RQB_SET_HANDLE(rb_, val_)         EDD_RQB_SET_HANDLE(rb_, val_)
#define OHA_EDD_RQB_SET_USERID_UVAR16(rb_, val_)  LSA_RQB_SET_USER_ID_UVAR16(rb_, val_)
#define OHA_EDD_RQB_SET_USERID_UVAR32(rb_, val_)  EDD_RQB_SET_USERID_UVAR32(rb_, val_)
#define OHA_EDD_RQB_SET_RESPONSE(rb_, val_)       EDD_RQB_SET_STATUS(rb_, val_)

#define OHA_EDD_RQB_GET_NEXT_RQB_PTR(rb_)         EDD_RQB_GET_NEXT_RQB_PTR(rb_)
#define OHA_EDD_RQB_GET_PREV_RQB_PTR(rb_)         EDD_RQB_GET_PREV_RQB_PTR(rb_)
#define OHA_EDD_RQB_GET_OPCODE(rb_)               EDD_RQB_GET_OPCODE(rb_)
#define OHA_EDD_RQB_GET_SERVICE(rb_)              EDD_RQB_GET_SERVICE(rb_)
#define OHA_EDD_RQB_GET_HANDLE(rb_)               EDD_RQB_GET_HANDLE(rb_)
#define OHA_EDD_RQB_GET_USERID_UVAR16(rb_)        LSA_RQB_GET_USER_ID_UVAR16(rb_)
#define OHA_EDD_RQB_GET_USERID_UVAR32(rb_)        EDD_RQB_GET_USERID_UVAR32(rb_)
#define OHA_EDD_RQB_GET_RESPONSE(rb_)             EDD_RQB_GET_RESPONSE(rb_)

/*===========================================================================*/
/* LOWER LAYER RQB MACROS to DCP                                             */
/*===========================================================================*/

#define OHA_DCP_RQB_SET_NEXT_RQB_PTR(rb_, val_)   DCP_UPPER_RQB_NEXT_SET(rb_, val_)
#define OHA_DCP_RQB_SET_PREV_RQB_PTR(rb_, val_)   DCP_UPPER_RQB_PREV_SET(rb_, val_)
#define OHA_DCP_RQB_SET_OPCODE(rb_, val_)         DCP_UPPER_RQB_OPCODE_SET(rb_, val_)
#define OHA_DCP_RQB_SET_HANDLE(rb_, val_)         DCP_UPPER_RQB_HANDLE_SET(rb_, val_)
#define OHA_DCP_RQB_SET_USERID_UVAR16(rb_, val_)  LSA_RQB_SET_USER_ID_UVAR16(rb_, val_)
#define OHA_DCP_RQB_SET_USERID_UVAR32(rb_, val_)  DCP_UPPER_RQB_USER32_SET(rb_, val_)
#define OHA_DCP_RQB_SET_RESPONSE(rb_, val_)       DCP_UPPER_RQB_RESPONSE_SET(rb_, val_)

#define OHA_DCP_RQB_GET_NEXT_RQB_PTR(rb_)         DCP_UPPER_RQB_NEXT_GET(rb_)
#define OHA_DCP_RQB_GET_PREV_RQB_PTR(rb_)         DCP_UPPER_RQB_PREV_GET(rb_)
#define OHA_DCP_RQB_GET_OPCODE(rb_)               DCP_UPPER_RQB_OPCODE_GET(rb_)
#define OHA_DCP_RQB_GET_HANDLE(rb_)               DCP_UPPER_RQB_HANDLE_GET(rb_)
#define OHA_DCP_RQB_GET_USERID_UVAR16(rb_)        LSA_RQB_GET_USER_ID_UVAR16(rb_)
#define OHA_DCP_RQB_GET_USERID_UVAR32(rb_)        DCP_UPPER_RQB_USER32_GET(rb_)
#define OHA_DCP_RQB_GET_RESPONSE(rb_)             DCP_UPPER_RQB_RESPONSE_GET(rb_)

/*===========================================================================*/
/* LOWER LAYER RQB MACROS to LLDP                                            */
/*===========================================================================*/

#define OHA_LLDP_RQB_SET_NEXT_RQB_PTR(rb_, val_)   LLDP_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define OHA_LLDP_RQB_SET_PREV_RQB_PTR(rb_, val_)   LLDP_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define OHA_LLDP_RQB_SET_OPCODE(rb_, val_)         LLDP_RQB_SET_OPCODE(rb_, val_)
#define OHA_LLDP_RQB_SET_HANDLE(rb_, val_)         LLDP_RQB_SET_HANDLE(rb_, val_)
#define OHA_LLDP_RQB_SET_USERID_UVAR16(rb_, val_)  LLDP_RQB_SET_USERID_UVAR16_1(rb_, val_)
#define OHA_LLDP_RQB_SET_USERID_UVAR32(rb_, val_)  LLDP_RQB_SET_USERID_UVAR32(rb_, val_)
#define OHA_LLDP_RQB_SET_RESPONSE(rb_, val_)       LLDP_RQB_SET_RESPONSE(rb_, val_)

#define OHA_LLDP_RQB_GET_NEXT_RQB_PTR(rb_)         LLDP_RQB_GET_NEXT_RQB_PTR(rb_)
#define OHA_LLDP_RQB_GET_PREV_RQB_PTR(rb_)         LLDP_RQB_GET_PREV_RQB_PTR(rb_)
#define OHA_LLDP_RQB_GET_OPCODE(rb_)               LLDP_RQB_GET_OPCODE(rb_)
#define OHA_LLDP_RQB_GET_HANDLE(rb_)               LLDP_RQB_GET_HANDLE(rb_)
#define OHA_LLDP_RQB_GET_USERID_UVAR16(rb_)        LLDP_RQB_GET_USERID_UVAR16_1(rb_)
#define OHA_LLDP_RQB_GET_USERID_UVAR32(rb_)        LLDP_RQB_GET_USERID_UVAR32(rb_)
#define OHA_LLDP_RQB_GET_RESPONSE(rb_)             LLDP_RQB_GET_RESPONSE(rb_)

/*===========================================================================*/
/* LOWER LAYER RQB MACROS to NARE                                            */
/*===========================================================================*/

#define OHA_NARE_RQB_SET_NEXT_RQB_PTR(rb_, val_)   NARE_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define OHA_NARE_RQB_SET_PREV_RQB_PTR(rb_, val_)   NARE_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define OHA_NARE_RQB_SET_OPCODE(rb_, val_)         NARE_RQB_SET_OPCODE(rb_, val_)
#define OHA_NARE_RQB_SET_HANDLE(rb_, val_)         NARE_RQB_SET_HANDLE(rb_, val_)
#define OHA_NARE_RQB_SET_USERID_UVAR16(rb_, val_)  NARE_RQB_SET_USERID_UVAR16_1(rb_, val_)
#define OHA_NARE_RQB_SET_USERID_UVAR32(rb_, val_)  NARE_RQB_SET_USERID_UVAR32(rb_, val_)
#define OHA_NARE_RQB_SET_RESPONSE(rb_, val_)       NARE_RQB_SET_RESPONSE(rb_, val_)

#ifdef OHA_CFG_USE_RQB_PRIORITY
#define OHA_NARE_RQB_SET_PRIORITY(rb_, val_)       NARE_RQB_SET_PRIORITY(rb_, val_)
#endif

#define OHA_NARE_RQB_GET_NEXT_RQB_PTR(rb_)         NARE_RQB_GET_NEXT_RQB_PTR(rb_)
#define OHA_NARE_RQB_GET_PREV_RQB_PTR(rb_)         NARE_RQB_GET_PREV_RQB_PTR(rb_)
#define OHA_NARE_RQB_GET_OPCODE(rb_)               NARE_RQB_GET_OPCODE(rb_)
#define OHA_NARE_RQB_GET_HANDLE(rb_)               NARE_RQB_GET_HANDLE(rb_)
#define OHA_NARE_RQB_GET_USERID_UVAR16(rb_)        NARE_RQB_GET_USERID_UVAR16_1(rb_)
#define OHA_NARE_RQB_GET_USERID_UVAR32(rb_)        NARE_RQB_GET_USERID_UVAR32(rb_)
#define OHA_NARE_RQB_GET_RESPONSE(rb_)             NARE_RQB_GET_RESPONSE(rb_)

/*===========================================================================*/
/* LOWER LAYER RQB MACROS to MRP                                             */
/*===========================================================================*/

#define OHA_MRP_RQB_SET_NEXT_RQB_PTR(rb_, val_)   MRP_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define OHA_MRP_RQB_SET_PREV_RQB_PTR(rb_, val_)   MRP_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define OHA_MRP_RQB_SET_OPCODE(rb_, val_)         MRP_RQB_SET_OPCODE(rb_, val_)
#define OHA_MRP_RQB_SET_HANDLE(rb_, val_)         MRP_RQB_SET_HANDLE(rb_, val_)
#define OHA_MRP_RQB_SET_USERID_UVAR16(rb_, val_)  MRP_RQB_SET_USER_ID_UVAR16(rb_, val_)
#define OHA_MRP_RQB_SET_RESPONSE(rb_, val_)       MRP_RQB_SET_RESPONSE(rb_, val_)

#define OHA_MRP_RQB_GET_NEXT_RQB_PTR(rb_)         MRP_RQB_GET_NEXT_RQB_PTR(rb_)
#define OHA_MRP_RQB_GET_PREV_RQB_PTR(rb_)         MRP_RQB_GET_PREV_RQB_PTR(rb_)
#define OHA_MRP_RQB_GET_OPCODE(rb_)               MRP_RQB_GET_OPCODE(rb_)
#define OHA_MRP_RQB_GET_HANDLE(rb_)               MRP_RQB_GET_HANDLE(rb_)
#define OHA_MRP_RQB_GET_USERID_UVAR16(rb_)        MRP_RQB_GET_USER_ID_UVAR16(rb_)
#define OHA_MRP_RQB_GET_RESPONSE(rb_)             MRP_RQB_GET_RESPONSE(rb_)

/*===========================================================================*/
/* LOWER LAYER RQB MACROS to SOCK                                            */
/*===========================================================================*/

#define OHA_SOCK_RQB_SET_NEXT_RQB_PTR(rb_, val_)   SOCK_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define OHA_SOCK_RQB_SET_PREV_RQB_PTR(rb_, val_)   SOCK_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define OHA_SOCK_RQB_SET_OPCODE(rb_, val_)         SOCK_RQB_SET_OPCODE(rb_, val_)
#define OHA_SOCK_RQB_SET_HANDLE(rb_, val_)         SOCK_RQB_SET_HANDLE(rb_, val_)
#define OHA_SOCK_RQB_SET_USERID_UVAR16(rb_, val_)  LSA_RQB_SET_USER_ID_UVAR16(rb_, val_)
#define OHA_SOCK_RQB_SET_USERID_UVAR32(rb_, val_)  LSA_RQB_SET_USER_ID_UVAR32(rb_, val_)
#define OHA_SOCK_RQB_SET_RESPONSE(rb_, val_)       SOCK_RQB_SET_RESPONSE(rb_, val_)

#define OHA_SOCK_RQB_GET_NEXT_RQB_PTR(rb_)         SOCK_RQB_GET_NEXT_RQB_PTR(rb_)
#define OHA_SOCK_RQB_GET_PREV_RQB_PTR(rb_)         SOCK_RQB_GET_PREV_RQB_PTR(rb_)
#define OHA_SOCK_RQB_GET_OPCODE(rb_)               SOCK_RQB_GET_OPCODE(rb_)
#define OHA_SOCK_RQB_GET_HANDLE(rb_)               SOCK_RQB_GET_HANDLE(rb_)
#define OHA_SOCK_RQB_GET_USERID_UVAR16(rb_)        LSA_RQB_GET_USER_ID_UVAR16(rb_)
#define OHA_SOCK_RQB_GET_USERID_UVAR32(rb_)        LSA_RQB_GET_USER_ID_UVAR32(rb_)
#define OHA_SOCK_RQB_GET_RESPONSE(rb_)             SOCK_RQB_GET_RESPONSE(rb_)


/*===========================================================================*/
/* LOWER LAYER RQB MACROS to SNMPX                                           */
/*===========================================================================*/

#define OHA_SNMPX_RQB_SET_NEXT_RQB_PTR(rb_, val_)  SNMPX_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define OHA_SNMPX_RQB_SET_PREV_RQB_PTR(rb_, val_)  SNMPX_RQB_SET_PREV_RQB_PTR(rb_, val_)
#define OHA_SNMPX_RQB_SET_OPCODE(rb_, val_)        SNMPX_RQB_SET_OPCODE(rb_, val_)
#define OHA_SNMPX_RQB_SET_HANDLE(rb_, val_)        SNMPX_RQB_SET_HANDLE(rb_, val_)
#define OHA_SNMPX_RQB_SET_USERID_UVAR32(rb_, val_) SNMPX_RQB_SET_USERID_UVAR32(rb_, val_)
#define OHA_SNMPX_RQB_SET_RESPONSE(rb_, val_)      SNMPX_RQB_SET_RESPONSE(rb_, val_)

#define OHA_SNMPX_RQB_GET_NEXT_RQB_PTR(rb_)        SNMPX_RQB_GET_NEXT_RQB_PTR(rb_)
#define OHA_SNMPX_RQB_GET_PREV_RQB_PTR(rb_)        SNMPX_RQB_GET_PREV_RQB_PTR(rb_)
#define OHA_SNMPX_RQB_GET_OPCODE(rb_)              SNMPX_RQB_GET_OPCODE(rb_)
#define OHA_SNMPX_RQB_GET_HANDLE(rb_)              SNMPX_RQB_GET_HANDLE(rb_)
#define OHA_SNMPX_RQB_GET_USERID_UVAR32(rb_)       SNMPX_RQB_GET_USERID_UVAR32(rb_)
#define OHA_SNMPX_RQB_GET_RESPONSE(rb_)            SNMPX_RQB_GET_RESPONSE(rb_)

/*===========================================================================*/
/* BTRACE - Support                                                          */
/*===========================================================================*/

#ifndef OHA_FILE_SYSTEM_EXTENSION
#define OHA_FILE_SYSTEM_EXTENSION(module_id_)
#endif

/*===========================================================================*/
/* LTRC - Support (LSA-Trace)                                                */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* define the Trace mode for OHA                                             */
/* 0: no traces or external traces (default)                                 */
/* 1: enable Traces and use LTRC (LSA-Trace module) see OHA_ltrc.h           */
/*---------------------------------------------------------------------------*/

#ifndef OHA_CFG_TRACE_MODE
 # error "OHA_CFG_TRACE_MODE not defined" 
#endif

/*=============================================================================
 * If the LSA component LTRC isn't used for trace in the target system, then
 * the OHA trace macros can be defined here. On default they are empty.
 *
 * See also files oha_trc.h and lsa_cfg.h/txt.
 *
 *===========================================================================*/

/*===========================================================================*/
/* Debugging (for developer use)                                             */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* If a a memory-free call error should result in an fatal-error call        */
/* with OHA_FATAL_ERR_MEM_FREE define OHA_CFG_FREE_ERROR_IS_FATAL. If not    */
/* memory-free errors will be ignored.                                       */
/*---------------------------------------------------------------------------*/

#define OHA_CFG_FREE_ERROR_IS_FATAL

/*---------------------------------------------------------------------------*/
/* define to enable spezial additonal assertion-checks (e.g. NULL-Ptr)       */
/* This assertion will cause an fatal-error.                                 */
/*---------------------------------------------------------------------------*/

#if PSI_DEBUG
#define OHA_CFG_DEBUG_ASSERT
#endif

/*----------------------------------------------------------------------------*/
/*  Assert                                                                    */
/*                                                                            */
/*  ==> MUST BE EMPTY in Release-Version!                                     */
/*                                                                            */
/*  check if condition holds or call fatal error otherwise (programming error)*/
/*                                                                            */
/*----------------------------------------------------------------------------*/

#ifdef OHA_CFG_DEBUG_ASSERT
#define OHA_ASSERT(Cond_)  \
{                                                         \
	if( ! (Cond_) )                                       \
	{                                                     \
		OHA_FatalError( OHA_FATAL_ERR_INCONSISTENZ,       \
			    	 	OHA_MODULE_ID,                    \
				     	__LINE__);                        \
	}                                                     \
}
#define OHA_ASSERT_NULL_PTR(pPtr) \
{                                                         \
	if( LSA_HOST_PTR_ARE_EQUAL(pPtr, LSA_NULL) )          \
	{                                                     \
		OHA_FatalError( OHA_FATAL_ERR_NULL_PTR,           \
			    	 	OHA_MODULE_ID,                    \
				     	__LINE__);                        \
	}                                                     \
}

#define OHA_ASSERT_FALSE(Cond_)  \
{                                                         \
	if( ! (Cond_) )                                       \
	{                                                     \
		OHA_FatalError( OHA_FATAL_ERR_INCONSISTENZ,       \
			    	 	OHA_MODULE_ID,                    \
				     	__LINE__);                        \
	}                                                     \
}
#else
#define OHA_ASSERT(Condition) {}
#define OHA_ASSERT_NULL_PTR(pPtr) {}
#define OHA_ASSERT_FALSE(Condition) {}
#endif

/*===========================================================================*/
/*                          macros/function                                  */
/*===========================================================================*/

/*****************************************************************************/
/*  end of file OHA_CFG.H                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of OHA_CFG_H */
