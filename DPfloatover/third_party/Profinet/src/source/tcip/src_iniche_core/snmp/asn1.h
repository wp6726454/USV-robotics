/*
 * v3.1 August 30, 2011 snmp/asn1.h
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: ASN1.H
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

#ifndef _ASN1_H_
#define  _ASN1_H_ 1

/*
 * Definitions for Abstract Syntax Notation One, ASN.1
 * As defined in ISO/IS 8824 and ISO/IS 8825
 *
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

#define  ASN_BOOLEAN       (0x01)
#define  ASN_INTEGER       (0x02)
#define  ASN_BIT_STR       (0x03)
#define  ASN_OCTET_STR     (0x04)
#define  ASN_NULL          (0x05)
#define  ASN_OBJECT_ID     (0x06)
#define  ASN_SEQUENCE      (0x10)
#define  ASN_SET           (0x11)
#define  ASN_NSO           (0x80)  /* RFC1905:noSuchObject   */
#define  ASN_NSI           (0x81)  /* RFC1905:noSuchInstance */
#define  ASN_EOM           (0x82)  /* RFC1905:endOfMib       */

#define  ASN_UNIVERSAL     (0x00)
#define  ASN_APPLICATION   (0x40)
#define  ASN_CONTEXT       (0x80)
#define  ASN_PRIVATE       (0xC0)

#define  ASN_PRIMITIVE     (0x00)
#define  ASN_CONSTRUCTOR   (0x20)

#define  ASN_LONG_LEN      (0x80)
#define  ASN_EXTENSION_ID  (0x1F)
#define  ASN_BIT8          (0x80)

#define  IS_CONSTRUCTOR(byte) ((byte)  &  ASN_CONSTRUCTOR)
#define  IS_EXTENSION_ID(byte)   (((byte) &  ASN_EXTENSION_ID) == ASN_EXTENSION_ID)

u_char  * asn_parse_int(u_char*, unsigned*, u_char*, long*, unsigned);
u_char * asn_build_int(u_char*, unsigned*, u_char, long*, unsigned);
u_char * asn_parse_string(u_char*, unsigned*, u_char*, u_char*, unsigned*);
u_char * asn_build_string(u_char*, unsigned*, u_char, u_char*, unsigned);
u_char * asn_parse_bits(u_char*, unsigned*, u_char*, u_char*, unsigned*);
u_char * asn_build_bits(u_char*, unsigned*, u_char, u_char*, unsigned);
u_char * asn_parse_header(u_char*, unsigned*, u_char*);
u_char * asn_parse_header2(u_char*, unsigned*, u_char*, unsigned* );
u_char * asn_build_header(u_char*, unsigned*, u_char, unsigned);
u_char * asn_parse_length(u_char*, u_long*);
u_char * asn_build_length(u_char*, unsigned*, unsigned);
u_char * asn_parse_objid(u_char*, unsigned*, u_char*, oid*, unsigned*);
u_char * asn_build_objid(u_char*, unsigned*, u_char, oid*, unsigned);
u_char * asn_parse_objid32(u_char*, unsigned*, u_char*, oid32*, unsigned*);
u_char * asn_build_objid32(u_char*, unsigned*, u_char, oid32*, unsigned);
u_char * asn_parse_null(u_char*, unsigned*, u_char*);
u_char * asn_build_null(u_char*, unsigned*, u_char);
#ifdef SNMP_COUNTER64
u_char * asn_parse_counter64(u_char *, unsigned *, u_char *, Counter64 *, unsigned);
u_char * asn_build_counter64(u_char *, unsigned *, u_char, Counter64 *, unsigned);
#endif

#endif   /* _ASN1_H_ */


