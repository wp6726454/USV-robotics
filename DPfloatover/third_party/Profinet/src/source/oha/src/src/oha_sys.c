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
/*  F i l e               &F: oha_sys.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  OHA-system functions                             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  3
#define OHA_MODULE_ID      LTRC_ACT_MODUL_ID /* OHA_MODULE_ID_OHA_SYS */

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

OHA_DATA_TYPE        OHA_Data;

/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_FreeMgm                                 +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    OHA_OK                                      +*/
/*+                             OHA_ERR_SEQUENCE                            +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees OHA Managmenent if allocated.                       +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_FreeMgm(LSA_VOID)
{
    LSA_UINT16 Response = OHA_OK;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "IN : OHA_FreeMgm()");
    {
        /* ----------------------------------------------------------------------*/
        /* free Timer if present                                                 */
        /* ----------------------------------------------------------------------*/
#ifndef OHA_CFG_NO_NARE
        if ( OHA_GET_GLOB_PTR()->CyclicTimerID != OHA_TIMER_ID_INVALID )
        {
            Response = OHA_TimerFree(OHA_GET_GLOB_PTR()->CyclicTimerID);

            /* Note: We dont expect the Timer RQB to be underway ! */
            if ( Response != OHA_OK )
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_FreeMgm. Cyclic Timer-free (0x%X)",Response);
                OHA_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
            }

            OHA_GET_GLOB_PTR()->CyclicTimerID = OHA_TIMER_ID_INVALID;
        }
#endif

    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_FreeMgm(), Response: 0x%X",
                          Response);
    return(Response);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_init                                    +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    OHA_RESP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      OHA_OK                                                    +*/
/*+               OHA_ERR_PARAM                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initializes the OHA.                                      +*/
/*+                                                                         +*/
/*+               Note: Must only be called if not already initialized, not +*/
/*+                     checked!                                            +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_init(LSA_VOID)
{
    LSA_UINT16             Status = OHA_OK;
    LSA_UINT16             i,j;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "IN : oha_init()");

    /* ----------------------------------------------------------------------*/
    /* If memory should be static we use global memory.                      */
    /* ----------------------------------------------------------------------*/
    OHA_MEMSET(&OHA_Data,0,sizeof(OHA_Data));

    /* --------------------------------------------------------------------------*/
    /* Setup management                                                          */
    /* --------------------------------------------------------------------------*/

    if (Status == OHA_OK )
    {
        /* init the OHA-database */
        OHA_MEMSET(&oha_database, 0, sizeof(oha_database));

        OHA_GET_GLOB_PTR()->UsedUserCnt   = 0;                     /* User-Channels in use */
        OHA_GET_GLOB_PTR()->CyclicTimerID = OHA_TIMER_ID_INVALID;
        OHA_GET_GLOB_PTR()->IfNicID       = OHA_INVALID_NIC_ID;	/* init. global if-nic invalid */

        for ( i=0; i<OHA_CFG_MAX_CHANNELS; i++)
        {
            OHA_GET_HUSER_PTR(i)->State      = OHA_HANDLE_STATE_FREE;
            OHA_GET_HUSER_PTR(i)->ThisHandle = (LSA_HANDLE_TYPE) (i | OHA_HANDLE_TYPE_USER);
		    OHA_GET_HUSER_PTR(i)->TopoState  = OHA_TOPO_STATE_CLOSE;

            for ( j=0; j<OHA_CFG_MAX_PORTS; j++)
            {
                OHA_GET_HUSER_PTR(i)->PtcpTimerID[j] = OHA_TIMER_ID_INVALID;
            }
        }

        OHA_GET_GLOB_PTR()->UsedLowerCnt   = 0;                   /* Lower-Channels in use */

        for ( i=0; i<OHA_CFG_MAX_NICS; i++)    /* lower channels */
        {
            /* init. nic-id with invalid */
            oha_database.IfData[i].NicId   = OHA_INVALID_NIC_ID;

            OHA_GET_HEDD_PTR(i)->State       = OHA_HANDLE_STATE_FREE;
            OHA_GET_HEDD_PTR(i)->ThisHandle  = (LSA_HANDLE_TYPE) (i | OHA_HANDLE_TYPE_EDD);

            for ( j=0; j<OHA_CFG_MAX_PORTS; j++)
            {
                /* EDD_SET_REMOTE_PORT_STATE_IGNORE means: nothing said to EDD */
                OHA_GET_HEDD_PTR(i)->LastRemotePortState[j].PortID = (LSA_UINT16)(j+1);
                OHA_GET_HEDD_PTR(i)->LastRemotePortState[j].RtClass2_TopoState = EDD_SET_REMOTE_PORT_STATE_IGNORE;
                OHA_GET_HEDD_PTR(i)->LastRemotePortState[j].RtClass3_RemoteState = EDD_SET_REMOTE_PORT_STATE_IGNORE;
                OHA_GET_HEDD_PTR(i)->LastRemotePortState[j].RtClass3_TopoState = EDD_SET_REMOTE_PORT_STATE_IGNORE;
                OHA_GET_HEDD_PTR(i)->LastRemotePortState[j].SyncId0_TopoState  = EDD_SET_REMOTE_PORT_STATE_IGNORE;
            }

            OHA_GET_HLLDP_PTR(i)->State      = OHA_HANDLE_STATE_FREE;
            OHA_GET_HLLDP_PTR(i)->ThisHandle = (LSA_HANDLE_TYPE) (i | OHA_HANDLE_TYPE_LLDP);

            OHA_GET_HDCP_PTR(i)->DcpState    = OHA_HANDLE_STATE_FREE;
            OHA_GET_HDCP_PTR(i)->ThisHandle  = (LSA_HANDLE_TYPE) (i | OHA_HANDLE_TYPE_DCP);
#ifndef OHA_CFG_NO_NARE
            OHA_GET_HNARE_PTR(i)->State      = OHA_HANDLE_STATE_FREE;
            OHA_GET_HNARE_PTR(i)->ThisHandle = (LSA_HANDLE_TYPE) (i | OHA_HANDLE_TYPE_NARE);
#endif
#ifndef OHA_CFG_NO_MRP
            {
                LSA_UINT16   k;
                OHA_GET_HMRP_PTR(i)->State      = OHA_HANDLE_STATE_FREE;
                OHA_GET_HMRP_PTR(i)->ThisHandle = (LSA_HANDLE_TYPE) (i | OHA_HANDLE_TYPE_MRP);
                for ( k=0; k<EDD_CFG_MAX_MRP_INSTANCE_CNT; k++)
                {
                    OHA_GET_HMRP_PTR(i)->MrpStateInd[k].topoCtrlMode = MRP_USR_ENUM_TOPO_CTRL_MAX; /* for Topo-Diag */
                }
			}
#endif
		}

        {               /* only one SOCK-channel supported! */
            OHA_GET_HSOCK_PTR()->State      = OHA_HANDLE_STATE_FREE;
            OHA_GET_HSOCK_PTR()->ThisHandle = (LSA_HANDLE_TYPE) (0 | OHA_HANDLE_TYPE_SOCK);

			/* init agents */
			OHA_GET_HSOCK_PTR()->LldpAgent.SnmpHandle = SOCK_NO_HANDLE;
			OHA_GET_HSOCK_PTR()->LldpAgent.RequestState = OHA_SNMP_STATE_STOP;
			OHA_GET_HSOCK_PTR()->LldpAgent.RxPendingCnt = 0; /* no Rx pending within SOCK */

			OHA_GET_HSOCK_PTR()->MrpAgent.SnmpHandle = SOCK_NO_HANDLE;
			OHA_GET_HSOCK_PTR()->MrpAgent.RequestState = OHA_SNMP_STATE_STOP;
			OHA_GET_HSOCK_PTR()->MrpAgent.RxPendingCnt = 0; /* no Rx pending within SOCK */
        }

#ifdef OHA_CFG_USE_SNMPX               /* only one SNMPX-channel supported! */
        OHA_GET_HSNMPX_PTR()->SnmpxState = OHA_HANDLE_STATE_FREE;
        OHA_GET_HSNMPX_PTR()->ThisHandle = (LSA_HANDLE_TYPE) (0 | OHA_HANDLE_TYPE_SNMPX);
#endif

        /* init the DEFVALs of the writeable MIB objects */
        Status = oha_init_defvals(LSA_NULL); /* all if's */

#ifndef OHA_CFG_NO_REMA
        oha_rema_init();
#endif
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_init(), Status: 0x%X",
                          Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_init_defvals                            +*/
/*+  Input/Output          :    OHA_DB_IF_PTR_TYPE pDB                      +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      OHA_OK                                                    +*/
/*+               OHA_ERR_SEQUENCE                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Inizializes the DEFVALs for the writeable MIB objects     +*/
/*+               pDB =  LSA_NULL:  init all interfaces                     +*/
/*+               pDB <> LSA_NULL:  init this interfaces                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR  oha_init_defvals(OHA_DB_IF_PTR_TYPE pDB)
{
    LSA_UINT16             Status = OHA_OK;
    LSA_UINT16             i,j;
	OHA_OID_LLDP_CONFIG_PTR_TYPE pDbLldpConfig = &oha_database.GlobalConfig;
	OHA_DB_IF_PTR_TYPE     pDBIf = pDB;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "IN : oha_init_defvals(pDB: 0x%X)", pDB);

	/* default values and access: see LSA_OHA_MIBs_Basic_Design_101R.doc */
	/* TxInterval:       Integer32 (5..32768 )DEFVAL { 30 }, PNIO DefVal { 5 }  */
	/* TxHoldMultiplier: Integer32 (2..10 ),  DEFVAL { 4 }   */
	/* ReinitDelay:      Integer32 (1..10 ),  DEFVAL { '0' } */
	/* TxDelay:          Integer32 (1..8192 ), DEFVAL { 2 }  */
	OHA_DB_SET_MIB_OBJECT(&pDbLldpConfig->TxInterval, LLDP_DEFAULT_MSG_DELAY, sizeof(LSA_UINT32), OHA_READ_ONLY);
	OHA_DB_SET_MIB_OBJECT(&pDbLldpConfig->TxHoldMultiplier, OHA_LLDP_TTL_FACTOR, sizeof(LSA_UINT32), OHA_READ_ONLY);
	OHA_DB_SET_MIB_OBJECT(&pDbLldpConfig->LldpReinitDelay, LLDP_DEFAULT_TX_REINIT_DELAY, sizeof(LSA_UINT32), OHA_READ_ONLY);
	OHA_DB_SET_MIB_OBJECT(&pDbLldpConfig->LldpTxDelay, LLDP_DEFAULT_TX_DELAY, sizeof(LSA_UINT32), OHA_READ_ONLY);
	OHA_DB_SET_MIB_OBJECT(&pDbLldpConfig->NotificationInterval, 5, sizeof(LSA_UINT32), OHA_NO_ACCESS);

    for ( i=0; i<OHA_CFG_MAX_NICS; i++)
    {
		if(OHA_IS_NULL(pDB))
		{
			pDBIf = &oha_database.IfData[i];
		}
		if(OHA_ARE_EQUAL(pDBIf, &oha_database.IfData[i]))
		{
			for ( j=0; j<OHA_CFG_MAX_PORTS; j++)
			{
				OHA_OID_PORT_CONFIG_PTR_TYPE pPortConfig = &pDBIf->LldpData[j].PortConfig;

				/* lldpPortConfigAdminStatus, DEFVAL { txAndRx } */
				OHA_DB_SET_MIB_OBJECT(&pPortConfig->AdminStatus, OHA_LLDP_SEND_RECV, sizeof(LSA_UINT32), OHA_READ_ONLY);

				/* lldpPortConfigNotificationEnable: DEFVAL { false } */
				OHA_DB_SET_MIB_OBJECT(&pPortConfig->NotificationEnable, OHA_LLDP_TX_DISABLE, sizeof(LSA_UINT32), OHA_READ_ONLY);

				/* lldpPortConfigTLVsTxEnable, DEFVAL {   }, default = True, BITS portDesc(0),sysName(1),sysDesc(2),sysCap(3)  */
				OHA_DB_SET_MIB_OBJECT(&pPortConfig->TLVsTxEnable, OHA_LLDP_TLVS_TX_ENABLE, sizeof(LSA_UINT8), OHA_READ_WRITE); /* bit 'portDesc(0)' */

				/* lldpConfigManAddrPortsTxEnable, DEFVAL { ''H } */
				OHA_DB_SET_MIB_OBJECT(&pPortConfig->ManAddrTxEnable, OHA_LLDP_TX_ENABLE, (OHA_CFG_MAX_PORTS + 7) / 8, OHA_READ_ONLY);  /* A set of ports... */

				/* lldpXPnoConfigSPDTxEnable, DEFVAL { true } */
				OHA_DB_SET_MIB_OBJECT(&pPortConfig->PnoDelayTxEnable, OHA_LLDP_TX_ENABLE, sizeof(LSA_UINT32), OHA_READ_ONLY);
				/* lldpXPnoConfigPortStatusTxEnable, DEFVAL { true } */
				OHA_DB_SET_MIB_OBJECT(&pPortConfig->PnoPortStatusTxEnable, OHA_LLDP_TX_ENABLE, sizeof(LSA_UINT32), OHA_READ_ONLY);
				/* lldpXPnoConfigAliasTxEnable, DEFVAL { true } */
				OHA_DB_SET_MIB_OBJECT(&pPortConfig->PnoAliasTxEnable, OHA_LLDP_TX_ENABLE, sizeof(LSA_UINT32), OHA_READ_ONLY);
				/* lldpXPnoConfigMrpTxEnable, DEFVAL { true } */
				OHA_DB_SET_MIB_OBJECT(&pPortConfig->PnoMrpTxEnable, OHA_LLDP_TX_ENABLE, sizeof(LSA_UINT32), OHA_READ_ONLY);
				/* lldpXPnoConfigPtcpTxEnable, DEFVAL { true } */
				OHA_DB_SET_MIB_OBJECT(&pPortConfig->PnoPtcpTxEnable, OHA_LLDP_TX_ENABLE, sizeof(LSA_UINT32), OHA_READ_ONLY);

				/* lldpXdot3PortConfigTLVsTxEnable, DEFVAL {   }, BITS macPhyConfigStatus(0),powerViaMDI(1),linkAggregation(2),maxFrameSize(3) */
				OHA_DB_SET_MIB_OBJECT(&pPortConfig->Dot3TLVsTxEnable, OHA_DOT3_TLVS_TX_ENABLE, sizeof(LSA_UINT8), OHA_READ_ONLY); /* bit 'macPhyConfigStatus(0)' */

				/* AP01319671 (writeable LLDP-MIB): wrong static values */
				/* lldpXdot1ConfigPortVlanTxEnable, DEFVAL { false } */ 
				OHA_DB_SET_MIB_OBJECT(&pPortConfig->Dot1VlanIdTxEnable, OHA_LLDP_TX_DISABLE, sizeof(LSA_UINT32), OHA_READ_ONLY);
				/* lldpXdot1ConfigVlanNameTxEnable, DEFVAL { false } */
				OHA_DB_SET_MIB_OBJECT(&pPortConfig->Dot1VlanNameTxEnable, OHA_LLDP_TX_DISABLE, sizeof(LSA_UINT32), OHA_READ_ONLY);
				/* lldpXdot1ConfigProtoVlanTxEnable, DEFVAL {   } */
				OHA_DB_SET_MIB_OBJECT(&pPortConfig->Dot1PortProtoVlanTxEnable, OHA_LLDP_TX_DISABLE, sizeof(LSA_UINT32), OHA_READ_ONLY);
				/* lldpXdot1ConfigProtocolTxEnable, DEFVAL {   } */
				OHA_DB_SET_MIB_OBJECT(&pPortConfig->Dot1ProtocolTxEnable, OHA_LLDP_TX_DISABLE, sizeof(LSA_UINT32), OHA_READ_ONLY);
			}
		}
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_init_defvals(), Status: 0x%X", Status);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_init_stats_info                         +*/
/*+  Input/Output          :    OHA_OID mib_node                            +*/
/*+  Result                :    -                                           +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      OHA_OK                                                    +*/
/*+               OHA_ERR_SEQUENCE                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initialization for the writeable MIB objects              +*/
/*+  mib_node: OHA_LLDP_MIB_NODE (8802) or OHA_MRP_MIB_NODE (62439)         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_SYSTEM_IN_FCT_ATTR  oha_init_stats_info(OHA_OID mib_node)
{
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "IN : oha_init_stats_info(MibNode: 0x%X)", mib_node);

	switch(mib_node)
	{
		case OHA_LLDP_MIB_NODE:
			oha_snmp_delete_lldp_statistics();
		break;
		case OHA_MRP_MIB_NODE:
#ifndef OHA_CFG_NO_MRP
#ifndef OHA_CFG_NO_MRP_MIB
			oha_snmp_delete_mrp_statistics();
#endif
#endif
		break;
		default:	/* not supported */
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"oha_init_stats_info: mib_node (0x%X) not supported!", mib_node);
	        OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ);
		break;
	}

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_init_stats_info()");
    return;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_undo_init                               +*/
/*+  Input/Output          :    ---                                         +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Result:      OHA_OK                                                    +*/
/*+               OHA_ERR_SEQUENCE                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Uninizializes OHA.                                        +*/
/*+               This is only possible if no device is open                +*/
/*+               OHA_ERR_SEQUENCE if device is open.                       +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR  oha_undo_init(LSA_VOID)
{
    LSA_UINT16  Status;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "IN : oha_undo_init()");

    if ( OHA_GET_GLOB_PTR()->UsedUserCnt || OHA_GET_GLOB_PTR()->UsedLowerCnt )
    {
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"oha_undo_init(). Not all channels closed yet.");
        Status = OHA_ERR_SEQUENCE;
    }
    else
    {
        Status = OHA_FreeMgm();
    }

    OHA_FreeDataBase();

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_undo_init(), Status: 0x%X",
                          Status);

    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_UserGetFreeHDB                          +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE OHA_LOCAL..pHDBUser   +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA user -channel handle to get pointer to HDB            +*/
/*+                                                                         +*/
/*+  Result     : OHA_OK                                                    +*/
/*+               OHA_ERR_RESOURCE                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free User-HDB                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_UserGetFreeHDB(
    OHA_HDB_USER_PTR_TYPE OHA_LOCAL_MEM_ATTR *pHDBUser)
{
    LSA_UINT16                Status  = OHA_OK;
    LSA_BOOL                  Found   = LSA_FALSE;
    OHA_HDB_USER_PTR_TYPE     pHDB    = OHA_GET_HUSER_PTR(0);
    LSA_UINT32                i       =   0;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_UserGetFreeHDB(pHDBUser: %Xh)",
                          pHDBUser);

    /* --------------------------------------------------------------------------*/
    /* Search for free handle-management                                         */
    /* --------------------------------------------------------------------------*/

    while ( (! Found)  &&  ( i < OHA_CFG_MAX_CHANNELS))
    {
        if ( pHDB->State == OHA_HANDLE_STATE_FREE )
        {
            Found = LSA_TRUE;
        }
        else
        {
            i++;
            pHDB++;
        }
    }

    /* --------------------------------------------------------------------------*/
    /* If a free handle management was found we initialize it..                  */
    /* --------------------------------------------------------------------------*/

    if ( Found )
    {
        /* HDB-entry of the UserCnt */
        OHA_GET_GLOB_PTR()->UsedUserCnt++;
        pHDB->State = OHA_HANDLE_STATE_CLOSED;
        for (i=0; i<OHA_CFG_MAX_PORTS; i++)
        {
            pHDB->PtcpTimerID[i] = OHA_TIMER_ID_INVALID;
        }
        *pHDBUser = pHDB;
    }
    else
    {
        Status = OHA_ERR_RESOURCE;
        OHA_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"OHAGetFreeUserHDB(). No more free handles.");
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_UserGetFreeHDB(Status: 0x%X, pHDB: 0x%X )",
                          Status,*pHDBUser);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_UserReleaseHDB                         +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE pHDBUser             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBUser   : Pointer to User Handle to release                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Release User-HDB                                          +*/
/*+               Release memory if needed.                                 +*/
/*+               Sets Handle-state to OHA_HANDLE_STATE_FREE               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID OHA_LOCAL_FCT_ATTR OHA_UserReleaseHDB(
    OHA_HDB_USER_PTR_TYPE pHDB)
{
    LSA_UINT32  i;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_UserReleaseHDB(pHDB: 0x%X)",
                          pHDB);

    OHA_ASSERT_NULL_PTR(pHDB);

    /* do a Topo-Stop if running */
    if (pHDB->TopoState == OHA_TOPO_STATE_START) /* channel is not registered ? */
    {
        OHA_InitTopo(pHDB);

        /* no more port(s) for topo check */
        pHDB->TopoState = OHA_TOPO_STATE_STOP; /* no checking of topo data  */
    }

    /* cancel all pending user-indication resources for TOPO */
    for (i = 0; i <= OHA_CFG_MAX_PORTS; i++)
    {
        while (pHDB->TopoIndReq[i].pBottom)
        {
            OHA_UPPER_RQB_PTR_TYPE   pRQBHelp;

            OHA_RQB_REM_BLOCK_BOTTOM(pHDB->TopoIndReq[i].pBottom,
                                     pHDB->TopoIndReq[i].pTop,
                                     pRQBHelp);
            pHDB->TopoIndReqPending--;

            OHA_UserRequestFinish(pHDB,pRQBHelp,OHA_OK_CANCEL);
        }
    }

    /* free the db for topo */
    OHA_FreeAllTopoDBs(pHDB);

    /* unlock the db for this user */
    OHA_UnlockDbUser(pHDB);

    for (i=0; i<OHA_CFG_MAX_PORTS; i++)
    {
        if ( pHDB->PtcpTimerID[i] != OHA_TIMER_ID_INVALID )
        {
            LSA_UINT16 Response;

            /* stop the PTCP timer if running */
            OHA_TimerStop(pHDB->PtcpTimerID[i]);

            Response = OHA_TimerFree(pHDB->PtcpTimerID[i]);

            /* Note: We dont expect the Timer RQB to be underway ! */
            if ( Response != OHA_OK )
            {
                OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,"OHA_FreeMgm. Timer-free (0x%X)",Response);
                OHA_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
            }

            pHDB->PtcpTimerID[i] = OHA_TIMER_ID_INVALID;
        }
    }

    pHDB->Params.pDetail = LSA_NULL;

    pHDB->State = OHA_HANDLE_STATE_FREE;
    pHDB->pOCReq = LSA_NULL;

    pHDB->TopoState = OHA_TOPO_STATE_CLOSE; /* channel is unregistered  */
    pHDB->PrmState  = OHA_PRM_STATE_CLOSE;  /* channel is unregistered  */

    /* HDB-entry of the UserCnt */
    OHA_GET_GLOB_PTR()->UsedUserCnt--;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_UserReleaseHDB()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_UserCheckHDB                            +*/
/*+  Input/Output          :    OHA_HDB_USER_PTR_TYPE OHA_LOCAL..pHDBUser   +*/
/*+  Result                :    LSA_UINT16                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle     : OHA user -channel handle to get pointer to HDB            +*/
/*+                                                                         +*/
/*+  Result     : OHA_OK                                                    +*/
/*+               OHA_ERR_RESOURCE                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free User-HDB                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#if 0
LSA_UINT16 OHA_LOCAL_FCT_ATTR OHA_UserCheckHDB(LSA_VOID)
{
    LSA_UINT16                Status  = OHA_OK;
    OHA_HDB_USER_PTR_TYPE     pHDB    = OHA_GET_HUSER_PTR(0);
    LSA_UINT32                i       = 0;

    OHA_FUNCTION_TRACE_00(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHAUserCheckHDB"
                         );

    /* --------------------------------------------------------------------------*/
    /* Search for opened user-channels                                           */
    /* --------------------------------------------------------------------------*/
    while (i < OHA_CFG_MAX_CHANNELS)
    {
        if ( pHDB->State == OHA_HANDLE_STATE_OPEN )
        {
            if (OHA_IS_NOT_NULL(pHDB->Params.pDetail) && pHDB->Params.pDetail->Params.User.DcpSetControl)  /* controlled by User ? */
                Status = OHA_ERR_PARAM;
            break;
        }
        else
        {
            i++;
            pHDB++;
        }
    }

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHAUserCheckHDB(Status: 0x%X, pHDB: 0x%X )",
                          Status,*pHDBUser);
    return(Status);
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DB_GET_INTERFACE                        +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_UPPER_DB_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a Database-Interface-Entry of by NICId.   +*/
/*+  If none is found LSA_NULL is returned                                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
OHA_DB_IF_PTR_TYPE OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_GET_INTERFACE(
    LSA_UINT16 NICId)
{
    OHA_DB_IF_PTR_TYPE    pDB  = &oha_database.IfData[0];
    LSA_UINT32            i     = 0;
    LSA_BOOL              Found = LSA_FALSE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DBGetIndexByNIC(NICId: 0x%X)",
                          NICId);

    if (NICId != OHA_INVALID_NIC_ID)
    {
        while ( (! Found ) && ( i < OHA_CFG_MAX_NICS) )
        {
            if (pDB->NicId == NICId)
            {
                Found = LSA_TRUE;
            }
            else
            {
                pDB++;
                i++;
            }
        }
    }

    if ( ! Found ) pDB = LSA_NULL;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DBGetIndexByNIC(pHDB: 0x%X)", pDB);

    return(pDB);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    OHA_DB_GET_NIC                              +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+  Result                :    OHA_UPPER_DB_PTR_TYPE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Nic-ID to a Database-Interface-Entry by IfId.        +*/
/*+  If none is found OHA_INVALID_NIC_ID is returned                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_GET_NIC(
    LSA_UINT32 IfId)
{
    OHA_DB_IF_PTR_TYPE    pDB  = &oha_database.IfData[0];
    LSA_UINT32            i     = 0;
    LSA_UINT16            NicID = OHA_INVALID_NIC_ID;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_GET_NIC(IfId: 0x%X)",
                          IfId);

    if (IfId <= EDD_INTERFACE_ID_MAX) /* 0 is valid (global channel) */
    {
        while ( i < OHA_CFG_MAX_NICS )
        {
            if (pDB->InterfaceID == IfId)
            {
				if(pDB->NicId != OHA_INVALID_NIC_ID)
				{
					NicID = pDB->NicId;
					break;
				}
            }

            pDB++;
            i++;
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_GET_NIC(NicID: 0x%X)", NicID);
    return(NicID);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    oha_init_interface                          +*/
/*+  Input/Output          :    LSA_UINT16       NICId                      +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  NICId      : NICId from SystemPath                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Inits the NICId in a Database-Interface.                  +*/
/*+  If none is found LSA_RET_ERR_RESOURCE is returned                      +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_init_interface(
    LSA_UINT16 NICId,
    LSA_UINT16 VendorID,
    LSA_UINT16 DeviceID,
    LSA_UINT16 InstanceID,       /* used if InstanceIDValid = True */
    LSA_BOOL   InstanceIDValid,  /* True: DCP suboption InstanceID will be created, False: no suboption */ 
    LSA_UINT8  ProfinetRole,     /* DCP_PN_DEVICE, DCP_PN_CONTROLLER, etc. 0 = no role, see IEC 61158-6 DeviceRoleDetails */
    OHA_COMMON_MEM_U8_PTR_TYPE DeviceVendorValue, /* see IEC 61158-6, (former StationType) */
    LSA_UINT16 DeviceVendorLen,
	LSA_BOOL   rtf_nos_ip_read_only, /* True: initial attributes are READ_ONLY (usecase Controller), False: READ_WRITE (usecase Device) */
	OHA_COMMON_MEM_U8_PTR_TYPE NameOfDevice, /* see IEC 61158-6 MultipleInterfaceMode.NameOfDevice */
    LSA_UINT16 NameOfDeviceLen,
	LSA_UINT16 SysCapSupported, /* see IEEE 802.1AB-2009, LLDP-Type 7 "System Capabilities" (LLDP_TLV_SYSCAB_OTHER, etc.) */
	LSA_UINT16 SysCapEnabled,   /* see IEEE 802.1AB-2009, LLDP-Type 7 "System Capabilities" (LLDP_TLV_SYSCAB_OTHER, etc.) */
	OHA_COMMON_MEM_U8_PTR_TYPE SysDescr, /* System Description (MIB2 sysDescr) */
    LSA_UINT16 SysDescrLen,
	OHA_COMMON_MEM_U8_PTR_TYPE IfDescr, /* Interface Description (MIB2 ifDescr of the interface) */
    LSA_UINT16 IfDescrLen
)
{
    OHA_DB_IF_PTR_TYPE    pDB  = &oha_database.IfData[0];
    LSA_UINT32            i     = 0;
    LSA_BOOL              Found = LSA_FALSE;
    LSA_UINT16            Status = LSA_RET_ERR_RESOURCE;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_INIT_INTERFACE(NICId: 0x%X)",
                          NICId);

	if(OHA_IS_NULL(NameOfDevice) && NameOfDeviceLen != 0)
	{
		Status = OHA_ERR_PARAM;
		OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP,"oha_init_interface() failed (NameOfDevice: 0x%X, NameOfDeviceLen: 0x%X).", NameOfDevice, NameOfDeviceLen);
		return(Status);
	}
	if(OHA_IS_NULL(SysDescr) && SysDescrLen != 0)
	{
		Status = OHA_ERR_PARAM;
		OHA_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP,"oha_init_interface() failed (SysDescr: 0x%X, SysDescrLen: 0x%X).", SysDescr, SysDescrLen);
		return(Status);
	}

    if (NICId != OHA_INVALID_NIC_ID)
    {
        while ( (! Found ) && ( i < OHA_CFG_MAX_NICS) )
        {
            if (pDB->NicId == OHA_INVALID_NIC_ID)   /* free interface */
            {
                Found = LSA_TRUE;
                pDB->NicId = NICId; /* write NicId to interface */
                Status = LSA_RET_OK;
            }
            else
            {
                pDB++;
                i++;
            }
        }
    }
    else
    {
        Status = LSA_RET_ERR_PARAM;
    }

	if(Found)
	{
        LSA_UINT8   DeviceIDValue[4];

		/* set NameOfStation default empty */
		Status = OHA_DB_WRITE_STATION_NAME (NICId, (OHA_COMMON_MEM_U8_PTR_TYPE)"", 0, rtf_nos_ip_read_only ? (LSA_UINT8)OHA_READ_ONLY : (LSA_UINT8)OHA_READ_WRITE, LSA_FALSE);
        if (Status != LSA_RET_OK)
        {
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_init_interface: OHA_DB_WRITE_STATION_NAME failed (Status: 0x%X).", Status);
			return(Status);
        }

        /* DeviceIDValue (see PNIO-Norm) as Octet String in network byte order:  */
        /* +---------------+------------------------------------------------------+ */
        /* + DeviceIDValue | VendorIDHigh, VendorIDLow, DeviceIDHigh, DeviceIDLow + */
        /* +---------------+------------------------------------------------------+ */
        DeviceIDValue[0] = (LSA_UINT8)(VendorID >> 8);          /* StationType  */
        DeviceIDValue[1] = (LSA_UINT8)(VendorID >> 0);
        DeviceIDValue[2] = (LSA_UINT8)(DeviceID >> 8);
        DeviceIDValue[3] = (LSA_UINT8)(DeviceID >> 0);

        Status = OHA_DB_WRITE_STATION_ID (NICId, DeviceIDValue, sizeof(DeviceIDValue), OHA_READ_ONLY, LSA_FALSE);
        if (Status != LSA_RET_OK)
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_init_interface: OHA_DB_WRITE_STATION_ID failed (Status: 0x%X).", Status);
			return(Status);
        }

		Status = OHA_DB_WRITE_IP_SUITE(NICId, LSA_NULL, LSA_NULL, LSA_NULL,
										OHA_IP_ADDR_SIZE, rtf_nos_ip_read_only ? (LSA_UINT8)OHA_READ_ONLY : (LSA_UINT8)OHA_READ_WRITE, LSA_FALSE);
		if (Status != LSA_RET_OK)
		{
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_init_interface: OHA_DB_WRITE_IP_SUITE failed (Status: 0x%X).", Status);
			return(Status);
		}

		/* init TCP-IP status */
		Status = OHA_DB_WRITE_TCPIP_STATUS(NICId, DCP_STATUS_TCPIP_PROJ); /* ip_addr,... available */
		if (Status != LSA_RET_OK)
		{
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_init_interface: OHA_DB_WRITE_TCPIP_STATUS failed (Status: 0x%X).", Status);
			return(Status);
		}

		if(InstanceIDValid)
		{
			Status = OHA_DB_WRITE_DEVICE_INSTANCE (NICId, InstanceID, OHA_READ_ONLY, LSA_FALSE);
			if (Status != LSA_RET_OK)
			{
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_init_interface: OHA_DB_WRITE_DEVICE_INST failed (Status: 0x%X).", Status);
				return(Status);
			}
		}

        Status = OHA_SYSTEM_IN_FCT_ATTR OHA_DB_WRITE_PROFINET_ROLE(NICId, ProfinetRole); /* 0 = no role, 1 = Profinet IO */
        if (Status != LSA_RET_OK)
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_init_interface: OHA_DB_WRITE_PROFINET_ROLE failed (Status: 0x%X).", Status);
			return(Status);
        }
		
		if(OHA_IS_NOT_NULL(DeviceVendorValue) && DeviceVendorLen)
		{
			Status = OHA_DB_WRITE_STATION_TYPE (NICId, DeviceVendorValue, DeviceVendorLen, OHA_READ_ONLY, LSA_FALSE);
			if (Status != LSA_RET_OK)
			{
				OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_init_interface: OHA_DB_WRITE_STATION_TYPE failed (Status: 0x%X).", Status);
				return(Status);
			}
		}

		Status = OHA_DB_WRITE_DCP_ELEMENT(NICId, DCP_DBID_CTRL, OHA_DCP_RESET_TO_FACTORY, (OHA_COMMON_MEM_U8_PTR_TYPE)"", 0, rtf_nos_ip_read_only ? (LSA_UINT8)OHA_READ_ONLY : (LSA_UINT8)OHA_READ_WRITE, LSA_FALSE);
		OHA_ASSERT(Status == LSA_RET_OK);
		Status = OHA_DB_WRITE_DCP_ELEMENT(NICId, DCP_DBID_CTRL, OHA_DCP_RESET, (OHA_COMMON_MEM_U8_PTR_TYPE)"", 0, rtf_nos_ip_read_only ? (LSA_UINT8)OHA_READ_ONLY : (LSA_UINT8)OHA_READ_WRITE, LSA_FALSE);
		OHA_ASSERT(Status == LSA_RET_OK);

        Status = OHA_DB_WRITE_SYS_CAP(SysCapSupported, SysCapEnabled);
        if (Status != LSA_RET_OK)
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_init_interface: OHA_DB_WRITE_SYS_CAP failed (Status: 0x%X).", Status);
			return(Status);
        }

		Status = OHA_DB_WRITE_CHASSIS_NAME(NameOfDevice, NameOfDeviceLen);
		if (Status != LSA_RET_OK)
		{
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_init_interface: OHA_DB_WRITE_CHASSIS_NAME failed (Status: 0x%X).", Status);
			return(Status);
		}

		Status = OHA_DB_WRITE_SYS_DESCR(SysDescr, SysDescrLen);
		if (Status != LSA_RET_OK)
		{
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_init_interface: OHA_DB_WRITE_SYS_DESCR failed (Status: 0x%X).", Status);
			return(Status);
		}

		Status = OHA_DB_WRITE_IF_DESCR(NICId, 0, IfDescr, IfDescrLen);
		if (Status != LSA_RET_OK)
		{
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_init_interface: OHA_DB_WRITE_IF_DESCR failed (Status: 0x%X).", Status);
			return(Status);
		}
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: oha_init_interface(Status: 0x%X)", Status);
    return(Status);
}

/*=============================================================================
 * function name:  oha_init_port
 *
 * function:       initialization of a port
 *
 * parameters:     ...
 *
 * return value:   LSA_UINT16:  LSA_RET_OK
 *                              or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR oha_init_port(
    LSA_UINT16 NICId,
    LSA_UINT16 PortId,   /* 1 ... OHA_CFG_MAX_PORTS */
    OHA_COMMON_MEM_U8_PTR_TYPE NameOfPort, /* see IEC 61158-6 */
    LSA_UINT16 NameOfPortLen,
    OHA_COMMON_MEM_U8_PTR_TYPE IfDescr, /* Interface Description (MIB2 ifDescr of the port) */
    LSA_UINT16 IfDescrLen
)
{
    OHA_DB_IF_PTR_TYPE  pDB;
    LSA_UINT16          Status;

    pDB = OHA_DB_GET_INTERFACE(NICId);
    if (OHA_IS_NOT_NULL(pDB))
    {
        if (PortId < 1 || PortId > OHA_CFG_MAX_PORTS)
        {
            Status = OHA_ERR_PARAM;
            OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "oha_init_port: PortID failed (Status: 0x%X)", Status);
            return(Status);
        }

        Status = OHA_DB_WRITE_PORT_NAME(NICId, PortId,
                                        NameOfPort, NameOfPortLen,
                                        LSA_FALSE);
        if (Status != LSA_RET_OK)
        {
            OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_init_port: OHA_DB_WRITE_PORT_NAME failed (Status: 0x%X).", Status);
            return(Status);
        }

		Status = OHA_DB_WRITE_IF_DESCR(NICId, PortId, IfDescr, IfDescrLen);
		if (Status != LSA_RET_OK)
		{
			OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"oha_init_port: OHA_DB_WRITE_IF_DESCR failed (Status: 0x%X).", Status);
			return(Status);
		}
	}
    else
    {
        Status = LSA_RET_ERR_PARAM;
    }

    return(Status);
}


/*=============================================================================
 * function name:  OHA_DB_WRITE_PORT_NAME
 * function:       write port name
 * parameters:     OHA_COMMON_MEM_U8_PTR_TYPE   NamePtr:    Ptr to port string
 *                 LSA_UINT8        NameLen:    Length of port string
 *                 LSA_UINT16       PortId:     1...OHA_CFG_MAX_PORTS
 *                 LSA_BOOL         Remanent:   Store value permanently
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if station name
 *                                              cannot be changed
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_PORT_NAME(
    LSA_UINT16                    NicId,
    LSA_UINT16                    PortId,
    OHA_COMMON_MEM_U8_PTR_TYPE    NamePtr,
    LSA_INT                       NameLen,
    LSA_BOOL                      Remanent
)
{
    OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 Ret = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_05(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_PORT_NAME(NicId: 0x%X, PortId: 0x%X, NamePtr: 0x%X, NameLen:  0x%X, Remanent:0x%X)",
                          NicId, PortId, NamePtr, NameLen, Remanent);
    LSA_UNUSED_ARG(Remanent);
    if (OHA_IS_NOT_NULL(pDB))
    {
        if ((NameLen && OHA_IS_NOT_NULL(NamePtr) && (NameLen <= OHA_MAX_STATION_NAME_LEN)) &&
            (PortId && PortId <= OHA_CFG_MAX_PORTS))
        {
            /* OHA_COMMON_MEM_U8_PTR_TYPE *ppName = &pDB->LldpData[PortId-1].pLocalPortName; */

            if (oha_is_valid_port_name((LSA_UINT16)NameLen, NamePtr))
            {
                Ret = OHA_DB_WRITE_LLDP_ELEMENT(NicId, PortId, OHA_LLDP_TYPE_PORT_ID, OHA_PORT_ID_LOCALLY_ASSIGNED, /* NicId, Type, SubType */
                                                0, NamePtr, (LSA_UINT16)NameLen);
            }
            /* Ret = oha_db_write_variable(ppName, NamePtr, (LSA_UINT16)NameLen); */
            /* pDB->LldpData[PortId-1].PortNameLen = (LSA_UINT16)NameLen;         */
        }
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_PORT_NAME(Ret: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_CHASSIS_NAME (renamed OHA_DB_WRITE_DEVICE_NAME)
 * function:       write device name
 * parameters:     OHA_COMMON_MEM_U8_PTR_TYPE   NamePtr:    Ptr to device string
 *                 LSA_UINT8        NameLen:    Length of chassis string
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if station name
 *                                              cannot be changed
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_CHASSIS_NAME(
    OHA_COMMON_MEM_U8_PTR_TYPE    NamePtr,
    LSA_INT                       NameLen
)
{
    LSA_UINT16 Ret = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_CHASSIS_NAME(NamePtr: 0x%X, NameLen:  0x%X)",
                          NamePtr, NameLen);

        if (NameLen <= OHA_MAX_STATION_NAME_LEN)
        {
        Ret = OHA_DB_WRITE_LLDP_ELEMENT(0, 0, OHA_LLDP_TYPE_CHASSIS_ID, OHA_CHASSIS_ID_LOCALLY_ASSIGNED, /* NicId, Type, SubType */
                                            0, NamePtr, (LSA_UINT16)NameLen);
        }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_CHASSIS_NAME(Ret: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_DB_READ_CHASSIS_NAME
 * function:       Reads the NameOfDevice string from the database (for LLDP-MIB)
 * parameters:     ppVariable (NameOfDevice in octets)
                   pLength (length of NameOfDevice in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_CHASSIS_NAME(
    OHA_COMMON_MEM_U8_PTR_TYPE * ppDeviceName,     /* NameOfDevice in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE  pDeviceNameLength /* length of NameOfDevice in octets */
)
{
    LSA_UINT16 RetVal;
    LSA_UINT8  SubType = 0;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_CHASSIS_NAME(ppSysName: 0x%X, pSysNameLength: 0x%X)",
                          ppDeviceName, pDeviceNameLength);

	OHA_ASSERT_NULL_PTR(ppDeviceName);
	OHA_ASSERT_NULL_PTR(pDeviceNameLength);

	RetVal = OHA_DB_READ_LLDP_ELEMENT(0, 0, OHA_LLDP_TYPE_CHASSIS_ID, &SubType, 0,
                                      ppDeviceName, pDeviceNameLength); /* OUI, pName, Len */

	if(!(RetVal == LSA_RET_OK && SubType == OHA_CHASSIS_ID_LOCALLY_ASSIGNED))
    {
        *ppDeviceName = LSA_NULL;
        *pDeviceNameLength = 0;
		RetVal = LSA_RET_ERR_RESOURCE;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_READ_CHASSIS_NAME(RetVal: 0x%X)", RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:   OHA_DB_WRITE_SYS_NAME
 * function:        Writes system name string to the database (from MIB II for LLDP-MIB)
 * parameters:      OHA_COMMON_MEM_U8_PTR_TYPE  SysNamePtr: Ptr to name string
 *                  LSA_UINT16      SysNameLen: Length of name string
 *                  LSA_BOOL        IsRema:     Store value permanently
 * return value:    LSA_UINT16      Error:      != LSA_RET_OK if name cannot be written
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_SYS_NAME(
    OHA_COMMON_MEM_U8_PTR_TYPE    SysNamePtr,
    LSA_UINT16                    SysNameLen,
    LSA_BOOL                      IsRema)
{
    LSA_UINT16 Ret = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_SYS_NAME(SysDescrPtr: 0x%X, SysDescrLen: 0x%X, IsRema: 0x%X)",
                          SysNamePtr, SysNameLen, IsRema);
    if (SysNameLen <= OHA_MAX_CHASSIS_STRLEN)
    {
        /* OHA_COMMON_MEM_U8_PTR_TYPE *ppName = &oha_database.StationData.pNameOfSystem; */

        Ret = OHA_DB_WRITE_LLDP_ELEMENT(0, 0, OHA_LLDP_TYPE_SYSTEM_NAME, 0, /* NicId, Type, SubType */
                                        0, SysNamePtr, SysNameLen);

        /* Ret = oha_db_write_variable(ppName, SysNamePtr, SysNameLen); */
        /* oha_database.StationData.SystemNameLen = SysNameLen;         */
		if (Ret == LSA_RET_OK)
		{
			Ret = oha_db_lldp_write_remanence(0, 0, OHA_LLDP_TYPE_SYSTEM_NAME, 0, 0, IsRema);
		}
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_SYS_NAME(Ret: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_DB_READ_SYS_NAME
 * function:       Reads the system name string from the database (for MIB II, LLDP-MIB)
 * parameters:     ppVariable (SysName in octets)
                   pLength (length of SysName in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_SYS_NAME(
    OHA_COMMON_MEM_U8_PTR_TYPE * ppSysName,     /* SysName in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pSysNameLength,  /* length of SysName in octets */
    LSA_BOOL                   * pIsRema)
{
    LSA_UINT16 RetVal;
    LSA_UINT8  SubType;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_SYS_NAME(ppSysName: 0x%X, pSysNameLength: 0x%X, pIsRema: 0x%X)",
                          ppSysName, pSysNameLength, pIsRema);

	OHA_ASSERT_NULL_PTR(ppSysName);
	OHA_ASSERT_NULL_PTR(pSysNameLength);

	RetVal = OHA_DB_READ_LLDP_ELEMENT(0, 0, OHA_LLDP_TYPE_SYSTEM_NAME, &SubType, 0, /* NicId, PortId, Type, SubType, OUI */
                                      ppSysName, pSysNameLength); /* OUI, pName, Len */
	if(RetVal == LSA_RET_OK || RetVal == LSA_RET_ERR_RESOURCE)
	{
		LSA_BOOL IsRema = LSA_FALSE;

	    if (*pSysNameLength != 0) OHA_ASSERT_NULL_PTR(*ppSysName);
		(LSA_UINT16)oha_db_lldp_read_remanence(0, 0, OHA_LLDP_TYPE_SYSTEM_NAME, &SubType, 0, &IsRema);
        if (OHA_IS_NOT_NULL(pIsRema))  *pIsRema = IsRema;
		RetVal = LSA_RET_OK;	/* the length 0 is valid */
	}
	else
    {
        *ppSysName = LSA_NULL;
        *pSysNameLength = 0;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_READ_SYS_NAME(RetVal: 0x%X)", RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  OHA_DB_READ_SYS_DESCR
 * function:       Reads the system description string from the database (for MIB II, LLDP-MIB)
 * parameters:     ppVariable (SysDescr in octets)
                   pLength (length of SysDescr in octets)
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_SYS_DESCR(
    OHA_COMMON_MEM_U8_PTR_TYPE * ppSysDescr,     /* SysDescr in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pSysDescrLength) /* length of SysDescr in octets */
{
    LSA_UINT16 RetVal;
    LSA_UINT8  SubType;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_SYS_DESCR(ppSysDescr: 0x%X, pSysDescrLength: 0x%X)",
                          ppSysDescr, pSysDescrLength);

	OHA_ASSERT_NULL_PTR(ppSysDescr);
	OHA_ASSERT_NULL_PTR(pSysDescrLength);

	RetVal = OHA_DB_READ_LLDP_ELEMENT(0, 0, OHA_LLDP_TYPE_SYSTEM_DESCR, &SubType, 0, /* NicId, PortId, Type, SubType, OUI */
                                      ppSysDescr, pSysDescrLength); /* OUI, pName, Len */
	if(RetVal == LSA_RET_OK || RetVal == LSA_RET_ERR_RESOURCE)
	{
	    if (*pSysDescrLength != 0) OHA_ASSERT_NULL_PTR(*ppSysDescr);
		RetVal = LSA_RET_OK;	/* the length 0 is valid */
	}
	else
    {
        *ppSysDescr = LSA_NULL;
        *pSysDescrLength = 0;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_READ_SYS_DESCR(RetVal: 0x%X)", RetVal);
    return RetVal;
}


/*=============================================================================
 * function name:   OHA_DB_READ_IF_DESCR
 * function:        Reads interface description string from the database (from MIB II for LLDP-MIB)
 * parameters:      LSA_UINT16      NicId:      Interface-Id
 *                  OHA_COMMON_MEM_U8_PTR_TYPE  IfDescrPtr: Ptr to name string
 *                  LSA_UINT16      IfDescrLen: Length of name string
 * return value:    LSA_UINT16      Error:      != LSA_RET_OK if name cannot be read
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_IF_DESCR (
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    OHA_COMMON_MEM_U8_PTR_TYPE * pIfDescrPtr, /* IfDescr in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pIfDescrLen)  /* length of IfDescr in octets */
{
    LSA_UINT16 RetVal;
    LSA_UINT8  SubType;

    OHA_FUNCTION_TRACE_04(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_IF_DESCR(NicId: 0x%X, PortId: 0x%X, pIfDescrPtr: 0x%X, pIfDescrLen: 0x%X)",
                          NicId, PortId, pIfDescrPtr, pIfDescrLen);

	OHA_ASSERT_NULL_PTR(pIfDescrPtr);
	OHA_ASSERT_NULL_PTR(pIfDescrLen);

	RetVal = OHA_DB_READ_LLDP_ELEMENT(NicId, PortId, OHA_LLDP_TYPE_PORT_DESCR, &SubType, 0, /* NicId, PortId, Type, SubType, OUI */
                                      pIfDescrPtr, pIfDescrLen); /* pName, Len */
	if(RetVal == LSA_RET_OK || RetVal == LSA_RET_ERR_RESOURCE)
	{
	    if (*pIfDescrLen != 0) OHA_ASSERT_NULL_PTR(*pIfDescrPtr);
		RetVal = LSA_RET_OK;	/* the length 0 is valid */
	}
	else
    {
        *pIfDescrPtr = LSA_NULL;
        *pIfDescrLen = 0;
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "OUT: OHA_DB_READ_IF_DESCR(RetVal: 0x%X)", RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:   OHA_DB_WRITE_SYS_DESCR
 * function:        Writes system description string to the database (from MIB II for LLDP-MIB)
 * parameters:      OHA_COMMON_MEM_U8_PTR_TYPE  SysDescrPtr:    Ptr to name string
 *                  LSA_UINT16      SysDescrLen:    Length of name string
 * return value:    LSA_UINT16      Error:      != LSA_RET_OK if name cannot be written
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_SYS_DESCR (
    OHA_COMMON_MEM_U8_PTR_TYPE    SysDescrPtr,
    LSA_UINT16                    SysDescrLen )
{
    LSA_UINT16 Ret = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_SYS_DESCR(SysDescrPtr: 0x%X, SysDescrLen: 0x%X)",
                          SysDescrPtr, SysDescrLen);
    if (SysDescrLen <= OHA_MAX_CHASSIS_STRLEN)
    {
        /* OHA_COMMON_MEM_U8_PTR_TYPE *ppName = &oha_database.StationData.pDescrOfSystem; */

        Ret = OHA_DB_WRITE_LLDP_ELEMENT(0, 0, OHA_LLDP_TYPE_SYSTEM_DESCR, 0, /* NicId, Type, SubType */
                                        0, SysDescrPtr, SysDescrLen);

        /* Ret = oha_db_write_variable(ppName, SysDescrPtr, SysDescrLen); */
        /* oha_database.StationData.SystemDescrLen = SysDescrLen;         */
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_SYS_DESCR(Ret: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:   OHA_DB_WRITE_SYS_CONTACT
 * function:        Writes system contact string to the database (for MIB II)
 * parameters:      OHA_COMMON_MEM_U8_PTR_TYPE  SysContactPtr:  Ptr to name string
 *                  LSA_UINT16      SysContactLen:    Length of contact string
 *                  LSA_BOOL        IsRema:     Store value permanently
 * return value:    LSA_UINT16      Error:      != LSA_RET_OK if contact cannot be written
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_SYS_CONTACT (
    OHA_COMMON_MEM_U8_PTR_TYPE    SysContactPtr,
    LSA_UINT16                    SysContactLen,
	LSA_BOOL                      IsRema)
{
    LSA_UINT16 Ret = LSA_RET_OK;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_SYS_CONTACT(SysContactPtr: 0x%X, SysContactLen: 0x%X, IsRema: 0x%X)",
                          SysContactPtr, SysContactLen, IsRema);
    if (SysContactLen <= OHA_MAX_CHASSIS_STRLEN)
    {
        /* device-global options needs no Interface (Nic independent) */
        Ret = oha_db_write_snmp_variable(&oha_database.StationData.pSysContact, SysContactPtr, SysContactLen, OHA_SNMP_STRING, IsRema);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_SYS_CONTACT(Ret: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_DB_READ_SYS_CONTACT
 * function:       reads SysContact of the database element
 * parameters:     NicId
                   pVariable (variable in octets)
                   Length (length of variable in octets)
 *                 pIsRema:     storage of value
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_SYS_CONTACT(
    OHA_COMMON_MEM_U8_PTR_TYPE * pSysContactPtr, /* SysContact in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pSysContactLen,  /* length of SysContact in octets */
	LSA_BOOL                   * pIsRema)
{
    LSA_UINT16 Ret;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_SYS_CONTACT(pSysContactPtr: 0x%X, pSysContactLen: 0x%X, pIsRema: 0x%X)",
                          pSysContactPtr, pSysContactLen, pIsRema);

	/* device-global options needs no Interface (Nic independent) */
    Ret = oha_db_read_snmp_variable(&oha_database.StationData.pSysContact, 
									pSysContactPtr, pSysContactLen, 
									LSA_NULL, pIsRema);
	if(Ret == LSA_RET_ERR_RESOURCE)
	{
		Ret = LSA_RET_OK;	/* the length 0 is valid */
	}
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_SYS_CONTACT(Ret: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:   OHA_DB_WRITE_SYS_LOCATION
 * function:        Writes system location string to the database (for MIB II)
 * parameters:      OHA_COMMON_MEM_U8_PTR_TYPE  SysLocationPtr:  Ptr to name string
 *                  LSA_UINT16      SysLocationLen:    Length of location string
 *                  LSA_BOOL        IsRema:     Store value permanently
 * return value:    LSA_UINT16      Error:      != LSA_RET_OK if location cannot be written
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_SYS_LOCATION (
    OHA_COMMON_MEM_U8_PTR_TYPE    SysLocationPtr,
    LSA_UINT16                    SysLocationLen,
	LSA_BOOL                      IsRema)
{
    LSA_UINT16 Ret = LSA_RET_OK;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_SYS_LOCATION(SysLocationPtr: 0x%X, SysLocationLen: 0x%X, IsRema: 0x%X)",
                          SysLocationPtr, SysLocationLen, IsRema);
    if (SysLocationLen <= OHA_MAX_CHASSIS_STRLEN)
    {
        /* device-global options needs no Interface (Nic independent) */
        Ret = oha_db_write_snmp_variable(&oha_database.StationData.pSysLocation, SysLocationPtr, SysLocationLen, OHA_SNMP_STRING, IsRema);
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_SYS_LOCATION(Ret: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_DB_READ_SYS_LOCATION
 * function:       reads SysLocation from the database
 * parameters:     NicId
                   pVariable (SysLocation in octets)
                   Length (length of SysLocation in octets)
 *                 pIsRema:     storage of SysLocation
 * return value:   LSA_UINT16:      LSA_RET_OK or LSA_RET_ERR_PARAM
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_SYS_LOCATION(
    OHA_COMMON_MEM_U8_PTR_TYPE * pSysLocationPtr, /* Location in octets */
    OHA_COMMON_MEM_U16_PTR_TYPE pSysLocationLen,  /* length of Location in octets */
	LSA_BOOL                   * pIsRema)
{
    LSA_UINT16 Ret;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_SYS_LOCATION(pSysLocationPtr: 0x%X, pSysLocationLen: 0x%X, pIsRema: 0x%X)",
                          pSysLocationPtr, pSysLocationLen, pIsRema);

	/* device-global options needs no Interface (Nic independent) */
    Ret = oha_db_read_snmp_variable(&oha_database.StationData.pSysLocation, 
									pSysLocationPtr, pSysLocationLen, 
									LSA_NULL, pIsRema);
	if(Ret == LSA_RET_ERR_RESOURCE)
	{
		Ret = LSA_RET_OK;	/* the length 0 is valid */
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_SYS_LOCATION(Ret: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:   OHA_DB_WRITE_SYS_CAP
 * function:        Writes system capabilities string to the database (from MIB II for LLDP-MIB)
 *
 *      2 octets         2 octets
 *  +--------------+-----------------+
 *  | system       |  enabled        |
 *  | capabilities.|  capabilities   |
 *  +--------------+-----------------+
 *
 * parameters:      LSA_UINT16    SysCapSupported
 *                  LSA_UINT16    SysCapEnabled
 * return value:    LSA_UINT16    Error: != LSA_RET_OK if capabilities cannot be written
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_SYS_CAP (
    LSA_UINT16      SysCapSupported,
    LSA_UINT16      SysCapEnabled )
{
    LSA_UINT16 Ret;
    LSA_UINT8  SysCap[LLDP_TLV_SYSTEM_CAP_SIZE];
    OHA_UPPER_MEM_U8_PTR_TYPE pSysCap = SysCap;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_SYS_CAP(SysCapSupported: 0x%X, SysCapEnabled: 0x%X)",
                          SysCapSupported, SysCapEnabled);

    OHA_PUT16_HTON (pSysCap, 0, SysCapSupported);
    OHA_PUT16_HTON (pSysCap, 2, SysCapEnabled);

    Ret = OHA_DB_WRITE_LLDP_ELEMENT(0, 0, OHA_LLDP_TYPE_SYSTEM_CAP, 0, /* NicId, Type, SubType */
                                    0, SysCap, LLDP_TLV_SYSTEM_CAP_SIZE);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_SYS_CAP(Ret: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:   OHA_DB_WRITE_IF_DESCR
 * function:        Writes interface description string to the database (from MIB II for LLDP-MIB)
 * parameters:      LSA_UINT16      NicId:      Interface-Id
 *                  LSA_UINT16      PortId:     Port-Id
 *                  OHA_COMMON_MEM_U8_PTR_TYPE  IfDescrPtr: Ptr to name string
 *                  LSA_UINT16      IfDescrLen: Length of name string
 * return value:    LSA_UINT16      Error:      != LSA_RET_OK if name cannot be written
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_IF_DESCR (
    LSA_UINT16                  NicId,
    LSA_UINT16                  PortId,
    OHA_COMMON_MEM_U8_PTR_TYPE  IfDescrPtr,
    LSA_UINT16                  IfDescrLen )
{
    OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 Ret = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_IF_DESCR(NicId: 0x%X, IfDescrPtr: 0x%X, IfDescrLen: 0x%X)",
                          NicId, IfDescrPtr, IfDescrLen);
    if (OHA_IS_NOT_NULL(pDB))
    {
		/* The Portstrings (port_id = 1...n) and the string for the IF (port_id = 0) is supported too! */ 
        if (IfDescrLen <= OHA_MAX_CHASSIS_STRLEN && /* PortId && */ PortId <= OHA_CFG_MAX_PORTS)
        {
            /* OHA_COMMON_MEM_U8_PTR_TYPE *ppName = &pDB->pDescrOfIf;       */

            Ret = OHA_DB_WRITE_LLDP_ELEMENT(NicId, PortId, OHA_LLDP_TYPE_PORT_DESCR, 0, /* NicId, Type, SubType */
                                            0, IfDescrPtr, IfDescrLen);
            /* Ret = oha_db_write_variable(ppName, IfDescrPtr, IfDescrLen); */
            /* pDB->IfDescrLen = IfDescrLen;                                */
        }
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_IF_DESCR(Ret: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_DB_READ_MAC_ADDR
 * function:       Get pointer to and length of of local MAC address
 * parameters:     OHA_SYS_WORD_PTR_TYPE * AddrPtrPtr:  Pointer for Ret.Ptr
 * return value:   LSA_INT:                             Length
 *===========================================================================*/
LSA_INT  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_MAC_ADDR(
    LSA_UINT16  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    * AddrPtrPtr
)
{
    LSA_BOOL   Remanent;
    LSA_UINT8  AccessControl;
    LSA_UINT16 Length = 0;
    OHA_COMMON_MEM_U8_PTR_TYPE  pType = LSA_NULL;
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_MAC_ADDR(NicId: 0x%X, AddrPtrPtr: 0x%X)",
                          NicId, AddrPtrPtr);

    RetVal = OHA_DB_READ_DCP_ELEMENT(NicId,
                                     OHA_DCP_OPTION_TCP,
                                     DCP_DSID_MACADR,
                                     &pType,
                                     &Length,
                                     &AccessControl,
                                     &Remanent);
    if (RetVal == LSA_RET_OK && Length == OHA_MAC_ADDR_SIZE)
    {
        if (OHA_IS_NOT_NULL(pType))
        {
            *AddrPtrPtr = pType;
            /* return (LSA_UINT8)Length; */
        }
        else
        {
            *AddrPtrPtr = LSA_NULL;
            Length = 0;
        }
    }
    else
    {
        *AddrPtrPtr = LSA_NULL;
        Length = 0;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_MAC_ADDR(Length: 0x%X)", Length);
    return(Length);
}

/*=============================================================================
 * function name:  OHA_DB_READ_IP_SUITE
 * function:       Get pointer to and length of of local IP suite
 * parameters:     OHA_SYS_WORD_PTR_TYPE * IpAddrPtrPtr: Pointer for IP address
                   OHA_SYS_WORD_PTR_TYPE * SubnetMaskPtrPtr: Pointer for subnet mask or NULL
                   OHA_SYS_WORD_PTR_TYPE * RouterAddrPtrPtr: Pointer for default router or NULL
 * return value:   LSA_INT:                             Length
 *===========================================================================*/
LSA_INT  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_IP_SUITE(
    LSA_UINT16                      NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    * IpAddrPtrPtr,
    OHA_COMMON_MEM_U8_PTR_TYPE    * SubnetMaskPtrPtr,
    OHA_COMMON_MEM_U8_PTR_TYPE    * RouterAddrPtrPtr
)
{
    LSA_BOOL   Remanent;
    LSA_UINT8  AccessControl;
    LSA_UINT16 Length = 0;
    OHA_COMMON_MEM_U8_PTR_TYPE pType = LSA_NULL;
    LSA_UINT16 RetVal;
    OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NicId);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_IP_SUITE(NicId: 0x%X, IpAddrPtrPtr: 0x%X)",
                          NicId, IpAddrPtrPtr);
    /* first look at the suite element in the database */
    RetVal = OHA_DB_READ_DCP_ELEMENT(NicId,
                                     OHA_DCP_OPTION_TCP,
                                     DCP_DSID_TCPIP,
                                     &pType,
                                     &Length,
                                     &AccessControl,
                                     &Remanent);
    if (RetVal == LSA_RET_OK && Length == OHA_IP_SUITE_SIZE && OHA_IS_NOT_NULL(pDB))
    {
        /* IpAddr, SubnetMask, RouterAddr */
        *IpAddrPtrPtr     = pType;
        if (OHA_IS_NOT_NULL(SubnetMaskPtrPtr))
		{
			*SubnetMaskPtrPtr = (pType + OHA_IP_ADDR_SIZE);
		}
        if (OHA_IS_NOT_NULL(RouterAddrPtrPtr))
		{
			*RouterAddrPtrPtr = (pType + OHA_IP_ADDR_SIZE + OHA_IP_ADDR_SIZE);
		}
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_DB_READ_IP_SUITE(RetVal: 0x%X)", RetVal);
        return(sizeof(OHA_IP_TYPE));
    }

    *IpAddrPtrPtr = LSA_NULL;
	if(OHA_IS_NOT_NULL(SubnetMaskPtrPtr))
	{
	    *SubnetMaskPtrPtr = LSA_NULL;
	}
	if(OHA_IS_NOT_NULL(RouterAddrPtrPtr))
	{
	    *RouterAddrPtrPtr = LSA_NULL;
	}

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_IP_SUITE(RetVal: 0x%X)", RetVal);
    return(0);
}

/*=============================================================================
 * function name:  OHA_DB_READ_STATION_TYPE
 * function:       Get pointer to and length of station type string
 * parameters:     OHA_SYS_BYTE_PTR_TYPE * TypePtrPtr:  Pointer for Ret.Ptr
 * return value:   LSA_INT:                             Length
 *===========================================================================*/
LSA_INT  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_STATION_TYPE(
    LSA_UINT16                   NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE * TypePtrPtr
)
{
    LSA_BOOL   Remanent;
    LSA_UINT8  AccessControl;
    LSA_UINT16 Length = 0;
    OHA_COMMON_MEM_U8_PTR_TYPE pType = LSA_NULL;
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_STATION_TYPE(NicId: 0x%X, TypePtrPtr: 0x%X)", NicId, TypePtrPtr);
    RetVal = OHA_DB_READ_DCP_ELEMENT(NicId,
                                     OHA_DCP_OPTION_IDENT,
                                     DCP_DSID_IDTYPE,
                                     &pType,
                                     &Length,
                                     &AccessControl,
                                     &Remanent);
    if (RetVal == LSA_RET_OK)
    {
        *TypePtrPtr = pType;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_DB_READ_STATION_TYPE(RetVal: 0x%X)", RetVal);
        return Length;
    }

    *TypePtrPtr = LSA_NULL;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_STATION_TYPE(RetVal: 0x%X)", RetVal);
    return(0);
}

/*=============================================================================
 * function name:  OHA_DB_READ_STATION_NAME
 * function:       Get pointer to and length of station name string
 * parameters:     OHA_SYS_BYTE_PTR_TYPE * NamePtrPtr:  Pointer for Ret.Ptr
 * return value:   LSA_INT:                             Length
 *===========================================================================*/
LSA_INT  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_STATION_NAME(
    LSA_UINT16  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE        * NamePtrPtr
)
{
    LSA_BOOL   Remanent;
    LSA_UINT8  AccessControl;
    LSA_UINT16 Length = 0;
    OHA_COMMON_MEM_U8_PTR_TYPE pType = LSA_NULL;
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_STATION_NAME(NicId: 0x%X, NamePtrPtr: 0x%X)",
                          NicId, NamePtrPtr);
    RetVal = OHA_DB_READ_DCP_ELEMENT(NicId,
                                     OHA_DCP_OPTION_IDENT,
                                     DCP_DSID_IDNAME,
                                     &pType,
                                     &Length,
                                     &AccessControl,
                                     &Remanent);
    if (RetVal == LSA_RET_OK)
    {
        *NamePtrPtr = pType;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_DB_READ_STATION_NAME(RetVal: 0x%X)", RetVal);
        return Length;
    }
    *NamePtrPtr = LSA_NULL;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_STATION_NAME(RetVal: 0x%X)", RetVal);
    return(0);
}

/*=============================================================================
 * function name:  OHA_DB_READ_STATION_ID
 * function:       Get ptr, len and type  of station identificator
 * parameters:     LSA_UINT8    **  IdPtr:      Ptr to RetPtr (Identificator)
 * return value:   LSA_UINT8        Length:     Length of identificator or 0
 *                                              if not present in station
 *===========================================================================*/
LSA_UINT8 OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_STATION_ID(
    LSA_UINT16                   NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE * IdPtrPtr
)
{
    LSA_BOOL   Remanent;
    LSA_UINT8  AccessControl;
    LSA_UINT16 Length = 0;
    OHA_COMMON_MEM_U8_PTR_TYPE pType = LSA_NULL;
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_STATION_ID(NicId: 0x%X, IdPtrPtr: 0x%X)", NicId, IdPtrPtr);
    RetVal = OHA_DB_READ_DCP_ELEMENT(NicId,
                                     OHA_DCP_OPTION_IDENT,
                                     DCP_DSID_IDID,
                                     &pType,
                                     &Length,
                                     &AccessControl,
                                     &Remanent);
    if (RetVal == LSA_RET_OK)
    {
        *IdPtrPtr = pType;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_DB_READ_STATION_ID(RetVal: 0x%X)", RetVal);
        return (LSA_UINT8)Length;
    }
    *IdPtrPtr = LSA_NULL;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_STATION_ID(RetVal: 0x%X)", RetVal);
    return(0);
}

/*=============================================================================
 * function name:  OHA_DB_READ_OEM_DEVICE_ID
 * function:       Get ptr, len and type  of OEMDeviceID
 * parameters:     LSA_UINT8    **  IdPtr:      Ptr to RetPtr (Identificator)
 * return value:   LSA_UINT8        Length:     Length of OEMDeviceID or 0
 *                                              if not present
 *===========================================================================*/
LSA_UINT8 OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_OEM_DEVICE_ID(
    LSA_UINT16                   NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE * IdPtrPtr
)
{
    LSA_BOOL   Remanent;
    LSA_UINT8  AccessControl;
    LSA_UINT16 Length = 0;
    OHA_COMMON_MEM_U8_PTR_TYPE pType = LSA_NULL;
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_OEM_DEVICE_ID(NicId: 0x%X, IdPtrPtr: 0x%X)", NicId, IdPtrPtr);
    RetVal = OHA_DB_READ_DCP_ELEMENT(NicId,
                                     OHA_DCP_OPTION_IDENT,
                                     DCP_DSID_IDOEMID,
                                     &pType,
                                     &Length,
                                     &AccessControl,
                                     &Remanent);
    if (RetVal == LSA_RET_OK)
    {
        *IdPtrPtr = pType;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_DB_READ_OEM_DEVICE_ID(RetVal: 0x%X)", RetVal);
        return (LSA_UINT8)Length;
    }
    *IdPtrPtr = LSA_NULL;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_OEM_DEVICE_ID(RetVal: 0x%X)", RetVal);
    return(0);
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_TCPIP_STATUS
 * function:       Get Status word for TCP/IP parameter (dataset 2)
 * parameters:  LSA_UINT16:                         Status
 *              DCP_STATUS_TCPIP_NONE - TCP/IP Parameter not initialized
 *              DCP_STATUS_TCPIP_PROJ - TCP/IP Parameter projektiert
 *              DCP_STATUS_TCPIP_DHCP - TCP/IP Parameter got from DHCP
 *              DCP_STATUS_TCPIP_RESERVED - 0x0003-0x00ff: Reserviert
 * return value:   LSA_UINT16       !LSA_RET_OK could not write status
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_TCPIP_STATUS(
    LSA_UINT16  NicId,
    LSA_UINT16  Status
)
{
    LSA_UINT16 RetVal = LSA_RET_ERR_RESOURCE;
    OHA_DB_IF_PTR_TYPE  pDB;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_TCPIP_STATUS(NicId: 0x%X, Status: 0x%X)",
                          NicId, Status);
    pDB = OHA_DB_GET_INTERFACE(NicId);
    if (OHA_IS_NOT_NULL(pDB))
    {
        pDB->DcpData.TcpIpStatus = Status; /* ip_addr,... available */
        RetVal = LSA_RET_OK;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_TCPIP_STATUS(RetVal: 0x%X)", RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  OHA_DB_READ_SUPPORTED_DATABASE
 * function:       Get pointer to and length of list of databases supported
 * parameters:     OHA_SYS_WORD_PTR_TYPE * DbasePtrPtr: Pointer for Ret.Ptr
 * return value:   LSA_INT:                             Length
 *===========================================================================*/
LSA_INT OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_SUPPORTED_DATABASE(
    LSA_UINT16             NicId,
    OHA_COMMON_MEM_U16_PTR_TYPE* DbasePtrPtr
)
{
    LSA_BOOL   Remanent;
    LSA_UINT8  AccessControl;
    LSA_UINT16 Length = 0;
    OHA_COMMON_MEM_U8_PTR_TYPE pType = LSA_NULL;
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_SUPPORTED_DATABASE(NicId: 0x%X, DbasePtrPtr: 0x%X)",
                          NicId, DbasePtrPtr);

	RetVal = oha_update_supported_database(NicId);
    if (RetVal != LSA_RET_OK)
	{
		OHA_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP,"OHA_DB_READ_SUPPORTED_DATABASE: update database failed (RetVal: 0x%X).", RetVal);
    }

    RetVal = OHA_DB_READ_DCP_ELEMENT(NicId,
                                     OHA_DCP_OPTION_IDENT,
                                     DCP_DSID_IDOPTS,
                                     &pType,
                                     &Length,
                                     &AccessControl,
                                     &Remanent);
    if (RetVal == LSA_RET_OK && (!(Length%2))) /* length must be even ! */
    {
		if(OHA_IS_NOT_NULL(DbasePtrPtr))
		{
            *DbasePtrPtr = (OHA_COMMON_MEM_U16_PTR_TYPE)pType;
		}
        /* Length = Length / 2; */  /* in WORDS */
        Length >>= 1;
    }
    else
    {
		if(OHA_IS_NOT_NULL(DbasePtrPtr))
		{
            *DbasePtrPtr = LSA_NULL;
		}
        Length = 0;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_SUPPORTED_DATABASE(Length: 0x%X)", Length);
    return((LSA_INT)Length);
}

/*=============================================================================
 * function name:  OHA_DB_READ_PROFINET_ROLE
 * function:       Get 32 bit field with stations ProfinetIO role(s)
 * parameters:     LSA_UINT16       NicId       indentifies interface
 * return value:   LSA_UINT8        role        ProfinetIO role of station
 *                                              0 if no role
 *===========================================================================*/
LSA_UINT8 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_PROFINET_ROLE(
    LSA_UINT16  NicId
)
{
    LSA_BOOL   Remanent;
    LSA_UINT8  AccessControl;
    LSA_UINT16 Length;
    OHA_COMMON_MEM_U8_PTR_TYPE pType = LSA_NULL;
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_PROFINET_ROLE(NicId: 0x%X)", NicId);
    RetVal = OHA_DB_READ_DCP_ELEMENT(NicId,
                                     OHA_DCP_OPTION_IDENT,
                                     DCP_DSID_IDROLE,
                                     &pType,
                                     &Length,
                                     &AccessControl,
                                     &Remanent);

    if (RetVal == LSA_RET_OK && OHA_IS_NOT_NULL(pType))
    {
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_DB_READ_PROFINET_ROLE(RetVal: 0x%X)", RetVal);
        return (*pType);
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_PROFINET_ROLE(RetVal: 0x%X)", RetVal);
    return(0);    /*0 = no role, 1 = Profinet IO */
}

/*=============================================================================
 * function name:  OHA_DB_READ_DEVICE_INITIATIVE
 * function:       Get 32 bit field with DeviceInitiativeValue
 * parameters:     LSA_UINT32 * pHelloMode
 *                              Bit 0: DeviceInitiativeValue.Hello
 *                  0x00    OFF Device does not issue a DCP-Hello-ReqPDU after power on.
 *                  0x01    ON  Device does issue a DCP-Hello-ReqPDU after power on.
 *                  0x02    ON  DCP-Hello-ReqPDU after HelloDelay.
 *                 LSA_UINT32 * pHelloRetry
 *                 LSA_UINT32 * HelloInterval  SendInterval (in ms)
 *                 LSA_UINT32 * pHelloDelay
 * return value:   LSA_UINT16   Error:      != LSA_RET_OK if DeviceInitiative
 *                                          cannot be changed
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_DEVICE_INITIATIVE(
    LSA_UINT16      NicId,
    OHA_COMMON_MEM_U32_PTR_TYPE pHelloMode,
    OHA_COMMON_MEM_U32_PTR_TYPE pHelloRetry,
    OHA_COMMON_MEM_U32_PTR_TYPE pHelloInterval,
    OHA_COMMON_MEM_U32_PTR_TYPE pHelloDelay)
{
    LSA_UINT16 RetVal = LSA_RET_OK;

    OHA_HDB_DCP_PTR_TYPE    pHDBDcp = OHA_DCPGetHandleByNIC(NicId);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_DEVICE_INITIATIVE(NicId: 0x%X, pHelloMode: 0x%X)",
                          NicId, pHelloMode);

    if (OHA_IS_NULL(pHDBDcp))    /* no interface */
    {
        RetVal = LSA_RET_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_DB_READ_DEVICE_INITIATIVE(RetVal: 0x%X)", RetVal);
        return RetVal;
    }

#if 0
    {
        OHA_COMMON_MEM_U8_PTR_TYPE  pType = LSA_NULL;
        LSA_BOOL   Remanent;
        LSA_UINT8  AccessControl;
        LSA_UINT16 Length;

        RetVal = OHA_DB_READ_DCP_ELEMENT(NicId,
                                         DCP_DBID_DEVINIT,
                                         DCP_DSID_DEVINIT,
                                         &pType,
                                         &Length,
                                         &AccessControl,
                                         &Remanent);
        if (RetVal == LSA_RET_OK && Length == DCP_DS_DEVINIT_SIZE) /* 16 bit */
        {
            LSA_UINT16  DeviceInitiative = OHA_GET16_NTOH(pType, 0);

            *pHelloMode = DeviceInitiative;
        }
        else
        {
            *pHelloMode = 0;
        }
    }
#endif

    /* HelloRetry, HelloDelay */
    *pHelloMode     = pHDBDcp->FSU_DataCommit.HelloMode;
    *pHelloRetry    = pHDBDcp->FSU_DataCommit.HelloRetry;
    *pHelloInterval = pHDBDcp->FSU_DataCommit.HelloInterval;
    *pHelloDelay    = pHDBDcp->FSU_DataCommit.HelloDelay;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_DEVICE_INITIATIVE(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*=============================================================================
 * function name:  OHA_DB_READ_TCPIP_STATUS
 * function:       Get Status word for TCP/IP parameter (dataset 2)
 * parameters:     LSA_VOID
 * return value:   LSA_UINT16:                          Status
 *              DCP_STATUS_TCPIP_NONE - TCP/IP Parameter not initialized
 *              DCP_STATUS_TCPIP_PROJ - TCP/IP Parameter projektiert
 *              DCP_STATUS_TCPIP_DHCP - TCP/IP Parameter got from DHCP
 *              DCP_STATUS_TCPIP_RESERVED - 0x0003-0x00ff: Reserviert
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_READ_TCPIP_STATUS(
    LSA_UINT16  NicId
)
{
    OHA_DB_IF_PTR_TYPE  pDB;

    OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_TCPIP_STATUS(NicId: 0x%X)",
                          NicId);
    pDB = OHA_DB_GET_INTERFACE(NicId);
    if (OHA_IS_NOT_NULL(pDB))
    {
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_DB_READ_TCPIP_STATUS(pDB: 0x%X)", pDB);
        return (pDB->DcpData.TcpIpStatus); /* ip_addr,... available */
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_TCPIP_STATUS(pDB: 0x%X)", pDB);
    return DCP_STATUS_TCPIP_NONE;
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_MAC_ADDR
 * function:       Set local MAC address
 * parameters:     OHA_MAC_ADDR AddrPtr:                Pointer to MAC address
 *                 LSA_INT     AddrLen:                 Length of address
 *                 LSA_UINT8   AccessControl:           Read, Write, Query,...
 *                 LSA_BOOL   Remanent:                 Store value permanently
 * return value:   LSA_UINT16:                          Error
 *===========================================================================*/
LSA_UINT16  OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_WRITE_MAC_ADDR(
    LSA_UINT16                          NicId,
    OHA_MAC_TYPE  OHA_LOCAL_MEM_ATTR    AddrPtr,
    LSA_INT                             AddrLen,
    LSA_UINT8                           AccessControl,
    LSA_BOOL                            Remanent
)
{
    LSA_UINT16  Ret = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_05(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_MAC_ADDR(NicId: 0x%X, AddrPtr: 0x%X, AddrLen: 0x%X, AccessControl: 0x%X, Remanent: 0x%X)",
                          NicId, AddrPtr, AddrLen, AccessControl, Remanent);
    if (AddrLen == 0 || (OHA_IS_NOT_NULL(AddrPtr) && AddrLen == sizeof(OHA_MAC_TYPE)))
    {
        Ret = OHA_DB_WRITE_DCP_ELEMENT(NicId, OHA_DCP_OPTION_TCP, DCP_DSID_MACADR, AddrPtr, (LSA_UINT16)AddrLen, AccessControl, Remanent);
#if 0 /* AP01301988: IEEE conform LLDP */
        /* write the lldp option CHASSIS_ID to the database */
        (LSA_VOID)OHA_DB_WRITE_LLDP_ELEMENT(NicId, 0, OHA_LLDP_TYPE_CHASSIS_ID,
                                            OHA_CHASSIS_ID_MAC_ADDR,/*SubType*/ 0,
                                            AddrPtr,/* pVariable */
                                            (LSA_UINT16)AddrLen /* Length */);
#endif
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_MAC_ADDR(RetVal: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_PROFINET_ROLE
 * function:       Set 32 bit field with stations ProfinetIO role(s)
 * parameters:     LSA_UINT8        role        ProfinetIO role of station
 *                                              0 if no role
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if station type
 *                                              cannot be changed
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_PROFINET_ROLE(
    LSA_UINT16      NicId,
    LSA_UINT8       role
)
{
    LSA_UINT16  Ret;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_PROFINET_ROLE(NicId: 0x%X, role: 0x%X)", NicId, role);
    Ret = OHA_DB_WRITE_DCP_ELEMENT(NicId, OHA_DCP_OPTION_IDENT,
                                   DCP_DSID_IDROLE, &role, 1, OHA_READ_ONLY, LSA_FALSE);
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_PROFINET_ROLE(RetVal: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_DEVICE_INITIATIVE
 * function:       Set 16 bit field with DeviceInitiativeValue
 * parameters:     LSA_UINT32   HelloMode
 *                  0x00    OFF Device does not issue a DCP-Hello-ReqPDU after power on.
 *                  0x01    ON  Device does issue a DCP-Hello-ReqPDU after power on.
 *                  0x02    ON  DCP-Hello-ReqPDU after HelloDelay.
 *                 LSA_UINT32   HelloRetry  Number of retransmission of the DCP-Hello-ReqPDU (1..x)
 *                 LSA_UINT32   HelloInterval  SendInterval (in ms)
 *                 LSA_UINT32   HelloDelay  wait this time before first DCP-Hello-ReqPDU (in ms)
 * return value:   LSA_UINT16   Error:      != LSA_RET_OK if DeviceInitiative
 *                                          cannot be changed
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_DEVICE_INITIATIVE(
    LSA_UINT16      NicId,
    LSA_UINT32      HelloMode,
    LSA_UINT32      HelloRetry,
    LSA_UINT32      HelloInterval,
    LSA_UINT32      HelloDelay
)
{
    LSA_UINT16  Ret;
    OHA_HDB_DCP_PTR_TYPE pHDBDcp = OHA_DCPGetHandleByNIC(NicId);
    LSA_UINT8   DevInit[DCP_DS_DEVINIT_SIZE];
    LSA_UINT16  DeviceInitiative;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_DEVICE_INITIATIVE(HelloMode: 0x%X, HelloRetry: 0x%X, HelloDelay: 0x%X)",
                          HelloMode, HelloRetry, HelloDelay);

    if (OHA_IS_NULL(pHDBDcp))    /* no interface */
    {
        Ret = LSA_RET_ERR_RESOURCE;
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
                              "OUT: OHA_DB_WRITE_DEVICE_INITIATIVE(RetVal: 0x%X)", Ret);
        return Ret;
    }

    /* HelloEnable (Bit 0) */
    switch (HelloMode & OHA_DCP_HELLO_MODE_MASK)
    {
        case OHA_DCP_HELLO_OFF:
            DeviceInitiative = OHA_DCP_HELLO_OFF;
            break;
        case OHA_DCP_HELLO_ON:
        case OHA_DCP_HELLO_ON_DELAY:
            DeviceInitiative = OHA_DCP_HELLO_ON;
            break;
        default:
            Ret = LSA_RET_ERR_PARAM;
            OHA_PROGRAM_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_UNEXP,"Request OHA_DB_WRITE_DEVICE_INITIATIVE failed. Invalid mode (HelloMode: 0x%X)",HelloMode);
            return Ret;
    }

    OHA_PUT16_HTON(DevInit, 0, DeviceInitiative);

    Ret = OHA_DB_WRITE_DCP_ELEMENT(NicId,
                                   DCP_DBID_DEVINIT,
                                   DCP_DSID_DEVINIT,
                                   DevInit,
                                   DCP_DS_DEVINIT_SIZE,
                                   OHA_READ_ONLY, LSA_FALSE);

    /* HelloMode, HelloRetry, HelloInterval, HelloDelay */
    pHDBDcp->FSU_DataCommit.HelloMode  = HelloMode;
    pHDBDcp->FSU_DataCommit.HelloInterval = HelloInterval;
    pHDBDcp->FSU_DataCommit.HelloRetry = HelloRetry;
    pHDBDcp->FSU_DataCommit.HelloDelay = HelloDelay;

    /* HelloState */
    pHDBDcp->HelloState = OHA_DCP_STATE_HELLO_WRITTEN;

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_DEVICE_INITIATIVE(RetVal: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_IP_SUITE
 * function:       Set local IP address
 * parameters:     OHA_IP_ADDR IpAddrPtr:               Pointer to IP address
 * parameters:     OHA_IP_ADDR SubnetMaskPtr:           Pointer to subnet mask
 * parameters:     OHA_IP_ADDR RouterAddrPtr:           Pointer to router address
 *                 LSA_INT    AddrLen:                  Length of address
 *                 LSA_UINT8  AccessControl:            Read, Write, Query,...
 *                 LSA_BOOL   Remanent:                 Store value permanently
 * return value:   LSA_UINT16:                          Error
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR OHA_DB_WRITE_IP_SUITE(
    LSA_UINT16                     NicId,
    OHA_IP_TYPE OHA_LOCAL_MEM_ATTR IpAddrPtr,
    OHA_IP_TYPE OHA_LOCAL_MEM_ATTR SubnetMaskPtr,
    OHA_IP_TYPE OHA_LOCAL_MEM_ATTR RouterAddrPtr,
    LSA_INT                        AddrLen,
    LSA_UINT8                      AccessControl,
    LSA_BOOL                       Remanent )
{
    OHA_DB_IF_PTR_TYPE  pDB = OHA_DB_GET_INTERFACE(NicId);
    LSA_UINT16 RetVal = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_05(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_IP_SUITE(NicId: 0x%X, IpAddrPtr: 0x%X, AddrLen: 0x%X, AccessControl: 0x%X, Remanent: 0x%X)",
                          NicId, IpAddrPtr, AddrLen, AccessControl, Remanent);
    if (OHA_IS_NOT_NULL(pDB))
    {
        LSA_UINT8   IpSuite[OHA_IP_SUITE_SIZE];

        OHA_MEMSET(IpSuite, 0, OHA_IP_SUITE_SIZE);

        if (AddrLen == 0 || AddrLen == OHA_IP_ADDR_SIZE)
        {
            /* build the ip suite (IpAddr, SubnetMask, RouterAddr) */
            if (AddrLen == OHA_IP_ADDR_SIZE)
            {
                if (OHA_IS_NOT_NULL(IpAddrPtr))
                {
                    OHA_MEMCPY(&IpSuite, IpAddrPtr, OHA_IP_ADDR_SIZE);
                }
                if (OHA_IS_NOT_NULL(SubnetMaskPtr))
                {
                    OHA_MEMCPY(&IpSuite[OHA_IP_ADDR_SIZE], SubnetMaskPtr, OHA_IP_ADDR_SIZE);
                }
                if (OHA_IS_NOT_NULL(RouterAddrPtr))
                {
                    OHA_MEMCPY(&IpSuite[OHA_IP_ADDR_SIZE+OHA_IP_ADDR_SIZE], RouterAddrPtr, OHA_IP_ADDR_SIZE);
                }
            }

            if (oha_is_valid_ip_address(OHA_GET32_NTOH(&IpSuite, 0),
                                        OHA_GET32_NTOH(&IpSuite, OHA_IP_ADDR_SIZE),
                                        OHA_GET32_NTOH(&IpSuite, OHA_IP_ADDR_SIZE + OHA_IP_ADDR_SIZE),
                                        LSA_FALSE)) /* 0.0.0.0 is allowed by a local set */
            {
                /* first write the ip address in the database */
                RetVal = OHA_DB_WRITE_DCP_ELEMENT(NicId, OHA_DCP_OPTION_TCP, DCP_DSID_TCPIP, IpSuite, OHA_IP_SUITE_SIZE, AccessControl, Remanent);
                OHA_ASSERT (RetVal == LSA_RET_OK);
            }
        }
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_IP_SUITE(RetVal: 0x%X)", RetVal);
    return RetVal;
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_STATION_ID
 * function:       Set station identificator (option 2, suboption 3)
 * parameters:     LSA_UINT8    *   IdPtr:      Ptr to identificator (DeviceIDValue)
 *                 LSA_UINT8        IdLen:      Length of identificator
 *                 LSA_UINT8        AccessControl:  Read, Write, Query,...
 *                 LSA_BOOL         Remanent:   Store value permanently
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if identificator
 *                                              cannot be changed
 *
 * DeviceIDValue (see PNIO-Norm) as Octet String in network byte order:
 * +---------------+------------------------------------------------------+
 * + DeviceIDValue | VendorIDHigh, VendorIDLow, DeviceIDHigh, DeviceIDLow +
 * +---------------+------------------------------------------------------+
 *
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR OHA_DB_WRITE_STATION_ID(
    LSA_UINT16                    NicId,
    LSA_UINT8 OHA_COMMON_MEM_ATTR* IdPtr,
    LSA_INT                       IdLen,
    LSA_UINT8                     AccessControl,
    LSA_BOOL                      Remanent
)
{
    LSA_UINT16  Ret = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_05(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_STATION_ID(NicId:  0x%X, IdPtr: 0x%X, IdLen: 0x%X, AccessControl: 0x%X, Remanent: 0x%X)",
                          NicId, IdPtr, IdLen, AccessControl, Remanent);
    if (IdLen == 0 || (OHA_IS_NOT_NULL(IdPtr) && IdLen && (IdLen <= OHA_MAX_STATION_TYPE_LEN)))
    {
        Ret = OHA_DB_WRITE_DCP_ELEMENT(NicId, OHA_DCP_OPTION_IDENT, DCP_DSID_IDID, IdPtr, (LSA_UINT16)IdLen, AccessControl, Remanent);
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_STATION_ID(RetVal: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_OEM_DEVICE_ID
 * function:       Set station identificator (option 2, suboption 8)
 * parameters:     LSA_UINT8    *   IdPtr:      Ptr to identificator (OEMDeviceID)
 *                 LSA_UINT8        IdLen:      Length of identificator
 *                 LSA_UINT8        AccessControl:  Read, Write, Query,...
 *                 LSA_BOOL         Remanent:   Store value permanently
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if identificator
 *                                              cannot be changed
 *
 * DeviceIDValue (see PNIO-Norm) as Octet String in network byte order:
 * +---------------+------------------------------------------------------+
 * + DeviceIDValue | VendorIDHigh, VendorIDLow, DeviceIDHigh, DeviceIDLow +
 * +---------------+------------------------------------------------------+
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR OHA_DB_WRITE_OEM_DEVICE_ID(
    LSA_UINT16                    NicId,
    LSA_UINT8 OHA_COMMON_MEM_ATTR* IdPtr,
    LSA_INT                       IdLen,
    LSA_UINT8                     AccessControl,
    LSA_BOOL                      Remanent
)
{
    LSA_UINT16  Ret = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_05(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_OEM_DEVICE_ID(NicId:  0x%X, IdPtr: 0x%X, IdLen: 0x%X, AccessControl: 0x%X, Remanent: 0x%X)",
                          NicId, IdPtr, IdLen, AccessControl, Remanent);
    if (IdLen == 0 || (OHA_IS_NOT_NULL(IdPtr) && IdLen && (IdLen <= OHA_MAX_STATION_TYPE_LEN)))
    {
        Ret = OHA_DB_WRITE_DCP_ELEMENT(NicId, OHA_DCP_OPTION_IDENT, DCP_DSID_IDOEMID, IdPtr, (LSA_UINT16)IdLen, AccessControl, Remanent);
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_OEM_DEVICE_ID(RetVal: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_STATION_NAME
 * function:       Set station name (database 8, dataset 2)
 * parameters:     LSA_UINT8    *   NamePtr:    Ptr to name string
 *                 LSA_UINT16       NameLen:    Length of name string
 *                 LSA_UINT8        AccessControl:  Read, Write, Query,...
 *                 LSA_BOOL         Remanent:   Store value permanently
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if station name
 *                                              cannot be changed
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR OHA_DB_WRITE_STATION_NAME(
    LSA_UINT16                    NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    NamePtr,
    LSA_INT                       NameLen,
    LSA_UINT8                     AccessControl,
    LSA_BOOL                      Remanent
)
{
    LSA_UINT16  Ret = LSA_RET_ERR_PARAM;
    LSA_UINT8   ConvertName[OHA_MAX_STATION_NAME_LEN];
    OHA_COMMON_MEM_U8_PTR_TYPE  pConvertName = ConvertName;

    OHA_FUNCTION_TRACE_05(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_STATION_NAME(NicId: 0x%X, NamePtr: 0x%X, NameLen: 0x%X, AccessControl: 0x%X, Remanent: 0x%X)",
                          NicId, NamePtr, NameLen, AccessControl, Remanent);
    if (NameLen <= OHA_MAX_STATION_NAME_LEN)
    {
        if (NameLen == 0)   /* AP00257533: write an empty string */
        {
            Ret = OHA_DB_WRITE_DCP_ELEMENT(NicId, OHA_DCP_OPTION_IDENT, DCP_DSID_IDNAME, NamePtr, (LSA_UINT16)NameLen, AccessControl, Remanent);
            OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_NOTE,
                                  "OUT: OHA_DB_WRITE_STATION_NAME(RetVal: 0x%X)", Ret);
            return Ret;
        }

        OHA_ASSERT_NULL_PTR(NamePtr);
        OHA_MEMCPY(pConvertName, NamePtr, NameLen); /* AP00352120: store olny in lower case */
        if(OHA_UPPER_CASES_ALLOWED(NicId))
        {
            OHA_STRNTOLOWER(pConvertName, NameLen);
		}

        /* check, if it's a RFC-name */
        if (oha_is_valid_station_name((LSA_UINT16)NameLen, NamePtr, OHA_UPPER_CASES_ALLOWED(NicId), OHA_UPPER_CASES_ALLOWED(NicId)))
        {
            Ret = OHA_DB_WRITE_DCP_ELEMENT(NicId, OHA_DCP_OPTION_IDENT, DCP_DSID_IDNAME, pConvertName, (LSA_UINT16)NameLen, AccessControl, Remanent);
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_STATION_NAME(RetVal: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_STATION_TYPE
 * function:       Set station type (option 2, dataset 1)
 * parameters:     LSA_UINT8    *   TypePtr:    Ptr to typee string
 *                 LSA_UINT16       TypeLen:    Length of type string
 *                 LSA_UINT8        AccessControl:  Read, Write, Query,...
 *                 LSA_BOOL         Remanent:   Store value permanently
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if station type
 *                                              cannot be changed
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR OHA_DB_WRITE_STATION_TYPE(
    LSA_UINT16                    NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE    TypePtr,
    LSA_INT                       TypeLen,
    LSA_UINT8                     AccessControl,
    LSA_BOOL                      Remanent
)
{
    LSA_UINT16  Ret = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_05(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_STATION_TYPE(NicId:  0x%X, TypePtr: 0x%X, TypeLen: 0x%X, AccessControl: 0x%X, Remanent: 0x%X)",
                          NicId, TypePtr, TypeLen, AccessControl, Remanent);
    if (TypeLen == 0 || (OHA_IS_NOT_NULL(TypePtr) && (TypeLen <= OHA_MAX_STATION_TYPE_LEN)))
    {
        Ret = OHA_DB_WRITE_DCP_ELEMENT(NicId, OHA_DCP_OPTION_IDENT, DCP_DSID_IDTYPE, TypePtr, (LSA_UINT16)TypeLen, AccessControl, Remanent);
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_STATION_TYPE(RetVal: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_DB_READ_DHCP_OPTION
 * function:       Read DHCP option from the system
 * parameters:     LSA_UINT8    **  OptionPtrPtr:   Ptr to RetPtr
 *                 LSA_UINT8        OptionId:       ID of DHCP-Option to read
 * return value:   LSA_UINT16       !LSA_RET_OK:    DHCP Option not exists
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR OHA_DB_READ_DHCP_OPTION(
    LSA_UINT16                  NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE* OptionPtrPtr,
    LSA_UINT8                   OptionId     /* suboption */
)
{
    LSA_BOOL   Remanent;
    LSA_UINT8  AccessControl;
    LSA_UINT16 Length;
    OHA_COMMON_MEM_U8_PTR_TYPE  pType = LSA_NULL;
    LSA_UINT16 RetVal = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_DHCP_OPTION(NicId: 0x%X, OptionPtrPtr: 0x%X, OptionId: 0x%X)",
                          NicId, OptionPtrPtr, OptionId);
    if (OptionId)   /* valid suboption, Null is invalid */
    {
        RetVal = OHA_DB_READ_DCP_ELEMENT(NicId,
                                         OHA_DCP_OPTION_DHCP,
                                         OptionId,
                                         &pType,
                                         &Length,
                                         &AccessControl,
                                         &Remanent);
    }
    if (RetVal == LSA_RET_OK)
    {
        *OptionPtrPtr = pType;
    }
    else
    {
        *OptionPtrPtr = LSA_NULL;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_DHCP_OPTION(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_DHCP_OPTION
 * function:       Write DHCP option to system
 *                 LSA_UINT8        OptionId:       ID of DHCP-Option
 * parameters:     LSA_UINT8    *   OptionPtr:      Ptr to Option
 * parameters:     LSA_UINT16       Length:         Length of Option
 * return value:   LSA_UINT16       !LSA_RET_OK:    invalid DHCP Option
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR OHA_DB_WRITE_DHCP_OPTION(
    LSA_UINT16                    NicId,
    LSA_UINT8 OHA_COMMON_MEM_ATTR* OptionPtr
)
{
    LSA_UINT16 Ret = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_DHCP_OPTION(NicId: 0x%X, OptionPtr: 0x%X)",
                          NicId, OptionPtr);
    if (OHA_IS_NOT_NULL(OptionPtr) && (*OptionPtr))  /* first byte = Id, suboption */
    {
        LSA_UINT8 AccessMode = OHA_READ_WRITE; /* default */
       (LSA_VOID)OHA_DB_READ_ACCESS_CONTROL(NicId, OHA_DCP_OPTION_DHCP, *OptionPtr, &AccessMode);

        /* option-length is in th first byte */
        /* write-length = length + one byte for option + one byte for length */
        Ret = OHA_DB_WRITE_DCP_ELEMENT(NicId, OHA_DCP_OPTION_DHCP, *OptionPtr,
                                       (OHA_COMMON_MEM_U8_PTR_TYPE)OptionPtr, (LSA_UINT16)(*(OptionPtr+1)+2), AccessMode, LSA_FALSE);
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_DHCP_OPTION(RetVal: 0x%X)", Ret);
    return Ret;
}


/*------------------------------------------------------------------------------------*/
/* returns the string that contains the same characters but in the opposite order.    */
/*------------------------------------------------------------------------------------*/
LSA_VOID oha_strreverse(OHA_COMMON_MEM_U8_PTR_TYPE begin, OHA_COMMON_MEM_U8_PTR_TYPE end)
{
	char aux;
	while(end>begin)
	{
		aux=*end;
		*end--=*begin;
		*begin++=aux;
	}
}

/*------------------------------------------------------------------------------------*/
/*  oha_ultoa converts an unsigned long value to a null-terminated string using the   */
/*  specified base and stores the result in the array given by str parameter.         */
/*  str should be an array long enough to contain any possible value:                 */
/*  (sizeof(long)*8+1) for radix=2, i.e. 33 bytes in 32-bits platforms.               */
/*                                                                                    */
/*  Parameters:                                                                       */
/*  value   Value (only positive) to be converted to a string.                        */
/*  str     Array in memory where to store the resulting null-terminated string.      */
/*  base    Numerical base used to represent the value as a string, between 2 and 16, */
/*          where 10 means decimal base, 16 hexadecimal, 8 octal, and 2 binary.       */
/*  numbers Fills up with leading zeros if numbers <> 0 is given.                     */
/*          If the buffer is too small, you risk a buffer overflow.                   */
/*                                                                                    */
/* returns the number of characters written, not including the terminating            */
/* null character or 0 if an output error occurs.                                     */
/*------------------------------------------------------------------------------------*/
LSA_INT oha_ultoa(LSA_UINT32 value, OHA_COMMON_MEM_U8_PTR_TYPE str, LSA_UINT base, LSA_INT numbers)
{
	LSA_INT count = 0;
	static char num[] = "0123456789abcdef";
	OHA_COMMON_MEM_U8_PTR_TYPE wstr=str;
	
	/* Validate base */
	if (base<2 || base>16){ *wstr='\0'; return count; }
	
	/* Conversion. Number is reversed. */
	do
	{
		*wstr++ = num[value%base];
		count++;
    } while(value/=base);

	/* Fill up with leading zeros */
    while(count < numbers)
    {
    	*wstr++ = '0';
		count++;
    }

	*wstr = '\0';

	/* Reverse string */
	oha_strreverse(str,wstr-1);

	return count;
}

/*----------------------------------------------------------------------------*/
/* MAC to ascii, e.g. "00-0f-01-a8-00-02"                                     */
/* Return Value:                                                              */
/* oha_mac2a returns the number of characters written,                        */
/* not including the terminating null character,                              */
/* the min. length of the buffer of 18 byte must be given !                   */
/*----------------------------------------------------------------------------*/
LSA_UINT16 oha_mac2a (OHA_COMMON_MEM_U8_PTR_TYPE mac,
                      OHA_COMMON_MEM_U8_PTR_TYPE s_mac)
{
    LSA_UINT16 i;
    LSA_INT c;  /* written characters */

	OHA_ASSERT_NULL_PTR(mac);
	OHA_ASSERT_NULL_PTR(s_mac);

	s_mac[0] = '\0';
    for (i = 0, c = 0; i < OHA_MAC_ADDR_SIZE; i++)
    {
        if (i)
        {
			s_mac[c++] = '-';
		}

		c += oha_ultoa(mac[i], &s_mac[c], 16, 2);
		if (c == 0) return 0;

		if (c >= OHA_MAC2A_LEN) { OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ); }	
	}

	s_mac[c+1] = '\0';

    return (LSA_UINT16)c;
}

/*----------------------------------------------------------------------------*/
/* IP to ascii, e.g. "140.80.0.1"                                             */
/* Return Value:                                                              */
/* oha_ip2a returns the number of characters written,                         */
/* not including the terminating null character,                              */
/* the min. length of the buffer of 16 byte must be given !                   */
/*----------------------------------------------------------------------------*/
LSA_UINT16 oha_ip2a (OHA_COMMON_MEM_U8_PTR_TYPE ip,
                     OHA_COMMON_MEM_U8_PTR_TYPE s_ip)
{
    LSA_UINT16 i;
    LSA_INT c;  /* written characters */

	OHA_ASSERT_NULL_PTR(ip);
	OHA_ASSERT_NULL_PTR(s_ip);

	s_ip[0] = '\0';

    for (i = 0, c = 0; i < OHA_IP_ADDR_SIZE; i++)
    {
        if (i)
        {
			s_ip[c++] = '.';
		}

		c += oha_ultoa(ip[i], &s_ip[c], 10, 0);
		if (c == 0) return 0;

		if (c >= OHA_IP2A_LEN) { OHA_FATAL(OHA_FATAL_ERR_INCONSISTENZ); }	
	}

	s_ip[c+1] = '\0';

    return (LSA_UINT16)c;
}

/*----------------------------------------------------------------------------*/
/* OID to ascii, e.g. "1.0.62439.1.1.1.1.1.1"                                 */
/* Return Value:                                                              */
/* oha_oid2a returns the number of characters written,                        */
/* not including the terminating null character,                              */
/* or a negative value if an output error occurs.                             */
/* A negative value is also returned if oidlen or more                        */
/* wide characters are requested to be written.                               */
/*----------------------------------------------------------------------------*/
LSA_INT oha_oid2a (OHA_OID_PTR_TYPE oid, LSA_UINT16 oidlen,
                   OHA_COMMON_MEM_U8_PTR_TYPE s_oid, LSA_UINT16 s_oidlen)
{
    LSA_UINT16 i;
    LSA_INT c;  /* written characters */

	OHA_ASSERT_NULL_PTR(oid);
	OHA_ASSERT_NULL_PTR(s_oid);

	s_oid[0] = '\0';
    for (i = 0, c = 0; i < oidlen; i++)
    {
        if (i)
        {
			s_oid[c++] = '.';
        }

		c += oha_ultoa(oid[i], &s_oid[c], 10, 0);
		if (c == 0) return 0;

		if (c >= s_oidlen) { s_oid[0] = '\0'; return -1; }
    }

	s_oid[c+1] = '\0';

    return c;
}

/*----------------------------------------------------------------------------*/
/* UUID to ascii(), e.g. "00010203-0405-0607-0809-fafbfcfdfeff"               */
/* Return Value:                                                              */
/* oha_oid2a returns the number of characters written,                        */
/* not including the terminating null character,                              */
/* or a negative value if an output error occurs.                             */
/* A negative value is also returned if uuidlen or more                       */
/* wide characters are requested to be written.                               */
/* the min. length of the buffer of 37 byte must be given                     */
/*----------------------------------------------------------------------------*/
LSA_INT OHA_UPPER_IN_FCT_ATTR  oha_uuid2a (
    OHA_COMMON_MEM_U8_PTR_TYPE uuid,
    OHA_COMMON_MEM_U8_PTR_TYPE uuid_string,
    LSA_UINT16 len
)
{
    LSA_UINT16 i;
    LSA_INT c;  /* written characters */

    OHA_ASSERT_NULL_PTR (uuid);
    OHA_ASSERT_NULL_PTR (uuid_string);

    if (len < (2*OHA_UUID_SIZE+4+1))
	{
        return (-1);
	}

	uuid_string[0] = '\0';
    for (i = 0, c = 0; i < OHA_UUID_SIZE; i++)
    {
        if (i==4 || i==6 || i==8 || i==10)
        {
			uuid_string[c++] = '-';
		}

		c += oha_ultoa(uuid[i], &uuid_string[c], 16, 2);
		if (c == 0) return 0;

		if (c > (2*OHA_UUID_SIZE+4)) { uuid_string[0] = '\0'; return 0; }	
	}

	uuid_string[c+1] = '\0';

    return (LSA_UINT16)c;
}

/*=============================================================================
 * function name:  OHA_DB_ALIAS_NAME_CHECK
 * function:       Compare alias name byte array with all the alias names
 * parameters:     LSA_UINT8  *     TypePtr:    Pointer to first byte of
 *                                              station type byte array
 *                 LSA_INT:         TypeLen:    Length of station type array
 * return value:   LSA_TRUE         if alias name exists
 *===========================================================================*/
LSA_BOOL OHA_DB_ALIAS_NAME_CHECK(
    LSA_UINT16                    NicId,
    LSA_UINT8 OHA_LOCAL_MEM_ATTR* AliasPtr,
    LSA_INT                       AliasLen
)
{
    LSA_BOOL                    Found = LSA_FALSE;
    LSA_UINT16                  PortId;
    OHA_DB_IF_PTR_TYPE          pDB = OHA_DB_GET_INTERFACE(NicId);

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_ALIAS_NAME_CHECK(AliasPtr: 0x%X, AliasLen: 0x%X)",
                          AliasPtr, AliasLen);

    if (AliasLen == 0 || OHA_IS_NULL(AliasPtr)|| OHA_IS_NULL(pDB))
    {
        OHA_FUNCTION_TRACE_01(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_DB_ALIAS_NAME_CHECK(Found: 0x%X)", Found);
        return Found;
    }

    /* read the neighbours and build the alias name */
    for (PortId = 1; PortId <= OHA_CFG_MAX_PORTS; PortId++)
    {
        LSA_UINT16 SenderId;

        /* AP00636031: in case of multiple sender (neighbours) no alias shall be given */
        if (pDB->LldpData[PortId-1].LldpSenderStatus != OHA_STATUS_DATA_VALID &&
            pDB->LldpData[PortId-1].LldpSenderStatus != OHA_STATUS_DATA_INVALID)
        {
            continue;
        }

        for (SenderId = 1; SenderId <= LLDP_MAX_PORT_SENDER; SenderId++)
        {
            OHA_COMMON_MEM_U8_PTR_TYPE      NamePtr = LSA_NULL;
            LSA_UINT16                      Length  = 0;
            LSA_UINT8                       SubType = 0;
            LSA_UINT8                       SenderStatus;

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

                if (Length != 0 && OHA_IS_NOT_NULL(NamePtr))
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

						if (PortLength != 0 && OHA_IS_NOT_NULL(PortPtr))
						{
							/* compare length and value of alias name */
							if ((Length + PortLength + 1) == AliasLen)
							{
								/* AliasNameValue := LLDP_PortID + "." + LLDP_ChassisID */
								Found = oha_namecmp(AliasPtr, PortPtr, PortLength, OHA_UPPER_CASES_ALLOWED(pDB->NicId));
								if (Found)
								{
									Found = oha_namecmp(AliasPtr+PortLength, (OHA_COMMON_MEM_U8_PTR_TYPE)".", 1, OHA_UPPER_CASES_ALLOWED(pDB->NicId));
									if (Found)
									{
										Found = oha_namecmp(AliasPtr+PortLength+1, NamePtr, Length, OHA_UPPER_CASES_ALLOWED(pDB->NicId));
										if (Found)
										{
											OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
																  "OUT: OHA_DB_ALIAS_NAME_CHECK(Found: 0x%X)", Found);
											return Found;
										}
									}
								}
							}
						}
					}
                }
            }
        }
    }

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_ALIAS_NAME_CHECK(Found: 0x%X)", Found);
    return Found;
}

/*=============================================================================
 * function name:  OHA_TOLOWER
 *
 * function:    convert uppercase letter to lowercase letter
 *
 * description: OhaToLower will return the ASCII value for the lowercase equivalent
 *              of the ASCII character chr, if it is a letter. If the character
 *              is already lowercase, it remains lowercase
 *
 *===========================================================================*/
LSA_INT OHA_TOLOWER(LSA_INT ch)
{
    if (ch >= 'A' && ch <= 'Z')
    {
        return ch|32;
    }
    return ch;
}

/*=============================================================================
 * function name:  OHA_STRNTOLOWER
 *
 * function:    make a string lowercase
 *
 * description: OhaStrnToLower makes string with all alphabetic characters converted to lowercase.
 *
 *===========================================================================*/
LSA_VOID OHA_STRNTOLOWER(OHA_COMMON_MEM_U8_PTR_TYPE pStr, LSA_INT Len)
{
    LSA_INT I;

    for (I = 0; I < (Len); I++)
    {
        *(pStr+I) = (LSA_UINT8)OHA_TOLOWER(*(pStr+I));
    }
    return;
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_DEVICE_INSTANCE
 * function:       Set device instance (option 2, suboption 7)
 * parameters:     LSA_UINT16       DeviceInstance
 *                 LSA_UINT8        AccessControl:  Read, Write, Query,...
 *                 LSA_BOOL         Remanent:   Store value permanently
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if instance
 *                                              cannot be changed
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_OUT_FCT_ATTR OHA_DB_WRITE_DEVICE_INSTANCE(
    LSA_UINT16                    NicId,
    LSA_UINT16                    DeviceInstance,
    LSA_UINT8                     AccessControl,
    LSA_BOOL                      Remanent)
{
    LSA_UINT16  Ret;
    LSA_UINT8   DevInst[sizeof(DeviceInstance)];

    OHA_FUNCTION_TRACE_03(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_DEVICE_INSTANCE(DeviceInstance: 0x%X, AccessControl: 0x%X, Remanent: 0x%X)",
                          DeviceInstance, AccessControl, Remanent);

    OHA_PUT16_HTON(DevInst, 0, DeviceInstance);

    Ret = OHA_DB_WRITE_DCP_ELEMENT(NicId,
                                   OHA_DCP_OPTION_IDENT,
                                   DCP_DSID_IDDEVINST,
                                   DevInst,
                                   sizeof(DeviceInstance),
                                   AccessControl, Remanent);

    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_DEVICE_INSTANCE(RetVal: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_DB_READ_DEVICE_INSTANCE
 * function:       Get device instance (option 2, suboption 7)
 * parameters:     LSA_UINT8    **  InstPtr:    Ptr to RetPtr (Identificator)
 * return value:   LSA_UINT8        Length:     Length of identificator (2) 
 *                                              or 0 if not present in station
  * DeviceInstanceValue (see PNIO-Norm) as Octet String in network byte order:
 * +---------------------+---------------------------------------------------+
 * + DeviceInstanceValue | InstanceHigh (U8), InstanceLow (U8)               +
 * +---------------------+---------------------------------------------------+
 *===========================================================================*/
LSA_UINT8 OHA_SYSTEM_OUT_FCT_ATTR  OHA_DB_READ_DEVICE_INSTANCE(
    LSA_UINT16                    NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE * InstPtr
)
{
    LSA_BOOL   Remanent;
    LSA_UINT8  AccessControl;
    LSA_UINT16 Length = 0;
    OHA_COMMON_MEM_U8_PTR_TYPE pType = LSA_NULL;
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_DEVICE_INSTANCE(NicId: 0x%X, InstPtr: 0x%X)", NicId, InstPtr);
    RetVal = OHA_DB_READ_DCP_ELEMENT(NicId,
                                     OHA_DCP_OPTION_IDENT,
                                     DCP_DSID_IDDEVINST,
                                     &pType,
                                     &Length,
                                     &AccessControl,
                                     &Remanent);
    if (RetVal == LSA_RET_OK && Length == sizeof(LSA_UINT16))
    {
        *InstPtr = pType;
        OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                              "OUT: OHA_DB_READ_DEVICE_INSTANCE(RetVal: 0x%X)", RetVal);
        return (LSA_UINT8)Length;
    }
    *InstPtr = LSA_NULL;
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_DEVICE_INSTANCE(RetVal: 0x%X)", RetVal);
    return(0);
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_PROFIBUS_ADDITION
 * function:       Set stations profibus parameters (database 1, dataset 4)
 * parameters:     DCP_PBPROFIL *   ParamPtr:   Ptr to structure
 * return value:   LSA_UINT16       Error:      != LSA_RET_OK if additional
 *                                              parameters cannot be changed
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR OHA_DB_WRITE_PROFIBUS_ADDITION(
    LSA_UINT16                      NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE      ParamPtr,
	LSA_BOOL                        IsRema
)
{
    LSA_UINT16  Ret = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_PROFIBUS_ADDITION(NicId: 0x%X, ParamPtr: 0x%X, IsRema: 0x%X)",
                          NicId, ParamPtr, IsRema);
    if (!(LSA_HOST_PTR_ARE_EQUAL(ParamPtr, LSA_NULL)))
    {
        Ret = OHA_DB_WRITE_DCP_ELEMENT(NicId, DCP_DBID_PBPARAM, DCP_DSID_PBADDITION,
                                       ParamPtr, DCP_DS_PBADDITION_SIZE, OHA_QUERY_WRITE, IsRema);
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_PROFIBUS_ADDITION(RetVal: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_DB_WRITE_PROFIBUS_COMMON
 * function:       Set stations profibus profile (database 1, dataset 3)
 * parameters:     DCP_PBPROFIL *   ProfilePtr: Ptr to structure
 * return value:   LSA_UINT16   Error:      != LSA_RET_OK if common
 *                                          parameters cannot be changed
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR OHA_DB_WRITE_PROFIBUS_COMMON(
    LSA_UINT16                       NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE       ParamPtr,
	LSA_BOOL                         IsRema
)
{
    LSA_UINT16  Ret = LSA_RET_ERR_PARAM;

    OHA_FUNCTION_TRACE_03(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_WRITE_PROFIBUS_COMMON(NicId: 0x%X, ParamPtr: 0x%X, IsRema: 0x%X)",
                          NicId, ParamPtr, IsRema);
    if (!(LSA_HOST_PTR_ARE_EQUAL(ParamPtr, LSA_NULL)))
    {
        Ret = OHA_DB_WRITE_DCP_ELEMENT(NicId, DCP_DBID_PBPARAM, DCP_DSID_PBCOMMON,
                                       ParamPtr, DCP_DS_PBCOMMON_SIZE, OHA_QUERY_WRITE, IsRema);
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_WRITE_PROFIBUS_COMMON(RetVal: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_DB_READ_PROFIBUS_COMMON
 * function:       Get profibus common parameters (database 1, dataset 3)
 * parameters:     OHA_COMMON_MEM_U8_PTR_TYPE*  ParamPtrPtr:  Ptr to structure ptr
 * return value:   LSA_UINT16  Error:      != LSA_RET_OK if parameters
 *                                         not existing or length not 28 byte
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR OHA_DB_READ_PROFIBUS_COMMON(
    LSA_UINT16                 NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE* ParamPtrPtr
)
{
    LSA_BOOL   Remanent;
    LSA_UINT8  AccessControl;
    LSA_UINT16 Length = 0;
    OHA_COMMON_MEM_U8_PTR_TYPE  pType = LSA_NULL;
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_PROFIBUS_COMMON(NicId: 0x%X, ParamPtrPtr: 0x%X)",
                          NicId, ParamPtrPtr);
    RetVal = OHA_DB_READ_DCP_ELEMENT(NicId,
                                     DCP_DBID_PBPARAM,
                                     DCP_DSID_PBCOMMON,
                                     &pType,
                                     &Length,
                                     &AccessControl,
                                     &Remanent);
    if (RetVal == LSA_RET_OK)
    {
        *ParamPtrPtr = pType;
		OHA_ASSERT(Length == DCP_DS_PBCOMMON_SIZE);
    }
    else
    {
        *ParamPtrPtr = LSA_NULL;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_PROFIBUS_COMMON(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*=============================================================================
 * function name:  OHA_DB_READ_PROFIBUS_ADDITION
 * function:       Get profibus additional parameters (database 1, dataset 4)
 * parameters:     DCP_PBPROFIL **  ParamPtrPtr:    Ptr to structure ptr
 * return value:   LSA_UINT16   Error:   != LSA_RET_OK parameters
 *                                       not existing or length not 26 byte
 *===========================================================================*/
LSA_UINT16 OHA_SYSTEM_IN_FCT_ATTR OHA_DB_READ_PROFIBUS_ADDITION(
    LSA_UINT16                NicId,
    OHA_COMMON_MEM_U8_PTR_TYPE* ParamPtrPtr
)
{
    LSA_BOOL   Remanent;
    LSA_UINT8  AccessControl;
    LSA_UINT16 Length;
    OHA_COMMON_MEM_U8_PTR_TYPE    pType = LSA_NULL;
    LSA_UINT16 RetVal;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_READ_PROFIBUS_ADDITION(NicId: 0x%X, ParamPtrPtr: 0x%X)",
                          NicId, ParamPtrPtr);
    RetVal = OHA_DB_READ_DCP_ELEMENT(NicId,
                                     DCP_DBID_PBPARAM,
                                     DCP_DSID_PBADDITION,
                                     &pType,
                                     &Length,
                                     &AccessControl,
                                     &Remanent);
    if (RetVal == LSA_RET_OK)
    {
        *ParamPtrPtr = pType;
		OHA_ASSERT(Length == DCP_DS_PBADDITION_SIZE);
    }
    else
    {
        *ParamPtrPtr = LSA_NULL;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_READ_PROFIBUS_ADDITION(RetVal: 0x%X)", RetVal);
    return (RetVal);
}

/*=============================================================================
 * function name:  OHA_DB_STATION_TYPE_CHECK
 * function:       Compare station type byte array with local station type
 * parameters:     LSA_UINT8  *     TypePtr:    Pointer to first byte of
 *                                              station type byte array
 *                 LSA_INT:         TypeLen:    Length of station type array
 * return value:   LSA_TRUE         if station types are identically
 *===========================================================================*/
LSA_BOOL OHA_DB_STATION_TYPE_CHECK(
    LSA_UINT16                    NicId,
    LSA_UINT8 OHA_LOCAL_MEM_ATTR* TypePtr,
    LSA_INT                       TypeLen
)
{
    OHA_COMMON_MEM_U8_PTR_TYPE p_station_type = LSA_NULL;
    LSA_INT               len;
    LSA_BOOL   Ret = LSA_FALSE;

    OHA_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_DB_STATION_TYPE_CHECK(TypePtr: 0x%X, TypeLen: 0x%X)",
                          TypePtr, TypeLen);
    len = OHA_DB_READ_STATION_TYPE(NicId, &p_station_type);
    OHA_ASSERT_NULL_PTR(p_station_type);

    /* Check length and value of station type */
    if (len == TypeLen)
    {
        Ret = OHA_MEMCMP(p_station_type, TypePtr, len);
    }
    /* DCP_SERVER_PBC */
    if (!Ret)
    {
        /* if it is another station type. Additional for PBC:
           Send an answer, if the asked station type = PBC_ND and
           the response flag indicates "send answer"  */
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_DB_STATION_TYPE_CHECK(Ret: 0x%X)", Ret);
    return Ret;
}

/*=============================================================================
 * function name:  OHA_UPPER_CASES_ALLOWED
 * function:       Get pointer to and length of station name string
 * parameters:     OHA_SYS_BYTE_PTR_TYPE * NamePtrPtr:  Pointer for Ret.Ptr
 * return value:   LSA_INT:                             Length
 *===========================================================================*/
LSA_BOOL  OHA_SYSTEM_OUT_FCT_ATTR  OHA_UPPER_CASES_ALLOWED(
    LSA_UINT16  NicId
)
{
    OHA_HDB_EDD_PTR_TYPE pHDBEdd = OHA_EDDGetHandleByNIC(NicId);
    LSA_BOOL   UpperCasesAllowed = LSA_FALSE;

    OHA_FUNCTION_TRACE_02(OHA_DbGetTrcIdx(NicId), LSA_TRACE_LEVEL_CHAT,
                          "IN : OHA_UPPER_CASES_ALLOWED(NicId: 0x%X), pHDBEdd: 0x%X", NicId, pHDBEdd);

    if (OHA_IS_NOT_NULL(pHDBEdd))
    {
		OHA_ASSERT_NULL_PTR(pHDBEdd->Params.pDetail);
        UpperCasesAllowed = pHDBEdd->Params.pDetail->Params.Edd.NoS_allow_upper_cases;
    }
    OHA_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,
                          "OUT: OHA_UPPER_CASES_ALLOWED(UpperCasesAllowed: 0x%X)", UpperCasesAllowed);
    return(UpperCasesAllowed);
}

/*****************************************************************************/
/*  end of file OHA_SYS.C                                                    */
/*****************************************************************************/

