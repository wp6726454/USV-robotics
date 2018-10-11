
#ifndef EDDS_INT_H                    /* ----- reinclude-protection ----- */
#define EDDS_INT_H

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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_int.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal headerfile                                                      */
/*  Defines internal constants, types, data, macros and prototyping for      */
/*  edds.                                                                    */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  12.04.02    JS    initial version. LSA P02.00.01.003                     */
/*  23.05.02    JS    ported to LSA_GLOB_P02.00.00.00_00.01.04.01            */
/*  11.11.03    JS    added EDDS_CFG_HW_POWER_PC,EDDS_CFG_HW_NETARM          */
/*  28.06.04    JS    faster implementation for flip-macros                  */
/*  17.08.04    JS    added Debug-Request                                    */
/*  21.01.05    JS    added Rx-Overload support.                             */
/*  03.02.05    JS    added EDDS_CFG_HW_TEMPLATE                             */
/*  01.06.05    JS    added EDDS_RQB_SET/GET_STATUS from edd_usr.h           */
/*  05.07.05    JS    added EDDS_FindLinkAutoMode                            */
/*  06.03.06    JS    added EDDS_SwitchRequest                                */
/*  10.03.06    JS    added EDDS_MODULE_ID_EDD_PRM and prm-externals         */
/*  08.05.06    JS    added EDDS_CFG_HW_XSCALE                               */
/*  03.12.07    JS    added EDDS_CFG_HW_WINCE                                */
/*                    Module id EDDS_MODULE_ID_EDD_USR changed from 0 to 10  */
/*  28.03.08    JS    added EDDS_CFG_HW_WPCAP                                */
/*  25.05.11    MA    removed *_CFG_HW_* specific stuff.                     */
#endif
/*****************************************************************************/

#ifdef EDDS_CFG_NRT_INCLUDE
#include "edds_nrt_inc.h"
#endif

#ifdef EDDS_CFG_SRT_INCLUDE
#include "edds_srt_inc.h"
#endif

#include "edds_dev.h"
#include "edds_dbg.h"
#include "edds_llif.h"

#ifdef EDDS_CFG_NRT_INCLUDE
#include "edds_nrt_ext.h"
#endif

#ifdef EDDS_CFG_SRT_INCLUDE
#include "edds_srt_ext.h"
#endif

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/* ========================================================================= */
/*                                                                           */
/* ModuelIDs                                                                 */
/*                                                                           */
/* Note : This is only for reference. The value is hardcoded in every C-File!*/
/*                                                                           */
/*        The main EDDS functions uses the following IDs                     */
/*                                                                           */
/*        EDDS:  0    ..  15                                                 */
/*                                                                           */
/*        Each Ethernethardwarefunctions uses the folling IDs                */
/*                                                                           */
/*        LL:    16   ..  31                                                 */
/*                                                                           */
/*        Each component uses the following module IDs (defined in component)*/
/*                                                                           */
/*        NRT:   128  ..  143                                                */
/*        SRT:   144  ..  159                                                */
/*                                                                           */
/* ========================================================================= */

/* EDDS module IDs used in C-Files */

#define EDDS_MODULE_ID_EDD_SYS             1
#define EDDS_MODULE_ID_EDD_DEV             2
#define EDDS_MODULE_ID_EDD_GLB             3
#define EDDS_MODULE_ID_EDD_DBG             4
#define EDDS_MODULE_ID_EDD_TIM             5
#define EDDS_MODULE_ID_EDD_TX              6
#define EDDS_MODULE_ID_EDD_ISR             7
#define EDDS_MODULE_ID_EDD_GEN             8
#define EDDS_MODULE_ID_EDD_PRM             9
#define EDDS_MODULE_ID_EDD_USR             10
#define EDDS_MODULE_ID_EDD_IO              11

#define EDDS_MODULE_ID_EDD_NRT_USR         128
#define EDDS_MODULE_ID_EDD_NRT_SND         129
#define EDDS_MODULE_ID_EDD_NRT_RCV         130

#define EDDS_MODULE_ID_EDD_SRT_USR         144
#define EDDS_MODULE_ID_EDD_SRT_PROV        145
#define EDDS_MODULE_ID_EDD_SRT_CONS        146

#define EDDS_MODULE_ID_EDD_LL              16

/*===========================================================================*/
/* some internal config settings.                                            */
/*===========================================================================*/

/* define to do more detailed (slower) check for valid hDDB (e.g. in RQBs)   */

#undef EDDS_CFG_DO_DDB_CHECK
#undef EDDS_CFG_DO_INTERNAL_FATAL_CHECK /* define to do some more internal   */
/* fatal error checks                */

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef struct edds_data_tag
{
    LSA_FATAL_ERROR_TYPE  edds_error;
    /*  TYPE2                 var2;
     *  TYPE3                 var3;
     *           and so on
     */
} EDDS_DATA_TYPE;


/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/

LSA_EXTERN EDDS_INFO_TYPE g_EDDSInfo;          /* global EDDS-INFO-Structure */

/*===========================================================================*/
/*                                macros                                     */
/*===========================================================================*/

/* for backwardcompatibility */

#define EDDS_RQB_SET_STATUS(_pRQB, _Value) EDD_RQB_SET_RESPONSE(_pRQB,_Value)
#define EDDS_RQB_GET_STATUS(_pRQB)        EDD_RQB_GET_RESPONSE(_pRQB)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDSGet2Potenz                              +*/
/*+  Input                      LSA_UINT32       Exponent.                  +*/
/*+  Result                :    2^Exponent                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Calculate 2^Exponent.                                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define EDDSGet2Potenz(Exponent) ((LSA_UINT32)1 << (Exponent))

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+ Conversion of Network byteorder to Hostbyteorder                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifndef EDDS_CFG_BIG_ENDIAN

#if 0
/* flips a byte pattern of 0102 to 0201 */
#define EDDS_FLIP16(C)     (LSA_UINT16)((((C)>>8)&0xFF)|(((C)<<8)&0xFF00))
/* flips a byte pattern of 01020304 to 04030201 */
#define EDDS_FLIP32(C)     ((LSA_UINT32)(((((LSA_UINT32)C)>>24)&0xFF)|((((LSA_UINT32)C)>>8)&0xFF00)|((((LSA_UINT32)C)<<8)&0xFF0000)|((((LSA_UINT32)C)<<24)&0xFF000000)))
#endif

//@fixme am2219 see lint comment - see #if 0 segment above
/* flips a byte pattern of 0102 to 0201 */
#define EDDS_FLIP16(C) ((LSA_UINT16) ( (((LSA_UINT16) (C))>>8) | ( ((LSA_UINT16)(C)) << 8)) )
/* flips a byte pattern of 01020304 to 04030201 */
#define EDDS_FLIP32(C) ((LSA_UINT32) (((C) >> 24) | (((C) & 0x00ff0000) >> 8) | (((C) & 0x0000ff00) <<  8) | (((C) << 24))) )

#else
#define EDDS_FLIP16(C)     (C)
#define EDDS_FLIP32(C)     (C)
#endif

#ifndef EDDS_NTOHS
#define EDDS_NTOHS(Value) ( EDDS_FLIP16(Value) )/* Network TO Host Short */
#endif

#ifndef EDDS_NTOHL
#define EDDS_NTOHL(Value) ( EDDS_FLIP32(Value) )/* Network TO Host Long */
#endif

#ifndef EDDS_HTONS
#define EDDS_HTONS(Value) ( EDDS_FLIP16(Value) )/* Host To Network Short */
#endif

#ifndef EDDS_HTONL
#define EDDS_HTONL(Value) ( EDDS_FLIP32(Value) )/* Host TO Network Long */
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+ Conversion of IOBuffer byteorder from/to Hostbyteorder                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifdef EDDS_CFG_BIG_ENDIAN
	#define EDDS_SWAP16(C) ((LSA_UINT16) ( (((LSA_UINT16) (C))>>8) | ( ((LSA_UINT16)(C)) << 8)) )
	#define EDDS_SWAP32(C) ((LSA_UINT32) (((C) >> 24) | (((C) & 0x00ff0000) >> 8) | (((C) & 0x0000ff00) <<  8) | (((C) << 24))) )
#else
	#define EDDS_SWAP16(C) (C)
	#define EDDS_SWAP32(C) (C)
#endif

#ifndef LE_TO_H_S
#define LE_TO_H_S(Value) ( EDDS_SWAP16(Value) )  /* Little Endian to Host Short */
#endif

#ifndef LE_TO_H_L
#define LE_TO_H_L(Value) ( EDDS_SWAP32(Value) )  /* Little Endian to Host Long  */
#endif

#ifndef H_TO_LE_S
#define H_TO_LE_S(Value) ( EDDS_SWAP16(Value) )  /* Host to Little Endian Short */
#endif

#ifndef H_TO_LE_L
#define H_TO_LE_L(Value) ( EDDS_SWAP32(Value) )  /* Host to Little Endian Long  */
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+ access to Network byteordered variables                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* pValue has to be a UINT8 - Ptr! */
#define EDDS_GET_U16(pValue) ((LSA_UINT16) (((LSA_UINT16)(*(pValue)) << 8 ) + *((pValue)+1)))

/* pValue has to be a UINT8 - Ptr! */
#define EDDS_GET_U32(pValue) ((LSA_UINT32) (((LSA_UINT32)(*(pValue))     << 24) + \
((LSA_UINT32)(*((pValue)+1)) << 16) + \
((LSA_UINT32)(*((pValue)+2)) << 8)  + \
*((pValue)+3)))

/* pValue has to be a UINT8 - Ptr! */
#define EDDS_GET_U16_INCR(pValue,Value) \
{                                      \
Value = EDDS_GET_U16(pValue);      \
pValue += sizeof(LSA_UINT16);      \
}

/* pValue has to be a UINT8 - Ptr! */
#define EDDS_GET_U32_INCR(pValue,Value) \
{                                      \
Value = EDDS_GET_U32(pValue);      \
pValue += sizeof(LSA_UINT32);      \
}

/* pValue has to be a UINT8 - Ptr! */
#define EDDS_SET_U8_INCR(pValue,Value) \
{                                       \
*(pValue)++ = (LSA_UINT8) ((Value));         \
}

/* pValue has to be a UINT8 - Ptr! */
#define EDDS_SET_U16_INCR(pValue,Value) \
{                                       \
*(pValue)++ = (LSA_UINT8) ((Value) >> 8 );   \
*(pValue)++ = (LSA_UINT8) ((Value));         \
}

/* pValue has to be a UINT8 - Ptr! */
#define EDDS_SET_U32_INCR(pValue,Value) \
{                                       \
*(pValue)++ = (LSA_UINT8) ((Value) >> 24 );  \
*(pValue)++ = (LSA_UINT8) ((Value) >> 16 );  \
*(pValue)++ = (LSA_UINT8) ((Value) >> 8  );  \
*(pValue)++ = (LSA_UINT8) ((Value));         \
}

/*===========================================================================*/
/*                              other macros                                 */
/*===========================================================================*/

#define EDDS_MACADDR_IS_0(_s1) \
((_s1.MacAdr[0] == 0) && (_s1.MacAdr[1] == 0) &&    \
(_s1.MacAdr[2] == 0) && (_s1.MacAdr[3] == 0) &&    \
(_s1.MacAdr[4] == 0) && (_s1.MacAdr[5] == 0))

/*===========================================================================*/
/*                           chain macros for rqb                            */
/*===========================================================================*/

/*=============================================================================
 * function name:  EDDS_RQB_PUT_BLOCK_TOP
 *
 * function:       insert block in queue on top position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define EDDS_RQB_PUT_BLOCK_TOP(_Bottom, _Top, _Block)           \
{                                                               \
\
EDD_RQB_SET_NEXT_RQB_PTR(_Block, LSA_NULL);                 \
EDD_RQB_SET_PREV_RQB_PTR(_Block, _Top);                     \
\
if(LSA_HOST_PTR_ARE_EQUAL((_Top),LSA_NULL))                 \
{                                                           \
_Top    = _Block;                                       \
_Bottom = _Block;                                       \
}                                                           \
else                                                        \
{                                                           \
EDD_RQB_SET_NEXT_RQB_PTR(_Top, _Block);                 \
_Top        = _Block;                                   \
}                                                           \
}                                                               \


/*=============================================================================
 * function name:  EDDS_RQB_PUT_BLOCK_BOTTOM
 *
 * function:       insert block in queue on bottom position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define EDDS_RQB_PUT_BLOCK_BOTTOM(_Bottom, _Top, _Block)        \
{                                                               \
EDD_RQB_SET_NEXT_RQB_PTR(_Block, _Bottom);                  \
EDD_RQB_SET_PREV_RQB_PTR(_Block, LSA_NULL);                 \
\
if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )           \
{                                                           \
_Bottom = _Block;                                       \
_Top    = _Block;                                       \
}                                                           \
else                                                        \
{                                                           \
EDD_RQB_SET_PREV_RQB_PTR(_Bottom, _Block);              \
_Bottom         = _Block;                               \
}                                                           \
}                                                               \


/*=============================================================================
 * function name:   EDDS_RQB_REM_BLOCK_TOP
 *
 * function:        remove block from queue on top position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_RQB_REM_BLOCK_TOP(_Bottom, _Top, _Block)           \
{                                                               \
_Block = _Top;                                              \
if( !LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )             \
{                                                           \
_Top = EDD_RQB_GET_PREV_RQB_PTR(_Top);                  \
\
if( LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )          \
_Bottom = LSA_NULL;                                 \
else                                                    \
EDD_RQB_SET_NEXT_RQB_PTR(_Top, LSA_NULL);           \
}                                                           \
}                                                               \

/*=============================================================================
 * function name:   EDDS_RQB_REM_BLOCK_BOTTOM
 *
 * function:        remove block from queue on bottom position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_RQB_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block)        \
{                                                               \
_Block = _Bottom;                                           \
\
if( !LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )          \
{                                                           \
_Bottom = EDD_RQB_GET_NEXT_RQB_PTR((_Bottom));          \
\
if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )       \
_Top = LSA_NULL;                                    \
else                                                    \
EDD_RQB_SET_PREV_RQB_PTR((_Bottom), LSA_NULL);      \
}                                                           \
}                                                               \


/*=============================================================================
 * function name:   EDDS_RQB_REM_BLOCK
 *
 * function:        remove block by Handle from queue
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_RQB_REM_BLOCK(_Bottom, _Top, _Handle, _Block)      \
{                                                               \
if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||      \
(LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||      \
(LSA_HOST_PTR_ARE_EQUAL((_Handle), LSA_NULL)) )         \
_Block = LSA_NULL;                                      \
else                                                        \
{                                                           \
_Block = _Bottom;                                       \
\
while((! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))  && \
(! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Top   ) )))    \
_Block = EDD_RQB_GET_NEXT_RQB_PTR(_Block);          \
\
if( ! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))        \
_Block = LSA_NULL;                                  \
else                                                    \
{                                                       \
if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Bottom) )     \
{                                                   \
EDDS_RQB_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block);   \
}                                                   \
else                                                \
{                                                   \
if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Top ) )   \
{                                               \
EDDS_RQB_REM_BLOCK_TOP(_Bottom, _Top, _Block);  \
}                                               \
else                                            \
{                                               \
EDD_RQB_SET_NEXT_RQB_PTR(EDD_RQB_GET_PREV_RQB_PTR(_Block),EDD_RQB_GET_NEXT_RQB_PTR(_Block)); \
EDD_RQB_SET_PREV_RQB_PTR(EDD_RQB_GET_NEXT_RQB_PTR(_Block),EDD_RQB_GET_PREV_RQB_PTR(_Block)); \
}                                               \
}                                                   \
}                                                       \
}                                                           \
}                                                               \


/*=============================================================================
 * function name:   EDDS_RQB_REM_VALID_BLOCK
 *
 * function:        remove valid block from queue
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_RQB_REM_VALID_BLOCK(_Bottom, _Top, _Block)         \
{                                                               \
if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Bottom) )     \
{                                                   \
EDDS_RQB_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block);   \
}                                                   \
else                                                \
{                                                   \
if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Top ) )   \
{                                               \
EDDS_RQB_REM_BLOCK_TOP(_Bottom, _Top, _Block);  \
}                                               \
else                                            \
{                                               \
EDD_RQB_SET_NEXT_RQB_PTR(EDD_RQB_GET_PREV_RQB_PTR(_Block),EDD_RQB_GET_NEXT_RQB_PTR(_Block)); \
EDD_RQB_SET_PREV_RQB_PTR(EDD_RQB_GET_NEXT_RQB_PTR(_Block),EDD_RQB_GET_PREV_RQB_PTR(_Block)); \
}                                               \
}                                                   \
}                                                               \

/*=============================================================================
 * function name:   EDDS_RQB_GET_BLOCK_HANDLE
 *
 * function:        get pointer of ab block by Handle
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_RQB_GET_BLOCK_HANDLE(_Bottom, _Top, _Handle, _Block)   \
{                                                                   \
if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||          \
(LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||          \
((_Handle) == 0) )                                          \
_Block = LSA_NULL;                                          \
else                                                            \
{                                                               \
_Block = _Bottom;                                           \
\
while( (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle))) &&    \
(!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Top))) )        \
_Block = EDD_RQB_GET_NEXT_RQB_PTR(_Block);              \
\
if( !LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle)) )          \
_Block = LSA_NULL;                                      \
}                                                               \
}                                                                   \

/*=============================================================================
 * function name:   EDDS_RQB_SEARCH_QUEUE_HANDLE
 *
 * function:        Search RQB-Queue for RQB with Handle.
 *
 * parameters:      EDD_UPPER_RQB_PTR_TYPE         pRQB
 *                  LSA_HANDLE_TYPE                Handle
 * return value:    LSA_VOID
 *
 * pRQB             points to start of RQB-queue (maybe NULL)
 * Handle           Handle to search in RQBs
 *
 * Description:     On return pRQB points to NULL if no entry was found, else
 *                  to the RQB with the spezified Handle. On entry pRQB may
 *                  already be NULL.
 *===========================================================================*/

#define EDDS_RQB_SEARCH_QUEUE_HANDLE(pRQB,Handle)                       \
{                                                                   \
while   ((! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL)) &&          \
( EDD_RQB_GET_HANDLE(pRQB) != Handle ))                 \
{                                                               \
pRQB = EDD_RQB_GET_NEXT_RQB_PTR(pRQB);                      \
}                                                               \
}                                                                   \


/*===========================================================================*/
/*                                chain-macros                               */
/*===========================================================================*/

/*=============================================================================
 * function name:  EDDS_PUT_BLOCK_TOP
 *
 * function:       insert block in queue on top position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define EDDS_PUT_BLOCK_TOP(_Bottom, _Top, _Block)               \
{                                                               \
\
_Block->pNext = LSA_NULL;                                   \
_Block->pPrev = _Top;                                       \
\
if(LSA_HOST_PTR_ARE_EQUAL((_Top),LSA_NULL))                 \
{                                                           \
_Top    = _Block;                                       \
_Bottom = _Block;                                       \
}                                                           \
else                                                        \
{                                                           \
_Top->pNext = _Block;                                   \
_Top        = _Block;                                   \
}                                                           \
}                                                               \


/*=============================================================================
 * function name:  EDDS_PUT_BLOCK_BOTTOM
 *
 * function:       insert block in queue on bottom position
 *
 * parameters:     _Botton, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/

#define EDDS_PUT_BLOCK_BOTTOM(_Bottom, _Top, _Block)            \
{                                                               \
_Block->pNext = _Bottom;                                    \
_Block->pPrev = LSA_NULL;                                   \
\
if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )           \
{                                                           \
_Bottom = _Block;                                       \
_Top    = _Block;                                       \
}                                                           \
else                                                        \
{                                                           \
_Bottom->pPrev  = _Block;                               \
_Bottom         = _Block;                               \
}                                                           \
}                                                               \


/*=============================================================================
 * function name:   EDDS_REM_BLOCK_TOP
 *
 * function:        remove block from queue on bottom position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_REM_BLOCK_TOP(_Bottom, _Top, _Block)               \
{                                                               \
_Block = _Top;                                              \
\
if( !LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )             \
{                                                           \
_Top = _Top->pPrev;                                     \
\
if( LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL) )          \
_Bottom = LSA_NULL;                                 \
else                                                    \
_Top->pNext = LSA_NULL;                             \
}                                                           \
}                                                               \

/*=============================================================================
 * function name:   EDDS_REM_BLOCK_BOTTOM
 *
 * function:        remove block from queue on top position
 *
 * parameters:      _Botton, _Top, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block)            \
{                                                               \
_Block = _Bottom;                                           \
\
if( !LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )          \
{                                                           \
_Bottom = _Bottom->pNext;                               \
\
if( LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL) )       \
_Top = LSA_NULL;                                    \
else                                                    \
_Bottom->pPrev = LSA_NULL;                          \
}                                                           \
}                                                               \


/*=============================================================================
 * function name:   EDDS_REM_BLOCK
 *
 * function:        remove block by Handle from queue
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_REM_BLOCK(_Bottom, _Top, _Handle, _Block)          \
{                                                               \
if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||      \
(LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||      \
(LSA_HOST_PTR_ARE_EQUAL((_Handle), LSA_NULL)) )         \
_Block = LSA_NULL;                                      \
else                                                        \
{                                                           \
_Block = _Bottom;                                       \
\
while((! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))  && \
(! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Top   ) )))    \
_Block = _Block->pNext;                             \
\
if( ! LSA_HOST_PTR_ARE_EQUAL(_Block,(_Handle) ))        \
_Block = LSA_NULL;                                  \
else                                                    \
{                                                       \
if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Bottom) )     \
{                                                   \
EDDS_REM_BLOCK_BOTTOM(_Bottom, _Top, _Block);   \
}                                                   \
else                                                \
{                                                   \
if( LSA_HOST_PTR_ARE_EQUAL((_Block), _Top ) )   \
{                                               \
EDDS_REM_BLOCK_TOP(_Bottom, _Top, _Block);  \
}                                               \
else                                            \
{                                               \
_Block->pPrev->pNext = _Block->pNext;       \
_Block->pNext->pPrev = _Block->pPrev;       \
}                                               \
}                                                   \
}                                                       \
}                                                           \
}                                                               \


/*=============================================================================
 * function name:   EDDS_GET_BLOCK_HANDLE
 *
 * function:        get pointer of ab block by Handle
 *
 * parameters:      _Botton, _Top, _Handle, _Block
 *
 * return value:    _Block
 *
 *===========================================================================*/

#define EDDS_GET_BLOCK_HANDLE(_Bottom, _Top, _Handle, _Block)       \
{                                                                   \
if( (LSA_HOST_PTR_ARE_EQUAL((_Bottom), LSA_NULL))   ||          \
(LSA_HOST_PTR_ARE_EQUAL((_Top), LSA_NULL))      ||          \
((_Handle) == 0) )                                          \
_Block = LSA_NULL;                                          \
else                                                            \
{                                                               \
_Block = _Bottom;                                           \
\
while( (!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle))) &&    \
(!LSA_HOST_PTR_ARE_EQUAL((_Block), (_Top))) )        \
_Block = (_Block)->pNext;                               \
\
if( !LSA_HOST_PTR_ARE_EQUAL((_Block), (_Handle)) )          \
_Block = LSA_NULL;                                      \
}                                                               \
}                                                                   \

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/* in edds_dev.c */

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_HandleNew( EDDS_LOCAL_HDB_PTR_TYPE EDDS_LOCAL_MEM_ATTR *ppHDB );

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_HandleRel( LSA_HANDLE_TYPE Handle );

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_CreateDDB( EDDS_LOCAL_DDB_PTR_TYPE EDDS_LOCAL_MEM_ATTR * const ppDDB,
                                                            EDDS_UPPER_DPB_PTR_TYPE const                 pDPB);

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_CloseDDB( EDDS_HANDLE hDDB);

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_GetDDB( EDDS_HANDLE const hDDB,
                                                         EDDS_LOCAL_DDB_PTR_TYPE EDDS_LOCAL_MEM_ATTR * const ppDDB );

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_HandleGetHDB( LSA_HANDLE_TYPE const Handle,
                                                               EDDS_LOCAL_HDB_PTR_TYPE EDDS_LOCAL_MEM_ATTR * const ppHDB );

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_HandleIni(LSA_VOID);

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_HandleClose(LSA_VOID);

LSA_EXTERN  LSA_VOID    EDDS_LOCAL_FCT_ATTR EDDS_DevInfoIni(LSA_VOID);

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_DevInfoClose(LSA_VOID);


LSA_EXTERN  LSA_BOOL EDDS_LOCAL_FCT_ATTR EDDS_HandleSearchDDB(EDDS_LOCAL_DDB_PTR_TYPE pDDB);

/* in edds_tx.c */

LSA_EXTERN  LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_TransmitSts(
    EDDS_LOCAL_DDB_PTR_TYPE         pDDB,
    LSA_BOOL                        StopTimeout);

LSA_EXTERN LSA_VOID   EDDS_LOCAL_FCT_ATTR EDDS_TransmitTrigger(EDDS_LOCAL_DDB_PTR_TYPE pDDB);

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
LSA_EXTERN LSA_VOID   EDDS_LOCAL_FCT_ATTR EDDS_TransmitTimeout(EDDS_LOCAL_DDB_PTR_TYPE pDDB);
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

LSA_EXTERN LSA_VOID   EDDS_LOCAL_FCT_ATTR EDDS_TransmitShutdown(EDDS_LOCAL_DDB_PTR_TYPE pDDB);

/* in edds_tim.c */

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_TimerInfoIni(LSA_VOID);

LSA_EXTERN  LSA_RESULT  EDDS_LOCAL_FCT_ATTR EDDS_TimerInfoFree(LSA_VOID);

LSA_EXTERN  LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_TimerSet(
    LSA_TIMER_ID_TYPE      TimerID,
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB,
    LSA_VOID                        LSA_FCT_PTR(EDDS_LOCAL_FCT_ATTR, Cbf)
    (EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB),
    volatile EDDS_LOCAL_MEM_U16_PTR_TYPE EDDSTimerID);

LSA_EXTERN  EDDS_LOCAL_TIMER_INFO_PTR_TYPE EDDS_LOCAL_FCT_ATTR EDDS_TimerSearch(LSA_UINT16 TimerID);

LSA_EXTERN  LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_TimerFree(LSA_UINT16 EDDSTimerID);

LSA_EXTERN LSA_VOID   EDDS_LOCAL_FCT_ATTR EDDS_LLTimeoutCallback(EDDS_LOCAL_DDB_PTR_TYPE pDDB);


/* in edds_glb.c */


LSA_EXTERN  LSA_VOID    EDDS_LOCAL_FCT_ATTR EDDS_RequestFinish( EDDS_LOCAL_HDB_PTR_TYPE     pHDB,
                                                                EDD_UPPER_RQB_PTR_TYPE      pRQB,
                                                                LSA_RESULT                  Status);

LSA_EXTERN  LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CallCbf(
    LSA_VOID LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)(EDD_UPPER_RQB_PTR_TYPE pRQB),
    EDD_UPPER_RQB_PTR_TYPE      pRQB);

LSA_EXTERN  LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_CallChannelCbf(
    LSA_VOID LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)(EDD_UPPER_RQB_PTR_TYPE pRQB),
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    LSA_SYS_PTR_TYPE            pSys);

LSA_EXTERN  EDD_UPPER_RQB_PTR_TYPE      EDDS_LOCAL_FCT_ATTR EDDS_AllocInternalRQB(LSA_VOID);

LSA_EXTERN  LSA_VOID                    EDDS_LOCAL_FCT_ATTR EDDS_FreeInternalRQB(EDD_UPPER_RQB_PTR_TYPE pRQB);

LSA_EXTERN  LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_SetupInternalRQBParams(
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pInternalRQB,
    EDD_SERVICE                     Service,
    LSA_HANDLE_TYPE                 Handle,
    EDDS_HANDLE                     hDDB,
    LSA_UINT32                      ID,
    LSA_UINT32                      Param);


LSA_EXTERN  LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_InsertSrcMac(
    EDD_MAC_ADR_TYPE  EDDS_LOCAL_MEM_ATTR *pMAC,
    EDD_UPPER_MEM_PTR_TYPE              pBuffer);

LSA_EXTERN EDDS_FAR_FCT LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_FatalError( EDDS_FATAL_ERROR_TYPE Error,
                                                                      LSA_UINT16             ModuleID,
                                                                      LSA_UINT32             Line);


LSA_EXTERN LSA_BOOL  EDDS_LOCAL_FCT_ATTR EDDS_FindLinkAutoMode(
    EDDS_LOCAL_DDB_PTR_TYPE  pDDB);

/* in edd_usr.c */

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
LSA_EXTERN  LSA_VOID    EDDS_LOCAL_FCT_ATTR EDDS_InterruptRequest(EDD_UPPER_RQB_PTR_TYPE pRQB);
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

LSA_EXTERN  LSA_VOID    EDDS_LOCAL_FCT_ATTR EDDS_TimeoutRequest(EDD_UPPER_RQB_PTR_TYPE pRQB);

/* in edds_gen.c */

LSA_EXTERN  LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_GeneralRequest(EDD_UPPER_RQB_PTR_TYPE      pRQB,
                                                                EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN  LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_SwitchRequest(EDD_UPPER_RQB_PTR_TYPE       pRQB,
                                                              EDDS_LOCAL_HDB_PTR_TYPE       pHDB);

LSA_EXTERN  LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPHYInterrupt(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB);

LSA_EXTERN  LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_DebugRequest(EDD_UPPER_RQB_PTR_TYPE      pRQB,
                                                              EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RxOverloadTimerCbf(
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RxOverloadCycleRequest(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_RxOverloadIndicateEvent(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_LLTimerCbf(
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_LEDBlinkTimerCbf(
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_LEDBlinkCycleRequest(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DiagTimerCbf(
    EDDS_INTERNAL_REQUEST_PTR_TYPE  pIntRQB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DiagCycleRequest(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestSetMRPState(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_IndicateExtLinkChange(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_UINT32                  PortID);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestGetStatistics(
	EDD_UPPER_GET_STATISTICS_PTR_TYPE const pStats,
    EDDS_LOCAL_HDB_PTR_TYPE const    pHDB);

LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestGetStatisticsAll(
    EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE const pStatsAll,
    EDDS_LOCAL_HDB_PTR_TYPE const     pHDB);

/* in edds_dbg.c */

LSA_EXTERN LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_ReentranceCheckInit(LSA_VOID);

LSA_EXTERN LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_EnterCheckRequest(
    EDD_UPPER_RQB_PTR_TYPE pRQB,LSA_UINT32 *pReentrPrio);

LSA_EXTERN  LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_ExitCheckRequest(
    LSA_UINT32 ReentrPrio);

LSA_EXTERN LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_EnterCheckISR(LSA_VOID);
LSA_EXTERN LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_ExitCheckISR(LSA_VOID);
LSA_EXTERN LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_EnterCheckTimer(LSA_VOID);
LSA_EXTERN LSA_VOID  EDDS_LOCAL_FCT_ATTR EDDS_ExitCheckTimer(LSA_VOID);

/* in edds_prm.c */

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmRequest(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_PrmAlloc(
    EDDS_PRM_PTR_TYPE     pPrm);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmFree(
    EDDS_PRM_PTR_TYPE     pPrm);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_PrmInit(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmIndication(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmWrite(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmRead(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmCommit(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmEnd(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_RequestPrmPrepare(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_DiagFrameDropHandler(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_UINT32                  DropCnt);

#if defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
/* Frame Handling of RX frames in ISR routine or scheduler,
 * depending of selected mode :
 * - EDDS_CFG_ISR_MODE_STANDARD
 * - EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD
 * - EDDS_CFG_CYCLIC_SCHEDULING_MODE
 */
LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_HandleRxFrame(
    EDDS_LOCAL_DDB_PTR_TYPE const                pDDB,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR  *ppRxBuffer,
    LSA_UINT32                                   RxLen,
    LSA_UINT32                                   PortID);
#endif /* defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

/*===========================================================================*/
/*                              compiler errors                              */
/*===========================================================================*/

#if !defined EDD_UPPER_IN_FCT_ATTR
#error "EDD_UPPER_IN_FCT_ATTR not defined in file edd_cfg.h"
#endif

#if !defined EDD_UPPER_OUT_FCT_ATTR
#error "EDD_UPPER_OUT_FCT_ATTR not defined in file edd_cfg.h"
#endif

#if !defined EDDS_SYSTEM_IN_FCT_ATTR
#error "EDDS_SYSTEM_IN_FCT_ATTR not defined in file edd_cfg.h"
#endif

#if !defined EDDS_SYSTEM_OUT_FCT_ATTR
#error "EDDS_SYSTEM_OUT_FCT_ATTR not defined in file edd_cfg.h"
#endif

#if !defined EDDS_LOCAL_FCT_ATTR
#error "EDDS_LOCAL_FCT_ATTR not defined in file edd_cfg.h"
#endif

#if !defined EDD_UPPER_RQB_ATTR
#error "EDD_UPPER_RQB_ATTR not defined in file edd_cfg.h"
#endif

#if !defined EDD_UPPER_MEM_ATTR
#error "EDD_UPPER_MEM_ATTR not defined in file edd_cfg.h"
#endif

#if !defined EDDS_SYSTEM_MEM_ATTR
#error "EDDS_SYSTEM_MEM_ATTR not defined in file edd_cfg.h"
#endif

#if !defined EDDS_LOCAL_MEM_ATTR
#error "EDDS_LOCAL_MEM_ATTR not defined in file edd_cfg.h"
#endif

#if !defined LSA_SYS_PATH_TYPE
#error "LSA_SYS_PATH_TYPE not defined in file lsa_cfg.h"
#endif

#if !defined LSA_SYS_PTR_TYPE
#error "LSA_SYS_PTR_TYPE not defined in file lsa_cfg.h"
#endif



/*****************************************************************************/
/*  end of file EDDS_INT.H                                                */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_INT_H */
