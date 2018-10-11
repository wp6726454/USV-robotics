/*
 * v3.1 August 30, 2011 snmpv1/trap_out.c
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: trap_out.c
 *
 * Portions Copyright 1988, 1989 by Carnegie Mellon University All Rights Reserved.
 * Copyright 1990-96 by NetPort Software. All rights reserved.
 * Copyright 1996-2004 By InterNiche Technologies Inc. All rights reserved.
 *
 * Protocol level code to handle building outgoing v1 and v2c traps. The 
 * implementation provides a routine named "snmp_trap()" which 
 * is called whenever a trap is to be sent. It calls snmp_build_v1trap()
 * to send SNMPv1 trap and snmp_build_v2trap() to send a SNMPv2c trap.
 * Trap is sent once for each trap target. 
 *
 * MODULE: SNMPV1
 *
 * ROUTINES: snmp_trap(), snmp_build_v1trap(), snmp_test_sp_trap(),  
 * PORTABLE: yes
 */


#include "snmpport.h"

#if (defined(INCLUDE_SNMPV1) || defined(INCLUDE_SNMPV2C))
#ifdef ENABLE_SNMP_TRAPS   /* whole file is within this ifdef */

#include "asn1.h"
#include "npsnmp.h"
#include "snmp_imp.h"
#include "snmp_var.h"

#ifndef AF_INET
#define  AF_INET     2     /* internetwork: UDP, TCP, etc. */
#endif

/* If no varbind is present, put empty varbind "30 00" in the PDU.
 * If SNMP_PUTEMPTYVARBIND option is disabled, "30 00" will not be put.
 * Required by
 * - SNMPUTIL ( on WinNT ) 
 * - Trap Ringer console of MG-SOFT
 */
#define SNMP_PUTEMPTYVARBIND  1 

struct trap_target  trap_targets[ MAX_TRAP_TARGETS ] = { {0}, {0}, {0}} ;
struct trap_info    tinfo = {{0}, 0, {0}, 0} ; /* struct for pass the snmp_build_v1trap() */

/* trap related agent values from mib2 system group */
oid trap_sys_id[MAX_OID_LEN] = {1,3,6,1,4,1,ENTERPRISE,1,5,1,1,1};   
unsigned trap_sys_id_len = 12;
int trap_sys_id_flag = 0;

/* SNMP traps OID (R.A) */
const oid traps_oid[] = {1,3,6,1,6,3,1,1,5};
unsigned traps_oid_len = sizeof(traps_oid)/sizeof(oid);

/* SNMPv1 and SNMPv2c don't use any dynamic memory. Usually global buffer
 * snmpbuf is used to send all SNMP RESPONSEs. trap_buffer should point
 * to a buffer (usually done in snmp_init()). trap_buffer is used to
 * send a trap.
 */

char *   defaultTrapComm   = "public";
unsigned trap_buffer_len   =  0;
u_char * trap_buffer =  NULL; /* may be set by snmp_init() */


/* FUNCTION: snmp_trap()
 * 
 * Send a SNMPv1/SNMPv2c trap to all targets (in trap_targets[]).
 * For each valid target, it calls snmp_trap2target() to send the trap.
 *
 * PARAM1: int   trapType
 * PARAM2: int   specificType
 * PARAM3: int   specificVarCount
 * PARAM4: struct trapVar * specificVars
 * PRARM5: int   version
 *
 * RETURNS: 
 */

int      SNMP_ENTRY
snmp_trap(int   trapType, 
   /* the rest of these parms are ignored if trapType != 6 */
   int    specificType,
   int    specificVarCount,        /* number of variables */
   struct trapVar *  specificVars, /* array of variable info */
   int    version)                 /* SNMP version (v1/v2c) */
{
   int   i, e=0;  /* make sure we don't return garbage */

   /* do some sanity check about version supported */
#if (defined(INCLUDE_SNMPV1) && !defined(INCLUDE_SNMPV2C))
   /* only SNMPV1 is defined */
   if (version != SNMP_VERSION_1)
     return -1;
#endif
#if (!defined(INCLUDE_SNMPV1) && defined(INCLUDE_SNMPV2C))
   /* only SNMPV2c is defined */
   if (version != SNMP_VERSION_2)
     return -1;
#endif

   SNMP_LOGTRAP(trapType, "");  /* send syslog() log for this trap */

   /* loop through trap target table, sending a copy of the trap 
    * to each valid entry. */
   for(i = 0; i < MAX_TRAP_TARGETS; i++)
   {
      if(!trap_targets[i].ipaddr.sa_family)  /* target not set, don't bother */
         continue;

      /* set up parms in tinfo structure for pass to snmp_build_v1trap() */
      if(trap_targets[i].commLen == 0) /* no community, install default */
      {
         MEMCPY(trap_targets[i].community, defaultTrapComm, strlen(defaultTrapComm));
         trap_targets[i].commLen =  strlen(defaultTrapComm);
      }

      e = snmp_trap2target(trapType,specificType,specificVarCount,
         specificVars,version,&trap_targets[i]);
   }
   return   e; /* report status of last send */
}

/* FUNCTION: snmp_trap2target()
 * 
 * Send a SNMPv1/SNMPv2c trap to a specific target.
 * Build the trap and call snmp_send_trap() to send it.
 *
 * PARAM1: int   trapType
 * PARAM2: int   specificType
 * PARAM3: int   specificVarCount
 * PARAM4: struct trapVar * specificVars
 * PARAM5: int   version
 * PARAM6: struct trap_target *targ
 *
 * RETURNS: 
 */

int      SNMP_ENTRY
snmp_trap2target(int   trapType, 
   /* the rest of these parms are ignored if trapType != 6 */
   int    specificType,
   int    specificVarCount,        /* number of variables */
   struct trapVar *  specificVars, /* array of variable info */
   int    version,                 /* SNMP version (v1/v2c) */
   struct trap_target * targ)      /* trap destination */
{
   int out_len=0;
   int e=0;  /* make sure we don't return garbage */

   /* do some sanity check about version supported */
#if (defined(INCLUDE_SNMPV1) && !defined(INCLUDE_SNMPV2C))
   /* only SNMPV1 is defined */
   if (version != SNMP_VERSION_1)
     return -1;
#endif
#if (!defined(INCLUDE_SNMPV1) && defined(INCLUDE_SNMPV2C))
   /* only SNMPV2c is defined */
   if (version != SNMP_VERSION_2)
     return -1;
#endif

   /* fill in tinfo struct with parameters for snmp_build_v1trap() */
   MEMCPY(tinfo.community, targ->community, targ->commLen);
   tinfo.commLen  =  targ->commLen;
   tinfo.community[targ->commLen] = 0;

    /* If it's SNMPv1 MIB-II trap, set Enterprise oid to MIB-II snmpTraps (Fixed by R.A) */
    if ((trapType != SNMP_TRAP_ENTERPRISESPECIFIC) && (version == SNMP_VERSION_1))
    {
        oidcpy(tinfo.objId, traps_oid, traps_oid_len);
        tinfo.objLen   =  traps_oid_len;
    }
    else /* it's an enterprise specific trap or v2/v3 trap */
    {
   oidcpy(tinfo.objId, trap_sys_id, trap_sys_id_len);
   tinfo.objLen   =  trap_sys_id_len;
    }
   /* get agent's IP address relative to target */
   tinfo.agent_ip = ip_myaddr(&targ->ipaddr);
   tinfo.trapType =  trapType;
   tinfo.time  = GetUptime();

   if(trapType != SNMP_TRAP_ENTERPRISESPECIFIC)
   {
      tinfo.trapSpecific   =  0; /* by definition */
      tinfo.trapVars =  0;    /* non-specific traps pass no variables */
   }
   else  /* it's an entrprise specific trap, pass specific parms */
   {
      tinfo.trapSpecific   =  specificType;
      tinfo.trapVars =  specificVarCount;
      tinfo.vars  =  specificVars;
   }

#ifdef INCLUDE_SNMPV2C
   if (version == SNMP_VERSION_2)
   {
      
      trap_buffer = npalloc(trap_buffer_len);
      if (trap_buffer == NULL)
         return   -1;
      out_len  = snmp_build_v2trap(trap_buffer, trap_buffer_len, &tinfo);
   }
#endif
#ifdef INCLUDE_SNMPV1
   if (version == SNMP_VERSION_1)
   {
      trap_buffer = npalloc(trap_buffer_len);
      if (trap_buffer == NULL)
         return   -1;
      out_len  = snmp_build_v1trap(trap_buffer, trap_buffer_len, &tinfo);
   }
#endif

   if (out_len == 0) /* build failed for some reason... */
   {
      npfree(trap_buffer);
      SNMPERROR("trap build failed");
      dtrap();
      return   -1;
   }

   SNMP_MIB_CTR(snmpOutPkts);
   SNMP_MIB_CTR(snmpOutTraps);

   e  = snmp_send_trap(trap_buffer, out_len, &targ->ipaddr);   
   npfree(trap_buffer);

   return   e; /* report status of last send */
}


#ifdef INCLUDE_SNMPV1

/* FUNCTION: snmp_build_v1trap()
 * 
 * snmp_build_v1trap() - This takes the passed parameters and builds a 
 * v1 trap. The trap is built in a buffer passed by the caller. The 
 * length of the trap image in the buffer is returned, so the caller 
 * can copy it into a transport packet and send it. Returns 0 if 
 * parse error, else returns length of trap image built in passed 
 * buffer. 
 *
 * PARAM1: u_char * trap_buffer
 * PARAM2: unsigned   length
 * PARAM3: struct trap_info * tinfo
 *
 * RETURNS: 
 */

int
snmp_build_v1trap(u_char * trap_buffer,     /* IN - Buffer to build trap packet in */
   unsigned    length,        /* length of trap_buffer */
   struct trap_info *   tinfo)   /* IN - struct with trap data */
{
   long     version  =  SNMP_VERSION_1;
   unsigned commLen;
   u_char * out_auth, * out_header, *  out_pdu;
   u_char * out_varHeader, *  out_varlist, * out_end;
   oid *    sysOid;
   unsigned sysOidLen;
   u_long   myAddr;
   u_long   trapType;
   u_long   specificType;
   u_long   time;
   /* variables used if trap variables are passed in tinfo struct */
   struct trapVar *  tvp;
   int      i;
   int      diff;

   /* set local trap info variables from passed struct */
   sysOid   =    tinfo->objId;
   sysOidLen=    tinfo->objLen;
   commLen  =    tinfo->commLen;
   myAddr   =    tinfo->agent_ip;
   trapType =    tinfo->trapType;
   specificType= tinfo->trapSpecific;
   time     =    tinfo->time;

   out_auth =  trap_buffer;
   out_header= snmp_auth_build(trap_buffer, &length, 
      (u_char *)tinfo->community, &commLen, &version, 
      0);   /* 0 is a dummy len. Correct len encoded later. */

   if (out_header == NULL)
   {
      SNMPERROR("auth build failed");
      return   0;
   }

   out_pdu  = asn_build_header(out_header, &length, (u_char)TRP_REQ_MSG, 90);
   if (out_pdu == NULL)
   {
      SNMPERROR("header build failed");
      return   0;
   }
   trap_buffer = asn_build_objid(out_pdu, &length, 
      (u_char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OBJECT_ID), 
      (oid *)sysOid, sysOidLen);

   if (trap_buffer == NULL)
   {
      SNMPERROR("build enterprise failed");
      return   0;
   }

   /* imbed object Id in trap */
   trap_buffer = asn_build_string(trap_buffer, &length, 
      (u_char)IPADDRESS, (u_char *)&myAddr, 4);
   if(trap_buffer == NULL)
   {
      SNMPERROR("build agent_addr failed");
      return   0;
   }

   trap_buffer = asn_build_int(trap_buffer, &length, 
      (u_char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER), 
      (long *)&trapType, sizeof(trapType));
   if (trap_buffer == NULL)
   {
      SNMPERROR("build trap_type failed");
      return   0;
   }
   trap_buffer = asn_build_int(trap_buffer, &length, 
      (u_char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER), 
      (long *)&specificType, sizeof(specificType));
   if (trap_buffer == NULL)
   {
      SNMPERROR("build specificType failed");
      return   0;
   }

   out_varHeader  = asn_build_int(trap_buffer, &length, 
      (u_char)(TIMETICKS),   (long *)&time, sizeof(time));
   if (out_varHeader == NULL)
   {
      SNMPERROR("build timestampfailed");
      return   0;
   }

   /* Append any variable data to trap packet */
   if(tinfo->trapVars)
   {
      out_varlist = asn_build_header(out_varHeader,  &length, 
      (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), 90);

      /* init parms for variable encoding loop */
      tvp   =  tinfo->vars;   /* get ptr to array of variables structures */
      out_end  =  out_varlist;   /* pointer to next variable encode area */

      for(i = 0; i < tinfo->trapVars; i++)   /* for number of variables */
      {
         out_end  = snmp_build_var_op(out_end, tvp->varName, 
            tvp->varNameLen,  tvp->varType,
            tvp->varValLen,   tvp->varBuf,   &length);

         if (out_end == NULL)
         {
            SNMPERROR("build_var_op failed");
            return   0;
         }
         tvp++;   /* point to next variable in table */
      }

      /* Now rebuild variables header with the actual length */
      if (snmp_build_adjust_len(out_varHeader, out_varlist,
         out_end-out_varlist, &diff,NULL) != TRUE  )
      {
         return   0;
      }
      if ( diff )
      {
         out_end  -= diff  ;
      }
   }
   else  /* no variables, just set end of packet */
   {
#ifdef SNMP_PUTEMPTYVARBIND
      out_varlist = asn_build_header(out_varHeader,  &length, 
         (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), 0);
      out_end  =  out_varlist;
#else
      out_end  =  out_varHeader;
#endif
   }

   /* Now rebuild the trap packet with the actual lengths */
   if (snmp_build_adjust_len(out_header, out_pdu, out_end-out_pdu, 
      &diff,NULL) != TRUE  )
   {
      return   0;
   }
   if ( diff )
   {
      out_end  -= diff  ;
   }
   if (snmp_build_adjust_len(out_auth, out_auth+2, (out_end-out_auth)-2, 
      &diff,NULL) != TRUE  )
   {
      return   0;
   }
   if ( diff )
   {
      out_end  -= diff  ;
   }

   return (out_end - out_auth);
}

#endif /* INCLUDE_SNMPV1 */


#ifdef SNMP_TEST_SP_TRAP

/* FUNCTION: snmp_test_sp_trap()
 * 
 * Sends an enterprise specific trap. Trap with varbinds.
 * Calls send_trap() to send the trap. Can send a SNMPv1/SNMPv2c trap.
 *
 * PARAM1: int version 
 *
 * RETURNS: 
 */

#define  NUM_VB   6

void snmp_test_sp_trap(int version)
{
   struct trapVar sp_var[NUM_VB]=   {
      {{1,3,6,1,2,1,1,1,0},   9, 0, 0, NULL, 0},
      {{1,3,6,1,2,1,1,2,0},   9, 0, 0, NULL, 0},
      {{1,3,6,1,2,1,1,3,0},   9, 0, 0, NULL, 0},
      {{1,3,6,1,2,1,1,4,0},   9, 0, 0, NULL, 0},
      {{1,3,6,1,2,1,1,5,0},   9, 0, 0, NULL, 0},
      {{1,3,6,1,2,1,1,6,0},   9, 0, 0, NULL, 0},
   };

   u_short  acl;
   int   i;
   long     err_code=-1; /* returned by getStatPtr() */

   for ( i=0 ; i < NUM_VB ; i++ )
   {
      sp_var[i].varBuf = getStatPtr(sp_var[i].varName, 
       &(sp_var[i].varNameLen), &(sp_var[i].varType), 
       &(sp_var[i].varValLen), &acl, GET_OP,&err_code,version);
   }

   snmp_trap(SNMP_TRAP_ENTERPRISESPECIFIC,125,NUM_VB,sp_var,version);
}

#endif   /* SNMP_TEST_SP_TRAP */


#endif /* ENABLE_SNMP_TRAPS - whole file is within this ifdef */
#endif /*  INCLUDE_SNMPV1 or INCLUDE_SNMPV2C */

/* end of file trap_out.c */

