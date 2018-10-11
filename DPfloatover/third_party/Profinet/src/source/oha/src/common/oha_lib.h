
#ifndef OHA_LIB_H                        /* ----- reinclude-protection ----- */
#define OHA_LIB_H

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
/*  C o m p o n e n t     &C: OHA (Option HAndler)                      :C&  */
/*                                                                           */
/*  F i l e               &F: oha_lib.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Library of OHA                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/* Low- and High Byte, Word, Swapping */
#ifndef OHA_HIBYTE
#define OHA_HIBYTE(u16_)    ((LSA_UINT8)(((LSA_UINT16)(u16_)) >> 8))
#endif
#ifndef OHA_LOBYTE
#define OHA_LOBYTE(u16_)    ((LSA_UINT8)(u16_))
#endif
#ifndef OHA_HIWORD
#define OHA_HIWORD(u32_)    ((LSA_UINT16)(((LSA_UINT32)(u32_)) >> 16))
#endif
#ifndef OHA_LOWORD
#define OHA_LOWORD(u32_)    ((LSA_UINT16)(u32_))
#endif


/* Macros for combining the high- and the lowbytes/words
 * to an unsigned 16/32-bit value */
#ifndef OHA_WORD
#define OHA_WORD(l8_,h8_)       \
((LSA_UINT16)((((LSA_UINT16)(l8_)) | (((LSA_UINT16)(h8_)) << 8))))
#endif

#ifndef OHA_DWORD
#define OHA_DWORD(l16_,h16_)    \
((LSA_UINT32)((((LSA_UINT32)(l16_)) | (((LSA_UINT32)(h16_)) << 16))))
#endif


#ifndef OHA_BIG_ENDIAN

/* Macros for changing the high- and the low-bytes of a unsigned 16-bit-word */
#ifndef OHA_SWAP16
#define OHA_SWAP16(u16_)    \
((LSA_UINT16)(((((LSA_UINT16)(u16_)) >> 8) | (((LSA_UINT16)(u16_)) << 8))))
#endif
#ifndef OHA_SWAPW
#define OHA_SWAPW(src16ptr_, dst16ptr_) \
{*((LSA_UINT8 *)dst16ptr_+0) = *((LSA_UINT8 *)src16ptr_+1); \
*((LSA_UINT8 *)dst16ptr_+1) = *((LSA_UINT8 *)src16ptr_+0);}
#endif

/* Macros for changing the high- and the low-words (and their high- and low-bytes)
 * of a unsigned 32-bit value: 0x44332211 -> 0x11223344 */

#ifndef OHA_SWAP32
#define OHA_SWAP32(u32_)    \
((LSA_UINT32)(OHA_SWAP16(OHA_HIWORD(u32_)) | (OHA_SWAP16(OHA_LOWORD(u32_)) << 16)))
#endif
#ifndef OHA_SWAPD
#define OHA_SWAPD(src32ptr_, dst32ptr_) \
{*((LSA_UINT8 *)dst32ptr_+0) = *((LSA_UINT8 *)src32ptr_+3); \
*((LSA_UINT8 *)dst32ptr_+1) = *((LSA_UINT8 *)src32ptr_+2); \
*((LSA_UINT8 *)dst32ptr_+2) = *((LSA_UINT8 *)src32ptr_+1); \
*((LSA_UINT8 *)dst32ptr_+3) = *((LSA_UINT8 *)src32ptr_+0);}
#endif

#else   /* OHA_BIG_ENDIAN defined: */

/* No swapping nessesary for OHA send/receive */
#ifndef OHA_SWAP16
#define OHA_SWAP16(u16_)    (u16_)
#endif
#ifndef OHA_SWAP32
#define OHA_SWAP32(u32_)    (u32_)
#endif
#ifndef OHA_SWAPW
#define OHA_SWAPW(src16ptr_, dst16ptr_) \
{*((LSA_UINT8 *)dst16ptr_+0) = *((LSA_UINT8 *)src16ptr_+0); \
*((LSA_UINT8 *)dst16ptr_+1) = *((LSA_UINT8 *)src16ptr_+1);}
#endif
#ifndef OHA_SWAPD
#define OHA_SWAPD(src32ptr_, dst32ptr_) \
{*((LSA_UINT8 *)dst32ptr_+0) = *((LSA_UINT8 *)src32ptr_+0); \
*((LSA_UINT8 *)dst32ptr_+1) = *((LSA_UINT8 *)src32ptr_+1); \
*((LSA_UINT8 *)dst32ptr_+2) = *((LSA_UINT8 *)src32ptr_+2); \
*((LSA_UINT8 *)dst32ptr_+3) = *((LSA_UINT8 *)src32ptr_+3);}
#endif

#endif  /* OHA_BIG_ENDIAN defined */

/*===========================================================================*/
/*                           Access to snmp objects                          */
/*===========================================================================*/

#define OHA_OIDCPY(dest,src,len) OHA_MEMCPY((char*)dest,(char*)src,((LSA_UINT32)(len)*sizeof(OHA_OID)))
#define OHA_OIDCMP(buf1,buf2,len) OHA_MEMCMP((char*)buf1,(char*)buf2,((LSA_UINT32)(len)*sizeof(OHA_OID)))


/*----------------------------------------------------------------------------
// circularly linked list macros
//---------------------------------------------------------------------------*/

struct oha_list_entry_tag
{
    struct oha_list_entry_tag OHA_COMMON_MEM_ATTR * Flink; /* forward link */
    struct oha_list_entry_tag OHA_COMMON_MEM_ATTR * Blink; /* backward link */
};

typedef struct oha_list_entry_tag  OHA_LIST_ENTRY_TYPE;

typedef struct oha_list_entry_tag  OHA_COMMON_MEM_ATTR *OHA_LIST_ENTRY_PTR_TYPE;

/*----------------------------------------------------------------------------*/

#define OHA_LIST_TYPED_ENTRY_TYPE(tag)  OHA_LIST_ENTRY_TYPE

/*----------------------------------------------------------------------------*/

#define OhaListInitialize(ListHead_) {\
(ListHead_)->Flink = (ListHead_)->Blink = (ListHead_);\
}

#define OhaListIsEmpty(ListHead_) (\
LSA_HOST_PTR_ARE_EQUAL((ListHead_)->Flink, (ListHead_))\
)

#define OhaListRemoveEntry(Entry_) {\
OHA_LIST_ENTRY_PTR_TYPE _EX_Entry;\
_EX_Entry = (Entry_);\
_EX_Entry->Blink->Flink = _EX_Entry->Flink;\
_EX_Entry->Flink->Blink = _EX_Entry->Blink;\
}

#define OhaListRemoveHead(ListHead_, Entry_, Type_) {\
(Entry_) = (Type_)((ListHead_)->Flink);\
OhaListRemoveEntry((ListHead_)->Flink);\
}

#define OhaListInsertHead(ListHead_, Entry_) {\
(Entry_)->Flink = (ListHead_)->Flink;\
(Entry_)->Blink = (ListHead_);\
(ListHead_)->Flink->Blink = (Entry_);\
(ListHead_)->Flink = (Entry_);\
}

#define OhaListInsertTail(ListHead_, Entry_) {\
(Entry_)->Flink = (ListHead_);\
(Entry_)->Blink = (ListHead_)->Blink;\
(ListHead_)->Blink->Flink = (Entry_);\
(ListHead_)->Blink = (Entry_);\
}

#define OhaListInsertBefore(Existing_, Entry_) {\
OhaListInsertTail (Existing_, Entry_);\
}

#define OhaListFirst(ListHead_, Type_) (\
(Type_)(OhaListIsEmpty(ListHead_) ? LSA_NULL : (ListHead_)->Flink)\
)

#define OhaListNext(ListHead_, Entry_, Type_) (\
(Type_)(LSA_HOST_PTR_ARE_EQUAL((Entry_)->Flink, (ListHead_)) ? LSA_NULL : (Entry_)->Flink)\
)

#define OhaListLength(ListHead_, LenPtr_) {\
OHA_LIST_ENTRY_PTR_TYPE el = OhaListFirst (ListHead_, OHA_LIST_ENTRY_PTR_TYPE);\
*(LenPtr_) = 0;\
while (! LSA_HOST_PTR_ARE_EQUAL(el,LSA_NULL)) {\
*(LenPtr_) += 1;\
el = OhaListNext (ListHead_, el, OHA_LIST_ENTRY_PTR_TYPE);\
}}

#define OhaListAppend(ListHead_, ListHeadToAppend_) {\
if (! OhaListIsEmpty (ListHeadToAppend_)) {\
(ListHead_)->Blink->Flink = (ListHeadToAppend_)->Flink;\
(ListHeadToAppend_)->Flink->Blink = (ListHead_)->Blink;\
(ListHead_)->Blink = (ListHeadToAppend_)->Blink;\
(ListHead_)->Blink->Flink = (ListHead_);\
OhaListInitialize (ListHeadToAppend_);\
}}

#define OhaListIsNotInitialized(ListHead_) \
( LSA_HOST_PTR_ARE_EQUAL((ListHead_)->Flink, LSA_NULL) && \
LSA_HOST_PTR_ARE_EQUAL((ListHead_)->Blink, LSA_NULL) )

/*----------------------------------------------------------------------------*/

/* usage: for (OhaListForeach(...)) { ... } -- TAP's variant */
#define OhaListForeach(Var_, ListHead_, Type_) \
(Var_) = OhaListFirst (ListHead_, Type_); \
! LSA_HOST_PTR_ARE_EQUAL((Var_), LSA_NULL); \
(Var_) = OhaListNext (ListHead_, (OHA_LIST_ENTRY_PTR_TYPE)(Var_), Type_)

/*----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
//  read data from LLDP / write data to LLDP
//----------------------------------------------------------------------------*/

/* pValue has to be a UINT8 - Ptr! */
#define OHA_LLDP_GET_U16(pValue)  ((LSA_UINT16) (((LSA_UINT16)(*(pValue)) << 8 ) + *((pValue)+1)))

/* pValue has to be a UINT8 - Ptr! */
#define OHA_LLDP_GET_U32(pValue)   ((LSA_UINT32) (((LSA_UINT32)(*(pValue))     << 24) + \
((LSA_UINT32)(*((pValue)+1)) << 16) + \
((LSA_UINT32)(*((pValue)+2)) << 8)  + \
*((pValue)+3)))

/* pValue has to be a UINT8 - Ptr! */
#define OHA_LLDP_SET_U16(pValue,Value) \
{                                       \
*(pValue)++ = (LSA_UINT8) ((Value) >> 8 );   \
*(pValue)++ = (LSA_UINT8) ((Value));         \
}

/* pValue has to be a UINT8 - Ptr! */
#define OHA_LLDP_SET_U32(pValue,Value) \
{                                       \
*(pValue)++ = (LSA_UINT8) ((Value) >> 24 );  \
*(pValue)++ = (LSA_UINT8) ((Value) >> 16 );  \
*(pValue)++ = (LSA_UINT8) ((Value) >> 8  );  \
*(pValue)++ = (LSA_UINT8) ((Value));         \
}

/* pTLV has to be a UINT8 - Ptr! */
#define OHA_LLDP_SET_TL_FIELD(pTLV,Type,Len) \
{                                                  \
*(pTLV)++ = (LSA_UINT8) (((Type) << LLDP_TLV_TYPE_SHIFT) | (((Len) >> 8) & LLDP_TLV_LENGTH_HIGH_MSK)); \
*(pTLV)++ = (LSA_UINT8) ((Len) & LLDP_TLV_LENGTH_LOW_MSK);                                             \
}

/* pTLV has to be a UINT8 - Ptr! */
#define OHA_LLDP_SET_OUI_SUBTYPE_FIELD(pTLV,Oui1,Oui2,Oui3,SubType) \
{                                                    \
*(pTLV)++ = (LSA_UINT8) (Oui1);   \
*(pTLV)++ = (LSA_UINT8) (Oui2);   \
*(pTLV)++ = (LSA_UINT8) (Oui3);   \
*(pTLV)++ = (LSA_UINT8) (SubType);\
}

/*===========================================================================*/
/*                           Access to network bytes                         */
/*===========================================================================*/

/*------------------------------------------------------------------------------
//  read data from PDU / write data to PDU
//----------------------------------------------------------------------------*/

#define OHA_PDU_SET_U8(dp_, u8_)        \
{OHA_PUT8_HTON ((dp_), 0, (u8_)); (dp_) += 1;}

#define OHA_PDU_GET_U8(u8_, dp_)        \
{u8_ = OHA_GET8_NTOH ((dp_), 0); (dp_) += 1;}

#define OHA_PDU_SET_U16(dp_, u16_)      \
{OHA_PUT16_HTON ((dp_), 0, (u16_)); (dp_) += 2;}

#define OHA_PDU_GET_U16(u16_, dp_)      \
{u16_ = OHA_GET16_NTOH ((dp_), 0); (dp_) += 2;}

#define OHA_PDU_SET_U32(dp_, u32_)      \
{OHA_PUT32_HTON (dp_, 0, u32_); (dp_) += 4;}

#define OHA_PDU_GET_U32(u32_, dp_)      \
{u32_ = OHA_GET32_NTOH ((dp_), 0); (dp_) += 4;}

#define OHA_PDU_SET_MAC(dp_, mac_) \
{OHA_MEMCPY (dp_, mac_, OHA_MAC_ADDR_SIZE); (dp_) += OHA_MAC_ADDR_SIZE;}

#define OHA_PDU_GET_MAC(mac_, dp_) \
{OHA_MEMCPY (mac_, dp_, OHA_MAC_ADDR_SIZE); (dp_) += OHA_MAC_ADDR_SIZE;}

#define OHA_PDU_SET_IP(dp_, ip_) \
{OHA_MEMCPY (dp_, ip_, OHA_IP_ADDR_SIZE); (dp_) += OHA_IP_ADDR_SIZE;}

#define OHA_PDU_GET_IP(ip_, dp_) \
{OHA_MEMCPY (ip_, dp_, OHA_IP_ADDR_SIZE); (dp_) += OHA_IP_ADDR_SIZE;}

#define OHA_PDU_SET_MEM(dp_, strn_, len_) \
{OHA_MEMCPY (dp_, strn_, len_); (dp_) += len_;}

#define OHA_PDU_GET_MEM(strn_, dp_, len_) \
{OHA_MEMCPY (strn_, dp_, len_); (dp_) += len_;}

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

typedef  LSA_BOOL  OHA_LOCAL_FCT_ATTR OhaListCmpFunctionType (  /* return (ListEntry->field < NewEntry->field); */
    OHA_LIST_ENTRY_PTR_TYPE  ListEntry,
    OHA_LIST_ENTRY_PTR_TYPE  NewEntry
);
/* naming convention: oha_list_cmp__<struct>__<field> */

LSA_VOID  OHA_LOCAL_FCT_ATTR OhaListInsertSorted (
    OHA_LIST_ENTRY_PTR_TYPE  ListHead,
    OHA_LIST_ENTRY_PTR_TYPE  NewEntry,
    OhaListCmpFunctionType  Compare_ListEntry_LT_NewEntry
);

/*----------------------------------------------------------------------------*/


/*****************************************************************************/
/*  end of file oha_lib.h                                                   */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of OHA_LIB_H */
