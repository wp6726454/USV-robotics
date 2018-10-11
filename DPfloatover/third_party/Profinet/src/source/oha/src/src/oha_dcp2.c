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
/*  F i l e               &F: oha_dcp2.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA DCP functions                                */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  07.09.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  11
#define OHA_MODULE_ID     LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_DCP2 */

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
/* DCP-SET-Trace */
LSA_UINT32 oha_dcp_ParseOptTcp  (LSA_UINT16 NicId, DCP_USR_DATASET_PTR DsPtr, OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd);
LSA_UINT32 oha_dcp_ParseOptDhcp (LSA_UINT16 NicId, DCP_USR_DATASET_PTR DsPtr, OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd);
LSA_UINT32 oha_dcp_ParseOptIdent(LSA_UINT16 NicId, DCP_USR_DATASET_PTR DsPtr, OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd);
LSA_UINT32 oha_dcp_ParseOptPbus (LSA_UINT16 NicId, DCP_USR_DATASET_PTR DsPtr, OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd);
LSA_UINT32 oha_dcp_ParseOptControl(LSA_UINT16 NicId, DCP_USR_DATASET_PTR DsPtr, OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd);

LSA_UINT32 oha_dcp_QueryOptTcp  (LSA_UINT16 NicId, DCP_USR_DATASET_PTR DsPtr, OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd);
LSA_UINT32 oha_dcp_QueryOptIdent(LSA_UINT16 NicId, DCP_USR_DATASET_PTR DsPtr, OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd);

LSA_EXTERN OHA_DCP_LOWER_RQB_PTR_TYPE  oha_dcp_alloc_rsp_rqb(OHA_HDB_DCP_PTR_TYPE pHDB);

/*****************************************************************************/
/* Internal input function: oha_dcp_NameOfStationFilter()                    */
/* Allocs and generates TLV                                                  */
/* Lower Channel IDENTIFY-Filter will be set.                                */
/*****************************************************************************/
OHA_EDD_LOWER_MEM_PTR_TYPE oha_dcp_NameOfStationFilter(
    LSA_UINT16              NICId,
	OHA_COMMON_MEM_U16_PTR_TYPE pTLVLen
)
{
    LSA_USER_ID_TYPE                UserId;
    OHA_EDD_LOWER_MEM_PTR_TYPE      MemPtr  = LSA_NULL;  /* Pointer to TLV-Frame memory. */
    DCP_DATASET_IDENT               * FilterPtr;
    OHA_COMMON_MEM_U8_PTR_TYPE      NamePtr = LSA_NULL;
    LSA_INT                         str_len = OHA_DB_READ_STATION_NAME(NICId, &NamePtr);
    OHA_HDB_EDD_PTR_TYPE            pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

    UserId.uvar32 = 0;
	if(OHA_IS_NOT_NULL(pTLVLen))
	{
		*pTLVLen = 0;
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_NameOfStationFilter(NICId: 0x%X)", NICId);

	OHA_ASSERT_NULL_PTR(pHDBEdd);

    if (str_len)   /* there's a NameOfStation existing */
    {
        OHA_EDD_ALLOC_LOWER_MEM(&MemPtr, UserId,
                                (LSA_UINT16)(str_len + DCP_DS_HDR_SIZE),  /* DCP-Header(Type,Length) + Value */
                                pHDBEdd->Params.pSys);

        if ( ! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
        {
            FilterPtr = (DCP_DATASET_IDENT*)MemPtr;
            FilterPtr->Len = OHA_SWAP16(str_len);
            FilterPtr->Opt = DCP_DBID_IDENT;
            FilterPtr->Id  = DCP_DSID_IDNAME;
            if (str_len != 0)
            {
                OHA_MEMCPY(&FilterPtr->Data, NamePtr, str_len);
            }
            if ((FilterPtr->Opt == DCP_DBID_IDENT) && (FilterPtr->Id == DCP_DSID_IDNAME))
            {
                /* if Filter == NameOfStation:
                 * convert uppercase- to lowercase letter */
                OHA_STRNTOLOWER((OHA_COMMON_MEM_U8_PTR_TYPE)&FilterPtr->Data, str_len);
            }
        }
		if(OHA_IS_NOT_NULL(pTLVLen))
		{
			*pTLVLen = (LSA_UINT16)(str_len + DCP_DS_HDR_SIZE);
		}
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_NameOfStationFilter(RetVal: 0x%X)", MemPtr);
    return MemPtr;
}

/*****************************************************************************/
/* Internal input function: oha_dcp_AliasNameFilter()                        */
/* Allocs and generates TLV                                                  */
/* Lower Channel IDENTIFY-Filter will be set.                                */
/*****************************************************************************/
OHA_EDD_LOWER_MEM_PTR_TYPE oha_dcp_AliasNameFilter(
    LSA_UINT16              NICId,
    LSA_UINT16              PortId,
    LSA_UINT16              SenderId,
	OHA_COMMON_MEM_U16_PTR_TYPE pTLVLen
)
{
    LSA_USER_ID_TYPE                UserId;
    OHA_EDD_LOWER_MEM_PTR_TYPE      MemPtr  = LSA_NULL;  /* Pointer to TLV-Frame memory. */
    DCP_DATASET_IDENT               * FilterPtr;
    OHA_COMMON_MEM_U8_PTR_TYPE      NamePtr = LSA_NULL;
    LSA_UINT16                      Length  = 0;
    LSA_UINT8                       SubType = 0;
    LSA_UINT8                       SenderStatus;
	OHA_DB_IF_PTR_TYPE              pDB;
    OHA_HDB_EDD_PTR_TYPE            pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_AliasNameFilter(NICId: 0x%X)", NICId);

    OHA_ASSERT_NULL_PTR(pHDBEdd);
	pDB = OHA_DB_GET_INTERFACE(NICId);
	OHA_ASSERT_NULL_PTR(pDB);
    OHA_ASSERT(PortId && (PortId <= OHA_CFG_MAX_PORTS));
    OHA_ASSERT(SenderId && (SenderId <= LLDP_MAX_PORT_SENDER));

    UserId.uvar32 = 0;
	if(OHA_IS_NOT_NULL(pTLVLen))
	{
		*pTLVLen = 0;
	}

    /* set the sender-status of the port-senders */
    SenderStatus = pDB->LldpData[PortId-1].RemoteSender[SenderId-1].SnmpSenderStatus;

    if (SenderStatus == OHA_STATUS_DATA_VALID ||
        SenderStatus == OHA_STATUS_DATA_INVALID)
    {
        (LSA_VOID)oha_db_read_rem_chassis_name(pDB->NicId,
                                               PortId,
                                               SenderId,
                                               &SubType,
                                               &NamePtr, &Length);

        /* TIA 832990: no check of LLDP data may lead to a cnf(-) at EDD_SRV_NRT_SET_DCP_FILTER */
        if (Length != 0 && OHA_IS_NOT_NULL(NamePtr) &&
            oha_is_valid_station_name(Length, NamePtr, OHA_UPPER_CASES_ALLOWED(NICId), LSA_FALSE))
        {
			/* AP00942580: remote NameOfStation existing ? */
			if (SubType == OHA_CHASSIS_ID_LOCALLY_ASSIGNED)
			{
				OHA_COMMON_MEM_U8_PTR_TYPE      PortPtr = LSA_NULL;
				LSA_UINT16                      PortLength  = 0;

				/* look at the PortID for the AliasName */
				(LSA_VOID)oha_db_read_rem_port_name(pDB->NicId,
													   PortId,
													   SenderId,
													   LSA_NULL,
													   &PortPtr, &PortLength);

                /* TIA 832990: no check of LLDP data may lead to a cnf(-) at EDD_SRV_NRT_SET_DCP_FILTER */
				if (PortLength != 0 && OHA_IS_NOT_NULL(PortPtr) &&
                    oha_is_valid_port_name(PortLength, PortPtr))
				{
					OHA_EDD_ALLOC_LOWER_MEM(&MemPtr, UserId,
											(LSA_UINT16)(Length + 1 + PortLength + DCP_DS_HDR_SIZE),  /* DCP-Header(Type,Length) + Value */
											pHDBEdd->Params.pSys);

					/* build the AliasNameValue := LLDP_PortID + "." + LLDP_ChassisID */
					if ( ! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
					{
						FilterPtr = (DCP_DATASET_IDENT*)MemPtr;
						FilterPtr->Len = OHA_SWAP16(Length + 1 + PortLength);
						FilterPtr->Opt = DCP_DBID_IDENT;
						FilterPtr->Id  = DCP_DSID_IDALIAS;

						OHA_MEMCPY(&FilterPtr->Data, PortPtr, PortLength);
						OHA_MEMCPY((OHA_EDD_LOWER_MEM_U8_PTR_TYPE)(&FilterPtr->Data)+PortLength, (OHA_COMMON_MEM_PTR_TYPE)".", 1);
						OHA_MEMCPY((OHA_EDD_LOWER_MEM_U8_PTR_TYPE)(&FilterPtr->Data)+PortLength+1, NamePtr, Length);

						/* If Filter == AliasNameValue: convert uppercase- to lowercase letter */
						OHA_STRNTOLOWER((OHA_COMMON_MEM_U8_PTR_TYPE)&FilterPtr->Data, Length);
					}
					if(OHA_IS_NOT_NULL(pTLVLen))
					{
						*pTLVLen = (LSA_UINT16)(Length + 1 + PortLength + DCP_DS_HDR_SIZE);
					}
				}
			}
        }
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_AliasNameFilter(RetVal: 0x%X)", MemPtr);
    return MemPtr;
}

/*****************************************************************************/
/* Internal input function: oha_dcp_PeerNameOfStationFilter()                */
/* Allocs and generates TLV                                                  */
/* Lower Channel IDENTIFY-Filter will be set.                                */
/*****************************************************************************/
OHA_EDD_LOWER_MEM_PTR_TYPE oha_dcp_PeerNameOfStationFilter(
    LSA_UINT16              NICId,
    LSA_UINT16              PortId,
    LSA_UINT16              SenderId,
	OHA_COMMON_MEM_U16_PTR_TYPE pTLVLen
)
{
    LSA_USER_ID_TYPE                UserId;
    OHA_EDD_LOWER_MEM_PTR_TYPE      MemPtr  = LSA_NULL;  /* Pointer to TLV-Frame memory. */
    DCP_DATASET_IDENT               * FilterPtr;
    OHA_COMMON_MEM_U8_PTR_TYPE      NamePtr = LSA_NULL;
    LSA_UINT16                      Length  = 0;
    LSA_UINT8                       SubType = 0;
    LSA_UINT8                       SenderStatus;
    OHA_DB_IF_PTR_TYPE              pDB;
    OHA_HDB_EDD_PTR_TYPE            pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_PeerNameOfStationFilter(NICId: 0x%X)", NICId);

    OHA_ASSERT_NULL_PTR(pHDBEdd);
	pDB = OHA_DB_GET_INTERFACE(NICId);
	OHA_ASSERT_NULL_PTR(pDB);
    OHA_ASSERT(PortId && (PortId <= OHA_CFG_MAX_PORTS));
    OHA_ASSERT(SenderId && (SenderId <= LLDP_MAX_PORT_SENDER));

    UserId.uvar32 = 0;
	if(OHA_IS_NOT_NULL(pTLVLen))
	{
		*pTLVLen = 0;
	}

    /* set the sender-status of the port-senders */
    SenderStatus = pDB->LldpData[PortId-1].RemoteSender[SenderId-1].SnmpSenderStatus;

    if (SenderStatus == OHA_STATUS_DATA_VALID ||
        SenderStatus == OHA_STATUS_DATA_INVALID)
    {
        (LSA_VOID)oha_db_read_rem_chassis_name(pDB->NicId,
                                               PortId,
                                               SenderId,
                                               &SubType,
                                               &NamePtr, &Length);

		/* AP00942580: remote NameOfStation existing ? */
		if (SubType == OHA_CHASSIS_ID_LOCALLY_ASSIGNED)
		{
			if (Length != 0 && OHA_IS_NOT_NULL(NamePtr))
			{
				OHA_EDD_ALLOC_LOWER_MEM(&MemPtr, UserId,
										(LSA_UINT16)(Length + DCP_DS_HDR_SIZE),  /* DCP-Header(Type,Length) + Value */
										pHDBEdd->Params.pSys);

				if ( ! LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
				{
					FilterPtr = (DCP_DATASET_IDENT*)MemPtr;
					FilterPtr->Len = OHA_SWAP16(Length);
					FilterPtr->Opt = DCP_DBID_IDENT;
					FilterPtr->Id  = DCP_DSID_IDNAME;

					OHA_MEMCPY(&FilterPtr->Data, NamePtr, Length);

					/* If Filter == AliasNameOfStation: convert uppercase- to lowercase-letter */
					OHA_STRNTOLOWER((OHA_COMMON_MEM_U8_PTR_TYPE)&FilterPtr->Data, Length);
				}
			}
			if(OHA_IS_NOT_NULL(pTLVLen))
			{
				*pTLVLen = (LSA_UINT16)(Length + DCP_DS_HDR_SIZE);
			}
		}
    }

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_PeerNameOfStationFilter(RetVal: 0x%X)", MemPtr);
    return MemPtr;
}


/*****************************************************************************/
/* Internal input function: oha_dcp_alloc_FilterTable()                      */
/* Allocs filter-table                                                       */
/* Lower Channel IDENTIFY-Filter table will be allocated.                    */
/*****************************************************************************/
OHA_EDD_LOWER_MEM_PTR_TYPE oha_dcp_alloc_FilterTable(
    LSA_UINT16                   NICId,
    LSA_UINT8                    Mode,
    OHA_COMMON_MEM_U16_PTR_TYPE  Entries)
{
    LSA_USER_ID_TYPE                UserId;
    LSA_SYS_PTR_TYPE                pSys;
    OHA_EDD_LOWER_MEM_PTR_TYPE      MemPtr = LSA_NULL;
	OHA_DB_IF_PTR_TYPE              pDB = OHA_DB_GET_INTERFACE(NICId);
    OHA_HDB_EDD_PTR_TYPE            pHDBEdd = OHA_EDDGetHandleByNIC(NICId);
    OHA_EDD_RQB_SET_DCP_FILTER_PTR_TYPE  pSetDcpFilter;

	OHA_ASSERT_NULL_PTR(pHDBEdd);
    pSys   =  pHDBEdd->Params.pSys;
    UserId.uvar32 = 0;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_alloc_FilterTable(NICId: 0x%X, Mode: 0x%X)", NICId, Mode);

	OHA_ASSERT_NULL_PTR(pDB);
	OHA_ASSERT_NULL_PTR(Entries);
    *Entries = 0;

    OHA_EDD_ALLOC_LOWER_MEM(&MemPtr,UserId,sizeof(*pSetDcpFilter),pSys);   /* NameOfStation + neighbours */
    pSetDcpFilter = (OHA_EDD_RQB_SET_DCP_FILTER_PTR_TYPE)MemPtr;

    if ( !LSA_HOST_PTR_ARE_EQUAL(MemPtr, LSA_NULL) )
    {
        EDD_UPPER_MEM_PTR_TYPE pTLV;
		LSA_UINT16       TLVLen = 0;
        LSA_UINT16       i;

        OHA_MEMSET(MemPtr, 0, sizeof(*pSetDcpFilter));

		if(Mode == OHA_SET_FILTER)
		{
			pTLV = (EDD_UPPER_MEM_PTR_TYPE)oha_dcp_NameOfStationFilter(NICId, &TLVLen); /* build TLV for NameOfStation */
			if (OHA_IS_NOT_NULL(pTLV))
			{
				pSetDcpFilter->FilterTLV[*Entries].pTLV = pTLV; /* Pointer to array of pointers to filter TLVs */
				pSetDcpFilter->FilterTLV[*Entries].TLVLen = TLVLen;
				(*Entries)++;
			}
#if 0	
			/* only 1 NOS filter is allowed (Rev6/7: in HW) */
			/* build TLV for peer NameOfStation */
			for (i = 1; i <= OHA_CFG_MAX_PORTS; i++)
			{
				LSA_UINT16 SenderId;

				for (SenderId = 1; SenderId <= LLDP_MAX_PORT_SENDER; SenderId++)
				{
					pTLV = (EDD_UPPER_MEM_PTR_TYPE)oha_dcp_PeerNameOfStationFilter(NICId, i, SenderId, &TLVLen); /* build TLV for peer NameOfStation */
					if (OHA_IS_NOT_NULL(pTLV))
					{
						pSetDcpFilter->FilterTLV[*Entries].pTLV = pTLV; /* Pointer to array of pointers to filter TLVs */
						pSetDcpFilter->FilterTLV[*Entries].TLVLen = TLVLen;
						(*Entries)++;
					}
				}
			}
#endif
			/* build TLV for AliasNameType */
			/* AP01256689, AP01265574: build AliasName if there's only one neighbor per port */
			for (i = 1; i <= OHA_CFG_MAX_PORTS; i++)
			{
				LSA_UINT16 SenderId = pDB->LldpData[i-1].LldpSenderId;

	            /* if there's no sender, SenderId is "0" */
    	        if (SenderId && (SenderId <= LLDP_MAX_PORT_SENDER)) /* get it */
        	    {
            	    LSA_UINT8  SenderStatus = pDB->LldpData[i-1].LldpSenderStatus;

	                /* is there a valid sender ? */
    	            if (SenderStatus == OHA_STATUS_DATA_VALID ||
        	            SenderStatus == OHA_STATUS_DATA_INVALID)
					{
						pTLV = (EDD_UPPER_MEM_PTR_TYPE)oha_dcp_AliasNameFilter(NICId, i, SenderId, &TLVLen); /* build TLV for AliasName */
						if (OHA_IS_NOT_NULL(pTLV))
						{
							pSetDcpFilter->FilterTLV[*Entries].pTLV = pTLV; /* Pointer to array of pointers to filter TLVs */
							pSetDcpFilter->FilterTLV[*Entries].TLVLen = TLVLen;
							(*Entries)++;
						}
					}
				}
			}

#if 0
			/* if OHA has to set the filter type AllSelector */
			{
				EDD_UPPER_MEM_PTR_TYPE FilterPtr = LSA_NULL;

				OHA_EDD_ALLOC_LOWER_MEM((OHA_EDD_LOWER_MEM_PTR_TYPE OHA_LOCAL_MEM_ATTR*)&FilterPtr, UserId,
										DCP_DS_HDR_SIZE,  /* DCP-Header(Type,Length) */
										pHDBEdd->Params.pSys);

				if ( ! LSA_HOST_PTR_ARE_EQUAL(FilterPtr, LSA_NULL) )
				{
					FilterPtr->Len = 0;
					FilterPtr->Opt = DCP_DBID_NOFILTER;
					FilterPtr->Id  = DCP_DSID_NOFILTER;

					pSetDcpFilter->FilterTLV[*Entries].pTLV = FilterPtr; /* Pointer to array of pointers to filter TLVs */
					pSetDcpFilter->FilterTLV[*Entries].TLVLen = DCP_DS_HDR_SIZE;
					(*Entries)++;
				}
			}
#endif
		}
    }
	else
	{
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_UNEXP,"DCP-Allocating FilterTable failed!");
	}

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_alloc_FilterTable(RetVal: 0x%X)", MemPtr);
    return MemPtr;
}


/*****************************************************************************/
/* Internal input function: oha_dcp_free_FilterTable()                       */
/* Frees the filter-table                                                    */
/* Lower Channel IDENTIFY-Filter Tabelle will be freed.                      */
/*****************************************************************************/
LSA_VOID oha_dcp_free_FilterTable  (
    LSA_UINT16                          NICId,
	OHA_EDD_RQB_SET_DCP_FILTER_PTR_TYPE pFilterSet)
{
    LSA_UINT16 i;
    OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NICId);

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_free_FilterTable(NICId: 0x%X, pFilterSet: 0x%X)", NICId, pFilterSet);

	OHA_ASSERT_NULL_PTR(pFilterSet);
	OHA_ASSERT_NULL_PTR(pHDBEdd);

    if (OHA_IS_NOT_NULL(pFilterSet))      /* get the table */
    {
        LSA_UINT16 RetVal = LSA_RET_OK;

        for (i=0; i < pFilterSet->FilterTLVCnt && i < (1 + OHA_CFG_MAX_PORTS); i++) /* loop over the table-entries */
        {
            EDD_UPPER_MEM_PTR_TYPE pTLV = pFilterSet->FilterTLV[i].pTLV;
            if (OHA_IS_NOT_NULL(pTLV))
            {                                      /* free the table-entries */
                OHA_EDD_FREE_LOWER_MEM(&RetVal,pTLV,pHDBEdd->Params.pSys);
                OHA_MEM_FREE_FAIL_CHECK(RetVal);

				pFilterSet->FilterTLV[i].pTLV   = LSA_NULL;
				pFilterSet->FilterTLV[i].TLVLen = 0;
            }
            else
            {              /* should be no more entries in the table */
                break;
            }
        }

		pFilterSet->FilterTLVCnt = 0;
		
		/* free the table itself */
        OHA_EDD_FREE_LOWER_MEM(&RetVal,pFilterSet,pHDBEdd->Params.pSys);
        OHA_MEM_FREE_FAIL_CHECK(RetVal);
    }

    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_free_FilterTable()");
}

/*****************************************************************************/
/* Local function: oha_dcp_CheckSet()                                        */
/*                                                                           */
/* Check of the DCP-Set-ReqPDU (see coding PNIO draft):                      */
/*  DCP-UC-Header, [StartTransactionBlock, BlockQualifier],                  */
/*  (FactoryResetBlock, BlockQualifier) h ^ (ResetToFactoryBlock,            */
/*  BlockQualifier) h ^ SetReqBlock*, [StopTransactionBlock, BlockQualifier] */
/*                                                                           */
/* h If StartTransaction is used together with FactoryResetBlock or          */
/*   ResetToFactoryBlock, StopTransaction shall be part of the PDU.          */
/*   StopTransaction may be used without StartTransaction.                   */
/*                                                                           */
/*****************************************************************************/
LSA_UINT16 oha_dcp_CheckSet(DCP_USR_BYTE_PTR DataPtr, 
							LSA_INT	OptCnt)
{
    LSA_UINT16              RetVal = OHA_OK;
    LSA_INT                 i;

	LSA_BOOL                StartTransBlock   = LSA_FALSE;
	LSA_BOOL                StopTransBlock    = LSA_FALSE;
	LSA_BOOL                FactoryResetBlock = LSA_FALSE;
	LSA_BOOL                AnyOtherBlock     = LSA_FALSE;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_CheckSet(DataPtr: 0x%X, OptCnt: 0x%X)",
                          DataPtr, OptCnt);

    /* check TLV-chain */
    for (i = 1; i <= OptCnt && RetVal == OHA_OK; i++)
    {
		DCP_USR_REQ_DATA_PTR ReqPtr = (DCP_USR_REQ_DATA_PTR)DataPtr;
        DCP_USR_DATASET_PTR  DsPtr  = &ReqPtr->Set.Dataset;

        LSA_INT ReadLen = DCP_DS_HDR_SIZE + OHA_SWAP16(DsPtr->Hdr.Len);

        if ((ReadLen % 2) != 0)
        {
            ReadLen++;   /* jump over fillbyte */
        }

        if (DsPtr->Hdr.Opt == DCP_DBID_CTRL)
		{
			/* TFS 852764: Check coding of the DCP-Set-ReqPDU, see PNIO draft */
			switch (DsPtr->Hdr.Id)
			{
				case DCP_DSID_START:
				{
					if(i != 1) /* StartTransactionBlock must be the first and only once */
					{
						OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"Wrong coding - StartTransactionBlock (block: 0x%X)!", i);
						RetVal = OHA_ERR_PROTOCOL;
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_dcp_CheckSet(RetVal: 0x%X)",RetVal);
						return(RetVal);
					}
					StartTransBlock = LSA_TRUE;
				}
				break;
				case DCP_DSID_END:
				{
					if(i != OptCnt) /* StopTransactionBlock must be the last and only once */
					{
						OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"Wrong coding - StopTransactionBlock (block: 0x%X)!", i);
						RetVal = OHA_ERR_PROTOCOL;
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_dcp_CheckSet(RetVal: 0x%X)",RetVal);
						return(RetVal);
					}
					StopTransBlock = LSA_TRUE;
				}
				break;
				case OHA_DCP_RESET_TO_FACTORY:
				case OHA_DCP_RESET:
				{
					if(FactoryResetBlock) /* ResetToFactory or FactoryReset only once */
					{
						OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"Wrong coding - StopTransactionBlock (block: 0x%X)!", i);
						RetVal = OHA_ERR_PROTOCOL;
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_dcp_CheckSet(RetVal: 0x%X)",RetVal);
						return(RetVal);
					}
					FactoryResetBlock = LSA_TRUE;
				}
				break;
				default:
					AnyOtherBlock = LSA_TRUE;
				break;
			}
		}
		else
		{
			AnyOtherBlock = LSA_TRUE;
		}

		DataPtr += ReadLen;
    }

	if (FactoryResetBlock)
	{
		if (AnyOtherBlock)
		{
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_WARN,"Wrong coding - FactoryResetBlock and other block existing (block: 0x%X)!", i);
			RetVal = OHA_ERR_PROTOCOL;
		}
		if (StartTransBlock && !StopTransBlock)
		{
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_WARN,"Wrong coding - StopTransBlock missing (block: 0x%X)!", i);
			RetVal = OHA_ERR_PROTOCOL;
		}
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_CheckSet(RetVal: 0x%X)",RetVal);
    return(RetVal);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_dcp_ParseSet                            +*/
/*+  Input                      OHA_DCP_LOWER_RQB_PTR_TYPE         pRQB     +*/
/*+                             OHA_UPPER_DCP_INDICATION_PTR_TYPE  pSetInd  +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+  pSetInd               :    Params for a DcpSet  / out /                +*/
/*+                                                                         +*/
/*+  Result: OHA_OK             the DCP-Set can be done                     +*/
/*+          OHA_ERR_RESOURCE   ther's no User-Ind Rqb for Query            +*/
/*+          OHA_ERR_PROTOCOL   no DS in the frame                          +*/
/*+          OHA_ERR_SEQUENCE   a DCP-Set is already active - Reject        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: scan a DCP-Set sequence                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 oha_dcp_ParseSet(
    OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_DCP_PTR_TYPE         pHDB,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pSetInd)
{
    LSA_UINT16              RetVal;
    LSA_INT                 i;
    LSA_INT                 ReqLen;
    LSA_INT                 ReadLen;
    LSA_INT                 WriteLen = 0;
    DCP_USR_BYTE_PTR        ReadPtr;
    DCP_USR_DATASET_PTR     DsPtr;
    DCP_RQB_ARGS_RECV_IND DCP_UPPER_MEM_ATTR * pRecv = &pRQB->args.RecvInd;
    DCP_USR_REQ_DATA_PTR    ReqPtr  = (DCP_USR_REQ_DATA_PTR)(pRecv->DataPtr);
    LSA_INT                 RestLen;
    LSA_UINT16              NicId;

	OHA_ASSERT_NULL_PTR(pHDB);
    OHA_ASSERT_NULL_PTR(pHDB->pDB);
	OHA_ASSERT_NULL_PTR(pSetInd);

	NicId = pHDB->Params.pDetail->Params.Dcp.NICId;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_ParseSet(pRQB: 0x%X,NIC-ID: 0x%X,pSetInd: 0x%X)",
                          pRQB, NicId, pSetInd);

    /* init the options     */
    OHA_MEMCPY(pSetInd->SenderMACAddr, pRecv->AddrPtr, OHA_MAC_ADDR_SIZE);
    pSetInd->Event = OHA_EVENT_REMOTE_SET;
    pSetInd->Count = 0;


	RetVal = oha_dcp_CheckSet(pRecv->DataPtr, pRecv->OptCnt);
	if (RetVal != OHA_OK)
	{
		/* TFS 852764: Wrong coding of the DCP-Set-ReqPDU, see PNIO draft */
		pSetInd->Event = OHA_MODE_NO_SET;
		pSetInd->Reason = OHA_DCP_REASON_DS_NOT_SET;
		RetVal = OHA_ERR_PROTOCOL;
		OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_WARN,"Wrong coding of the DCP-Set-ReqPDU (RetVal: 0x%X)!", RetVal);

        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_dcp_ParseSet(RetVal: 0x%X)",RetVal);
		return(RetVal);
	}

    /* check request: at minimum one dataset existing */
    if (pRecv->OptCnt > 0)
    {
        LSA_UINT8               DbaseID;
        LSA_UINT32              QueryOptions = 0;

        DsPtr = &ReqPtr->Set.Dataset;
        ReadPtr = (DCP_USR_BYTE_PTR)DsPtr;

        /* get length of TLV-chain */
        for (i = 0, ReqLen = 0; i < pRecv->OptCnt; i++)
        {
            ReadLen = DCP_DS_HDR_SIZE + OHA_SWAP16(DsPtr->Hdr.Len);
            if ((ReadLen % 2) != 0)
            {
                /* jump over fillbyte */
                ReadLen++;
            }
            ReqLen += ReadLen;
            ReadPtr += ReadLen;
            DsPtr = (DCP_USR_DATASET_PTR)ReadPtr;
        }

        RestLen   = ReqLen;
        DsPtr = &ReqPtr->Set.Dataset;
        ReadPtr = (DCP_USR_BYTE_PTR)DsPtr;

        while (RestLen > 0)
        {
            ReadLen = DCP_DS_HDR_SIZE + OHA_SWAP16(DsPtr->Hdr.Len);
            WriteLen += DCP_DS_RESULT_MIN_SIZE;

            if (RestLen >= ReadLen)
            {
                DbaseID = DsPtr->Hdr.Opt;

				if (WriteLen > DCP_MAX_RSP_DATA_SIZE)
				{
					/* TFS 743037: The (further) requested data do not fit in the DCP-Set-ResPDU ! */
					pSetInd->Event = OHA_MODE_NO_SET;
					pSetInd->Reason = OHA_DCP_REASON_NO_DATASET;
					RetVal = OHA_ERR_PROTOCOL;
					OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_HIGH,"Too many datasets (OptCnt: %d) requested from the Ethenet!", pRecv->OptCnt);

	                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_dcp_ParseSet(RetVal: 0x%X)",RetVal);
					return(RetVal);
				}

                switch (DbaseID)
                {
                    case DCP_DBID_TCP:          /* TCP/IP-Parameter 1 */
                        QueryOptions |= oha_dcp_ParseOptTcp(NicId, DsPtr, pSetInd); /* MacAddr, IpAddr, SubnetMask, DefRouterAddr */
                        break;
                    case DCP_DBID_DHCP:         /* DHCP-Parameter 3 */
                        QueryOptions |= oha_dcp_ParseOptDhcp(NicId, DsPtr, pSetInd);  /* DhcpOption */
                        break;
                    case DCP_DBID_IDENT:        /* Device Identification 2 */
                        QueryOptions |= oha_dcp_ParseOptIdent(NicId, DsPtr, pSetInd); /* StationType, StationName, StationId */
                        break;
                    case DCP_DBID_CTRL:         /* Control Option 5 */
                        QueryOptions |= oha_dcp_ParseOptControl(NicId, DsPtr, pSetInd);
                        break;                  /* LockBegin, FlashStart */
                    case DCP_DBID_PBC:          /* PBC-Profil (OrangeBox) 128 */
                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,"OrangeBox (DbaseID = 0x%X) not supported anymore.", DbaseID);
                        break;
                    case DCP_DBID_PBPARAM:      /* Profibusparameter 129 */
                        QueryOptions |= oha_dcp_ParseOptPbus(NicId, DsPtr, pSetInd); /* ProfibusProfil, ProfibusParam */
                        break;
                    default:                    /* Unknown Database */
                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,"Database (DbaseID = 0x%X) not existing in the directory.", DbaseID);
                        break;
                }
            }
            else
            {                   /* remaining length too short for a further dataset */
                OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,"Remaining length too short for this dataset!");
            }

            /* Further Dataset in the Request? */
            if ((ReadLen % 2) != 0)
            {
                /* Jump over Fillbyte */
                ReadLen++;
            }
            RestLen -= ReadLen;
            if (RestLen != 0)
            {
                ReadPtr += ReadLen;
                DsPtr = (DCP_USR_DATASET_PTR)ReadPtr;   /* FIX_03 */
            }
        }

        /* prepare the pSetInd.Event for oha_dcp_Set():  */

        /* are there parameters to set and are they allowed to set ? */
        if (pSetInd->Options)      /* are there parameters to set - QUERY the SYSTEM first */
        {
            LSA_UINT8 Reason;   /* result for the DCP protocol */

            if (QueryOptions)  /* are there Query-Options */
            {
                if (OHA_IS_NULL(pHDB->pHDBUserCtrl)) /* no query-user existing ? */
                {
                    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_HIGH, "OUT: oha_dcp_ParseSet() - no query-user existing !");
                    pSetInd->Event = OHA_EVENT_REMOTE_SET;
                    (LSA_VOID)OHA_REPORT_DCP_SET_IND(NicId, pSetInd, (DCP_USR_BYTE_PTR)ReqPtr, ReqLen); /* report */
                    pSetInd->Event = OHA_MODE_NO_SET;
                    pSetInd->Reason = OHA_DCP_REASON_NO_RESOURCE;
                    RetVal = OHA_ERR_RESOURCE;
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_dcp_ParseSet(RetVal: 0x%X)",RetVal);
                    return(RetVal);
                }
            }

            /* IP or NoS is to set - look for a running user set */
            if ((pSetInd->Options & OHA_CTRL_STATION_NAME) ||
                (pSetInd->Options & OHA_CTRL_IP_SUITE))
            {
                if (OHA_IsAnyLocalSetActive(NicId))
                {
                    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_HIGH, "OUT: oha_dcp_ParseSet() - a user set is active !");
                    pSetInd->Event = OHA_EVENT_REMOTE_SET;
                    (LSA_VOID)OHA_REPORT_DCP_SET_IND(NicId, pSetInd, (DCP_USR_BYTE_PTR)ReqPtr, ReqLen); /* report */
                    pSetInd->Event = OHA_MODE_NO_SET;
                    pSetInd->Reason = OHA_DCP_REASON_NOT_APPLICABLE;
                    RetVal = OHA_ERR_SEQUENCE;
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_dcp_ParseSet(RetVal: 0x%X)",RetVal);
                    return(RetVal);
                }
            }

#if 0
            /* ResetToFactory could lead to a REMA-Set */
            if (pSetInd->Options & OHA_CTRL_RESET_TO_FACTORY || 
				pSetInd->Options & OHA_CTRL_RESET_COMMUNICATION)
            {
                OHA_HDB_USER_PTR_TYPE pHDBPrmUser;

                /* wait for REMA Set (ResetToFactory) or PrmCommit */
                if (OHA_IsRemaResponsePending(pHDB->pDB->InterfaceID))
                {
                    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OUT: oha_dcp_ParseSet() - a REMA Set (DCP or PrmCommit is active !");
                    pSetInd->Event = OHA_EVENT_REMOTE_SET;
                    (LSA_VOID)OHA_REPORT_DCP_SET_IND(NicId, pSetInd, (DCP_USR_BYTE_PTR)ReqPtr, ReqLen); /* report */
                    pSetInd->Event = OHA_MODE_NO_SET;
                    pSetInd->Reason = OHA_DCP_REASON_NOT_APPLICABLE;
                    RetVal = OHA_ERR_SEQUENCE;
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_dcp_ParseSet(RetVal: 0x%X)",RetVal);
                    return(RetVal);
                }

                /* wait for OHA_OPC_REMA_RESPONSE */
                pHDBPrmUser = OHA_IsAnyPrmCommitActive(NICId);
                if (OHA_IS_NOT_NULL(pHDBPrmUser))
                {
                    OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NICId), LSA_TRACE_LEVEL_NOTE_HIGH, "OUT: oha_dcp_ParseSet() - no set, OHA_OPC_PRM_COMMIT is active !");
                    pSetInd->Event = OHA_EVENT_REMOTE_SET;
                    (LSA_VOID)OHA_REPORT_DCP_SET_IND(NicId, pSetInd, (DCP_USR_BYTE_PTR)ReqPtr, ReqLen); /* report */
                    pSetInd->Event = OHA_MODE_NO_SET;
                    pSetInd->Reason = OHA_DCP_REASON_NOT_APPLICABLE;
                    RetVal = OHA_ERR_SEQUENCE;
                    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_dcp_ParseSet(RetVal: 0x%X)",RetVal);
                    return(RetVal);
                }
            }
#endif

            /* Inidcate SET-Request: DCP-Indication (EventSet) */
            pSetInd->Event = OHA_EVENT_REMOTE_SET;
            Reason = OHA_REPORT_DCP_SET_IND(NicId, pSetInd, (DCP_USR_BYTE_PTR)ReqPtr, ReqLen);
            OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                                  "OUT: oha_dcp_Set-Ind.Report (RetVal: 0x%X, ChangeOf: 0x%X, Option: 0x%X, SubOption: 0x%X)",
                                  Reason, pSetInd->Options, pSetInd->Type, pSetInd->SubType);

            RetVal = OHA_DcpGetReason(Reason);
            pSetInd->Reason = Reason;
            if (RetVal != OHA_OK)
            {
                pSetInd->Event = OHA_MODE_NO_SET;
                OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_dcp_ParseSet(RetVal: 0x%X)",RetVal);
                return(RetVal);
            }

			/* DCP Security-Test (AP01337555, AP01348107) */
			/* Rtf: The NameOfStation and / or the IP-Address is to delete => no OHA_CTRL_IP_SUITE */
			/* or OHA_CTRL_STATION_NAME is allowed. */
            if (pSetInd->Options & OHA_CTRL_RESET_TO_FACTORY ||
				pSetInd->Options & OHA_CTRL_RESET_COMMUNICATION)
            {
				/* RTF and NoS are to set - look for a valid set */
				if (pSetInd->Options & OHA_CTRL_STATION_NAME)
				{
					OHA_COMMON_MEM_U8_PTR_TYPE NamePtr = LSA_NULL;
                    LSA_UINT16 length = (LSA_UINT16)oha_db_read_temp_station_name(NicId, &NamePtr, LSA_NULL, LSA_NULL);
					if(length != 0) /* only len=0 is valid within RTF */
					{
						OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_HIGH, "OUT: oha_dcp_ParseSet() - RtF and NoS, not a valid set !");
						pSetInd->Reason = OHA_DCP_REASON_DS_NOT_SET;
	                    pSetInd->Event = OHA_MODE_NO_SET;
						RetVal = OHA_ERR_PARAM;
						OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_dcp_ParseSet(RetVal: 0x%X)",RetVal);
						return(RetVal);
					}
				}

				/* RTF and IP are to set - look for a valid set */
				if (pSetInd->Options & OHA_CTRL_IP_SUITE)
				{
                    OHA_COMMON_MEM_U8_PTR_TYPE  IpSuitePtr = LSA_NULL;
                    LSA_UINT16 size = oha_db_read_temp_ip_address(NicId, &IpSuitePtr, LSA_NULL, LSA_NULL);
					if(size == OHA_IP_SUITE_SIZE && OHA_IS_NOT_NULL(IpSuitePtr)) /* only IpSuite={0} is valid within RTF */
					{
						if(!OHA_MEMCMP(IpSuitePtr, (OHA_COMMON_MEM_U8_PTR_TYPE)OHA_ZERO_IP_SUITE, OHA_IP_SUITE_SIZE))
						{
							OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE_HIGH, "OUT: oha_dcp_ParseSet() - RtF and IP, not a valid set !");
							pSetInd->Reason = OHA_DCP_REASON_DS_NOT_SET;
							pSetInd->Event = OHA_MODE_NO_SET;
							RetVal = OHA_ERR_PARAM;
							OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_dcp_ParseSet(RetVal: 0x%X)",RetVal);
							return(RetVal);
						}
					}
				}
			}

            if (QueryOptions)    /* are there Options for QUERY the USER */
            {
                if (pHDB->pHDBUserCtrl) /* user-control for a DcpSet ? */
                {
                    pSetInd->Event = OHA_EVENT_QUERY_SET;
                    if (LSA_RET_OK == OHA_DcpIndicateEvent(pHDB->pHDBUserCtrl, pSetInd)) /* event only to control-user */
                    {                         /* got indication rqb */
                        OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DCPRecvDone()");
                    }
                    else     /* No DataSet because of no UserIndRqbs */
                    {
                        OHA_FUNCTION_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "OUT: oha_dcp_ParseSet() - no UserIndRqbs found !");
                        pSetInd->Event = OHA_MODE_NO_SET;
                        pSetInd->Reason = OHA_DCP_REASON_NO_RESOURCE;
                        RetVal = OHA_ERR_RESOURCE;
                    }
                }
            }
        }
    }
    else
    {
        pSetInd->Event = OHA_MODE_NO_SET;
        pSetInd->Reason = OHA_DCP_REASON_NO_DATASET;
        RetVal = OHA_ERR_PROTOCOL;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"No Dataset requested from the Ethenet!");
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_ParseSet(RetVal: 0x%X)",RetVal);
    return(RetVal);
}

/*****************************************************************************/
/* Local function: oha_dcp_ParseOptControl()                                 */
/* Datasets of the Database "Control Options" (5) in a set-req               */
/* will be stored locally                                                    */
/*****************************************************************************/
LSA_UINT32                oha_dcp_ParseOptControl(
    LSA_UINT16              NicId,
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd)
{
    LSA_UINT16  Status = OHA_SWAP16(DsPtr->Status);
    LSA_UINT32 QueryOptions = 0;

	OHA_ASSERT_NULL_PTR(pInd);

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_ParseOptControl(NicId: 0x%X, DsPtr: 0x%X, DsPtr->Hdr.Id: 0x%X)",
                          NicId, DsPtr, DsPtr->Hdr.Id);

    /* only set the option-flag, if there is a control-option for query in the DB */
    /* all the control-options are confirmed by OhaDcp.Ind (OHA_EVENT_CONTROL) */
    switch (DsPtr->Hdr.Id)
    {
        case DCP_DSID_START: /* StartTransactionBlock   StartTransactionType, BlockLength=0 */
            break;

        case DCP_DSID_END:  /* StopTransactionBlock StopTransactionType, BlockLength=0 */
            break;

        case DCP_DSID_SIGNAL: /* SignalBlock    SignalType, BlockLength=2, SignalValue */
            break;

        case  DCP_DSID_RESPONSE: /* TFS 852764: Suboption Response only for coding of a DCP-ResPDU, see PNIO draft */
            break;

        case  OHA_DCP_RESET_TO_FACTORY: /* FactoryResetBlock    FactoryResetType, BlockLength=0 */
        {
            LSA_UINT8 AccessControl = oha_db_read_set_access(NicId,
                                                             DCP_DBID_CTRL, OHA_DCP_RESET_TO_FACTORY); /* is it locked ? */
            if (AccessControl)
            {
                pInd->Options |= OHA_CTRL_RESET_TO_FACTORY;

                if (oha_db_read_query_set_access(NicId, DCP_DBID_CTRL, OHA_DCP_RESET_TO_FACTORY)) /* is it to query ? */
                {
                    QueryOptions |= OHA_CTRL_RESET_TO_FACTORY;
                }
            }
        }
        break;

        case  OHA_DCP_RESET: /* ResetToFactoryBlock    ResetToFactoryType, BlockLength=0 */
        {
			/* BlockQualifier with option ControlOption and suboption SuboptionResetToFactory */
			/* Bit 0		reserved */
			/* Bit 1 - 15	behavior */
			/*       1 ... Reset application data */
			/*       2 ... Reset communication parameter (only supported by PNIO) */
			/*       3 ... Reset engineering parameter */
			/*  5 -  7 ... Reserved                    */
			/*       8 ... Reset all stored data ...   */
			/*       9 ... Reset and restore data      */
			/* 10 - 15 ... Reserved                    */

			Status >>= 1;      /* Bit 0 reserved */
			if(Status == DCP_STATUS_RESET_COMMUNICATION)   /* only supported by PNIO */ 
			{
				LSA_UINT8 AccessControl = oha_db_read_set_access(NicId,
																 DCP_DBID_CTRL, OHA_DCP_RESET); /* is it locked ? */
				if (AccessControl)
				{
					pInd->Options |= OHA_CTRL_RESET_COMMUNICATION;

					if (oha_db_read_query_set_access(NicId, DCP_DBID_CTRL, OHA_DCP_RESET)) /* is it to query ? */
					{
						QueryOptions |= OHA_CTRL_RESET_COMMUNICATION;
					}
				}
			}
        }
        break;

        default:        /* Unknown Suboption */
            break;
    }

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_ParseOptControl(), DsPtr: 0x%X, QueryOptions:0x%X", DsPtr, QueryOptions);
    return (QueryOptions);
}

/*****************************************************************************/
/* Local function: oha_dcp_ParseOptPbus()                                    */
/* Datasets of the Profibus-Database in a SET-Req. will be stored locally    */
/*****************************************************************************/
LSA_UINT32                oha_dcp_ParseOptPbus(
    LSA_UINT16              NicId,
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd)
{
    LSA_UINT16      DsLen;
    LSA_UINT16      Status = OHA_SWAP16(DsPtr->Status);
    LSA_UINT32      QueryOptions = 0;
	LSA_UINT8       AccessMode = 0;

	OHA_ASSERT_NULL_PTR(pInd);

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_ParseOptPbus(NICId: 0x%X, DsPtr: 0x%X, DsPtr->Hdr.Id: 0x%X)",
                          NicId, DsPtr, DsPtr->Hdr.Id);

    if (OHA_SWAP16(DsPtr->Hdr.Len) < 2)
    {
        DsLen  = 0;
    }
    else
    {
        DsLen  = (LSA_UINT16)(OHA_SWAP16(DsPtr->Hdr.Len) - 2);
    }

    switch (DsPtr->Hdr.Id)
    {
            /* get values of the dataset from the request and put them to the system */
        case DCP_DSID_PBCOMMON:         /* Common profibus parameters */
            (LSA_VOID)OHA_DB_READ_ACCESS_CONTROL(NicId, DCP_DBID_PBPARAM, DCP_DSID_PBCOMMON, &AccessMode);
			if(!AccessMode)
			{
                break; /* no dataset */
			}
            if (DsLen == DCP_DS_PBCOMMON_SIZE + 3/*OUI*/)
            {
				if(DsPtr->Data.PbManufact.OUI[0] == 0x08 &&
					DsPtr->Data.PbManufact.OUI[1] == 0x00 &&
					DsPtr->Data.PbManufact.OUI[2] == 0x06)
				{
					pInd->Options |= OHA_CTRL_PROFIBUS_COMMON;
					pInd->Count++;
					if (oha_db_read_query_set_access(NicId, DCP_DBID_PBPARAM, DCP_DSID_PBCOMMON)) /* is'nt it locked ? */
					{         /* it's to set (not locked) */
						QueryOptions |= OHA_CTRL_PROFIBUS_COMMON;
					}
					if (Status & DCP_MASK_DS_REMANENT)
					{
						pInd->RemaOptions |= OHA_CTRL_PROFIBUS_COMMON;
					}
				}
				else
				{
					OHA_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,"Common profibus parameters: OUI (0x%X 0x%X 0x%X) is wrong, must be (0x08 0x00 0x06).", 
											DsPtr->Data.PbManufact.OUI[0], DsPtr->Data.PbManufact.OUI[1], DsPtr->Data.PbManufact.OUI[2]);
				}
			}
			else
			{
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,"Common profibus parameters: DsLen (0x%X) invalid, must be 28+3.", DsLen);
			}
            break;
        case DCP_DSID_PBADDITION:          /* additional profibus parameters */
            (LSA_VOID)OHA_DB_READ_ACCESS_CONTROL(NicId, DCP_DBID_PBPARAM, DCP_DSID_PBADDITION, &AccessMode);
			if(!AccessMode)
			{
                break; /* no dataset */
			}
            if (DsLen == DCP_DS_PBADDITION_SIZE + 3/*OUI*/)
            {
				if(DsPtr->Data.PbManufact.OUI[0] == 0x08 &&
					DsPtr->Data.PbManufact.OUI[1] == 0x00 &&
					DsPtr->Data.PbManufact.OUI[2] == 0x06)
				{
					pInd->Options |= OHA_CTRL_PROFIBUS_ADDITION;
					pInd->Count++;
					if (oha_db_read_query_set_access(NicId, DCP_DBID_PBPARAM, DCP_DSID_PBADDITION)) /* is'nt it locked ? */
					{         /* it's to set (not locked) */
						QueryOptions |= OHA_CTRL_PROFIBUS_ADDITION;
					}
					if (Status & DCP_MASK_DS_REMANENT)
					{
						pInd->RemaOptions |= OHA_CTRL_PROFIBUS_ADDITION;
					}
				}
				else
				{
					OHA_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,"Additional profibus parameters: OUI (0x%X 0x%X 0x%X) is wrong, must be (0x08 0x00 0x06).", 
											DsPtr->Data.PbManufact.OUI[0], DsPtr->Data.PbManufact.OUI[1], DsPtr->Data.PbManufact.OUI[2]);
				}
            }
			else
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE,"Additional profibus parameters: DsLen (0x%X) invalid, must be 26+3.", DsLen);
            }
            break;
        default:                    /* unknowm dataset */
            if (DsLen > 1)
            {       /* put the PB-Option to the System */
                pInd->Options |= OHA_CTRL_ANY_OTHER_OPTION;
                pInd->Count++;
                if (oha_db_read_query_set_access(NicId, DCP_DBID_PBPARAM, DsPtr->Hdr.Id)) /* is it locked ? */
                {
                    QueryOptions |= OHA_CTRL_ANY_OTHER_OPTION;
                }
                pInd->Type = DsPtr->Hdr.Opt;
                pInd->SubType = DsPtr->Hdr.Id;
                if (Status & DCP_MASK_DS_REMANENT)
                {
                    pInd->RemaOptions |= OHA_CTRL_ANY_OTHER_OPTION;
                }
            }
            break;
    }

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_ParseOptPbus(), DsPtr: 0x%X, QueryOptions:0x%X", DsPtr, QueryOptions);
    return (QueryOptions);
}

/*****************************************************************************/
/* Local function: oha_dcp_ParseOptIdent()                                   */
/* Datasets of the Database "General. DeviceInfo"(8) in the SET-Request      */
/* will be stored locally                                                    */
/*****************************************************************************/
LSA_UINT32                oha_dcp_ParseOptIdent(
    LSA_UINT16              NicId,
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd)
{
    LSA_UINT16  Status;
    LSA_UINT16  DsLen;
    LSA_UINT16  DataLen = 0;
    LSA_UINT8 AccessControl;
    LSA_UINT32  QueryOptions = 0;

	OHA_ASSERT_NULL_PTR(pInd);

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_ParseOptIdent(NicId: 0x%X, DsPtr: 0x%X, DsPtr->Hdr.Id: 0x%X)",
                          NicId, DsPtr, DsPtr->Hdr.Id);

    if (OHA_SWAP16(DsPtr->Hdr.Len) < 2)
    {
        DsLen  = 0;
    }
    else
    {
        DsLen  = (LSA_UINT16)(OHA_SWAP16(DsPtr->Hdr.Len) - 2);
    }

    /* Todo: no check necessary, if output function will be changed to LSA_UINT16 */
    if (DsLen < 256)
        DataLen = (LSA_UINT8)DsLen;

    /* remanence?   */
    Status = OHA_SWAP16(DsPtr->Status);

    switch (DsPtr->Hdr.Id)
    {
            /* Put the Values of the Datasets to the System  */
        case DCP_DSID_IDTYPE:       /* Stationstype = ReadOnly */
            break;
        case DCP_DSID_IDNAME:       /* NameOfStation */
            AccessControl = oha_db_read_set_access(NicId,
                                                   DCP_DBID_IDENT, DCP_DSID_IDNAME); /* is it locked ? */
            if (AccessControl)
            {
                LSA_BOOL Found = LSA_FALSE;
                OHA_COMMON_MEM_U8_PTR_TYPE pName = LSA_NULL;

                LSA_INT len = OHA_DB_READ_STATION_NAME(NicId, &pName);

                /* AP01337555, AP01348107: store temporary at any rate */
                LSA_UINT16 RetVal = oha_db_store_temp_station_name (NicId, DsPtr->Data.IdentName.String, DataLen);
                if (RetVal != LSA_RET_OK)
                {
                    OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_dcp_ParseOptIdent: oha_db_store_temp_station_name failed (RetVal: 0x%X).", RetVal);
                }

                /* check the "incoming" NameOfStation strictly */
                /* NameLen = 0 shall delete the NameOfStation */
                if (DataLen && (!oha_is_valid_station_name(DataLen, DsPtr->Data.IdentName.String, OHA_UPPER_CASES_ALLOWED(NicId), LSA_FALSE)))
                {
                    OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE,"oha_dcp_ParseOptIdent: Invalid NameOfStation.");
                    break;  /* invalid name of station */
                }

                if (len == DataLen)
                {
                    Found = oha_namecmp(DsPtr->Data.IdentName.String, pName, len, OHA_UPPER_CASES_ALLOWED(NicId));
                    if (Found)  /* compare the remanence status too ! */
                        Found = oha_db_compare_remanence(NicId, DCP_DBID_IDENT, DCP_DSID_IDNAME, Status);
                }

                if (!Found)     /* if NAME and remananence is the same, no set of options */
                {
                    pInd->Options |= OHA_CTRL_STATION_NAME;
                    pInd->Count++;
                    if ((Status & DCP_MASK_DS_REMANENT) != 0)
                    {
                        pInd->RemaOptions |= OHA_CTRL_STATION_NAME;
                    }

                    if (oha_db_read_query_set_access(NicId, DCP_DBID_IDENT, DCP_DSID_IDNAME)) /* is there a query ? */
                    {   /* NAME is not the same, set the query options */
                        QueryOptions |= OHA_CTRL_STATION_NAME;
                    }
                }
            }
            break;
        case DCP_DSID_IDID:     /* Station's Identificator = ReadOnly */
            break;
        case DCP_DSID_IDROLE:   /* ProfinetIO Role = ReadOnly */
            break;
        case DCP_DSID_IDDEVINST:   /* DeviceInstance = ReadOnly */
            break;
        default:                    /* unknown dataset */
            pInd->Type = DsPtr->Hdr.Opt;
            pInd->SubType = DsPtr->Hdr.Id;
            break;
    }

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_ParseOptIdent(), DsPtr: 0x%X, QueryOptions:0x%X", DsPtr, QueryOptions);
    return (QueryOptions);
}


/*****************************************************************************/
/* Local function: oha_dcp_QueryOptIdent()                                   */
/* Datasets of the database "Device Identification" (8)                      */
/* of a SET-Request will be stored locally                                   */
/*****************************************************************************/
LSA_UINT32                oha_dcp_QueryOptIdent(
    LSA_UINT16              NicId,
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd)
{
    LSA_UINT16  Status;
    LSA_UINT32  QueryOptions = 0;

	OHA_ASSERT_NULL_PTR(pInd);

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_QueryOptIdent(NicId: 0x%X, DsPtr: 0x%X, DsPtr->Hdr.Id: 0x%X)",
                          NicId, DsPtr, DsPtr->Hdr.Id);

    /* remanence?   */
    Status = OHA_SWAP16(DsPtr->Status);

    switch (DsPtr->Hdr.Id)
    {
            /* Put the values of the dataset to the System geben */
        case DCP_DSID_IDTYPE:       /* Stationstype = ReadOnly */
            break;
        case DCP_DSID_IDNAME:       /* NameOfStation */
            pInd->Options |= OHA_CTRL_STATION_NAME;
            pInd->Count++;
            if (oha_db_read_query_set_access(NicId, DCP_DBID_IDENT, DCP_DSID_IDNAME)) /* is it locked ? */
            {
                QueryOptions |= OHA_CTRL_STATION_NAME;
            }
            if ((Status & DCP_MASK_DS_REMANENT) != 0)
            {
                pInd->RemaOptions |= OHA_CTRL_STATION_NAME;
            }
            break;
        case DCP_DSID_IDID:     /* Station identificator = ReadOnly */
            break;
        case DCP_DSID_IDDEVINST:   /* DeviceInstance = ReadOnly */
        case DCP_DSID_IDROLE:   /* ProfinetIO Role = ReadOnly */
            break;
        default:                    /* ununknown dataset */
            pInd->Type = DsPtr->Hdr.Opt;
            pInd->SubType = DsPtr->Hdr.Id;
            break;
    }

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_QueryOptIdent(), DsPtr: 0x%X, QueryOptions:0x%X", DsPtr, QueryOptions);
    return (QueryOptions);
}


/*****************************************************************************/
/* Local function: oha_dcp_ParseOptDhcp()                                    */
/* DHCP-options from the SET-Request will be stored locally                  */
/*****************************************************************************/
LSA_UINT32                oha_dcp_ParseOptDhcp(
    LSA_UINT16              NicId,
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd)
{
    LSA_UINT16  DsLen;
    LSA_UINT16  Status = OHA_SWAP16(DsPtr->Status);
    LSA_UINT32  QueryOptions = 0;

	OHA_ASSERT_NULL_PTR(pInd);

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_ParseOptDhcp(NicId: 0x%X, DsPtr: 0x%X, DsPtr->Hdr.Id: 0x%X)",
                          NicId, DsPtr, DsPtr->Hdr.Id);

    if (OHA_SWAP16(DsPtr->Hdr.Len) < 2)
    {
        DsLen  = 0;
    }
    else
    {
        DsLen  = (LSA_UINT16)(OHA_SWAP16(DsPtr->Hdr.Len) - 2);  /* length of Hdr.Data */
    }

    /* At minimum DHCP-Option code and length existing... */
    if (DsLen >= 2)
    {
		/* TFS 857716: check the DHCP-Option */
		if (DsPtr->Hdr.Id == 61)
		{
			if(oha_is_valid_dhcp_option(DsLen, (OHA_COMMON_MEM_U8_PTR_TYPE)&DsPtr->Data.Dhcp))
			{
				if (oha_db_read_query_set_access(NicId, DCP_DBID_DHCP, DsPtr->Hdr.Id/*DsPtr->Data.Dhcp.OptionCode*/)) /* is it locked ? */
				{
					QueryOptions |= OHA_CTRL_DHCP_OPTION;
				}
				if (Status & DCP_MASK_DS_REMANENT)
				{
					pInd->RemaOptions |= OHA_CTRL_DHCP_OPTION;
				}
				pInd->Options |= OHA_CTRL_DHCP_OPTION;
				pInd->Count++;
			}
			else
			{
			    OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "Invalid or inconsistent DHCP option 61.");
			}
		}
		else
		{
		    OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE, "DHCP options (0x%X) invalid or not supported.", DsPtr->Hdr.Id);
		}
    }
    pInd->Type = DsPtr->Hdr.Opt;
    pInd->SubType = DsPtr->Hdr.Id;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_ParseOptDhcp(), DsPtr: 0x%X, QueryOptions:0x%X", DsPtr, QueryOptions);
    return (QueryOptions);
}


/*****************************************************************************/
/* Local function: oha_dcp_ParseOptTcp()                                     */
/* Datasets of the TCP-Database of a SET-Request will be stored locally      */
/*****************************************************************************/
LSA_UINT32                oha_dcp_ParseOptTcp(
    LSA_UINT16              NicId,
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd)
{
    LSA_UINT16  Status = OHA_SWAP16(DsPtr->Status);
    LSA_UINT16  DsLen;
    LSA_UINT32  QueryOptions = 0;

	OHA_ASSERT_NULL_PTR(pInd);

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_ParseOptTcp(NicId: 0x%X, DsPtr: 0x%X, DsPtr->Hdr.Id: 0x%X)",
                          NicId, DsPtr, DsPtr->Hdr.Id);

    if (OHA_SWAP16(DsPtr->Hdr.Len) < 2)
    {
        DsLen  = 0;
    }
    else
    {
        DsLen  = (LSA_UINT16)(OHA_SWAP16(DsPtr->Hdr.Len) - 2);
    }

    switch (DsPtr->Hdr.Id)
    {
            /* Put the value of the dataset to the System */
        case DCP_DSID_MACADR:       /* MAC-Address */
            if (DsLen == OHA_MAC_ADDR_SIZE)
            {
                LSA_UINT8 AccessControl = oha_db_read_set_access(NicId,
                                                                 OHA_DCP_OPTION_TCP, DCP_DSID_MACADR); /* is it locked ? */
                if (AccessControl)
                {
                    pInd->Options |= OHA_CTRL_MAC_ADDR;
                    if ((Status & DCP_MASK_DS_REMANENT) != 0)
                    {
                        pInd->RemaOptions |= OHA_CTRL_MAC_ADDR;
                    }
                    pInd->Count++;
                    if (oha_db_read_query_set_access(NicId, OHA_DCP_OPTION_TCP, DCP_DSID_MACADR))  /* is there a query ? */
                    {
                        QueryOptions = OHA_CTRL_MAC_ADDR;
                    }
                }
            }
            break;
        case DCP_DSID_TCPIP:        /* IP-Parameter */
            if (DsLen == DCP_DS_TCPIP_SIZE)
            {
                /* write IP-Address, Subnet-mask, Default Router Address... */
                LSA_UINT8 AccessControl = oha_db_read_set_access(NicId,
                                                                 OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP); /* is it locked ? */
                if (AccessControl)
                {
                    LSA_UINT16      TcpIpStatus;

                    /* is the IP-address already the same ? */
                    OHA_COMMON_MEM_U8_PTR_TYPE pIpAddr = LSA_NULL;
                    OHA_COMMON_MEM_U8_PTR_TYPE pSubnet = LSA_NULL;
                    OHA_COMMON_MEM_U8_PTR_TYPE pRouter = LSA_NULL;
                    LSA_BOOL Found = LSA_FALSE;

                    /* read the IP-suite and compare */
                    LSA_INT  len = OHA_DB_READ_IP_SUITE(NicId, &pIpAddr, &pSubnet, &pRouter);

                    /* store at any rate, if it's the same DCP could not be responded correctly */
                    LSA_UINT16 RetVal = oha_db_store_temp_ip_address (NicId,
                                                                      DsPtr->Data.TcpIp.IpAddr,
                                                                      OHA_IP_SUITE_SIZE);
                    if (RetVal != LSA_RET_OK)
                    {
                        OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"oha_dcp_ParseOptTcp: oha_db_store_temp_ip_address failed (RetVal: 0x%X).", RetVal);
                        break;
                    }

                    /* !!! read the TCPIP_STATUS too !!! */
                    TcpIpStatus = OHA_DB_READ_TCPIP_STATUS(NicId);

                    /* do not compare IP if it's got from DHCP - even the same address must be set again */
                    if (TcpIpStatus != DCP_STATUS_TCPIP_DHCP)    /* TCP/IP Parameter not got from DHCP   */
                    {
                        /* compare IP, if it is the same -> ignore */
                        if ((len == OHA_IP_ADDR_SIZE) && OHA_IS_NOT_NULL(pIpAddr) && OHA_IS_NOT_NULL(pSubnet) && OHA_IS_NOT_NULL(pRouter))
                        {
                            Found = OHA_MEMCMP(&DsPtr->Data.TcpIp.IpAddr, pIpAddr, OHA_IP_ADDR_SIZE);
                            if (Found)
                                Found = OHA_MEMCMP(&DsPtr->Data.TcpIp.SubnetMask, pSubnet, OHA_IP_ADDR_SIZE);
                            if (Found)
                                Found = OHA_MEMCMP(&DsPtr->Data.TcpIp.DefaultRouter, pRouter, OHA_IP_ADDR_SIZE);
                            if (Found)  /* compare the remanence status too ! */
                                Found = oha_db_compare_remanence(NicId, OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP, Status);
                        }
                    }

                    if (!Found)  /* IP is not the same or is not compared, set the options */
                    {
                        /* Is remanence of the TCP/IP-parameter in the Status demanded? */
                        if ((Status & DCP_MASK_DS_REMANENT) != 0)
                        {
                            pInd->RemaOptions |= OHA_CTRL_IP_SUITE;
                        }
                        /* wrtie IP-Address, Subnet-mask, Default Router Address */
                        pInd->Options |= OHA_CTRL_IP_SUITE;
                        pInd->Count++;

                        if (oha_db_read_query_set_access(NicId, OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP)) /* is there a query ? */
                        {        /* IP is not the same, set the query */
                            QueryOptions |= OHA_CTRL_IP_SUITE;
                        }
                    }
                }
            }
            break;
        default:                    /* unknown Dataset */
            pInd->Type = DsPtr->Hdr.Opt;
            pInd->SubType = DsPtr->Hdr.Id;
            break;
    }

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_ParseOptTcp(), DsPtr: 0x%X, QueryOptions:0x%X", DsPtr, QueryOptions);
    return (QueryOptions);
}


/*****************************************************************************/
/* Local function: oha_dcp_QueryOptTcp()                                     */
/* Dataset of the TCP-Database in the SET-Request will be stored locally     */
/*****************************************************************************/
LSA_UINT32                oha_dcp_QueryOptTcp(
    LSA_UINT16              NicId,
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_UPPER_DCP_INDICATION_PTR_TYPE pInd)
{
    LSA_UINT16  Status;
    LSA_UINT16  DsLen;
    LSA_UINT32  QueryOptions = 0;

	OHA_ASSERT_NULL_PTR(pInd);

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_QueryOptTcp(NicId: 0x%X, DsPtr: 0x%X, DsPtr->Hdr.Id: 0x%X)",
                          NicId, DsPtr, DsPtr->Hdr.Id);

    if (OHA_SWAP16(DsPtr->Hdr.Len) < 2)
    {
        DsLen  = 0;
    }
    else
    {
        DsLen  = (LSA_UINT16)(OHA_SWAP16(DsPtr->Hdr.Len) - 2);
    }

    switch (DsPtr->Hdr.Id)
    {
            /* put the value of the dataset to the System */
        case DCP_DSID_MACADR:       /* MAC-Address */
            if (DsLen == OHA_MAC_ADDR_SIZE)
            {
                pInd->Options |= OHA_CTRL_MAC_ADDR;
                pInd->RemaOptions |= OHA_CTRL_MAC_ADDR;
                pInd->Count++;
                if (oha_db_read_query_set_access(NicId, OHA_DCP_OPTION_TCP, DCP_DSID_MACADR))  /* is it locked ? */
                {
                    QueryOptions = OHA_CTRL_MAC_ADDR;
                }
            }
            break;
        case DCP_DSID_TCPIP:        /* IP-Parameter */
            if (DsLen == DCP_DS_TCPIP_SIZE)
            {
                /* Is remanence of the TCP/IP-parameter in the Status demanded? */
                Status = OHA_SWAP16(DsPtr->Status);
                if ((Status & DCP_MASK_DS_REMANENT) != 0)
                {
                    pInd->RemaOptions |= OHA_CTRL_IP_SUITE;
                }

                /* write IP-Address, Subnet-mask, Default Router Address */
                pInd->Options |= OHA_CTRL_IP_SUITE;
                pInd->Count++;
                if (oha_db_read_query_set_access(NicId, OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP)) /* is it locked ? */
                {
                    QueryOptions |= OHA_CTRL_IP_SUITE;
                }
            }
            break;
        default:                    /* unknown dataset */
            pInd->Type = DsPtr->Hdr.Opt;
            pInd->SubType = DsPtr->Hdr.Id;
            break;
    }

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_QueryOptTcp(), DsPtr: 0x%X, QueryOptions:0x%X", DsPtr, QueryOptions);
    return (QueryOptions);
}


/*****************************************************************************/
/* Local function: oha_dcp_free_rcv_rqb()                                    */
/* free a lower Receive-RQB for DCP                                          */
/*****************************************************************************/
LSA_VOID OHA_LOCAL_FCT_ATTR oha_dcp_free_rcv_rqb(OHA_HDB_DCP_PTR_TYPE pHDB,
                                                 OHA_DCP_LOWER_RQB_PTR_TYPE pRQB)
{
    OHA_DCP_LOWER_MEM_PTR_TYPE  MemPtr;
    LSA_UINT16                  RetVal = LSA_RET_OK;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_free_rcv_rqb(pHDB: 0x%X, pRQB: 0x%X)",
                          pHDB, pRQB);

    /* ... free data buffer of the Receive-RQB */
    MemPtr = DCP_UPPER_RQB_RECV_IND_DATA_PTR_GET(pRQB);
    OHA_DCP_FREE_LOWER_MEM(&RetVal,MemPtr,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    /* ... free address buffer of the Receive-RQB */
    MemPtr = DCP_UPPER_RQB_RECV_IND_ADDR_PTR_GET(pRQB);
    OHA_DCP_FREE_LOWER_MEM(&RetVal,MemPtr,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_DCP_FREE_LOWER_RQB(&RetVal,pRQB,pHDB->Params.pSys);
    OHA_MEM_FREE_FAIL_CHECK(RetVal);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_free_rcv_rqb(RetVal: 0x%X)", RetVal);
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_dcp_QuerySet                            +*/
/*+  Input                      OHA_DCP_LOWER_RQB_PTR_TYPE      pRQB        +*/
/*+                             OHA_HDB_DCP_PTR_TYPE            pHDB        +*/
/*+                             OHA_QUERY_ELEM_PTR_TYPE         pQueryElem  +*/
/*+                                                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB                  :    Pointer to lower RQB done                   +*/
/*+  pHDB                  :    Pointer to HDB for this Channel.            +*/
/*+  pQueryElem            :    Params for a DcpSet  / out /                +*/
/*+                                                                         +*/
/*+  Result: OHA_OK             the DCP-Query frame can be done             +*/
/*+          OHA_ERR_RESOURCE   ther's no User-Ind Rqb for Query            +*/
/*+          OHA_ERR_PROTOCOL   no DS in the frame                          +*/
/*+          OHA_ERR_SEQUENCE   a DCP-Set is not active - Reject            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: scan a DCP-Set sequence                                   +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 oha_dcp_QuerySet(
    OHA_DCP_LOWER_RQB_PTR_TYPE   pRQB,
    OHA_HDB_DCP_PTR_TYPE         pHDB,
    OHA_QUERY_ELEM_PTR_TYPE      pQueryElem)
{
    LSA_UINT16              RetVal = OHA_OK;
    LSA_INT                 i;
    LSA_INT                 ReqLen;
    LSA_INT                 ReadLen;
    DCP_USR_BYTE_PTR        ReadPtr;
    DCP_USR_DATASET_PTR     DsPtr;
    DCP_RQB_ARGS_RECV_IND DCP_UPPER_MEM_ATTR * pRecv = &pRQB->args.RecvInd;
    DCP_USR_REQ_DATA_PTR    ReqPtr = (DCP_USR_REQ_DATA_PTR)(pRecv->DataPtr);
    LSA_INT                 RestLen;

    LSA_UINT16              NICId = pHDB->Params.pDetail->Params.Dcp.NICId;
    LSA_UINT16              ElemSize;
    OHA_DB_ELEM_PTR_TYPE    pElemList; /* array (list) of DCP elements */
    OHA_COMMON_MEM_U8_PTR_TYPE pBuffer;
    LSA_UINT16              WriteLength = 0;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_QuerySet(pRQB: 0x%X,pHDB: 0x%X, pQueryElem: 0x%X)",
                          pRQB, pHDB, pQueryElem);

    OHA_ASSERT_NULL_PTR(pQueryElem);

    /* init the options     */
    ElemSize = pQueryElem->Count;
    pQueryElem->Count = 0;
    pElemList = pQueryElem->pElements; /* array (list) of DCP elements */
    OHA_ASSERT_NULL_PTR(pElemList);
    pBuffer = pQueryElem->pVarBuffer;

    /* check Request: at minimum one dataset existing */
    if (pRecv->OptCnt > 0)
    {
        LSA_UINT8               DbaseID;

        DsPtr = &ReqPtr->Set.Dataset;
        ReadPtr = (DCP_USR_BYTE_PTR)DsPtr;

        /* get length of the TLV-chain */
        for (i = 0, ReqLen = 0; i < pRecv->OptCnt; i++)
        {
            ReadLen = DCP_DS_HDR_SIZE + OHA_SWAP16(DsPtr->Hdr.Len);
            if ((ReadLen % 2) != 0)
            {
                /* jump over Fillbyte */
                ReadLen++;
            }
            ReqLen += ReadLen;
            ReadPtr += ReadLen;
            DsPtr = (DCP_USR_DATASET_PTR)ReadPtr;
        }

        RestLen   = ReqLen;
        DsPtr = &ReqPtr->Set.Dataset;
        ReadPtr = (DCP_USR_BYTE_PTR)DsPtr;

        while (RestLen > 0)
        {
            OHA_DCP_INDICATION_TYPE SetInd;

            OHA_MEMSET(&SetInd, 0, sizeof(OHA_DCP_INDICATION_TYPE));
            SetInd.Count = 0;

            ReadLen = DCP_DS_HDR_SIZE + OHA_SWAP16(DsPtr->Hdr.Len);
            if (RestLen >= ReadLen)
            {
                DbaseID = DsPtr->Hdr.Opt;

                switch (DbaseID)
                {
                    case DCP_DBID_TCP:          /* TCP/IP-Parameter 1 */
                        oha_dcp_QueryOptTcp(NICId, DsPtr, &SetInd); /* MacAddr, IpAddr, SubnetMask, DefRouterAddr */
                        break;
                    case DCP_DBID_DHCP:         /* DHCP-Parameter 3 */
                        oha_dcp_ParseOptDhcp(NICId, DsPtr, &SetInd);  /* DhcpOption */
                        break;
                    case DCP_DBID_IDENT:        /* Device Identification 2 */
                        oha_dcp_QueryOptIdent(NICId, DsPtr, &SetInd); /* StationType, StationName, StationId */
                        break;
                    case DCP_DBID_PBPARAM:      /* Profibusparameter 129 */
                        oha_dcp_ParseOptPbus(NICId, DsPtr, &SetInd); /* ProfibusProfil, ProfibusParam */
                        break;
                    case DCP_DBID_CTRL:         /* Control Option 5, not counted */
                        break;                  /* LockBegin, FlashStart */
                    default:                    /* Unknown Database */
                        break;
                }

                /* fill the pQueryElem for OHA_OPC_READ_QUERY_ELEMENTS.Req:  */
                if (SetInd.Count)   /* element found ? */
                {
                    if (pQueryElem->Count < ElemSize)
                    {
                        OHA_COMMON_MEM_U8_PTR_TYPE pDsVar = (OHA_COMMON_MEM_U8_PTR_TYPE)&DsPtr->Data;  /* variable in octets */
                        LSA_UINT16  DsLen;                  /* length of variable in octets */
                        LSA_UINT16  Status = OHA_SWAP16(DsPtr->Status);      /* remanence ? */

                        if (OHA_SWAP16(DsPtr->Hdr.Len) < 2)
                        {
                            DsLen  = 0;
                        }
                        else
                        {
                            DsLen  = (LSA_UINT16)(OHA_SWAP16(DsPtr->Hdr.Len) - 2);
                        }

						if(DbaseID == DCP_DBID_PBPARAM)
						{
							pDsVar = (OHA_COMMON_MEM_U8_PTR_TYPE)&DsPtr->Data.PbManufact.String;
							OHA_ASSERT(DsLen > sizeof(DsPtr->Data.PbManufact.OUI));
							DsLen -= (LSA_UINT16)sizeof(DsPtr->Data.PbManufact.OUI);
						}

                        OHA_MEMSET(pElemList, 0, sizeof(OHA_DB_ELEM_TYPE));
                        pElemList->Type     = DsPtr->Hdr.Opt;
                        pElemList->SubType  = DsPtr->Hdr.Id;
                        pElemList->Remanent = ((Status & DCP_MASK_DS_REMANENT) == 0) ? LSA_FALSE : LSA_TRUE;

                        if (OHA_IS_NOT_NULL(pBuffer))       /* the global provided user-buffer */
                        {
                            LSA_UINT16 Length = DsLen;
                            if (Length & 1) Length++;       /* even address */
                            WriteLength += Length;
                            if (OHA_IS_NOT_NULL(pDsVar) && DsLen && (WriteLength <= OHA_MAX_TLV_DATA_SIZE))
                            {
                                pElemList->pVariable = pBuffer; /* first entry in pBuffer */
                                pElemList->Length    = DsLen;
                                OHA_MEMCPY(pElemList->pVariable, pDsVar, pElemList->Length);
                                pBuffer += Length;
                            }
                        }
                        pElemList++;
                    }
                    pQueryElem->Count++;
                }
            }
            else
            {                   /* remaining length toot short for a further dataset */
            }

            /* further dataset in the Request existing? */
            if ((ReadLen % 2) != 0)
            {
                /* jump over the Fillbyte */
                ReadLen++;
            }
            RestLen -= ReadLen;
            if (RestLen != 0)
            {
                ReadPtr += ReadLen;
                DsPtr = (DCP_USR_DATASET_PTR)ReadPtr;
            }
        }
    }
    else
    {
        RetVal = OHA_ERR_PROTOCOL;
        OHA_PROGRAM_TRACE_00(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_UNEXP,"No Dataset from Ethenet demanded!");
    }
    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(pHDB->Params.pDetail->Params.Dcp.NICId), LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_QuerySet(RetVal: 0x%X)",RetVal);
    return(RetVal);
}

/*****************************************************************************/
/* Local function: oha_dcp_HelloOptTcp()                                     */
/* Dataset of the TCP-Database in the SET-Request will be stored locally     */
/*****************************************************************************/
LSA_UINT8               oha_dcp_HelloOptTcp(
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_UPPER_HELLO_PTR_TYPE pInd)
{
    LSA_UINT16  DsLen;
    LSA_UINT8   Result = DCP_RESULT_NO_DATASET;

    OHA_ASSERT_NULL_PTR(pInd);

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_HelloOptTcp(DsPtr: 0x%X, pInd: 0x%X)", DsPtr, pInd);

    OHA_ASSERT_NULL_PTR(DsPtr);

    if (OHA_SWAP16(DsPtr->Hdr.Len) < 2)
    {
        DsLen  = 0;
    }
    else
    {
        DsLen  = (LSA_UINT16)(OHA_SWAP16(DsPtr->Hdr.Len) - 2);
    }

    switch (DsPtr->Hdr.Id)
    {
            /* put values of the Dataset to the System */
        case DCP_DSID_MACADR:       /* MAC-Address */
            if (DsLen == OHA_MAC_ADDR_SIZE)
            {
                OHA_MEMCPY(pInd->MacAddr, &DsPtr->Data.MacAddr.Addr[0], OHA_MAC_ADDR_SIZE);
                Result =  DCP_RESULT_OK;
            }
            break;

        case DCP_DSID_TCPIP:        /* IP-Parameter */
            if (DsLen == DCP_DS_TCPIP_SIZE)
            {
                OHA_MEMCPY(pInd->IPAddr, &DsPtr->Data.TcpIp.IpAddr, OHA_IP_ADDR_SIZE);
                OHA_MEMCPY(pInd->SubnetMask, &DsPtr->Data.TcpIp.SubnetMask, OHA_IP_ADDR_SIZE);
                OHA_MEMCPY(pInd->DefaultRouterAddr, &DsPtr->Data.TcpIp.DefaultRouter, OHA_IP_ADDR_SIZE);
                Result =  DCP_RESULT_OK;
            }
            break;

        default:                    /* unknown dataset */
            Result =  DCP_RESULT_OK;
            break;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_HelloOptTcp(), DsPtr: 0x%X, Result:0x%X", DsPtr, Result);
    return(Result);
}

/*****************************************************************************/
/* Local function: oha_dcp_HelloOptIdent()                                   */
/* Dataset of the Database "Device Identification" (8)                       */
/* in a SET-Request will be stored locally                                   */
/*****************************************************************************/
LSA_UINT8               oha_dcp_HelloOptIdent(
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_UPPER_HELLO_PTR_TYPE pInd)
{
    LSA_UINT16  DsLen;
    LSA_UINT16  DataLen = 0;
    LSA_UINT8   Result;

    OHA_ASSERT_NULL_PTR(pInd);

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_HelloOptIdent(DsPtr: 0x%X, pInd: 0x%X)", DsPtr, pInd);

    OHA_ASSERT_NULL_PTR(DsPtr);

    if (OHA_SWAP16(DsPtr->Hdr.Len) < 2)
    {
        DsLen = 0;
    }
    else
    {
        DsLen = (LSA_UINT16)(OHA_SWAP16(DsPtr->Hdr.Len) - 2);
    }

    /* Todo: no check necessary, if output function will be changed to LSA_UINT16 */
    if (DsLen < 256)
        DataLen = DsLen;

    switch (DsPtr->Hdr.Id)
    {
            /* put the values of a dataset to the System */
        case DCP_DSID_IDTYPE:       /* Stationstyp */
		    Result =  DCP_RESULT_OK;
            break;

        case DCP_DSID_IDNAME:       /* Stationsname */
            if (DsLen > 0)
            {
                pInd->pStationName   = DsPtr->Data.IdentName.String;
                pInd->StationNameLen = DataLen;
            }
	        Result =  DCP_RESULT_OK;
            break;

        case DCP_DSID_IDID:     /* station identifier */
        case DCP_DSID_IDDEVINST: /* devcie instance */
        case DCP_DSID_IDROLE:   /* ProfinetIO Role = ReadOnly */
        case DCP_DSID_IDOPTS:   /* IDENT: Option list (5)     */
	        Result =  DCP_RESULT_OK;
            break;

        default:                    /* unknown Dataset */
	        Result =  DCP_RESULT_OK;
            break;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_HelloOptIdent(), DsPtr: 0x%X, Result:0x%X", DsPtr, Result);
    return(Result);
}


/*****************************************************************************/
/* Local function: oha_dcp_HelloOptDevInit()                                 */
/* Datasets of the TCP-Database in a SET-Request will be stored loclally     */
/*****************************************************************************/
LSA_UINT8               oha_dcp_HelloOptDevInit(
    DCP_USR_DATASET_PTR     DsPtr,
    OHA_COMMON_MEM_U16_PTR_TYPE DevInit)
{
    LSA_UINT16  DsLen;
    LSA_UINT8   Result = DCP_RESULT_NO_DATASET;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_dcp_HelloOptDevInit(DsPtr: 0x%X, DevInit: 0x%X)", DsPtr, DevInit);

    OHA_ASSERT_NULL_PTR(DsPtr);

    *DevInit = OHA_DCP_HELLO_OFF;

    if (OHA_SWAP16(DsPtr->Hdr.Len) < 2)
    {
        DsLen = 0;
    }
    else
    {
        DsLen = (LSA_UINT16)(OHA_SWAP16(DsPtr->Hdr.Len) - 2);
    }

    switch (DsPtr->Hdr.Id)
    {
            /* put values of a Dataset to the System */
        case DCP_DSID_DEVINIT:      /* Device Initiative Suboption */
            if (DsLen == DCP_DS_DEVINIT_SIZE)
            {
                *DevInit = OHA_SWAP16(DsPtr->Data.DevInit.Value);
                Result =  DCP_RESULT_OK;
            }
            break;

        default:                    /* unknown Dataset */
	        Result =  DCP_RESULT_OK;
            break;
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_dcp_HelloOptDevInit(), DsPtr: 0x%X, Result:0x%X", DsPtr, Result);
    return(Result);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DcpGetReason                            +*/
/*+  Input/Output          :    LSA_UINT16       Reason                     +*/
/*+  Result                :    LSA_UINT16       RetVal                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks if reason is valid, if not -> fatal is called      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_DcpGetReason(
    LSA_UINT8 Reason)
{
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "IN : OHA_DcpGetReason(Reason: 0x%X)", Reason);

    switch (Reason)
    {
        case OHA_DCP_REASON_OK: /* Positive response, Parameter delivered and accepted  */
            RetVal = OHA_OK;
            break;
        case OHA_DCP_REASON_NO_DATABASE:    /* Option not supported */
        case OHA_DCP_REASON_NO_DATASET:     /* Suboption not supported or no dataset available */
        case OHA_DCP_REASON_NO_RESOURCE:    /* (temporary) resource error (server) */
            RetVal = OHA_ERR_RESOURCE;
            break;
        case OHA_DCP_REASON_DS_NOT_SET:     /* default value, suboption not set (local reasons) */
        case OHA_DCP_REASON_NOT_POSSIBLE:   /* SET not possible (local reasons) */
            RetVal = OHA_ERR_FAILED;
            break;
        case OHA_DCP_REASON_NOT_APPLICABLE: /* SET not possible (application operation) */
            RetVal = OHA_ERR_USER_LOCK;
            break;
        default:
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_DcpGetReason(Reason: 0x%X) - wrong Reason !", Reason);
            RetVal = OHA_ERR_PROTOCOL;
            OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
            break;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DcpGetReason(RetVal:  0x%X)", RetVal);
    return(RetVal);
}

/*------------------------------------------------------------------------------
// validate DHCP option 61 
     *
     * syntax:
     *
     * Code = 61		valid
     * Len  = 0	        not valid, is there a "DHCP client identifier (off)" by Len=0?
     * Type = 0 		valid for Len = 1 (NoS) and for Len > 1 (ClientID)
     * Type = 1			only valid for Len = 1 (MAC)
     *
	 * use MAC:
	 * 0000   3d 01  01  cd
	 * use NoS:
	 * 0000   3d 01  00  cd
	 * use ClientID:
	 * 0000   3d 0b  00  6d 79 43 6c 69 65 6e 74 49 44 cd
     *
//----------------------------------------------------------------------------*/
LSA_BOOL  OHA_LOCAL_FCT_ATTR  oha_is_valid_dhcp_option(
    LSA_UINT16 Length,
    OHA_COMMON_MEM_U8_PTR_TYPE pOption
)
{
	LSA_UINT8			DhcpCode;		/* ID of the DHCP option of list		*/
	LSA_UINT8			DhcpLen;		/* Length of the DHCP option of list	*/
	LSA_UINT8			DhcpType;		/* Type of the DHCP option of list		*/

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : oha_is_valid_dhcp_option(length: 0x%X, data: 0x%X)", Length, pOption);

	/* The code for this option is 61, and its minimum length is 2. */
	OHA_ASSERT_NULL_PTR(pOption);
	OHA_ASSERT(Length >= 2); 	/* At minimum DHCP-Option code and length existing... */

	DhcpCode = *pOption;
	pOption++;
	DhcpLen  = *pOption;
	
	/*    Code   Len   Type  Client-Identifier   */
	/*    +-----+-----+-----+-----+-----+---     */
	/*    |  61 |  n  |  t1 |  i1 |  i2 | ...    */
	/*    +-----+-----+-----+-----+-----+---     */

	if (DhcpCode != 61)
	{
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_is_valid_dhcp_option(invalid DHCP Code %x): LSA_FALSE", DhcpCode);
        return LSA_FALSE;
    }
	
	if(DhcpLen == 0) /* note: must not test for >= 2 for historical reasons */
	{
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_is_valid_dhcp_option(invalid DHCP Len %x): LSA_FALSE",DhcpLen);
        return LSA_FALSE;
    }

	if (1/*Code*/ + 1/*Len*/ + DhcpLen > Length) /* note: must not test for equality for historical reasons */
	{
        OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_is_valid_dhcp_option(DHCP Len %x not fitting into DCP, Len %x): LSA_FALSE", DhcpLen, Length);
        return LSA_FALSE;
    }

	pOption++;
	DhcpType = *pOption;
	if (DhcpType > 1)
	{
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_is_valid_dhcp_option(invalid DHCP Type %x): LSA_FALSE", DhcpType);
        return LSA_FALSE;
    }
	if (DhcpType == 1 && DhcpLen != 1)
	{
        OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "OUT: oha_is_valid_dhcp_option(invalid DHCP Type %x with Len %x): LSA_FALSE", DhcpType, DhcpLen);
        return LSA_FALSE;
    }

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "OUT: oha_is_valid_dhcp_option(): LSA_TRUE");
    return LSA_TRUE; /* ok */
}

/*****************************************************************************/
/*  end of file OHA_DCP2.C                                                    */
/*****************************************************************************/

