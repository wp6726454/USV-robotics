
#ifndef OHA_ASN1_H                        /* ----- reinclude-protection ----- */
#define OHA_ASN1_H

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
/*  F i l e               &F: oha_asn1.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Definitions for Abstract Syntax Notation One, ASN.1                      */
/*  As defined in ISO/IS 8824 and ISO/IS 8825                                */
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

#define SNMP_COUNTER64

/*
 * Basic system type definitions, taken from the BSD file sys/types.h.
 */

#ifndef U_CHAR
typedef unsigned char   U_CHAR;    /* uint8_t -> LSA_UINT8 */
#endif
#ifndef U_SHORT
typedef unsigned short  U_SHORT;   /* uint16_t -> LSA_UINT16 */
#endif
#ifndef U_INT
typedef unsigned int    U_INT;     /* LSA_UINT */
#endif
#ifndef U_LONG
typedef LSA_UINT32      U_LONG;    /* unsigned long -> LSA_UINT32 */
#endif

/* 1075501: usage of datatype long and implicit approval of its size */

/* define the datatype to hold a 64-bit counter.
 * It can be done in a variety of ways (one 64bit value, 2 32bit values,
 * 8 bytes, etc. Hence use a union to define it. */
#ifndef OHA_COUNTER64
typedef union oha_counter64_tag
{
    struct oha_longlong
    {
        U_LONG h,l;  /* two 32bit values h is higher, l is lower */
    } ll;
    char s[8];             /* s0 is LSB, s7 is MSB */
} OHA_COUNTER64;
#endif

#define HUGEOIDMASK 0xF0000000

#define  ASN_BOOLEAN       (0x01)
#define  ASN_INTEGER       (0x02)
#define  ASN_BIT_STR       (0x03)
#define  ASN_OCTET_STR     (0x04)
#define  ASN_NULL          (0x05)
#define  ASN_OBJECT_ID     (0x06)
#define  ASN_SEQUENCE      (0x10)
#define  ASN_SET           (0x11)
/* #define  ASN_NSO        (0x80) */  /* RFC1905:noSuchObject   */
/* #define  ASN_NSI        (0x81) */  /* RFC1905:noSuchInstance */
/* #define  ASN_EOM        (0x82) */  /* RFC1905:endOfMib       */

#define  ASN_UNIVERSAL     (0x00)
#define  ASN_APPLICATION   (0x40)
#define  ASN_CONTEXT       (0x80)
#define  ASN_PRIVATE       (0xC0)

#define  ASN_PRIMITIVE     (0x00)
#define  ASN_CONSTRUCTOR   (0x20)

#define  ASN_LONG_LEN      (0x80)
#define  ASN_EXTENSION_ID  (0x1F)
#define  ASN_BIT8          (0x80)

#define  ASN_IS_CONSTRUCTOR(byte) ((byte)  &  ASN_CONSTRUCTOR)
#define  ASN_IS_EXTENSION_ID(byte)   (((byte) &  ASN_EXTENSION_ID) == ASN_EXTENSION_ID)

U_CHAR * oha_asn_parse_int(U_CHAR*, unsigned*, U_CHAR*, LSA_INT32*, unsigned);
U_CHAR * oha_asn_build_int(U_CHAR*, unsigned*, U_CHAR, LSA_INT32*, unsigned);
U_CHAR * oha_asn_parse_string(U_CHAR*, unsigned*, U_CHAR*, U_CHAR*, unsigned*);
U_CHAR * oha_asn_build_string(U_CHAR*, unsigned*, U_CHAR, U_CHAR*, unsigned);
U_CHAR * oha_asn_parse_bits(U_CHAR*, unsigned*, U_CHAR*, U_CHAR*, unsigned*);
U_CHAR * oha_asn_build_bits(U_CHAR*, unsigned*, U_CHAR, U_CHAR*, unsigned);
U_CHAR * oha_asn_parse_header(U_CHAR*, unsigned*, U_CHAR*);
U_CHAR * oha_asn_parse_header2(U_CHAR*, unsigned*, U_CHAR*, unsigned* );
U_CHAR * oha_asn_build_header(U_CHAR*, unsigned*, U_CHAR, unsigned);
U_CHAR * oha_asn_parse_length(U_CHAR*, U_LONG*);
U_CHAR * oha_asn_build_length(U_CHAR*, unsigned*, unsigned);
U_CHAR * oha_asn_parse_objid(U_CHAR*, unsigned*, U_CHAR*, OHA_OID*, unsigned*);
U_CHAR * oha_asn_build_objid(U_CHAR*, unsigned*, U_CHAR, OHA_OID*, unsigned);
U_CHAR * oha_asn_parse_null(U_CHAR*, unsigned*, U_CHAR*);
U_CHAR * oha_asn_build_null(U_CHAR*, unsigned*, U_CHAR);
#ifdef SNMP_COUNTER64
U_CHAR * oha_asn_parse_counter64(U_CHAR *, unsigned *, U_CHAR *, OHA_COUNTER64 *, unsigned);
U_CHAR * oha_asn_build_counter64(U_CHAR *, unsigned *, U_CHAR, OHA_COUNTER64 *, unsigned);
#endif


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* end of OHA_ASN1_H */


