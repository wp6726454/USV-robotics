#ifndef CM_INT_H
#define CM_INT_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  F i l e               &F: cm_int.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal Headerfile                                                      */
/*                                                                           */
/*****************************************************************************/

#ifndef CM_MODULE_ID
# error "CM_MODULE_ID not defined!"
#endif

/*----------------------------------------------------------------------------*/

#include "cm_inc.h"

#include "cm_pdu.h" /* PDUs are internal */

#include "cm_md5.h" /* md5 functions */

/*------------------------------------------------------------------------------
// stringify
//----------------------------------------------------------------------------*/

#define CM_STRINGIFY_(n)  # n
#define CM_STRINGIFY(n)   CM_STRINGIFY_(n) /* make it a string */

/*------------------------------------------------------------------------------
// pragma message
// intended usage: #pragma CM_MESSAGE(...)
//----------------------------------------------------------------------------*/

#ifdef _MSC_VER /* Microsoft, pragma-args are expanded */

# ifdef CM_MESSAGE
#  error "who defined CM_MESSAGE?"
# else
#  ifdef CM_CFG_MESSAGE /* for developers only */
#    define CM_MESSAGE(str) message( __FILE__ "(" CM_STRINGIFY(__LINE__) ") : " str)
#  endif
# endif /* CM_MESSAGE */

#endif

/*------------------------------------------------------------------------------
// logging
//----------------------------------------------------------------------------*/

#ifndef CM_LOG_FCT
# define CM_LOG_FCT(fn_)  /* empty */
#endif

/*------------------------------------------------------------------------------
// assertions
//----------------------------------------------------------------------------*/

/*
 * #ifndef ... moved to cm_pls.h
 */

/*------------------------------------------------------------------------------
// pointer compare macros
//----------------------------------------------------------------------------*/

#	define is_null(ptr_)			(LSA_HOST_PTR_ARE_EQUAL(CM_TEST_POINTER(ptr_), LSA_NULL))
#	define is_not_null(ptr_)		(! LSA_HOST_PTR_ARE_EQUAL(CM_TEST_POINTER(ptr_), LSA_NULL))
#	define are_equal(ptr1_, ptr2_)	(LSA_HOST_PTR_ARE_EQUAL(CM_TEST_POINTER(ptr1_), CM_TEST_POINTER(ptr2_)))

/*------------------------------------------------------------------------------
// helper for casting and warning-level
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_CASTS

#	define CM_CAST_ARGR_IOCR_PTR(val_)		((CM_AR_GRAPH_IO_CR_PTR_TYPE)(val_))
#	define CM_CAST_ARGR_ALARMCR_PTR(val_)	((CM_AR_GRAPH_ALARM_CR_PTR_TYPE)(val_))

#	define CM_CAST_UPPER_MEM_PTR_PTR(val_)	((CM_UPPER_MEM_PTR_TYPE CM_LOCAL_MEM_ATTR *)(val_))
#	define CM_CAST_COMMON_MEM_U8_PTR(val_)	((CM_COMMON_MEM_U8_PTR_TYPE                )(val_))

#	define CM_CAST_LOCAL_MEM_PTR(val_)		((CM_LOCAL_MEM_PTR_TYPE)(val_))
#	define CM_CAST_LOCAL_MEM_PTR_PTR(val_)	((CM_LOCAL_MEM_PTR_TYPE CM_LOCAL_MEM_ATTR *)(val_))

#	define CM_CAST_ACP_MEM_PTR_PTR(val_)	((CM_ACP_LOWER_MEM_PTR_TYPE CM_LOCAL_MEM_ATTR *)(val_))

#	define CM_CAST_EDD_MEM_PTR_PTR(val_)	((CM_EDD_LOWER_MEM_PTR_TYPE CM_LOCAL_MEM_ATTR *)(val_))
#	define CM_CAST_EDD_MEM_U8_PTR(val_)		((EDD_UPPER_MEM_U8_PTR_TYPE                    )(val_))

#	define CM_CAST_GSY_MEM_PTR_PTR(val_)	((CM_GSY_LOWER_MEM_PTR_TYPE CM_LOCAL_MEM_ATTR *)(val_))
#	define CM_CAST_GSY_MEM_U8_PTR(val_)		((GSY_UPPER_BYTE_PTR_TYPE                      )(val_))

#	define CM_CAST_NARE_MEM_PTR_PTR(val_)	((CM_NARE_LOWER_MEM_PTR_TYPE CM_LOCAL_MEM_ATTR *)(val_))

#	define CM_CAST_OHA_MEM_PTR_PTR(val_)	((CM_OHA_LOWER_MEM_PTR_TYPE CM_LOCAL_MEM_ATTR *)(val_))
#	define CM_CAST_OHA_MEM_U8_PTR(val_)		((OHA_UPPER_MEM_U8_PTR_TYPE                    )(val_))

#	define CM_CAST_RPC_MEM_PTR_PTR(val_)	((CM_RPC_LOWER_MEM_PTR_TYPE CM_LOCAL_MEM_ATTR *)(val_))

#	define CM_CAST_MRP_MEM_PTR_PTR(val_)	((CM_MRP_LOWER_MEM_PTR_TYPE CM_LOCAL_MEM_ATTR *)(val_))
#	define CM_CAST_MRP_MEM_U8_PTR(val_)		((MRP_UPPER_MEM_UINT8_PTR_TYPE                 )(val_))

#	define CM_CAST_POF_MEM_PTR_PTR(val_)	((CM_POF_LOWER_MEM_PTR_TYPE CM_LOCAL_MEM_ATTR *)(val_))
#	define CM_CAST_POF_MEM_U8_PTR(val_)		((POF_UPPER_MEM_U8_PTR_TYPE                    )(val_))

#else

#	define CM_CAST_ARGR_IOCR_PTR(val_)		(val_)
#	define CM_CAST_ARGR_ALARMCR_PTR(val_)	(val_)

#	define CM_CAST_UPPER_MEM_PTR_PTR(val_)	(val_)
#	define CM_CAST_COMMON_MEM_U8_PTR(val_)	(val_)

#	define CM_CAST_LOCAL_MEM_PTR(val_)		(val_)
#	define CM_CAST_LOCAL_MEM_PTR_PTR(val_)	(val_)

#	define CM_CAST_ACP_MEM_PTR_PTR(val_)	(val_)

#	define CM_CAST_EDD_MEM_PTR_PTR(val_)	(val_)
#	define CM_CAST_EDD_MEM_U8_PTR(val_)		(val_)

#	define CM_CAST_GSY_MEM_PTR_PTR(val_)	(val_)
#	define CM_CAST_GSY_MEM_U8_PTR(val_)		(val_)

#	define CM_CAST_NARE_MEM_PTR_PTR(val_)	(val_)

#	define CM_CAST_OHA_MEM_PTR_PTR(val_)	(val_)
#	define CM_CAST_OHA_MEM_U8_PTR(val_)		(val_)

#	define CM_CAST_RPC_MEM_PTR_PTR(val_)	(val_)

#	define CM_CAST_MRP_MEM_PTR_PTR(val_)	(val_)
#	define CM_CAST_MRP_MEM_U8_PTR(val_)		(val_)

#	define CM_CAST_POF_MEM_PTR_PTR(val_)	(val_)
#	define CM_CAST_POF_MEM_U8_PTR(val_)		(val_)

#endif

#if CM_CFG_USE_CASTS || CM_DEBUG

#	define CM_CAST_U8(val_)			((LSA_UINT8)(val_))
#	define CM_CAST_U16(val_)		((LSA_UINT16)(val_))
#	define CM_CAST_U32(val_)		((LSA_UINT32)(val_))

#else

#	define CM_CAST_U8(val_)			(val_)
#	define CM_CAST_U16(val_)		(val_)
#	define CM_CAST_U32(val_)		(val_)

#endif


/*------------------------------------------------------------------------------
// get the containing struct
// (analog to window's CONTAINING_RECORD)
//----------------------------------------------------------------------------*/

#define CM_CONTAINING_STRUCT(field_ptr, struct_ptr_type, field) \
	((struct_ptr_type) \
		((CM_COMMON_MEM_U8_PTR_TYPE)(field_ptr) - (LSA_INT)( &((struct_ptr_type)0)->field )))


/*------------------------------------------------------------------------------
// oneshot timer -- implemented in file cm_tim.c
// (copy/paste from CLRPC)
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_timeout_synchronized(
	CM_UPPER_RQB_PTR_TYPE  rb
);

typedef struct cm_oneshot_tag CM_LOCAL_MEM_ATTR * CM_ONESHOT_PTR_TYPE;

typedef LSA_VOID LSA_FCT_PTR(CM_LOCAL_FCT_ATTR,
	CM_ONESHOT_CALLBACK)(
		CM_ONESHOT_PTR_TYPE oneshot,
		LSA_VOID_PTR_TYPE user_ptr
	);

typedef struct cm_oneshot_tag {
	CM_LIST_ENTRY_TYPE dlink; /* must be first in struct */
	CM_ONESHOT_CALLBACK callback;
	LSA_VOID_PTR_TYPE user_ptr;
	LSA_UINT32 ticks;
	LSA_UINT16 running;
} CM_ONESHOT_TYPE;

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oneshot_sys_init(
	LSA_UINT16 time_base,
	LSA_UINT16 time_factor
);

LSA_VOID cm_oneshot_sys_undo_init(
	LSA_VOID
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oneshot_init(
	CM_ONESHOT_PTR_TYPE oneshot,
	CM_ONESHOT_CALLBACK callback,
	LSA_VOID_PTR_TYPE user_ptr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oneshot_trigger(
	CM_ONESHOT_PTR_TYPE oneshot,
	LSA_UINT32 timeout_msec
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_oneshot_stop(
	CM_ONESHOT_PTR_TYPE oneshot
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_oneshot_is_running(
	CM_ONESHOT_PTR_TYPE oneshot
);


/*------------------------------------------------------------------------------
// Range, see cm_cmn.c
//----------------------------------------------------------------------------*/

enum cm_range_id_type {
	CM_RANGE_ID_ALARM_REF = 0,
	CM_RANGE_ID_CLASS_1_UNICAST,
	CM_RANGE_ID_CLASS_1_MULTICAST,
	CM_RANGE_ID_CLASS_2_UNICAST,
	CM_RANGE_ID_CLASS_2_MULTICAST,
	CM_RANGE_ID_MAX
};

typedef struct cm_range_tag {
	LSA_UINT16				base;
	LSA_UINT16				count;
	CM_COMMON_MEM_U8_PTR_TYPE arr; /* allocated bit-array */
} CM_RANGE_TYPE;

typedef CM_RANGE_TYPE  CM_COMMON_MEM_ATTR * CM_RANGE_PTR_TYPE;

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_range_init (
	CM_RANGE_PTR_TYPE range
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_range_undo_init (
	CM_RANGE_PTR_TYPE range
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_range_alloc (
	CM_RANGE_PTR_TYPE range,
	LSA_UINT16	base,
	LSA_UINT16	count
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_range_alloc_id  (
	CM_RANGE_PTR_TYPE	range,
	LSA_UINT16 CM_COMMON_MEM_ATTR * id_ptr
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_range_reserve_id  (
	CM_RANGE_PTR_TYPE	range,
	LSA_UINT16  id
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_range_free_id  (
	CM_RANGE_PTR_TYPE	range,
	LSA_UINT16			id
);


/*------------------------------------------------------------------------------
// helper function: convert a format_0 sub_state to an format_1 sub_state
//----------------------------------------------------------------------------*/

LSA_UINT16  CM_UPPER_IN_FCT_ATTR
cm_sub_state_to_format_1 (
	LSA_UINT16  sub_state	/* in: sub_state in format_0(return converted) or format_1(simple return) */
	);


/*------------------------------------------------------------------------------
// null user_id and NIL UUID
//----------------------------------------------------------------------------*/

extern const LSA_USER_ID_TYPE  cm_null_user_id; /* defined in cm_sys.c */

extern const CLRPC_UUID_TYPE  cm_uuid_nil_uuid; /* defined in cm_sys.c */


/*------------------------------------------------------------------------------
// forward
//----------------------------------------------------------------------------*/

typedef struct cm_channel_tag CM_LOCAL_MEM_ATTR * CM_CHANNEL_PTR_TYPE;


/*------------------------------------------------------------------------------
// helper function: alloc upper rqb for internal use
//----------------------------------------------------------------------------*/

CM_UPPER_RQB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_upper_alloc_rqb (
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_upper_free_rqb (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE  upper
);


/*------------------------------------------------------------------------------
// helper function: record index handling
//----------------------------------------------------------------------------*/

#define CM_VENDOR_ID_DONTCARE 0x0000 /* see where used (pdev iterators) */
#define CM_VENDOR_ID_SIEMENS  0x002A /* see AP00579173 */

enum cm_record_index_classify_enum {

	CM_REC_IDX_CMPD_MASK      = 0x0000000F, /* see also cm_sub_comp_id_enum */
	CM_REC_IDX_CMPD_NONE      = 0x00000000,
	CM_REC_IDX_CMPD_EDD       = 0x00000001,
#if CM_CFG_USE_MRP
	CM_REC_IDX_CMPD_MRP       = 0x00000002,
#endif
#if CM_CFG_USE_GSY
	CM_REC_IDX_CMPD_GSY_CLOCK = 0x00000003,
	CM_REC_IDX_CMPD_GSY_TIME  = 0x00000004,
#endif
#if CM_CFG_USE_POF
	CM_REC_IDX_CMPD_POF       = 0x00000005,
#endif
	CM_REC_IDX_CMPD_OHA       = 0x00000006,
	CM_REC_IDX_CMPD_BBUF      = 0x00000007, /* usage see CM_OPC_SV_AR_PDEVPRM_READ/WRITE (TIA 1095777) */
	CM_REC_IDX_CMPD_PD        = 0x00000008,

	CM_REC_IDX_INTERFACE = 0x00000010, /* the index addresses an interface submodule */
	CM_REC_IDX_PORT      = 0x00000020, /* the index addresses a port submodule */

	CM_REC_IDX_MASK    = 0x00003F00,
	CM_REC_IDX_0000    = 0x00000100,
	CM_REC_IDX_8000    = 0x00000200,
	CM_REC_IDX_C000    = 0x00000400,
	CM_REC_IDX_E000    = 0x00000800,
	CM_REC_IDX_F000    = 0x00001000,
	CM_REC_IDX_F800    = 0x00002000,

	CM_REC_IDX_READ        = 0x00010000, /* note: reserved := neither readable nor writeable */
	CM_REC_IDX_WRITE       = 0x00020000, /* note: reserved := neither readable nor writeable */
	CM_REC_IDX_READ_LOCAL  = 0x00040000, /* only the pdev-user is allowed to read */
	CM_REC_IDX_WRITE_LOCAL = 0x00080000, /* only the pdev-user is allowed to write */

	CM_REC_IDX_USER    = 0x00100000, /* handled by cm-user */
	CM_REC_IDX_PDEV    = 0x00200000, /* handled by cm-pd */
	CM_REC_IDX_NORM    = 0x00400000, /* handled by cm-sv or cm-pd (diagnosis) */
	CM_REC_IDX_DIAG    = 0x00800000, /* diagnosis-part of normative */
	CM_REC_IDX_SIMATIC = 0x01000000, /* simatic-indices (0xBxxx) */
	CM_REC_IDX_NONPRM  = 0x02000000, /* these indices are "out of band" and must not be stored remanent */
	CM_REC_IDX_MKREMA  = 0x04000000, /* index is NONPRM but "must" stored remanent */
	CM_REC_IDX_IM1234  = 0x08000000  /* I&M1-4 */
};

LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_record_index_classify(
	LSA_UINT32 api,
	LSA_UINT32 index32,
	LSA_UINT16 vendor_id
);

/*----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_record_marshal_diag_yes(
	LSA_UINT16 index,
	LSA_BOOL is_generic, /* false: channel or ext-channel lists, true: generic-list */
	LSA_UINT16 prop
);


/*------------------------------------------------------------------------------
// AR-Graph things
//----------------------------------------------------------------------------*/

#include "cm_argr.h"


/*------------------------------------------------------------------------------
// physical device things
//----------------------------------------------------------------------------*/

/* === derived typedef (AP01034411) ==== */

#if !defined(EDD_CFG_MAX_PORT_CNT)
 #error "EDD_CFG_MAX_PORT_CNT not defined"

#elif EDD_CFG_MAX_PORT_CNT <= 31
 typedef LSA_UINT32  CM_PORT_UINT; /* 32 bits = 1 interface and up to 31 ports */

#elif  EDD_CFG_MAX_PORT_CNT <= 63
 typedef LSA_UINT64  CM_PORT_UINT; /* 64 bits = 1 interface and up to 63 ports */

#else
 #error "EDD_CFG_MAX_PORT_CNT >= 64 not supported"
#endif


#include "cm_ipd.h"

#include "cm_prm.h" /* the prm-interface of the subcomponents */


/*------------------------------------------------------------------------------
// rpc things
//----------------------------------------------------------------------------*/

#include "cm_rpc.h"


/*------------------------------------------------------------------------------
// client things
//----------------------------------------------------------------------------*/

#if CM_CFG_MAX_CLIENTS
#include "cm_icl.h"
#endif


/*------------------------------------------------------------------------------
// server things
//----------------------------------------------------------------------------*/

#if CM_CFG_MAX_SERVERS
#include "cm_isv.h"
#endif


/*------------------------------------------------------------------------------
// multicast things
//----------------------------------------------------------------------------*/

#if CM_CFG_MAX_MULTICAST_CHANNELS
#include "cm_imc.h"
#endif


/*------------------------------------------------------------------------------
// channel states, -type and -table
//----------------------------------------------------------------------------*/

enum cm_channel_state_enum { /* state machine CM-CH */
	CM_CH_FREE = 0,
	CM_CH_ALLOCATED,

	/*
	 * the state machine CM-CH-<ANY>
	 */

	CM_CH_OPEN,
	CM_CH_INFO,
	CM_CH_READY, /* ready for create/delete or close channel */

	/*
	 * the "in-lined" state machine CM-PD
	 */

	CM_PD_STARTING, /* RPC:EPM_START, OHA:SET_IDENT */
	CM_PD_READY,
	CM_PD_EPM_STOP,

	/*
	 * the "in-lined" state machine CM-CL
	 */

#if CM_CFG_MAX_CLIENTS
	CM_CL_GLOBAL_CTRL,
	CM_CL_REG_IFC,
	CM_CL_READY,
	CM_CL_UNREGISTER,
#endif

	/*
	 * the "in-lined" state machine CM-SV
	 */

#if CM_CFG_MAX_SERVERS
	CM_SV_CREATING,
	CM_SV_CREATED,
	CM_SV_DELETING,
#endif

	/*
	 * the "in-lined" state machine CM-MC
	 */

#if CM_CFG_MAX_MULTICAST_CHANNELS
	CM_MC_GLOBAL_CTRL,
	CM_MC_READY,
#endif

	/*
	 * the state machine CM-CH-<ANY>
	 */

	CM_CH_CLOSE
};

struct cm_edd_port_status_tag {
	LSA_UINT8  status;
	LSA_UINT8  speed;
	LSA_UINT8  mode;
	LSA_UINT8  irt_port_status;
	LSA_UINT8  autoneg;
};

typedef struct cm_edd_port_status_tag CM_LOCAL_MEM_ATTR * CM_EDD_PORT_STATUS_PTR_TYPE;


typedef struct cm_channel_tag {

	LSA_UINT16			state;			/* see cm_channel_state_enum */
	LSA_HANDLE_TYPE		my_handle;		/* my own handle */
	LSA_HANDLE_TYPE		handle_upper;	/* the handle of the user */
	CM_UPPER_CALLBACK_FCT_PTR_TYPE callback; /* the callback-function to the user */
	LSA_SYS_PTR_TYPE	sysptr;			/* the 'hint' for the system adaption */
	CM_DETAIL_PTR_TYPE	detailptr;		/* pointer to channel-details */
	LSA_UINT32			trace_idx;		/* for "LSA index trace" */

	/***/

	union cm_channel_usr_tag {

		struct {
			CM_UPPER_RQB_PTR_TYPE curr; /* current working upper request block for open / close channel */
		} ch;

		struct {
			CM_EDD_PORT_STATUS_PTR_TYPE  ports; /* array[ 0: auto-port / port_nr ] */
		} edd; /* edd things */

#if CM_CFG_USE_GSY
		struct {
			LSA_BOOL do_sync;
			LSA_BOOL is_sync;
			LSA_BOOL is_rate_valid;
			LSA_UINT8 rcv_sync_prio; /* MasterPriority1 from SyncFrame (0 at active master) */
			LSA_UINT8 local_prio;    /* MasterPriority1 from SyncRecord (0 at slave) with ActiveBit set at active Master after Startup */
			LSA_UINT8 pll_state;    /* see cm_pd_ppl_state_enum */
			GSY_MAC_ADR_TYPE master_mac_addr;
		} gsy; /* gsy things */
#endif

		CM_PD_CHANNEL_TYPE pd; /* physical device things */

#if CM_CFG_MAX_CLIENTS
		CM_CL_CHANNEL_TYPE cl; /* client things */
#endif

#if CM_CFG_MAX_SERVERS
		CM_SV_CHANNEL_TYPE sv; /* server things */
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS
		CM_MC_CHANNEL_TYPE mc; /* multicast things */
#endif

		struct {
			LSA_UINT8  name_of_station[OHA_MAX_STATION_NAME_LEN];
			LSA_UINT16 name_of_station_length;

			OHA_IP_SUITE_TYPE	IPSuite;                /* current IP suite of the stack */
			LSA_BOOL			is_IPAddr_null;         /* a shortcut: LSA_TRUE if IPSuite.IpAddr == 0.0.0.0 (NetMask and Gateway are not considered) */
			LSA_BOOL			is_IPSuite_emergency;   /* LSA_TRUE if the current IP suite is the "emergency" IP suite */
			LSA_BOOL			is_IPSuite_conflicting; /* LSA_TRUE if the current IP suite is conflicting with another station */
		} oha;

	} usr;

	/***/

	struct cm_channel_res_tag {

		CM_LIST_ENTRY_TYPE eventQ;    /* pool of notification resources */
		CM_LIST_ENTRY_TYPE alarmQ[2]; /* pool of alarm resources, prio low=[0], high=[1] */

		LSA_UINT16 withdraw_cnt_events;
		LSA_UINT16 withdraw_cnt_alarms[2];

		/***/

		LSA_UINT16 provider_cnt[2]; /* 0=class1_2, 1=class3 */

		CM_RANGE_TYPE id_range[CM_RANGE_ID_MAX]; /* id range for frame_id(class 1..3) and alarm_ref */

		LSA_UINT16 consumer_cnt[2]; /* 0=class1_2, 1=class3, see acp.info.is_combined */

	} res;

	/***/

	struct cm_channel_overload_tag {
		CM_LIST_ENTRY_TYPE overload_list;
		LSA_BOOL acp_overload_running;
		LSA_UINT32 send_overload_counter;
		LSA_UINT32 recv_overload_counter;
	} overload;

	/***/

	struct cm_channel_acp_tag {

		LSA_UINT16 max_alarm_data_length;

		LSA_UINT32 alarm_reject;          /* see cm_alarm_type_is_supported() */
		LSA_UINT32 alarm_priority;        /* bit set: alarm send with high priority */

	} acp;

	/***/

	struct { /* each channel has 0 or 1 lower-layer */

		LSA_HANDLE_TYPE handle;

		union /* see open/close/prm */
		{
			CM_EDD_LOWER_RQB_PTR_TYPE  edd;
			CM_OHA_LOWER_RQB_PTR_TYPE  oha;
#if CM_CFG_USE_GSY
			CM_GSY_LOWER_RQB_PTR_TYPE  gsy;
#endif
#if CM_CFG_USE_MRP
			CM_MRP_LOWER_RQB_PTR_TYPE  mrp;
#endif
#if CM_CFG_USE_POF			
			CM_POF_LOWER_RQB_PTR_TYPE  pof;
#endif
		} prealloc;

		union {

			struct { /* only a part of EDD_RQB_CSRT_PARAM_TYPE */
				LSA_UINT32       InterfaceID;
				LSA_UINT32       HardwareType;
				EDD_MAC_ADR_TYPE MACAddress;
				LSA_UINT16       MaxPortCnt;
				LSA_UINT16       CycleBaseFactor;
				LSA_UINT8        MRPSupportedRole; /* EDD.doc: If MRP is not supported the parameter is set to 0 */
			} edd;

#if CM_CFG_USE_GSY
			GSY_CHANNEL_INFO_TYPE gsy;
#endif

#if CM_CFG_USE_POF
			POF_RQB_GET_PARAMS_TYPE pof;
#endif

			OHA_CONFIG_TYPE oha;

			CLRPC_SET_INTERFACE_PARAM_TYPE rpc;
			ACP_CHANNEL_INFO_TYPE acp;

		} info;

		/* each channel has >= 0 associated lower-layers (system channels) */

		CM_CHANNEL_PTR_TYPE channel[CM_PATH_TYPE_ANY_MAX]; /* not: CM_PATH_TYPE_SYS_MAX */

	} lower;

} CM_CHANNEL_TYPE;

/* the channel table */

extern CM_CHANNEL_TYPE  cm_channels[CM_MAX_CHANNELS]; /* defined in cm_usr.c */

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_channels_sys_init(
	LSA_VOID
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_channels_sys_undo_init(
	LSA_VOID
);

CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_channel_from_handle(
	LSA_HANDLE_TYPE handle
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_request_pd_cl_mc (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_callback(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 response,
	CM_UPPER_RQB_PTR_TYPE CM_LOCAL_MEM_ATTR *upper_ptr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_channel_done(
	CM_CHANNEL_PTR_TYPE channel, LSA_UINT32 result
);

/*----------------------------------------------------------------------------*/

CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_channel_find_path_type(
	LSA_UINT8  nic_id,
	LSA_UINT8  path_type
);

#if CM_CFG_USE_GSY
CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_channel_find_gsy(
	LSA_UINT8  nic_id,
	LSA_UINT8  sync_id
);
#endif

/*----------------------------------------------------------------------------*/

CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_peer_channel_get(
	CM_CHANNEL_PTR_TYPE channel, /* PD, CL, MC or SV */
	LSA_UINT8 peer_path_type
		/* PD: CM_PATH_TYPE_CLIENT, CM_PATH_TYPE_MULTICAST or CM_PATH_TYPE_SERVER
		 * CL: CM_PATH_TYPE_PD
		 * MC: CM_PATH_TYPE_PD
		 * SV: CM_PATH_TYPE_PD
		 */
);

/*------------------------------------------------------------------------------
// Resources, see cm_res.c
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_res_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_res_undo_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_event_provide(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

CM_UPPER_RQB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_event_get(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_alarm_provide(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

CM_UPPER_RQB_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_res_get_alarm(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 prio /* pool: 0=low, 1=high */
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_res_do_reserve(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 events,
	LSA_UINT16 alarms_low,
	LSA_UINT16 alarms_high
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_res_undo_reserve(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 events,
	LSA_UINT16 alarms_low,
	LSA_UINT16 alarms_high
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_res_withdraw(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_res_cancel(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_res_frame_alloc (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_BOOL is_consumer,
	LSA_BOOL is_multicast,
	LSA_UINT32 rt_class,
	LSA_UINT16 CM_COMMON_MEM_ATTR * frame_id_ptr /* consumer only */
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_res_frame_free (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_BOOL is_consumer,
	LSA_BOOL is_multicast,
	LSA_UINT32 rt_class,
	LSA_UINT16 frame_id /* consumer only */
);

/*===========================================================================*/
/*===========================================================================*/

LSA_VOID CM_LOCAL_FCT_ATTR
cm_acp_alloc_rqb  (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_ACP_LOWER_RQB_PTR_TYPE	*lower_rqb_ptr_ptr
);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_acp_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  *lower_rqb_ptr_ptr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_channel_open(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_channel_info(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_channel_close(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_request_lower(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_OPCODE_TYPE opcode,
	LSA_VOID_PTR_TYPE  user_id,
	CM_ACP_LOWER_RQB_PTR_TYPE acp
);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_acp_nop (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_VOID_PTR_TYPE  user_id,
	LSA_UINT32  user_1,
	LSA_UINT32  user_2
);

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_frame_indication(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

/*===========================================================================*/

LSA_VOID CM_UPPER_IN_FCT_ATTR
cm_channel_info (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE  rb
);

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_overload_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_overload_undo_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_overload_cancel(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16  rsp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_overload_info(
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_RQB_PTR_TYPE rb
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_overload_indication(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT32  tx_counter,
	LSA_UINT32  rx_counter
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_overload_info_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);


/*===========================================================================*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_link_status_is_up(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_link_status_matches_ar_properties(
	CM_CHANNEL_PTR_TYPE channel,
	EDD_UPPER_GET_LINK_STATUS_PTR_TYPE param,
	LSA_UINT32 ar_properties
);

/*===========================================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_frame_add (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_frame_add_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_frame_control(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	LSA_UINT16  acp_mode,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_frame_control_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_frame_remove (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_frame_remove_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_acp_frame_get_apdu_status (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	LSA_VOID_PTR_TYPE  user_id
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_frame_get_apdu_status_done (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_VOID_PTR_TYPE  user_id,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_add (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_add_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_remove (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_remove_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_multicast_mac(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_MAC_PTR_TYPE  mmac,
	LSA_UINT8 mode, /* ACP_MULTICAST_ENABLE or ACP_MULTICAST_DISABLE */
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_multicast_mac_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

/*===========================================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_init (
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_ready (
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_down_debug_data_set (
	CM_AR_GRAPH_PTR_TYPE  argr,
	ACP_UPPER_ERROR_INFO_DATA_PTR_TYPE  debug_data
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_down_debug_data_fmt (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  module_id,
	LSA_UINT32  line_nr,
	LSA_UINT8  reason,
	LSA_UINT32  data1
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_down2(
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8  reason,
	LSA_UINT16  cycle_counter
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_down(
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8  reason
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_idle (
	CM_AR_GRAPH_PTR_TYPE  argr
);

/***/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_timer_init(
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_timer_trigger(
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_timer_stop(
	CM_AR_GRAPH_PTR_TYPE  argr
);

/*===========================================================================*/

enum cm_arcq_opcode_enum {
	CM_ARCQ_OPC_IDLE			=  0,

	CM_ARCQ_OPC_FRAMES_ADD			=  1, /* add provider + consumer / unicast only */
	CM_ARCQ_OPC_FRAMES_REMOVE		=  2, /* remove provider + consumer / unicast and multicast */
	CM_ARCQ_OPC_FRAMES_PASSIVATE	=  3, /* passivate provider + consumer / unicast and multicast */

	CM_ARCQ_OPC_FRAMES_RTF			=  4, /* only CMCL, prepare added frames for reset-to-factory */

	CM_ARCQ_OPC_PROV_ACTIVATE			= 11, /* + config-autostop */
	CM_ARCQ_OPC_PROV_ACTIVATE_AUX		= 12,
	CM_ARCQ_OPC_PROV_PASSIVATE_AUX		= 13,
	CM_ARCQ_OPC_PROV_DATASTATUS_SET		= 14,
	CM_ARCQ_OPC_PROV_IOPARAMS_SET		= 15,

	CM_ARCQ_OPC_CONS_ACTIVATE			= 21,
	CM_ARCQ_OPC_CONS_ACTIVATE_SET2UNK	= 22, /* + set-to-unknown */
	CM_ARCQ_OPC_CONS_SET2UNK			= 23,
	CM_ARCQ_OPC_CONS_SET2UNK_ENPRAS		= 24, /* + enable-provider-autostop */

	CM_ARCQ_OPC_ALARM_ADD			= 31,
	CM_ARCQ_OPC_ALARM_REMOVE		= 32,

	CM_ARCQ_OPC_MPROV_ADD			= 41,
	CM_ARCQ_OPC_MPROV_ACTIVATE		= 42,

	CM_ARCQ_OPC_MCONS_ADD			= 51,

	CM_ARCQ_OPC_CALLBACK			= 91, /* internal: does a callback */
	CM_ARCQ_OPC_FREE_AR_GRAPH		= 92  /* internal for cm_arcq_free_ar_graph() */
};

/*callback*/  LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_arcq_init (
	CM_AR_GRAPH_PTR_TYPE  argr
);

/*callback*/  LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcq_undo_init (
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcq_free_ar_graph (
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcq_request (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8  arcq_opcode, /* see cm_arcq_opcode_enum */
	LSA_UINT16  para1
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcq_chain_next_request ( /* call is only allowed from within cm_$CHANNEL_request_done - callback */
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT8  arcq_opcode,
	LSA_UINT16  para1
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcq_step_next (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_VOID_PTR_TYPE  cr,	/* io_cr or alarm_cr */
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arcq_request_done (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);	/* callback to: cm_cl_arcq_request_done(), cm_mc_arcq_request_done() or cm_sv_arcq_request_done() */



/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_link_status_changed(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_BOOL is_up,
	LSA_BOOL is_transient /* LSA_TRUE: the "down" is transient (some 100ms) */
);

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_cons_apdu_status_changed (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr,
	LSA_UINT32 state_event
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_cons_apdu_status_check_unicast (
	CM_AR_GRAPH_PTR_TYPE  argr
);

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_mcr_start(
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_mcr_stop(
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_mcr_stop_done(
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr
);

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mprov_start(
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mprov_stop(
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mprov_link_up(
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mprov_up(
	CM_AR_GRAPH_PTR_TYPE  argr
);

/*===========================================================================*/

#if CM_CFG_USE_NARE

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_start(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_stop(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_link_up(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_upper_response(
	CM_CHANNEL_PTR_TYPE channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE iocr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_resolve_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE iocr,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_is_resolved(
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_resolve_cancel_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE iocr,
	CM_NARE_LOWER_RQB_PTR_TYPE nare
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_add_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_mmac_register_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_control_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_up(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_is_up(
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_down(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_timeout(
	CM_ONESHOT_PTR_TYPE oneshot,
	LSA_VOID_PTR_TYPE   user_ptr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_mmac_unregister_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_passivate_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_remove_done(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_arsm_mcons_apdu_status_changed(
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr
);

#endif

/*===========================================================================*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_com_check(
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_com_is_up(
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_com_is_rir(
	CM_AR_GRAPH_PTR_TYPE  argr
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_ar_com_is_down(
	CM_AR_GRAPH_PTR_TYPE  argr
);


LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_mcr_count (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  CM_LOCAL_MEM_ATTR * p_mcr_cnt,
	LSA_UINT16  CM_LOCAL_MEM_ATTR * p_mup_cnt
	);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_ar_com_cons_apdu_status_count (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *cr_cnt,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *operate,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *station,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *primary,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *sysred,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *mcr_cnt,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *moperate,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *mstation,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *mprimary,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  *msysred
	);

/*===========================================================================*/
/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_params (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT32  alarm_reject,
	LSA_UINT32  alarm_priority
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_acp_alarm_prio_from_type (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  alarm_type
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_alarm_type_is_internal ( /* user is not allowed to send this alarm-type */
	LSA_UINT16  alarm_type
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_alarm_type_is_supported (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  alarm_type
);

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_send (
	CM_AR_GRAPH_PTR_TYPE  argr,
	LSA_OPCODE_TYPE  opcode,
	CM_UPPER_RQB_PTR_TYPE  upper,
	LSA_UINT16  upper_alarm_tag,
	LSA_UINT16  upper_alarm_data_length,
	CM_COMMON_MEM_U8_PTR_TYPE  upper_alarm_data,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_send_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_ack_send_done (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_ALARM_CR_PTR_TYPE  alarm_cr,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

/*===========================================================================*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_indication (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_copy (
	CM_UPPER_RQB_PTR_TYPE  cm,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp,
	LSA_OPCODE_TYPE cm_opcode
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_copy_ack (
	CM_UPPER_RQB_PTR_TYPE  cm,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp,
	LSA_OPCODE_TYPE cm_opcode
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_acp_alarm_ack_indication (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_acp_alarm_ack_matches (
	CM_UPPER_RQB_PTR_TYPE  upper,
	CM_ACP_LOWER_RQB_PTR_TYPE  acp
);

/*------------------------------------------------------------------------------
// EDD
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_edd_channel_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_edd_channel_undo_init(
	CM_CHANNEL_PTR_TYPE channel
);

CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_edd_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_edd_alloc_rqb  (
	CM_CHANNEL_PTR_TYPE  channel,
	EDD_SERVICE  service, /* use 0 for open- and close-channel */
	CM_EDD_LOWER_RQB_PTR_TYPE  *lower_rqb_ptr_ptr
);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_edd_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_EDD_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_edd_request_lower(
	CM_CHANNEL_PTR_TYPE channel,
	EDD_SERVICE service,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	CM_EDD_LOWER_RQB_PTR_TYPE edd
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_edd_can_set_send_clock(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_edd_can_partial_data(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT32 rt_class
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_edd_can_autopadding(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT32 rt_class
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_edd_get_apdu_status_length(
	CM_CHANNEL_PTR_TYPE channel
);

/*----------------------------------------------------------------------------*/

LSA_VOID CM_LOCAL_FCT_ATTR
cm_edd_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_edd_channel_close (
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_edd_channel_info(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_edd_send_clock_range_ok(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 send_clock,
	LSA_BOOL CM_LOCAL_MEM_ATTR *is_power_of_two /* LSA_NULL: do not allow non-power-of-two values */
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_edd_set_send_clock(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 send_clock
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_edd_link_init (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_edd_link_undo_init (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_edd_link_data_changed (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT16  port_id,
	LSA_UINT8  status,
	LSA_UINT8  speed,
	LSA_UINT8  mode,
	LSA_UINT8  irt_port_status,
	LSA_UINT8  autoneg
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_edd_link_indication (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_EDD_LOWER_RQB_PTR_TYPE  edd
);

#if CM_CFG_MAX_CLIENTS
LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_edd_set_dcp_hello_filter (
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT8  mode,
	LSA_UINT32  user_id,
	EDD_UPPER_MEM_U8_PTR_TYPE  name_of_station,
	LSA_UINT32 name_of_station_length
);
#endif

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_edd_get_link_status(
	CM_CHANNEL_PTR_TYPE channel
);

/*------------------------------------------------------------------------------
// MRP
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_MRP

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mrp_channel_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_mrp_channel_undo_init(
	CM_CHANNEL_PTR_TYPE channel
);

CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_mrp_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_mrp_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_MRP_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

/*----------------------------------------------------------------------------*/

LSA_VOID CM_LOCAL_FCT_ATTR
cm_mrp_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_mrp_channel_close (
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_mrp_channel_info(
	CM_CHANNEL_PTR_TYPE channel
);

#endif


/*------------------------------------------------------------------------------
// GSY
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_GSY

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_channel_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_gsy_channel_undo_init(
	CM_CHANNEL_PTR_TYPE channel
);

CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_gsy_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_gsy_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_GSY_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

/*----------------------------------------------------------------------------*/

LSA_VOID CM_LOCAL_FCT_ATTR
cm_gsy_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_gsy_channel_close (
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_gsy_channel_info(
	CM_CHANNEL_PTR_TYPE channel
);

#endif

/*------------------------------------------------------------------------------
// POF
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_POF

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pof_channel_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_pof_channel_undo_init(
	CM_CHANNEL_PTR_TYPE channel
);

CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_pof_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_pof_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_POF_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

/*----------------------------------------------------------------------------*/

LSA_VOID CM_LOCAL_FCT_ATTR
cm_pof_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_pof_channel_close (
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_pof_channel_info(
	CM_CHANNEL_PTR_TYPE channel
);

#endif


/*------------------------------------------------------------------------------
// OHA
//----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_channel_init(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_UINT16  CM_LOCAL_FCT_ATTR
cm_oha_channel_undo_init(
	CM_CHANNEL_PTR_TYPE channel
);

CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_oha_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_oha_alloc_rqb  (
	CM_CHANNEL_PTR_TYPE			channel,
	CM_OHA_LOWER_RQB_PTR_TYPE	*lower_rqb_ptr_ptr
);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_oha_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_OHA_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

/*----------------------------------------------------------------------------*/

LSA_VOID CM_LOCAL_FCT_ATTR
cm_oha_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_channel_info(
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_oha_channel_close (
	CM_CHANNEL_PTR_TYPE channel
);

#if CM_CFG_MAX_CLIENTS

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_register_client (
	CM_CHANNEL_PTR_TYPE  oha_channel,
	CM_CHANNEL_PTR_TYPE  cl_channel /* LSA_NULL..unregister */
);

#endif

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_set_ident (
	CM_CHANNEL_PTR_TYPE channel,
	CM_UPPER_OEM_DATA_PTR_TYPE  oem_data
);

/*----------------------------------------------------------------------------*/

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_control_ip_suite(
	CM_AR_GRAPH_PTR_TYPE argr,
	LSA_UINT8 oha_mode, /* OHA_LOCK_SET or OHA_UNLOCK_SET */
	CM_OHA_LOWER_RQB_PTR_TYPE oha
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_oha_write_ptcp_data(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_BOOL do_sync,
	LSA_BOOL is_sync,
	OHA_MAC_TYPE CM_LOCAL_MEM_ATTR *master_mac
);

/*----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_oha_ip_suite_is_notnull_nonconflicting_and_expected (
	CM_CHANNEL_PTR_TYPE  user_channel
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_oha_get_ip_suite (
	CM_CHANNEL_PTR_TYPE  user_channel,
	LSA_UINT32  CM_LOCAL_MEM_ATTR  * ip_addr,
	LSA_UINT32  CM_LOCAL_MEM_ATTR  * subnet_mask,
	LSA_UINT32  CM_LOCAL_MEM_ATTR  * gateway_ip
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_oha_name_of_station_is_valid (
	CM_CHANNEL_PTR_TYPE  user_channel
);

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_oha_get_name_of_station (
	CM_CHANNEL_PTR_TYPE  user_channel,
	LSA_UINT16  CM_LOCAL_MEM_ATTR  * nos_length,
	CM_COMMON_MEM_U8_PTR_TYPE  CM_LOCAL_MEM_ATTR  * nos
);


/*------------------------------------------------------------------------------
// NARE
//----------------------------------------------------------------------------*/

#if CM_CFG_USE_NARE

LSA_VOID CM_LOCAL_FCT_ATTR
cm_nare_free_rqb (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_nare_free_rqb_alias (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_NARE_LOWER_RQB_PTR_TYPE  lower_rqb_ptr
);

CM_CHANNEL_PTR_TYPE  CM_LOCAL_FCT_ATTR
cm_nare_channel_get(
	CM_CHANNEL_PTR_TYPE  channel
);

/*----------------------------------------------------------------------------*/

LSA_VOID CM_LOCAL_FCT_ATTR
cm_nare_channel_open (
	CM_CHANNEL_PTR_TYPE  channel
);

LSA_VOID CM_LOCAL_FCT_ATTR
cm_nare_channel_close (
	CM_CHANNEL_PTR_TYPE channel
);

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_nare_channel_info(
	CM_CHANNEL_PTR_TYPE channel
);

#if CM_CFG_MAX_CLIENTS

LSA_VOID  CM_LOCAL_FCT_ATTR
cm_nare_register_client (
	CM_CHANNEL_PTR_TYPE  nare_channel,
	CM_CHANNEL_PTR_TYPE  cl_channel /* LSA_NULL..unregister */
);

#endif

/*----------------------------------------------------------------------------*/

LSA_BOOL CM_LOCAL_FCT_ATTR
cm_nare_resolve (			/* called by cm_arsm_mcons_resolve() and cm_cl_record_read_implicit() */
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT32  local_ip_addr,	/* used if do_check_ip == TRUE */
	LSA_UINT8  mode,
	LSA_BOOL  do_check_ip,
	LSA_UINT8  name_length,
	CM_COMMON_MEM_U8_PTR_TYPE  name_ptr,
	LSA_UINT16  device_nr,
	LSA_UINT16  ar_nr,
	LSA_VOID_PTR_TYPE  request_id
);

#if CM_CFG_MAX_CLIENTS

LSA_BOOL CM_LOCAL_FCT_ATTR
cm_nare_resolve_alias (
	CM_CHANNEL_PTR_TYPE  channel,
	LSA_UINT32  local_ip_addr,	/* used if do_check_ip == TRUE */
	LSA_UINT8  mode,
	LSA_BOOL  do_check_ip,
	LSA_UINT8  name_length,
	CM_COMMON_MEM_U8_PTR_TYPE  name_ptr,
	LSA_UINT32  ip_addr,
	LSA_UINT32  subnet_mask,
	LSA_UINT32  def_router,
	LSA_BOOL  set_ip_remanent,
	LSA_UINT16  ident_timeout,
	LSA_UINT16  device_nr,
	LSA_UINT16  ar_nr,
	LSA_UINT32  notify_id,
	LSA_VOID_PTR_TYPE  cancel_request_id,
	CM_LIST_ENTRY_PTR_TYPE  alias_list,
	LSA_BOOL  nos_overwrite,
	LSA_UINT16  vendor_id,
	LSA_UINT16  device_id,
	LSA_UINT16  instance_id
);

#endif

LSA_BOOL CM_LOCAL_FCT_ATTR
cm_nare_resolve_cancel(
	CM_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 device_nr,
	LSA_UINT16 ar_nr,
	LSA_VOID_PTR_TYPE request_id
);

#endif

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CM_INT_H */
