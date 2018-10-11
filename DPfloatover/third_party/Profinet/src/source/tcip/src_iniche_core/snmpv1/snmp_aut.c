/*
 * v3.1 August 30, 2011 snmpv1/snmp_aut.c
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: snmp_aut.c
 *
 * Portions Copyright 1988, 1989 by Carnegie Mellon University All Rights Reserved.
 * Copyright 1990-96 by NetPort Software. All rights reserved.
 * Copyright 1996-2004 By InterNiche Technologies Inc. All rights reserved.
 *
 *
 * MODULE: SNMPV1
 *
 * ROUTINES: snmp_auth_parse(), snmp_auth_build()
 *
 * PORTABLE: yes
 */


/*
 * snmp_auth.c - Authentication for SNMP (RFC 1067).  This implements a null
 * authentication layer.
 *
 */

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
#include "snmpport.h"

#if (defined(INCLUDE_SNMPV1) || defined(INCLUDE_SNMPV2C))

#include "asn1.h"
#include "npsnmp.h"

#include "snmp_imp.h"


/* FUNCTION: snmp_auth_parse()
 * 
 * PARAM1: u_char * data
 * PARAM2: unsigned * length
 * PARAM3: u_char * sid
 * PARAM4: unsigned * slen
 * PARAM5: long *    version
 *
 * RETURNS: 
 */

u_char * 
snmp_auth_parse(u_char * data, 
     unsigned *  length,
     u_char * sid,
     unsigned *  slen,
     long *      version)
{
   u_char   type;

   data = asn_parse_header(data, length, &type);
   if (data == NULL)
   {
      SNMPERROR("bad header");
      return NULL;
   }
   if (type != (ASN_SEQUENCE | ASN_CONSTRUCTOR))
   {
      SNMPERROR("wrong auth header type");
      return NULL;
   }
   data = asn_parse_int(data, length, &type, version, sizeof(*version));
   if (data == NULL)
   {
      SNMPERROR("bad parse of version");
      return NULL;
   }
   if (type != ASN_INTEGER)
   {
      SNMPERROR("bad data type of version");
      return NULL;
   }
   data = asn_parse_string(data, length, &type, sid, slen);
   if (data == NULL)
   {
      SNMPERROR("bad parse of community");
      return NULL;
   }
   sid[*slen] = '\0';
   return (u_char *)data;
}



/* FUNCTION: snmp_auth_build()
 * 
 * PARAM1: u_char * data
 * PARAM2: unsigned * length
 * PARAM3: u_char *   sid
 * PARAM4: unsigned * slen
 * PARAM5: long *     version
 * PARAM6: unsigned   messagelen
 *
 * RETURNS: 
 */

u_char * 
snmp_auth_build(u_char * data, 
     unsigned *  length,
     u_char *    sid,
     unsigned *  slen,
     long *      version,
     unsigned    messagelen)
{
   data = asn_build_header(data, length, (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), messagelen + *slen + 5);
   if (data == NULL)
   {
      SNMPERROR("buildheader");
      return NULL;
   }
   data = asn_build_int(data, length,
    (u_char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_INTEGER),
    (long *)version, sizeof(*version));
   if (data == NULL)
   {
      SNMPERROR("buildint");
      return NULL;
   }
   data = asn_build_string(data, length,
    (u_char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OCTET_STR), 
    sid, *slen);
   if (data == NULL)
   {
      SNMPERROR("buildstring");
      return NULL;
   }
   return (u_char *)data;
}

#endif /* INCLUDE_SNMPV1 or INCLUDE_SNMPV2C */

/* end of file snmp_aut.c */


