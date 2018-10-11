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
/*  F i l e               &F: clrpc_uuid.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the UUID routines                                             */
/*                                                                           */
/*  see OSF Sources, files UUID.C and UUID.H                                 */
/*                                                                           */
/*  cited OSF sources DCE 1.1 are available from http://www.opengroup.org    */
/*                                                                           */
/*****************************************************************************/

#define CLRPC_MODULE_ID    31
#define LTRC_ACT_MODUL_ID  31

#include "clrpc_inc.h"
#include "clrpc_int.h"

CLRPC_FILE_SYSTEM_EXTENSION(CLRPC_MODULE_ID)


/*---------------------------------------------------------------------------*/

CLRPC_UUID_TYPE clrpc_uuid_nil_uuid; /* see clrpc_uuid_sys_init(), do not initialize here! */

CLRPC_UUID_TYPE clrpc_conversation_manager_uuid; /* see clrpc_uuid_sys_init(), do not initialize here! */

CLRPC_UUID_TYPE clrpc_convc_uuid; /* see clrpc_uuid_sys_init(), do not initialize here! */

CLRPC_UUID_TYPE clrpc_endpoint_mapper_uuid; /* see clrpc_uuid_sys_init(), do not initialize here! */

#if CLRPC_CFG_MAX_SERVERS
CLRPC_UUID_TYPE clrpc_endpoint_type_id; /* see clrpc_uuid_sys_init(), do not initialize here! */
#endif

/*---------------------------------------------------------------------------*/

#define CLRPC_TIME_MID_MASK                 0x0000ffff00000000ULL
#define CLRPC_TIME_HIGH_MASK                0x0fff000000000000ULL
#define CLRPC_TIME_HIGH_SHIFT_COUNT         48
#define CLRPC_TIME_MID_SHIFT_COUNT          32

/* activity uuid uses 52 bits only */
#define CLRPC_ACT_UUID_MASK                 0x000fffffffffffffULL

/* maximum value for adjustment */
#define CLRPC_MAX_TIME_ADJUST               0x000fffffffffffffLL
/*---------------------------------------------------------------------------*/

#define CLRPC_CLOCK_SEQ_MAX               0x3fff
#define CLRPC_CLOCK_SEQ_LOW_MASK          0xff
#define CLRPC_CLOCK_SEQ_HIGH_MASK         0x3f00
#define CLRPC_CLOCK_SEQ_HIGH_SHIFT_COUNT  8

/*#define CLRPC_CLOCK_SEQ_FIRST             1*/
//#define CLRPC_CLOCK_SEQ_LAST              0x3fff

/*#define CLRPC_CLOCK_SEQ_BUMP(seq_)         ((*seq_) = ((*seq_) + 1) & CLRPC_CLOCK_SEQ_LAST)*/

/*---------------------------------------------------------------------------*/

#define CLRPC_UUID_VERSION        (1)
#define CLRPC_UUID_VERSION_BITS   (CLRPC_UUID_VERSION << 12)

#define CLRPC_UUID_RESERVED_BITS  0x80 /* "DCE Variant" */

/*---------------------------------------------------------------------------*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_uuid_sys_init (
	LSA_VOID
) {
	CLRPC_LOG_FCT("clrpc_uuid_sys_init")

	/***/

	CLRPC_MEMSET(&clrpc_uuid_nil_uuid, 0, sizeof(clrpc_uuid_nil_uuid));

	/***/

	{
	CLRPC_UUID_TYPE tmp = { 0x333a2276, 0x0000, 0x0000, /*clk*/0x0d, 0x00, /*node*/{0x00, 0x80, 0x9c, 0x00, 0x00, 0x00} };

	clrpc_conversation_manager_uuid = tmp;
	}

	/***/

	{
	CLRPC_UUID_TYPE tmp = { 0x4a967f14, 0x3000, 0x0000, /*clk*/0x0d, 0x00, /*node*/{0x01, 0x28, 0x82, 0x00, 0x00, 0x00} };

	clrpc_convc_uuid = tmp;
	}

	/***/

	{
		/* uuid(e1af8308-5d1f-11c9-91a4-08002b14a0fa), version(3.0), */
	CLRPC_UUID_TYPE tmp = { 0xe1af8308, 0x5d1f, 0x11c9, /*clk*/0x91, 0xa4, /*node*/{0x08, 0x00, 0x2b, 0x14, 0xa0, 0xfa} };

	clrpc_endpoint_mapper_uuid = tmp;
	}

#if CLRPC_CFG_MAX_SERVERS
	{
	CLRPC_UUID_TYPE tmp = { 0x8a885d04, 0x1ceb, 0x11c9, /*clk*/0x9f, 0xe8, /*node*/{0x08, 0x00, 0x2b, 0x10, 0x48, 0x60} };

	clrpc_endpoint_type_id = tmp;
	}
#endif
}

/*---------------------------------------------------------------------------*/

LSA_BOOL CLRPC_LOCAL_FCT_ATTR
clrpc_mac_addr_is_zero (
	CLRPC_COMMON_MEM_U8_PTR_TYPE mac_addr
) {
	CLRPC_COMMON_MEM_U8_PTR_TYPE  a = mac_addr;

	if (a[0] == 0 && a[1] == 0 && a[2] == 0 && a[3] == 0 && a[4] == 0 && a[5] == 0)
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_VOID CLRPC_LOCAL_FCT_ATTR
clrpc_uuid_node_set (
	CLRPC_CHANNEL_PTR_TYPE channel,
	CLRPC_COMMON_MEM_U8_PTR_TYPE mac_addr
) {
	CLRPC_LOG_FCT("clrpc_uuid_node_set")

	//if (clrpc_mac_addr_is_zero (channel->mac_addr) && !clrpc_mac_addr_is_zero (mac_addr))
	if (!clrpc_mac_addr_is_valid(channel->mac_addr) && !clrpc_mac_addr_is_zero (mac_addr))
	{
		CLRPC_UPPER_TRACE_06 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "clrpc_uuid_set_node: mac(%02x-%02x-%02x-%02x-%02x-%02x)"
			, mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]
			);

		CLRPC_MEMCPY(channel->mac_addr, mac_addr, sizeof(channel->mac_addr));

#if CLRPC_CFG_MAX_CLIENTS
		CLRPC_ASSERT(clrpc_uuid_is_nil(&channel->cl.cas_uuid));
		clrpc_uuid_create(channel, &channel->cl.cas_uuid);

		CLRPC_UPPER_TRACE_11 (channel->trace_idx, LSA_TRACE_LEVEL_CHAT
			, "channel.cas-uuid(%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x)"
			, channel->cl.cas_uuid.time_low, channel->cl.cas_uuid.time_mid, channel->cl.cas_uuid.time_hi_and_version
			, channel->cl.cas_uuid.clock_seq_hi_and_reserved, channel->cl.cas_uuid.clock_seq_low
			, channel->cl.cas_uuid.node[0], channel->cl.cas_uuid.node[1], channel->cl.cas_uuid.node[2]
			, channel->cl.cas_uuid.node[3], channel->cl.cas_uuid.node[4], channel->cl.cas_uuid.node[5]
			);
#endif
	}
}

/*---------------------------------------------------------------------------*/

LSA_VOID  CLRPC_UPPER_IN_FCT_ATTR
clrpc_uuid_create_actuid (
	CLRPC_CHANNEL_PTR_TYPE channel,
	CLRPC_UUID_PTR_TYPE	uuid
) {
	LSA_UINT16 tmp;

	/*
	 * see OSF Sources, File DGCCT.C line 457ff
	 */

	clrpc_uuid_create(channel, uuid);

    tmp = uuid->time_hi_and_version;

    uuid->time_hi_and_version =
        (tmp        & 0xf000) |     /* Preserve version # */
        ((tmp << 8) & 0x0f00) |     /* Dup <51:48> of time into high byte */
        ((tmp >> 8) & 0x00f0) |     /* Dup version # down to low byte */
        (tmp        & 0x000f);      /* Preserve <51:48> of time */
}

/*---------------------------------------------------------------------------*/
LSA_VOID CLRPC_LOCAL_FCT_ATTR clrpc_create_new_clkseq(LSA_VOID)
{
    /* for clkseq only 14 bits are used */
    if (clrpc_data.clkseq >= CLRPC_CLOCK_SEQ_MAX)   //0x3fff)
    {
        clrpc_data.clkseq = 0;
    }
    else
    {
        clrpc_data.clkseq++;
    }
    clrpc_data.counter_adjust = 0;
}
/*---------------------------------------------------------------------------*/

LSA_VOID  CLRPC_UPPER_IN_FCT_ATTR
clrpc_uuid_create(
	CLRPC_CHANNEL_PTR_TYPE channel,
	CLRPC_UUID_PTR_TYPE	uuid
) 
{
    LSA_UINT64 act_counter;
    LSA_UINT64 act_int_counter;
    LSA_UINT16 ret_val = LSA_RET_ERR_PARAM;
    LSA_INT64 tmp_adjust;

	/*
	 * OSF Sources, file UUID.C line 543
	 */

    CLRPC_GET_100NS_COUNTER(&ret_val, &act_counter);
    if (ret_val != LSA_RET_OK)
    {
        CLRPC_FATAL1(ret_val);
    }
    else
    {
        act_int_counter = act_counter;
        /* for act_uuids we use only 52 bits of the time, so only these bits are checked */
        act_int_counter &= CLRPC_ACT_UUID_MASK;
    
        if (act_int_counter < clrpc_data.last_counter)
        {
            /* clock was set, new clocksequence */
            clrpc_create_new_clkseq();
        }
        else if (act_int_counter == clrpc_data.last_counter)
        {
            if (clrpc_data.counter_adjust >= CLRPC_MAX_TIME_ADJUST) 
            {
                clrpc_create_new_clkseq();
            }
            else
            {
                clrpc_data.counter_adjust++;
                if ((act_int_counter + clrpc_data.counter_adjust) > CLRPC_ACT_UUID_MASK)
                {
                    /* new clocksequence */
                    clrpc_create_new_clkseq();
                }
            }
        }
        else
        {
            tmp_adjust =(LSA_INT64) ((clrpc_data.last_counter + clrpc_data.counter_adjust) - act_int_counter) + 1;
            if (tmp_adjust <= 0)
            {
                clrpc_data.counter_adjust = 0;
            }
            else if (tmp_adjust >= CLRPC_MAX_TIME_ADJUST) 
            {
                clrpc_create_new_clkseq();
            }
            else
            {
                clrpc_data.counter_adjust = (LSA_UINT16)tmp_adjust;
                if ((act_int_counter + clrpc_data.counter_adjust) > CLRPC_ACT_UUID_MASK)
                {
                    /* new clocksequence */
                    clrpc_create_new_clkseq();
                }
            }
        }
        clrpc_data.last_counter = act_int_counter;
        
        act_counter += clrpc_data.counter_adjust;
    
    
        uuid->time_low = (LSA_UINT32)act_counter;
        uuid->time_mid = (LSA_UINT16)((act_counter & CLRPC_TIME_MID_MASK) >> CLRPC_TIME_MID_SHIFT_COUNT);
        
        uuid->time_hi_and_version = (LSA_UINT16)((act_counter & CLRPC_TIME_HIGH_MASK) >> CLRPC_TIME_HIGH_SHIFT_COUNT);
	    uuid->time_hi_and_version |= CLRPC_UUID_VERSION_BITS;
    
	    uuid->clock_seq_low = clrpc_data.clkseq & CLRPC_CLOCK_SEQ_LOW_MASK;
	    uuid->clock_seq_hi_and_reserved = (clrpc_data.clkseq & CLRPC_CLOCK_SEQ_HIGH_MASK) >> CLRPC_CLOCK_SEQ_HIGH_SHIFT_COUNT;

	    uuid->clock_seq_hi_and_reserved |= CLRPC_UUID_RESERVED_BITS;

	    CLRPC_MEMCPY(uuid->node, channel->mac_addr, sizeof(uuid->node));
    }
}

/*---------------------------------------------------------------------------*/
LSA_UINT16  CLRPC_UPPER_IN_FCT_ATTR
clrpc_uuid_equal (
	CLRPC_UUID_PTR_TYPE	uuid1,
	CLRPC_UUID_PTR_TYPE	uuid2
	)
{
	/* OSF Sources UUID.C line 1056
	 *
	 * Note: This used to be a memcmp(), but changed to a field-by-field compare
	 * because of portability problems with alignment and garbage in a UUID.
	 *
	 * see AESRPC/Appendix A. Universal Unique Identifier/A.4 Comparing UUIDs
	 */

	if ((uuid1->time_low == uuid2->time_low)
	&& (uuid1->time_mid == uuid2->time_mid)
	&& (uuid1->time_hi_and_version == uuid2->time_hi_and_version)
	&& (uuid1->clock_seq_hi_and_reserved == uuid2->clock_seq_hi_and_reserved)
	&& (uuid1->clock_seq_low == uuid2->clock_seq_low)
	&& uuid1->node[0] == uuid2->node[0]
	&& uuid1->node[1] == uuid2->node[1]
	&& uuid1->node[2] == uuid2->node[2]
	&& uuid1->node[3] == uuid2->node[3]
	&& uuid1->node[4] == uuid2->node[4]
	&& uuid1->node[5] == uuid2->node[5]
	) {
		return 1;
	}
	return 0;
}

/*---------------------------------------------------------------------------*/

LSA_UINT16  CLRPC_UPPER_IN_FCT_ATTR
clrpc_uuid_is_nil (
	CLRPC_UUID_PTR_TYPE	uuid
	)
{
	return clrpc_uuid_equal (uuid, & clrpc_uuid_nil_uuid);
}

/*---------------------------------------------------------------------------*/

LSA_UINT16  CLRPC_LOCAL_FCT_ATTR
clrpc_uuid_to_hash (
	CLRPC_UUID_PTR_TYPE  uuid
	)
{
#if 1

	/* see OSF sources, file DGUTL.C, line 631, rpc__dg_uuid_hash()
	 *
	 * A significantly faster (though likely not as good) version of uuid_hash()
	 * (this is the NCS 1.5.1 implementation).
	 */

#if 0
	/* original OSF-Code breaks because of packing of CLRPC_UUID_TYPE */
CLRPC_TEST_ALIGN4(uuid);
{
    LSA_UINT32 *lp = (LSA_UINT32 *) uuid;
    LSA_UINT32 hash = lp[0] ^ lp[1] ^ lp[2] ^ lp[3];
    return (LSA_UINT16)((hash >> 16) ^ hash); /* note: may give CLRPC_NO_HINT */
}
#endif

uuid = (CLRPC_UUID_PTR_TYPE)CLRPC_TEST_ALIGN2(uuid);
{
    LSA_UINT16 CLRPC_COMMON_MEM_ATTR * wp = (LSA_UINT16 CLRPC_COMMON_MEM_ATTR *) uuid;
    LSA_UINT16 hash = wp[0] ^ wp[1] ^ wp[2] ^ wp[3] ^ wp[4] ^ wp[5] ^ wp[6] ^ wp[7];
    return hash; /* note: may give CLRPC_NO_HINT */
}
#else

	/* see OSF sources, file UUID.C, line 1313 */
	/* nearly original from OSF */

	LSA_UINT16 hash_id = 0;

	LSA_UINT16          i, c0, c1;
	LSA_UINT16          x, y;
	LSA_UINT8           *next_uuid;

	/*
	 * initialize counters
	 */
	c0 = c1 = 0;
	next_uuid = (LSA_UINT8*)uuid;

	for (i = 0; i < sizeof(*uuid); i++)
	{
		c0 = c0 + *next_uuid++;
		c1 = c1 + c0;
	}

	/*
	 *  Calculate the value for "First octet" of the hash
	 */
	x = -c1 % 255;
	if (x < 0)
	{
		x = x + 255;
	}

	/*
	 *  Calculate the value for "second octet" of the hash
	 */
	y = (c1 - c0) % 255;
	if (y < 0)
	{
		y = y + 255;
	}

	/*
	 * return the pieces put together
	 */
	hash_id = (y << 8) + x;

	return hash_id;

#endif
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
