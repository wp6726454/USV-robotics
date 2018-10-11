/*
 * no re-include protection!!!
 * this header does a sanity check internally
 */

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
/*  C o m p o n e n t     &C: PNIO                                      :C&  */
/*                                                                           */
/*  F i l e               &F: pnio_pck2_on.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Headerfile to support packing within PNIO components                     */
/*                                                                           */
/*  -- DEFINITIONS FOR 2-BYTE-PACKING --                                     */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*****************************************************************************/


/*---------------------------------------------------------------------------*/
/*      0.  USAGE                                                            */
/*---------------------------------------------------------------------------*/
/*
There are some Toolchains (Microsoft, GNU, ...) supported in these packing headers. 
In combination with the attribute keywords the packing mechanism of every compiler
should be supported. If the compiler doesn't use the attribute keywords, then
define these keywords as empty defines. Special pragma instructions can be placed
into the pack_on and pack_off-header, to hide these settings internally.

1)  Select at first one of the predefined Toolchains or add a new one if not existing here.
    --> #define TOOL_CHAIN_MICROSOFT

2)  include the pnio_pckx_on.h at the beginning of the structure. "x" stands for 1,2 or 4

3)  Use the defined keywords "PNIO_PACKED_ATTRIBUTE_PRE" and "PNIO_PACKED_ATTRIBUTE_POST"
    to encapsulate the structure.

4)  include the pnio_pck_off.h at the end of the structure

5)  example:
    ========

    #include "pnio_pck2_on.h"

    typedef struct _PNIP_ACW_RCV_RBASE_TYPE     EDD_COMMON_MEM_ATTR *   PNIP_ACW_RCV_RBASE_PTR_TYPE;
    PNIO_PACKED_ATTRIBUTE_PRE struct _PNIP_ACW_RCV_RBASE_TYPE
    {
        LSA_UINT16          ACW_List_Ptr;
    } PNIO_PACKED_ATTRIBUTE_POST;
    typedef struct _PNIP_ACW_RCV_RBASE_TYPE     PNIP_ACW_RCV_RBASE_TYPE;

    #include "pnio_pck_off.h"
*/


/*---------------------------------------------------------------------------*/
/*      1.  SANITY CHECK                                                     */
/*---------------------------------------------------------------------------*/

#ifdef PNIO_PCK_ON_H
    #error "why defined --> check include packon/packoff pair !!!"
#endif


/*---------------------------------------------------------------------------*/
/*      2.  DEFINE 2-BYTE-PACKED                                             */
/*---------------------------------------------------------------------------*/

#if defined (TOOL_CHAIN_MICROSOFT)

	#define PNIO_PACKED_ATTRIBUTE_PRE   /* nothing, see pragma below */
	#define PNIO_PACKED_ATTRIBUTE_POST  /* nothing, see pragma below */

	/*
	 * doing pack here gives a level 1 warning
     * --> disable this warning and keep it disabled to avoid "warning: pack changed within a header file"
	 */
    #define PRAGMA_PCK_WARN 4103
    #pragma warning( disable : PRAGMA_PCK_WARN )
    #define PACK_LBL PACK16_LBL
    #pragma pack(push, PACK_LBL, 2/*16bit*/)

#elif defined (TOOL_CHAIN_TASKING_TRICORE)

	#define PNIO_PACKED_ATTRIBUTE_PRE   /* nothing, see pragma below */
	#define PNIO_PACKED_ATTRIBUTE_POST  /* nothing, see pragma below */

	#pragma pack 2                /* tricore word-packed      */

#elif defined (TOOL_CHAIN_GNU_PPC)

	#define PNIO_PACKED_ATTRIBUTE_PRE   /* nothing, see pragma below */
	#define PNIO_PACKED_ATTRIBUTE_POST  /* nothing, see pragma below */
	#pragma pack (2)                    /* 1=byte 2=short 4=long alignment */

#elif defined (TOOL_CHAIN_CC386)

	#define PNIO_PACKED_ATTRIBUTE_PRE   /* nothing, see pragma below */
	#define PNIO_PACKED_ATTRIBUTE_POST  /* nothing, see pragma below */
	#pragma noalign

#elif defined (TOOL_CHAIN_GREENHILLS_ARM)

	#define PNIO_PACKED_ATTRIBUTE_PRE   /* nothing, see pragma below */
	#define PNIO_PACKED_ATTRIBUTE_POST  /* nothing, see pragma below */
	#pragma pack(2)               /* 1=byte 2=short 4=long alignment */

#elif defined (TOOL_CHAIN_NRK) /* ARM/Thumb C/C++ Compiler */

	#define PNIO_PACKED_ATTRIBUTE_PRE   __packed
	#define PNIO_PACKED_ATTRIBUTE_POST  /* no postfix */
	/* does not use pragma, see define above */

#elif defined (TOOL_CHAIN_GNU)

	#define PNIO_PACKED_ATTRIBUTE_PRE   /* nothing, see pragma below */
	#define PNIO_PACKED_ATTRIBUTE_POST  /* nothing, see pragma below */
	#pragma pack (2)              /* 1=byte 2=short 4=long alignment */

#elif defined (TOOL_CHAIN_GHS_MIPS)

	#define PNIO_PACKED_ATTRIBUTE_PRE   /* nothing, see pragma below */
	#define PNIO_PACKED_ATTRIBUTE_POST  /* nothing, see pragma below */
	#pragma pack (2)

#else

	#error "unknown tool-chain"

#endif


/*---------------------------------------------------------------------------*/
/*      3.  set MARKER for packing header                                    */
/*---------------------------------------------------------------------------*/

#define PNIO_PCK_ON_H


/*---------------------------------------------------------------------------*/
/*      4.  undef MARKER for unpack header                                   */
/*---------------------------------------------------------------------------*/

#undef PNIO_PCK_OFF_H


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
