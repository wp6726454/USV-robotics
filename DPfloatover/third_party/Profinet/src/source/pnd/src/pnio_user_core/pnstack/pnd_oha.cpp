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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_oha.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*                                                                           */
/*  base adaption for OHA                                                    */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID 3602
#define PND_MODULE_ID   3602

#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_trc.h"

/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */

#include "pnd_pnstack.h"

#include "psi_cfg.h"
#include "psi_usr.h"
#include "psi_sys.h"

#include "oha_inc.h"


/* StdLib++ */
#include <vector>
#include <string>

using namespace std;

#include "pnd_ValueHelper.h"

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define PND_OHA_LLDP_LIST_ENTRIES   14
#define PND_OHA_REMA_DATA_LENGTH    0x4000  // 16k

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

LSA_VOID_PTR_TYPE pnd_oha_alloc_rqb( LSA_OPCODE_TYPE opcode, LSA_HANDLE_TYPE handle, PND_HANDLE_PTR_TYPE pnd_sys )
{
	// Allocate an user RQB for opcode
	CValueHelper           valObj;
	OHA_UPPER_RQB_PTR_TYPE pRQB;
	PSI_SYS_TYPE           psi_sys;
	LSA_USER_ID_TYPE       dummy_user_id = {0};

	PND_ASSERT( pnd_sys != 0 );

	// Prepare the "PSI sysptr" used from OHA to address the pools over output macros
	// Note: only the PSI addressing elements are set
	pnd_memset( &psi_sys, 0, sizeof( PSI_SYS_TYPE ));

	valObj.PndHandle2SysPtrPSI( pnd_sys, &psi_sys );

	OHA_ALLOC_UPPER_RQB_LOCAL(&pRQB, sizeof(*pRQB));
	PND_ASSERT(pRQB);

	pnd_memset( pRQB, 0, sizeof(*pRQB));

	OHA_RQB_SET_OPCODE( pRQB, opcode );
	OHA_RQB_SET_HANDLE( pRQB, handle );
	LSA_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_OHA );

	switch( opcode ) // alloc additional data based on opcode
	{
	case OHA_OPC_DCP_INDICATION:
		{
			OHA_ALLOC_UPPER_MEM((LSA_VOID**)&pRQB->args.DcpInd.pStationData, dummy_user_id, sizeof(OHA_STATION_TYPE), &psi_sys );
			PND_ASSERT(pRQB->args.DcpInd.pStationData);
			pnd_memset(pRQB->args.DcpInd.pStationData, 0, sizeof(OHA_STATION_TYPE));
			OHA_ALLOC_UPPER_MEM(&pRQB->args.DcpInd.pStationData->pStationName, dummy_user_id, 255, &psi_sys );
			PND_ASSERT(pRQB->args.DcpInd.pStationData->pStationName);
		}
		break;
	case OHA_OPC_HELLO_INDICATION:
		{
			pRQB->args.HelloInd.StationNameLen = OHA_MAX_STATION_NAME_LEN;
			OHA_ALLOC_UPPER_MEM((LSA_VOID**)&pRQB->args.HelloInd.pStationName, dummy_user_id, OHA_MAX_STATION_NAME_LEN, &psi_sys );
			PND_ASSERT(pRQB->args.HelloInd.pStationName);
			pnd_memset(pRQB->args.HelloInd.pStationName, 0, OHA_MAX_STATION_NAME_LEN);
		}
		break;
	case OHA_OPC_REMA_READ:
	case OHA_OPC_REMA_INDICATION:
		break;
	case OHA_OPC_PRM_INDICATION:
		{
			OHA_ALLOC_UPPER_MEM((LSA_VOID**)&pRQB->args.PrmInd.diag_ptr, dummy_user_id, OHA_TOPO_MAX_IND_DATA_SIZE, &psi_sys );
		}
		break;
	case OHA_OPC_GET_NEIGHBOUR:
		{
			OHA_ALLOC_UPPER_MEM((LSA_VOID**)&pRQB->args.Neighbour.pLLDPDU, dummy_user_id, sizeof(OHA_LLDPDU_TYPE), &psi_sys );
			pnd_memset(pRQB->args.Neighbour.pLLDPDU, 0, sizeof(OHA_LLDPDU_TYPE));
		}
		break;
	case OHA_OPC_ADDR_INFO_IND:
		{
			OHA_ALLOC_UPPER_MEM((LSA_VOID**)&pRQB->args.AddrInfo.NoS.pName, dummy_user_id, OHA_MAX_STATION_NAME_LEN, &psi_sys);
			PND_ASSERT(pRQB->args.AddrInfo.NoS.pName);
			pnd_memset(pRQB->args.AddrInfo.NoS.pName, 0, OHA_MAX_STATION_NAME_LEN);
		}
		break;
	case OHA_OPC_GET_ADDRESS:
	case OHA_OPC_SET_ADDRESS:
	case OHA_OPC_SET_EMERGENCY:
		{
			OHA_ALLOC_UPPER_MEM((LSA_VOID**)&pRQB->args.Address.pStationData, dummy_user_id, sizeof(OHA_STATION_TYPE), &psi_sys );
			PND_ASSERT(pRQB->args.Address.pStationData);
			pnd_memset(pRQB->args.Address.pStationData, 0, sizeof(OHA_STATION_TYPE));
			pRQB->args.Address.pStationData->StationNameLen = OHA_MAX_STATION_NAME_LEN;
		}
		break;
	case OHA_OPC_LLDP_CONTROL_OID:
	case OHA_OPC_MRP_CONTROL_OID:
		{
			OHA_ALLOC_UPPER_MEM((LSA_VOID**)&pRQB->args.SnmpOid.pVarValue, dummy_user_id, OHA_MAX_STATION_NAME_LEN, &psi_sys );
			PND_ASSERT(pRQB->args.SnmpOid.pVarValue);
			pnd_memset(pRQB->args.SnmpOid.pVarValue, 0, sizeof(OHA_STATION_TYPE));
		}
		break;
	case OHA_OPC_DCP_WRITE_ELEMENT:
		{
			OHA_ALLOC_UPPER_MEM((LSA_VOID**)&pRQB->args.DcpElem.pVariable, dummy_user_id, OHA_MAX_STATION_NAME_LEN, &psi_sys );
			PND_ASSERT(pRQB->args.DcpElem.pVariable);
			pnd_memset(pRQB->args.DcpElem.pVariable, 0, OHA_MAX_STATION_NAME_LEN);
		}
		break;
	case OHA_OPC_LLDP_WRITE_OPTION:
		{
			OHA_ALLOC_UPPER_MEM((LSA_VOID**)&pRQB->args.LldpOption.pVariable, dummy_user_id, OHA_MAX_STATION_NAME_LEN, &psi_sys );
			PND_ASSERT(pRQB->args.LldpOption.pVariable);
			pnd_memset(pRQB->args.LldpOption.pVariable, 0, OHA_MAX_STATION_NAME_LEN);
		}
		break;
	case OHA_OPC_LLDP_READ_ELEMENT:
		{
			OHA_ALLOC_UPPER_MEM((LSA_VOID**)&pRQB->args.LldpElem.pElements, dummy_user_id, sizeof(OHA_DB_LLDP_ELEM_TYPE) * PND_OHA_LLDP_LIST_ENTRIES, &psi_sys );
			PND_ASSERT(pRQB->args.LldpElem.pElements);
			pnd_memset(pRQB->args.LldpElem.pElements, 0, sizeof(OHA_DB_LLDP_ELEM_TYPE) * PND_OHA_LLDP_LIST_ENTRIES);
			OHA_ALLOC_UPPER_MEM((LSA_VOID**)&pRQB->args.LldpElem.pVarBuffer, dummy_user_id, OHA_MAX_TLV_DATA_SIZE, &psi_sys );
			PND_ASSERT(pRQB->args.LldpElem.pVarBuffer);
		}
		break;
	case OHA_OPC_LLDP_READ_OPTIONS:
		{
			OHA_ALLOC_UPPER_MEM((LSA_VOID**)&pRQB->args.LldpOptions.pElements, dummy_user_id, sizeof(OHA_DB_LLDP_ELEM_TYPE) * PND_OHA_LLDP_LIST_ENTRIES, &psi_sys );
			PND_ASSERT(pRQB->args.LldpOptions.pElements);
			pnd_memset(pRQB->args.LldpOptions.pElements, 0, sizeof(OHA_DB_LLDP_ELEM_TYPE) * PND_OHA_LLDP_LIST_ENTRIES);
			OHA_ALLOC_UPPER_MEM((LSA_VOID**)&pRQB->args.LldpOptions.pVarBuffer, dummy_user_id, OHA_MAX_TLV_DATA_SIZE, &psi_sys );
			PND_ASSERT(pRQB->args.LldpOptions.pVarBuffer);
		}
		break;
	case OHA_OPC_READ_QUERY_ELEMENTS:
		{
			OHA_ALLOC_UPPER_MEM((LSA_VOID**)&pRQB->args.QueryElem.pElements, dummy_user_id, sizeof(OHA_DB_ELEM_TYPE) * PND_OHA_LLDP_LIST_ENTRIES, &psi_sys );
			PND_ASSERT(pRQB->args.QueryElem.pElements);
			OHA_ALLOC_UPPER_MEM((LSA_VOID**)&pRQB->args.QueryElem.pVarBuffer, dummy_user_id, OHA_MAX_TLV_DATA_SIZE, &psi_sys );
			PND_ASSERT(pRQB->args.QueryElem.pVarBuffer);
		}
		break;
	case OHA_OPC_PRM_WRITE:
	case OHA_OPC_PRM_READ:
		{
			OHA_ALLOC_UPPER_MEM((LSA_VOID**)&pRQB->args.PrmData.record_data, dummy_user_id, OHA_MAX_TLV_DATA_SIZE, &psi_sys );
			PND_ASSERT(pRQB->args.PrmData.record_data);
			pnd_memset(pRQB->args.PrmData.record_data, 0, OHA_MAX_TLV_DATA_SIZE);
			pRQB->args.PrmData.record_data_length = OHA_MAX_TLV_DATA_SIZE;
		}
		break;
	default:
		break;
	}

	return( pRQB );
}

/*----------------------------------------------------------------------------*/
LSA_VOID pnd_oha_free_rqb( LSA_VOID_PTR_TYPE p_rqb, PND_HANDLE_PTR_TYPE pnd_sys )
{
	CValueHelper valObj;
	LSA_UINT16   rc;
	PSI_SYS_TYPE psi_sys;

	OHA_UPPER_RQB_PTR_TYPE pRQB = (OHA_UPPER_RQB_PTR_TYPE)p_rqb;
	PND_ASSERT( pRQB != LSA_NULL );
	PND_ASSERT( pnd_sys != 0 );

	// Prepare the "PSI sysptr" used from OHA to address the pools over output macros
	// Note: only the PSI addressing elements are set
	pnd_memset( &psi_sys, 0, sizeof( PSI_SYS_TYPE ));

	valObj.PndHandle2SysPtrPSI( pnd_sys, &psi_sys );

	switch ( OHA_RQB_GET_OPCODE(pRQB) ) // Free additional data based on opcode
	{
	case OHA_OPC_DCP_INDICATION:
		{
			if ( pRQB->args.DcpInd.pStationData != LSA_NULL )
			{
				if ( pRQB->args.DcpInd.pStationData->pStationName != LSA_NULL )
				{
					OHA_FREE_UPPER_MEM(&rc, pRQB->args.DcpInd.pStationData->pStationName, &psi_sys );
					PND_ASSERT(rc == LSA_RET_OK);
				}

				OHA_FREE_UPPER_MEM(&rc, pRQB->args.DcpInd.pStationData, &psi_sys );
				PND_ASSERT(rc == LSA_RET_OK);
			}
		}
		break;
	case OHA_OPC_PRM_INDICATION:
		{
			if ( pRQB->args.PrmInd.diag_ptr != LSA_NULL )
			{
				OHA_FREE_UPPER_MEM(&rc, pRQB->args.PrmInd.diag_ptr, &psi_sys );
				PND_ASSERT(rc == LSA_RET_OK);
			}
		}
		break;
	case OHA_OPC_HELLO_INDICATION:
		{
			if ( pRQB->args.HelloInd.pStationName != LSA_NULL )
			{
				OHA_FREE_UPPER_MEM(&rc, pRQB->args.HelloInd.pStationName, &psi_sys);
				PND_ASSERT(rc == LSA_RET_OK);
			}
		}
		break;
	case OHA_OPC_REMA_INDICATION:
	case OHA_OPC_REMA_READ:
		{
			if ( pRQB->args.RemaData.data != LSA_NULL )
			{
				OHA_FREE_UPPER_MEM(&rc, pRQB->args.RemaData.data, &psi_sys);
				PND_ASSERT(rc == LSA_RET_OK);
			}
		}
		break;
	case OHA_OPC_GET_NEIGHBOUR:
		{
			if ( pRQB->args.Neighbour.pLLDPDU != LSA_NULL )
			{
				OHA_FREE_UPPER_MEM(&rc, pRQB->args.Neighbour.pLLDPDU, &psi_sys);
				PND_ASSERT(rc == LSA_RET_OK);
			}
		}
		break;
	case OHA_OPC_LLDP_CONTROL_OID:
	case OHA_OPC_MRP_CONTROL_OID:
		{
			if ( pRQB->args.SnmpOid.pVarValue != LSA_NULL )
			{
				OHA_FREE_UPPER_MEM(&rc, pRQB->args.SnmpOid.pVarValue, &psi_sys);
				PND_ASSERT(rc == LSA_RET_OK);
			}
		}
		break;
	case OHA_OPC_LLDP_WRITE_OPTION:
		{
			OHA_FREE_UPPER_MEM(&rc, pRQB->args.LldpOption.pVariable, &psi_sys);
			PND_ASSERT(rc == LSA_RET_OK);
		}
		break;
	case OHA_OPC_ADDR_INFO_IND:
		{
			if ( pRQB->args.AddrInfo.NoS.pName != LSA_NULL )
			{
				OHA_FREE_UPPER_MEM(&rc, pRQB->args.AddrInfo.NoS.pName, &psi_sys);
				PND_ASSERT(rc == LSA_RET_OK);
			}
		}
		break;
	case OHA_OPC_GET_ADDRESS:
	case OHA_OPC_SET_ADDRESS:
	case OHA_OPC_SET_EMERGENCY:
		{
			if ( pRQB->args.Address.pStationData != LSA_NULL )
			{
				if ( pRQB->args.Address.pStationData->pStationName != LSA_NULL )
				{
					OHA_FREE_UPPER_MEM(&rc, pRQB->args.Address.pStationData->pStationName, &psi_sys);
					PND_ASSERT(rc == LSA_RET_OK);
				}

				OHA_FREE_UPPER_MEM(&rc, pRQB->args.Address.pStationData, &psi_sys);
				PND_ASSERT(rc == LSA_RET_OK);
			}
		}
		break;
	case OHA_OPC_LLDP_READ_ELEMENT:
		{
			if ( pRQB->args.LldpElem.pElements != LSA_NULL )
			{
				OHA_FREE_UPPER_MEM(&rc, pRQB->args.LldpElem.pElements, &psi_sys);
				PND_ASSERT(rc == LSA_RET_OK);
			}

			if ( pRQB->args.LldpElem.pVarBuffer != LSA_NULL )
			{
				OHA_FREE_UPPER_MEM(&rc, pRQB->args.LldpElem.pVarBuffer, &psi_sys);
				PND_ASSERT(rc == LSA_RET_OK);
			}
		}
		break;
	case OHA_OPC_DCP_WRITE_ELEMENT:
		{
			if ( pRQB->args.DcpElem.pVariable != LSA_NULL )
			{
				OHA_FREE_UPPER_MEM(&rc, pRQB->args.DcpElem.pVariable, &psi_sys);
				PND_ASSERT(rc == LSA_RET_OK);
			}
		}
		break;
	case OHA_OPC_LLDP_READ_OPTIONS:
		{
			if ( pRQB->args.LldpOptions.pElements != LSA_NULL )
			{
				OHA_FREE_UPPER_MEM(&rc, pRQB->args.LldpOptions.pElements, &psi_sys);
				PND_ASSERT(rc == LSA_RET_OK);
			}
			if ( pRQB->args.LldpOptions.pVarBuffer != LSA_NULL )
			{
				OHA_FREE_UPPER_MEM(&rc, pRQB->args.LldpOptions.pVarBuffer, &psi_sys);
				PND_ASSERT(rc == LSA_RET_OK);
			}
		}
		break;
	case OHA_OPC_READ_QUERY_ELEMENTS:
		{
			if ( pRQB->args.QueryElem.pElements != LSA_NULL )
			{
				OHA_FREE_UPPER_MEM(&rc, pRQB->args.QueryElem.pElements, &psi_sys);
				PND_ASSERT(rc == LSA_RET_OK);
			}
			if ( pRQB->args.QueryElem.pVarBuffer != LSA_NULL )
			{
				OHA_FREE_UPPER_MEM(&rc, pRQB->args.QueryElem.pVarBuffer, &psi_sys);
				PND_ASSERT(rc == LSA_RET_OK);
			}
		}
		break;
	case OHA_OPC_PRM_WRITE:
	case OHA_OPC_PRM_READ:
		{
			if ( pRQB->args.PrmData.record_data != LSA_NULL )
			{
				OHA_FREE_UPPER_MEM(&rc, pRQB->args.PrmData.record_data, &psi_sys);
				PND_ASSERT(rc == LSA_RET_OK);
			}
		}
		break;
	default:
		break;
	}

	OHA_FREE_UPPER_RQB_LOCAL(&rc, pRQB );
	PND_ASSERT( rc == LSA_RET_OK );
}

/*----------------------------------------------------------------------------*/
LSA_VOID_PTR_TYPE pnd_oha_alloc_upper_mem( LSA_UINT16 len, PND_HANDLE_PTR_TYPE pnd_sys )
{
	CValueHelper      valObj;
	LSA_VOID_PTR_TYPE p_ptr;
	PSI_SYS_TYPE      psi_sys;
	LSA_USER_ID_TYPE  dummy_user_id = {0};

	PND_ASSERT( pnd_sys != 0 );

	// Prepare the "PSI sysptr" used from OHA to address the pools over output macros
	// Note: only the PSI addressing elements are set
	pnd_memset( &psi_sys, 0, sizeof( PSI_SYS_TYPE ));

	valObj.PndHandle2SysPtrPSI( pnd_sys, &psi_sys );

	OHA_ALLOC_UPPER_MEM(&p_ptr, dummy_user_id, len, &psi_sys );
	PND_ASSERT( p_ptr != 0 );

	return ( p_ptr );
}

/*----------------------------------------------------------------------------*/
LSA_VOID pnd_oha_free_upper_mem( LSA_VOID_PTR_TYPE p_mem, PND_HANDLE_PTR_TYPE pnd_sys )
{
	CValueHelper valObj;
	LSA_UINT16   rc;
	PSI_SYS_TYPE psi_sys;

	PND_ASSERT( pnd_sys != 0 );

	// Prepare the "PSI sysptr" used from OHA to address the pools over output macros
	// Note: only the PSI addressing elements are set
	pnd_memset( &psi_sys, 0, sizeof( PSI_SYS_TYPE ));

	valObj.PndHandle2SysPtrPSI( pnd_sys, &psi_sys );

	OHA_FREE_UPPER_MEM(&rc, p_mem, &psi_sys);
	PND_ASSERT(rc == LSA_RET_OK);
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
