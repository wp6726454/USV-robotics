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
/*  F i l e               &F: oha_sock.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA SOCK-LowerLayer interface functions          */
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

#define LTRC_ACT_MODUL_ID  32
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_ASN1 */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "oha_inc.h"            /* OHA headerfiles */
#include "oha_int.h"            /* internal header */

/* BTRACE-IF */
OHA_FILE_SYSTEM_EXTENSION(OHA_MODULE_ID)

#include "oha_asn1.h"
#include "oha_agnt.h"

/* FUNCTION: asn_parse_int()
 *
 * asn_parse_int - pulls a long out of an ASN int type.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the end of this object.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * datalength
 * PARAM3: U_CHAR * type
 * PARAM4: LSA_INT32 * intp
 * PARAM5: unsigned intsize
 *
 * RETURNS: NULL on any error.
 * Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 */

U_CHAR *  oha_asn_parse_int(
    U_CHAR * data,  /* IN - pointer to start of object */
    unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer (data) */
    U_CHAR *    type,       /* OUT - asn type of object */
    LSA_INT32 * intp,       /* IN/OUT - pointer to start of output buffer */
    unsigned    intsize)    /* IN - size of output buffer */
{
    /*
     * ASN.1 integer ::= 0x02 asnlength byte {byte}*
     */
    U_CHAR*  bufp  =  data;
    U_LONG   asn_length = 0;
    LSA_INT32  value =  0; /* accumulator for actual length */

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_parse_int(data: 0x%X, datalength: 0x%X, type: 0x%X, intp: 0x%X, intsize: 0x%X)",
                          data, datalength, type, intp, intsize);

    if ( (!data) || (!datalength) || (!type) || (!intp) )
    {
        return LSA_NULL;
    }

    if (intsize != sizeof(LSA_INT32))
    {
        OHA_SNMPERROR("oha_asn_parse_int(): out buff length <> size of long");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }

    *type = *bufp++;

    /* make sure ASN1 parameter is from integer type (fix SilverCrick 7.0.3.3) */
    if (*type != ASN_INTEGER)
    {
        OHA_SNMPERROR("oha_asn_parse_int(): asn1 parameter isn't integer");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }

    bufp = oha_asn_parse_length(bufp, &asn_length);
    if (bufp == LSA_NULL)
    {
        OHA_SNMPERROR("oha_asn_parse_int(): bad length");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }
    if (asn_length + (bufp - data) > *datalength)
    {
        OHA_SNMPERROR("oha_asn_parse_int(): overflow of message");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }
    if (asn_length > intsize)
    {
        OHA_SNMPERROR("oha_asn_parse_int(): var too small");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }

    *datalength -= (int)asn_length + (bufp - data);

    if (*bufp & 0x80)
        value = -1; /* integer is negative */

    while (asn_length--)
        value = (value << 8) | *bufp++;

    *intp = value;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_parse_int(bufp: 0x%X)", bufp);
    return bufp;
}


/* FUNCTION: asn_build_int()
 *
 * asn_build_int - builds an ASN object containing an integer.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the end of this object.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned *  datalength
 * PARAM3: U_CHAR      type
 * PARAM4: LSA_INT32 *  intp
 * PARAM5: unsigned    intsize
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */

U_CHAR *  oha_asn_build_int(
    U_CHAR * data,     /* IN - pointer to start of output buffer */
    unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
    U_CHAR      type,       /* IN - asn type of object */
    LSA_INT32 *  intp,       /* IN - pointer to start of long integer */
    unsigned    intsize) /* IN - size of *intp */
{
    LSA_INT32 integer;
    U_LONG   mask;

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_build_int(data: 0x%X, datalength: 0x%X, type: 0x%X, intp: 0x%X, intsize: 0x%X)",
                          data, datalength, type, intp, intsize);

    if ( (!data) || (!datalength) || (!intp) )
    {
        return LSA_NULL;
    }
    if ((intsize < 1) || (intsize > sizeof(LSA_INT32)))
    {
        return LSA_NULL;
    }

    integer = *intp;

    if (intsize == sizeof (LSA_INT32))
    {
        /*
         * Truncate "unnecessary" bytes off of the most significant end of this 2's complement integer.
         * There should be no sequence of 9 consecutive 1's or 0's at the most significant end of the
         * integer.
         */
        /*    mask = 0x1FF << ((8 * (sizeof(LSA_INT32) - 1)) - 1); */
        /* Microsoft C v6.0a breaks the line above. It inits mask to 0L. -JB- */
        mask = 0xff800000L;     /* try this - should work on all machines? -JB- */
        /* mask is 0xFF800000 on a big-endian machine */
        while ((((integer & mask) == 0) || ((integer & mask) == mask)) && intsize > 1)
        {
            intsize--;
            integer <<= 8;
        }
    }
    else
    {
        integer <<= (8*(sizeof(LSA_INT32) - intsize));
    }

    data = oha_asn_build_header(data, datalength, type, intsize);
    if (data == LSA_NULL)
        return LSA_NULL;
    if (*datalength < intsize)
        return LSA_NULL;
    *datalength -= intsize;

    mask = 0xFF000000L;     /* Extract high 8 bits of long */

    while (intsize--)
    {
        *data++ = (U_CHAR)((integer & mask) >> (8 * (sizeof(LSA_INT32) - 1)));
        integer <<= 8;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_build_int(data: 0x%X)", data);
    return data;
}


/* FUNCTION: asn_parse_string()
 *
 * asn_parse_string - pulls an octet string out of an ASN octet string type.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the beginning of the next object.
 *
 *  "string" is filled with the octet string.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * datalength
 * PARAM3: U_CHAR *   type
 * PARAM4: U_CHAR *   string
 * PARAM5: unsigned * strlength
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */

U_CHAR *  oha_asn_parse_string(
    U_CHAR * data,  /* IN - pointer to start of object */
    unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
    U_CHAR *    type,       /* OUT - asn type of object */
    U_CHAR *    string,     /* IN/OUT - pointer to start of output buffer */
    unsigned *  strlength)  /* IN/OUT - size of output buffer */
{
    /*
     * ASN.1 octet string ::= primstring | cmpdstring
     * primstring ::= 0x04 asnlength byte {byte}*
     * cmpdstring ::= 0x24 asnlength string {string}*
     * This doesn't yet support the compound string.
     */
    U_CHAR * bufp  =  data;
    U_LONG   asn_length = 0;

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_parse_string(data: 0x%X, datalength: 0x%X, type: 0x%X, string: 0x%X, strlength: 0x%X)",
                          data, datalength, type, string, strlength);

    if ( (!data) || (!datalength) || (!type) || (!string) || (!strlength) )
    {
        return LSA_NULL;
    }

    *type = *bufp++;       /* type should contain the type (0x04 = octet string) */
    /* make sure we parse STRING . Was added to fix SilverCrick test 7.0.6.8 */
    if ( (*type != OHA_SNMP_STRING) && (*type != OHA_SNMP_IPADDRESS) )
    {
        return LSA_NULL;
    }

    bufp = oha_asn_parse_length(bufp, &asn_length);
    if (bufp == LSA_NULL)
        return LSA_NULL;
    if (asn_length + (bufp - data) > *datalength)
    {
        OHA_SNMPERROR("oha_asn_parse_string(): overflow of message");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }
    if (asn_length > *strlength)
    {
        OHA_SNMPERROR("oha_asn_parse_string(): I don't support such long strings");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }
    OHA_MEMCPY((char *)string, (char *)bufp, (int)asn_length);
    *strlength = (int)asn_length;
    *datalength -= (int)asn_length + (bufp - data);
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_parse_string(bufp + asn_length: 0x%X)", bufp + asn_length);
    return bufp + asn_length;
}


/* FUNCTION: asn_build_string()
 *
 * Builds an ASN octet string object containing the input string.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the beginning of the next object.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * datalength
 * PARAM3: U_CHAR   type
 * PARAM4: U_CHAR * string
 * PARAM5: unsigned strlength
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */

U_CHAR *  oha_asn_build_string(
    U_CHAR * data,  /* IN - pointer to start of object */
    unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
    U_CHAR   type,          /* IN - ASN type of string */
    U_CHAR * string,        /* IN - pointer to start of input buffer */
    unsigned strlength)     /* IN - size of input buffer */
{
    /*
     * ASN.1 octet string ::= primstring | cmpdstring
     * primstring ::= 0x04 asnlength byte {byte}*
     * cmpdstring ::= 0x24 asnlength string {string}*
     * This code will never send a compound string.
     */
    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_build_string(data: 0x%X, datalength: 0x%X, type: 0x%X, string: 0x%X, strlength: 0x%X)",
                          data, datalength, type, string, strlength);

    if ( (!data) || (!datalength) || (!string) )
    {
        return LSA_NULL;
    }

    data = oha_asn_build_header(data, datalength, type, strlength);
    if (data == LSA_NULL)
        return LSA_NULL;
    if (*datalength < strlength)
        return LSA_NULL;
    OHA_MEMCPY((char *)data, (char *)string, strlength);
    *datalength -= strlength;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_build_string(data + strlength: 0x%X)", data + strlength);
    return data + strlength;
}


/* FUNCTION: asn_parse_bits()
 *
 * asn_parse_string - pulls a bit pattern string out of an ASN BIT type.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the beginning of the next object.
 *
 *  "string" is filled with the received bit pattern.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * datalength
 * PARAM3: U_CHAR *   type
 * PARAM4: U_CHAR *   string
 * PARAM5: unsigned * strlength
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */

U_CHAR *  oha_asn_parse_bits(
    U_CHAR * data,  /* IN - pointer to start of object */
    unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
    U_CHAR *    type,       /* OUT - asn type of object */
    U_CHAR *    string,     /* IN/OUT - pointer to start of output buffer */
    unsigned *  strlength)  /* IN/OUT - size of output buffer */
{
    /*
     * ASN.1 BIT pattern ::= 0x03 numofbytes unusedbitsinlastoctet {byte}+
     */
    U_CHAR * bufp  =  data;
    U_LONG   asn_length = 0;

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_parse_bits(data: 0x%X, datalength: 0x%X, type: 0x%X, string: 0x%X, strlength: 0x%X)",
                          data, datalength, type, string, strlength);

    if ( (!data) || (!datalength) || (!type) || (!string) || (!strlength) )
    {
        return LSA_NULL;
    }

    *type = *bufp++;
    bufp = oha_asn_parse_length(bufp, &asn_length);
    if (bufp == LSA_NULL)
        return LSA_NULL;
    if (asn_length + (bufp - data) > *datalength)
    {
        OHA_SNMPERROR("oha_asn_parse_bits(): overflow of message");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }
    if ((asn_length-1) > *strlength)
    {
        OHA_SNMPERROR("oha_asn_parse_bits(): I don't support such long strings");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }
    /* bufp points to a byte which indicates "number of unused bits"
     * ignore it and hence start from bufp+1 */
    OHA_MEMCPY((char *)string, (char *)bufp+1, (int)asn_length-1);

    /* *strlength should contain the number of bits = total - unused
     * *bufp gives the unused count
     */
    *strlength = (int)(asn_length-1)*8 - *bufp;

    *datalength -= (int)asn_length + (bufp - data);
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_parse_bits(bufp + asn_length: 0x%X)", bufp + asn_length);
    return bufp + asn_length;
}


/* FUNCTION: asn_build_bits()
 *
 * Builds an ASN octet BITS object containing the input bit pattern.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the beginning of the next object.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * datalength
 * PARAM3: U_CHAR   type
 * PARAM4: U_CHAR * string
 * PARAM5: unsigned strlength
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */

U_CHAR *  oha_asn_build_bits(
    U_CHAR * data,  /* IN - pointer to start of object */
    unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
    U_CHAR   type,          /* IN - ASN type of string */
    U_CHAR * string,        /* IN - pointer to start of input buffer */
    unsigned strlength)     /* IN - size of input buffer in bits */
{
    /*
     * ASN.1 BIT pattern ::= 0x03 numofbytes unusedbitsinlastoctet {byte}+
     */
    unsigned unusedbits = 8 - (strlength & 0x07);
    unsigned octetlen = strlength/8;
    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_build_bits(data: 0x%X, datalength: 0x%X, type: 0x%X, string: 0x%X, strlength: 0x%X)",
                          data, datalength, type, string, strlength);

    if ( (!data) || (!datalength) || (!string) )
    {
        return LSA_NULL;
    }

    if (strlength%8 /*unusedbits*/)  /* ??? is that correct ??? */
        octetlen++; /* there is a partially used octet */

    data = oha_asn_build_header(data, datalength, type, octetlen+1);
    if (data == LSA_NULL)
        return LSA_NULL;
    if (*datalength < (octetlen+1))
        return LSA_NULL;
    *data = (U_CHAR)unusedbits;
    data++; /* point to the next byte */
    OHA_MEMCPY((char *)data, (char *)string, octetlen);
    *datalength -= octetlen+1;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_build_bits(data + octetlen: 0x%X)", data + octetlen);
    return data + octetlen; /* data has already been incremented */
}


/* FUNCTION: asn_parse_header()
 *
 * asn_parse_header - interprets the ID and length of the current object.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * in this object following the id and length.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * datalength
 * PARAM3: U_CHAR *   type
 *
 * RETURNS: Returns a pointer to the first byte of the contents of this object.
 * Returns NULL on any error.
 */

U_CHAR *  oha_asn_parse_header(
    U_CHAR * data,  /* IN - pointer to start of object */
    unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
    U_CHAR *    type)       /* OUT - ASN type of object */
{
    U_CHAR * bufp  =  data;
    unsigned header_len;
    U_LONG   asn_length = 0;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_parse_header(data: 0x%X, datalength: 0x%X, type: 0x%X)",
                          data, datalength, type);

    if ( (!data) || (!datalength) || (!type) )
    {
        return LSA_NULL;
    }

    /* this only works on data types < 30, i.e. no extension octets */
    if (ASN_IS_EXTENSION_ID(*bufp))
    {
        OHA_SNMPERROR("oha_asn_parse_header(): can't process SNMP Header != 0x30");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }
    *type = *bufp;
    bufp = oha_asn_parse_length(bufp + 1, &asn_length);
    if (bufp == LSA_NULL)
        return LSA_NULL;
    header_len = bufp - data;
    if (header_len + asn_length > *datalength)
    {
        OHA_SNMPERROR("oha_asn_parse_header(): asn length too long");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }
    *datalength = (int)asn_length;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_parse_header(bufp: 0x%X)", bufp);
    return bufp;
}

/* FUNCTION: asn_build_header()
 *
 * asn_build_header - builds an ASN header for an object with the ID and
 * length specified.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * in this object following the id and length.
 *
 * This only works on data types < 30, i.e. no extension octets.
 * The maximum length is 0xFFFF;
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * datalength
 * PARAM3: U_CHAR     type
 * PARAM4: unsigned   length
 *
 * RETURNS: Returns a pointer to the first byte of the contents of this object.
 * Returns NULL on any error.
 */

U_CHAR *  oha_asn_build_header(
    U_CHAR * data,     /* IN - pointer to start of object */
    unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
    U_CHAR      type,       /* IN - ASN type of object */
    unsigned    length)     /* IN - length of object */
{
    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_build_header(data: 0x%X, datalength: 0x%X, type: 0x%X, length: 0x%X)",
                          data, datalength, type, length);

    if ( (!data) || (!datalength) )
    {
        return LSA_NULL;
    }

    if (*datalength < 1)
        return LSA_NULL;
    *data++ = type;
    (*datalength)--;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_build_header(*datalength: 0x%X)", *datalength);
    return oha_asn_build_length(data, datalength, length);
}

/* FUNCTION: asn_parse_length()
 *
 * asn_parse_length - interprets the length of the current object.
 * On exit, length contains the value of this length field.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: U_LONG  *length
 *
 * RETURNS: Returns a pointer to the first byte after this length
 * field (aka: the start of the data field).
 * Returns NULL on any error.
 */

U_CHAR *  oha_asn_parse_length(
    U_CHAR * data,     /* IN - pointer to start of length field */
    U_LONG  *   length)  /* OUT - value of length field */
{
    U_CHAR lengthbyte;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_parse_length(data: 0x%X, length: 0x%X)",
                          data, length);

    if ( (!data) || (!length) )
    {
        return LSA_NULL;
    }

    lengthbyte = *data;

    if (lengthbyte & ASN_LONG_LEN)
    {
        lengthbyte &= (U_CHAR)(~ASN_LONG_LEN);     /* turn MSb off */
        if (lengthbyte == 0)
        {
            OHA_SNMPERROR("oha_asn_parse_length(): We don't support indefinite lengths");
            OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
            return LSA_NULL;
        }
        if (lengthbyte > sizeof(LSA_INT32))
        {
            OHA_SNMPERROR("oha_asn_parse_length(): we can't support data lengths that long");
            OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
            return LSA_NULL;
        }
        OHA_MEMCPY((char *)length, (char *)data + 1, (int)lengthbyte);
        *length = OHA_NTOHL/*ntohl*/(*length);
        *length >>= (8 * ((sizeof *length) - lengthbyte));
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_parse_length(data + lengthbyte + 1: 0x%X)", data + lengthbyte + 1);
        return data + lengthbyte + 1;
    }
    else
    {   /* short asnlength */
        *length = (LSA_INT32)lengthbyte;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_parse_length(data + 1: 0x%X)", data + 1);
        return data + 1;
    }
}



/* FUNCTION: asn_build_length()
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned *    datalength
 * PARAM3: unsigned      length
 *
 * RETURNS:
 */

U_CHAR *  oha_asn_build_length(
    U_CHAR * data,  /* IN - pointer to start of object */
    unsigned *     datalength, /* IN/OUT - number of valid bytes left in buffer */
    unsigned       length)     /* IN - length of object */
{
    U_CHAR *    start_data  =  data;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_build_length(data: 0x%X, datalength: 0x%X, length: 0x%X)",
                          data, datalength, length);

    if ( (!data) || (!datalength) )
    {
        return LSA_NULL;
    }

    /* no indefinite lengths sent */
    if (length < 0x80)
    {
        *data++ = (U_CHAR)length;
    }
    else if (length <= 0xFF)
    {
        *data++ = (U_CHAR)(0x01 | ASN_LONG_LEN);
        *data++ = (U_CHAR)length;
    }
    else  /* 0xFF < length <= 0xFFFF */
    {
        *data++ = (U_CHAR)(0x02 | ASN_LONG_LEN);
        *data++ = (U_CHAR)((length >> 8) & 0xFF);
        *data++ = (U_CHAR)(length & 0xFF);
    }

    if (*datalength < (unsigned)(data - start_data))
    {
        OHA_SNMPERROR("oha_asn_build_length(): build_length");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }
    *datalength -= (data - start_data);
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_build_length(data: 0x%X)", data);
    return data;

}

/* FUNCTION: asn_parse_objid()
 *
 * asn_parse_objid - pulls an object indentifier out of an ASN object identifier type.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the beginning of the next object.
 *
 * "objid" is filled with the object identifier.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * datalength
 * PARAM3: U_CHAR *   type
 * PARAM4: OHA_OID *  objid
 * PARAM5: unsigned * objidlength
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */

U_CHAR *  oha_asn_parse_objid(
    U_CHAR * data, /* IN - pointer to start of object */
    unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
    U_CHAR *    type,       /* OUT - ASN type of object */
    OHA_OID *   objid,      /* OUT - pointer to start of output buffer */
    unsigned *  objidlength)   /* IN/OUT - number of sub-id's in objid */
{
    /*
     * ASN.1 objid ::= 0x06 asnlength subidentifier {subidentifier}*
     * subidentifier ::= {leadingbyte}* lastbyte
     * leadingbyte ::= 1 7bitvalue
     * lastbyte ::= 0 7bitvalue
     */
    U_CHAR * bufp  =  data;
    OHA_OID * oidp = objid + 1;
    U_LONG   subidentifier;
    LSA_INT32  length;
    U_LONG   asn_length = 0;

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_parse_objid(data: 0x%X, datalength: 0x%X, type: 0x%X, objid: 0x%X, objidlength: 0x%X)",
                          data, datalength, type, objid, objidlength);

    if ( (!data) || (!datalength) || (!type) || (!objid) || (!objidlength) )
    {
        return LSA_NULL;
    }

    *type = *bufp++;
    bufp = oha_asn_parse_length(bufp, &asn_length);
    if (bufp == LSA_NULL)
        return LSA_NULL;
    if (asn_length + (bufp - data) > *datalength)
    {
        OHA_SNMPERROR("oha_asn_parse_objid(): overflow of message");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }
    *datalength -= (int)asn_length + (bufp - data);

    if (asn_length > *objidlength)
    {
        OHA_SNMPERROR("oha_asn_parse_objid(): rcvd objid too big");
    }

    length = asn_length;    /* number of bytes in encoded image of object Id */
    while (length > 0)
    {
        subidentifier = 0;
        do
        {     /* shift and add in low order 7 bits */
            subidentifier = (subidentifier << 7) + ((U_LONG)(*bufp) & ~ASN_BIT8);
            length--;
        }
        while (*(U_CHAR *)bufp++ & ASN_BIT8);   /* last byte has high bit clear */

        /* check subidentifier value limit */
#if (MAX_SUBID != 0xFFFFFFFF)    /* don't do both if 32bit sub Ids */
        if (subidentifier > MAX_SUBID)
        {
            OHA_SNMPERROR("oha_asn_parse_objid(): subidentifier too big");
            OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
            return LSA_NULL;
        }
#endif

        /* if asn_length is greater than objidlength, then
         * the received OID is too large to fit in objid.
         * Hence ignore the extra subids.
         */
        if ( (int)(oidp - objid) < (int)*objidlength)
            *oidp++ = (OHA_OID)subidentifier;
        else
        {
            /* ignore extra subids */
        }
    }

    /* The first two subidentifiers are encoded into the first
     * component with the value (X 40) + Y, where: X is the value of
     * the first subidentifier. Y is the value of the second
     * subidentifier.
     */
    subidentifier = (U_LONG)objid[1];
    objid[1] = (U_CHAR)(subidentifier % 40);
    objid[0] = (U_CHAR)((subidentifier - objid[1]) / 40);

    *objidlength = (int)(oidp - objid);
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_parse_objid(bufp: 0x%X)", bufp);
    return bufp;
}

static U_CHAR   oha_buf[OHA_MAX_OID_LEN];
static OHA_OID  oha_objbuf[OHA_MAX_OID_LEN];

/* FUNCTION: asn_build_objid()
 *
 * asn_build_objid - Builds an ASN object identifier object containing the input string.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the beginning of the next object.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * datalength
 * PARAM3: U_CHAR     type
 * PARAM4: OHA_OID *  objid
 * PARAM5: unsigned   objidlength
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */

U_CHAR *  oha_asn_build_objid(
    U_CHAR * data,   /* IN - pointer to start of object */
    unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
    U_CHAR      type,       /* IN - ASN type of object */
    OHA_OID *   objid,      /* IN - pointer to start of input buffer */
    unsigned    objidlength)/* IN -  number   of sub-id's in objid */
{
    /*
     * ASN.1 objid ::= 0x06 asnlength subidentifier {subidentifier}*
     * subidentifier ::= {leadingbyte}* lastbyte
     * leadingbyte ::= 1 7bitvalue
     * lastbyte ::= 0 7bitvalue
     */
    U_CHAR * bp =  oha_buf;
    OHA_OID * op = oha_objbuf;
    unsigned asnlength;
    U_LONG   subid,   mask, testmask;
    unsigned bits, testbits;

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_build_objid(data: 0x%X, datalength: 0x%X, type: 0x%X, objid: 0x%X, objidlength: 0x%X)",
                          data, datalength, type, objid, objidlength);

    if ( (!data) || (!datalength) || (!objid) )
    {
        return LSA_NULL;
    }

    OHA_MEMCPY((char *)oha_objbuf, (char *)objid, objidlength * sizeof(OHA_OID));

    /* Special case for null oid 0.0
     * When encoded, it is 06 01 00  (length 1)
     * When parsed, it is 0.0 (length 2)
     * When response is built with received value then asn_build_objid()
     * is called with objidlength as 1. Which would be encoded as 06 00.
     * Hence bump the length in this case.
     */
    if (objidlength == 1)
    {
        if (op[0])
            return LSA_NULL;  /* badly formed oid */
        else  /* op[0] is 0 - hence can be interpreted as 0.0 */
        {
            op[1] = 0;     /* make the next subid as 0 */
            objidlength++; /* increase objidlength to 2 */
        }
    }
    /* transform size in bytes to size in subid's */
    /* encode the first two components into the first subidentifier */
    op[1] = op[1] + (op[0] * 40);
    op++;

    if (objidlength > 0)
    {
        objidlength--;

        while (objidlength > 0)
        {
            subid = *op++;
            mask = 0x7F;   /* handle subid == 0 case */
            bits = 0;
            /* testmask *MUST* !!!! be of an unsigned type */
            for (testmask = 0x7F, testbits = 0; testmask != 0; testmask <<= 7, testbits += 7)
            {
                if (subid & testmask)
                {     /* if any bits set */
                    mask = testmask;
                    bits = testbits;
                }
            }

            /* mask can't be zero here */
            OHA_ASSERT (mask != 0);

            /* take care of special case when subid > 0x0FFFFFFF
             * because for this case "(mask>>bits) != 7F"
             */
            if (subid & HUGEOIDMASK)
            {
                *bp = (U_CHAR)((subid & HUGEOIDMASK) >> bits);
                *bp++ |= ASN_BIT8;
                bits-=7;  /* bits becomes 28-7=21 */
                mask = 0x7F << bits;
            }
            for (; mask != 0x7F; mask >>= 7, bits -= 7)
            {
                /* MSC 8.0 gets warning on this next line: = (U_CHAR)((subid
                 * & mask) >> bits) | ASN_BIT8;
                 */
                *bp = (U_CHAR)((subid & mask) >> bits);
                *bp++ |= ASN_BIT8;
            }
            *bp++ = (U_CHAR)(subid & mask);
            if (objidlength > 0)
            {
                objidlength--;
            }
        }
    }

    asnlength = bp - oha_buf;
    data = oha_asn_build_header(data, datalength, type, asnlength);
    if (data == LSA_NULL)
        return LSA_NULL;
    if (*datalength < asnlength)
        return LSA_NULL;
    OHA_MEMCPY((char *)data, (char *)oha_buf, asnlength);
    *datalength -= asnlength;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_build_objid(data + asnlength: 0x%X)", data + asnlength);
    return data + asnlength;
}


/* FUNCTION: asn_parse_null()
 *
 * asn_parse_null - Interprets an ASN null type.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the beginning of the next object.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * datalength
 * PARAM3: U_CHAR *   type
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (Returns NULL on any error.
 */

U_CHAR *  oha_asn_parse_null(
    U_CHAR * data, /* IN - pointer to start of object */
    unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
    U_CHAR *    type)       /* OUT - ASN type of object */
{
    /*
     * ASN.1 null ::= 0x05 0x00
     */
    U_CHAR * bufp  =  data;
    U_LONG   asn_length = 0;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_parse_null(data: 0x%X, datalength: 0x%X, type: 0x%X)",
                          data, datalength, type);

    if ( (!data) || (!datalength) || (!type) )
    {
        return LSA_NULL;
    }

    *type = *bufp++;
    bufp = oha_asn_parse_length(bufp, &asn_length);
    if (bufp == LSA_NULL)
        return LSA_NULL;
    if (asn_length != 0)
    {
        OHA_SNMPERROR("oha_asn_parse_null(): Malformed NULL");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }
    *datalength -= (bufp - data);
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_parse_null(bufp + asn_length: 0x%X)", bufp + asn_length);
    return bufp + asn_length;
}


/* FUNCTION: asn_build_null()
 *
 * asn_build_null - Builds an ASN null object.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the beginning of the next object.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * datalength
 * PARAM3: U_CHAR     type
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */

U_CHAR *  oha_asn_build_null(
    U_CHAR * data, /* IN - pointer to start of object */
    unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
    U_CHAR      type)       /* IN - ASN type of object */
{
    /*
     * ASN.1 null ::= 0x05 0x00
     */
    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN/OUT : oha_asn_build_null(data: 0x%X, datalength: 0x%X, type: 0x%X)",
                          data, datalength, type);
    return oha_asn_build_header(data, datalength, type, 0);
}

/* FUNCTION: asn_parse_header2()
 *
 * asn_parse_header2 - interprets the ID and length of the current object.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * in this object following the id and length.
 *
 * This function has been derived from asn_parse_header(). In SNMPv3,
 * the MessageType is encoded in the ID field. That is the first byte
 * of the PDU is the MessageType and the 2nd is length of PDU.
 * Hence we don't need the IS_EXTENSION_ID() check of asn_parse_header().
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * datalength
 * PARAM3: U_CHAR *   type
 * PARAM4: unsigned * seqlength
 *
 * RETURNS: Returns a pointer to the first byte of the contents of this object.
 * Returns NULL on any error.
 */

U_CHAR *  oha_asn_parse_header2(
    U_CHAR * data, /* IN - pointer to start of object */
    unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
    U_CHAR *    type,       /* OUT - ASN type of object */
    unsigned *  seqlength)  /* OUT - Length of BER sequence */
{
    U_CHAR * bufp  =  data;
    unsigned header_len;
    U_LONG   asn_length = 0;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_parse_header2(data: 0x%X, datalength: 0x%X, type: 0x%X, seqlength: 0x%X)",
                          data, datalength, type, seqlength);

    if ( (!data) || (!datalength) || (!type) || (!seqlength) )
    {
        return LSA_NULL;
    }

    *type = *bufp;
    bufp = oha_asn_parse_length(bufp + 1, &asn_length);
    if (bufp == LSA_NULL)
        return LSA_NULL;
    header_len = bufp - data;
    if (header_len + asn_length > *datalength)
    {
        OHA_SNMPERROR("oha_asn_parse_header2(): asn length too long");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }
    *datalength -= header_len;
    *seqlength  = (int)asn_length;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_parse_header2(bufp: 0x%X)", bufp);
    return bufp;
}


#ifdef SNMP_COUNTER64

/* FUNCTION: asn_parse_counter64()
 *
 * asn_parse_counter64 - pulls a 64bit value out of an ASN COUNTER64 type.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the end of this object.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * datalength
 * PARAM3: U_CHAR * type
 * PARAM4: union counter64 * cp
 * PARAM5: unsigned cpsize
 *
 * RETURNS: NULL on any error.
 * Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 */

U_CHAR *  oha_asn_parse_counter64(
    U_CHAR * data,  /* IN - pointer to start of object */
    unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer (data) */
    U_CHAR *    type,       /* OUT - asn type of object */
    OHA_COUNTER64 * cp,       /* IN/OUT - pointer to start of output buffer */
    unsigned    cpsize)    /* IN - size of output buffer */
{
    /*
     * ASN.1 counter64 ::= 0x46 asnlength byte {byte}*
     */
    U_CHAR*  bufp  =  data;
    U_LONG   asn_length = 0;

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_parse_counter64(data: 0x%X, datalength: 0x%X, type: 0x%X, cp: 0x%X, cpsize: 0x%X)",
                          data, datalength, type, cp, cpsize);

    if ( (!data) || (!datalength) || (!type) || (!cp) )
    {
        return LSA_NULL;
    }

    if (!cp)
    {
        OHA_SNMPERROR("oha_asn_parse_counter64(): output buffer NULL");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }
    if (cpsize != sizeof(union oha_counter64_tag))
    {
        OHA_SNMPERROR("oha_asn_parse_counter64(): not counter64");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }

    *type = *bufp++;
    bufp = oha_asn_parse_length(bufp, &asn_length);
    if (bufp == LSA_NULL)
    {
        OHA_SNMPERROR("oha_asn_parse_counter64(): bad length");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }
    if (asn_length + (bufp - data) > *datalength)
    {
        OHA_SNMPERROR("oha_asn_parse_counter64(): overflow of message");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }
    if (asn_length > cpsize)
    {
        OHA_SNMPERROR("oha_asn_parse_counter64(): var too small");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return LSA_NULL;
    }

    *datalength -= (int)asn_length + (bufp - data);

    cp->ll.h = cp->ll.l = 0; /* first clear the value in cp */

    /* now write the recieved value into cp */
    OHA_MEMCPY(&(cp->s[8-asn_length]), (char *)bufp, asn_length);
    bufp+=asn_length;

    /* cp->ll.h and cp->ll.l now contain the 64bit value
     * correct their endian-ness */
    cp->ll.l = OHA_HTONL/*htonl*/(cp->ll.l);
    cp->ll.h = OHA_HTONL/*htonl*/(cp->ll.h);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_parse_counter64(bufp: 0x%X)", bufp);
    return bufp;
}


/* FUNCTION: asn_build_counter64()
 *
 * asn_build_counter64 - builds an ASN object containing an integer.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the end of this object.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned *  datalength
 * PARAM3: U_CHAR      type
 * PARAM4: LSA_INT32 *   intp
 * PARAM5: unsigned    intsize
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */

U_CHAR *  oha_asn_build_counter64(
    U_CHAR * data,     /* IN - pointer to start of output buffer */
    unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
    U_CHAR      type,       /* IN - asn type of object */
    OHA_COUNTER64 * cp,    /* IN - pointer to start of long integer */
    unsigned    cpsize)     /* IN - size of *cp  = 8 bytes */
{
    /*
     * ASN.1 counter64 ::= 0x46 asnlength byte {byte}*
     */
    int startindex=0;

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_build_counter64(data: 0x%X, datalength: 0x%X, type: 0x%X, cp: 0x%X, cpsize: 0x%X)",
                          data, datalength, type, cp, cpsize);
    LSA_UNUSED_ARG(cpsize);

    if ( (!data) || (!datalength) || (!cp) )
    {
        return LSA_NULL;
    }

    /* bring values in network endian b4 sending */
    cp->ll.l = OHA_HTONL/*htonl*/(cp->ll.l);
    cp->ll.h = OHA_HTONL/*htonl*/(cp->ll.h);
    while ((cp->s[startindex] == 0) && startindex <7)
        startindex++;

    /* 8-startindex = len of data */

    data = oha_asn_build_header(data, datalength, type, 8-startindex);
    if (data && ((int)*datalength > (8-startindex)))
    {
        OHA_MEMCPY((char *)data, &(cp->s[startindex]), 8-startindex);
        *datalength -= 8-startindex;
        /* restore original values */
        cp->ll.l = OHA_HTONL/*htonl*/(cp->ll.l);
        cp->ll.h = OHA_HTONL/*htonl*/(cp->ll.h);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_build_counter64(data + 8-startindex: 0x%X)", data + 8-startindex);
        return data + 8-startindex;
    }
    /* restore original values */
    cp->ll.l = OHA_HTONL/*htonl*/(cp->ll.l);
    cp->ll.h = OHA_HTONL/*htonl*/(cp->ll.h);
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn_build_counter64() - return NULL");
    return LSA_NULL;
}

#endif /* SNMP_COUNTER64 */

/* end of file oha_asn1.c */


