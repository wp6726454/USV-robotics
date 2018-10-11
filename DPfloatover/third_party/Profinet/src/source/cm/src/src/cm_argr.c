/*****************************************************************************/
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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_argr.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  AR-graph functions                                                       */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  6
#define CM_MODULE_ID       6

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_ACP

/*------------------------------------------------------------------------------
// static variables
//----------------------------------------------------------------------------*/

static LSA_UINT8 cm_argr_used[CM_PNIO_C_SDU_MAX_LENGTH];
/* ^--- static is ok, because CM is single-threaded (see doc) */

/*------------------------------------------------------------------------------
// validate a station-name
//----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_is_valid_port_name(
	LSA_UINT16 length,
	CM_COMMON_MEM_U8_PTR_TYPE name
) {
	CM_LOG_FCT("cm_is_valid_port_name")

	CM_ASSERT(is_not_null(name));

	/*
	 * port-names have the following forms (see the definition of station-name below)
	 *
	 * "port-xyz" with x, y, z = 0...9
	 * "port-xyz-abcde" with x, y, z, a, b, c, d, e = 0...9
	 */

	if(
		length >= 8
		&& (name[0] == 'p' || name[0] == 'P') /* note: case insensitive, see caller */
		&& (name[1] == 'o' || name[1] == 'O')
		&& (name[2] == 'r' || name[2] == 'R')
		&& (name[3] == 't' || name[3] == 'T')
		&& name[4] == '-'
		&& (name[5] >= '0' && name[5] <= '9')
		&& (name[6] >= '0' && name[6] <= '9')
		&& (name[7] >= '0' && name[7] <= '9')
	) {

		if( length == 8 ) {
			return LSA_TRUE; /* matches with "port-xyz" with x, y, z = 0...9 */
		}

		if(
			length == 14
			&& name[8] == '-'
			&& (name[ 9] >= '0' && name[ 9] <= '9')
			&& (name[10] >= '0' && name[10] <= '9')
			&& (name[11] >= '0' && name[11] <= '9')
			&& (name[12] >= '0' && name[12] <= '9')
			&& (name[13] >= '0' && name[13] <= '9')
		) {
			return LSA_TRUE; /* matches with "port-xyz-abcde" with x, y, z, a, b, c, d, e = 0...9 */
		}
	}

	return LSA_FALSE; /* not a port-name */
}

/*----------------------------------------------------------------------------*/

static LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_is_valid_name(
	LSA_UINT16 length,
	CM_COMMON_MEM_U8_PTR_TYPE name,
	LSA_BOOL check_if_is_alias_name
) {
	CM_LOG_FCT("cm_is_valid_name")
	LSA_UINT16 max_length;
	LSA_UINT16 idx;
	LSA_UINT16 lbl;
	LSA_UINT16 nr_of_labels;
	LSA_BOOL looks_like_ipaddr;

	CM_ASSERT( /* ascii */
		'-' == 0x2d
		&& '.' == 0x2e
		&& '0' == 0x30
		&& 'a' == 0x61
		&& 'A' == 0x41
		&& '0' < '9'
		&& 'a' < 'z'
		&& 'A' < 'Z'
	);

	/*
	 * a station-name has the following syntax:
	 * (see AP00216830)
	 *
	 * 1 or more labels, separated by [.]
	 * total length is 1-240
	 * label-length is 1-63
	 * labels consist of [a-z0-9-]
	 * labels do not start with [-]
	 * labels do not end with [-]
	 * the first (leftmost) label does not match with "port-xyz" with x, y, z = 0...9
	 * the first (leftmost) label does not match with "port-xyz-abcde" with x, y, z, a, b, c, d, e = 0...9
	 * station-names do not have the form n.n.n.n, n = 0...999
	 * labels do only start with 'xn-' if RFC 3490 is applied
	 *
	 * an alias-name has the following syntax:
	 *
	 * AliasNameValue = LLDP_PortID + "." + LLDP_ChassisID
	 * LLDP_PortID    = "port-xyz" or "port-xyz-rstuv"
	 * LLDP_ChassisID = NameOfStationValue
	 */

	max_length = 240;

	if( check_if_is_alias_name ) {

		max_length += 14 + 1; /* prefix LLDP_PortID + "." */
	}

	if( ! (
		length >= 1
		&& length <= max_length
	) ) {
		return LSA_FALSE; /* station-name is too short or too long */
	}

	if( is_null(name) ) {
		return LSA_FALSE; /* a bug */
	}

	/***/

	nr_of_labels = 0;

	looks_like_ipaddr = LSA_TRUE;

	lbl = 0; /* start of label */

	for( idx = 0; idx <= length; ++idx ) {

		LSA_UINT8 ch;

		if( idx < length ) {
			ch = name[idx];
		}
		else {
			ch = '.'; /* fake a dot at the end */
		}

		/***/

		if( ch == '.' ) {

			LSA_UINT16 len = CM_CAST_U16 (idx - lbl);

			if( len < 1 || len > 63 ) {
				return LSA_FALSE; /* the label is too short or too long */
			}

			CM_ASSERT(idx > 0);

			if( name[idx - 1] == '-' ) {
				return LSA_FALSE; /* the label ends with '-' */
			}

			if( len > 3 ) {

				looks_like_ipaddr = LSA_FALSE; /* the label has more than 3 characters */
			}

			if( check_if_is_alias_name ) {

				if( nr_of_labels == 0 ) {

					if( ! cm_is_valid_port_name(len, &name[lbl]) ) {
						return LSA_FALSE; /* the first (leftmost) label does not match a port-name */
					}
				}
				else if( nr_of_labels == 1 ) {

					if( cm_is_valid_port_name(len, &name[lbl]) ) {
						return LSA_FALSE; /* the second label (i.e. first label of station-name) matches a port-name */
					}
				}

				/* see check for "at least 2 labels" below */
			}
			else { /* station-name */

				if( nr_of_labels == 0 ) {

					if( cm_is_valid_port_name(len, &name[lbl]) ) {
						return LSA_FALSE; /* the first (leftmost) label matches a port-name */
					}
				}
			}

			nr_of_labels += 1;

			lbl = CM_CAST_U16 (idx + 1); /* start of the next label */

			continue;
		}

		if( ch >= '0' && ch <= '9' ) {
			continue;
		}

		looks_like_ipaddr = LSA_FALSE; /* the label contains not only digits */

		if( ch >= 'a' && ch <= 'z' ) {
			continue;
		}

		if( ch >= 'A' && ch <= 'Z' ) {
			/* for backwards-compatibility: allow upper-case here, convert in cm_station_name_copy() */
			continue;
		}

		if( idx == lbl ) {
			return LSA_FALSE; /* the label does not start with a letter or digit */
		}

		if( ch == '-' ) {
			continue;
		}

		return LSA_FALSE; /* the label contains an illegal char */
	}

	if( looks_like_ipaddr && nr_of_labels == 4 ) {

		return LSA_FALSE; /* station-name looks like an ip-addr */
	}

	if( check_if_is_alias_name && nr_of_labels < 2 ) {

		return LSA_FALSE; /* alias-names have at least 2 labels */
	}

	return LSA_TRUE; /* ok */
}

/*----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_is_valid_station_name(
	LSA_UINT16 length,
	CM_COMMON_MEM_U8_PTR_TYPE name
) {
	return cm_is_valid_name (length, name, LSA_FALSE);
}

/*----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_is_valid_alias_name(
	LSA_UINT16 length,
	CM_COMMON_MEM_U8_PTR_TYPE name
) {

	return cm_is_valid_name (length, name, LSA_TRUE);
}

/*------------------------------------------------------------------------------
// copy a station-name (and convert to lower-case)
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_station_name_copy (
	CM_COMMON_MEM_U8_PTR_TYPE to_station_name,
	CM_COMMON_MEM_U8_PTR_TYPE from_station_name,
	LSA_UINT16  station_name_length
) {
	LSA_UINT16  i;

	/* for compatibility to CM V2 we have to accept (and convert) upper-case names */

	for (i = 0; i < station_name_length; ++i) {

		LSA_UINT8 ch = from_station_name[i];

		if ('A' <= ch && ch <= 'Z') {
			ch = CM_CAST_U8 (ch - 'A' + 'a');
		}

		to_station_name[i] = ch;
	}
}


/*------------------------------------------------------------------------------
// check if "is IOSAR with device-access"
//----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_is_iosar_with_device_access(
	CM_AR_GRAPH_PTR_TYPE ar_graph
) {

	if( ar_graph->ar.ar_type == CM_AR_TYPE_SUPERVISOR ) {

		if( (ar_graph->ar.ar_properties & CM_AR_PROP_DEVICE_ACCESS) != 0 ) {

			return LSA_TRUE; /* Special case: "IOSAR with ARProperties.DeviceAccess=1" (a.k.a. easy-supervisor) */
		}
	}

	return LSA_FALSE;
}

/*------------------------------------------------------------------------------
// check if frame-id is within normative range
//----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_is_frame_id_within_normative_range(
	LSA_BOOL				is_consumer,
	LSA_BOOL				is_multicast,
	LSA_UINT32				rt_class,
	LSA_UINT16				frame_id
) {
	CM_LOG_FCT ("cm_is_frame_id_within_normative_range")

	/*
	AP00423463 (unicast / multicast division is wrong)
	change was somewhere between 65C_3xx_NP_PROFINET_IO_v20_i282.doc and WD61158-610_v21_i081.doc

	AP00710306 (V4.0i80.10: range 3 changed and split into more ranges)

	IEC-Update FDIS 61158-6-10_v23_FINAL__TC2WG11_TO_PNO_CO__Rev13.doc (range 6), V5.2i1.28

	IOCRProperties.RTClass
	0x01  Use FrameID range 7 for the IOCRs (RTClass == 0x02 shall be used as substitution by the engineering tool)
	0x02  Use FrameID range 6 for the IOCRs
	0x03  Use FrameID range 3 for the IOCRs
	0x04  Use FrameID range 7 for the IOCRs

	FrameID range 3
	0x0100 - 0x0FFF  Dedicated to RT_CLASS_3 (RED) unicast and multicast
	0x1000 - 0x47FF  Reserved

	FrameID range 4 and 5
	0x4800 - 0x7FFF  Reserved

	FrameID range 6
	0x8000 - 0xBBFF  Dedicated to RT_CLASS_1 (GREEN) unicast   (and RT_CLASS_2 (ORANGE) if ARProperties.StartupMode == Legacy)
	0xBC00 - 0xBFFF  Dedicated to RT_CLASS_1 (GREEN) multicast (and RT_CLASS_2 (ORANGE) if ARProperties.StartupMode == Legacy)

	FrameID range 7
	0xC000 - 0xF7FF  Dedicated to RT_CLASS_UDP unicast   (legacy: RT_CLASS_1)
	0xF800 - 0xFBFF  Dedicated to RT_CLASS_UDP multicast (legacy: RT_CLASS_1)
	*/

	switch (rt_class) {

	case CM_IOCR_PROP_RT_CLASS_1:
	case CM_IOCR_PROP_RT_CLASS_1_UDP:

		if( is_consumer ) {
			/* don't care */
		}
		else {

			if( is_multicast ) {

				if( ! (
					frame_id >= 0xF800
					&& frame_id <= 0xFBFF
				) ) {
					return LSA_FALSE; /* class 1, multicast, frame-id out of range */
				}
			}
			else {

				if( ! (
					frame_id >= 0xC000
					&& frame_id <= 0xF7FF
				) ) {
					return LSA_FALSE; /* class 1, unicast, frame-id out of range */
				}
			}
		}
		break;

	case CM_IOCR_PROP_RT_CLASS_2:

		if( is_consumer ) {
			/* don't care */
		}
		else {

			if( is_multicast ) {

				if( ! (
					frame_id >= 0xBC00
					&& frame_id <= 0xBFFF
				) ) {
					return LSA_FALSE; /* class 2, multicast, frame-id out of range */
				}
			}
			else {

				if( ! (
					frame_id >= 0x8000
					&& frame_id <= 0xBBFF
				) ) {
					return LSA_FALSE; /* class 2, unicast, frame-id out of range */
				}
			}
		}
		break;

	case CM_IOCR_PROP_RT_CLASS_3:
		if( ! (
			frame_id >= 0x0100
			&& frame_id <= 0x0FFF
		) ) {
			return LSA_FALSE; /* class 3, unicast+multicast, frame-id out of range */
		}
		break;

	default:
		return LSA_FALSE; /* unreachable */
	}

	return LSA_TRUE; /* ok */
}

/*------------------------------------------------------------------------------
// allocate a new ar
//----------------------------------------------------------------------------*/

CM_AR_GRAPH_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_alloc(
	CM_CHANNEL_PTR_TYPE		channel,
	LSA_UINT16				device_nr,
	LSA_UINT16				ar_nr
)
{
	CM_LOG_FCT ("cm_ar_graph_alloc")
	CM_AR_GRAPH_PTR_TYPE  ar_graph;

	if (ar_nr > 255) { /* note: cm-sv uses ar_nr 0 here (assignment is done later) */
		CM_AR_TRACE_01 (channel->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "wrong ar_nr(%u), allowed 0..255", ar_nr
			);
		/* see CM_RPC_SET_USER_ID_HANDLE_DEV_AR() and EDD/GSY/NARE/... */
		return LSA_NULL;
	}

	/***/

	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (& ar_graph), sizeof (*ar_graph));

	if (is_null (ar_graph)) {
		CM_AR_TRACE_01(channel->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "no local-mem for ar_graph, size(%u)", sizeof(*ar_graph)
			);
		return LSA_NULL;
	}

	/***/

	ar_graph->link.Blink = LSA_NULL;
	ar_graph->link.Flink = LSA_NULL;

	/***/

	ar_graph->trace_idx = channel->trace_idx; /* a copy to avoid the indirection */

	ar_graph->device_nr   = device_nr;
	ar_graph->ar_nr       = ar_nr;
	ar_graph->session_key = 0; /* unknown */

	/*
	 * init so that freeing will work
	 */

	ar_graph->ar_com.channel = channel; /* set backlink */

	ar_graph->ar_com.path_type = channel->detailptr->path_type;

	ar_graph->ar_com.state = CM_AR_COM_INIT;

	ar_graph->ar_com.ar_send_clock   = 0; /* unknown (case: easy-supervisor) */
	ar_graph->ar_com.is_power_of_two = LSA_TRUE; /* don't care */
	ar_graph->ar_com.has_udp = LSA_FALSE; /* see cm_ar_graph_insert_io_cr() and cm_ar_graph_insert_alarm_cr() */
	ar_graph->ar_com.autostop_cons_id = ACP_INVALID_ACP_ID;

	cm_ar_com_init (ar_graph);

	cm_ar_com_timer_init(ar_graph);

	ar_graph->ar_com.mcr_busy_cnt = 0;

	ar_graph->ar_com.do_reset = LSA_FALSE;

	if (! cm_arcq_init (ar_graph))
	{
		LSA_UINT16  rc;
		CM_AR_TRACE_00 (ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR, "no mem for acp-rqb");
		CM_FREE_LOCAL_MEM (&rc, ar_graph);
		CM_ASSERT (rc == LSA_RET_OK);
		return LSA_NULL;
	}

	/***/

	ar_graph->ar.cmi_station_name_length = 0;
	ar_graph->ar.cmi_station_name = LSA_NULL;

	/***/

	ar_graph->alarm_cr	= LSA_NULL;

	CmListInitialize (& ar_graph->io_cr_list);
	CmListInitialize (& ar_graph->ap_list);
	CmListInitialize (& ar_graph->mcr_list);

	ar_graph->ir_info = LSA_NULL;

	ar_graph->sr_info = LSA_NULL;

	CmListInitialize (& ar_graph->arvendor_list);

	/***/

	{
	LSA_BOOL ok;

	switch( ar_graph->ar_com.path_type ) {
#if CM_CFG_MAX_CLIENTS
	case CM_PATH_TYPE_CLIENT:
		ok = cm_cl_ar_init (ar_graph);
		break;
#endif
#if CM_CFG_MAX_MULTICAST_CHANNELS
	case CM_PATH_TYPE_MULTICAST:
		cm_mc_ar_init (ar_graph);
		ok = LSA_TRUE;
		break;
#endif
#if CM_CFG_MAX_SERVERS
	case CM_PATH_TYPE_SERVER:
		ok = LSA_TRUE; /* see cm_sv_argr_alloc_and_init() */
		break;
#endif
	default:
		ok = LSA_FALSE; /* unreachable */
		break;
	}

	if( ! ok ) {
		LSA_UINT16 ret_val;
		CM_FREE_LOCAL_MEM (& ret_val, ar_graph);
		CM_ASSERT(ret_val == LSA_RET_OK);
		LSA_UNUSED_ARG(ret_val);
		return LSA_NULL;
	}
	}

	/***/

	CM_AR_TRACE_03(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "new ar_graph(%08X): dev(%u) ar(%u)"
		, ar_graph, ar_graph->device_nr, ar_graph->ar_nr
		);

	return ar_graph;
}


/*------------------------------------------------------------------------------
// insert a new ar into an existing ar_graph
//----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_io_ar (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT32				host_ip, /* network byte order */

	LSA_UINT16				ar_type,
	const CLRPC_UUID_TYPE CM_COMMON_MEM_ATTR * const ar_uuid,

	CM_COMMON_MEM_U8_PTR_TYPE
							cmi_mac,

	CLRPC_UUID_PTR_TYPE		cmi_obj_uuid,

	LSA_UINT32				ar_properties,
	LSA_UINT16				cmi_timeout100ms,
	LSA_UINT16				cmi_udp_port, /* network byte order */

	LSA_UINT16/*!*/			cmi_station_name_length,

	CM_COMMON_MEM_U8_PTR_TYPE
							cmi_station_name
) {
	CM_LOG_FCT ("cm_ar_graph_insert_io_ar")

	CM_ASSERT(is_not_null(ar_graph));
	CM_ASSERT(is_not_null((LSA_VOID_PTR_TYPE)ar_uuid));
	CM_ASSERT(is_not_null(cmi_mac));
	CM_ASSERT(is_not_null(cmi_obj_uuid));

	CM_ASSERT(ar_graph->ar_com.state == CM_AR_COM_INIT); /* see caller */

	CM_AR_TRACE_12(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "ar_type(%u) ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, ar_type
		, ar_uuid->time_low, ar_uuid->time_mid, ar_uuid->time_hi_and_version
		, ar_uuid->clock_seq_hi_and_reserved, ar_uuid->clock_seq_low
		, ar_uuid->node[0], ar_uuid->node[1], ar_uuid->node[2]
		, ar_uuid->node[3], ar_uuid->node[4], ar_uuid->node[5]
		);

	/***/

	/*
	host_ip = not checked (ip-stack makes sure that it is a valid unicast address)
	*/

	/***/

	switch( ar_type ) {
	case CM_AR_TYPE_SINGLE:
	case CM_AR_TYPE_SUPERVISOR:
#if CM_CFG_RT_CLASS_3_SUPPORT_ON
	case CM_AR_TYPE_SINGLE_RTC3:
#endif
	case CM_AR_TYPE_SINGLE_SYSRED:
		break;
	default:
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ar_type(%u) out of range or not CM_CFG_RT_CLASS_3_SUPPORT_ON"
			, ar_type
			);
		return 4; /* ar-type out of range */
	}

	/***/

	if( ar_graph->ar_com.path_type == CM_PATH_TYPE_MULTICAST ) {
		CM_ASSERT(cm_uuid_is_nil(ar_uuid)); /* see caller */
	}
	else if( cm_uuid_is_nil(ar_uuid) ) {
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "connect with NIL ar-uuid"
			);
		return 5; /* NIL ARUUID */
	}

	/***/

	if( (cmi_mac[0] & 0x01) != 0 ) {
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "cmi-mac not unicast"
			);
		return 7; /* cmi-mac not unicast */
	}

	/***/

	/*
	 * CMInitiatorObjectUUID ::=
	 *		PROFINETIOConstantValue, InstanceHigh, InstanceLow, DeviceIdentNumber
	 *
	 * The value of the field PROFINETIOConstantValue shall be DEA00000-6C97-11D1-8271.
	 *
	 * see cm_rpc_make_object_uuid()
	 */

	if( cmi_obj_uuid->time_low					!= 0xDEA00000
	||	cmi_obj_uuid->time_mid					!= 0x6C97
	||	cmi_obj_uuid->time_hi_and_version		!= 0x11D1
	||	cmi_obj_uuid->clock_seq_hi_and_reserved	!= 0x82
	||	cmi_obj_uuid->clock_seq_low				!= 0x71
	) {
		CM_AR_TRACE_05 (ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "wrong format of cmi_obj_uuid(%u-%u-%u-%u%u), see PROFINETIOConstantValue"
			, cmi_obj_uuid->time_low
			, cmi_obj_uuid->time_mid
			, cmi_obj_uuid->time_hi_and_version
			, cmi_obj_uuid->clock_seq_hi_and_reserved
			, cmi_obj_uuid->clock_seq_low
			);
		return 8;
	}

	/***/

	if( (ar_properties & CM_AR_PROP_STATE_MASK) != CM_AR_PROP_STATE_ACTIVE ) {

		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ar_properties.state not 'active'"
			);
		return 9; /* unknown 'state' */
	}

	if( (ar_properties & CM_AR_PROP_SUPERVISOR_TAKEOVER_ALLOWED) != 0 ) {

		/* all values valid, only checked by server */
	}

	if( (ar_properties & CM_AR_PROP_PRM_SERVER_CMINITIATOR) == 0 ) {

		/* all values valid, only checked by server */
	}

	if( (ar_properties & CM_AR_PROP_UNCHECKED_5_7) != 0 ) {

		/* ok, not checked by the receiver */
	}

	if( (ar_properties & CM_AR_PROP_DEVICE_ACCESS) != 0 ) {

		/* "true" allowed only for IOS-AR */

		if( ar_type != CM_AR_TYPE_SUPERVISOR ) {
			CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "ar_properties.device_access but not IOSAR"
				);
			return 9; /* device-access but not IOSAR */
		}
	}

	if( (ar_properties & CM_AR_PROP_COMPANION_AR_MASK) != CM_AR_PROP_COMPANION_AR_SINGLE ) {
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ar_properties.CompanionAR not 'AR single'"
			);
		return 9; /* unknown 'CompanionAR' */
	}

	if( (ar_properties & CM_AR_PROP_ACKNOWLEDGE_COMPANION_AR) != 0 ) {

		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ar_properties.AcknowledgeCompanionAR not 'no ack'"
			);
		return 9; /* unknown 'AcknowledgeCompanionAR' */
	}

	if( (ar_properties & CM_AR_PROP_UNCHECKED_12_23) != 0 ) {

		/* ok, not checked by the receiver */
	}

	if( (ar_properties & CM_AR_PROP_RESERVED_24_29) != 0 ) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ar_properties(0x%x) 'reserved_3' not zero"
			, ar_properties
			);
		return 9; /* 'reserved' not zero */
	}

	if( (ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0 ) {

		/* functionality introduced with V5.0 (IEC61158 V2.3) */

		if( (ar_graph->ar.ar_properties & CM_AR_PROP_DEVICE_ACCESS) != 0 ) {

			/* supported for easy-supervisor ("advanced" is just the same as "legacy") */
		}
		else {

			/* valid for all ar_types */
		}
	}
	else { /* legacy */

		if( ar_type == CM_AR_TYPE_SINGLE_SYSRED ) {

			CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "ar_properties.'Legacy' and ar_type(SINGLE_SYSRED) not supported"
				);
			return 9; /* ar_type SINGLE_SYSRED not supported */
		}
	}

	if( (ar_properties & CM_AR_PROP_PULL_MODULE_ALARM_ALLOWED) != 0 ) {

		/* ok, functionality introduced with V4.1 (AP00517543) */
	}

	/***/

	{
	LSA_UINT  cmi_min;
	if( (ar_properties & CM_AR_PROP_DEVICE_ACCESS) != 0
	||	(ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0 )
	{
		cmi_min = 100;
	}
	else
	{
		cmi_min = 1;
	}
	if( ! (cmi_min <= cmi_timeout100ms && cmi_timeout100ms <= 1000) )
	{
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "cmi_timeout out-of-range"
			);
		return 10; /* cmi_timeout out-of-range */
	}
	}

	/***/

	/* cmi_udp_port
	 * ... checked later in cm_ar_graph_check_io_ar()
	 */

	/***/

	CM_ASSERT(ar_graph->ar.cmi_station_name_length == 0);
	CM_ASSERT(is_null(ar_graph->ar.cmi_station_name));

	if( is_null(cmi_station_name) ) {

		CM_ASSERT(cmi_station_name_length == 0);
		CM_ASSERT(ar_graph->ar_com.path_type == CM_PATH_TYPE_CLIENT || ar_graph->ar_com.path_type == CM_PATH_TYPE_MULTICAST);
	}
	else {

		if( ! (
			cmi_station_name_length >= 1
			&& cmi_station_name_length <= 240
		) ) {
			CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "invalid cmi_station_name_length(%u)"
				, cmi_station_name_length
				);
			return 12; /* invalid cmi_station_name_length */
		}

		if( ! cm_is_valid_station_name(cmi_station_name_length, cmi_station_name) ) {
			CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "invalid cmi-station-name"
				);
			return 13; /* invalid cmi-station-name */
		}

		if( ar_graph->ar_com.path_type == CM_PATH_TYPE_SERVER ) {

			CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&ar_graph->ar.cmi_station_name), cmi_station_name_length);

			if( is_null(ar_graph->ar.cmi_station_name) ) {
				CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
					, "no local-mem, size(%u)", cmi_station_name_length
					);
				return 255; /* no mem (for ioar::cmi-station-name) */
			}

			/* assign now, see cm_ar_graph_free() */

			ar_graph->ar.cmi_station_name_length = CM_CAST_U8(cmi_station_name_length); /* see cm_is_valid_station_name() */

			cm_station_name_copy (ar_graph->ar.cmi_station_name, cmi_station_name, cmi_station_name_length);
		}
		else {
			CM_ASSERT(ar_graph->ar_com.path_type == CM_PATH_TYPE_CLIENT);
		}
	}

	/***/

	ar_graph->ar_com.state = CM_AR_COM_IDLE;

	ar_graph->ar.host_ip			= host_ip; /* network byte order */

	ar_graph->ar.ar_type			= ar_type;
	ar_graph->ar.ar_uuid			= *ar_uuid;

	CM_MEMCPY(ar_graph->ar.eth_mac.mac, cmi_mac, sizeof(ar_graph->ar.eth_mac.mac));

	ar_graph->ar.cmi_obj_uuid		= *cmi_obj_uuid;
	ar_graph->ar.ar_properties		= ar_properties;
	ar_graph->ar.cmi_timeout100ms	= cmi_timeout100ms;
	ar_graph->ar.cmi_udp_port		= cmi_udp_port; /* network byte order */
	ar_graph->ar.cmr_udp_port		= 0; /* see cm_sv_argr_build() and cm_ar_graph_update_io_ar() */

	/*
	ar_graph->ar.cmi_station_name_length = see above
	ar_graph->ar.cmi_station_name        = see above
	*/

	/***/

	CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "made io_ar(%08X)"
		, ar_graph
		);

	return 0; /* ok */
}


/*------------------------------------------------------------------------------
// insert a new IRInfoBlock into an existing ar_graph
//----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_ir_info (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	CLRPC_UUID_PTR_TYPE		ir_data_uuid,
	LSA_UINT16				alloc_more /* for CM-SV, else 0 */
) {
	CM_LOG_FCT ("cm_ar_graph_insert_ir_info")
	CM_AR_GRAPH_IR_INFO_PTR_TYPE irInfoBlockPtr;

	/* note: CM_AR_PROP_STARTUP_MODE_ADVANCED is checked in caller */

	if (is_not_null (ar_graph->ir_info))
	{
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "more than one ir info block"
			);
		CM_FATAL(); /* a bug in caller */
	}

	if (cm_uuid_is_nil (ir_data_uuid))
	{
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "ir_data_uuid shall not be NIL"
			);
		return 5; /* NIL IRDataUUID */
	}

	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&irInfoBlockPtr), sizeof (*irInfoBlockPtr) + alloc_more);

	if (is_null (irInfoBlockPtr))
	{
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "no local-mem, size(%u)", sizeof (*irInfoBlockPtr)
			);

		return 255; /* no mem (for ir_info) */
	}

	irInfoBlockPtr->ir_data_uuid = *ir_data_uuid;

	CmListInitialize (&(irInfoBlockPtr->ir_info_sub_list));

	ar_graph->ir_info = irInfoBlockPtr;

	return 0;
}

/*------------------------------------------------------------------------------
// insert a new subframe info
//----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_ir_info_sub (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT16				iocr_ref,
	LSA_UINT16				subframe_offset,
	LSA_UINT32				subframe_data
) {
	CM_LOG_FCT ("cm_ar_graph_insert_ir_info_sub")
	CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE ir_info_sub;

	if (is_null (ar_graph) || is_null (ar_graph->ir_info))
	{
		CM_AR_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "no ir-info block");
		CM_FATAL(); /* a bug in caller */
	}

	ir_info_sub = cm_ar_graph_find_ir_info_sub (ar_graph, iocr_ref);
	if (is_not_null (ir_info_sub))
	{
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "iocr_ref(%u) defined twice", iocr_ref);
		return 8;
	}

	if (! (6 <= subframe_offset && subframe_offset <= 0x059B))
	{
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "subframe_offset(%u) out of range", subframe_offset);
		return 9;
	}

	if ((subframe_data & 0x7F) == 0
	/* ||	(subframe_data & 0x80) != 0 reserved and do not check */
	||	(subframe_data & 0xFF00) == 0
	/* ||  (subframe_data & 0xFFFF0000) != 0 reserved and do not check */)
	{
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "subframe_data(0x%x) out of range", subframe_data);
		return 10;
	}

	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (&ir_info_sub), sizeof (*ir_info_sub));

	if (is_null (ir_info_sub))
	{
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "no local-mem, size(%u)", sizeof (*ir_info_sub));
		return 255; /* no mem (for ir_info) */
	}

	ir_info_sub->iocr_ref			= iocr_ref;
	ir_info_sub->subframe_offset	= subframe_offset;
	ir_info_sub->subframe_data		= subframe_data;

	CmListInsertTail (& ar_graph->ir_info->ir_info_sub_list, & ir_info_sub->link);

	return 0;
}

/*------------------------------------------------------------------------------
// update an io_ar (client side)
//----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_graph_update_io_ar (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	CM_COMMON_MEM_U8_PTR_TYPE
							cmr_mac,

	LSA_UINT16				cmr_udp_port /* network byte order */
) {
	CM_LOG_FCT ("cm_ar_graph_update_io_ar")
	CM_CHANNEL_PTR_TYPE  channel = ar_graph->ar_com.channel;

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_CLIENT);

	CM_AR_TRACE_09(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "dev(%u) ar(%u) update ar_graph mac(%02x-%02x-%02x-%02x-%02x-%02x) udp_port(%u)"
		, ar_graph->device_nr, ar_graph->ar_nr
		, cmr_mac[0], cmr_mac[1], cmr_mac[2]
		, cmr_mac[3], cmr_mac[4], cmr_mac[5]
		, CM_NTOH16(cmr_udp_port)
		);

	if( (cmr_mac[0] & 0x01) != 0 ) {
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "cmr-mac not unicast"
			);
		return LSA_FALSE; /* cmr-mac not unicast */
	}

	if( ar_graph->ar_com.has_udp )
	{
		if( cmr_udp_port == 0 )
		{
			CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "cmr_udp_port(0x%x) is zero but but have udp-transport"
				, CM_NTOH16(cmr_udp_port)
				);
			return LSA_FALSE; /* cmr_udp_port not as expected */
		}

		if( cmr_udp_port != channel->lower.info.acp.udp_port )
		{
			/* note: cmr_udp_port must be cmi_udp_port. see ACP_OPC_PROVIDER_ADD and EDD_SRV_SRT_PROVIDER_ADD */

			CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "cmr_udp_port(0x%x) not edd_port(%u)"
				, CM_NTOH16(cmr_udp_port), channel->lower.info.acp.udp_port
				);
			return LSA_FALSE; /* cmr_udp_port not as expected */
		}
	}
	else /* no udp-transport (AP00583478) */
	{
		if( cmr_udp_port == 0 )
		{
			/* zero is ok too for compatibility reasons (see AP00583478) */
		}
		else if( cmr_udp_port != CM_HTON16(0x8892) )
		{
			CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "cmr_udp_port(0x%x) not 0x8892"
				, CM_NTOH16(cmr_udp_port)
				);
			return LSA_FALSE; /* cmr_udp_port not as expected */
		}
	}

	/***/

	CM_MEMCPY(ar_graph->ar.eth_mac.mac, cmr_mac, sizeof(ar_graph->ar.eth_mac.mac));

	ar_graph->ar.cmr_udp_port = cmr_udp_port; /* network byte order */

	return LSA_TRUE; /* ok */
}

#endif

/*------------------------------------------------------------------------------
// check an io_ar (server side)
//----------------------------------------------------------------------------*/

#if CM_CFG_MAX_SERVERS

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_graph_check_io_ar(
	CM_AR_GRAPH_PTR_TYPE ar_graph
) {
	CM_LOG_FCT("cm_ar_graph_check_io_ar")
	LSA_UINT16 edd_port = ar_graph->ar_com.channel->lower.info.acp.udp_port;

	/*
	 * compare to cm_ar_graph_update_io_ar()
	 */

	if( ar_graph->ar_com.has_udp ) {

		if( ar_graph->ar.cmi_udp_port == 0 ) {

			CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "cmi_udp_port(0x%x) is zero but have udp-transport"
				, CM_NTOH16(ar_graph->ar.cmi_udp_port)
				);
			return LSA_FALSE; /* cmi_udp_port not as expected */
		}

		if( ar_graph->ar.cmi_udp_port != edd_port ) {

			/* note: cmr_udp_port must be cmi_udp_port. see ACP_OPC_PROVIDER_ADD and EDD_SRV_SRT_PROVIDER_ADD */

			CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "cmi_udp_port(0x%x) not edd_port(%u)"
				, CM_NTOH16(ar_graph->ar.cmi_udp_port), CM_NTOH16(edd_port)
				);
			return LSA_FALSE; /* cmi_udp_port not as expected */
		}
	}
	else { /* no udp-transport (AP00583478) */

		if( ar_graph->ar.cmi_udp_port == 0 ) {

			/* zero is ok too for compatibility reasons (see AP00583478) */
		}
		else if( ar_graph->ar.cmi_udp_port != CM_HTON16(0x8892) ) {

			CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "cmi_udp_port(0x%x) not 0x8892"
				, CM_NTOH16(ar_graph->ar.cmi_udp_port)
				);
			return LSA_FALSE; /* cmi_udp_port not as expected */
		}
	}

	/***/

	return LSA_TRUE; /* ok */
}

#endif

/*------------------------------------------------------------------------------
// insert a new io cr into an existing ar_graph
//----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_graph_reduction_ratio_range_ok (
	LSA_UINT16  reduction_ratio,
	LSA_BOOL  is_power_of_two,
	LSA_UINT32  rt_class
) {
	CM_LOG_FCT("cm_ar_graph_reduction_ratio_range_ok")

	switch( reduction_ratio ) {
	case 1:
	case 2:
	case 4:
	case 8:
	case 16:
		/* mandatory for RTC1/2 and RTC3, optional for UDP */
		break;

	case 32:
	case 64:
		/* mandatory for RTC1/2, optional for UDP */
	case 128:
	case 256:
	case 512:
		/* mandatory for RTC1/2 and UDP */
		if (rt_class == CM_IOCR_PROP_RT_CLASS_3) {
			CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "reduction_ratio(%u) not supported for class 3", reduction_ratio
				);
			return LSA_FALSE;
		}
		break;

	case 1024:
	case 2048:
	case 4096:
	case 8192:
	case 16384:
		/* mandatory for UDP */
		if( rt_class != CM_IOCR_PROP_RT_CLASS_1_UDP ) {
			CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "reduction_ratio(%u) not supported for class 1,2,3", reduction_ratio
				);
			return LSA_FALSE;
		}

		LSA_UNUSED_ARG ( is_power_of_two);  /* AP00455992 invalidated by AP01374252 */
		break;

	default:
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "non-power-of-two reduction_ratio(%u) not supported", reduction_ratio
			);
		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/*----------------------------------------------------------------------------*/

static LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_list_cmp__iocr__iocr_ref (
	CM_LIST_ENTRY_PTR_TYPE  ListEntry,
	CM_LIST_ENTRY_PTR_TYPE  NewEntry
	)
{
	CM_AR_GRAPH_IO_CR_PTR_TYPE  p = (CM_AR_GRAPH_IO_CR_PTR_TYPE) ListEntry;
	CM_AR_GRAPH_IO_CR_PTR_TYPE  e = (CM_AR_GRAPH_IO_CR_PTR_TYPE) NewEntry;

	return p->iocr_ref < e->iocr_ref;
}

/*----------------------------------------------------------------------------*/

static  LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_graph_wd_dh_exceed_limit (
	LSA_UINT32  rt_class,
	LSA_UINT16  reduction_ratio,
	LSA_UINT16  send_clock,
	LSA_UINT16  check_this_wd_or_dh_factor
	)
{
	LSA_UINT64  wd_dh_time;
	LSA_UINT32  limit; /* note: changed from U64 to U32 because of a GNU-PPC compiler bug */

	/*
	** xx_time = xx_factor * reduction_ratio * send_clock * 31.25;
	**     max =    0x1E00 *           16384 *        128 = 0x3_C000_0000 --> 34(!) bits
	**
	** limitRT  =  1.92 sec / 31.25 us =   61440 =   0xF000
	** limitUDP = 61.44 sec / 31.25 us = 1966080 = 0x1E0000
	**
	*/

	switch (rt_class)
	{
	case CM_IOCR_PROP_RT_CLASS_1:
	case CM_IOCR_PROP_RT_CLASS_2:
	case CM_IOCR_PROP_RT_CLASS_3:
		limit = 61440;
		break;

	case CM_IOCR_PROP_RT_CLASS_1_UDP:
		limit = 1966080;
		break;

	default:
		limit = 0;
		CM_FATAL();
		break;
	}

	wd_dh_time = check_this_wd_or_dh_factor;
	wd_dh_time *= reduction_ratio;
	wd_dh_time *= send_clock;

	if (wd_dh_time > limit) {
		return LSA_TRUE; /* out of range */
	}
	return LSA_FALSE;
}


/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_io_cr (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT16			iocr_type,
	LSA_UINT16			iocr_ref,
	LSA_UINT16			ethertype,
	LSA_UINT32			iocr_properties,

	LSA_UINT16			data_length,
	LSA_UINT16			frame_id,

	LSA_UINT16			send_clock,
	LSA_UINT16			reduction_ratio,
	LSA_UINT16			phase,
	LSA_UINT32			frame_send_offset,

	LSA_UINT16			watchdog_factor_legacy, /* CM-CL/CM-MC: pass in data_hold_factor, CM-SV: pass in value from PDU (AP01304706) */
	LSA_UINT16			data_hold_factor,

	LSA_UINT16			iocr_tag_header, /* only for provider, don't care for consumer */

	CM_COMMON_MEM_U8_PTR_TYPE
						mac_addr,

	CM_AR_GRAPH_IO_CR_PTR_TYPE CM_LOCAL_MEM_ATTR  *iocr_ptr_ptr /* out */
)
{
	CM_LOG_FCT ("cm_ar_graph_insert_io_cr")
	CM_CHANNEL_PTR_TYPE  channel = ar_graph->ar_com.channel;
	CM_AR_GRAPH_IO_CR_PTR_TYPE  ptr;
	LSA_UINT32  rt_class;
	LSA_BOOL    is_consumer;
	LSA_BOOL    is_multicast;
	LSA_BOOL    is_power_of_two;

	CM_ASSERT (is_not_null (mac_addr));

	/***/

	switch( iocr_type ) {
	case CM_IOCR_TYPE_INPUT:
		switch( ar_graph->ar_com.path_type ) {
		case CM_PATH_TYPE_CLIENT:
			is_consumer = LSA_TRUE;
			break;
		case CM_PATH_TYPE_SERVER:
			is_consumer = LSA_FALSE;
			break;
		default:
			CM_FATAL(); /* a bug */
			is_consumer = 0; /* unreachable */
			break;
		}
		is_multicast = LSA_FALSE;
		break;

	case CM_IOCR_TYPE_OUTPUT:
		switch( ar_graph->ar_com.path_type ) {
		case CM_PATH_TYPE_CLIENT:
			is_consumer = LSA_FALSE;
			break;
		case CM_PATH_TYPE_SERVER:
			is_consumer = LSA_TRUE;
			break;
		default:
			CM_FATAL(); /* a bug */
			is_consumer = 0; /* unreachable */
			break;
		}
		is_multicast = LSA_FALSE;
		break;

	case CM_IOCR_TYPE_MULTICAST_CONSUMER:
		is_consumer  = LSA_TRUE;
		is_multicast = LSA_TRUE;
		break;

	case CM_IOCR_TYPE_MULTICAST_PROVIDER:
		is_consumer  = LSA_FALSE;
		is_multicast = LSA_TRUE;
		break;

	default:
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "unknown iocr_type(%u)", iocr_type
			);
		return 4; /* unknown iocr-type */
	}

	/***/

#if CM_CFG_USE_NARE == 0

#if CM_CFG_SV_AR_HAS_MCONS != 0
# error "inconsistency, see cm_pls.h"
#endif

	if( is_multicast && is_consumer ) { /* m-consumer need NARE */

		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "unsupported iocr_type(%u)", iocr_type
			);
		return 4; /* unsupported iocr-type */
	}

#endif

	/***/

#if CM_CFG_MAX_SERVERS

	if( ar_graph->ar_com.path_type == CM_PATH_TYPE_SERVER ) {

		if( is_multicast ) {

			if( is_consumer ) { /* m-consumer */

				/* note: must check CM_CFG_SV_AR_HAS_MCONS here if m-consumers will be supported */

				CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "unsupported iocr_type(%u) due to AP00774501", iocr_type
					);
				return 4; /* unsupported due to AP00774501 */
			}
			else { /* m-provider */

				CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "unsupported iocr_type(%u) due to AP00774495", iocr_type
					);
				return 4; /* unsupported due to AP00774495 */
			}
		}
	}

#endif

	/***/

	if( ! is_multicast ) {

		CM_AR_GRAPH_IO_CR_PTR_TYPE  ex_iocr;

		for (CmListForeach (ex_iocr, & ar_graph->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE)) {

			if (ex_iocr->iocr_type == iocr_type) {

				CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "iocr_type(%u) exists, only 1 input and 1 output supported", iocr_type
					);
				return 4; /* only 1 input and 1 output supported (AP00774504) */
			}
		}
	}

	/***/

	{
	CM_AR_GRAPH_IO_CR_PTR_TYPE existing;

	existing = cm_ar_graph_find_io_cr(ar_graph, iocr_ref);

	if( is_not_null(existing) ) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "iocr_ref(%u) not unique", iocr_ref
			);
		return 5; /* iocr-ref not unique */
	}
	}

	/***/

	/*
	ethertype = checked below together with properties
	*/

	/***/

	rt_class = (iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK);

	switch (rt_class) {
	case CM_IOCR_PROP_RT_CLASS_1:
	case CM_IOCR_PROP_RT_CLASS_2:
#if CM_CFG_RT_CLASS_3_SUPPORT_ON
	case CM_IOCR_PROP_RT_CLASS_3:
#endif
		if( ethertype != 0x8892 ) {
			CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "wrong ethertype(0x%x)", ethertype
				);
			return 6; /* wrong ethertype */
		}
		break;

	case CM_IOCR_PROP_RT_CLASS_1_UDP:
		ar_graph->ar_com.has_udp = LSA_TRUE; /* AP00583478 */
		if( ethertype != 0x0800 ) {
			CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "wrong ethertype(0x%x)", ethertype
				);
			return 6; /* wrong ethertype */
		}

		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "UDP not yet supported"
			);
		return 7; /* UDP not yet supported (AP01195665) */

	default:
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "unknown rt_class(0x%x) or not CM_CFG_RT_CLASS_3_SUPPORT_ON", rt_class
			);
		return 7; /* unknown rt-class */
	}

	if(
		(ar_graph->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0 
		&& rt_class == CM_IOCR_PROP_RT_CLASS_1
	) {
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "IOCR_PROP_RT_CLASS_1 but AR_PROP_STARTUP_MODE_ADVANCED"
			);
		return 7; /* IOCR_PROP_RT_CLASS_1 but AR_PROP_STARTUP_MODE_ADVANCED */
	}

	if( ar_graph->ar.ar_type == CM_AR_TYPE_SINGLE_RTC3 ) {

		if( rt_class != CM_IOCR_PROP_RT_CLASS_3 ) {
			CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "SINGLE_RTC3 but not RT_CLASS_3"
				);
			return 7; /* SINGLE_RTC3 but not RT_CLASS_3 */
		}
	}
	else {

		if( rt_class == CM_IOCR_PROP_RT_CLASS_3 ) {
			CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "not SINGLE_RTC3 but RT_CLASS_3"
				);
			return 7; /* not SINGLE_RTC3 but RT_CLASS_3 */
		}
	}

	if( (iocr_properties & CM_IOCR_PROP_RESERVED_4_12) != 0 ) {
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "'reserved_1' not zero"
			);
		return 7; /* 'reserved_1' not zero */
	}

	if( (iocr_properties & CM_IOCR_PROP_UNCHECKED_13_23) != 0 ) {

		/* ok, not checked by the receiver (FDIS 61158-6-10_v23_i01) */
	}

	if( (iocr_properties & CM_IOCR_PROP_RESERVED_24_31) != 0 ) {
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "'reserved_3' not zero"
			);
		return 7; /* 'reserved_3' not zero */
	}

	/***/

	switch (rt_class) {

	case CM_IOCR_PROP_RT_CLASS_1:
	case CM_IOCR_PROP_RT_CLASS_2:
	case CM_IOCR_PROP_RT_CLASS_3: /* V4.1i1: minimum length changed */
		if( data_length < CM_PNIO_C_SDU_MIN_LENGTH_RT ) {
			CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "data_length(%u) < CM_PNIO_C_SDU_MIN_LENGTH_RT(%u)"
				, data_length, CM_PNIO_C_SDU_MIN_LENGTH_RT
				);
			return 8; /* data-length too small */
		}
		break;

	case CM_IOCR_PROP_RT_CLASS_1_UDP:
		if( data_length < CM_PNIO_C_SDU_MIN_LENGTH_UDP ) {
			CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "data_length(%u) < CM_PNIO_C_SDU_MIN_LENGTH_UDP(%u)"
				, data_length, CM_PNIO_C_SDU_MIN_LENGTH_UDP
				);
			return 8; /* data-length too small */
		}
		break;

	default:
		CM_FATAL();
		return 8; /* unreachable */
	}

	if( data_length > CM_PNIO_C_SDU_MAX_LENGTH ) {
		CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "data_length(%u) > CM_PNIO_C_SDU_MAX_LENGTH(%u)"
			, data_length, CM_PNIO_C_SDU_MAX_LENGTH
			);
		return 8; /* data-length too long */
	}

	/***/

	if( is_multicast || rt_class == CM_IOCR_PROP_RT_CLASS_3 ) {

		/* frame-id is predetermined */

		if( ! cm_is_frame_id_within_normative_range(is_consumer, is_multicast, rt_class, frame_id) ) {
			return 9; /* frame-id not within the normative range of the class */
		}
	}
	else { /* unicast, not class 3 */

		switch( ar_graph->ar_com.path_type ) {

		case CM_PATH_TYPE_CLIENT:
			/* note: don't check here, see cm_ar_graph_update_io_cr() */
			frame_id = ACP_INVALID_FRAME_ID; /* allocated below (sanity) */
			break;

		case CM_PATH_TYPE_SERVER:
			if( is_consumer ) {
				frame_id = ACP_INVALID_FRAME_ID; /* allocated below (sanity) */
			}
			else {
				if( ! cm_is_frame_id_within_normative_range(is_consumer, is_multicast, rt_class, frame_id) ) {
					return 9; /* frame-id not within the normative range of the class */
				}
			}
			break;

		default:
			CM_FATAL(); /* a bug */
			break;
		}
	}

	/***/

	{
	LSA_BOOL ok;

	ok = cm_edd_send_clock_range_ok(channel, send_clock, &is_power_of_two);

	if( ! ok ) {

		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "send_clock(%u) not supported", send_clock
			);
		return 10; /* unsupported send-clock */
	}
	}

	CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "send_clock(%u) is-power-of-two(%u)"
		, send_clock, is_power_of_two
		);

	/***/

	if( ar_graph->ar_com.ar_send_clock == 0 ) { /* unknown */

		ar_graph->ar_com.ar_send_clock   = send_clock; /* now known */
		ar_graph->ar_com.is_power_of_two = is_power_of_two;
	}

	if( send_clock != ar_graph->ar_com.ar_send_clock ) {

		CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "cr_send_clock(%u) != ar_send_clock(%u)"
			, send_clock, ar_graph->ar_com.ar_send_clock
			);
		return 10; /* send_clocks are different */
	}

	/***/

	if ( ! cm_ar_graph_reduction_ratio_range_ok (reduction_ratio, is_power_of_two, rt_class)) {
		CM_AR_TRACE_03 (ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "reduction_ratio(%u) not supported, is_power_of_two(%u) rt_class(0x%x)"
			, reduction_ratio, is_power_of_two, rt_class
			);
		return 11; /* unsupported reduction-ratio */
	}

	/***/

#if CM_CFG_MAX_SERVERS

	if( ar_graph->ar_com.path_type == CM_PATH_TYPE_SERVER ) {

		LSA_UINT32 min_device_interval = ar_graph->cm.sv.dev->min_device_interval_31250ns;

		if( min_device_interval != 0 ) { /* AP01451871 */

			if( 1UL * send_clock * reduction_ratio < min_device_interval ) {
				CM_AR_TRACE_03 (ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "send_clock(%u) * reduction_ratio(%u) < min_device_interval(%u)"
					, send_clock, reduction_ratio, min_device_interval
					);
				return 11; /* reduction-ratio too small */
			}
		}
	}

#endif

	/***/

	if( ! (
		phase >= 1
		&& phase <= reduction_ratio
	) ) {
		CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "phase(%u) > reduction_ratio(%u)"
			, phase, reduction_ratio
			);
		return 12; /* phase out of range */
	}

	/*
	sequence = ignoring ARCB and Connect-PDU
	*/

	/***/

	if( frame_send_offset != 0xFFFFFFFF ) { /* i.e. not "best effort" */

		LSA_UINT32 send_cycle_time = send_clock * 31250; /* note: send_clock <= 32ms, see cm_edd_send_clock_range_ok() */

		if( frame_send_offset >= send_cycle_time ) {

			CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "frame_send_offset(%u) >= send_cycle_time(%u)", frame_send_offset, send_cycle_time
				);
			return 14; /* frame-send-offset out of range */
		}
	}

	/***/

	if( watchdog_factor_legacy != data_hold_factor	) { /* must be the same as data_hold_factor (AP01304706, PNIO-Spec V2.3, Part 6, Rev 12) */

		CM_AR_TRACE_03 (ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "iocr_ref(%u): watchdog_factor_legacy(%u) != data_hold_factor(%u)"
			, iocr_ref, watchdog_factor_legacy, data_hold_factor
			);
		return 15; /* out of range (AP01304706, PNIO-Spec V2.3, Part 6, Rev 12) */
	}

	/***/

	if( ! (
		data_hold_factor >= 1
		&& data_hold_factor <= 0x1E00
	) ) {
		/* 0x0001-0x0002 optional, 0x0003-0x00FF mandatory, 0x0100-0x1E00 optional, rest reserved */
		return 16; /* out of range */
	}

	if (cm_ar_graph_wd_dh_exceed_limit (rt_class, reduction_ratio, send_clock, data_hold_factor)) {
		CM_AR_TRACE_04 (ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "iocr_ref(%u): DataHoldTime out of range, data_hold_factor(%u) * send_clock(%u) * reduction_ratio(%u) * 31.25"
			, iocr_ref, data_hold_factor, send_clock, reduction_ratio
			);
		return 16; /* out of range */
	}

	/***/

	if( iocr_tag_header != 0xC000 ) { /* TIA 208236 */
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "iocr_tag_header, prio not 6 or bit 12 not 0 or vlan-id not 0"
			);
		return 17; /* vlan-tag */
	}

	/***/

	if( is_multicast ) {

		/* m-cons registers m-mac in mac-table */
		/* m-prov uses m-mac for sending */

		if( (mac_addr[0] & 0x01) == 0 ) {

			CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "multicast-mac has not set multicast-bit"
				);
			return 18; /* wrong multicast-mac */
		}
	}

	/*
	 * alloc a new element
	 */

	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (& ptr), sizeof (*ptr));

	if (is_null (ptr)) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "no local-mem, size(%u)", sizeof (*ptr)
			);
		return 255; /* no mem (for iocr) */
	}

	ptr->cm.backlink = ar_graph;

	ptr->cm.is_consumer  = is_consumer;
	ptr->cm.is_multicast = is_multicast;

	if( is_multicast )
	{
		switch( ar_graph->ar_com.path_type ) {
		case CM_PATH_TYPE_CLIENT:
			ptr->cm.cr_state = CM_AR_CR_ALLOCATED;
			break;
		case CM_PATH_TYPE_MULTICAST:
			ptr->cm.cr_state = CM_AR_CR_IDLE;
			break;
		case CM_PATH_TYPE_SERVER:
			if( is_consumer ) {
				ptr->cm.cr_state = CM_AR_CR_IDLE;
			}
			else {
				if( rt_class == CM_IOCR_PROP_RT_CLASS_3 ) {
#ifdef CM_MESSAGE
#pragma CM_MESSAGE("REVIEW: RTC3-QUERVERKEHR + CM-SV needs review if re-enabled. see AP00774495 too")
#endif /* CM_MESSAGE */
					CM_FATAL();
				}
				else {
					ptr->cm.cr_state = CM_AR_CR_IDLE;
				}
			}
			break;
		default:
			CM_FATAL(); /* a bug */
			break;
		}
	}
	else
	{
		ptr->cm.cr_state = CM_AR_CR_IDLE;
	}

	ptr->cm.is_rir = LSA_FALSE;

	/*
	 * alloc a provider- or consumer-resource
	 */

	if (ptr->cm.cr_state != CM_AR_CR_ALLOCATED) {

		LSA_BOOL  ok;

		ok = cm_res_frame_alloc(channel, is_consumer, is_multicast, rt_class, &frame_id);

		if (! ok) {

			CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "cannot alloc provider- or consumer-resource"
				);

			/***/

			{
			LSA_UINT16 rc;

			CM_FREE_LOCAL_MEM (& rc, ptr);

			CM_ASSERT (rc == LSA_RET_OK);
			LSA_UNUSED_ARG(rc);
			}

			return 254; /* no provider or consumer resource */
		}
	}

	/***/

	ptr->cm.acp_id = ACP_INVALID_ACP_ID;

	/* see cm_cl_ar_graph_set_iocr_edd_data() and cm_sv_ar_connect_update() */
	ptr->cm.edd_properties      = 0;
	ptr->cm.edd_data_offset     = EDD_DATAOFFSET_INVALID;
	ptr->cm.edd_ds_offset       = EDD_DATAOFFSET_INVALID;
	ptr->cm.edd_cr_number       = EDD_CRNUMBER_UNDEFINED;
	ptr->cm.edd_forward_mode    = EDD_FORWARDMODE_UNDEFINED;
	ptr->cm.edd_ct_outdatastart = 0;

	/* see cm_ar_graph_check_iocrs_against_expsubmods() and cm_arcb_calc_iocr_buffer_size() */
	ptr->cm.apdu_status_length = cm_edd_get_apdu_status_length(channel);
	ptr->cm.partial_length     = 0;
	ptr->cm.partial_offset     = 0;
	ptr->cm.subframe_data      = 0;
	ptr->cm.subframe_offset    = 0;

	/***/

	ptr->cm.edd_cw_offset = EDD_DATAOFFSET_INVALID; /* see cm_acp_frame_add_done() */
	ptr->cm.edd_as_offset = EDD_DATAOFFSET_INVALID; /* see cm_acp_frame_add_done() */

	/***/

	ptr->cm.do_stop = LSA_FALSE;

	/***/

	ptr->cm.cons_data_status = 0;

	/***/

#if CM_CFG_USE_NARE
	cm_oneshot_init(&ptr->cm.mc_timer, cm_arsm_mcons_timeout, channel); /* used by m-consumers only */
	ptr->cm.mc_flags = 0;
#endif

	/***/

#if CM_CFG_MAX_SERVERS
	ptr->cm.sv.iocr_user_id.void_ptr = 0; /* sanity, see cm_sv_opc_ar_ownership_response() */
#if CM_CFG_SV_AR_HAS_MCONS
	ptr->cm.sv.data_status_ind = 0;
#endif
#endif

	/***/

	ptr->cm.adpt.send_clock = 0; /* 0..no adaption set */
	ptr->cm.adpt.reduction_ratio = 0;
	ptr->cm.adpt.phase = 0;
	ptr->cm.adpt.sequence = 0; /* search for AP00542516 and AP00619993 (stable sequence of provider frames) */
	ptr->cm.adpt.data_hold_factor = 0;

	/***/

	ptr->iocr_type			= iocr_type;
	ptr->iocr_ref			= iocr_ref;
	ptr->iocr_properties	= iocr_properties;

	ptr->ethertype			= ethertype;

	ptr->data_length		= data_length;

	ptr->frame_id			= frame_id;

	ptr->send_clock			= send_clock;
	ptr->reduction_ratio	= reduction_ratio;
	ptr->phase				= phase;
	ptr->frame_send_offset	= frame_send_offset;

	ptr->data_hold_factor	= data_hold_factor;

	CM_MEMCPY (ptr->multicast_mac.mac, mac_addr, sizeof(ptr->multicast_mac.mac));

	/***/

	CmListInitialize (& ptr->related_ap_list);

	/***/

	CmListInsertSorted (& ar_graph->io_cr_list, & ptr->link, cm_list_cmp__iocr__iocr_ref);

	/***/

	CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "inserted: iocr_ref(%u) fid(%u)"
		, ptr->iocr_ref, ptr->frame_id
		);

	*iocr_ptr_ptr = ptr;

	return 0; /* ok */
}


/*------------------------------------------------------------------------------
// update an io cr
//----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS  ||  CM_CFG_MAX_MULTICAST_CHANNELS

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_ar_graph_set_iocr_edd_data (
	CM_AR_GRAPH_IO_CR_PTR_TYPE  ptr,

	LSA_UINT32		edd_data_offset,
	LSA_UINT32		edd_properties,
	LSA_UINT16		edd_cr_number,
	LSA_UINT8		edd_forward_mode,
	LSA_UINT32		edd_ct_outdatastar,

	LSA_UINT16		partial_length,
	LSA_UINT16		partial_offset,
	LSA_UINT16		apdu_status_length,
	LSA_UINT16		subframe_data,
	LSA_UINT16		subframe_offset
	)
{
	CM_LOG_FCT("cm_cl_ar_graph_set_iocr_edd_data")

	ptr->cm.edd_properties		= edd_properties;
	ptr->cm.edd_data_offset		= edd_data_offset;
	ptr->cm.edd_cr_number		= edd_cr_number;
	ptr->cm.edd_forward_mode	= edd_forward_mode;
	ptr->cm.edd_ct_outdatastart	= edd_ct_outdatastar;

	/* see cm_ar_graph_check_iocrs_against_expsubmods() and cm_arcb_calc_iocr_buffer_size() */
	ptr->cm.apdu_status_length	= apdu_status_length;
	ptr->cm.partial_length		= partial_length;
	ptr->cm.partial_offset		= partial_offset;
	ptr->cm.subframe_data		= subframe_data;
	ptr->cm.subframe_offset		= subframe_offset;

	return 0; /* ok */
}

/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_cl_ar_graph_set_iocr_adaption (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT16			iocr_ref,

	LSA_UINT16			send_clock,
	LSA_UINT16			reduction_ratio,
	LSA_UINT16			phase,
	LSA_UINT32			frame_send_offset,
	LSA_UINT16			data_hold_factor
)
{
	CM_LOG_FCT("cm_cl_ar_graph_set_iocr_adaption")
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;
	LSA_BOOL  is_power_of_two;
	LSA_UINT32  rt_class;

	iocr = cm_ar_graph_find_io_cr (ar_graph, iocr_ref);

	if (is_null(iocr))
	{
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "iocr_ref(%u) not found", iocr_ref
			);
		return 5; /* iocr-ref not found */
	}

	if (iocr->cm.adpt.send_clock != 0)
	{
		CM_AR_TRACE_02 (ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "iocr_ref(%u) already adapted with send_clock(%u)"
			, iocr_ref, iocr->cm.adpt.send_clock
			);
		return 5; /* twice-adaption */
	}

	/* AP01295614: set if zero */
	/* AP01337258: don't mix adpt and iocr */
	if (data_hold_factor == 0)
	{
		data_hold_factor = iocr->data_hold_factor;
	}

	/*
	 * param-check: send_clock, reduction_ratio, phase and frame_send_offset
	 */

	if( ! cm_edd_send_clock_range_ok(ar_graph->ar_com.channel, send_clock, &is_power_of_two) )
	{
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "send_clock(%u) not supported", send_clock
			);
		return 10; /* unsupported send-clock (out of range) */
	}

	rt_class = (iocr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK);

	if ( ! cm_ar_graph_reduction_ratio_range_ok (reduction_ratio, is_power_of_two, rt_class))
	{
		CM_AR_TRACE_03 (ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "reduction_ratio(%u) not supported, is_power_of_two(%u) rt_class(0x%x)"
			, reduction_ratio, is_power_of_two, rt_class
			);
		return 11; /* unsupported reduction-ratio */
	}

	if( ! (1 <= phase && phase <= reduction_ratio) )
	{
		CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "phase(%u) > reduction_ratio(%u)"
			, phase, reduction_ratio
			);
		return 12; /* phase out of range */
	}

	if( frame_send_offset != 0xFFFFFFFF ) /* i.e. not "best effort" */
	{
		LSA_UINT32 send_cycle_time = send_clock * 31250; /* note: send_clock <= 32ms, see cm_edd_send_clock_range_ok() */

		if( frame_send_offset >= send_cycle_time ) {

			CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "frame_send_offset(%u) >= send_cycle_time(%u)", frame_send_offset, send_cycle_time
				);
			return 14; /* frame-send-offset out of range */
		}
	}

	if( ! (1 <= data_hold_factor && data_hold_factor <= 0x1E00))
	{
		/* 0x0001-0x0002 optional, 0x0003-0x00FF mandatory, 0x0100-0x1E00 optional, rest reserved */
		return 16; /* out of range */
	}

	if (cm_ar_graph_wd_dh_exceed_limit (rt_class, reduction_ratio, send_clock, data_hold_factor)) {
		CM_AR_TRACE_03 (ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "DataHoldTime out of range, data_hold_factor(%u) * send_clock(%u) * reduction_ratio(%u) * 31.25"
			, data_hold_factor, send_clock, reduction_ratio
			);
		return 16; /* out of range */
	}

	/*
	 * cross-check: send_clock
	 */

#if 0 /* there is a use-case. this kind of adaption shall be possible */
	if( send_clock > ar_graph->ar_com.ar_send_clock ) {

		CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "adpt.send_clock(%u) > ar_com.ar_send_clock(%u) is counterproductive"
			, send_clock, ar_graph->ar_com.ar_send_clock
			);
		return 10; /* this send-clock is counterproductive */
	}
#endif

	/*
	 * cross-check: phase
	 */

#if 0 /* AP00305489 */
	if( send_clock <= iocr->send_clock ) {

		/*
		 * reducing the adapation must lead to the original phase
		 *
		 * AP00305489: there is a use-case where phases are rearranged (reserved phases)
		 *
		 * <code deleted>
		 */
	}
	else {

		/* don't know how to check the relationship */
	}
#endif

	/*
	 * cross-check: frame_send_offset
	 */

	if( send_clock == iocr->send_clock )
	{
		if( frame_send_offset != iocr->frame_send_offset )
		{
			CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "adpt.frame_send_offset(%u) != iocr.frame_send_offset(%u)"
				, frame_send_offset, iocr->frame_send_offset
				);
			return 14; /* frame_send_offsets do not match */
		}
	}
	else
	{
		/* don't know how to check the relationship */
	}

	/*
	 * cross-check: RTClass3
	 */

	if( rt_class == CM_IOCR_PROP_RT_CLASS_3)
	{
		if( ! (
			send_clock == iocr->send_clock
			&& reduction_ratio == iocr->reduction_ratio
			&& phase == iocr->phase
			&& frame_send_offset == iocr->frame_send_offset
		) ) {
			CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "RTClass3, adaption not supported"
				);
			return 10; /* RTClass3, adaption not supported */
		}
	}

	/* AP01267940
	 * cross-check: UpdateTime(local) <= UpdateTime(remote)
	 */

	if(! (1UL * send_clock * reduction_ratio <= 1UL * iocr->send_clock * iocr->reduction_ratio) )
	{
		CM_AR_TRACE_04(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "! (adpt.sc(%u) * adpt.rr(%u) <= iocr.sc(%u) * iocr.rr(%u))"
			, send_clock, reduction_ratio
			, iocr->send_clock, iocr->reduction_ratio
			);
		return 11; /* reduction-ratio does not match */
	}

	/* AP01267940
	 * cross-check: WatchdogTime(adapted) >= WatchdogTime(original)
	 * note: the RQ says "WatchdogTime" but in the PNIO-Spec this is the "DataHoldTime"
	 */

#if 0 /* AP01542826 */
	/* AP01542826: WatchdogTime of the local adapted WatchdogTime(InputCR) is faulty if it raises the limit 1.92 msec
	 *	To avoid this, the engineering must reduce watchdog_factor, and the check is a "false fail".
	 *
	 *	The other possible check is not implemented: see "WDF(IOD) <= WDF(IOC) <= 2*WDF(IOD)"
	 */
	if (iocr->cm.is_consumer)
	{
		if(! (1UL * send_clock * reduction_ratio * data_hold_factor >= 1UL * iocr->send_clock * iocr->reduction_ratio * iocr->data_hold_factor) )
		{
			CM_AR_TRACE_06(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "! (adpt.sc(%u) * adpt.rr(%u) * adpt.dhf(%u) >= iocr.sc(%u) * iocr.rr(%u) * iocr.dhf(%u))"
				, send_clock, reduction_ratio, data_hold_factor
				, iocr->send_clock, iocr->reduction_ratio, iocr->data_hold_factor
				);
			return 16; /* data-hold-factor does not match */
		}
	}
#endif

	/***/

	iocr->cm.adpt.send_clock		= send_clock;
	iocr->cm.adpt.reduction_ratio	= reduction_ratio;
	iocr->cm.adpt.phase				= phase;
	iocr->cm.adpt.sequence			= 0; /* search for AP00542516 (stable sequence of provider frames) */
	iocr->cm.adpt.data_hold_factor	= data_hold_factor;

	CM_AR_TRACE_08 (ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "adaption from sc(%u)/rr(%u)/ph(%u)/dhf(%u) to sc(%u)/rr(%u)/ph(%u)/dhf(%u)"
		, iocr->send_clock, iocr->reduction_ratio, iocr->phase, iocr->data_hold_factor
		, iocr->cm.adpt.send_clock, iocr->cm.adpt.reduction_ratio, iocr->cm.adpt.phase, iocr->cm.adpt.data_hold_factor
		);
	return 0; /* ok */
}

#endif

/*----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_graph_update_io_cr (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT16			iocr_type,
	LSA_UINT16			iocr_ref,
	LSA_UINT16			frame_id
)
{
	CM_LOG_FCT ("cm_ar_graph_update_io_cr")
	CM_AR_GRAPH_IO_CR_PTR_TYPE iocr;

	/*
	switch( iocr_type ) ...
	... implicitly done in compare of iocr_type below
	*/

	iocr = cm_ar_graph_find_io_cr (ar_graph, iocr_ref);

	if( is_null(iocr) ) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "not iocr with iocr_ref(%u)", iocr_ref
			);
		return LSA_FALSE;
	}

	if( iocr->iocr_type != iocr_type ) {
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "iocr_type mismatch"
			);
		return LSA_FALSE;
	}

	if (! cm_is_frame_id_within_normative_range (iocr->cm.is_consumer, iocr->cm.is_multicast,
		(iocr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK), frame_id))
	{
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "frame_id out of range"
			);
		return LSA_FALSE;
	}

	if( iocr->cm.is_consumer || iocr->cm.is_multicast ) {

		if( iocr->frame_id != frame_id ) {
			CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "frame_id not equal"
				);
			return LSA_FALSE;
		}
	}
	else { /* unicast provider */

		iocr->frame_id = frame_id; /* update */
	}

	return LSA_TRUE;
}

#endif


/*----------------------------------------------------------------------------*/
/*
 * lookup a unicast consumer which is usable for provider-autostop
 */
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_graph_update_autostop_cons_id (
	CM_AR_GRAPH_PTR_TYPE	ar_graph
	)
{
	CM_LOG_FCT ("cm_ar_graph_update_autostop_cons_id")
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;
	LSA_UINT16 autostop_cons_id = ACP_INVALID_ACP_ID;

	for (CmListForeach (iocr, & ar_graph->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
	{
		if (iocr->cm.is_consumer && ! iocr->cm.is_multicast) /* unicast consumer */
		{
			if ( iocr->cm.acp_id != ACP_INVALID_ACP_ID) /* add.cnf(+) */
			{
				autostop_cons_id = iocr->cm.acp_id;
				break;
			}
		}
	}

	if (autostop_cons_id != ACP_INVALID_ACP_ID)
	{
		ar_graph->ar_com.autostop_cons_id = autostop_cons_id;

		CM_AR_TRACE_01 (ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "autostop_cons_id(%u) found", ar_graph->ar_com.autostop_cons_id
			);
	}
	else
	{
		ar_graph->ar_com.autostop_cons_id = ACP_INVALID_ACP_ID;

		CM_AR_TRACE_00 (ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "autostop_cons_id not found"
			);

		/* CM-SV: this is the normal case in the startup of an RTC3-AR */
	}
}

/*----------------------------------------------------------------------------*/

static CM_AR_GRAPH_IOAPI_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_argr_find_iocr_ap (
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	LSA_UINT32		api
	)
{
	CM_LOG_FCT ("cm_argr_find_iocr_ap")
	CM_AR_GRAPH_IOAPI_PTR_TYPE  ptr;

	for (CmListForeach (ptr, & iocr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE)) {
		if (ptr->api == api) {
			return ptr;
		}
	}
	return LSA_NULL;
}

/*----------------------------------------------------------------------------*/

static LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_list_cmp__ioapi__api (
	CM_LIST_ENTRY_PTR_TYPE  ListEntry,
	CM_LIST_ENTRY_PTR_TYPE  NewEntry
	)
{
	CM_AR_GRAPH_IOAPI_PTR_TYPE  p = (CM_AR_GRAPH_IOAPI_PTR_TYPE) ListEntry;
	CM_AR_GRAPH_IOAPI_PTR_TYPE  e = (CM_AR_GRAPH_IOAPI_PTR_TYPE) NewEntry;

	return p->api < e->api;
}

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_argr_insert_ioap (
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,

	LSA_UINT32		api,

	CM_AR_GRAPH_IOAPI_PTR_TYPE CM_LOCAL_MEM_ATTR *ioapi_ptr_ptr /* out */
)
{
	CM_LOG_FCT ("cm_argr_insert_ioap")
	CM_AR_GRAPH_IOAPI_PTR_TYPE  ptr;

	{
	CM_AR_GRAPH_IOAPI_PTR_TYPE  existing;
	existing = cm_argr_find_iocr_ap(iocr, api);
	if( is_not_null(existing) ) {
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "api(%u) not unique", api
			);
		return 20; /* api not unique */
	}
	}

		/* insert a new element */
	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (& ptr), sizeof (*ptr));
	if (is_null (ptr)) {
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "no local-mem, size(%u)", sizeof(*ptr)
			);
		return 255; /* no mem (for iocr::ioap) */
	}

	CmListInitialize (& ptr->related_iodata_objects_list)
	CmListInitialize (& ptr->related_iocs_list)

	ptr->api = api;

	CmListInsertSorted (& iocr->related_ap_list, & ptr->link, cm_list_cmp__ioapi__api);

	CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
		, "inserted: ioapi(%u)", ptr->api
		);

	/***/

	*ioapi_ptr_ptr = ptr;

	return 0; /* ok */
}

/*----------------------------------------------------------------------------*/

static LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_list_cmp__iodata__slot_subslot (
	CM_LIST_ENTRY_PTR_TYPE  ListEntry,
	CM_LIST_ENTRY_PTR_TYPE  NewEntry
	)
{
	CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE  p = (CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE) ListEntry;
	CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE  e = (CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE) NewEntry;

	if( p->slot_nr == e->slot_nr ) {
		return p->subslot_nr < e->subslot_nr;
	}

	return p->slot_nr < e->slot_nr;
}

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_argr_insert_iodata_objects (
	CM_AR_GRAPH_IOAPI_PTR_TYPE  iocr_ap,

	LSA_UINT16		slot_nr,
	LSA_UINT16		subslot_nr,
	LSA_UINT16		frame_offset,

	CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE CM_LOCAL_MEM_ATTR *iodata_ptr_ptr
)
{
	CM_LOG_FCT ("cm_argr_insert_iodata_objects")
	CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE  ptr;

	CM_ASSERT (is_not_null (iocr_ap));

	{
	CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE  iodata;
	for (CmListForeach (iodata, & iocr_ap->related_iodata_objects_list, CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE)) {
		if (iodata->slot_nr == slot_nr && iodata->subslot_nr == subslot_nr) {
			CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "subslot(%u) not unique", subslot_nr
				);
			return 23; /* subslot not unique */
		}
	}
	}

	/*
	frame_offset = checked in cm_ar_graph_check_iocrs_against_expsubmods()
	*/

		/* insert a new element */
	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (& ptr), sizeof (*ptr));
	if (is_null (ptr)) {
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "no local-mem, size(%u)", sizeof(*ptr)
			);
		return 255; /* no mem (for iocr::iodata) */
	}

	ptr->slot_nr		= slot_nr;
	ptr->subslot_nr		= subslot_nr;
	ptr->frame_offset	= frame_offset;

	/***/

	CmListInsertSorted (& iocr_ap->related_iodata_objects_list, & ptr->link, cm_list_cmp__iodata__slot_subslot);

	/***/

	*iodata_ptr_ptr = ptr;

	return 0; /* ok */
}

/*----------------------------------------------------------------------------*/

static LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_list_cmp__iocs__slot_subslot (
	CM_LIST_ENTRY_PTR_TYPE  ListEntry,
	CM_LIST_ENTRY_PTR_TYPE  NewEntry
	)
{
	CM_AR_GRAPH_IOCS_PTR_TYPE  p = (CM_AR_GRAPH_IOCS_PTR_TYPE) ListEntry;
	CM_AR_GRAPH_IOCS_PTR_TYPE  e = (CM_AR_GRAPH_IOCS_PTR_TYPE) NewEntry;

	if( p->slot_nr == e->slot_nr ) {
		return p->subslot_nr < e->subslot_nr;
	}

	return p->slot_nr < e->slot_nr;
}

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_argr_insert_iocs (
	CM_AR_GRAPH_IOAPI_PTR_TYPE  iocr_ap,

	LSA_UINT16		slot_nr,
	LSA_UINT16		subslot_nr,
	LSA_UINT16		frame_offset,

	CM_AR_GRAPH_IOCS_PTR_TYPE CM_LOCAL_MEM_ATTR *iocs_ptr_ptr
)
{
	CM_LOG_FCT ("cm_arcb_insert_iocs")
	CM_AR_GRAPH_IOCS_PTR_TYPE  ptr;

	CM_ASSERT (is_not_null (iocr_ap));

	{
	CM_AR_GRAPH_IOCS_PTR_TYPE  iocs;
	for (CmListForeach (iocs, & iocr_ap->related_iocs_list, CM_AR_GRAPH_IOCS_PTR_TYPE)) {
		if (iocs->slot_nr == slot_nr) {
			if (iocs->subslot_nr == subslot_nr) {
				CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
					, "subslot(%u) not unique", subslot_nr
					);
				return 27; /* subslot not unique */
			}
		}
	}
	}

	/*
	frame_offset = checked in cm_ar_graph_check_iocrs_against_expsubmods()
	*/

		/* insert a new element */
	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (& ptr), sizeof (*ptr));
	if (is_null (ptr)) {
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "no local-mem, size(%u)", sizeof(*ptr)
			);
		return 255; /* no mem (for iocr::iocs) */
	}

	ptr->slot_nr		= slot_nr;
	ptr->subslot_nr		= subslot_nr;
	ptr->frame_offset	= frame_offset;

	/***/

	CmListInsertSorted (& iocr_ap->related_iocs_list, & ptr->link, cm_list_cmp__iocs__slot_subslot);

	/***/

	*iocs_ptr_ptr = ptr;

	return 0; /* ok */
}


/*------------------------------------------------------------------------------
// insert a new mcr-block into an existing ar_graph
//----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_mcr (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT16		iocr_ref,
	LSA_UINT32		mcr_properties,	/* see cm_mcr_properties_enum */
	LSA_UINT16		mci_timeout_100ms,
	LSA_UINT16/*!*/	provider_name_length,
	CM_UPPER_MEM_U8_PTR_TYPE					provider_name,
	CM_AR_GRAPH_MCR_PTR_TYPE CM_LOCAL_MEM_ATTR	*mcr_ptr_ptr	/* out */
	)
{
	CM_LOG_FCT ("cm_ar_graph_insert_mcr")
	CM_AR_GRAPH_MCR_PTR_TYPE  ptr;
	LSA_UINT32  mcr_prop_factor;

	ptr = cm_ar_graph_find_mcr (ar_graph, iocr_ref);

	if (is_not_null (ptr) ) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "iocr_ref(%u) not unique", iocr_ref
			);
		return 4; /* iocr_ref not unique */
	}

	switch (mcr_properties & CM_MCR_PROP_PROTOCOL_MASK) {
	case CM_MCR_PROP_PROTOCOL_DNS:
	case CM_MCR_PROP_PROTOCOL_DCP:
		break;
	default:
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "unknown mcr_properties.protocol(0x%x)", mcr_properties
			);
		return 5; /* mcr-properties */
	}

	/* mcr_properties::CM_MCR_PROP_UNCHECKED_3_7
	 * ... not checked by the receiver (pre-FDIS 61158-6-10_v21_i378.doc)
	 */

	if ((mcr_properties & CM_MCR_PROP_RESERVED_8_15) != 0) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "mcr_properties.reserved(0x%x) not zero", mcr_properties
			);
		return 5; /* mcr-properties */
	}

	mcr_prop_factor = CM_MCR_PROP_GET_FACTOR_1SEC (mcr_properties);
	if (mcr_prop_factor == 0) { /* 1..0x64 mandatory  0x65..0xffff optional */
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "mcr_properties.factor(%u) out of range", mcr_properties
			);
		return 5; /* mcr-properties */
	}

	if (! (/*0 <= mci_timeout_100ms &&*/ mci_timeout_100ms <= 100)) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "mci_timeout_100ms(%u) out of range", mci_timeout_100ms
			);
		return 6; /* mci-timeout */
	}

	if (! (1 <= provider_name_length && provider_name_length <= 240)) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "provider_name_length(%u) out of range", provider_name_length
			);
		return 7; /* provider-name */
	}

	if (is_null (provider_name)) {
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "is_null(provider_name)"
			);
		return 8; /* provider-name */
	}

	if (! cm_is_valid_station_name(provider_name_length, provider_name)) {
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "invalid provider_name"
			);
		return 8; /* provider-name */
	}

	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (& ptr), sizeof(*ptr));
	if (is_null (ptr)) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "no local-mem, size(%u)", sizeof(*ptr)
			);
		return 255; /* no mem (for mcr) */
	}

	ptr->iocr_ref				= iocr_ref;
	ptr->mcr_properties			= mcr_properties;
	ptr->mci_timeout_100ms		= mci_timeout_100ms;
	ptr->provider_name_length	= CM_CAST_U8(provider_name_length); /* see cm_is_valid_station_name() */
	ptr->provider_name			= LSA_NULL;

	CmListInsertTail(&ar_graph->mcr_list, &ptr->link); /* NOTE: not sorted! */

	CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "inserted, iocr_ref(%u)", iocr_ref
		);

	/***/

	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (& ptr->provider_name), provider_name_length);
	if (is_null (ptr->provider_name)) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "no local-mem, size(%u)", ptr->provider_name_length
			);
		return 255; /* no mem (mcr::provider-name) */
	}

	cm_station_name_copy (ptr->provider_name, provider_name, ptr->provider_name_length);

	/***/

	*mcr_ptr_ptr = ptr;

	return 0; /* ok */
}


/*------------------------------------------------------------------------------
// insert a new alarm cr into an existing ar_graph
//----------------------------------------------------------------------------*/

#if CM_ALARM_PRIORITY_LOW != 0  || CM_ALARM_PRIORITY_HIGH != 1
# error "alarm priority wrong index"
#endif

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_alarm_cr (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT16				alcr_type,
	LSA_UINT16				ethertype,
	LSA_UINT32				alcr_properties,

	LSA_UINT16				timeout100ms,
	LSA_UINT16				retries,

	LSA_UINT16				alarm_ref,
	LSA_UINT16				max_alarm_data_length,

	LSA_UINT16				alcr_tag_header_high,
	LSA_UINT16				alcr_tag_header_low,

	CM_AR_GRAPH_ALARM_CR_PTR_TYPE CM_LOCAL_MEM_ATTR *alcr_ptr_ptr /* out */
) {
	CM_LOG_FCT ("cm_ar_graph_insert_alarm_cr")
	CM_CHANNEL_PTR_TYPE  channel = ar_graph->ar_com.channel;
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  ptr;
	LSA_UINT16  local_alarm_ref;
	LSA_UINT16  rc;

	if( is_not_null (ar_graph->alarm_cr) ) {
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "more than one alcr"
			);
		CM_FATAL(); /* a bug in caller */
	}

	if( alcr_type != CM_ALCR_TYPE_ALARM ) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "wrong alcr_type(%u)", alcr_type
			);
		return 4; /* wrong AlarmCRType */
	}

	/*
	ethertype = checked below together with transport-property
	*/

	/* alcr_properties::CM_ALCR_PROP_PRIO_MASK
	 * ... all values valid
	 */

	if( (alcr_properties & CM_ALCR_PROP_TRANSPORT_MASK) == CM_ALCR_PROP_TRANSPORT_UDP ) {

		ar_graph->ar_com.has_udp = LSA_TRUE; /* AP00583478 */

		/***/

		if( ethertype == 0x0800 ) {
			CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "wrong ethertype(0x%x)", ethertype
				);
			return 5; /* wrong ethertype */
		}

		/***/

		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "UDP not yet supported"
			);
		return 6; /* UDP not yet supported (AP01195665) */
	}
	else {

		if( ethertype != 0x8892 ) {
			CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "wrong ethertype(0x%x)", ethertype
				);
			return 5; /* wrong ethertype */
		}
	}

	/* alcr_properties::CM_ALCR_PROP_UNCHECKED_2_23
	 * ... not checked by the receiver (WD61158-6-10_v21_i323_20060912)
	 */

	if( (alcr_properties & CM_ALCR_PROP_RESERVED_24_31) != 0 ) {
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "alcr_properties::reserved_2 not zero"
			);
		return 6; /* 'reserved' not zero */
	}

	if( ! (
		timeout100ms >= 1
		&& timeout100ms <= 100
	) ) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "timeout100ms(%u) out of range", timeout100ms
			);
		return 7; /* timeout */
	}

	if( ! (
		retries >= 3
		&& retries <= 15
	) ) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "retries(%u) out of range", retries
			);
		return 8; /* retries */
	}

	/*
	alarm_ref = not checked
	*/

	if( ! (
		max_alarm_data_length >= CM_ALARM_DATA_LENGTH_MIN
		&& max_alarm_data_length <= CM_ALARM_DATA_LENGTH_MAX
	) ) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "max_alarm_data_length(%u) out of range", max_alarm_data_length
			);
		return 10; /* max_alarm_data_length */
	}

	if( alcr_tag_header_high != 0xC000 ) { /* TIA 208236 */
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "alcr_tag_header_high, prio not 6 or bit 12 not 0 or vlan-id not 0"
			);
		return 11; /* vlan-tag */
	}

	if( alcr_tag_header_low != 0xA000 ) { /* TIA 208236 */
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "alcr_tag_header_low, prio not 5 or bit 12 not 0 or vlan-id not 0"
			);
		return 12; /* vlan-tag */
	}

	/***/

	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (& ptr), sizeof (*ptr));
	if (is_null (ptr)) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "no local-mem, size(%u)", sizeof(*ptr)
			);
		return 255; /* no mem (for alarmcr) */
	}

	/***/

	{
	LSA_BOOL ok;

	ok = cm_range_alloc_id (&channel->res.id_range[CM_RANGE_ID_ALARM_REF], &local_alarm_ref);

	if (!ok) {
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "cannot alloc alarm_ref"
			);
		CM_FREE_LOCAL_MEM (& rc, ptr);
		CM_ASSERT (rc == LSA_RET_OK);
		return 254; /* no alarm-ref */
	}
	}

	/***/

	ptr->cm.backlink = ar_graph;

	ptr->cm.cr_state = CM_AR_CR_IDLE;

	/***/

	ptr->alcr_type			= alcr_type;
	ptr->alcr_properties	= alcr_properties;

	ptr->timeout100ms		= CM_CAST_U8(timeout100ms); /* see range check above */
	ptr->retries			= CM_CAST_U8(retries); /* see range check above */

	ptr->local_alarm_ref	= local_alarm_ref;	/* Local-Alarm-Reference */
	ptr->remote_alarm_ref	= alarm_ref;		/* Remote-Alarm-Reference */

	ptr->max_alarm_data_len	= max_alarm_data_length;

	ptr->ethertype			= ethertype;

	/***/

	ar_graph->alarm_cr = ptr;

	CM_AR_TRACE_03(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "inserted, prop(0x%x) local_alarm_ref(%u) remote_alarm_ref(%u)"
		, ptr->alcr_properties, ptr->local_alarm_ref, ptr->remote_alarm_ref
		);

	/***/

	*alcr_ptr_ptr = ptr;

	return 0; /* ok */
}

/*------------------------------------------------------------------------------
// insert a new 'ap-module' pair into an existing ar_graph
//----------------------------------------------------------------------------*/

static LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_list_cmp__argr_ap__api (
	CM_LIST_ENTRY_PTR_TYPE  ListEntry,
	CM_LIST_ENTRY_PTR_TYPE  NewEntry
	)
{
	CM_AR_GRAPH_AP_PTR_TYPE  p = (CM_AR_GRAPH_AP_PTR_TYPE) ListEntry;
	CM_AR_GRAPH_AP_PTR_TYPE  e = (CM_AR_GRAPH_AP_PTR_TYPE) NewEntry;

	return p->api < e->api;
}

/*----------------------------------------------------------------------------*/

static LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_list_cmp__argr_mod__slot_nr (
	CM_LIST_ENTRY_PTR_TYPE  ListEntry,
	CM_LIST_ENTRY_PTR_TYPE  NewEntry
	)
{
	CM_AR_GRAPH_MODULE_PTR_TYPE  p = (CM_AR_GRAPH_MODULE_PTR_TYPE) ListEntry;
	CM_AR_GRAPH_MODULE_PTR_TYPE  e = (CM_AR_GRAPH_MODULE_PTR_TYPE) NewEntry;

	return p->slot_nr < e->slot_nr;
}

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_ap_module (
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT32	api,

	LSA_UINT16	slot_nr,
	LSA_UINT32	mod_ident,
	LSA_UINT16	mod_properties,

	LSA_UINT16	CM_LOCAL_MEM_ATTR *exp_slot_nr_ptr, /* LSA_NULL or expected slot_nr */

	CM_AR_GRAPH_AP_PTR_TYPE CM_LOCAL_MEM_ATTR *ap_ptr_ptr, /* out */
	CM_AR_GRAPH_MODULE_PTR_TYPE CM_LOCAL_MEM_ATTR *mod_ptr_ptr /* out */
) {
	CM_LOG_FCT ("cm_ar_graph_insert_ap_module")
	CM_AR_GRAPH_AP_PTR_TYPE ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE mod;

	CM_ASSERT (is_not_null (ar_graph));

	/*
	 * why ap-module pairs?
	 * see structure of ExpectedSubmoduleBlockReq!
	 */

	ap = cm_ar_graph_find_ap(ar_graph, api);

	if( is_null(ap) ) {

			/* insert a new 'ap' */
		CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (& ap), sizeof (*ap));

		if (is_null (ap)) {
			CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "no local-mem, size(%u)", sizeof(*ap)
				);
			return 255; /* no mem (for ap) */
		}

		/***/

		ap->api = api;

		CmListInitialize (& ap->mod_list);

#if CM_CFG_MAX_SERVERS
		ap->sv.real_ap = LSA_NULL;
#endif

		/***/

		CmListInsertSorted (& ar_graph->ap_list, & ap->link, cm_list_cmp__argr_ap__api);

		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "inserted, api(%u)", ap->api);
	}

	CM_ASSERT(is_not_null(ap));

	/***/

	mod = cm_ar_graph_find_module(ap, slot_nr);

	if( is_not_null(mod) ) {

		/* not both ap and module may already exist */

		CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "both api(%u) and slot_nr(%u) already exist"
			, api, slot_nr
			);

		return 6; /* api/slot_nr not unique (dup expsubmod-block?) */
	}

	if( is_not_null(exp_slot_nr_ptr) ) {

		if( slot_nr != *exp_slot_nr_ptr ) {

			CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "slot_nr(%u) not const(%u) withing expsubmod-block"
				, slot_nr, *exp_slot_nr_ptr
				);
			return 6; /* slot not const within expsubmod-block */
		}
	}

	if( ! (
		/*slot_nr >= 0 &&*/
		slot_nr <= 0x7FFF
	) ) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "slot_nr(%u) out of range", slot_nr
			);
		return 6; /* slot_nr out of range (general) */
	}

	/***/

	if( mod_ident == 0 ) {
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "mod_ident is zero"
			);
		return 7; /* mod_ident is zero */
	}

	/***/

	/* mod_properties::CM_MOD_PROP_UNCHECKED_0_7
	 * ... not checked by the receiver (pre-FDIS 61158-6-10_v21_i378.doc)
	 */

	if( (mod_properties & CM_MOD_PROP_RESERVED_8_15) != 0 ) {
		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "mod_properties.reserved not zero"
			);
		return 8; /* 'reserved' not zero */
	}

	/***/

		/* insert a new module */
	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (& mod), sizeof (*mod));

	if (is_null (mod)) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "no local-mem, size(%u)", sizeof(*mod)
			);
		return 255; /* no mem (for mod) */
	}

	/***/

	mod->slot_nr			= slot_nr;

	mod->soll.mod_ident		= mod_ident;
	mod->soll.mod_properties= mod_properties;

#if CM_CFG_MAX_CLIENTS /* note: CM-SV uses sv.real_sl->mod_ident */
	mod->ist.mod_ident		= 0; /* dummy-assignment, see cm_cl_marshal_connect() set to soll.mod_ident */
#endif

#if CM_CFG_MAX_SERVERS
	mod->sv.real_sl   = LSA_NULL;
#endif

	CmListInitialize (& mod->submod_list);

	/***/

	mod->ap = ap;

	CmListInsertSorted (& ap->mod_list, & mod->link, cm_list_cmp__argr_mod__slot_nr);

	CM_AR_TRACE_04(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "inserted, api(%u) slot(%u) soll.module_id(%u) soll.prop(%u)"
		, ap->api, mod->slot_nr, mod->soll.mod_ident, mod->soll.mod_properties
		);

	/***/

	*ap_ptr_ptr = ap;

	*mod_ptr_ptr = mod;

	return 0; /* ok */
}


/*------------------------------------------------------------------------------
// insert a new submodule into an existing ar_graph
//----------------------------------------------------------------------------*/

static LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_list_cmp__argr_submod__subslot_nr (
	CM_LIST_ENTRY_PTR_TYPE  ListEntry,
	CM_LIST_ENTRY_PTR_TYPE  NewEntry
	)
{
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE p = (CM_AR_GRAPH_SUBMODULE_PTR_TYPE) ListEntry;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE e = (CM_AR_GRAPH_SUBMODULE_PTR_TYPE) NewEntry;

	return p->subslot_nr < e->subslot_nr;
}

/*----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_submodule (
	LSA_UINT32		ar_properties,
	CM_AR_GRAPH_MODULE_PTR_TYPE  module,

	LSA_UINT16		subslot_nr,
	LSA_UINT32		sub_ident,
	LSA_UINT16		sub_properties,
	LSA_BOOL		IOXS_Required,

	CM_AR_GRAPH_SUBMODULE_PTR_TYPE CM_LOCAL_MEM_ATTR *sub_ptr_ptr /* out */
) {
	CM_LOG_FCT ("cm_ar_graph_insert_submodule")
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE ptr;

	CM_ASSERT (is_not_null (module));

	ptr = cm_ar_graph_find_submodule (LSA_NULL, module, 0, subslot_nr);

	if (is_not_null (ptr)) {
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "subslot(%u) already exists", subslot_nr
			);
		return 10; /* subslot_nr not unique */
	}

	if( subslot_nr == 0 ) { /* special case */

		/* AP00517543: allow subslot_nr 0 if CM_ALARM_TYPE_PULL_MODULE is supported */

		if( (ar_properties & CM_AR_PROP_PULL_MODULE_ALARM_ALLOWED) == 0 ) {

			CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "subslot_nr(%u) out of range (pull-module alarm not allowed)", subslot_nr
				);
			return 10; /* subslot_nr out of range (ar_properties do not allow 0) */
		}
	}
	else if( subslot_nr > 0x8FFF ) { /* 1 - 0x7FFF and 0x8ipp */

		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "subslot_nr(%u) out of range", subslot_nr
			);
		return 10; /* subslot_nr out of range (general) */
	}

	/***/

	/*
	sub_ident = all values valid
	*/

	/***/

	switch (sub_properties & CM_SUB_PROP_TYPE_MASK) {
	case CM_SUB_PROP_TYPE_NO_DATA:
	case CM_SUB_PROP_TYPE_INPUT:
	case CM_SUB_PROP_TYPE_OUTPUT:
	case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
		break;
	default:
		return 12; /* not possible (2 bits) */
	}

	if( (sub_properties & CM_SUB_PROP_SHARED_INP_MASK) == CM_SUB_PROP_SHARED_INP_YES ) {

		return 12; /* sub_properties.shared not supported (22. conclave) */
	}

	if( (sub_properties & CM_SUB_PROP_REDUCE_INP_LEN_MASK) == CM_SUB_PROP_REDUCE_INP_LEN_YES ) {

		return 12; /* sub_properties.reduce_inp_len not supported (AP01511086) */
	}

	if( (sub_properties & CM_SUB_PROP_REDUCE_OUT_LEN_MASK) == CM_SUB_PROP_REDUCE_OUT_LEN_YES ) {

		return 12; /* sub_properties.reduce_out_len not supported (AP01511086) */
	}

	if( (sub_properties & CM_SUB_PROP_DISCARD_IOXS_MASK) == CM_SUB_PROP_DISCARD_IOXS_YES ) {

		if( (sub_properties & CM_SUB_PROP_TYPE_MASK) != CM_SUB_PROP_TYPE_NO_DATA ) {

			return 12; /* sub_properties.discard_ioxs but not sub_properties.no_data (AP01511084) */
		}

		if( IOXS_Required ) {

			return 12; /* sub_properties.discard_ioxs not supported (TIA 1219981) */
		}
	}

	/* sub_properties::CM_SUB_PROP_UNCHECKED_6_7
	 * ... not checked by the receiver (pre-FDIS 61158-6-10_v21_i378.doc)
	 */

	if( (sub_properties & CM_SUB_PROP_RESERVED_8_15) != 0 ) {
		CM_AR_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP
			, "sub_properties.reserved not zero"
			);
		return 12; /* 'reserved' not zero */
	}

	/***/

	CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (& ptr), sizeof (*ptr));

	if (is_null (ptr)) {
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "no local-mem, size(%u)", sizeof(*ptr)
			);
		return 255; /* no mem (for submod) */
	}

	/***/

	ptr->subslot_nr			= subslot_nr;

	ptr->soll.sub_properties= sub_properties;
	ptr->soll.sub_ident		= sub_ident;

#if CM_CFG_MAX_CLIENTS /* note: CM-SV uses sv.real_sl->mod_ident */
	ptr->ist.sub_ident = 0;
	ptr->ist.sub_state = 0; /* dummy-assignment, see cm_cl_marshal_connect() */
#endif

	ptr->descr[0].data_length = 0xFFFF; /* invalid, see cm_ar_graph_set_data_descr() */
	ptr->descr[1].data_length = 0xFFFF; /* invalid, see cm_ar_graph_set_data_descr() */

	ptr->inp_via_mcr = LSA_FALSE;
	ptr->out_via_mcr = LSA_FALSE;

#if CM_CFG_MAX_CLIENTS
	CmListInitialize (& ptr->cl.prm_record_list);
#endif

#if CM_CFG_MAX_SERVERS
	ptr->sv.real_su  = LSA_NULL;
	ptr->sv.sm_flags = CM_SV_AR_SMFL_WRONG; /* start with wrong (implicit-read of ModuleDiffBlock) */
	ptr->sv.st_told  = 0;
#endif

	/***/

	ptr->mod = module;

	CmListInsertSorted (& module->submod_list, & ptr->link, cm_list_cmp__argr_submod__subslot_nr);

	CM_AR_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
		, "inserted, subslot(%u) ident(0x%x) prop(0x%x)"
		, ptr->subslot_nr, ptr->soll.sub_ident, ptr->soll.sub_properties
		);

	/***/

	*sub_ptr_ptr = ptr;

	return 0; /* ok */
}


/*------------------------------------------------------------------------------
// set a data_description to an existing submodule
//----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_set_data_descr (
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  submod,

	LSA_UINT16		direction, /* 0..input, 1..output, 2..use-description */

	LSA_UINT16		data_description,
	LSA_UINT16		data_length,
	LSA_UINT8		iops_length,
	LSA_UINT8		iocs_length
)
{
	CM_LOG_FCT ("cm_ar_graph_set_data_descr")
	LSA_UINT16  dscr;

	CM_ASSERT (is_not_null (submod));

	/***/

	dscr = CM_CAST_U16(data_description & CM_SUB_DATA_DESCR_TYPE_MASK);

	switch (dscr) {
	case CM_SUB_DATA_DESCR_TYPE_INPUT:
		if (direction == 2) {
			direction = 0;
		}
		if (direction != 0) {
			CM_AR_TRACE_02(0, LSA_TRACE_LEVEL_ERROR
				, "direction(%u) and data_description(0x%x) don't match"
				, direction, data_description
				);
			return 13; /* description */
		}
		break;

	case CM_SUB_DATA_DESCR_TYPE_OUTPUT:
		if (direction == 2) {
			direction = 1;
		}
		if (direction != 1) {
			CM_AR_TRACE_02(0, LSA_TRACE_LEVEL_ERROR
				, "direction(%u) and data_description(0x%x) don't match"
				, direction, data_description
				);
			return 13; /* description */
		}
		break;

	default:
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "wrong data_description(0x%x)", data_description
			);
		return 13; /* description */
	}

	switch (submod->soll.sub_properties & CM_SUB_PROP_TYPE_MASK) {
	case CM_SUB_PROP_TYPE_NO_DATA:
	case CM_SUB_PROP_TYPE_INPUT:
		if (dscr != CM_SUB_DATA_DESCR_TYPE_INPUT) {
			CM_AR_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP
				, "sub_properties(0x%x) and data_description(0x%x) don't match"
				, submod->soll.sub_properties, data_description
				);
			return 13; /* description */
		}
		break;

	case CM_SUB_PROP_TYPE_OUTPUT:
		if (dscr != CM_SUB_DATA_DESCR_TYPE_OUTPUT) {
			CM_AR_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP
				, "sub_properties(0x%x) and data_description(0x%x) don't match"
				, submod->soll.sub_properties, data_description
				);
			return 13; /* description */
		}
		break;

	case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
		/* needs in and out */
		break;

	default:
		CM_FATAL(); /* not possible */
		break;
	}

	if (submod->descr[direction].data_length != 0xFFFF) {
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "direction(%u) already set", direction
			);
		return 13; /* description */
	}

	/***/

	if( (submod->soll.sub_properties & CM_SUB_PROP_TYPE_MASK) == CM_SUB_PROP_TYPE_NO_DATA ) {

		/* PNIO-Spec, Part 510, chapter "IO Data ASE"
		 *
		 * NOTE If a submodule does not contain any input or output data
		 *      it is considered as an Input Data item with the length of
		 *      Input Data attribute set to zero.
		 */

		if( data_length != 0 ) {
			CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
				, "property 'no-data' but data_length(%u) not zero"
				, data_length
				);
			return 14; /* data-length */
		}
	}

	if( ! (
		/*0 <= data_length &&*/
		data_length <= CM_SUB_DATA_LENGTH_MAX
	) ) {
		CM_AR_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP
			, "data_length(%u) > max(%u)"
			, data_length, CM_SUB_DATA_LENGTH_MAX
			);
		return 14; /* data-length */
	}

	/***/

	if( iops_length != 1 ) {
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "wrong iops_length(%u)", iops_length
			);
		return 15; /* iops-length */
	}

	if( iocs_length != 1 ) {
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP
			, "wrong iocs_length(%u)", iocs_length
			);
		return 16; /* iocs-length */
	}

	/***/

	submod->descr[direction].data_description	= data_description;
	submod->descr[direction].data_length		= data_length;
	submod->descr[direction].iops_length		= iops_length;
	submod->descr[direction].iocs_length		= iocs_length;

	CM_AR_TRACE_06(0, LSA_TRACE_LEVEL_CHAT
		, "subslot(%u) dir(%u) descr(%u) length(%u) iops(%u) iocs(%u)"
		, submod->subslot_nr, direction, data_description
		, data_length, iops_length, iocs_length
		);

	/***/

	return 0; /* ok */
}

/*------------------------------------------------------------------------------
// insert SRInfoBlock
//----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_sr_info(
	CM_AR_GRAPH_PTR_TYPE	ar_graph,

	LSA_UINT16	rdh_factor,
	LSA_UINT32	sr_properties,
	LSA_UINT16	alloc_more /* for CM-SV, else 0 */
) {
	CM_LOG_FCT("cm_ar_graph_insert_sr_info")
	CM_AR_GRAPH_SR_INFO_PTR_TYPE ptr;

	/***/

	if( is_not_null(ar_graph->sr_info) ) {
		CM_FATAL(); /* see caller */
	}

	/***/

	if( rdh_factor < 3 ) { /* AP01316987 */

		return 4;
	}

	if( (sr_properties & CM_SR_PROP_INP_VALID_ON_BACKUP_AR) != 0 ) {

		return 5; /* not yet supported */
	}

	if( (sr_properties & CM_SR_PROP_RESERVED_1_MASK) != 0 ) {

		return 5; /* reserved_1 must be zero */
	}

	if( (sr_properties & CM_SR_PROP_MODE) != 0 ) {

		return 5; /* not yet supported */
	}

	if( (sr_properties & CM_SR_PROP_RESERVED_2_MASK) != 0 ) {

		return 5; /* reserved_2 must be zero */
	}

	if( (sr_properties & CM_SR_PROP_RESERVED_3_MASK) != 0 ) {

		/* not checked */
	}

	/***/

	CM_ALLOC_LOCAL_MEM(CM_CAST_LOCAL_MEM_PTR_PTR(&ptr), sizeof(*ptr) + alloc_more);

	if( is_null(ptr) ) {

		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "no local-mem, size(%u)", sizeof(*ptr)
			);

		return 255; /* no mem (for sr_info) */
	}

	/***/

	ptr->rdh_factor    = rdh_factor;
	ptr->sr_properties = sr_properties;

	ar_graph->sr_info = ptr;

	return 0; /* ok */
}


/*------------------------------------------------------------------------------
// insert ARVendorBlock
//----------------------------------------------------------------------------*/

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_insert_arvendor (
	CM_AR_GRAPH_PTR_TYPE  argr,

	LSA_UINT32					api,
	LSA_UINT16					ap_structure_identifier,
	LSA_UINT16					req_data_len,
	CM_COMMON_MEM_U8_PTR_TYPE	req_data,
	LSA_UINT16					res_alloc_len
) {
	CM_LOG_FCT ("cm_ar_graph_insert_arvendor")
	CM_AR_GRAPH_ARVENDOR_PTR_TYPE  ptr;
	LSA_UINT32  alloc_size;

	/* note: CM_AR_PROP_STARTUP_MODE_ADVANCED is checked in caller */

	ptr = cm_ar_graph_find_arvendor (argr, api, ap_structure_identifier);

	if (is_not_null(ptr)) {
		CM_AR_TRACE_02(0, LSA_TRACE_LEVEL_ERROR
			, "duplicate api(0x%x) ap_structure_identifier(0x%x)"
			, api, ap_structure_identifier
			);
		return 4; /* duplicate */
	}

	alloc_size = sizeof (*ptr) + req_data_len + res_alloc_len;

	if (alloc_size > 0xFFFF) {

		ptr = LSA_NULL;
	}
	else {

		CM_ALLOC_LOCAL_MEM (CM_CAST_LOCAL_MEM_PTR_PTR (& ptr), (LSA_UINT16)alloc_size);
	}

	if (is_null (ptr)) {
		CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR
			, "no local-mem, size(%u)", (LSA_UINT16)alloc_size
			);
		return 255; /* no mem */
	}

	/*
	 * note: no checking rules (PNIO-Spec, Part 5: vendor data is interpreted by local means)
	 */

	ptr->api						= api;
	ptr->ap_structure_identifier	= ap_structure_identifier;
	ptr->req_data_len				= req_data_len;
	ptr->res_data_len				= 0;

	CM_MEMCPY (ptr->data, req_data, req_data_len); /* dynamic size */

	/***/

	CmListInsertTail (& argr->arvendor_list, & ptr->link);

	/***/

	return 0; /* ok */
}

/*------------------------------------------------------------------------------
// find ARVendorBlock
//----------------------------------------------------------------------------*/

CM_AR_GRAPH_ARVENDOR_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_arvendor (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT32  api,
	LSA_UINT16  ap_structure_identifier
) {
	CM_AR_GRAPH_ARVENDOR_PTR_TYPE  arv;

	for (CmListForeach (arv, &argr->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE))
	{
		if (arv->api == api && arv->ap_structure_identifier == ap_structure_identifier)
		{
			return arv;
		}
	}
	return LSA_NULL;
}


/*------------------------------------------------------------------------------
// consistency checks
//----------------------------------------------------------------------------*/

static LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_check_expsubmods_against_iocrs(
	CM_AR_GRAPH_PTR_TYPE ar_graph
) {
	CM_LOG_FCT("cm_ar_graph_check_expsubmods_against_iocrs")
	CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

	/*
	 * all "expected" submodules have to be in an iocr
	 */

	for (CmListForeach(ar_ap, &ar_graph->ap_list, CM_AR_GRAPH_AP_PTR_TYPE)) {

		CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

		for (CmListForeach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE)) {

			CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

			for (CmListForeach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE)) {

				LSA_UINT16  iocs_cnt = 0;
				LSA_UINT16  iops_cnt = 0;
				LSA_UINT16  mcons_cnt = 0;
				LSA_UINT16  mprov_cnt = 0;

				{
				CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

				for (CmListForeach (ar_iocr, & ar_graph->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE)) {

					CM_AR_GRAPH_IOAPI_PTR_TYPE ar_ioapi;
					CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE ar_iodata;
					CM_AR_GRAPH_IOCS_PTR_TYPE ar_iocs;

					ar_ioapi = cm_ar_graph_find_ioapi (ar_iocr, ar_ap->api);

					if (is_null (ar_ioapi)) {
						/* use another iocr */
						continue;
					}

					ar_iodata = cm_ar_graph_find_iodata (
							ar_ioapi, ar_mod->slot_nr, ar_sub->subslot_nr
						);

					ar_iocs = cm_ar_graph_find_iocs (
							ar_ioapi, ar_mod->slot_nr, ar_sub->subslot_nr
						);

					switch( ar_iocr->iocr_type ) {
					case CM_IOCR_TYPE_INPUT:
					case CM_IOCR_TYPE_OUTPUT:
						if (is_not_null (ar_iodata)) {
							iops_cnt += 1;
						}
						if (is_not_null (ar_iocs)) {
							iocs_cnt += 1;
						}
						break;

					case CM_IOCR_TYPE_MULTICAST_PROVIDER:
						if (is_not_null (ar_iodata)) {
							CM_ASSERT (ar_sub->inp_via_mcr);
							mprov_cnt += 1;
						}
						if (is_not_null (ar_iocs)) {
							return 10; /* multicast-provider and iocs */
						}
						break;

					case CM_IOCR_TYPE_MULTICAST_CONSUMER:
						if (is_not_null (ar_iodata)) {
							CM_ASSERT (ar_sub->out_via_mcr);
							mcons_cnt += 1;
						}
						if (is_not_null (ar_iocs)) {
							return 10; /* multicast-consumer and iocs */
						}
						break;

					default:
						CM_FATAL();
						break;
					}
				}
				}

				/***/

				if (iops_cnt == 0 || iocs_cnt == 0) {
					return 10; /* not releated */
				}
				if (iops_cnt != iocs_cnt) {
					return 10; /* iops / iocs mismatch */
				}

				if (ar_sub->inp_via_mcr) {
					/* strange, because this is the condition for setting _via_mcr to true */
					if (mprov_cnt == 0) {
						return 10; /* mc-submodule and no m-consumer */
					}
				}

				if (ar_sub->out_via_mcr) {
					/* strange, because this is the condition for setting _via_mcr to true */
					if (mcons_cnt == 0) {
						return 10; /* mc-submodule and no m-provider */
					}
				}
			}
		}
	}

	/***/

	return 0; /* ok */
}

/*----------------------------------------------------------------------------*/

static LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_check_iocrs_against_expsubmods(
	CM_AR_GRAPH_PTR_TYPE ar_graph
) {
	CM_LOG_FCT("cm_ar_graph_check_iocrs_against_expsubmods")
	CM_CHANNEL_PTR_TYPE  channel = ar_graph->ar_com.channel;

	/*
	 * all "related" submodules have to be in the expected-config
	 *
	 * and iodata, iops and iocs must not overlap
	 */

	{
	CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

	for (CmListForeach (ar_iocr, & ar_graph->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE)) {

		LSA_UINT16 used_from;
		LSA_UINT16 used_to;
		LSA_UINT16 iocr_dir;

		CM_ASSERT(ar_iocr->data_length <= CM_PNIO_C_SDU_MAX_LENGTH); /* see above */

		/*
		 * instead of repeatedly finding the minimum-frame-offset of all ioapis/iodata/iocs
		 * we use a bitfield, telling if a byte is already used by another submodule.
		 */

		CM_MEMSET(cm_argr_used, 0/*free*/, sizeof(cm_argr_used));

		used_from = 0xFFFF; /* see below */
		used_to   = 0;

		/***/

		switch( ar_iocr->iocr_type ) { /* note: cannot use is_consumer here */
		case CM_IOCR_TYPE_INPUT:
		case CM_IOCR_TYPE_MULTICAST_PROVIDER:
			iocr_dir = 0;
			break;
		case CM_IOCR_TYPE_OUTPUT:
		case CM_IOCR_TYPE_MULTICAST_CONSUMER:
			iocr_dir = 1;
			break;
		default:
			CM_FATAL();
			iocr_dir = 0; /* not reached */
			break;
		}

		/***/

		{
		CM_AR_GRAPH_IOAPI_PTR_TYPE ar_ioapi;

		for (CmListForeach (ar_ioapi, & ar_iocr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE)) {

			CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

			ar_ap = cm_ar_graph_find_ap(ar_graph, ar_ioapi->api);

			if( is_null(ar_ap) ) {
				return 20; /* api not found */
			}

			/* api found */

			{
			CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE ar_iodata;

			for (CmListForeach (ar_iodata, &ar_ioapi->related_iodata_objects_list, CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE)) {

				CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;
				LSA_UINT16 descr_dir;
				LSA_BOOL reduce_iodata;
				LSA_BOOL reduce_ioxs;

				ar_mod = cm_ar_graph_find_module(ar_ap, ar_iodata->slot_nr);

				if( is_null(ar_mod) ) {
					return 22; /* slot_nr not found */
				}

				ar_sub = cm_ar_graph_find_submodule(LSA_NULL, ar_mod, 0/*ignored*/, ar_iodata->subslot_nr);

				if( is_null(ar_sub) ) {
					return 23; /* subslot_nr not found */
				}

				/* slot and subslot found */

				switch (ar_sub->soll.sub_properties & CM_SUB_PROP_TYPE_MASK) {
				case CM_SUB_PROP_TYPE_NO_DATA:
				case CM_SUB_PROP_TYPE_INPUT:
					descr_dir = 0;
					if( iocr_dir != descr_dir ) {
						return 23; /* submodule and iocr not same direction */
					}
					break;

				case CM_SUB_PROP_TYPE_OUTPUT:
					descr_dir = 1;
					if( iocr_dir != descr_dir ) {
						return 23; /* submodule and iocr not same direction */
					}
					break;

				case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
					descr_dir = iocr_dir;
					break;

				default:
					descr_dir = 0;
					CM_FATAL(); /* not possible */
					break;
				}

				/***/

				reduce_iodata = LSA_FALSE;

				if( descr_dir == 0 ) {

					if( (ar_sub->soll.sub_properties & CM_SUB_PROP_REDUCE_INP_LEN_MASK) == CM_SUB_PROP_REDUCE_INP_LEN_YES ) {

						reduce_iodata = LSA_TRUE;
					}
				}
				else {

					if( (ar_sub->soll.sub_properties & CM_SUB_PROP_REDUCE_OUT_LEN_MASK) == CM_SUB_PROP_REDUCE_OUT_LEN_YES ) {

						reduce_iodata = LSA_TRUE;
					}
				}

				/***/

				reduce_ioxs = LSA_FALSE;

				if( (ar_sub->soll.sub_properties & CM_SUB_PROP_DISCARD_IOXS_MASK) == CM_SUB_PROP_DISCARD_IOXS_YES ) {

					reduce_ioxs = LSA_TRUE;
				}

				/***/

				{
				LSA_UINT16 from;
				LSA_UINT16 to;

				if( ar_iodata->frame_offset > ar_iocr->data_length ) { /* note: frame_offset == data_length possible in case of "DiscardIOXS" (AP01511078) */
					return 24; /* frame-offset too big (or iocr too short?) */
				}

				CM_ASSERT(ar_sub->descr[descr_dir].data_length != 0xFFFF); /* must exist, see exp-iocr check */

				from = ar_iodata->frame_offset;

				to = from;

				if( ! reduce_iodata ) {

					to += ar_sub->descr[descr_dir].data_length;
				}

				if( ! reduce_ioxs ) {

					to += ar_sub->descr[descr_dir].iops_length;
				}

				if( to > ar_iocr->data_length ) {
					return 24; /* submodule-data-length too big (or iocr too short?) */
				}

				if( used_from > from ) {
					used_from = from;
				}

				if( used_to < to ) {
					used_to = to;
				}

				for( /*from=from*/; from < to; ++from ) {

					if( cm_argr_used[from] ) {
						return 24; /* iodata/iops overlap */
					}

					cm_argr_used[from] = 1/*used*/;
				}
				}
			}
			}

			/***/

			{
			CM_AR_GRAPH_IOCS_PTR_TYPE ar_iocs;

			for (CmListForeach (ar_iocs, &ar_ioapi->related_iocs_list, CM_AR_GRAPH_IOCS_PTR_TYPE)) {

				CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;
				LSA_UINT16 descr_dir;
				LSA_BOOL reduce_ioxs;

				ar_mod = cm_ar_graph_find_module(ar_ap, ar_iocs->slot_nr);

				if( is_null(ar_mod) ) {
					return 26; /* slot_nr not found */
				}

				ar_sub = cm_ar_graph_find_submodule(LSA_NULL, ar_mod, 0/*ignored*/, ar_iocs->subslot_nr);

				if( is_null(ar_sub) ) {
					return 27; /* subslot_nr not found */
				}

				/* slot and subslot found */

				switch (ar_sub->soll.sub_properties & CM_SUB_PROP_TYPE_MASK) {
				case CM_SUB_PROP_TYPE_NO_DATA:
				case CM_SUB_PROP_TYPE_INPUT:
					descr_dir = 0;
					if( iocr_dir == descr_dir ) {
						return 23; /* submodule and iocr not different direction */
					}
					break;

				case CM_SUB_PROP_TYPE_OUTPUT:
					descr_dir = 1;
					if( iocr_dir == descr_dir ) {
						return 23; /* submodule and iocr not different direction */
					}
					break;

				case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
					if (iocr_dir == 0) {
						descr_dir = 1;
					}
					else {
						descr_dir = 0;
					}
					break;

				default:
					descr_dir = 0;
					CM_FATAL(); /* not possible */
					break;
				}

				/***/

				reduce_ioxs = LSA_FALSE;

				if( (ar_sub->soll.sub_properties & CM_SUB_PROP_DISCARD_IOXS_MASK) == CM_SUB_PROP_DISCARD_IOXS_YES ) {

					reduce_ioxs = LSA_TRUE;
				}

				/***/

				{
				LSA_UINT16 from;
				LSA_UINT16 to;

				if( ar_iocs->frame_offset > ar_iocr->data_length ) { /* note: frame_offset == data_length possible in case of "DiscardIOXS" (AP01511078) */
					return 28; /* frame-offset too big (or iocr too short?) */
				}

				CM_ASSERT(ar_sub->descr[descr_dir].data_length != 0xFFFF); /* must exist, see exp-iocr check */

				from = ar_iocs->frame_offset;

				to = from;

				if( ! reduce_ioxs ) {

					to += ar_sub->descr[descr_dir].iocs_length;
				}

				/***/

				if( to > ar_iocr->data_length ) {
					return 28; /* frame-offset too big (or iocr too short?) */
				}

				if( used_from > from ) {
					used_from = from;
				}

				if( used_to < to ) {
					used_to = to;
				}

				for( /*from=from*/; from < to; ++from ) {

					if( cm_argr_used[from] ) {
						return 28; /* iocs overlap */
					}

					cm_argr_used[from] = 1/*used*/;
				}
				}
			}
			}
		}
		}

		/***/

		if( used_from == 0xFFFF ) {
			/* empty frame */
			used_from = 0;
			used_to   = 0; /* sanity, is already zero */
		}

		/***/

		{ /* AP01381691 */
		if (ar_iocr->cm.subframe_data != 0) /* is-dfp */
		{
			if(
				used_from < ar_iocr->cm.subframe_offset
				|| used_to > ar_iocr->cm.subframe_offset + ((ar_iocr->cm.subframe_data >> 8) & 0xFF)
			) {
				return 24; /* frame-offset does not match DFP parameters */
			}
		}
		}

		/***/

		{
		LSA_UINT16 mlen;

		if( (ar_iocr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_1_UDP ) {

			mlen = CM_PNIO_C_SDU_MIN_LENGTH_UDP;
		}
		else {

			mlen = CM_PNIO_C_SDU_MIN_LENGTH_RT;
		}

		if( ar_graph->ar_com.path_type == CM_PATH_TYPE_SERVER ) {

			ar_iocr->cm.apdu_status_length = cm_edd_get_apdu_status_length(channel);

			CM_ASSERT (ar_iocr->cm.partial_offset == 0);
			CM_ASSERT (ar_iocr->cm.partial_length == 0);

			ar_iocr->cm.partial_offset = 0;
			ar_iocr->cm.partial_length = ar_iocr->data_length;

			if( ar_iocr->cm.subframe_data != 0 ) {

				/* DFP: partial = full size */
			}
			else if( ar_iocr->cm.is_consumer ) {

				if( cm_edd_can_partial_data(channel, (ar_iocr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK)) ) {

					ar_iocr->cm.partial_offset = used_from;

					if( used_to == used_from ) { /* everything discarded or empty frame */

						ar_iocr->cm.partial_length = 1; /* use the smallest possible buffer size (0 means "no partial data access") */
					}
					else { /* not empty */

						ar_iocr->cm.partial_length = CM_CAST_U16(used_to - used_from);
					}
				}
			}
			else { /* provider */

				if( ar_iocr->data_length == mlen ) {

					/*
					 * data_length <  mlen : range error
					 * data_length == mlen : autopadding possible
					 * data_length >  mlen : there are no paddings
					 */

					if( cm_edd_can_autopadding(channel, (ar_iocr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK)) ) {

						ar_iocr->cm.partial_offset = 0; /* note: only the end is padded */

						if( used_to == 0 ) { /* everything discarded or empty frame */

							ar_iocr->cm.partial_length = 1; /* use the smallest possible buffer size (0 means "no autopadding") */
						}
						else { /* not empty */

							ar_iocr->cm.partial_length = used_to;
						}
					}
				}
			}
		}
		else { /* cm-cl or cm-mc, see cm_arcb_calc_iocr_buffer_size() */

			if( ar_iocr->cm.apdu_status_length != cm_edd_get_apdu_status_length(channel) ) {
				return 103; /* note: a user-bug */
			}

			/* must not change partial_offset / partial_length. user allocated the buffer already! */

			if( ar_iocr->cm.subframe_data != 0 ) { /* is-dfp */

				if (ar_iocr->cm.partial_offset != 0) {
					return 101; /* note: a user-bug */
				}

				/* SOC => full frame */
				/* PNIO => subframe */
			}
			else if( ar_iocr->cm.is_consumer ) {

				if( cm_edd_can_partial_data(channel, (ar_iocr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK)) ) {

					if( ar_iocr->cm.partial_offset > used_from ) {
						return 101; /* note: a user-bug */
					}

					if( (ar_iocr->cm.partial_offset + ar_iocr->cm.partial_length) < used_to ) {
						return 102; /* note: a user-bug */
					}
				}
				else {

					if( ar_iocr->cm.partial_length != ar_iocr->data_length ) {
						return 100; /* note: a user-bug */
					}
				}
			}
			else { /* provider */

				if( ar_iocr->cm.partial_offset != 0 ) {
					return 101; /* note: a user-bug */
				}

				if( ar_iocr->data_length == mlen
				&&	cm_edd_can_autopadding(channel, (ar_iocr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK)))
				{
					if( ar_iocr->cm.partial_length < used_to ) {
						return 102; /* note: a user-bug */
					}
				}
				else {

					if( ar_iocr->cm.partial_length != ar_iocr->data_length ) {
						return 100; /* note: a user-bug */
					}
				}
			}
		}

		CM_AR_TRACE_06(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT,
				"iocr_ref(%u) is_consumer(%u) mlen(%u)  partial_offset(%u) partial_length(%u) apdu_status_length(%u)"
				, ar_iocr->iocr_ref
				, ar_iocr->cm.is_consumer
				, mlen
				, ar_iocr->cm.partial_offset
				, ar_iocr->cm.partial_length
				, ar_iocr->cm.apdu_status_length
			);
		}
	}
	}

	/***/

	return 0; /* ok */
}


/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_check_mcr_against_iocr (
	CM_AR_GRAPH_PTR_TYPE ar_graph
	)
{
	CM_LOG_FCT("cm_ar_graph_check_mcr_against_iocr")
	CM_AR_GRAPH_MCR_PTR_TYPE ar_mcr;

	for (CmListForeach (ar_mcr, & ar_graph->mcr_list, CM_AR_GRAPH_MCR_PTR_TYPE)) {
		CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

		ar_iocr = cm_ar_graph_find_io_cr (ar_graph, ar_mcr->iocr_ref);

		if( is_null(ar_iocr) ) {
			CM_AR_TRACE_01 (ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP,
					"no iocr-block for mcr-block with iocr_ref(%u)", ar_mcr->iocr_ref
				);
			return 4; /* no corresponding IOCR */
		}

		if( ar_iocr->iocr_type != CM_IOCR_TYPE_MULTICAST_CONSUMER ) {
			CM_AR_TRACE_02 (ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP,
					"wrong iocr-type(%u) for mcr-block with iocr_ref(%u)",
					ar_iocr->iocr_type, ar_mcr->iocr_ref
				);
			return 4; /* IOCR not m-consumer */
		}
	}
	return 0; /* ok */
}


/*----------------------------------------------------------------------------*/
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_ar_graph_check_iocr_against_mcr (
	CM_AR_GRAPH_PTR_TYPE ar_graph
	)
{
	CM_LOG_FCT("cm_ar_graph_check_iocr_against_mcr")
	CM_AR_GRAPH_IO_CR_PTR_TYPE  ar_iocr;

	for (CmListForeach (ar_iocr, & ar_graph->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE)) {
		CM_AR_GRAPH_MCR_PTR_TYPE  ar_mcr;

		if (ar_iocr->iocr_type != CM_IOCR_TYPE_MULTICAST_CONSUMER) {
			continue;
		}

		ar_mcr = cm_ar_graph_find_mcr (ar_graph, ar_iocr->iocr_ref);
		if (is_null (ar_mcr)) {
			CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "no mcrblock for m-consumer with iocr_ref(%u)", ar_iocr->iocr_ref
				);
			return 5; /* no corresponding MCR */
		}
	}
	return 0; /* ok */
}

/*----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS || CM_CFG_MAX_MULTICAST_CHANNELS

LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_cl_ar_graph_check_send_clock_adaption (
	CM_AR_GRAPH_PTR_TYPE ar_graph,
	LSA_UINT16 local_clock
) {
	CM_LOG_FCT("cm_cl_ar_graph_check_send_clock_adaption")
	CM_CHANNEL_PTR_TYPE  channel = ar_graph->ar_com.channel;

	CM_ASSERT (channel->detailptr->path_type == CM_PATH_TYPE_CLIENT || channel->detailptr->path_type == CM_PATH_TYPE_MULTICAST);

	CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "local_clock(%u) ar_send_clock(%u)"
		, local_clock, ar_graph->ar_com.ar_send_clock
		);

	/***/

	if( CmListIsEmpty(&ar_graph->io_cr_list) ) {

		/* no send-clock, no problem (easy-supervisor) */
	}
	else {

		LSA_BOOL is_power_of_two;
		LSA_BOOL has_adaption;
		LSA_UINT16  adpt_send_clock;
		LSA_BOOL  adpt_is_power_of_two;

#if 0
		if( local_clock == 0 ) {
			CM_FATAL(); /* a bug, see caller */
		}

		if( cm_edd_send_clock_range_ok(channel, local_clock, LSA_NULL/*only 2^x*/) ) {

			is_power_of_two = LSA_TRUE;
		}
		else {

			/* note: range is already checked, thus "not ok" means not 2^x */

			is_power_of_two = LSA_FALSE;
		}

#else
		if (! cm_edd_send_clock_range_ok (channel, local_clock, & is_power_of_two) ) {
			CM_FATAL(); /* a bug, see caller */
		}
#endif

		/***/

		has_adaption = LSA_FALSE;
		adpt_send_clock = 0;
		adpt_is_power_of_two = LSA_FALSE;

		{
		CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

		for (CmListForeach (ar_iocr, & ar_graph->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
		{
			if( ar_iocr->cm.adpt.send_clock != 0 )
			{
				has_adaption = LSA_TRUE;
				adpt_send_clock = ar_iocr->cm.adpt.send_clock;
				if (! cm_edd_send_clock_range_ok (channel, adpt_send_clock, & adpt_is_power_of_two) ) {
					CM_FATAL(); /* a bug, see caller */
				}
				break;
			}
		}
		}

		/***/

		if( has_adaption ) { /* local-adaption */

			if (adpt_send_clock != local_clock)
			{
				CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
					, "adpt.send_clock(%u) != local_clock(%u)"
					, adpt_send_clock, local_clock
					);
				return CM_PNIO_FAULTY_IOCRBlockReq(10); /* adpt.send_clock not local_clock */
			}
			else
			{
				CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

				for (CmListForeach (ar_iocr, & ar_graph->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE)) {

					if( ar_iocr->cm.adpt.send_clock != adpt_send_clock ) { /* cm-documentation: all or none */

						CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
							, "not all iocr-s have adaption-send_clock(%u) set", adpt_send_clock
							);
						return CM_PNIO_FAULTY_IOCRBlockReq(10); /* not all iocr-s have local-adaption set */
					}
				}

				ar_graph->ar_com.ar_send_clock = adpt_send_clock; /* == local_clock */
			}
		}
		else
		{
			CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR, "no local-adaption given");
			return CM_PNIO_FAULTY_IOCRBlockReq(10); /* no local-adaption given */
		}
	}

	/***/

	return CM_PNIO_ERR_NONE; /* ok */
}

#endif

/*----------------------------------------------------------------------------*/

static  LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_ar_graph_check_ir_info_block (
	CM_AR_GRAPH_PTR_TYPE  ar_graph
	)
{
	CM_LOG_FCT("cm_ar_graph_check_ir_info_block")

	if( ar_graph->ar.ar_type == CM_AR_TYPE_SINGLE_RTC3
		&& (ar_graph->ar.ar_properties & CM_AR_PROP_STARTUP_MODE_ADVANCED) != 0
	) {

		if (is_null (ar_graph->ir_info))
		{
			return CM_PNIO_FAULTY_IRInfoBlock(0); /* no IRInfoBlock */
		}
		else
		{
			CM_AR_GRAPH_IO_CR_PTR_TYPE first_iocr = LSA_NULL;
			CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE ir_info_sub;
			LSA_INT32 subCount = 0;

			CmListLength (&(ar_graph->ir_info->ir_info_sub_list), &subCount);

			if (!(subCount == 0 || subCount == 2))
			{
				return CM_PNIO_FAULTY_IRInfoBlock(8);
			}

			for (CmListForeach (ir_info_sub, &(ar_graph->ir_info->ir_info_sub_list), CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE))
			{
				LSA_UINT32 subframe_length = ((ir_info_sub->subframe_data >> 8) & 0xFF);
				CM_AR_GRAPH_IO_CR_PTR_TYPE iocr;

				iocr = cm_ar_graph_find_io_cr (ar_graph, ir_info_sub->iocr_ref);
				if (is_null (iocr))
				{
					return CM_PNIO_FAULTY_IRInfoBlock(8);
				}

				if (is_null (first_iocr))
				{
					first_iocr = iocr;
				}
				else
				{
					if (are_equal(first_iocr, iocr))
					{
						return CM_PNIO_FAULTY_IRInfoBlock(8);
					}
				}

				if (ir_info_sub->subframe_offset + 2 /* SFCRC16 */ + 2 /* SFEndDelimiter */ >= iocr->data_length)
				{
					CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "SubframeOffset >= DataLength - (2 /* SFCRC16 */ + 2 /* SFEndDelimiter */)"
						);

					return CM_PNIO_FAULTY_IRInfoBlock(9); /* SubframeOffset >= DataLength - (2 Byte SFCRC16 + 2 Byte SFEndDelimiter ) */
				}

				if ((ir_info_sub->subframe_data & 0x7F) == 0)
				{
					CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "SubframeData.Position == 0"
						);

					return CM_PNIO_FAULTY_IRInfoBlock(10); /* SubframeData.Position == 0 */
				}

				if (subframe_length == 0)
				{
					CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "SubframeData.DataLength == 0"
						);

					return CM_PNIO_FAULTY_IRInfoBlock(10); /* SubframeData.DataLength == 0 */
				}

				if (ir_info_sub->subframe_offset + subframe_length + 2 /* SFCRC16 */ + 2 /* SFEndDelimiter */ > iocr->data_length)
				{
					CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
						, "SubframeData.DataLength and SubframeOffset too big"
						);

					return CM_PNIO_FAULTY_IRInfoBlock(10); /* SubframeData.DataLength and SubframeOffset too big */
				}

				if( ar_graph->ar_com.path_type == CM_PATH_TYPE_SERVER )
				{
					iocr->cm.subframe_data   = ir_info_sub->subframe_data & 0xFF7F;
					iocr->cm.subframe_offset = ir_info_sub->subframe_offset;
				}
				else
				{
					if (iocr->cm.subframe_data   != (ir_info_sub->subframe_data & 0xFF7F)
					||	iocr->cm.subframe_offset != ir_info_sub->subframe_offset)
					{
						CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
							, "subframe_data, cm-user has not set correct, see cm_arcb_calc_iocr_buffer_size()"
							);
						return CM_PNIO_FAULTY_IRInfoBlock(0); /* cm-user parameter error */
					}

				}
			}
		}
	}
	else
	{
		if (is_not_null (ar_graph->ir_info))
		{
			return CM_PNIO_FAULTY_IRInfoBlock(0); /* IRInfoBlock exists */
		}
	}

	return 0;
}

/*----------------------------------------------------------------------------*/

LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_ar_graph_check_consistency(
	CM_AR_GRAPH_PTR_TYPE ar_graph
) {
	CM_LOG_FCT("cm_ar_graph_check_consistency")
	LSA_UINT16  block_field;

	/*
	 * an AR has
	 * - one or more "expected" submodules
	 * with one exception
	 * - easy-supervisor has none
	 */

	if( cm_is_iosar_with_device_access(ar_graph) ) {

		if( ! CmListIsEmpty(&ar_graph->ap_list) ) {

			CM_FATAL(); /* a bug, see cm_sv_argr_build() */
		}
	}
	else if( CmListIsEmpty(&ar_graph->ap_list) ) {

		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "no submodules"
			);

		return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq(0);
	}
	else {

		CM_AR_GRAPH_AP_PTR_TYPE ar_ap;

		for (CmListForeach(ar_ap, &ar_graph->ap_list, CM_AR_GRAPH_AP_PTR_TYPE)) {

			CM_AR_GRAPH_MODULE_PTR_TYPE ar_mod;

			CM_ASSERT( ! CmListIsEmpty(&ar_ap->mod_list)); /* see cm_ar_graph_insert_ap_module() */

			for (CmListForeach(ar_mod, &ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE)) {

				CM_AR_GRAPH_SUBMODULE_PTR_TYPE ar_sub;

				if( CmListIsEmpty(&ar_mod->submod_list) ) {

					CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
						, "slot(%u) but no submodules", ar_mod->slot_nr
						);

					return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq(9); /* number_of_submodules == 0 */
				}

				for (CmListForeach(ar_sub, &ar_mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE)) {

					/* assert only, see caller of cm_ar_graph_set_data_descr() */

					switch (ar_sub->soll.sub_properties & CM_SUB_PROP_TYPE_MASK) {
					case CM_SUB_PROP_TYPE_NO_DATA:
					case CM_SUB_PROP_TYPE_INPUT:
						CM_ASSERT(ar_sub->descr[0].data_length != 0xFFFF); /* set */
						CM_ASSERT(ar_sub->descr[1].data_length == 0xFFFF); /* not set */
						break;
					case CM_SUB_PROP_TYPE_OUTPUT:
						CM_ASSERT(ar_sub->descr[0].data_length == 0xFFFF); /* not set */
						CM_ASSERT(ar_sub->descr[1].data_length != 0xFFFF); /* set */
						break;
					case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
						CM_ASSERT(ar_sub->descr[0].data_length != 0xFFFF); /* set */
						CM_ASSERT(ar_sub->descr[1].data_length != 0xFFFF); /* set */
						break;
					default:
						CM_FATAL(); /* not possible */
						break;
					}

					/***/

					{
					CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;

					for (CmListForeach (ar_iocr, & ar_graph->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE)) {

						CM_AR_GRAPH_IOAPI_PTR_TYPE ar_ioapi;

						for (CmListForeach (ar_ioapi, & ar_iocr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE)) {

							if( ar_ioapi->api == ar_ap->api ) { /* api found */

								CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE ar_iodata;

								for (CmListForeach (ar_iodata, &ar_ioapi->related_iodata_objects_list, CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE)) {

									if(
										ar_mod->slot_nr == ar_iodata->slot_nr
										&& ar_sub->subslot_nr == ar_iodata->subslot_nr
									) {
										/* slot and subslot found */

										switch( ar_iocr->iocr_type ) { /* note: cannot use is_consumer here */
										case CM_IOCR_TYPE_INPUT:
											break;
										case CM_IOCR_TYPE_MULTICAST_PROVIDER:
											ar_sub->inp_via_mcr = LSA_TRUE;
											break;
										case CM_IOCR_TYPE_OUTPUT:
											break;
										case CM_IOCR_TYPE_MULTICAST_CONSUMER:
											ar_sub->out_via_mcr = LSA_TRUE;
											break;
										default:
											break;
										}
									}
								}
							}
						}
					}
					}
				}
			}
		}
	}

	/*
	 * an AR has
	 * - one or more input-crs
	 * - one or more output-crs
	 * with one exception
	 * - easy-supervisor has none
	 */

	if( cm_is_iosar_with_device_access(ar_graph) ) {

		if( ! CmListIsEmpty(&ar_graph->io_cr_list) ) {

			CM_FATAL(); /* a bug, see cm_sv_argr_build() */
		}

		CM_ASSERT(ar_graph->ar_com.ar_send_clock == 0);
	}
	else {

		CM_AR_GRAPH_IO_CR_PTR_TYPE ar_iocr;
		LSA_UINT16 inp = 0;
		LSA_UINT16 out = 0;

		for (CmListForeach (ar_iocr, & ar_graph->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE)) {

			/* note: cm_ar_graph_insert_io_cr() checks ARType vs. RTClass */

			switch( ar_iocr->iocr_type ) { /* note: cannot use "cm.is_consumer" here */

			case CM_IOCR_TYPE_INPUT:
				inp += 1;
				break;

			case CM_IOCR_TYPE_OUTPUT:
				out += 1;
				break;

			case CM_IOCR_TYPE_MULTICAST_PROVIDER:
			case CM_IOCR_TYPE_MULTICAST_CONSUMER:
				break;

			default:
				CM_FATAL();
				break;
			}

			/***/

			if( CmListIsEmpty(&ar_iocr->related_ap_list) ) {

				return CM_PNIO_FAULTY_IOCRBlockReq(19); /* no submodules in IOCR */
			}
			else {

				CM_AR_GRAPH_IOAPI_PTR_TYPE ar_ioapi;

				for (CmListForeach (ar_ioapi, & ar_iocr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE)) {

					if(
						CmListIsEmpty(&ar_ioapi->related_iodata_objects_list)
						&& CmListIsEmpty(&ar_ioapi->related_iocs_list)
					) {

						CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "neither related iodata nor iocs"
							);

						/* see PNIO-Spec, PDU checking rules:
						 *
						 * NumberOfIODataObjects == 0x0 AND NumberOfIOCS == 0x0
						 *   -> RSP-, 0xDB, 0x81, 0x02, 20
						 * NumberOfIOCS == 0x0 AND NumberOfIODataObjects == 0x0
						 *   -> RSP-, 0xDB, 0x81, 0x02, 25
						 *
						 * first: there seems to be a typo. the code2 20 should be 21
						 * second: these two table rows test the same thing
						 *
						 * since the second line gives us more details, we take this.
						 */

						return CM_PNIO_FAULTY_IOCRBlockReq(25); /* neither related iodata nor iocs */
					}
				}
			}
		}

		/***/

		if( inp == 0 || out == 0 ) {

			CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
				, "no input(%u) or no output(%u) iocr"
				, inp, out
				);

			return CM_PNIO_FAULTY_ConnectBlockReq(64/*RMPM*/, 2/*IOCR Missing*/);
		}
	}

	/*
	 * check IOCR/cons/multicast against MCRBlocks and vice versa
	 */

	block_field = cm_ar_graph_check_iocr_against_mcr(ar_graph);

	if( block_field != 0 ) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "check IOCR against MCR failed, block_field(%u)", block_field
			);
		return CM_PNIO_FAULTY_IOCRBlockReq(block_field);
	}

	block_field = cm_ar_graph_check_mcr_against_iocr (ar_graph);

	if( block_field != 0 ) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "check MCR against IOCR failed, block_field(%u)", block_field
			);
		return CM_PNIO_FAULTY_MCRBlockReq(block_field);
	}

	/*
	 * an RTC3-AR with "StartupMode == Advanced" has an IRInfoBlock (and the IRDataUUID is not NIL)
	 *
	 *	side effect: set iocr.cm.subframe_data, and iocr.cm.subframe_offset
	 */
	{
	LSA_UINT32 pnio_status = cm_ar_graph_check_ir_info_block (ar_graph);

	if( pnio_status != 0 ) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "check IRInfoBlock against IOCR failed, pnio_status(0x%x)", pnio_status
			);
		return pnio_status;
	}
	}

	/*
	 * check expected-config against iocr and vice versa
	 */

	block_field = cm_ar_graph_check_expsubmods_against_iocrs(ar_graph);

	if( block_field != 0 ) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "check EXPSUBMODS against IOCR failed, block_field(%u)", block_field
			);
		return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq(block_field);
	}

	block_field = cm_ar_graph_check_iocrs_against_expsubmods(ar_graph);

	if( block_field != 0 ) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "check IOCR against EXPSUBMODS failed, block_field(%u)", block_field
			);
		return CM_PNIO_FAULTY_IOCRBlockReq(block_field);
	}

	/*
	 * an AR has
	 * - exactly one alarm-cr
	 * with one exception
	 * - easy-supervisor has none
	 */

	if( cm_is_iosar_with_device_access(ar_graph) ) {

		if( is_not_null (ar_graph->alarm_cr) ) {

			CM_FATAL(); /* a bug, see cm_sv_argr_build() */
		}
	}
	else if( is_null (ar_graph->alarm_cr) ) {

		CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_UNEXP
			, "no alcr"
			);

		return CM_PNIO_FAULTY_ConnectBlockReq(64/*RMPM*/, 3/*Wrong AlarmCRBlock count*/);
	}

	/*
	 * a system-redundancy AR has an SRInfoBlock (and startup mode is advanced)
	 */

	if( ar_graph->ar.ar_type == CM_AR_TYPE_SINGLE_SYSRED ) {

		if( is_null(ar_graph->sr_info) ) {

			CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "ARType SYSRED needs an SRInfoBlock"
				);
			return CM_PNIO_FAULTY_ConnectBlockReq(64/*RMPM*/, 1/*Unknown Blocks*/); /* note: there is no code2 for "missing blocks" */
		}
	}
	else {

		if( is_not_null(ar_graph->sr_info) ) {

			CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_ERROR
				, "SRInfoBlock but not an ARType SYSRED"
				);
			return CM_PNIO_FAULTY_ConnectBlockReq(64/*RMPM*/, 3/*Wrong block count*/);
		}
	}

	/***/

	return CM_PNIO_ERR_NONE;
}

/*------------------------------------------------------------------------------
// free the ar-graph
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_graph_free (
	CM_AR_GRAPH_PTR_TYPE	ar_graph
) {
	CM_LOG_FCT ("cm_ar_graph_free")
	CM_CHANNEL_PTR_TYPE  channel = ar_graph->ar_com.channel;
	LSA_UINT16  ret_val;

	CM_AR_TRACE_03(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "free argr(%08X):  dev(%u) ar(%u)", ar_graph, ar_graph->device_nr, ar_graph->ar_nr
		);

	/***/

	cm_arcq_undo_init (ar_graph);

	/***/

	CM_ASSERT( ! cm_oneshot_is_running(&ar_graph->ar_com.timer));

	if (is_not_null(ar_graph->ar.cmi_station_name)) {
		CM_ASSERT(ar_graph->ar.cmi_station_name_length != 0);
		CM_FREE_LOCAL_MEM (& ret_val, ar_graph->ar.cmi_station_name);
		CM_ASSERT(ret_val == LSA_RET_OK);
	}

	/*** traverse the graph an free everything ***/

	{
	CM_AR_GRAPH_IO_CR_PTR_TYPE iocr;

	for (CmListForeach (iocr, &ar_graph->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE)) {

		CM_AR_GRAPH_IOAPI_PTR_TYPE iocr_ap;

		if (iocr->cm.acp_id != ACP_INVALID_ACP_ID)
		{
			CM_FATAL1(iocr->cm.acp_id);
		}

		if (iocr->cm.cr_state != CM_AR_CR_ALLOCATED) {

			cm_res_frame_free(channel
				, iocr->cm.is_consumer
				, iocr->cm.is_multicast
				, (iocr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK)
				, iocr->frame_id
				);
		}

		for (CmListForeach (iocr_ap, & iocr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE)) {
			cm_ar_graph_free_list_local_mem(&iocr_ap->related_iodata_objects_list);
			cm_ar_graph_free_list_local_mem(&iocr_ap->related_iocs_list);
		}

		cm_ar_graph_free_list_local_mem(&iocr->related_ap_list);
	}
	}

	cm_ar_graph_free_list_local_mem(&ar_graph->io_cr_list);

	/***/

	{
	CM_AR_GRAPH_MCR_PTR_TYPE mcr;

	for (CmListForeach (mcr, &ar_graph->mcr_list, CM_AR_GRAPH_MCR_PTR_TYPE)) {
		if( is_not_null(mcr->provider_name) ) {
			CM_FREE_LOCAL_MEM(&ret_val, mcr->provider_name);
			CM_ASSERT(ret_val == LSA_RET_OK);
		}
	}
	}

	cm_ar_graph_free_list_local_mem(& ar_graph->mcr_list);

	/***/

	{
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr = ar_graph->alarm_cr;
	ar_graph->alarm_cr = LSA_NULL;

	if( is_not_null(alarm_cr) ) { /* there is at most one (easy-supervisor has none) */

		cm_range_free_id (&channel->res.id_range[CM_RANGE_ID_ALARM_REF], alarm_cr->local_alarm_ref);

		CM_FREE_LOCAL_MEM (& ret_val, alarm_cr);
		CM_ASSERT(ret_val == LSA_RET_OK);
	}
	}

	/***/

	{
	CM_AR_GRAPH_IR_INFO_PTR_TYPE  ir_info = ar_graph->ir_info;
	ar_graph->ir_info = LSA_NULL;

	if( is_not_null(ir_info) ) {

		cm_ar_graph_free_list_local_mem(& ir_info->ir_info_sub_list);

		CM_FREE_LOCAL_MEM (& ret_val, ir_info);
		CM_ASSERT(ret_val == LSA_RET_OK);
	}
	}

	/***/

	{
	CM_AR_GRAPH_AP_PTR_TYPE ap;

	for (CmListForeach (ap, &ar_graph->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
	{
		CM_AR_GRAPH_MODULE_PTR_TYPE mod;

		for (CmListForeach (mod, &ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE))
		{
#if CM_CFG_MAX_CLIENTS
			CM_AR_GRAPH_SUBMODULE_PTR_TYPE sub;
			CM_AR_GRAPH_RECORD_PTR_TYPE rec;

			for (CmListForeach (sub, &mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
			{
				for (CmListForeach (rec, &sub->cl.prm_record_list, CM_AR_GRAPH_RECORD_PTR_TYPE)) 
				{
					if (is_not_null(rec->tailor_data))
					{
						CM_FREE_LOCAL_MEM (&ret_val, rec->tailor_data);
						CM_ASSERT (ret_val == LSA_RET_OK);

						rec->tailor_data = LSA_NULL;
					}
				}

				cm_ar_graph_free_list_local_mem (&sub->cl.prm_record_list);
			}
#endif
			cm_ar_graph_free_list_local_mem (&mod->submod_list);
		}

		cm_ar_graph_free_list_local_mem (&ap->mod_list);
	}

	cm_ar_graph_free_list_local_mem (&ar_graph->ap_list);
	}

	/***/

	{
	CM_AR_GRAPH_SR_INFO_PTR_TYPE sr_info = ar_graph->sr_info;

	if( is_not_null(sr_info) ) {

		ar_graph->sr_info = LSA_NULL;

		CM_FREE_LOCAL_MEM(&ret_val, sr_info);
		CM_ASSERT(ret_val == LSA_RET_OK);
	}
	}

	/***/

	cm_ar_graph_free_list_local_mem (&ar_graph->arvendor_list);

	/***/

	switch( ar_graph->ar_com.path_type ) {
#if CM_CFG_MAX_CLIENTS
	case CM_PATH_TYPE_CLIENT:
		cm_cl_ar_undo_init (ar_graph);
		break;
#endif
#if CM_CFG_MAX_MULTICAST_CHANNELS
	case CM_PATH_TYPE_MULTICAST:
		cm_mc_ar_undo_init (ar_graph);
		break;
#endif
#if CM_CFG_MAX_SERVERS
	case CM_PATH_TYPE_SERVER:
		/* see cm_sv_argr_undo_init_and_free() */
		break;
#endif
	default:
		break; /* unreachable */
	}

	/***/

	CM_FREE_LOCAL_MEM (& ret_val, ar_graph);
	CM_ASSERT(ret_val == LSA_RET_OK);
}


/*------------------------------------------------------------------------------
// find io_cr within a ar_graph
//----------------------------------------------------------------------------*/
CM_AR_GRAPH_IO_CR_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_io_cr (
	CM_AR_GRAPH_PTR_TYPE  ar_graph,
	LSA_UINT16  iocr_ref
) {
	CM_LOG_FCT ("cm_ar_graph_find_io_cr")
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;

	CM_ASSERT (is_not_null (ar_graph));

	for (CmListForeach (iocr, & ar_graph->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE)) {

		if (iocr->iocr_ref == iocr_ref) {
			return iocr;
		}
	}

	CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr with iocr_ref(%u) not found", iocr_ref
		);

	return LSA_NULL;
}

/*------------------------------------------------------------------------------
// find ir_info_sub block within a ar_graph
//----------------------------------------------------------------------------*/
CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_ir_info_sub (
	CM_AR_GRAPH_PTR_TYPE  ar_graph,
	LSA_UINT16  iocr_ref
) {
	CM_LOG_FCT ("cm_ar_graph_find_ir_info_sub")
	CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE  irInfoSub;

	CM_ASSERT (is_not_null (ar_graph));
	if (is_null (ar_graph->ir_info))
	{
		return LSA_NULL;
	}

	for (CmListForeach (irInfoSub, & ar_graph->ir_info->ir_info_sub_list, CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE)) {

		if (irInfoSub->iocr_ref == iocr_ref) {
			return irInfoSub;
		}
	}

	CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "iocr with iocr_ref(%u) not found", iocr_ref
		);

	return LSA_NULL;
}

/*------------------------------------------------------------------------------
// find ioapi within a io_cr
//----------------------------------------------------------------------------*/
CM_AR_GRAPH_IOAPI_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_ioapi (
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	LSA_UINT32  api
) {
	CM_LOG_FCT ("cm_ar_graph_find_ioapi")
	CM_AR_GRAPH_IOAPI_PTR_TYPE  ioapi;

	CM_ASSERT (is_not_null (iocr));

	for (CmListForeach (ioapi, & iocr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE)) {
		if (ioapi->api == api) {
			return ioapi;
		}
	}

	CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
		, "ioapi with api(%u) not found", api
		);

	return LSA_NULL;
}


/*------------------------------------------------------------------------------
// find iodata within a ioapi
//----------------------------------------------------------------------------*/
CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_iodata (
	CM_AR_GRAPH_IOAPI_PTR_TYPE  ioapi,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr
) {
	CM_LOG_FCT ("cm_ar_graph_find_iodata")
	CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE  iodata;

	CM_ASSERT (is_not_null (ioapi));

	for (CmListForeach (iodata, & ioapi->related_iodata_objects_list, CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE)) {
		if (iodata->slot_nr == slot_nr && iodata->subslot_nr == subslot_nr) {
			return iodata;
		}
	}

	CM_AR_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
		, "iodata for ap/sl/su(%u/%u/%u) not found", ioapi->api, slot_nr, subslot_nr
		);

	return LSA_NULL;
}


/*------------------------------------------------------------------------------
// find iocs within a ioapi
//----------------------------------------------------------------------------*/
CM_AR_GRAPH_IOCS_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_iocs (
	CM_AR_GRAPH_IOAPI_PTR_TYPE  ioapi,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr
) {
	CM_LOG_FCT ("cm_ar_graph_find_iocs")
	CM_AR_GRAPH_IOCS_PTR_TYPE  iocs;

	CM_ASSERT (is_not_null (ioapi));

	for (CmListForeach (iocs, & ioapi->related_iocs_list, CM_AR_GRAPH_IOCS_PTR_TYPE)) {
		if (iocs->slot_nr == slot_nr && iocs->subslot_nr == subslot_nr) {
			return iocs;
		}
	}

	CM_AR_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
		, "iocs for ap/sl/su(%u/%u/%u) not found", ioapi->api, slot_nr, subslot_nr
		);

	return LSA_NULL;
}


/*------------------------------------------------------------------------------
// find mcr within a ar_graph
//----------------------------------------------------------------------------*/
CM_AR_GRAPH_MCR_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_mcr (
	CM_AR_GRAPH_PTR_TYPE  ar_graph,
	LSA_UINT16  iocr_ref
) {
	CM_LOG_FCT ("cm_ar_graph_find_mcr")
	CM_AR_GRAPH_MCR_PTR_TYPE  mcr;

	CM_ASSERT (is_not_null (ar_graph));

	for (CmListForeach (mcr, & ar_graph->mcr_list, CM_AR_GRAPH_MCR_PTR_TYPE)) {
		if (mcr->iocr_ref == iocr_ref) {
			return mcr;
		}
	}

	CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "mcr with iocr_ref(%u) not found", iocr_ref
		);

	return LSA_NULL;
}


/*------------------------------------------------------------------------------
// find alarm_cr within a ar_graph
//----------------------------------------------------------------------------*/
CM_AR_GRAPH_ALARM_CR_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_alarm_cr (
	CM_AR_GRAPH_PTR_TYPE  ar_graph
) {
	CM_LOG_FCT ("cm_ar_graph_find_alarm_cr")

	CM_ASSERT (is_not_null (ar_graph));

	if (is_not_null (ar_graph->alarm_cr)) {
		return ar_graph->alarm_cr;
	}

	CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "alarm-cr not found"
		);

	return LSA_NULL;
}


/*------------------------------------------------------------------------------
// find 'ap' within a ar_graph
//----------------------------------------------------------------------------*/
CM_AR_GRAPH_AP_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_ap (
	CM_AR_GRAPH_PTR_TYPE  ar_graph,
	LSA_UINT32  api
) {
	CM_LOG_FCT ("cm_ar_graph_find_ap")
	CM_AR_GRAPH_AP_PTR_TYPE  ar_ap;

	CM_ASSERT (is_not_null (ar_graph));

	for (CmListForeach (ar_ap, & ar_graph->ap_list, CM_AR_GRAPH_AP_PTR_TYPE)) {
		if (ar_ap->api == api) {
			return ar_ap;
		}
	}

	CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "api(%u) not found", api
		);

	return LSA_NULL;
}


/*------------------------------------------------------------------------------
// find module within a ap
//----------------------------------------------------------------------------*/
CM_AR_GRAPH_MODULE_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_module (
	CM_AR_GRAPH_AP_PTR_TYPE  ar_ap,
	LSA_UINT16  slot_nr
) {
	CM_LOG_FCT ("cm_ar_graph_find_module")
	CM_AR_GRAPH_MODULE_PTR_TYPE  ar_mod;

	CM_ASSERT (is_not_null (ar_ap));

	for (CmListForeach (ar_mod, & ar_ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE)) {
		if (ar_mod->slot_nr == slot_nr) {
			return ar_mod;
		}
	}

	CM_AR_TRACE_01(0, LSA_TRACE_LEVEL_CHAT
		, "slot_nr(%u) not found", slot_nr
		);

	return LSA_NULL;
}


/*------------------------------------------------------------------------------
// find submodule within a module
//----------------------------------------------------------------------------*/
CM_AR_GRAPH_SUBMODULE_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_submodule (
	CM_AR_GRAPH_AP_PTR_TYPE  ap, /* in: if ap == NULL then mod must specified */
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod, /* in: mod or slot_nr must specified */
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr
) {
	CM_LOG_FCT ("cm_ar_graph_find_submodule")
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

	CM_ASSERT (! (is_null (ap) && is_null (mod)));

	if (is_null (mod)) {
		if (is_null (ap)) {
			CM_FATAL(); /* a bug */
			return LSA_NULL;
		}
		mod = cm_ar_graph_find_module (ap, slot_nr);
	}

	if (is_not_null (mod)) {
		for (CmListForeach (sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE)) {
			if (sub->subslot_nr == subslot_nr) {
				return sub;
			}
		}
	}

	CM_AR_TRACE_03(0, LSA_TRACE_LEVEL_CHAT
		, "mod(%08X) slot_nr(%u) subslot_nr(%u) not found"
		, mod, slot_nr, subslot_nr
		);

	return LSA_NULL;
}

/*------------------------------------------------------------------------------
// find api, slot, subslot
//----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_graph_find_api_slot_subslot (
	CM_AR_GRAPH_PTR_TYPE  ar_graph,
	LSA_UINT32  api,
	LSA_UINT16  slot_nr,
	LSA_UINT16  subslot_nr,
	CM_AR_GRAPH_AP_PTR_TYPE  CM_LOCAL_MEM_ATTR * ap_pp,
	CM_AR_GRAPH_MODULE_PTR_TYPE  CM_LOCAL_MEM_ATTR * mod_pp,
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  CM_LOCAL_MEM_ATTR * sub_pp
)
{
	CM_LOG_FCT("cm_ar_graph_find_api_slot_subslot")
	CM_AR_GRAPH_AP_PTR_TYPE  ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE  mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;

	if (is_not_null (ap_pp)) {
		*ap_pp = LSA_NULL;
	}
	if (is_not_null (mod_pp)) {
		*mod_pp = LSA_NULL;
	}
	if (is_not_null (sub_pp)) {
		*sub_pp = LSA_NULL;
	}

	sub = LSA_NULL;
	mod = LSA_NULL;
	ap = cm_ar_graph_find_ap (ar_graph, api);
	if (is_not_null (ap))
	{
		mod = cm_ar_graph_find_module (ap, slot_nr);
		if (is_not_null (mod) && is_not_null (sub_pp))
		{
			sub = cm_ar_graph_find_submodule (ap, mod, slot_nr, subslot_nr);
		}
	}
	if (is_not_null (ap_pp))
	{
		if (is_null (ap)) {
			CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH, "api(%u) not found", api);
			return;
		}
		*ap_pp = ap;
	}
	if (is_not_null (mod_pp))
	{
		if (is_null (mod)) {
			CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH, "api(%u), slot(%u) not found", api, slot_nr);
			return;
		}
		*mod_pp = mod;
	}
	if (is_not_null (sub_pp))
	{
		if (is_null (sub)) {
			CM_AR_TRACE_03(ar_graph->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH, "api(%u) slot(%u), subslot(%u) not found", api, slot_nr, subslot_nr);
			return;
		}
		*sub_pp = sub;
	}
}


/*------------------------------------------------------------------------------
// dump data of an ar-graph
//----------------------------------------------------------------------------*/

#if CM_DEBUG

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_graph_dump (
	CM_AR_GRAPH_PTR_TYPE  ar_graph
)
{
	CM_LOG_FCT ("cm_ar_graph_dump")

	CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "AR_GRAPH: arg(%08X), begin of dump", ar_graph
		);
	CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "  device_nr(%u) <=>  dev(%u)", ar_graph->device_nr, ar_graph->device_nr
		);
	CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "  ar_nr(%u) <=> ar(%u)", ar_graph->ar_nr, ar_graph->ar_nr
		);
	CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "  session_key(%u)", ar_graph->session_key
		);
#if CM_CFG_MAX_SERVERS
	if (ar_graph->ar_com.path_type == CM_PATH_TYPE_SERVER) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  cm.sv.session_key_connect(%u)", ar_graph->cm.sv.session_key_connect
			);
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  cm.sv.nr_of_submodules(%u)", ar_graph->cm.sv.nr_of_submodules
			);
	}
#endif
	CM_AR_TRACE_03(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "  ar_properties(0x%x)  ar_type(%u)  -  ar_send_clock(%u)"
		, ar_graph->ar.ar_properties, ar_graph->ar.ar_type
		, ar_graph->ar_com.ar_send_clock
		);
	CM_AR_TRACE_11(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "  ar_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, ar_graph->ar.ar_uuid.time_low, ar_graph->ar.ar_uuid.time_mid, ar_graph->ar.ar_uuid.time_hi_and_version
		, ar_graph->ar.ar_uuid.clock_seq_hi_and_reserved, ar_graph->ar.ar_uuid.clock_seq_low
		, ar_graph->ar.ar_uuid.node[0], ar_graph->ar.ar_uuid.node[1], ar_graph->ar.ar_uuid.node[2]
		, ar_graph->ar.ar_uuid.node[3], ar_graph->ar.ar_uuid.node[4], ar_graph->ar.ar_uuid.node[5]
		);
	CM_AR_TRACE_04(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "  host_ip(%u.%u.%u.%u)"
		, (LSA_UINT8)(CM_NTOH32(ar_graph->ar.host_ip) >> 24)
		, (LSA_UINT8)(CM_NTOH32(ar_graph->ar.host_ip) >> 16)
		, (LSA_UINT8)(CM_NTOH32(ar_graph->ar.host_ip) >> 8)
		, (LSA_UINT8)(CM_NTOH32(ar_graph->ar.host_ip) >> 0)
		);
	CM_AR_TRACE_06(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "  eth_mac(%02x-%02x-%02x-%02x-%02x-%02x)"
		, ar_graph->ar.eth_mac.mac[0], ar_graph->ar.eth_mac.mac[1], ar_graph->ar.eth_mac.mac[2]
		, ar_graph->ar.eth_mac.mac[3], ar_graph->ar.eth_mac.mac[4], ar_graph->ar.eth_mac.mac[5]
		);
	CM_AR_TRACE_11(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "  cmi_obj_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
		, ar_graph->ar.cmi_obj_uuid.time_low, ar_graph->ar.cmi_obj_uuid.time_mid, ar_graph->ar.cmi_obj_uuid.time_hi_and_version
		, ar_graph->ar.cmi_obj_uuid.clock_seq_hi_and_reserved, ar_graph->ar.cmi_obj_uuid.clock_seq_low
		, ar_graph->ar.cmi_obj_uuid.node[0], ar_graph->ar.cmi_obj_uuid.node[1], ar_graph->ar.cmi_obj_uuid.node[2]
		, ar_graph->ar.cmi_obj_uuid.node[3], ar_graph->ar.cmi_obj_uuid.node[4], ar_graph->ar.cmi_obj_uuid.node[5]
		);
	CM_AR_TRACE_05(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "  cmi_timeout100ms(%u)  cmi_udp_port(%u 0x%x)  cmr_udp_port(%u 0x%x)"
		, ar_graph->ar.cmi_timeout100ms
		, CM_NTOH16(ar_graph->ar.cmi_udp_port), CM_NTOH16(ar_graph->ar.cmi_udp_port)
		, CM_NTOH16(ar_graph->ar.cmr_udp_port), CM_NTOH16(ar_graph->ar.cmr_udp_port)
		);

	CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "  cmi_station_name_length(%u) name(not traced)"
		, ar_graph->ar.cmi_station_name_length
		);

	{
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr;

	for (CmListForeach (io_cr, &ar_graph->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE)) {

		CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "IO_CR: io_cr(%08X)  iocr_ref(%u)", io_cr, io_cr->iocr_ref
			);
		CM_AR_TRACE_03(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  is_cons(%u) is_multi(%u) acp_id(%u)"
			, io_cr->cm.is_consumer, io_cr->cm.is_multicast, io_cr->cm.acp_id
			);
		CM_AR_TRACE_03(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  iocr_type(%u) iocr_properties(0x%x) ethertype(0x%x)"
			, io_cr->iocr_type, io_cr->iocr_properties, io_cr->ethertype
			);
		CM_AR_TRACE_04(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  data_length(%u)  frame_id(%u 0x%x)  frame_send_offset(0x%x)"
			, io_cr->data_length, io_cr->frame_id, io_cr->frame_id, io_cr->frame_send_offset
			);
		CM_AR_TRACE_04(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  clock(%u)  ratio(%u)  phase(%u)  data_hold_factor(%u)"
			, io_cr->send_clock, io_cr->reduction_ratio, io_cr->phase, io_cr->data_hold_factor
			);
		CM_AR_TRACE_05(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  adaption: clock(%u)  ratio(%u)  phase(%u)  sequence(%u)  data_hold_factor(%u)"
			, io_cr->cm.adpt.send_clock, io_cr->cm.adpt.reduction_ratio, io_cr->cm.adpt.phase, io_cr->cm.adpt.sequence, io_cr->cm.adpt.data_hold_factor
			);
		CM_AR_TRACE_06(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  mac_addr(%02x-%02x-%02x-%02x-%02x-%02x)"
			, io_cr->multicast_mac.mac[0], io_cr->multicast_mac.mac[1], io_cr->multicast_mac.mac[2]
			, io_cr->multicast_mac.mac[3], io_cr->multicast_mac.mac[4], io_cr->multicast_mac.mac[5]
			);
		CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  edd_data_offset(%u)  edd_properties(0x%x)"
			, io_cr->cm.edd_data_offset, io_cr->cm.edd_properties
			);
		CM_AR_TRACE_03(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  partial_length(%u)  partial_offset(%u)  apdu_status_length(%u)"
			, io_cr->cm.partial_length, io_cr->cm.partial_offset, io_cr->cm.apdu_status_length
			);

		{
		CM_AR_GRAPH_IOAPI_PTR_TYPE iocr_ap;

		for (CmListForeach (iocr_ap, & io_cr->related_ap_list, CM_AR_GRAPH_IOAPI_PTR_TYPE)) {
			CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE iodata;
			CM_AR_GRAPH_IOCS_PTR_TYPE iocs;

			CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "  RELATED AP"
				);
			CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "    api(%u 0x%x)", iocr_ap->api, iocr_ap->api
				);

			CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "    RELATED IODATA OBJECTS"
				);
			for (CmListForeach (iodata, &iocr_ap->related_iodata_objects_list, CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE)) {
				CM_AR_TRACE_03(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "      slot_nr(%u)  subslot_nr(%u) frame_offset(%u)"
					, iodata->slot_nr, iodata->subslot_nr, iodata->frame_offset
					);
			}

			CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "    RELATED IOCS"
				);

			for (CmListForeach (iocs, &iocr_ap->related_iocs_list, CM_AR_GRAPH_IOCS_PTR_TYPE)) {
				CM_AR_TRACE_03(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "      slot_nr(%u) subslot_nr(%u) frame_offset(%u)"
					, iocs->slot_nr, iocs->subslot_nr, iocs->frame_offset
					);
			}
		}
		}
	}
	}

	{
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr = ar_graph->alarm_cr;

	if (is_not_null (alarm_cr))
	{
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "AL_CR: alarm_cr(0x%x)", alarm_cr
			);
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  alcr_type(%u)", alarm_cr->alcr_type
			);
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  alcr_properties(0x%x)", alarm_cr->alcr_properties
			);
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  timeout100ms(%u)", alarm_cr->timeout100ms
			);
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  retries(%u)", alarm_cr->retries
			);
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  local_alarm_ref(%u)", alarm_cr->local_alarm_ref
			);
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  remote_alarm_ref(%u)", alarm_cr->remote_alarm_ref
			);
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  max_alarm_data_len(%u)", alarm_cr->max_alarm_data_len
			);
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  ethertype(0x%x)", alarm_cr->ethertype
			);
	}
	}

	{
	CM_AR_GRAPH_AP_PTR_TYPE  ap;

	for (CmListForeach(ap, &ar_graph->ap_list, CM_AR_GRAPH_AP_PTR_TYPE)) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "AP: ap(%08X)", ap
			);
		CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  api(%u 0x%x)", ap->api, ap->api
			);

		{
		CM_AR_GRAPH_MODULE_PTR_TYPE  module;

		for (CmListForeach(module, &ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE)) {
			CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "  MODULE: mod(%08X)", module
				);
			CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "    slot_nr(%u)", module->slot_nr
				);
			CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "    soll.mod_ident(%u 0x%x)", module->soll.mod_ident, module->soll.mod_ident
				);
			CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "    soll.mod_prop(0x%x)", module->soll.mod_properties
				);
#if CM_CFG_MAX_CLIENTS
			CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "    ist.mod_ident(%u 0x%x)", module->ist.mod_ident, module->ist.mod_ident
				);
#endif

			{
			CM_AR_GRAPH_SUBMODULE_PTR_TYPE  submodule;

			for (CmListForeach(submodule, &module->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE)) {
				CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "    SUBMODULE: sub(%08X)", submodule
					);
				CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "      subslot_nr(%u/0x%x)", submodule->subslot_nr, submodule->subslot_nr
					);
				CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "      soll.sub_ident(%u 0x%x)", submodule->soll.sub_ident, submodule->soll.sub_ident
					);
				CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "      soll.sub_prop(0x%x)", submodule->soll.sub_properties
					);
#if CM_CFG_MAX_CLIENTS
				CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "      ist.sub_ident(%u 0x%x)", submodule->ist.sub_ident, submodule->ist.sub_ident
					);
				CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
					, "      ist.sub_state(0x%x)", submodule->ist.sub_state
					);
#endif
				switch (submodule->soll.sub_properties & CM_SUB_PROP_TYPE_MASK) {
				case CM_SUB_PROP_TYPE_NO_DATA:
				case CM_SUB_PROP_TYPE_INPUT:
				case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
					CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "      descr[0].data_description(0x%x)", submodule->descr[0].data_description
						);
					CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "      descr[0].data_length(%u)", submodule->descr[0].data_length
						);
					CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "      descr[0].iops_length(%u)", submodule->descr[0].iops_length
						);
					CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "      descr[0].iocs_length(%u)", submodule->descr[0].iocs_length
						);
					break;
				default:
					break;
				}
				switch (submodule->soll.sub_properties & CM_SUB_PROP_TYPE_MASK) {
				case CM_SUB_PROP_TYPE_OUTPUT:
				case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
					CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "      descr[1].data_description(0x%x)", submodule->descr[1].data_description
						);
					CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "      descr[1].data_length(%u)", submodule->descr[1].data_length
						);
					CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "      descr[1].iops_length(%u)", submodule->descr[1].iops_length
						);
					CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
						, "      descr[1].iocs_length(%u)", submodule->descr[1].iocs_length
						);
					break;
				default:
					break;
				}

#if CM_CFG_MAX_CLIENTS
				if (ar_graph->ar_com.path_type == CM_PATH_TYPE_CLIENT)
				{
					CM_AR_GRAPH_RECORD_PTR_TYPE  record;

					for (CmListForeach(record, &submodule->cl.prm_record_list, CM_AR_GRAPH_RECORD_PTR_TYPE)) {
						CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
							, "    RECORD: rec(%08X)", record
							);
						CM_AR_TRACE_03(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
							, "      index(%u 0x%x) transfer_sequence(%u)", record->record_index, record->record_index, record->transfer_sequence
							);
						CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
							, "      data_len(%u)", record->record_data_len
							);
						if (record->record_data_len > 0) {
							CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
								, "      data[0](0x%x)", record->record_data[0]
								);
						}
					}
				}
#endif
			}
			}
		}
		}
	}
	}

	{
	CM_AR_GRAPH_MCR_PTR_TYPE  mcr;

	for (CmListForeach (mcr, & ar_graph->mcr_list, CM_AR_GRAPH_MCR_PTR_TYPE)) {
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "MCR: mcr(%08X)", mcr
			);
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "   iocr_ref(%u)", mcr->iocr_ref
			);
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "   mcr_properties(0x%x)", mcr->mcr_properties
			);
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "   mci_timeout_100ms(%u)", mcr->mci_timeout_100ms
			);
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "   provider_name_length(%u)", mcr->provider_name_length
			);
	}
	}

	if (is_not_null (ar_graph->ir_info))
	{
		CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE  irisub;

		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "IRInfoBlock: ir_info(0x%08x)"
			, ar_graph->ir_info
			);
		CM_AR_TRACE_11(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  ir_data_uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
			, ar_graph->ir_info->ir_data_uuid.time_low, ar_graph->ir_info->ir_data_uuid.time_mid, ar_graph->ir_info->ir_data_uuid.time_hi_and_version
			, ar_graph->ir_info->ir_data_uuid.clock_seq_hi_and_reserved, ar_graph->ir_info->ir_data_uuid.clock_seq_low
			, ar_graph->ir_info->ir_data_uuid.node[0], ar_graph->ir_info->ir_data_uuid.node[1], ar_graph->ir_info->ir_data_uuid.node[2]
			, ar_graph->ir_info->ir_data_uuid.node[3], ar_graph->ir_info->ir_data_uuid.node[4], ar_graph->ir_info->ir_data_uuid.node[5]
			);

		for (CmListForeach (irisub, & ar_graph->ir_info->ir_info_sub_list, CM_AR_GRAPH_IR_INFO_SUB_PTR_TYPE))
		{
			CM_AR_TRACE_03(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "  iocr_ref(%u)  subframe_offset(%u)  subframe_data(0x%x)"
				, irisub->iocr_ref, irisub->subframe_offset, irisub->subframe_data
				);
		}
	}

	if (is_not_null (ar_graph->sr_info))
	{
		CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "SRInfoBlock: sr_info(0x%08x)"
			, ar_graph->sr_info
			);
		CM_AR_TRACE_02(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  redundancy_data_hold_factor(%u)  sr_properties(0x%x)"
			, ar_graph->sr_info->rdh_factor, ar_graph->sr_info->sr_properties
			);
	}

	{
	CM_AR_GRAPH_ARVENDOR_PTR_TYPE  arvendor;

	for (CmListForeach (arvendor, & ar_graph->arvendor_list, CM_AR_GRAPH_ARVENDOR_PTR_TYPE))
	{
		CM_AR_TRACE_01 (ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "ARVendorBlock: arvendor(0x%08x)"
			, arvendor
			);
		CM_AR_TRACE_04 (ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "  api(0x%x) ap_structure_identifier(0x%x) req_data_len(%u) res_data_len(%u)"
			, arvendor->api, arvendor->ap_structure_identifier, arvendor->req_data_len, arvendor->res_data_len
			);
	}
	}

#if CM_CFG_MAX_CLIENTS
	if (ar_graph->ar_com.path_type == CM_PATH_TYPE_CLIENT)
	{
		CM_AR_GRAPH_SLOT_NR_PTR_TYPE  slot_nr;

		if (! CmListIsEmpty (& ar_graph->cm.cl.unique_slot_nr_list)) {
			CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "UNIQUE_SLOT_NR:"
				);
		}
		for (CmListForeach (slot_nr, & ar_graph->cm.cl.unique_slot_nr_list, CM_AR_GRAPH_SLOT_NR_PTR_TYPE)) {
			CM_AR_TRACE_01(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "  slot_nr(%u)", slot_nr->slot_nr
				);
		}
	}
#endif

	CM_AR_TRACE_00(ar_graph->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "AR_GRAPH: end of dump"
		);
}

#endif

/*===========================================================================*/

#else
#  ifdef CM_MESSAGE
#    pragma CM_MESSAGE ("compiled _WITHOUT_ ACP; CM_CFG_USE_ACP=" CM_STRINGIFY(CM_CFG_USE_ACP))
#  endif /* CM_MESSAGE */
#endif /* CM_CFG_USE_ACP */


/*------------------------------------------------------------------------------
// used from PDev
//----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
// free a list
//----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_graph_free_list_local_mem(
	CM_LIST_ENTRY_PTR_TYPE	list
) {
	CM_LOG_FCT ("cm_ar_graph_free_list_local_mem")
	CM_LIST_ENTRY_PTR_TYPE item;
	LSA_UINT16 ret_val;

	while( ! CmListIsEmpty(list) ) {

#if 0 /* moved vars out of loop: workaround for bug of the tricore-compiler */
		CM_LIST_ENTRY_PTR_TYPE item;
		LSA_UINT16 ret_val;
#endif

		CmListRemoveHead(list, item, CM_LIST_ENTRY_PTR_TYPE);

		CM_FREE_LOCAL_MEM (& ret_val, item);
		CM_ASSERT(ret_val == LSA_RET_OK);
	}
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
