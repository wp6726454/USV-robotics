#ifndef SOCK_CFG_H
#define SOCK_CFG_H
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
/*  F i l e               &F: sock_cfg.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Configuration for LSA to socket-Interface                                */
/*  Defines constants, types and macros for SOCK.                            */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some definitions depend on the different system, compiler or             */
/*  operating system.                                                        */ 
/*                                                                           */
/*****************************************************************************/

void sock_psi_startstop( int start );

#define SOCK_DEBUG  PSI_DEBUG

/*==============================================================================*/

/* -------------------------------------------------------------------------------------*/
/* enables fixing the LSA_SOCK to interniche TCP IP with the lower layer EDD/ EDD-ERTEC */
/* -------------------------------------------------------------------------------------*/

/* Define one of the following switches here:
   SOCK_CFG_INTERNICHE (and SOCK_CFG_INTERNICHE_V20 if suitable)
   SOCK_CFG_SUN
   SOCK_CFG_LINUX386
   SOCK_CFG_WINSOCK
   SOCK_CFG_FUSION
----------------------------------------------------------------------------*/

#if SOCK_CFG_INTERNICHE

/*=============================================================================
 *       socket functions for Version 1.9 (default) or 2.0
 * which supports IPV6
#define SOCK_CFG_INTERNICHE_V20 1
 *===========================================================================*/
#define SOCK_CFG_INTERNICHE_V20 1

/*=============================================================================
    // If SOCK_CFG_INTERNICHE_DEF_FD is 1, the  SOCKIF_FD_...
    // macros have to be defined
    // If SOCK_CFG_INTERNICHE_DEF_FD is 1, the necessary header files have to
    // be included !
 *===========================================================================*/
#define SOCK_CFG_INTERNICHE_DEF_FD 0

#endif /*SOCK_CFG_INTERNICHE*/

/*============================================================================*/
/*============================================================================*/

#define SOCK_CFG_DO_TIMER_SYNCHRONIZATION 1

/*============================================================================*/
/*
 *       set local_mem static or dynamic:
 * static, don't use (no use of SOCK_ALLOC_LOCAL_MEM):
 * #define SOCK_LOCAL_MEM_STATIC
 * dynamic, must be used (use of SOCK_ALLOC_LOCAL_MEM):
 */
#define SOCK_LOCAL_MEM_DYNAMIC

/*============================================================================*/
/*       clear global data:
 * clear, don't use (SOCK_DATA_TYPE sock_data = {0};)
 * #define SOCK_CLEAR_GLOBAL_DATA
 * or no clear:
 */
#define SOCK_NO_CLEAR_GLOBAL_DATA                                           

/*============================================================================*/

/*  Die SOCK_Komponente unterstützt UDP-Dienste     */
#define SOCK_CFG_UDP_ON 1 /* 0 or 1 */

/*============================================================================*/
 
#ifndef SOCK_CFG_ENABLE_MULTICASTS
# error "SOCK_CFG_ENABLE_MULTICASTS not defined"
#endif

/*============================================================================*/

/* Die SOCK_Komponente unterstützt SNMP-Dienste      */
#define SOCK_CFG_SNMP_ON          1
#define SOCK_CFG_SNMP_BUFFER_LEN  1400               
#define SOCK_CFG_SNMP_PORTNUMBER  0 /* 0 for default (161) */               

/*============================================================================*/

/* SOCK führt keine Timer-Überwachung von Aufrufen durch */
/* #define SOCK_CFG_TIMER_OFF 1  */
 
/* SOCK führt die Timer-Überwachung von Aufrufen durch */
#define SOCK_CFG_TIMER_OFF 0
 
/*============================================================================*/

/* SOCK soll DNS - Anfragen unterstützen */
/*    #define SOCK_CFG_DNS_CLIENT 1 */
/* SOCK unterstützt keine DNS - Anfragen  */
#define SOCK_CFG_DNS_CLIENT 0

/*============================================================================*/
/*============================================================================*/

/* Maximale Anzahl der LSA-Kanaele (bzw. SOCK_CHANNEL Datenstrukturen) */
#ifndef SOCK_CFG_MAX_CHANNELS
# error "SOCK_CFG_MAX_CHANNELS not defined"
#endif

/* Maximale unterstützte Anzahl der Verbindungen */
#ifndef SOCK_CFG_MAX_CONNECTIONS
# error "SOCK_CFG_MAX_CONNECTIONS not defined"
#endif

/* maxímum number of supported interfaces */
#ifndef SOCK_CFG_MAX_INTERFACE_CNT
# error "SOCK_CFG_MAX_INTERFACE_CNT not defined"
#endif

/* Trace der Requestblöcke der übergeordneten LSA-Komponente. */
#define SOCK_UPPER_REQ_TRACE_BYTE_ARRAY(level, msg, ptr, len)



/*===========================================================================*/
/*                          basic attributes for sock                        */
/*===========================================================================*/

#define SOCK_ATTR_NONE /* nothing */

/*===========================================================================*/
/*                      code- and data-attributes for sock                   */
/*===========================================================================*/

/*===== SOCK - global RQB attribute  =====*/
#define SOCK_UPPER_RQB_ATTR

/*===== SOCK - global telegramm / data buffer attribute  =====*/
#define SOCK_UPPER_MEM_ATTR

/*===== SOCK - internal data attribute  =====*/
#define SOCK_LOCAL_MEM_ATTR

/*===== SOCK - sock address data attribute  =====*/
#define SOCK_SOCKADDR_ATTR

/*===== SOCK - system data attribute  =====*/
#define SOCK_SYSTEM_MEM_ATTR

/*===== SOCK - central system data attribute   =====*/
#define SOCK_SYS_DATA_ATTR

/*===========================================================================*/
/*                         code- and data-attributes                         */
/*===========================================================================*/

/* Lokale Funktionen */
#define SOCK_LOCAL_FCT_ATTR                  SOCK_ATTR_NONE

/* Eingangsfunktionen vom System */
#define SOCK_SYSTEM_IN_FCT_ATTR              SOCK_ATTR_NONE

/* Ausgangsfunktionen zum System */
#define SOCK_SYSTEM_OUT_FCT_ATTR             SOCK_ATTR_NONE

/* Eingangsfunktionen vom User */
#define SOCK_UPPER_IN_FCT_ATTR               SOCK_ATTR_NONE

/* Ausgangsfunktionen zum User */
#define SOCK_UPPER_OUT_FCT_ATTR              SOCK_ATTR_NONE

/* Datenpuffer der unterlagerten Ebene */
#define SOCK_LOWER_MEM_ATTR                  SOCK_ATTR_NONE

/* "Groesstes gemeinsames Datenattribut" */
#define SOCK_COMMON_MEM_ATTR                 SOCK_ATTR_NONE

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/




/****************************************************************************/
/* Macro-Definitionen                                                       */
/****************************************************************************/

#if SOCK_DEBUG

#define SOCK_TEST_POINTER(ptr_)  (PSI_TEST_POINTER(ptr_)) /* test for strange values */

#else

#define SOCK_TEST_POINTER(ptr_)  (ptr_) /* don't test */

#endif

/*
 * Ausgangs-Funktionen zum System (sock_sys.h)
 */
/*
 * Ausgangs-Funktionen zu LSA-ERROR-STRUKTUR
 */
/*----------------------------*/
/* Zugriff auf Error-Struktur */
/*----------------------------*/
#define SOCK_RQB_GET_ERR_ADDR(rqb_ptr_) \
    (&(rqb_ptr_) -> err)

#define SOCK_RQB_GET_ERR_LSA_COMPONENT_ID(rqb_ptr_) \
    ((rqb_ptr_) -> err.lsa_component_id)

#define SOCK_RQB_SET_ERR_LSA_COMPONENT_ID(rqb_ptr_, value_) \
    ((rqb_ptr_) -> err.lsa_component_id = (value_))


/*----------------------------*/
/* DNS - MAKROS              */
/*----------------------------*/
#if 0 /*  implemented as functions in sock_cfg.c    */
#define SOCK_DNS_QUERY(ret_val_ptr_, ptr_name_, ptr_ip_addr_)\
            *(ret_val_ptr_) = dns_query((ptr_name_), (ptr_ip_addr_))
            
#define SOCK_DNS_LOOKUP(ret_val_ptr_, ptr_ip_addr_, ptr_name_)\
            *(ret_val_ptr_) = dns_lookup((ptr_ip_addr_), (ptr_name_))

#define SOCK_DNS_SERVERS(ret_val_ptr_)\
            *(ret_val_ptr_) = eim_tcip_DnsServers()
#endif


/*--------------------------------------------------------------------------*/
/* set and get parameter of the rqb-header                                  */
/*--------------------------------------------------------------------------*/

#ifdef  LSA_RQB_HEADER
#define SOCK_RQB_SET_NEXT_RQB_PTR(rb_, val_)    LSA_RQB_SET_NEXT_RQB_PTR(rb_, val_)
#define SOCK_RQB_SET_PREV_RQB_PTR(rb_, val_)    LSA_RQB_SET_PREV_RQB_PTR(rb_, val_)

#define SOCK_RQB_SET_OPCODE(rb_, val_)          LSA_RQB_SET_OPCODE(rb_, val_)
#define SOCK_RQB_GET_OPCODE(rb_)                LSA_RQB_GET_OPCODE(rb_)

#define SOCK_RQB_SET_HANDLE(rb_, val_)          LSA_RQB_SET_HANDLE(rb_, val_)
#define SOCK_RQB_GET_HANDLE(rb_)                LSA_RQB_GET_HANDLE(rb_)

#define SOCK_RQB_SET_RESPONSE(rb_, val_)        LSA_RQB_SET_RESPONSE(rb_, val_)
#define SOCK_RQB_GET_RESPONSE(rb_)              LSA_RQB_GET_RESPONSE(rb_)

#define SOCK_RQB_GET_NEXT_RQB_PTR(rb_)          LSA_RQB_GET_NEXT_RQB_PTR(rb_)
#define SOCK_RQB_GET_PREV_RQB_PTR(rb_)          LSA_RQB_GET_PREV_RQB_PTR(rb_)

#define SOCK_RQB_SET_USER_ID_PTR(_pRQB, _Value) LSA_RQB_SET_USER_ID_PTR(_pRQB, _Value)
#define SOCK_RQB_GET_USER_ID_PTR(_pRQB)         LSA_RQB_GET_USER_ID_PTR(_pRQB)

#else
#error "How this? LSA_RQB_HEADER must be befined at this point"
#endif

#define SOCK_RQB_SET_COMPID(_pRQB, _Value) \
    /*( (_pRQB)->comp_id = (_Value) )*/ /* !!! leer ist Ok (G. Meyer, 19.12.2005) !!! */

#define SOCK_RQB_GET_COMPID(_pRQB) \
    ( 0 /*(_pRQB)->comp_id*/ ) /* !!! "something" ist Ok (G. Meyer, 19.12.2005) !!! */

#define SOCK_RQB_SET_RETRY(_pRQB, _Value) \
    ( (_pRQB)->args.get.retry = (_Value) )

#define SOCK_RQB_SET_SNMP_HANDLE(_pRQB, _Value) \
    ( (_pRQB)->args.snmp_recv.snmp_handle = (_Value) )

#define SOCK_RQB_GET_SNMP_HANDLE(_pRQB) \
    ( (_pRQB)->args.snmp_recv.snmp_handle )

#define SOCK_RQB_SET_SNMP_DONE(_pRQB, _Value) \
    ( (_pRQB)->args.snmp_recv.snmp_done = (_Value) )

#define SOCK_RQB_GET_SNMP_DONE(_pRQB) \
    ( (_pRQB)->args.snmp_recv.snmp_done )


/*------------------------------------------------------------------------------
// interface to LSA trace
//	0 = no LSA trace
//	1 = LSA trace [default]
//----------------------------------------------------------------------------*/

#ifndef SOCK_CFG_TRACE_MODE
 # error "SOCK_CFG_TRACE_MODE not defined" 
#endif

/*------------------------------------------------------------------------------
// interface to BTRACE
//----------------------------------------------------------------------------*/

#ifndef SOCK_FILE_SYSTEM_EXTENSION
# define SOCK_FILE_SYSTEM_EXTENSION(module_id_)	/* nothing */
#endif


/*===========================================================================*/
/*                                types for sock                             */
/*===========================================================================*/

#define SOCK_EXCHANGE_TYPE long

/*=============================================================================
 *      sock common request block type
=============================================================================*/

#define SOCK_RQB_HEADER   LSA_RQB_HEADER(SOCK_UPPER_RQB_PTR_TYPE)

#define SOCK_RQB_TRAILER  /* nothing by default */


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif 
