/*
 * v3.1 August 30, 2011 snmp/snmp_var.h
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: SNMP_VAR.H
 *
 * Portions Copyright 1988, 1989 by Carnegie Mellon University All Rights Reserved.
 * Copyright 1990-96 by NetPort Software. All rights reserved.
 * Copyright 1996-2004 By InterNiche Technologies Inc. All rights reserved.
 *
 * Definitions for SNMP (RFC 1067) agent variable finder.
 *
 *
 * MODULE: SNMP
 *
 * ROUTINES:
 *
 * PORTABLE: yes
 */

/* Additional Copyrights: */


#ifndef _SNMP_VAR_H_
#define  _SNMP_VAR_H_   1

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

/* Enable MIB_VALIDATION when the validation information in the MIBs 
 * is to be used to validation the values of MIB objects (at 
 * runtime). This is a 2 step operation. 
 * 1. Enable MIB_VALIDATION in mibcomp\parse.h. 
 *    Recompile the MIB compiler and run the MIBs 
 *    through it. Copy the generated SNMPVARS.C file to this directory. 
 *    This file has all the validation information for each object. 
 * 2. Enable MIB_VALIDATION in this file. Recompile the sources in this 
 *    file. This will enable the code in SNMP Engine which validates all 
 *    incoming values (objects in SET operation). 
 */

#define MIB_VALIDATION  1 

#ifdef MIB_VALIDATION 
#define  MCT_OTHER   1
#define  MCT_INT     2
#define  MCT_STR     3

#define  MCT_RANGES  1
#define  MCT_VALUES  2

#define  MCT_MAX_VALUES 10

struct size_info  {
   int      itype ;  /* informationType - MCT_INT, MCT_STR, MCT_OTHER */
   int      rtype ;  /* representationType - MCT_RANGES, MCT_VALUES */
   unsigned count ;  /* Number of values/ranges */
   long     values[MCT_MAX_VALUES];
};

typedef struct size_info * SIZE_INFO;
#endif   /* MIB_VALIDATION */

#define  DEF_VARLEN  20 /* default length of oid buffers */

struct variable   {
     oid      name[DEF_VARLEN]; /* object identifier of variable */
     u_char   namelen;       /* length of above */
     char     type;       /* type of variable, INTEGER or (octet) STRING */
     u_char   magic;      /* passed to function as a hint */
     u_short  acl;        /* access control list for variable */
     u_char  * (*findVar)(struct variable *, oid *, int *, int,int *);  
   /* function that finds variable */
#ifdef MIB_VALIDATION 
     struct size_info  ranges;  /* Set of ranges specified in MIB file */
#endif
};

struct SnmpObjects;

/* variables set by var_xxxx() routines if specialized SET is required */

struct SetParms{
   int (*access_method)(u_char *, u_char,int,u_char *, int);
   struct variable * vp;   /* pointer to variables table entry */
   oid  *name;       /* full Object ID of variable */
   int   name_len;   /* object count in "name". */
   int   do_range;   /* TRUE for range/length checking */
   long  hi_range;   /* high range for numeric sets */
   long  lo_range;   /* low  range for numeric sets */
   int   vbflags;    /* flags for special conditions like SF_NOASCIICHK 
                      * cleared before processing every varbind */
   int   pduflags;   /* flags like SF_PDU_NEWROW, which are valid for PDU 
                      * cleared before processing every pdu */
   struct SnmpObjects *vbinds; /* list of received varbinds in SET pdu */

#ifdef MIB_VALIDATION 
   SIZE_INFO p_ranges; /* Set of ranges specified in MIB */
#endif
};

/* SnmpObject defines the tuple to hold all information about
 * a SNMP object (varbind).
 * For example, when a SET request is received, it has a varbind for
 * each object. Hence
 * - SnmpObject will hold varbind of each object
 * - SnmpObjects will hold all the objects
 */

struct SnmpObject
{
   oid      name[DEF_VARLEN*3];     /* object's OID  */
   unsigned namelen;                /* length of OID */
   u_char * value;                  /* pointer to value in rcvd buffer */
   unsigned valuelen;               /* length of value */
   u_char   valuetype;              /* type of value */
};

#ifndef SNMP_MAX_OIDS               /* if already defined, skip defn */
#define SNMP_MAX_OIDS      32       /* max number of OIDs in SET request */
#endif 

struct SnmpObjects
{
   struct SnmpObject objs[SNMP_MAX_OIDS];
   unsigned num; /* number of objects */
};


/* Set of flags for the SetParms's vbflags field */

#define SF_NOASCIICHK      0x0001  /* don't do is_us_ascii() check for this obj */
#define SF_UNSIGNEDVALUE   0x0002  /* verify that object is non-negative */
#define SF_STORAGETYPE     0x0004  /* verify values of StorageType object */
#define SF_SECURITYLEVEL   0x0008  /* verify values of SecurityLevel object */
#define SF_NOCREATION      0x0010  /* can't create new row, return noCreation */
#define SF_WRONGVALUE      0x0020  /* can't create new row, return wrongValue */

/* Set of flags for the SetParms's pduflags field */
#define SF_PDU_NEWROW      0x0001  /* new row created */

extern   struct SetParms   set_parms;  /* single static copy of set parms */
extern   struct variable   variables[];   /* produced by MIB compiler */


u_char * getStatPtr(oid* name, unsigned* namelen, u_char* type, unsigned* len,
   u_short* acl, int exact, long* err_code, int version);
int    compare(oid* name1, unsigned len1, oid* name2, unsigned len2);
void   oid2bytes(char* octetbuf, oid* oidbuf, int count); /* convert octets */
int    setTestAndIncr(u_char * var_val, u_char var_val_type,
   int var_val_len, u_char * statP, int statLen);
int    snmp_update_rowstatus(int * cur_status, int new_status);
int    snmp_preview(u_char * data, unsigned length, struct SnmpObjects *objs);

/* For enumerated data-types in MIB, the following structure is used */

struct enumList   {
   char *   name;
   long  value;
};

#endif   /* _SNMP_VAR_H_ */



