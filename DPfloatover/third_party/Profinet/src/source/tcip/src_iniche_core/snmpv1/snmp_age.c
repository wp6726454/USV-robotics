/*
 * v3.1 February 9, 2012 snmpv1/snmp_age.c (bugfix in snmp_agt_parse())
 * v3.1 August 30, 2011 snmpv1/snmp_age.c
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: snmp_age.c
 *
 * Portions Copyright 1988, 1989 by Carnegie Mellon University All Rights Reserved.
 * Copyright 1990-96 by NetPort Software. All rights reserved.
 * Copyright 1996-2004 By InterNiche Technologies Inc. All rights reserved.
 *
 * Simple Network Management Protocol (RFC 1067).
 *
 *
 * MODULE: SNMPV1
 *
 * ROUTINES: snmp_agt_parse(), parse_var_op_list(), 
 * ROUTINES: create_identical(), snmp_access(), get_community(), 
 *
 * PORTABLE: yes
 */

/* Additional Copyrights: */

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

/* NetPort port general includes */
#include "snmpport.h"

#if (defined(INCLUDE_SNMPV1) || defined(INCLUDE_SNMPV2C))

/* snmp protocol includes */
#include "asn1.h"
#include "npsnmp.h"
#include "snmp_imp.h"
#include "snmp_var.h"

#ifdef MIB_COUNTERS
/* include the file with the MIB-2 SNMP counters defined */
#include SNMPMIB_H_FILE   
#endif   /* MIB_COUNTERS */

#define  GET_OP   1  /* oper is GET operation */
#define  NEXT_OP  0  /* oper is GETNEXT operation */
#define  SET_OP   -1 /* oper is SET operation */

int create_identical(u_char * snmp_in, u_char * snmp_out, 
   unsigned snmp_length, long errstat, long errindex);
int create_identical_no_var_bind(u_char *snmp_in, u_char *snmp_out,
                     unsigned snmp_length, long errstat, long errindex);
int snmp_access(u_short acl, unsigned community, unsigned rw);

extern struct snmp_mib   SnmpMib;

struct communityId   communities[NUM_COMMUNITIES]  =  {
   { "private", RWRITE, },
   { "public",  RONLY,  }
};

u_char sidbuf[MAX_COMMUNITY_SIZE+1];   /* Community string buffer */
unsigned sidlen = 0;

/* these can't be global in a re-entrant system */
u_char * packet_end = NULL; /* end of built packet data */
int community;  /* access code for current packet's community string */


/* Set the PDU parsing routine. 
 *   For SNMPv1, use parse_var_op_list() to parse SNMPv1 varbinds
 *   For SNMPv2c, use parse_var_op_v2list() to parse SNMPv1, SNMPv2c varbinds
 */
#ifdef INCLUDE_SNMPV2C
#define SNMP_PARSE_VARBIND   parse_var_op_v2list
#else
#define SNMP_PARSE_VARBIND   parse_var_op_list
#endif

#ifdef INCLUDE_SNMPV3
extern int v3_community_check(u_char * rcvdcomm);
#define SNMP_COMMUNITY_CHECK v3_community_check
#else
#define SNMP_COMMUNITY_CHECK get_community
#endif


/* FUNCTION: snmp_agt_parse()
 * 
 * PARAM1: u_char * data
 * PARAM2: unsigned length
 * PARAM3: u_char * out_data
 * PARAM4: unsigned out_length
 *
 * snmp_agt_parse() - This is the main entry point for incoming snmp 
 * packets directed to the agent's UDP connection. As an embedded 
 * agent we will get these packets by direct upcalls from the UDP 
 * layer. We check authentication, community, message type, and build 
 * a response packet. 
 * 
 * RETURNS: 
 * Returns 0 if no response packet was left in 
 * out_data, else returns size of response packet; 
 * 
 */

int      SNMP_ENTRY
snmp_agt_parse(u_char * data,   /* IN pointer to packet data (past UDP header) */
     unsigned    length,        /* IN/SCRTACH length of packet data */
     u_char *    out_data,      /* IN/OUT buffer for response packet */
     unsigned    out_length)    /* IN/SCRTACH size of output buffer */
{
   u_char   msg_type,   type;
   long     zero  =  0L;
   long     reqid,   errstat, errindex, outerrstat;
   u_char * out_auth;   /* this also happens to be start of out buffer */
   u_char * out_header, *  out_reqid;
   u_char * startData   =  data;
   int      startLength =  length;
   long     version;
   int      header_shift,  auth_shift;

   SNMP_MIB_CTR(snmpInPkts);     /* update MIB-II stats */

   if (length > SNMPSIZ)
   {
      SNMPERROR("incoming pkt too big");
#if 0 /* INICHE_TCIP_ITGR (AP00392322): not according to MIB2 */
      SNMP_MIB_CTR(snmpInTooBigs);
#endif
      /* truncate incoming packet & try to build error reply */
      length = SNMPSIZ;
      outerrstat = SNMP_ERR_TOOBIG;
   }
   else
   {
      outerrstat = SNMP_ERR_NOERROR;
      errindex = 0;
   }

   sidlen = MAX_COMMUNITY_SIZE;
   /* authenticates message and returns length if valid */
   data = snmp_auth_parse(data, &length, sidbuf, &sidlen, &version); 
   if (data == NULL)
   {
      SNMP_MIB_CTR(snmpInASNParseErrs);
      SNMPERROR("bad authentication");
      snmp_trap(SNMP_TRAP_AUTHFAIL, 0, 0, NULL, version);
      return 0;
   }
#if (defined(INCLUDE_SNMPV1) && defined(INCLUDE_SNMPV2C))
   /* Both SNMPv1 and SNMPv2c are defined. Discard any other version */
   if (!((version == SNMP_VERSION_1) || (version == SNMP_VERSION_2)))
   {
      SNMP_MIB_CTR(snmpInBadVersions);
      SNMPERROR("wrong version");
      return 0;
   }
#endif
#if (defined(INCLUDE_SNMPV1) && !defined(INCLUDE_SNMPV2C))
   /* only SNMPv1 is defined. Discard SNMPv2c packets */
   if (version != SNMP_VERSION_1)
   {
      SNMP_MIB_CTR(snmpInBadVersions);
      SNMPERROR("wrong version");
      return 0;
   }
#endif
#if (!defined(INCLUDE_SNMPV1) && defined(INCLUDE_SNMPV2C))
   /* only SNMPv2c is defined. Discard SNMPv1 packets */
   if (version != SNMP_VERSION_2)
   {
      SNMP_MIB_CTR(snmpInBadVersions);
      SNMPERROR("wrong version");
      return 0;
   }
#endif

   community = SNMP_COMMUNITY_CHECK(sidbuf); /* set to RONLY or RWRITE */
   if (community == -1)
   {
      SNMP_MIB_CTR(snmpInBadCommunityNames);
      SNMPERROR("bad community");
      snmp_trap(SNMP_TRAP_AUTHFAIL, 0, 0, NULL, version);
      return 0;
   }
   data = asn_parse_header(data, &length, &msg_type);
   if (data == NULL)
   {
#if 0 /* INICHE_TCIP_ITGR (AP00392322): not according to MIB2 */
      SNMP_MIB_CTR(snmpInGenErrs);
#endif
      SNMPERROR("bad header");
      return 0;
   }

   /* check message type, support mib counters: */
   switch (msg_type)
   {
   case GETNEXT_REQ_MSG:
      SNMP_MIB_CTR(snmpInGetNexts);
      break;
   case GET_REQ_MSG:
      SNMP_MIB_CTR(snmpInGetRequests);
      break;
   case SET_REQ_MSG:
      SNMP_MIB_CTR(snmpInSetRequests);
      break;
   case GETBULK_REQ_MSG:  
      /* no counter for GETBULK in struct snmp_mib */
      break;
   default:
      SNMP_MIB_CTR(snmpInASNParseErrs);
      SNMPERROR("Unsupported message type");
      return 0;   /* check for traps, responses, later */
   }

   data = asn_parse_int(data, &length, &type, (long *)&reqid, sizeof(reqid));
   if (data == NULL)
   {
      SNMP_MIB_CTR(snmpInASNParseErrs);
      SNMPERROR("bad parse of reqid");
      return 0;
   }
   data = asn_parse_int(data, &length, &type, (long *)&errstat, sizeof(errstat));
   if (data == NULL)
   {
      SNMP_MIB_CTR(snmpInASNParseErrs);
      SNMPERROR("bad parse of errstat");
      return 0;
   }
   data = asn_parse_int(data, &length, &type, (long *)&errindex, sizeof(errindex));
   if (data == NULL)
   {
      SNMP_MIB_CTR(snmpInASNParseErrs);
      SNMPERROR("bad parse of errindex");
      return 0;
   }
   /*
    * Now start cobbling together what is known about the output packet.
    * The final lengths are not known now, so they will have to be recomputed
    * later.
    */
   out_auth = out_data;
   out_header = snmp_auth_build(out_auth, &out_length, sidbuf, &sidlen, &version, 0);
   if (out_header == NULL)
   {
      SNMP_MIB_CTR(snmpOutGenErrs);
      SNMPERROR("snmp_auth_build failed");
      return 0;
   }
   out_reqid = asn_build_header(out_header, &out_length, (u_char)GET_RSP_MSG, 0);
   if (out_reqid == NULL)
   {
      SNMP_MIB_CTR(snmpOutGenErrs);
      SNMPERROR("snmp_agt_parse: build opcode failed");
      return 0;
   }

   type = (u_char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER);
   /* return identical request id */
   out_data = asn_build_int(out_reqid, &out_length, type, &reqid, sizeof(reqid));
   if (out_data == NULL)
   {
      SNMP_MIB_CTR(snmpOutGenErrs);
      SNMPERROR("build reqid failed");
      return 0;
   }

   /* assume that error status will be zero */
   out_data = asn_build_int(out_data, &out_length, type, &zero, sizeof(zero));
   if (out_data == NULL)
   {
      SNMP_MIB_CTR(snmpOutGenErrs);
      SNMPERROR("build errstat failed");
      return 0;
   }

   /* assume that error index will be zero */
   out_data = asn_build_int(out_data, &out_length, type, &zero, sizeof(zero));
   if (out_data == NULL)
   {
      SNMP_MIB_CTR(snmpOutGenErrs);
      SNMPERROR("build errindex failed");
      return 0;
   }

   if (outerrstat == SNMP_ERR_NOERROR )
   {
      LOCK_NET_RESOURCE(NET_RESID); /* INICHE_TCIP_ITGR InterNiche's bugbix from February 9, 2012 */
      if (msg_type != SET_REQ_MSG)
      {
         outerrstat = SNMP_PARSE_VARBIND(data, length, out_data, out_length, 
          msg_type, &errindex, errstat, version);
      }
      else     /* just set the variable and exit */
      {
         outerrstat = SNMP_PARSE_VARBIND(data, length, out_data, out_length, 
          msg_type, &errindex, errstat, version);

         UNLOCK_NET_RESOURCE(NET_RESID);

         if (outerrstat < 0)  /* Usually AUTH_ERROR - bad set community */
            return 0;

         /* create the get response packet for the set operation */
         return(create_identical(startData, out_auth, startLength, 
          outerrstat, errindex));
      }
      UNLOCK_NET_RESOURCE(NET_RESID);
   }

   if (outerrstat < 0) return 0;    /* usually AUTH_ERROR */

   switch ((short)outerrstat)
   {
   case SNMP_ERR_NOERROR:
      /*
       * Because of the assumption above that header lengths would be encoded
       * in one byte, things need to be fixed, now that the actual lengths are known.
       */
      header_shift = 0;
      out_length = packet_end - out_reqid;
      if (out_length >= 0x80)
      {
         header_shift++;
         if (out_length > 0xFF)
            header_shift++;
      }
      auth_shift = 0;
      out_length = (packet_end - out_auth) - 2 + header_shift;
      if (out_length >= 0x80)
      {
         auth_shift++;
         if (out_length > 0xFF)
            auth_shift++;
      }
      if (auth_shift + header_shift)
      {
      /*
       * Shift packet (from request id to end of packet) by the sum of the
       * necessary shift counts.
       */
         shift_array(out_reqid, packet_end - out_reqid, auth_shift + header_shift);
         /* Now adjust pointers into the packet */
         packet_end += auth_shift + header_shift;
         out_reqid += auth_shift + header_shift;
         out_header += auth_shift;
      }

      /* re-encode the headers with the real lengths */
      out_data = out_header;
      out_length = packet_end - out_reqid;
      out_data = asn_build_header(out_data, &out_length, GET_RSP_MSG, out_length);
      if (out_data != out_reqid)
      {
         SNMPERROR("internal error: header");
         return 0;
      }
      out_data = out_auth;
      out_length = packet_end - out_auth;
      out_data = snmp_auth_build(out_data, &out_length, sidbuf, &sidlen, 
         &version, packet_end - out_header);
      if (out_data != out_header)
      {
         SNMPERROR("internal error");
         return 0;
      }
      SNMP_MIB_CTR(snmpOutGetResponses);
      break;

   case SNMP_ERR_NOSUCHNAME:
      SNMP_MIB_CTR(snmpOutNoSuchNames);
      goto OutErrPkt;
   case SNMP_ERR_TOOBIG:
      SNMP_MIB_CTR(snmpOutTooBigs);
      errindex = 0;  /* SNMPV1/2 expect err_index=0, and no var bind in reply */        
      if(version == SNMP_VERSION_1)  /* for SNMPV1 return all Var Bind's */
         return(create_identical(startData, out_auth, startLength, outerrstat, errindex));
      else   /* for SNMPV2 don't return any VB (same for SNMPV3) */
         return( create_identical_no_var_bind(startData, out_auth, startLength, outerrstat, errindex));
   case SNMP_ERR_BADVALUE:
      SNMP_MIB_CTR(snmpOutBadValues);
      goto OutErrPkt;
   case SNMP_ERR_READONLY:
      SNMP_MIB_CTR(snmpInBadCommunityUses);
      goto OutErrPkt;
   case ASN_EOM: /* endOfMib */
   case SNMP_ERR_GENERR:
      SNMP_MIB_CTR(snmpOutGenErrs);

      OutErrPkt:

      return(create_identical(startData, out_auth, startLength, outerrstat, errindex));
   default:
      /*
      SNMP_MIB_CTR(snmpSilentDrops);
       */
      return 0;
   }

   /* Assume this will result in an SNMP send & update MIB-II stats */
   SNMP_MIB_CTR(snmpOutPkts);
   return(packet_end - out_auth);
}

#ifndef INCLUDE_SNMPV2C

/* FUNCTION: parse_var_op_list()
 * 
 * Parse_var_op_list goes through the list of variables and retrieves 
 * each one, placing it's value in the output packet. For SET request,
 * the variable is set with the value in the packet. If any 
 * error occurs, an error code is returned. 
 *
 * PARAM1: u_char * data
 * PARAM2: unsigned   length
 * PARAM3: u_char *   out_data
 * PARAM4: unsigned   out_length
 * PARAM5: u_char     msgtype
 * PARAM6: long *     index
 * PARAM6: long       errstat
 * PARAM7: int        version
 *
 * RETURNS: 
 * Returns one of SNMP_ERR codes.
 * If there is no error, it returns SNMP_ERR_NOERROR
 * Some of the other error codes returned are SNMP_ERR_NOSUCHNAME,
 * SNMP_ERR_BADVALUE, and SNMP_ERR_TOOBIG.
 */

int
parse_var_op_list(u_char * data, 
     unsigned    length,
     u_char *    out_data,
     unsigned    out_length,
     u_char      msgtype,
     long *      index,
     long        errstat,
     int         version)
{
   u_char   type;
   oid      var_name[MAX_NAME_LEN];
   unsigned var_name_len, var_val_len;
   u_char   var_val_type, *   var_val, statType;
   u_char * statP;
   unsigned statLen;
   u_short  acl;
   int      rw,   exact;
   u_char * headerP, *  var_list_start;
   unsigned dummyLen;
   int      header_shift;
   long     err_code=-1; /* returned by getStatPtr() */

   /* set exact & rw flag based on message type, which has already  */
   /* been checked for a valid type by caller. */
   if ((msgtype == GETNEXT_REQ_MSG) ||
      (msgtype == GETBULK_REQ_MSG))
   {
      rw = READ;
      exact = NEXT_OP;
   }
   else     /* must be a plain GET or SET */
   {
      if (msgtype == SET_REQ_MSG)
      {
         rw = WRITE;
         exact = SET_OP;
      }
      else
      {
         rw = READ;
         exact = GET_OP;
      }
   }

   data = asn_parse_header(data, &length, &type);
   if (data == NULL)
   {
      SNMPERROR("not enough space for varlist");
      return PARSE_ERROR;
   }
   if (type != (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR))
   {
      SNMPERROR("wrong type");
      return PARSE_ERROR;
   }
   headerP = out_data;
   out_data = asn_build_header(out_data, &out_length, (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), 0);
   if (out_data == NULL)
   {
      SNMP_MIB_CTR(snmpOutTooBigs);
      SNMPERROR("not enough space in output packet");
      return BUILD_ERROR;
   }

   var_list_start = out_data;
   *index = 1;
   set_parms.pduflags = 0;  /* clear the pduflags field */

   while ((int)length > 0)
   {
      /* clear static vars for set paramters */
      set_parms.access_method = NULL;
      set_parms.do_range = FALSE;
      set_parms.vbflags = 0;

      /* parse the name, value pair */
      var_name_len = MAX_NAME_LEN;
      data = snmp_parse_var_op(data, var_name, &var_name_len, &var_val_type, 
       &var_val_len, &var_val, &length);

      if (data == NULL)
         return PARSE_ERROR;

      /* now attempt to retrieve the variable on the local entity */
      statP = getStatPtr(var_name, &var_name_len, &statType, &statLen, &acl,
         exact,&err_code,version);
      if (statP == NULL)
         return SNMP_ERR_NOSUCHNAME;

      /* Check if this user has access rights to this variable */
      if (!snmp_access(acl, community, rw))
      {
         SNMPERROR("SNMP agent: authentication failed\n");
         SNMP_LOGOID((char *)sidbuf, var_name, var_name_len, SNMP_ERR_READONLY, 
            var_val, var_val_type);
         snmp_trap(SNMP_TRAP_AUTHFAIL, 0, 0, NULL, version);
         return (community == (int)RWRITE) ? SNMP_ERR_NOSUCHNAME : AUTH_ERROR;
      }

      if (msgtype == SET_REQ_MSG)
      {
         /* see if the type and value is consistent with this
         entry's variable. Do range checking */
         if (goodValue(var_val,var_val_type, var_val_len, 
            statType, statLen) != TRUE)
         {
            SNMP_LOGOID((char *)sidbuf, var_name, var_name_len, 
               SNMP_ERR_BADVALUE, var_val, var_val_type);
            return SNMP_ERR_BADVALUE;
         }

         /* actually do the set if necessary */
         if (msgtype == SET_REQ_MSG)
         {
            int   err;
            /* see if access_method was set in getStatPtr() call */
            if (set_parms.access_method)
            {
               /* call port routine for custom set of variable */
               err = (*set_parms.access_method)(var_val, var_val_type,
                  var_val_len, statP, statLen);
            }
            else     /* generic set of variable */
            {
               err = setVariable(var_val, var_val_type, statP, statLen);
               SNMP_LOGOID(sidbuf, var_name, var_name_len, err, 
                  var_val, var_val_type);

#if 1 /* INICHE_TCIP_ITGR added hook */
               tcip_iniche_mib2_written(statP);
#endif
            }
            if (err) 
            {
               if (err > SNMP_ERR_GENERR)
                  return SNMP_ERR_BADVALUE;
               else
                  return err; /* return any errors */
            }
         }
         SNMP_MIB_CTR(snmpInTotalSetVars);
      }
      else     /* not a set, just keep mib counter for total requested vars */
      {
         SNMP_MIB_CTR(snmpInTotalReqVars);
      }

      /* retrieve the value of the variable and place it into the outgoing packet */
      out_data = snmp_build_var_op(out_data, var_name, var_name_len, statType, 
       statLen, statP, &out_length);

      if (out_data == NULL)
         return SNMP_ERR_TOOBIG;

      (*index)++;
   }

   packet_end = out_data;  /* save a pointer to the end of the packet */

   /*
    * Because of the assumption above that header lengths would be encoded
    * in one byte, things need to be fixed, now that the actual lengths are known.
    */
   header_shift = 0;
   out_length = packet_end - var_list_start;
   if (out_length >= 0x80)
   {
      header_shift++;
      if (out_length > 0xFF)
         header_shift++;
   }

   if (header_shift)
   {
      shift_array(var_list_start, packet_end - var_list_start, header_shift);
      packet_end += header_shift;
      var_list_start += header_shift;
   }

   /* Now rebuild header with the actual lengths */
   dummyLen = packet_end - var_list_start;
   if (asn_build_header(headerP, &dummyLen, (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), dummyLen) == NULL)
   {
      return SNMP_ERR_TOOBIG;    /* bogus error ???? */
   }
   *index = 0;
   USE_ARG(errstat);
   return SNMP_ERR_NOERROR;
}

#endif  /* ifndef INCLUDE_SNMPV2C */


/* FUNCTION: create_identical()
 * 
 * Create a packet identical to the input packet, except for the 
 * error status and the error index which are set according to the 
 * input variables. This function is called for the following cases 
 * only. 
 * 1. When a GET RESPONSE is to be sent for a SET REQUEST 
 * 2. When a received GET REQUEST has an error and a GET RESPONSE is to 
 *    be sent with the error codes. 
 * 
 * Some implementations use extraneous bytes in length encoding. 
 * For example a length of 4 will be encoded as 82 00 04 instead of 04
 * (that is, len is not minimally encoded).
 * So if we use our asn_build_*() routines, then our packet will be 
 * smaller than the received packet. The other party might not like it. 
 * Hence if SNMP_MINIMAL_LEN_ENC is enabled, then we will use our encoding 
 * routine for PDUHDR. Otherwise we will be use the encoding that 
 * comes with the received packet. 
 *
 * PARAM1: u_char * snmp_in
 * PARAM2: u_char * snmp_out
 * PARAM3: unsigned snmp_length
 * PARAM4: long     errstat
 * PARAM5: long     errindex
 *
 * RETURNS: 
 * Returns 0 if it fails, 
 * else returns size of new packet in snmp_out. 
 *
 */

int
create_identical(u_char * snmp_in, /* SNMP packet to duplicate */
   u_char * snmp_out,              /* OUT - buffer to build duplicate in */
   unsigned snmp_length,           /* length of snmp packet */
   long     errstat,
   long     errindex)
{
   u_char   type;
   u_long   dummy;
   unsigned length, headerLength;
   u_char * headerPtr, *   reqidPtr, * errstatPtr, *  errindexPtr, * varListPtr;

   MEMCPY((char *)snmp_out, (char *)snmp_in, snmp_length);
   length = snmp_length;
   headerPtr = snmp_auth_parse(snmp_out, &length, sidbuf, &sidlen, (long *)&dummy);
   if (headerPtr == NULL)
      return 0;
   reqidPtr = asn_parse_header(headerPtr, &length, (u_char *)&dummy);
   if (reqidPtr == NULL)
      return 0;
   headerLength = length;
   errstatPtr = asn_parse_int(reqidPtr, &length, &type, (long*)&dummy, sizeof(dummy) );   /* request id */
   if (errstatPtr == NULL)
      return 0;
   errindexPtr = asn_parse_int(errstatPtr, &length, &type, (long*)&dummy, sizeof(dummy) );   /* error status */
   if (errindexPtr == NULL)
      return 0;
   varListPtr = asn_parse_int(errindexPtr, &length, &type, (long*)&dummy, sizeof(dummy) );   /* error index */
   if (varListPtr == NULL)
      return 0;

#ifdef INCLUDE_SNMPV2C
   /* If errstat is ASN_EOM then we need to encode the EndOfMib situation.
    * That is done by
    * 1. setting errorStatus and errorIndex as 0
    * 2. setting the corresponding varbind's value to ASN_EOM 
    *    
    * Step 2 was already done in v2agent.c.
    */
    if (errstat == ASN_EOM)
    {
       errstat = 0;
       errindex = 0;
    }
#endif

#ifdef SNMP_MINIMAL_LEN_ENC
   {
      u_char * data;
      data = asn_build_header(headerPtr, &headerLength, GET_RSP_MSG, headerLength);
      if (data != reqidPtr)
         return 0;
   
      length = snmp_length;
      type = (u_char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER);
      data = asn_build_int(errstatPtr, &length, type, &errstat, sizeof(errstat));
      if (data != errindexPtr)
         return 0;
   
      data = asn_build_int(errindexPtr, &length, type, &errindex, sizeof(errindex));
      if (data != varListPtr)
         return 0;
   }
#else /* SNMP_MINIMAL_LEN_ENC */
   /* each of the values to be changed is a "byte". Hence just
    * change it directly.
    */
   *headerPtr = GET_RSP_MSG ; /* Just change the message-type */
   *(errindexPtr-1) = (u_char)errstat; /* INICHE_TCIP_ITGR added cast */
   *(varListPtr-1) = (u_char)errindex; /* INICHE_TCIP_ITGR added cast */

#endif   /* SNMP_MINIMAL_LEN_ENC */

   packet_end = snmp_out + snmp_length;
   return snmp_length;
}



int create_identical_no_var_bind(u_char * snmp_in    , /* SNMP packet to duplicate           */
                                 u_char * snmp_out   , /* OUT - buffer to build duplicate in */
                                 unsigned snmp_length, /* length of snmp packet              */
                                 long     errstat    ,
                                 long     errindex)
{
    u_char     type        ;
    u_long     dummy       ;
#if 1 /* INICHE_TCIP_ITGR make version a long throughout the whole SNMP files */
    long       version     ;   /* extract SNMP Ver from original packet */
#else
    u_long     version     ; /* extract SNMP Ver from original packet */
#endif
    unsigned   length      ;
#if 1 /*INICHE_TCIP_ITGR u_long -> long */
    long       reqid;
#else
    u_long     reqid       ;
#endif
    unsigned   tx_length   ;
    unsigned   headerLength;
    u_char   * headerPtr   ;
    u_char   * reqidPtr    ;
    u_char   * errstatPtr  ;
    u_char   * errindexPtr ;
    u_char   * varListPtr  ;
    u_char   * endPtr      ;
    /* copy rec packet to out buffer ( from 30 82 <lenHigh> <LenLow>....) */
    MEMCPY((char *)snmp_out, (char *)snmp_in, snmp_length);
    length = snmp_length;
    /* -- Process received packet -- */    
    /* extract from RX packet community string, its length, and SNMP version. return ptr to 1st byte after community string */
    headerPtr = snmp_auth_parse(snmp_out, &length, sidbuf, &sidlen, &version);  
    if (headerPtr == NULL)
        return 0;
    reqidPtr = asn_parse_header(headerPtr, &length, (u_char *)&dummy);  /* GET */
    if (reqidPtr == NULL)
        return 0;
    headerLength = length;
    errstatPtr = asn_parse_int(reqidPtr   , &length, &type, &reqid, sizeof(reqid) );   /* request id */
    if (errstatPtr == NULL)
        return 0;
    errindexPtr = asn_parse_int(errstatPtr, &length, &type, (long*)&dummy, sizeof(dummy) );   /* error status */
    if (errindexPtr == NULL)
        return 0;
    varListPtr = asn_parse_int(errindexPtr, &length, &type, (long*)&dummy, sizeof(dummy) );   /* error index */
    if (varListPtr == NULL)
        return 0;    
    headerLength = headerLength - length ; /* calc size of ASN1 RedId + ErrStat + ErrIndex */        
    /* ------- Start to build reply packet based on received processed packet -------- */    
    tx_length = snmp_length ; /* max theoretical pkt size memory space we have ( we don't need so much) */    
    /* Header being build will place requested size+authentication size after 0x30 in the start of SNMP packet 
       Add 1st 2 to size in order to take into account GET_RESPONSE ASN.1 obj 
       Add 2nd 2 to size in order to take into account 2 byte end of SNMP packet  */
    headerPtr = snmp_auth_build(snmp_out, &tx_length, sidbuf, &sidlen,
                                &version, headerLength + 2 + 2 );   
    if (headerPtr == NULL)
        return 0;    
    /* Place GET_RESPONSE and the size of ASN.1 obj which follow it (RedId + ErrStat + ErrIndex)
       Add 2 to size in order to take into account 2 byte end of SNMP packet  */
    reqidPtr  = asn_build_header(headerPtr, &tx_length, GET_RSP_MSG, headerLength + 2 );
    if (reqidPtr == NULL)
        return 0;
    type = (u_char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER);
    errstatPtr = asn_build_int(reqidPtr, &tx_length, type, &reqid, sizeof(reqid));
    if (errstatPtr == NULL)
        return 0;    
    errindexPtr  = asn_build_int(errstatPtr, &tx_length, type, &errstat, sizeof(errstat));
    if (errindexPtr == NULL)
        return 0;
    varListPtr = asn_build_int(errindexPtr, &tx_length, type, &errindex, sizeof(errindex));
    if (varListPtr == NULL)
        return 0;
    endPtr = asn_build_null(varListPtr, &tx_length , ASN_CONSTRUCTOR | ASN_SEQUENCE);
    if (endPtr == NULL)
        return 0;
    tx_length = snmp_length - tx_length  ; /* calc real SNMP size (UDP data) */
    packet_end = snmp_out + tx_length    ; /* calc end memory address of pkt to be send */
    return tx_length;
}

/* FUNCTION: snmp_access()
 * 
 * snmp_access() - Check for a legal combination of variable access, 
 * community priviledge, and operation. This gets called after 
 * community has been looked up via SNMP_COMMUNITY_CHECK(), so we know the 
 * community at least exists in our table... 
 *
 * PARAM1: u_short    acl
 * PARAM2: unsigned   community
 * PARAM3: unsigned   rw
 *
 * RETURNS: 
 * Returns 1 if this access is OK, else returns 0.
 */

int
snmp_access(u_short acl,   /* variable access, RONLY or RWRITE */
   unsigned    community,  /* community access, RONLY or RWRITE */
   unsigned    rw)         /* operation; READ or WRITE */
{
   switch (rw)
   {
      case READ:
         switch (acl)
         {
            case ACCESSIBLE_FOR_NOTIFY:
            case NOACCESS:
            case WRITEONLY:
               return 0; /* reads not allowed */
            case RWRITE:
            case READCREATE:
            case RONLY:
               return 1; /* read operations are OK */
         }
         break;

      case WRITE:
         switch (acl)
         {
            case ACCESSIBLE_FOR_NOTIFY:
            case NOACCESS:
            case RONLY:
#if 1 /* INICHE_TCIP_ITGR: AP00977791 added increment */
               SNMP_MIB_CTR(snmpInBadCommunityUses);
#endif
               return 0; /* writes not allowed */
            case RWRITE:
            case READCREATE:
            case WRITEONLY:
               if (community == RWRITE) 
               {
                  return 1; /* write operations are OK */
               }
               else
               {
                  SNMP_MIB_CTR(snmpInBadCommunityUses);
                  return 0; /* writes not allowed */
               }
         }
         break;
   }

   return 0; /* caller will log failure */
}

/* FUNCTION: get_community()
 * 
 * PARAM1: u_char * rcvdcomm - received community name
 *
 * SNMPv1/SNMPv2c uses get_community() to verify the received 
 * community name.
 *
 * If SNMPv3 is used, then SNMPv1/SNMPv2c calls v3_community_check().
 * If SNMPv3 is not used, then SNMPv1/SNMPv2c calls its own get_community().
 *
 * RETURNS: 
 * If the rcvdcomm is valid, return access permission 
 * Else return -1.
 */

int
get_community(u_char * rcvdcomm)
{
   int   count;

   for (count = 0; count < NUM_COMMUNITIES; count++)
   {
      if (!strcmp(communities[count].name, (char*)rcvdcomm))
         break;
   }
   if (count == NUM_COMMUNITIES)
      return(-1);
   return(communities[count].access);
}

#endif /* INCLUDE_SNMPV1 or INCLUDE_SNMPV2C */

/* end of file snmp_age.c */


