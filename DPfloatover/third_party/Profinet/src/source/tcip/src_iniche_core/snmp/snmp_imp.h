/*
 * v3.1 August 30, 2011 snmp/snmp_imp.h
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: SNMP_IMP.H
 *
 * Portions Copyright 1988, 1989 by Carnegie Mellon University All Rights Reserved.
 * Copyright 1990-96 by NetPort Software. All rights reserved.
 * Copyright 1996-2004 By InterNiche Technologies Inc. All rights reserved.
 *
 * MODULE: SNMP
 *
 * ROUTINES:
 *
 * PORTABLE: yes
 */

/* Additional Copyrights: */

/* snmp_imp.h 
 * Portions Copyright 1990, 1993 by NetPort Software. 
 */

#ifndef _SNMP_IMP_H_
#define  _SNMP_IMP_H_   1

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

/*
 * Error codes:
 */
/*
 * These must not clash with SNMP error codes (all positive).
 */
#define  PARSE_ERROR -1
#define  BUILD_ERROR -2
#define  AUTH_ERROR  -3

#define  RONLY    0xAAAA   /* read access for everyone */
#define  RWRITE      0xAABA   /* add write access for community private */
#define  NOACCESS    0x0000   /* no access for anybody */
#define  READCREATE  0xBBBB   /* read,write,create access for everyone */
#define  WRITEONLY   0xCCCC   /* writeonly access for everyone */
#define  ACCESSIBLE_FOR_NOTIFY   0xDDDD   /* notify access for everyone */

#define  INTEGER     ASN_INTEGER
#define  INTEGER32   ASN_INTEGER
#define  BITS        ASN_BIT_STR
#define  STRING      ASN_OCTET_STR
#define  OBJID       ASN_OBJECT_ID
#define  NULLOBJ     ASN_NULL

#define  SEQUENCE (ASN_SEQUENCE  |  ASN_CONSTRUCTOR)

/* defined types (from the SMI, RFC 1065) */
#define  IPADDRESS   (ASN_APPLICATION |  0)
#define  COUNTER     (ASN_APPLICATION |  1)
#define  COUNTER32   (ASN_APPLICATION |  1)
#define  GAUGE       (ASN_APPLICATION |  2)
#define  GAUGE32     (ASN_APPLICATION |  2)
#define  UNSIGNED32  (ASN_APPLICATION |  2)
#define  TIMETICKS   (ASN_APPLICATION |  3)
#ifdef OPAQUE
#undef OPAQUE
#endif
#define  OPAQUE      (ASN_APPLICATION |  4)

/*       NSAP        (ASN_APPLICATION |  5)            historic */
#define  COUNTER64   (ASN_APPLICATION |  6)
/*       UINTEGER    (ASN_APPLICATION |  7)            historic */
#define  FLOAT       (ASN_APPLICATION |  8)
#define  DOUBLE      (ASN_APPLICATION |  7)
#define  INTEGER64   (ASN_APPLICATION | 10)
#define  UNSIGNED64  (ASN_APPLICATION | 11)


struct communityId   {
     char  name[MAX_COMMUNITY_SIZE];
     int   access;  /* either RONLY or RWRITE */
};

#ifndef NUM_COMMUNITIES
#define NUM_COMMUNITIES   2
#endif
extern   struct communityId   communities[NUM_COMMUNITIES];


/* TRAPS - the following structures are designed to pass trap PDU 
 * data between the SNMP code and the host APP or system. A pointer 
 * to trap info is used as a parameter to snmp_build_v1trap() and 
 * snmp_parse_trap(). The rules for use vary slightly between the 
 * two. snmp_build_v1trap() expects the structure to be filled in by 
 * the caller. If trapVars is zero, no variables are appended to the 
 * trap PDU image. All other fields will be encoded into the trap PDU 
 * as passed. snmp_parse_trap() expects only trapVars to be filled 
 * in. It assumes the trapVar array has the indicated number of 
 * entrys, and that each trapVar.varVal pointer points to a 
 * preallocated buffer of varLen size. If fills in the variable data 
 * as space permits, and sets varLen to indicate the actual size 
 * received. Overlong variables will be truncated. The caller can 
 * recover the truncated data by calling snmp_parse_trap() again with 
 * bigger buffers. Likewise if there are more trap variables than 
 * trapVars entries, the returned value in trapVars will be greater 
 * the the passed value, and the caller can use this information to 
 * try again with more entries. Worst case to recover all the 
 * variables in their entirity is 3 calls: first to determine number 
 * of variables in the trap, second to read all their 
 * sizes, and the third to return full copies of all the data. 
 */
struct trapVar {  /* struct for each trap variable */
     oid   varName[MAX_OID_LEN];/* ObjectId of variable */
     unsigned varNameLen;       /* oid components in varName */
     u_char   varType;          /* ASN.1 type of variable */
     unsigned varValLen;        /* octets in variable data field */
     u_char * varBuf;           /* the actual variable data */
     unsigned varBufLen;        /* used only by snmp_parse_trap() */
};

/* trap data, in local format */

struct trap_info  {
     char  community[MAX_COMMUNITY_SIZE];   /* trap's community */
     int   commLen;             /* length of community */
     oid   objId[MAX_OID_LEN];  /* sysObjectId of sending machine */
     int   objLen;              /* oid count in objId */
     u_long   agent_ip;         /* IP address of sending agent */
     u_long   trapType;         /* SNMP TRAP type, 1 - 6 for v1 */
     u_long   trapSpecific;     /* Vendor specific type if trapType == 6 */
     u_long   time;             /* timetick on agent when trap was generated */
     int   trapVars;            /* number of entrys in trapVar table */
     struct trapVar *  vars;    /* trap variables data structures */
};

char * snmp_parse_trap(u_char * trapdata, unsigned dataLen, 
              struct trap_info *   trapInfo);

struct trap_target   {
     struct sockaddr  ipaddr;           /* ipaddress of IP host to get trap */
     char  community[MAX_COMMUNITY_SIZE];   /* target's community string */
     int   commLen;             /* number of valid chars in community */
};


#ifdef ENABLE_SNMP_TRAPS

extern struct trap_target   trap_targets[MAX_TRAP_TARGETS];
extern int SNMP_ENTRY snmp_trap(int generic, int specific, int varCt, 
   struct trapVar *, int version);
extern int SNMP_ENTRY snmp_trap2target(int generic, int specific, int varCt, 
   struct trapVar *, int version, struct trap_target *targ);
extern u_char *    trap_buffer;
extern unsigned    trap_buffer_len;
extern int snmp_send_trap(u_char * data, int len, struct sockaddr * addr);
#endif   /* ENABLE_SNMP_TRAPS */


/* from snmp.c*/
extern   u_char   sidbuf[MAX_COMMUNITY_SIZE  +  1];   /* System Id */

u_char * snmp_parse_var_op(u_char*, oid*, unsigned*, u_char*, 
   unsigned*, u_char**, unsigned*);
u_char * snmp_build_var_op(u_char*, oid*, unsigned, u_char, 
   unsigned, u_char*, unsigned*);
int      snmp_build_adjust_len(u_char * start_hdr, u_char * start_data, 
   unsigned len_data, int *diff_hdr, unsigned *len_final);
int      snmp_sizeof_int(long value);

/* low level trap PDU builder: */
int snmp_build_v1trap(u_char*, unsigned, struct trap_info * );
int snmp_build_v2trap(u_char*, unsigned, struct trap_info * );

/* from snmp_aut.c */
u_char * snmp_auth_parse(u_char*, unsigned*, u_char*, unsigned*, long*);
u_char * snmp_auth_build(u_char*, unsigned*, u_char*, unsigned*, long*, 
   unsigned);


/* from snmp_age.c (NetPort agent source) */
int  snmp_init(void);    /* startup netport embedded agent */
int  SNMP_ENTRY snmp_agt_parse(u_char*, unsigned, u_char*, unsigned);

int  parse_var_op_list(u_char*, unsigned, u_char*, unsigned, u_char, long*, 
   long, int version);
int  parse_var_op_v2list(u_char*, unsigned, u_char*, unsigned, u_char, long*, 
   long, int version);
int  create_identical(u_char*, u_char*, unsigned, long, long);
int  snmp_access(u_short, unsigned, unsigned);
int  get_community(u_char*);
int  goodValue(u_char *, u_char, int, u_char, int);
int  setVariable(u_char*, u_char, u_char*, int);
void shift_array(u_char*, int, int);

/* functions supplied to the agent by the SNMP port */
u_long GetUptime(void); /* return a value suitable for the sysUptime 
   variable in the mib-2 system group. */
int get_num_variables(void);     /* returns num_variables. */

struct variable;  /* forward decl */

#define  oidcpy(dest,src,len) MEMCPY((char*)dest,(char*)src,((int)(len)*sizeof(oid)))
#define  MIB1  1, 3, 6, 1, 2, 1  /* standard mib Object Id, from mib.h */
#define  MIB2  1, 3, 6, 1, 2, 1  /* standard mib-II Object Id, from mib.h */

extern   long  long_return;

/* macro for MIB-2 SNMP group counters */
#ifdef MIB_COUNTERS
extern   struct snmp_mib   SnmpMib;
#define  SNMP_MIB_CTR(varname);  SnmpMib.varname++;    /* increment */
#define  SNMP_MIB_DEC(varname);  SnmpMib.varname--;    /* decrement */
#else
#define  SNMP_MIB_CTR(varname);  ;
#define  SNMP_MIB_DEC(varname);  ;
#endif 

/* if READ and WRITE are not defined in the development system or
snmpport.h, define them here: */
#ifndef READ
#define READ  0
#endif
#ifndef WRITE
#define WRITE 1
#endif

/* map Berkeley-ish lib calls into standard C lib: */
#ifndef bcopy
#define bcopy(src,  dest, len)  MEMCPY(dest,   src,  len)
#endif

/* values for "oper", passed to var_ routines. */
#define SET_OP   -1
#define NEXT_OP  0
#define GET_OP   1

#define MAX_VB         ((2*MAX_OID_LEN) + 4)

/* map flags to values of SecurityLevel (as defined in RFC) 
 * Output one of these 3 values.
 * noAuthnoPriv  - 1
 * AuthNoPriv    - 2
 * AuthPriv      - 3
 */
#define SNMP_F2SL(flg)  ((flg & V3_AUTH_MASK) ? ((flg & V3_PRIV_MASK) ? 3:2) : 1)
/* generate flags from SecurityLevel 
 * Output one of these 3 values.
 * noAuthnoPriv  - 0   - bit 0 is disabled, bit 1 is disabled
 * AuthNoPriv    - 1   - bit 0 is enabled,  bit 1 is disabled
 * AuthPriv      - 3   - bit 0 is enabled,  bit 1 is enabled
 */
#define SNMP_SL2F(sl)  ((sl == 1) ? 0 : ((sl ==2) ? 1 : 3))


#endif   /* _SNMP_IMP_H_ */



