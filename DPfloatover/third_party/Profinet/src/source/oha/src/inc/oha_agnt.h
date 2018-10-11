
#ifndef OHA_AGNT_H                        /* ----- reinclude-protection ----- */
#define OHA_AGNT_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*****************************************************************************/
/** Portions Copyright 1988, 1989 by Carnegie Mellon University All Rights Reserved. */
/** Copyright 1990-96 by NetPort Software. All rights reserved.              */
/** Copyright 1996-2004 By InterNiche Technologies Inc. All rights reserved. */
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
/*  C o m p o n e n t     &C: OHA (Option HAndler)                      :C&  */
/*                                                                           */
/*  F i l e               &F: oha_agnt.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  SNMP-Library of OHA-Agent                                                */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                defines                                    */
/*===========================================================================*/

#ifndef SNMP_COUNTER64
#define SNMP_COUNTER64
#endif

#ifndef INCLUDE_SNMPV1
#define INCLUDE_SNMPV1        1   /* SNMPv1 library, agent & hook */
#endif
/* #define INCLUDE_SNMPV2C    1  */ /* Include SNMPv2c over SNMPv1 */
/* #define INCLUDE_SNMPV3     1  */ /* Include SNMPv3 code in build */

/* If traps are not enabled, then snmp_trap() does nothing */
#ifndef ENABLE_SNMP_TRAPS
#define  oha_snmp_trap(_v, _w, _x, _y, _z)
#endif

#define  MAX_SUBID            0xFFFFFFFF

/* per-port local limitations */
#define  OHA_MAX_NAME_LEN         OHA_MAX_OID_LEN /* max number of subid's in a objid */
#define  OHA_MAX_COMMUNITY_SIZE   32 /* max chars in a community string */

#define  OHA_SYS_STRING_MAX    256         /* max length of sys group strings */

#define  GET_REQ_MSG       (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x0)
#define  GETNEXT_REQ_MSG   (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x1)
#define  GET_RSP_MSG       (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x2)
#define  SET_REQ_MSG       (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x3)
#define  TRP_REQ_MSG       (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x4)
#define  GETBULK_REQ_MSG   (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x5)
#define  V3_INFORM_PDU     (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x6)
#define  V3_TRAP_PDU       (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x7)
#define  V3_REPORT_PDU     (ASN_CONTEXT   |  ASN_CONSTRUCTOR   |  0x8)

/* values of various error codes as defined in RFC1905 */
#define  V3_VB_NOERROR              0
#define  V3_VB_TOOBIG               1
#define  V3_VB_NOSUCHNAME           2
#define  V3_VB_BADVALUE             3
#define  V3_VB_READONLY             4
#define  V3_VB_GENERR               5
#define  V3_VB_NOACCESS             6
#define  V3_VB_WRONGTYPE            7
#define  V3_VB_WRONGLENGTH          8
#define  V3_VB_WRONGENCODING        9
#define  V3_VB_WRONGVALUE           10
#define  V3_VB_NOCREATION           11
#define  V3_VB_INCONSISTENTVALUE    12
#define  V3_VB_RESOURCEUNAVAILABLE  13
#define  V3_VB_COMMITFAILED         14
#define  V3_VB_UNDOFAILED           15
#define  V3_VB_AUTHORIZATIONERROR   16
#define  V3_VB_NOTWRITABLE          17
#define  V3_VB_INCONSISTENTNAME     18

/* RowStatus values */
#define  OHA_SNMP_RS_INVALID            0
#define  OHA_SNMP_RS_ACTIVE             1
#define  OHA_SNMP_RS_NOTINSERVICE       2
#define  OHA_SNMP_RS_NOTREADY           3
#define  OHA_SNMP_RS_CREATEANDGO        4
#define  OHA_SNMP_RS_CREATEANDWAIT      5
#define  OHA_SNMP_RS_DESTROY            6

/* StorageType values */
#define  OHA_SNMP_ST_OTHER              1
#define  OHA_SNMP_ST_VOLATILE           2
#define  OHA_SNMP_ST_NONVOLATILE        3
#define  OHA_SNMP_ST_PERMANENT          4
#define  OHA_SNMP_ST_READONLY           5

/*  Error codes:  */
/*  These must not clash with SNMP error codes (all positive).  */
#define  PARSE_ERROR -1
#define  BUILD_ERROR -2
#define  AUTH_ERROR  -3


struct communityId
{
    char  name[OHA_MAX_COMMUNITY_SIZE];
    int   access;  /* either RONLY or RWRITE */
};

#ifndef NUM_COMMUNITIES
#define NUM_COMMUNITIES   2
#endif

extern   struct communityId   communities[NUM_COMMUNITIES];

/* map Berkeley-ish lib calls into standard C lib:             */
/* #ifndef bcopy                                               */
/* #define bcopy(src,  dest, len)  MEMCPY(dest,   src,  len)   */
/* #endif                                                      */

/* values for "oper", passed to var_ routines.    */
#define  GET_OP   1  /* oper is GET operation     */
#define  NEXT_OP  0  /* oper is GETNEXT operation */
#define  SET_OP   -1 /* oper is SET operation     */

/* generate flags from SecurityLevel
 * Output one of these 3 values.
 * noAuthnoPriv  - 0   - bit 0 is disabled, bit 1 is disabled
 * AuthNoPriv    - 1   - bit 0 is enabled,  bit 1 is disabled
 * AuthPriv      - 3   - bit 0 is enabled,  bit 1 is enabled
 */
#define OHA_SNMP_SL2F(sl)  ((sl == 1) ? 0 : ((sl ==2) ? 1 : 3))

#ifndef OHA_SNMPERROR
#define OHA_SNMPERROR(_x)       OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, _x)
#endif
#define OHA_SNMP_MIB_CTR(_x)

#define OHA_SNMP_LOGOID(_a,_b,_c,_d,_e,_f)
#define OHA_SNMP_LOGTRAP(_a,_b)

/*===========================================================================*/
/*                                macros                                     */
/*===========================================================================*/

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

LSA_EXTERN void oha_snmp_shift_array(U_CHAR*, int, int);
LSA_EXTERN int  oha_snmp_goodValue(U_CHAR *, U_CHAR, int, U_CHAR, int);
LSA_EXTERN int  oha_snmp_setVariable(U_CHAR*, U_CHAR, U_CHAR*, int);

LSA_EXTERN U_CHAR * oha_snmp_parse_var_op(
    U_CHAR*, OHA_OID*, unsigned*, U_CHAR*,
    unsigned*, U_CHAR**, unsigned*);

LSA_EXTERN U_CHAR * oha_snmp_build_var_op(
    U_CHAR*, OHA_OID*, unsigned, U_CHAR,
    unsigned, U_CHAR*, unsigned*);

LSA_EXTERN LSA_BOOL /* int */ oha_snmp_build_adjust_len(
    U_CHAR * start_hdr, U_CHAR * start_data,
    unsigned len_data, int *diff_hdr, unsigned *len_final);

LSA_EXTERN int      oha_snmp_sizeof_int(LSA_INT32 value);

LSA_EXTERN U_CHAR * oha_snmp_getStatPtr(
    OHA_OID mib_node,
    OHA_OID* name, unsigned* namelen, 
    U_CHAR* type, unsigned* len,
    U_SHORT* acl, int exact, LSA_INT32* err_code, int version);

LSA_EXTERN int  oha_snmp_compare(
    OHA_OID* name1, unsigned len1, 
    OHA_OID* name2, unsigned len2);

LSA_EXTERN void oha_snmp_oid2bytes(
    char* octetbuf, 
    OHA_OID* oidbuf, 
    int count); /* convert octets */

LSA_EXTERN U_CHAR oha_parse_int(
    U_CHAR * data,
    unsigned    datalength,
    U_CHAR      type,
    LSA_INT32 * intp,
    unsigned    intsize);

LSA_EXTERN int oha_snmp_get_num_variables(void);
LSA_EXTERN int oha_mrp_get_num_variables(void);

LSA_EXTERN LSA_INT oha_snmp_set_TxInterval(
    U_CHAR					* var_val, 
    U_CHAR					var_val_type,
    LSA_INT					var_val_len, 
    U_CHAR					* statP, 
    LSA_INT					statLen);

/*****************************************************************************/
/*  end of file oha_agnt.h                                                   */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* end of OHA_AGNT_H */
