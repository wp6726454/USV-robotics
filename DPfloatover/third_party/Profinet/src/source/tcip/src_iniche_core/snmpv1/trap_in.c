/*
 * v3.1 August 30, 2011 snmpv1/trap_in.c
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: trap_in.c
 *
 * Copyright 1990-96 by NetPort Software. All rights reserved.
 * Copyright 1996-2004 By InterNiche Technologies Inc. All rights reserved.
 *
 * Protocol level code to handle incoming traps. 
 *
 * MODULE: SNMPV1
 *
 * ROUTINES: snmp_parse_trap(), 
 *
 * PORTABLE: yes
 */


#include "snmpport.h"

#ifdef INCLUDE_SNMPV1

#include "asn1.h"
#include "npsnmp.h"
#include "snmp_imp.h"

#define  MAX_TRAP_VAR   512   /* max size for a received trap variable */

/* tmp local storage for trap variables. Not on stack due to 
potentially large size of varName[] */
static oid      varName[MAX_OID_LEN];   /* ObjectId of variable */
static unsigned varNameLen;  /* oid components in varName */
static u_char   varType;     /* ASN.1 type of variable */
static unsigned varLen;      /* octets in variable data field */
static u_char   varVal[MAX_TRAP_VAR];  /* the actual variable data */




/* FUNCTION: snmp_parse_trap()
 * 
 * snmp_parse_trap() - decode all the myriad data from a received 
 * trap packet into the structures passed. See snmp_imp.h for usage 
 * details. Returns NULL if structs are all filled in without error, else 
 * a pointer to a bit of text which describes the portion of the 
 * received TRAP PDU that was unparseable. Truncating variables due 
 * to insufficient space in the passed buffers is not considered an 
 * error. 
 *
 * PARAM1: u_char * trapdata
 * PARAM2: unsigned   datalen
 * PARAM3: struct trap_info * trapInfo
 *
 * RETURNS: 
 */

char *   
snmp_parse_trap(u_char * trapdata,  /* IN - actual trap data from packet */
   unsigned    datalen,             /* IN - length of trapdata */
   struct trap_info *   trapInfo)   /* IN/OUT - struct to fill in */
{
   u_char * more;    /* pointer to next unparsed PDU data */
   u_char * nextVar; /* pointer to next unparsed TRAP variable */
   unsigned   left;  /* length of more */
   long     version; /* SNMP version, always v1 */
   u_char   type;    /* data types for checking */
   u_char * varValPtr;
   int   vars_in_struct;
   struct trapVar *  vars; /* trap variables data structures */
   u_long   trapType;
   unsigned length;  /* scratch for passing to asn1 */

   /* do some basic setup of the passed data structures */
   vars_in_struct = trapInfo->trapVars;
   trapInfo->trapVars = 0;       /* bump later if vars are present */

   left = datalen;
   more = snmp_auth_parse(trapdata, &left, (u_char*)trapInfo->community, 
    (unsigned*)&trapInfo->commLen, &version);
   if (!more)
      return "community parse";

   more = asn_parse_header(more, &left, &type);
   if (!more || type != TRP_REQ_MSG)
      return "trap pdu header";

   more = asn_parse_objid(more, &left, &type, &trapInfo->objId[0],
    (unsigned*)&trapInfo->objLen);
   if (!more || type != ASN_OBJECT_ID)
      return "sys ObjId";

   length = 4;       /* number of bytes in IP address buffer */
   more = asn_parse_string(more, &left, &type, 
    (u_char*)(&trapInfo->agent_ip), &length);

   if (!more || type != IPADDRESS)
      return "agent's IP address";

   more = asn_parse_int(more, &left, &type, (long*)&trapType, sizeof(long));
   if (!more || type != ASN_INTEGER || trapType > 6)
      return "trap type";
   trapInfo->trapType = trapType;

   more = asn_parse_int(more, &left, &type, (long*)&trapType, sizeof(long));
   if (!more || type != ASN_INTEGER)
      return "specific type";
   trapInfo->trapSpecific = trapType;

   more = asn_parse_int(more, &left, &type, (long*)&trapInfo->time, sizeof(long));
   if (!more || type != TIMETICKS)
      return "timestamp";

   if (!left)     /* no variables in trap ? */
      return NULL;

   /* rest of packet is a sequence of variables */
   more = asn_parse_header(more, &left, &type);    /* strip off leading SEQ */
   if (!more || type != SEQUENCE)
      return "trap vars header";

   vars = trapInfo->vars;     /* set pointer array to receive variables data */

   while (left)   /* keep parsing & counting vars as long as there's data */
   {
      /* extract variable info for this var */
      length = left;    /* number of valid bytes left in packet buffer */
      varNameLen = MAX_OID_LEN ; /* define the max size of varName */
      nextVar = snmp_parse_var_op(more, varName, &varNameLen, &varType,
       &varLen, &varValPtr, &left);

      if (!nextVar)     /* quit if parse failed */
         return ("var_op header");

      /* extract the variable's value; pointed to by varValPtr: */
      length -= more - nextVar;     /* deduct size of header -> max. length of var */
      switch (varType)
      {
      case ASN_INTEGER:
      case GAUGE:
      case COUNTER:
      case TIMETICKS:
         varLen = sizeof(long);     /* size required by asn_parse_int() */
         nextVar = asn_parse_int(varValPtr, &length, &varType,
          (long *)(&varVal[0]), varLen);
         break;
      case ASN_OCTET_STR:
      case IPADDRESS:
      case OPAQUE:
         varLen = MAX_TRAP_VAR;
         nextVar = asn_parse_string(varValPtr, &length, &varType,
          varVal, &varLen);
         break;
      case ASN_OBJECT_ID:
         varLen = MAX_TRAP_VAR;
         nextVar = asn_parse_objid(varValPtr, &length, &varType,
          (oid *)(&varVal[0]), &varLen);
         break;
      case ASN_NULL:
         nextVar = asn_parse_null(varValPtr, &length, &varType);
         break;
         default:
         SNMPERROR("wrong type");
         return NULL;
      }

      if (!nextVar)
         return("variable");

      /* if another trap variable info slot is available, fill it in */
      if (trapInfo->trapVars <= vars_in_struct)
      {
         oidcpy(vars->varName, varName, varNameLen);
         if (vars->varBufLen <= varLen)   /* need to truncate? */
            vars->varValLen = vars->varBufLen-1;
         else     /* we have room for the full variable data field */
            vars->varValLen = varLen;
         MEMCPY(vars->varBuf, varVal, vars->varValLen);
         vars->varNameLen = varNameLen;
         vars->varType = varType;
         vars++;     /* point to next variable info structure */
      }
      trapInfo->trapVars++;      /* another var */
      more = nextVar;      /* bump pointer to pdu */
   }
   return NULL;
}

#endif /*  INCLUDE_SNMPV1 */

/* end of file trap_in.c */


