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
/*  C o m p o n e n t     &C: LLDP (Link Layer Discovery Protocol)      :C&  */
/*                                                                           */
/*  F i l e               &F: lldp_tlv.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  LLDP-global functions for TLV handling           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef LLDP_MESSAGE
/*  06.04.04    JS    initial version.                                       */
/*  13.07.04    JS    additional check for length==0 with End of LLPDU TLV   */
/*  20.10.04    JS    modified check for max-tlv size within set-function to */
/*                    be able to insert an end-of-lldpdu because it is       */
/*                    mandatory now.                                         */
/*                    Additional parameter checks                            */
/*                    Added many new functions                               */
/*  18.02.05    JS    some changes to prevent compiler warnings              */
/*  17.10.05    JS    added support for optional tlvs to lldp_TLVGetParams   */
/*  04.11.05    JS    added typecast to avoid compiler warning               */
/*  21.06.06    JS    some casts to avoid compiler warnings with gcc         */
/*  27.06.06    JS    added  LLDP_FILE_SYSTEM_EXTENSION                      */
/*  29.05.08    JS    changes to avoid C++ errors with void asignments       */
/*  21.09.09    JS    changes to avoid compiler warning (AP00827939)         */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  6
#define LLDP_MODULE_ID     LTRC_ACT_MODUL_ID /* LLDP_MODULE_ID_LLDP_TLV */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "lldp_inc.h"            /* LLDP headerfiles */
#include "lldp_int.h"            /* internal header */

LLDP_FILE_SYSTEM_EXTENSION(LLDP_MODULE_ID)

#define LLDP_CFG_UTIL_PTR_PARAM_CHECK

/*===========================================================================*/
/*                                    defines                                */
/*===========================================================================*/

/* pTLV has to be a UINT8 - Ptr! */
#define LLDP_SET_TL_FIELD_INCR(pTLV,Type,Len) \
{                                                  \
*(pTLV)++ = (LSA_UINT8) (((Type) << LLDP_TLV_TYPE_SHIFT) | (((Len) >> 8) & LLDP_TLV_LENGTH_HIGH_MSK)); \
*(pTLV)++ = (LSA_UINT8) ((Len) & LLDP_TLV_LENGTH_LOW_MSK);                                             \
}

/* pTLV has to be a UINT8 - Ptr! */
#define LLDP_SET_OUI_GROUP_FIELD_INCR(pTLV,Oui1,Oui2,Oui3,Group) \
{                                                    \
*(pTLV)++ = (LSA_UINT8) (Oui1);  \
*(pTLV)++ = (LSA_UINT8) (Oui2);  \
*(pTLV)++ = (LSA_UINT8) (Oui3);  \
*(pTLV)++ = (LSA_UINT8) (Group); \
}
/* pValue has to be a UINT8 - Ptr! */
#define LLDP_GET_U16(pValue)  ((LSA_UINT16) (((LSA_UINT16)(*(pValue)) << 8 ) + *((pValue)+1)))

/* pValue has to be a UINT8 - Ptr! */
#define LLDP_GET_U32(pValue)  ((LSA_UINT32) (((LSA_UINT32)(*(pValue))     << 24) + \
((LSA_UINT32)(*((pValue)+1)) << 16) + \
((LSA_UINT32)(*((pValue)+2)) << 8)  + \
*((pValue)+3)))
/* pValue has to be a UINT8 - Ptr! */
#define LLDP_SET_U16_INCR(pValue,Value) \
{                                       \
*(pValue)++ = (LSA_UINT8) ((Value) >> 8 );   \
*(pValue)++ = (LSA_UINT8) ((Value));         \
}

/* pValue has to be a UINT8 - Ptr! */
#define LLDP_SET_U32_INCR(pValue,Value) \
{                                       \
*(pValue)++ = (LSA_UINT8) ((Value) >> 24 );  \
*(pValue)++ = (LSA_UINT8) ((Value) >> 16 );  \
*(pValue)++ = (LSA_UINT8) ((Value) >> 8  );  \
*(pValue)++ = (LSA_UINT8) ((Value));         \
}

/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      LLDP_TLVSetCommon                                    +*/
/*+  Input/Output      LLDP_COMMON_MEM_U8_PTR_TYPE  LLDP_COMM..    *ppTLV   +*/
/*+                    LLDP_COMMON_MEM_U32_PTR_TYPE                 pTLength+*/
/*+                    LSA_UINT32                                   Type    +*/
/*+                    LSA_UINT32                                   Length  +*/
/*+                    LLDP_COMMON_MEM_PTR_TYPE                     pValue  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  Type                 : in: Type (T) of TLV element to add              +*/
/*+  Length               : in: Length (L) of Value-Field of TLV to add     +*/
/*+  pValue               : in: Pointer to Value (V) of TLV element to add  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by pTLV    +*/
/*+               ppTLV and pLength is  updated.                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_TLVSetCommon(
    LLDP_COMMON_MEM_U8_PTR_TYPE LLDP_COMMON_MEM_ATTR *ppTLV,
    LLDP_COMMON_MEM_U32_PTR_TYPE                      pTLength,
    LSA_UINT32                                        Type,
    LSA_UINT32                                        Length,
    LLDP_COMMON_MEM_PTR_TYPE                          pValue)
{

    LLDP_COMMON_MEM_U8_PTR_TYPE pTLVHelp;

    pTLVHelp = *ppTLV;

    pTLVHelp[0] = (LSA_UINT8) ((Type << LLDP_TLV_TYPE_SHIFT) | ((Length >> 8) & LLDP_TLV_LENGTH_HIGH_MSK));
    pTLVHelp[1] = (LSA_UINT8) (Length  & LLDP_TLV_LENGTH_LOW_MSK);

    if ( Length ) /* length of 0 is allowed ! */
    {
        LLDP_MEMCOPY_COMMON( &pTLVHelp[LLDP_TL_SIZE],
                             pValue,
                             Length);
    }

    *pTLength += Length + LLDP_TL_SIZE; /* new length  */
    *ppTLV    += Length + LLDP_TL_SIZE; /* new pointer */


}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      LLDP_TLVGetCommon                                    +*/
/*+  Input/Output      LLDP_COMMON_MEM_U8_PTR_TYPE  LLDP_COMMON..  *ppTLV   +*/
/*+                    LLDP_COMMON_MEM_U32_PTR_TYPE                 pTLength+*/
/*+                    LLDP_COMMON_MEM_U32_PTR_TYPE                 pType   +*/
/*+                    LLDP_COMMON_MEM_U32_PTR_TYPE                 pLength +*/
/*+                    LLDP_COMMON_MEM_PTR_TYPE     LLDP_COMMON    *ppValue +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to read TLV element from +*/
/*+                         out:Pointer behind read TLV element (next TLV)  +*/
/*+  pTLength             : in: Length of TLV chain before TLV read         +*/
/*+                         out:remaining length of TLV-chain.              +*/
/*+  pType                : out:Type (T) of TLV element read                +*/
/*+  pLength              : out:Length (L) of TLV element read              +*/
/*+  ppValue              : out:Pointer to address of Value (V) field       +*/
/*+                                                                         +*/
/*+  Result:           LLDP_RSP_OK                                          +*/
/*+                    LLDP_RSP_ERR_TLV_MISMATCH                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets (read) an TLV-element beginning at *ppTLV and updates+*/
/*+               pLength and ppTLV to next TLV in chain.                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_TLVGetCommon(
    LLDP_COMMON_MEM_U8_PTR_TYPE LLDP_COMMON_MEM_ATTR *ppTLV,
    LLDP_COMMON_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_COMMON_MEM_U32_PTR_TYPE                      pType,
    LLDP_COMMON_MEM_U32_PTR_TYPE                      pLength,
    LLDP_COMMON_MEM_PTR_TYPE    LLDP_COMMON_MEM_ATTR *ppValue)
{

    LLDP_COMMON_MEM_U8_PTR_TYPE pTLVHelp;
    LLDP_RSP                    Response;
    LSA_UINT32                  Len;

    Response = LLDP_RSP_OK;

    pTLVHelp = *ppTLV;

    /* check if total length is big enough to hold the TL. */

    if ( *pTLength < LLDP_TL_SIZE)
    {
        Response = LLDP_RSP_ERR_TLV_MISMATCH;
    }
    else
    {
        *pType   = (pTLVHelp[0] & LLDP_TLV_TYPE_MSK) >> LLDP_TLV_TYPE_SHIFT;
        *pLength = (((LSA_UINT32) (pTLVHelp[0] & LLDP_TLV_LENGTH_HIGH_MSK)) << 8) + pTLVHelp[1];
        *ppValue = &pTLVHelp[LLDP_TL_SIZE];

        Len = *pLength + LLDP_TL_SIZE;

        /* check if total length is big enough to hold the TLV we read */
        /* if not we have a TLV mismatch.                              */

        if ( *pTLength < Len )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {

            /* update remaining length */
            *pTLength = *pTLength - Len;
            *ppTLV    = &pTLVHelp[Len]; /* point to next TLV */
        }
    }

    return(Response);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      LLDP_TLVSetChassisPortTTLCommon                      +*/
/*+  Input/Output      LLDP_COMMON_MEM_PTR_TYPE LLDP_COMMON...  *ppTLV      +*/
/*+                    LLDP_COMMON_MEM_U32_PTR_TYPE              pTLength   +*/
/*+                    LLDP_TLV_SET_INFO_TYPE  LLDP_COMMON...   *pTLVInfo   +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : out:Pointer to TLV chain length                 +*/
/*+  pTLVInfo             : in: Pointer to TLVInfo structure with TLVs      +*/
/*+                             to put into *ppTLV.                         +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds the TLVs from pTLVInfo into the ppTLV.               +*/
/*+               These must be the first TLVs within a LLDP-PDU            +*/
/*+                                                                         +*/
/*+               ppTLV must have room for the TLVs added.                  +*/
/*+               The max length of all TLVs is LLDP_MAX_TLV_DATA_SIZE.     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_TLVSetChassisPortTTLCommon(
    LLDP_COMMON_MEM_PTR_TYPE    LLDP_COMMON_MEM_ATTR *ppTLV,
    LLDP_COMMON_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_TLV_SET_INFO_TYPE      LLDP_COMMON_MEM_ATTR *pTLVInfo)
{

    LLDP_RSP                    Response;
    LLDP_COMMON_MEM_U8_PTR_TYPE pTLVHelp;
    LSA_UINT32                  Len;
    LSA_UINT32                  TLen;

    Response = LLDP_RSP_OK;

    if ( ( pTLVInfo->ChassisIDLen < LLDP_MIN_CHASSIS_ID_STRING_SIZE   )  ||
         ( pTLVInfo->ChassisIDLen > LLDP_MAX_CHASSIS_ID_STRING_SIZE   )  ||
         ( pTLVInfo->PortIDLen    < LLDP_MIN_PORT_ID_STRING_SIZE      )  ||
         ( pTLVInfo->PortIDLen    > LLDP_MAX_PORT_ID_STRING_SIZE      )  ||
         ( LSA_HOST_PTR_ARE_EQUAL(pTLVInfo->pChassisIDString,LSA_NULL))  ||
         ( LSA_HOST_PTR_ARE_EQUAL(pTLVInfo->pPortIDString,LSA_NULL   ))
       )
    {

        Response = LLDP_RSP_ERR_PARAM;

    }
    else
    {
        pTLVHelp = (LLDP_COMMON_MEM_U8_PTR_TYPE)*ppTLV;
        TLen = 0;

        /* ------------------------------------------------------------------*/
        /* insert Chassis ID                                                 */
        /* ------------------------------------------------------------------*/

        Len = pTLVInfo->ChassisIDLen + LLDP_TLV_CHASSIS_ID_SUBTYPE_SIZE;

        /* TL-Fields */

        LLDP_SET_TL_FIELD_INCR(pTLVHelp,LLDP_TLV_TYPE_CHASSIS_ID,Len);

        /* V-Field */

        *pTLVHelp++ = pTLVInfo->ChassisIDSubtype;

        if (pTLVInfo->ChassisIDLen)
        {
            LLDP_MEMCOPY_COMMON(
                pTLVHelp,
                pTLVInfo->pChassisIDString,
                pTLVInfo->ChassisIDLen);

            pTLVHelp += pTLVInfo->ChassisIDLen;
        }

        TLen += LLDP_TL_SIZE + Len;

        /* ------------------------------------------------------------------*/
        /* insert Port ID                                                    */
        /* ------------------------------------------------------------------*/

        Len = pTLVInfo->PortIDLen + LLDP_TLV_PORT_ID_SUBTYPE_SIZE;

        /* TL-Fields */

        LLDP_SET_TL_FIELD_INCR(pTLVHelp,LLDP_TLV_TYPE_PORT_ID,Len);

        /* V-Field */

        *pTLVHelp++ = pTLVInfo->PortIDSubtype;

        if (pTLVInfo->PortIDLen)
        {
            LLDP_MEMCOPY_COMMON(
                pTLVHelp,
                pTLVInfo->pPortIDString,
                pTLVInfo->PortIDLen);

            pTLVHelp += pTLVInfo->PortIDLen;
        }

        TLen += LLDP_TL_SIZE + Len;

        /* ------------------------------------------------------------------*/
        /* insert TTL                                                        */
        /* ------------------------------------------------------------------*/

        Len = LLDP_TLV_TTL_SIZE;

        /* TL-Fields */

        LLDP_SET_TL_FIELD_INCR(pTLVHelp,LLDP_TLV_TYPE_TTL,Len);

        /* V-Field */
        *pTLVHelp++ = (LSA_UINT8) (pTLVInfo->TimeToLive >> 8 );
        *pTLVHelp++ = (LSA_UINT8) (pTLVInfo->TimeToLive & 0xFF);

        TLen += LLDP_TL_SIZE + Len;

        *ppTLV    = pTLVHelp;
        *pTLength = TLen;

    }

    return(Response);


}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      LLDP_TLVGetChassisPortTTLCommon                      +*/
/*+  Input/Output      LLDP_COMMON_MEM_PTR_TYPE LLDP_COMMON... *ppTLV       +*/
/*+                    LLDP_COMMON_MEM_U32_PTR_TYPE             pTLength    +*/
/*+                    LLDP_TLV_GET_INFO_TYPE   LLDP_COMMON... *pTLVInfo    +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in :Pointer to TLV chain length                 +*/
/*+                         out:Pointer to remaining TLV chain length       +*/
/*+  pTLVInfo             : in: Pointer to TLVInfo structure with TLVs      +*/
/*+                             read from chain on success.                 +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads the TLVs from the TLV chain into the TLVInfo        +*/
/*+                                                                         +*/
/*+               The TLVs must be the first TLVs within the chain and      +*/
/*+               in the correct order (Chassis,Port,TTL). If not we        +*/
/*+               set LLDP_RSP_ERR_TLV_MISMATCH.                            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_TLVGetChassisPortTTLCommon(
    LLDP_COMMON_MEM_PTR_TYPE    LLDP_COMMON_MEM_ATTR *ppTLV,
    LLDP_COMMON_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_TLV_GET_INFO_TYPE      LLDP_COMMON_MEM_ATTR *pTLVInfo)
{

    LLDP_RSP                    Response;
    LLDP_COMMON_MEM_U8_PTR_TYPE pTLVHelp;
    LSA_UINT32                  TLen;
    LSA_UINT32                  Type;
    LSA_UINT32                  Length;
    LLDP_COMMON_MEM_U8_PTR_TYPE pValue;


    Response = LLDP_RSP_OK;

    pTLVHelp = (LLDP_COMMON_MEM_U8_PTR_TYPE)*ppTLV;
    TLen     = *pTLength;
    Type     = 0;         /* initialize to prevent compiler warning */
    Length   = 0;         /* initialize to prevent compiler warning */
    pValue   = LSA_NULL;  /* initialize to prevent compiler warning */

    /* ------------------------------------------------------------------*/
    /* get Chassis ID                                                    */
    /* ------------------------------------------------------------------*/

    Response = LLDP_TLVGetCommon(
                   (LLDP_COMMON_MEM_U8_PTR_TYPE LLDP_COMMON_MEM_ATTR *) &pTLVHelp,
                   (LLDP_COMMON_MEM_U32_PTR_TYPE) &TLen,
                   (LLDP_COMMON_MEM_U32_PTR_TYPE) &Type,
                   (LLDP_COMMON_MEM_U32_PTR_TYPE) &Length,
                   (LLDP_COMMON_MEM_PTR_TYPE    LLDP_COMMON_MEM_ATTR *) &pValue);

    if ( Response == LLDP_RSP_OK )
    {
        if (( Type   == LLDP_TLV_TYPE_CHASSIS_ID     ) &&
            ( Length >= LLDP_TLV_CHASSIS_ID_MIN_SIZE ) &&
            ( Length <= LLDP_TLV_CHASSIS_ID_SUBTYPE_SIZE + LLDP_MAX_CHASSIS_ID_STRING_SIZE ))
        {
            pTLVInfo->ChassisIDSubtype = *pValue++;
            pTLVInfo->ChassisIDLen     = (LSA_UINT16) (Length - LLDP_TLV_CHASSIS_ID_SUBTYPE_SIZE);

            LLDP_MEMCOPY_COMMON( (LLDP_COMMON_MEM_PTR_TYPE) pTLVInfo->ChassisIDString,
                                 pValue,
                                 Length);

        }
        else
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }

    }


    /* ------------------------------------------------------------------*/
    /* get Port ID                                                       */
    /* ------------------------------------------------------------------*/

    if ( Response == LLDP_RSP_OK )
    {

        Response = LLDP_TLVGetCommon(
                       (LLDP_COMMON_MEM_U8_PTR_TYPE LLDP_COMMON_MEM_ATTR *) &pTLVHelp,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE)&TLen,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE)&Type,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE)&Length,
                       (LLDP_COMMON_MEM_PTR_TYPE    LLDP_COMMON_MEM_ATTR *) &pValue);

        if ( Response == LLDP_RSP_OK )
        {
            if (( Type   == LLDP_TLV_TYPE_PORT_ID    ) &&
                ( Length >= LLDP_TLV_PORT_ID_MIN_SIZE ) &&
                ( Length <= LLDP_TLV_PORT_ID_SUBTYPE_SIZE + LLDP_MAX_PORT_ID_STRING_SIZE ))
            {
                pTLVInfo->PortIDSubtype = *pValue++;
                pTLVInfo->PortIDLen     = (LSA_UINT16)(Length - LLDP_TLV_PORT_ID_SUBTYPE_SIZE);

                LLDP_MEMCOPY_COMMON( (LLDP_COMMON_MEM_PTR_TYPE) pTLVInfo->PortIDString,
                                     pValue,
                                     Length);

            }
            else
            {
                Response = LLDP_RSP_ERR_TLV_MISMATCH;
            }

        }

    }


    /* ------------------------------------------------------------------*/
    /* get TTL                                                           */
    /* ------------------------------------------------------------------*/


    if ( Response == LLDP_RSP_OK )
    {

        Response = LLDP_TLVGetCommon(
                       (LLDP_COMMON_MEM_U8_PTR_TYPE LLDP_COMMON_MEM_ATTR *) &pTLVHelp,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE) &TLen,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE) &Type,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE) &Length,
                       (LLDP_COMMON_MEM_PTR_TYPE    LLDP_COMMON_MEM_ATTR *) &pValue);

        if ( Response == LLDP_RSP_OK )
        {
            if (( Type   == LLDP_TLV_TYPE_TTL    ) &&
                ( Length >= LLDP_TLV_TTL_SIZE    ))
            {
                pTLVInfo->TimeToLive    = (LSA_UINT16) (((LSA_UINT16)(*pValue) << 8 ) + *(pValue+1));
            }
            else
            {
                Response = LLDP_RSP_ERR_TLV_MISMATCH;
            }
        }
    }
    /* ------------------------------------------------------------------*/
    /* Ok return it.                                                     */
    /* ------------------------------------------------------------------*/
    if ( Response == LLDP_RSP_OK )
    {
        *ppTLV     = pTLVHelp;  /* points to next TLV */
        *pTLength  = TLen;      /* remaining len      */
    }
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      LLDP_TLVValidateMgmAddrLen                           +*/
/*+  Input/Output      LLDP_COMMON_MEM_U8_PTR_TYPE              pTLV        +*/
/*+                    LSA_UINT32                               TLength     +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pTLV                 : in: Pointer to start of value-field of          +*/
/*+                             mgmaddr-tlv                                 +*/
/*+                             (points to mgmaddrstring length field)      +*/
/*+  TLength              : in: Total remaining length of TLV-chain         +*/
/*+                                                                         +*/
/*+  Result:           LLDP_RSP_OK                                          +*/
/*+                    LLDP_RSP_ERR_TLV_MISMATCH                            +*/
/*+                    LLDP_RSP_ERR_TLV_PARAM_ERROR                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Validates the Management Address TLV for Length validness +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_TLVValidateMgmAddrLen(
    LLDP_COMMON_MEM_U8_PTR_TYPE    pTLV,
    LSA_UINT32                     TLength)
{

    LLDP_RSP     Response;
    LSA_UINT32   MgmStrLen;
    LSA_UINT32   OIDStrLen;

    Response = LLDP_RSP_OK;

    if ( TLength >= LLDP_TLV_MGM_ADDR_MIN_SIZE )
    {
        MgmStrLen = pTLV[0];  /* read mgm addr string length */

        /* check for valid parameter */
        if (( MgmStrLen >= LLDP_TLV_MGM_ADDR_MIN_MGMT_ADDR_STRING_LEN ) &&
            ( MgmStrLen <= LLDP_TLV_MGM_ADDR_MAX_MGMT_ADDR_STRING_LEN ))
        {

            /* check against remaining length */
            if ( TLength >= LLDP_TLV_MGM_ADDR_MIN_SIZE
                 + MgmStrLen
                 - LLDP_TLV_MGM_ADDR_MIN_MGMT_ADDR_STRING_LEN )
            {

                /* read OIDStrLen */
                OIDStrLen = pTLV[LLDP_TLV_MGM_ADDR_MGMT_ADDR_STRING_LENGTH_SIZE
                                 +MgmStrLen
                                 +LLDP_TLV_MGM_ADDR_IFNUM_SUBTYPE_SIZE
                                 +LLDP_TLV_MGM_ADDR_IFNUM_SIZE];

                /* check for valid parameter */
#if LLDP_TLV_MGM_ADDR_MIN_OID_STRING_LEN > 0
                if (( OIDStrLen  >= LLDP_TLV_MGM_ADDR_MIN_OID_STRING_LEN ) &&
                    ( OIDStrLen  <= LLDP_TLV_MGM_ADDR_MAX_OID_STRING_LEN ))
#else
                if (( OIDStrLen  <= LLDP_TLV_MGM_ADDR_MAX_OID_STRING_LEN ))
#endif
                {
                    /* check against remaining length */
                    if ( TLength >= LLDP_TLV_MGM_ADDR_MIN_SIZE
                         + MgmStrLen
                         - LLDP_TLV_MGM_ADDR_MIN_MGMT_ADDR_STRING_LEN
                         + OIDStrLen
                         - LLDP_TLV_MGM_ADDR_MIN_OID_STRING_LEN  )
                    {
                        Response = LLDP_RSP_OK;
                    }
                    else
                    {
                        Response = LLDP_RSP_ERR_TLV_MISMATCH;
                    }
                }
                else
                {
                    Response = LLDP_RSP_ERR_TLV_PARAM_ERROR;
                }
            }
            else
            {
                Response = LLDP_RSP_ERR_TLV_MISMATCH;
            }
        }
        else
        {
            Response = LLDP_RSP_ERR_TLV_PARAM_ERROR;
        }
    }
    else
    {
        Response = LLDP_RSP_ERR_TLV_MISMATCH;
    }
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      LLDP_TLVValidateVLANNameLen                          +*/
/*+  Input/Output      LLDP_COMMON_MEM_U8_PTR_TYPE              pTLV        +*/
/*+                    LSA_UINT32                               TLength     +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pTLV                 : in: Pointer to start of Group-specific TLV      +*/
/*+                             Part of VLAN Name TLV                       +*/
/*+                             (points to VLAN ID field )                  +*/
/*+  TLength              : in: Total remaining length of TLV-chain         +*/
/*+                                                                         +*/
/*+  Result:           LLDP_RSP_OK                                          +*/
/*+                    LLDP_RSP_ERR_TLV_MISMATCH                            +*/
/*+                    LLDP_RSP_ERR_TLV_PARAM_ERROR                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Validates the VLAN Name TLV for length validness          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_TLVValidateVLANNameLen(
    LLDP_COMMON_MEM_U8_PTR_TYPE    pTLV,
    LSA_UINT32                     TLength)
{

    LLDP_RSP     Response;
    LSA_UINT32   VLANNameLen;

    Response = LLDP_RSP_OK;

    if ( TLength >= LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MIN_SIZE )
    {

        VLANNameLen = pTLV[LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_VLAN_ID_SIZE];

        if (( VLANNameLen >= LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MIN_VLAN_NAME_LEN ) &&
            ( VLANNameLen <= LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MAX_VLAN_NAME_LEN ))
        {

            if ( TLength >= (LSA_UINT32) (LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MIN_SIZE
                                          + VLANNameLen
                                          - LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MIN_VLAN_NAME_LEN))
            {
                Response = LLDP_RSP_OK;
            }
            else
            {
                Response = LLDP_RSP_ERR_TLV_MISMATCH;
            }
        }
        else
        {
            Response = LLDP_RSP_ERR_TLV_PARAM_ERROR;
        }
    }
    else
    {
        Response = LLDP_RSP_ERR_TLV_MISMATCH;
    }
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      LLDP_TLVValidateProtIdentityLen                      +*/
/*+  Input/Output      LLDP_COMMON_MEM_U8_PTR_TYPE              pTLV        +*/
/*+                    LSA_UINT32                               TLength     +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pTLV                 : in: Pointer to start of Group-specific TLV      +*/
/*+                             Part of Protocol Identity TLV               +*/
/*+                             (points to protocol identity length field)  +*/
/*+  TLength              : in: Total remaining length of TLV-chain         +*/
/*+                                                                         +*/
/*+  Result:           LLDP_RSP_OK                                          +*/
/*+                    LLDP_RSP_ERR_TLV_MISMATCH                            +*/
/*+                    LLDP_RSP_ERR_TLV_PARAM_ERROR                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Validates the Protocol Identity length validness          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_TLVValidateProtIdentityLen(
    LLDP_COMMON_MEM_U8_PTR_TYPE    pTLV,
    LSA_UINT32                     TLength)
{

    LLDP_RSP     Response;
    LSA_UINT32   ProtIdentLen;

    Response = LLDP_RSP_OK;

    if ( TLength >= LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MIN_SIZE )
    {
        ProtIdentLen = pTLV[0];
#if LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MIN_LEN > 0
        if (( ProtIdentLen >= LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MIN_LEN ) &&
            ( ProtIdentLen <= LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MAX_LEN ))
#else
        if ( ProtIdentLen <= LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MAX_LEN )
#endif
        {

            if ( TLength >= (LSA_UINT32) (LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MIN_SIZE
                                          + ProtIdentLen
                                          - LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MIN_LEN))
            {
                Response = LLDP_RSP_OK;
            }
            else
            {
                Response = LLDP_RSP_ERR_TLV_MISMATCH;
            }
        }
        else
        {
            Response = LLDP_RSP_ERR_TLV_PARAM_ERROR;
        }
    }
    else
    {
        Response = LLDP_RSP_ERR_TLV_MISMATCH;
    }
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      LLDP_TLVValidateChain                                +*/
/*+  Input/Output      LLDP_COMMON_MEM_U8_PTR_TYPE              pTLV        +*/
/*+                    LSA_UINT32                               TLength     +*/
/*+                    LLDP_COMMON_MEM_U32_PTR_TYPE             pTTL        +*/
/*+                    LLDP_COMMON_MEM_U32_PTR_TYPE             pNLength    +*/
/*+                    LLDP_COMMON_MEM_BOOL_PTR_TYPE            pEndTLV     +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pTLV                 : in: Pointer to Start of TLV chain (PDU)         +*/
/*+  TLength              : in: Length of TLV chain (PDU)                   +*/
/*+  pTTL                 : out:TTL Value from TTL-TLV                      +*/
/*+  pNLength             : out:Total length of TLV-Chain till End-TLV      +*/
/*+                             (included) or frame end                     +*/
/*+  pEndTLV              : out:LSA_TRUE: End TLV present.                  +*/
/*+                             LSA_FALSE:End TLV not present.              +*/
/*+                                                                         +*/
/*+  Result:           LLDP_RSP_OK                                          +*/
/*+                    LLDP_RSP_ERR_TLV_MISMATCH                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Validates the TLV-Chain(PDU) and on success returns TTL.  +*/
/*+               and length of TLV-Chain (maybe shorter then TLength)      +*/
/*+                                                                         +*/
/*+               Validation:                                               +*/
/*+               - chassis ID, PortID, TTL must be first TLVs              +*/
/*+               - only one Chassis ID, PortID and TTL                     +*/
/*+               - Length of TLVs must match. Checked for all known TLVs   +*/
/*+               - Total Length of TLV chain must match.                   +*/
/*+                                                                         +*/
/*+               Note: Parameter errors within length-fields within TLVs   +*/
/*+                     will not result in an Mismatch-error. This will be  +*/
/*+                     a TLV error which will only void the TLV not the    +*/
/*+                     whole chain.                                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_TLVValidateChain(
    LLDP_COMMON_MEM_U8_PTR_TYPE    pTLV,
    LSA_UINT32                     TLength,
    LLDP_COMMON_MEM_U32_PTR_TYPE   pTTL,
    LLDP_COMMON_MEM_U32_PTR_TYPE   pNLength,
    LLDP_COMMON_MEM_BOOL_PTR_TYPE  pEndTLV)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  Type;
    LSA_UINT32                  Length;
    LSA_UINT32                  RLength;
    LLDP_COMMON_MEM_U8_PTR_TYPE pValue;

    *pEndTLV  = LSA_FALSE;
    *pNLength = TLength;
    RLength   = TLength; /* remaining length */

    if ( TLength > LLDP_MAX_TLV_DATA_SIZE )
    {
        Response = LLDP_RSP_ERR_TLV_MISMATCH;
    }
    else
    {
        /* ------------------------------------------------------------------*/
        /* First TLV must be Chassis-ID                                      */
        /* ------------------------------------------------------------------*/

        Type     = 0;         /* initialize to prevent compiler warning */
        Length   = 0;         /* initialize to prevent compiler warning */

        Response = LLDP_TLVGetCommon(
                       (LLDP_COMMON_MEM_U8_PTR_TYPE LLDP_COMMON_MEM_ATTR *) &pTLV,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE)&RLength,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE)&Type,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE)&Length,
                       (LLDP_COMMON_MEM_PTR_TYPE    LLDP_COMMON_MEM_ATTR *) &pValue);

        if ( Response == LLDP_RSP_OK )
        {
            if ( ( Type   != LLDP_TLV_TYPE_CHASSIS_ID    )  ||
                 ( Length < LLDP_TLV_CHASSIS_ID_MIN_SIZE )  ||
                 ( Length > LLDP_TLV_CHASSIS_ID_SUBTYPE_SIZE + LLDP_MAX_CHASSIS_ID_STRING_SIZE ))
            {
                Response = LLDP_RSP_ERR_TLV_MISMATCH;
            }
        }

        /* ------------------------------------------------------------------*/
        /* Second TLV must be Port-ID                                        */
        /* ------------------------------------------------------------------*/

        if ( Response == LLDP_RSP_OK )
        {
            Response = LLDP_TLVGetCommon(
                           (LLDP_COMMON_MEM_U8_PTR_TYPE LLDP_COMMON_MEM_ATTR *) &pTLV,
                           (LLDP_COMMON_MEM_U32_PTR_TYPE)&RLength,
                           (LLDP_COMMON_MEM_U32_PTR_TYPE)&Type,
                           (LLDP_COMMON_MEM_U32_PTR_TYPE)&Length,
                           (LLDP_COMMON_MEM_PTR_TYPE    LLDP_COMMON_MEM_ATTR *) &pValue);

            if ( Response == LLDP_RSP_OK )
            {
                if (( Type   != LLDP_TLV_TYPE_PORT_ID    )  ||
                    ( Length < LLDP_TLV_PORT_ID_MIN_SIZE )  ||
                    ( Length > LLDP_TLV_PORT_ID_SUBTYPE_SIZE + LLDP_MAX_PORT_ID_STRING_SIZE ))
                {
                    Response = LLDP_RSP_ERR_TLV_MISMATCH;
                }
            }
        }

        /* ------------------------------------------------------------------*/
        /* Third TLV must be TTL                                             */
        /* ------------------------------------------------------------------*/

        if ( Response == LLDP_RSP_OK )
        {

            Response = LLDP_TLVGetCommon(
                           (LLDP_COMMON_MEM_U8_PTR_TYPE LLDP_COMMON_MEM_ATTR *) &pTLV,
                           (LLDP_COMMON_MEM_U32_PTR_TYPE)&RLength,
                           (LLDP_COMMON_MEM_U32_PTR_TYPE)&Type,
                           (LLDP_COMMON_MEM_U32_PTR_TYPE)&Length,
                           (LLDP_COMMON_MEM_PTR_TYPE    LLDP_COMMON_MEM_ATTR *) &pValue);

            if ( Response == LLDP_RSP_OK )
            {
                if (( Type   != LLDP_TLV_TYPE_TTL     ) ||
                    ( Length <  LLDP_TLV_TTL_SIZE     )) /* Note: according to spec > 2 is allowed */
                {
                    Response = LLDP_RSP_ERR_TLV_MISMATCH;
                }
                else
                {
                    *pTTL = (LSA_UINT16) (((LSA_UINT16)(*pValue) << 8 ) + *(pValue+1));
                    /* according to LLDP-Spec TTL is max 65535 */
                    if ( *pTTL > LLDP_TLV_TTL_MAX ) *pTTL = LLDP_TLV_TTL_MAX;
                }
            }
        }

        /* -------------------------------------------------------------------*/
        /* No more ChassisID,PortID and TTL shall appear and length must match*/
        /* -------------------------------------------------------------------*/

        while (( Response == LLDP_RSP_OK ) && RLength )
        {

            Response = LLDP_TLVGetCommon(
                           (LLDP_COMMON_MEM_U8_PTR_TYPE LLDP_COMMON_MEM_ATTR *) &pTLV,
                           (LLDP_COMMON_MEM_U32_PTR_TYPE)&RLength,
                           (LLDP_COMMON_MEM_U32_PTR_TYPE)&Type,
                           (LLDP_COMMON_MEM_U32_PTR_TYPE)&Length,
                           (LLDP_COMMON_MEM_PTR_TYPE    LLDP_COMMON_MEM_ATTR *) &pValue);

            if ( Response == LLDP_RSP_OK )
            {
                switch (Type)
                {
                    case LLDP_TLV_TYPE_CHASSIS_ID:
                    case LLDP_TLV_TYPE_PORT_ID:
                    case LLDP_TLV_TYPE_TTL:
                        Response = LLDP_RSP_ERR_TLV_MISMATCH;
                        break;

                    case LLDP_TLV_TYPE_PORT_DESCR:
#if LLDP_TLV_PORT_DESCR_MIN_SIZE > 0
                        if (( Length < LLDP_TLV_PORT_DESCR_MIN_SIZE   ) ||
                            ( Length > LLDP_TLV_PORT_DESCR_MAX_SIZE   ))
#else
                        if ( Length > LLDP_TLV_PORT_DESCR_MAX_SIZE   )
#endif
                        {
                            Response = LLDP_RSP_ERR_TLV_MISMATCH;
                        }
                        break;

                    case LLDP_TLV_TYPE_SYSTEM_NAME:
#if LLDP_TLV_SYSTEM_NAME_MIN_SIZE > 0
                        if (( Length < LLDP_TLV_SYSTEM_NAME_MIN_SIZE   ) ||
                            ( Length > LLDP_TLV_SYSTEM_NAME_MAX_SIZE   ))
#else
                        if ( Length > LLDP_TLV_SYSTEM_NAME_MAX_SIZE   )
#endif
                        {
                            Response = LLDP_RSP_ERR_TLV_MISMATCH;
                        }
                        break;

                    case LLDP_TLV_TYPE_SYSTEM_DESCR:
#if LLDP_TLV_SYSTEM_DESCR_MIN_SIZE > 0
                        if (( Length < LLDP_TLV_SYSTEM_DESCR_MIN_SIZE   ) ||
                            ( Length > LLDP_TLV_SYSTEM_DESCR_MAX_SIZE   ))
#else
                        if ( Length > LLDP_TLV_SYSTEM_DESCR_MAX_SIZE   )
#endif
                        {
                            Response = LLDP_RSP_ERR_TLV_MISMATCH;
                        }
                        break;

                    case LLDP_TLV_TYPE_SYSTEM_CAP:
                        if ( Length < LLDP_TLV_SYSTEM_CAP_SIZE )
                        {
                            Response = LLDP_RSP_ERR_TLV_MISMATCH;
                        }
                        break;

                    case LLDP_TLV_TYPE_MGM_ADDR:
                        /* only Mismatch error will abort */
                        if ( LLDP_TLVValidateMgmAddrLen(pValue,Length) == LLDP_RSP_ERR_TLV_MISMATCH )
                        {
                            Response = LLDP_RSP_ERR_TLV_MISMATCH;
                        }
                        break;

                    case LLDP_TLV_TYPE_ORG_SPECIFIC:
                        if (( Length < LLDP_TLV_ORG_SPECIFIC_MIN_SIZE   ) ||
                            ( Length > LLDP_TLV_ORG_SPECIFIC_MAX_SIZE   ))
                        {
                            Response = LLDP_RSP_ERR_TLV_MISMATCH;
                        }
                        else
                        {
                            LSA_UINT32  OUIValue;
                            LSA_UINT32  Subtype;

                            OUIValue  = ((LSA_UINT32)(pValue[0])) << 16;
                            OUIValue += (pValue[1]) << 8;
                            OUIValue +=  pValue[2];
                            pValue   += LLDP_TLV_ORG_SPECIFIC_OUI_SIZE;

                            Subtype   = *pValue++;

                            switch (OUIValue)
                            {
                                case LLDP_TLV_ORG_OUI_IEEE_802_1:
                                    switch ( Subtype )
                                    {
                                        case LLDP_TLV_IEEE_802_1_TYPE_PORT_VLAN_ID:
                                            if ( Length < LLDP_TLV_IEEE_802_1_PORT_VLAN_ID_SIZE )
                                            {
                                                Response = LLDP_RSP_ERR_TLV_MISMATCH;
                                            }
                                            break;
                                        case LLDP_TLV_IEEE_802_1_TYPE_PPVID:
                                            if ( Length < LLDP_TLV_IEEE_802_1_PPVID_SIZE )
                                            {
                                                Response = LLDP_RSP_ERR_TLV_MISMATCH;
                                            }
                                            break;
                                        case LLDP_TLV_IEEE_802_1_TYPE_VLAN_NAME:
                                            /* only Mismatch error will abort */
                                            if ( LLDP_TLVValidateVLANNameLen((LLDP_COMMON_MEM_U8_PTR_TYPE) pValue,Length)  == LLDP_RSP_ERR_TLV_MISMATCH )
                                            {
                                                Response = LLDP_RSP_ERR_TLV_MISMATCH;
                                            }
                                            break;
                                        case LLDP_TLV_IEEE_802_1_TYPE_PROTOCOL_IDENTITY:
                                            /* only Mismatch error will abort */
                                            if ( LLDP_TLVValidateProtIdentityLen((LLDP_COMMON_MEM_U8_PTR_TYPE) pValue,Length) == LLDP_RSP_ERR_TLV_MISMATCH )
                                            {
                                                Response = LLDP_RSP_ERR_TLV_MISMATCH;
                                            }
                                            break;
                                        default:
                                            /* unknown. no more checks */
                                            break;
                                    }
                                    break;

                                case LLDP_TLV_ORG_OUI_IEEE_802_3:
                                    switch ( Subtype )
                                    {
                                        case LLDP_TLV_IEEE_802_3_TYPE_MAC_PHY_CFG_STATUS:
                                            if ( Length < LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_SIZE )
                                            {
                                                Response = LLDP_RSP_ERR_TLV_MISMATCH;
                                            }
                                            break;
                                        case LLDP_TLV_IEEE_802_3_TYPE_POWER_VIA_MDI:
                                            if ( Length < LLDP_TLV_IEEE_802_3_POWER_VIA_MDI_SIZE )
                                            {
                                                Response = LLDP_RSP_ERR_TLV_MISMATCH;
                                            }
                                            break;
                                        case LLDP_TLV_IEEE_802_3_TYPE_LINK_AGGREGATION:
                                            if ( Length < LLDP_TLV_IEEE_802_3_LINK_AGGREGATION_SIZE )
                                            {
                                                Response = LLDP_RSP_ERR_TLV_MISMATCH;
                                            }
                                            break;
                                        case LLDP_TLV_IEEE_802_3_TYPE_MAX_FRAME_SIZE:
                                            if ( Length < LLDP_TLV_IEEE_802_3_MAX_FRAME_SIZE_SIZE )
                                            {
                                                Response = LLDP_RSP_ERR_TLV_MISMATCH;
                                            }
                                            break;
                                        default:
                                            /* unknown. no more checks */
                                            break;
                                    }
                                    break;

                                default:
                                    /* unknown. no more checks */
                                    break;
                            }
                        }
                        break;

                    case LLDP_TLV_TYPE_LLDPDU_END:  /* end of chain */
                        if (Length == LLDP_TLV_LLDPDU_END_SIZE) /* End must have a length of 0 ! */
                        {
                            /* we have a end before reaching and of frame */
                            *pNLength = TLength - RLength;
                            *pEndTLV  = LSA_TRUE;
                            RLength   = 0;              /* break loop   */
                        }
                        else
                        {
                            Response = LLDP_RSP_ERR_TLV_MISMATCH;
                        }
                        break;

                    default: /* unknown tlv */
                        if ( Length > LLDP_LEN_MAX_SIZE )
                        {
                            Response = LLDP_RSP_ERR_TLV_MISMATCH;
                        }
                        break;
                }
            }
        }
    }
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSet                                          +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_..       *ppTLV  +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE                  pTLength+*/
/*+                    LSA_UINT32                                   Type    +*/
/*+                    LSA_UINT32                                   Length  +*/
/*+                    LLDP_UPPER_MEM_PTR_TYPE                      pValue  +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  Type                 : in: Type (T) of TLV element to add              +*/
/*+  Length               : in: Length (L) of Value field of TLV element    +*/
/*+                             (maybe 0 ! )                                +*/
/*+  pValue               : in: Pointer to Value (V) of TLV element to add  +*/
/*+                             LSA_NULL if no Value (Length = 0)           +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to be pTLV    +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated.                                                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_UPPER_IN_FCT_ATTR lldp_TLVSet(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LSA_UINT32                                       Type,
    LSA_UINT32                                       Length,
    LLDP_UPPER_MEM_PTR_TYPE                          pValue)
{

    LLDP_RSP   Response;
    LSA_UINT32 MaxLength;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (
        #ifdef LLDP_CFG_UTIL_PTR_PARAM_CHECK
        (( Length != 0) && ( LSA_HOST_PTR_ARE_EQUAL(pValue,LSA_NULL))) ||
        ( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))  ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL)) ||
        #endif
        ( Length > LLDP_LEN_MAX_SIZE ))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        /* if this is not a END OF LLDPDU we must leave room for a End of LLDPDU */
        if ( Type == LLDP_TLV_TYPE_LLDPDU_END )
            MaxLength = LLDP_MAX_TLV_DATA_SIZE;
        else
            MaxLength = LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END;

        /* verify TLV fit into */
        if ( (*pTLength + Length + LLDP_TL_SIZE) > MaxLength )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {

            /* append TLV */
            LLDP_TLVSetCommon((LLDP_COMMON_MEM_U8_PTR_TYPE  LLDP_COMMON_MEM_ATTR *)ppTLV,
                              (LLDP_COMMON_MEM_U32_PTR_TYPE) pTLength,
                              Type,
                              Length,
                              (LLDP_COMMON_MEM_PTR_TYPE) pValue);
        }
    }
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVGet                                          +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER ..       *ppTLV   +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE                  pTLength+*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE                  pType   +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE                  pLength +*/
/*+                    LLDP_UPPER_MEM_PTR_TYPE    LLDP_UPPER...    *ppValue +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to read TLV element from +*/
/*+                         out:Pointer behind read TLV element (next TLV)  +*/
/*+  pTLength             : in: Length of TLV chain before TLV read         +*/
/*+                         out:remaining length of TLV-chain.              +*/
/*+  pType                : out:Type (T) of TLV element read                +*/
/*+  pLength              : out:Length (L) of TLV element read              +*/
/*+  ppValue              : out:Pointer to address of Value (V) field       +*/
/*+                                                                         +*/
/*+  Result:           LLDP_RSP_OK                                          +*/
/*+                    LLDP_RSP_ERR_PARAM                                   +*/
/*+                    LLDP_RSP_ERR_TLV_MISMATCH                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets (read) an TLV-element beginning at *ppTLV and updates+*/
/*+               pLength and ppTLV to next TLV in chain.                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVGet(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pType,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pLength,
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppValue)
{

    LLDP_RSP                    Response;

    Response = LLDP_RSP_OK;

    #ifdef LLDP_CFG_UTIL_PTR_PARAM_CHECK
    /* check parameters */
    if ( ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL))||
         ( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))   ||
         ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))  ||
         ( LSA_HOST_PTR_ARE_EQUAL(pType,LSA_NULL))   ||
         ( LSA_HOST_PTR_ARE_EQUAL(pLength,LSA_NULL)) ||
         ( LSA_HOST_PTR_ARE_EQUAL(ppValue,LSA_NULL)))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
        #endif
    {
        /* read TLV */
        Response = LLDP_TLVGetCommon(
                       (LLDP_COMMON_MEM_U8_PTR_TYPE LLDP_COMMON_MEM_ATTR *)ppTLV,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE)  pTLength,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE)  pType,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE)  pLength,
                       (LLDP_COMMON_MEM_PTR_TYPE    LLDP_COMMON_MEM_ATTR *)ppValue);
    }
    return(Response);
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetChassisPortTTL                            +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR *ppTLV   +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE                  pTLength+*/
/*+                    LLDP_TLV_SET_INFO_TYPE  LLDP_UPPER_MEM_ATTR *pTLVInfo+*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : out:Pointer to TLV chain length                 +*/
/*+  pTLVInfo             : in: Pointer to TLVInfo structure with TLVs      +*/
/*+                             to put into *ppTLV.                         +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds the TLVs from pTLVInfo into the ppTLV.               +*/
/*+               These must be the first TLVs within a LLDP-PDU            +*/
/*+                                                                         +*/
/*+               ppTLV must have room for the TLVs added.                  +*/
/*+               The max length of all TLVs is LLDP_MAX_TLV_DATA_SIZE.     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_UPPER_IN_FCT_ATTR lldp_TLVSetChassisPortTTL(
    LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                  pTLength,
    LLDP_TLV_SET_INFO_TYPE  LLDP_UPPER_MEM_ATTR *pTLVInfo)
{

    LLDP_RSP                    Response;

    Response = LLDP_RSP_OK;

    /* check parameters */
    #ifdef LLDP_CFG_UTIL_PTR_PARAM_CHECK
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVInfo,LSA_NULL)))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
        #endif
    {

        /* set TLV */
        Response = LLDP_TLVSetChassisPortTTLCommon(
                       (LLDP_COMMON_MEM_PTR_TYPE LLDP_COMMON_MEM_ATTR *)ppTLV,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE)  pTLength,
                       (LLDP_TLV_SET_INFO_TYPE LLDP_COMMON_MEM_ATTR *) pTLVInfo);
    }
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVGetChassisPortTTL                            +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR *ppTLV   +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE                  pTLength+*/
/*+                    LLDP_TLV_GET_INFO_TYPE  LLDP_UPPER_MEM_ATTR *pTLVInfo+*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in :Pointer to TLV chain length                 +*/
/*+                         out:Pointer to remaining TLV chain length       +*/
/*+  pTLVInfo             : in: Pointer to TLVInfo structure with TLVs      +*/
/*+                             read from chain on success.                 +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads the TLVs from the TLV chain into the TLVInfo        +*/
/*+                                                                         +*/
/*+               The TLVs must be the first TLVs within the chain and      +*/
/*+               in the correct order (Chassis,Port,TTL). If not we        +*/
/*+               set LLDP_RSP_ERR_TLV_MISMATCH.                            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_UPPER_IN_FCT_ATTR lldp_TLVGetChassisPortTTL(
    LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                  pTLength,
    LLDP_TLV_GET_INFO_TYPE  LLDP_UPPER_MEM_ATTR *pTLVInfo)
{

    LLDP_RSP                    Response;


    Response = LLDP_RSP_OK;

    /* check parameters */
    #ifdef LLDP_CFG_UTIL_PTR_PARAM_CHECK
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVInfo,LSA_NULL)))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
        #endif
    {
        /* read TLV */
        Response = LLDP_TLVGetChassisPortTTLCommon(
                       (LLDP_COMMON_MEM_PTR_TYPE LLDP_COMMON_MEM_ATTR *)ppTLV,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE)  pTLength,
                       (LLDP_TLV_GET_INFO_TYPE   LLDP_COMMON_MEM_ATTR *) pTLVInfo);
    }
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetEnd                                       +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_LLDPDU_END                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetEnd(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = LLDP_TLV_LLDPDU_END_SIZE;
        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_LLDPDU_END,VLength);
            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetChassisID                                 +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_UPPER_PARAMS_CHASSIS_ID_PTR_TYPE     pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pTLVParams           : in: Pointer to TLV parameters for TLV to add    +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_CHASSIS_ID                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetChassisID(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_CHASSIS_ID_PTR_TYPE            pTLVParams)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL))                  ||
        ( pTLVParams->ChassisIDLen < LLDP_MIN_CHASSIS_ID_STRING_SIZE )  ||
        ( pTLVParams->ChassisIDLen > LLDP_MAX_CHASSIS_ID_STRING_SIZE ))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = pTLVParams->ChassisIDLen + LLDP_TLV_CHASSIS_ID_SUBTYPE_SIZE;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )

        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_CHASSIS_ID,VLength);

            *pTLV++ = pTLVParams->ChassisIDSubtype;

            if (pTLVParams->ChassisIDLen)
            {
                LLDP_MEMCOPY_COMMON(
                    pTLV,
                    pTLVParams->ChassisIDString,
                    pTLVParams->ChassisIDLen);

                pTLV += pTLVParams->ChassisIDLen;
            }
            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetPortID                                    +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_UPPER_PARAMS_PORT_ID_PTR_TYPE        pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pTLVParams           : in: Pointer to TLV parameters for TLV to add    +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_PORT_ID                           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetPortID(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_PORT_ID_PTR_TYPE               pTLVParams)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL))            ||
        ( pTLVParams->PortIDLen < LLDP_MIN_PORT_ID_STRING_SIZE  ) ||
        ( pTLVParams->PortIDLen > LLDP_MAX_PORT_ID_STRING_SIZE  ))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = pTLVParams->PortIDLen + LLDP_TLV_PORT_ID_SUBTYPE_SIZE;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_PORT_ID,VLength);

            *pTLV++ = pTLVParams->PortIDSubtype;

            if (pTLVParams->PortIDLen)
            {
                LLDP_MEMCOPY_COMMON(
                    pTLV,
                    pTLVParams->PortIDString,
                    pTLVParams->PortIDLen);

                pTLV += pTLVParams->PortIDLen;
            }

            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetTTL                                       +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_UPPER_PARAMS_TTL_PTR_TYPE            pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pTLVParams           : in: Pointer to TLV parameters for TLV to add    +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_TTL                               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetTTL(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_TTL_PTR_TYPE                   pTLVParams)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL)))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = LLDP_TLV_TTL_SIZE;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_TTL,VLength);
            LLDP_SET_U16_INCR(pTLV,pTLVParams->TimeToLive);
            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetPortDescr                                 +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_UPPER_PARAMS_PORT_DESCR_PTR_TYPE     pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pTLVParams           : in: Pointer to TLV parameters for TLV to add    +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_PORT_DESCR                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetPortDescr(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_PORT_DESCR_PTR_TYPE            pTLVParams)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL))                    ||
#if LLDP_TLV_PORT_DESCR_MIN_SIZE > 0
        ( pTLVParams->PortDescriptionLen < LLDP_TLV_PORT_DESCR_MIN_SIZE ) ||
#endif
        ( pTLVParams->PortDescriptionLen > LLDP_TLV_PORT_DESCR_MAX_SIZE ))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = pTLVParams->PortDescriptionLen;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END  )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_PORT_DESCR,VLength);

            if (pTLVParams->PortDescriptionLen)
            {
                LLDP_MEMCOPY_COMMON(
                    pTLV,
                    pTLVParams->PortDescription,
                    pTLVParams->PortDescriptionLen);
                pTLV += pTLVParams->PortDescriptionLen;
            }
            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetSystemName                                +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_UPPER_PARAMS_SYSTEM_NAME_PTR_TYPE    pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pTLVParams           : in: Pointer to TLV parameters for TLV to add    +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_SYSTEM_NAME                       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetSystemName(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_SYSTEM_NAME_PTR_TYPE           pTLVParams)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL))                 ||
#if LLDP_TLV_SYSTEM_NAME_MIN_SIZE > 0
        ( pTLVParams->SystemNameLen < LLDP_TLV_SYSTEM_NAME_MIN_SIZE  ) ||
#endif
        ( pTLVParams->SystemNameLen > LLDP_TLV_SYSTEM_NAME_MAX_SIZE  ))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = pTLVParams->SystemNameLen;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_SYSTEM_NAME,VLength);

            if (pTLVParams->SystemNameLen)
            {
                LLDP_MEMCOPY_COMMON(
                    pTLV,
                    pTLVParams->SystemName,
                    pTLVParams->SystemNameLen);

                pTLV += pTLVParams->SystemNameLen;
            }
            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetSystemDescr                               +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_UPPER_PARAMS_SYSTEM_DESCR_PTR_TYPE   pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pTLVParams           : in: Pointer to TLV parameters for TLV to add    +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_SYSTEM_DESCR                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetSystemDescr(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_SYSTEM_DESCR_PTR_TYPE          pTLVParams)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL))                  ||
#if LLDP_TLV_SYSTEM_DESCR_MIN_SIZE > 0
        ( pTLVParams->SystemDescrLen < LLDP_TLV_SYSTEM_DESCR_MIN_SIZE ) ||
#endif
        ( pTLVParams->SystemDescrLen > LLDP_TLV_SYSTEM_DESCR_MAX_SIZE ))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = pTLVParams->SystemDescrLen;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_SYSTEM_DESCR,VLength);

            if (pTLVParams->SystemDescrLen)
            {
                LLDP_MEMCOPY_COMMON(
                    pTLV,
                    pTLVParams->SystemDescr,
                    pTLVParams->SystemDescrLen);

                pTLV += pTLVParams->SystemDescrLen;
            }

            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetSystemCap                                 +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_UPPER_PARAMS_SYSTEM_CAP_PTR_TYPE     pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pTLVParams           : in: Pointer to TLV parameters for TLV to add    +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_SYSTEM_CAP                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetSystemCap(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_SYSTEM_CAP_PTR_TYPE            pTLVParams)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL)))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = LLDP_TLV_SYSTEM_CAP_SIZE;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_SYSTEM_CAP,VLength);

            LLDP_SET_U16_INCR(pTLV,pTLVParams->SystemCapabilities);
            LLDP_SET_U16_INCR(pTLV,pTLVParams->EnabledCapabilities);

            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetMgmAddr                                   +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_UPPER_PARAMS_MGM_ADDR_PTR_TYPE       pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pTLVParams           : in: Pointer to TLV parameters for TLV to add    +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_MGM_ADDR                          +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetMgmAddr(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_MGM_ADDR_PTR_TYPE              pTLVParams)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL))                                 ||
        ( pTLVParams->MgmtAddrStringLen < LLDP_TLV_MGM_ADDR_MIN_MGMT_ADDR_STRING_LEN ) ||
        ( pTLVParams->MgmtAddrStringLen > LLDP_TLV_MGM_ADDR_MAX_MGMT_ADDR_STRING_LEN ) ||
#if LLDP_TLV_MGM_ADDR_MIN_OID_STRING_LEN > 0
        ( pTLVParams->OIDStringLen      < LLDP_TLV_MGM_ADDR_MIN_OID_STRING_LEN       ) ||
#endif
        ( pTLVParams->OIDStringLen      > LLDP_TLV_MGM_ADDR_MAX_OID_STRING_LEN       ))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = LLDP_TLV_MGM_ADDR_MGMT_ADDR_STRING_LENGTH_SIZE +
                  pTLVParams->MgmtAddrStringLen                  +
                  LLDP_TLV_MGM_ADDR_IFNUM_SUBTYPE_SIZE           +
                  LLDP_TLV_MGM_ADDR_IFNUM_SIZE                   +
                  LLDP_TLV_MGM_ADDR_OI_STRING_LENGTH_SIZE        +
                  pTLVParams->OIDStringLen;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END    )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_MGM_ADDR,VLength);

            *pTLV++ = pTLVParams->MgmtAddrStringLen;
            *pTLV++ = pTLVParams->MgmtAddrSubtype;

            if (pTLVParams->MgmtAddrStringLen-LLDP_TLV_MGM_ADDR_MGMT_ADDR_SUBTYPE_SIZE)
            {
                LLDP_MEMCOPY_COMMON(
                    pTLV,
                    pTLVParams->MgmtAddress,
                    pTLVParams->MgmtAddrStringLen-LLDP_TLV_MGM_ADDR_MGMT_ADDR_SUBTYPE_SIZE);

                pTLV += pTLVParams->MgmtAddrStringLen-LLDP_TLV_MGM_ADDR_MGMT_ADDR_SUBTYPE_SIZE;
            }

            *pTLV++ = pTLVParams->InterfaceNumberingSubtype;
            LLDP_SET_U32_INCR(pTLV,pTLVParams->InterfaceNumber);
            *pTLV++ = pTLVParams->OIDStringLen;

            if (pTLVParams->OIDStringLen)
            {
                LLDP_MEMCOPY_COMMON(
                    pTLV,
                    pTLVParams->OIDIdentifier,
                    pTLVParams->OIDStringLen);

                pTLV += pTLVParams->OIDStringLen;
            }

            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetPortVLANID                                +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_UPPER_PARAMS_PORT_VLAN_ID_PTR_TYPE   pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pTLVParams           : in: Pointer to TLV parameters for TLV to add    +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_ORG_SPECIFIC                      +*/
/*+                    OUI: IEEE 802.1                                      +*/
/*+                Subtype: LLDP_TLV_IEEE_802_1_TYPE_PORT_VLAN_ID           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetPortVLANID(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_PORT_VLAN_ID_PTR_TYPE          pTLVParams)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL)))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = LLDP_TLV_IEEE_802_1_PORT_VLAN_ID_SIZE;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END    )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_ORG_SPECIFIC,VLength);

            LLDP_SET_OUI_GROUP_FIELD_INCR(pTLV,LLDP_TLV_ORG_OUI_IEEE_802_1_1,
                                          LLDP_TLV_ORG_OUI_IEEE_802_1_2,
                                          LLDP_TLV_ORG_OUI_IEEE_802_1_3,
                                          LLDP_TLV_IEEE_802_1_TYPE_PORT_VLAN_ID);

            LLDP_SET_U16_INCR(pTLV,pTLVParams->PortVLANIdentifier);
            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetPPVID                                     +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_UPPER_PARAMS_PPVID_PTR_TYPE          pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pTLVParams           : in: Pointer to TLV parameters for TLV to add    +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_ORG_SPECIFIC                      +*/
/*+                    OUI: IEEE 802.1                                      +*/
/*+                Subtype: LLDP_TLV_IEEE_802_1_TYPE_PPVID                  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetPPVID(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_PPVID_PTR_TYPE                 pTLVParams)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL)))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = LLDP_TLV_IEEE_802_1_PPVID_SIZE;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END    )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_ORG_SPECIFIC,VLength);

            LLDP_SET_OUI_GROUP_FIELD_INCR(pTLV,LLDP_TLV_ORG_OUI_IEEE_802_1_1,
                                          LLDP_TLV_ORG_OUI_IEEE_802_1_2,
                                          LLDP_TLV_ORG_OUI_IEEE_802_1_3,
                                          LLDP_TLV_IEEE_802_1_TYPE_PPVID);

            *pTLV++ = pTLVParams->Flags;
            LLDP_SET_U16_INCR(pTLV,pTLVParams->ReferenceNumber);

            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetVLANName                                  +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_UPPER_PARAMS_VLAN_NAME_PTR_TYPE      pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pTLVParams           : in: Pointer to TLV parameters for TLV to add    +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_ORG_SPECIFIC                      +*/
/*+                    OUI: IEEE 802.1                                      +*/
/*+                Subtype: LLDP_TLV_IEEE_802_1_TYPE_VLAN_NAME              +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetVLANName(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_VLAN_NAME_PTR_TYPE             pTLVParams)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL))                                        ||
        ( pTLVParams->VLANNameLength < LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MIN_VLAN_NAME_LEN ) ||
        ( pTLVParams->VLANNameLength > LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MAX_VLAN_NAME_LEN ))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MIN_SIZE +
                  pTLVParams->VLANNameLength - LLDP_TLV_IEEE_802_1_PORT_VLAN_NAME_MIN_VLAN_NAME_LEN;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_ORG_SPECIFIC,VLength);

            LLDP_SET_OUI_GROUP_FIELD_INCR(pTLV,LLDP_TLV_ORG_OUI_IEEE_802_1_1,
                                          LLDP_TLV_ORG_OUI_IEEE_802_1_2,
                                          LLDP_TLV_ORG_OUI_IEEE_802_1_3,
                                          LLDP_TLV_IEEE_802_1_TYPE_VLAN_NAME);

            LLDP_SET_U16_INCR(pTLV,pTLVParams->VLANID);
            *pTLV++ = pTLVParams->VLANNameLength;

            if (pTLVParams->VLANNameLength)
            {
                LLDP_MEMCOPY_COMMON(
                    pTLV,
                    pTLVParams->VLANName,
                    pTLVParams->VLANNameLength);

                pTLV += pTLVParams->VLANNameLength;
            }

            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetProtIdentity                              +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_UPPER_PARAMS_PROTOCOL_IDENTITY_TYPE  pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pTLVParams           : in: Pointer to TLV parameters for TLV to add    +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_ORG_SPECIFIC                      +*/
/*+                    OUI: IEEE 802.1                                      +*/
/*+                Subtype: LLDP_TLV_IEEE_802_1_TYPE_PROTOCOL_IDENTITY      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetProtIdentity(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_PARAMS_PROTOCOL_IDENTITY_TYPE         pTLVParams)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL))
#if LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MIN_LEN > 0
         || ( pTLVParams->Length < LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MIN_LEN ) 
#endif
#if LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MAX_LEN < 255
         || ( pTLVParams->Length > LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MAX_LEN )
#endif
        )
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MIN_SIZE +
                  pTLVParams->Length - LLDP_TLV_IEEE_802_1_PROTOCOL_IDENTITY_MIN_LEN;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_ORG_SPECIFIC,VLength);

            LLDP_SET_OUI_GROUP_FIELD_INCR(pTLV,LLDP_TLV_ORG_OUI_IEEE_802_1_1,
                                          LLDP_TLV_ORG_OUI_IEEE_802_1_2,
                                          LLDP_TLV_ORG_OUI_IEEE_802_1_3,
                                          LLDP_TLV_IEEE_802_1_TYPE_PROTOCOL_IDENTITY);

            *pTLV++ = pTLVParams->Length;

            if (pTLVParams->Length)
            {
                LLDP_MEMCOPY_COMMON(
                    pTLV,
                    pTLVParams->Identity,
                    pTLVParams->Length);

                pTLV += pTLVParams->Length;
            }

            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetMACPHYConfigStatus                        +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_... _MAC_PHY_CONFIG_STATUS_PTR_TYPE  pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pTLVParams           : in: Pointer to TLV parameters for TLV to add    +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_ORG_SPECIFIC                      +*/
/*+                    OUI: IEEE 802.3                                      +*/
/*+                Subtype: LLDP_TLV_IEEE_802_3_TYPE_MAC_PHY_CFG_STATUS     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetMACPHYConfigStatus(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR  *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                       pTLength,
    LLDP_UPPER_PARAMS_MAC_PHY_CONFIG_STATUS_PTR_TYPE  pTLVParams)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL)))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_SIZE;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_ORG_SPECIFIC,VLength);

            LLDP_SET_OUI_GROUP_FIELD_INCR(pTLV,LLDP_TLV_ORG_OUI_IEEE_802_3_1,
                                          LLDP_TLV_ORG_OUI_IEEE_802_3_2,
                                          LLDP_TLV_ORG_OUI_IEEE_802_3_3,
                                          LLDP_TLV_IEEE_802_3_TYPE_MAC_PHY_CFG_STATUS);

            *pTLV++ = pTLVParams->AutonegSupportStatus;
            LLDP_SET_U16_INCR(pTLV,pTLVParams->PMDAutonegAdvertisedCap);
            LLDP_SET_U16_INCR(pTLV,pTLVParams->OperationalMAUType);

            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetPowerViaMDI                               +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_UPPER_PARAMS_POWER_VIA_MDI_PTR_TYPE  pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pTLVParams           : in: Pointer to TLV parameters for TLV to add    +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_ORG_SPECIFIC                      +*/
/*+                    OUI: IEEE 802.3                                      +*/
/*+                Subtype: LLDP_TLV_IEEE_802_3_TYPE_POWER_VIA_MDI          +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetPowerViaMDI(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR  *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                       pTLength,
    LLDP_UPPER_PARAMS_POWER_VIA_MDI_PTR_TYPE          pTLVParams)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL)))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = LLDP_TLV_IEEE_802_3_POWER_VIA_MDI_SIZE;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_ORG_SPECIFIC,VLength);

            LLDP_SET_OUI_GROUP_FIELD_INCR(pTLV,LLDP_TLV_ORG_OUI_IEEE_802_3_1,
                                          LLDP_TLV_ORG_OUI_IEEE_802_3_2,
                                          LLDP_TLV_ORG_OUI_IEEE_802_3_3,
                                          LLDP_TLV_IEEE_802_3_TYPE_POWER_VIA_MDI);

            *pTLV++ = pTLVParams->MDIPowerSupport;
            *pTLV++ = pTLVParams->PowerPair;
            *pTLV++ = pTLVParams->PowerClass;

            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetLinkAgg                                   +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_UPPER_..._LINK_AGGREGATION_PTR_TYPE  pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pTLVParams           : in: Pointer to TLV parameters for TLV to add    +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_ORG_SPECIFIC                      +*/
/*+                    OUI: IEEE 802.3                                      +*/
/*+                Subtype: LLDP_TLV_IEEE_802_3_TYPE_LINK_AGGREGATION       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetLinkAgg(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR  *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                       pTLength,
    LLDP_UPPER_PARAMS_LINK_AGGREGATION_PTR_TYPE       pTLVParams)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL)))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = LLDP_TLV_IEEE_802_3_LINK_AGGREGATION_SIZE;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_ORG_SPECIFIC,VLength);

            LLDP_SET_OUI_GROUP_FIELD_INCR(pTLV,LLDP_TLV_ORG_OUI_IEEE_802_3_1,
                                          LLDP_TLV_ORG_OUI_IEEE_802_3_2,
                                          LLDP_TLV_ORG_OUI_IEEE_802_3_3,
                                          LLDP_TLV_IEEE_802_3_TYPE_LINK_AGGREGATION);

            *pTLV++ = pTLVParams->LinkAggregationStatus;
            LLDP_SET_U32_INCR(pTLV,pTLVParams->AggregatedPortID);

            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVSetMaxFrameSize                              +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE  LLDP_UPPER_.    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_UPPER_PARAMS_MAX_FRAME_SIZE_PTR_TYPE pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to put TLV element to.   +*/
/*+                         out:Pointer behind inserted TLV element.        +*/
/*+  pTLength             : in: Length of TLV chain before adding TLV       +*/
/*+                         out:Length of TLV chain after adding TLV        +*/
/*+  pTLVParams           : in: Pointer to TLV parameters for TLV to add    +*/
/*+                                                                         +*/
/*+  Result: LLDP_RSP_OK                                                    +*/
/*+          LLDP_RSP_ERR_PARAM                                             +*/
/*+          LLDP_RSP_ERR_TLV_MISMATCH                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Adds a TLV element to the TLV-chain pointed to by ppTLV   +*/
/*+               On success the TLV is added and ppTLV and pLength is      +*/
/*+               updated. pTLVParams points to a strucure with all         +*/
/*+               TLV parameters                                            +*/
/*+                                                                         +*/
/*+               TLV-Type: LLDP_TLV_TYPE_ORG_SPECIFIC                      +*/
/*+                    OUI: IEEE 802.3                                      +*/
/*+                Subtype: LLDP_TLV_IEEE_802_3_TYPE_MAX_FRAME_SIZE         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVSetMaxFrameSize(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR  *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                       pTLength,
    LLDP_UPPER_PARAMS_MAX_FRAME_SIZE_PTR_TYPE         pTLVParams)
{

    LLDP_RSP                    Response;
    LSA_UINT32                  VLength;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pTLV;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(*ppTLV,LSA_NULL))   ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL)))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        pTLV    = (LLDP_UPPER_MEM_U8_PTR_TYPE)*ppTLV;
        VLength = LLDP_TLV_IEEE_802_3_MAX_FRAME_SIZE_SIZE;

        /* verify TLV fit into */
        if ( *pTLength + VLength + LLDP_TL_SIZE > LLDP_MAX_TLV_DATA_SIZE_WITHOUT_END )
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        else
        {
            LLDP_SET_TL_FIELD_INCR(pTLV,LLDP_TLV_TYPE_ORG_SPECIFIC,VLength);

            LLDP_SET_OUI_GROUP_FIELD_INCR(pTLV,LLDP_TLV_ORG_OUI_IEEE_802_3_1,
                                          LLDP_TLV_ORG_OUI_IEEE_802_3_2,
                                          LLDP_TLV_ORG_OUI_IEEE_802_3_3,
                                          LLDP_TLV_IEEE_802_3_TYPE_MAX_FRAME_SIZE);

            LLDP_SET_U16_INCR(pTLV,pTLVParams->MaxFrameSize);

            *pTLength += LLDP_TL_SIZE + VLength;
            *ppTLV     = pTLV;
        }
    }
    return(Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      lldp_TLVGetParams                                    +*/
/*+  Input/Output      LLDP_UPPER_MEM_PTR_TYPE LLDP_UPPER ..    *ppTLV      +*/
/*+                    LLDP_UPPER_MEM_U32_PTR_TYPE               pTLength   +*/
/*+                    LLDP_UPPER_TLV_GET_EX_PARAMS_PTR_TYPE     pTLVParams +*/
/*+                                                                         +*/
/*+  Result            LLDP_RSP                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppTLV                : in: Pointer to address to TLV element to analyse+*/
/*+                         out:Pointer behind TLV analysed (next TLV)      +*/
/*+  pTLength             : in: Length of TLV chain before TLV read         +*/
/*+                         out:remaining length of TLV-chain.              +*/
/*+  pTLVParams           : out:Filled structure with TLV-Parameters        +*/
/*+                                                                         +*/
/*+  Result:           LLDP_RSP_OK                                          +*/
/*+                    LLDP_RSP_ERR_PARAM                                   +*/
/*+                    LLDP_RSP_ERR_TLV_MISMATCH                            +*/
/*+                    LLDP_RSP_ERR_TLV_PARAM_ERROR                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets (read) Parameters from a TLV-element beginning at    +*/
/*+               *ppTLV and updates pTLVParams with the TLV specific       +*/
/*+               parameters. *ppTLV and pTLength will be modified so they  +*/
/*+               can be used for further calls.                            +*/
/*+                                                                         +*/
/*+               If we find a not supported TLV -Type we set               +*/
/*+               LLDP_RSP_ERR_TLV_UNKNOWN but update                       +*/
/*+               ppTLV and pTLength, so the TLV can be skipped.            +*/
/*+               If we find a not supported ORG-specific TLV we fill the   +*/
/*+               structure pTLVParams->Type = OrgUnknown and set OK.       +*/
/*+                                                                         +*/
/*+               If we find a parameter error within an TLV we set         +*/
/*+               LLDP_RSP_ERR_TLV_PARAM_ERROR but we update                +*/
/*+               ppTLV and pTLength, so the TLV can be skipped.            +*/
/*+                                                                         +*/
/*+               LLDP_RSP_ERR_TLV_MISMATCH is set if the TLV-chain is      +*/
/*+               corrupted or a TLV- total length is less then             +*/
/*+               the sum of contained TLV-Elements. No further             +*/
/*+               processing possible. User should discard the whole TLV    +*/
/*+                                                                         +*/
/*+               If pTLength returns 0, no more TLVs present.              +*/
/*+               Function should be called till pTLength is 0 or till      +*/
/*+               a END of LLDPDU-TLV is returned.                          +*/
/*+                                                                         +*/
/*+        Note:  To validate a TLV-Chain the use can call this function    +*/
/*+               until it reaches the END-TLV or pTLength gets 0           +*/
/*+               (Not that a valid TLV must contain a END-TLV!) without    +*/
/*+               storing the parameters.                                   +*/
/*+               If all was ok the use can start over and store each       +*/
/*+               TLVs parameter for futher processing (knowing the TLV     +*/
/*+               chain is valid)                                           +*/
/*+                                                                         +*/
/*+        Note:  The LLDP-component still does some basic validation on    +*/
/*+               TLV-reception (e.g. checks for mandatory TLVs and         +*/
/*+               validness!). Optional TLVs are not validated on reception!+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR lldp_TLVGetParams(
    LLDP_UPPER_MEM_PTR_TYPE     LLDP_UPPER_MEM_ATTR *ppTLV,
    LLDP_UPPER_MEM_U32_PTR_TYPE                      pTLength,
    LLDP_UPPER_TLV_GET_EX_PARAMS_PTR_TYPE            pTLVParams)
{

    LLDP_RSP                    Response;
    LLDP_UPPER_MEM_PTR_TYPE     pTLV;
    LLDP_UPPER_MEM_U8_PTR_TYPE  pValue;
    LSA_UINT32                  Length;
    LSA_UINT32                  Type;
    LSA_UINT32                  TLength;

    Response = LLDP_RSP_OK;

    /* check parameters */
    if (( LSA_HOST_PTR_ARE_EQUAL(ppTLV,LSA_NULL))    ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLength,LSA_NULL)) ||
        ( LSA_HOST_PTR_ARE_EQUAL(pTLVParams,LSA_NULL)))
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {

        pTLV    = *ppTLV;
        TLength = *pTLength;

        /* read TLV */
        Response = LLDP_TLVGetCommon(
                       (LLDP_COMMON_MEM_U8_PTR_TYPE LLDP_COMMON_MEM_ATTR *)&pTLV,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE)  &TLength,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE)  &Type,
                       (LLDP_COMMON_MEM_U32_PTR_TYPE)  &Length,
                       (LLDP_COMMON_MEM_PTR_TYPE    LLDP_COMMON_MEM_ATTR *)&pValue);

        if ( Response == LLDP_RSP_OK )
        {

            switch ( Type )
            {
                    /*---------------------------------------------------------------*/
                case LLDP_TLV_TYPE_LLDPDU_END:
                    /*---------------------------------------------------------------*/
                    pTLVParams->Type = LLDP_PARAM_TYPE_LLDPU_END;

                    if ( Length != LLDP_TLV_LLDPDU_END_SIZE ) /* END must have length of 0 !*/
                    {
                        Response = LLDP_RSP_ERR_TLV_MISMATCH;
                    }
                    break;

                    /*---------------------------------------------------------------*/
                case LLDP_TLV_TYPE_CHASSIS_ID:
                    /*---------------------------------------------------------------*/
                    pTLVParams->Type = LLDP_PARAM_TYPE_CHASSIS_ID;

                    if (( Length >= LLDP_TLV_CHASSIS_ID_MIN_SIZE ) &&
                        ( Length <= LLDP_TLV_CHASSIS_ID_SUBTYPE_SIZE + LLDP_MAX_CHASSIS_ID_STRING_SIZE ))
                    {
                        pTLVParams->Params.ChassisID.ChassisIDSubtype = *pValue++;
                        pTLVParams->Params.ChassisID.ChassisIDLen     = (LSA_UINT16) (Length - LLDP_TLV_CHASSIS_ID_SUBTYPE_SIZE);

                        LLDP_MEMCOPY_COMMON( (LLDP_COMMON_MEM_PTR_TYPE) pTLVParams->Params.ChassisID.ChassisIDString,
                                             pValue,
                                             Length);

                    }
                    else
                    {
                        Response = LLDP_RSP_ERR_TLV_MISMATCH;
                    }
                    break;

                    /*---------------------------------------------------------------*/
                case LLDP_TLV_TYPE_PORT_ID:
                    /*---------------------------------------------------------------*/
                    pTLVParams->Type = LLDP_PARAM_TYPE_PORT_ID;

                    if (( Length >= LLDP_TLV_PORT_ID_MIN_SIZE ) &&
                        ( Length <= LLDP_TLV_PORT_ID_SUBTYPE_SIZE + LLDP_MAX_PORT_ID_STRING_SIZE ))
                    {
                        pTLVParams->Params.PortID.PortIDSubtype = *pValue++;
                        pTLVParams->Params.PortID.PortIDLen     = (LSA_UINT16)(Length - LLDP_TLV_PORT_ID_SUBTYPE_SIZE);

                        LLDP_MEMCOPY_COMMON( (LLDP_COMMON_MEM_PTR_TYPE) pTLVParams->Params.PortID.PortIDString,
                                             pValue,
                                             Length);

                    }
                    else
                    {
                        Response = LLDP_RSP_ERR_TLV_MISMATCH;
                    }
                    break;

                    /*---------------------------------------------------------------*/
                case LLDP_TLV_TYPE_TTL:
                    /*---------------------------------------------------------------*/
                    pTLVParams->Type = LLDP_PARAM_TYPE_TTL;

                    if ( Length >= LLDP_TLV_TTL_SIZE )
                    {
                        pTLVParams->Params.TTL.TimeToLive = LLDP_GET_U16(pValue);
                    }
                    else
                    {
                        Response = LLDP_RSP_ERR_TLV_MISMATCH;
                    }
                    break;

                    /*---------------------------------------------------------------*/
                case LLDP_TLV_TYPE_PORT_DESCR:
                    /*---------------------------------------------------------------*/
                    pTLVParams->Type = LLDP_PARAM_TYPE_PORT_DESCR;
#if LLDP_TLV_PORT_DESCR_MIN_SIZE > 0
                    if (( Length >= LLDP_TLV_PORT_DESCR_MIN_SIZE ) &&
                        ( Length <= LLDP_TLV_PORT_DESCR_MAX_SIZE ))
#else
                    if ( Length <= LLDP_TLV_PORT_DESCR_MAX_SIZE )
#endif
                    {
                        pTLVParams->Params.PortDescr.PortDescriptionLen  = (LSA_UINT16) (Length);

                        LLDP_MEMCOPY_COMMON( (LLDP_COMMON_MEM_PTR_TYPE) pTLVParams->Params.PortDescr.PortDescription,
                                             pValue,
                                             Length);

                    }
                    else
                    {
                        Response = LLDP_RSP_ERR_TLV_MISMATCH;
                    }


                    break;

                    /*---------------------------------------------------------------*/
                case LLDP_TLV_TYPE_SYSTEM_NAME:
                    /*---------------------------------------------------------------*/
                    pTLVParams->Type = LLDP_PARAM_TYPE_SYSTEM_NAME;
#if LLDP_TLV_SYSTEM_NAME_MIN_SIZE > 0
                    if (( Length >= LLDP_TLV_SYSTEM_NAME_MIN_SIZE ) &&
                        ( Length <= LLDP_TLV_SYSTEM_NAME_MAX_SIZE  ))
#else
                    if ( Length <= LLDP_TLV_SYSTEM_NAME_MAX_SIZE  )
#endif
                    {
                        pTLVParams->Params.SystemName.SystemNameLen  = (LSA_UINT16) (Length);

                        LLDP_MEMCOPY_COMMON( (LLDP_COMMON_MEM_PTR_TYPE) pTLVParams->Params.SystemName.SystemName,
                                             pValue,
                                             Length);

                    }
                    else
                    {
                        Response = LLDP_RSP_ERR_TLV_MISMATCH;
                    }

                    break;

                    /*---------------------------------------------------------------*/
                case LLDP_TLV_TYPE_SYSTEM_DESCR:
                    /*---------------------------------------------------------------*/
                    pTLVParams->Type = LLDP_PARAM_TYPE_SYSTEM_DESCR;
#if LLDP_TLV_SYSTEM_DESCR_MIN_SIZE > 0
                    if (( Length >= LLDP_TLV_SYSTEM_DESCR_MIN_SIZE ) &&
                        ( Length <= LLDP_TLV_SYSTEM_DESCR_MAX_SIZE  ))
#else
                    if ( Length <= LLDP_TLV_SYSTEM_DESCR_MAX_SIZE  )
#endif
                    {
                        pTLVParams->Params.SystemDescr.SystemDescrLen  = (LSA_UINT16) (Length);

                        LLDP_MEMCOPY_COMMON( (LLDP_COMMON_MEM_PTR_TYPE) pTLVParams->Params.SystemDescr.SystemDescr,
                                             pValue,
                                             Length);

                    }
                    else
                    {
                        Response = LLDP_RSP_ERR_TLV_MISMATCH;
                    }

                    break;

                    /*---------------------------------------------------------------*/
                case LLDP_TLV_TYPE_SYSTEM_CAP:
                    /*---------------------------------------------------------------*/
                    pTLVParams->Type = LLDP_PARAM_TYPE_SYSTEM_CAP;

                    if ( Length >= LLDP_TLV_SYSTEM_CAP_SIZE )
                    {
                        pTLVParams->Params.SystemCap.SystemCapabilities   = LLDP_GET_U16(pValue);
                        pValue += 2;
                        pTLVParams->Params.SystemCap.EnabledCapabilities  = LLDP_GET_U16(pValue);
                    }
                    else
                    {
                        Response = LLDP_RSP_ERR_TLV_MISMATCH;
                    }


                    break;

                    /*---------------------------------------------------------------*/
                case LLDP_TLV_TYPE_MGM_ADDR:
                    /*---------------------------------------------------------------*/
                    pTLVParams->Type = LLDP_PARAM_TYPE_MGM_ADDR;

                    /* Validate Length fields */
                    Response = LLDP_TLVValidateMgmAddrLen((LLDP_COMMON_MEM_U8_PTR_TYPE) pValue,Length);

                    if ( Response == LLDP_RSP_OK)
                    {
                        pTLVParams->Params.MgmAddr.MgmtAddrStringLen = *pValue++;
                        pTLVParams->Params.MgmAddr.MgmtAddrSubtype   = *pValue++;

                        if ( pTLVParams->Params.MgmAddr.MgmtAddrStringLen-LLDP_TLV_MGM_ADDR_MGMT_ADDR_SUBTYPE_SIZE)
                        {
                            LLDP_MEMCOPY_COMMON( (LLDP_COMMON_MEM_PTR_TYPE) pTLVParams->Params.MgmAddr.MgmtAddress,
                                                 pValue,
                                                 pTLVParams->Params.MgmAddr.MgmtAddrStringLen-LLDP_TLV_MGM_ADDR_MGMT_ADDR_SUBTYPE_SIZE);
                        }

                        pValue += pTLVParams->Params.MgmAddr.MgmtAddrStringLen-LLDP_TLV_MGM_ADDR_MGMT_ADDR_SUBTYPE_SIZE;

                        pTLVParams->Params.MgmAddr.InterfaceNumberingSubtype = *pValue++;
                        pTLVParams->Params.MgmAddr.InterfaceNumber           = LLDP_GET_U32(pValue);
                        pValue += 4;

                        pTLVParams->Params.MgmAddr.OIDStringLen              = *pValue++;

                        if ( pTLVParams->Params.MgmAddr.OIDStringLen )
                        {
                            LLDP_MEMCOPY_COMMON( (LLDP_COMMON_MEM_PTR_TYPE) pTLVParams->Params.MgmAddr.OIDIdentifier,
                                                 pValue,
                                                 pTLVParams->Params.MgmAddr.OIDStringLen);
                        }
                    }
                    break;

                    /*---------------------------------------------------------------*/
                case LLDP_TLV_TYPE_ORG_SPECIFIC:
                    /*---------------------------------------------------------------*/
                    /* ORG specific TLV has several subtypes..*/

                    if (( Length >= LLDP_TLV_ORG_SPECIFIC_MIN_SIZE ) &&
                        ( Length <= LLDP_TLV_ORG_SPECIFIC_MAX_SIZE ))
                    {
                        LSA_UINT8   OUI[LLDP_TLV_ORG_SPECIFIC_OUI_SIZE];
                        LSA_UINT32  OUIValue;
                        LSA_UINT32  Subtype;
                        LSA_BOOL    OrgUnknown;

                        OrgUnknown = LSA_FALSE;

                        OUI[0] = *pValue++;
                        OUI[1] = *pValue++;
                        OUI[2] = *pValue++;

                        OUIValue  = ((LSA_UINT32)(OUI[0])) << 16;
                        OUIValue += (OUI[1]) << 8;
                        OUIValue +=  OUI[2];

                        Subtype   = *pValue++;

                        switch (OUIValue)
                        {
                                /*---------------------------------------------------*/
                            case LLDP_TLV_ORG_OUI_IEEE_802_1:
                                /*---------------------------------------------------*/
                                switch (Subtype )
                                {

                                        /*-------------------------------------------*/
                                    case LLDP_TLV_IEEE_802_1_TYPE_PORT_VLAN_ID:
                                        /*-------------------------------------------*/
                                        pTLVParams->Type = LLDP_PARAM_TYPE_IEEE_802_1_PORT_VLAN_ID;

                                        if ( Length >= LLDP_TLV_IEEE_802_1_PORT_VLAN_ID_SIZE )
                                        {
                                            pTLVParams->Params.PortVLANID.PortVLANIdentifier = LLDP_GET_U16(pValue);
                                        }
                                        else
                                        {
                                            Response = LLDP_RSP_ERR_TLV_MISMATCH;
                                        }
                                        break;

                                        /*-------------------------------------------*/
                                    case LLDP_TLV_IEEE_802_1_TYPE_PPVID:
                                        /*-------------------------------------------*/
                                        pTLVParams->Type = LLDP_PARAM_TYPE_IEEE_802_1_PPVID;

                                        if ( Length >= LLDP_TLV_IEEE_802_1_PPVID_SIZE )
                                        {
                                            pTLVParams->Params.PPVID.Flags           = *pValue++;
                                            pTLVParams->Params.PPVID.ReferenceNumber = LLDP_GET_U16(pValue);
                                        }
                                        else
                                        {
                                            Response = LLDP_RSP_ERR_TLV_MISMATCH;
                                        }
                                        break;

                                        /*-------------------------------------------*/
                                    case LLDP_TLV_IEEE_802_1_TYPE_VLAN_NAME:
                                        /*-------------------------------------------*/
                                        pTLVParams->Type = LLDP_PARAM_TYPE_IEEE_802_1_VLAN_NAME;

                                        Response = LLDP_TLVValidateVLANNameLen((LLDP_COMMON_MEM_U8_PTR_TYPE) pValue,Length);

                                        if ( Response == LLDP_RSP_OK )
                                        {
                                            pTLVParams->Params.VLANName.VLANID = LLDP_GET_U16(pValue);
                                            pValue += 2;
                                            pTLVParams->Params.VLANName.VLANNameLength = *pValue++;

                                            if ( pTLVParams->Params.VLANName.VLANNameLength )
                                            {
                                                LLDP_MEMCOPY_COMMON( (LLDP_COMMON_MEM_PTR_TYPE) pTLVParams->Params.VLANName.VLANName,
                                                                     pValue,
                                                                     pTLVParams->Params.VLANName.VLANNameLength);
                                            }
                                        }
                                        break;

                                        /*-------------------------------------------*/
                                    case LLDP_TLV_IEEE_802_1_TYPE_PROTOCOL_IDENTITY:
                                        /*-------------------------------------------*/
                                        pTLVParams->Type = LLDP_PARAM_TYPE_IEEE_802_1_PROTOCOL_IDENTITY;

                                        Response = LLDP_TLVValidateProtIdentityLen((LLDP_COMMON_MEM_U8_PTR_TYPE) pValue,Length);

                                        if ( Response == LLDP_RSP_OK )
                                        {
                                            pTLVParams->Params.ProtIdentity.Length = *pValue++;

                                            if ( pTLVParams->Params.ProtIdentity.Length )
                                            {
                                                LLDP_MEMCOPY_COMMON( (LLDP_COMMON_MEM_PTR_TYPE) pTLVParams->Params.ProtIdentity.Identity,
                                                                     pValue,
                                                                     pTLVParams->Params.ProtIdentity.Length);
                                            }
                                        }

                                        break;
                                        /*-------------------------------------------*/
                                    default:
                                        /*-------------------------------------------*/
                                        /* unknown */
                                        OrgUnknown = LSA_TRUE;
                                        break;
                                }

                                break;
                                /*---------------------------------------------------*/
                            case LLDP_TLV_ORG_OUI_IEEE_802_3:
                                /*---------------------------------------------------*/
                                switch (Subtype )
                                {

                                        /*-------------------------------------------*/
                                    case LLDP_TLV_IEEE_802_3_TYPE_MAC_PHY_CFG_STATUS:
                                        /*-------------------------------------------*/
                                        pTLVParams->Type = LLDP_PARAM_TYPE_IEEE_802_3_MAC_PHY_CFG_STATUS;

                                        if ( Length >= LLDP_TLV_IEEE_802_3_MAC_PHY_CFG_STATUS_SIZE )
                                        {
                                            pTLVParams->Params.MACPHYConfigStatus.AutonegSupportStatus    = *pValue++;
                                            pTLVParams->Params.MACPHYConfigStatus.PMDAutonegAdvertisedCap = LLDP_GET_U16(pValue);
                                            pValue += 2;
                                            pTLVParams->Params.MACPHYConfigStatus.OperationalMAUType      = LLDP_GET_U16(pValue);
                                        }
                                        else
                                        {
                                            Response = LLDP_RSP_ERR_TLV_MISMATCH;
                                        }

                                        break;
                                        /*-------------------------------------------*/
                                    case LLDP_TLV_IEEE_802_3_TYPE_POWER_VIA_MDI:
                                        /*-------------------------------------------*/
                                        pTLVParams->Type = LLDP_PARAM_TYPE_IEEE_802_3_POWER_VIA_MDI;

                                        if ( Length >= LLDP_TLV_IEEE_802_3_POWER_VIA_MDI_SIZE )
                                        {
                                            pTLVParams->Params.PowerViaMDI.MDIPowerSupport = *pValue++;
                                            pTLVParams->Params.PowerViaMDI.PowerPair       = *pValue++;
                                            pTLVParams->Params.PowerViaMDI.PowerClass      = *pValue++;
                                        }
                                        else
                                        {
                                            Response = LLDP_RSP_ERR_TLV_MISMATCH;
                                        }

                                        break;
                                        /*-------------------------------------------*/
                                    case LLDP_TLV_IEEE_802_3_TYPE_LINK_AGGREGATION:
                                        /*-------------------------------------------*/
                                        pTLVParams->Type = LLDP_PARAM_TYPE_IEEE_802_3_LINK_AGGREGATION;

                                        if ( Length >= LLDP_TLV_IEEE_802_3_LINK_AGGREGATION_SIZE )
                                        {
                                            pTLVParams->Params.LinkAgg.LinkAggregationStatus = *pValue++;
                                            pTLVParams->Params.LinkAgg.AggregatedPortID      = LLDP_GET_U32(pValue);
                                        }
                                        else
                                        {
                                            Response = LLDP_RSP_ERR_TLV_MISMATCH;
                                        }
                                        break;
                                        /*-------------------------------------------*/
                                    case LLDP_TLV_IEEE_802_3_TYPE_MAX_FRAME_SIZE:
                                        /*-------------------------------------------*/
                                        pTLVParams->Type = LLDP_PARAM_TYPE_IEEE_802_3_MAX_FRAME_SIZE;

                                        if ( Length >= LLDP_TLV_IEEE_802_3_MAX_FRAME_SIZE_SIZE )
                                        {
                                            pTLVParams->Params.MaxFrameSize.MaxFrameSize = LLDP_GET_U16(pValue);
                                        }
                                        else
                                        {
                                            Response = LLDP_RSP_ERR_TLV_MISMATCH;
                                        }

                                        break;
                                        /*-------------------------------------------*/
                                    default:
                                        /*-------------------------------------------*/
                                        /* unknown */
                                        OrgUnknown = LSA_TRUE;
                                        break;
                                }
                                break;
                                /*---------------------------------------------------*/
                            default:
                                /*---------------------------------------------------*/
                                /* unknown OUI */
                                OrgUnknown = LSA_TRUE;
                                break;
                        }

                        if ( OrgUnknown )
                        {
                            pTLVParams->Type = LLDP_PARAM_TYPE_ORG_SPECIFIC;

                            pTLVParams->Params.OrgUnknown.OUI[0]  = OUI[0];
                            pTLVParams->Params.OrgUnknown.OUI[1]  = OUI[1];
                            pTLVParams->Params.OrgUnknown.OUI[2]  = OUI[2];

                            pTLVParams->Params.OrgUnknown.Subtype = (LSA_UINT8) Subtype;

                            /* Note: Length already checked, so no underflow ! */
                            Length -= LLDP_TLV_ORG_SPECIFIC_MIN_SIZE;

                            pTLVParams->Params.OrgUnknown.GroupDefInfoStringLen = (LSA_UINT16) Length;

                            if ( Length )
                            {
                                LLDP_MEMCOPY_COMMON( (LLDP_COMMON_MEM_PTR_TYPE) pTLVParams->Params.OrgUnknown.GroupDefInfoString,
                                                     pValue,
                                                     Length);
                            }
                        }
                    }
                    else
                    {
                        Response = LLDP_RSP_ERR_TLV_MISMATCH;
                    }

                    break;
                    /*---------------------------------------------------------------*/
                default:
                    /*---------------------------------------------------------------*/
                    /* unknown optional TLV.  */

                    pTLVParams->Type = LLDP_PARAM_TYPE_UNKNOWN_OPTIONAL;

                    pTLVParams->Params.UnknownOptional.Type = (LSA_UINT8) Type;

                    if ( Length <= LLDP_LEN_MAX_SIZE )
                    {
                        pTLVParams->Params.UnknownOptional.ValueLen  = (LSA_UINT16) (Length);

                        LLDP_MEMCOPY_COMMON( (LLDP_COMMON_MEM_PTR_TYPE) pTLVParams->Params.UnknownOptional.Value,
                                             pValue,
                                             Length);
                    }
                    else
                    {
                        Response = LLDP_RSP_ERR_TLV_MISMATCH;
                    }
                    break;

            } /* switch */

            /* We advance to next TLV if possible */
            if ( ( Response == LLDP_RSP_OK ) ||
                 ( Response == LLDP_RSP_ERR_TLV_UNKNOWN ) ||
                 ( Response == LLDP_RSP_ERR_TLV_PARAM_ERROR ))
            {
                *ppTLV    = pTLV;     /* behind read TLV element    */
                *pTLength = TLength;  /* remaining TLV-Chain length */
            }
        }
    }
    return(Response);
}


/*****************************************************************************/
/*  end of file LLDP_TLV.C                                                   */
/*****************************************************************************/

