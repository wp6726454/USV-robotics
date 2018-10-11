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
/*  F i l e               &F: oha_rema.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA REMA functions                               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  15.10.08    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  37
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_REMA */

#include "oha_inc.h"            /* OHA headerfiles */
#include "oha_int.h"            /* internal header */

/* BTRACE-IF */
OHA_FILE_SYSTEM_EXTENSION(OHA_MODULE_ID)

#ifndef OHA_CFG_NO_REMA

LSA_UINT16 OHA_GetMultiplePDEVData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen);

LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateRemaPDEV(
    LSA_UINT16 NICId,
    OHA_HDB_USER_PTR_TYPE pHDBUser);

LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateRemaStation(
    LSA_UINT16 NICId,
    OHA_HDB_USER_PTR_TYPE pHDBUser);

LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateRemaNameOfStation(
    LSA_UINT16 NICId,
    OHA_HDB_USER_PTR_TYPE pHDBUser);

LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateRemaIPSuite(
    LSA_UINT16 NICId,
    OHA_HDB_USER_PTR_TYPE pHDBUser);

LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateRemaDHCP(
    LSA_UINT16 NICId,
    OHA_HDB_USER_PTR_TYPE pHDBUser);

LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateRemaSnmp(
    LSA_UINT16 NICId,
    OHA_HDB_USER_PTR_TYPE pHDBUser);

LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateRemaBus(
    LSA_UINT16 NICId,
    OHA_HDB_USER_PTR_TYPE pHDBUser);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestRemaIndication                   +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
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
/*+     LSA_OPCODE_TYPE         opcode:      OHA_OPC_REMA_INDICATION        +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LSA_UINT16              Response     Response                       +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.RemaInd                                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to provide REMA Indication Resources              +*/
/*+                                                                         +*/
/*+               Stores the RQB wthin User-Handlemanagement                +*/
/*+               If this is the first RQB we check if some indications     +*/
/*+               pending to be indicated. If so we indicate them, if not   +*/
/*+               we queue the request.                                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestRemaIndication(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    LSA_UINT16      NICId, RemaStationFormat;
    LSA_HANDLE_TYPE Handle;
    OHA_HDB_REMA_PTR_TYPE   pHDBRema;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestRemaIndication(pRQB: 0x%X, pHDB: 0x%X)",
                          pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);

    NICId    = pHDB->Params.pDetail->Params.User.NICId;
	RemaStationFormat = pHDB->Params.pDetail->Params.User.RemaStationFormat;

    pHDBRema = OHA_REMAGetHandleByNIC(NICId);

	if ((OHA_IS_NULL(pHDBRema)) || (!OHA_EDDCheckNICUsed(NICId)))
    {
        /* Error: no such interface. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_REMA_INDICATION failed. No valid NicID for this interface (0x%X)",NICId);

		OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestRemaIndication()");
		OHA_UserRequestFinish(pHDB,pRQB,OHA_ERR_RESOURCE);
        return;
    }

	if (RemaStationFormat != OHA_REMA_STATION_FORMAT_STANDARD &&
		RemaStationFormat != OHA_REMA_STATION_FORMAT_ADVANCED)
	{
        /* Error: no such REMA user. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_REMA_INDICATION failed. No valid RemaStationFormat for this user (0x%X)", RemaStationFormat);

		OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestRemaIndication()");
		OHA_UserRequestFinish(pHDB,pRQB,OHA_ERR_PARAM);
        return;
	}

    Handle = (LSA_HANDLE_TYPE)OHA_HANDLE_IDX(pHDB->ThisHandle);
    OHA_ASSERT(Handle < OHA_CFG_MAX_CHANNELS);

    /* (i) rema PDEV data written anyway ? */
    if (pHDBRema->PDEV_Last_Ind.data_tag)
    {
        /* are the rema indications locked? */
        if (!pHDBRema->Ind_Locked[Handle])
        {
            /* are the rema data already indicated or read? */
            if (!pHDBRema->PDEV_Ind_Done[Handle])
            {
                LSA_UINT16 Response;

                /* build a multiple PDEV record for the REMA user(s) */
                pRQB->args.RemaData.index = OHA_RECORD_INDEX_MULTIPLE_PDEV;
                pRQB->args.RemaData.data_tag = OHA_PDEV_DATA;   /* pdev data */

                Response = OHA_GetMultiplePDEVData(NICId,
                                                     pRQB->args.RemaData.data,
                                                     &pRQB->args.RemaData.data_length);

                pHDBRema->PDEV_Ind_Done[Handle] = LSA_TRUE; /* rema indicated */

                OHA_UserRequestFinish(pHDB,pRQB,Response);

                OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestRemaIndication(Resp: 0x%X)", Response);
                return;
            }
        }
        else
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"REMA-Indications are locked (pHDB: 0x%X)", pHDB);
        }
    }

    /* COMPARE WITH THE BUFFERED REMA-RECORD, ONLY INDICATE IF DIFFERENT */

    /* (ii) rema Station data - MultipleStationRecord(IP, NoS) written anyway ? */
    if (pHDBRema->Station_Last_Ind.data_tag && 
		RemaStationFormat == OHA_REMA_STATION_FORMAT_STANDARD)
    {
        /* AP00958658, if not indicated */
        {
            /* are the rema indications locked? */
            if (!pHDBRema->Ind_Locked[Handle])
            {
                /* are the rema data already indicated or read? */
                if (!pHDBRema->Station_Ind_Done[Handle])
                {
                    LSA_UINT16 Response;

                    /* build a multiple Station record for the REMA user(s) */
                    pRQB->args.RemaData.index = OHA_RECORD_INDEX_MULTIPLE_STATION;
                    pRQB->args.RemaData.data_tag = OHA_STATION_DATA;

                    Response = OHA_GetMultipleStationData(NICId,
                                                            pRQB->args.RemaData.data,
                                                            &pRQB->args.RemaData.data_length,
                                                            LSA_TRUE);

                    pHDBRema->Station_Ind_Done[Handle] = LSA_TRUE; /* rema indicated */

                    OHA_UserRequestFinish(pHDB,pRQB,Response);

                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestRemaIndication(Resp: 0x%X)", Response);
                    return;
                }
            }
            else
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"REMA-Indications are locked (pHDB: 0x%X)", pHDB);
            }
        }
    }

    /* (ii-a) rema Station data - NameOfStationRecord written anyway ? */ /* AP01234182 */
    if (pHDBRema->NameOfStation_Last_Ind.data_tag &&
		RemaStationFormat == OHA_REMA_STATION_FORMAT_ADVANCED)
    {
        /* AP00958658, if not indicated */
        {
            /* are the rema indications locked? */
            if (!pHDBRema->Ind_Locked[Handle])
            {
                /* are the rema data already indicated or read? */
                if (!pHDBRema->NameOfStation_Ind_Done[Handle])
                {
                    LSA_UINT16 Response;

                    /* build NameOfStationRecord for the REMA user(s) */
                    pRQB->args.RemaData.index = OHA_RECORD_INDEX_NAME_OF_STATION;
                    pRQB->args.RemaData.data_tag = OHA_STATION_DATA;

                    Response = OHA_GetNameOfStationData(NICId,
                                                        pRQB->args.RemaData.data,
                                                        &pRQB->args.RemaData.data_length,
                                                        LSA_TRUE);

                    pHDBRema->NameOfStation_Ind_Done[Handle] = LSA_TRUE; /* rema indicated */

                    OHA_UserRequestFinish(pHDB,pRQB,Response);

                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestRemaIndication(Resp: 0x%X)", Response);
                    return;
                }
            }
            else
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"REMA-Indications are locked (pUser: 0x%X)", pHDB);
            }
        }
    }

	/* (ii-b) rema Station data - IPSuiteRecord written anyway ? */ /* AP01234182 */
    if (pHDBRema->IPSuite_Last_Ind.data_tag &&
		RemaStationFormat == OHA_REMA_STATION_FORMAT_ADVANCED)
    {
        /* AP00958658, if not indicated */
        {
            /* are the rema indications locked? */
            if (!pHDBRema->Ind_Locked[Handle])
            {
                /* are the rema data already indicated or read? */
                if (!pHDBRema->IPSuite_Ind_Done[Handle])
                {
                    LSA_UINT16 Response;

                    /* build IPSuiteRecord for the REMA user(s) */
                    pRQB->args.RemaData.index = OHA_RECORD_INDEX_IP_SUITE;
                    pRQB->args.RemaData.data_tag = OHA_STATION_DATA;

                    Response = OHA_GetIPSuiteData(NICId,
                                                  pRQB->args.RemaData.data,
                                                  &pRQB->args.RemaData.data_length,
                                                  LSA_TRUE);

                    pHDBRema->IPSuite_Ind_Done[Handle] = LSA_TRUE; /* rema indicated */

                    OHA_UserRequestFinish(pHDB,pRQB,Response);

                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestRemaIndication(Resp: 0x%X)", Response);
                    return;
                }
            }
            else
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"REMA-Indications are locked (pUser: 0x%X)", pHDB);
            }
        }
    }

	/* (ii-c) rema Station data - DHCPData written anyway ? */
    if (pHDBRema->DHCPData_Last_Ind.data_tag &&
		RemaStationFormat == OHA_REMA_STATION_FORMAT_ADVANCED)
    {
        {
            /* are the rema indications locked? */
            if (!pHDBRema->Ind_Locked[Handle])
            {
                /* are the rema data already indicated or read? */
                if (!pHDBRema->DHCPData_Ind_Done[Handle])
                {
                    LSA_UINT16 Response;

                    /* build DHCPParametersRecord for the REMA user(s) */
                    pRQB->args.RemaData.index = OHA_RECORD_INDEX_DHCP_PARAMS;
                    pRQB->args.RemaData.data_tag = OHA_STATION_DATA;

                    Response = OHA_GetDHCPData(NICId,
                                               pRQB->args.RemaData.data,
                                               &pRQB->args.RemaData.data_length,
                                               LSA_TRUE);

                    pHDBRema->DHCPData_Ind_Done[Handle] = LSA_TRUE; /* rema indicated */

                    OHA_UserRequestFinish(pHDB,pRQB,Response);

                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestRemaIndication(Resp: 0x%X)", Response);
                    return;
                }
            }
            else
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"REMA-Indications are locked (pUser: 0x%X)", pHDB);
            }
        }
    }

    /* (iii) rema SNMP data (SysName, SysLocation, SysContact) written anyway ? */
    if (pHDBRema->Snmp_Last_Ind.data_tag)
    {
        /* AP00958658, if not indicated */
        {
            /* are the rema indications locked? */
            if (!pHDBRema->Ind_Locked[Handle])
            {
                /* are the rema data already indicated or read? */
                if (!pHDBRema->Snmp_Ind_Done[Handle])
                {
                    LSA_UINT16 Response;

                    /* build a multiple SNMP record for the REMA user(s) */
                    pRQB->args.RemaData.index = OHA_RECORD_INDEX_MULTIPLE_SNMP;
                    pRQB->args.RemaData.data_tag = OHA_SNMP_DATA;

                    Response = OHA_GetMultipleSNMPData(NICId,
                                                         pRQB->args.RemaData.data,
                                                         &pRQB->args.RemaData.data_length,
                                                         LSA_TRUE);

                    pHDBRema->Snmp_Ind_Done[Handle] = LSA_TRUE; /* rema indicated */

                    OHA_UserRequestFinish(pHDB,pRQB,Response);

                    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestRemaIndication(Resp: 0x%X)", Response);
                    return;
                }
            }
            else
            {
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"REMA-Indications are locked (pHDB: 0x%X)", pHDB);
            }
        }
    }

    /* queue the request */
	OHA_ASSERT(pHDBRema->this_idx < OHA_CFG_MAX_NICS);
    OHA_RQB_PUT_BLOCK_TOP(pHDB->RemaInd[pHDBRema->this_idx].pBottom,
                          pHDB->RemaInd[pHDBRema->this_idx].pTop,
                          pRQB);

    pHDB->RemaIndPending[pHDBRema->this_idx]++;

    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestRemaIndication()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestRemaControl                      +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_REMA_CONTROL            +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response     Response                        +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.RemaControl                             +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to control the REMA for sending rema indications  +*/
/*+               to the user or not                                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestRemaControl(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDB)
{
    LSA_UINT16      NICId, RemaStationFormat;
    LSA_HANDLE_TYPE Handle;
    OHA_HDB_REMA_PTR_TYPE   pHDBRema;
    LSA_UINT16      Response = OHA_OK;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestRemaControl(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDB);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDB);

    NICId = pHDB->Params.pDetail->Params.User.NICId;
	RemaStationFormat = pHDB->Params.pDetail->Params.User.RemaStationFormat;

    /* check the mode */
    if (pRQB->args.RemaControl.Mode != OHA_HOLD_REMA_IND &&
        pRQB->args.RemaControl.Mode != OHA_UNHOLD_REMA_IND)
    {
        OHA_UserRequestFinish(pHDB,pRQB,OHA_ERR_PARAM);
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN, "ParamError - Mode (0x%X): OHA_RequestRemaControl()", pRQB->args.RemaControl.Mode);
        return;
    }

    pHDBRema = OHA_REMAGetHandleByNIC(NICId);

    if (OHA_IS_NULL(pHDBRema))
    {
        OHA_UserRequestFinish(pHDB,pRQB,OHA_ERR_PARAM);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_WARN, "ParamError - NICId (0x%X): OHA_RequestRemaControl()", NICId);
        return;
    }

    Handle = (LSA_HANDLE_TYPE)OHA_HANDLE_IDX(pHDB->ThisHandle);
    OHA_ASSERT(Handle < OHA_CFG_MAX_CHANNELS);

    switch (pRQB->args.RemaControl.Mode)
    {
        case OHA_HOLD_REMA_IND:  /* send no rema indications to the user */
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"REMA-Control HOLD successful");
            pHDBRema->Ind_Locked[Handle] = LSA_TRUE;
            break;
        case OHA_UNHOLD_REMA_IND:  /* send rema indications to the user */
            OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"REMA-Control UNHOLD successful");
            pHDBRema->Ind_Locked[Handle] = LSA_FALSE;

            if (!pHDBRema->PDEV_Ind_Done[Handle])
            {
                OHA_IndicateRemaPDEV(NICId, pHDB);
            }
            if ((!pHDBRema->Station_Ind_Done[Handle]) && (RemaStationFormat == OHA_REMA_STATION_FORMAT_STANDARD))
            {
                OHA_IndicateRemaStation(NICId, pHDB);
            }
            if ((!pHDBRema->NameOfStation_Ind_Done[Handle]) && (RemaStationFormat == OHA_REMA_STATION_FORMAT_ADVANCED))
            {
                OHA_IndicateRemaNameOfStation(NICId, pHDB);
            }
            if ((!pHDBRema->IPSuite_Ind_Done[Handle]) && (RemaStationFormat == OHA_REMA_STATION_FORMAT_ADVANCED))
            {
                OHA_IndicateRemaIPSuite(NICId, pHDB);
            }
            if ((!pHDBRema->DHCPData_Ind_Done[Handle]) && (RemaStationFormat == OHA_REMA_STATION_FORMAT_ADVANCED))
            {
                OHA_IndicateRemaDHCP(NICId, pHDB);
            }
            if (!pHDBRema->Snmp_Ind_Done[Handle])
            {
                OHA_IndicateRemaSnmp(NICId, pHDB);
            }
            break;
        default:
            Response = OHA_ERR_SYNTAX;
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"REMA-Control failed.[0x%X]", Response);
            break;
    }

    OHA_UserRequestFinish(pHDB,pRQB,Response);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestRemaControl (Response: 0x%X)", Response);
}

/*----------------------------------------------------------------------------*/

/* build a multiple PDEV record:

   RemaWriteMultiple    RemaWriteHeader a, (RemaWriteHeader, RemaRecordData, [Padding*] a)*

      a) The number of padding octets shall be adapted to make the block Unsigned32 aligned.

   RemaWriteHeader  RecordIndex (u32), API, SlotNumber, SubslotNumber, RemaRecordDataLength (u32)
                    RemaCompID(u16), Reserved(u16)

   RemaRecordData   RecordDataWrite ^ Data* ^ NameOfStationBlock b ^ IPParameterBlock b

      b) syntax see pnio draft
*/
LSA_UINT16 OHA_GetMultiplePDEVData(
    LSA_UINT16 NICId,
    OHA_UPPER_MEM_U8_PTR_TYPE pData,
    OHA_COMMON_MEM_U32_PTR_TYPE pDataLen)
{
    OHA_HDB_REMA_PTR_TYPE pHDBRema = OHA_REMAGetHandleByNIC(NICId);
    LSA_UINT16 RetVal   = OHA_OK;

    OHA_UPPER_MEM_U8_PTR_TYPE pRecordData = pData;
    LSA_UINT32 list_cnt;
    LSA_UINT32 block_length = 0;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_GetMultiplePDEVData(NICId: 0x%X, pData: 0x%X, pDataLen: 0x%X)",
                          NICId, pData, pDataLen);

    OHA_ASSERT_NULL_PTR(pDataLen);
    OHA_ASSERT_NULL_PTR(pHDBRema);

    OhaListLength (&pHDBRema->set_a, &list_cnt);

    if (!OhaListIsEmpty(&pHDBRema->set_a))
    {
        OHA_PRM_RECORD_PTR_TYPE rec;

        for (OhaListForeach (rec, &pHDBRema->set_a, OHA_PRM_RECORD_PTR_TYPE))
        {
            /* RemaWriteHeader */
            block_length += OHA_REMA_RECORD_HEADER_LENGTH;

            if (OHA_IS_NOT_NULL(pData))
            {
                if (block_length > *pDataLen) goto RecLenFailed;

                /* RemaWriteHeader: RecordIndex (u32), API, SlotNumber, SubslotNumber,   */
                /*                  RemaRecordDataLength, Reserved(u16), Reserved(u16)   */
                OHA_PDU_SET_U32 (pRecordData, rec->rec.record_index);
                OHA_PDU_SET_U32 (pRecordData, 0);                       /* API (default) */
                OHA_PDU_SET_U16 (pRecordData, rec->rec.slot_number);    /* SlotNumber    */
                OHA_PDU_SET_U16 (pRecordData, rec->rec.subslot_number); /* SubslotNumber */
                OHA_PDU_SET_U32 (pRecordData, rec->rec.record_data_length); /* the record data length */
                OHA_PDU_SET_U16 (pRecordData, 0);			/* AP01092248: comp_id removed */
                OHA_PDU_SET_U16 (pRecordData, 0);                       /* reserved */

            }
            /* RemaRecordData */
            block_length += rec->rec.record_data_length;
            if (OHA_IS_NOT_NULL(pData))
            {
                if (block_length > *pDataLen) goto RecLenFailed;
                OHA_PDU_SET_MEM (pRecordData, rec->rec.record_data, rec->rec.record_data_length);
            }
            /* padding byte(s) after RemaRecordData */
            while ( (block_length & 3) != 0 )
            {
                block_length += 1;
                if (OHA_IS_NOT_NULL(pData))
                {
                    if (block_length > *pDataLen) goto RecLenFailed;
                    OHA_PDU_SET_U8 (pRecordData, 0x0);      /* padding byte    */
                }
            }

            if (block_length > 0xFFFF)
            {
                OHA_PROGRAM_TRACE_02 (OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR
                                      , "too much records in Set_A or too much data, list_cnt(%u) block_length(%u)"
                                      , list_cnt, block_length);
                RetVal = OHA_ERR_RESOURCE;
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetMultiplePDEVData(RetVal: 0x%X)", RetVal);
                return(RetVal);
            }
        }
    }

    if (OHA_IS_NOT_NULL(pData))
    {
        if (block_length > *pDataLen)
        {
RecLenFailed:
            RetVal = OHA_ERR_BUFFER_OVERFLOW;

            OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE
                                  , "OUT: OHA_GetMultiplePDEVData(RetVal: 0x%X)  got DataLen(%u) but need block_length(%u)"
                                  , RetVal, *pDataLen, block_length
                                 );

            /* response the needed buffer length */
            *pDataLen = block_length;

            return (RetVal);
        }
    }

    /* finally write the length ! */
    *pDataLen = block_length;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_GetMultiplePDEVData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_RequestRemaRead                         +*/
/*+  Input/Output          :    OHA_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+                        :    OHA_HDB_PTR_TYPE           pHDB             +*/
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
/*+     LSA_OPCODE_TYPE        opcode:      OHA_OPC_REMA_READ               +*/
/*+     LSA_HANDLE_TYPE        handle:                                      +*/
/*+     LSA_USER_ID_TYPE       user_id:                                     +*/
/*+     LSA_UINT16             Response                                     +*/
/*+     OHA_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL +*/
/*+     OHA_RQB_ARGS_TYPE      args.DcpRema                                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to read REMA data                                 +*/
/*+                                                                         +*/
/*+               Queues the RQB wthin User-Handlemanagement                +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  OHA_LOCAL_FCT_ATTR OHA_RequestRemaRead(
    OHA_UPPER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_USER_PTR_TYPE    pHDBUser)
{
    LSA_UINT16              Response;
    LSA_UINT16              NICId;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_RequestRemaRead(pRQB: 0x%X, pHDBUser: 0x%X)",
                          pRQB, pHDBUser);

    OHA_ASSERT_NULL_PTR(pRQB);
    OHA_ASSERT_NULL_PTR(pHDBUser);

	NICId = pHDBUser->Params.pDetail->Params.User.NICId;

	if (!OHA_EDDCheckNICUsed(NICId))
    {
        /* Error: no such interface. Abort. */
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_OPC_REMA_READ failed. No valid NicID for this interface (0x%X)",NICId);

		OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "OUT: OHA_RequestRemaRead()");
		OHA_UserRequestFinish(pHDBUser,pRQB,OHA_ERR_RESOURCE);
        return;
    }

    if ((LSA_HOST_PTR_ARE_EQUAL(pRQB->args.RemaData.data, LSA_NULL)) && (pRQB->args.RemaData.data_length))
    {
        OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN, "ParamError, no buffer: OHA_RequestRemaRead()");
        OHA_UserRequestFinish(pHDBUser,pRQB,OHA_ERR_PARAM);
        return;
    }

    switch (pRQB->args.RemaData.index)
    {
        case OHA_RECORD_INDEX_MULTIPLE_PDEV:
            pRQB->args.RemaData.data_tag = OHA_PDEV_DATA;   /* pdev data */

            /* build a multiple write record for the REMA user(s) */
            Response = OHA_GetMultiplePDEVData(NICId,
                                                 pRQB->args.RemaData.data,
                                                 &pRQB->args.RemaData.data_length);
            break;

        case OHA_RECORD_INDEX_MULTIPLE_STATION:
            pRQB->args.RemaData.data_tag = OHA_STATION_DATA;    /* station data */
            Response = OHA_GetMultipleStationData(NICId,
                                                    pRQB->args.RemaData.data,
                                                    &pRQB->args.RemaData.data_length,
                                                    LSA_TRUE);
            break;

        case OHA_RECORD_INDEX_NAME_OF_STATION:
            pRQB->args.RemaData.data_tag = OHA_STATION_DATA;    /* NameOfStation data */
            Response = OHA_GetNameOfStationData(NICId,
                                                pRQB->args.RemaData.data,
                                                &pRQB->args.RemaData.data_length,
                                                LSA_TRUE);
            break;

        case OHA_RECORD_INDEX_IP_SUITE:
            pRQB->args.RemaData.data_tag = OHA_STATION_DATA;    /* NameOfStation data */
            Response = OHA_GetIPSuiteData(NICId,
                                          pRQB->args.RemaData.data,
                                          &pRQB->args.RemaData.data_length,
                                          LSA_TRUE);
            break;

        case OHA_RECORD_INDEX_DHCP_PARAMS:
            pRQB->args.RemaData.data_tag = OHA_STATION_DATA;    /* DHCPParameters data */
            Response = OHA_GetDHCPData(NICId,
                                       pRQB->args.RemaData.data,
                                       &pRQB->args.RemaData.data_length,
                                       LSA_TRUE);
            break;

        case OHA_RECORD_INDEX_MULTIPLE_SNMP:
            pRQB->args.RemaData.data_tag = OHA_SNMP_DATA;    /* SNMP data */
            Response = OHA_GetMultipleSNMPData(NICId,
                                                 pRQB->args.RemaData.data,
                                                 &pRQB->args.RemaData.data_length,
                                                 LSA_TRUE);
            break;

        case OHA_RECORD_INDEX_MULTIPLE_BUS:
            pRQB->args.RemaData.data_tag = OHA_BUS_DATA;    /* BUS data */
            Response = OHA_GetMultipleBusData(NICId,
                                              pRQB->args.RemaData.data,
                                              &pRQB->args.RemaData.data_length,
                                              LSA_TRUE);
            break;

        default:
            Response = OHA_ERR_PARAM;
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN, "ParamError, invalid index (0x%X)", pRQB->args.RemaData.index);
            break;
    }

    OHA_UserRequestFinish(pHDBUser,pRQB,Response);

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_RequestRemaRead(Response: 0x%X)", Response);
}


/*----------------------------------------------------------------------------*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateRemaPDEV(
    LSA_UINT16 NICId, 
    OHA_HDB_USER_PTR_TYPE pHDBUser)
{
    OHA_UPPER_RQB_PTR_TYPE pRQBBottom;
    LSA_HANDLE_TYPE        Handle;
    OHA_HDB_REMA_PTR_TYPE  pHDBRema = OHA_REMAGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IndicateRemaPDEV(NICId: 0x%X, pHDBUser: 0x%X)",
                          NICId, pHDBUser);

    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBRema);

	OHA_ASSERT(pHDBRema->this_idx < OHA_CFG_MAX_NICS);
    pRQBBottom = pHDBUser->RemaInd[pHDBRema->this_idx].pBottom;
    Handle     = (LSA_HANDLE_TYPE)OHA_HANDLE_IDX(pHDBUser->ThisHandle);

    OHA_ASSERT(Handle < OHA_CFG_MAX_CHANNELS);

    if (OHA_IS_NOT_NULL(pRQBBottom))
    {
        /* indication for this interface ? */
        if (NICId == pHDBUser->Params.pDetail->Params.User.NICId)
        {
            OHA_UPPER_RQB_PTR_TYPE pRQB;

            /* are the rema indications locked? */
            if (!pHDBRema->Ind_Locked[Handle])
            {
                LSA_UINT16 Response;

                /* get the ind rqb */
                OHA_RQB_REM_BLOCK_BOTTOM(
                    pHDBUser->RemaInd[pHDBRema->this_idx].pBottom,
                    pHDBUser->RemaInd[pHDBRema->this_idx].pTop,
                    pRQB);

                if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, pRQBBottom))
                {
                    OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Bottom RQB is not the removed one (pRQBBottom: 0x%X, pRQB: 0x%X) - why?!", pRQBBottom, pRQB);
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }

                /* build a multiple PDEV record for the REMA user(s) */
                pRQB->args.RemaData.index = OHA_RECORD_INDEX_MULTIPLE_PDEV;
                pRQB->args.RemaData.data_tag = OHA_PDEV_DATA;   /* pdev data */

                Response = OHA_GetMultiplePDEVData(NICId,
                                                     pRQB->args.RemaData.data,
                                                     &pRQB->args.RemaData.data_length);

                pHDBRema->PDEV_Ind_Done[Handle] = LSA_TRUE;

                /* indicate the user */
                OHA_UserRequestFinish(pHDBUser,pRQB,Response);
            }
            else
            {
                pHDBRema->PDEV_Ind_Done[Handle] = LSA_FALSE;    /* the user is not indicated */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"REMA-Indications are locked (pHDBUser: 0x%X)", pHDBUser);
            }
        }
        else
        {
            pHDBRema->PDEV_Ind_Done[Handle] = LSA_FALSE;    /* the user is not indicated */
        }
    }
    else
    {
        pHDBRema->PDEV_Ind_Done[Handle] = LSA_FALSE;    /* the user is not indicated */
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IndicateRemaPDEV()");
    return;
}


/*----------------------------------------------------------------------------*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateRemaStation(
    LSA_UINT16 NICId, 
    OHA_HDB_USER_PTR_TYPE pHDBUser)
{
    OHA_UPPER_RQB_PTR_TYPE pRQBBottom;
    LSA_HANDLE_TYPE        Handle;
    OHA_HDB_REMA_PTR_TYPE  pHDBRema = OHA_REMAGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IndicateRemaStation(NICId: 0x%X, pHDBUser: 0x%X)",
                          NICId, pHDBUser);

    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBRema);

	OHA_ASSERT(pHDBRema->this_idx < OHA_CFG_MAX_NICS);
    pRQBBottom = pHDBUser->RemaInd[pHDBRema->this_idx].pBottom;
    Handle     = (LSA_HANDLE_TYPE)OHA_HANDLE_IDX(pHDBUser->ThisHandle);

    OHA_ASSERT(Handle < OHA_CFG_MAX_CHANNELS);

    if (OHA_IS_NOT_NULL(pRQBBottom))
    {
        /* indication for this interface ? */
        if (NICId == pHDBUser->Params.pDetail->Params.User.NICId)
        {
            OHA_UPPER_RQB_PTR_TYPE pRQB;

            /* are the rema indications locked? */
            if (!pHDBRema->Ind_Locked[Handle])
            {
                LSA_UINT16 Response;

                /* get the ind rqb */
                OHA_RQB_REM_BLOCK_BOTTOM(
                    pHDBUser->RemaInd[pHDBRema->this_idx].pBottom,
                    pHDBUser->RemaInd[pHDBRema->this_idx].pTop,
                    pRQB);

                if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, pRQBBottom))
                {
                    OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Bottom RQB is not the removed one (pRQBBottom: 0x%X, pRQB: 0x%X) - why?!", pRQBBottom, pRQB);
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }

                /* build a multiple station record for the REMA user(s) */
                pRQB->args.RemaData.index = OHA_RECORD_INDEX_MULTIPLE_STATION;
                pRQB->args.RemaData.data_tag = OHA_STATION_DATA;    /* station data */

                Response = OHA_GetMultipleStationData(NICId,
                                                        pRQB->args.RemaData.data,
                                                        &pRQB->args.RemaData.data_length,
                                                        LSA_TRUE);

                pHDBRema->Station_Ind_Done[Handle] = LSA_TRUE;

                /* indicate the user */
                OHA_UserRequestFinish(pHDBUser,pRQB,Response);
            }
            else
            {
                pHDBRema->Station_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"REMA-Indications are locked (pHDBUser: 0x%X)", pHDBUser);
            }
        }
        else
        {
            pHDBRema->Station_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
        }
    }
    else
    {
        pHDBRema->Station_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IndicateRemaStation()");
    return;
}

/*----------------------------------------------------------------------------*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateRemaNameOfStation(
    LSA_UINT16 NICId, 
    OHA_HDB_USER_PTR_TYPE pHDBUser)
{
    OHA_UPPER_RQB_PTR_TYPE pRQBBottom;
    LSA_HANDLE_TYPE        Handle;
    OHA_HDB_REMA_PTR_TYPE  pHDBRema = OHA_REMAGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IndicateRemaNameOfStation(NICId: 0x%X, pHDBUser: 0x%X)",
                          NICId, pHDBUser);

    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBRema);

	OHA_ASSERT(pHDBRema->this_idx < OHA_CFG_MAX_NICS);
    pRQBBottom = pHDBUser->RemaInd[pHDBRema->this_idx].pBottom;
    Handle     = (LSA_HANDLE_TYPE)OHA_HANDLE_IDX(pHDBUser->ThisHandle);

    OHA_ASSERT(Handle < OHA_CFG_MAX_CHANNELS);

    if (OHA_IS_NOT_NULL(pRQBBottom))
    {
        /* indication for this interface ? */
        if (NICId == pHDBUser->Params.pDetail->Params.User.NICId)
        {
            OHA_UPPER_RQB_PTR_TYPE pRQB;

            /* are the rema indications locked? */
            if (!pHDBRema->Ind_Locked[Handle])
            {
                LSA_UINT16 Response;

                /* get the ind rqb */
                OHA_RQB_REM_BLOCK_BOTTOM(
                    pHDBUser->RemaInd[pHDBRema->this_idx].pBottom,
                    pHDBUser->RemaInd[pHDBRema->this_idx].pTop,
                    pRQB);

                if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, pRQBBottom))
                {
                    OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Bottom RQB is not the removed one (pRQBBottom: 0x%X, pRQB: 0x%X) - why?!", pRQBBottom, pRQB);
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }

                /* build a multiple station record for the REMA user(s) */
                pRQB->args.RemaData.index = OHA_RECORD_INDEX_NAME_OF_STATION;
                pRQB->args.RemaData.data_tag = OHA_STATION_DATA;    /* station data */

                Response = OHA_GetNameOfStationData(NICId,
                                                    pRQB->args.RemaData.data,
                                                    &pRQB->args.RemaData.data_length,
                                                    LSA_TRUE);

                pHDBRema->NameOfStation_Ind_Done[Handle] = LSA_TRUE;

                /* indicate the user */
                OHA_UserRequestFinish(pHDBUser,pRQB,Response);
            }
            else
            {
                pHDBRema->NameOfStation_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"REMA-Indications are locked (pHDBUser: 0x%X)", pHDBUser);
            }
        }
        else
        {
            pHDBRema->NameOfStation_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
        }
    }
    else
    {
        pHDBRema->NameOfStation_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IndicateRemaNameOfStation()");
    return;
}

/*----------------------------------------------------------------------------*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateRemaIPSuite(
    LSA_UINT16 NICId, 
    OHA_HDB_USER_PTR_TYPE pHDBUser)
{
    OHA_UPPER_RQB_PTR_TYPE pRQBBottom;
    LSA_HANDLE_TYPE        Handle;
    OHA_HDB_REMA_PTR_TYPE  pHDBRema = OHA_REMAGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IndicateRemaIPSuite(NICId: 0x%X, pHDBUser: 0x%X)",
                          NICId, pHDBUser);

    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBRema);

	OHA_ASSERT(pHDBRema->this_idx < OHA_CFG_MAX_NICS);
    pRQBBottom = pHDBUser->RemaInd[pHDBRema->this_idx].pBottom;
    Handle     = (LSA_HANDLE_TYPE)OHA_HANDLE_IDX(pHDBUser->ThisHandle);

    OHA_ASSERT(Handle < OHA_CFG_MAX_CHANNELS);

    if (OHA_IS_NOT_NULL(pRQBBottom))
    {
        /* indication for this interface ? */
        if (NICId == pHDBUser->Params.pDetail->Params.User.NICId)
        {
            OHA_UPPER_RQB_PTR_TYPE pRQB;

            /* are the rema indications locked? */
            if (!pHDBRema->Ind_Locked[Handle])
            {
                LSA_UINT16 Response;

                /* get the ind rqb */
                OHA_RQB_REM_BLOCK_BOTTOM(
                    pHDBUser->RemaInd[pHDBRema->this_idx].pBottom,
                    pHDBUser->RemaInd[pHDBRema->this_idx].pTop,
                    pRQB);

                if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, pRQBBottom))
                {
                    OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Bottom RQB is not the removed one (pRQBBottom: 0x%X, pRQB: 0x%X) - why?!", pRQBBottom, pRQB);
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }

                /* build a multiple station record for the REMA user(s) */
                pRQB->args.RemaData.index = OHA_RECORD_INDEX_IP_SUITE;
                pRQB->args.RemaData.data_tag = OHA_STATION_DATA;    /* station data */

                Response = OHA_GetIPSuiteData(NICId,
                                              pRQB->args.RemaData.data,
                                              &pRQB->args.RemaData.data_length,
                                              LSA_TRUE);

                pHDBRema->IPSuite_Ind_Done[Handle] = LSA_TRUE;

                /* indicate the user */
                OHA_UserRequestFinish(pHDBUser,pRQB,Response);
            }
            else
            {
                pHDBRema->IPSuite_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"REMA-Indications are locked (pHDBUser: 0x%X)", pHDBUser);
            }
        }
        else
        {
            pHDBRema->IPSuite_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
        }
    }
    else
    {
        pHDBRema->IPSuite_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IndicateRemaIPSuite()");
    return;
}

/*----------------------------------------------------------------------------*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateRemaDHCP(
    LSA_UINT16 NICId, 
    OHA_HDB_USER_PTR_TYPE pHDBUser)
{
    OHA_UPPER_RQB_PTR_TYPE pRQBBottom;
    LSA_HANDLE_TYPE        Handle;
    OHA_HDB_REMA_PTR_TYPE  pHDBRema = OHA_REMAGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IndicateRemaDHCP(NICId: 0x%X, pHDBUser: 0x%X)",
                          NICId, pHDBUser);

    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBRema);

	OHA_ASSERT(pHDBRema->this_idx < OHA_CFG_MAX_NICS);
    pRQBBottom = pHDBUser->RemaInd[pHDBRema->this_idx].pBottom;
    Handle     = (LSA_HANDLE_TYPE)OHA_HANDLE_IDX(pHDBUser->ThisHandle);

    OHA_ASSERT(Handle < OHA_CFG_MAX_CHANNELS);

    if (OHA_IS_NOT_NULL(pRQBBottom))
    {
        /* indication for this interface ? */
        if (NICId == pHDBUser->Params.pDetail->Params.User.NICId)
        {
            OHA_UPPER_RQB_PTR_TYPE pRQB;

            /* are the rema indications locked? */
            if (!pHDBRema->Ind_Locked[Handle])
            {
                LSA_UINT16 Response;

                /* get the ind rqb */
                OHA_RQB_REM_BLOCK_BOTTOM(
                    pHDBUser->RemaInd[pHDBRema->this_idx].pBottom,
                    pHDBUser->RemaInd[pHDBRema->this_idx].pTop,
                    pRQB);

                if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, pRQBBottom))
                {
                    OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Bottom RQB is not the removed one (pRQBBottom: 0x%X, pRQB: 0x%X) - why?!", pRQBBottom, pRQB);
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }

                /* build a DHCPData record for the REMA user(s) */
                pRQB->args.RemaData.index = OHA_RECORD_INDEX_DHCP_PARAMS;
                pRQB->args.RemaData.data_tag = OHA_STATION_DATA;    /* station data */

                Response = OHA_GetDHCPData(NICId,
                                           pRQB->args.RemaData.data,
                                           &pRQB->args.RemaData.data_length,
                                           LSA_TRUE);

                pHDBRema->DHCPData_Ind_Done[Handle] = LSA_TRUE;

                /* indicate the user */
                OHA_UserRequestFinish(pHDBUser,pRQB,Response);
            }
            else
            {
                pHDBRema->DHCPData_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"REMA-Indications are locked (pHDBUser: 0x%X)", pHDBUser);
            }
        }
        else
        {
            pHDBRema->DHCPData_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
        }
    }
    else
    {
        pHDBRema->DHCPData_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IndicateRemaDHCP()");
    return;
}

/*----------------------------------------------------------------------------*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateRemaSnmp(
    LSA_UINT16 NICId, 
    OHA_HDB_USER_PTR_TYPE pHDBUser)
{
    OHA_UPPER_RQB_PTR_TYPE pRQBBottom;
    LSA_HANDLE_TYPE        Handle;
    OHA_HDB_REMA_PTR_TYPE  pHDBRema = OHA_REMAGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IndicateRemaSnmp(NICId: 0x%X, pHDBUser: 0x%X)",
                          NICId, pHDBUser);

    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBRema);

	OHA_ASSERT(pHDBRema->this_idx < OHA_CFG_MAX_NICS);
    pRQBBottom = pHDBUser->RemaInd[pHDBRema->this_idx].pBottom;
    Handle     = (LSA_HANDLE_TYPE)OHA_HANDLE_IDX(pHDBUser->ThisHandle);

    OHA_ASSERT(Handle < OHA_CFG_MAX_CHANNELS);

    if (OHA_IS_NOT_NULL(pRQBBottom))
    {
        /* indication for this interface ? */
        if (NICId == pHDBUser->Params.pDetail->Params.User.NICId)
        {
            OHA_UPPER_RQB_PTR_TYPE pRQB;

            /* are the rema indications locked? */
            if (!pHDBRema->Ind_Locked[Handle])
            {
                LSA_UINT16 Response;

                /* get the ind rqb */
                OHA_RQB_REM_BLOCK_BOTTOM(
                    pHDBUser->RemaInd[pHDBRema->this_idx].pBottom,
                    pHDBUser->RemaInd[pHDBRema->this_idx].pTop,
                    pRQB);

                if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, pRQBBottom))
                {
                    OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Bottom RQB is not the removed one (pRQBBottom: 0x%X, pRQB: 0x%X) - why?!", pRQBBottom, pRQB);
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }

                /* build a multiple SNMP record for the REMA user(s) */
                pRQB->args.RemaData.index = OHA_RECORD_INDEX_MULTIPLE_SNMP;
                pRQB->args.RemaData.data_tag = OHA_SNMP_DATA;    /* SNMP data */

                Response = OHA_GetMultipleSNMPData(NICId,
                                                     pRQB->args.RemaData.data,
                                                     &pRQB->args.RemaData.data_length,
                                                     LSA_TRUE);

                pHDBRema->Snmp_Ind_Done[Handle] = LSA_TRUE;

                /* indicate the user */
                OHA_UserRequestFinish(pHDBUser,pRQB,Response);
            }
            else
            {
                pHDBRema->Snmp_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"REMA-Indications are locked (pHDBUser: 0x%X)", pHDBUser);
            }
        }
        else
        {
            pHDBRema->Snmp_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
        }
    }
    else
    {
        pHDBRema->Snmp_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IndicateRemaSnmp()");
    return;
}

/*----------------------------------------------------------------------------*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateRemaBus(
    LSA_UINT16 NICId, 
    OHA_HDB_USER_PTR_TYPE pHDBUser)
{
    OHA_UPPER_RQB_PTR_TYPE pRQBBottom;
    LSA_HANDLE_TYPE        Handle;
    OHA_HDB_REMA_PTR_TYPE  pHDBRema = OHA_REMAGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IndicateRemaBus(NICId: 0x%X, pHDBUser: 0x%X)",
                          NICId, pHDBUser);

    OHA_ASSERT_NULL_PTR(pHDBUser);
    OHA_ASSERT_NULL_PTR(pHDBRema);

	OHA_ASSERT(pHDBRema->this_idx < OHA_CFG_MAX_NICS);
    pRQBBottom = pHDBUser->RemaInd[pHDBRema->this_idx].pBottom;
    Handle     = (LSA_HANDLE_TYPE)OHA_HANDLE_IDX(pHDBUser->ThisHandle);

    OHA_ASSERT(Handle < OHA_CFG_MAX_CHANNELS);

    if (OHA_IS_NOT_NULL(pRQBBottom))
    {
        /* indication for this interface ? */
        if (NICId == pHDBUser->Params.pDetail->Params.User.NICId)
        {
            OHA_UPPER_RQB_PTR_TYPE pRQB;

            /* are the rema indications locked? */
            if (!pHDBRema->Ind_Locked[Handle])
            {
                LSA_UINT16 Response;

                /* get the ind rqb */
                OHA_RQB_REM_BLOCK_BOTTOM(
                    pHDBUser->RemaInd[pHDBRema->this_idx].pBottom,
                    pHDBUser->RemaInd[pHDBRema->this_idx].pTop,
                    pRQB);

                if (!LSA_HOST_PTR_ARE_EQUAL(pRQB, pRQBBottom))
                {
                    OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Bottom RQB is not the removed one (pRQBBottom: 0x%X, pRQB: 0x%X) - why?!", pRQBBottom, pRQB);
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }

                /* build a multiple BUS record for the REMA user(s) */
                pRQB->args.RemaData.index = OHA_RECORD_INDEX_MULTIPLE_BUS;
                pRQB->args.RemaData.data_tag = OHA_BUS_DATA;

                Response = OHA_GetMultipleBusData(NICId,
                                                  pRQB->args.RemaData.data,
                                                  &pRQB->args.RemaData.data_length,
                                                  LSA_TRUE);

                pHDBRema->Bus_Ind_Done[Handle] = LSA_TRUE;

                /* indicate the user */
                OHA_UserRequestFinish(pHDBUser,pRQB,Response);
            }
            else
            {
                pHDBRema->Bus_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
                OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"REMA-Indications are locked (pHDBUser: 0x%X)", pHDBUser);
            }
        }
        else
        {
            pHDBRema->Bus_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
        }
    }
    else
    {
        pHDBRema->Bus_Ind_Done[Handle] = LSA_FALSE; /* the user is not indicated */
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IndicateRemaBus()");
    return;
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_UpdatePDEVData(
    LSA_UINT16 NICId,
    LSA_BOOL OHA_COMMON_MEM_ATTR *pDataChanged)
{
    OHA_HDB_REMA_PTR_TYPE pHDBRema = OHA_REMAGetHandleByNIC(NICId);
    LSA_UINT16 RetVal;

    OHA_UPPER_REMA_DATA_PTR_TYPE pPDEV_Last_Ind;
    LSA_BOOL   DataChanged = LSA_FALSE;
    LSA_UINT32 DataLength  = 0;
    LSA_UINT16 DataTag     = OHA_PDEV_DATA;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_UpdatePDEVData(NICId: 0x%X, pDataChanged: 0x%X)",
                          NICId, pDataChanged);

    OHA_ASSERT_NULL_PTR(pHDBRema);
    OHA_ASSERT_NULL_PTR(pDataChanged);

    pPDEV_Last_Ind = &pHDBRema->PDEV_Last_Ind;

    *pDataChanged = LSA_FALSE;

    /* get length of the multiple write record */
    RetVal = OHA_GetMultiplePDEVData(NICId,
                                       LSA_NULL,
                                       &DataLength);

    if (RetVal != OHA_OK)
    {
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Get length of the Multiple PDEV Record failed (NICId: 0x%X, RetVal: 0x%X) !", NICId, RetVal);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdatePDEVData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* compare the multiple record data from the last indication */
    if (pPDEV_Last_Ind->data_length  != DataLength
        /* || pPDEV_Last_Ind->data_tag != DataTag */)
    {
        DataChanged = LSA_TRUE;
    }

    if (pPDEV_Last_Ind->data_length  != 0 && DataLength != 0)
    {
        if (!DataChanged)   /* compare the rema data */
        {
            OHA_UPPER_MEM_U8_PTR_TYPE pData = LSA_NULL;

            if (DataLength)
            {
                LSA_UINT16 rc;

                OHA_ALLOC_LOCAL_MEM ((OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR *)(&pData),
                                     (LSA_UINT16)DataLength);
                if (OHA_IS_NULL (pData))
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Alloc for the indication Multiple PDEV Record data failed !");
                    return OHA_ERR_RESOURCE;
                }

                /* get the multiple write record data and compare */
                rc = OHA_GetMultiplePDEVData(NICId,
                                               pData,
                                               &DataLength);

                if (rc != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"Temporary write of Multiple PDEV Record data failed (rc: 0x%X) !", rc);
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }

                OHA_ASSERT_NULL_PTR(pPDEV_Last_Ind->data);
                if (!OHA_MEMCMP(pPDEV_Last_Ind->data, pData, DataLength))
                {
                    DataChanged = LSA_TRUE;
                }

                OHA_FREE_LOCAL_MEM (&rc, pData);
                OHA_ASSERT (rc == LSA_RET_OK);
            }
        }
    }

    if (!DataChanged)
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"Multiple PDEV Record data are the same (NICId: 0x%X)", NICId);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdatePDEVData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* store the last indicated data */
    if (DataChanged)
    {
        LSA_UINT16 rc;
        if (OHA_IS_NOT_NULL(pPDEV_Last_Ind->data))
        {
            OHA_FREE_LOCAL_MEM (&rc, pPDEV_Last_Ind->data);
            pPDEV_Last_Ind->data = LSA_NULL;
            OHA_ASSERT (rc == LSA_RET_OK);
        }
        if (DataLength != 0)
        {
            OHA_ALLOC_LOCAL_MEM ((OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR *)(&pPDEV_Last_Ind->data),
                                 (LSA_UINT16)DataLength);
            if (OHA_IS_NULL (pPDEV_Last_Ind->data))
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Alloc for the last indicated Multiple PDEV Record data failed !");
                return OHA_ERR_RESOURCE;
            }
        }

        pPDEV_Last_Ind->data_length  = DataLength;
        pPDEV_Last_Ind->data_tag     = DataTag;

        rc = OHA_GetMultiplePDEVData(NICId,
                                       pPDEV_Last_Ind->data,
                                       &pPDEV_Last_Ind->data_length);
        if (rc != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Storage of the last indicated Multiple PDEV Record data failed (rc: 0x%X) !", rc);
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
        }
    }

    *pDataChanged = DataChanged;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdatePDEVData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_UpdateStationData(
    LSA_UINT16 NICId,
    LSA_BOOL OHA_COMMON_MEM_ATTR *pDataChanged)
{
    OHA_HDB_REMA_PTR_TYPE pHDBRema = OHA_REMAGetHandleByNIC(NICId);
    LSA_UINT16 RetVal;

    OHA_UPPER_REMA_DATA_PTR_TYPE pStation_Last_Ind;
    LSA_BOOL   DataChanged = LSA_FALSE;
    LSA_UINT32 DataLength  = 0;
    LSA_UINT16 DataTag     = OHA_STATION_DATA;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_UpdateStationData(NICId: 0x%X, pDataChanged: 0x%X)",
                          NICId, pDataChanged);

    OHA_ASSERT_NULL_PTR(pHDBRema);
    OHA_ASSERT_NULL_PTR(pDataChanged);

    pStation_Last_Ind = &pHDBRema->Station_Last_Ind;

    *pDataChanged = LSA_FALSE;

    /* get length of the multiple write record */
    RetVal = OHA_GetMultipleStationData(NICId,
                                          LSA_NULL,
                                          &DataLength,
                                          LSA_TRUE);

    if (RetVal != OHA_OK)
    {
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Get length of the Multiple Station Record failed (NICId: 0x%X, RetVal: 0x%X) !", NICId, RetVal);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateStationData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* compare the multiple record data from the last indication */
    if (pStation_Last_Ind->data_length != DataLength)
    {
        DataChanged = LSA_TRUE;
    }

    if (pStation_Last_Ind->data_length  != 0 && DataLength != 0)
    {
        if (!DataChanged)   /* compare the rema data */
        {
            OHA_UPPER_MEM_U8_PTR_TYPE pData = LSA_NULL;

            if (DataLength)
            {
                LSA_UINT16 rc;

                OHA_ALLOC_LOCAL_MEM ((OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR *)(&pData),
                                     (LSA_UINT16)DataLength);
                if (OHA_IS_NULL (pData))
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Alloc for the indication Multiple Station Record data failed !");
                    return OHA_ERR_RESOURCE;
                }

                /* get the multiple write record data and compare */
                rc = OHA_GetMultipleStationData(NICId,
                                                  pData,
                                                  &DataLength,
                                                  LSA_TRUE);

                if (rc != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Temporary write of Multiple Station Record data failed (rc: 0x%X) !", rc);
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }

                OHA_ASSERT_NULL_PTR(pStation_Last_Ind->data);
                if (!OHA_MEMCMP(pStation_Last_Ind->data, pData, DataLength))
                {
                    DataChanged = LSA_TRUE;
                }

                OHA_FREE_LOCAL_MEM (&rc, pData);
                OHA_ASSERT (rc == LSA_RET_OK);
            }
        }
    }

    if (!DataChanged)
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"Multiple Station Record data are the same (NICId: 0x%X)", NICId);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateStationData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* store the last indicated data */
    if (DataChanged)
    {
        LSA_UINT16 rc;

        if (OHA_IS_NOT_NULL(pStation_Last_Ind->data))
        {
            OHA_FREE_LOCAL_MEM (&rc, pStation_Last_Ind->data);
            pStation_Last_Ind->data = LSA_NULL;
            OHA_ASSERT (rc == LSA_RET_OK);
        }
        if (DataLength != 0)
        {
            OHA_ALLOC_LOCAL_MEM ((OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR *)(&pStation_Last_Ind->data),
                                 (LSA_UINT16)DataLength);
            if (OHA_IS_NULL (pStation_Last_Ind->data))
            {
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"Alloc for the last indicated Multiple Station Record data failed !");
                return OHA_ERR_RESOURCE;
            }
        }

        pStation_Last_Ind->data_length  = DataLength;
        pStation_Last_Ind->data_tag     = DataTag;

        rc = OHA_GetMultipleStationData(NICId,
                                          pStation_Last_Ind->data,
                                          &pStation_Last_Ind->data_length,
                                          LSA_TRUE);
        if (rc != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Storage of the last indicated Multiple Station Record data failed (rc: 0x%X) !", rc);
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
        }
    }

    *pDataChanged = DataChanged;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateStationData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}


/*----------------------------------------------------------------------------*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_UpdateNameOfStationData(
    LSA_UINT16 NICId,
    LSA_BOOL OHA_COMMON_MEM_ATTR *pDataChanged)
{
    OHA_HDB_REMA_PTR_TYPE pHDBRema = OHA_REMAGetHandleByNIC(NICId);
    LSA_UINT16 RetVal;

    OHA_UPPER_REMA_DATA_PTR_TYPE pNameOfStation_Last_Ind;
    LSA_BOOL   DataChanged = LSA_FALSE;
    LSA_UINT32 DataLength  = 0;
    LSA_UINT16 DataTag     = OHA_STATION_DATA;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_UpdateNameOfStationData(NICId: 0x%X, pDataChanged: 0x%X)",
                          NICId, pDataChanged);

    OHA_ASSERT_NULL_PTR(pHDBRema);
    OHA_ASSERT_NULL_PTR(pDataChanged);

    pNameOfStation_Last_Ind = &pHDBRema->NameOfStation_Last_Ind;

    *pDataChanged = LSA_FALSE;

    /* get length of the multiple write record */
    RetVal = OHA_GetNameOfStationData(NICId,
                                      LSA_NULL,
                                      &DataLength,
                                      LSA_TRUE);

    if (RetVal != OHA_OK)
    {
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Get length of the NameOfStationRecord failed (NICId: 0x%X, RetVal: 0x%X) !", NICId, RetVal);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateNameOfStationData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* compare the multiple record data from the last indication */
    if (pNameOfStation_Last_Ind->data_length != DataLength)
    {
        DataChanged = LSA_TRUE;
    }

    if (pNameOfStation_Last_Ind->data_length  != 0 && DataLength != 0)
    {
        if (!DataChanged)   /* compare the rema data */
        {
            OHA_UPPER_MEM_U8_PTR_TYPE pData = LSA_NULL;

            if (DataLength)
            {
                LSA_UINT16 rc;

                OHA_ALLOC_LOCAL_MEM ((OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR *)(&pData),
                                     (LSA_UINT16)DataLength);
                if (OHA_IS_NULL (pData))
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Alloc for the indication NameOfStationRecord data failed !");
                    return OHA_ERR_RESOURCE;
                }

                /* get the NameOfStationRecord data and compare */
                rc = OHA_GetNameOfStationData(NICId,
                                              pData,
                                              &DataLength,
                                              LSA_TRUE);

                if (rc != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Temporary write of NameOfStationRecord data failed (rc: 0x%X) !", rc);
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }

                OHA_ASSERT_NULL_PTR(pNameOfStation_Last_Ind->data);
                if (!OHA_MEMCMP(pNameOfStation_Last_Ind->data, pData, DataLength))
                {
                    DataChanged = LSA_TRUE;
                }

                OHA_FREE_LOCAL_MEM (&rc, pData);
                OHA_ASSERT (rc == LSA_RET_OK);
            }
        }
    }

    if (!DataChanged)
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"NameOfStationRecord data are the same (NICId: 0x%X)", NICId);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateNameOfStationData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* store the last indicated data */
    if (DataChanged)
    {
        LSA_UINT16 rc;

        if (OHA_IS_NOT_NULL(pNameOfStation_Last_Ind->data))
        {
            OHA_FREE_LOCAL_MEM (&rc, pNameOfStation_Last_Ind->data);
            pNameOfStation_Last_Ind->data = LSA_NULL;
            OHA_ASSERT (rc == LSA_RET_OK);
        }
        if (DataLength != 0)
        {
            OHA_ALLOC_LOCAL_MEM ((OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR *)(&pNameOfStation_Last_Ind->data),
                                 (LSA_UINT16)DataLength);
            if (OHA_IS_NULL (pNameOfStation_Last_Ind->data))
            {
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"Alloc for the last indicated NameOfStationRecord data failed !");
                return OHA_ERR_RESOURCE;
            }
        }

        pNameOfStation_Last_Ind->data_length  = DataLength;
        pNameOfStation_Last_Ind->data_tag     = DataTag;

        rc = OHA_GetNameOfStationData(NICId,
                                      pNameOfStation_Last_Ind->data,
                                      &pNameOfStation_Last_Ind->data_length,
                                      LSA_TRUE);
        if (rc != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Storage of the last indicated NameOfStationRecord data failed (rc: 0x%X) !", rc);
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
        }
    }

    *pDataChanged = DataChanged;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateNameOfStationData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_UpdateIPSuiteData(
    LSA_UINT16 NICId,
    LSA_BOOL OHA_COMMON_MEM_ATTR *pDataChanged)
{
    OHA_HDB_REMA_PTR_TYPE pHDBRema = OHA_REMAGetHandleByNIC(NICId);
    LSA_UINT16 RetVal;

    OHA_UPPER_REMA_DATA_PTR_TYPE pIPSuite_Last_Ind;
    LSA_BOOL   DataChanged = LSA_FALSE;
    LSA_UINT32 DataLength  = 0;
    LSA_UINT16 DataTag     = OHA_STATION_DATA;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_UpdateIPSuiteData(NICId: 0x%X, pDataChanged: 0x%X)",
                          NICId, pDataChanged);

    OHA_ASSERT_NULL_PTR(pHDBRema);
    OHA_ASSERT_NULL_PTR(pDataChanged);

    pIPSuite_Last_Ind = &pHDBRema->IPSuite_Last_Ind;

    *pDataChanged = LSA_FALSE;

    /* get length of the multiple write record */
    RetVal = OHA_GetIPSuiteData(NICId,
                                LSA_NULL,
                                &DataLength,
                                LSA_TRUE);

    if (RetVal != OHA_OK)
    {
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Get length of the IPSuiteRecord failed (NICId: 0x%X, RetVal: 0x%X) !", NICId, RetVal);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateIPSuiteData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* compare the multiple record data from the last indication */
    if (pIPSuite_Last_Ind->data_length != DataLength)
    {
        DataChanged = LSA_TRUE;
    }

    if (pIPSuite_Last_Ind->data_length  != 0 && DataLength != 0)
    {
        if (!DataChanged)   /* compare the rema data */
        {
            OHA_UPPER_MEM_U8_PTR_TYPE pData = LSA_NULL;

            if (DataLength)
            {
                LSA_UINT16 rc;

                OHA_ALLOC_LOCAL_MEM ((OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR *)(&pData),
                                     (LSA_UINT16)DataLength);
                if (OHA_IS_NULL (pData))
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Alloc for the indication IPSuiteRecord data failed !");
                    return OHA_ERR_RESOURCE;
                }

                /* get the NameOfStationRecord data and compare */
                rc = OHA_GetIPSuiteData(NICId,
                                        pData,
                                        &DataLength,
                                        LSA_TRUE);

                if (rc != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Temporary write of IPSuiteRecord data failed (rc: 0x%X) !", rc);
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }

                OHA_ASSERT_NULL_PTR(pIPSuite_Last_Ind->data);
                if (!OHA_MEMCMP(pIPSuite_Last_Ind->data, pData, DataLength))
                {
                    DataChanged = LSA_TRUE;
                }

                OHA_FREE_LOCAL_MEM (&rc, pData);
                OHA_ASSERT (rc == LSA_RET_OK);
            }
        }
    }

    if (!DataChanged)
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"IPSuiteRecord data are the same (NICId: 0x%X)", NICId);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateIPSuiteData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* store the last indicated data */
    if (DataChanged)
    {
        LSA_UINT16 rc;

        if (OHA_IS_NOT_NULL(pIPSuite_Last_Ind->data))
        {
            OHA_FREE_LOCAL_MEM (&rc, pIPSuite_Last_Ind->data);
            pIPSuite_Last_Ind->data = LSA_NULL;
            OHA_ASSERT (rc == LSA_RET_OK);
        }
        if (DataLength != 0)
        {
            OHA_ALLOC_LOCAL_MEM ((OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR *)(&pIPSuite_Last_Ind->data),
                                 (LSA_UINT16)DataLength);
            if (OHA_IS_NULL (pIPSuite_Last_Ind->data))
            {
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"Alloc for the last indicated IPSuiteRecord data failed !");
                return OHA_ERR_RESOURCE;
            }
        }

        pIPSuite_Last_Ind->data_length  = DataLength;
        pIPSuite_Last_Ind->data_tag     = DataTag;

        rc = OHA_GetIPSuiteData(NICId,
                                pIPSuite_Last_Ind->data,
                                &pIPSuite_Last_Ind->data_length,
                                LSA_TRUE);
        if (rc != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Storage of the last indicated IPSuiteRecord data failed (rc: 0x%X) !", rc);
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
        }
    }

    *pDataChanged = DataChanged;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateIPSuiteData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_UpdateDHCPData(
    LSA_UINT16 NICId,
    LSA_BOOL OHA_COMMON_MEM_ATTR *pDataChanged)
{
    OHA_HDB_REMA_PTR_TYPE pHDBRema = OHA_REMAGetHandleByNIC(NICId);
    LSA_UINT16 RetVal;

    OHA_UPPER_REMA_DATA_PTR_TYPE pDHCPData_Last_Ind;
    LSA_BOOL   DataChanged = LSA_FALSE;
    LSA_UINT32 DataLength  = 0;
    LSA_UINT16 DataTag     = OHA_STATION_DATA;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_UpdateDHCPData(NICId: 0x%X, pDataChanged: 0x%X)",
                          NICId, pDataChanged);

    OHA_ASSERT_NULL_PTR(pHDBRema);
    OHA_ASSERT_NULL_PTR(pDataChanged);

    pDHCPData_Last_Ind = &pHDBRema->DHCPData_Last_Ind;

    *pDataChanged = LSA_FALSE;

    /* get length of the write record */
    RetVal = OHA_GetDHCPData(NICId,
                             LSA_NULL,
                             &DataLength,
                             LSA_TRUE);

    if (RetVal != OHA_OK)
    {
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Get length of the DHCPParametersRecord failed (NICId: 0x%X, RetVal: 0x%X) !", NICId, RetVal);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateDHCPData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* compare the record data from the last indication */
    if (pDHCPData_Last_Ind->data_length != DataLength)
    {
        DataChanged = LSA_TRUE;
    }

    if (pDHCPData_Last_Ind->data_length  != 0 && DataLength != 0)
    {
        if (!DataChanged)   /* compare the rema data */
        {
            OHA_UPPER_MEM_U8_PTR_TYPE pData = LSA_NULL;

            if (DataLength)
            {
                LSA_UINT16 rc;

                OHA_ALLOC_LOCAL_MEM ((OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR *)(&pData),
                                     (LSA_UINT16)DataLength);
                if (OHA_IS_NULL (pData))
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Alloc for the indication DHCPParametersRecord data failed !");
                    return OHA_ERR_RESOURCE;
                }

                /* get the data and compare */
                rc = OHA_GetDHCPData(NICId,
                                     pData,
                                     &DataLength,
                                     LSA_TRUE);

                if (rc != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Temporary write of DHCPParametersRecord data failed (rc: 0x%X) !", rc);
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }

                OHA_ASSERT_NULL_PTR(pDHCPData_Last_Ind->data);
                if (!OHA_MEMCMP(pDHCPData_Last_Ind->data, pData, DataLength))
                {
                    DataChanged = LSA_TRUE;
                }

                OHA_FREE_LOCAL_MEM (&rc, pData);
                OHA_ASSERT (rc == LSA_RET_OK);
            }
        }
    }

    if (!DataChanged)
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"DHCPParametersRecord data are the same (NICId: 0x%X)", NICId);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateDHCPData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* store the last indicated data */
    if (DataChanged)
    {
        LSA_UINT16 rc;

        if (OHA_IS_NOT_NULL(pDHCPData_Last_Ind->data))
        {
            OHA_FREE_LOCAL_MEM (&rc, pDHCPData_Last_Ind->data);
            pDHCPData_Last_Ind->data = LSA_NULL;
            OHA_ASSERT (rc == LSA_RET_OK);
        }
        if (DataLength != 0)
        {
            OHA_ALLOC_LOCAL_MEM ((OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR *)(&pDHCPData_Last_Ind->data),
                                 (LSA_UINT16)DataLength);
            if (OHA_IS_NULL (pDHCPData_Last_Ind->data))
            {
                OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,"Alloc for the last indicated DHCPParametersRecord data failed !");
                return OHA_ERR_RESOURCE;
            }
        }

        pDHCPData_Last_Ind->data_length  = DataLength;
        pDHCPData_Last_Ind->data_tag     = DataTag;

        rc = OHA_GetDHCPData(NICId,
                             pDHCPData_Last_Ind->data,
                             &pDHCPData_Last_Ind->data_length,
                             LSA_TRUE);
        if (rc != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Storage of the last indicated DHCPParametersRecord data failed (rc: 0x%X) !", rc);
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
        }
    }

    *pDataChanged = DataChanged;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateDHCPData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_UpdateSnmpData(
    LSA_UINT16 NICId,
    LSA_BOOL OHA_COMMON_MEM_ATTR *pDataChanged)
{
    OHA_HDB_REMA_PTR_TYPE pHDBRema = OHA_REMAGetHandleByNIC(NICId);
    LSA_UINT16 RetVal;

    OHA_UPPER_REMA_DATA_PTR_TYPE pSnmp_Last_Ind;
    LSA_BOOL   DataChanged = LSA_FALSE;
    LSA_UINT32 DataLength  = 0;
    LSA_UINT16 DataTag     = OHA_SNMP_DATA;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_UpdateSnmpData(NICId: 0x%X, pDataChanged: 0x%X)",
                          NICId, pDataChanged);

    OHA_ASSERT_NULL_PTR(pHDBRema);
    OHA_ASSERT_NULL_PTR(pDataChanged);

    pSnmp_Last_Ind = &pHDBRema->Snmp_Last_Ind;

    *pDataChanged = LSA_FALSE;

    /* get length of the multiple write record */
    RetVal = OHA_GetMultipleSNMPData(NICId,
                                       LSA_NULL,
                                       &DataLength,
                                       LSA_TRUE);

    if (RetVal != OHA_OK)
    {
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Get length of the Multiple SNMP Record failed (NICId: 0x%X, RetVal: 0x%X) !", NICId, RetVal);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateSnmpData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* compare the multiple record data from the last indication */
    if (pSnmp_Last_Ind->data_length != DataLength)
    {
        DataChanged = LSA_TRUE;
    }

    if (pSnmp_Last_Ind->data_length  != 0 && DataLength != 0)
    {
        if (!DataChanged)   /* compare the rema data */
        {
            OHA_UPPER_MEM_U8_PTR_TYPE pData = LSA_NULL;

            if (DataLength)
            {
                LSA_UINT16 rc;

                OHA_ALLOC_LOCAL_MEM ((OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR *)(&pData),
                                     (LSA_UINT16)DataLength);
                if (OHA_IS_NULL (pData))
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Alloc for the indication Multiple SNMP Record data failed !");
                    return OHA_ERR_RESOURCE;
                }

                /* get the multiple write record data and compare */
                rc = OHA_GetMultipleSNMPData(NICId,
                                               pData,
                                               &DataLength,
                                               LSA_TRUE);

                if (rc != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Temporary write of Multiple SNMP Record data failed (rc: 0x%X) !", rc);
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }

                OHA_ASSERT_NULL_PTR(pSnmp_Last_Ind->data);
                if (!OHA_MEMCMP(pSnmp_Last_Ind->data, pData, DataLength))
                {
                    DataChanged = LSA_TRUE;
                }

                OHA_FREE_LOCAL_MEM (&rc, pData);
                OHA_ASSERT (rc == LSA_RET_OK);
            }
        }
    }

    if (!DataChanged)
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"Multiple SNMP Record data are the same (NICId: 0x%X)", NICId);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateSnmpData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* store the last indicated data */
    if (DataChanged)
    {
        LSA_UINT16 rc;

        if (OHA_IS_NOT_NULL(pSnmp_Last_Ind->data))
        {
            OHA_FREE_LOCAL_MEM (&rc, pSnmp_Last_Ind->data);
            pSnmp_Last_Ind->data = LSA_NULL;
            OHA_ASSERT (rc == LSA_RET_OK);
        }
        if (DataLength != 0)
        {
            OHA_ALLOC_LOCAL_MEM ((OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR *)(&pSnmp_Last_Ind->data),
                                 (LSA_UINT16)DataLength);
            if (OHA_IS_NULL (pSnmp_Last_Ind->data))
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Alloc for the last indicated Multiple SNMP Record data failed !");
                return OHA_ERR_RESOURCE;
            }
        }

        pSnmp_Last_Ind->data_length  = DataLength;
        pSnmp_Last_Ind->data_tag     = DataTag;

        rc = OHA_GetMultipleSNMPData(NICId,
                                       pSnmp_Last_Ind->data,
                                       &pSnmp_Last_Ind->data_length,
                                       LSA_TRUE);
        if (rc != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Storage of the last indicated Multiple SNMP Record data failed (rc: 0x%X) !", rc);
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
        }
    }

    *pDataChanged = DataChanged;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateSnmpData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*----------------------------------------------------------------------------*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_UpdateBusData(
    LSA_UINT16 NICId,
    LSA_BOOL OHA_COMMON_MEM_ATTR *pDataChanged)
{
    OHA_HDB_REMA_PTR_TYPE pHDBRema = OHA_REMAGetHandleByNIC(NICId);
    LSA_UINT16 RetVal;

    OHA_UPPER_REMA_DATA_PTR_TYPE pBus_Last_Ind;
    LSA_BOOL   DataChanged = LSA_FALSE;
    LSA_UINT32 DataLength  = 0;
    LSA_UINT16 DataTag     = OHA_BUS_DATA;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_UpdateBusData(NICId: 0x%X, pDataChanged: 0x%X)",
                          NICId, pDataChanged);

    OHA_ASSERT_NULL_PTR(pHDBRema);
    OHA_ASSERT_NULL_PTR(pDataChanged);

    pBus_Last_Ind = &pHDBRema->Bus_Last_Ind;

    *pDataChanged = LSA_FALSE;

    /* get length of the multiple write record */
    RetVal = OHA_GetMultipleBusData(NICId,
                                    LSA_NULL,
                                    &DataLength,
                                    LSA_TRUE);

    if (RetVal != OHA_OK)
    {
        OHA_PROGRAM_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Get length of the Multiple BUS Record failed (NICId: 0x%X, RetVal: 0x%X) !", NICId, RetVal);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateBusData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* compare the multiple record data from the last indication */
    if (pBus_Last_Ind->data_length != DataLength)
    {
        DataChanged = LSA_TRUE;
    }

    if (pBus_Last_Ind->data_length != 0 && DataLength != 0)
    {
        if (!DataChanged)   /* compare the rema data */
        {
            OHA_UPPER_MEM_U8_PTR_TYPE pData = LSA_NULL;

            if (DataLength)
            {
                LSA_UINT16 rc;

                OHA_ALLOC_LOCAL_MEM ((OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR *)(&pData),
                                     (LSA_UINT16)DataLength);
                if (OHA_IS_NULL (pData))
                {
                    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Alloc for the indication Multiple BUS Record data failed !");
                    return OHA_ERR_RESOURCE;
                }

                /* get the multiple write record data and compare */
                rc = OHA_GetMultipleBusData(NICId,
                                            pData,
                                            &DataLength,
                                            LSA_TRUE);

                if (rc != OHA_OK)
                {
                    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Temporary write of Multiple BUS Record data failed (rc: 0x%X) !", rc);
                    OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
                }

                OHA_ASSERT_NULL_PTR(pBus_Last_Ind->data);
                if (!OHA_MEMCMP(pBus_Last_Ind->data, pData, DataLength))
                {
                    DataChanged = LSA_TRUE;
                }

                OHA_FREE_LOCAL_MEM (&rc, pData);
                OHA_ASSERT (rc == LSA_RET_OK);
            }
        }
    }

    if (!DataChanged)
    {
        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_LOW,"Multiple BUS Record data are the same (NICId: 0x%X)", NICId);
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateBusData(RetVal: 0x%X)", RetVal);
        return(RetVal);
    }

    /* store the last indicated data */
    if (DataChanged)
    {
        LSA_UINT16 rc;

        if (OHA_IS_NOT_NULL(pBus_Last_Ind->data))
        {
            OHA_FREE_LOCAL_MEM (&rc, pBus_Last_Ind->data);
            pBus_Last_Ind->data = LSA_NULL;
            OHA_ASSERT (rc == LSA_RET_OK);
        }
        if (DataLength != 0)
        {
            OHA_ALLOC_LOCAL_MEM ((OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR *)(&pBus_Last_Ind->data),
                                 (LSA_UINT16)DataLength);
            if (OHA_IS_NULL(pBus_Last_Ind->data))
            {
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Alloc for the last indicated Multiple BUS Record data failed !");
                return OHA_ERR_RESOURCE;
            }
        }

        pBus_Last_Ind->data_length  = DataLength;
        pBus_Last_Ind->data_tag     = DataTag;

        rc = OHA_GetMultipleBusData(NICId,
                                    pBus_Last_Ind->data,
                                    &pBus_Last_Ind->data_length,
                                    LSA_TRUE);
        if (rc != OHA_OK)
        {
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,"Storage of the last indicated Multiple BUS Record data failed (rc: 0x%X) !", rc);
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
        }
    }

    *pDataChanged = DataChanged;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_UpdateBusData(RetVal: 0x%X)", RetVal);
    return(RetVal);
}

/*----------------------------------------------------------------------------*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_IndicateRemaPDEVUsers(
    LSA_UINT16 NICId,
    LSA_HANDLE_TYPE Orginator)
{
    LSA_UINT16 i;
    OHA_HDB_USER_PTR_TYPE pHDBUser = OHA_GET_HUSER_PTR(0);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_IndicateRemaPDEVUsers(NICId: 0x%X, Orginator: 0x%X)",
                          NICId, Orginator);

    switch (OHA_HANDLE_GET_TYPE(Orginator))
    {
        case OHA_HANDLE_TYPE_USER:
        case OHA_HANDLE_TYPE_DCP:
            break;
        default:
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL,"Unknown Orginator (0x%X)!", Orginator);
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    /* look for used User-Channels */
    for (i=0; i < OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
    {
        if (OHA_IS_NOT_NULL(pHDBUser) && (pHDBUser->State == OHA_HANDLE_STATE_OPEN) ) /* user channel is opened ? */
        {
			/* AP01022057: confirm only the user with the same Nic */
			if(NICId == pHDBUser->Params.pDetail->Params.User.NICId)
			{
		        OHA_IndicateRemaPDEV(NICId, pHDBUser);
			}
		}
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_IndicateRemaPDEVUsers(NICId: 0x%X)", NICId);
    return;
}

/*----------------------------------------------------------------------------*/

LSA_VOID  OHA_UPPER_IN_FCT_ATTR oha_rema_init(LSA_VOID) /* AP01039458 */
{
    OHA_HDB_REMA_PTR_TYPE pHDBRema = OHA_GET_HREMA_PTR(0);
    LSA_UINT32 i;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "IN : oha_rema_init()");

    for ( i=0; i<OHA_CFG_MAX_NICS; i++, pHDBRema++) /* lower channels */
    {
        if (OHA_IS_NOT_NULL(pHDBRema))
        {
            pHDBRema->this_idx   = i;

            pHDBRema->rema_state = OHA_PRM_STATE_PREPARE;

            OhaListInitialize (&pHDBRema->set_b);

            OhaListInitialize (&pHDBRema->set_a);
        }
    }

    /***/
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_rema_init() done");
    return;
}

/*----------------------------------------------------------------------------*/

static LSA_VOID  OHA_LOCAL_FCT_ATTR oha_rema_prm_free_list (
    OHA_LIST_ENTRY_PTR_TYPE  list
)
{
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_rema_prm_free_list(list: 0x%X)", list);

    while (! OhaListIsEmpty (list))
    {
        OHA_PRM_RECORD_PTR_TYPE  prm;
        LSA_UINT16  rc;

        OhaListRemoveHead (list, prm, OHA_PRM_RECORD_PTR_TYPE);
        OHA_ASSERT (OHA_IS_NOT_NULL (prm));

        OHA_FREE_LOCAL_MEM (& rc, prm->rec.record_data);
        OHA_ASSERT (rc == LSA_RET_OK);

        OHA_FREE_LOCAL_MEM (& rc, prm);
        OHA_ASSERT (rc == LSA_RET_OK);
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_rema_prm_free_list()");
}

/*----------------------------------------------------------------------------*/
LSA_VOID  OHA_UPPER_IN_FCT_ATTR  oha_PDEV_undo_init(
    LSA_UINT16               NICId
)
{
    OHA_HDB_REMA_PTR_TYPE    pHDBRema;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_PDEV_undo_init(NICId: 0x%X)", NICId);

    pHDBRema = OHA_REMAGetHandleByNIC(NICId);

    if (OHA_IS_NOT_NULL(pHDBRema))
    {
        OHA_UPPER_REMA_DATA_PTR_TYPE pPDEV_Last_Ind = &pHDBRema->PDEV_Last_Ind;

        if (!OhaListIsEmpty(&pHDBRema->set_b))
        {
            oha_rema_prm_free_list (&pHDBRema->set_b);
        }

        if (!OhaListIsEmpty(&pHDBRema->set_a))
        {
            oha_rema_prm_free_list (&pHDBRema->set_a);
        }

        if (OHA_IS_NOT_NULL(pPDEV_Last_Ind->data))
        {
            LSA_UINT16 rc;
            OHA_FREE_LOCAL_MEM (&rc, pPDEV_Last_Ind->data);
            OHA_ASSERT (rc == LSA_RET_OK);
            pPDEV_Last_Ind->data = LSA_NULL;
            pPDEV_Last_Ind->data_length  = 0;
        }

        pHDBRema->rema_state = OHA_PRM_STATE_PREPARE; /* outside done */
    }

    /***/
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_PDEV_undo_init() done");
    return;
}

/*----------------------------------------------------------------------------*/
LSA_VOID  OHA_UPPER_IN_FCT_ATTR  oha_station_undo_init(
    LSA_UINT16               NICId
)
{
    OHA_HDB_REMA_PTR_TYPE    pHDBRema;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_station_undo_init(NICId: 0x%X)", NICId);

    pHDBRema = OHA_REMAGetHandleByNIC(NICId);

    if (OHA_IS_NOT_NULL(pHDBRema))
    {
        OHA_UPPER_REMA_DATA_PTR_TYPE pStation_Last_Ind = &pHDBRema->Station_Last_Ind;
        OHA_UPPER_REMA_DATA_PTR_TYPE pNameOfStation_Last_Ind = &pHDBRema->NameOfStation_Last_Ind;
        OHA_UPPER_REMA_DATA_PTR_TYPE pIPSuite_Last_Ind = &pHDBRema->IPSuite_Last_Ind;
	    OHA_UPPER_REMA_DATA_PTR_TYPE pDHCPData_Last_Ind = &pHDBRema->DHCPData_Last_Ind;

        if (OHA_IS_NOT_NULL(pStation_Last_Ind->data))
        {
            LSA_UINT16 rc;
            OHA_FREE_LOCAL_MEM (&rc, pStation_Last_Ind->data);
            OHA_ASSERT (rc == LSA_RET_OK);
            pStation_Last_Ind->data = LSA_NULL;
            pStation_Last_Ind->data_length  = 0;
        }

        if (OHA_IS_NOT_NULL(pNameOfStation_Last_Ind->data))
        {
            LSA_UINT16 rc;
            OHA_FREE_LOCAL_MEM (&rc, pNameOfStation_Last_Ind->data);
            OHA_ASSERT (rc == LSA_RET_OK);
            pNameOfStation_Last_Ind->data = LSA_NULL;
            pNameOfStation_Last_Ind->data_length  = 0;
        }

        if (OHA_IS_NOT_NULL(pIPSuite_Last_Ind->data))
        {
            LSA_UINT16 rc;
            OHA_FREE_LOCAL_MEM (&rc, pIPSuite_Last_Ind->data);
            OHA_ASSERT (rc == LSA_RET_OK);
            pIPSuite_Last_Ind->data = LSA_NULL;
            pIPSuite_Last_Ind->data_length  = 0;
        }

        if (OHA_IS_NOT_NULL(pDHCPData_Last_Ind->data))
        {
            LSA_UINT16 rc;
            OHA_FREE_LOCAL_MEM (&rc, pDHCPData_Last_Ind->data);
            OHA_ASSERT (rc == LSA_RET_OK);
            pDHCPData_Last_Ind->data = LSA_NULL;
            pDHCPData_Last_Ind->data_length  = 0;
        }
    }

    /***/
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_station_undo_init() done");
    return;
}

/*----------------------------------------------------------------------------*/
LSA_VOID  OHA_UPPER_IN_FCT_ATTR  oha_snmp_undo_init(
    LSA_UINT16               NICId
)
{
    OHA_HDB_REMA_PTR_TYPE    pHDBRema;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_snmp_undo_init(NICId: 0x%X)", NICId);

    pHDBRema = OHA_REMAGetHandleByNIC(NICId);

    if (OHA_IS_NOT_NULL(pHDBRema))
    {
        OHA_UPPER_REMA_DATA_PTR_TYPE pSnmp_Last_Ind = &pHDBRema->Snmp_Last_Ind;

        if (OHA_IS_NOT_NULL(pSnmp_Last_Ind->data))
        {
            LSA_UINT16 rc;
            OHA_FREE_LOCAL_MEM (&rc, pSnmp_Last_Ind->data);
            OHA_ASSERT (rc == LSA_RET_OK);
            pSnmp_Last_Ind->data = LSA_NULL;
            pSnmp_Last_Ind->data_length  = 0;
        }
    }

    /***/
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_snmp_undo_init() done");
    return;
}

/*----------------------------------------------------------------------------*/
LSA_VOID  OHA_UPPER_IN_FCT_ATTR  oha_bus_undo_init(
    LSA_UINT16               NICId
)
{
    OHA_HDB_REMA_PTR_TYPE    pHDBRema;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_bus_undo_init(NICId: 0x%X)", NICId);

    pHDBRema = OHA_REMAGetHandleByNIC(NICId);

    if (OHA_IS_NOT_NULL(pHDBRema))
    {
        OHA_UPPER_REMA_DATA_PTR_TYPE pBus_Last_Ind = &pHDBRema->Bus_Last_Ind;

        if (OHA_IS_NOT_NULL(pBus_Last_Ind->data))
        {
            LSA_UINT16 rc;
            OHA_FREE_LOCAL_MEM (&rc, pBus_Last_Ind->data);
            OHA_ASSERT (rc == LSA_RET_OK);
            pBus_Last_Ind->data = LSA_NULL;
            pBus_Last_Ind->data_length  = 0;
        }
    }

    /***/
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_bus_undo_init() done");
    return;
}

/*----------------------------------------------------------------------------*/
LSA_VOID  OHA_UPPER_IN_FCT_ATTR  oha_rema_prm_free_recordset_a(
    LSA_UINT16               NICId
)
{
    OHA_HDB_REMA_PTR_TYPE    pHDBRema;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_rema_prm_free_recordset_a(NICId: 0x%X)", NICId);

    pHDBRema = OHA_REMAGetHandleByNIC(NICId);

    if (OHA_IS_NOT_NULL(pHDBRema))
    {
        if (!OhaListIsEmpty(&pHDBRema->set_a))
        {
            oha_rema_prm_free_list (&pHDBRema->set_a);
        }
    }

    /***/
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_rema_prm_free_recordset_a() done");
    return;
}

/*----------------------------------------------------------------------------*/

static LSA_VOID  OHA_LOCAL_FCT_ATTR  oha_rema_prm_add_or_replace (
    LSA_UINT16              NICId,
    OHA_PRM_RECORD_PTR_TYPE  prm
)
{
    OHA_PRM_RECORD_PTR_TYPE  old;
    OHA_HDB_REMA_PTR_TYPE    pHDBRema;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_rema_prm_add_or_replace(NICId: 0x%X, prm: 0x%X)", NICId, prm);

    pHDBRema = OHA_REMAGetHandleByNIC(NICId);
    OHA_ASSERT_NULL_PTR(pHDBRema);

    for (OhaListForeach (old, & pHDBRema->set_b, OHA_PRM_RECORD_PTR_TYPE))
    {
        if (
            old->rec.slot_number       == prm->rec.slot_number
            && old->rec.subslot_number == prm->rec.subslot_number
            && old->rec.edd_port_id    == prm->rec.edd_port_id
            && old->rec.record_index   == prm->rec.record_index
        )
        {
            break; /* found */
        }
    }

    if (OHA_IS_NOT_NULL (old))
    {
        LSA_UINT16  rc;

        OhaListRemoveEntry (& old->link);

        OHA_FREE_LOCAL_MEM (& rc, old->rec.record_data);
        OHA_ASSERT (rc == LSA_RET_OK);

        OHA_FREE_LOCAL_MEM (& rc, old);
        OHA_ASSERT (rc == LSA_RET_OK);
    }

    OhaListInsertTail (& pHDBRema->set_b, & prm->link);

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_rema_prm_add_or_replace()");
}

/*----------------------------------------------------------------------------*/

LSA_UINT16  OHA_UPPER_IN_FCT_ATTR  oha_rema_prm_prepare(
    LSA_UINT16 NICId,
    LSA_BOOL is_commit
)
{
    OHA_HDB_REMA_PTR_TYPE    pHDBRema;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_rema_prm_prepare(NICId: 0x%X, is_commit: 0x%X)", NICId, is_commit);

    pHDBRema = OHA_REMAGetHandleByNIC(NICId);
    OHA_ASSERT_NULL_PTR(pHDBRema);

    switch (pHDBRema->rema_state)
    {
        case OHA_PRM_STATE_PREPARE:
        case OHA_PRM_STATE_WRITTEN:
        case OHA_PRM_STATE_END:
            break;

        case OHA_PRM_STATE_COMMIT:
            if ( is_commit )
            {
                break;
            }
            OHA_FATAL (pHDBRema->rema_state);
            return OHA_ERR_SEQUENCE;

        default:
            OHA_FUNCTION_TRACE_01 (OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL, "wrong state(%u)", pHDBRema->rema_state);
            OHA_FATAL (pHDBRema->rema_state);
            return OHA_ERR_SEQUENCE;
    }

    /*
     * clear the record set B
     */

    oha_rema_prm_free_list (&pHDBRema->set_b);

    pHDBRema->rema_state = OHA_PRM_STATE_PREPARE; /* outside done */

    /***/
    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_rema_prm_add_or_replace() done (OHA_OK)");
    return OHA_OK;
}


/*----------------------------------------------------------------------------*/

LSA_UINT16  OHA_UPPER_IN_FCT_ATTR  oha_rema_prm_write(
    LSA_UINT16              NICId,
    OHA_UPPER_RQB_PTR_TYPE  rqb
)
{
    OHA_HDB_REMA_PTR_TYPE    pHDBRema;
    OHA_PRM_RECORD_PTR_TYPE  prm;
    LSA_UINT16  rc;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_rema_prm_write(NICId: 0x%X, rqb: 0x%X)", NICId, rqb);

    pHDBRema = OHA_REMAGetHandleByNIC(NICId);
    OHA_ASSERT_NULL_PTR(pHDBRema);

    switch (pHDBRema->rema_state)
    {

        case OHA_PRM_STATE_PREPARE:
        case OHA_PRM_STATE_WRITTEN:
            break;

        default:
            OHA_FUNCTION_TRACE_01 (OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL, "sequence.error within state(%u)", pHDBRema->rema_state);
            OHA_FATAL (pHDBRema->rema_state);
            return OHA_ERR_SEQUENCE;
    }

    /*
     * write to the record set B
     */

    if (rqb->args.PrmData.record_data_length > 0xFFFF)
    {
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR,
                              "record_data_length(%u) too large for LSA", rqb->args.PrmData.record_data_length);
        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_WARN, "OUT: oha_rema_prm_write() done (OHA_ERR_PARAM)");
        return OHA_ERR_PARAM;
    }

    OHA_ALLOC_LOCAL_MEM ((OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR *)(& prm), sizeof(*prm));

    if (OHA_IS_NULL (prm))
    {
        OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN, "OUT: oha_rema_prm_write() done (OHA_ERR_RESOURCE)");
        return OHA_ERR_RESOURCE;
    }

    prm->rec.slot_number    = rqb->args.PrmData.slot_number;
    prm->rec.subslot_number = rqb->args.PrmData.subslot_number;
    prm->rec.edd_port_id    = rqb->args.PrmData.edd_port_id;

    prm->rec.record_index   = rqb->args.PrmData.record_index;
    prm->rec.mk_rema        = rqb->args.PrmData.mk_rema;

    OHA_ALLOC_LOCAL_MEM ((OHA_UPPER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR *)(& prm->rec.record_data),
                         (LSA_UINT16)rqb->args.PrmData.record_data_length);

    if (OHA_IS_NULL (prm->rec.record_data))
    {
        OHA_FREE_LOCAL_MEM (&rc, prm);
        OHA_ASSERT (rc == LSA_RET_OK);
        OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_WARN, "OUT: oha_rema_prm_write() done (OHA_ERR_RESOURCE)");
        return OHA_ERR_RESOURCE;
    }

    OHA_MEMCPY (prm->rec.record_data, rqb->args.PrmData.record_data, rqb->args.PrmData.record_data_length);

    prm->rec.record_data_length = rqb->args.PrmData.record_data_length;

    /***/

    oha_rema_prm_add_or_replace (NICId, prm);

    pHDBRema->rema_state = OHA_PRM_STATE_WRITTEN;

    /***/

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_rema_prm_write() done (OHA_OK)");
    return OHA_OK;
}


/*----------------------------------------------------------------------------*/

LSA_UINT16  OHA_UPPER_IN_FCT_ATTR  oha_rema_prm_end(
    LSA_UINT16              NICId
)
{
    LSA_UINT16 rc = OHA_OK;
    OHA_HDB_REMA_PTR_TYPE    pHDBRema;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_rema_prm_end(NICId: 0x%X)", NICId);

    pHDBRema = OHA_REMAGetHandleByNIC(NICId);
    OHA_ASSERT_NULL_PTR(pHDBRema);

    switch (pHDBRema->rema_state)
    {

        case OHA_PRM_STATE_PREPARE:
        case OHA_PRM_STATE_WRITTEN:
            break;

        default:
            OHA_FUNCTION_TRACE_01 (OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_FATAL, "sequence.error within state(%u)", pHDBRema->rema_state);
            OHA_FATAL (pHDBRema->rema_state);
            return OHA_ERR_SEQUENCE;
    }

    /*
     * the record set B is written now
     * check if there are rema indication resources provided,
     * if their bufferlength is sufficient
     * and set up everything so that PrmCommit will not fail
     *
     * may report an error here
     */

    pHDBRema->rema_state = OHA_PRM_STATE_END;

    /***/
    {
        OHA_PRM_RECORD_PTR_TYPE  prm;

        OHA_FUNCTION_TRACE_00 (OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "rema-store: BEGIN-OF-DUMP");

        for ( OhaListForeach(prm, &pHDBRema->set_b, OHA_PRM_RECORD_PTR_TYPE) )
        {
            OHA_FUNCTION_TRACE_04 (OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT
                                   , "rema-store: index(%#x) slot(%u) subslot(%#x) data_length(%u)"
                                   , prm->rec.record_index
                                   , prm->rec.slot_number
                                   , prm->rec.subslot_number
                                   , prm->rec.record_data_length
                                  );
        }

        OHA_FUNCTION_TRACE_00 (OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT, "rema-store: END-OF-DUMP");
    }

    /***/
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_rema_prm_write() done (%u)", rc);
    return rc;
}


/*----------------------------------------------------------------------------*/

LSA_UINT16  OHA_UPPER_IN_FCT_ATTR  oha_rema_prm_commit(
    LSA_UINT16      NICId,
    LSA_HANDLE_TYPE ChannelHandle,
    LSA_BOOL        do_rtf
)
{
    LSA_UINT16 rc;
    OHA_HDB_REMA_PTR_TYPE    pHDBRema;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_rema_prm_commit(NICId: 0x%X, ChannelHandle: 0x%X, do_rtf: 0x%X)", NICId, ChannelHandle, do_rtf);

    pHDBRema = OHA_REMAGetHandleByNIC(NICId);
    OHA_ASSERT_NULL_PTR(pHDBRema);

    switch (pHDBRema->rema_state)
    {

        case OHA_PRM_STATE_END:
            rc = OHA_OK;
            break;

        default:
            OHA_FUNCTION_TRACE_01 (OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_ERROR, "sequence.error within state(%u)", pHDBRema->rema_state);
            rc = OHA_ERR_SEQUENCE;
            return (rc);
    }

    /*
     * atomically replace the remanence with the shadow-remanence
     *
     * this must not fail (we said ok on PrmEnd)
     *
     */

    pHDBRema->rema_state = OHA_PRM_STATE_COMMIT;

    /*
     * the record set A is written now
     */
    if ( rc == OHA_OK )
    {
        /*
         * clear the record set A
         */
        oha_rema_prm_free_list (&pHDBRema->set_a);

        /*
         * move record set B to record set A
         */
        while (!OhaListIsEmpty(&pHDBRema->set_b))
        {
            OHA_PRM_RECORD_PTR_TYPE rec;

            OhaListRemoveHead(&pHDBRema->set_b, rec, OHA_PRM_RECORD_PTR_TYPE);

            /* store and indicate the rema records only */
            OHA_ASSERT(rec->rec.mk_rema);

            OhaListInsertTail (&pHDBRema->set_a, &rec->link);
        }

        /* do_rtf=False: signalize the user only if rema data are written. */
        /* do_rtf=True:  ResetToFactory, signal also an empty dataset, so  */
        /*                existing data are deleted.                       */
        if ((!OhaListIsEmpty(&pHDBRema->set_a)) || do_rtf )
        {
            LSA_BOOL DataChanged = LSA_FALSE;

            (LSA_VOID)OHA_UpdatePDEVData(NICId, &DataChanged);

            if (DataChanged)
            {
                /* indicate a multiple PDEV record to the REMA user(s) */
                OHA_IndicateRemaPDEVUsers(NICId, ChannelHandle);
            }
        }

        /***/
        rc = oha_rema_prm_prepare (NICId, LSA_TRUE);
        OHA_ASSERT(rc == OHA_OK);

        OHA_ASSERT(pHDBRema->rema_state == OHA_PRM_STATE_PREPARE);
    }

    /***/
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_rema_prm_commit() done (%u)", rc);
    return (rc);
}

/*----------------------------------------------------------------------------*/

LSA_UINT16 OHA_UPPER_IN_FCT_ATTR oha_rema_station_update(
    LSA_UINT16      NICId
)
{
    OHA_HDB_USER_PTR_TYPE pHDBUser = OHA_GET_HUSER_PTR(0);
    LSA_BOOL DataChanged = LSA_FALSE;
    LSA_UINT16 rc;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_rema_station_update(NICId: 0x%X)", NICId);

	OHA_ASSERT(OHA_EDDCheckNICUsed(NICId)); /* must be an interface-Nic */

    rc = OHA_UpdateStationData(NICId, &DataChanged);

    if (DataChanged)
    {
        LSA_UINT16 i;
        /* indicate a multiple write station record to the REMA user(s) */
        /* look for used User-Channels */
        for (i=0; i < OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
        {
			if (OHA_IS_NOT_NULL(pHDBUser) && (pHDBUser->State == OHA_HANDLE_STATE_OPEN) ) /* user channel is opened ? */
			{
				/* AP01022057: confirm only the user with the same Nic */
				if(NICId == pHDBUser->Params.pDetail->Params.User.NICId)
				{
					if(pHDBUser->Params.pDetail->Params.User.RemaStationFormat == OHA_REMA_STATION_FORMAT_STANDARD)
					{
			            OHA_IndicateRemaStation(NICId, pHDBUser);
					}
				}
			}
        }
    }

    rc |= OHA_UpdateNameOfStationData(NICId, &DataChanged);

    if (DataChanged)
    {
        LSA_UINT16 i;
        /* indicate a NameOfStationRecord to the REMA user(s) */
        /* look for used User-Channels */
        for (pHDBUser = OHA_GET_HUSER_PTR(0), i=0; i < OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
        {
			if (OHA_IS_NOT_NULL(pHDBUser) && (pHDBUser->State == OHA_HANDLE_STATE_OPEN) ) /* user channel is opened ? */
			{
				/* AP01022057: confirm only the user with the same Nic */
				if(NICId == pHDBUser->Params.pDetail->Params.User.NICId)
				{
					if(pHDBUser->Params.pDetail->Params.User.RemaStationFormat == OHA_REMA_STATION_FORMAT_ADVANCED)
					{
			            OHA_IndicateRemaNameOfStation(NICId, pHDBUser);
					}
				}
			}
        }
    }

    rc |= OHA_UpdateIPSuiteData(NICId, &DataChanged);

    if (DataChanged)
    {
        LSA_UINT16 i;
        /* indicate an IPSuiteRecord to the REMA user(s) */
        /* look for used User-Channels */
        for (pHDBUser = OHA_GET_HUSER_PTR(0), i=0; i < OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
        {
			if (OHA_IS_NOT_NULL(pHDBUser) && (pHDBUser->State == OHA_HANDLE_STATE_OPEN) ) /* user channel is opened ? */
			{
				/* AP01022057: confirm only the user with the same Nic */
				if(NICId == pHDBUser->Params.pDetail->Params.User.NICId)
				{
					if(pHDBUser->Params.pDetail->Params.User.RemaStationFormat == OHA_REMA_STATION_FORMAT_ADVANCED)
					{
			            OHA_IndicateRemaIPSuite(NICId, pHDBUser);
					}
				}
			}
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_rema_station_update() done (%u)", rc);
    return (rc);
}

/*----------------------------------------------------------------------------*/

LSA_UINT16 OHA_UPPER_IN_FCT_ATTR oha_rema_dhcp_update(
    LSA_UINT16      NICId
)
{
	OHA_HDB_USER_PTR_TYPE pHDBUser;
    LSA_BOOL DataChanged = LSA_FALSE;
    LSA_UINT16 rc;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_rema_dhcp_update(NICId: 0x%X)", NICId);

	OHA_ASSERT(OHA_EDDCheckNICUsed(NICId)); /* must be an interface-Nic */

    rc = OHA_UpdateDHCPData(NICId, &DataChanged);

    if (DataChanged)
    {
        LSA_UINT16 i;
        /* indicate a DHCPParametersRecord to the REMA user(s) */
        /* look for used User-Channels */
        for (pHDBUser = OHA_GET_HUSER_PTR(0), i=0; i < OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
        {
			if (OHA_IS_NOT_NULL(pHDBUser) && (pHDBUser->State == OHA_HANDLE_STATE_OPEN) ) /* user channel is opened ? */
			{
				/* AP01022057: confirm only the user with the same Nic */
				if(NICId == pHDBUser->Params.pDetail->Params.User.NICId)
				{
					if(pHDBUser->Params.pDetail->Params.User.RemaStationFormat == OHA_REMA_STATION_FORMAT_ADVANCED)
					{
			            OHA_IndicateRemaDHCP(NICId, pHDBUser);
					}
				}
			}
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_rema_dhcp_update() done (%u)", rc);
    return (rc);
}

/*----------------------------------------------------------------------------*/

LSA_UINT16 OHA_UPPER_IN_FCT_ATTR oha_rema_snmp_update(
    LSA_UINT16      NICId
)
{
    OHA_HDB_USER_PTR_TYPE pHDBUser = OHA_GET_HUSER_PTR(0);
    LSA_BOOL DataChanged = LSA_FALSE;
    LSA_UINT16 rc;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_rema_snmp_update(NICId: 0x%X)", NICId);

    rc = OHA_UpdateSnmpData(NICId, &DataChanged);

    if (DataChanged)
    {
        LSA_UINT16 i;
        /* indicate a multiple write station record to the REMA user(s) */
        /* look for used User-Channels */
        for (i=0; i < OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
        {
			if (OHA_IS_NOT_NULL(pHDBUser) && (pHDBUser->State == OHA_HANDLE_STATE_OPEN) ) /* user channel is opened ? */
			{
				/* AP01022057: mirror to every user */
				LSA_UINT16 NicId = pHDBUser->Params.pDetail->Params.User.NICId;
				{
		            OHA_IndicateRemaSnmp(NicId, pHDBUser);
				}
			}
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_rema_snmp_update() done (%u)", rc);
    return (rc);
}

/*----------------------------------------------------------------------------*/

LSA_UINT16 OHA_UPPER_IN_FCT_ATTR oha_rema_bus_update(
    LSA_UINT16      NICId
)
{
    OHA_HDB_USER_PTR_TYPE pHDBUser = OHA_GET_HUSER_PTR(0);
    LSA_BOOL DataChanged = LSA_FALSE;
    LSA_UINT16 rc;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_rema_bus_update(NICId: 0x%X)", NICId);

    rc = OHA_UpdateBusData(NICId, &DataChanged);

    if (DataChanged)
    {
        LSA_UINT16 i;
        /* indicate a multiple write station record to the REMA user(s) */
        /* look for used User-Channels */
        for (i=0; i < OHA_CFG_MAX_CHANNELS; i++, pHDBUser++)
        {
			if (OHA_IS_NOT_NULL(pHDBUser) && (pHDBUser->State == OHA_HANDLE_STATE_OPEN) ) /* user channel is opened ? */
			{
				/* AP01022057: mirror to every user */
				LSA_UINT16 NicId = pHDBUser->Params.pDetail->Params.User.NICId;
				{
		            OHA_IndicateRemaBus(NicId, pHDBUser);
				}
			}
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_rema_bus_update() done (%u)", rc);
    return (rc);
}

/*----------------------------------------------------------------------------*/

#endif    /* OHA_CFG_NO_REMA */

/*****************************************************************************/
/*  end of file OHA_REMA.C                                                   */
/*****************************************************************************/
