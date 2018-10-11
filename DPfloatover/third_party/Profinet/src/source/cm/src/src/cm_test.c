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
/*  F i l e               &F: cm_test.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  some internal test functions                                             */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  9
#define CM_MODULE_ID       9

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if 0 /* tests disabled, must not depend on a foreign #define (AP01476188) */

/* note: entry point is cm_run_test_cases(), to be called after cm_init() */

/*----------------------------------------------------------------------------*/

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("warning: TEST-CASES configured!")
#endif

/*----------------------------------------------------------------------------*/

#define CM_XLOG_01   CM_UPPER_TRA##CE_01 /* to hide 'malformed traces' from LTRC-scanner (AP01366017) */
#define CM_SLOG(s_)  CM_XLOG_01(0, LSA_TRACE_LEVEL_CHAT, "%u  struct:" #s_ , sizeof(s_));
#define CM_DLOG(s_)  CM_XLOG_01(0, LSA_TRACE_LEVEL_CHAT, "%u  define:" #s_ , s_);

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_test_log_size (LSA_VOID)
{
	CM_LOG_FCT ("cm_test_log_size")

	CM_DLOG (CM_CFG_MAX_NICS);

	CM_DLOG (CM_CFG_MAX_CLIENTS);

	CM_DLOG (CM_CFG_MAX_SERVERS);

	CM_DLOG (CM_CFG_MAX_MULTICAST_CHANNELS);

	CM_DLOG (CM_MAX_CHANNELS);

	/* conditional see beneath */

	CM_SLOG (cm_channels);

	CM_SLOG (CM_RQB_TYPE);
	CM_SLOG (CM_ANY_ARGS_TYPE);

	CM_SLOG (CM_CHANNEL_TYPE);
	CM_SLOG (cm_channels[0].usr);
	CM_SLOG (cm_channels[0].usr.pd);
#if CM_CFG_MAX_CLIENTS
	CM_SLOG (cm_channels[0].usr.cl);
#endif
#if CM_CFG_MAX_MULTICAST_CHANNELS
	CM_SLOG (cm_channels[0].usr.mc);
#endif
#if CM_CFG_MAX_SERVERS
	CM_SLOG (cm_channels[0].usr.sv);
#endif
	CM_SLOG (cm_channels[0].res);
	CM_SLOG (cm_channels[0].lower);
	CM_SLOG (cm_channels[0].lower.info);
	CM_SLOG (cm_channels[0].lower.info.acp);
	CM_SLOG (cm_channels[0].lower.info.edd);
#if CM_CFG_USE_GSY
	CM_SLOG (cm_channels[0].lower.info.gsy);
#endif
#if CM_CFG_USE_POF
	CM_SLOG (cm_channels[0].lower.info.pof);
#endif
	CM_SLOG (cm_channels[0].lower.info.oha);

	CM_SLOG (CM_DETAIL_TYPE);
	CM_SLOG (CM_INIT_TYPE);

	CM_SLOG (CM_BLK_HEADER);
	CM_SLOG (CM_REQ_AR);
	CM_SLOG (CM_RSP_AR);
	CM_SLOG (CM_REQ_IOCR1);
	CM_SLOG (CM_REQ_IOCR2);
	CM_SLOG (CM_REQ_IOCR3);
	CM_SLOG (CM_REQ_IOCR4);
	CM_SLOG (CM_REQ_IOCR5);
	CM_SLOG (CM_RSP_IOCR);
	CM_SLOG (CM_REQ_MCR);
	CM_SLOG (CM_REQ_ALCR);
	CM_SLOG (CM_RSP_ALCR);
	CM_SLOG (CM_REQ_API);
	CM_SLOG (CM_REQ_MOD);
	CM_SLOG (CM_REQ_SUB);
	CM_SLOG (CM_REQ_DSC);
	CM_SLOG (CM_RSP_DIFF);
	CM_SLOG (CM_RSP_API);
	CM_SLOG (CM_RSP_MOD);
	CM_SLOG (CM_RSP_SUB);
	CM_SLOG (CM_REQ_PRMSRV);
	CM_SLOG (CM_BLK_CTRL);
	CM_SLOG (CM_BLK_READ_WRITE);

	CM_SLOG (CM_PDU_LOG_ENTRY);

	CM_SLOG (CM_ARCB_ADD_TYPE);

	CM_SLOG (CM_ARCB_IOCR_TYPE);
	CM_SLOG (CM_ARCB_IOAPI_TYPE);
	CM_SLOG (CM_ARCB_IODATA_OBJECT_TYPE);
	CM_SLOG (CM_ARCB_IOCS_TYPE);

	CM_SLOG (CM_ARCB_AP_TYPE);
	CM_SLOG (CM_ARCB_MODULE_TYPE);
	CM_SLOG (CM_ARCB_SUBMODULE_TYPE);
#if CM_CFG_MAX_CLIENTS
	CM_SLOG (CM_ARCB_RECORD_TYPE);
#endif

	CM_SLOG (struct cm_ar_graph_tag);

	CM_SLOG (CM_AR_GRAPH_AR_TYPE);

	CM_SLOG (struct cm_ar_graph_io_cr_tag);
	CM_SLOG (CM_ARCB_IOAPI_TYPE);
	CM_SLOG (CM_ARCB_IODATA_OBJECT_TYPE);
	CM_SLOG (CM_ARCB_IOCS_TYPE);

	CM_SLOG (struct cm_ar_graph_alarm_cr_tag);

	CM_SLOG (struct cm_ar_graph_ap_tag);
	CM_SLOG (struct cm_ar_graph_module_tag);
	CM_SLOG (struct cm_ar_graph_submodule_tag);

	CM_SLOG (struct cm_ar_graph_mcr_tag);

	CM_SLOG (CM_ARCB_IR_INFO_TYPE);

	CM_SLOG (CM_ARCB_SR_INFO_TYPE);

#if CM_CFG_MAX_CLIENTS
	CM_DLOG (CM_CFG_MAX_CL_DEVICES);
	CM_DLOG (CM_CFG_MAX_CL_LOG_ENTRIES);
	CM_DLOG (CM_CFG_MAX_CL_IMPLICIT_READ);

	CM_SLOG (CM_CL_CHANNEL_TYPE);
	CM_SLOG (CM_CL_DEVICE_TYPE);
	CM_SLOG (CM_CL_ARGR_TYPE);
	CM_SLOG (struct cm_ar_graph_record_tag);
	CM_SLOG (struct cm_ar_graph_slot_nr_tag);
#endif

#if CM_CFG_MAX_SERVERS
	CM_DLOG (CM_CFG_MAX_SV_DEVICES);
	CM_DLOG (CM_CFG_MAX_SV_ARS);
	CM_DLOG (CM_CFG_MAX_SV_LOG_ENTRIES);

	CM_SLOG (CM_SV_CHANNEL_TYPE);
	CM_SLOG (CM_SV_DEVICE_TYPE);
	CM_SLOG (CM_SV_LOG_ENTRY_TYPE);
	CM_SLOG (CM_SV_AP_TYPE);
	CM_SLOG (CM_SV_SLOT_TYPE);
	CM_SLOG (CM_SV_SUBSLOT_TYPE);
	CM_SLOG (CM_SV_DIAG_TYPE);
	CM_SLOG (CM_SV_ARGR_TYPE);
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
	CM_DLOG (CM_CFG_MAX_MC_DEVICES);

	CM_SLOG (CM_MC_CHANNEL_TYPE);
	CM_SLOG (CM_MC_DEVICE_TYPE);
#endif
}


/*----------------------------------------------------------------------------*/

static LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_test_strlen(LSA_UINT8 *str) /* AP01315376 */
{
	LSA_UINT16 len = 0;

	while( *str != 0 ) {
		str += 1;
		len += 1;
	}

	return len;
}


/*----------------------------------------------------------------------------*/

static LSA_VOID  CM_LOCAL_FCT_ATTR
cm_test_station_name (LSA_VOID)
{
	CM_LOG_FCT ("cm_test_station_name")

	/* test-cases for station-name validation */
	LSA_UINT8 * s;
	LSA_BOOL ok;

	s = (LSA_UINT8*)"";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"";
	ok = cm_is_valid_station_name(10, LSA_NULL);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"A"; /* case-insensitive!!! (V3.2i2 or later) */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"ABCDEFGHIJKLMNOPQRSTUVWXYZ.abcdefghijklmnopqrstuvwxyz"; /* allow upper case for compatibility */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)".";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)".a";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"-";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"-a";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"a-";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"a-.b";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"a.";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"a..";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"a.-";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaz"; /*64*/
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"b.aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaz";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"b.aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaz.c";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"1";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"a.9";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"a1.1b.c";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"a-a.b-b-b.c-c-c.d----d.d";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa."
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa."
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)
		"a12345678.a12345678.a12345678.a12345678.a12345678.a12345678.a12345678."
		"a12345678.a12345678.a12345678.a12345678.a12345678.a12345678.a12345678."
		"a12345678.a12345678.a12345678.a12345678.a12345678.a12345678.a12345678."
		"a12345678.a12345678.a12345678.z";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)
		"a12345678.a12345678.a12345678.a12345678.a12345678.a12345678.a12345678."
		"a12345678.a12345678.a12345678.a12345678.a12345678.a12345678.a12345678."
		"a12345678.a12345678.a12345678.a12345678.a12345678.a12345678.a12345678."
		"a12345678.a12345678.a12345678z";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"port-123";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"port-123456";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"ok.port-123";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"ok.port-123.b";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	/* the first (leftmost) label does not match with "port-xyz" with x, y, z = 0...9 */
	/* the first (leftmost) label does not match "port-xyz-abcde" with x, y, z, a, b, c, d, e = 0...9 */

	s = (LSA_UINT8*)"port-1";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"port-123";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"port-1234";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"port-123-1";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"port-123-12345";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"Port-123-12345";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"port-123-123456";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"ok.port-123";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"1.2.3.4"; /* ip-addr */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"999.999.999.999"; /* ip-addr */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"1.2.3"; /* not an ip-addr */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"1.2.3.4.5"; /* not an ip-addr */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"1a.2.3.4"; /* not an ip-addr */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"9991.2.3.4"; /* not an ip-addr */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"12345"; /* ok, "abcde" with a, b, c, d, e = 0...9 */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	/*
	 * AliasNameValue = LLDP_PortID + "." + LLDP_ChassisID
	 * LLDP_PortID    = "port-xyz" or "port-xyz-rstuv"
	 * LLDP_ChassisID = NameOfStationValue
	 */

	s = (LSA_UINT8*)"port-123";
	ok = cm_is_valid_alias_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"port-123.";
	ok = cm_is_valid_alias_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"port-123.port-123";
	ok = cm_is_valid_alias_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"port-123.a";
	ok = cm_is_valid_alias_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	ok = ok;
}


/*----------------------------------------------------------------------------*/

LSA_VOID  CM_SYSTEM_IN_FCT_ATTR
cm_run_test_cases (
	LSA_VOID
)
{
	cm_test_log_size ();
	cm_test_station_name ();
}


/*----------------------------------------------------------------------------*/

#endif /* test-cases */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
