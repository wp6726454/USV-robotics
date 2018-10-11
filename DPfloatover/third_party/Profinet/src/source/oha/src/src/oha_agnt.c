/*****************************************************************************/
/** Portions Copyright 1988, 1989 by Carnegie Mellon University All Rights Reserved. */
/** Copyright 1990-96 by NetPort Software. All rights reserved.              */
/** Copyright 1996-2002 By InterNiche Technologies Inc. All rights reserved. */
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
/*  F i l e               &F: oha_agnt.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA SNMP-Agent functions                         */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  14
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_AGNT */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "oha_inc.h"            /* OHA headerfiles  */
#include "oha_int.h"            /* internal header  */

/* BTRACE-IF */
OHA_FILE_SYSTEM_EXTENSION(OHA_MODULE_ID)

#include "oha_asn1.h"
#include "oha_agnt.h"           /* OHA-Agent header */
#include "oha_snmp.h"           /* SNMP header      */

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

struct communityId   oha_communities[NUM_COMMUNITIES]  =
{
    {"private", OHA_SNMP_RWRITE},
    {"public", OHA_SNMP_RONLY},
};

U_CHAR   oha_sidbuf[OHA_MAX_COMMUNITY_SIZE  +  1];   /* Community string buffer */
unsigned oha_sidlen = 0;

/* these can't be global in a re-entrant system */
U_CHAR * oha_packet_end  =  LSA_NULL; /* end of built packet data */
int      oha_community;  /* access code for current packet's community string */

/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/

int oha_snmp_create_identical(
    U_CHAR * snmp_in, U_CHAR * snmp_out,
    unsigned snmp_length, LSA_INT32 errstat, LSA_INT32 errindex);

int oha_snmp_create_identical_no_var_bind(
    U_CHAR * snmp_in, 
    U_CHAR * snmp_out,
    unsigned snmp_length, 
    LSA_INT32 errstat, 
    LSA_INT32 errindex);

int oha_snmp_access(
    U_SHORT acl, 
    unsigned community, 
    unsigned rw);

/*  int snmp_get_community(U_CHAR*); */
int oha_snmp_get_community(
    U_CHAR*, unsigned);

int oha_snmp_parse_var_op_list(
	OHA_OID,
    U_CHAR*, 
    unsigned, 
    U_CHAR*, 
    unsigned, 
    U_CHAR, 
    LSA_INT32*,
    LSA_INT32, 
    int version);

int oha_snmp_parse_var_op_v2list(
	OHA_OID,
    U_CHAR*, 
    unsigned, 
    U_CHAR*, 
    unsigned, 
    U_CHAR, LSA_INT32*,
    LSA_INT32, int version);

U_CHAR * oha_snmp_auth_parse(
    U_CHAR*, 
    unsigned*, 
    U_CHAR*, 
    unsigned*, 
    LSA_INT32*);

U_CHAR * oha_snmp_auth_build(
    U_CHAR*, 
    unsigned*, 
    U_CHAR*, 
    unsigned*, 
    LSA_INT32*,
    unsigned);

/* Set the PDU parsing routine.
 *   For SNMPv1, use snmp_parse_var_op_list() to parse SNMPv1 varbinds
 *   For SNMPv2c, use snmp_parse_var_op_v2list() to parse SNMPv1, SNMPv2c varbinds
 */
#ifdef INCLUDE_SNMPV2C
#define SNMP_PARSE_VARBIND   oha_snmp_parse_var_op_v2list
#else
#define SNMP_PARSE_VARBIND   oha_snmp_parse_var_op_list
#endif

#ifdef INCLUDE_SNMPV3
/* extern int v3_community_check(U_CHAR * rcvdcomm);      */
extern int oha_snmp_v3_community_check(U_CHAR * rcvdcomm, unsigned sidlen);
#define SNMP_COMMUNITY_CHECK oha_snmp_v3_community_check
#else
#define SNMP_COMMUNITY_CHECK oha_snmp_get_community
#endif


/* FUNCTION: snmp_agt_parse()
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned length
 *
 * snmp_msg_parse() - This is the main entry point for incoming snmp
 * packets directed to the agent's UDP connection. As an embedded
 * agent we will get these packets by direct upcalls from the UDP
 * layer. We check authentication, community, message type, and return
 * the snmp message type.
 *
 * RETURNS:
 * Returns 0 if no message type was found in the packet
 * else returns SNMP message type;
 *
 */
LSA_UINT8 oha_snmp_msg_parse(
    LSA_UINT8 * data,   /* IN pointer to packet data (past UDP header) */
    LSA_UINT    length)        /* IN/SCRTACH length of packet data */
{
    U_CHAR   msg_type = 0xff;
    LSA_INT32  version  = 0;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_msg_parse(data: 0x%X, length: 0x%X)", data, length);

    if (length > OHA_SNMP_SIZE)
    {
        OHA_SNMPERROR("oha_snmp_agt_parse(): incoming pkt too big");

        /* truncate incoming packet */
        length = OHA_SNMP_SIZE;
    }

    oha_sidlen = OHA_MAX_COMMUNITY_SIZE;
    /* authenticates message and returns length if valid */
    data = oha_snmp_auth_parse(data, &length, oha_sidbuf, &oha_sidlen, &version);
    if (data == LSA_NULL)
    {
        OHA_SNMPERROR("oha_snmp_msg_parse(): bad authentication");
        return msg_type;
    }

    data = oha_asn_parse_header(data, &length, &msg_type);
    if (data == LSA_NULL)
    {
        OHA_SNMPERROR("oha_snmp_msg_parse(): bad header");
        return msg_type;
    }

    /* get message type */
    switch (msg_type)
    {
        case GETNEXT_REQ_MSG:
			msg_type = OHA_SNMP_GETNEXT_REQ;
            break;
        case GET_REQ_MSG:
        case GETBULK_REQ_MSG:
			msg_type = OHA_SNMP_GET_REQ;
            break;
        case SET_REQ_MSG:
			msg_type = OHA_SNMP_SET_REQ;
            break;
        case GET_RSP_MSG:
			msg_type = OHA_SNMP_GET_RSP;
            break;
        default:
		    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_msg_parse(unsupported message type: 0x%X)", msg_type);
            return msg_type;   /* check for traps, responses, later */
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_msg_parse(message type: 0x%X)", msg_type);
    return(msg_type);
}


/* FUNCTION: snmp_agt_parse()
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned length
 * PARAM3: U_CHAR * out_data
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

LSA_INT  oha_snmp_agt_parse(
    OHA_OID mib_node,    /* IN - OHA_LLDP_MIB_NODE (8802) or OHA_MRP_MIB_NODE (62439) */
    LSA_UINT8 * data,   /* IN pointer to packet data (past UDP header) */
    LSA_UINT    length,        /* IN/SCRTACH length of packet data */
    LSA_UINT8 *    out_data,      /* IN/OUT buffer for response packet */
    LSA_UINT    out_length,    /* IN/SCRTACH size of output buffer */
    LSA_BOOL *  oha_oid)       /* OUT/True: it's an LLDP-Oid, False: no lldp-oid */
{
    U_CHAR   msg_type = 0;
    U_CHAR   type;
    LSA_INT32  zero  =  0L;
    LSA_INT32  reqid, outerrstat;
    LSA_INT32  errstat = 0;
    LSA_INT32  errindex = 0;
    U_CHAR * out_auth;   /* this also happens to be start of out buffer */
    U_CHAR * out_header, *  out_reqid;
    U_CHAR * startData   =  data;
    int      startLength =  length;
    LSA_INT32  version     = 0;
    int      header_shift,  auth_shift;

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_agt_parse(data: 0x%X, length: 0x%X, out_data: 0x%X, out_length: 0x%X, oha_oid: 0x%X)",
                          data, length, out_data, out_length, oha_oid);
    OHA_SNMP_MIB_CTR(snmpInPkts);     /* update MIB-II stats */

    if (OHA_IS_NOT_NULL(oha_oid))
    {
        * oha_oid = LSA_TRUE;    /* it's an oid for OHA */
    }
    if (length > OHA_SNMP_SIZE)
    {
        OHA_SNMPERROR("oha_snmp_agt_parse(): incoming pkt too big");
        OHA_SNMP_MIB_CTR(snmpInTooBigs);

        /* truncate incoming packet & try to build error reply */
        length = OHA_SNMP_SIZE;
        outerrstat = OHA_SNMP_ERR_TOOBIG;
    }
    else
    {
        outerrstat = OHA_SNMP_ERR_NOERROR;
        errindex = 0;
    }

    oha_sidlen = OHA_MAX_COMMUNITY_SIZE;
    /* authenticates message and returns length if valid */
    data = oha_snmp_auth_parse(data, &length, oha_sidbuf, &oha_sidlen, &version);
    if (data == LSA_NULL)
    {
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        OHA_SNMPERROR("oha_snmp_agt_parse(): bad authentication");
        oha_snmp_trap(SNMP_TRAP_AUTHFAIL, 0, 0, LSA_NULL, version);
        return 0;
    }
#if (defined(INCLUDE_SNMPV1) && defined(INCLUDE_SNMPV2C))
    /* Both SNMPv1 and SNMPv2c are defined. Discard any other version */
    if (!((version == SNMP_VERSION_1) || (version == SNMP_VERSION_2)))
    {
        OHA_SNMP_MIB_CTR(snmpInBadVersions);
        OHA_SNMPERROR("oha_snmp_agt_parse(): wrong version");
        return 0;
    }
#endif
#if (defined(INCLUDE_SNMPV1) && !defined(INCLUDE_SNMPV2C))
    /* only SNMPv1 is defined. Discard SNMPv2c packets */
    if (version != OHA_SNMP_VERSION_1)
    {
        OHA_SNMP_MIB_CTR(snmpInBadVersions);
        OHA_SNMPERROR("oha_snmp_agt_parse(): wrong version");
        return 0;
    }
#endif
#if (!defined(INCLUDE_SNMPV1) && defined(INCLUDE_SNMPV2C))
    /* only SNMPv2c is defined. Discard SNMPv1 packets */
    if (version != OHA_SNMP_VERSION_2)
    {
        OHA_SNMP_MIB_CTR(snmpInBadVersions);
        OHA_SNMPERROR("oha_snmp_agt_parse(): wrong version");
        return 0;
    }
#endif

    /* oha_community = SNMP_COMMUNITY_CHECK(oha_sidbuf); */ /* set to RONLY or RWRITE */
    oha_community = SNMP_COMMUNITY_CHECK(oha_sidbuf, oha_sidlen); /* set to RONLY or RWRITE */
    if (oha_community == -1)
    {
        OHA_SNMP_MIB_CTR(snmpInBadCommunityNames);
        OHA_SNMPERROR("oha_snmp_agt_parse(): bad community");
        oha_snmp_trap(SNMP_TRAP_AUTHFAIL, 0, 0, LSA_NULL, version);
        return 0;
    }
    data = oha_asn_parse_header(data, &length, &msg_type);
    if (data == LSA_NULL)
    {
        OHA_SNMP_MIB_CTR(snmpInGenErrs);
        OHA_SNMPERROR("oha_snmp_agt_parse(): bad header");
        return 0;
    }

    /* check message type, support mib counters: */
    switch (msg_type)
    {
        case GETNEXT_REQ_MSG:
            OHA_SNMP_MIB_CTR(snmpInGetNexts);
            break;
        case GET_REQ_MSG:
            OHA_SNMP_MIB_CTR(snmpInGetRequests);
            break;
        case SET_REQ_MSG:
            OHA_SNMP_MIB_CTR(snmpInSetRequests);
            break;
        case GETBULK_REQ_MSG:
            /* no counter for GETBULK in struct snmp_mib */
            break;
        default:
            OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
            OHA_SNMPERROR("oha_snmp_agt_parse(): Unsupported message type");
            return 0;   /* check for traps, responses, later */
    }

    data = oha_asn_parse_int(data, &length, &type, (LSA_INT32 *)&reqid, sizeof(reqid));
    if (data == LSA_NULL)
    {
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        OHA_SNMPERROR("oha_snmp_agt_parse(): bad parse of reqid");
        return 0;
    }
    data = oha_asn_parse_int(data, &length, &type, (LSA_INT32 *)&errstat, sizeof(errstat));
    if (data == LSA_NULL)
    {
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        OHA_SNMPERROR("oha_snmp_agt_parse(): bad parse of errstat");
        return 0;
    }
    data = oha_asn_parse_int(data, &length, &type, (LSA_INT32 *)&errindex, sizeof(errindex));
    if (data == LSA_NULL)
    {
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        OHA_SNMPERROR("oha_snmp_agt_parse(): bad parse of errindex");
        return 0;
    }
    /*
     * Now start cobbling together what is known about the output packet.
     * The final lengths are not known now, so they will have to be recomputed
     * later.
     */
    out_auth = out_data;
    out_header = oha_snmp_auth_build(out_auth, &out_length, oha_sidbuf, &oha_sidlen, &version, 0);
    if (out_header == LSA_NULL)
    {
        OHA_SNMP_MIB_CTR(snmpOutGenErrs);
        OHA_SNMPERROR("oha_snmp_agt_parse(): snmp_auth_build failed");
        return 0;
    }
    out_reqid = oha_asn_build_header(out_header, &out_length, (U_CHAR)GET_RSP_MSG, 0);
    if (out_reqid == LSA_NULL)
    {
        OHA_SNMP_MIB_CTR(snmpOutGenErrs);
        OHA_SNMPERROR("oha_snmp_agt_parse(): build opcode failed");
        return 0;
    }

    type = (U_CHAR)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER);
    /* return identical request id */
    out_data = oha_asn_build_int(out_reqid, &out_length, type, &reqid, sizeof(reqid));
    if (out_data == LSA_NULL)
    {
        OHA_SNMP_MIB_CTR(snmpOutGenErrs);
        OHA_SNMPERROR("oha_snmp_agt_parse(): build reqid failed");
        return 0;
    }

    /* assume that error status will be zero */
    out_data = oha_asn_build_int(out_data, &out_length, type, &zero, sizeof(zero));
    if (out_data == LSA_NULL)
    {
        OHA_SNMP_MIB_CTR(snmpOutGenErrs);
        OHA_SNMPERROR("oha_snmp_agt_parse(): build errstat failed");
        return 0;
    }

    /* assume that error index will be zero */
    out_data = oha_asn_build_int(out_data, &out_length, type, &zero, sizeof(zero));
    if (out_data == LSA_NULL)
    {
        OHA_SNMP_MIB_CTR(snmpOutGenErrs);
        OHA_SNMPERROR("oha_snmp_agt_parse(): build errindex failed");
        return 0;
    }

    if (outerrstat == OHA_SNMP_ERR_NOERROR )
    {
        if (msg_type != SET_REQ_MSG)
        {
            outerrstat = SNMP_PARSE_VARBIND(mib_node, data, length, out_data, out_length,
                                            msg_type, &errindex, errstat, version);
        }
        else     /* just set the variable and exit */
        {
            outerrstat = SNMP_PARSE_VARBIND(mib_node, data, length, out_data, out_length,
                                            msg_type, &errindex, errstat, version);

            if (outerrstat < 0)  /* Usually AUTH_ERROR - bad set community */
                return 0;

            if (outerrstat != OHA_SNMP_ERR_NOERROR)	/* delete the set options - they're set by getStatPtr() */
			{
				oha_snmp_delete_lldp_statistics(); /* AP01388134 */
			}

            if (outerrstat == OHA_SNMP_ERR_NOSUCHNAME)
            {                          /* only if getStatPtr() delivers NoSuchName */
                if (OHA_IS_NOT_NULL(oha_oid))
                {
                    * oha_oid = LSA_FALSE;  /* it's not an oid for OHA */
                }
            }

            /* create the get response packet for the set operation */
            return (oha_snmp_create_identical(startData, out_auth, startLength,
                                              outerrstat, errindex));
        }
    }

    if (outerrstat < 0) return 0;    /* usually AUTH_ERROR */

    switch ((short)outerrstat)
    {
        case OHA_SNMP_ERR_NOERROR:
            /*
             * Because of the assumption above that header lengths would be encoded
             * in one byte, things need to be fixed, now that the actual lengths are known.
             */
            header_shift = 0;
            out_length = oha_packet_end - out_reqid;
            if (out_length >= 0x80)
            {
                header_shift++;
                if (out_length > 0xFF)
                    header_shift++;
            }
            auth_shift = 0;
            out_length = (oha_packet_end - out_auth) - 2 + header_shift;
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
                oha_snmp_shift_array(out_reqid, oha_packet_end - out_reqid, auth_shift + header_shift);
                /* Now adjust pointers into the packet */
                oha_packet_end += auth_shift + header_shift;
                out_reqid += auth_shift + header_shift;
                out_header += auth_shift;
            }

            /* re-encode the headers with the real lengths */
            out_data = out_header;
            out_length = oha_packet_end - out_reqid;
            out_data = oha_asn_build_header(out_data, &out_length, GET_RSP_MSG, out_length);
            if (out_data != out_reqid)
            {
                OHA_SNMPERROR("oha_snmp_agt_parse(): internal error: header");
                return 0;
            }
            out_data = out_auth;
            out_length = oha_packet_end - out_auth;
            out_data = oha_snmp_auth_build(out_data, &out_length, oha_sidbuf, &oha_sidlen,
                                           &version, oha_packet_end - out_header);
            if (out_data != out_header)
            {
                OHA_SNMPERROR("oha_snmp_agt_parse(): internal error");
                return 0;
            }
            OHA_SNMP_MIB_CTR(snmpOutGetResponses);
            break;

        case OHA_SNMP_ERR_NOSUCHNAME:
            OHA_SNMP_MIB_CTR(snmpOutNoSuchNames);
            goto OutErrPkt;
        case OHA_SNMP_ERR_TOOBIG:
            OHA_SNMP_MIB_CTR(snmpOutTooBigs);
            errindex = 0;  /* SNMPV1/2 expect err_index=0, and no var bind in reply */
            if (OHA_IS_NOT_NULL(oha_oid))
            {
                * oha_oid = LSA_FALSE;  /* it's not an oid for OHA */
            }
            if (version == OHA_SNMP_VERSION_1) /* for SNMPV1 return all Var Bind's */
                return(oha_snmp_create_identical(startData, out_auth, startLength, outerrstat, errindex));
            else   /* for SNMPV2 don't return any VB (same for SNMPV3) */
                return( oha_snmp_create_identical_no_var_bind(startData, out_auth, startLength, outerrstat, errindex));
        case OHA_SNMP_ERR_BADVALUE:
            OHA_SNMP_MIB_CTR(snmpOutBadValues);
            goto OutErrPkt;
        case OHA_SNMP_ERR_READONLY:
            OHA_SNMP_MIB_CTR(snmpInBadCommunityUses);
            goto OutErrPkt;
        case OHA_SNMP_ASN_EOM: /* endOfMib */
        case OHA_SNMP_ERR_GENERR:
            OHA_SNMP_MIB_CTR(snmpOutGenErrs);

OutErrPkt:

            if (OHA_IS_NOT_NULL(oha_oid))
            {
                * oha_oid = LSA_FALSE;  /* it's not an oid for OHA */
            }
            return(oha_snmp_create_identical(startData, out_auth, startLength,
                                             outerrstat, errindex));
        default:
            /*
            OHA_SNMP_MIB_CTR(snmpSilentDrops);
             */
            return 0;
    }

    /* Assume this will result in an SNMP send & update MIB-II stats */
    OHA_SNMP_MIB_CTR(snmpOutPkts);
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_agt_parse(RetLength: 0x%X)", (oha_packet_end - out_auth));
    return(oha_packet_end - out_auth);
}

#ifndef INCLUDE_SNMPV2C

/* FUNCTION: snmp_parse_var_op_list()
 *
 * Parse_var_op_list goes through the list of variables and retrieves
 * each one, placing it's value in the output packet. If doSet is
 * non-zero, the variable is set with the value in the packet. If any
 * error occurs, an error code is returned.
 *
 * PARAM1: OHA_OID    mib_node
 * PARAM2: U_CHAR * data
 * PARAM3: unsigned   length
 * PARAM4: U_CHAR *   out_data
 * PARAM5: unsigned   out_length
 * PARAM6: U_CHAR     msgtype
 * PARAM7: LSA_INT32 *  index
 * PARAM8: LSA_INT32    errstat
 * PARAM9: int        version
 *
 * RETURNS:
 * Returns one of SNMP_ERR codes.
 * If there is no error, it returns SNMP_ERR_NOERROR
 * Some of the other error codes returned are SNMP_ERR_NOSUCHNAME,
 * SNMP_ERR_BADVALUE, and SNMP_ERR_TOOBIG.
 */

int
oha_snmp_parse_var_op_list(
    OHA_OID mib_node,    /* IN - OHA_LLDP_MIB_NODE (8802) or OHA_MRP_MIB_NODE (62439) */
    U_CHAR * data,
    unsigned    length,
    U_CHAR *    out_data,
    unsigned    out_length,
    U_CHAR      msgtype,
    LSA_INT32 *   index,
    LSA_INT32     errstat,
    int         version)
{
    U_CHAR   type = 0;
    OHA_OID  var_name[OHA_MAX_NAME_LEN];
    unsigned var_name_len;
    unsigned var_val_len = 0;
    U_CHAR   statType = 0;
    U_CHAR   var_val_type = 0;
    U_CHAR * var_val = LSA_NULL;
    U_CHAR * statP;
    unsigned statLen = 0;
    U_SHORT  acl = 0;
    int      exact;
    U_CHAR * headerP, *  var_list_start;
    unsigned dummyLen;
    int      header_shift;
    LSA_INT32     err_code=-1; /* returned by snmp_getStatPtr() */

    OHA_FUNCTION_TRACE_06(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_parse_var_op_list(mib_node: 0x%X, data: 0x%X, length: 0x%X, out_data: 0x%X, out_length: 0x%X, msgtype: 0x%X)",
                          mib_node, data, length, out_data, out_length, msgtype);
    /* set exact & rw flag based on message type, which has already  */
    /* been checked for a valid type by caller. */
    if ((msgtype == GETNEXT_REQ_MSG) ||
        (msgtype == GETBULK_REQ_MSG))
    {
        /* rw = READ; */
        exact = NEXT_OP;
    }
    else     /* must be a plain GET or SET */
    {
        if (msgtype == SET_REQ_MSG)
        {
            /* rw = WRITE; */
            exact = SET_OP;
        }
        else
        {
            /* rw = READ; */
            exact = GET_OP;
        }
    }

    data = oha_asn_parse_header(data, &length, &type);
    if (data == LSA_NULL)
    {
        OHA_SNMPERROR("oha_snmp_parse_var_op_list(): not enough space for varlist");
        return PARSE_ERROR;
    }
    if (type != (U_CHAR)(ASN_SEQUENCE | ASN_CONSTRUCTOR))
    {
        OHA_SNMPERROR("oha_snmp_parse_var_op_list(): wrong type");
        return PARSE_ERROR;
    }
    headerP = out_data;
    out_data = oha_asn_build_header(out_data, &out_length, (U_CHAR)(ASN_SEQUENCE | ASN_CONSTRUCTOR), 0);
    if (out_data == LSA_NULL)
    {
        OHA_SNMP_MIB_CTR(snmpOutTooBigs);
        OHA_SNMPERROR("oha_snmp_parse_var_op_list(): not enough space in output packet");
        return BUILD_ERROR;
    }

    var_list_start = out_data;
    *index = 1;
    oha_set_parms.pduflags = 0;  /* clear the pduflags field */

	/* AP01388134: more than one mib object possible */
	oha_init_stats_info(mib_node);

    while ((int)length > 0)
    {
        /* clear static vars for set paramters */
        oha_set_parms.access_method = LSA_NULL;
        oha_set_parms.do_range = LSA_FALSE;
        oha_set_parms.vbflags = 0;

        /* parse the name, value pair */
        var_name_len = OHA_MAX_NAME_LEN;
        data = oha_snmp_parse_var_op(data, var_name, &var_name_len, &var_val_type,
                                     &var_val_len, &var_val, &length);

        if (data == LSA_NULL)
            return PARSE_ERROR;

        /* now attempt to retrieve the variable on the local entity */
        statP = oha_snmp_getStatPtr(mib_node, var_name, &var_name_len, &statType, &statLen, &acl,
                                    exact,&err_code,version);
        if (statP == LSA_NULL)
            return OHA_SNMP_ERR_NOSUCHNAME;

        /* Check if this user has access rights to this variable */
        if (!oha_snmp_access(acl, oha_community, exact))
        {
            OHA_SNMPERROR("oha_snmp_parse_var_op_list(): agent authentication failed");
            OHA_SNMP_LOGOID((char *)oha_sidbuf, var_name, var_name_len, OHA_SNMP_ERR_READONLY,
                            var_val, var_val_type);
            oha_snmp_trap(SNMP_TRAP_AUTHFAIL, 0, 0, LSA_NULL, version);
            return (oha_community == (int)OHA_SNMP_RWRITE) ? OHA_SNMP_ERR_NOSUCHNAME : AUTH_ERROR;
        }

        if (msgtype == SET_REQ_MSG)
        {
            /* see if the type and value is consistent with this
            entry's variable. Do range checking */
            if (oha_snmp_goodValue(var_val,var_val_type, var_val_len,
                                   statType, statLen) /*!= TRUE*/)
            {
                OHA_SNMPERROR("oha_snmp_parse_var_op_list(): type or value failed (InASNParseErrs)");
                OHA_SNMP_LOGOID((char *)oha_sidbuf, var_name, var_name_len,
                                OHA_SNMP_ERR_BADVALUE, var_val, var_val_type);
                return OHA_SNMP_ERR_BADVALUE;
            }

            /* actually do the set if necessary */
            if (msgtype == SET_REQ_MSG)
            {
                int   err; /* OHA_SNMP_ERR_BADVALUE,... */
                /* see if access_method was set in snmp_getStatPtr() call */
                if (oha_set_parms.access_method)
                {
                    /* call port routine for custom set of variable */
                    err = (*oha_set_parms.access_method)(OHA_HANDLE_TYPE_SOCK, var_val, var_val_type, var_val_len,
                                                         var_name, var_name_len);
                }
                else     /* generic set of variable */
                {
                    err = oha_snmp_setVariable(var_val, var_val_type, statP, statLen);
                    OHA_SNMP_LOGOID(oha_sidbuf, var_name, var_name_len, err,
                                    var_val, var_val_type);
                }
                if (err)
                {
                    if (err > OHA_SNMP_ERR_GENERR)
                        return OHA_SNMP_ERR_BADVALUE;
                    else
                        return err; /* return any errors */
                }
            }
            OHA_SNMP_MIB_CTR(snmpInTotalSetVars);
        }
        else     /* not a set, just keep mib counter for total requested vars */
        {
            OHA_SNMP_MIB_CTR(snmpInTotalReqVars);
        }

        /* retrieve the value of the variable and place it into the outgoing packet */
        out_data = oha_snmp_build_var_op(out_data, var_name, var_name_len, statType,
                                         statLen, statP, &out_length);

        if (out_data == LSA_NULL)
            return OHA_SNMP_ERR_TOOBIG;

        (*index)++;
    }

    oha_packet_end = out_data;  /* save a pointer to the end of the packet */

    /*
     * Because of the assumption above that header lengths would be encoded
     * in one byte, things need to be fixed, now that the actual lengths are known.
     */
    header_shift = 0;
    out_length = oha_packet_end - var_list_start;
    if (out_length >= 0x80)
    {
        header_shift++;
        if (out_length > 0xFF)
            header_shift++;
    }

    if (header_shift)
    {
        oha_snmp_shift_array(var_list_start, oha_packet_end - var_list_start, header_shift);
        oha_packet_end += header_shift;
        var_list_start += header_shift;
    }

    /* Now rebuild header with the actual lengths */
    dummyLen = oha_packet_end - var_list_start;
    if (oha_asn_build_header(headerP, &dummyLen, (U_CHAR)(ASN_SEQUENCE | ASN_CONSTRUCTOR), dummyLen) == LSA_NULL)
    {
        return OHA_SNMP_ERR_TOOBIG;    /* bogus error ? */
    }
    *index = 0;
    LSA_UNUSED_ARG(errstat);
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_parse_var_op_list() - OHA_SNMP_ERR_NOERROR");
    return OHA_SNMP_ERR_NOERROR;
}

#endif  /* ifndef INCLUDE_SNMPV2C */


/* FUNCTION: snmp_create_identical()
 *
 * Create a packet identical to the input packet, except for the
 * error status and the error index which are set according to the
 * input variables. This function is called for the following cases
 * only. 1. When a GET RESPONSE is to be sent for a SET REQUEST 2.
 * When a received GET REQUEST has an error and a GET RESPONSE is to
 * be sent with the error codes. Some implementations use extraneous
 * bytes in length encoding. For example a length of 4 will be
 * encoded as 82 00 04 instead of 04. So if we use our asn_build_*()
 * routines, then our packet will be smaller than the received
 * packet. The other party might not like it. Hence if
 * SNMP_ERR_PDUHDR_ENC is enabled, then we will use our encoding
 * routine for PDUHDR. Otherwise we will be use the encoding that
 * comes with the received packet. Returns 0 if it fails, else
 * returns size of new packet in snmp_out.
 *
 * PARAM1: U_CHAR * snmp_in
 * PARAM2: U_CHAR * snmp_out
 * PARAM3: unsigned snmp_length
 * PARAM4: LSA_INT32  errstat
 * PARAM5: LSA_INT32  errindex
 *
 * RETURNS:
 * Returns 0 if it fails,
 * else returns size of new packet in snmp_out.
 */

int oha_snmp_create_identical(
    U_CHAR * snmp_in,     /* SNMP packet to duplicate */
    U_CHAR * snmp_out,   /* OUT - buffer to build duplicate in */
    unsigned snmp_length,   /* length of snmp packet */
    LSA_INT32  errstat,
    LSA_INT32  errindex)
{
    U_CHAR   type;
    U_LONG   dummy;
    unsigned length;
#ifdef SNMP_ERR_PDUHDR_ENC
    unsigned headerLength;
#endif
    U_CHAR * headerPtr, *   reqidPtr, * errstatPtr, *  errindexPtr, * varListPtr;

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_create_identical(snmp_in: 0x%X, snmp_out: 0x%X, snmp_length: 0x%X, errstat: 0x%X, errindex: 0x%X)",
                          snmp_in, snmp_out, snmp_length, errstat, errindex);
    OHA_MEMCPY((char *)snmp_out, (char *)snmp_in, snmp_length);
    length = snmp_length;
    headerPtr = oha_snmp_auth_parse(snmp_out, &length, oha_sidbuf, &oha_sidlen, (LSA_INT32 *)&dummy);
    if (headerPtr == LSA_NULL)
        return 0;
    reqidPtr = oha_asn_parse_header(headerPtr, &length, (U_CHAR *)&dummy);
    if (reqidPtr == LSA_NULL)
        return 0;
#ifdef SNMP_ERR_PDUHDR_ENC
    headerLength = length;
#endif
    errstatPtr = oha_asn_parse_int(reqidPtr, &length, &type, (LSA_INT32*)&dummy, sizeof(dummy) );   /* request id */
    if (errstatPtr == LSA_NULL)
        return 0;
    errindexPtr = oha_asn_parse_int(errstatPtr, &length, &type, (LSA_INT32*)&dummy, sizeof(dummy) );   /* error status */
    if (errindexPtr == LSA_NULL)
        return 0;
    varListPtr = oha_asn_parse_int(errindexPtr, &length, &type, (LSA_INT32*)&dummy, sizeof(dummy) );   /* error index */
    if (varListPtr == LSA_NULL)
        return 0;

#ifdef INCLUDE_SNMPV2C
    /* If errstat is ASN_EOM then we need to encode the EndOfMib situation.
     * That is done by
     * 1. setting errorStatus and errorIndex as 0
     * 2. setting the corresponding varbind's value to ASN_EOM
     *
     * Step 2 was already done in v2agent.c.
     */
    if (errstat == OHA_SNMP_ASN_EOM)
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
    *headerPtr = GET_RSP_MSG;  /* Just change the message-type */
    *(errindexPtr-1) = (U_CHAR)errstat;
    *(varListPtr-1) = (U_CHAR)errindex;
#endif   /* SNMP_MINIMAL_LEN_ENC */

    oha_packet_end = snmp_out + snmp_length;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_create_identical(RetLength: 0x%X)", snmp_length);
    return snmp_length;
}

LSA_INT oha_snmp_create_ident(LSA_UINT8 *in, LSA_UINT8 *out, LSA_UINT length, LSA_UINT32 errindex)
{
	int RetVal = oha_snmp_create_identical((U_CHAR *)in, (U_CHAR *)out, (unsigned)length,
											(LSA_INT32)errindex, 1);
	return (LSA_INT)RetVal;
}

int oha_snmp_create_identical_no_var_bind(
    U_CHAR * snmp_in    , /* SNMP packet to duplicate           */
    U_CHAR * snmp_out   , /* OUT - buffer to build duplicate in */
    unsigned snmp_length, /* length of snmp packet              */
    LSA_INT32  errstat  ,
    LSA_INT32  errindex)
{
    U_CHAR     type        ;
    U_LONG     dummy       ;
    U_LONG     version     ; /* extract SNMP Ver from original packet */
    unsigned   length      ;
    U_LONG     reqid       ;
    unsigned   tx_length   ;
    unsigned   headerLength;
    U_CHAR   * headerPtr   ;
    U_CHAR   * reqidPtr    ;
    U_CHAR   * errstatPtr  ;
    U_CHAR   * errindexPtr ;
    U_CHAR   * varListPtr  ;
    U_CHAR   * endPtr      ;
    /* copy rec packet to out buffer ( from 30 82 <lenHigh> <LenLow>....) */
    OHA_MEMCPY((char *)snmp_out, (char *)snmp_in, snmp_length);
    length = snmp_length;
    /* -- Process received packet -- */
    /* extract from RX packet community string, its length, and SNMP version. return ptr to 1st byte after community string */
    headerPtr = oha_snmp_auth_parse(snmp_out, &length, oha_sidbuf, &oha_sidlen, (LSA_INT32 *)&version);
    if (headerPtr == LSA_NULL)
        return 0;
    reqidPtr = oha_asn_parse_header(headerPtr, &length, (U_CHAR *)&dummy);  /* GET */
    if (reqidPtr == LSA_NULL)
        return 0;
    headerLength = length;
    errstatPtr = oha_asn_parse_int(reqidPtr   , &length, &type, (LSA_INT32*)&reqid, sizeof(reqid) );   /* request id */
    if (errstatPtr == LSA_NULL)
        return 0;
    errindexPtr = oha_asn_parse_int(errstatPtr, &length, &type, (LSA_INT32*)&dummy, sizeof(dummy) );   /* error status */
    if (errindexPtr == LSA_NULL)
        return 0;
    varListPtr = oha_asn_parse_int(errindexPtr, &length, &type, (LSA_INT32*)&dummy, sizeof(dummy) );   /* error index */
    if (varListPtr == LSA_NULL)
        return 0;
    headerLength = headerLength - length ; /* calc size of ASN1 RedId + ErrStat + ErrIndex */
    /* ------- Start to build reply packet based on received processed packet -------- */
    tx_length = snmp_length ; /* max theoretical pkt size memory space we have ( we don't need so much) */
    /* Header being build will place requested size+authentication size after 0x30 in the start of SNMP packet
       Add 1st 2 to size in order to take into account GET_RESPONSE ASN.1 obj
       Add 2nd 2 to size in order to take into account 2 byte end of SNMP packet  */
    headerPtr = oha_snmp_auth_build( snmp_out, &tx_length, oha_sidbuf, &oha_sidlen, (LSA_INT32 *)&version, headerLength + 2 + 2 );
    if (headerPtr == LSA_NULL)
        return 0;
    /* Place GET_RESPONSE and the size of ASN.1 obj which follow it (RedId + ErrStat + ErrIndex)
       Add 2 to size in order to take into account 2 byte end of SNMP packet  */
    reqidPtr  = oha_asn_build_header(headerPtr, &tx_length, GET_RSP_MSG, headerLength + 2 );
    if (reqidPtr == LSA_NULL)
        return 0;
    type = (U_CHAR)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER);
    errstatPtr = oha_asn_build_int(reqidPtr, &tx_length, type, (LSA_INT32 *)&reqid, sizeof(reqid));
    if (errstatPtr == LSA_NULL)
        return 0;
    errindexPtr  = oha_asn_build_int(errstatPtr, &tx_length, type, &errstat, sizeof(errstat));
    if (errindexPtr == LSA_NULL)
        return 0;
    varListPtr = oha_asn_build_int(errindexPtr, &tx_length, type, &errindex, sizeof(errindex));
    if (varListPtr == LSA_NULL)
        return 0;
    endPtr = oha_asn_build_null(varListPtr, &tx_length , ASN_CONSTRUCTOR | ASN_SEQUENCE);
    if (endPtr == LSA_NULL)
        return 0;
    tx_length = snmp_length - tx_length  ; /* calc real SNMP size (UDP data) */
    oha_packet_end = snmp_out + tx_length; /* calc end memory address of pkt to be send */
    return tx_length;
}

/* FUNCTION: snmp_access()
 *
 * snmp_access() - Check for a legal combination of variable access,
 * community priviledge, and operation. This gets called after
 * community has been looked up via SNMP_COMMUNITY_CHECK(), so we know the
 * community at least exists in our table...
 *
 * PARAM1: U_SHORT    acl
 * PARAM2: unsigned   community
 * PARAM3: unsigned   rw
 *
 * RETURNS:
 * Returns 1 if this access is OK, else returns 0
 */

int
oha_snmp_access(
    U_SHORT acl,   /* variable access, RONLY or RWRITE */
    unsigned    community,  /* community access, RONLY or RWRITE */
    unsigned    rw)         /* operation; GET_OP, NEXT_OP (READ) or SET_OP (WRITE) */
{
    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_access(acl: 0x%X, community: 0x%X, rw: 0x%X)",
                          acl, community, rw);
    /* If we got here, community was in our list, so... */
    if (rw == GET_OP || rw == NEXT_OP)
        return 1;   /* read operations are always always OK */

    /* fall to here if operation is write... */
    if ((acl == OHA_SNMP_RWRITE)||(acl == OHA_SNMP_READCREATE)||(acl == OHA_SNMP_WRITEONLY))
    {
        /* variable is writable */
        if (community == OHA_SNMP_RWRITE)   /* both var * comm are write OK */
            return 1;   /* write OK for this community */
    }

    /* else either var or community is not OK for write */
    OHA_SNMP_MIB_CTR(snmpInBadCommunityUses);
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_access() - either var or community is not OK for write");
    return 0;
}



/* FUNCTION: snmp_get_community()
 *
 * PARAM1: U_CHAR * rcvdcomm - received community name
 *
 * SNMPv1/SNMPv2c uses snmp_get_community() to verify the received
 * community name.
 *
 * If SNMPv3 is used, then SNMPv1/SNMPv2c calls v3_community_check().
 * If SNMPv3 is not used, then SNMPv1/SNMPv2c calls its own snmp_get_community().
 *
 * RETURNS:
 * If the rcvdcomm is valid, return access permission
 * Else return -1.
 */

int  oha_snmp_get_community( /* snmp_get_community(U_CHAR * rcvdcomm)  */
    U_CHAR * rcvdcomm, 
    unsigned sidlen)
{
    int   count;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_get_community(rcvdcomm: 0x%X, sidlen: 0x%X)",
                          rcvdcomm, sidlen);
    for (count = 0; count < NUM_COMMUNITIES; count++)
    {
        /* if (!strcmp(oha_communities[count].name, (char*)rcvdcomm)) */
        if (OHA_MEMCMP(oha_communities[count].name, (char*)rcvdcomm, sidlen))
            break;
    }
    if (count == NUM_COMMUNITIES)
        return(-1);
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_get_community(access: 0x%X)", oha_communities[count].access);
    return(oha_communities[count].access);
}

/*
 * Authentication for SNMP (RFC 1067).  This implements a null
 * authentication layer.
 */

/* FUNCTION: snmp_auth_parse()
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * length
 * PARAM3: U_CHAR * sid
 * PARAM4: unsigned * slen
 * PARAM5: LSA_INT32 * version
 *
 * RETURNS:
 */

U_CHAR * oha_snmp_auth_parse(
    U_CHAR * data,
    unsigned *  length,
    U_CHAR * sid,
    unsigned *  slen,
    LSA_INT32 *   version)
{
    U_CHAR   type = 0;

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_auth_parse(data: 0x%X, length: 0x%X, sid: 0x%X, slen: 0x%X, version: 0x%X)",
                          data, length, sid, slen, version);
    data = oha_asn_parse_header(data, length, &type);
    if (data == LSA_NULL)
    {
        OHA_SNMPERROR("oha_snmp_auth_parse(): bad header");
        return LSA_NULL;
    }
    if (type != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
    {
        OHA_SNMPERROR("oha_snmp_auth_parse(): wrong auth header type");
        return LSA_NULL;
    }
    data = oha_asn_parse_int(data, length, &type, version, sizeof(*version));
    if (data == LSA_NULL)
    {
        OHA_SNMPERROR("oha_snmp_auth_parse(): bad parse of version");
        return LSA_NULL;
    }
    if (type != ASN_INTEGER)
    {
        OHA_SNMPERROR("oha_snmp_auth_parse(): bad data type of version");
        return LSA_NULL;
    }
    data = oha_asn_parse_string(data, length, &type, sid, slen);
    if (data == LSA_NULL)
    {
        OHA_SNMPERROR("oha_snmp_auth_parse(): bad parse of community");
        return LSA_NULL;
    }
    sid[*slen] = '\0';
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_auth_parse(*data: 0x%X)", *data);
    return (U_CHAR *)data;
}



/* FUNCTION: snmp_auth_build()
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * length
 * PARAM3: U_CHAR *   sid
 * PARAM4: unsigned * slen
 * PARAM5: LSA_INT32 *  version
 * PARAM6: unsigned   messagelen
 *
 * RETURNS:
 */

U_CHAR *  oha_snmp_auth_build(
    U_CHAR * data,
    unsigned *  length,
    U_CHAR *    sid,
    unsigned *  slen,
    LSA_INT32 *   version,
    unsigned    messagelen)
{
    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_auth_build(data: 0x%X, length: 0x%X, sid: 0x%X, slen: 0x%X, version: 0x%X)",
                          data, length, sid, slen, version);
    data = oha_asn_build_header(data, length, (U_CHAR)(ASN_SEQUENCE | ASN_CONSTRUCTOR), messagelen + *slen + 5);
    if (data == LSA_NULL)
    {
        OHA_SNMPERROR("oha_snmp_auth_build(): buildheader");
        return LSA_NULL;
    }
    data = oha_asn_build_int(data, length,
                             (U_CHAR)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
                             (LSA_INT32 *)version, sizeof(*version));
    if (data == LSA_NULL)
    {
        OHA_SNMPERROR("oha_snmp_auth_build(): buildint");
        return LSA_NULL;
    }
    data = oha_asn_build_string(data, length,
                                (U_CHAR)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OCTET_STR),
                                sid, *slen);
    if (data == LSA_NULL)
    {
        OHA_SNMPERROR("oha_snmp_auth_build(): buildstring");
        return LSA_NULL;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_auth_build(*data: 0x%X)", *data);
    return (U_CHAR *)data;
}


/* end of file oha_agnt.c */
