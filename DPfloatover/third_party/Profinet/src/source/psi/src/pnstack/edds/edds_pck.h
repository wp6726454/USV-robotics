
#ifndef EDDS_PCK_H                       /* ----- reinclude-protection ----- */
#define EDDS_PCK_H

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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: edds_pck.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                       Frame for file "edds_pck.h".                        */
/*                       ==============================                      */
/*                                                                           */
/*  Internal headerfile                                                      */
/*  Defines internal types and data (and constants) for EDDS                 */
/*  which must be used with packing alignment.                               */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  the packing alignment instructions depend on the compiler.               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  23.06.04    JS    initial version.                                       */
/*  22.11.06    JS    added IP-Protocol types                                */
/*                    changed memory attribut                                */
/*  01.12.06    JS    #ifdef EDDS_CFG_XRT_OVER_UDP_INCLUDE removed           */
#endif
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
/* Note: It must be assured that the compiler dont inserts any padding bytes.*/
/*       The structure elements are already well aligned so typically no     */
/*       padding should be inserted/neccessary.                              */
/*       - all 16-Bit Values start on 16-Bit boundary                        */
/*       - all 32-Bit Values start on 32-Bit boundary                        */
/*       - all Structures are a multiple of 32-Bit in size                   */
/*       If and what kind of padding directive is neccessary depends on the  */
/*       compiler. Typically no directive is neccessary.                     */
/*---------------------------------------------------------------------------*/

/*===========================================================================*/
/*                           EDDS-Defines                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Help-Structure to access Type/len/FrameID within ISR                      */
/*---------------------------------------------------------------------------*/

typedef union _EDDS_TYPE_LEN_FRAME_ID_TYPE
{
    struct
    {
        LSA_UINT16  TypeLen;
        LSA_UINT16  FrameID;    /* Frame ID if SRT Frame             */
    } w;

    struct
    {
        LSA_UINT8   TypeLen_H;  /* because inet-format is big-endian */
        LSA_UINT8   TypeLen_L;  /* because inet-format is big-endian */
        LSA_UINT8   FrameID_H;  /* because inet-format is big-endian */
        LSA_UINT8   FrameID_L;  /* because inet-format is big-endian */
    } b;

    struct
    {
        LSA_UINT8   TypeLen_H;  /* because inet-format is big-endian */
        LSA_UINT8   TypeLen_L;  /* because inet-format is big-endian */
        LSA_UINT8   IPVersIHL;  /* IPHeader Version+IHL              */
        LSA_UINT8   IPTOS;      /* IPHeader TOS                      */
    } bIP;

    LSA_UINT32 LenTypeID;
} EDDS_TYPE_LEN_FRAME_ID_TYPE;

typedef LSA_UINT16   EDDS_FRAME_ID_TYPE;

/*---------------------------------------------------------------------------*/
/* MAC-Address union                                                         */
/* Used to access the MAC-Address in different ways.                         */
/*---------------------------------------------------------------------------*/

typedef union _EDDS_MAC_TYPE
{
    struct
    {
        LSA_UINT16  Dummy;
        LSA_UINT16  MAC12;
        LSA_UINT32  MAC3456;
    } w;

    struct
    {
        LSA_UINT8   Dummy[2];
        LSA_UINT8   MAC[EDD_MAC_ADDR_SIZE];
    } b;
} EDDS_MAC_TYPE;

/*---------------------------------------------------------------------------*/
/* IP-Address union                                                          */
/* Used to access the IP-Address in different ways.                          */
/*---------------------------------------------------------------------------*/

typedef union _EDDS_IP_TYPE
{
    LSA_UINT32 dw;

    struct
    {
        LSA_UINT16 W0;
        LSA_UINT16 W1;
    } w;

    struct
    {
        LSA_UINT8  IP[EDD_IP_ADDR_SIZE];
    } b;

} EDDS_IP_TYPE;


/*---------------------------------------------------------------------------*/
/* IP-Header                                                                 */
/*                                                                           */
/* Offset|Len|Fields                               |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*     0 | 1 |Version and IHL                      |                         */
/*     1 | 1 |TOS (Type of Service)                |                         */
/*     2 | 2 |Total Length                         |                         */
/*     2 | 2 |Identification                       |                         */
/*     2 | 2 |Flags and Fragmentoffset             |                         */
/*     1 | 1 |TTL                                  |                         */
/*     1 | 1 |Protocol                             |                         */
/*     2 | 2 |Header checksum                      |                         */
/*     4 | 4 |Source IP                            |                         */
/*     4 | 4 |Destination IP                       |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*                                                                           */
/* Note: The IP-Header starts 16-Bit aligned within an Ethernetframe.        */
/*       Not 32-Bit aligned!! So dont access the SrcIP/DestIP 32-Bit wise!   */
/* Note: The "options"-field within IP-Header is not included. Maybe present */
/*       but not allowed with xRToverUDP!                                    */
/*---------------------------------------------------------------------------*/

#define EDDS_IP_HEADER_SIZE                       20

typedef struct _EDDS_IP_HEADER_TYPE
{
    LSA_UINT8          VersionIHL;
    LSA_UINT8          TOS;
    LSA_UINT16         TotalLength;
    LSA_UINT16         Identification;
    LSA_UINT16         FragmentOffset;
    LSA_UINT8          TTL;
    LSA_UINT8          Protocol;
    LSA_UINT16         Checksum;
    EDDS_IP_TYPE       SrcIP;
    EDDS_IP_TYPE       DestIP;
} EDDS_IP_HEADER_TYPE; /* total ip header length: 20 bytes (=160 bits) */

typedef struct _EDDS_IP_HEADER_TYPE EDD_UPPER_MEM_ATTR * EDDS_IP_FRAME_PTR_TYPE;

/* IMPORTANT NOTE:                                                           */
/* SrcIP and DestIP may not be accessed as DWORDs because they may not be    */
/* DWORD aligned (within an ethernetframe, the IP-Header dont starts DWORD   */
/* aligned but WORD aligned!!). So if the machine dont allows unaligned      */
/* accesses, WORD access shall be used to access SrcIP and DestIP.           */

/* bit fields in VersionIHL */
#define EDDS_IP_VER         0xF0    /* IP version mask            */
#define EDDS_IP_IHL         0x0F    /* IP header length mask      */

/* bit fields in FragmentOffset */
#define EDDS_IP_RS          0x8000  /* reserved                   */
#define EDDS_IP_DF          0x4000  /* Don't fragment flag        */
#define EDDS_IP_MF          0x2000  /* More fragments flag        */
#define EDDS_IP_OFFSET      0x1FFF  /* Fragment Offset in segment */

/* for receive-check. Fragment offset and MF = 0, others dont care*/
#ifdef EDDS_CFG_BIG_ENDIAN
#define EDDS_IP_FLAG_OFFSET_MSK  0x3FFF
#define EDDS_IP_FLAG_OFFSET_VAL  0
#else
#define EDDS_IP_FLAG_OFFSET_MSK  0xFF3F
#define EDDS_IP_FLAG_OFFSET_VAL  0
#endif


/* TTL               */
#define EDDS_IP_TTL_MAX     64      /* maximum packet life        */

/* IP protocol codes */
#define EDDS_IP_ICMP        1       /* ICMP-Protocol              */
#define EDDS_IP_IGMP        2       /* IGMP-Protocol              */
#define EDDS_IP_TCP         6       /* TCP-Protocol               */
#define EDDS_IP_UDP         17      /* UDP-Protocol               */

/* Defaults for xRToverUDP */
#define EDDS_IP_XRT_VER_IHL ( 0x40 | 5 )  /* Version: 4. IHL: 5 */
#define EDDS_IP_XRT_TOS     ( 0 )         /* TOS: 0             */

/*---------------------------------------------------------------------------*/
/* UDP-Header                                                                */
/*                                                                           */
/* Offset|Len|Fields                               |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*     0 | 2 |Source Port                          |                         */
/*     2 | 2 |Destination Port                     |                         */
/*     4 | 2 |UDP Length                           |                         */
/*     6 | 2 |UDP Checksum                         |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*                                                                           */
/* Note: The UDP-Header starts 16-Bit aligned within an Ethernetframe.       */
/*---------------------------------------------------------------------------*/

#define EDDS_UDP_HEADER_SIZE                       8

typedef struct _EDDS_UDP_HEADER_TYPE
{
    LSA_UINT16         SrcPort;
    LSA_UINT16         DstPort;
    LSA_UINT16         Length;
    LSA_UINT16         Checksum;
} EDDS_UDP_HEADER_TYPE;

/* NOTE:                                                                     */
/* Within an ethernetframe the UDP-Header always starts WORD aligned, so     */
/* the structure members can be accessed as WORDS. The UDP Header dont starts*/
/* DWORD aligned within an ethernetframe!!                                   */

/*---------------------------------------------------------------------------*/
/* xRToverUDP general frame format (without ethernet header)                 */
/*                                                                           */
/* Note:  Both IP-Header and UPD-Header type are a multiple of 4 Byte, so    */
/*        they will fit back2back. We use "Data" to fill the header to a     */
/*        a multiple of 4-Bytes so the compiler dont need to fill 2 bytes    */
/*        for alignment!                                                     */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_UDP_XRT_FRAME_TYPE
{
    EDDS_IP_HEADER_TYPE  Ip;       /* 20 Bytes/5 DWORDs*/
    EDDS_UDP_HEADER_TYPE Udp;      /*  8 Bytes/2 DWORDs*/
    EDDS_FRAME_ID_TYPE   FrameID;      /*  2 Bytes         */
    LSA_UINT16           Data;
} EDDS_UDP_XRT_FRAME_TYPE;

typedef struct _EDDS_UDP_XRT_FRAME_TYPE EDD_UPPER_MEM_ATTR * EDDS_UDP_XRT_FRAME_PTR_TYPE;

/* Header length of IP+UPD-Header with xRT over UDP-Frames */
#define EDDS_UDP_XRT_HEADER_SIZE (EDDS_IP_HEADER_SIZE+EDDS_UDP_HEADER_SIZE)


/*===========================================================================*/
/*                           ARP-Defines                                     */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Ethernet transmission layer                                    For IPv4   */
/*                                                              and Ethernet */
/*                                                                  Bytes    */
/*   48.bit: Ethernet address of destination                          6      */
/*   48.bit: Ethernet address of sender                               6      */
/*   16.bit: Protocol type = ether_type$ADDRESS_RESOLUTION  = 0x806   2      */
/*                                                                           */
/* Ethernet packet data:                                                     */
/*   16.bit: (ar$hrd) Hardware address space (e.g., Ethernet,         2      */
/*                    Packet Radio Net.)                                     */
/*   16.bit: (ar$pro) Protocol address space.  For Ethernet           2      */
/*                    hardware, this is from the set of type                 */
/*                    fields ether_typ$&lt;protocol&gt;.                     */
/*    8.bit: (ar$hln) byte length of each hardware address            1      */
/*    8.bit: (ar$pln) byte length of each protocol address            1      */
/*   16.bit: (ar$op)  opcode (ares_op$REQUEST | ares_op$REPLY)        2      */
/*   nbytes: (ar$sha) Hardware address of sender of this              6      */
/*                    packet, n from the ar$hln field.                       */
/*   mbytes: (ar$spa) Protocol address of sender of this              4      */
/*                    packet, m from the ar$pln field.                       */
/*   nbytes: (ar$tha) Hardware address of target of this              6      */
/*                    packet (if known).                                     */
/*   mbytes: (ar$tpa) Protocol address of target.                     4      */
/*                                                                  -----    */
/*                                                                   42      */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* Note: must start at least 16-Bit aligned!*/

typedef struct _EDDS_ARP_FRAME_TYPE
{
    LSA_UINT16      Hrd;            /* Headertype    (1)       */
    LSA_UINT16      Pro;            /* Protocoltype  (0x800)   */
    LSA_UINT8       Hln;            /* Hardware addr. len (6)  */
    LSA_UINT8       Pln;            /* Protocol len (4)        */
    LSA_UINT16      Op;             /* Opcode (1,2)            */
    LSA_UINT8       Sha[EDD_MAC_ADDR_SIZE];  /* Sender Hardware addr.   */
    LSA_UINT8       Spa[EDD_IP_ADDR_SIZE];   /* Sender Protocol addr    */
    LSA_UINT8       Tha[EDD_MAC_ADDR_SIZE];  /* Target Hardware addr.   */
    LSA_UINT8       Tpa[EDD_IP_ADDR_SIZE];   /* Target Protocol addr.   */
} EDDS_ARP_FRAME_TYPE;

typedef struct _EDDS_ARP_FRAME_TYPE EDD_UPPER_MEM_ATTR * EDDS_ARP_FRAME_PTR_TYPE;

/* field ar$hdr */
#define EDDS_ARP_HRD_ETHERNET    1

/* field ar$pro */
#define EDDS_ARP_PRO_IP          0x800

/* field ar$op*/
#define EDDS_ARP_OP_REQUEST      1
#define EDDS_ARP_OP_RESPONSE     2

/* field ar$hln */
#define EDDS_ARP_HLN             EDD_MAC_ADDR_SIZE /* 6 */

/* field ar$pln */
#define EDDS_ARP_PLN             EDD_IP_ADDR_SIZE  /* 4 */

/*****************************************************************************/
/*  end of file EDDS_PCK.H                                                   */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_PCK_H */
