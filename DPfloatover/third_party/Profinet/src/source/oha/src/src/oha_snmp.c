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
/*  F i l e               &F: oha_snmp.c                                :F&  */
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

#define LTRC_ACT_MODUL_ID  33
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_SNMP */

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
/* int snmp_compare(OHA_OID* name1, unsigned len1, OHA_OID* name2, unsigned len2);   */
int  oha_goodValue(U_CHAR * value, U_CHAR inType, int inLen,
                       U_CHAR actualType, int actualLen);
int  oha_setVariable(U_CHAR * var_val, U_CHAR var_val_type, int var_len,
						U_CHAR * statP, int statLen);

/* set parms is used by  SNMP agents to SET value of MIB objects */
struct SetParms   oha_set_parms;


/* FUNCTION: oha_control_snmp_oid()
 *
 * When a OID is found,
 * that is lexicographicly preceded by the input string, the function
 * for that entry is called to find the method of access of the
 * instance of the named variable. If that variable is not found,
 * NULL is returned. If it is found, the function
 * returns a character pointer and a length.
 *
 * Checks the version field. If the version is SNMPv1, then
 * restrict the use of 64-bit datatypes
 * - return NULL on a GET for 64-bit datatype
 * - skip 64-bit objects when checking for GETNEXT
 *
 * PARAM1: OHA_OID mib_node
 * PARAM2: OHA_OID * name
 * PARAM3: unsigned * namelen
 * PARAM4: u_char *   var
 * PARAM5: u_char *   type
 * PARAM6: unsigned * len
 * PARAM7: u_short *  acl
 * PARAM8: int exact
 * PARAM9: int version
 *
 * RETURNS: snmp error code
 */

LSA_UINT32 OHA_SYSTEM_OUT_FCT_ATTR  oha_control_snmp_oid(
    OHA_OID mib_node,       /* IN - OHA_LLDP_MIB_NODE (8802) or OHA_MRP_MIB_NODE (62439) */
    OHA_OID_PTR_TYPE     oid_name,/* IN - name of var, OUT - name matched */
    LSA_UINT32 * oid_len, /* IN -number of sub-ids in name, OUT - sub-ids in matched name */
    OHA_UPPER_MEM_U8_PTR_TYPE   var_val, /* IN/OUT - buffer of variable */
    OHA_COMMON_MEM_U8_PTR_TYPE   var_type,    /* IN/OUT - type of variable */
    /* or exception code 0x80XX (ASN_EOM, ASN_NSO, ASN_NSI) if version is SNMP-V2 */
    LSA_UINT32 *  var_len,     /* IN/OUT - length of variable */
    OHA_COMMON_MEM_U16_PTR_TYPE  access_control, /* OUT - access control */
    LSA_UINT8     msg_type, /* IN - message type */
    LSA_INT32     version)
{
	U_CHAR   stat_type = 0;
	LSA_UINT stat_len  = 0;
	
    LSA_INT   operation;
    LSA_UINT8 * element;

    LSA_INT32 err_code = OHA_SNMP_ERR_NOERROR;


    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_control_snmp_oid(mib_node: 0x%X, oid_name: 0x%X, oid_len: 0x%X)",
                          mib_node, oid_name, oid_len);

    OHA_ASSERT(mib_node == OHA_LLDP_MIB_NODE || mib_node == OHA_MRP_MIB_NODE);
	OHA_ASSERT_NULL_PTR(oid_name);
	OHA_ASSERT_NULL_PTR(oid_len);
	OHA_ASSERT_NULL_PTR(var_val);
	OHA_ASSERT_NULL_PTR(var_type);
	OHA_ASSERT_NULL_PTR(var_len);
	OHA_ASSERT_NULL_PTR(access_control);

	/* check message type, support mib counters: */
    /* set exact based on message type  */
    switch (msg_type)
    {
        case OHA_SNMP_GETNEXT_REQ:
        case OHA_SNMP_GETBULK_REQ:
            operation = NEXT_OP;
            break;
        case OHA_SNMP_GET_REQ:
            operation = GET_OP;
            break;
        case OHA_SNMP_SET_REQ:
            operation = SET_OP;
            break;
        default:
            err_code = OHA_SNMP_ERR_BADVALUE;
            OHA_SNMPERROR("oha_control_snmp_oid(): Unsupported message type");
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_control_snmp_oid(snmp err_code: 0x%X)", err_code);
			return(err_code);
    }

    {
		/* AP01388134: more than one mib object possible */
		oha_init_stats_info(mib_node);

        /* clear static vars for set paramters (see other oha_snmp_getStatPtr call) */
		oha_set_parms.access_method = LSA_NULL;
		oha_set_parms.do_range = LSA_FALSE;
		oha_set_parms.vbflags = 0;

        /* now attempt to retrieve the variable on the local entity */
		{
			/* AP01268561: Error bei Generierung */
			unsigned len = (unsigned) *oid_len;	/* OHA_MAX_OID_LEN only */
			element = oha_snmp_getStatPtr(mib_node, oid_name, /*oid_len*/&len, &stat_type, &stat_len, access_control,
										  operation,&err_code,version);
			*oid_len = len;
		}
		if (err_code == OHA_SNMP_ERR_NOERROR)
		{

        /* retrieve the value of the variable and place it into the outgoing buffer */
		if (msg_type != OHA_SNMP_SET_REQ)
		{
			*var_type = stat_type;
			*var_len  = stat_len;
			if (stat_len <= OHA_MAX_STATION_NAME_LEN)
			{
				if (OHA_IS_NOT_NULL(element))
				{
					OHA_MEMCPY(var_val,element,stat_len);
				}
			}
			else
			{
				err_code = OHA_SNMP_ERR_TOOBIG;
			}
		}
		else
		{
			/* fall to here if operation is write... */
			if (*access_control != OHA_SNMP_RWRITE && 
				*access_control != OHA_SNMP_READCREATE &&
				*access_control != OHA_SNMP_WRITEONLY)
			{
				/* variable is not writable, see oha_snmp_access() */
				err_code = OHA_SNMP_ERR_NOSUCHNAME;
			    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_control_snmp_oid(snmp err_code: 0x%X)", err_code);
				return(err_code);
			}

			/* see if the type and value is consistent with this
			entry's variable. Do range checking */
			if(oha_goodValue(var_val, *var_type, *var_len,
							   stat_type, stat_len) /*!= TRUE*/)
			{
				OHA_SNMPERROR("oha_snmp_goodValue(): type or value failed (InASNParseErrs)");
				OHA_SNMP_LOGOID((char *)oha_sidbuf, var_name, var_name_len,
								OHA_SNMP_ERR_BADVALUE, var_val, var_val_type);
				err_code = OHA_SNMP_ERR_BADVALUE;
			    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_control_snmp_oid(snmp err_code: 0x%X)", err_code);
				return(err_code);
			}

			/* actually do the set if necessary */
			if (msg_type == OHA_SNMP_SET_REQ)
			{
				int   err;
				/* see if access_method was set in snmp_getStatPtr() call */
				if (oha_set_parms.access_method)
				{
					/* call port routine for custom set of variable */
					err = (*oha_set_parms.access_method)(OHA_HANDLE_TYPE_USER, var_val, *var_type, *var_len,
														 oid_name, *oid_len);
				}
				else     /* generic set of variable */
				{
					err = oha_setVariable(var_val, *var_type, *var_len,
												element, stat_len);
					OHA_SNMP_LOGOID(oha_sidbuf, var_name, var_len, err,
									var_val, var_type);
				}
				if (err)
				{
					if (err > OHA_SNMP_ERR_GENERR)
						err_code = OHA_SNMP_ERR_BADVALUE;
					else
						err_code = err; /* return any errors */
				}
			}
		}
		}

		/* do the mapping if necessary */
        switch (version)
        {
            case OHA_SNMP_VERSION_1:        /* map the err_code */
                switch (err_code)
                {
                    case OHA_SNMP_ASN_EOM:    /* endOfMib */
                    case OHA_SNMP_ASN_NSO:  /* noSuchObject */
                    case OHA_SNMP_ASN_NSI:  /* noSuchInstance */
                        err_code = OHA_SNMP_ERR_NOSUCHNAME;
                        break;
                    default:
                        break;
                }
                break;
            case OHA_SNMP_VERSION_2:        /* map the exception code */
            case OHA_SNMP_VERSION_3: /* err_code = OHA_SNMP_ERR_NOERROR */
                switch (err_code)
                {
                    case OHA_SNMP_ASN_EOM:   /* endOfMib */
                    case OHA_SNMP_ASN_NSO:  /* noSuchObject */
                    case OHA_SNMP_ASN_NSI:  /* noSuchInstance */
                        *var_type = (LSA_UINT8)err_code;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_control_snmp_oid(snmp err_code: 0x%X)", err_code);
    return(err_code);
}

/* FUNCTION: snmp_parse_var_op()
 *
 * snmp_parse_var_op() - parse next variable from a sequence of
 * received variables. This extracts the object ID and type of
 * variable for the caller. A pointer to the start of the
 *
 * PARAM1: U_CHAR * data
 * PARAM2: OHA_OID *       var_name
 * PARAM3: unsigned *    var_name_len
 * PARAM4: U_CHAR * var_val_type
 * PARAM5: unsigned *    var_val_len
 * PARAM6: U_CHAR ** var_val
 * PARAM7: unsigned * listlength
 *
 * RETURNS:
 */

U_CHAR *  oha_snmp_parse_var_op(
    U_CHAR * data, /* IN - pointer to the start of object */
    OHA_OID *  var_name,   /* OUT - object id of variable */
    unsigned * var_name_len,  /* IN/OUT - size of var_name, in oids */
                      U_CHAR *   var_val_type,     /* OUT - type of variable (int or octet string) (one byte) */
                      unsigned * var_val_len,   /* OUT - length of variable */
                      U_CHAR **  var_val,       /* OUT - pointer to ASN1 encoded value of variable */
                      unsigned * listlength)    /* IN/OUT - number of valid bytes left in var_op_list */
{
    U_CHAR   var_op_type = 0;
    unsigned var_op_len = *listlength;
    U_CHAR * var_op_start   =  data;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_parse_var_op(var_name: 0x%X, var_name_len: 0x%X)",
                          var_name, var_name_len);
    /* pull off leading SEQUENCE header */
    data = oha_asn_parse_header(data, &var_op_len, &var_op_type);
    if (data == LSA_NULL)
        return LSA_NULL;

    if (var_op_type != (U_CHAR)(ASN_SEQUENCE | ASN_CONSTRUCTOR))
        return LSA_NULL;
    /* pull off field with variable's object Id: */
    data = oha_asn_parse_objid(data, &var_op_len, &var_op_type, var_name, var_name_len);
    if (data == LSA_NULL)
        return LSA_NULL;

    if (var_op_type != (U_CHAR)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OBJECT_ID))
        return LSA_NULL;
    *var_val = data;     /* save pointer to this object */

    /* find out what type of object this is */
    data = oha_asn_parse_header(data, &var_op_len, var_val_type);
    if (data == LSA_NULL)
        return LSA_NULL;

    *var_val_len = var_op_len;
    data += var_op_len;
    *listlength -= (int)(data - var_op_start);
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_parse_var_op(data: 0x%X)", data);
    return data;   /* return */
}



/* FUNCTION: snmp_build_var_op()
 *
 * PARAM1: U_CHAR * data
 * PARAM2: OHA_OID * var_name
 * PARAM3: unsigned var_name_len
 * PARAM4: U_CHAR   var_val_type
 * PARAM5: unsigned var_val_len
 * PARAM6: U_CHAR * var_val
 * PARAM7: unsigned * listlength
 *
 * RETURNS:
 */

U_CHAR *
oha_snmp_build_var_op(
    U_CHAR * data,    /* IN - pointer to the beginning of the output buffer */
    OHA_OID * var_name,      /* IN - object id of variable */
    unsigned var_name_len,  /* IN - length of object id */
    U_CHAR   var_val_type,  /* IN - type of variable */
    unsigned var_val_len,   /* IN - length of variable */
    U_CHAR * var_val,       /* IN - value of variable */
    unsigned *  listlength) /* IN/OUT - number of valid bytes left in output buffer */
{
    unsigned    dummyLen, headerLen;
    U_CHAR  *   dataPtr;
    unsigned    adjustment;    /* number of bytes to move date when re-alaigning */

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_build_var_op(var_name: 0x%X, var_name_len: 0x%X)",
                          var_name, var_name_len);
    dummyLen = *listlength;
    dataPtr = data;
    /* initially build header for maximum size variable ( >255 ) */
    data = oha_asn_build_header(data, &dummyLen,
                                (U_CHAR)(ASN_SEQUENCE | ASN_CONSTRUCTOR), 256);
    if (data == LSA_NULL)
    {
        OHA_SNMPERROR("oha_snmp_build_var_op(): asn build var");
        OHA_SNMP_MIB_CTR(snmpOutTooBigs);
        return LSA_NULL;
    }
    headerLen = data - dataPtr;
    *listlength -= headerLen;
    data = oha_asn_build_objid(data, listlength,
                               (U_CHAR)(ASN_UNIVERSAL | ASN_PRIMITIVE | ASN_OBJECT_ID),
                               var_name, var_name_len);
    if (data == LSA_NULL)
    {
        OHA_SNMPERROR("oha_snmp_build_var_op(): asn build obj");
        OHA_SNMP_MIB_CTR(snmpOutTooBigs);
        return LSA_NULL;
    }
    switch (var_val_type)
    {
        case ASN_INTEGER:
        case OHA_SNMP_GAUGE: /* valid for GAUGE32 and UNSIGNED32 too */
        case OHA_SNMP_COUNTER:
        case OHA_SNMP_TIMETICKS:
            data = oha_asn_build_int(data, listlength, var_val_type,
                                     (LSA_INT32 *)var_val, var_val_len);
            break;
        case ASN_BIT_STR:
            data = oha_asn_build_bits(data, listlength, var_val_type,
                                      var_val, var_val_len);
            break;
        case ASN_OCTET_STR:
        case OHA_SNMP_IPADDRESS:
        case OHA_SNMP_OPAQUE:
            data = oha_asn_build_string(data, listlength, var_val_type,
                                        var_val, var_val_len);
            break;
        case ASN_OBJECT_ID:
            data = oha_asn_build_objid(data, listlength, var_val_type,
                                       (OHA_OID *)var_val, var_val_len / sizeof(OHA_OID));
            break;
        case ASN_NULL:
        case OHA_SNMP_ASN_NSO:
        case OHA_SNMP_ASN_NSI:
        case OHA_SNMP_ASN_EOM:
            data = oha_asn_build_null(data, listlength, var_val_type);
            break;

#ifdef SNMP_COUNTER64
        case OHA_SNMP_COUNTER64:
        case OHA_SNMP_INTEGER64:
        case OHA_SNMP_UNSIGNED64:
            data = oha_asn_build_counter64(data, listlength, var_val_type,
                                           (OHA_COUNTER64 *)var_val, var_val_len);
            break;
#endif

        default:
            OHA_SNMPERROR("oha_snmp_build_var_op(): bad type");
            OHA_SNMP_MIB_CTR(snmpOutTooBigs);
            return LSA_NULL;
    }

    if (data == LSA_NULL)
    {
        OHA_SNMPERROR("oha_snmp_build_var_op(): data");
        OHA_SNMP_MIB_CTR(snmpOutTooBigs);
        return LSA_NULL;
    }

    /* now rebuild the header with the actual size */
    dummyLen = (data - dataPtr) - headerLen;     /* size to put in header */
    if (dummyLen > 255)  /* how many bytes to move data in buffer? */
        adjustment = 0;   /* since we assumed >255, no adjustment needed */
    else if (dummyLen > 127) /* header will shrink 1 byte ? */
        adjustment = 1;   /* byte data left 1 byte */
    else  /* else length encodes in 1 byte, shrink header by 2 bytes */
        adjustment = 2;
    if (adjustment)   /* left shift data right behind header area */
        OHA_MEMMOVE(dataPtr, dataPtr+adjustment, dummyLen+ headerLen - adjustment);
    *listlength += adjustment;    /* fixed return value for bytes left in buf */
    data -= adjustment;  /* fix return pointer to next free space */

    /* rebuild header with correct length */
    if (oha_asn_build_header(dataPtr, &dummyLen,
                             (U_CHAR)(ASN_SEQUENCE | ASN_CONSTRUCTOR), dummyLen) == LSA_NULL)
    {
        OHA_SNMPERROR("oha_snmp_build_var_op(): bad hdr");
        OHA_SNMP_MIB_CTR(snmpOutTooBigs);
        return LSA_NULL;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_build_var_op(data: 0x%X)", data);
    return data;
}


struct snmp_variable *  oha_snmp_get_variables_arr(void);
struct snmp_variable *  oha_mrp_get_variables_arr(void);


/* FUNCTION: snmp_getStatPtr()
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
 * PARAM1: OHA_OID mib_node
 * PARAM2: OHA_OID * name
 * PARAM3: unsigned * namelen
 * PARAM4: U_CHAR *   type
 * PARAM5: unsigned * len
 * PARAM6: U_SHORT *  acl
 * PARAM7: int exact
 * PARAM8: int version
 *
 * RETURNS:
 */

U_CHAR  * oha_snmp_getStatPtr(
    OHA_OID mib_node,    /* IN - OHA_LLDP_MIB_NODE (8802) or OHA_MRP_MIB_NODE (62439) */
    OHA_OID *   name,    /* IN - name of var, OUT - name matched */
    unsigned *  namelen, /* IN -number of sub-ids in name, OUT - sub-ids in matched name */
    U_CHAR *    type,    /* OUT - type of matched variable */
    unsigned *  len,     /* OUT - length of matched variable */
    U_SHORT *   acl,     /* OUT - access control list */
    int         exact,   /* IN - nonzero if exact match wanted */
    LSA_INT32 * err_code,/* OUT - error code,if match is not found */
    int         version)
{
    struct snmp_variable * vp = LSA_NULL;
    int      x;
    U_CHAR * access   =  LSA_NULL;
    int      result;
    int      num_variables = 0;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_getStatPtr(mib_node: 0x%X, name: 0x%X, name_len: 0x%X)",
                          mib_node, name, namelen);

	switch(mib_node)
	{
		case OHA_LLDP_MIB_NODE:
			num_variables = oha_snmp_get_num_variables();
			vp = oha_snmp_get_variables_arr();
		break;
#ifndef OHA_CFG_NO_MRP
#ifndef OHA_CFG_NO_MRP_MIB
		case OHA_MRP_MIB_NODE:
			num_variables = oha_mrp_get_num_variables();
			vp = oha_mrp_get_variables_arr();
		break;
#endif
#endif
		default:	/* not supported */
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"oha_snmp_getStatPtr: mib_node (0x%X) not supported!", mib_node);
		break;
	}

    /* search variables table for match */
    for (x = 0; x < num_variables; vp++, x++)
    {
        /* newer MIB compilers skip the .0 when doing multiple-varbind
         * GET request. Hence doing a smarter compare when
         * namelen < vp->namelen.
         */
		OHA_ASSERT_NULL_PTR(vp);
        if ( *namelen >= vp->namelen )
            result = oha_snmp_compare(name, (int)vp->namelen-1,vp->name,(int)vp->namelen-1);
        else
        {
            /* if it is a GET/SET operation and the SNMP Mgr has skipped ".0"
             * (for a simple, non-table object), then treat it as a valid match */
            if (exact && (*namelen == (unsigned)(vp->namelen-1)) && !(vp->name[vp->namelen-1]))
                result = oha_snmp_compare(name, (*namelen), vp->name, (*namelen));
            else
                result = oha_snmp_compare(name, (*namelen), vp->name, (int)vp->namelen);
        }

        /* AP00866681: GetNext problem, e.g. to lldp.1.3.7.1.3.0 delivers lldp.1.3.7.1.4.1 */
        /*                                   instead of lldp.1.3.7.1.3.1                   */
        /* AP00917457: Remote objects are indexed by "lldpRemTimeMark". In case of a "late" */
		/*             plug the conditon "vp->name[] > name[]" does not match, so we've to  */
		/*             ask for a table too. */
        if ((result < 0) || (exact && (result == 0)) ||
            ((exact == 0) && (*namelen >= vp->namelen) && (result == 0) && 
              ((vp->name[vp->namelen-1] > name[vp->namelen-1])||(vp->name[vp->namelen-1]== 0xFF) ) ))
        {
            /* 64-bit datatypes not supported in SNMPv1 */
            if ((version == OHA_SNMP_VERSION_1) &&
                ((vp->type == OHA_SNMP_COUNTER64) || (vp->type == OHA_SNMP_INTEGER64) || (vp->type == OHA_SNMP_UNSIGNED64)))
            {
                if (exact) /* GET/SET */
                    return LSA_NULL;
                else /* GETNEXT */
                    continue;
            }
            access = (*(vp->findVar))(vp, name, (int *)namelen, exact, (int *)len);
            if (access != LSA_NULL)
                break;
            if ( exact )
                break;
        }
    }

    if (x == num_variables)
    {
        *err_code = OHA_SNMP_ASN_EOM;    /* endOfMib */
        return LSA_NULL;
    }

	OHA_ASSERT_NULL_PTR(vp);
    if (access == LSA_NULL)
    {
        if (vp->name[vp->namelen-1] == 0)  /* Leaf node, no index */
            *err_code = OHA_SNMP_ASN_NSO; /* noSuchObject */
        else
            *err_code = OHA_SNMP_ASN_NSI; /* noSuchInstance */
    }
    /* vp now points to the approprate struct */
    *type = vp->type;
    *acl = vp->acl;

#ifdef OHA_MIB_VALIDATION
    oha_set_parms.p_ranges=&(vp->ranges);
#endif

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_getStatPtr(access: 0x%X, *err_code: 0x%X)", access, *err_code);
    return access;
}



/* FUNCTION: snmp_compare()
 *
 * PARAM1: OHA_OID *  name1
 * PARAM2: unsigned   len1
 * PARAM3: OHA_OID *  name2
 * PARAM4: unsigned   len2
 *
 * RETURNS:
 */

int  oha_snmp_compare(
    OHA_OID *  name1,
    unsigned    len1,
    OHA_OID *   name2,
    unsigned    len2)
{
    unsigned len;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_compare(name1: 0x%X, len1: 0x%X, name2: 0x%X, len2: 0x%X)",
                          name1, len1, name2, len2);
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

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_compare(): both strings are equal");
    return 0;   /* both strings are equal */
}



/* FUNCTION: snmp_sizeof_int()
 *
 * Return the number of bytes needed to store "value". "value" represents
 * a length. It can be length of PDU, header, pkt , etc.
 *
 * PARAM1: LSA_INT32 value
 *
 * RETURNS:
 */

int
oha_snmp_sizeof_int(LSA_INT32 value)
{
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN/OUT : oha_snmp_sizeof_int(value: 0x%X)", value);
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
 * PARAM1: U_CHAR *start_hdr
 * PARAM2: U_CHAR *start_data
 * PARAM3: unsigned len_data
 * PARAM4: int *diff_hdr
 * PARAM5: unsigned *len_final
 *
 * RETURNS:
 */

/* int */
LSA_BOOL  oha_snmp_build_adjust_len(
    U_CHAR * start_hdr, U_CHAR * start_data, 
    unsigned len_data,
    int * diff_hdr,   
    unsigned *  len_final)
{
    int   len_pkt_encoding  =  start_data  -  start_hdr   ;
    U_CHAR   hdr_type       = *   start_hdr   ;           /* Retain the hdr_type */
    unsigned dummy_pktlen;        /* as a parameter to asn_build_header */
    int   diff;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_build_adjust_len(start_hdr: 0x%X, start_data: 0x%X, len_data: 0x%X, len_final: 0x%X)",
                          start_hdr, start_data, len_data, len_final);
    diff = len_pkt_encoding - oha_snmp_sizeof_int(len_data);

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
        OHA_MEMMOVE(start_data-diff, start_data, len_data );
        start_data -= diff ;       /* Adjust start_data accordingly */
    }

    dummy_pktlen = len_data ;

    start_hdr = oha_asn_build_header(start_hdr, &dummy_pktlen, hdr_type, len_data);
    if ( start_hdr == LSA_NULL )
        return LSA_FALSE;

    if ( start_hdr != start_data )
        return LSA_FALSE ;

    if ( diff_hdr != LSA_NULL )       /* Return the diff in encodings */
        *diff_hdr = diff ;

    if ( len_final != LSA_NULL )      /* Return the final len starting from header */
        *len_final = len_data + oha_snmp_sizeof_int(len_data);

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_build_adjust_len(): LSA_TRUE");
    return LSA_TRUE;
}


#ifdef OHA_MIB_VALIDATION
/* FUNCTION : validateValue()
 *
 * Validate a new value with specifications from MIB
 *
 * PARAM1 : value - value to be validated
 * PARAM2 : p_ranges - structure having validation information
 *
 * Returns : LSA_TRUE if the value is good, else LSA_FALSE
 */

/* int */
LSA_BOOL oha_validateValue(LSA_INT32 value, SIZE_INFO p_ranges)
{
    unsigned i;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_build_adjust_len(value: 0x%X, p_ranges: 0x%X)", value, p_ranges);
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
            return LSA_FALSE;
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_validateValue(): LSA_TRUE");
    return LSA_TRUE;
}
#endif   /* OHA_MIB_VALIDATION */


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

int oha_is_us_ascii(char * str,int len)
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

/* FUNCTION: snmp_goodValue()
 *
 * PARAM1: U_CHAR *value
 * PARAM2: U_CHAR inType
 * PARAM3: int      inLen
 * PARAM4: U_CHAR   actualType
 * PARAM5: int      actualLen
 *
 * RETURNS: TRUE if the value is good. Otherwise it returns an error
 * code which informs about the bad value. Error codes being
 * V3_VB_WRONGTYPE, V3_VB_WRONGLENGTH, V3_VB_WRONGVALUE.
 */

int  oha_snmp_goodValue(
    U_CHAR * value,
    U_CHAR   inType,
    int      inLen,
    U_CHAR   actualType,
    int      actualLen)
{
    int   type;
    LSA_INT32  int_value  = 0;
    unsigned datalen = 6;
    U_CHAR   val_type;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_goodValue(value: 0x%X, inType: 0x%X, inLen: 0x%X)", value, inType, inLen);

    /* convert intype to internal representation */
    type = inType == ASN_INTEGER ? ASN_INTEGER : inType == ASN_OCTET_STR ? ASN_OCTET_STR : inType == ASN_OBJECT_ID ? ASN_OBJECT_ID : ASN_NULL;
    if ( !((inType == actualType) || (type == actualType)) )
    {
        return V3_VB_WRONGTYPE;
    }

    switch (actualType)
    {
        case ASN_INTEGER:
        case OHA_SNMP_COUNTER:
        case OHA_SNMP_IPADDRESS:
        case OHA_SNMP_GAUGE: /* valid for GAUGE32 and UNSIGNED32 too */
        case OHA_SNMP_TIMETICKS:
            if (actualLen != 4)  /* check that length is for 32 bit number */
            {
                OHA_SNMPERROR("oha_snmp_goodValue(): INT size not 4 bytes");
                OHA_SNMP_MIB_CTR(snmpInBadValues);
                return V3_VB_WRONGLENGTH;
            }
            /* get the value from varbind */
            if (LSA_NULL==oha_asn_parse_int(value, &datalen, &val_type, &int_value, sizeof(LSA_INT32) ))
                return V3_VB_WRONGLENGTH;
            if (actualType == OHA_SNMP_IPADDRESS)  /* No range checking required for IP Address */
                break;

            /* Validate with the user specified range of values */
            if (oha_set_parms.do_range)
            {
                if (int_value > oha_set_parms.hi_range)
                    return V3_VB_WRONGVALUE;
                if (int_value < oha_set_parms.lo_range)
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
            if (oha_set_parms.vbflags)
            {
                /* Some user defined datatypes like TimeInterval
                 * accept only positive values. If the SF_UNSIGNEDVALUE flag is
                 * set, then verify that the value is positive.
                 */
                if (oha_set_parms.vbflags & SF_UNSIGNEDVALUE)
                    if (int_value < 0)
                        return V3_VB_WRONGVALUE;
                if (oha_set_parms.vbflags & SF_STORAGETYPE)
                    if ((int_value < 1) || (int_value > 5))  /* 1..5 */
                        return V3_VB_WRONGVALUE;
                if (oha_set_parms.vbflags & SF_SECURITYLEVEL)
                    if ((int_value < 1) || (int_value > 3))  /* 1..3 */
                        return V3_VB_WRONGVALUE;
            }
#ifdef OHA_MIB_VALIDATION
            if ( (!oha_set_parms.access_method) && (oha_set_parms.p_ranges) &&
                 oha_set_parms.p_ranges->count)
            {
                if ( oha_validateValue(int_value,oha_set_parms.p_ranges) )
                    break;
                else
                    return V3_VB_WRONGVALUE;
            }
#endif   /* OHA_MIB_VALIDATION */
            break;
        case ASN_BIT_STR:
            break; /* assume all BIT patterns are good values */
        case ASN_OCTET_STR:
        case ASN_OBJECT_ID:
        case OHA_SNMP_OPAQUE:
            if (oha_set_parms.do_range) /* check user's range */
            {
                if (inLen > oha_set_parms.hi_range)
                    return V3_VB_WRONGLENGTH;
                if (inLen < oha_set_parms.lo_range)
                    return V3_VB_WRONGLENGTH;
            }
            else  /* Ranges not specified in stub function. Use those from MIB */
            {
                if (oha_set_parms.access_method)
                {
                    /* access_method is set. Hence validation will be
                     * done in access_method.
                     * Thus user can provide access_method to override
                     * default range checking.
                     */
                }
                else
                {
#ifdef OHA_MIB_VALIDATION
                    if (set_parms.p_ranges && set_parms.p_ranges->count)
                    {
                        if ( validateValue(inLen,set_parms.p_ranges) == TRUE )
                            break;
                        else
                            return V3_VB_WRONGLENGTH;
                    }
                    else
#endif   /* OHA_MIB_VALIDATION */
                    {
                        /* Range not specified in MIB. */
                        if (inLen > actualLen)
                            return V3_VB_WRONGLENGTH;
                    }
                }
            }

            /* for a string, validate all chars to be part of US ASCII */
            if ((actualType == ASN_OCTET_STR) && (!(oha_set_parms.vbflags & SF_NOASCIICHK)))
            {
				/* INICHE_TCPIP_ITGR:  CHANGED TO -->   */
                char buf[OHA_SYS_STRING_MAX];
                int bufsize=OHA_SYS_STRING_MAX;
                datalen=OHA_SYS_STRING_MAX;

                /* read the input string into buf */
                if (!oha_asn_parse_string(value, &datalen, &val_type,
                                          (U_CHAR *)buf, (unsigned *)&bufsize))
                {
                    return V3_VB_WRONGVALUE;  /* input string might be too big */
                }

                /* validate all chars */
                if (!oha_is_us_ascii(buf,bufsize))
                    return V3_VB_WRONGVALUE;  /* input char is non US ASCII */
            }
            break;
        case OHA_SNMP_COUNTER64:
        case OHA_SNMP_INTEGER64:
        case OHA_SNMP_UNSIGNED64:
            /* we don't have to check for the validity of Counter64 value
             * because as per spec it can only be ReadOnly or
             *"accessible-for-notify". Hence this value would never be modified
             * and hence this function would never be called for it */

        default:
            OHA_SNMPERROR("oha_snmp_goodValue(): bad type");
            return V3_VB_WRONGTYPE; /* bad type, should this be assert later? */
    }

    /* return TRUE; */
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_goodValue(): V3_VB_NOERROR");
    return V3_VB_NOERROR;
}

/* FUNCTION: oha_parse_int()
 *
 * oha_parse_int - pulls a long out of an int type.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes
 * following the end of this object.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * datalength
 * PARAM3: U_CHAR type
 * PARAM4: LSA_INT32 * intp
 * PARAM5: unsigned intsize
 *
 * RETURNS: NULL on any error.
 */

U_CHAR oha_parse_int(
    U_CHAR * data,  /* IN - pointer to start of object */
    unsigned    datalength, /* IN - number of valid bytes in buffer (data) */
    U_CHAR      type,       /* IN - asn type of object */
    LSA_INT32 * intp,       /* IN/OUT - pointer to start of output buffer */
    unsigned    intsize)    /* IN - size of output buffer */
{
    /*
     * ASN.1 integer ::= 0x02 asnlength byte {byte}*
     */
    U_CHAR*  bufp   =  data;
    LSA_INT32  value  =  0; /* accumulator for actual length */

    OHA_FUNCTION_TRACE_05(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn_parse_int(data: 0x%X, datalength: 0x%X, type: 0x%X, intp: 0x%X, intsize: 0x%X)",
                          data, datalength, type, intp, intsize);

    if ( (!data) || (!datalength) || (!type) || (!intp) )
    {
        return V3_VB_WRONGLENGTH;
    }

    if (intsize != sizeof(LSA_INT32))
    {
        OHA_SNMPERROR("oha_parse_int(): out buff length <> size of long");
        return V3_VB_WRONGLENGTH;
    }

    /* make sure ASN1 parameter is from integer type (fix SilverCrick 7.0.3.3) */
    if (type != ASN_INTEGER)
    {
        OHA_SNMPERROR("oha_asn_parse_int(): asn1 parameter isn't integer");
        OHA_SNMP_MIB_CTR(snmpInASNParseErrs);
        return V3_VB_WRONGTYPE;
    }

    if (datalength == 0)
    {
        OHA_SNMPERROR("oha_asn_parse_length(): We don't support indefinite lengths");
        return V3_VB_WRONGLENGTH;
    }
    if (datalength > sizeof(LSA_INT32))
    {
        OHA_SNMPERROR("oha_asn_parse_length(): we can't support data lengths that long");
        return V3_VB_WRONGLENGTH;
    }
    if (datalength > intsize)
    {
        OHA_SNMPERROR("oha_parse_int(): var too small");
        return V3_VB_WRONGLENGTH;
    }

	OHA_MEMCPY(&value, bufp, datalength);

    *intp = value;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_parse_int(bufp: 0x%X)", bufp);
    return V3_VB_NOERROR;
}

/* FUNCTION: oha_goodValue()
 *
 * PARAM1: U_CHAR *value
 * PARAM2: U_CHAR inType
 * PARAM3: int      inLen
 * PARAM4: U_CHAR   actualType
 * PARAM5: int      actualLen
 *
 * RETURNS: TRUE if the value is good. Otherwise it returns an error
 * code which informs about the bad value. Error codes being
 * V3_VB_WRONGTYPE, V3_VB_WRONGLENGTH, V3_VB_WRONGVALUE.
 */

int  oha_goodValue(
    U_CHAR * value,
    U_CHAR   inType,
    int      inLen,
    U_CHAR   actualType,
    int      actualLen)
{
    int   type;
    LSA_INT32  int_value = 0;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_goodValue(value: 0x%X, inType: 0x%X, inLen: 0x%X)", value, inType, inLen);
	OHA_ASSERT_NULL_PTR(value);

    /* convert intype to internal representation */
    type = inType == ASN_INTEGER ? ASN_INTEGER : inType == ASN_OCTET_STR ? ASN_OCTET_STR : inType == ASN_OBJECT_ID ? ASN_OBJECT_ID : ASN_NULL;
    if ( !((inType == actualType) || (type == actualType)) )
    {
        return V3_VB_WRONGTYPE;
    }

    switch (actualType)
    {
        case ASN_INTEGER:
        case OHA_SNMP_COUNTER:
        case OHA_SNMP_IPADDRESS:
        case OHA_SNMP_GAUGE: /* valid for GAUGE32 and UNSIGNED32 too */
        case OHA_SNMP_TIMETICKS:
            if (actualLen != 4)  /* check that length is for 32 bit number */
            {
                OHA_SNMPERROR("oha_goodValue(): INT size not 4 bytes");
                return V3_VB_WRONGLENGTH;
            }
            /* get the value from request */
            if (oha_parse_int(value, inLen, inType, &int_value, sizeof(LSA_INT32) ))
                return V3_VB_WRONGLENGTH;
            if (actualType == OHA_SNMP_IPADDRESS)  /* No range checking required for IP Address */
                break;

            /* Validate with the user specified range of values */
            if (oha_set_parms.do_range)
            {
                if (int_value > oha_set_parms.hi_range)
                    return V3_VB_WRONGVALUE;
                if (int_value < oha_set_parms.lo_range)
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
            if (oha_set_parms.vbflags)
            {
                /* Some user defined datatypes like TimeInterval
                 * accept only positive values. If the SF_UNSIGNEDVALUE flag is
                 * set, then verify that the value is positive.
                 */
                if (oha_set_parms.vbflags & SF_UNSIGNEDVALUE)
                    if (int_value < 0)
                        return V3_VB_WRONGVALUE;
                if (oha_set_parms.vbflags & SF_STORAGETYPE)
                    if ((int_value < 1) || (int_value > 5))  /* 1..5 */
                        return V3_VB_WRONGVALUE;
                if (oha_set_parms.vbflags & SF_SECURITYLEVEL)
                    if ((int_value < 1) || (int_value > 3))  /* 1..3 */
                        return V3_VB_WRONGVALUE;
            }
#ifdef OHA_MIB_VALIDATION
            if ( (!oha_set_parms.access_method) && (oha_set_parms.p_ranges) &&
                 oha_set_parms.p_ranges->count)
            {
                if ( oha_validateValue(int_value,oha_set_parms.p_ranges) )
                    break;
                else
                    return V3_VB_WRONGVALUE;
            }
#endif   /* OHA_MIB_VALIDATION */
            break;
        case ASN_BIT_STR:
            break; /* assume all BIT patterns are good values */
        case ASN_OCTET_STR:
        case ASN_OBJECT_ID:
        case OHA_SNMP_OPAQUE:
            if (oha_set_parms.do_range) /* check user's range */
            {
                if (inLen > oha_set_parms.hi_range)
                    return V3_VB_WRONGLENGTH;
                if (inLen < oha_set_parms.lo_range)
                    return V3_VB_WRONGLENGTH;
            }
            else  /* Ranges not specified in stub function. Use those from MIB */
            {
                if (oha_set_parms.access_method)
                {
                    /* access_method is set. Hence validation will be
                     * done in access_method.
                     * Thus user can provide access_method to override
                     * default range checking.
                     */
                }
                else
                {
#ifdef OHA_MIB_VALIDATION
                    if (set_parms.p_ranges && set_parms.p_ranges->count)
                    {
                        if ( validateValue(inLen,set_parms.p_ranges) == TRUE )
                            break;
                        else
                            return V3_VB_WRONGLENGTH;
                    }
                    else
#endif   /* OHA_MIB_VALIDATION */
                    {
                        /* Range not specified in MIB. */
                        if (inLen > actualLen)
                            return V3_VB_WRONGLENGTH;
                    }
                }
            }

            /* for a string, validate all chars to be part of US ASCII */
            if ((actualType == ASN_OCTET_STR) && (!(oha_set_parms.vbflags & SF_NOASCIICHK)))
            {
				/* INICHE_TCPIP_ITGR:  CHANGED TO -->   */
                char buf[OHA_SYS_STRING_MAX];
                int bufsize=OHA_SYS_STRING_MAX;

                /* read the input string into buf */
                if (inLen > bufsize)
				{
                    return V3_VB_WRONGVALUE;  /* input string might be too big */
                }
			    OHA_MEMCPY((char *)buf, (char *)value, (int)inLen);

                /* validate all chars */
                if (!oha_is_us_ascii(buf,bufsize))
                    return V3_VB_WRONGVALUE;  /* input char is non US ASCII */
            }
            break;
        case OHA_SNMP_COUNTER64:
        case OHA_SNMP_INTEGER64:
        case OHA_SNMP_UNSIGNED64:
            /* we don't have to check for the validity of Counter64 value
             * because as per spec it can only be ReadOnly or
             *"accessible-for-notify". Hence this value would never be modified
             * and hence this function would never be called for it */

        default:
            OHA_SNMPERROR("oha_goodValue(): bad type");
            return V3_VB_WRONGTYPE; /* bad type, should this be assert later? */
    }

    /* return TRUE; */
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_goodValue(): V3_VB_NOERROR");
    return V3_VB_NOERROR;
}


/* FUNCTION: snmp_setVariable()
 *
 * PARAM1: U_CHAR * var_val
 * PARAM2: U_CHAR   var_val_type
 * PARAM3: U_CHAR * statP
 * PARAM4: int      statLen
 *
 * RETURNS: 0 on success, else error code
 */

int  oha_setVariable(
    U_CHAR * var_val, /* ASN.1 encoded value of variable to set */
    U_CHAR   var_val_type,  /* ASN.1 type of var_val */
    int      var_len,
    U_CHAR * statP,      /* pointer returned by var_ routine */
    int      statLen)    /* var_len set by var_routine */
{
    /* set an abritrarily large buffer parm for asn1 parsing. We don't
    use the returned value for this anyway. -JB- */
    unsigned   asn_buf_size = 1000;     /*  */
    unsigned   set_var_size = statLen;  /* bytes in field to set */
    LSA_INT32  tmpvalue     = 0;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_setVariable(var_val: 0x%X, var_val_type: 0x%X, statLen: 0x%X)", var_val, var_val_type, statLen);
    switch (var_val_type)
    {
        case ASN_INTEGER:
        case OHA_SNMP_COUNTER:
        case OHA_SNMP_GAUGE: /* valid for GAUGE32 and UNSIGNED32 too */
        case OHA_SNMP_TIMETICKS:
            if (oha_parse_int(var_val, var_len, var_val_type,
                              &tmpvalue, set_var_size))
            {
                return V3_VB_BADVALUE;
            }
            if ((oha_set_parms.vbflags & SF_STORAGETYPE) &&
                ((LSA_INT32)(*statP) != tmpvalue))
            {
                if ((LSA_INT32)(*statP) == OHA_SNMP_ST_PERMANENT)
                    return V3_VB_WRONGVALUE;  /* can't change this value */
                if ((LSA_INT32)(*statP) == OHA_SNMP_ST_READONLY)
                    return V3_VB_WRONGVALUE;  /* can't change this value */
            }
            if (oha_set_parms.vbflags & SF_SECURITYLEVEL)
                *(LSA_INT32 *)(statP) = OHA_SNMP_SL2F(tmpvalue); /* map rcvd SecurityLevel to flags */
            else
                *(LSA_INT32 *)(statP) = tmpvalue; /* assign the read value */
            break;
        case ASN_BIT_STR:
            if (var_len > statLen) /* set_var_size */
            {
                return V3_VB_BADVALUE;
            }
            OHA_MEMCPY((char *)statP, (char *)var_val, (int)set_var_size);
            break;
        case ASN_OCTET_STR:
        case OHA_SNMP_IPADDRESS:
        case OHA_SNMP_OPAQUE:

#ifdef OHA_MIB_VALIDATION
            if (oha_set_parms.p_ranges && (oha_set_parms.p_ranges->count >0))
            {
                if ( oha_set_parms.p_ranges->rtype == MCT_VALUES )
                {
                    /* Set the highest value. Assuming last value is the highest */
                    set_var_size = (unsigned)
                                   oha_set_parms.p_ranges->values[oha_set_parms.p_ranges->count-1];
                }
                else  /* oha_set_parms.p_ranges.rtype == MCT_RANGES */
                {
                    /* Set the highest value. Assuming last pair is the highest */
                    set_var_size = (unsigned)
                                   oha_set_parms.p_ranges->values[(oha_set_parms.p_ranges->count-1)*2+1];
                }
            }
#endif   /* OHA_MIB_VALIDATION */

            if (var_len > statLen) /* set_var_size */
            {
                return V3_VB_BADVALUE;
            }

            OHA_MEMCPY((char *)statP, (char *)var_val, (int)set_var_size);

            /* null terminate the strings */
            if ( var_val_type == ASN_OCTET_STR )
                *(statP + set_var_size) = '\0';
            break;
        case ASN_OBJECT_ID:
            if (!oha_asn_parse_objid(var_val, &asn_buf_size, &var_val_type,
                                     (OHA_OID*)statP, &set_var_size))
            {
                return V3_VB_BADVALUE;
            }
            break;

#ifdef SNMP_COUNTER64
        case OHA_SNMP_COUNTER64:
        case OHA_SNMP_INTEGER64:
        case OHA_SNMP_UNSIGNED64:
            (void)oha_asn_parse_counter64(var_val, &asn_buf_size, &var_val_type,
                                          (OHA_COUNTER64  *)statP, set_var_size);
            break;
#endif

    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_setVariable()");
    return V3_VB_NOERROR;
}


/* FUNCTION: snmp_setVariable()
 *
 * PARAM1: U_CHAR * var_val
 * PARAM2: U_CHAR   var_val_type
 * PARAM3: U_CHAR * statP
 * PARAM4: int      statLen
 *
 * RETURNS: 0 on success, else error code
 */

int  oha_snmp_setVariable(
    U_CHAR * var_val, /* ASN.1 encoded value of variable to set */
    U_CHAR   var_val_type,  /* ASN.1 type of var_val */
    U_CHAR * statP,      /* pointer returned by var_ routine */
    int      statLen)    /* var_len set by var_ routine */
{
    /* set an abritrarily large buffer parm for asn1 parsing. We don't
    use the returned value for this anyway. -JB- */
    unsigned   asn_buf_size = 1000;     /*  */
    unsigned   set_var_size = statLen;  /* bytes in field to set */
    LSA_INT32  tmpvalue     = 0;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_setVariable(var_val: 0x%X, var_val_type: 0x%X, statLen: 0x%X)", var_val, var_val_type, statLen);
    switch (var_val_type)
    {
        case ASN_INTEGER:
        case OHA_SNMP_COUNTER:
        case OHA_SNMP_GAUGE: /* valid for GAUGE32 and UNSIGNED32 too */
        case OHA_SNMP_TIMETICKS:
            if (!oha_asn_parse_int(var_val, &asn_buf_size, &var_val_type,
                                   &tmpvalue, set_var_size))
            {
                return V3_VB_BADVALUE;
            }
            if ((oha_set_parms.vbflags & SF_STORAGETYPE) &&
                ((LSA_INT32)(*statP) != tmpvalue))
            {
                if ((LSA_INT32)(*statP) == OHA_SNMP_ST_PERMANENT)
                    return V3_VB_WRONGVALUE;  /* can't change this value */
                if ((LSA_INT32)(*statP) == OHA_SNMP_ST_READONLY)
                    return V3_VB_WRONGVALUE;  /* can't change this value */
            }
            if (oha_set_parms.vbflags & SF_SECURITYLEVEL)
                *(LSA_INT32 *)(statP) = OHA_SNMP_SL2F(tmpvalue); /* map rcvd SecurityLevel to flags */
            else
                *(LSA_INT32 *)(statP) = tmpvalue; /* assign the read value */
            break;
        case ASN_BIT_STR:
            if (!oha_asn_parse_bits(var_val, &asn_buf_size, &var_val_type,
                                    statP, &set_var_size))
            {
                return V3_VB_BADVALUE;
            }
            break;
        case ASN_OCTET_STR:
        case OHA_SNMP_IPADDRESS:
        case OHA_SNMP_OPAQUE:
            /* allow buffer sizes up limit set in var_ routine */
            if (oha_set_parms.do_range) /* if var_ routine set limits */
                set_var_size = (unsigned)oha_set_parms.hi_range;
#ifdef OHA_MIB_VALIDATION
            else if (oha_set_parms.p_ranges && (oha_set_parms.p_ranges->count >0))
            {
                if ( oha_set_parms.p_ranges->rtype == MCT_VALUES )
                {
                    /* Set the highest value. Assuming last value is the highest */
                    set_var_size = (unsigned)
                                   oha_set_parms.p_ranges->values[oha_set_parms.p_ranges->count-1];
                }
                else  /* oha_set_parms.p_ranges.rtype == MCT_RANGES */
                {
                    /* Set the highest value. Assuming last pair is the highest */
                    set_var_size = (unsigned)
                                   oha_set_parms.p_ranges->values[(oha_set_parms.p_ranges->count-1)*2+1];
                }
            }
#endif   /* OHA_MIB_VALIDATION */
            if (!oha_asn_parse_string(var_val, &asn_buf_size, &var_val_type,
                                      statP, &set_var_size))
            {
                return V3_VB_BADVALUE;
            }
            /* null terminate the strings */
            if ( var_val_type == ASN_OCTET_STR )
                *(statP + set_var_size) = '\0';
            break;
        case ASN_OBJECT_ID:
            if (!oha_asn_parse_objid(var_val, &asn_buf_size, &var_val_type,
                                     (OHA_OID*)statP, &set_var_size))
            {
                return V3_VB_BADVALUE;
            }
            break;

#ifdef SNMP_COUNTER64
        case OHA_SNMP_COUNTER64:
        case OHA_SNMP_INTEGER64:
        case OHA_SNMP_UNSIGNED64:
            (void)oha_asn_parse_counter64(var_val, &asn_buf_size, &var_val_type,
                                          (OHA_COUNTER64  *)statP, set_var_size);
            break;
#endif

    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_setVariable()");
    return V3_VB_NOERROR;
}



/* FUNCTION: snmp_shift_array()
 *
 * PARAM1: U_CHAR * begin
 * PARAM2: int      length
 * PARAM3: int      shift_amount
 *
 * RETURNS:
 */

void  oha_snmp_shift_array(
    U_CHAR * begin,
    int      length,
    int      shift_amount)
{
    U_CHAR * oldp, *  newp;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_shift_array(begin: 0x%X, length: 0x%X, shift_amount: 0x%X)", begin, length, shift_amount);
    if (shift_amount >= 0)
    {
        oldp = begin + length - 1;
        newp = oldp + shift_amount;

        while (length--)
            *newp-- = *oldp--;
    }
    else
    {
        oldp = begin;
        newp = begin + shift_amount;

        while (length--)
            *newp++ = *oldp++;
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_shift_array()");
}

/* move oid values into a byte array. Assumes all values are < 256! */


/* FUNCTION: oid2bytes()
 *
 * PARAM1: char * bytebuf
 * PARAM2: OHA_OID * oidbuf
 * PARAM3: int count
 *
 * RETURNS:
 */

void  oha_snmp_oid2bytes(
    char * bytebuf, 
    OHA_OID * oidbuf, 
    int count)
{
    int   i;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_oid2bytes(bytebuf: 0x%X, oidbuf: 0x%X, count: 0x%X)", bytebuf, oidbuf, count);
    for (i = 0; i < count; i++)
        *bytebuf++ = *(char *)oidbuf++;
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_oid2bytes()");
}

/* FUNCTION: oha_lldp_objid_to_asn1()
 *
 * build_asn1 - Builds an ASN1 object identifier object containing the input OID.
 * On entry, datalength is input as the number of valid bytes following
 * "data".
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * datalength
 *
 * RETURNS: - OK if the identifier is written,
 *          - ERR_PARAM if no buffer exists
 *          - ERR_RESOURCE if the buffer is to small.
 */

LSA_UINT16 oha_lldp_objid_to_asn1(
    LSA_UINT8 * data,/* IN/OUT - pointer to start of encoded object */
    LSA_UINT *  datalength)   /* IN/OUT - number of valid bytes left in buffer */
{
    /* ASN.1 objid ::= 0x06 asnlength subidentifier {subidentifier}*
     * subidentifier ::= {leadingbyte}* lastbyte
     * leadingbyte ::= 1 7bitvalue
     * lastbyte ::= 0 7bitvalue  */

    OHA_OID  Oid_lldpLocManAddrOID[] = {OHA_SNMP_PNO_SMI_OID};

    U_CHAR * bp =  data;
    OHA_OID * op = Oid_lldpLocManAddrOID;
    unsigned objidlength = OHA_SNMP_PNO_SMI_OID_LEN;
    unsigned asn_length = 0;
    U_LONG   subid, mask, testmask;
    unsigned bits, testbits;
    LSA_UINT16 RetVal = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_lldp_objid_to_asn1(data: 0x%X, datalength: 0x%X)", data, datalength);

    if (OHA_IS_NULL(data) || OHA_IS_NULL(datalength) || *datalength == 0)
    {
        RetVal = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_lldp_objid_to_asn1(RetVal: 0x%X)", RetVal);
        return RetVal;
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

        /* mask can't be zero here */
        OHA_ASSERT (mask != 0);

        /* take care of special case when subid > 0x0FFFFFFF
         * because for this case "(mask>>bits) != 7F" */
        if (subid & HUGEOIDMASK)
        {
            asn_length++;
            if (*datalength < asn_length || asn_length > LLDP_TLV_MGM_ADDR_MAX_OID_STRING_LEN)
            {
                *datalength = 0;
                RetVal = OHA_ERR_RESOURCE;
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_lldp_objid_to_asn1(RetVal: 0x%X)", RetVal);
                return RetVal;
            }

            *bp = (U_CHAR)((subid & HUGEOIDMASK) >> bits);
            *bp++ |= ASN_BIT8;
            bits-=7;  /* bits becomes 28-7=21 */
            mask = 0x7F << bits;
        }
        for (; mask != 0x7F; mask >>= 7, bits -= 7)
        {
            asn_length++;
            if (*datalength < asn_length || asn_length > LLDP_TLV_MGM_ADDR_MAX_OID_STRING_LEN)
            {
                *datalength = 0;
                RetVal = OHA_ERR_RESOURCE;
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_lldp_objid_to_asn1(RetVal: 0x%X)", RetVal);
                return RetVal;
            }

            /* MSC 8.0 gets warning on this next line: = (U_CHAR)((subid
               * & mask) >> bits) | ASN_BIT8;   */
            *bp = (U_CHAR)((subid & mask) >> bits);
            *bp++ |= ASN_BIT8;
        }

        asn_length++;
        if (*datalength < asn_length || asn_length > LLDP_TLV_MGM_ADDR_MAX_OID_STRING_LEN)
        {
            *datalength = 0;
            RetVal = OHA_ERR_RESOURCE;
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_lldp_objid_to_asn1(RetVal: 0x%X)", RetVal);
            return RetVal;
        }

        *bp++ = (U_CHAR)(subid & mask);
    }

    *datalength = asn_length;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_lldp_objid_to_asn1(RetVal: 0x%X)", RetVal);

    return RetVal;
}

/* FUNCTION: asn_parse_objid()
 *
 * asn1_to_objid - pulls an object indentifier out of an ASN object identifier type.
 * On entry, datalength is input as the number of valid bytes following
 * "data".  On exit, it is returned as the number of valid bytes.
 *
 * "objid" is filled with the object identifier.
 *
 * PARAM1: U_CHAR * data
 * PARAM2: unsigned * datalength
 * PARAM4: OHA_OID *  objid
 * PARAM5: unsigned * objidlength
 *
 * RETURNS: - OK if the identifier is written,
 *          - ERR_PARAM if no buffer exists
 *          - ERR_RESOURCE if the buffer is to small.
 *          - ERR_SYNTAX if subidentifier is too big
 */

LSA_UINT16  oha_asn1_to_objid(
    LSA_UINT8 * data, /* IN - pointer to start of object */
    LSA_UINT    datalength,    /* IN - number of coded bytes */
    OHA_OID *   objid,         /* OUT - pointer to start of output buffer */
    LSA_UINT *  objidlength)   /* IN/OUT - number of sub-id's in objid */
{
    /* ASN.1 objid ::= 0x06 asnlength subidentifier {subidentifier}*
     * subidentifier ::= {leadingbyte}* lastbyte
     * leadingbyte ::= 1 7bitvalue
     * lastbyte ::= 0 7bitvalue  */

    U_CHAR * bufp  =  data;
    OHA_OID * oidp = objid + 1;
    U_LONG   subidentifier;
    U_LONG   oid_length = 1;
    U_LONG   asn_length = datalength;    /* number of bytes in encoded image of object Id */
    LSA_UINT16 RetVal = OHA_OK;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_asn1_to_objid(data: 0x%X, datalength: 0x%X, objid: 0x%X, objidlength: 0x%X)",
                          data, datalength, objid, objidlength);

    if (OHA_IS_NULL(data) || datalength == 0 || datalength > LLDP_TLV_MGM_ADDR_MAX_OID_STRING_LEN ||
        OHA_IS_NULL(objid) || OHA_IS_NULL(objidlength) || *objidlength < 2 )
    {
        RetVal = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn1_to_objid(RetVal: 0x%X)", RetVal);
        return RetVal;
    }

    while (asn_length > 0)
    {
        subidentifier = 0;
        do
        {     /* shift and add in low order 7 bits */
            subidentifier = (subidentifier << 7) + ((U_LONG)(*bufp) & ~ASN_BIT8);
            asn_length--;
        }
        while (*(U_CHAR *)bufp++ & ASN_BIT8);   /* last byte has high bit clear */

        /* check subidentifier value limit */
#if (MAX_SUBID != 0xFFFFFFFF)    /* don't do both if 32bit sub Ids */
        if (subidentifier > MAX_SUBID)
        {
            *objidlength = 0;
            RetVal = OHA_ERR_SYNTAX;
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "subidentifier (0x%X) too big", subidentifier);
            return RetVal;
        }
#endif
        oid_length++;
        if (*objidlength < oid_length || *objidlength > LLDP_TLV_MGM_ADDR_MAX_OID_STRING_LEN)
        {
            *objidlength = 0;
            RetVal = OHA_ERR_RESOURCE;
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn1_to_objid(RetVal: 0x%X)", RetVal);
            return RetVal;
        }

        *oidp++ = (OHA_OID)subidentifier;
    }

    /* The first two subidentifiers are encoded into the first
     * component with the value (X 40) + Y, where: X is the value of
     * the first subidentifier. Y is the value of the second
     * subidentifier.     */

    subidentifier = (U_LONG)objid[1];
    objid[1] = (U_CHAR)(subidentifier % 40);
    objid[0] = (U_CHAR)((subidentifier - objid[1]) / 40);

    *objidlength = (int)(oidp - objid);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_asn1_to_objid(RetVal: 0x%X)", RetVal);
    return RetVal;
}

#if 0
/* FUNCTION: oha_set_TxInterval()
 * Generic function to set the LLDP TxInterval.
 * If a snmp object needs to be set, then in the var_*() function,
 * set the access_method to set.
 *
 * Test if the received value is equal to the current value.
 * If not, call LldpSet(AllPorts).
 * If it is equal, there is nothing to do.
 *
 * PARAM1: u_char *var_val      - rcvd value (asn encoded)
 * PARAM2: u_char  var_val_type - type of received value
 * PARAM3: int     var_val_len  - len of received value
 * PARAM4: u_char *statP        - return value of var_*()
 * PARAM5: int     statLen      - *var_len set in var_*()
 *
 * RETURNS:
 * Returns 0 is the SET was successful, else an error code.
 */
int oha_set_TxInterval(
    U_CHAR * var_val,  /* pointer to asn1 encoded set value  */
    U_CHAR   var_val_type,  /* asn1 type of set value   */
    int      var_val_len,   /* length of set value      */
    U_CHAR  *   statP,      /* pointer to actual object */
    int      statLen)       /* length of actual object  */
{
    LSA_INT32 received_value;
    LSA_INT32 current_value;
    int  err;

    LSA_UNUSED_ARG(var_val_len);

    /* Get the value received in SNMP-Set message */
    err = oha_snmp_setVariable(var_val, var_val_type, (U_CHAR *)&received_value, statLen);
    if (err)
        return err;

    if (received_value < 0 || statLen != sizeof(LSA_UINT32))
    {
        return OHA_SNMP_ERR_BADVALUE; /* badValue */
    }

    current_value  = (LSA_INT32)*(LSA_INT32 *)statP;

    if ( current_value != received_value )
    {
        OHA_HDB_LLDP_PTR_TYPE pHDBLldp = OHA_LLDPGetHandleByIndex(1);

        if (OHA_IS_NOT_NULL(pHDBLldp))
        {
            LSA_UINT16 PortID;

            /* The values are different -> write the value to the "database" */
            (LSA_VOID)OHA_DbStoreLocalTTL(pHDBLldp->pDB->NicId, (LSA_UINT16)received_value,
                                          OHA_LLDP_TTL_FACTOR);

            /* .. and do a LldpSet(AllPorts) - loop over all ports */
            for (PortID = 1; PortID <= OHA_CFG_MAX_PORTS && PortID <= pHDBLldp->pDB->PortCnt; PortID++)
            {
                if (pHDBLldp->RequestState == OHA_LLDP_STATE_START) /* LLDP-Set is possible ? */
                {
                    (LSA_VOID)OHA_LLDPSet(pHDBLldp, PortID, OHA_HANDLE_TYPE_SYS); /* send them */
                }
            }
        }
    }
    return 0;
}
#endif

/* end of file oha_snmp.c */

