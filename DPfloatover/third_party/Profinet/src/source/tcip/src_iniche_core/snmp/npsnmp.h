/*
 * v3.1 August 30, 2011 snmp/npsnmp.h
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: NPSNMP.H
 *
 * Portions Copyright 1988, 1989 by Carnegie Mellon University All Rights Reserved.
 * Copyright 1990-96 by NetPort Software. All rights reserved.
 * Copyright 1996-2004 By InterNiche Technologies Inc. All rights reserved.
 *
 *
 * MODULE: SNMP
 *
 * ROUTINES:
 *
 * PORTABLE: yes
 */

/* Additional Copyrights: */

/* npsnmp.h (Previously it was snmp.h Borland compiler ships with a 
 * file snmp.h and hence Borland compiler picks up the wrong snmp.h 
 * file when snmp.h is included in other directories.) 
 */

#ifndef _SNMP_H_
#define  _SNMP_H_ 1

/* 
 * Portions Copyright 1988, 1989 by Carnegie Mellon University All 
 * Rights Reserved Permission to use, copy, modify, and distribute 
 * this software and its documentation for any purpose and without 
 * fee is hereby granted, provided that the above copyright notice 
 * appear in all copies and that both that copyright notice and this 
 * permission notice appear in supporting documentation, and that the 
 * name of CMU not be used in advertising or publicity pertaining to 
 * distribution of the software without specific, written prior 
 * permission. CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS 
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL CMU BE LIABLE FOR ANY SPECIAL, INDIRECT 
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
 */

#define  SNMP_VERSION_1            0
#define  SNMP_VERSION_2            1
#define  SNMP_VERSION_3            3

#define  SNMP_MAX_LEN              484

#define  GET_REQ_MSG               (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x0)
#define  GETNEXT_REQ_MSG           (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x1)
#define  GET_RSP_MSG               (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x2)
#define  SET_REQ_MSG               (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x3)
#define  TRP_REQ_MSG               (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x4)
#define  GETBULK_REQ_MSG           (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x5)
#define  V3_INFORM_PDU             (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x6)
#define  V3_TRAP_PDU               (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x7)
#define  V3_REPORT_PDU             (ASN_CONTEXT | ASN_CONSTRUCTOR | 0x8)

#define  SNMP_ERR_NOERROR          (0x0)
#define  SNMP_ERR_TOOBIG           (0x1)
#define  SNMP_ERR_NOSUCHNAME       (0x2)
#define  SNMP_ERR_NOSUCH           (0x2) /* hate doing all that typing... */
#define  SNMP_ERR_BADVALUE         (0x3)
#define  SNMP_ERR_READONLY         (0x4)
#define  SNMP_ERR_GENERR           (0x5)

#define  SNMP_TRAP_COLDSTART       0
#define  SNMP_TRAP_WARMSTART       1
#define  SNMP_TRAP_LINKDOWN        2
#define  SNMP_TRAP_LINKUP          3
#define  SNMP_TRAP_AUTHFAIL        4
#define  SNMP_TRAP_EGPNEIGHBORLOSS 5
#define  SNMP_TRAP_ENTERPRISESPECIFIC  6

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
#define  SNMP_RS_INVALID            0
#define  SNMP_RS_ACTIVE             1
#define  SNMP_RS_NOTINSERVICE       2
#define  SNMP_RS_NOTREADY           3
#define  SNMP_RS_CREATEANDGO        4
#define  SNMP_RS_CREATEANDWAIT      5
#define  SNMP_RS_DESTROY            6

/* StorageType values */
#define  SNMP_ST_OTHER              1
#define  SNMP_ST_VOLATILE           2
#define  SNMP_ST_NONVOLATILE        3
#define  SNMP_ST_PERMANENT          4
#define  SNMP_ST_READONLY           5

#endif   /* _SNMP_H_ */



