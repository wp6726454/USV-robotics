/*
 * v3.1 August 30, 2011 snmp/snmp.c
 * Distributed per ref EMEA2011-3
 */
/*
 * FILENAME: snmp.c
 *
 * Copyright 1990-96 by NetPort Software. All rights reserved.
 * Copyright 1996-2004 By InterNiche Technologies Inc. All rights reserved.
 *
 *
 * MODULE: SNMP
 *
 * ROUTINES: snmp_parse_var_op(), snmp_build_var_op(), 
 * ROUTINES: getStatPtr(), compare(), snmp_sizeof_int(), 
 * ROUTINES: snmp_build_adjust_len(), validateValue(),
 * ROUTINES: is_us_ascii(), goodValue(), setVariable(), shift_array(), 
 * ROUTINES: oid2bytes(), setTestAndIncr(), snmp_update_rowstatus(), 
 *
 * PORTABLE: yes
 */

/* Additional Copyrights: */

/*
 * Simple Network Management Protocol (RFC 1067).
 *
Copyright 1988, 1989 by Carnegie Mellon University All Rights Reserved
Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  
CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/

#include "snmpport.h"

#ifdef INCLUDE_SNMP

#include "asn1.h"
#include "npsnmp.h"
#include "snmp_imp.h"

#include "snmp_var.h"


/* set parms is used by  SNMP agents to SET value of MIB objects */
struct SetParms   set_parms;


/* FUNCTION: snmp_parse_var_op()
 * 
 * snmp_parse_var_op() - parse next variable from a sequence of 
 * received variables. This extracts the object ID and type of 
 * variable for the caller. A pointer to the start of the 
 *
 * PARAM1: u_char * data
 * PARAM2: oid *       var_name
 * PARAM3: unsigned *    var_name_len
 * PARAM4: u_char * var_val_type
 * PARAM5: unsigned *    var_val_len
 * PARAM6: u_char ** var_val
 * PARAM7: unsigned * listlength
 *
 * RETURNS: 
 */

u_char * 
snmp_parse_var_op(u_char * data, /* IN - pointer to the start of object */
     oid *       var_name,   /* OUT - object id of variable */
     unsigned *     var_name_len,  /* IN/OUT - size of var_name, in oids */
     u_char *    var_val_type,     /* OUT - type of variable (int or octet string) (one byte) */
     unsigned *     var_val_len,   /* OUT - length of variable */
     u_char **      var_val,       /* OUT - pointer to ASN1 encoded value of variable */
     unsigned *     listlength)    /* IN/OUT - number of valid bytes left in var_op_list */
{
   u_char   var_op_type;
   unsigned var_op_len = *listlength;
   u_char * var_op_start   =  data;

   /* pull off leading SEQUENCE header */
   data = asn_parse_header(data, &var_op_len, &var_op_type);
   if (data == NULL)
      return NULL;

   if (var_op_type != (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR))
      return NULL;
   /* pull off field with variable's object Id: */
   data = asn_parse_objid(data, &var_op_len, &var_op_type, var_name, var_name_len);
   if (data == NULL)
      return NULL;

   if (var_op_type != (u_char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OBJECT_ID))
      return NULL;
   *var_val = data;     /* save pointer to this object */

   /* find out what type of object this is */
   data = asn_parse_header(data, &var_op_len, var_val_type);
   if (data == NULL)
      return NULL;

   *var_val_len = var_op_len;
   data += var_op_len;
   *listlength -= (int)(data - var_op_start);
   return data;   /* return */
}



/* FUNCTION: snmp_build_var_op()
 * 
 * PARAM1: u_char * data
 * PARAM2: oid *    var_name
 * PARAM3: unsigned var_name_len
 * PARAM4: u_char   var_val_type
 * PARAM5: unsigned var_val_len
 * PARAM6: u_char * var_val
 * PARAM7: unsigned * listlength
 *
 * RETURNS: 
 */

u_char * 
snmp_build_var_op(u_char * data,    /* IN - pointer to the beginning of the output buffer */
   oid *    var_name,      /* IN - object id of variable */
   unsigned var_name_len,  /* IN - length of object id */
   u_char   var_val_type,  /* IN - type of variable */
   unsigned var_val_len,   /* IN - length of variable */
   u_char * var_val,       /* IN - value of variable */
   unsigned *  listlength) /* IN/OUT - number of valid bytes left in output buffer */
{
   unsigned    dummyLen, headerLen;
   u_char  *   dataPtr;
   unsigned    adjustment;    /* number of bytes to move date when re-alaigning */

   dummyLen = *listlength;
   dataPtr = data;
   /* initially build header for maximum size variable ( >255 ) */
   data = asn_build_header(data, &dummyLen, 
    (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), 256);
   if (data == NULL)
   {
      SNMPERROR("asn build var");
      SNMP_MIB_CTR(snmpOutTooBigs);
      return NULL;
   }
   headerLen = data - dataPtr;
   *listlength -= headerLen;
   data = asn_build_objid(data, listlength,
    (u_char)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OBJECT_ID),
    var_name, var_name_len);
   if (data == NULL)
   {
      SNMPERROR("asn build obj");
      SNMP_MIB_CTR(snmpOutTooBigs);
      return NULL;
   }
   switch (var_val_type)
   {
   case ASN_INTEGER:
   case GAUGE: /* valid for GAUGE32 and UNSIGNED32 too */
   case COUNTER:
   case TIMETICKS:
      data = asn_build_int(data, listlength, var_val_type,
       (long *)var_val, var_val_len);
      break;
   case ASN_BIT_STR:
      data = asn_build_bits(data, listlength, var_val_type,
       var_val, var_val_len);
      break;
   case ASN_OCTET_STR:
   case IPADDRESS:
   case OPAQUE:
      data = asn_build_string(data, listlength, var_val_type,
       var_val, var_val_len);
      break;
   case ASN_OBJECT_ID:
      data = asn_build_objid(data, listlength, var_val_type,
       (oid *)var_val, var_val_len / sizeof(oid));
      break;
   case ASN_NULL:
   case ASN_NSO:
   case ASN_NSI:
   case ASN_EOM:
      data = asn_build_null(data, listlength, var_val_type);
         break;

#ifdef SNMP_COUNTER64
   case COUNTER64:
   case INTEGER64:
   case UNSIGNED64:
      data = asn_build_counter64(data, listlength, var_val_type,
       (Counter64 *)var_val, var_val_len);
      break;
#endif

      default:
         SNMPERROR("bad type");
         SNMP_MIB_CTR(snmpOutTooBigs);
         return NULL;
   }

   if (data == NULL)
   {
      SNMPERROR("data");
      SNMP_MIB_CTR(snmpOutTooBigs);
      return NULL;
   }

   /* now rebuild the header with the actual size */
   dummyLen = (data - dataPtr) - headerLen;     /* size to put in header */
   if (dummyLen > 255)  /* how many bytes to move data in buffer? */
      adjustment = 0;   /* since we assumed >255, no adjustment needed */
   else if(dummyLen > 127) /* header will shrink 1 byte ? */
      adjustment = 1;   /* byte data left 1 byte */
   else  /* else length encodes in 1 byte, shrink header by 2 bytes */
      adjustment = 2;
   if (adjustment)   /* left shift data right behind header area */
      MEMMOVE(dataPtr, dataPtr+adjustment, dummyLen+ headerLen - adjustment);
   *listlength += adjustment;    /* fixed return value for bytes left in buf */
   data -= adjustment;  /* fix return pointer to next free space */

   /* rebuild header with correct length */
   if (asn_build_header(dataPtr, &dummyLen, 
       (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR), dummyLen) == NULL)
   {
      SNMPERROR("bad hdr");
      SNMP_MIB_CTR(snmpOutTooBigs);
      return NULL;
   }
   return data;
}


struct variable *get_variables_arr(void);

/* snmp_smartmatches - number of times we could start search from where we left */
int snmp_smartmatches=0;
/* snmp_srchsaved - total number of searches saved due to smart search */
unsigned long snmp_srchsaved = 0; 


/* FUNCTION: getStatPtr()
 * 
 * Get pointer to MIB element entry
 *
 * PARAM1: oid *              OID of MIB variable to lookup
 * PARAM2: unsigned *         length of OID string
 * PARAM3: u_char *           set to matched variable's type
 * PARAM4: unsigned * len     set to matched variable's OID length
 * PARAM5: u_short *  acl     set to matched variable's access control list
 * PARAM6: int                <> 0 if exact match required`
 * PARAM7: int                SNMP version code
 *
 * RETURNS: u_char *          ptr to matched variable entry or NULL
 *
 * Each variable name has been placed in the variable table generated 
 * by the MIB compiler, without the terminating substring that 
 * determines the instance of the variable. When a string is found 
 * that is lexicographicly preceded by the input string, the function 
 * for that entry is called to find the method of access of the 
 * instance of the named variable. If that variable is not found, 
 * NULL is returned, and the search through the table continues (it 
 * should stop at the next entry). If it is found, the function 
 * returns a character pointer and a length or a function pointer. 
 * The former is the address of the operand, the latter is an access 
 * routine for the variable. If an exact match for the variable name 
 * exists, it is returned. If not, (and exact is zero), the next 
 * variable lexicographically after the requested one is returned. 
 * Set the "p_ranges" member of global struct set_params. That way 
 * the SET routines can do validation of input values. If no 
 * appropriate variable can be found, NULL is returned. 
 *
 * Check the version field. If the version is SNMPv1, then
 * restrict the use of 64-bit datatypes
 * - return NULL on a GET for 64-bit datatype
 * - skip 64-bit objects when checking for GETNEXT
 *
 * getStatPtr() now does a smart searching. It remembers the
 * last entry and compares that with the incoming "name".
 * That way, the searches for GetBulk can be made faster.
 */

u_char *   
getStatPtr(oid *name, unsigned *namelen, u_char *type, unsigned *len,
           u_short *acl, int exact, long *err_code, int version)
{
   struct variable *vp;
   int    x;
   u_char *access=NULL;
   int    result=0;
   int    comp;
   int    num_variables = get_num_variables();
   /* INICHE_TCIP_ITGR -- AP00564949 removed this static definition */
   /*static */ int lastindex = 0; /* points to index of last OID returned */

   /* do a quick match to see whether the lastindex can be used
    * as a starting point */
   vp = get_variables_arr();
   vp += lastindex;
   comp = compare(name, *namelen, vp->name, (int)(vp->namelen-1));
   if (comp >= 0)
   {
      /* variables[lastindex] is less than (or equal to) name. 
       * Hence lastindex can be used as a starting point for the search */
      snmp_smartmatches++;
      snmp_srchsaved += lastindex;
   }
   else
   {
      vp -= lastindex; /* that is, vp = variables[0] */
      lastindex=0;
   }

   /* search variables table for match */
   for (x = lastindex ; x < num_variables; vp++, x++)
   {
      /* newer MIB compilers skip the .0 when doing multiple-varbind
       * GET request. Hence doing a smarter compare when 
       * namelen < vp->namelen. 
       */
      if ( *namelen >= vp->namelen )
         result = compare(name, (int)vp->namelen-1,vp->name,(int)vp->namelen-1);
      else
      {
         /* if it is a GET/SET operation and the SNMP Mgr has skipped ".0"
          * (for a simple, non-table object), then treat it as a valid match */
         if (exact && (*namelen == (unsigned)(vp->namelen-1)) && !(vp->name[vp->namelen-1])) 
            result = compare(name, (*namelen), vp->name, (*namelen));
         else
            result = compare(name, (*namelen), vp->name, (int)vp->namelen);
      }

      if ((result < 0) || (exact && (result == 0)) ||
          ((exact == 0) && (*namelen >= vp->namelen) && (result == 0) && 
             (vp->name[vp->namelen-1] > name[vp->namelen-1])))
      {
         /* 64-bit datatypes not supported in SNMPv1 */
         if ((version == SNMP_VERSION_1) && 
            ((vp->type == COUNTER64) || (vp->type == INTEGER64) || (vp->type == UNSIGNED64)))
         {
            if (exact) /* GET/SET */
               return NULL;
            else /* GETNEXT */
               continue;
         }
         access = (*(vp->findVar))(vp, name, (int *)namelen, exact, (int *)len);
         if (access != NULL)
            break;
         if ( exact )
            break;
      }
   }

   if ((exact ==0) && (x == num_variables))
   {
      /* return ASN_EOM only for GETNEXT or GETBULK */
      *err_code = ASN_EOM;    /* endOfMib */
      return NULL;
   }

   if (access == NULL)
   {
      if (result != 0) /* the initial prefix didn't match */
         *err_code = ASN_NSO; /* noSuchObject */
      else
         *err_code = ASN_NSI; /* noSuchInstance */
   }

   /* vp now points to the approprate struct */
   *type = vp->type;
   *acl = vp->acl;

#ifdef MIB_VALIDATION
   set_parms.p_ranges=&(vp->ranges);
#endif 

   lastindex = x;   /* save it - potential starting point of next search */

   return access;
}



/* FUNCTION: compare()
 * 
 * PARAM1: oid *  name1
 * PARAM2: unsigned   len1
 * PARAM3: oid *      name2
 * PARAM4: unsigned   len2
 *
 * RETURNS: 
 */

int
compare(oid *  name1, 
     unsigned    len1,
     oid *       name2,
     unsigned    len2)
{
   unsigned len;

   /* len = minimum of len1 and len2 */
   if (len1 < len2)
      len = len1;
   else
      len = len2;
   /* find first non-matching byte */
   while (len-- > 0)
   {
      if (*name1 < *name2)
         return -1;
      if (*name2++ < *name1++)
         return 1;
   }
   /* bytes match up to length of shorter string */
   if (len1 < len2)
      return -1;  /* name1 shorter, so it is "less" */
   if (len2 < len1)
      return 1;

   return 0;   /* both strings are equal */
}



/* FUNCTION: snmp_sizeof_int()
 * 
 * Return the number of bytes needed to store "value". "value" represents
 * a length. It can be length of PDU, header, pkt , etc. 
 *
 * PARAM1: long value
 *
 * RETURNS: 
 */

int 
snmp_sizeof_int(long value)
{
   if ( value < 0x80 )   
      return 2;
   else if ( value <= 0xFF )
      return 3;
   else  /* if ( value <= 0xFFFF ) */
      return 4;
}

/* FUNCTION: snmp_build_adjust_len()
 * 
 * Encode the header for the data. Finally, in the pkt,
 * we would need HEADER (length of data) followed by DATA.
 * When we start building data, we don't know its length.
 * Hence the following sequence happens
 * 1. Build header with dummy length.
 * 2. Build the data and find its length
 * 3. call snmp_build_adjust_len() to put the right length
 * in the header field.
 * The gotchha is that the header field's size can vary
 * depending on the length to encoded. Here is how it goes
 * 80   > Length > 0  :  30 <len>
 * ff   > Length > 80 :  30 81 <len>
 * ffff > length > ff :  30 82 <len> <len>
 * Hence, 
 * 1. Check the len of encoding of orig (dummy_len) header. 
 * 2. Then check the len of encoding for new header (correct len).
 * 3. If they are different, then move the DATA, so that
 * there is proper space for length encoding.
 * 4. Encode the new length.
 *
 * PARAM1: u_char *start_hdr
 * PARAM2: u_char *start_data
 * PARAM3: unsigned len_data
 * PARAM4: int *diff_hdr
 * PARAM5: unsigned *len_final
 *
 * RETURNS: 
 */

int
snmp_build_adjust_len(u_char * start_hdr, u_char * start_data, unsigned len_data, 
   int * diff_hdr,   unsigned *  len_final)
{
   int   len_pkt_encoding  =  start_data  -  start_hdr   ;
   u_char   hdr_type       = *   start_hdr   ;           /* Retain the hdr_type */
   unsigned dummy_pktlen;        /* as a parameter to asn_build_header */
   int   diff;

   diff = len_pkt_encoding - snmp_sizeof_int(len_data);

   if ( diff == 0 )
   {
      /* Length field encodings required the same number of bytes. 
       * Hence MEMMOVE won't be required 
       */
   }
   else 
   {
      /* if (diff > 0 ), then To encode the correct length, lesser 
       * bytes are needed. if (diff < 0 ), then To encode the correct 
       * length, more bytes are needed. In both cases, the following 
       * MEMMOVE should do the shifting. 
       */
      MEMMOVE(start_data-diff, start_data, len_data );
      start_data -= diff ;       /* Adjust start_data accordingly */
   }

   dummy_pktlen = len_data ;

   start_hdr = asn_build_header(start_hdr, &dummy_pktlen, hdr_type, len_data);
   if ( start_hdr == NULL )
      return FALSE;

   if ( start_hdr != start_data )
      return FALSE ;

   if ( diff_hdr != NULL )       /* Return the diff in encodings */
      *diff_hdr = diff ;

   if ( len_final != NULL )      /* Return the final len starting from header */
      *len_final = len_data + snmp_sizeof_int(len_data);

   return TRUE;
}



#ifdef MIB_VALIDATION
/* FUNCTION : validateValue()
 *
 * Validate a new value with specifications from MIB 
 *
 * PARAM1 : value - value to be validated
 * PARAM2 : p_ranges - structure having validation information
 *
 * Returns : TRUE if the value is good, else FALSE
 */


int
validateValue(long value, SIZE_INFO p_ranges)
{
   unsigned i;

   /* Check if the value is consistent with MIB specs */
   if ( p_ranges->rtype == MCT_VALUES )
   {
      /* The new value has to be one of those specified in MIB */
      for (i=0 ; i < p_ranges->count ; i++)
         if ( value == p_ranges->values[i] )
         break;
      if (i == p_ranges->count )
         return FALSE;
   }
   else  /* p_ranges.rtype == MCT_RANGES */
   {
      /* The new value has to be in one of the  ranges */
      for (i=0 ; i < p_ranges->count ; i++)
      if ((value >= p_ranges->values[i*2]) &&
          (value <= p_ranges->values[i*2+1]))
      {
         break;
      }
      if (i == p_ranges->count )
         return FALSE;
   }
   return TRUE;
}
#endif   /* MIB_VALIDATION */


/* FUNCTION: is_us_ascii()
 * Verify whether all characters in the input string are US ASCII or not.
 * 
 * The return code is logic is such that reading the code makes sense.
 *
 * PARAM1: char * str    - string to be validated
 * PARAM2: int   len     - len of string 
 *
 * RETURNS: 
 * If any character is not US ASCII, then return 0
 * Else return 1
 */


int
is_us_ascii(char * str,int len)
{
   int match;
   int i; 
   for (i=0; i < len; i++)
   {
      match=0;
      switch (str[i])
      {
         case 0:    /* NUL - null            */
         case 7:    /* BEL - bell            */
         case 8:    /* BS  - back space      */
         case 9:    /* HT  - horizontal tab  */
         case 10:   /* LF  - line feed       */
         case 11:   /* VT  - vertical tab    */
         case 12:   /* FF  - form feed       */
            match=1;
            break;
         case 13:   /* CR  - carriage return */
            /* next char must be NUL OR LF */
            if ((str[i+1] == 0) || (str[i+1] == 10))
               match=1;
            break;
         default:  
            if ((str[i] >= 32) && (str[i] <= 126))
               match=1;
            break;
      }
      if (!match)
         return 0;  /* input char is non US ASCII */
   }

   return 1;
}

/* FUNCTION: goodValue()
 * 
 * PARAM1: u_char *value
 * PARAM2: u_char inType
 * PARAM3: int      inLen
 * PARAM4: u_char   actualType
 * PARAM5: int      actualLen
 *
 * RETURNS: TRUE if the value is good. Otherwise it returns an error
 * code which informs about the bad value. Error codes being
 * V3_VB_WRONGTYPE, V3_VB_WRONGLENGTH, V3_VB_WRONGVALUE. 
 */

int
goodValue(u_char * value, 
   u_char   inType,
   int      inLen,
   u_char   actualType,
   int      actualLen)
{
   int   type;
   long  int_value;
   unsigned datalen=6;
   u_char   val_type;

   /* convert intype to internal representation */
   type = inType == ASN_INTEGER ? INTEGER : inType == ASN_OCTET_STR ? STRING : inType == ASN_OBJECT_ID ? OBJID : NULLOBJ;
   if ( !((inType == actualType) || (type == actualType)) )
   {
      return V3_VB_WRONGTYPE;
   }

   switch (actualType)
   {
   case INTEGER:
   case COUNTER:
   case IPADDRESS:
   case GAUGE: /* valid for GAUGE32 and UNSIGNED32 too */
   case TIMETICKS:
      if (actualLen != 4)  /* check that length is for 32 bit number */
      {
         SNMPERROR("goodValue: INT size not 4 bytes");
#if 0 /*INICHE_TCIP_ITGR (AP00392322): not according to MIB2*/
         SNMP_MIB_CTR(snmpInBadValues);
#endif
         dtrap();
         return V3_VB_WRONGLENGTH;
      }
      /* get the value from varbind */
      asn_parse_int(value, &datalen, &val_type, &int_value,sizeof(long) );
      if (actualType == IPADDRESS)  /* No range checking required for IP Address */
         break;

      /* Validate with the user specified range of values */
      if (set_parms.do_range) 
      {
         if (int_value > set_parms.hi_range)
            return V3_VB_WRONGVALUE;
         if (int_value < set_parms.lo_range)
            return V3_VB_WRONGVALUE;
      }
      /* Validation of popular data types (e.g. TimeInterval,
       * StorageType, SecurityLevel) is supported via SF_* flags.
       * In the var_*() function, the appropriate SF_* flag is set.
       * Accordingly, the received value is validated here.
       * For example, for a StorageType object SnmpTargetAddrStorageType, 
       * the SF_STORAGETYPE flag is set in var_snmpTargetAddrEntry().
       * Hence the recieved value would be validated here.
       */
      if (set_parms.vbflags)
      {
         /* Some user defined datatypes like TimeInterval
          * accept only positive values. If the SF_UNSIGNEDVALUE flag is
          * set, then verify that the value is positive.
          */
         if (set_parms.vbflags & SF_UNSIGNEDVALUE)
            if (int_value < 0)
               return V3_VB_WRONGVALUE;
         if (set_parms.vbflags & SF_STORAGETYPE)
            if ((int_value < 1) || (int_value > 5))  /* 1..5 */
               return V3_VB_WRONGVALUE;
         if (set_parms.vbflags & SF_SECURITYLEVEL)
            if ((int_value < 1) || (int_value > 3))  /* 1..3 */
               return V3_VB_WRONGVALUE;
      }
#ifdef MIB_VALIDATION
      if ((!set_parms.access_method) && (set_parms.p_ranges) && 
          set_parms.p_ranges->count)
      {
         if ( validateValue(int_value,set_parms.p_ranges) == TRUE )
            break;
         else
            return V3_VB_WRONGVALUE;
      }
#endif   /* MIB_VALIDATION */
      break;
   case ASN_BIT_STR:
      break; /* assume all BIT patterns are good values */
   case STRING:
   case OBJID:
   case OPAQUE:
      if (set_parms.do_range) /* check user's range */
      {
         if (inLen > set_parms.hi_range)
            return V3_VB_WRONGLENGTH;
         if (inLen < set_parms.lo_range)
            return V3_VB_WRONGLENGTH;
      }
      else  /* Ranges not specified in stub function. Use those from MIB */
      {
         if (set_parms.access_method)
         {
            /* access_method is set. Hence validation will be
             * done in access_method.
             * Thus user can provide access_method to override
             * default range checking.
             */
         }
         else
         {
#ifdef MIB_VALIDATION
            if (set_parms.p_ranges && set_parms.p_ranges->count)
            {
               if ( validateValue(inLen,set_parms.p_ranges) == TRUE )
                  break;
               else
                  return V3_VB_WRONGLENGTH;
            }
            else
#endif   /* MIB_VALIDATION */
            {
               /* Range not specified in MIB. */
               if (inLen > actualLen)
                  return V3_VB_WRONGLENGTH;
            }
         }
      }

      /* for a string, validate all chars to be part of US ASCII */
      if ((actualType == STRING) && (!(set_parms.vbflags & SF_NOASCIICHK)))
      {
#if 1 /* INICHE_TCIP_ITGR */
         char buf[6+SYS_STRING_MAX];
         int bufsize=6+SYS_STRING_MAX;
         datalen=6+SYS_STRING_MAX;
#else
         char buf[SYS_STRING_MAX];
         int bufsize = sizeof(buf);
         datalen = bufsize;
#endif
         /* read the input string into buf */
         if (!asn_parse_string(value, &datalen, &val_type, 
            (u_char *)buf, (unsigned *)&bufsize))
         {
            return V3_VB_WRONGVALUE;  /* input string might be too big */
         }

         /* validate all chars */
         if (!is_us_ascii(buf,bufsize))
            return V3_VB_WRONGVALUE;  /* input char is non US ASCII */
      }
      break;
      case COUNTER64:
      case INTEGER64:
      case UNSIGNED64:
         /* we don't have to check for the validity of Counter64 value 
          * because as per spec it can only be ReadOnly or 
          *"accessible-for-notify". Hence this value would never be modified
          * and hence this function would never be called for it */

      default:
         dtrap(); 
         return V3_VB_WRONGTYPE; /* bad type, should this be assert later? */
   }

   return TRUE;
}


/* FUNCTION: setVariable()
 * 
 * PARAM1: u_char * var_val
 * PARAM2: u_char   var_val_type
 * PARAM3: u_char * statP
 * PARAM4: int      statLen
 *
 * RETURNS: 0 on success, else error code
 */

int
setVariable(u_char * var_val, /* ASN.1 encoded value of variable to set */
   u_char   var_val_type,     /* ASN.1 type of var_val */
   u_char * statP,            /* pointer returned by var_ routine */
   int      statLen)          /* var_len set by var_ routine */
{
   /* set an abritrarily large buffer parm for asn1 parsing. 
    * We don't use the returned value for this anyway.
    */
   unsigned   asn_buf_size = 1000;     /*  */
   unsigned   set_var_size = statLen;  /* bytes in field to set */
   long tmpvalue=0;

   switch (var_val_type)
   {
   case ASN_INTEGER:
   case COUNTER:
   case GAUGE: /* valid for GAUGE32 and UNSIGNED32 too */
   case TIMETICKS:
      if (!asn_parse_int(var_val, &asn_buf_size, &var_val_type, 
         &tmpvalue, set_var_size))
      {
         return V3_VB_BADVALUE;
      }
      if ((set_parms.vbflags & SF_STORAGETYPE) &&
         ((long)(*statP) != tmpvalue))
      {
         if ((long)(*statP) == SNMP_ST_PERMANENT)
            return V3_VB_WRONGVALUE;  /* can't change this value */
         if ((long)(*statP) == SNMP_ST_READONLY)
            return V3_VB_WRONGVALUE;  /* can't change this value */
      }
      if (set_parms.vbflags & SF_SECURITYLEVEL)
         *(long *)(statP) = SNMP_SL2F(tmpvalue); /* map rcvd SecurityLevel to flags */
      else
         *(long *)(statP) = tmpvalue; /* assign the read value */

      break;
   case ASN_BIT_STR:
      if (!asn_parse_bits(var_val, &asn_buf_size, &var_val_type, 
         statP, &set_var_size))
      {
         return V3_VB_BADVALUE;
      }
      break;
   case ASN_OCTET_STR:
   case IPADDRESS:
   case OPAQUE:
      /* allow buffer sizes up limit set in var_ routine */
      if (set_parms.do_range) /* if var_ routine set limits */
         set_var_size = (unsigned)set_parms.hi_range;
#ifdef MIB_VALIDATION
      else if (set_parms.p_ranges && (set_parms.p_ranges->count >0))
      {
         if ( set_parms.p_ranges->rtype == MCT_VALUES )
         {
            /* Set the highest value. Assuming last value is the highest */
            set_var_size = (unsigned)
            set_parms.p_ranges->values[set_parms.p_ranges->count-1];
         }
         else  /* set_parms.p_ranges.rtype == MCT_RANGES */
         {
            /* Set the highest value. Assuming last pair is the highest */
            set_var_size = (unsigned)
            set_parms.p_ranges->values[(set_parms.p_ranges->count-1)*2+1];
         }
      }
#endif   /* MIB_VALIDATION */
      if (!asn_parse_string(var_val, &asn_buf_size, &var_val_type, 
         statP, &set_var_size))
      {
         return V3_VB_BADVALUE;
      }
      /* null terminate the strings */
      if ( var_val_type == ASN_OCTET_STR )
         *(statP + set_var_size) = '\0';
      break;
   case ASN_OBJECT_ID:
      if (!asn_parse_objid(var_val, &asn_buf_size, &var_val_type, 
         (oid*)statP, &set_var_size))
      {
         return V3_VB_BADVALUE;
      }
      break;

#ifdef SNMP_COUNTER64
   case COUNTER64:
   case INTEGER64:
   case UNSIGNED64:
      asn_parse_counter64(var_val, &asn_buf_size, &var_val_type, 
       (Counter64  *)statP, set_var_size);
      break;
#endif

   }

   return 0;
}



/* FUNCTION: shift_array()
 * 
 * PARAM1: u_char * begin
 * PARAM2: int      length
 * PARAM3: int      shift_amount
 *
 * RETURNS: 
 */

void
shift_array(u_char * begin, 
   int      length,
   int      shift_amount)
{
   u_char * oldp, *  newp;

   if (shift_amount >= 0)
   {
      oldp = begin + length - 1;
      newp = oldp + shift_amount;

      while (length--)
         *newp-- = *oldp--;
   } else 
   {
      oldp = begin;
      newp = begin + shift_amount;

      while (length--)
         *newp++ = *oldp++;
   }
}

/* move oid values into a byte array. Assumes all values are < 256! */


/* FUNCTION: oid2bytes()
 * 
 * PARAM1: char * bytebuf
 * PARAM2: oid * oidbuf
 * PARAM3: int count
 *
 * RETURNS: 
 */

void
oid2bytes(char * bytebuf, oid * oidbuf, int count)
{
   int   i;
#if 1 /* INICHE_TCIP_ITGR AP01532164 */
	for (i = 0; i < count; i++)
	{
		*bytebuf++ = (u_char)*oidbuf++;
	}
#else
   for (i = 0; i < count; i++)
      *bytebuf++ = *(u_char*)oidbuf++;
#endif
}


/* FUNCTION: setTestAndIncr()
 * Generic function to set the value of a TestAndIncr object.
 * If a TestAndIncr object needs to be set, then in the var_*() function,
 * set the access_method to setTestAndIncr.
 *
 * Test if the received value is equal to current value.
 * If not, return V3_VB_INCONSISTENTVALUE.
 * If it is equal, then increment the current value and return 0.
 * 
 * PARAM1: u_char *var_val      - rcvd value (asn encoded)
 * PARAM2: u_char  var_val_type - type of received value
 * PARAM3: int     var_val_len  - len of received value
 * PARAM4: u_char *statP        - return value of var_*()
 * PARAM5: int     statLen      - *var_len set in var_*()
 *
 * RETURNS: 
 * Returns 0 is the SET was successful, else error code.
 * 
 */

int
setTestAndIncr(u_char * var_val,  /* pointer to asn1 encoded set value  */
   u_char   var_val_type,  /* asn1 type of set value */
   int      var_val_len,   /* length of set value */
   u_char  *   statP,      /* pointer to TestAndIncr object */
   int      statLen)       /* length of TestAndIncr object */
{
   long   rcvd_value  ;
   int err;

   USE_ARG(var_val_len);

   /* Get the value received in SNMP-Set message */
   err = setVariable(var_val, var_val_type, (u_char *)&rcvd_value, statLen);
   if (err)
      return err;

   if (rcvd_value < 0) 
      return SNMP_ERR_BADVALUE; /* badValue */

   if ( rcvd_value == (long)*(long *)statP )
   {
      /* Thats correct. The value received is same as the one which we sent */
      /* Test succeeded. Increment the value */
      (*(long *)statP)++;
      if ( (*(long *)statP) > 2147483647) /* wrap it around the max value */
         (*(long *)statP) = 0;
   
      return 0;
   }
   else
   {
      /* Oops looks like a conflict. The values are different */
      return V3_VB_INCONSISTENTVALUE; /* inconsistentValue */
   }
}

/* FUNCTION: snmp_update_rowstatus()
 * 
 * Update the rowstatus value.
 * Examine the current value and based on that process the
 * new value. If the new value is not acceptable, return 
 * V3_VB_WRONGVALUE.
 *
 * If the new value is acceptable, then update the
 * current value and return 0.
 *
 * PARAM1: int * cur_status
 * PARAM2: int new_status
 *
 * RETURNS: 
 * 0                   - if new_status is a valid value
 * V3_VB_WRONGVALUE  - if new_status is an invalid value
 */


int
snmp_update_rowstatus(int * cur_status, int new_status)
{
   if ((new_status < 1) || (new_status > 6))  /* 1..6 */
       return V3_VB_BADVALUE;
   switch(*cur_status)
   {
      case SNMP_RS_INVALID: /* new instance- value not yet set */
         /* accept only createAndGo or createAndWait */
         if (new_status == SNMP_RS_CREATEANDWAIT)
            *cur_status = SNMP_RS_NOTREADY;
         else if (new_status == SNMP_RS_CREATEANDGO)
            *cur_status = SNMP_RS_ACTIVE;
         else if (new_status == SNMP_RS_NOTREADY)
            return V3_VB_WRONGVALUE;
         else
            return V3_VB_INCONSISTENTVALUE;
         break;
      case SNMP_RS_NOTINSERVICE:
      case SNMP_RS_ACTIVE:
         /* accept only notInService or active */
         if (new_status == SNMP_RS_NOTINSERVICE)
            *cur_status = new_status;
         else if (new_status == SNMP_RS_ACTIVE)
            *cur_status = new_status;
         else if (new_status == SNMP_RS_NOTREADY)
            return V3_VB_WRONGVALUE;
         else
            return V3_VB_INCONSISTENTVALUE; 
         break;
      case SNMP_RS_NOTREADY:
         if ((new_status == SNMP_RS_ACTIVE) ||
            (new_status == SNMP_RS_NOTINSERVICE))
         {
            if ((set_parms.vbinds)->num == 1) /* is the the only varbind */
               return V3_VB_INCONSISTENTVALUE; 
            else
               *cur_status = new_status;
         }
         else if (new_status == SNMP_RS_NOTREADY)
            return V3_VB_WRONGVALUE;
         else
            return V3_VB_INCONSISTENTVALUE; 
         break;
      case SNMP_RS_CREATEANDGO:
      case SNMP_RS_CREATEANDWAIT:
      case SNMP_RS_DESTROY:
      default:
         /* bad existing rowstatus value */
         return V3_VB_WRONGVALUE;
   }
   return 0;
}


/* end of file snmp.c */
#endif /*  INCLUDE_SNMP */
