/*
 * v3.1 August 30, 2011 snmp/asn1.c
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: asn1.c
 *
 * Portions Copyright 1988, 1989 by Carnegie Mellon University All Rights Reserved.
 * Copyright 1990-96 by NetPort Software. All rights reserved.
 * Copyright 1996-2004 By InterNiche Technologies Inc. All rights reserved.
 *
 * Abstract Syntax Notation One, ASN.1
 * As defined in ISO/IS 8824 and ISO/IS 8825
 * This implements a subset of the above International Standards that
 * is sufficient to implement SNMP.
 *
 * Encodes abstract data types into a machine independent stream of bytes.
 *
 *
 * MODULE: SNMP
 *
 * ROUTINES: asn_parse_int(), asn_build_int(), 
 * ROUTINES: asn_parse_string(), asn_build_string(), asn_parse_header(), 
 * ROUTINES: asn_build_header(), asn_parse_length(), asn_build_length(), 
 * ROUTINES: asn_parse_objid(), asn_build_objid(), 
 * ROUTINES: asn_parse_objid32(), asn_build_objid32(), asn_parse_null(), 
 * ROUTINES: asn_build_null(), asn_parse_header2()
 * ROUTINES: asn_parse_counter64(), asn_build_counter64(), 
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

#include "snmpport.h"

#ifdef INCLUDE_SNMP

#include "asn1.h"
#include "snmp_imp.h"

/* FUNCTION: asn_parse_int()
 * 
 * asn_parse_int - pulls a long out of an ASN int type.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the end of this object.
 *
 * PARAM1: u_char * data
 * PARAM2: unsigned * datalength
 * PARAM3: u_char * type
 * PARAM4: long * intp
 * PARAM5: unsigned intsize
 *
 * RETURNS: NULL on any error.
 * Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 */
u_char * asn_parse_int(u_char   * data,       /* IN     - pointer to start of object */
     unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer (data) */
     u_char *    type,       /* OUT - asn type of object */
     long *      intp,       /* IN/OUT - pointer to start of output buffer */
     unsigned    intsize)    /* IN - size of output buffer */
{
/*
 * ASN.1 integer ::= 0x02 asnlength byte {byte}*
 */
   u_char*     bufp  =  data;
   u_long   asn_length;
   long     value =  0; /* accumulator for actual length */

    if ( (!data) || (!datalength) || (!type) || (!intp) )
        return NULL;

   if (intsize != sizeof(long))
   {
        SNMPERROR("asn_parse_int: out buff length <> size of long");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }

   *type = *bufp++;
   /* make sure ASN1 parameter is from integer type (fix SilverCrick 7.0.3.3) */
    if ((*type != ASN_INTEGER) && (*type != GAUGE))  
    {
        SNMPERROR("asn_parse_int: asn1 parameter isn't integer");
        SNMP_MIB_CTR(snmpInASNParseErrs);
        return NULL;
    }

   bufp = asn_parse_length(bufp, &asn_length);
   if (bufp == NULL)
   {
      SNMPERROR("bad length");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }
   if (asn_length + (bufp - data) > *datalength)
   {
      SNMPERROR("overflow of message");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }
   if (asn_length > intsize)
   {
      SNMPERROR("asn1_parse_int: var too small");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }

   *datalength -= (int)asn_length + (bufp - data);

   if (*bufp & 0x80)
      value = -1; /* integer is negative */

   while (asn_length--)
      value = (value << 8) | *bufp++;

   *intp = value;
   return bufp;
}


/* FUNCTION: asn_build_int()
 * 
 * asn_build_int - builds an ASN object containing an integer.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the end of this object.
 *
 * PARAM1: u_char * data
 * PARAM2: unsigned *  datalength
 * PARAM3: u_char      type
 * PARAM4: long *      intp
 * PARAM5: unsigned    intsize
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */
u_char * asn_build_int(u_char   * data,       /* IN - pointer to start of output buffer */
   unsigned *  datalength,   /* IN/OUT - number of valid bytes left in buffer */
   u_char      type,         /* IN - asn type of object */
   long *      intp,         /* IN - pointer to start of long integer */
   unsigned    intsize)      /* IN - size of *intp */
{
   long  integer;
   u_long   mask;

    if ( (!data) || (!datalength) || (!intp) )
        return NULL;
    
    
   if ((intsize < 1) || (intsize > sizeof(long)))
      return NULL;

   integer = *intp;

   if (intsize == sizeof (long))
   {
      /*
       * Truncate "unnecessary" bytes off of the most significant end of 
       * this 2's complement integer.
       * There should be no sequence of 9 consecutive 1's or 0's at the 
       * most significant end of the integer.
       * 
       * mask = 0x1FF << ((8 * (sizeof(long) - 1)) - 1);
       * Microsoft C v6.0a breaks the above line. It inits mask to 0L. -JB-
       * mask is 0xFF800000 on a big-endian machine
       */
      mask = 0xff800000L;  /* should work on all machines */
      while ((((integer & mask) == 0) || ((integer & mask) == mask)) && intsize > 1)
      {
         intsize--;
         integer <<= 8;
      }
   }
   else
      integer <<= (8*(sizeof(long) - intsize));

   data = asn_build_header(data, datalength, type, intsize);
   if (data == NULL)
      return NULL;
   if (*datalength < intsize)
      return NULL;
   *datalength -= intsize;

   mask = 0xFF000000L;     /* Extract high 8 bits of long */

   while (intsize--)
   {
      *data++ = (u_char)((integer & mask) >> (8 * (sizeof(long) - 1)));
      integer <<= 8;
   }
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
 * PARAM1: u_char * data
 * PARAM2: unsigned * datalength
 * PARAM3: u_char *   type
 * PARAM4: u_char *   string
 * PARAM5: unsigned * strlength
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */
u_char * asn_parse_string(u_char   * data,       /* IN - pointer to start of object */
     unsigned *  datalength,    /* IN/OUT - number of valid bytes left in buffer */
     u_char *    type,          /* OUT - asn type of object */
     u_char *    string,        /* IN/OUT - pointer to start of output buffer */
     unsigned *  strlength)     /* IN/OUT - size of output buffer */
{
/*
 * ASN.1 octet string ::= primstring | cmpdstring
 * primstring ::= 0x04 asnlength byte {byte}*
 * cmpdstring ::= 0x24 asnlength string {string}*
 * This doesn't yet support the compound string.
 */
   u_char * bufp  =  data;
   u_long   asn_length;

   if ((!data) || (!datalength) || (!type) || (!string) || (!strlength))
      return NULL;
    
   *type = *bufp++;    /* type should contain the type (0x04 = octet string) */
                       /* make sure we parse STRING. */
                       /* Was added to fix SilverCrick test 7.0.6.8 */
   if ((*type != ASN_OCTET_STR) && (*type != IPADDRESS))
      return NULL;

   bufp = asn_parse_length(bufp, &asn_length);
   if (bufp == NULL)
      return NULL;

   if (asn_length + (bufp - data) > *datalength)
   {
      SNMPERROR("overflow of message");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }

   if (asn_length > *strlength)
   {
      SNMPERROR("I don't support such long strings");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }

   MEMCPY((char *)string, (char *)bufp, (int)asn_length);
   *strlength = (int)asn_length;
   *datalength -= (int)asn_length + (bufp - data);
   return bufp + asn_length;
}


/* FUNCTION: asn_build_string()
 * 
 * Builds an ASN octet string object containing the input string.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the beginning of the next object.
 *
 * PARAM1: u_char * data
 * PARAM2: unsigned * datalength
 * PARAM3: u_char   type
 * PARAM4: u_char * string
 * PARAM5: unsigned strlength
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */
u_char * asn_build_string(u_char    * data,       /* IN - pointer to start of object */
     unsigned *  datalength,    /* IN/OUT - number of valid bytes left in buffer */
     u_char   type,             /* IN - ASN type of string */
     u_char * string,           /* IN - pointer to start of input buffer */
     unsigned strlength)        /* IN - size of input buffer */
{
/*
 * ASN.1 octet string ::= primstring | cmpdstring
 * primstring ::= 0x04 asnlength byte {byte}*
 * cmpdstring ::= 0x24 asnlength string {string}*
 * This code will never send a compound string.
 */
    if ( (!data) || (!datalength) || (!string) )
        return NULL;
    
    
    
   data = asn_build_header(data, datalength, type, strlength);
   if (data == NULL)
      return NULL;

   if (*datalength < strlength)
      return NULL;

   MEMCPY((char *)data, (char *)string, strlength);
   *datalength -= strlength;
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
 * PARAM1: u_char * data
 * PARAM2: unsigned * datalength
 * PARAM3: u_char *   type
 * PARAM4: u_char *   string
 * PARAM5: unsigned * strlength
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */
u_char * asn_parse_bits(u_char   * data,       /* IN - pointer to start of object */
     unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
     u_char *    type,       /* OUT - asn type of object */
     u_char *    string,     /* IN/OUT - pointer to start of output buffer */
     unsigned *  strlength)  /* IN/OUT - size of output buffer */
{
/*
 * ASN.1 BIT pattern ::= 0x03 numofbytes unusedbitsinlastoctet {byte}+
 */
   u_char * bufp  =  data;
   u_long   asn_length;

    if ( (!data) || (!datalength) || (!type) || (!string) || (!strlength) )
        return NULL;

   *type = *bufp++;
   bufp = asn_parse_length(bufp, &asn_length);
   if (bufp == NULL)
      return NULL;
   if (asn_length + (bufp - data) > *datalength)
   {
      SNMPERROR("overflow of message");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }
   if ((asn_length-1) > *strlength)
   {
      SNMPERROR("I don't support such long strings");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }
   /* bufp points to a byte which indicates "number of unused bits" 
    * ignore it and hence start from bufp+1 */
   MEMCPY((char *)string, (char *)bufp+1, (int)asn_length-1);

   /* *strlength should contain the number of bits = total - unused 
    * *bufp gives the unused count
    */
   *strlength = (int)(asn_length-1)*8 - *bufp;

   *datalength -= (int)asn_length + (bufp - data);
   return bufp + asn_length;
}


/* FUNCTION: asn_build_bits()
 * 
 * Builds an ASN octet BITS object containing the input bit pattern.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the beginning of the next object.
 *
 * PARAM1: u_char * data
 * PARAM2: unsigned * datalength
 * PARAM3: u_char   type
 * PARAM4: u_char * string
 * PARAM5: unsigned strlength
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */
u_char * asn_build_bits(u_char   * data,        /* IN - pointer to start of object */
     unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
     u_char   type,          /* IN - ASN type of string */
     u_char * string,        /* IN - pointer to start of input buffer */
     unsigned strlength)     /* IN - size of input buffer in bits */
{
/*
 * ASN.1 BIT pattern ::= 0x03 numofbytes unusedbitsinlastoctet {byte}+
 */
   unsigned unusedbits = 8 - (strlength & 0x07);
   unsigned octetlen = strlength/8;
    
    if ( (!data) || (!datalength) || (!string) )
        return NULL;
    
   if (unusedbits)
      octetlen++; /* there is a partially used octet */

   data = asn_build_header(data, datalength, type, octetlen+1);
   if (data == NULL)
      return NULL;
   if (*datalength < (octetlen+1))
      return NULL;
   *data = (u_char)unusedbits;
   data++; /* point to the next byte */
   MEMCPY((char *)data, (char *)string, octetlen);
   *datalength -= octetlen+1;
   return data + octetlen; /* data has already been incremented */
}


/* FUNCTION: asn_parse_header()
 * 
 * asn_parse_header - interprets the ID and length of the current object.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * in this object following the id and length.
 *
 * PARAM1: u_char * data
 * PARAM2: unsigned * datalength
 * PARAM3: u_char *   type
 *
 * RETURNS: Returns a pointer to the first byte of the contents of this object.
 * Returns NULL on any error.
 */
u_char * asn_parse_header(u_char   * data,       /* IN - pointer to start of object */
     unsigned *  datalength,   /* IN/OUT - number of valid bytes left in buffer */
     u_char *    type)         /* OUT - ASN type of object */
{
   u_char * bufp  =  data;
   unsigned header_len;
   u_long   asn_length;

    if ( (!data) || (!datalength) || (!type) )
        return NULL;


   /* this only works on data types < 30, i.e. no extension octets */
   if (IS_EXTENSION_ID(*bufp))
   {
        SNMPERROR("can't process SNMP Header != 0x30");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }
   *type = *bufp;

   bufp = asn_parse_length(bufp + 1, &asn_length);
   if (bufp == NULL)
      return NULL;

   header_len = bufp - data;
   if (header_len + asn_length > *datalength)
   {
      SNMPERROR("asn length too long");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }
   *datalength = (int)asn_length;
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
 * PARAM1: u_char * data
 * PARAM2: unsigned * datalength
 * PARAM3: u_char     type
 * PARAM4: unsigned   length
 *
 * RETURNS: Returns a pointer to the first byte of the contents of this object.
 * Returns NULL on any error.
 */
u_char * asn_build_header(u_char   * data,       /* IN - pointer to start of object */
     unsigned *  datalength,   /* IN/OUT - number of valid bytes left in buffer */
     u_char      type,         /* IN - ASN type of object */
     unsigned    length)       /* IN - length of object */
{
    if ( (!data) || (!datalength) )
        return NULL;
    
   if (*datalength < 1)
      return NULL;
    
   *data++ = type;
   (*datalength)--;
   return asn_build_length(data, datalength, length);
}

/* FUNCTION: asn_parse_length()
 * 
 * asn_parse_length - interprets the length of the current object.
 * On exit, length contains the value of this length field.
 *
 * PARAM1: u_char * data
 * PARAM2: u_long  *length
 *
 * RETURNS: Returns a pointer to the first byte after this length
 * field (aka: the start of the data field).
 * Returns NULL on any error.
 */
u_char * asn_parse_length(u_char * data,    /* IN - pointer to start of length field */
     u_long  *   length)        /* OUT - value of length field */
{
    u_char lengthbyte;
    
    if ( (!data) || (!length) )
        return NULL;
    
    lengthbyte = *data;

   if (lengthbyte & ASN_LONG_LEN)
   {
      lengthbyte &= ~ASN_LONG_LEN;     /* turn MSb off */
      if (lengthbyte == 0)
      {
         SNMPERROR("We don't support indefinite lengths");
         SNMP_MIB_CTR(snmpInASNParseErrs);
         return NULL;
      }
      if (lengthbyte > sizeof(long))
      {
         SNMPERROR("we can't support data lengths that long");
         SNMP_MIB_CTR(snmpInASNParseErrs);
         return NULL;
      }
      MEMCPY((char *)length, (char *)data + 1, (int)lengthbyte);
      *length = ntohl(*length);
      *length >>= (8 * ((sizeof *length) - lengthbyte));
      return data + lengthbyte + 1;
   } 
   else 
   {   
      /* short asnlength */
      *length = (long)lengthbyte;
      return data + 1;
   }
}



/* FUNCTION: asn_build_length()
 *
 * Insert the OID length into the output buffer
 * 
 * PARAM1: u_char *           output buffer pointer
 * PARAM2: unsigned *         pointer to "bytes left in buffer" variable
 * PARAM3: unsigned           length of OID object
 *
 * RETURNS: u_char *          updated output buffer pointer
 *                            or NULL if there is a buffer overflow
 *
 * If there is room in the output buffer, the 1, 2, or 3-byte OID length
 * field is appended to output buffer and the buffer pointer is incremented
 * by the size of the length field.
 */

u_char *
asn_build_length(u_char *data, unsigned *datalength, unsigned length)
{
#if 0 /* INICHE_TCIP_ITGR removed unused ! */
   u_char *start_data = data;
#endif

   if ( (!data) || (!datalength) )
        return NULL;

   /* no indefinite lengths sent */
   /* make sure there is sufficient space for the length field */
   if ((length < 0x80) && (*datalength >= 1))
   {
      *data++ = (u_char)length;
      *datalength -= 1;
   }
   else if ((length <= 0xFF) && (*datalength >= 2))
   {
      *data++ = (u_char)(0x01 | ASN_LONG_LEN);
      *data++ = (u_char)length;
      *datalength -= 2;
   }
   else if (*datalength >= 3) /* 0xFF < length <= 0xFFFF */
   {
      *data++ = (u_char)(0x02 | ASN_LONG_LEN);
      *data++ = (u_char)((length >> 8) & 0xFF);
      *data++ = (u_char)(length & 0xFF);
      *datalength -= 3;
   }
   else
   {
      SNMPERROR("build_length");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }

   return (data);       /* return updated data pointer */
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
 * PARAM1: u_char * data
 * PARAM2: unsigned * datalength
 * PARAM3: u_char *   type
 * PARAM4: oid *      objid
 * PARAM5: unsigned * objidlength
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */
u_char * asn_parse_objid(u_char   * data,         /* IN - pointer to start of object */
     unsigned *  datalength,   /* IN/OUT - number of valid bytes left in buffer */
     u_char *    type,         /* OUT - ASN type of object */
     oid *       objid,        /* OUT - pointer to start of output buffer */
     unsigned *  objidlength)  /* IN/OUT - number of sub-id's in objid */
{
/*
 * ASN.1 objid ::= 0x06 asnlength subidentifier {subidentifier}*
 * subidentifier ::= {leadingbyte}* lastbyte
 * leadingbyte ::= 1 7bitvalue
 * lastbyte ::= 0 7bitvalue
 */
   u_char * bufp  =  data;
   oid *    oidp = objid + 1;
   u_long   subidentifier;
   long     length;
   u_long   asn_length;

    if ( (!data) || (!datalength) || (!type) || (!objid) || (!objidlength) )
        return NULL;

   *type = *bufp++;
   bufp = asn_parse_length(bufp, &asn_length);
   if (bufp == NULL)
      return NULL;

   if (asn_length + (bufp - data) > *datalength)
   {
      SNMPERROR("overflow of message");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }
   *datalength -= (int)asn_length + (bufp - data);

   if (asn_length > *objidlength)
   {
      SNMPERROR("rcvd objid too big");
   }

   length = asn_length;    /* number of bytes in encoded image of object Id */
   while (length > 0)
   {
      subidentifier = 0;
      do 
      {     /* shift and add in low order 7 bits */
         subidentifier = (subidentifier << 7) + ((u_long)(*bufp) & ~ASN_BIT8);
         length--;
      } while (*(u_char *)bufp++ & ASN_BIT8);   /* last byte has high bit clear */

         /* check subidentifier value limit */
#if (MAX_SUBID != 0xFFFFFFFF)    /* don't do both if 32bit sub Ids */
      if (subidentifier > MAX_SUBID)
      {
         SNMPERROR("subidentifier too big");
         SNMP_MIB_CTR(snmpInASNParseErrs);
         return NULL;
      }
#endif

      /* if asn_length is greater than objidlength, then
       * the received OID is too large to fit in objid.
       * Hence ignore the extra subids.
       */
      if ( (int)(oidp - objid) < (int)*objidlength)
         *oidp++ = (oid)subidentifier;
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
   subidentifier = (u_long)objid[1];
   objid[1] = (u_char)(subidentifier % 40);
   objid[0] = (u_char)((subidentifier - objid[1]) / 40);

   *objidlength = (int)(oidp - objid);
   return bufp;
}

static u_char   buf[MAX_OID_LEN];
static oid      objbuf[MAX_OID_LEN];

/* FUNCTION: asn_build_objid()
 * 
 * asn_build_objid - Builds an ASN object identifier object containing the input string.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the beginning of the next object.
 *
 * PARAM1: u_char * data
 * PARAM2: unsigned * datalength
 * PARAM3: u_char     type
 * PARAM4: oid *      objid
 * PARAM5: unsigned   objidlength
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */

#define HUGEOIDMASK 0xF0000000

u_char * asn_build_objid(u_char   * data,         /* IN - pointer to start of object */
     unsigned *  datalength,   /* IN/OUT - number of valid bytes left in buffer */
     u_char      type,         /* IN - ASN type of object */
     oid *       objid,        /* IN - pointer to start of input buffer */
     unsigned    objidlength)  /* IN -  number   of sub-id's in objid */
{
/*
 * ASN.1 objid ::= 0x06 asnlength subidentifier {subidentifier}*
 * subidentifier ::= {leadingbyte}* lastbyte
 * leadingbyte ::= 1 7bitvalue
 * lastbyte ::= 0 7bitvalue
 */
   u_char * bp =  buf;
   oid *    op = objbuf;
   unsigned asnlength;
   u_long   subid,   mask, testmask;
   unsigned bits, testbits;

    if ( (!data) || (!datalength) || (!objid) )
        return NULL;

   MEMCPY((char *)objbuf, (char *)objid, objidlength * sizeof(oid));

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
         return NULL;  /* badly formed oid */
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
   if(objidlength > 0)
   {
   objidlength--;
      while(objidlength > 0)
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
#ifdef NPDEBUG
      /* mask can't be zero here */
           if (mask == 0)
           {
               dtrap();
           }
#endif
      /* take care of special case when subid > 0x0FFFFFFF   
       * because for this case "(mask>>bits) != 7F"
       */
      if (subid & HUGEOIDMASK)
      {
         *bp = (u_char)((subid & HUGEOIDMASK) >> bits);
         *bp++ |= ASN_BIT8;
         bits-=7;  /* bits becomes 28-7=21 */
         mask = 0x7F << bits;
      }
      for (;mask != 0x7F; mask >>= 7, bits -= 7)
      {
         /* MSC 8.0 gets warning on this next line: = (u_char)((subid 
          * & mask) >> bits) | ASN_BIT8; 
          */
         *bp = (u_char)((subid & mask) >> bits);
         *bp++ |= ASN_BIT8;
      }
      *bp++ = (u_char)(subid & mask);
         if(objidlength > 0)
         {
            objidlength--;
         }
      }
   }
   asnlength = bp - buf;
   data = asn_build_header(data, datalength, type, asnlength);
   if (data == NULL)
      return NULL;
   if (*datalength < asnlength)
      return NULL;
   MEMCPY((char *)data, (char *)buf, asnlength);
   *datalength -= asnlength;
   return data + asnlength;
}

/* FUNCTION: asn_parse_objid32()
 * 
 * asn_parse_objid32 - pulls an object indentifier out of an ASN 
 * object identifier type.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the beginning of the next object.
 *
 * "objid" is filled with the object identifier.
 *
 * PARAM1: u_char * data
 * PARAM2: unsigned * datalength
 * PARAM3: u_char *   type
 * PARAM4: oid32 *      objid
 * PARAM5: unsigned * objidlength
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */

#ifdef SNMP_32BIT_OID

u_char * asn_parse_objid32(u_char   * data,        /* IN - pointer to start of object */
     unsigned *  datalength,     /* IN/OUT - number of valid bytes left in buffer */
     u_char *    type,           /* OUT - ASN type of object */
     oid32 *       objid,        /* OUT - pointer to start of output buffer */
     unsigned *  objidlength)    /* OUT - number of sub-id's in objid */
{
/*
 * ASN.1 objid ::= 0x06 asnlength subidentifier {subidentifier}*
 * subidentifier ::= {leadingbyte}* lastbyte
 * leadingbyte ::= 1 7bitvalue
 * lastbyte ::= 0 7bitvalue
 */
   u_char * bufp  =  data;
   oid32 *  oidp = objid + 1;
   u_long   subidentifier;
   long     length;
   u_long   asn_length;

    if ( (!data) || (!datalength) || (!type) || (!objid) || (!objidlength) )
        return NULL;

   *type = *bufp++;
   bufp = asn_parse_length(bufp, &asn_length);
   if (bufp == NULL)
      return NULL;
   if (asn_length + (bufp - data) > *datalength)
   {
      SNMPERROR("overflow of message");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }
   *datalength -= (int)asn_length + (bufp - data);

   if (asn_length > *objidlength)
   {
      SNMPERROR("rcvd objid too big");
   }

   length = asn_length;    /* number of bytes in encoded image of object Id */
   while (length > 0)
   {
      subidentifier = 0;
      do 
      {  
         /* shift and add in low order 7 bits */
         subidentifier = (subidentifier << 7) 
            + ((u_long)(*bufp) & ~ASN_BIT8);
         length--;
      } while (*(u_char *)bufp++ & ASN_BIT8);   /* last byte has high bit clear */

         /* check subidentifier value limit */
#if (MAX_SUBID != 0xFFFFFFFF)    /* don't both if 32bit sub Ids */
      if (subidentifier > MAX_SUBID)
      {
         SNMPERROR("subidentifier too big");
         SNMP_MIB_CTR(snmpInASNParseErrs);
         return NULL;
      }
#endif

      /* if asn_length is greater than objidlength, then
       * the received OID is too large to fit in objid.
       * Hence ignore the extra subids.
       */
      if ( (int)(oidp - objid) < (int)*objidlength)
         *oidp++ = (oid)subidentifier;
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
   subidentifier = (u_long)objid[1];
   objid[1] = (oid32)(subidentifier % 40);
   objid[0] = (oid32)((subidentifier - objid[1]) / 40);

   *objidlength = (int)(oidp - objid);
   return bufp;
}

#endif /* SNMP_32BIT_OID */

static oid32     objbuf32[MAX_OID_LEN];

/* FUNCTION: asn_build_objid32()
 * 
 * asn_build_objid32 - Builds an ASN object identifier object 
 * containing the input string.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the beginning of the next object.
 *
 * PARAM1: u_char * data
 * PARAM2: unsigned * datalength
 * PARAM3: u_char     type
 * PARAM4: oid32 *      objid
 * PARAM5: unsigned   objidlength
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */

#ifdef SNMP_32BIT_OID

u_char * asn_build_objid32(u_char   * data,        /* IN - pointer to start of object */
     unsigned *  datalength,     /* IN/OUT - number of valid bytes left in buffer */
     u_char      type,           /* IN - ASN type of object */
     oid32 *       objid,        /* IN - pointer to start of input buffer */
     unsigned    objidlength)    /* IN -  number   of sub-id's in objid */
{
/*
 * ASN.1 objid ::= 0x06 asnlength subidentifier {subidentifier}*
 * subidentifier ::= {leadingbyte}* lastbyte
 * leadingbyte ::= 1 7bitvalue
 * lastbyte ::= 0 7bitvalue
 */
   u_char * bp =  buf;
   oid32 *    op = objbuf32;
   unsigned asnlength;
   u_long   subid,   mask, testmask;
   unsigned bits, testbits;

    if ( (!data) || (!datalength) || (!objid) )
        return NULL;
    
    
   MEMCPY((char *)objbuf32, (char *)objid, objidlength * sizeof(oid32));

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
         return NULL;  /* badly formed oid */
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
   objidlength--;

   while (objidlength-- > 0)
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

#ifdef NPDEBUG
      /* mask can't be zero here */
        if (mask == 0)
        {
            dtrap();
        }
#endif

      for (;mask != 0x7F; mask >>= 7, bits -= 7)
      {
         /* MSC 8.0 gets warning on this next line: = (u_char)((subid 
          * & mask) >> bits) | ASN_BIT8; 
          */
         *bp = (u_char)((subid & mask) >> bits);
         *bp++ |= ASN_BIT8;
      }
      *bp++ = (u_char)(subid & mask);
   }

   asnlength = bp - buf;
   data = asn_build_header(data, datalength, type, asnlength);
   if (data == NULL)
      return NULL;
   if (*datalength < asnlength)
      return NULL;
   MEMCPY((char *)data, (char *)buf, asnlength);
   *datalength -= asnlength;
   return data + asnlength;
}

#endif /* SNMP_32BIT_OID */

/* FUNCTION: asn_parse_null()
 * 
 * asn_parse_null - Interprets an ASN null type.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the beginning of the next object.
 *
 * PARAM1: u_char * data
 * PARAM2: unsigned * datalength
 * PARAM3: u_char *   type
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (Returns NULL on any error.
 */
u_char * asn_parse_null(u_char   * data,       /* IN - pointer to start of object */
     unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
     u_char *    type)       /* OUT - ASN type of object */
{
/*
 * ASN.1 null ::= 0x05 0x00
 */
   u_char * bufp  =  data;
   u_long   asn_length;

    if ( (!data) || (!datalength) || (!type) )
        return NULL;

   *type = *bufp++;
   bufp = asn_parse_length(bufp, &asn_length);
   if (bufp == NULL)
      return NULL;
   if (asn_length != 0)
   {
      SNMPERROR("Malformed NULL");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }
   *datalength -= (bufp - data);
   return bufp + asn_length;
}


/* FUNCTION: asn_build_null()
 * 
 * asn_build_null - Builds an ASN null object.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the beginning of the next object.
 *
 * PARAM1: u_char * data
 * PARAM2: unsigned * datalength
 * PARAM3: u_char     type
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */
u_char * asn_build_null(u_char   * data,       /* IN - pointer to start of object */
     unsigned *  datalength, /* IN/OUT - number of valid bytes left in buffer */
     u_char      type)       /* IN - ASN type of object */
{
/*
 * ASN.1 null ::= 0x05 0x00
 */
   return asn_build_header(data, datalength, type, 0);
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
 * PARAM1: u_char * data
 * PARAM2: unsigned * datalength
 * PARAM3: u_char *   type
 * PARAM4: unsigned * seqlength
 *
 * RETURNS: Returns a pointer to the first byte of the contents of this object.
 * Returns NULL on any error.
 */
u_char * asn_parse_header2(u_char   * data,       /* IN - pointer to start of object */
     unsigned *  datalength,     /* IN/OUT - number of valid bytes left in buffer */
     u_char *    type,           /* OUT - ASN type of object */
     unsigned *  seqlength)      /* OUT - Length of BER sequence */     
{
   u_char * bufp  =  data;
   unsigned header_len;
   u_long   asn_length;

    if ( (!data) || (!datalength) || (!type) || (!seqlength) )
        return NULL;

   *type = *bufp;
   bufp = asn_parse_length(bufp + 1, &asn_length);
   if (bufp == NULL)
      return NULL;
   header_len = bufp - data;
   if (header_len + asn_length > *datalength)
   {
      SNMPERROR("asn length too long");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }
   *datalength -= header_len;
   *seqlength  = (int)asn_length;
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
 * PARAM1: u_char * data
 * PARAM2: unsigned * datalength
 * PARAM3: u_char * type
 * PARAM4: union counter64 * cp
 * PARAM5: unsigned cpsize
 *
 * RETURNS: NULL on any error.
 * Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 */
u_char * asn_parse_counter64(u_char    * data,       /* IN - pointer to start of object */
     unsigned *  datalength,       /* IN/OUT - number of valid bytes left in buffer (data) */
     u_char *    type,             /* OUT - asn type of object */
     Counter64 * cp,               /* IN/OUT - pointer to start of output buffer */
     unsigned    cpsize)           /* IN - size of output buffer */
{
/*
 * ASN.1 counter64 ::= 0x46 asnlength byte {byte}*
 */
   u_char*     bufp  =  data;
   u_long   asn_length;

    if ( (!data) || (!datalength) || (!type) || (!cp) )
        return NULL;

   if (!cp)
   {
      SNMPERROR("output buffer NULL");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }
   if (cpsize != sizeof(union counter64))
   {
      SNMPERROR("not counter64");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }

   *type = *bufp++;  
   bufp = asn_parse_length(bufp, &asn_length);
   if (bufp == NULL)
   {
      SNMPERROR("bad length");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }
   if (asn_length + (bufp - data) > *datalength)
   {
      SNMPERROR("overflow of message");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }
   if (asn_length > cpsize)
   {
      SNMPERROR("asn1_parse_counter64: var too small");
      SNMP_MIB_CTR(snmpInASNParseErrs);
      return NULL;
   }

   *datalength -= (int)asn_length + (bufp - data);

   cp->ll.h = cp->ll.l = 0; /* first clear the value in cp */

   /* now write the recieved value into cp */
   MEMCPY(&(cp->s[8-asn_length]), (char *)bufp, asn_length);
   bufp+=asn_length;

   /* cp->ll.h and cp->ll.l now contain the 64bit value 
    * correct their endian-ness */   
   cp->ll.l = htonl(cp->ll.l);
   cp->ll.h = htonl(cp->ll.h);

   return bufp;
}


/* FUNCTION: asn_build_counter64()
 * 
 * asn_build_counter64 - builds an ASN object containing an integer.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the end of this object.
 *
 * PARAM1: u_char * data
 * PARAM2: unsigned *  datalength
 * PARAM3: u_char      type
 * PARAM4: long *      intp
 * PARAM5: unsigned    intsize
 *
 * RETURNS: Returns a pointer to the first byte past the end
 * of this object (i.e. the start of the next object).
 * Returns NULL on any error.
 */
u_char * asn_build_counter64(u_char    * data,       /* IN - pointer to start of output buffer */
   unsigned *  datalength,         /* IN/OUT - number of valid bytes left in buffer */
   u_char      type,               /* IN - asn type of object */
   Counter64 * cp,                 /* IN - pointer to start of long integer */
   unsigned    cpsize)             /* IN - size of *cp  = 8 bytes */
{
/*
 * ASN.1 counter64 ::= 0x46 asnlength byte {byte}*
 */
   int startindex = 0;
   Counter64 c64;
   if ( (!data) || (!datalength) || (!cp) )
      return NULL;
   /* bring values in network endian b4 sending */
   c64.ll.l = htonl(cp->ll.l);
   c64.ll.h = htonl(cp->ll.h);
/*   while ((cp->s[startindex] == 0) && startindex <7)
      startindex++;*/
   /* 8-startindex = len of data */
   data = asn_build_header(data, datalength, type, 8-startindex);
   if (data && ((int)*datalength > (8-startindex)))
   {
      MEMCPY((char *)data, &(c64.s[startindex]), 8-startindex);
      *datalength -= 8-startindex;
      return data + 8-startindex;
   }
   USE_ARG(cpsize);
   return NULL;
}

#endif /* SNMP_COUNTER64 */


/* end of file asn1.c */


#endif /*  INCLUDE_SNMP */
