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
/*  F i l e               &F: oha_db_l.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA USER-database interface functions            */
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

#define LTRC_ACT_MODUL_ID  35
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_DB_L */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "oha_inc.h"            /* OHA headerfiles */
#include "oha_int.h"            /* internal header */

/* BTRACE-IF */
OHA_FILE_SYSTEM_EXTENSION(OHA_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/


/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/


/*===========================================================================*/
/*                                 Macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/

LSA_BOOL OHA_LldpIsPortOption(LSA_UINT8 Type, LSA_UINT16 PortId)
{
    LSA_BOOL IsPortOption;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LldpIsPortOption(Type: 0x%X, PortId: 0x%X)", Type, PortId);

    /* check portID */
    switch (Type)
    {
            /* interface-global options needs no PortId (port independent) */
        case OHA_LLDP_TYPE_CHASSIS_ID:      /* NameOfStation       */
        case OHA_LLDP_TYPE_TTL:             /* Option 3 TimeToLive */
        case OHA_LLDP_TYPE_SYSTEM_NAME:     /* Option 5 System Name */
        case OHA_LLDP_TYPE_SYSTEM_DESCR:    /* Option 6 SystemDescription */
        case OHA_LLDP_TYPE_SYSTEM_CAP:      /* Option 7 System Capabilities */
        case OHA_LLDP_TYPE_MGM_ADDR:        /* Optiion 8 Management Address */
            IsPortOption = LSA_FALSE;
            break;
		/* The Portstrings (port_id = 1...n) and the string for the IF (port_id = 0) is supported too! */ 
		case OHA_LLDP_TYPE_PORT_DESCR:
			if(PortId == 0)	/* interface */
			{
	            IsPortOption = LSA_FALSE;
			}
			else
			{
	            IsPortOption = LSA_TRUE;
			}
            break;
        default:
            IsPortOption = LSA_TRUE;
            break;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LldpIsPortOption(IsPortOption: 0x%X)", IsPortOption);
    return(IsPortOption);
}


LSA_BOOL OHA_LldpIsMib2Object(LSA_UINT8 Type)
{
    LSA_BOOL IsMib2Object;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_LldpIsMib2Object(Type: 0x%X)", Type);

    /* check portID */
    switch (Type)
    {
            /* interface-global options needs no PortId (port independent) */
        case OHA_LLDP_TYPE_SYSTEM_NAME:     /* Option 5 System Name */
        case OHA_LLDP_TYPE_SYSTEM_DESCR:    /* Option 6 SystemDescription */
        case OHA_LLDP_TYPE_PORT_DESCR:      /* Option 4 InterfaceDescription */
            IsMib2Object = LSA_TRUE;
            break;

        default:
            IsMib2Object = LSA_FALSE;
            break;
    }
    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_LldpIsMib2Object(IsMib2Object: 0x%X, Type: 0x%X)", IsMib2Object, Type);
    return(IsMib2Object);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestReadLLdpElement                  +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                        :    OHA_HDB_PTR_TYPE            pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_LLDP_READ_ELEMENT      +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.LldpElem                               +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+     Type   OUI   SubType    Usage                                       +*/
/*+                                                                         +*/
/*+        0    -       -       All TLVs of the frame                       +*/
/*+     1..8    -       0       All TLVs of this type of the frame          +*/
/*+     1..8    -    1..x       All TLVs of this Type/SubType of the frame  +*/
/*+      127   <>0      0       All TLVs with this OUI                      +*/
/*+      127   <>0   1..x       All TLVs of this SubType with this OUI      +*/
/*+   9..126    -       -       Error!                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to read remote lldp parameters from the database  +*/
/*+                                                                         +*/
/*+               Fills RQB with station data and confirms request.         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestReadLldpElement(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    OHA_HDB_EDD_PTR_TYPE      pHDBEdd;
    LSA_UINT16                Response;
    OHA_LLDP_ELEM_PTR_TYPE    pLldpElement;
    OHA_DB_IF_PTR_TYPE        pDB;
    LSA_UINT16                NicId = pHDB->Params.pDetail->Params.User.NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestReadLldpElement(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);

    pLldpElement = &pRQB->args.LldpElem;
    pDB = OHA_DB_GET_INTERFACE(NicId);

    OHA_ASSERT_NULL_PTR(pDB);

    /* Check if we have a EDD-Channel for NIC-ID. */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NicId); /* only EDD channel for this request necessary */

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;  /* Mode is wrong */
        pLldpElement->Count = 0;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_READ_ELEMENT failed. No Lower Channel for this NicId (NIC-ID: 0x%X)", NicId);
    }
    else if ( OHA_IS_NULL(pLldpElement->pElements) && pLldpElement->Count != 0)
    {
        Response = OHA_ERR_PARAM;  /* No ArrayOfElements given */
        /* Error on pElements. Abort. */
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_READ_ELEMENT failed. No ArrayOfElements given(NIC-ID: 0x%X, Count: 0x%X)", NicId, pLldpElement->Count);
        pLldpElement->Count = 0;
    }
    else if ((!pLldpElement->PortId) || (pLldpElement->PortId > OHA_CFG_MAX_PORTS))
    {
        Response = OHA_ERR_PARAM;  /* Mode is wrong */
        pLldpElement->Count = 0;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_READ_ELEMENT failed. PortID is wrong (PORT-ID: 0x%X)", pLldpElement->PortId);
    }
    else if (pLldpElement->Type == OHA_LLDP_TYPE_ORG_SPECIFIC && /* check the LLDP-type */
             pLldpElement->OUI == 0)
    {
        Response = OHA_ERR_PARAM;  /* OUI is wrong */
        pLldpElement->Count = 0;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_READ_ELEMENT failed. OUI is wrong (OUI: 0x%X)", pLldpElement->OUI);
    }
    else
    {
        LSA_UINT16 SenderId = pDB->LldpData[pLldpElement->PortId-1].LldpSenderId;
        LSA_UINT8  LldpSenderStatus = pDB->LldpData[pLldpElement->PortId-1].LldpSenderStatus;
        LSA_UINT16 Count  = 0;

        Response = OHA_ERR_FAILED;
        if (SenderId && (SenderId <= LLDP_MAX_PORT_SENDER)) /* check it */
        {
            OHA_LLDP_SENDER_PTR_TYPE pDbSender  = &pDB->LldpData[pLldpElement->PortId-1].RemoteSender[SenderId-1];

            OHA_DB_LLDP_ELEM_PTR_TYPE pUserList = (pLldpElement->Count) ? pLldpElement->pElements : LSA_NULL; /* array (list) of LLDP elements */

            OHA_ASSERT(pDbSender);

            /* set the sender-status in the IndCb of the default-sender */
            pLldpElement->Status = (LldpSenderStatus) ? LldpSenderStatus : (LSA_UINT8)OHA_STATUS_NOT_PRESENT;   /* entry of presence */

            OHA_MEMCPY(&pLldpElement->SenderMacAddr, &pDbSender->SenderMacAddr, OHA_MAC_ADDR_SIZE);

            if (pDbSender->SnmpSenderStatus == OHA_STATUS_DATA_VALID ||
                pDbSender->SnmpSenderStatus == OHA_STATUS_DATA_INVALID )
            {
                OHA_COMMON_MEM_U8_PTR_TYPE pBuffer = pLldpElement->pVarBuffer;
                LSA_UINT16 WriteLength             = 0;

                if (pLldpElement->Type == 0)    /* all elements selected */
                {
                    pLldpElement->SubType = 0;  /* set the initial value */
                    pLldpElement->OUI = 0;      /* set the initial value */
                }

                /*********************************************************/
                /* is only one type (1...8) or 127 with subtype to read ? */
                /*********************************************************/
                if (pLldpElement->Type && pLldpElement->SubType)
                {
                    LSA_UINT8 SubType = pLldpElement->SubType; /* subtype of variable, suboption */
                    OHA_COMMON_MEM_U8_PTR_TYPE pVariable = LSA_NULL; /* variable in octets */
                    LSA_UINT16 VarLength = 0;                  /* length of variable in octets */
                    if (LSA_RET_OK == oha_db_read_rem_lldp_element(NicId,
                                                                   pLldpElement->PortId, SenderId,
                                                                   pLldpElement->Type, &SubType,
                                                                   pLldpElement->OUI,
                                                                   &pVariable, &VarLength))
                    {
                        if (SubType == pLldpElement->SubType)
                        {
                            Response = OHA_OK;
                            Count++;

                            if (OHA_IS_NOT_NULL(pUserList))
                            {
                                OHA_MEMSET(pUserList, 0, sizeof(OHA_DB_LLDP_ELEM_TYPE));
                                pUserList->Type     = pLldpElement->Type;
                                pUserList->SubType  = pLldpElement->SubType;
                                pUserList->OUI      = pLldpElement->OUI;

                                if (OHA_IS_NOT_NULL(pBuffer))       /* the global provided user-buffer */
                                {
                                    LSA_UINT16 Length = VarLength;
                                    if (Length & 1) Length++;       /* even address */
                                    WriteLength += Length;
                                    if (OHA_IS_NOT_NULL(pVariable) && VarLength && (WriteLength <= LLDP_MAX_TLV_DATA_SIZE))
                                    {
                                        pUserList->pVariable = pBuffer; /* first entry in pBuffer */
                                        pUserList->Length    = VarLength;
                                        OHA_MEMCPY(pUserList->pVariable, pVariable, pUserList->Length);
                                        /* pBuffer += Length; */ /* is only one type! */
                                    }
                                }
                            }
                            /* is only one type! */
                            /* if (OHA_IS_NOT_NULL(pUserList) && (Count < pLldpElement->Count)) */
                            /* {                */
                            /*  pUserList++;    */
                            /* }                */
                        }
                    }

                    pLldpElement->Count = Count;
                    OHA_UserRequestFinish(pHDB,pRQB,Response);
                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadOneLldpElement(Response: 0x%X)", Response);
                    return; /*  ==========>>>>>>>>>>>>  */
                }

                /*********************************************************/
                /* all the types (1...8) are to read (subtype = 0) ?     */
                /* for the types (1...8) there's only one subtype: only one element is to read */
                /*********************************************************/
                if (pLldpElement->Type <= OHA_LLDP_TYPE_MGM_ADDR)
                {
                    LSA_UINT8  Type    = (pLldpElement->Type) ? pLldpElement->Type : (LSA_UINT8)OHA_LLDP_TYPE_CHASSIS_ID;

                    for (; Type <= OHA_LLDP_TYPE_MGM_ADDR; Type++)
                    {
                        OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;

                        pAnchor = oha_get_rem_lldp_db_element (NicId, pLldpElement->PortId, SenderId, Type,    /* type of variable */
                                                               1, 1);      /* should not be zero */
                        if (OHA_IS_NOT_NULL(pAnchor))       /* anchor exists */
                        {
                            OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

                            if (OHA_IS_NOT_NULL(pElement))
                            {
                                Response = OHA_OK;
                                Count++;

                                if (OHA_IS_NOT_NULL(pUserList))
                                {
                                    OHA_MEMSET(pUserList, 0, sizeof(OHA_DB_LLDP_ELEM_TYPE));
                                    pUserList->Type     = pElement->Type;
                                    pUserList->SubType  = pElement->SubType;
                                    pUserList->OUI      = pElement->OUI;
                                    if (OHA_IS_NOT_NULL(pBuffer))       /* the global provided user-buffer */
                                    {
                                        LSA_UINT16 Length  = pElement->Length;
                                        if (Length & 1) Length++;       /* even address */
                                        WriteLength += Length;
                                        if (OHA_IS_NOT_NULL(pElement->pVariable) && pElement->Length && (WriteLength <= LLDP_MAX_TLV_DATA_SIZE))
                                        {
                                            pUserList->pVariable = pBuffer; /* first entry in pBuffer */
                                            pUserList->Length    = pElement->Length;
                                            OHA_MEMCPY(pUserList->pVariable, pElement->pVariable, pUserList->Length);
                                            pBuffer += Length;
                                        }
                                    }
                                }
                                if (OHA_IS_NOT_NULL(pUserList) && (Count < pLldpElement->Count))
                                {
                                    pUserList++;
                                }
                            }
                        }

                        if (pLldpElement->Type)     /* only one element is to read ? */
                        {
                            break;                  /* that's it */
                        }
                    }
                }


                /*********************************************************/
                /* all the types (9...126) are to read (subtype = 0) ?   */
                /*********************************************************/
                if (pLldpElement->Type == 0)
                {
                    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;

                    pAnchor = oha_get_rem_lldp_db_element (NicId, pLldpElement->PortId,
                                                           SenderId, OHA_LLDP_TYPE_MGM_ADDR+1,  /* type of variable */
                                                           1, 1);      /* should not be zero */
                    if (OHA_IS_NOT_NULL(pAnchor))       /* anchor exists */
                    {
                        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

                        while (OHA_IS_NOT_NULL(pElement))
                        {
                            Response = OHA_OK;
                            Count++;

                            if (OHA_IS_NOT_NULL(pUserList))
                            {
                                OHA_MEMSET(pUserList, 0, sizeof(OHA_DB_LLDP_ELEM_TYPE));
                                pUserList->Type     = pElement->Type;
                                pUserList->SubType  = pElement->SubType;
                                pUserList->OUI      = pElement->OUI;
                                if (OHA_IS_NOT_NULL(pBuffer))       /* the global provided user-buffer */
                                {
                                    LSA_UINT16 Length  = pElement->Length;
                                    if (Length & 1) Length++;       /* even address */
                                    WriteLength += Length;
                                    if (OHA_IS_NOT_NULL(pElement->pVariable) && pElement->Length && (WriteLength <= LLDP_MAX_TLV_DATA_SIZE))
                                    {
                                        pUserList->pVariable = pBuffer; /* first entry in pBuffer */
                                        pUserList->Length    = pElement->Length;
                                        OHA_MEMCPY(pUserList->pVariable, pElement->pVariable, pUserList->Length);
                                        pBuffer += Length;
                                    }
                                }
                            }
                            if (OHA_IS_NOT_NULL(pUserList) && (Count < pLldpElement->Count))
                            {
                                pUserList++;
                            }

                            pElement = OhaListNext(pAnchor, &pElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
                        }
                    }
                }

                /*********************************************************/
                /******** type 127, init the loop for the OUI 802.1 ******/
                /*********************************************************/
                if ((pLldpElement->Type == OHA_LLDP_TYPE_ORG_SPECIFIC && pLldpElement->OUI == OHA_LLDP_TYPE_ORG_OUI_IEEE_802_1) ||
                    pLldpElement->Type == 0)
                {
                    LSA_UINT8  Type    = (pLldpElement->Type) ? pLldpElement->Type : (LSA_UINT8)OHA_LLDP_TYPE_ORG_SPECIFIC;
                    LSA_UINT32  OUI    = (pLldpElement->OUI) ? (pLldpElement->OUI) : OHA_LLDP_TYPE_ORG_OUI_IEEE_802_1;
                    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;

                    pAnchor = oha_get_rem_lldp_db_element (NicId, pLldpElement->PortId, SenderId, Type,    /* type of variable */
                                                           1, OUI);
                    if (OHA_IS_NOT_NULL(pAnchor))       /* anchor exists */
                    {
                        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

                        while (OHA_IS_NOT_NULL(pElement))
                        {
                            Response = OHA_OK;
                            Count++;

                            if (OHA_IS_NOT_NULL(pUserList))
                            {
                                OHA_MEMSET(pUserList, 0, sizeof(OHA_DB_LLDP_ELEM_TYPE));
                                pUserList->Type     = pElement->Type;
                                pUserList->SubType  = pElement->SubType;
                                pUserList->OUI      = pElement->OUI;
                                if (OHA_IS_NOT_NULL(pBuffer))       /* the global provided user-buffer */
                                {
                                    LSA_UINT16 Length  = pElement->Length;
                                    if (Length & 1) Length++;       /* even address */
                                    WriteLength += Length;
                                    if (OHA_IS_NOT_NULL(pElement->pVariable) && pElement->Length && (WriteLength <= LLDP_MAX_TLV_DATA_SIZE))
                                    {
                                        pUserList->pVariable = pBuffer; /* first entry in pBuffer */
                                        pUserList->Length    = pElement->Length;
                                        OHA_MEMCPY(pUserList->pVariable, pElement->pVariable, pUserList->Length);
                                        pBuffer += Length;
                                    }
                                }
                            }
                            if (OHA_IS_NOT_NULL(pUserList) && (Count < pLldpElement->Count))
                            {
                                pUserList++;
                            }
                            pElement = OhaListNext(pAnchor, &pElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
                        }
                    }
                }

                /*********************************************************/
                /******** type 127, init the loop for the OUI 802.3 ******/
                /*********************************************************/
                if ((pLldpElement->Type == OHA_LLDP_TYPE_ORG_SPECIFIC && pLldpElement->OUI == OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3) ||
                    pLldpElement->Type == 0)
                {
                    LSA_UINT8  Type    = (pLldpElement->Type) ? pLldpElement->Type : (LSA_UINT8)OHA_LLDP_TYPE_ORG_SPECIFIC;
                    LSA_UINT32 OUI     = (pLldpElement->OUI) ? (pLldpElement->OUI) : OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3;

                    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;

                    pAnchor = oha_get_rem_lldp_db_element (NicId, pLldpElement->PortId, SenderId, Type,    /* type of variable */
                                                           1, OUI);
                    if (OHA_IS_NOT_NULL(pAnchor))       /* anchor exists */
                    {
                        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

                        while (OHA_IS_NOT_NULL(pElement))
                        {
                            Response = OHA_OK;
                            Count++;

                            if (OHA_IS_NOT_NULL(pUserList))
                            {
                                OHA_MEMSET(pUserList, 0, sizeof(OHA_DB_LLDP_ELEM_TYPE));
                                pUserList->Type     = pElement->Type;
                                pUserList->SubType  = pElement->SubType;
                                pUserList->OUI      = pElement->OUI;
                                if (OHA_IS_NOT_NULL(pBuffer))       /* the global provided user-buffer */
                                {
                                    LSA_UINT16 Length  = pElement->Length;
                                    if (Length & 1) Length++;       /* even address */
                                    WriteLength += Length;
                                    if (OHA_IS_NOT_NULL(pElement->pVariable) && pElement->Length && (WriteLength <= LLDP_MAX_TLV_DATA_SIZE))
                                    {
                                        pUserList->pVariable = pBuffer; /* first entry in pBuffer */
                                        pUserList->Length    = pElement->Length;
                                        OHA_MEMCPY(pUserList->pVariable, pElement->pVariable, pUserList->Length);
                                        pBuffer += Length;
                                    }
                                }
                            }
                            if (OHA_IS_NOT_NULL(pUserList) && (Count < pLldpElement->Count))
                            {
                                pUserList++;
                            }
                            pElement = OhaListNext(pAnchor, &pElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
                        }
                    }
                }

                /*********************************************************/
                /******** type 127, init the loop for the OUI PNO   ******/
                /*********************************************************/
                if ((pLldpElement->Type == OHA_LLDP_TYPE_ORG_SPECIFIC && pLldpElement->OUI == OHA_LLDP_TYPE_ORG_OUI_PNO) ||
                    pLldpElement->Type == 0)
                {
                    LSA_UINT8  Type    = (pLldpElement->Type) ? pLldpElement->Type : (LSA_UINT8)OHA_LLDP_TYPE_ORG_SPECIFIC;
                    LSA_UINT32 OUI     = (pLldpElement->OUI) ? (pLldpElement->OUI) : OHA_LLDP_TYPE_ORG_OUI_PNO;

                    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;

                    pAnchor = oha_get_rem_lldp_db_element (NicId, pLldpElement->PortId, SenderId, Type,    /* type of variable */
                                                           1, OUI);
                    if (OHA_IS_NOT_NULL(pAnchor))       /* anchor exists */
                    {
                        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

                        while (OHA_IS_NOT_NULL(pElement))
                        {
                            Response = OHA_OK;
                            Count++;

                            if (OHA_IS_NOT_NULL(pUserList))
                            {
                                OHA_MEMSET(pUserList, 0, sizeof(OHA_DB_LLDP_ELEM_TYPE));
                                pUserList->Type     = pElement->Type;
                                pUserList->SubType  = pElement->SubType;
                                pUserList->OUI      = pElement->OUI;
                                if (OHA_IS_NOT_NULL(pBuffer))       /* the global provided user-buffer */
                                {
                                    LSA_UINT16 Length  = pElement->Length;
                                    if (Length & 1) Length++;       /* even address */
                                    WriteLength += Length;
                                    if (OHA_IS_NOT_NULL(pElement->pVariable) && pElement->Length && (WriteLength <= LLDP_MAX_TLV_DATA_SIZE))
                                    {
                                        pUserList->pVariable = pBuffer; /* first entry in pBuffer */
                                        pUserList->Length    = pElement->Length;
                                        OHA_MEMCPY(pUserList->pVariable, pElement->pVariable, pUserList->Length);
                                        pBuffer += Length;
                                    }
                                }
                            }
                            if (OHA_IS_NOT_NULL(pUserList) && (Count < pLldpElement->Count))
                            {
                                pUserList++;
                            }
                            pElement = OhaListNext(pAnchor, &pElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
                        }
                    }
                }

                /*********************************************************/
                /******** type 127, init the loop for other OUI's   ******/
                /*********************************************************/
                if ((pLldpElement->Type == OHA_LLDP_TYPE_ORG_SPECIFIC && (pLldpElement->OUI != OHA_LLDP_TYPE_ORG_OUI_IEEE_802_1 &&
                                                                          pLldpElement->OUI != OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3 && pLldpElement->OUI != OHA_LLDP_TYPE_ORG_OUI_PNO)) ||
                    pLldpElement->Type == 0)
                {
                    LSA_UINT8  Type    = (pLldpElement->Type) ? pLldpElement->Type : (LSA_UINT8)OHA_LLDP_TYPE_ORG_SPECIFIC;
                    LSA_UINT32 OUI     = (pLldpElement->OUI) ? (pLldpElement->OUI) : 1;
                    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;

                    pAnchor = oha_get_rem_lldp_db_element (NicId, pLldpElement->PortId, SenderId, Type,    /* type of variable */
                                                           1, OUI);
                    if (OHA_IS_NOT_NULL(pAnchor))       /* anchor exists */
                    {
                        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

                        while (OHA_IS_NOT_NULL(pElement))
                        {
                            if (pLldpElement->Type == 0 ||      /* get all elements */
                                (/* pLldpElement->Type &&*/ (pLldpElement->OUI == pElement->OUI)) ) /* or only this OUI */
                            {
                                Response = OHA_OK;
                                Count++;

                                if (OHA_IS_NOT_NULL(pUserList))
                                {
                                    OHA_MEMSET(pUserList, 0, sizeof(OHA_DB_LLDP_ELEM_TYPE));
                                    pUserList->Type     = pElement->Type;
                                    pUserList->SubType  = pElement->SubType;
                                    pUserList->OUI      = pElement->OUI;
                                    if (OHA_IS_NOT_NULL(pBuffer))       /* the global provided user-buffer */
                                    {
                                        LSA_UINT16 Length  = pElement->Length;
                                        if (Length & 1) Length++;       /* even address */
                                        WriteLength += Length;
                                        if (OHA_IS_NOT_NULL(pElement->pVariable) && pElement->Length && (WriteLength <= LLDP_MAX_TLV_DATA_SIZE))
                                        {
                                            pUserList->pVariable = pBuffer; /* first entry in pBuffer */
                                            pUserList->Length    = pElement->Length;
                                            OHA_MEMCPY(pUserList->pVariable, pElement->pVariable, pUserList->Length);
                                            pBuffer += Length;
                                        }
                                    }
                                }
                                if (OHA_IS_NOT_NULL(pUserList) && (Count < pLldpElement->Count))
                                {
                                    pUserList++;
                                }
                            }
                            pElement = OhaListNext(pAnchor, &pElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
                        }
                    }
                }
            }
        }
        else
        {
            Response = OHA_ERR_STATION_NOT_FOUND;
        }
        pLldpElement->Count = Count;
    }

    OHA_UserRequestFinish(pHDB,pRQB,Response);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadLldpElement(Response: 0x%X)", Response);
}


/*****************************************************************************/
/* Internal input function: oha_create_lldp_DbElement()                      */
/* Allocs local database-element for lldp                                    */
/*****************************************************************************/
OHA_DB_LLDP_ELEM_PTR_TYPE oha_create_lldp_DbElement(
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* subtype of variable */
    LSA_UINT32                  OUI,        /* OUI, if subtype=127 */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable,  /* variable in octets */
    LSA_UINT16                  Length      /* length of pVariable in octets */
)
{
    OHA_DB_LLDP_ELEM_PTR_TYPE pElement = LSA_NULL;
    OHA_LOCAL_MEM_PTR_TYPE  MemPtr = LSA_NULL;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_create_lldp_DbElement(Type: 0x%X, SubType: 0x%X, Length: 0x%X, OUI: 0x%X)",
                          Type, SubType, Length, OUI);
    OHA_ALLOC_LOCAL_MEM(&MemPtr, sizeof(OHA_DB_LLDP_ELEM_TYPE));
    if (! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
    {
        OHA_MEMSET(MemPtr,0,sizeof(OHA_DB_LLDP_ELEM_TYPE));
        pElement = (OHA_DB_LLDP_ELEM_PTR_TYPE)MemPtr;

        pElement->Type     = Type;
        pElement->SubType  = SubType;
        pElement->OUI      = OUI;
        OhaListInitialize(& pElement->ListElem);

        if (Length && pVariable)
        {
            OHA_ALLOC_LOCAL_MEM(&MemPtr, Length);
            if (! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
            {
                pElement->pVariable = (OHA_COMMON_MEM_U8_PTR_TYPE)MemPtr;
                OHA_MEMCPY(pElement->pVariable, pVariable, Length);
                pElement->Length = Length;
            }
            else
            {
                LSA_UINT16   FreeStat = LSA_RET_OK;
                OHA_FREE_LOCAL_MEM(&FreeStat, pElement);
                OHA_MEM_FREE_FAIL_CHECK(FreeStat);
                pElement = 0;
            }
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"oha_create_lldp_DbElement(). Allocating memory failed.");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_create_lldp_DbElement(pElement: 0x%X)", pElement);
    return pElement;
}


/*****************************************************************************/
/* Internal input function: oha_find_lldp_DbElement()                        */
/* Search for a database-element                                             */
/*****************************************************************************/
OHA_DB_LLDP_ELEM_PTR_TYPE oha_find_lldp_DbElement(
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor,  /* start of a list  */
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* subtype of variable */
    LSA_UINT32                  OUI         /* OUI, if type = 127  */
)
{
    OHA_DB_LLDP_ELEM_PTR_TYPE pElement = LSA_NULL;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_find_lldp_DbElement(Type: 0x%X, SubType: 0x%X, OUI: 0x%X)",
                          Type, SubType, OUI);

    if (!(LSA_HOST_PTR_ARE_EQUAL(pAnchor, LSA_NULL) || OhaListIsEmpty(pAnchor)) )
    {
        switch (Type)
        {
            case OHA_LLDP_TYPE_ORG_SPECIFIC:
            {
                OHA_DB_LLDP_ELEM_PTR_TYPE pTempElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

                while (OHA_IS_NOT_NULL(pTempElement))
                {
                    if (pTempElement->Type == Type &&
                        pTempElement->SubType == SubType &&
                        pTempElement->OUI == OUI)
                    {
                        pElement = pTempElement;
                        break;  /* element found */
                    }
                    pTempElement = OhaListNext(pAnchor, &pTempElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
                }
            }
            break;
            default:
                if (Type >= OHA_LLDP_TYPE_CHASSIS_ID && Type <= OHA_LLDP_TYPE_MGM_ADDR)
                {
                    pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);    /* Type 1..8 only one element possible */
                }
                else if (Type > OHA_LLDP_TYPE_MGM_ADDR && Type < OHA_LLDP_TYPE_ORG_SPECIFIC)
                {                                                               /* Type 9..126 */
                    OHA_DB_LLDP_ELEM_PTR_TYPE pTempElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

                    while (OHA_IS_NOT_NULL(pTempElement))
                    {
                        if (pTempElement->Type == Type)
                        {
                            pElement = pTempElement;
                            break;  /* element found */
                        }
                        pTempElement = OhaListNext(pAnchor, &pTempElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
                    }
                }
                break;
        }
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"oha_find_lldp_DbElement(). List is empty or Anchor is NULL.");
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_find_lldp_DbElement(pElement: 0x%X)", pElement);
    return pElement;
}


/*****************************************************************************/
/* Internal input function: oha_update_lldp_DbElement()                      */
/* updates an existing database-element                                      */
/*****************************************************************************/
LSA_UINT16 oha_update_lldp_DbElement(
    OHA_DB_LLDP_ELEM_PTR_TYPE   pElement,
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* subtype of variable */
    LSA_UINT32                  OUI,        /* OUI, if subtype=127 */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable,  /* variable in octets */
    LSA_UINT16                  Length      /* length of pVariable in octets */
)
{
    LSA_UINT16 RetVal = LSA_RET_OK;
    OHA_LOCAL_MEM_PTR_TYPE  pVarTemp = LSA_NULL;    /* variable in octets */

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_update_lldp_DbElement(Type: 0x%X, SubType: 0x%X, Length: 0x%X, OUI: 0x%X)",
                          Type, SubType, Length, OUI);
    if (! LSA_HOST_PTR_ARE_EQUAL(pElement, LSA_NULL) )
    {
        LSA_UINT16 FreeStat = LSA_RET_OK;   /* free the actual buffer for var. */
		LSA_BOOL Remanent = pElement->Remanent;	/* store the previous setting */

        if (Length && OHA_IS_NOT_NULL(pVariable))   /* is there a buffer to allocate ? */
        {
            OHA_ALLOC_LOCAL_MEM(&pVarTemp, Length);  /* alloc the new buffer for the variable */
            if ( LSA_HOST_PTR_ARE_EQUAL(pVarTemp, LSA_NULL) )  /* failed the allocation */
            {
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"oha_update_lldp_DbElement(). Allocating memory failed.");
                RetVal = LSA_RET_ERR_RESOURCE;
                return RetVal;
            }
        }

        /* is there a buffer to free ? */
        if (pElement->pVariable)
        {
            OHA_FREE_LOCAL_MEM(&FreeStat, pElement->pVariable);
            OHA_MEM_FREE_FAIL_CHECK(FreeStat);
        }

        pElement->pVariable = (OHA_COMMON_MEM_U8_PTR_TYPE)pVarTemp;   /* now supply the new var. */

        pElement->Type = Type;
        pElement->SubType = SubType;
        if (Length && OHA_IS_NOT_NULL(pVariable))
        {
            OHA_MEMCPY(pElement->pVariable, pVariable, Length);
            pElement->Length = Length;
        }
        else
        {
            pElement->Length = 0;
        }
        pElement->OUI    = OUI;
        pElement->Remanent = Remanent;
    }
    else
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,"oha_update_lldp_DbElement(). No such element.");
        RetVal = LSA_RET_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_update_lldp_DbElement(RetVal: 0x%X, pElement: 0x%X)", RetVal, pElement);
    return RetVal;
}

/*****************************************************************************/
/* Internal input function: oha_remove_lldp_DbElement()                      */
/* Frees the database-element                                                */
/*****************************************************************************/
LSA_VOID oha_remove_lldp_DbElement (OHA_DB_LLDP_ELEM_PTR_TYPE pElement)
{
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_remove_lldp_DbElement(pElement: 0x%X)", pElement);

    if (OHA_IS_NOT_NULL(pElement))      /* get the element */
    {
        LSA_UINT16 RetVal = LSA_RET_OK;

        if (! LSA_HOST_PTR_ARE_EQUAL(pElement->pVariable, LSA_NULL) )
        {
            LSA_UINT16   FreeStat = LSA_RET_OK;
            OHA_FREE_LOCAL_MEM(&FreeStat, pElement->pVariable);
            OHA_MEM_FREE_FAIL_CHECK(FreeStat);
        }
        OHA_FREE_LOCAL_MEM(&RetVal,pElement);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);
    }
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_remove_lldp_DbElement()");
}


/*****************************************************************************/
/* Internal input function: oha_delete_lldp_DbElement()                      */
/* Frees one database-element                                                */
/*****************************************************************************/
LSA_VOID oha_delete_lldp_DbElement (OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor,
                                    OHA_LOCAL_MEM_U16_PTR_TYPE pCountOfDeletedElem)
{
    LSA_UINT16 Count = 0;   /* count of eleted elements */

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_delete_lldp_DbElement(pAnchor: 0x%X)", pAnchor);

    if (OHA_IS_NOT_NULL(pAnchor))       /* get the element */
    {
        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

        if (OHA_IS_NOT_NULL(pElement))
        {
#if 0
            switch (pElement->Type)
            {
                case OHA_LLDP_TYPE_ORG_SPECIFIC:
                {
#endif
                    while (OHA_IS_NOT_NULL(pElement))
                    {
                        /* remove from list and free */
                        OhaListRemoveEntry(&pElement->ListElem);
                        Count++;
                        oha_remove_lldp_DbElement (pElement);
                        pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);    /* reinit. */
                        /* pElement = OhaListNext(pAnchor, &Element->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE); */
                    }
#if 0
                }
                break;
                default:
                    oha_remove_lldp_DbElement (pElement);
                    break;
            }
#endif
        }
    }

    if (OHA_IS_NOT_NULL(pCountOfDeletedElem))   /* count for satistics */
    {
        *pCountOfDeletedElem = Count;
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_delete_lldp_DbElement()");
}


/*=============================================================================
 * function name:  oha_db_delete_rem_lldp_pno_others
 * function:       deletes the database element
 * parameters:     NicId
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_delete_rem_lldp_pno_others(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_delete_rem_pno_others(NicId: 0x%X, PortId: 0x%X, SenderId: 0x%X)",
                          NicId, PortId, SenderId);

    pAnchor = oha_get_rem_lldp_db_element (NicId, PortId, SenderId, OHA_LLDP_TYPE_ORG_SPECIFIC,   /* type of variable */
                                           0, OHA_LLDP_TYPE_ORG_OUI_PNO);  /* type of variable */
    if (OHA_IS_NOT_NULL(pAnchor))
    {
        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

        while (OHA_IS_NOT_NULL(pElement))
        {
            OHA_ASSERT(pElement->Type == OHA_LLDP_TYPE_ORG_SPECIFIC);

            switch (pElement->SubType)
            {
                case OHA_LLDP_SUB_TYPE_PNO_DELAY:
                case OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS:
                case OHA_LLDP_SUB_TYPE_PNO_PORT_STATUS_MRP:
                case OHA_LLDP_SUB_TYPE_PNO_CHASSIS_MAC_ADR:
                case OHA_LLDP_SUB_TYPE_PNO_PTCP_STATUS:
                    pElement = OhaListNext(pAnchor, &pElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
                    break;

                default:    /* others */
                    /* remove from list and free */
					OHA_ASSERT_NULL_PTR(pElement); /* splint warning */
                    OhaListRemoveEntry(&pElement->ListElem);
                    oha_remove_lldp_DbElement (pElement);
                    pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);    /* reinit. */
                    break;
            }
        }
    }
    else    /* no such element in DB */
    {
        RetVal = LSA_RET_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_delete_rem_lldp_pno_others(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_delete_rem_lldp_orgspec_others
 * function:       deletes the database element
 * parameters:     NicId
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_delete_rem_lldp_orgspec_others(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_delete_rem_lldp_orgspec_others(NicId: 0x%X, PortId: 0x%X, SenderId: 0x%X)",
                          NicId, PortId, SenderId);

    pAnchor = oha_get_rem_lldp_db_element (NicId, PortId, SenderId, OHA_LLDP_TYPE_ORG_SPECIFIC,   /* type of variable */
                                           0, 0);  /* type of variable */
    if (OHA_IS_NOT_NULL(pAnchor))
    {
        oha_delete_lldp_DbElement(pAnchor, LSA_NULL);
#if 0
        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

        while (OHA_IS_NOT_NULL(pElement))
        {
            /* remove from list and free */
            OhaListRemoveEntry(&pElement->ListElem);
            oha_remove_lldp_DbElement (pElement);
            pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);    /* reinit. */
            break;
        }
#endif
    }
    else    /* no such element in DB */
    {
        RetVal = LSA_RET_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_delete_rem_lldp_orgspec_others(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_delete_rem_lldp_unknown_optionals
 * function:       deletes the database element
 * parameters:     NicId
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_delete_rem_lldp_unknown_optionals(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_delete_rem_lldp_unknown_optionals(NicId: 0x%X, PortId: 0x%X, SenderId: 0x%X)",
                          NicId, PortId, SenderId);

    pAnchor = oha_get_rem_lldp_db_element (NicId, PortId, SenderId, OHA_LLDP_TYPE_MGM_ADDR + 1 ,   /* LLDP-types (9...126) */
                                           0, 0);
    if (OHA_IS_NOT_NULL(pAnchor))
    {
        oha_delete_lldp_DbElement(pAnchor, LSA_NULL);
#if 0
        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

        while (OHA_IS_NOT_NULL(pElement))
        {
            /* remove from list and free */
            OhaListRemoveEntry(&pElement->ListElem);
            oha_remove_lldp_DbElement (pElement);
            pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);    /* reinit. */
            break;
        }
#endif
    }
    else    /* no such element in DB */
    {
        RetVal = LSA_RET_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_delete_rem_lldp_unknown_optionals(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_get_rem_lldp_db_element
 * function:       writes the parameters to the database
 * parameters:     Type (type of variable, LLDP-option)
                   SubType (LLDP-suboption)
                   SenderId;  (index + 1 to RemoteSender[] - for SNMP)
 * return value:   pElement:        NULL or <> NULL
 *===========================================================================*/
OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * oha_get_rem_lldp_db_element(
    LSA_UINT16  NicId,
    LSA_UINT16  PortId,
    LSA_UINT16  SenderId,              /* index + 1 to RemoteSender[] - for SNMP */
    LSA_UINT8   Type,
    LSA_UINT8   SubType,
    LSA_UINT32  OUI                    /* OUI, if subtype=127 */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pElement = LSA_NULL;

    OHA_DB_IF_PTR_TYPE  pDbIf;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_get_rem_lldp_db_element(NicId: 0x%X, Type: 0x%X, SubType: 0x%X)",
                          NicId, Type, SubType);
    LSA_UNUSED_ARG(SubType);

    /* ++ check the params ++ check the params ++ */
    if (!(SenderId && (SenderId <= LLDP_MAX_PORT_SENDER)))
    {
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_get_rem_lldp_db_element(pElement: 0x%X)", pElement);
        return (pElement);           /* NULL */
    }

    if ((!PortId) || (PortId > OHA_CFG_MAX_PORTS))
    {
        /* Error on getting port. Abort. */
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_get_rem_lldp_db_element(pElement: 0x%X)", pElement);
        return (pElement);           /* NULL */
    }

    /* ++ check the params ++ check the params ++ */
    if (!Type) /* invalid (sub)option or no element */
    {
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_get_rem_lldp_db_element(pElement: 0x%X)", pElement);
        return (pElement);           /* NULL */
    }

    pDbIf = OHA_DB_GET_INTERFACE(NicId);
    if (!pDbIf)            /* no interface */
    {
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_get_rem_lldp_db_element(pElement: 0x%X)", pElement);
        return (pElement);       /* NULL */
    }

    /* ++ get the db-entry ++ gwt the db-entry ++ */
    switch (Type)
    {
        case OHA_LLDP_TYPE_CHASSIS_ID:
        {
            pElement = &pDbIf->LldpData[PortId-1].RemoteSender[SenderId-1].DbLLDPDU.pChassisId;
        }
        break;
        case OHA_LLDP_TYPE_PORT_ID:
        {
            pElement = &pDbIf->LldpData[PortId-1].RemoteSender[SenderId-1].DbLLDPDU.pPortId;
        }
        break;
        case OHA_LLDP_TYPE_TTL:
        {
            pElement = &pDbIf->LldpData[PortId-1].RemoteSender[SenderId-1].DbLLDPDU.pTTL;
        }
        break;
        case OHA_LLDP_TYPE_PORT_DESCR:
        {
            pElement = &pDbIf->LldpData[PortId-1].RemoteSender[SenderId-1].DbLLDPDU.pPortDescr;
        }
        break;
        case OHA_LLDP_TYPE_SYSTEM_NAME:
        {
            pElement = &pDbIf->LldpData[PortId-1].RemoteSender[SenderId-1].DbLLDPDU.pSystemName;
        }
        break;
        case OHA_LLDP_TYPE_SYSTEM_DESCR:
        {
            pElement = &pDbIf->LldpData[PortId-1].RemoteSender[SenderId-1].DbLLDPDU.pSystemDescr;
        }
        break;
        case OHA_LLDP_TYPE_SYSTEM_CAP:
        {
            pElement = &pDbIf->LldpData[PortId-1].RemoteSender[SenderId-1].DbLLDPDU.pSystemCap;
        }
        break;
        case OHA_LLDP_TYPE_MGM_ADDR:
        {
            pElement = &pDbIf->LldpData[PortId-1].RemoteSender[SenderId-1].DbLLDPDU.pMgmAddr;
        }
        break;
        case OHA_LLDP_TYPE_ORG_SPECIFIC:
        {
            switch (OUI)
            {
                case OHA_LLDP_TYPE_ORG_OUI_IEEE_802_1:  /* IEEE 802.1 */
                    pElement = &pDbIf->LldpData[PortId-1].RemoteSender[SenderId-1].DbLLDPDU.pOrgSpec_802_1;
                    break;
                case OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3:  /* IEEE 802.3 */
                    pElement = &pDbIf->LldpData[PortId-1].RemoteSender[SenderId-1].DbLLDPDU.pOrgSpec_802_3;
                    break;
                case OHA_LLDP_TYPE_ORG_OUI_PNO:         /* IEEE 802.3 */
                    pElement = &pDbIf->LldpData[PortId-1].RemoteSender[SenderId-1].DbLLDPDU.pOrgSpec_Pno;
                    break;
                default:
                    pElement = &pDbIf->LldpData[PortId-1].RemoteSender[SenderId-1].DbLLDPDU.pOrgSpec_Others;
                    break;
            }
        }
        break;
        default:        /* invalid options */
            if (Type > OHA_LLDP_TYPE_MGM_ADDR &&    /* the LLDP-types (9...126) */
                Type < OHA_LLDP_TYPE_ORG_SPECIFIC)
            {
                pElement = &pDbIf->LldpData[PortId-1].RemoteSender[SenderId-1].DbLLDPDU.pUnknown_Optionals;
            }
            break;
    }

    /* only initialized with NULL */
    if (OHA_IS_NOT_NULL(pElement) && OhaListIsNotInitialized(pElement))
    {
        OhaListInitialize(pElement);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_get_rem_lldp_db_element(pElement: 0x%X)", pElement);
    return pElement;
}

/*=============================================================================
 * function name:  oha_db_write_rem_lldp_element
 * function:       writes the parameters to the database (deleted by expiration)
 * parameters:     NicId
                   Type (type of variable, LLDP-option)
                   SubType (LLDP-suboption)
                   pVariable (variable in octets)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_write_rem_lldp_element(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId,   /* index + 1 to RemoteSender[] - for SNMP */
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* subtype of variable */
    LSA_UINT32                  OUI,        /* OUI, if subtype=127 */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable,  /* variable in octets */
    LSA_UINT16                  Length      /* length of pVariable in octets */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_05(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_write_rem_lldp_element(SenderId: 0x%X, Type: 0x%X, SubType: 0x%X, Length: 0x%X, OUI: 0x%X)",
                          SenderId, Type, SubType, Length, OUI);
    /* ++ check the params ++ check the params ++ */
    if ((!Type) || OHA_IS_NULL(pVariable) || (!Length) ) /* invalid (sub)option or no element */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_write_rem_lldp_element(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    if (Length > EDD_NRT_MAX_SND_LEN) /* EDD_IEEE_FRAME_LENGTH */  /* absolute maximum */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_write_rem_lldp_element(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    pAnchor = oha_get_rem_lldp_db_element (NicId, PortId, SenderId, Type,    /* get the anchor */
                                           SubType, OUI);
    if (OHA_IS_NOT_NULL(pAnchor))
    {                           /* find the element */
        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = oha_find_lldp_DbElement(pAnchor, Type, SubType, OUI);
        if (OHA_IS_NULL(pElement))      /* element existing ? */
        {                       /* create it ! */
            pElement = oha_create_lldp_DbElement(Type, SubType, OUI, pVariable, Length);
            if (OHA_IS_NOT_NULL(pElement))
            {
                OhaListInsertTail(pAnchor, &pElement->ListElem);        /* queue the element */
            }
            else
            {
                RetVal = LSA_RET_ERR_PARAM;
            }
        }
        else
        {
            RetVal = oha_update_lldp_DbElement(pElement, Type, SubType, OUI, pVariable, Length);
        }
    }
    else
    {
        RetVal = LSA_RET_ERR_PARAM;
    }
    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_write_rem_lldp_element(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}


/*=============================================================================
 * function name:  oha_db_read_rem_lldp_element
 * function:       reades the parameters of the database element
 * parameters:     NicId
                   Type (type of variable, LLDP-option)
                   SubType (LLDP-suboption)
                   pVariable (variable in octets)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_rem_lldp_element(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId,   /* index + 1 to RemoteSender[] - for SNMP */
    LSA_UINT8                   Type,        /* type of variable, option */
    OHA_COMMON_MEM_U8_PTR_TYPE  pSubType,    /* subtype of variable, suboption */
    LSA_UINT32                  OUI,        /* OUI, if subtype=127 */
    OHA_COMMON_MEM_U8_PTR_TYPE * ppVariable, /* variable in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pLength     /* length of variable in octets */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_05(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_rem_lldp_element(SenderId: 0x%X, Type: 0x%X, pSubType: 0x%X, pLength: 0x%X, OUI: 0x%X)",
                          SenderId, Type, pSubType, pLength, OUI);
    /* ++ check the params ++ check the params ++ */
    if (!Type) /* invalid option */
    {
        *ppVariable = LSA_NULL;
        *pLength    = 0;
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_read_rem_lldp_element(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    pAnchor = oha_get_rem_lldp_db_element (NicId, PortId, SenderId, Type,    /* type of variable */
                                           *pSubType, OUI);
    if (OHA_IS_NOT_NULL(pAnchor))       /* anchor exists */
    {                   /* find the element */
        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = oha_find_lldp_DbElement(pAnchor, Type, *pSubType, OUI);
        if (OHA_IS_NULL(pElement))     /* element does not exist */
        {
            *ppVariable = LSA_NULL;
            *pLength    = 0;
            RetVal = LSA_RET_ERR_PARAM;
        }
        else
        {
            *ppVariable = pElement->pVariable;
            *pLength    = pElement->Length;
            *pSubType   = pElement->SubType;
        }
    }
    else    /* no such anchor in the DB */
    {
        *ppVariable = LSA_NULL;
        *pLength    = 0;
        RetVal = LSA_RET_ERR_PARAM;
    }
    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_rem_lldp_element(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}


/*=============================================================================
 * function name:  oha_get_loc_lldp_db_element
 * function:       writes the parameters to the database
 * parameters:     Type (type of variable, LLDP-option)
                   SubType (LLDP-suboption)
                   SenderId;  (index + 1 to RemoteSender[] - for SNMP)
 * return value:   pElement:        NULL or <> NULL
 *===========================================================================*/
OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * oha_get_loc_lldp_db_element(
    LSA_UINT16  NicId,
    LSA_UINT16  PortId,
    LSA_UINT8   Type,
    LSA_UINT8   SubType,
    LSA_UINT32  OUI        /* OUI, if type=127 */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pElement = LSA_NULL;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_get_loc_lldp_db_element(NicId: 0x%X, Type: 0x%X, SubType: 0x%X)",
                          NicId, Type, SubType);

    LSA_UNUSED_ARG(SubType);
    /* ++ check the params ++ check the params ++ */
    if (!Type) /* invalid (sub)option or no element */
    {
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_get_loc_lldp_db_element(pElement: 0x%X, Type: 0x%X)", pElement, Type);
        return (pElement);           /* NULL */
    }

    /* check portID */
    if (OHA_LldpIsPortOption(Type, PortId))
    {
        if (PortId == 0 || (PortId > OHA_CFG_MAX_PORTS))
        {
            /* Error on getting port. Abort. */
            OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_get_loc_lldp_db_element(pElement: 0x%X, PortId:  0x%X)", pElement, PortId);
            return (pElement);           /* NULL */
        }
    }

    /* device-global options needs no Interface (Nic independent) */
	if (Type == OHA_LLDP_TYPE_CHASSIS_ID)          /* NameOfDevice       */
	{
		pElement = &oha_database.StationData.pChassisId;
	}
    else if (Type == OHA_LLDP_TYPE_SYSTEM_NAME)          /* Option 5 System Name */
    {
        pElement = &oha_database.StationData.pSysName;
    }
    else if (Type == OHA_LLDP_TYPE_SYSTEM_DESCR)    /* Option 6 SystemDescription */
    {
        pElement = &oha_database.StationData.pSysDescr;
    }
    else if (Type == OHA_LLDP_TYPE_SYSTEM_CAP)      /* Option 7 System Capabilities */
    {
        pElement = &oha_database.StationData.pSysCap;
    }
    else
    {
        OHA_DB_IF_PTR_TYPE  pDbIf = OHA_DB_GET_INTERFACE(NicId);
        if (OHA_IS_NULL(pDbIf))            /* no interface */
        {
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_get_loc_lldp_db_element(pElement: 0x%X)", pElement);
            return (pElement);       /* NULL */
        }

        /* ++ get the db-entry ++ gwt the db-entry ++ */
        switch (Type)
        {
            case OHA_LLDP_TYPE_PORT_ID:             /* Option 2 Port ID    */
            {
                pElement = &pDbIf->LldpData[PortId-1].pPortId;
            }
            break;
            case OHA_LLDP_TYPE_TTL:                 /* Option 3 TimeToLive */
            {
                pElement = &pDbIf->pTTL;            /* TTL is in DetailPtr */
            }
            break;
			/* The Portstrings (port_id = 1...n) and the string for the IF (port_id = 0) is supported too! */ 
            case OHA_LLDP_TYPE_PORT_DESCR:          /* Option 4 Port Description */
            {
				if (PortId == 0)
				{
					pElement = &pDbIf->pIfDescr_0;
				}
				else
				{
	                pElement = &pDbIf->LldpData[PortId-1].pIfDescr; /* ifDescr */
				}
            }
            break;
            case OHA_LLDP_TYPE_MGM_ADDR:            /* Optiion 8 Management Address */
            {
                pElement = &pDbIf->pMgmAddr;        /* IP-address */
            }
            break;
            case OHA_LLDP_TYPE_ORG_SPECIFIC:
            {
                switch (OUI)
                {
                    case OHA_LLDP_TYPE_ORG_OUI_IEEE_802_1:  /* IEEE 802.1 */
                        pElement = &pDbIf->LldpData[PortId-1].pOrgSpec_802_1;
                        break;
                    case OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3:  /* IEEE 802.3 */
                        pElement = &pDbIf->LldpData[PortId-1].pOrgSpec_802_3;
                        break;
                    case OHA_LLDP_TYPE_ORG_OUI_PNO:         /* PNO */
                        pElement = &pDbIf->LldpData[PortId-1].pOrgSpec_Pno;
                        break;
                    default:
                        pElement = &pDbIf->LldpData[PortId-1].pOrgSpec_Others;
                        break;
                }
            }
            break;
            default:        /* invalid options */
                if (Type > OHA_LLDP_TYPE_MGM_ADDR &&    /* the LLDP-types (9...126) */
                    Type < OHA_LLDP_TYPE_ORG_SPECIFIC)
                {
                    pElement = &pDbIf->LldpData[PortId-1].pUnknown_Optionals;
                }
                break;
        }
    }

    /* only initialized with NULL */
    if (OHA_IS_NOT_NULL(pElement) && OhaListIsNotInitialized(pElement))
    {
        OhaListInitialize(pElement);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_get_loc_lldp_db_element(pElement: 0x%X)", pElement);
    return pElement;
}

/*=============================================================================
 * function name:  oha_db_lldp_write_remanence
 * function:       writes the parameter RemaStatus to the database-element
 * parameters:     NicId
                   Type (type of variable, LLDP-option)
                   SubType (LLDP-suboption)
                   RemaStatus (LSA_TRUE, LSA_FALSE)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 oha_db_lldp_write_remanence(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* type of variable */
    LSA_UINT32                  OUI,        /* OUI, if type=127 */
    LSA_BOOL                   RemaStatus  /* rema for variable */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = LSA_RET_OK;
    LSA_BOOL Remanence = (RemaStatus) ? LSA_TRUE : LSA_FALSE;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_lldp_write_remanence(Type: 0x%X, SubType: 0x%X, RemaStatus: 0x%X)",
                          Type, SubType, RemaStatus);

    /* ++ check the params ++ check the params ++ */
    if (!Type) /* invalid (sub)option or no element */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_lldp_write_remanence(NicId: 0x%X, Type: 0x%X)", NicId, Type);
        return (RetVal);
    }

    /* check portID */
    if (OHA_LldpIsPortOption(Type, PortId))
    {
        if (PortId == 0 || (PortId > OHA_CFG_MAX_PORTS))
        {
            /* Error on getting port. Abort. */
	        RetVal = LSA_RET_ERR_PARAM;
            OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_lldp_write_remanence(NicId: 0x%X, PortId:  0x%X)", NicId, PortId);
            return (RetVal);
        }
    }

    pAnchor = oha_get_loc_lldp_db_element (NicId, PortId, Type, SubType, OUI);    /* type of variable */
    if (OHA_IS_NOT_NULL(pAnchor))
    {
		/* find the element */		
        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = oha_find_lldp_DbElement(pAnchor, Type, SubType, OUI);

        if (OHA_IS_NOT_NULL(pElement))     /* element existing already ? */
        {
            (pElement)->Remanent = Remanence;
        }
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_lldp_write_remanence(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_lldp_read_remanence
 * function:       reades the RemaStatus from the database element
 * parameters:     NicId
                   Type (type of variable, LLDP-option)
                   SubType (LLDP-suboption)
                   pVariable (variable in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_lldp_read_remanence(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT8                   Type,       /* type of variable, option */
    OHA_COMMON_MEM_U8_PTR_TYPE  pSubType,   /* subtype of variable, suboption */
    LSA_UINT32                  OUI,        /* OUI, if subtype=127 */
    LSA_BOOL                  * RemaStatus  /* rema for variable (LSA_TRUE, LSA_FALSE) */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_lldp_read_remanence(Type: 0x%X, pSubType: 0x%X, RemaStatus: 0x%X, OUI: 0x%X)",
                          Type, pSubType, RemaStatus, OUI);
    /* ++ check the params ++ check the params ++ */
    if (!Type) /* invalid option */
    {
        *RemaStatus = 0;
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_lldp_read_remanence(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    pAnchor = oha_get_loc_lldp_db_element (NicId, PortId, Type,   /* type of variable */
                                           *pSubType, OUI);    /* type of variable */
    if (OHA_IS_NOT_NULL(pAnchor))
    {                       /* find the element */
        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = oha_find_lldp_DbElement(pAnchor, Type, *pSubType, OUI);
        if (OHA_IS_NULL(pElement))     /* element does not exist */
        {
            *RemaStatus = 0;
            RetVal = LSA_RET_ERR_RESOURCE; /* changed from LSA_RET_ERR_PARAM */
        }
        else
        {
			*RemaStatus = pElement->Remanent;
        }
    }
    else    /* no such element in DB */
    {
        *RemaStatus = 0;
        RetVal = LSA_RET_ERR_PARAM;
    }
    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_lldp_read_remanence(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}


/*=============================================================================
 * function name:  oha_db_lldp_compare_remanence
 * function:       reades the RemaStatus from the database element
                   and compares it with the given RemaStatus
 * parameters:     NicId
                   Type (type of variable, LLDP-option)
                   SubType (LLDP-suboption)
 * return value:   LSA_FALSE:   remanence is different
                   LSA_TRUE:    remanence is identical
 *===========================================================================*/
LSA_BOOL OHA_SYSTEM_OUT_FCT_ATTR oha_db_lldp_compare_remanence(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT8                   Type,       /* type of variable, option */
    OHA_COMMON_MEM_U8_PTR_TYPE  pSubType,   /* subtype of variable, suboption */
    LSA_UINT32                  OUI,        /* OUI, if subtype=127 */
    LSA_BOOL                    RemaStatus
)
{
    LSA_BOOL			IsDifferent;
	LSA_BOOL			Remanence = LSA_FALSE;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_lldp_compare_remanence(Type: 0x%X, pSubType: 0x%X, RemaStatus: 0x%X, OUI: 0x%X)",
                          Type, pSubType, RemaStatus, OUI);

	(LSA_UINT16)oha_db_lldp_read_remanence(NicId, PortId, Type, pSubType, OUI, &Remanence);

	OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,"oha_db_lldp_compare_remanence: Remanence(0x%X) read",Remanence);

	if((RemaStatus && Remanence) ||	/* both True */
		((!RemaStatus) && (!Remanence)))
	{
		IsDifferent = LSA_FALSE;
	}
	else
	{
		IsDifferent = LSA_TRUE;
	}

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_lldp_compare_remanence(NicId: 0x%X, IsDifferent: 0x%X)", NicId, IsDifferent);
    return IsDifferent;
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_LLDP_ELEMENT
 * function:       writes the local parameters to the database
 * parameters:     NicId
                   Type (type of variable, LLDP-option)
                   SubType (LLDP-suboption)
                   pVariable (variable in octets)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_LLDP_ELEMENT(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT8                   Type,       /* type of variable */
    LSA_UINT8                   SubType,    /* subtype of variable */
    LSA_UINT32                  OUI,        /* OUI, if type=127 */
    OHA_COMMON_MEM_U8_PTR_TYPE  pVariable,  /* variable in octets */
    LSA_UINT16                  Length      /* length of pVariable in octets */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;

    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_LLDP_ELEMENT(Type: 0x%X, SubType: 0x%X, Length: 0x%X, OUI: 0x%X)",
                          Type, SubType, Length, OUI);
    /* ++ check the params ++ check the params ++ */
    if (!Type) /* invalid (sub)option or no element */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_LLDP_ELEMENT(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    if (Length > EDD_NRT_MAX_SND_LEN) /* EDD_IEEE_FRAME_LENGTH */  /* absolute maximum */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_LLDP_ELEMENT(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    pAnchor = oha_get_loc_lldp_db_element (NicId, PortId, Type,    /* get the anchor */
                                           SubType, OUI);
    if (OHA_IS_NOT_NULL(pAnchor))
    {                       /* find the element */
        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = oha_find_lldp_DbElement(pAnchor, Type, SubType, OUI);
        if (OHA_IS_NULL(pElement))     /* element existing ? */
        {                      /* create it ! */
            pElement = oha_create_lldp_DbElement(Type, SubType, OUI, pVariable, Length);
            if (OHA_IS_NOT_NULL(pElement))
            {
                OhaListInsertTail(pAnchor, &pElement->ListElem);        /* queue the element */
            }
            else
            {
                RetVal = LSA_RET_ERR_PARAM;
            }
        }
        else
        {
            RetVal = oha_update_lldp_DbElement(pElement, Type, SubType, OUI, pVariable, Length);
        }
    }
    else
    {
        RetVal = LSA_RET_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_WRITE_LLDP_ELEMENT(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  OHA_DB_READ_LLDP_ELEMENT
 * function:       reades the parameters of the database element
 * parameters:     NicId
                   Type (type of variable, LLDP-option)
                   SubType (LLDP-suboption)
                   pVariable (variable in octets)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_LLDP_ELEMENT(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT8                   Type,        /* type of variable, option */
    OHA_COMMON_MEM_U8_PTR_TYPE  pSubType,    /* subtype of variable, suboption */
    LSA_UINT32                  OUI,        /* OUI, if subtype=127 */
    OHA_COMMON_MEM_U8_PTR_TYPE * ppVariable, /* variable in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pLength     /* length of variable in octets */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_loc_lldp_element(Type: 0x%X, pSubType: 0x%X, pLength: 0x%X, OUI: 0x%X)",
                          Type, pSubType, pLength, OUI);
    /* ++ check the params ++ check the params ++ */
    if (!Type) /* invalid option */
    {
        *ppVariable = 0;
        *pLength    = 0;
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_read_loc_lldp_element(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    pAnchor = oha_get_loc_lldp_db_element (NicId, PortId, Type,   /* type of variable */
                                           *pSubType, OUI);    /* type of variable */
    if (OHA_IS_NOT_NULL(pAnchor))
    {                       /* find the element */
        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = oha_find_lldp_DbElement(pAnchor, Type, *pSubType, OUI);
        if (OHA_IS_NULL(pElement))     /* element does not exist */
        {
            *ppVariable = 0;
            *pLength    = 0;
            RetVal = LSA_RET_ERR_RESOURCE; /* changed from LSA_RET_ERR_PARAM */
        }
        else
        {
            *ppVariable = pElement->pVariable;
            *pLength    = pElement->Length;
            *pSubType   = pElement->SubType;
        }
    }
    else    /* no such element in DB */
    {
        *ppVariable = 0;
        *pLength    = 0;
        RetVal = LSA_RET_ERR_PARAM;
    }
    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_loc_lldp_element(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_delete_loc_lldp_element
 * function:       deletes the database element
 * parameters:     NicId
                   Type (type of variable, LLDP-option)
                   SubType (LLDP-suboption)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_delete_loc_lldp_element(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT8                   Type,       /* type of variable, option */
    LSA_UINT8                   SubType,    /* subtype of variable, suboption */
    LSA_UINT32                  OUI         /* OUI, if subtype=127 */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_delete_loc_lldp_element(Type: 0x%X, SubType: 0x%X, OUI: 0x%X)",
                          Type, SubType, OUI);
    /* ++ check the params ++ check the params ++ */
    if (!Type) /* invalid option */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_delete_loc_lldp_element(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    pAnchor = oha_get_loc_lldp_db_element (NicId, PortId, Type,   /* type of variable */
                                           SubType, OUI);  /* type of variable */
    if (OHA_IS_NOT_NULL(pAnchor))
    {                       /* find the element */
        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = oha_find_lldp_DbElement(pAnchor, Type, SubType, OUI);
        if (OHA_IS_NOT_NULL(pElement))     /* element exists */
        {
            /* remove from list and free */
            OhaListRemoveEntry(&pElement->ListElem);
            oha_remove_lldp_DbElement (pElement);
        }
    }
    else    /* no such element in DB */
    {
        RetVal = LSA_RET_ERR_PARAM;
    }
    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_delete_loc_lldp_element(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_delete_rem_lldp_element
 * function:       deletes the database element
 * parameters:     NicId
                   Type (type of variable, LLDP-option)
                   SubType (LLDP-suboption)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_delete_rem_lldp_element(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId,
    LSA_UINT8                   Type,       /* type of variable, option */
    LSA_UINT8                   SubType,    /* subtype of variable, suboption */
    LSA_UINT32                  OUI         /* OUI, if subtype=127 */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_delete_rem_lldp_element(Type: 0x%X, SubType: 0x%X, OUI: 0x%X)",
                          Type, SubType, OUI);

    /* ++ check the params ++ check the params ++ */
    if (!Type) /* invalid option */
    {
        RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_delete_rem_lldp_element(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
        return RetVal;
    }

    pAnchor = oha_get_rem_lldp_db_element (NicId, PortId, SenderId, Type,   /* type of variable */
                                           SubType, OUI);  /* type of variable */
    if (OHA_IS_NOT_NULL(pAnchor))
    {
        /* find the element */
        OHA_DB_LLDP_ELEM_PTR_TYPE pElement = oha_find_lldp_DbElement(pAnchor, Type, SubType, OUI);
        if (OHA_IS_NOT_NULL(pElement))     /* element exists */
        {
            OHA_HDB_LLDP_PTR_TYPE pHDBLldp = OHA_LLDPGetHandleByNIC(NicId);
            OHA_ASSERT_NULL_PTR(pHDBLldp);

            /* remove from list and free */
            OhaListRemoveEntry(&pElement->ListElem);
            oha_remove_lldp_DbElement (pElement);

            /* count statistics */
            if (OHA_IS_NOT_NULL(pHDBLldp->pDB))
            {
                if (PortId && (PortId <= pHDBLldp->pDB->PortCnt))
                {
                    pHDBLldp->pDB->LldpData[PortId-1].PortStats.RxPortTLVsDiscardedTotal++;
                }
            }
        }
    }
    else    /* no such element in DB */
    {
        RetVal = LSA_RET_ERR_PARAM;
    }

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_rem_lldp_element(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestReadLldpOptions                  +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                        :    OHA_HDB_PTR_TYPE            pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_LLDP_READ_OPTIONS      +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.LldpElem                               +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+     Type   OUI   SubType    Usage                                       +*/
/*+                                                                         +*/
/*+        0    -       -       All TLVs of the frame                       +*/
/*+     1..8    -       0       All TLVs of the frame with this Type        +*/
/*+     1..8    -    1..x       All TLVs of the frame with this Type/SubType+*/
/*+      127   <>0      0       All TLVs with this OUI                      +*/
/*+      127   <>0   1..x       All TLVs with this OUI and this SubType     +*/
/*+   9..126    -       -       Error!                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to read local lldp parameters from the database   +*/
/*+                                                                         +*/
/*+               Fills RQB with station data and confirms request.         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestReadLldpOptions(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    OHA_HDB_EDD_PTR_TYPE      pHDBEdd;
    LSA_UINT16                Response;
    OHA_LLDP_OPTIONS_PTR_TYPE pLldpOptions;
    LSA_UINT16                NicId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestReadLldpOptions(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);
    OHA_ASSERT_NULL_PTR(pRQB);

    pLldpOptions = &pRQB->args.LldpOptions;
    OHA_ASSERT_NULL_PTR(pLldpOptions);

	NicId = pHDB->Params.pDetail->Params.User.NICId;

    /* Check if we have a EDD-Channel for NIC-ID. */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NicId); /* only EDD channel for this request necessary */

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) ||
         LSA_HOST_PTR_ARE_EQUAL(pLldpOptions->pElements, LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;  /* Mode is wrong */
        pLldpOptions->Count = 0;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_READ_OPTIONS failed. No Lower Channel for this NicId (NIC-ID: 0x%X)", NicId);
    }
    else if ((!pLldpOptions->PortId) || (pLldpOptions->PortId > OHA_CFG_MAX_PORTS))
    {
        Response = OHA_ERR_PARAM;  /* Mode is wrong */
        pLldpOptions->Count = 0;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_READ_OPTIONS failed. PortID is wrong (PORT-ID: 0x%X)", pLldpOptions->PortId);
    }
    else if (pLldpOptions->Type == OHA_LLDP_TYPE_ORG_SPECIFIC && /* check the LLDP-type */
             pLldpOptions->OUI == 0)
    {
        Response = OHA_ERR_PARAM;  /* OUI is wrong */
        pLldpOptions->Count = 0;
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_READ_OPTIONS failed. OUI is wrong (OUI: 0x%X)", pLldpOptions->OUI);
    }
    else
    {
        OHA_DB_LLDP_ELEM_PTR_TYPE pUserList = (pLldpOptions->Count) ? pLldpOptions->pElements : LSA_NULL; /* array (list) of LLDP elements */

        LSA_UINT16      Count       = 0;
        LSA_UINT16      WriteLength = 0;

        Response = OHA_ERR_FAILED;

        if (pLldpOptions->Type == 0)    /* all elements selected */
        {
            pLldpOptions->SubType = 0;  /* set the initial value */
            pLldpOptions->OUI = 0;      /* set the initial value */
        }

        /*********************************************************/
        /* is only one type (1...8) or 127 with subtype to read ? */
        /*********************************************************/
        if (pLldpOptions->Type && pLldpOptions->SubType)
        {
            LSA_UINT8 SubType = pLldpOptions->SubType; /* subtype of variable, suboption */
            OHA_COMMON_MEM_U8_PTR_TYPE pVariable = LSA_NULL; /* variable in octets */
            LSA_UINT16 VarLength = 0;                  /* length of variable in octets */
            if (LSA_RET_OK == OHA_DB_READ_LLDP_ELEMENT(NicId,
                                                       pLldpOptions->PortId,
                                                       pLldpOptions->Type, &SubType,
                                                       pLldpOptions->OUI,
                                                       &pVariable, &VarLength))
            {
                if (SubType == pLldpOptions->SubType)
                {
                    Response = OHA_OK;
                    Count++;

                    if (OHA_IS_NOT_NULL(pUserList))
                    {
                        OHA_MEMSET(pUserList, 0, sizeof(OHA_DB_LLDP_ELEM_TYPE));
                        pUserList->Type     = pLldpOptions->Type;
                        pUserList->SubType  = pLldpOptions->SubType;
                        pUserList->OUI      = pLldpOptions->OUI;

                        if (OHA_IS_NOT_NULL(pLldpOptions->pVarBuffer)) /* the global provided user-buffer */
                        {
                            LSA_UINT16 Length = VarLength;
							OHA_COMMON_MEM_U8_PTR_TYPE pBuffer = &pLldpOptions->pVarBuffer[WriteLength];

                            if (Length & 1) Length++;       /* even address */
                            WriteLength += Length;
                            if (OHA_IS_NOT_NULL(pVariable) && VarLength && (WriteLength <= LLDP_MAX_TLV_DATA_SIZE))
                            {
                                pUserList->pVariable = pBuffer; /* first entry in pBuffer */
                                pUserList->Length    = VarLength;
                                OHA_MEMCPY(pUserList->pVariable, pVariable, pUserList->Length);
                            }
                        }
                    }
                    /* is only one type! */
                    /* if (OHA_IS_NOT_NULL(pUserList) && (Count < pLldpOptions->Count)) */
                    /* {                */
                    /*  pUserList++;    */
                    /* }                */
                }
            }

            pLldpOptions->Count = Count;
            OHA_UserRequestFinish(pHDB,pRQB,Response);
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadLldpOptions(Response: 0x%X)", Response);
            return; /*  ==========>>>>>>>>>>>>  */
        }

        /*********************************************************/
        /* all the types (1...8) are to read (subtype = 0) ?     */
        /* for the types (1...8) there's only one subtype: only one element is to read */
        /*********************************************************/
        if (pLldpOptions->Type <= OHA_LLDP_TYPE_MGM_ADDR)
        {
            LSA_UINT8  Type    = (pLldpOptions->Type) ? pLldpOptions->Type : (LSA_UINT8)OHA_LLDP_TYPE_CHASSIS_ID;

            for (; Type <= OHA_LLDP_TYPE_MGM_ADDR; Type++)
            {
                OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;

                pAnchor = oha_get_loc_lldp_db_element (NicId, pLldpOptions->PortId, Type,    /* type of variable */
                                                       1, 1);      /* should not be zero */
                if (OHA_IS_NOT_NULL(pAnchor))       /* anchor exists */
                {
                    OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

                    if (OHA_IS_NOT_NULL(pElement))
                    {
                        Response = OHA_OK;
                        Count++;

                        if (OHA_IS_NOT_NULL(pUserList))
                        {
                            OHA_MEMSET(pUserList, 0, sizeof(OHA_DB_LLDP_ELEM_TYPE));
                            pUserList->Type     = pElement->Type;
                            pUserList->SubType  = pElement->SubType;
                            pUserList->OUI      = pElement->OUI;
                            if (OHA_IS_NOT_NULL(pLldpOptions->pVarBuffer)) /* the global provided user-buffer */
                            {
                                LSA_UINT16 Length  = pElement->Length;
								OHA_COMMON_MEM_U8_PTR_TYPE pBuffer = &pLldpOptions->pVarBuffer[WriteLength];

                                if (Length & 1) Length++;       /* even address */
                                WriteLength += Length;
                                if (OHA_IS_NOT_NULL(pElement->pVariable) && pElement->Length && (WriteLength <= LLDP_MAX_TLV_DATA_SIZE))
                                {
                                    pUserList->pVariable = pBuffer; /* first entry in pBuffer */
                                    pUserList->Length    = pElement->Length;
                                    OHA_MEMCPY(pUserList->pVariable, pElement->pVariable, pUserList->Length);
                                }
                            }
                        }
                        if (OHA_IS_NOT_NULL(pUserList) && (Count < pLldpOptions->Count))
                        {
                            pUserList++;
                        }
                    }
                }

                if (pLldpOptions->Type)     /* only one element is to read ? */
                {
                    break;                  /* that's it */
                }
            }
        }

        /*********************************************************/
        /* all the types (9...126) are to read (subtype = 0) ?   */
        /*********************************************************/
        if (pLldpOptions->Type == 0)
        {
            OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;

            pAnchor = oha_get_loc_lldp_db_element (NicId, pLldpOptions->PortId,
                                                   OHA_LLDP_TYPE_MGM_ADDR+1,  /* type of variable */
                                                   1, 1);      /* should not be zero */
            if (OHA_IS_NOT_NULL(pAnchor))       /* anchor exists */
            {
                OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

                while (OHA_IS_NOT_NULL(pElement))
                {
                    Response = OHA_OK;
                    Count++;

                    if (OHA_IS_NOT_NULL(pUserList))
                    {
                        OHA_MEMSET(pUserList, 0, sizeof(OHA_DB_LLDP_ELEM_TYPE));
                        pUserList->Type     = pElement->Type;
                        pUserList->SubType  = pElement->SubType;
                        pUserList->OUI      = pElement->OUI;
                        if (OHA_IS_NOT_NULL(pLldpOptions->pVarBuffer)) /* the global provided user-buffer */
                        {
                            LSA_UINT16 Length  = pElement->Length;
							OHA_COMMON_MEM_U8_PTR_TYPE pBuffer = &pLldpOptions->pVarBuffer[WriteLength];

                            if (Length & 1) Length++;       /* even address */
                            WriteLength += Length;
                            if (OHA_IS_NOT_NULL(pElement->pVariable) && pElement->Length && (WriteLength <= LLDP_MAX_TLV_DATA_SIZE))
                            {
                                pUserList->pVariable = pBuffer; /* first entry in pBuffer */
                                pUserList->Length    = pElement->Length;
                                OHA_MEMCPY(pUserList->pVariable, pElement->pVariable, pUserList->Length);
                            }
                        }
                    }
                    if (OHA_IS_NOT_NULL(pUserList) && (Count < pLldpOptions->Count))
                    {
                        pUserList++;
                    }

                    pElement = OhaListNext(pAnchor, &pElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
                }
            }
        }

        /*********************************************************/
        /******** type 127, init the loop for the OUI 802.1 ******/
        /*********************************************************/
        if ((pLldpOptions->Type == OHA_LLDP_TYPE_ORG_SPECIFIC && pLldpOptions->OUI == OHA_LLDP_TYPE_ORG_OUI_IEEE_802_1) ||
            pLldpOptions->Type == 0)
        {
            LSA_UINT8  Type    = (pLldpOptions->Type) ? pLldpOptions->Type : (LSA_UINT8)OHA_LLDP_TYPE_ORG_SPECIFIC;
            LSA_UINT32  OUI    = (pLldpOptions->OUI) ? (pLldpOptions->OUI) : OHA_LLDP_TYPE_ORG_OUI_IEEE_802_1;
            OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;

            pAnchor = oha_get_loc_lldp_db_element (NicId, pLldpOptions->PortId, Type,    /* type of variable */
                                                   1, OUI);
            if (OHA_IS_NOT_NULL(pAnchor))       /* anchor exists */
            {
                OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

                while (OHA_IS_NOT_NULL(pElement))
                {
                    Response = OHA_OK;
                    Count++;

                    if (OHA_IS_NOT_NULL(pUserList))
                    {
                        OHA_MEMSET(pUserList, 0, sizeof(OHA_DB_LLDP_ELEM_TYPE));
                        pUserList->Type     = pElement->Type;
                        pUserList->SubType  = pElement->SubType;
                        pUserList->OUI      = pElement->OUI;
                        if (OHA_IS_NOT_NULL(pLldpOptions->pVarBuffer)) /* the global provided user-buffer */
                        {
                            LSA_UINT16 Length  = pElement->Length;
							OHA_COMMON_MEM_U8_PTR_TYPE pBuffer = &pLldpOptions->pVarBuffer[WriteLength];

                            if (Length & 1) Length++;       /* even address */
                            WriteLength += Length;
                            if (OHA_IS_NOT_NULL(pElement->pVariable) && pElement->Length && (WriteLength <= LLDP_MAX_TLV_DATA_SIZE))
                            {
                                pUserList->pVariable = pBuffer; /* first entry in pBuffer */
                                pUserList->Length    = pElement->Length;
                                OHA_MEMCPY(pUserList->pVariable, pElement->pVariable, pUserList->Length);
                            }
                        }
                    }
                    if (OHA_IS_NOT_NULL(pUserList) && (Count < pLldpOptions->Count))
                    {
                        pUserList++;
                    }
                    pElement = OhaListNext(pAnchor, &pElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
                }
            }
        }

        /*********************************************************/
        /******** type 127, init the loop for the OUI 802.3 ******/
        /*********************************************************/
        if ((pLldpOptions->Type == OHA_LLDP_TYPE_ORG_SPECIFIC && pLldpOptions->OUI == OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3) ||
            pLldpOptions->Type == 0)
        {
            LSA_UINT8  Type    = (pLldpOptions->Type) ? pLldpOptions->Type : (LSA_UINT8)OHA_LLDP_TYPE_ORG_SPECIFIC;
            LSA_UINT32 OUI     = (pLldpOptions->OUI) ? (pLldpOptions->OUI) : OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3;

            OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;

            pAnchor = oha_get_loc_lldp_db_element (NicId, pLldpOptions->PortId, Type,    /* type of variable */
                                                   1, OUI);
            if (OHA_IS_NOT_NULL(pAnchor))       /* anchor exists */
            {
                OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

                while (OHA_IS_NOT_NULL(pElement))
                {
                    Response = OHA_OK;
                    Count++;

                    if (OHA_IS_NOT_NULL(pUserList))
                    {
                        OHA_MEMSET(pUserList, 0, sizeof(OHA_DB_LLDP_ELEM_TYPE));
                        pUserList->Type     = pElement->Type;
                        pUserList->SubType  = pElement->SubType;
                        pUserList->OUI      = pElement->OUI;
                        if (OHA_IS_NOT_NULL(pLldpOptions->pVarBuffer)) /* the global provided user-buffer */
                        {
                            LSA_UINT16 Length  = pElement->Length;
							OHA_COMMON_MEM_U8_PTR_TYPE pBuffer = &pLldpOptions->pVarBuffer[WriteLength];

                            if (Length & 1) Length++;       /* even address */
                            WriteLength += Length;
                            if (OHA_IS_NOT_NULL(pElement->pVariable) && pElement->Length && (WriteLength <= LLDP_MAX_TLV_DATA_SIZE))
                            {
                                pUserList->pVariable = pBuffer; /* first entry in pBuffer */
                                pUserList->Length    = pElement->Length;
                                OHA_MEMCPY(pUserList->pVariable, pElement->pVariable, pUserList->Length);
                            }
                        }
                    }
                    if (OHA_IS_NOT_NULL(pUserList) && (Count < pLldpOptions->Count))
                    {
                        pUserList++;
                    }
                    pElement = OhaListNext(pAnchor, &pElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
                }
            }
        }

        /*********************************************************/
        /******** type 127, init the loop for the OUI PNO   ******/
        /*********************************************************/
        if ((pLldpOptions->Type == OHA_LLDP_TYPE_ORG_SPECIFIC && pLldpOptions->OUI == OHA_LLDP_TYPE_ORG_OUI_PNO) ||
            pLldpOptions->Type == 0)
        {
            LSA_UINT8  Type    = (pLldpOptions->Type) ? pLldpOptions->Type : (LSA_UINT8)OHA_LLDP_TYPE_ORG_SPECIFIC;
            LSA_UINT32 OUI     = (pLldpOptions->OUI) ? (pLldpOptions->OUI) : OHA_LLDP_TYPE_ORG_OUI_PNO;

            OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;

            pAnchor = oha_get_loc_lldp_db_element (NicId, pLldpOptions->PortId, Type,    /* type of variable */
                                                   1, OUI);
            if (OHA_IS_NOT_NULL(pAnchor))       /* anchor exists */
            {
                OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

                while (OHA_IS_NOT_NULL(pElement))
                {
                    Response = OHA_OK;
                    Count++;

                    if (OHA_IS_NOT_NULL(pUserList))
                    {
                        OHA_MEMSET(pUserList, 0, sizeof(OHA_DB_LLDP_ELEM_TYPE));
                        pUserList->Type     = pElement->Type;
                        pUserList->SubType  = pElement->SubType;
                        pUserList->OUI      = pElement->OUI;
                        if (OHA_IS_NOT_NULL(pLldpOptions->pVarBuffer)) /* the global provided user-buffer */
                        {
                            LSA_UINT16 Length  = pElement->Length;
							OHA_COMMON_MEM_U8_PTR_TYPE pBuffer = &pLldpOptions->pVarBuffer[WriteLength];

                            if (Length & 1) Length++;       /* even address */
                            WriteLength += Length;
                            if (OHA_IS_NOT_NULL(pElement->pVariable) && pElement->Length && (WriteLength <= LLDP_MAX_TLV_DATA_SIZE))
                            {
                                pUserList->pVariable = pBuffer; /* first entry in pBuffer */
                                pUserList->Length    = pElement->Length;
                                OHA_MEMCPY(pUserList->pVariable, pElement->pVariable, pUserList->Length);
                            }
                        }
                    }
                    if (OHA_IS_NOT_NULL(pUserList) && (Count < pLldpOptions->Count))
                    {
                        pUserList++;
                    }
                    pElement = OhaListNext(pAnchor, &pElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
                }
            }
        }

        /*********************************************************/
        /******** type 127, init the loop for other OUI's   ******/
        /*********************************************************/
        if ((pLldpOptions->Type == OHA_LLDP_TYPE_ORG_SPECIFIC && (pLldpOptions->OUI != OHA_LLDP_TYPE_ORG_OUI_IEEE_802_1 &&
                                                                  pLldpOptions->OUI != OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3)) ||
            pLldpOptions->Type == 0)
        {
            LSA_UINT8  Type    = (pLldpOptions->Type) ? pLldpOptions->Type : (LSA_UINT8)OHA_LLDP_TYPE_ORG_SPECIFIC;
            LSA_UINT32 OUI     = (pLldpOptions->OUI) ? (pLldpOptions->OUI) : 1;
            OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;

            pAnchor = oha_get_loc_lldp_db_element (NicId, pLldpOptions->PortId, Type,    /* type of variable */
                                                   1, OUI);
            if (OHA_IS_NOT_NULL(pAnchor))       /* anchor exists */
            {
                OHA_DB_LLDP_ELEM_PTR_TYPE pElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

                while (OHA_IS_NOT_NULL(pElement))
                {
                    if (pLldpOptions->Type == 0 ||      /* get all elements */
                        (/* pLldpOptions->Type &&*/ (pLldpOptions->OUI == pElement->OUI)) ) /* or only this OUI */
                    {
                        Response = OHA_OK;
                        Count++;

                        if (OHA_IS_NOT_NULL(pUserList))
                        {
                            OHA_MEMSET(pUserList, 0, sizeof(OHA_DB_LLDP_ELEM_TYPE));
                            pUserList->Type     = pElement->Type;
                            pUserList->SubType  = pElement->SubType;
                            pUserList->OUI      = pElement->OUI;
                            if (OHA_IS_NOT_NULL(pLldpOptions->pVarBuffer)) /* the global provided user-buffer */
                            {
                                LSA_UINT16 Length  = pElement->Length;
								OHA_COMMON_MEM_U8_PTR_TYPE pBuffer = &pLldpOptions->pVarBuffer[WriteLength];

                                if (Length & 1) Length++;       /* even address */
                                WriteLength += Length;
                                if (OHA_IS_NOT_NULL(pElement->pVariable) && pElement->Length && (WriteLength <= LLDP_MAX_TLV_DATA_SIZE))
                                {
                                    pUserList->pVariable = pBuffer; /* first entry in pBuffer */
                                    pUserList->Length    = pElement->Length;
                                    OHA_MEMCPY(pUserList->pVariable, pElement->pVariable, pUserList->Length);
                                }
                            }
                        }
                        if (OHA_IS_NOT_NULL(pUserList) && (Count < pLldpOptions->Count))
                        {
                            pUserList++;
                        }
                    }
                    pElement = OhaListNext(pAnchor, &pElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
                }
            }
        }

        pLldpOptions->Count = Count;
    }

    OHA_UserRequestFinish(pHDB,pRQB,Response);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestReadLldpOptions(Response: 0x%X)", Response);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestWriteLldpOption                  +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                        :    OHA_HDB_PTR_TYPE            pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_LLDP_WRITE_OPTION      +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.LldpOption                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to write local lldp parameters from the database  +*/
/*+                                                                         +*/
/*+               Fills DataBase with local data and confirms request.      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestWriteLldpOption(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    OHA_HDB_EDD_PTR_TYPE      pHDBEdd;
    LSA_UINT16                Response = OHA_OK;
    OHA_LLDP_OPTION_PTR_TYPE  pLldpOption;
    LSA_UINT16                NicId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestWriteLldpOption(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);
    OHA_ASSERT_NULL_PTR(pRQB);

    pLldpOption = &pRQB->args.LldpOption;
    OHA_ASSERT_NULL_PTR(pLldpOption);

    NicId = pHDB->Params.pDetail->Params.User.NICId;

    /* Check if we have a EDD-Channel for NIC-ID. */
    pHDBEdd  = OHA_EDDGetHandleByNIC(NicId); /* only EDD channel for this request necessary */

    if ( LSA_HOST_PTR_ARE_EQUAL(pHDBEdd,  LSA_NULL) )
    {
        Response = OHA_ERR_PARAM;
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_WRITE_OPTION failed. No Lower Channel for this NicId (NIC-ID: 0x%X)", NicId);
    }
    else if (pLldpOption->PortId > OHA_CFG_MAX_PORTS)
    {
        Response = OHA_ERR_PARAM;  /* Mode is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_WRITE_OPTION failed. PortID is wrong (PORT-ID: 0x%X)", pLldpOption->PortId);
    }
	/* AP01556824: accept PortID=0 if IF is addressed! */ 
    else if (pLldpOption->PortId == 0 && OHA_LldpIsPortOption(pLldpOption->Type, pLldpOption->PortId))
    {
        Response = OHA_ERR_PARAM;  /* Mode is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_WRITE_OPTION failed. PortID is wrong (PORT-ID: 0x%X)", pLldpOption->PortId);
    }
    else if (pLldpOption->Type == OHA_LLDP_TYPE_ORG_SPECIFIC && /* check the LLDP-type */
             pLldpOption->OUI == 0)
    {
        Response = OHA_ERR_PARAM;  /* OUI is wrong */
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_WRITE_OPTION failed. OUI is wrong (OUI: 0x%X)", pLldpOption->OUI);
    }
    else if (pLldpOption->Type == OHA_LLDP_TYPE_ORG_SPECIFIC &&  /* AP00776366: IEEE- and PNO-OUI's are not allowed */
             (pLldpOption->OUI == OHA_LLDP_TYPE_ORG_OUI_IEEE_802_1 ||
              pLldpOption->OUI == OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3 ||
              pLldpOption->OUI == OHA_LLDP_TYPE_ORG_OUI_PNO        ||
              pLldpOption->OUI == OHA_LLDP_TYPE_ORG_OUI_PNO_1      ||
              pLldpOption->OUI == OHA_LLDP_TYPE_ORG_OUI_PNO_2      ||
              pLldpOption->OUI == OHA_LLDP_TYPE_ORG_OUI_PNO_3) )
    {
        Response = OHA_ERR_PARAM;  /* OUI is wrong */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_WRITE_OPTION failed. OUI is not allowed (OUI: 0x%X)", pLldpOption->OUI);
    }

    /* if the SOCK is to inform about a mib2 parameter (sysName, sysDescr or ifDescr) */
    if (OHA_LldpIsMib2Object(pLldpOption->Type))
	{
		if (OHA_IsAnyLocalSetMib2Active(NicId)) /* SetMib2Params from any other user running ? */
		{
			Response = OHA_ERR_SEQUENCE;  /* already one request active */
			OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_WRITE_OPTION failed. Already one User-Request active for this NicId (NIC-ID: 0x%X)",NicId);
		}
	}

	if (Response == OHA_OK)
    {
        /* first read the option if existing */
        OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
        LSA_UINT16 Len     = 0;
        LSA_UINT8  SubType = pLldpOption->SubType;
        LSA_BOOL   Difference = LSA_TRUE;
        LSA_UINT16 RetVal;

        /* read option from LLDP-DB */
        if (pLldpOption->Type != OHA_LLDP_TYPE_CHASSIS_ID &&
            pLldpOption->Type != OHA_LLDP_TYPE_TTL)
        {
            RetVal = OHA_DB_READ_LLDP_ELEMENT(NicId, pLldpOption->PortId,
                                              pLldpOption->Type,
                                              &SubType, pLldpOption->OUI,
                                              &NamePtr, &Len);
            /* is the option different ? */
            if (RetVal == LSA_RET_OK &&
                SubType == pLldpOption->SubType &&
                Len == pLldpOption->Length)
            {
                if (Len == 0)
                {
                    Difference = LSA_FALSE; /* the same */
                }
                else
                {
                    /* compare for changes */
                    if (OHA_IS_NOT_NULL(NamePtr) &&
                        OHA_IS_NOT_NULL(pLldpOption->pVariable) &&
                        OHA_MEMCMP(NamePtr, pLldpOption->pVariable, Len))
                    {
                        Difference = LSA_FALSE; /* identical */
                    }
                }
            }
        }

        Response = OHA_DB_WRITE_LLDP_ELEMENT(NicId,
                                             pLldpOption->PortId,
                                             pLldpOption->Type,      /* type of variable, option */
                                             pLldpOption->SubType,   /* subtype of variable, suboption */
                                             pLldpOption->OUI,       /* OUI, if type=127 */
                                             pLldpOption->pVariable, /* variable in octets */
                                             pLldpOption->Length);   /* length of variable in octets */

        /* if LLDPSend is active, we've to update the LLDP data if different, but not ChassisID and TTL */
        /* "ChassisId" represents the Chassis MAC, "TTL" is a detail parameter */
        if (Response == LSA_RET_OK &&
            pLldpOption->Type != OHA_LLDP_TYPE_CHASSIS_ID &&
            pLldpOption->Type != OHA_LLDP_TYPE_TTL &&
            Difference)
        {
            LSA_UINT16 Status;

            if (OHA_LldpIsPortOption(pLldpOption->Type, pLldpOption->PortId))
            {
                OHA_HDB_LLDP_PTR_TYPE pHDBLldp = OHA_LLDPGetHandleByNIC(NicId);

                if ((!LSA_HOST_PTR_ARE_EQUAL(pHDBLldp, LSA_NULL)) && /* LLDP-Set is possible ? */
                    (pHDBLldp->RequestState == OHA_LLDP_STATE_START || 
					 pHDBLldp->RequestState == OHA_LLDP_STATE_RE_START ||
					 pHDBLldp->RequestState == OHA_LLDP_STATE_WF_START)) /* AP01070629, AP01137963 */
                {
                    Status = OHA_LLDPSet(pHDBLldp, pLldpOption->PortId, OHA_HANDLE_TYPE_SYS); /* send them */
                    if (Status != LSA_RET_OK)
                    {
                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OHA_RequestWriteLldpOption: OHA_LLDPSet failed (Status: 0x%X).", Status);
                    }
                }
            }
            else
            {
				/* AP01022057: mirror the globals (sysName, sysDesc) */
				if(pLldpOption->Type == OHA_LLDP_TYPE_SYSTEM_NAME || /* global object ?       */
					pLldpOption->Type == OHA_LLDP_TYPE_SYSTEM_CAP ||
					pLldpOption->Type == OHA_LLDP_TYPE_SYSTEM_DESCR) /* update all interfaces */
				{
					OHA_LLDPUpdateSendGlobal();
				}
				else	/* update this interface */
				{
					/* update the LLDP-send data if LLDP-Send is active */
					Status = OHA_LLDPUpdateSend(NicId,
												OHA_CTRL_STATION_NAME,
												OHA_HANDLE_TYPE_SYS);
					if (Status != OHA_OK)
					{
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestWriteLldpOption - LLDPUpdateSend(RetVal: 0x%X)", Status);
					}
				}
            }

			/* if the SOCK is to inform about a mib2 parameter (sysName, sysDescr or ifDescr) */
			if (OHA_LldpIsMib2Object(pLldpOption->Type))
			{
				if(pLldpOption->Type == OHA_LLDP_TYPE_PORT_DESCR) /* interface specific */
				{
					Status = OHA_SOCKSetMib2Status(NicId, pLldpOption->PortId, OHA_RQB_GET_HANDLE(pRQB));
				}
				else
				{
					Status = OHA_SOCKSetMib2Params(NicId, OHA_RQB_GET_HANDLE(pRQB));
				}
				if (Status != OHA_OK)
				{
					OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_HIGH,"OHA_OPC_LLDP_WRITE_OPTION: Setting MIB2-Params failed (Status=%u).", Status);
				}
				else
				{
					/* queue the user-rqb */
					OHA_RQB_PUT_BLOCK_TOP(pHDB->Mib2UserReq.pBottom,
											pHDB->Mib2UserReq.pTop,
											pRQB);

					Response = OHA_OK_ACTIVE;
					OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_PrmWriteSNMP: 0x%X)", Response);
					return;
				}
			}
		}
    }

    OHA_UserRequestFinish(pHDB,pRQB,Response);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestWriteLldpOption(Response: 0x%X)", Response);
}

/*=============================================================================
 * function name:  oha_db_count_rem_lldp_elements
 * function:       counts the database elements of this port
 * parameters:     NicId
                   Type (type of variable, LLDP-option)
 * return value:   LSA_UINT16:      Count
 *===========================================================================*/
LSA_UINT32 OHA_SYSTEM_OUT_FCT_ATTR oha_db_count_rem_lldp_elements(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT8                   Type,       /* type of variable, option */
    LSA_UINT32                  OUI         /* OUI, if subtype=127 */
)
{
    OHA_LIST_TYPED_ENTRY_TYPE(oha_db_lldp_elem_tag) * pAnchor;
    LSA_UINT32 Count  = 0;
    LSA_UINT16 SenderId;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_count_rem_lldp_element(PortId: 0x%X, Type: 0x%X, OUI: 0x%X)",
                          PortId, Type, OUI);

    /* ++ check the params ++ check the params ++ */
    if (!Type) /* invalid option */
    {
        OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_count_rem_lldp_elements(NicId: 0x%X, Count: 0x%X)", NicId, Count);
        return Count;
    }

    for (SenderId = 1; SenderId <= LLDP_MAX_PORT_SENDER; SenderId++)  /* count elements of all senders */
    {
        pAnchor = oha_get_rem_lldp_db_element (NicId, PortId, SenderId, Type,   /* type of variable */
                                               0, OUI);    /* type of variable */
        if (OHA_IS_NOT_NULL(pAnchor))   /* element(s) in DB exists */
        {
            /* count the elements */
            OHA_DB_LLDP_ELEM_PTR_TYPE pTempElement = OhaListFirst(pAnchor, OHA_DB_LLDP_ELEM_PTR_TYPE);  /* starts with the first element */

            while (OHA_IS_NOT_NULL(pTempElement))
            {
                Count++;
                pTempElement = OhaListNext(pAnchor, &pTempElement->ListElem, OHA_DB_LLDP_ELEM_PTR_TYPE);
            }
        }
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_count_rem_lldp_elements(NicId: 0x%X, Count: 0x%X)", NicId, Count);
    return Count;
}

#if 0
/*=============================================================================
 * function name:  oha_db_count_all_rem_lldp_elements
 * function:       counts all database elements of this port
 * parameters:     NicId
 * return value:   LSA_UINT16:      Count
 *===========================================================================*/
LSA_UINT32 OHA_SYSTEM_OUT_FCT_ATTR oha_db_count_all_rem_lldp_elements(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId
)
{
    LSA_UINT32 Count  = 0;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_count_all_rem_lldp_elements(NicId: 0x%X, PortId: 0x%X)",
                          NicId, PortId);

    Count += oha_db_count_rem_lldp_elements(NicId, PortId, OHA_LLDP_TYPE_CHASSIS_ID, 0);
    Count += oha_db_count_rem_lldp_elements(NicId, PortId, OHA_LLDP_TYPE_PORT_ID, 0);
    Count += oha_db_count_rem_lldp_elements(NicId, PortId, OHA_LLDP_TYPE_TTL, 0);
    Count += oha_db_count_rem_lldp_elements(NicId, PortId, OHA_LLDP_TYPE_PORT_DESCR, 0);
    Count += oha_db_count_rem_lldp_elements(NicId, PortId, OHA_LLDP_TYPE_SYSTEM_NAME, 0);
    Count += oha_db_count_rem_lldp_elements(NicId, PortId, OHA_LLDP_TYPE_SYSTEM_DESCR, 0);
    Count += oha_db_count_rem_lldp_elements(NicId, PortId, OHA_LLDP_TYPE_SYSTEM_CAP, 0);
    Count += oha_db_count_rem_lldp_elements(NicId, PortId, OHA_LLDP_TYPE_MGM_ADDR, 0);

    Count += oha_db_count_rem_lldp_elements(NicId, PortId, OHA_LLDP_TYPE_MGM_ADDR + 1, 0); /* the LLDP-types (9...126) */

    Count += oha_db_count_rem_lldp_elements(NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_1); /* IEEE 802.1 */
    Count += oha_db_count_rem_lldp_elements(NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC, OHA_LLDP_TYPE_ORG_OUI_IEEE_802_3); /* IEEE 802.3 */
    Count += oha_db_count_rem_lldp_elements(NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC, OHA_LLDP_TYPE_ORG_OUI_PNO);  /* PNO */
    Count += oha_db_count_rem_lldp_elements(NicId, PortId, OHA_LLDP_TYPE_ORG_SPECIFIC, 0);  /* others */

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_count_all_rem_lldp_elements(NicId: 0x%X, Count: 0x%X)", NicId, Count);
    return Count;
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestLldpAccess                       +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                        :    OHA_HDB_PTR_TYPE            pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_LLDP_ACCESS_CONTROL    +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE      Error        Addition errorcodes from LowerL+*/
/*+     OHA_RQB_ARGS_TYPE       args.LldpAccess                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+ Description: Request to set access for lldp mib objects to the database +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestLldpAccess(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    LSA_UINT16              Response = OHA_OK;
    LSA_UINT16              NICId;
    OHA_HDB_LLDP_PTR_TYPE pHDBLldp;
    OHA_UPPER_LLDP_ACCESS_PTR_TYPE pLldpAccess;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestLldpAccess(pRQB: 0x%X, pHDB: 0x%X)", pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);
    OHA_ASSERT_NULL_PTR(pHDB->Params.pDetail);

    pLldpAccess = &pRQB->args.LldpAccess;
    OHA_ASSERT_NULL_PTR(pLldpAccess);
    NICId = pHDB->Params.pDetail->Params.User.NICId;
	pHDBLldp = OHA_LLDPGetHandleByNIC(NICId);

	/* there must be an if channel */
	if (!OHA_EDDCheckNICUsed(NICId))     /* EDD System channel is opened */
	{
        Response = OHA_ERR_CHANNEL_USE;  /* not an if channel */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_ACCESS_CONTROL failed. Must be an interface channel (NICId: 0x%X)", NICId);
	}
	else if (pLldpAccess->PortId > OHA_CFG_MAX_PORTS)
    {
        Response = OHA_ERR_PARAM;  /* PortId is wrong */
        /* Error on getting handle. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_ACCESS_CONTROL failed. PortID is wrong (PORT-ID: 0x%X)", pLldpAccess->PortId);
    }
    else if ( pLldpAccess->Options == 0 && pLldpAccess->PortOptions == 0 )
    {
        Response = OHA_ERR_PARAM;  /* Parameter Options is wrong */
        /* Error on getting Options. Abort. */
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP, "Request OHA_OPC_LLDP_ACCESS_CONTROL failed. No Parameter to write (Options: 0x%X, PortOptions: 0x%X)",
									pLldpAccess->Options, pLldpAccess->PortOptions);
    }
    else if (!(pLldpAccess->AccessMode == OHA_READ_ONLY || 
               pLldpAccess->AccessMode == OHA_READ_WRITE ||
               pLldpAccess->AccessMode == OHA_QUERY_WRITE ||
			   pLldpAccess->AccessMode == OHA_NO_ACCESS))
    {
        Response = OHA_ERR_PARAM;  /* Parameter AccessMode is wrong */
        /* Error on getting AccessMode. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_ACCESS_CONTROL failed. Invalid AccessMode (0x%X)", pLldpAccess->AccessMode);
	}
	else if (pLldpAccess->AccessMode == OHA_QUERY_WRITE && OHA_IS_NULL(pHDBLldp))
	{
        Response = OHA_ERR_RESOURCE;  /* no LLDP channel existing */
        /* Error on getting LLDP channel. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_LLDP_ACCESS_CONTROL failed. No LLDP channel for NicId (0x%X) existing", NICId);
	}

	if(Response != OHA_OK)
	{
		OHA_UserRequestFinish(pHDB,pRQB,Response);
		OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestLldpAccess(NicId: 0x%X, Response: 0x%X)", NICId, Response);
		return;
	}

	if(pLldpAccess->Options && pLldpAccess->AccessMode == OHA_QUERY_WRITE)
	{
		/* is there already a query-user ? */
		if (OHA_IS_NOT_NULL(pHDBLldp->pHDBUserQueryIf)) /* check the if query-user */
		{
			if(!OHA_ARE_EQUAL(pHDBLldp->pHDBUserQueryIf, pHDB)) /* not the same */
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"Already a SNMP-Query If-User (0x%X) existing!", pHDBLldp->pHDBUserQueryIf);
			    OHA_UserRequestFinish(pHDB,pRQB,Response);

			    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestLldpAccess(NicId: 0x%X, Response: 0x%X)", NICId, Response);
				return;
			}
		}
	}

	if(pLldpAccess->PortOptions && pLldpAccess->AccessMode == OHA_QUERY_WRITE)
	{
		/* is there already a query-user ? */
		if (OHA_IS_NOT_NULL(pHDBLldp->pHDBUserQueryPort)) /* check the port query-user */
		{
			if(!OHA_ARE_EQUAL(pHDBLldp->pHDBUserQueryPort, pHDB)) /* not the same */
			{
				OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE,"Already a SNMP-Query Port-User (0x%X) existing!", pHDBLldp->pHDBUserQueryPort);
			    OHA_UserRequestFinish(pHDB,pRQB,Response);

			    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestLldpAccess(NicId: 0x%X, Response: 0x%X)", NICId, Response);
				return;
			}
		}
	}

	if(pLldpAccess->Options)
	{
		if (pLldpAccess->AccessMode == OHA_QUERY_WRITE)
		{
			pHDBLldp->pHDBUserQueryIf = pHDB; /* store the global control-user */
			OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"Set SNMP-Query If-User successful");
	    }
		Response = OHA_DB_WRITE_LLDP_MIB_ACCESS(pLldpAccess->Options,
												pLldpAccess->AccessMode);
	}

	if(pLldpAccess->PortOptions)
	{
		if (pLldpAccess->AccessMode == OHA_QUERY_WRITE)
		{
			pHDBLldp->pHDBUserQueryPort = pHDB; /* store the global control-user */
			OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"Set SNMP-Query Port-User successful");
	    }
		Response = OHA_DB_WRITE_LLDP_MIB_PORT_ACCESS(NICId,
													 pLldpAccess->PortId,
													 pLldpAccess->PortOptions,
													 pLldpAccess->AccessMode);
	}

    OHA_UserRequestFinish(pHDB,pRQB,Response);

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestLldpAccess(NicId: 0x%X, Response: 0x%X)", NICId, Response);
    return;
}

/*=============================================================================
 * function name:  OHA_DB_READ_LLDP_MIB_VALUE
 * function:       reades the global parameters of an LLDP mib object
 * parameters:     Options (LLDP.MIB option flags)
                   pValue (variable in octets)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_LLDP_MIB_VALUE(
    LSA_UINT32                  Options,    /* type of variable   */
    OHA_COMMON_MEM_U32_PTR_TYPE pValue,     /* variable in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pLength,    /* length of variable in octets */
    LSA_BOOL                  * pRema       /* storage of variable in memory */
)
{
    LSA_UINT16                   RetVal = OHA_ERR_PARAM;
	OHA_OID_LLDP_CONFIG_PTR_TYPE pDbLldpConfig = &oha_database.GlobalConfig;
	LSA_UINT16                   Length = 0;
	LSA_BOOL                     Rema   = LSA_FALSE;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_LLDP_MIB_VALUE(Options: 0x%X, pLength: 0x%X, pValue: 0x%X, pRema: 0x%X)",
                          Options, pLength, pValue, pRema);

    if ( Options == 0 )
    {
        OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_READ_LLDP_MIB_VALUE (No Parameter given (Options: 0x%X, pValue: 0x%X)",
							 Options, pValue);
        return (RetVal);
    }
    if ( OHA_IS_NULL(pValue) )
    {
        OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_READ_LLDP_MIB_VALUE (No Parameter given (Options: 0x%X, pValue: 0x%X)",
							 Options, pValue);
        return (RetVal);
    }

	if (Options & OHA_CTRL_TX_INTERVAL)		/* lldpConfiguration "lldpMessageTxInterval" */
	{
		OHA_DB_GET_MIB_VALUE(&pDbLldpConfig->TxInterval, *pValue, Length, Rema);
	}
	if (Options & OHA_CTRL_TX_HOLD_MULTIPLIER)	/* lldpConfiguration "lldpMessageTxHoldMultiplier" */
	{
		OHA_DB_GET_MIB_VALUE(&pDbLldpConfig->TxHoldMultiplier, *pValue, Length, Rema);
	}
	if (Options & OHA_CTRL_REINIT_DELAY)		/* lldpConfiguration "lldpReinitDelay" */
	{
		OHA_DB_GET_MIB_VALUE(&pDbLldpConfig->LldpReinitDelay, *pValue, Length, Rema);
	}
	if (Options & OHA_CTRL_TX_DELAY)			/* lldpConfiguration "lldpTxDelay" */
	{
		OHA_DB_GET_MIB_VALUE(&pDbLldpConfig->LldpTxDelay, *pValue, Length, Rema);
	}
	if (Options & OHA_CTRL_NOTIFICATION_INTERVAL) /* lldpConfiguration "lldpNotificationInterval" */
	{
		OHA_DB_GET_MIB_VALUE(&pDbLldpConfig->NotificationInterval, *pValue, Length, Rema);
	}

	if(OHA_IS_NOT_NULL(pLength))
	{
		*pLength = Length;
	}
	if(OHA_IS_NOT_NULL(pRema))
	{
		*pRema = Rema;
	}

	RetVal = LSA_RET_OK;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_READ_LLDP_MIB_VALUE(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*=============================================================================
 * function name:  OHA_DB_READ_LLDP_MIB_ACCESS
 * function:       reades the access parameter of a global LLDP mib object
 * parameters:     Options (LLDP.MIB option flags)
                   AccessControl (OHA_READ_ONLY, OHA_READ_WRITE,
                                  OHA_NO_ACCESS, OHA_QUERY_ACCESS)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_LLDP_MIB_ACCESS(
    LSA_UINT32                  Options,    /* type of variable   */
    OHA_COMMON_MEM_U8_PTR_TYPE  pAccCtrl    /* access control for variable */
)
{
    LSA_UINT16                   RetVal = OHA_ERR_PARAM;
	OHA_OID_LLDP_CONFIG_PTR_TYPE pDbLldpConfig = &oha_database.GlobalConfig;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_LLDP_MIB_ACCESS(Options: 0x%X, pAccCtrl: 0x%X)",
                          Options, pAccCtrl);

    if ( Options == 0 )
    {
        OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_READ_LLDP_MIB_ACCESS (No Parameter given (Options: 0x%X, pAccCtrl: 0x%X)",
							 Options, pAccCtrl);
        return (RetVal);
    }
    if ( OHA_IS_NULL(pAccCtrl) )
    {
        OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_READ_LLDP_MIB_ACCESS (No Parameter given (Options: 0x%X, pAccCtrl: 0x%X)",
							 Options, pAccCtrl);
        return (RetVal);
    }

	if (Options & OHA_CTRL_TX_INTERVAL)		/* lldpConfiguration "lldpMessageTxInterval" */
	{
		OHA_DB_GET_MIB_ACCESS(&pDbLldpConfig->TxInterval, *pAccCtrl);
	}
	if (Options & OHA_CTRL_TX_HOLD_MULTIPLIER)	/* lldpConfiguration "lldpMessageTxHoldMultiplier" */
	{
		OHA_DB_GET_MIB_ACCESS(&pDbLldpConfig->TxHoldMultiplier, *pAccCtrl);
	}
	if (Options & OHA_CTRL_REINIT_DELAY)		/* lldpConfiguration "lldpReinitDelay" */
	{
		OHA_DB_GET_MIB_ACCESS(&pDbLldpConfig->LldpReinitDelay, *pAccCtrl);
	}
	if (Options & OHA_CTRL_TX_DELAY)			/* lldpConfiguration "lldpTxDelay" */
	{
		OHA_DB_GET_MIB_ACCESS(&pDbLldpConfig->LldpTxDelay, *pAccCtrl);
	}
	if (Options & OHA_CTRL_NOTIFICATION_INTERVAL) /* lldpConfiguration "lldpNotificationInterval" */
	{
		OHA_DB_GET_MIB_ACCESS(&pDbLldpConfig->NotificationInterval, *pAccCtrl);
	}

	RetVal = LSA_RET_OK;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_READ_LLDP_MIB_ACCESS(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*=============================================================================
 * function name:  OHA_DB_READ_LLDP_MIB_PORT_VALUE
 * function:       reades a port parameter of the lldp-mib object
 * parameters:     NicId
                   PortId
                   Options (LLDP.MIB option flags)
                   pValue (variable in octets)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_LLDP_MIB_PORT_VALUE(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT32                  PortOptions,/* type of variable   */
    OHA_COMMON_MEM_U32_PTR_TYPE pValue,     /* variable in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pLength,    /* length of variable in octets */
    LSA_BOOL                  * pRema       /* storage of variable in memory */
)
{
    OHA_DB_IF_PTR_TYPE           pDbIf  = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16                   RetVal = LSA_RET_OK;
	OHA_OID_PORT_CONFIG_PTR_TYPE pPortConfig;
	LSA_UINT16                   Length = 0;
	LSA_BOOL                     Rema   = LSA_FALSE;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_LLDP_MIB_PORT_VALUE(PortOptions: 0x%X, pLength: 0x%X, pValue: 0x%X, pRema: 0x%X)",
                          PortOptions, pLength, pValue, pRema);

    if (OHA_IS_NULL(pDbIf))            /* no interface */
    {
		RetVal = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_READ_LLDP_MIB_PORT_VALUE(No such NicID: 0x%X , RetVal: 0x%X)", NicId, RetVal);
        return (RetVal);
    }
    if (PortId == 0 || (PortId > OHA_CFG_MAX_PORTS))
    {
		RetVal = OHA_ERR_PARAM;
        /* Error on getting port. Abort. */
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_READ_LLDP_MIB_PORT_VALUE(Invalid PortId:  0x%X)", PortId);
        return (RetVal);
    }
    if ( PortOptions == 0)
    {
		RetVal = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_READ_LLDP_MIB_PORT_VALUE (No Parameter to read (PortOptions: 0x%X, pValue: 0x%X)",
							 PortOptions, pValue);
        return (RetVal);
    }

    if ( OHA_IS_NULL(pValue) )
    {
		RetVal = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_READ_LLDP_MIB_PORT_VALUE (No Parameter given (PortOptions: 0x%X, pValue: 0x%X)",
							 PortOptions, pValue);
        return (RetVal);
    }

	pPortConfig = &pDbIf->LldpData[PortId-1].PortConfig;

	switch (PortOptions)
	{
		case OHA_CTRL_ADMIN_STATUS: /* lldpPortConfigTable "lldpPortConfigAdminStatus"            */
			OHA_DB_GET_MIB_VALUE(&pPortConfig->AdminStatus, *pValue, Length, Rema);
		break;
		case OHA_CTRL_NOTIFICATION: /* lldpPortConfigTable "lldpPortConfigNotificationEnable"     */
			OHA_DB_GET_MIB_VALUE(&pPortConfig->NotificationEnable, *pValue, Length, Rema);
		break;
		case OHA_CTRL_LLDP_MAN_ADDR: /* lldpManAddrConfigTxPortsTable "lldpConfigManAddrPortsTxEnable" */
			OHA_DB_GET_MIB_VALUE(&pPortConfig->ManAddrTxEnable, *pValue, Length, Rema);
		break;
		case OHA_CTRL_PNO_DELAY:    /* lldpXPnoConfigTable "lldpXPnoConfigSPDTxEnable"            */
			OHA_DB_GET_MIB_VALUE(&pPortConfig->PnoDelayTxEnable, *pValue, Length, Rema);
		break;
		case OHA_CTRL_PNO_PORT_STATUS: /* lldpXPnoConfigTable "lldpXPnoConfigPortStatusTxEnable"  */
			OHA_DB_GET_MIB_VALUE(&pPortConfig->PnoPortStatusTxEnable, *pValue, Length, Rema);
		break;
		case OHA_CTRL_PNO_ALIAS:    /* lldpXPnoConfigTable "lldpXPnoConfigAliasTxEnable"          */
			OHA_DB_GET_MIB_VALUE(&pPortConfig->PnoAliasTxEnable, *pValue, Length, Rema);
		break;
		case OHA_CTRL_PNO_MRP:      /* lldpXPnoConfigTable "lldpXPnoConfigMrpTxEnable"            */
			OHA_DB_GET_MIB_VALUE(&pPortConfig->PnoMrpTxEnable, *pValue, Length, Rema);
		break;
		case OHA_CTRL_PNO_PTCP:     /* lldpXPnoConfigTable "lldpXPnoConfigPtcpTxEnable"           */
			OHA_DB_GET_MIB_VALUE(&pPortConfig->PnoPtcpTxEnable, *pValue, Length, Rema);
		break;
		case OHA_CTRL_DOT1_VLAN_ID: /* lldpXdot1ConfigPortVlanTable "lldpXdot1ConfigPortVlanTxEnable" */
			OHA_DB_GET_MIB_VALUE(&pPortConfig->Dot1VlanIdTxEnable, *pValue, Length, Rema);
		break;
		case OHA_CTRL_DOT1_VLAN_NAME:	 /* lldpXdot1ConfigVlanNameTable "lldpXdot1ConfigVlanNameTxEnable" */
			OHA_DB_GET_MIB_VALUE(&pPortConfig->Dot1VlanNameTxEnable, *pValue, Length, Rema);
		break;
		case OHA_CTRL_DOT1_PROTO_VLAN: /* lldpXdot1ConfigProtoVlanTable "lldpXdot1ConfigProtoVlanTxEnable" */
			OHA_DB_GET_MIB_VALUE(&pPortConfig->Dot1PortProtoVlanTxEnable, *pValue, Length, Rema);
		break;
		case OHA_CTRL_DOT1_PROTOCOL: /* lldpXdot1ConfigProtocolTable "lldpXdot1ConfigProtocolTxEnable" */
			OHA_DB_GET_MIB_VALUE(&pPortConfig->Dot1ProtocolTxEnable, *pValue, Length, Rema);
		break;

		case OHA_CTRL_LLDP_TLVS:    /* lldpPortConfigTable "lldpPortConfigTLVsTxEnable" */
			/* BITS: portDesc(0), sysName(1), sysDesc(2), sysCap(3)  */
			OHA_DB_GET_MIB_VALUE(&pPortConfig->TLVsTxEnable, *pValue, Length, Rema);
		break;

		case OHA_CTRL_DOT3_TLVS:    /* lldpXdot3PortConfigTable "lldpXdot3PortConfigTLVsTxEnable" */
			/* BITS: macPhyConfigStatus(0), powerViaMDI(1), linkAggregation(2), maxFrameSize(3) */
			OHA_DB_GET_MIB_VALUE(&pPortConfig->Dot3TLVsTxEnable, *pValue, Length, Rema);
		break;
		default:
			RetVal = OHA_ERR_PARAM;
			OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_READ_LLDP_MIB_PORT_VALUE (No valid option to read ( NicId: 0x%X, PortOptions: 0x%X)",
								 NicId, PortOptions);
		break;
	}

	if(OHA_IS_NOT_NULL(pLength))
	{
		*pLength = Length;
	}
	if(OHA_IS_NOT_NULL(pRema))
	{
		*pRema = Rema;
	}

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_READ_LLDP_MIB_PORT_VALUE(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return(RetVal);
}

/*=============================================================================
 * function name:  OHA_DB_READ_LLDP_MIB_PORT_ACCESS
 * function:       reades the access parameter of the lldp-mib port object
 * parameters:     NicId
                   PortId
                   PortOptions (LLDP.MIB option flags)
                   AccessControl (OHA_READ_ONLY, OHA_READ_WRITE,
                                  OHA_NO_ACCESS, OHA_QUERY_ACCESS)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_LLDP_MIB_PORT_ACCESS(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT32                  PortOptions,    /* type of variable   */
    OHA_COMMON_MEM_U8_PTR_TYPE  pAccCtrl    /* access control for variable */
)
{
    OHA_DB_IF_PTR_TYPE           pDbIf  = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16                   RetVal = LSA_RET_OK;
	OHA_OID_PORT_CONFIG_PTR_TYPE pPortConfig;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_LLDP_MIB_PORT_ACCESS(PortOptions: 0x%X, pAccCtrl: 0x%X)",
                          PortOptions, pAccCtrl);

    if (OHA_IS_NULL(pDbIf))            /* no interface */
    {
		RetVal = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_READ_LLDP_MIB_PORT_ACCESS(No such NicID: 0x%X , RetVal: 0x%X)", NicId, RetVal);
        return (RetVal);
    }
    if (PortId == 0 || (PortId > OHA_CFG_MAX_PORTS))
    {
		RetVal = OHA_ERR_PARAM;
        /* Error on getting port. Abort. */
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_READ_LLDP_MIB_PORT_ACCESS(Invalid PortId:  0x%X)", PortId);
        return (RetVal);
    }
    if ( PortOptions == 0 )
    {
		RetVal = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_READ_LLDP_MIB_PORT_ACCESS (No Parameter to read (PortOptions: 0x%X, pAccCtrl: 0x%X)",
							 PortOptions, pAccCtrl);
        return (RetVal);
    }

    if ( OHA_IS_NULL(pAccCtrl) )
    {
		RetVal = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_READ_LLDP_MIB_PORT_ACCESS (No Parameter given (PortOptions: 0x%X, pAccCtrl: 0x%X)",
							 PortOptions, pAccCtrl);
        return (RetVal);
    }

	pPortConfig = &pDbIf->LldpData[PortId-1].PortConfig;

	switch (PortOptions)  /* only one allowed */
	{
		case OHA_CTRL_ADMIN_STATUS: /* lldpPortConfigTable "lldpPortConfigAdminStatus"            */
			OHA_DB_GET_MIB_ACCESS(&pPortConfig->AdminStatus, *pAccCtrl);
		break;
		case OHA_CTRL_NOTIFICATION: /* lldpPortConfigTable "lldpPortConfigNotificationEnable"     */
			OHA_DB_GET_MIB_ACCESS(&pPortConfig->NotificationEnable, *pAccCtrl);
		break;

		case OHA_CTRL_LLDP_MAN_ADDR: /* lldpManAddrConfigTxPortsTable "lldpConfigManAddrPortsTxEnable" */
			OHA_DB_GET_MIB_ACCESS(&pPortConfig->ManAddrTxEnable, *pAccCtrl);
		break;
		case OHA_CTRL_PNO_DELAY:    /* lldpXPnoConfigTable "lldpXPnoConfigSPDTxEnable"            */
			OHA_DB_GET_MIB_ACCESS(&pPortConfig->PnoDelayTxEnable, *pAccCtrl);
		break;
		case OHA_CTRL_PNO_PORT_STATUS: /* lldpXPnoConfigTable "lldpXPnoConfigPortStatusTxEnable"  */
			OHA_DB_GET_MIB_ACCESS(&pPortConfig->PnoPortStatusTxEnable, *pAccCtrl);
		break;
		case OHA_CTRL_PNO_ALIAS:    /* lldpXPnoConfigTable "lldpXPnoConfigAliasTxEnable"          */
			OHA_DB_GET_MIB_ACCESS(&pPortConfig->PnoAliasTxEnable, *pAccCtrl);
		break;
		case OHA_CTRL_PNO_MRP:      /* lldpXPnoConfigTable "lldpXPnoConfigMrpTxEnable"            */
			OHA_DB_GET_MIB_ACCESS(&pPortConfig->PnoMrpTxEnable, *pAccCtrl);
		break;
		case OHA_CTRL_PNO_PTCP:     /* lldpXPnoConfigTable "lldpXPnoConfigPtcpTxEnable"           */
			OHA_DB_GET_MIB_ACCESS(&pPortConfig->PnoPtcpTxEnable, *pAccCtrl);
		break;

		case OHA_CTRL_DOT1_VLAN_ID: /* lldpXdot1ConfigPortVlanTable "lldpXdot1ConfigPortVlanTxEnable" */
			OHA_DB_GET_MIB_ACCESS(&pPortConfig->Dot1VlanIdTxEnable, *pAccCtrl);
		break;
		case OHA_CTRL_DOT1_VLAN_NAME:	 /* lldpXdot1ConfigVlanNameTable "lldpXdot1ConfigVlanNameTxEnable" */
			OHA_DB_GET_MIB_ACCESS(&pPortConfig->Dot1VlanNameTxEnable, *pAccCtrl);
		break;
		case OHA_CTRL_DOT1_PROTO_VLAN: /* lldpXdot1ConfigProtoVlanTable "lldpXdot1ConfigProtoVlanTxEnable" */
			OHA_DB_GET_MIB_ACCESS(&pPortConfig->Dot1PortProtoVlanTxEnable, *pAccCtrl);
		break;
		case OHA_CTRL_DOT1_PROTOCOL: /* lldpXdot1ConfigProtocolTable "lldpXdot1ConfigProtocolTxEnable" */
			OHA_DB_GET_MIB_ACCESS(&pPortConfig->Dot1ProtocolTxEnable, *pAccCtrl);
		break;

		case OHA_CTRL_LLDP_TLVS:    /* lldpPortConfigTable "lldpPortConfigTLVsTxEnable"      */
									/* BITS: portDesc(0), sysName(1), sysDesc(2), sysCap(3)  */
			OHA_DB_GET_MIB_ACCESS(&pPortConfig->TLVsTxEnable, *pAccCtrl);
		break;
		case OHA_CTRL_DOT3_TLVS:    /* lldpXdot3PortConfigTable "lldpXdot3PortConfigTLVsTxEnable" */
				/* BITS: macPhyConfigStatus(0), powerViaMDI(1), linkAggregation(2), maxFrameSize(3) */
			OHA_DB_GET_MIB_ACCESS(&pPortConfig->Dot3TLVsTxEnable, *pAccCtrl);
		break;
		default:
			RetVal = OHA_ERR_PARAM;
			OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_READ_LLDP_MIB_PORT_ACCESS (No valid option to read (PortOptions: 0x%X, pAccCtrl: 0x%X)",
								 PortOptions, pAccCtrl);
		break;
	}

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_READ_LLDP_MIB_PORT_ACCESS(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return(RetVal);
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_LLDP_MIB_VALUE
 * function:       writes the global parameters for LLDP.MIB value to the database
 * parameters:     Options (LLDP.MIB option flags)
                   Variable (variable in octets)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:  LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_LLDP_MIB_VALUE(
    LSA_UINT32                  Options,    /* type of variable   */
    LSA_UINT32					Value,      /* variable in octets */
    LSA_UINT16                  Length,     /* length of variable in octets */
    LSA_BOOL                    Remanent    /* storage of variable in memory */
)
{
    LSA_UINT16                   RetVal = OHA_ERR_PARAM;
	OHA_OID_LLDP_CONFIG_PTR_TYPE pDbLldpConfig = &oha_database.GlobalConfig;

    OHA_FUNCTION_TRACE_04(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_LLDP_MIB_VALUE(Options: 0x%X, Length: 0x%X, Value: 0x%X, Remanent: 0x%X)",
                          Options, Length, Value, Remanent);

    if ( Options == 0 )
    {
        OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_LLDP_MIB_VALUE (No Parameter to write (Options: 0x%X, Value: 0x%X)",
							 Options, Value);
        return (RetVal);
    }

	if (Length != sizeof(LSA_UINT32)) /* we've SYNTAX Integer32 at all lldp config params */
	{
		/* Parameter Length is wrong */
		OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"OUT: OHA_DB_WRITE_LLDP_MIB_VALUE. Invalid Length (0x%X)", Length);
		return (RetVal);
	}

	if (Options & OHA_CTRL_TX_INTERVAL)		/* lldpConfiguration "lldpMessageTxInterval" */
	{
		OHA_DB_SET_MIB_VALUE(&pDbLldpConfig->TxInterval, Value, Length, Remanent);
	}
	if (Options & OHA_CTRL_TX_HOLD_MULTIPLIER)	/* lldpConfiguration "lldpMessageTxHoldMultiplier" */
	{
		OHA_DB_SET_MIB_VALUE(&pDbLldpConfig->TxHoldMultiplier, Value, Length, Remanent);
	}
	if (Options & OHA_CTRL_REINIT_DELAY)		/* lldpConfiguration "lldpReinitDelay" */
	{
		OHA_DB_SET_MIB_VALUE(&pDbLldpConfig->LldpReinitDelay, Value, Length, Remanent);
	}
	if (Options & OHA_CTRL_TX_DELAY)			/* lldpConfiguration "lldpTxDelay" */
	{
		OHA_DB_SET_MIB_VALUE(&pDbLldpConfig->LldpTxDelay, Value, Length, Remanent);
	}
	if (Options & OHA_CTRL_NOTIFICATION_INTERVAL) /* lldpConfiguration "lldpNotificationInterval" */
	{
		OHA_DB_SET_MIB_VALUE(&pDbLldpConfig->NotificationInterval, Value, Length, Remanent);
	}

	RetVal = LSA_RET_OK;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_WRITE_LLDP_MIB_VALUE(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_LLDP_MIB_ACCESS
 * function:       writes the parameters for LLDP.MIB access to the database
 * parameters:     Options (LLDP.MIB option flags)
                   AccCtrl (OHA_READ_ONLY, OHA_READ_WRITE,
                            OHA_NO_ACCESS, OHA_QUERY_ACCESS)
 * return value:   LSA_UINT16:  LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_LLDP_MIB_ACCESS(
    LSA_UINT32                  Options,    /* type of variable   */
    LSA_UINT8                   AccCtrl    /* access control for variable */
)
{
    LSA_UINT16                   RetVal = OHA_ERR_PARAM;
	OHA_OID_LLDP_CONFIG_PTR_TYPE pDbLldpConfig = &oha_database.GlobalConfig;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_LLDP_MIB_ACCESS(Options: 0x%X, AccCtrl: 0x%X)",
                          Options, AccCtrl);

    if ( Options == 0 )
    {
        OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_LLDP_MIB_ACCESS (No Parameter to write (Options: 0x%X)",
							 Options);
        return (RetVal);
    }
	if (!(AccCtrl == OHA_READ_ONLY   || AccCtrl == OHA_READ_WRITE ||
          AccCtrl == OHA_QUERY_WRITE || AccCtrl == OHA_NO_ACCESS))
	{
		/* Parameter AccessMode is wrong */
		OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"OUT: OHA_DB_WRITE_LLDP_MIB_ACCESS. Invalid AccessMode (0x%X)", AccCtrl);
		return (RetVal);
	}

	if (Options & OHA_CTRL_TX_INTERVAL)		/* lldpConfiguration "lldpMessageTxInterval" */
	{
		OHA_DB_SET_MIB_ACCESS(&pDbLldpConfig->TxInterval, AccCtrl);
	}
	if (Options & OHA_CTRL_TX_HOLD_MULTIPLIER)	/* lldpConfiguration "lldpMessageTxHoldMultiplier" */
	{
		OHA_DB_SET_MIB_ACCESS(&pDbLldpConfig->TxHoldMultiplier, AccCtrl);
	}
	if (Options & OHA_CTRL_REINIT_DELAY)		/* lldpConfiguration "lldpReinitDelay" */
	{
		OHA_DB_SET_MIB_ACCESS(&pDbLldpConfig->LldpReinitDelay, AccCtrl);
	}
	if (Options & OHA_CTRL_TX_DELAY)			/* lldpConfiguration "lldpTxDelay" */
	{
		OHA_DB_SET_MIB_ACCESS(&pDbLldpConfig->LldpTxDelay, AccCtrl);
	}
	if (Options & OHA_CTRL_NOTIFICATION_INTERVAL) /* lldpConfiguration "lldpNotificationInterval" */
	{
		OHA_DB_SET_MIB_ACCESS(&pDbLldpConfig->NotificationInterval, AccCtrl);
	}

	RetVal = LSA_RET_OK;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_WRITE_LLDP_MIB_ACCESS(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_LLDP_MIB_PORT_VALUE
 * function:       writes the parameters for LLDP.MIB value to the database
 * parameters:     NicId
                   PortId
                   Options (LLDP.MIB option flags)
                   pVariable (variable in octets)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:  LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_LLDP_MIB_PORT_VALUE(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT32                  PortOptions,/* type of variable   */
    LSA_UINT32					Value,     /* variable in octets */
    LSA_UINT16                  Length,    /* length of variable in octets */
    LSA_BOOL                    Remanent    /* storage of variable in memory */
)
{
    OHA_DB_IF_PTR_TYPE           pDbIf  = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16                   RetVal = OHA_ERR_PARAM;
	OHA_OID_PORT_CONFIG_PTR_TYPE pPortConfig;
	LSA_UINT16                   i;

    OHA_FUNCTION_TRACE_05(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_LLDP_MIB_PORT_VALUE(PortId: 0x%X, PortOptions: 0x%X, Length: 0x%X, Value: 0x%X, Remanent: 0x%X)",
                          PortId, PortOptions, Length, Value, Remanent);

    if (OHA_IS_NULL(pDbIf))            /* no interface */
    {
        OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_LLDP_MIB_PORT_VALUE(No such NicID: 0x%X , RetVal: 0x%X)", NicId, RetVal);
        return (RetVal);
    }
    if (PortId > OHA_CFG_MAX_PORTS)  /* 0 ... set all ports of this interface */
    {
        /* Error on getting port. Abort. */
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_LLDP_MIB_PORT_VALUE(Invalid PortId:  0x%X)", PortId);
        return (RetVal);
    }
    if ( PortOptions == 0 )
    {
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_LLDP_MIB_PORT_VALUE (No Parameter to write (PortOptions: 0x%X, Value: 0x%X)",
							 PortOptions, Value);
        return (RetVal);
    }

	if (Length == 0) /* we've a SYNTAX at all lldp port config params */
	{
		/* Parameter Length is wrong */
		OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OUT: OHA_DB_WRITE_LLDP_MIB_PORT_VALUE. Invalid Length (0x%X)", Length);
		return (RetVal);
	}
	
	for(i = 0; i < OHA_CFG_MAX_PORTS; i++)
	{
		pPortConfig = &pDbIf->LldpData[i].PortConfig;

		if((PortId == 0) ||	/* all ports or */
			(PortId == (i + 1)))  /* this port */
		{
			if (PortOptions & OHA_CTRL_ADMIN_STATUS) /* lldpPortConfigTable "lldpPortConfigAdminStatus"            */
			{
				OHA_DB_SET_MIB_VALUE(&pPortConfig->AdminStatus, Value, Length, Remanent);
			}
			if (PortOptions & OHA_CTRL_NOTIFICATION) /* lldpPortConfigTable "lldpPortConfigNotificationEnable"     */
			{
				OHA_DB_SET_MIB_VALUE(&pPortConfig->NotificationEnable, Value, Length, Remanent);
			}

			if (PortOptions & OHA_CTRL_LLDP_TLVS)    /* lldpPortConfigTable "lldpPortConfigTLVsTxEnable"           */
			{
				OHA_DB_SET_MIB_VALUE(&pPortConfig->TLVsTxEnable, Value, Length, Remanent);
			}

			if (PortOptions & OHA_CTRL_LLDP_MAN_ADDR) /* lldpManAddrConfigTxPortsTable "lldpConfigManAddrPortsTxEnable" */
			{
				OHA_DB_SET_MIB_VALUE(&pPortConfig->ManAddrTxEnable, Value, Length, Remanent);
			}
			if (PortOptions & OHA_CTRL_PNO_DELAY)    /* lldpXPnoConfigTable "lldpXPnoConfigSPDTxEnable"            */
			{
				OHA_DB_SET_MIB_VALUE(&pPortConfig->PnoDelayTxEnable, Value, Length, Remanent);
			}
			if (PortOptions & OHA_CTRL_PNO_PORT_STATUS) /* lldpXPnoConfigTable "lldpXPnoConfigPortStatusTxEnable"  */
			{
				OHA_DB_SET_MIB_VALUE(&pPortConfig->PnoPortStatusTxEnable, Value, Length, Remanent);
			}
			if (PortOptions & OHA_CTRL_PNO_ALIAS)    /* lldpXPnoConfigTable "lldpXPnoConfigAliasTxEnable"          */
			{
				OHA_DB_SET_MIB_VALUE(&pPortConfig->PnoAliasTxEnable, Value, Length, Remanent);
			}
			if (PortOptions & OHA_CTRL_PNO_MRP)      /* lldpXPnoConfigTable "lldpXPnoConfigMrpTxEnable"            */
			{
				OHA_DB_SET_MIB_VALUE(&pPortConfig->PnoMrpTxEnable, Value, Length, Remanent);
			}
			if (PortOptions & OHA_CTRL_PNO_PTCP)     /* lldpXPnoConfigTable "lldpXPnoConfigPtcpTxEnable"           */
			{
				OHA_DB_SET_MIB_VALUE(&pPortConfig->PnoPtcpTxEnable, Value, Length, Remanent);
			}

			if (PortOptions & OHA_CTRL_DOT3_TLVS)    /* lldpXdot3PortConfigTable "lldpXdot3PortConfigTLVsTxEnable" */
			{
				OHA_DB_SET_MIB_VALUE(&pPortConfig->Dot3TLVsTxEnable, Value, Length, Remanent);
			}

			if (PortOptions & OHA_CTRL_DOT1_VLAN_ID) /* lldpXdot1ConfigPortVlanTable "lldpXdot1ConfigPortVlanTxEnable" */
			{
				OHA_DB_SET_MIB_VALUE(&pPortConfig->Dot1VlanIdTxEnable, Value, Length, Remanent);
			}
			if (PortOptions & OHA_CTRL_DOT1_VLAN_NAME)	 /* lldpXdot1ConfigVlanNameTable "lldpXdot1ConfigVlanNameTxEnable" */
			{
				OHA_DB_SET_MIB_VALUE(&pPortConfig->Dot1VlanNameTxEnable, Value, Length, Remanent);
			}
			if (PortOptions & OHA_CTRL_DOT1_PROTO_VLAN) /* lldpXdot1ConfigProtoVlanTable "lldpXdot1ConfigProtoVlanTxEnable" */
			{
				OHA_DB_SET_MIB_VALUE(&pPortConfig->Dot1PortProtoVlanTxEnable, Value, Length, Remanent);
			}
			if (PortOptions & OHA_CTRL_DOT1_PROTOCOL) /* lldpXdot1ConfigProtocolTable "lldpXdot1ConfigProtocolTxEnable" */
			{
				OHA_DB_SET_MIB_VALUE(&pPortConfig->Dot1ProtocolTxEnable, Value, Length, Remanent);
			}
		}
	}

	RetVal = LSA_RET_OK;
    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_WRITE_LLDP_MIB_PORT_VALUE(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return(RetVal);
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_LLDP_MIB_PORT_ACCESS
 * function:       writes the parameters for LLDP.MIB access to the database
 * parameters:     NicId
                   PortId
                   Options (LLDP.MIB option flags)
                   AccessControl (OHA_READ_ONLY, OHA_READ_WRITE,
                                  OHA_NO_ACCESS, OHA_QUERY_ACCESS)
 * return value:   LSA_UINT16:  LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_LLDP_MIB_PORT_ACCESS(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT32                  PortOptions,/* type of variable   */
    LSA_UINT8                   AccCtrl    /* access control for variable */
)
{
    OHA_DB_IF_PTR_TYPE           pDbIf  = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16                   RetVal = OHA_ERR_PARAM;
	OHA_OID_PORT_CONFIG_PTR_TYPE pPortConfig;
	LSA_UINT16                   i;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_LLDP_MIB_PORT_OBJECT(PortId: 0x%X, PortOptions: 0x%X, AccCtrl: 0x%X)",
                          PortId, PortOptions, AccCtrl);

    if (OHA_IS_NULL(pDbIf))            /* no interface */
    {
        OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_LLDP_MIB_PORT_ACCESS(No such NicID: 0x%X , RetVal: 0x%X)", NicId, RetVal);
        return (RetVal);
    }
    if (PortId > OHA_CFG_MAX_PORTS)	/* 0 ... set all ports of this interface */
    {
        /* Error on getting port. Abort. */
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_LLDP_MIB_PORT_ACCESS(Invalid PortId:  0x%X)", PortId);
        return (RetVal);
    }
    if ( PortOptions == 0 )
    {
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_LLDP_MIB_PORT_ACCESS (No Parameter to write (PortOptions: 0x%X, AccCtrl: 0x%X)",
							 PortOptions, AccCtrl);
        return (RetVal);
    }

	if ( ((PortOptions & 0x000F0000) != 0x000F0000) &&   /* all or no lldp Tlv */
		 ((PortOptions & 0x000F0000) != 0) ) 		/* _PORT_DESC, _SYS_NAME, _SYS_DESC, _SYS_CAP) */
    {
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_LLDP_MIB_PORT_ACCESS (No valid Parameter for lldpPortConfigTLVsTxEnable (PortOptions: 0x%X, AccCtrl: 0x%X)",
							 PortOptions, AccCtrl);
        return (RetVal);
    }
		
	if ( ((PortOptions & 0x00F00000) != 0x00F00000) &&   /* all or no dot3 lldp Tlv */
		 ((PortOptions & 0x00F00000) != 0) ) 		/* _MAC_PHY_CONFIG_STATUS, _POWER_VIA_MDI, _LINK_AGGREGATION, _MAX_FRAME_SIZE) */
    {
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_DB_WRITE_LLDP_MIB_PORT_ACCESS (No valid Parameter for lldpXdot3PortConfigTLVsTxEnable (PortOptions: 0x%X, AccCtrl: 0x%X)",
							 PortOptions, AccCtrl);
        return (RetVal);
    }

	if (!(AccCtrl == OHA_READ_ONLY   || AccCtrl == OHA_READ_WRITE ||
          AccCtrl == OHA_QUERY_WRITE || AccCtrl == OHA_NO_ACCESS))
	{
		/* Parameter AccessMode is wrong */
		OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"OUT: OHA_DB_WRITE_LLDP_MIB_PORT_ACCESS. Invalid AccessMode (0x%X)", AccCtrl);
		return (RetVal);
	}

	for(i = 0; i < OHA_CFG_MAX_PORTS; i++)
	{
		pPortConfig = &pDbIf->LldpData[i].PortConfig;

		if((PortId == 0) ||	/* all ports or */
			(PortId == (i + 1)))  /* this port */
		{
			if (PortOptions & OHA_CTRL_ADMIN_STATUS) /* lldpPortConfigTable "lldpPortConfigAdminStatus"            */
			{
				OHA_DB_SET_MIB_ACCESS(&pPortConfig->AdminStatus, AccCtrl);
			}
			if (PortOptions & OHA_CTRL_NOTIFICATION) /* lldpPortConfigTable "lldpPortConfigNotificationEnable"     */
			{
				OHA_DB_SET_MIB_ACCESS(&pPortConfig->NotificationEnable, AccCtrl);
			}
			if (PortOptions & OHA_CTRL_LLDP_TLVS)    /* lldpPortConfigTable "lldpPortConfigTLVsTxEnable"           */
			{
				OHA_DB_SET_MIB_ACCESS(&pPortConfig->TLVsTxEnable, AccCtrl);
			}
			if (PortOptions & OHA_CTRL_LLDP_MAN_ADDR) /* lldpManAddrConfigTxPortsTable "lldpConfigManAddrPortsTxEnable" */
			{
				OHA_DB_SET_MIB_ACCESS(&pPortConfig->ManAddrTxEnable, AccCtrl);
			}
			if (PortOptions & OHA_CTRL_PNO_DELAY)    /* lldpXPnoConfigTable "lldpXPnoConfigSPDTxEnable"            */
			{
				OHA_DB_SET_MIB_ACCESS(&pPortConfig->PnoDelayTxEnable, AccCtrl);
			}
			if (PortOptions & OHA_CTRL_PNO_PORT_STATUS) /* lldpXPnoConfigTable "lldpXPnoConfigPortStatusTxEnable"  */
			{
				OHA_DB_SET_MIB_ACCESS(&pPortConfig->PnoPortStatusTxEnable, AccCtrl);
			}
			if (PortOptions & OHA_CTRL_PNO_ALIAS)    /* lldpXPnoConfigTable "lldpXPnoConfigAliasTxEnable"          */
			{
				OHA_DB_SET_MIB_ACCESS(&pPortConfig->PnoAliasTxEnable, AccCtrl);
			}
			if (PortOptions & OHA_CTRL_PNO_MRP)      /* lldpXPnoConfigTable "lldpXPnoConfigMrpTxEnable"            */
			{
				OHA_DB_SET_MIB_ACCESS(&pPortConfig->PnoMrpTxEnable, AccCtrl);
			}
			if (PortOptions & OHA_CTRL_PNO_PTCP)     /* lldpXPnoConfigTable "lldpXPnoConfigPtcpTxEnable"           */
			{
				OHA_DB_SET_MIB_ACCESS(&pPortConfig->PnoPtcpTxEnable, AccCtrl);
			}
			if (PortOptions & OHA_CTRL_DOT3_TLVS)    /* lldpXdot3PortConfigTable "lldpXdot3PortConfigTLVsTxEnable" */
			{
				OHA_DB_SET_MIB_ACCESS(&pPortConfig->Dot3TLVsTxEnable, AccCtrl);
			}
			if (PortOptions & OHA_CTRL_DOT1_VLAN_ID) /* lldpXdot1ConfigPortVlanTable "lldpXdot1ConfigPortVlanTxEnable" */
			{
				OHA_DB_SET_MIB_ACCESS(&pPortConfig->Dot1VlanIdTxEnable, AccCtrl);
			}
			if (PortOptions & OHA_CTRL_DOT1_VLAN_NAME)	 /* lldpXdot1ConfigVlanNameTable "lldpXdot1ConfigVlanNameTxEnable" */
			{
				OHA_DB_SET_MIB_ACCESS(&pPortConfig->Dot1VlanNameTxEnable, AccCtrl);
			}
			if (PortOptions & OHA_CTRL_DOT1_PROTO_VLAN) /* lldpXdot1ConfigProtoVlanTable "lldpXdot1ConfigProtoVlanTxEnable" */
			{
				OHA_DB_SET_MIB_ACCESS(&pPortConfig->Dot1PortProtoVlanTxEnable, AccCtrl);
			}
			if (PortOptions & OHA_CTRL_DOT1_PROTOCOL) /* lldpXdot1ConfigProtocolTable "lldpXdot1ConfigProtocolTxEnable" */
			{
				OHA_DB_SET_MIB_ACCESS(&pPortConfig->Dot1ProtocolTxEnable, AccCtrl);
			}
		}
	}

	RetVal = LSA_RET_OK;
    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_WRITE_LLDP_MIB_PORT_ACCESS(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return(RetVal);
}

/*=============================================================================
 * function name:  oha_db_read_temp_lldp_mib_value
 * function:       reades the temporary written global parameters of an LLDP mib object
 * parameters:     Options (LLDP.MIB option flags)
                   ppValue (variable in octets)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_temp_lldp_mib_value(
    LSA_UINT32                  Options,    /* type of variable   */
    OHA_COMMON_MEM_U32_PTR_TYPE *ppValue,   /* variable in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pLength     /* length of variable in octets */
)
{
    LSA_UINT16                   RetVal = OHA_ERR_PARAM;
	OHA_OID_LLDP_CONFIG_PTR_TYPE pDbLldpConfig = &oha_database.GlobalConfig;
	LSA_UINT16                   Length = 0;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_temp_lldp_mib_value(Options: 0x%X, pLength: 0x%X, ppValue: 0x%X)",
                          Options, pLength, ppValue);

    if ( Options == 0 )
    {
        OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_read_temp_lldp_mib_value (No Parameter given (Options: 0x%X, ppValue: 0x%X)",
							 Options, ppValue);
        return (RetVal);
    }
    if ( OHA_IS_NULL(ppValue) )
    {
        OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_read_temp_lldp_mib_value (No Parameter given (Options: 0x%X, ppValue: 0x%X)",
							 Options, ppValue);
        return (RetVal);
    }

	if (Options & OHA_CTRL_TX_INTERVAL)		/* lldpConfiguration "lldpMessageTxInterval" */
	{
		*ppValue = &pDbLldpConfig->TxInterval.TmpSetVal;
		Length   = pDbLldpConfig->TxInterval.Length;
	}
	if (Options & OHA_CTRL_TX_HOLD_MULTIPLIER)	/* lldpConfiguration "lldpMessageTxHoldMultiplier" */
	{
		*ppValue = &pDbLldpConfig->TxHoldMultiplier.TmpSetVal;
		Length   = pDbLldpConfig->TxHoldMultiplier.Length;
	}
	if (Options & OHA_CTRL_REINIT_DELAY)		/* lldpConfiguration "lldpReinitDelay" */
	{
		*ppValue = &pDbLldpConfig->LldpReinitDelay.TmpSetVal;
		Length   = pDbLldpConfig->LldpReinitDelay.Length;
	}
	if (Options & OHA_CTRL_TX_DELAY)			/* lldpConfiguration "lldpTxDelay" */
	{
		*ppValue = &pDbLldpConfig->LldpTxDelay.TmpSetVal;
		Length   = pDbLldpConfig->LldpTxDelay.Length;
	}
	if (Options & OHA_CTRL_NOTIFICATION_INTERVAL) /* lldpConfiguration "lldpNotificationInterval" */
	{
		*ppValue = &pDbLldpConfig->NotificationInterval.TmpSetVal;
		Length   = pDbLldpConfig->NotificationInterval.Length;
	}

	if(OHA_IS_NOT_NULL(pLength))
	{
		*pLength = Length;
	}

	RetVal = LSA_RET_OK;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_temp_lldp_mib_value(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*=============================================================================
 * function name:  oha_db_read_temp_lldp_mib_port_value
 * function:       reades a temporary set port parameter of the lldp-mib object
 * parameters:     NicId
                   PortId
                   Options (LLDP.MIB option flags)
                   pValue (variable in octets)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_temp_lldp_mib_port_value(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT32                  PortOptions,/* type of variable   */
    OHA_COMMON_MEM_U32_PTR_TYPE *ppValue,   /* variable in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pLength     /* length of variable in octets */
)
{
    OHA_DB_IF_PTR_TYPE           pDbIf  = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16                   RetVal = LSA_RET_OK;
	OHA_OID_PORT_CONFIG_PTR_TYPE pPortConfig;
	LSA_UINT16                   Length = 0;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_temp_lldp_mib_port_value(PortOptions: 0x%X, pLength: 0x%X, ppValue: 0x%X)",
                          PortOptions, pLength, ppValue);

    if (OHA_IS_NULL(pDbIf))            /* no interface */
    {
		RetVal = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_read_temp_lldp_mib_port_value(No such NicID: 0x%X , RetVal: 0x%X)", NicId, RetVal);
        return (RetVal);
    }
    if (PortId == 0 || (PortId > OHA_CFG_MAX_PORTS))
    {
		RetVal = OHA_ERR_PARAM;
        /* Error on getting port. Abort. */
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_read_temp_lldp_mib_port_value(Invalid PortId:  0x%X)", PortId);
        return (RetVal);
    }
    if ( PortOptions == 0)
    {
		RetVal = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_read_temp_lldp_mib_port_value (No Parameter to read (PortOptions: 0x%X, pValue: 0x%X)",
							 PortOptions, ppValue);
        return (RetVal);
    }

    if ( OHA_IS_NULL(ppValue) )
    {
		RetVal = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_read_temp_lldp_mib_port_value (No Parameter given (PortOptions: 0x%X, ppValue: 0x%X)",
							 PortOptions, ppValue);
        return (RetVal);
    }

	pPortConfig = &pDbIf->LldpData[PortId-1].PortConfig;

	switch (PortOptions)
	{
		case OHA_CTRL_ADMIN_STATUS: /* lldpPortConfigTable "lldpPortConfigAdminStatus"            */
			*ppValue = &pPortConfig->AdminStatus.TmpSetVal;
			Length   = pPortConfig->AdminStatus.Length;
		break;
		case OHA_CTRL_NOTIFICATION: /* lldpPortConfigTable "lldpPortConfigNotificationEnable"     */
			*ppValue = &pPortConfig->NotificationEnable.TmpSetVal;
			Length   = pPortConfig->NotificationEnable.Length;
		break;
		case OHA_CTRL_LLDP_MAN_ADDR: /* lldpManAddrConfigTxPortsTable "lldpConfigManAddrPortsTxEnable" */
			*ppValue = &pPortConfig->ManAddrTxEnable.TmpSetVal;
			Length   = pPortConfig->ManAddrTxEnable.Length;
		break;
		case OHA_CTRL_PNO_DELAY:    /* lldpXPnoConfigTable "lldpXPnoConfigSPDTxEnable"            */
			*ppValue = &pPortConfig->PnoDelayTxEnable.TmpSetVal;
			Length   = pPortConfig->PnoDelayTxEnable.Length;
		break;
		case OHA_CTRL_PNO_PORT_STATUS: /* lldpXPnoConfigTable "lldpXPnoConfigPortStatusTxEnable"  */
			*ppValue = &pPortConfig->PnoPortStatusTxEnable.TmpSetVal;
			Length   = pPortConfig->PnoPortStatusTxEnable.Length;
		break;
		case OHA_CTRL_PNO_ALIAS:    /* lldpXPnoConfigTable "lldpXPnoConfigAliasTxEnable"          */
			*ppValue = &pPortConfig->PnoAliasTxEnable.TmpSetVal;
			Length   = pPortConfig->PnoAliasTxEnable.Length;
		break;
		case OHA_CTRL_PNO_MRP:      /* lldpXPnoConfigTable "lldpXPnoConfigMrpTxEnable"            */
			*ppValue = &pPortConfig->PnoMrpTxEnable.TmpSetVal;
			Length   = pPortConfig->PnoMrpTxEnable.Length;
		break;
		case OHA_CTRL_PNO_PTCP:     /* lldpXPnoConfigTable "lldpXPnoConfigPtcpTxEnable"           */
			*ppValue = &pPortConfig->PnoPtcpTxEnable.TmpSetVal;
			Length   = pPortConfig->PnoPtcpTxEnable.Length;
		break;
		case OHA_CTRL_DOT1_VLAN_ID: /* lldpXdot1ConfigPortVlanTable "lldpXdot1ConfigPortVlanTxEnable" */
			*ppValue = &pPortConfig->Dot1VlanIdTxEnable.TmpSetVal;
			Length   = pPortConfig->Dot1VlanIdTxEnable.Length;
		break;
		case OHA_CTRL_DOT1_VLAN_NAME:	 /* lldpXdot1ConfigVlanNameTable "lldpXdot1ConfigVlanNameTxEnable" */
			*ppValue = &pPortConfig->Dot1VlanNameTxEnable.TmpSetVal;
			Length   = pPortConfig->Dot1VlanNameTxEnable.Length;
		break;
		case OHA_CTRL_DOT1_PROTO_VLAN: /* lldpXdot1ConfigProtoVlanTable "lldpXdot1ConfigProtoVlanTxEnable" */
			*ppValue = &pPortConfig->Dot1PortProtoVlanTxEnable.TmpSetVal;
			Length   = pPortConfig->Dot1PortProtoVlanTxEnable.Length;
		break;
		case OHA_CTRL_DOT1_PROTOCOL: /* lldpXdot1ConfigProtocolTable "lldpXdot1ConfigProtocolTxEnable" */
			*ppValue = &pPortConfig->Dot1ProtocolTxEnable.TmpSetVal;
			Length   = pPortConfig->Dot1ProtocolTxEnable.Length;
		break;
		case OHA_CTRL_LLDP_TLVS:    /* lldpPortConfigTable "lldpPortConfigTLVsTxEnable"           */
			*ppValue = &pPortConfig->TLVsTxEnable.TmpSetVal;
			Length   = pPortConfig->TLVsTxEnable.Length;
		break;
		case OHA_CTRL_DOT3_TLVS:    /* lldpXdot3PortConfigTable "lldpXdot3PortConfigTLVsTxEnable" */
			*ppValue = &pPortConfig->Dot3TLVsTxEnable.TmpSetVal;
			Length   = pPortConfig->Dot3TLVsTxEnable.Length;
		break;
		default:
			RetVal = OHA_ERR_PARAM;
			OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_read_temp_lldp_mib_port_value (No valid option to read ( NicId: 0x%X, PortOptions: 0x%X)",
								 NicId, PortOptions);
		break;
	}

	if(OHA_IS_NOT_NULL(pLength))
	{
		*pLength = Length;
	}

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_temp_lldp_mib_port_value(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return(RetVal);
}


/*=============================================================================
 * function name:  oha_db_read_temp_lldp_mib_if_value
 * function:       reades a temporary set port parameter of the lldp-mib object
 * parameters:     NicId
                   IfOptions (LLDP.MIB option flags)
                   ppValue (variable in octets)
                   Length (length of variable in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_temp_lldp_mib_if_value(
    LSA_UINT16                  NicId,
    LSA_UINT32                  IfOptions,/* type of variable   */
    OHA_COMMON_MEM_U8_PTR_TYPE  *ppValue,   /* variable in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pLength     /* length of variable in octets */
)
{
    OHA_DB_IF_PTR_TYPE           pDbIf  = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16                   RetVal = LSA_RET_OK;
	LSA_UINT16                   Length = 0;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_temp_lldp_mib_if_value(IfOptions: 0x%X, pLength: 0x%X, ppValue: 0x%X)",
                          IfOptions, pLength, ppValue);

    if (OHA_IS_NULL(pDbIf))            /* no interface */
    {
		RetVal = OHA_ERR_PARAM;
        OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_read_temp_lldp_mib_if_value(No such NicID: 0x%X , RetVal: 0x%X)", NicId, RetVal);
        return (RetVal);
    }
    if ( IfOptions == 0)
    {
		RetVal = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_read_temp_lldp_mib_if_value (No Parameter to read (IfOptions: 0x%X, pValue: 0x%X)",
							 IfOptions, ppValue);
        return (RetVal);
    }

    if ( OHA_IS_NULL(ppValue) )
    {
		RetVal = OHA_ERR_PARAM;
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_read_temp_lldp_mib_if_value (No Parameter given (IfOptions: 0x%X, ppValue: 0x%X)",
							 IfOptions, ppValue);
        return (RetVal);
    }

	switch (IfOptions)
	{
		case OHA_CTRL_LLDP_MAN_ADDR: /* lldpManAddrConfigTxPortsTable "lldpConfigManAddrPortsTxEnable" */
			*ppValue = (OHA_COMMON_MEM_U8_PTR_TYPE)&pDbIf->TempTxPortTable;
			Length   = sizeof(pDbIf->TempTxPortTable);
		break;
		default:
			RetVal = OHA_ERR_PARAM;
			OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_read_temp_lldp_mib_if_value (No valid option to read ( NicId: 0x%X, IfOptions: 0x%X)",
								 NicId, IfOptions);
		break;
	}

	if(OHA_IS_NOT_NULL(pLength))
	{
		*pLength = Length;
	}

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_temp_lldp_mib_if_value(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return(RetVal);
}

/*=============================================================================
 * function name:  oha_db_write_temp_lldp_mib_values
 * function:       writes the global mib objects to the temporary mib database
 * parameters:     Options (global option flags)
                   pConfig (global mib variables)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_write_temp_lldp_mib_values(
    LSA_UINT32                   Options,
	OHA_LLDP_IF_PARAM_TYPE OHA_COMMON_MEM_ATTR *pConfig
)
{
    LSA_UINT16                   RetVal = LSA_RET_OK;
	OHA_OID_LLDP_CONFIG_PTR_TYPE pDbLldpConfig = &oha_database.GlobalConfig;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_write_temp_lldp_mib_values(Options: 0x%X, pConfig: 0x%X)",
                          Options, pConfig);

    if ( Options == 0 || OHA_IS_NULL(pConfig))
    {
		RetVal = LSA_RET_ERR_PARAM;
        OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_write_temp_lldp_mib_values (No Parameter given (Options: 0x%X, pConfig: 0x%X)",
							 Options, pConfig);
        return (RetVal);
    }

	if (Options & OHA_CTRL_TX_INTERVAL)		/* lldpConfiguration "lldpMessageTxInterval" */
	{
		pDbLldpConfig->TxInterval.TmpSetVal = pConfig->TxInterval;
	}
	if (Options & OHA_CTRL_TX_HOLD_MULTIPLIER)	/* lldpConfiguration "lldpMessageTxHoldMultiplier" */
	{
		pDbLldpConfig->TxHoldMultiplier.TmpSetVal = pConfig->TxHoldMultiplier;
	}
	if (Options & OHA_CTRL_REINIT_DELAY)		/* lldpConfiguration "lldpReinitDelay" */
	{
		pDbLldpConfig->LldpReinitDelay.TmpSetVal = pConfig->ReinitDelay;
	}
	if (Options & OHA_CTRL_TX_DELAY)			/* lldpConfiguration "lldpTxDelay" */
	{
		pDbLldpConfig->LldpTxDelay.TmpSetVal = pConfig->TxDelay;
	}
	if (Options & OHA_CTRL_NOTIFICATION_INTERVAL) /* lldpConfiguration "lldpNotificationInterval" */
	{
		pDbLldpConfig->NotificationInterval.TmpSetVal = pConfig->NotificationInterval;
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_write_temp_lldp_mib_values(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*=============================================================================
 * function name:  OHA_GetGlobQueryOptions
 * function:       reads the global query mib options
 * parameters:     -
 * return value:   Options (global option flags)
 *===========================================================================*/
LSA_UINT32 OHA_GetGlobQueryOptions(LSA_VOID)
{
	LSA_UINT32 GlobQueryOptions = 0;
	LSA_UINT8  AccessControl    = OHA_NO_ACCESS;

	(LSA_VOID)OHA_DB_READ_LLDP_MIB_ACCESS(OHA_CTRL_TX_INTERVAL, &AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		GlobQueryOptions |= OHA_CTRL_TX_INTERVAL;
	}
	(LSA_VOID)OHA_DB_READ_LLDP_MIB_ACCESS(OHA_CTRL_TX_HOLD_MULTIPLIER, &AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		GlobQueryOptions |= OHA_CTRL_TX_HOLD_MULTIPLIER;
	}
	(LSA_VOID)OHA_DB_READ_LLDP_MIB_ACCESS(OHA_CTRL_REINIT_DELAY, &AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		GlobQueryOptions |= OHA_CTRL_REINIT_DELAY;
	}
	(LSA_VOID)OHA_DB_READ_LLDP_MIB_ACCESS(OHA_CTRL_TX_DELAY, &AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		GlobQueryOptions |= OHA_CTRL_TX_DELAY;
	}
	(LSA_VOID)OHA_DB_READ_LLDP_MIB_ACCESS(OHA_CTRL_NOTIFICATION_INTERVAL, &AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		GlobQueryOptions |= OHA_CTRL_NOTIFICATION_INTERVAL;
	}
	return(GlobQueryOptions);
}

/*=============================================================================
 * function name:  OHA_GetPortQueryOptions
 * function:       reades the port query mib options
 * parameters:     NicId
                   PortId
 * return value:   PortOptions (port option flags)
 *===========================================================================*/
LSA_UINT32 OHA_SYSTEM_OUT_FCT_ATTR OHA_GetPortQueryOptions(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId
)
{
    OHA_DB_IF_PTR_TYPE           pDbIf  = OHA_DB_GET_INTERFACE(NicId);
	OHA_OID_PORT_CONFIG_PTR_TYPE pPortConfig;
	LSA_UINT32 PortQueryOptions = 0;
	LSA_UINT8  AccessControl;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetPortQueryOptions(NicId: 0x%X, PortId: 0x%X)",
                          NicId, PortId);

    if (OHA_IS_NULL(pDbIf))            /* no interface */
    {
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: OHA_GetPortQueryOptions(No such NicID: 0x%X)", NicId);
        return (PortQueryOptions);
    }
    if (PortId == 0 || (PortId > OHA_CFG_MAX_PORTS))
    {
        /* Error on getting port. Abort. */
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: OHA_GetPortQueryOptions(Invalid PortId:  0x%X)", PortId);
        return (PortQueryOptions);
    }

	pPortConfig = &pDbIf->LldpData[PortId-1].PortConfig;

	OHA_DB_GET_MIB_ACCESS(&pPortConfig->AdminStatus, AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		PortQueryOptions |= OHA_CTRL_ADMIN_STATUS;
	}
	OHA_DB_GET_MIB_ACCESS(&pPortConfig->NotificationEnable, AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		PortQueryOptions |= OHA_CTRL_NOTIFICATION;
	}
	OHA_DB_GET_MIB_ACCESS(&pPortConfig->ManAddrTxEnable, AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		PortQueryOptions |= OHA_CTRL_LLDP_MAN_ADDR;
	}
	OHA_DB_GET_MIB_ACCESS(&pPortConfig->PnoDelayTxEnable, AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		PortQueryOptions |= OHA_CTRL_PNO_DELAY;
	}
	OHA_DB_GET_MIB_ACCESS(&pPortConfig->PnoPortStatusTxEnable, AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		PortQueryOptions |= OHA_CTRL_PNO_PORT_STATUS;
	}
	OHA_DB_GET_MIB_ACCESS(&pPortConfig->PnoAliasTxEnable, AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		PortQueryOptions |= OHA_CTRL_PNO_ALIAS;
	}
	OHA_DB_GET_MIB_ACCESS(&pPortConfig->PnoMrpTxEnable, AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		PortQueryOptions |= OHA_CTRL_PNO_MRP;
	}
	OHA_DB_GET_MIB_ACCESS(&pPortConfig->PnoPtcpTxEnable, AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		PortQueryOptions |= OHA_CTRL_PNO_PTCP;
	}

	OHA_DB_GET_MIB_ACCESS(&pPortConfig->Dot1VlanIdTxEnable, AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		PortQueryOptions |= OHA_CTRL_DOT1_VLAN_ID;
	}
	OHA_DB_GET_MIB_ACCESS(&pPortConfig->Dot1VlanNameTxEnable, AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		PortQueryOptions |= OHA_CTRL_DOT1_VLAN_NAME;
	}
	OHA_DB_GET_MIB_ACCESS(&pPortConfig->Dot1PortProtoVlanTxEnable, AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		PortQueryOptions |= OHA_CTRL_DOT1_PROTO_VLAN;
	}
	OHA_DB_GET_MIB_ACCESS(&pPortConfig->Dot1ProtocolTxEnable, AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		PortQueryOptions |= OHA_CTRL_DOT1_PROTOCOL;
	}

	OHA_DB_GET_MIB_ACCESS(&pPortConfig->TLVsTxEnable, AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		PortQueryOptions |= OHA_CTRL_LLDP_TLVS;
	}

	OHA_DB_GET_MIB_ACCESS(&pPortConfig->Dot3TLVsTxEnable, AccessControl);
	if (AccessControl == OHA_QUERY_WRITE)
	{
		PortQueryOptions |= OHA_CTRL_DOT3_TLVS;
	}

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetPortQueryOptions(NicId: 0x%X, PortQueryOptions: 0x%X)", NicId, PortQueryOptions);
    return(PortQueryOptions);
}

/*=============================================================================
 * function name:  oha_db_write_temp_lldp_mib_port_values
 * function:       writes the port mib objects to the temporary mib database
 * parameters:     Options (port option flags)
                   pPortConfig (port mib variables)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_write_temp_lldp_mib_port_values(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT32                  Options,
	OHA_LLDP_PORT_PARAM_TYPE OHA_COMMON_MEM_ATTR *pConfig
)
{
    OHA_DB_IF_PTR_TYPE           pDbIf  = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16                   RetVal = LSA_RET_OK;
	OHA_OID_PORT_CONFIG_PTR_TYPE pDbConfig;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_write_temp_lldp_mib_port_values(Options: 0x%X, pConfig: 0x%X)",
                          Options, pConfig);

    if (OHA_IS_NULL(pDbIf))            /* no interface */
    {
		RetVal = LSA_RET_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_write_temp_lldp_mib_port_values(No such NicID: 0x%X)", NicId);
        return (RetVal);
    }
    if (PortId == 0 || (PortId > OHA_CFG_MAX_PORTS))
    {
        /* Error on getting port. Abort. */
		RetVal = LSA_RET_ERR_PARAM;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_write_temp_lldp_mib_port_values(Invalid PortId:  0x%X)", PortId);
        return (RetVal);
    }
    if ( Options == 0 || OHA_IS_NULL(pConfig))
    {
		RetVal = LSA_RET_ERR_PARAM;
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_db_write_temp_lldp_mib_port_values (No Parameter given (Options: 0x%X, pConfig: 0x%X)",
							 Options, pConfig);
        return (RetVal);
    }

	pDbConfig = &pDbIf->LldpData[PortId-1].PortConfig;
	OHA_ASSERT_NULL_PTR(pDbConfig);

	if (Options & OHA_CTRL_ADMIN_STATUS)
	{
		pDbConfig->AdminStatus.TmpSetVal = pConfig->AdminStatus;
	}
	if (Options & OHA_CTRL_NOTIFICATION)
	{
		pDbConfig->NotificationEnable.TmpSetVal = pConfig->NotificationEnable;
	}
	if (Options & OHA_CTRL_LLDP_MAN_ADDR)
	{
		pDbConfig->ManAddrTxEnable.TmpSetVal = pConfig->ManAddrTxEnable;
	}
	if (Options & OHA_CTRL_PNO_DELAY)
	{
		pDbConfig->PnoDelayTxEnable.TmpSetVal = pConfig->PnoDelayTxEnable;
	}
	if (Options & OHA_CTRL_PNO_PORT_STATUS)
	{
		pDbConfig->PnoPortStatusTxEnable.TmpSetVal = pConfig->PnoPortStatusTxEnable;
	}
	if (Options & OHA_CTRL_PNO_ALIAS)
	{
		pDbConfig->PnoAliasTxEnable.TmpSetVal = pConfig->PnoAliasTxEnable;
	}
	if (Options & OHA_CTRL_PNO_MRP)
	{
		pDbConfig->PnoMrpTxEnable.TmpSetVal = pConfig->PnoMrpTxEnable;
	}
	if (Options & OHA_CTRL_PNO_PTCP)
	{
		pDbConfig->PnoPtcpTxEnable.TmpSetVal = pConfig->PnoPtcpTxEnable;
	}
	if (Options & OHA_CTRL_DOT1_VLAN_ID)
	{
		pDbConfig->Dot1VlanIdTxEnable.TmpSetVal = pConfig->Dot1VlanIdTxEnable;
	}
	if (Options & OHA_CTRL_DOT1_VLAN_NAME)
	{
		pDbConfig->Dot1VlanNameTxEnable.TmpSetVal = pConfig->Dot1VlanNameTxEnable;
	}
	if (Options & OHA_CTRL_DOT1_PROTO_VLAN)
	{
		pDbConfig->Dot1PortProtoVlanTxEnable.TmpSetVal = pConfig->Dot1PortProtoVlanTxEnable;
	}
	if (Options & OHA_CTRL_DOT1_PROTOCOL)
	{
		pDbConfig->Dot1ProtocolTxEnable.TmpSetVal = pConfig->Dot1ProtocolTxEnable;
	}
	if (Options & OHA_CTRL_LLDP_TLVS)
	{
		pDbConfig->TLVsTxEnable.TmpSetVal = pConfig->TLVsTxEnable;
	}
	if (Options & OHA_CTRL_DOT3_TLVS)
	{
		pDbConfig->Dot3TLVsTxEnable.TmpSetVal = pConfig->Dot3TLVsTxEnable;
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_write_temp_lldp_mib_port_values(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*=============================================================================
 * function name:  OHA_DB_READ_LLDP_PORT_ID
 * function:       Get pointer and length of the LLDP_PortID string
 *                 IfMode := 0 LLDP_PortID = NameOfPort, e.g. "port-001"
 *                 IfMode := 1 LLDP_PortID = NameOfPort.NameOfStation
 * parameters:     OHA_COMMON_MEM_U8_PTR_TYPE * AliasNamePtrPtr:  Pointer for Ret.Ptr
 * return value:   LSA_UINT16:                             Length
 *===========================================================================*/
LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_LLDP_PORT_ID(
    LSA_UINT16  NicId,
    LSA_UINT16  PortId,
    OHA_COMMON_MEM_U8_PTR_TYPE * NamePtrPtr
)
{
    LSA_UINT8  SubType = 0;
    OHA_COMMON_MEM_U8_PTR_TYPE pVar = LSA_NULL;
    LSA_UINT16 Len = 0;
	LSA_UINT16 RetVal = LSA_RET_OK;
    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NicId);

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_LLDP_PORT_ID(NicId: 0x%X, PortId: 0x%X, NamePtrPtr: 0x%X)",
                          NicId, PortId, NamePtrPtr);

	OHA_ASSERT_NULL_PTR(pDB);
	OHA_ASSERT(PortId && PortId <= OHA_CFG_MAX_PORTS);

	/* NameOfPort */
    (LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId,
                                           OHA_LLDP_TYPE_PORT_ID,
                                           &SubType, 0,
                                           &pVar, &Len);

    /* check, if PortID is a RFC-name */
    if (!(oha_is_valid_port_name(Len, pVar)))
    {
        *NamePtrPtr = LSA_NULL;
		RetVal      = OHA_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,
                              "OUT: OHA_DB_READ_LLDP_PORT_ID(RetVal: 0x%X)", RetVal);
        return(0);
    }

	*NamePtrPtr = pVar;

	/* LLDP_PortID = NameOfPort + "." + NameOfStation or NameOfPort + "." */
    if (pDB->MultipleIfModeValid & OHA_NAME_OF_DEVICE_MASK)
	{
		OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
		LSA_UINT16 NoSLen = (LSA_UINT16)OHA_DB_READ_STATION_NAME(pDB->NicId, &NamePtr);
		/* if (NoSLen) */   /* NameOfStation present or not */
		{
            LSA_UINT16   FreeStat = LSA_RET_OK;
			OHA_LOCAL_MEM_PTR_TYPE MemPtr = LSA_NULL;

			/* we need a (new) buffer for the alias name */
			if(OHA_IS_NOT_NULL(pDB->LldpData[PortId-1].pLldpPortID))
			{
                OHA_FREE_LOCAL_MEM(&FreeStat, pDB->LldpData[PortId-1].pLldpPortID);
                OHA_MEM_FREE_FAIL_CHECK(FreeStat);
                pDB->LldpData[PortId-1].pLldpPortID = LSA_NULL;
			}
            OHA_ALLOC_LOCAL_MEM(&MemPtr, Len + 1 + NoSLen);
            if (OHA_IS_NULL(MemPtr))
            {
                *NamePtrPtr = LSA_NULL;
				RetVal      = OHA_ERR_RESOURCE;
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,
                                      "OUT: OHA_DB_READ_LLDP_PORT_ID(RetVal: 0x%X)", RetVal);
                return(0);
			}

			pDB->LldpData[PortId-1].pLldpPortID = (OHA_COMMON_MEM_U8_PTR_TYPE)MemPtr;

			OHA_MEMCPY(pDB->LldpData[PortId-1].pLldpPortID, pVar, Len);
			OHA_MEMCPY(pDB->LldpData[PortId-1].pLldpPortID + Len, (OHA_COMMON_MEM_PTR_TYPE)".", 1);
			OHA_MEMCPY(pDB->LldpData[PortId-1].pLldpPortID + Len + 1, NamePtr, NoSLen);

            *NamePtrPtr = pDB->LldpData[PortId-1].pLldpPortID;
			Len = (LSA_UINT16)(Len + 1 + NoSLen);
		}
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_LLDP_PORT_ID(RetVal: 0x%X)", RetVal);
    return Len;
}

/*=============================================================================
 * function name:  OHA_DB_READ_LLDP_CHASSIS_ID
 * function:       Get pointer and length of the LLDP_ChassisID string
 * parameters:     NicId
                   SubType (LLDP-suboption)
                   NamePtrPtr (LLDP_ChassisID in octets)
 * return value:   LSA_UINT16 Length (length of name in octets)
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_LLDP_CHASSIS_ID(
    LSA_UINT16                  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE * NamePtrPtr, /* LLDP_ChassisID in octets */
    OHA_COMMON_MEM_U8_PTR_TYPE  pSubType     /* subtype of variable, suboption */
)
{
    OHA_COMMON_MEM_U8_PTR_TYPE pVar = LSA_NULL;
    LSA_UINT16 Len = 0;
    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NicId);

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_LLDP_CHASSIS_ID(NicId: 0x%X, NamePtrPtr: 0x%X), pSubType: 0x%X",
                          NicId, NamePtrPtr, pSubType);

	OHA_ASSERT_NULL_PTR(pDB);

    if (pDB->MultipleIfModeValid & OHA_NAME_OF_DEVICE_MASK)
	{
		(LSA_VOID)OHA_DB_READ_CHASSIS_NAME(&pVar, &Len);
		if (Len != 0 && OHA_IS_NOT_NULL(pVar))	/* NameOfDevice existing */
		{
			if(OHA_IS_NOT_NULL(NamePtrPtr)) { *NamePtrPtr = pVar; }
			if(OHA_IS_NOT_NULL(pSubType))   { *pSubType = OHA_CHASSIS_ID_LOCALLY_ASSIGNED; }
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
								  "OUT: OHA_DB_READ_LLDP_CHASSIS_ID(Len: 0x%X)", Len);
			return Len;
		}
	}

	/* AP01234692: lldpLocChassisId    := NameOfStation or StationMAC */
	Len = (LSA_UINT16)OHA_DB_READ_STATION_NAME(NicId, &pVar);
	if (Len != 0 && OHA_IS_NOT_NULL(pVar))	/* station name present */
	{
		/* check, if ChassisID is a valid name */
		if (oha_is_valid_station_name(Len, pVar, OHA_UPPER_CASES_ALLOWED(NicId), OHA_UPPER_CASES_ALLOWED(NicId)))
		{
			if(OHA_IS_NOT_NULL(NamePtrPtr)) { *NamePtrPtr = pVar; }
			if(OHA_IS_NOT_NULL(pSubType))   { *pSubType = OHA_CHASSIS_ID_LOCALLY_ASSIGNED; }
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
								  "OUT: OHA_DB_READ_LLDP_CHASSIS_ID(Len: 0x%X)", Len);
			return Len;
		}
	}

	Len = (LSA_UINT16)OHA_DB_READ_MAC_ADDR(NicId, &pVar);
	if (Len != 0 && OHA_IS_NOT_NULL(pVar))	/* MAC present */
	{
			if(OHA_IS_NOT_NULL(NamePtrPtr)) { *NamePtrPtr = pVar; }
			if(OHA_IS_NOT_NULL(pSubType))   { *pSubType = OHA_CHASSIS_ID_MAC_ADDR; }
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
								  "OUT: OHA_DB_READ_LLDP_CHASSIS_ID(Len: 0x%X)", Len);
			return Len;
	}

	/* AP01116732: no MAC (or 00-00-00-00-00-00) */
	(LSA_VOID)OHA_DB_READ_CHASSIS_NAME(&pVar, &Len);
	if(Len != 0 && OHA_IS_NOT_NULL(pVar))	/* NameOfDevice existing */
	{
		if(OHA_IS_NOT_NULL(NamePtrPtr)) { *NamePtrPtr = pVar; }
		if(OHA_IS_NOT_NULL(pSubType))   { *pSubType = OHA_CHASSIS_ID_LOCALLY_ASSIGNED; }
		OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
							  "OUT: OHA_DB_READ_LLDP_CHASSIS_ID(Len: 0x%X)", Len);
		return Len;
	}

	if(OHA_IS_NOT_NULL(NamePtrPtr)) { *NamePtrPtr = LSA_NULL; }
	if(OHA_IS_NOT_NULL(pSubType))   { *pSubType = 0; }

	OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
						  "OUT: OHA_DB_READ_LLDP_CHASSIS_ID(Len: 0x%X)", Len);
    return 0;
}

/*=============================================================================
 * function name:  oha_db_read_rem_chassis_name
 * function:       reades the ChassisID of the remote station
 * parameters:     NicId
                   pChassisID (ChassisID in octets)
                   Len   (length of ChassisID in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_rem_chassis_name(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId,      /* index + 1 to RemoteSender[] - for SNMP */
    OHA_COMMON_MEM_U8_PTR_TYPE  pSubType,      /* subtype of ChassisID, suboption */
    OHA_COMMON_MEM_U8_PTR_TYPE * ppChassis,    /* ChassisID in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pChassisLen    /* length of ChassisID in octets */
)
{
    OHA_COMMON_MEM_U8_PTR_TYPE pPortID = LSA_NULL;
	LSA_UINT16 PortLen  = 0;
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_05(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_rem_chassis_name(NicId: 0x%X, PortId: 0x%X, SenderId: 0x%X, ppChassis: 0x%X, pChassisLen: 0x%X)",
                          NicId, PortId, SenderId, ppChassis, pChassisLen);
	
	OHA_ASSERT_NULL_PTR(ppChassis);
	OHA_ASSERT_NULL_PTR(pChassisLen);
	OHA_ASSERT_NULL_PTR(pSubType);

	/* MultipleInterfaceMode.NameOfDevice(1): NameOfPort + "." + NameOfStation or NameOfPort + "." ? */
	RetVal = oha_db_read_rem_lldp_element(NicId, PortId, SenderId,
										   OHA_LLDP_TYPE_PORT_ID,
										   pSubType, 0,
										   &pPortID, &PortLen);

	if (OHA_IS_NOT_NULL(pPortID) && (PortLen <= OHA_MAX_PORT_STRLEN) &&
		*pSubType == OHA_PORT_ID_LOCALLY_ASSIGNED)
	{
		LSA_UINT16 Len;
		OHA_COMMON_MEM_U8_PTR_TYPE pStr = pPortID;

		/* scan the string for a "." */
		for(Len=0; Len < PortLen; Len++, pStr++)
		{
			if(*pStr == '.')
			{
				/* check, if PortID is valid */
				if (oha_is_valid_port_name(Len, pPortID))
				{
					Len++;
					*pChassisLen = (LSA_UINT16)(PortLen - Len);
					if(*pChassisLen)
					{
						pStr++;
					}
					else /* len = 0 */
					{
						pStr = (OHA_COMMON_MEM_U8_PTR_TYPE)pChassisLen; /* not NULL */
					}

					*ppChassis = pStr;
					*pSubType  = OHA_CHASSIS_ID_LOCALLY_ASSIGNED;
					OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_rem_chassis_name(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
					return RetVal;
				}
				else
				{
                    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "Get remote ChassisID: Not a vaild remote LLDP_PortID (NicId: 0x%X, PortId: 0x%X, SenderId: 0x%X)", NicId, PortId, SenderId);
					break;
				}
			}
		}
	}

	/* MultipleInterfaceMode.NameOfDevice(0): NameOfStation */
	RetVal = oha_db_read_rem_lldp_element(NicId, PortId, SenderId,
										   OHA_LLDP_TYPE_CHASSIS_ID,
										   pSubType, 0,
										   ppChassis, pChassisLen);

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_rem_chassis_name(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_db_read_rem_port_name
 * function:       reades the NameOfPort of the remote station
 * parameters:     NicId
                   pName (NameOfPort in octets)
                   Len   (length of NameOfPort in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR oha_db_read_rem_port_name(
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    LSA_UINT16                  SenderId,   /* index + 1 to RemoteSender[] - for SNMP */
	OHA_COMMON_MEM_U8_PTR_TYPE  pSubType,   /* subtype of PortID, suboption */
    OHA_COMMON_MEM_U8_PTR_TYPE * ppPortName,/* NameOfPort in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pPortLen    /* length of NameOfPort in octets */
)
{
	LSA_UINT8  SubType  = 0xff;
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_05(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_db_read_rem_port_name(NicId: 0x%X, PortId: 0x%X, SenderId: 0x%X, ppPortName: 0x%X, pPortLen: 0x%X)",
                          NicId, PortId, SenderId, ppPortName, pPortLen);

	OHA_ASSERT_NULL_PTR(ppPortName);
	OHA_ASSERT_NULL_PTR(pPortLen);

	RetVal = oha_db_read_rem_lldp_element(NicId, PortId, SenderId,
										   OHA_LLDP_TYPE_PORT_ID,
										   &SubType, 0,
										   ppPortName, pPortLen);

	if (OHA_IS_NOT_NULL(pSubType)) /* SubType returned (TFS 1025560) */
	{
		*pSubType = SubType;
	}

	if (OHA_IS_NOT_NULL(*ppPortName) && (*pPortLen <= OHA_MAX_PORT_STRLEN))
	{
		LSA_UINT16 Len;
		OHA_COMMON_MEM_U8_PTR_TYPE pStr = *ppPortName;

		if(SubType != OHA_PORT_ID_LOCALLY_ASSIGNED)
		{
			OHA_PROGRAM_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "~~~No (valid) LLDP_PortID present PortId: 0x%X, SenderId: 0x%X, PortID-Subtype: 0x%X", 
								 PortId,SenderId,SubType);

		    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_rem_port_name(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
			return(RetVal);
		}

		/* MultipleInterfaceMode.NameOfDevice(0): NameOfPort */
		/* MultipleInterfaceMode.NameOfDevice(1): NameOfPort + "." + NameOfStation or NameOfPort + "." */

		for(Len=0; Len < *pPortLen; Len++, pStr++)
		{
			if(*pStr == '.')
			{
				/* check, if PortID is valid */
				if (oha_is_valid_port_name(Len, *ppPortName))
				{
					*pPortLen = Len;
					break;
				}
				else
				{
                    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "Get remote PortID: Not a vaild remote LLDP_PortID (NicId: 0x%X, PortId: 0x%X, SenderId: 0x%X)", NicId, PortId, SenderId);
					break;
				}
			}
		}
	}

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_db_read_rem_port_name(NicId: 0x%X, RetVal: 0x%X)", NicId, RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  oha_port_names_valid
 * function:       Get pointer and length of all LLDP_PortID strings and
 *                 check, if PortIDs have a RFC-name
 * parameters:     LSA_UINT16 NicId
 * return value:   LSA_BOOL                  
 *===========================================================================*/
LSA_BOOL  OHA_SYSTEM_OUT_FCT_ATTR  oha_port_names_valid(
    LSA_UINT16  NicId
)
{
	LSA_UINT16 PortId;

    OHA_DB_IF_PTR_TYPE pDB = OHA_DB_GET_INTERFACE(NicId);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_port_names_valid(NicId: 0x%X", NicId);

	OHA_ASSERT_NULL_PTR(pDB);

	for (PortId = 1; PortId <= pDB->PortCnt; PortId++)
	{
		OHA_COMMON_MEM_U8_PTR_TYPE pVar = LSA_NULL;
		LSA_UINT8  SubType = 0;
		LSA_UINT16 Len = 0;

		(LSA_VOID)oha_db_read_loc_lldp_element(NicId, PortId, 	/* NameOfPort */
											   OHA_LLDP_TYPE_PORT_ID,
											   &SubType, 0,
											   &pVar, &Len);
		if(OHA_IS_NULL(pVar))
		{
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,
								  "OUT: oha_port_names_valid failed. No PortId (0x%X) set!", PortId);
			return(LSA_FALSE);
		}

		/* check, if PortID is a RFC-name */
		if (!(oha_is_valid_port_name(Len, pVar)))
		{
			OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,
								  "OUT: oha_port_names_valid failed. Not a RFC-name at PortId (0x%X)!", PortId);
			return(LSA_FALSE);
		}
	}

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_port_names_valid()");
    return LSA_TRUE;
}

/*****************************************************************************/
/*  end of file OHA_DB_L.C                                                   */
/*****************************************************************************/

