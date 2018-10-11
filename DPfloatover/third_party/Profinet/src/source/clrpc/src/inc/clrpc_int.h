#ifndef CLRPC_INT_H
#define CLRPC_INT_H

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
/*  C o m p o n e n t     &C: CLRPC (ConnectionLess RPC)                :C&  */
/*                                                                           */
/*  F i l e               &F: clrpc_int.h                               :F&  */
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

#ifndef CLRPC_MODULE_ID
# error "CLRPC_MODULE_ID not defined!"
#endif

/*----------------------------------------------------------------------------*/

//#include "clrpc_inc.h" /* includes all "official" things */

#include "clrpc_pdu.h" /* PDU things */

/*------------------------------------------------------------------------------
// limits
//----------------------------------------------------------------------------*/

#if !defined(CLRPC_CFG_MAX_CHANNELS) || CLRPC_CFG_MAX_CHANNELS < 1
# error "CLRPC_CFG_MAX_CHANNELS not defined or invalid value"
#endif

#if !defined(CLRPC_CFG_MAX_CLIENTS) || CLRPC_CFG_MAX_CLIENTS < 0
# error "CLRPC_CFG_MAX_CLIENTS not defined or invalid value"
#endif

#if !defined(CLRPC_CFG_MAX_SERVERS) || CLRPC_CFG_MAX_SERVERS < 0
# error "CLRPC_CFG_MAX_SERVERS not defined or invalid value"
#endif

#if ! defined(CLRPC_MUST_RECV_BODY_SIZE) || CLRPC_MUST_RECV_BODY_SIZE < 8
#error "CLRPC_MUST_RECV_BODY_SIZE incorrect"
#endif

/*------------------------------------------------------------------------------
// pragma message etc.
//----------------------------------------------------------------------------*/

#define CLRPC_STRINGIFY_(n) # n
#define CLRPC_STRINGIFY(n)  CLRPC_STRINGIFY_(n) /* make it a string */

#ifdef _MSC_VER
#ifdef CLRPC_MESSAGE
#error "who defined CLRPC_MESSAGE?"
#else
#ifdef CLRPC_CFG_MESSAGE    /* for developpers only */
/* intended usage: #pragma CLRPC_MESSAGE(...) */
#define CLRPC_MESSAGE(str) message( __FILE__ "(" CLRPC_STRINGIFY(__LINE__) ") : " str)
#endif
#endif  // CLRPC_MESSAGE
#endif  // _MSC_VER

/*------------------------------------------------------------------------------
//	Logging
//----------------------------------------------------------------------------*/

#ifndef CLRPC_LOG_FCT
# define CLRPC_LOG_FCT(fn_)	/* empty */
#endif

/*------------------------------------------------------------------------------
// assertions
//----------------------------------------------------------------------------*/

/*
 * #ifndef ... moved to clrpc_pls.h
 */

/*------------------------------------------------------------------------------
// pointer compare macros
//----------------------------------------------------------------------------*/

#define CLRPC_PTR_IS_NULL(ptr_)             (LSA_HOST_PTR_ARE_EQUAL(CLRPC_TEST_POINTER(ptr_), LSA_NULL))
#define CLRPC_PTR_IS_NOT_NULL(ptr_)         (! LSA_HOST_PTR_ARE_EQUAL(CLRPC_TEST_POINTER(ptr_), LSA_NULL))
#define CLRPC_PTR_ARE_EQUAL(ptr1_, ptr2_)   (LSA_HOST_PTR_ARE_EQUAL(CLRPC_TEST_POINTER(ptr1_), CLRPC_TEST_POINTER(ptr2_)))

/*------------------------------------------------------------------------------
// helper for casting and warning-level
//----------------------------------------------------------------------------*/

#if CLRPC_CFG_USE_CASTS

# define CLRPC_CAST_UPPER_RQB_PTR_PTR(val_)  ((CLRPC_UPPER_RQB_PTR_TYPE CLRPC_LOCAL_MEM_ATTR *)(val_))
# define CLRPC_CAST_UPPER_MEM_PTR_PTR(val_)  ((CLRPC_UPPER_MEM_PTR_TYPE CLRPC_LOCAL_MEM_ATTR *)(val_))

# define CLRPC_CAST_LOCAL_MEM_PTR_PTR(val_)  ((CLRPC_LOCAL_MEM_PTR_TYPE CLRPC_LOCAL_MEM_ATTR *)(val_))

# define CLRPC_CAST_LOWER_RQB_PTR_PTR(val_)  ((CLRPC_LOWER_RQB_PTR_TYPE CLRPC_LOCAL_MEM_ATTR *)(val_))
# define CLRPC_CAST_LOWER_MEM_PTR_PTR(val_)  ((CLRPC_LOWER_MEM_PTR_TYPE CLRPC_LOCAL_MEM_ATTR *)(val_))

#else

# define CLRPC_CAST_UPPER_RQB_PTR_PTR(val_)  (val_)
# define CLRPC_CAST_UPPER_MEM_PTR_PTR(val_)  (val_)

# define CLRPC_CAST_LOCAL_MEM_PTR_PTR(val_)  (val_)

# define CLRPC_CAST_LOWER_RQB_PTR_PTR(val_)  (val_)
# define CLRPC_CAST_LOWER_MEM_PTR_PTR(val_)  (val_)

#endif

#if CLRPC_CFG_USE_CASTS || CLRPC_DEBUG

# define CLRPC_CAST_U16(val_)  ((LSA_UINT16)(val_)) /* checked warnings */

#else

# define CLRPC_CAST_U16(val_)  (val_) /* we want to see all warnings */

#endif

/*------------------------------------------------------------------------------
// channel forward
//----------------------------------------------------------------------------*/

typedef struct clrpc_channel_tag CLRPC_LOCAL_MEM_ATTR * CLRPC_CHANNEL_PTR_TYPE; /* forward */


/*------------------------------------------------------------------------------
// internal UUID functions
// see clrpc_uuid.h for the rest
//----------------------------------------------------------------------------*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_uuid_sys_init (
	LSA_VOID
);

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_uuid_node_set (
	CLRPC_CHANNEL_PTR_TYPE channel,
	CLRPC_COMMON_MEM_U8_PTR_TYPE mac
);

LSA_VOID  CLRPC_UPPER_IN_FCT_ATTR
clrpc_uuid_create (
	CLRPC_CHANNEL_PTR_TYPE channel,
	CLRPC_UUID_PTR_TYPE	uuid
	);

LSA_VOID  CLRPC_UPPER_IN_FCT_ATTR
clrpc_uuid_create_actuid (
	CLRPC_CHANNEL_PTR_TYPE channel,
	CLRPC_UUID_PTR_TYPE	uuid
	);

LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_uuid_to_hash (
	CLRPC_UUID_PTR_TYPE  uuid
	);


/*-----------------------------------------------------------------------------
// circularly linked list macros
// adapted from Microsoft Header SRVSTRUC.H and WDM.H
//---------------------------------------------------------------------------*/

typedef struct clrpc_list_entry_tag CLRPC_COMMON_MEM_ATTR *CLRPC_LIST_ENTRY_PTR_TYPE;

typedef struct clrpc_list_entry_tag {
	CLRPC_LIST_ENTRY_PTR_TYPE Flink; /* forward link */
	CLRPC_LIST_ENTRY_PTR_TYPE Blink; /* backward link */
} CLRPC_LIST_ENTRY_TYPE;

#define CLRPC_LIST_INITIALIZE(ListHead_) {\
	(ListHead_)->Flink = (ListHead_)->Blink = (ListHead_);\
	}

#define CLRPC_LIST_IS_EMPTY(ListHead_) (\
	(ListHead_)->Flink == (ListHead_)\
	)

#define CLRPC_LIST_REMOVE_ENTRY(Entry_) {\
	CLRPC_LIST_ENTRY_PTR_TYPE EX_Entry_;\
	EX_Entry_ = (Entry_);\
	EX_Entry_->Blink->Flink = EX_Entry_->Flink;\
	EX_Entry_->Flink->Blink = EX_Entry_->Blink;\
	}

#define CLRPC_LIST_REMOVE_HEAD(ListHead_, Entry_, Type_) {\
	(Entry_) = (Type_)((ListHead_)->Flink);\
	CLRPC_LIST_REMOVE_ENTRY((ListHead_)->Flink);\
	}

#define CLRPC_LIST_INSERT_HEAD(ListHead_, Entry_) {\
	(Entry_)->Flink = (ListHead_)->Flink;\
	(Entry_)->Blink = (ListHead_);\
	(ListHead_)->Flink->Blink = (Entry_);\
	(ListHead_)->Flink = (Entry_);\
	}

#define CLRPC_LIST_INSERT_TAIL(ListHead_, Entry_) {\
	(Entry_)->Flink = (ListHead_);\
	(Entry_)->Blink = (ListHead_)->Blink;\
	(ListHead_)->Blink->Flink = (Entry_);\
	(ListHead_)->Blink = (Entry_);\
	}

#define CLRPC_LIST_PEEK_HEAD(ListHead_) (\
	CLRPC_LIST_IS_EMPTY(ListHead_) ? LSA_NULL : (ListHead_)->Flink\
	)

#define CLRPC_LIST_FIRST(ListHead_, Type_) (\
	(Type_)(CLRPC_LIST_IS_EMPTY(ListHead_) ? LSA_NULL : (ListHead_)->Flink)\
	)

#define CLRPC_LIST_NEXT(ListHead_, Entry_, Type_) (\
	(Type_)((Entry_)->Flink == (ListHead_) ? LSA_NULL : (Entry_)->Flink)\
	)


/*------------------------------------------------------------------------------
// RQB-Queue
//----------------------------------------------------------------------------*/

typedef struct {
	CLRPC_LIST_ENTRY_TYPE anchor;
} CLRPC_QUEUE_TYPE, CLRPC_COMMON_MEM_ATTR *CLRPC_QUEUE_PTR_TYPE;

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_queue_init(
	CLRPC_QUEUE_PTR_TYPE q
);

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_queue_enqueue(
	CLRPC_QUEUE_PTR_TYPE q,
	LSA_VOID_PTR_TYPE elem
);

LSA_VOID_PTR_TYPE CLRPC_LOCAL_FCT_ATTR
clrpc_queue_dequeue(
	CLRPC_QUEUE_PTR_TYPE q
);

LSA_UINT16 CLRPC_LOCAL_FCT_ATTR
clrpc_queue_not_empty(
	CLRPC_QUEUE_PTR_TYPE q
);

LSA_VOID_PTR_TYPE CLRPC_LOCAL_FCT_ATTR
clrpc_queue_peek(
	CLRPC_QUEUE_PTR_TYPE q
);

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_queue_unlink (
	CLRPC_QUEUE_PTR_TYPE q,
	LSA_VOID_PTR_TYPE elem
);


/*------------------------------------------------------------------------------
// oneshot timer
//----------------------------------------------------------------------------*/

typedef struct clrpc_oneshot_glob_tag
{
	CLRPC_LIST_ENTRY_TYPE dlist; /* anchor of oneshot-list */
	LSA_UINT32 msec_per_tick;
	LSA_UINT16 factor;
	LSA_UINT16 timer_running;
	LSA_UINT16 timer_id;
	LSA_USER_ID_TYPE user_id;
	CLRPC_RQB_TYPE timerSyncRQB; /* 'response' is used as 'is-posted-flag', see clrpc_timeout() and clrpc_timeout_synchronized() */
} CLRPC_ONESHOT_GLOB_TYPE;



LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_timeout_synchronized(
    CLRPC_UPPER_RQB_PTR_TYPE  rb
);

typedef struct clrpc_oneshot_tag CLRPC_LOCAL_MEM_ATTR * CLRPC_ONESHOT_PTR_TYPE;

typedef LSA_VOID LSA_FCT_PTR(CLRPC_LOCAL_FCT_ATTR,
	CLRPC_ONESHOT_CALLBACK)(
		CLRPC_ONESHOT_PTR_TYPE oneshot,
		LSA_VOID_PTR_TYPE user_ptr
	);

typedef struct clrpc_oneshot_tag {
	CLRPC_LIST_ENTRY_TYPE dlink; /* must be first in struct */
	CLRPC_ONESHOT_CALLBACK callback;
	LSA_VOID_PTR_TYPE user_ptr;
	LSA_UINT32 ticks;
	LSA_UINT16 running;
} CLRPC_ONESHOT_TYPE;

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_oneshot_sys_init(
	LSA_UINT16 time_base,
	LSA_UINT16 time_factor
);

LSA_VOID clrpc_oneshot_sys_undo_init(
	LSA_VOID
);

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_oneshot_init(
	CLRPC_ONESHOT_PTR_TYPE oneshot,
	CLRPC_ONESHOT_CALLBACK callback,
	LSA_VOID_PTR_TYPE user_ptr
);

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_oneshot_trigger(
	CLRPC_ONESHOT_PTR_TYPE oneshot,
	LSA_UINT32 timeout_msec
);

LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_oneshot_stop(
	CLRPC_ONESHOT_PTR_TYPE oneshot
);

LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_oneshot_is_running(
	CLRPC_ONESHOT_PTR_TYPE oneshot
);


/*------------------------------------------------------------------------------
// HASH functions
//----------------------------------------------------------------------------*/

#define CLRPC_HASH_BUCKETS_MAX 32 /* VV: make configurable? max. NO-HINT - 1! */

#define CLRPC_HASH_HINT_TO_INDEX(hint_) ((hint_) % CLRPC_HASH_BUCKETS_MAX)

typedef struct {
	CLRPC_LIST_ENTRY_TYPE dlink; /* first in struct! */
	CLRPC_UUID_TYPE uuid;
	LSA_VOID_PTR_TYPE user_ptr;
} CLRPC_HASH_ENTRY_TYPE, CLRPC_LOCAL_MEM_ATTR * CLRPC_HASH_ENTRY_PTR_TYPE;

typedef struct {
	CLRPC_LIST_ENTRY_TYPE bucket[CLRPC_HASH_BUCKETS_MAX]; /* an array of lists of hash-listentry-type */
	CLRPC_LIST_ENTRY_TYPE iter_list; /* a list of iterators */
} CLRPC_HASH_TYPE, CLRPC_LOCAL_MEM_ATTR * CLRPC_HASH_PTR_TYPE;

typedef struct {
	CLRPC_LIST_ENTRY_TYPE dlink; /* first in struct! */
	CLRPC_HASH_PTR_TYPE hash;
	LSA_UINT16 bucket;
	CLRPC_HASH_ENTRY_PTR_TYPE entry;
} CLRPC_HASH_ITERATOR_TYPE, CLRPC_LOCAL_MEM_ATTR * CLRPC_HASH_ITERATOR_PTR_TYPE;


LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_init (
	CLRPC_HASH_PTR_TYPE  hash
);

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_undo_init (
	CLRPC_HASH_PTR_TYPE  hash
);

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_entry_setup ( /* prepare this entry for adding */
	CLRPC_HASH_ENTRY_PTR_TYPE  entry,
	CLRPC_UUID_PTR_TYPE uuid,
	LSA_VOID_PTR_TYPE user_ptr
);

LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_add (
	CLRPC_HASH_PTR_TYPE  hash,
	CLRPC_HASH_ENTRY_PTR_TYPE entry /* uuid and user_ptr must be set (user_ptr NOT NULL!!!)*/
); /* returns the hint, succeeds always */

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_remove (
	CLRPC_HASH_PTR_TYPE  hash,
	CLRPC_UUID_PTR_TYPE uuid /* must exist or fatal */
);

CLRPC_HASH_ENTRY_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_find ( /* returns the hash-entry! */
	CLRPC_HASH_PTR_TYPE  hash,
	CLRPC_UUID_PTR_TYPE  uuid,
	LSA_UINT16 hint
);

LSA_VOID_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_lookup ( /* returns the user_ptr for convenience! */
	CLRPC_HASH_PTR_TYPE  hash,
	CLRPC_UUID_PTR_TYPE uuid,
	LSA_UINT16  hint
);

#if CLRPC_DEBUG
LSA_UINT32  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_count_entries (
	CLRPC_HASH_PTR_TYPE  hash
);
#endif

/*------------------------------------------------------------------------------
// HASH iterator
//----------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_iter_setup ( /* prepare this iterator for attaching */
	CLRPC_HASH_ITERATOR_PTR_TYPE  iter
);

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_iter_attach ( /* attach this iterator to the hash */
	CLRPC_HASH_ITERATOR_PTR_TYPE  iter,
	CLRPC_HASH_PTR_TYPE  hash
);

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_iter_detach ( /* detach this iterator from the hash */
	CLRPC_HASH_ITERATOR_PTR_TYPE  iter
);

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_iter_reset (
	CLRPC_HASH_ITERATOR_PTR_TYPE  iter
);

LSA_VOID_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_hash_iter_next ( /* returns the user_ptr for convenience! */
	CLRPC_HASH_ITERATOR_PTR_TYPE  iter
);


/*------------------------------------------------------------------------------
// dispatch tag
//----------------------------------------------------------------------------*/

typedef LSA_VOID
	LSA_FCT_PTR(CLRPC_LOCAL_FCT_ATTR, CLRPC_DISPATCH_TAG)( /* the tag is a function pointer */
		CLRPC_CHANNEL_PTR_TYPE channel,
		LSA_VOID_PTR_TYPE instance,
		CLRPC_LOWER_RQB_PTR_TYPE lower
	);

#define CLRPC_SET_DISPATCH_TAG(tag,value) {tag = value;}

#define CLRPC_DISPATCH(ptr, channel, instance, lower) { \
		( *(CLRPC_DISPATCH_TAG CLRPC_COMMON_MEM_ATTR *)(ptr) )(channel, instance, lower); \
	}

#define CLRPC_CONTAINING_STRUCT(field_ptr, struct_ptr_type, field) \
	((struct_ptr_type) \
		((CLRPC_COMMON_MEM_U8_PTR_TYPE)(field_ptr) - (LSA_INT)( &((struct_ptr_type)0)->field )))

/*------------------------------------------------------------------------------
// client states, -type, -prototypes
//----------------------------------------------------------------------------*/

#if CLRPC_CFG_MAX_CLIENTS

#include "clrpc_icl.h" /* declares CLRPC_CL_CHANNEL */

#endif


/*------------------------------------------------------------------------------
// server states, -type, -prototypes
//----------------------------------------------------------------------------*/

#if CLRPC_CFG_MAX_SERVERS

#include "clrpc_isv.h" /* CLRPC_SV_CHANNEL */

#endif


/*------------------------------------------------------------------------------
// channel states, -type and -table
//----------------------------------------------------------------------------*/

/* sys has lower-handle if OPEN <= state <= CLOSE */
enum clrpc_channel_state_enum {
	CLRPC_ST_FREE = 0,
	CLRPC_ST_IDLE,

	CLRPC_ST_OPEN,
	CLRPC_ST_READY,
	CLRPC_ST_CLOSE,

	CLRPC_ST_OPEN_ERROR
};

typedef struct clrpc_channel_tag {

	LSA_UINT16				state;			/* channel state, see clrpc_channel_state_enum */
	LSA_HANDLE_TYPE			my_handle;		/* my own handle */
	LSA_HANDLE_TYPE			handle_upper;	/* the handle of the user */
	CLRPC_UPPER_CALLBACK_FCT_PTR_TYPE
							callback;		/* the callback-function of the user */

	LSA_SYS_PTR_TYPE		sysptr;			/* the 'hint' for the system adaption */
	CLRPC_DETAIL_PTR_TYPE	detailptr;		/* pointer to channel-details */

	CLRPC_UPPER_RQB_PTR_TYPE
							upper;			/* current working upper request block */

	LSA_HANDLE_TYPE			handle_lower;	/* handle of lower layer */

	LSA_UINT32				trace_idx;		/* for "LSA index trace" */
	LSA_UINT8				mac_addr[6];	/* for clrpc_uuid_create() */

#if CLRPC_CFG_MAX_CLIENTS
	CLRPC_CL_CHANNEL_TYPE	cl; /* client things */
#endif

#if CLRPC_CFG_MAX_SERVERS
	CLRPC_SV_CHANNEL_TYPE	sv;	/* server things */
#endif

    CLRPC_QUEUE_TYPE        tmp_rcv_queue;  /* temp Queue for Receive-Resources during SET_INTERFACE_PARAM */
    
} CLRPC_CHANNEL_TYPE;

/* the channel table */

extern CLRPC_CHANNEL_TYPE clrpc_channels[CLRPC_CFG_MAX_CHANNELS]; /* defined in clrpc_usr.c */

extern LSA_USER_ID_TYPE clrpc_null_user_id; /* defined in clrpc_sys.c */


/*------------------------------------------------------------------------------
//global defines 
//----------------------------------------------------------------------------*/
#define CLRPC_MAC_ADDR_INVALID  0xff
#define CLRPC_BROADCAST_ADDR    0x01

#define CLRPC_CLOCK_SEQ_MASK    0x3fff

#define CLRPC_100NS_PER_SEC    10000000
#define CLRPC_100NS_PER_MSEC   10000

/*------------------------------------------------------------------------------
//global clrpc_data 
//----------------------------------------------------------------------------*/
typedef struct clrpc_data_tag
{
    LSA_UINT16                                  clkseq;     
    LSA_UINT64                                  counter_adjust;
    LSA_UINT64                                  last_counter;
    LSA_UINT32                                  last_boot_time;
    CLRPC_CHANNEL_TYPE  (CLRPC_LOCAL_MEM_ATTR *   channels_ptr)[CLRPC_CFG_MAX_CHANNELS];
#if CLRPC_CFG_MAX_SERVERS 
    CLRPC_SERVER_TYPE      (CLRPC_LOCAL_MEM_ATTR * servers_ptr)[CLRPC_CFG_MAX_SERVERS];
    CLRPC_HASH_PTR_TYPE                         sc_hash_ptr;
    CLRPC_HASH_PTR_TYPE                         ctx_hash_ptr;
#endif
    CLRPC_ONESHOT_GLOB_TYPE CLRPC_LOCAL_MEM_ATTR * oneshot_ptr;
}CLRPC_DATA_TYPE;



#if CLRPC_CFG_MAX_CLIENTS /* compile only, if client-part is configured */
/*===========================================================================*/
/*                            constants for pdu handling                     */
/*===========================================================================*/
#define CLRPC_MARSHAL16(ptr_, val_) { \
	CLRPC_PUT16_ALIGNED((*(ptr_)), (val_)); \
	(*(ptr_)) += sizeof(LSA_UINT16); \
	}

#define CLRPC_MARSHAL32(ptr_, val_) { \
	CLRPC_PUT32_ALIGNED((*(ptr_)), (val_)); \
	(*(ptr_)) += sizeof(LSA_UINT32); \
	}

#define CLRPC_MARSHAL_UUID(ptr_, uuidptr_) { \
	CLRPC_PUT_UUID_ALIGNED((*(ptr_)), (uuidptr_)); \
	(*(ptr_)) += sizeof(CLRPC_UUID_TYPE); \
	}


#define CLRPC_ALIGN(bytes_left_, ptr_, type_) { \
	(*(ptr_)) = (LSA_UINT8*)((LSA_UINT32)((*(ptr_)) + (sizeof(type_) - 1)) & ~(sizeof(type_) - 1)); \
	(*(bytes_left_)) &= ~(sizeof(type_) - 1); \
	}

#define CLRPC_ADVANCE(ptr_, bytes_left_, bytes_) { \
	(*(ptr_)) += (bytes_); \
	(*(bytes_left_)) -= (bytes_); \
	}

#define CLRPC_UNMARSHAL32(ptr_, bytes_left_, dst_, qry_, jump_label_) { \
	if( (*(bytes_left_)) < sizeof(LSA_UINT32) ) goto jump_label_; \
	(*(dst_)) = CLRPC_GET32_ALIGNED(*(ptr_)); \
	if( CLRPC_DREP_INT_DIFFERS((qry_)->u.call.drep.drep_int) ) { \
		CLRPC_SWAP_U32(&(*(dst_))); \
	} \
	CLRPC_ADVANCE((ptr_), (bytes_left_), sizeof(LSA_UINT32)); \
	}

#define CLRPC_UNMARSHAL_UUID(ptr_, bytes_left_, dst_, qry_, jump_label_) { \
	if( (*(bytes_left_)) < sizeof(CLRPC_UUID_TYPE) ) goto jump_label_; \
  	*(dst_) = *(CLRPC_UUID_TYPE CLRPC_COMMON_MEM_ATTR *)(CLRPC_TEST_ALIGN4(*(ptr_))); \
	if( CLRPC_DREP_INT_DIFFERS((qry_)->u.call.drep.drep_int) ) { \
		CLRPC_SWAP_UUID(&(*(dst_))); \
	} \
	CLRPC_ADVANCE((ptr_), (bytes_left_), sizeof(CLRPC_UUID_TYPE)); \
	}

#define CLRPC_UNMARSHAL_BUF(ptr_, bytes_left_, dstptr_, bytes_, jump_label_) { \
	if( *(bytes_left_) < (bytes_) ) goto jump_label_ ; \
	*(dstptr_) = (LSA_UINT8*)(*(ptr_)); \
	CLRPC_ADVANCE((ptr_), (bytes_left_), bytes_); \
	}

#define CLRPC_UNMARSHAL16_LITTLE(ptr_, bytes_left_, dst_, jump_label_) { \
	if( *(bytes_left_) < sizeof(LSA_UINT16) ) goto jump_label_; \
	*(dst_) = CLRPC_GET16_LITTLE(*(ptr_)); \
	CLRPC_ADVANCE((ptr_), (bytes_left_), sizeof(LSA_UINT16)); \
	}
    
#endif  // CLRPC_CFG_MAX_CLIENTS 

#define CLRPC_MUST_RECV_BODY_SIZE_INT   CLRPC_MUST_RECV_BODY_SIZE   
        /* AP01309324: local constant for different values in test cases */
        
#if ! defined(CLRPC_MUST_RECV_BODY_SIZE_INT) || CLRPC_MUST_RECV_BODY_SIZE_INT < 8 || CLRPC_MUST_RECV_BODY_SIZE_INT > CLRPC_MUST_RECV_BODY_SIZE
#error "CLRPC_MUST_RECV_BODY_SIZE_INT incorrect"
#endif

	/* compile only, when server-part is needed */
#if CLRPC_CFG_MAX_SERVERS
/*===========================================================================*/
/*      constants for EPM                                                    */
/*===========================================================================*/

#define CLRPC_EPM_DP_U32(dp, u32)		{CLRPC_PUT32_ALIGNED    (dp, (u32)); (dp) += 4;}
#define CLRPC_EPM_DP_U16(dp, u16)		{CLRPC_PUT16_ALIGNED    (dp, (u16)); (dp) += 2;}
#define CLRPC_EPM_DP_U8(dp, u8)			{CLRPC_PUT8_ALIGNED     (dp, (u8)); (dp) += 1;}
#define CLRPC_EPM_DP_UUID(dp, uuid_ptr)	{CLRPC_PUT_UUID_ALIGNED (dp, (uuid_ptr)); dp += sizeof(CLRPC_UUID_TYPE);}

#define CLRPC_EPM_DP_U16_LITTLE(dp, u16)		{CLRPC_PUT16_LITTLE    (dp, (u16)); (dp) += 2;}
#define CLRPC_EPM_DP_UUID_LITTLE(dp, uuid_ptr)	{CLRPC_PUT_UUID_LITTLE (dp, (uuid_ptr)); dp += sizeof(CLRPC_UUID_TYPE);}
    
#endif  // CLRPC_CFG_MAX_SERVERS

/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/

#if (CLRPC_MODULE_ID == 1)

#ifdef CLRPC_CLEAR_GLOBAL_DATA
CLRPC_DATA_TYPE  CLRPC_LOCAL_MEM_ATTR   clrpc_data = {0};
CLRPC_ONESHOT_GLOB_TYPE                 clrpc_oneshot = {0};
#else
CLRPC_DATA_TYPE  CLRPC_LOCAL_MEM_ATTR   clrpc_data;
CLRPC_ONESHOT_GLOB_TYPE                 clrpc_oneshot;
#endif

#else
LSA_EXTERN CLRPC_DATA_TYPE  CLRPC_LOCAL_MEM_ATTR  clrpc_data;
LSA_EXTERN CLRPC_ONESHOT_GLOB_TYPE clrpc_oneshot;
#endif



/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_callback (
	CLRPC_CHANNEL_PTR_TYPE channel,
	LSA_UINT16 response,
	CLRPC_UPPER_RQB_PTR_TYPE CLRPC_LOCAL_MEM_ATTR *upper_ptr, 
    LSA_UINT16 module, 
    LSA_INT line
);

/*---------------------------------------------------------------------------*/

CLRPC_CHANNEL_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_system_channel (
	LSA_VOID
);

LSA_BOOL  CLRPC_LOCAL_FCT_ATTR
clrpc_system_channel_is_ready (
	LSA_VOID
);

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_request_lower(
    CLRPC_LOWER_RQB_PTR_TYPE  lower,
    LSA_OPCODE_TYPE  opcode,
    LSA_VOID_PTR_TYPE  user_id
);

/*---------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_channel_done (
	CLRPC_CHANNEL_PTR_TYPE channel,
	LSA_UINT32 result
);

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_channel_set_trace_idx (
	CLRPC_CHANNEL_PTR_TYPE channel,
	LSA_UINT32 trace_idx
);


/*---------------------------------------------------------------------------*/

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_ll_alloc_rqb (
	CLRPC_CHANNEL_PTR_TYPE  channel,
	LSA_OPCODE_TYPE  opcode,
	LSA_VOID_PTR_TYPE  user_id,
	CLRPC_LOWER_RQB_PTR_TYPE  *lower_rqb_ptr_ptr
);

LSA_VOID  CLRPC_LOCAL_FCT_ATTR
clrpc_ll_free_rqb (
	CLRPC_CHANNEL_PTR_TYPE  channel,
	CLRPC_LOWER_RQB_PTR_TYPE  *lower_rqb_ptr
);

CLRPC_LOWER_RQB_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_sock_alloc_rqb (
	LSA_VOID
);

CLRPC_LOWER_RQB_PTR_TYPE  CLRPC_LOCAL_FCT_ATTR
clrpc_sock_alloc_rxtx_rqb (
	LSA_VOID
);

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_clear_interface_param_int (
	CLRPC_CHANNEL_PTR_TYPE      channel
);
    
LSA_VOID CLRPC_LOCAL_FCT_ATTR 
clrpc_op_set_interface_param(
	CLRPC_CHANNEL_PTR_TYPE      channel,
	CLRPC_UPPER_RQB_PTR_TYPE    rqb_ptr
);

LSA_BOOL CLRPC_LOCAL_FCT_ATTR
clrpc_mac_addr_is_valid (
	CLRPC_COMMON_MEM_U8_PTR_TYPE mac_addr
); 

LSA_BOOL CLRPC_LOCAL_FCT_ATTR clrpc_alloc_rcv_resources(
	CLRPC_CHANNEL_PTR_TYPE      channel
);

LSA_VOID CLRPC_LOCAL_FCT_ATTR 
clrpc_op_clear_interface_param(
	CLRPC_CHANNEL_PTR_TYPE      channel,
	CLRPC_UPPER_RQB_PTR_TYPE    rqb_ptr
);

LSA_BOOL CLRPC_LOCAL_FCT_ATTR
clrpc_mac_addr_is_zero (
	CLRPC_COMMON_MEM_U8_PTR_TYPE mac_addr
);

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_close_channel_next_step(
    CLRPC_CHANNEL_PTR_TYPE channel
);

LSA_VOID CLRPC_LOCAL_FCT_ATTR 
clrpc_clear_interface_param_next_step(
	CLRPC_CHANNEL_PTR_TYPE      channel
);

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_udp_open_socket_cnf(
	CLRPC_CHANNEL_PTR_TYPE channel
); 

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_cl_close_socket_cnf(
	CLRPC_CHANNEL_PTR_TYPE channel
);

/*----------------------------------------------------------------------------*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of CLRPC_INT_H */
