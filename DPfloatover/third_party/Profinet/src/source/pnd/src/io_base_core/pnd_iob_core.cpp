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
/*  F i l e               &F: pnd_iob_core.cpp                          :F&  */
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
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID   1121
#define PND_MODULE_ID        1121


#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_trc.h"


#include "pniobase.h"
#include "servusrx.h"
#include "pniobase.h"
#include "pniousrx.h"

/* StdLib++ */
#include <map>
#include <vector>

using namespace std;

#include "pnd_iob_core.h"
#include "pnd_pnio_user_core.h"

#include "pnd_iodu.h"
//#include "pnd_iodu_perif.h"
#include "pnd_iodu_edds.h"

#define PND_MAX_INSTANCES  PSI_CFG_MAX_HD_CNT

typedef struct {
   PNIO_UINT16 ios;       // IOSystem Number
   PNIO_UINT16 device;
   PNIO_UINT16 slot;
   PNIO_UINT16 subslot;
} GEO_ADDR;


//first: log_adr, second: GEO_ADDR
typedef map<PNIO_UINT32, GEO_ADDR> AddressMap;

AddressMap addressMap;

// user handele = number of controller instance
typedef struct pnd_user_core_cp
{
    PNIO_UINT32  user_handle;
    IController  *instance_pointer;
    CIODU *      IODU_pointer;
	IIOInterface *interface_instance_pointer;

    pnd_user_core_cp()
    {
        user_handle = PND_INVALID_HANDLE;
        instance_pointer = 0;
        IODU_pointer = 0;
        interface_instance_pointer = 0;
    };
}PND_USER_CORE_CP_TYPE;

typedef struct
{
    PNIO_BOOL  isInitialized;
    PNIO_UINT8 logAddrList[PND_LADDR_MAX]; // LADDR list for each HD/IF
}PND_LADDR_LIST_TYPE;

static PND_USER_CORE_CP_TYPE g_pnd_user_core_cp_list[PND_MAX_INSTANCES];


/* global critical section to serialise the dll function calls */
static PNIO_UINT16 g_pnd_iob_core_lock_id = 0;

static PNIO_UINT16 g_pnd_shared_list_lock_id = 0;

static PNIO_UINT16 g_pnd_allocated_resources = 0;

static PND_LADDR_LIST_TYPE g_pnd_log_addr;

/* locks */
/*************************************************************************/
void pnd_iobase_core_usr_enter(void)
{
    pnd_enter_critical_section(g_pnd_iob_core_lock_id);
}
/*************************************************************************/
void pnd_iobase_core_usr_exit(void)
{
    pnd_exit_critical_section(g_pnd_iob_core_lock_id);
}


typedef struct pnd_rqb_list_tag
{
    PNIO_UINT16 size;
    PNIO_UINT16 index;
    PND_RQB_PTR_TYPE* pnd_rqb;
    PNIO_UINT16 resource_free_event;
} PND_RQB_LIST_TYPE;

PND_RQB_LIST_TYPE pnd_rqb_list = {0};
PND_RQB_LIST_TYPE pnd_datarecord_read_rqb_list = {0};
PND_RQB_LIST_TYPE pnd_datarecord_write_rqb_list = {0};
PND_RQB_LIST_TYPE pnd_ifc_datarecord_read_rqb_list = {0};

PNIO_VOID pnd_alloc_rqb_list(PND_RQB_LIST_TYPE &rqb_list)
{
    PNIO_UINT16 idx = 0;

    rqb_list.index = 0;
    rqb_list.resource_free_event = 0;

    rqb_list.pnd_rqb = (PND_RQB_PTR_TYPE*) pnd_mem_alloc( sizeof(PND_RQB_TYPE) * rqb_list.size );

    for (idx=0;idx<rqb_list.size;idx++)
    {
        rqb_list.pnd_rqb[idx] = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(PND_RQB_TYPE) );
        pnd_memset( rqb_list.pnd_rqb[idx], 0,  sizeof(PND_RQB_TYPE) );
    }
}

PNIO_VOID pnd_free_rqb_list(PND_RQB_LIST_TYPE &rqb_list)
{
    PNIO_UINT16 idx = 0;

    for (idx=0;idx<rqb_list.size;idx++)
    {
        pnd_mem_free(rqb_list.pnd_rqb[idx]);
    }

    pnd_mem_free(rqb_list.pnd_rqb);
}

PND_RQB_PTR_TYPE pnd_get_rqb_from_list(PND_RQB_LIST_TYPE &rqb_list)
{
    PND_RQB_PTR_TYPE pRqb = PNIO_NULL;

    if (( rqb_list.index >= 0) && (rqb_list.index < rqb_list.size) )
    {
        pRqb = rqb_list.pnd_rqb[rqb_list.index];

        PND_ASSERT(pRqb != 0);

        rqb_list.pnd_rqb[rqb_list.index] = PNIO_NULL;

        rqb_list.index ++;

        pnd_reset_event(rqb_list.resource_free_event);

        return pRqb;
    }

    return PNIO_NULL;
}

PNIO_VOID pnd_put_rqb_to_list(PND_RQB_PTR_TYPE pRqb, PND_RQB_LIST_TYPE &rqb_list)
{
    PND_ASSERT(pRqb != 0);

    if ( (rqb_list.index > 0) && (rqb_list.index <= rqb_list.size) )
    {
        rqb_list.index --;

        PND_ASSERT(rqb_list.pnd_rqb[rqb_list.index] == 0);

        rqb_list.pnd_rqb[rqb_list.index] = pRqb;

        pRqb = 0;
    }
    else
    {
        PND_ASSERT(pRqb != 0);
    }

    // set event - all resource free (close controller waits till all resourcecs are free)
    if (rqb_list.index == 0)
    {
        pnd_set_event( rqb_list.resource_free_event );
    }
}

PND_RQB_PTR_TYPE IController::pnd_lower_rqb_alloc( LSA_OPCODE_TYPE opcode, LSA_HANDLE_TYPE handle, PNIO_VOID_PTR_TYPE pInst,PNIO_VOID(*func)(PNIO_VOID*_this, PNIO_VOID*_rqb) )
{
    // Allocate an user RQB for opcode
    PND_RQB_PTR_TYPE   pRQB;
    PND_RQB_LIST_TYPE *pRqbList = PNIO_NULL;

    if(opcode == PND_OPC_REC_READ)
    {
        pRqbList = &pnd_datarecord_read_rqb_list;
    }
    else if(opcode == PND_OPC_REC_WRITE)
    {
        pRqbList = &pnd_datarecord_write_rqb_list;
    }
    else
    {
        pRqbList = &pnd_rqb_list;
    }

    pRQB = pnd_get_rqb_from_list(*pRqbList);

    if (!pRQB)
    {
        return PNIO_NULL;
    }

    pnd_memset( pRQB, 0,  sizeof(PND_RQB_TYPE) );

    LSA_RQB_SET_OPCODE( pRQB, opcode );
    LSA_RQB_SET_HANDLE( pRQB, handle );
    PND_RQB_SET_USER_INST_PTR(pRQB, pInst);
    PND_RQB_SET_USER_CBF_PTR(pRQB, func);

    g_pnd_allocated_resources++;

    return pRQB;

}

PNIO_VOID IController::pnd_lower_rqb_free( PNIO_VOID_PTR_TYPE rqb )
{
    PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)rqb;
    PND_RQB_LIST_TYPE *pRqbList = PNIO_NULL;

    if(LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_REC_READ)
    {
        pRqbList = &pnd_datarecord_read_rqb_list;
    }
    else if(LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_REC_WRITE)
    {
        pRqbList = &pnd_datarecord_write_rqb_list;
    }
    else
    {
        pRqbList = &pnd_rqb_list;
    }
    
    pnd_put_rqb_to_list((PND_RQB_PTR_TYPE)pRQB, *pRqbList);
    g_pnd_allocated_resources--;
}

/*************************************************************************/
PNIO_UINT32 ConvertGeoToLogAddr( PNIO_UINT16 stationNumber, PNIO_UINT16 slotNumber, PNIO_UINT16 subslotNumber )
{
    PNIO_UINT32 laddr = 0;

	for(AddressMap::iterator m_it = addressMap.begin(); m_it != addressMap.end(); m_it++)
	{
		GEO_ADDR ga = m_it->second;
		if( ga.device == stationNumber
			&& ga.slot == slotNumber 
			&& ga.subslot == subslotNumber)
		{
            laddr = m_it->first;
			break;
		}
	}

	return laddr;
}
/*************************************************************************/
GEO_ADDR ConvertLogToGeoAddr( PNIO_ADDR laddr )
{
	AddressMap::iterator m_it;
    GEO_ADDR ga = {0};
	
	if( !addressMap.empty() )
	{
		m_it = addressMap.find( laddr.u.Addr );
		
		if( m_it != addressMap.end() )
		{
			ga = m_it->second;
		}
	}
	
	return ga;
}
/*************************************************************************/ 
PNIO_VOID AddNode( AddressMap &geoMap, PNIO_UINT32 laddr, GEO_ADDR &geoaddr )
{
	//PND_ASSERT( geoaddr != 0 ); // TODO: check geoaddr parameters

	AddressMap::iterator m_it;

	m_it = geoMap.find( laddr );

	PND_ASSERT( m_it == geoMap.end() );  // in MAP ?
	geoMap[laddr] = geoaddr;
} 
/*************************************************************************/ 
PNIO_VOID EmptyMap( AddressMap &geoMap )
{
	geoMap.clear();
}


/* startup - shut down */
/*************************************************************************/
void pnd_iobase_core_init(PNIO_VOID)
{
    PNIO_UINT32 i;
    PNIO_UINT32 result = PNIO_OK;
    
    // void pnd_iobase_core_init(controller instanze and interfaces)
    result = pnd_alloc_critical_section(&g_pnd_iob_core_lock_id, PNIO_FALSE );
    if(result != PNIO_RET_OK)          // PNIO_RET_OK != PNIO_OK !!!
        PND_FATAL("event alloc failed");
    
    result = pnd_alloc_critical_section(&g_pnd_shared_list_lock_id, LSA_FALSE );
    if(result != LSA_RET_OK)          // LSA_RET_OK != PNIO_OK !!!
        PND_FATAL("event alloc failed");

    for (i=0; i<PND_MAX_INSTANCES;i++)
    {
        g_pnd_user_core_cp_list[i].instance_pointer = 0;
        g_pnd_user_core_cp_list[i].IODU_pointer = 0;
        g_pnd_user_core_cp_list[i].user_handle = PND_INVALID_HANDLE;
		g_pnd_user_core_cp_list[i].interface_instance_pointer = 0;
    }

    pnd_rqb_list.size = PND_PNIO_MAX_RESOURCES;
    pnd_alloc_rqb_list(pnd_rqb_list);

    pnd_datarecord_read_rqb_list.size = PND_PNIO_MAX_RECORD_READ_WRITE_RESOURCES;
    pnd_alloc_rqb_list(pnd_datarecord_read_rqb_list);

    pnd_datarecord_write_rqb_list.size = PND_PNIO_MAX_RECORD_READ_WRITE_RESOURCES;
    pnd_alloc_rqb_list(pnd_datarecord_write_rqb_list);

    pnd_ifc_datarecord_read_rqb_list.size = PND_PNIO_MAX_RECORD_READ_WRITE_RESOURCES;
    pnd_alloc_rqb_list(pnd_ifc_datarecord_read_rqb_list);
}


/* startup - shut down */
/*************************************************************************/
void pnd_iobase_core_startup(PNIO_VOID_PTR_TYPE rqb_ptr)
{
    PNIO_UINT32 i;
    PND_RQB_PTR_TYPE              pRQB;
    PND_STARTUP_PTR_TYPE          pStart;
    pRQB = (PND_RQB_PTR_TYPE)rqb_ptr;

    pStart = &pRQB->args.startup;
    
    for (i=0; i<pStart->startup_param.nr_of_cp; i++)
    {
        g_pnd_user_core_cp_list[i].instance_pointer = new IController(i);
		g_pnd_user_core_cp_list[i].interface_instance_pointer = new IIOInterface(i);

        if(LSA_COMP_ID_EDDP == pStart->startup_param.pnd_handle[i].edd_comp_id)
        {
            //g_pnd_user_core_cp_list[i].IODU_pointer = new CIODU_PERIF(pStart->startup_param.pnd_handle[i]);
            PND_FATAL("PNIP is no longer supported!");
        }
        else if (LSA_COMP_ID_EDDS == pStart->startup_param.pnd_handle[i].edd_comp_id)
        {
            g_pnd_user_core_cp_list[i].IODU_pointer = new CIODU_EDDS(pStart->startup_param.pnd_handle[i]);
        }

        g_pnd_user_core_cp_list[i].user_handle = i;
    }
}


/*************************************************************************/
PNIO_VOID pnd_iobase_core_undo_init(PNIO_VOID)
{
    PNIO_UINT32 i;

    for (i=0; i< PND_MAX_INSTANCES;i++)
    {
        if (g_pnd_user_core_cp_list[i].instance_pointer) 
            delete g_pnd_user_core_cp_list[i].instance_pointer;

		if (g_pnd_user_core_cp_list[i].interface_instance_pointer)
            delete g_pnd_user_core_cp_list[i].interface_instance_pointer;

        if (g_pnd_user_core_cp_list[i].IODU_pointer) 
            delete g_pnd_user_core_cp_list[i].IODU_pointer;
        
        g_pnd_user_core_cp_list[i].user_handle      = PND_INVALID_HANDLE;
        g_pnd_user_core_cp_list[i].instance_pointer = 0;
        g_pnd_user_core_cp_list[i].IODU_pointer =     0;
    }

    pnd_free_critical_section(g_pnd_iob_core_lock_id );
    pnd_free_critical_section(g_pnd_shared_list_lock_id );

    pnd_free_rqb_list(pnd_rqb_list);
    pnd_free_rqb_list(pnd_datarecord_read_rqb_list);
    pnd_free_rqb_list(pnd_datarecord_write_rqb_list);
    pnd_free_rqb_list(pnd_ifc_datarecord_read_rqb_list);
}

/* instances & handles */

/*************************************************************************/
PNIO_UINT32 IController::get_handle(IController *ptr)
{
    if(ptr)
        return ptr->m_hInstanceHandle;
        else{
            PND_ASSERT(ptr);
        return 0xFFFFFFFF;
    }
}

/*************************************************************************/
PNIO_UINT32 IController::get_handle_by_index(PNIO_UINT32 CpIndex)
{
    if ((CpIndex >= 0) && (CpIndex < PND_MAX_INSTANCES))
    {
        if (g_pnd_user_core_cp_list[CpIndex].user_handle != PND_INVALID_HANDLE)
            return g_pnd_user_core_cp_list[CpIndex].user_handle;
        else
            return PND_INVALID_HANDLE;

    }
    else 
        return PND_INVALID_HANDLE;
}

/*************************************************************************/
IController* IController::get_instance(PNIO_UINT32 CpIndex)
{
    if ((CpIndex >= 0) && (CpIndex < PND_MAX_INSTANCES))
    {
        if (g_pnd_user_core_cp_list[CpIndex].instance_pointer != PNIO_NULL)
            return g_pnd_user_core_cp_list[CpIndex].instance_pointer;
        else
            return PNIO_NULL;
    }
    else 
        return PNIO_NULL;
}


/*************************************************************************/
CIODU* IController::get_IODU(PNIO_UINT32 CpIndex)
{
    if ((CpIndex >= 0) && (CpIndex < PND_MAX_INSTANCES))
    {
        if (g_pnd_user_core_cp_list[CpIndex].instance_pointer != PNIO_NULL)
            return g_pnd_user_core_cp_list[CpIndex].IODU_pointer;
        else
            return PNIO_NULL;
    }
    else 
        return PNIO_NULL;
}

/*************************************************************************/
PNIO_VOID IController::remove_instance(PNIO_UINT16 UserHandle)
{
    delete g_pnd_user_core_cp_list[UserHandle].instance_pointer;
}

/*************************************************************************/

pnd_iob_controller_status IController::m_get_status(PNIO_VOID)
{
    return this->m_status;
}

/*************************************************************************/

PNIO_VOID  IController::m_set_status(pnd_iob_controller_status status)
{
    this->m_status = status;
}

/**********************************************************/
PNIO_VOID IController::IOSysRcfg_PrepareDeactPendingList( LogAddrVector& pendingList )
{
	PNIO_ADDR tmpAddr;
	tmpAddr.AddrType = PNIO_ADDR_LOG;
	tmpAddr.IODataType = PNIO_IO_IN;

    pnd_enter_critical_section(g_pnd_shared_list_lock_id);

	for( PNIO_UINT32 idx = 0; idx < PND_LADDR_MAX; idx++)
	{
		if( g_pnd_log_addr.logAddrList[idx] == IOCDEVICE )
		{
			tmpAddr.u.Addr = idx;
			pendingList.push_back( tmpAddr.u.Addr );
		}
	}
    
    pnd_exit_critical_section(g_pnd_shared_list_lock_id);
}
/**********************************************************/
PNIO_UINT32 IController::GetNextDeviceFromPendingList( PNIO_ADDR *laddr, LogAddrVector& pendingList, 
													                     LogAddrVector& inProgressList )
{
	PNIO_UINT8 result;

    PND_ASSERT( laddr );
	pnd_memset( laddr, 0, sizeof(PNIO_ADDR) );

	// 3 return types
	// -> OK
	// -> pInProgressActivationList is full
	// -> pPendingActivationList is empty

    pnd_enter_critical_section(g_pnd_shared_list_lock_id);

	// PendingActivationList is empty
	if( pendingList.empty() )
	{
		result = PNIO_FALSE;
	}
	else
	{
		laddr->AddrType = PNIO_ADDR_LOG;
		laddr->IODataType = PNIO_IO_IN;

		laddr->u.Addr = pendingList.at( pendingList.size()-1 );
        
        inProgressList.push_back( laddr->u.Addr );
        
		pendingList.pop_back();        

		result = PNIO_TRUE;
	}

    pnd_exit_critical_section(g_pnd_shared_list_lock_id);

	return result;
}
/**********************************************************/
PNIO_VOID IController::IOSysRcfg_PrepareDeActRequest( PNIO_ADDR &laddr, PNIO_DEV_ACT_TYPE actType, PNIO_VOID *rqb )
{
	PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE) rqb;
    PND_ASSERT( rqb_ptr );
    pnd_memset(rqb_ptr, 0, sizeof(PND_RQB_TYPE));

	// set opcode for device de/activate
	LSA_RQB_SET_OPCODE( rqb_ptr, PND_OPC_DEVICE_ACTIVATE );
    LSA_RQB_SET_HANDLE( rqb_ptr, (PNIO_UINT8) get_handle(this) );
    PND_RQB_SET_USER_INST_PTR(rqb_ptr, this);
    PND_RQB_SET_USER_CBF_PTR(rqb_ptr, IController::iosystem_reconfig_done);
	
	PND_DEV_ACTIVATE_TYPE *pActivate = &rqb_ptr->args.iob_dev_act;

	pnd_memset( pActivate, 0, sizeof(PND_DEV_ACTIVATE_TYPE) );

	pActivate->Addr.u.Addr      = laddr.u.Addr;
	pActivate->Addr.AddrType    = PNIO_ADDR_LOG;
	pActivate->Addr.IODataType  = PNIO_IO_IN;
	pActivate->DevActMode       = actType;
	pActivate->pnd_handle.cp_nr = m_hInstanceHandle;
}
/**********************************************************/
PNIO_VOID IController::IOSysRcfg_PrepareReadReq( PNIO_ADDR &laddr, PNIO_VOID *rqb )
{
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE) rqb;
    PND_ASSERT( rqb_ptr );
    pnd_memset(rqb_ptr, 0, sizeof(PND_RQB_TYPE));

    // set opcode for record read
    LSA_RQB_SET_OPCODE( rqb_ptr, PND_OPC_REC_READ );
    LSA_RQB_SET_HANDLE( rqb_ptr, (PNIO_UINT8) get_handle(this) );
    PND_RQB_SET_USER_INST_PTR(rqb_ptr, this);
    PND_RQB_SET_USER_CBF_PTR(rqb_ptr, IController::iosystem_reconfig_done);
	
	// RecodRead Rqb
	PND_RECORD_READ_WRITE_TYPE *pRecRead = &rqb_ptr->args.rec_read;

	pnd_memset(pRecRead, 0, sizeof(PND_RECORD_READ_WRITE_TYPE));

	pRecRead->UserRef          = 0;
	pRecRead->pnd_handle.cp_nr = m_hInstanceHandle;
	pRecRead->Addr.AddrType    = PNIO_ADDR_LOG;
	pRecRead->Addr.IODataType  = PNIO_IO_IN;
	pRecRead->Addr.u.Addr      = laddr.u.Addr;
	pRecRead->RecordIndex      = 0x0000B081; // CM: PDIOSystemInfo
	pRecRead->Length           = 10 + (PSI_MAX_CL_DEVICES * 4);
	pRecRead->pBuffer          = (PNIO_UINT8 *)pnd_mem_alloc(pRecRead->Length);

	PND_ASSERT( pRecRead->pBuffer );
	
	pnd_memset( pRecRead->pBuffer, 0, pRecRead->Length );

}
/**********************************************************/
PNIO_UINT32 IController::IOSysRcfg_EvaluateReadReq( PNIO_VOID *rqb )
{
	PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE) rqb;
    PNIO_UINT32 result = PNIO_TRUE;
	
	if( rqb_ptr->args.rec_read.pnio_err != PNIO_OK )
	{
        rqb_ptr->args.iosys_recon.pnio_err = IOSysRcfg_MapCmErrToPnioErr(rqb_ptr->args.rec_read.err);

		result = PNIO_FALSE;
	}
    else
    {
        PNIO_VOID *pPDIOSytemInfoBuffer = rqb_ptr->args.rec_read.pBuffer;
	
	    // Create Mandatory and Optional Device List
	    IOSysRcfg_CreateMandatoryAndOptionalDeviceList( pPDIOSytemInfoBuffer );
	    
    }
	
    pnd_mem_free(rqb_ptr->args.rec_read.pBuffer);

	return result;
}
/**********************************************************/
PNIO_VOID IController::IOSysRcfg_PrepareWriteReq( PNIO_ADDR &laddr, PNIO_VOID *rqb )
{
	PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE) rqb;
    PND_ASSERT( rqb_ptr );
    pnd_memset(rqb_ptr, 0, sizeof(PND_RQB_TYPE));

	PNIO_VOID_PTR_TYPE  pRecWriteBuffer = PNIO_NULL;
	PNIO_UINT32        recWriteLength  = 0;
	
	// Creating PDInstanceTailorData for RecordWrite
	IOSysRcfg_CreatePDInstanceTailorData( &pRecWriteBuffer, &recWriteLength );

	LSA_RQB_SET_OPCODE( rqb_ptr, PND_OPC_REC_WRITE);
	PND_RQB_SET_USER_CBF_PTR(rqb_ptr, IController::iosystem_reconfig_done);
	LSA_RQB_SET_HANDLE(rqb_ptr, (PNIO_UINT8) get_handle(this));
	PND_RQB_SET_USER_INST_PTR(rqb_ptr, this);
	
	PND_RECORD_READ_WRITE_PTR_TYPE pRecWrite = &rqb_ptr->args.rec_write;
	pnd_memset( pRecWrite, 0, sizeof(PND_RECORD_READ_WRITE_TYPE) );
	
	pRecWrite->UserRef          = 0;
	pRecWrite->pnd_handle.cp_nr = m_hInstanceHandle;
	pRecWrite->Addr.AddrType    = PNIO_ADDR_LOG;
	pRecWrite->Addr.IODataType  = PNIO_IO_IN;
	pRecWrite->Addr.u.Addr      = laddr.u.Addr;
	pRecWrite->RecordIndex      = 0x0000B080; // PDInstanceTailorData
	pRecWrite->Length           = recWriteLength;
	pRecWrite->pBuffer          = pRecWriteBuffer;
}
/**********************************************************/
PNIO_UINT32 IController::IOSysRcfg_EvaluateWriteReq( PNIO_VOID *rqb )
{
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE) rqb;
    PNIO_UINT32 result = PNIO_TRUE;

	if( rqb_ptr->args.rec_write.pnio_err != PNIO_OK )
    {
		// break the operation with callback
        rqb_ptr->args.iosys_recon.pnio_err = IOSysRcfg_MapCmErrToPnioErr(rqb_ptr->args.rec_write.err);
        
		result =  PNIO_FALSE;
	}

    pnd_mem_free(rqb_ptr->args.rec_write.pBuffer);

	return result;
}
/**********************************************************/
PNIO_VOID IController::IOSysRcfg_PrepareActPendingList( LogAddrVector& pendingList )
{
    PNIO_UINT16 size = (PNIO_UINT16) m_MandatoryDeviceList.size();

    pnd_enter_critical_section(g_pnd_shared_list_lock_id);

	for( PNIO_UINT16 idx = 0; idx < size; idx++ )
    {
		pendingList.push_back( m_MandatoryDeviceList.at(idx) );
	}

	for( PNIO_UINT16 idx = 0; idx < m_IosReconfigParams->DeviceCount; idx++)
	{
		pendingList.push_back( m_IosReconfigParams->DeviceList[idx].u.Addr );
    }

    pnd_exit_critical_section(g_pnd_shared_list_lock_id);
}
/**********************************************************/
PNIO_UINT32 IController::IOSysRcfg_StoreUserParams( PNIO_UINT32 DeviceCnt, PNIO_ADDR * DeviceList, 
								                     PNIO_UINT32 PortInterconnectionCnt, PNIO_ADDR * PortInterconnectionList )
{
	if( DeviceCnt > 0 )
    {
		if( DeviceList == PNIO_NULL )
			return PNIO_ERR_PRM_BUF;
    }

	if( PortInterconnectionCnt > 0 )
    {
		if( PortInterconnectionList == PNIO_NULL )
			return PNIO_ERR_PRM_BUF;
	}

    // Storing DeviceList and PortInterconnectionList
	m_IosReconfigParams = (PNIO_IOS_RECONFIG_PTR_TYPE) pnd_mem_alloc( sizeof(PNIO_IOS_RECONFIG_TYPE) );
	PND_ASSERT( m_IosReconfigParams );

	m_IosReconfigParams->DeviceList = PNIO_NULL;
	m_IosReconfigParams->PortInterconnectionList = PNIO_NULL;

	m_IosReconfigParams->DeviceCount = DeviceCnt;

	// DeviceCount may be zero and no need to allocate&copy for it
	if( m_IosReconfigParams->DeviceCount != 0 )
	{
		m_IosReconfigParams->DeviceList  = (PNIO_ADDR *) pnd_mem_alloc( DeviceCnt * sizeof(PNIO_ADDR) );
		PND_ASSERT( m_IosReconfigParams->DeviceList );
		pnd_memcpy( m_IosReconfigParams->DeviceList, DeviceList, DeviceCnt * sizeof(PNIO_ADDR) );
	}
		
	m_IosReconfigParams->PortInterconnectionCnt  = PortInterconnectionCnt;

	// PortInterconnectionCnt may be zero and no need to allocate&copy for it
	if( m_IosReconfigParams->PortInterconnectionCnt != 0 )
	{
		m_IosReconfigParams->PortInterconnectionList = (PNIO_ADDR *) pnd_mem_alloc( 2 * PortInterconnectionCnt * sizeof(PNIO_ADDR) );
		PND_ASSERT( m_IosReconfigParams->PortInterconnectionList );
		pnd_memcpy( m_IosReconfigParams->PortInterconnectionList, PortInterconnectionList, 2 * PortInterconnectionCnt * sizeof(PNIO_ADDR) );
	}

    return PNIO_OK;
}
/**********************************************************/
PNIO_VOID IController::IOSysRcfg_FreeUserParams()
{
	if( m_IosReconfigParams )
	{
		if( m_IosReconfigParams->DeviceList )
		{
			pnd_mem_free( m_IosReconfigParams->DeviceList );
		}

		if( m_IosReconfigParams->PortInterconnectionList )
		{
			pnd_mem_free( m_IosReconfigParams->PortInterconnectionList );
		}

		pnd_mem_free( m_IosReconfigParams );
	}
}

/**********************************************************/
PNIO_UINT32 IController::IOSysRcfg_MapCmErrToPnioErr(PNIO_ERR_STAT err)
{
    // cmErr can start by 0xDE while reading operation. 0xDE.. --> read, 0xDF.. --> write
    PNIO_UINT32 cmErr;

    cmErr = (PNIO_UINT32) (err.ErrCode<<24) + (PNIO_UINT32) (err.ErrDecode<<16) + (PNIO_UINT32) (err.ErrCode1<<8) + (PNIO_UINT32) (err.ErrCode2);

    switch(cmErr)
    {
        case 0:
            return PNIO_OK;

        case 0xDF80AA00:
            return PNIO_ERR_CORRUPTED_DATA;

        case 0xDF80AA02:
            return PNIO_ERR_INVALID_STATION;

        case 0xDF80AA03:
            return PNIO_ERR_INVALID_PORT;

        case 0xDF80B602:
            return PNIO_ERR_ACCESS_DENIED;

        default: // 0xDFxxxxxx & 0xXXXXXXXX
            return PNIO_ERR_INTERNAL;
    }

}

/**********************************************************/
PNIO_UINT32 IController::GetLogAddrOfInterfaceSubmodule()
{
	for( PNIO_UINT16 idx = 0; idx < PND_LADDR_MAX; idx++ )
	{
		if( g_pnd_log_addr.logAddrList[idx] == PDSUBSLOTINTERFACE )
		{
			return idx;
		}
	}
						
	return 0;
}
/*************************************************************************/
PNIO_VOID IController::IOSysRcfg_CreateMandatoryAndOptionalDeviceList( PNIO_VOID_PTR_TYPE record )
{
	// ### PDIOSystemInfo Record ###
	// ----------------------------------------------------------------------------------------------
	// | Block Header | IOSystemProperties | Number of Devices | Station Number | Device Properties |
	// ----------------------------------------------------------------------------------------------
	//
	// ----------------------------------------
	// Block Header ( Block Type(PNIO_UINT16) + Block Version High(PNIO_UINT8)
	//              + Block Version Low(PNIO_UINT8) + Block Length(PNIO_UINT16)
	//                Total: 6 bytes
	// ----------------------------------------
	// IOSystemProperties (PNIO_UINT16): 
	//      0x00: No optional device
	//      0x01: At least one optional device
	// ----------------------------------------
	// Number Of Devices (PNIO_UINT16):
	//      It contains the number of devices projected
	// ----------------------------------------
	// Station Number (PNIO_UINT16): 
	//      Device's station number
	// ----------------------------------------
	// Device Properties (PNIO_UINT16):
	//      Bit 0: IsPartOfAddressTailoring
	//             0x00: Device is not part of address tailoring
	//             0x01: Device is part of address tailoring
	//      Bit 1: IsOptional
	//             0x00: Device is mandatory
	//             0x01: Device is optional
	//      Bit 2: HasFixedPortToOptionalNeighbor
	//             0x00: None of device's port is fixed peer to optional neighbor
	//             0x01: At least one of the device's port is fixed to peer to optional neighbor
	//      Bit 3: HasProgrammablePeer
	//             0x00: None of device's port is programmable peer
	//             0x01: At least one of the device's ports is programmable peer
	//      Bit 4-15: reserved
	// ----------------------------------------

	m_MandatoryDeviceList.clear();
	m_OptionalDeviceList.clear();

	PNIO_UINT8 *pPDIoSystemInfoRec = (PNIO_UINT8 *) record;

	// IOSystemProperties shows the properties of RTC3 devices
	// PNIO_UINT16 IOSystemProperties = pnd_hton16( *(PNIO_UINT16*) (pPDIoSystemInfoRec + 6) ); // not used.

	PNIO_UINT16 NumberOfDevices = pnd_hton16( *(PNIO_UINT16*) (pPDIoSystemInfoRec + 8) );

	for( PNIO_UINT16 i = 0; i < NumberOfDevices; i++ )
	{
		PNIO_UINT16 StationNumber    = pnd_hton16( *(PNIO_UINT16*) (pPDIoSystemInfoRec + 10 + 4*i) ); // StationNumber + DeviceProperties = 4 bytes
		PNIO_UINT16 DeviceProperties = pnd_hton16( *(PNIO_UINT16*) (pPDIoSystemInfoRec + 12 + 4*i) ); // StationNumber + DeviceProperties = 4 bytes
		PNIO_ADDR tmpAddr;

		if( StationNumber != 0 ) // In case of controller's logical address and station number
		{

			tmpAddr.AddrType = PNIO_ADDR_LOG;
			tmpAddr.IODataType = PNIO_IO_IN;
			tmpAddr.u.Addr = ConvertGeoToLogAddr( StationNumber, 0, 0 );

			// Bit 1: IsOptional
			//     0x00: Device is mandatory
			//     0x01: Device is optional
			if( (DeviceProperties & 0x02) == 0x00 )
			{
				// Mandatory device
				m_MandatoryDeviceList.push_back( tmpAddr.u.Addr );
			}
			else
			{
				// Optional device
				m_OptionalDeviceList.push_back( tmpAddr.u.Addr );
			}

		}
			
	}
}
/*************************************************************************/
PNIO_VOID IController::IOSysRcfg_CreatePDInstanceTailorData( PNIO_VOID_PTR_TYPE *ppPDInstanceTailorDataBuffer, PNIO_UINT32 *pPDInstanceTailorDataBufferLen )
{
	// calculating PDInstanceTailorData block length
	PNIO_UINT16 BlockLength = (PNIO_UINT16) ( 8 + (m_IosReconfigParams->DeviceCount * 4));

	if( m_IosReconfigParams->PortInterconnectionCnt != 0 )
		BlockLength += (PNIO_UINT16) (4 + m_IosReconfigParams->PortInterconnectionCnt * 16);

	
	PNIO_UINT8 *pTailorDataBuffer;
	PNIO_UINT32 *pTailorDataBufferLen = (PNIO_UINT32*) pPDInstanceTailorDataBufferLen;

	*pTailorDataBufferLen = BlockLength + 4;

	// allocating memory for PDInstanceTailorData
	pTailorDataBuffer = (PNIO_UINT8*) pnd_mem_alloc( (*pTailorDataBufferLen) * sizeof(pTailorDataBuffer) );
	PND_ASSERT( pTailorDataBuffer );

	*(PNIO_UINT8**) ppPDInstanceTailorDataBuffer = pTailorDataBuffer;

	PNIO_UINT8 *pTailorDataBufferHead = pTailorDataBuffer;
	PNIO_UINT8 *pTailorDataBufferTail = pTailorDataBufferHead;

	// Adding PDInstanceTailorData Block Header
	*(PNIO_UINT16 *)pTailorDataBufferTail = pnd_hton16( (PNIO_UINT16) (0x7080) );       // BlockType
	pTailorDataBufferTail += 2;
	*(PNIO_UINT16 *)pTailorDataBufferTail = pnd_hton16( (PNIO_UINT16) (BlockLength) );  // Block Length
	pTailorDataBufferTail += 2;
	*(PNIO_UINT8 *)pTailorDataBufferTail  = (PNIO_UINT8)  (0x01);                       // Block Version High
	pTailorDataBufferTail += 1;
	*(PNIO_UINT8 *)pTailorDataBufferTail  = (PNIO_UINT8)  (0x00);                       // Block Version Low
	pTailorDataBufferTail += 1;
	
	// Adding PDInstanceTailorData Block Header
	
	*(PNIO_UINT8 *)pTailorDataBufferTail  = (PNIO_UINT8)  (0x00);         // Padding
	pTailorDataBufferTail += 1;
	*(PNIO_UINT8 *)pTailorDataBufferTail  = (PNIO_UINT8)  (0x00);         // Padding
	pTailorDataBufferTail += 1;

	// Device Instance Tailor Data
	*(PNIO_UINT8 *)pTailorDataBufferTail  = (PNIO_UINT8)  (0x00);         // Padding
	pTailorDataBufferTail += 1;
	*(PNIO_UINT8 *)pTailorDataBufferTail  = (PNIO_UINT8)  (0x00);         // Padding
	pTailorDataBufferTail += 1;
	*(PNIO_UINT16 *)pTailorDataBufferTail = pnd_hton16( (PNIO_UINT16) (m_IosReconfigParams->DeviceCount) );  // Number Of Devices
	pTailorDataBufferTail += 2;

	for(PNIO_UINT16 i = 0; i < (PNIO_UINT16) m_IosReconfigParams->DeviceCount; i++)
	{
		PNIO_UINT16 tmpDeviceNumber;
		tmpDeviceNumber = m_pndDevNumberList[m_IosReconfigParams->DeviceList[i].u.Addr];

		*(PNIO_UINT16 *)pTailorDataBufferTail = pnd_hton16( (PNIO_UINT16) (tmpDeviceNumber) );  // Station Number
		pTailorDataBufferTail += 2;
		*(PNIO_UINT8 *)pTailorDataBufferTail  = (PNIO_UINT8)  (0x00);                           // Padding
		pTailorDataBufferTail += 1;
		*(PNIO_UINT8 *)pTailorDataBufferTail  = (PNIO_UINT8)  (0x00);                           // Padding
		pTailorDataBufferTail += 1;
	}

	// Checking if there is any programmable peers
	if( m_IosReconfigParams->PortInterconnectionCnt != 0 ) 
	{
		// Port Instance Tailor Data
		*(PNIO_UINT8 *)pTailorDataBufferTail  = (PNIO_UINT8)  (0x00);   // Padding
		pTailorDataBufferTail += 1;
		*(PNIO_UINT8 *)pTailorDataBufferTail  = (PNIO_UINT8)  (0x00);   // Padding
		pTailorDataBufferTail += 1;
		*(PNIO_UINT16 *)pTailorDataBufferTail = pnd_hton16( (PNIO_UINT16) (m_IosReconfigParams->PortInterconnectionCnt) );  // Number Of Interconnections
		pTailorDataBufferTail += 2;

		for(PNIO_UINT16 i = 0; i < (PNIO_UINT16) m_IosReconfigParams->PortInterconnectionCnt * 2; i++)
		{
			PNIO_UINT16 tmpDeviceNumber  = m_pndDevNumberList[m_IosReconfigParams->PortInterconnectionList[i].u.Addr];
			PNIO_UINT16 tmpSlotNumber    = m_pndSlotNumberList[m_IosReconfigParams->PortInterconnectionList[i].u.Addr];
			PNIO_UINT16 tmpSubslotNumber = m_pndSubslotNumberList[m_IosReconfigParams->PortInterconnectionList[i].u.Addr];

			*(PNIO_UINT16 *)pTailorDataBufferTail = pnd_hton16( (PNIO_UINT16) (tmpDeviceNumber) );   // Station Number
			pTailorDataBufferTail += 2;
			*(PNIO_UINT16 *)pTailorDataBufferTail = pnd_hton16( (PNIO_UINT16) (tmpSlotNumber) );     // Slot Number
			pTailorDataBufferTail += 2;
			*(PNIO_UINT16 *)pTailorDataBufferTail = pnd_hton16( (PNIO_UINT16) (tmpSubslotNumber) );  // Subslot Number
			pTailorDataBufferTail += 2;
			*(PNIO_UINT8 *)pTailorDataBufferTail  = (PNIO_UINT8)  (0x00);                            // Padding
			pTailorDataBufferTail += 1;
			*(PNIO_UINT8 *)pTailorDataBufferTail  = (PNIO_UINT8)  (0x00);                            // Padding
			pTailorDataBufferTail += 1;
		}
	}
}
/*************************************************************************/
PNIO_VOID IController::RemoveDeviceFromInProgressList( PNIO_ADDR &laddr, LogAddrVector& inProgressList )
{
    pnd_enter_critical_section(g_pnd_shared_list_lock_id);

	LogAddrVector::iterator listIter = inProgressList.begin();

	while( listIter != inProgressList.end() )
	{
		if ( *listIter == laddr.u.Addr )
		{
			listIter = inProgressList.erase( listIter );
		}
		else
	{
			++listIter;
		}
	}

    pnd_exit_critical_section(g_pnd_shared_list_lock_id);
}
/*************************************************************************/
PNIO_UINT32 IController::IsInProgressListNotFull( LogAddrVector& inProgressList )
{
	// Check if InProgress List is empty
	if( inProgressList.size() >= MAX_DEVICE_ACTIVATION_SIZE )
	{
		return PNIO_FALSE;
		}

	return PNIO_TRUE;
}

/*************************************************************************/
PNIO_VOID IController::AddDeviceToInProgressList( PNIO_UINT32 laddr, LogAddrVector& inProgressList )
{
    pnd_enter_critical_section(g_pnd_shared_list_lock_id);
	inProgressList.push_back( laddr );
    pnd_exit_critical_section(g_pnd_shared_list_lock_id);
}

/* requests */
/*************************************************************************/
// poste request to pnio_user_core
PNIO_VOID pnio_user_request_start(PNIO_VOID_PTR_TYPE  pRQB)
{
    pnd_request_start(PND_MBX_ID_PNIO_USER_CORE, (PND_REQUEST_FCT)pnd_pnio_user_core_request, (PNIO_VOID_PTR_TYPE)pRQB);
}

/*************************************************************************/
PNIO_VOID IController::controller_open_done(PNIO_VOID_PTR_TYPE pInst,PNIO_VOID_PTR_TYPE  pRqb)
{
    IController *pThis = (IController *)pInst;

    LSA_UNUSED_ARG (pRqb);

    pnd_set_event( pThis->m_pnd_ioc_wait_event );
}

/*************************************************************************/
PNIO_VOID IController::controller_close_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    IController *pThis = (IController *)pInst;

    LSA_UNUSED_ARG (pRqb);

    pnd_set_event( pThis->m_pnd_ioc_wait_event );
}


/*************************************************************************************************/
PNIO_VOID IController::alarm_indication(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    IController *pThis = (IController *)pInst;
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;
    PND_ALARM_PTR_TYPE     pAlarm;

    pAlarm = &rqb_ptr->args.iob_alarm;

    /* fill in alarm data */

    if (LSA_RQB_GET_RESPONSE(rqb_ptr) == PNIO_OK_CANCEL)
     {
         // pThis->pnd_lower_rqb_free(rqb_ptr);
         pnd_mem_free(rqb_ptr);
     }
     else
     {

        if (pThis)
        {
            PNIO_ADDR Addr;
            PNIO_CBE_PRM cbf_prm;
            PNIO_CTRL_ALARM_DATA AlarmData;

            pnd_memset(&AlarmData,0,sizeof(AlarmData));

            cbf_prm.Handle = pThis->get_handle(pThis); // = CPIndex
            cbf_prm.CbeType = PNIO_CBE_ALARM_IND;

            cbf_prm.AlarmInd.IndRef = pAlarm->ind_ref;

            switch (pAlarm->alarm.AlarmType)
            {
                case PNIO_ALARM_DEV_FAILURE:
                    {
                         CIODU *pIODU = pThis->get_IODU(cbf_prm.Handle);
                         
                         pIODU->update_IQ_table(pAlarm->alarm.DeviceNum, PNIO_FALSE/*offline*/);
                         break;
                    }
                case PNIO_ALARM_DEV_RETURN:
                    {
                         CIODU *pIODU = pThis->get_IODU(cbf_prm.Handle);
                         
                         pIODU->update_IQ_table(pAlarm->alarm.DeviceNum, PNIO_TRUE/*online*/);
                         break;
                    }
                default:
                    {
                        // Do nothing;
                        break;
                    }
            } // end switch


            switch (pAlarm->alarm.AlarmType)
            {
                case PNIO_ALARM_DEV_FAILURE:
                case PNIO_ALARM_DEV_RETURN:
                case PNIO_ALARM_DIAGNOSTIC:
                case PNIO_ALARM_PROCESS:
                case PNIO_ALARM_PULL:
                case PNIO_ALARM_PLUG:
                case PNIO_ALARM_STATUS:
                case PNIO_ALARM_UPDATE:
                case PNIO_ALARM_REDUNDANCY:
                case PNIO_ALARM_CONTROLLED_BY_SUPERVISOR:
                case PNIO_ALARM_RELEASED_BY_SUPERVISOR:
                case PNIO_ALARM_PLUG_WRONG:
                case PNIO_ALARM_RETURN_OF_SUBMODULE:
                case PNIO_ALARM_DIAGNOSTIC_DISAPPEARS:
                case PNIO_ALARM_MCR_MISMATCH:
                case PNIO_ALARM_PORT_DATA_CHANGED:
                case PNIO_ALARM_SYNC_DATA_CHANGED:
                case PNIO_ALARM_ISOCHRONE_MODE_PROBLEM:
                case PNIO_ALARM_NETWORK_COMPONENT_PROBLEM:
                case PNIO_ALARM_UPLOAD_AND_STORAGE:
                case PNIO_ALARM_PULL_MODULE:
                    {
                        AlarmData.AlarmType     = pAlarm->alarm.AlarmType;
                        AlarmData.AlarmPriority = pAlarm->alarm.AlarmPriority;
                        AlarmData.DeviceNum     = pAlarm->alarm.DeviceNum;
                        AlarmData.SlotNum       = pAlarm->alarm.SlotNum;
                        AlarmData.SubslotNum    = pAlarm->alarm.SubslotNum;
                        AlarmData.CpuAlarmPriority  = pAlarm->alarm.CpuAlarmPriority;
                        AlarmData.PnioCompatModtype = pAlarm->alarm.PnioCompatModtype;
                        AlarmData.AlarmTinfo.CompatDevGeoaddr = pAlarm->alarm.AlarmTinfo.CompatDevGeoaddr;
                        AlarmData.AlarmTinfo.ProfileType      = pAlarm->alarm.AlarmTinfo.ProfileType;
                        AlarmData.AlarmTinfo.AinfoType        = pAlarm->alarm.AlarmTinfo.AinfoType;
                        AlarmData.AlarmTinfo.ControllerFlags  = pAlarm->alarm.AlarmTinfo.ControllerFlags;
                        AlarmData.AlarmTinfo.DeviceFlag       = pAlarm->alarm.AlarmTinfo.DeviceFlag;
                        AlarmData.AlarmTinfo.PnioVendorIdent  = pAlarm->alarm.AlarmTinfo.PnioVendorIdent;
                        AlarmData.AlarmTinfo.PnioDevIdent     = pAlarm->alarm.AlarmTinfo.PnioDevIdent;
                        AlarmData.AlarmTinfo.PnioDevInstance  = pAlarm->alarm.AlarmTinfo.PnioDevInstance;

                        pnd_memcpy(AlarmData.DiagDs, pAlarm->alarm.DiagDs, sizeof(AlarmData.DiagDs));
                        pnd_memcpy(AlarmData.PrAlarmInfo, pAlarm->alarm.PrAlarmInfo, sizeof(AlarmData.PrAlarmInfo));

                        AlarmData.AlarmAinfo.BlockType    = pAlarm->alarm.AlarmAinfo.BlockType;
                        AlarmData.AlarmAinfo.BlockVersion = pAlarm->alarm.AlarmAinfo.BlockVersion;
                        AlarmData.AlarmAinfo.Api          = pAlarm->alarm.AlarmAinfo.Api;
                        AlarmData.AlarmAinfo.AlarmSpecifier   = pAlarm->alarm.AlarmAinfo.AlarmSpecifier;
                        AlarmData.AlarmAinfo.ModIdent         = pAlarm->alarm.AlarmAinfo.ModIdent;
                        AlarmData.AlarmAinfo.SubIdent         = pAlarm->alarm.AlarmAinfo.SubIdent;
                        AlarmData.AlarmAinfo.UserStrucIdent   = pAlarm->alarm.AlarmAinfo.UserStrucIdent;
                        AlarmData.AlarmAinfo.UserAlarmDataLen = pAlarm->alarm.AlarmAinfo.UserAlarmDataLen;

#ifdef PNIO_ALARM_OLD_STRUC
                        pnd_memcpy(AlarmData.AlarmAinfo.UserAlarmData,  pAlarm->alarm.AlarmAinfo.UserAlarmData,
                            AlarmData.AlarmAinfo.UserAlarmDataLen);
#else
                        if (pAlarm->alarm.AlarmAinfo.UAData.UserAlarmData)
                        {
                            pnd_memcpy(AlarmData.AlarmAinfo.UAData.UserAlarmData, pAlarm->alarm.AlarmAinfo.UAData.UserAlarmData,
                                AlarmData.AlarmAinfo.UserAlarmDataLen);
                        }
#endif
                        Addr.AddrType   = pAlarm->addr.AddrType;
                        Addr.IODataType = pAlarm->addr.IODataType;
                        Addr.u.Addr     = pAlarm->addr.u.Addr;
                        cbf_prm.AlarmInd.pAddr = &Addr;
                        break;
                    }
                default:
                    {
                        // PND_FATAL("unknowen alarm type");
                        break;
                    }
            } // end switch

            // alarm callback 

            cbf_prm.AlarmInd.pAlarmData = &AlarmData;

            if (pThis->m_pUserCbf_Alarm)
            {
                pThis->m_pUserCbf_Alarm(&cbf_prm);
            }
            else
            {
                PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "No CBF_ALARM_IND user callback() defined");
            }

            // reprovide alarm resource
            alarm_response(pThis, rqb_ptr);

        } // end if 
        else
        {
            PND_FATAL("wrong  instance ");
        }

    } // end else
}

/*************************************************************************/
PNIO_UINT32 IController::alarm_provide(PNIO_VOID_PTR_TYPE  pInst, PNIO_UINT16 nrOfAlarmResource)
{
    IController *pThis = (IController *)pInst;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>alarm_provide");

    PND_RQB_PTR_TYPE       pRQB = 0;
    PND_ALARM_PTR_TYPE     pAlarm = 0;
    PNIO_UINT16            i = 0;

    for (i = 0;i< nrOfAlarmResource; i++)
    {
        pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));
    
        if(!pRQB)
        {
            return PNIO_ERR_INTERNAL;
        }

        pAlarm = &pRQB->args.iob_alarm;

        pnd_memset(pAlarm,0,sizeof(PND_ALARM_TYPE));

        pAlarm->pnd_handle.cp_nr = pThis->m_hInstanceHandle;
        pAlarm->ind_ref = PNIO_REF_IOC;

        LSA_RQB_SET_OPCODE( pRQB, PND_OPC_ALARM_PROVIDE );
        LSA_RQB_SET_HANDLE( pRQB, 0 );
        PND_RQB_SET_USER_INST_PTR(pRQB, pThis);
        PND_RQB_SET_USER_CBF_PTR(pRQB, IController::alarm_indication);

        pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<alarm_provide");

    return PNIO_OK;
}

/*************************************************************************/
PNIO_VOID IController::alarm_response(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  rqb_ptr)
{
    PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)rqb_ptr;
    PND_ASSERT( pRQB );

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>alarm_response");

    IController *pThis = (IController *)pInst;

    LSA_RQB_SET_OPCODE( pRQB, PND_OPC_ALARM_RESPONSE );
    LSA_RQB_SET_HANDLE( pRQB, 0 );
    PND_RQB_SET_USER_INST_PTR(pRQB, pThis);
    PND_RQB_SET_USER_CBF_PTR(pRQB,IController::alarm_indication);

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<alarm_response");
}

/*************************************************************************/
PNIO_VOID IController::write_snmp_record(PNIO_VOID_PTR_TYPE pInst)
{
    PND_RQB_PTR_TYPE          pRQB;
    PND_PRM_WRITE_DATA_TYPE*  pPrmWrite;
    IController *pThis = (IController *)pInst;

    pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB) );
    PND_ASSERT( pRQB );

    LSA_RQB_SET_OPCODE( pRQB, PND_OPC_PRM_WRITE );
    
    pPrmWrite = &pRQB->args.prm_write;

    pPrmWrite->pnd_handle.cp_nr   = (PNIO_UINT16) pThis->m_CpIndex;
    pPrmWrite->edd_port_id        = 0;
    pPrmWrite->mk_rema            = 1;           // mk_rema have to be SET.
    pPrmWrite->record_index       = 0xF8000002;  /* MultipleSNMPRecord */
    pPrmWrite->slot_number        = 0;
    pPrmWrite->subslot_number     = 0x8000;

    pnd_request_start(PND_MBX_ID_PNIO_USER_CORE, (PND_REQUEST_FCT)pnd_pnio_user_core_request, (PNIO_VOID_PTR_TYPE)pRQB);
}
/*************************************************************************/
PNIO_VOID pnd_iob_core_request( PND_RQB_PTR_TYPE  rqb_ptr )
{
    PND_ASSERT( rqb_ptr != 0 );

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>pnd_iob_core_request");
    
    switch (LSA_RQB_GET_OPCODE(rqb_ptr))
    {
        case PND_OPC_STARTUP :
            pnd_iobase_core_startup(rqb_ptr);
            rqb_ptr->args.startup.cbf(rqb_ptr);
        break;
        case PND_OPC_SHUTDOWN:
            {
                // pnd_iobase_core_undo_init();
                PNIO_UINT16 i = 0;
                IController *pInsIController;
                IIOInterface *pInsIIOInterface;

                while ((i < PND_MAX_INSTANCES) && (g_pnd_user_core_cp_list[i].user_handle != PND_INVALID_HANDLE))
                {
                    pInsIController = g_pnd_user_core_cp_list[i].instance_pointer;
                    pInsIIOInterface = g_pnd_user_core_cp_list[i].interface_instance_pointer;

                    if (pInsIController->m_status == pnd_controller_opened || pInsIIOInterface->m_status == pnd_interface_opened)
                    {
                        rqb_ptr->args.shutdown.pnio_err = PNIO_ERR_SEQUENCE;
                        break;
                    }
                    i++;
                }
                rqb_ptr->args.shutdown.cbf(rqb_ptr);

             break;
            }
        case PND_OPC_CONTROLLER_OPEN :
            pnio_user_request_start(rqb_ptr);
        break;
        case PND_OPC_CONTROLLER_CLOSE :
            pnio_user_request_start(rqb_ptr);
        break;
        case PND_OPC_PI_ALLOC :
            PND_ASSERT((rqb_ptr->args.pi_alloc.pnd_handle.cp_nr >= 0) && (rqb_ptr->args.pi_alloc.pnd_handle.cp_nr < PND_MAX_INSTANCES));
            PND_ASSERT(g_pnd_user_core_cp_list[(rqb_ptr->args.pi_alloc.pnd_handle.cp_nr)].IODU_pointer);

            g_pnd_user_core_cp_list[(rqb_ptr->args.pi_alloc.pnd_handle.cp_nr)].IODU_pointer->PI_Alloc(rqb_ptr);
        break;
        case PND_OPC_PI_FREE :
            PND_ASSERT((rqb_ptr->args.pi_free.pnd_handle.cp_nr >= 0) && (rqb_ptr->args.pi_free.pnd_handle.cp_nr < PND_MAX_INSTANCES));
            PND_ASSERT(g_pnd_user_core_cp_list[(rqb_ptr->args.pi_free.pnd_handle.cp_nr)].IODU_pointer);

            g_pnd_user_core_cp_list[(rqb_ptr->args.pi_free.pnd_handle.cp_nr)].IODU_pointer->PI_Free(rqb_ptr);
        break;
		case PND_OPC_INTERFACE_OPEN :
            pnio_user_request_start(rqb_ptr);
        break;
        case PND_OPC_INTERFACE_CLOSE :
            pnio_user_request_start(rqb_ptr);
        break;
        default:
            PND_FATAL("not supported opcode received !");
    }
    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<pnd_iob_core_request");
}

/*************************************************************************/
PNIO_VOID pnd_iob_core_request_done( PND_RQB_PTR_TYPE  rqb_ptr )
{
    PND_ASSERT( rqb_ptr != 0 );

    /* main CBF function PNIO user core, called in context of PNIO user core task */
    /* note: addressed instance is stored in extended RQB Header, for callback    */
    PNIO_VOID_PTR_TYPE   p_inst = PND_RQB_GET_USER_INST_PTR( rqb_ptr );
    EPS_LOWER_DONE_FCT  p_fct  = PND_RQB_GET_USER_CBF_PTR( rqb_ptr );

    // execute the CBF for instance
    p_fct( p_inst, rqb_ptr );
}

/* constructor */
/*************************************************************************/
IController::IController(PNIO_UINT32 UserHandle):
    m_pUserCbf_Mode(PNIO_NULL),
    m_pUserCbf_DevAct(PNIO_NULL),
    m_pUserCbf_Alarm(PNIO_NULL),
    m_pUserCbf_DR_Read(PNIO_NULL),
    m_pUserCbf_DR_Write(PNIO_NULL),
    m_pCbf_AlarmInd(PNIO_NULL),
	m_pUserCbf_DiagReq(PNIO_NULL),
    m_pUserCbf_IosytemReconfig(PNIO_NULL),
    m_ExtPar(0),
    m_CpIndex(0),
    m_Mode(PNIO_MODE_OFFLINE),
    m_iOSystemReconfigMode(PNIO_IOS_RECONFIG_MODE_DEACT),
    m_pnioErrorCodeOfLastFailedDevice(0),
    m_status(pnd_controller_closed),
    m_pnd_ioc_wait_event(0),
    m_nr_of_alarm_res(0),
    m_set_mode_req_running(PNIO_FALSE),
    m_IosReconfigParams(PNIO_NULL),
    m_state_iosystem_reconfig(IDLE),
    m_iosys_reconfig_running(PNIO_FALSE)
{

    m_hInstanceHandle = (PNIO_UINT16)UserHandle;

    pnd_memset( &m_pndDevNumberList, 0, sizeof( m_pndDevNumberList ));         // PND_CORE Device Number List
	pnd_memset( &m_pndSlotNumberList, 0, sizeof( m_pndSlotNumberList ));       // PND_CORE Slot Number List
	pnd_memset( &m_pndSubslotNumberList, 0, sizeof( m_pndSubslotNumberList )); // PND_CORE Subslot Number List

	m_PendingActivationList.clear();
	m_InProgressActivationList.clear();

    pnd_memset( &m_laddrOfLastFailedDevice, 0, sizeof( m_laddrOfLastFailedDevice ) );

    pnd_memset(g_pnd_log_addr.logAddrList, 0, sizeof(g_pnd_log_addr.logAddrList));
    g_pnd_log_addr.isInitialized = PNIO_FALSE;
}


/* destuctor */
/*************************************************************************/
IController::~IController(void)
{
}

/* callbacks */
/*************************************************************************/
PNIO_UINT32 IController::register_cbf(PNIO_CBE_TYPE CbeType, PNIO_CBF cbf)
{
    PNIO_UINT32 Ret = PNIO_OK;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>register_cbf");

    if (m_status != pnd_controller_opened)
        return PNIO_ERR_WRONG_HND;

    if(!cbf)
        return PNIO_ERR_PRM_CALLBACK;

    if(m_Mode != PNIO_MODE_OFFLINE)
        return PNIO_ERR_MODE_VALUE;

    switch (CbeType) {
    case PNIO_CBE_MODE_IND:
        if(m_pUserCbf_Mode) {
            Ret = PNIO_ERR_ALREADY_DONE;
        } else {
            m_pUserCbf_Mode = cbf;
            Ret = PNIO_OK;
        }
        break;
    case PNIO_CBE_DEV_ACT_CONF:
        if(m_pUserCbf_DevAct) {
            Ret = PNIO_ERR_ALREADY_DONE;
        } else {
            m_pUserCbf_DevAct = cbf;
            Ret = PNIO_OK;
        }
        break;
    case PNIO_CBE_CP_STOP_REQ:

        break;

	case PNIO_CBE_CTRL_DIAG_CONF:
		if(m_pUserCbf_DiagReq) {
			Ret = PNIO_ERR_ALREADY_DONE;
		} else {
			m_pUserCbf_DiagReq = cbf;
			Ret = PNIO_OK;
		}
		break;

    case PNIO_CBE_IOSYSTEM_RECONFIG:
		if(m_pUserCbf_IosytemReconfig) {
			Ret = PNIO_ERR_ALREADY_DONE;
		} else {
			m_pUserCbf_IosytemReconfig = cbf;
			Ret = PNIO_OK;
		}
		break;
    default:
        Ret = PNIO_ERR_PRM_TYPE;
        break;
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<register_cbf");

    return Ret;
}

/*************************************************************************/
PNIO_VOID IController::de_register_all_cbf()
{
    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>de_register_cbf");

    m_pUserCbf_Mode = 0;
    m_pUserCbf_DevAct= 0;
    m_pUserCbf_Alarm = 0;
    m_pUserCbf_DR_Read = 0;
    m_pUserCbf_DR_Write = 0;
    m_pCbf_AlarmInd = 0;
	m_pUserCbf_DiagReq = 0;
    m_pUserCbf_IosytemReconfig = 0;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<de_register_cbf");
}

/*************************************************************************/
PNIO_UINT32 IController::controller_open(PNIO_UINT32 CpIndex,
    PNIO_UINT32 ExtPar, PNIO_CBF cbf_RecReadConf,
    PNIO_CBF cbf_RecWriteConf, PNIO_CBF cbf_AlarmInd,
    PNIO_UINT32 * pApplHandle)
{
    PNIO_UINT32 result = PNIO_OK;
    IController *pThis = NULL;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>controller_open");

    if(!pApplHandle)
        return PNIO_ERR_PRM_HND;

    if(!cbf_RecReadConf || !cbf_RecWriteConf)
        return PNIO_ERR_PRM_CALLBACK;

    if(!(ExtPar & PNIO_CEP_MODE_CTRL))
        return PNIO_ERR_PRM_EXT_PAR;

    if(ExtPar & PNIO_CEP_MODE_CTRL) {
    // to do : from SCCI
    // only one instance mit PNIO_CEP_MODE_CTRL on CP can exist
    }

    *pApplHandle = IController::get_handle_by_index(CpIndex);
    if (*pApplHandle == PND_INVALID_HANDLE)
        return PNIO_ERR_PRM_CP_ID;

    pThis = IController::get_instance(CpIndex);
    if(pThis == PNIO_NULL )
        return PNIO_ERR_CREATE_INSTANCE;

    if (pThis->m_status != pnd_controller_closed)
        return PNIO_ERR_WRONG_HND;

    pThis->m_ExtPar = ExtPar;
    pThis->m_CpIndex = CpIndex;
    pThis->m_pUserCbf_Alarm = cbf_AlarmInd;
    pThis->m_pUserCbf_DR_Read = cbf_RecReadConf;
    pThis->m_pUserCbf_DR_Write = cbf_RecWriteConf;
    pThis->m_Mode = PNIO_MODE_OFFLINE;

    // Wait on callback, after request 
    /*******/

    PND_RQB_PTR_TYPE               pRQB;
    PND_IOB_CORE_OPEN_PTR_TYPE     pOpen;
    PNIO_VOID_PTR_TYPE  pnd_laddr_list_ptr;
	PNIO_VOID_PTR_TYPE  pnd_dev_number_list_ptr;
	PNIO_VOID_PTR_TYPE  pnd_slot_number_list_ptr;
	PNIO_VOID_PTR_TYPE  pnd_subslot_number_list_ptr;

    pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));
   // pRQB = pThis->pnd_lower_rqb_alloc(PND_OPC_CONTROLLER_OPEN, 0, pThis, IController::controller_open_done);

    if(!pRQB)
    {
        return PNIO_ERR_NO_RESOURCE;
    }
    
    if(g_pnd_log_addr.isInitialized != PNIO_TRUE)
    {
        pnd_laddr_list_ptr = pnd_mem_alloc( sizeof(g_pnd_log_addr.logAddrList) );
    }
    else
    {
        pnd_laddr_list_ptr = PNIO_NULL;
    }

	pnd_dev_number_list_ptr = pnd_mem_alloc( sizeof(pThis->m_pndDevNumberList) );
	pnd_slot_number_list_ptr = pnd_mem_alloc( sizeof(pThis->m_pndSlotNumberList) );
	pnd_subslot_number_list_ptr = pnd_mem_alloc( sizeof(pThis->m_pndSubslotNumberList) );

    if (pRQB == 0)
        PND_FATAL("mem alloc failed");

    pOpen = &pRQB->args.iob_open;

    pnd_memset(pOpen, 0, sizeof(PND_IOB_CORE_OPEN_TYPE));

    pOpen->laddr_list_ptr = pnd_laddr_list_ptr;
    pOpen->dev_number_list_ptr = pnd_dev_number_list_ptr;
	pOpen->slot_number_list_ptr = pnd_slot_number_list_ptr;
	pOpen->subslot_number_list_ptr = pnd_subslot_number_list_ptr;

    pOpen->pnd_handle.cp_nr = (PNIO_UINT16)CpIndex;

    PND_ASSERT( pRQB );

    result = pnd_alloc_event( &pThis->m_pnd_ioc_wait_event );
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    
    pnd_reset_event( pThis->m_pnd_ioc_wait_event );

    LSA_RQB_SET_OPCODE( pRQB, PND_OPC_CONTROLLER_OPEN );
    LSA_RQB_SET_HANDLE( pRQB, 0 );
    PND_RQB_SET_USER_INST_PTR(pRQB, pThis);
    PND_RQB_SET_USER_CBF_PTR(pRQB, IController::controller_open_done);
    
    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnd_iob_core_request, (PNIO_VOID_PTR_TYPE)pRQB);

    pnd_wait_event( pThis->m_pnd_ioc_wait_event );

    result = pnd_free_event( pThis->m_pnd_ioc_wait_event );
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    pThis->m_nr_of_alarm_res = pOpen->nr_of_alarm_res;

    result = pRQB->_response;
    if(result != PNIO_RET_OK)
        return PNIO_ERR_INTERNAL;
    else
        result = PNIO_OK;

    /******/
    result = pOpen->pnio_err;
    if (result != PNIO_OK)
        return result;

    if(g_pnd_log_addr.isInitialized != PNIO_TRUE)
    {
        pnd_memcpy(g_pnd_log_addr.logAddrList, pOpen->laddr_list_ptr, sizeof(g_pnd_log_addr.logAddrList));
        g_pnd_log_addr.isInitialized = PNIO_TRUE;

        pnd_mem_free( pnd_laddr_list_ptr);
    }
    pnd_memcpy(pThis->m_pndDevNumberList, pOpen->dev_number_list_ptr, sizeof(pThis->m_pndDevNumberList));
	pnd_memcpy(pThis->m_pndSlotNumberList, pOpen->slot_number_list_ptr, sizeof(pThis->m_pndSlotNumberList));
	pnd_memcpy(pThis->m_pndSubslotNumberList, pOpen->subslot_number_list_ptr, sizeof(pThis->m_pndSubslotNumberList));
    
    // Create addressMap
    for(PNIO_UINT32 i = 0; i < PND_LADDR_MAX; i++)
    {
        if( g_pnd_log_addr.logAddrList[i] >= PDSUBSLOTINTERFACE && g_pnd_log_addr.logAddrList[i] <= IOCSUBSLOT )
        {
            GEO_ADDR geoAddr;
            geoAddr.device  = pThis->m_pndDevNumberList[i];
            geoAddr.slot    = pThis->m_pndSlotNumberList[i];
            geoAddr.subslot = pThis->m_pndSubslotNumberList[i];

            AddNode(addressMap, i, geoAddr);
        }
    }
        
    pnd_mem_free( pnd_dev_number_list_ptr );
	pnd_mem_free( pnd_slot_number_list_ptr );
	pnd_mem_free( pnd_subslot_number_list_ptr );

    pnd_mem_free(pRQB);

    result = pThis->alarm_provide(pThis ,pThis->m_nr_of_alarm_res);
    if (result != PNIO_OK)
        return  PNIO_ERR_NO_RESOURCE;

    pThis->write_snmp_record(pThis);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<controller_open");

    pThis->m_set_status( pnd_controller_opened );

    result = pnd_alloc_event(&pnd_rqb_list.resource_free_event);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    pnd_set_event( pnd_rqb_list.resource_free_event );

    result = pnd_alloc_event(&pnd_datarecord_read_rqb_list.resource_free_event);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    pnd_set_event( pnd_datarecord_read_rqb_list.resource_free_event );

    result = pnd_alloc_event(&pnd_datarecord_write_rqb_list.resource_free_event);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    pnd_set_event( pnd_datarecord_write_rqb_list.resource_free_event );

    return PNIO_OK;
}

/*************************************************************************/
PNIO_UINT32 IController::controller_close(PNIO_UINT32 ApplHandle)
{
    PNIO_UINT32 result = PNIO_OK;
    IController *pThis = NULL;
    PND_RQB_PTR_TYPE                pRQB;
    PND_IOB_CORE_CLOSE_PTR_TYPE     pClose;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>controller_close");

    pThis = IController::get_instance(ApplHandle);

    PND_ASSERT( pThis != PNIO_NULL );

    if(!pThis) 
        return PNIO_ERR_WRONG_HND;

    if (pnd_controller_closed == pThis->m_get_status())
    {
        return PNIO_ERR_WRONG_HND;
    }

    pThis->de_register_all_cbf();

    pnd_wait_event(pnd_rqb_list.resource_free_event);
    pnd_wait_event(pnd_datarecord_read_rqb_list.resource_free_event);
    pnd_wait_event(pnd_datarecord_write_rqb_list.resource_free_event);

    if (pThis->m_Mode != PNIO_MODE_OFFLINE)
    {
        result = pThis->set_mode(PNIO_MODE_OFFLINE);
        if(result != PNIO_OK)
        {
            return result;
        }

        pnd_wait_event(pnd_rqb_list.resource_free_event);
    }

    PND_ASSERT(pThis->m_Mode == PNIO_MODE_OFFLINE);

    result = pnd_alloc_event( &pThis->m_pnd_ioc_wait_event );
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }


    pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));
    // pRQB = pnd_lower_rqb_alloc(PND_OPC_CONTROLLER_CLOSE,0,pThis,IController::controller_close_done);
    
    if(!pRQB)
    {
        return PNIO_ERR_INTERNAL;
    }

    pClose = &pRQB->args.iob_close;

    pnd_memset(pClose, 0, sizeof(PND_IOB_CORE_CLOSE_TYPE));
    
    pClose->pnd_handle.cp_nr = (PNIO_UINT16)pThis->m_CpIndex;

    LSA_RQB_SET_OPCODE( pRQB, PND_OPC_CONTROLLER_CLOSE );
    LSA_RQB_SET_HANDLE( pRQB, 0 );
    PND_RQB_SET_USER_INST_PTR(pRQB, pThis);
    PND_RQB_SET_USER_CBF_PTR(pRQB, IController::controller_close_done);
    
    pnd_reset_event( pThis->m_pnd_ioc_wait_event );

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnd_iob_core_request, (PNIO_VOID_PTR_TYPE)pRQB);

    pnd_wait_event( pThis->m_pnd_ioc_wait_event );

    result = pClose->pnio_err;

    pnd_mem_free( pRQB);

    result = pnd_free_event( pThis->m_pnd_ioc_wait_event );
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }


    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<controller_close");

    result = pnd_free_event(pnd_rqb_list.resource_free_event);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    result = pnd_free_event(pnd_datarecord_read_rqb_list.resource_free_event);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    result = pnd_free_event(pnd_datarecord_write_rqb_list.resource_free_event);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    // empty addressMap
    EmptyMap(addressMap);

    pThis->m_set_status( pnd_controller_closed );

    return PNIO_OK;
}

/*************************************************************************/
PNIO_VOID IController::set_mode_done(PNIO_VOID_PTR_TYPE pInst,PNIO_VOID_PTR_TYPE  pRqb)
{
    IController *pThis = (IController *)pInst;
    PNIO_CBE_PRM cbf_prm;

    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>set_mode_done");

    if(LSA_RQB_GET_OPCODE(rqb_ptr) != PND_OPC_SET_MODE) 
    {
        PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CBF_MODE_IND Wrong Opcode ; %d", LSA_RQB_GET_OPCODE(rqb_ptr));
        PND_FATAL("wrong OPCODE");;
    }

    cbf_prm.Handle       = pThis->get_handle(pThis);

    if(pThis->m_pUserCbf_Mode) 
    {
                
        PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CBF_MODE_IND call user callback() ends ApplHandle= %d Mode= %d", cbf_prm.Handle, pThis->m_pUserCbf_Mode);
                
        cbf_prm.CbeType      = PNIO_CBE_MODE_IND;
        cbf_prm.ModeInd.Mode = (PNIO_MODE_TYPE)rqb_ptr->args.iob_set_mode.mode;

        pThis->m_pUserCbf_Mode(&cbf_prm); 
    } 
    else 
    {
        PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "No CBF_MODE_IND user callback() defined. ApplHandle= %d Mode= %d", cbf_prm.Handle, pThis->m_pUserCbf_Mode);
    }

    pThis->m_Mode = (PNIO_MODE_TYPE)rqb_ptr->args.iob_set_mode.mode;

    pThis->m_set_mode_req_running = PNIO_FALSE;

    pThis->pnd_lower_rqb_free(pRqb);

#if 0
    {
        PND_RQB_PTR_TYPE pRQB;

        pThis->pnd_get_from_rqb_queue( &pThis->m_pnd_lower_rqb_queu, &pRQB);

        if (pRQB)
        {
            pThis->m_set_mode_req_running = PNIO_TRUE;
            pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);
        }
    }
#endif // 0

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<set_mode_done");
}

/*************************************************************************/
PNIO_UINT32 IController::set_mode(PNIO_MODE_TYPE Mode)
{
    PND_RQB_PTR_TYPE          pRQB;
    PND_SET_MODE_PTR_TYPE     pSetMode;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>set_mode");

    if(!(m_ExtPar & PNIO_CEP_MODE_CTRL))
        return PNIO_ERR_SET_MODE_NOT_ALLOWED;

    if(Mode != PNIO_MODE_OFFLINE &&
        Mode != PNIO_MODE_CLEAR &&
        Mode != PNIO_MODE_OPERATE)
        return PNIO_ERR_MODE_VALUE;

    if(m_Mode == Mode)
        return PNIO_OK;

    if (m_status != pnd_controller_opened)
        return PNIO_ERR_WRONG_HND;

    // pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));
    pRQB = pnd_lower_rqb_alloc( PND_OPC_SET_MODE, 0, this ,IController::set_mode_done );

    if (!pRQB)
    {
        return PNIO_ERR_NO_RESOURCE;
    }

    pSetMode = &pRQB->args.iob_set_mode;

    pnd_memset(pSetMode, 0, sizeof(PND_SET_MODE_TYPE));

    pSetMode->mode = Mode; // Set Mode in RQB

    pSetMode->pnd_handle.cp_nr = this->m_hInstanceHandle;

    if (m_set_mode_req_running == PNIO_TRUE)
    {
        pnd_lower_rqb_free( pRQB );
        return PNIO_ERR_SEQUENCE;
        // pnd_add_to_rqb_queue( &m_pnd_lower_rqb_queu,  pRQB);
    }
    else
    {
        m_set_mode_req_running = PNIO_TRUE;
        pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<set_mode");

    return PNIO_OK;
}
/*************************************************************************/
PNIO_UINT32 IController::ctrl_diag_request(LSA_HANDLE_TYPE handle, PNIO_CTRL_DIAG *pDiagInfo)
{
	PND_RQB_PTR_TYPE       pRQB = PNIO_NULL;
	PND_DIAG_REQ_PTR_TYPE  pDiag = PNIO_NULL;
	PNIO_ADDR *            pAddr = &(pDiagInfo->u.Addr);

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>ctrl_diag_request");

    if (this->m_status != pnd_controller_opened)
        return PNIO_ERR_WRONG_HND;

    if(!pDiagInfo) {
		return PNIO_ERR_PRM_POINTER;
	}

	if( pDiagInfo->DiagService != PNIO_CTRL_DIAG_CONFIG_SUBMODULE_LIST &&
		pDiagInfo->DiagService != PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC &&
		pDiagInfo->DiagService != PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO ) {
			return PNIO_ERR_PRM_INVALIDARG;
	}

	// LADDR is given only in PNIO_CTRL_DIAG_DEVICE_STATE service.
	if( pDiagInfo->DiagService == PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC)
	{
		PNIO_UINT32 result = PNIO_OK;

		if(!pAddr ||
			pAddr->AddrType != PNIO_ADDR_LOG ||
			(pAddr->IODataType != PNIO_IO_IN && pAddr->IODataType != PNIO_IO_OUT))
			return PNIO_ERR_PRM_ADD;

		result = check_log_adress(pAddr);
		if (result != PNIO_OK)
		{
			return result;
		}
	}

    pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));

	if(!pRQB)
    {
        return PNIO_ERR_INTERNAL;
    }

    LSA_RQB_SET_OPCODE( pRQB, PND_OPC_DIAG_REQ );
    LSA_RQB_SET_HANDLE( pRQB, handle );
    PND_RQB_SET_USER_INST_PTR(pRQB, this);
    PND_RQB_SET_USER_CBF_PTR(pRQB, IController::ctrl_diag_req_done);

	pDiag = &pRQB->args.diag_req;
    
    pDiag->pnio_err = PNIO_OK;
	pDiag->DiagReq = (PNIO_CTRL_DIAG *) pnd_mem_alloc( sizeof(PNIO_CTRL_DIAG));
	PND_ASSERT( pDiag->DiagReq );

	pnd_memset(pDiag->DiagReq, 0, sizeof(PNIO_CTRL_DIAG));

	// transferring the diagnostic information
	pnd_memcpy(pDiag->DiagReq, pDiagInfo, sizeof(PNIO_CTRL_DIAG));

	pDiag->pnd_handle.cp_nr = (PNIO_UINT16)m_CpIndex;

	pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<ctrl_diag_request");

	return PNIO_OK;
}

/*************************************************************************/
PNIO_UINT32 IController::iosystem_reconfig( PNIO_UINT32 handle,
										    PNIO_IOS_RECONFIG_MODE Mode, 
											PNIO_UINT32 DeviceCnt, PNIO_ADDR * DeviceList, 
											PNIO_UINT32 PortInterconnectionCnt, PNIO_ADDR * PortInterconnectionList)
{
	PND_RQB_PTR_TYPE pRQB = PNIO_NULL;

	PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>iosystem_reconfig, Mode : %d", Mode);

	if (this->m_status != pnd_controller_opened)
        return PNIO_ERR_WRONG_HND;

	if(!(m_ExtPar & PNIO_CEP_MODE_CTRL))
        return PNIO_ERR_DEV_ACT_NOT_ALLOWED;

	 if( handle != get_handle(this) )
        return PNIO_ERR_PRM_HND;

	 if(( Mode != PNIO_IOS_RECONFIG_MODE_DEACT) && ( Mode != PNIO_IOS_RECONFIG_MODE_TAILOR))
		 return PNIO_ERR_PRM_TYPE;

	if( m_iosys_reconfig_running == PNIO_TRUE )
        return PNIO_ERR_NOT_REENTERABLE;

    if( Mode == PNIO_IOS_RECONFIG_MODE_TAILOR )
	{
		PNIO_UINT32 result = IOSysRcfg_StoreUserParams( DeviceCnt, DeviceList, 
		                            PortInterconnectionCnt, PortInterconnectionList );

        if(result != PNIO_OK)
            return result;
	}

    m_iosys_reconfig_running = PNIO_TRUE;

	// store Mode information for CBE
	m_iOSystemReconfigMode = Mode;
	
	// IOSystem Reconfig RQB
    pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));

    if(!pRQB)
    {
	    return PNIO_ERR_INTERNAL;
    }
	pnd_memset(pRQB, 0, sizeof(PND_RQB_TYPE));

	// execute state-machine
	iosystem_reconfig_sm(pRQB);

	PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<iosystem_reconfig, Mode : %d", Mode);

	return PNIO_OK;

}
/*************************************************************************/
PNIO_VOID IController::iosystem_reconfig_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
	PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE) pRqb;
    IController *pThis       = (IController *) pInst;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>iosystem_reconfig_done");

	if(pThis->m_state_iosystem_reconfig == IDLE)
	{
        PNIO_CBE_PRM cbf_prm;
        PNIO_ADDR addr;

        cbf_prm.Handle  = get_handle(pThis);
		cbf_prm.CbeType = PNIO_CBE_IOSYSTEM_RECONFIG;
        cbf_prm.IoSystemReconf.pAddr              = &addr;
        cbf_prm.IoSystemReconf.pAddr->u.Addr      = rqb_ptr->args.iosys_recon.Addr.u.Addr;
        cbf_prm.IoSystemReconf.pAddr->AddrType    = rqb_ptr->args.iosys_recon.Addr.AddrType;
		cbf_prm.IoSystemReconf.pAddr->IODataType  = rqb_ptr->args.iosys_recon.Addr.IODataType;
        cbf_prm.IoSystemReconf.Mode               = rqb_ptr->args.iosys_recon.Mode;
        cbf_prm.IoSystemReconf.Status             = rqb_ptr->args.iosys_recon.pnio_err;

		if(pThis->m_pUserCbf_IosytemReconfig) 
		{
			PND_IOBC_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CBF_IOSYSTEM_RECONFIG call user callback() finished with Result = %d. ApplHandle = %d Cbf = %d", 
										                    cbf_prm.IoSystemReconf.Status,
										                    cbf_prm.Handle, 
															pThis->m_pUserCbf_IosytemReconfig );
			
			pThis->m_pUserCbf_IosytemReconfig( &cbf_prm );
		}
		else
		{
			PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "No CBF_IOSYSTEM_RECONFIG user callback() defined. ApplHandle = %d Cbf = %d", 
															cbf_prm.Handle, 
															pThis->m_pUserCbf_IosytemReconfig);
		}

        pThis->m_pnioErrorCodeOfLastFailedDevice = PNIO_OK;
        pnd_memset( &pThis->m_laddrOfLastFailedDevice, 0,  sizeof(PNIO_ADDR) );
		
        pnd_mem_free(rqb_ptr);

		pThis->m_iosys_reconfig_running = PNIO_FALSE;
	}
	else
    {
		// execute state-machine
		pThis->iosystem_reconfig_sm(rqb_ptr);
	}
	
	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<iosystem_reconfig_done");
}


/*************************************************************************/
PNIO_VOID IController::iosystem_reconfig_sm(PNIO_VOID_PTR_TYPE  pRqb)
{
    PNIO_UINT32 result;
    PNIO_ADDR laddr;
	PND_RQB_PTR_TYPE rqb = (PND_RQB_PTR_TYPE) pRqb;

	switch(m_state_iosystem_reconfig)
	{
		case IDLE:
                        if(m_iOSystemReconfigMode == PNIO_IOS_RECONFIG_MODE_DEACT)
				        {
                            m_state_iosystem_reconfig = DEACT;
                            IOSysRcfg_PrepareDeactPendingList(m_PendingActivationList); 

                            pnd_mem_free(rqb);

                            result = PNIO_TRUE;
                            while(result)
                            {
                                result = IsInProgressListNotFull( m_InProgressActivationList );
                                if( result )
					            {
                                    result = GetNextDeviceFromPendingList( &laddr, 
																            m_PendingActivationList,
																            m_InProgressActivationList);
							        if(result)
						            {
                                        PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "Next Device Address to de-activate : %d, activationType : %d", 
                                                                                        laddr.u.Addr, 
                                                                                        PNIO_DA_FALSE );

								        PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));
                                        pnd_memset(pRQB, 0, sizeof(PND_RQB_TYPE));

								        IOSysRcfg_PrepareDeActRequest(laddr, PNIO_DA_FALSE, pRQB);
								        pnd_request_start( PND_MBX_ID_IO_BASE_CORE, 
												           (PND_REQUEST_FCT)pnio_user_request_start, 
												           (PNIO_VOID_PTR_TYPE)pRQB);
			                        }
	                            }
                            }

                        }
						else if(m_iOSystemReconfigMode == PNIO_IOS_RECONFIG_MODE_TAILOR)
                        {
							m_state_iosystem_reconfig = TAILOR;

                            laddr.u.Addr = GetLogAddrOfInterfaceSubmodule();
							IOSysRcfg_PrepareReadReq(laddr, rqb);
							pnd_request_start( PND_MBX_ID_IO_BASE_CORE, 
											   (PND_REQUEST_FCT)pnio_user_request_start, 
											   (PNIO_VOID_PTR_TYPE)rqb);
						}
		                break;

		case DEACT:
                        if( rqb->args.iob_dev_act.pnio_err != PNIO_OK )
				        {
					        m_pnioErrorCodeOfLastFailedDevice = rqb->args.iob_dev_act.pnio_err;
					        m_laddrOfLastFailedDevice         = rqb->args.iob_dev_act.Addr;
				        }

                        RemoveDeviceFromInProgressList( rqb->args.iob_dev_act.Addr, m_InProgressActivationList );
				
                        if( m_InProgressActivationList.empty() && m_PendingActivationList.empty() )
				        {
                            m_state_iosystem_reconfig = IDLE;

                            rqb->args.iosys_recon.Mode     = rqb->args.iob_dev_act.DevActMode;
                            rqb->args.iosys_recon.Addr     = m_laddrOfLastFailedDevice;
							rqb->args.iosys_recon.pnio_err = m_pnioErrorCodeOfLastFailedDevice;

							iosystem_reconfig_done(this, rqb);
                            break;
                        }

                        result = GetNextDeviceFromPendingList(&laddr, m_PendingActivationList, m_InProgressActivationList);
						if(result)
						{
                            PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "Next Device Address to de-activate : %d, activationType : %d", 
                                                                                        laddr.u.Addr, 
                                                                                        PNIO_DA_FALSE );

							IOSysRcfg_PrepareDeActRequest(laddr, PNIO_DA_FALSE, rqb);
							pnd_request_start( PND_MBX_ID_IO_BASE_CORE, 
											   (PND_REQUEST_FCT)pnio_user_request_start, 
											   (PNIO_VOID_PTR_TYPE)rqb);
				        }
				        else
				        {
					        pnd_mem_free(rqb);
				        }
				        break;

		case TAILOR:
                        PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "PDIOSystemInfo was read! Pnio Error : %d",
                                                                         rqb->args.rec_read.pnio_err );

                       result = IOSysRcfg_EvaluateReadReq(rqb);

						if(result)
						{
							m_state_iosystem_reconfig = TAILOR_WRITE;
							laddr.u.Addr = GetLogAddrOfInterfaceSubmodule();
							IOSysRcfg_PrepareWriteReq(laddr, rqb);
							pnd_request_start( PND_MBX_ID_IO_BASE_CORE, 
											  (PND_REQUEST_FCT)pnio_user_request_start, 
											  (PNIO_VOID_PTR_TYPE)rqb );
						}
						else
						{
							m_state_iosystem_reconfig = IDLE;
							IOSysRcfg_FreeUserParams();
                            
							iosystem_reconfig_done(this, rqb);
						}
		                break;
							
		case TAILOR_WRITE:

                        PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "PDInstanceTailorData was written! Pnio Error : %d", 
                                                                         rqb->args.rec_write.pnio_err );
                                                
						result = IOSysRcfg_EvaluateWriteReq(rqb);

						if(result)
						{
                            m_state_iosystem_reconfig = TAILOR_ACT;

							IOSysRcfg_PrepareActPendingList(m_PendingActivationList);

                            pnd_mem_free(rqb);

                            result = PNIO_TRUE;
                            while(result)
							{
                                result = IsInProgressListNotFull( m_InProgressActivationList );
                                if( result )
						        {
                                    result = GetNextDeviceFromPendingList( &laddr, 
																            m_PendingActivationList,
																            m_InProgressActivationList);
							        if(result)
							        {
                                        PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "Next Device Address to activate : %d, activationType : %d", 
                                                                                        laddr.u.Addr, 
                                                                                        PNIO_DA_TRUE );

								        PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));
                                        pnd_memset(pRQB, 0, sizeof(PND_RQB_TYPE));

                                        IOSysRcfg_PrepareDeActRequest(laddr, PNIO_DA_TRUE, pRQB);
								        pnd_request_start( PND_MBX_ID_IO_BASE_CORE, 
												           (PND_REQUEST_FCT)pnio_user_request_start, 
												           (PNIO_VOID_PTR_TYPE)pRQB);
									}
								}
							}
						}
						else
						{
							m_state_iosystem_reconfig = IDLE;
							IOSysRcfg_FreeUserParams();

							iosystem_reconfig_done(this, rqb);
						}
						break;

		case TAILOR_ACT:
                        if( rqb->args.iob_dev_act.pnio_err != PNIO_OK )
						{
					        m_pnioErrorCodeOfLastFailedDevice = rqb->args.iob_dev_act.pnio_err;
					        m_laddrOfLastFailedDevice         = rqb->args.iob_dev_act.Addr;
						}

                        RemoveDeviceFromInProgressList( rqb->args.iob_dev_act.Addr, m_InProgressActivationList );
				
                        if( m_InProgressActivationList.empty() && m_PendingActivationList.empty() )
						{
                            m_state_iosystem_reconfig = IDLE;
							IOSysRcfg_FreeUserParams();

                            rqb->args.iosys_recon.Mode     = rqb->args.iob_dev_act.DevActMode;
                            rqb->args.iosys_recon.Addr     = m_laddrOfLastFailedDevice;
							rqb->args.iosys_recon.pnio_err = m_pnioErrorCodeOfLastFailedDevice;

							iosystem_reconfig_done(this, rqb);
                            break;
						}

						result = GetNextDeviceFromPendingList(&laddr, m_PendingActivationList, m_InProgressActivationList);
						if(result)
						{
                            PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "Next Device Address to activate : %d, activationType : %d", 
                                                                                        laddr.u.Addr, 
                                                                                        PNIO_DA_TRUE );

							IOSysRcfg_PrepareDeActRequest(laddr, PNIO_DA_TRUE, rqb);
							pnd_request_start( PND_MBX_ID_IO_BASE_CORE, 
											    (PND_REQUEST_FCT)pnio_user_request_start, 
											    (PNIO_VOID_PTR_TYPE)rqb );
						}
						else
						{
							pnd_mem_free(rqb);
						}
						break;
	}
}

/*************************************************************************/
PNIO_VOID IController::device_activate_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    IController *pThis = (IController *)pInst;
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>device_activate_done");

    if(LSA_RQB_GET_OPCODE(rqb_ptr) != PND_OPC_DEVICE_ACTIVATE) {
        PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "wrong OPCODE : %d", LSA_RQB_GET_OPCODE(rqb_ptr));
        PND_FATAL("wrong OPCODE");;
    }    

	pThis->RemoveDeviceFromInProgressList( rqb_ptr->args.iob_dev_act.Addr, pThis->m_InProgressActivationList );

    if(pThis->m_pUserCbf_DevAct) 
    {
        PND_DEV_ACTIVATE_PTR_TYPE pDevActivate = &rqb_ptr->args.iob_dev_act;
        PNIO_ADDR Addr;
        /*TRC_OUT01(GR_STATE, LV_FCTCLBF,
        "-> CBF_DEV_ACT_IND call user callback() ApplHandle=0x%x",
        Handle); */

        PNIO_CBE_PRM cbf_prm;
        cbf_prm.Handle = pThis->get_handle(pThis);

        PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "-> CBF_DEV_ACT_IND call user callback() ApplHandle= %d", cbf_prm.Handle);

        cbf_prm.CbeType = PNIO_CBE_DEV_ACT_CONF;

        Addr.AddrType   = (PNIO_ADDR_TYPE)(pDevActivate->Addr.AddrType);
        Addr.IODataType = (PNIO_IO_TYPE)(pDevActivate->Addr.IODataType);
        Addr.u.Addr     = (pDevActivate->Addr.u.Addr);
        cbf_prm.DevActConf.pAddr = &Addr;

        cbf_prm.DevActConf.Mode   = (PNIO_DEV_ACT_TYPE)(pDevActivate->DevActMode);
        cbf_prm.DevActConf.Result = pDevActivate->pnio_err;

        pThis->m_pUserCbf_DevAct(&cbf_prm);
            /*    TRC_OUT01(GR_STATE, LV_FCTCLBF,
                    "<- CBF_DEV_ACT_IND call user callback() ends ApplHandle=0x%x", Handle); */
        PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CBF_DEV_ACT_IND call user callback() finished.");
    } 
    else 
    {
        /* no callback fct registered */
        PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CBF_DEV_ACT_IND : No user callback fct registered");
    }

    pThis->pnd_lower_rqb_free(pRqb);


    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<device_activate_done");
}

/*************************************************************************/
PNIO_UINT32 IController::device_activate(const PNIO_ADDR * pAddr, PNIO_DEV_ACT_TYPE mode)
{
    PNIO_UINT32 result = PNIO_OK;

    PND_RQB_PTR_TYPE          pRQB;
    PND_DEV_ACTIVATE_PTR_TYPE     pActivate;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>device_activate");

    if (this->m_status != pnd_controller_opened)
        return PNIO_ERR_WRONG_HND;

    if(!pAddr ||
        pAddr->AddrType != PNIO_ADDR_LOG ||
        (pAddr->IODataType != PNIO_IO_IN && pAddr->IODataType != PNIO_IO_OUT))
        return PNIO_ERR_PRM_ADD;

    // sanity addres bound
    if(pAddr->u.Addr > 0x7FFF)
        return PNIO_ERR_PRM_ADD;

    if(!(m_ExtPar & PNIO_CEP_MODE_CTRL))
        return PNIO_ERR_DEV_ACT_NOT_ALLOWED;

    if(mode != PNIO_DA_FALSE && mode != PNIO_DA_TRUE)
        return PNIO_ERR_MODE_VALUE;

    if( IsInProgressListNotFull( m_InProgressActivationList ) == PNIO_FALSE )
	{
		return  PNIO_ERR_NO_RESOURCE;
	}

    pRQB = pnd_lower_rqb_alloc(PND_OPC_DEVICE_ACTIVATE, 0, this, device_activate_done);

    if (!pRQB)
    {
        return PNIO_ERR_NO_RESOURCE;
    }

	AddDeviceToInProgressList( pAddr->u.Addr, m_InProgressActivationList );

    pActivate =&pRQB->args.iob_dev_act;

    pnd_memset(pActivate, 0, sizeof(PND_DEV_ACTIVATE_TYPE));

    LSA_RQB_SET_OPCODE( pRQB, PND_OPC_DEVICE_ACTIVATE);
    PND_RQB_SET_USER_CBF_PTR(pRQB, device_activate_done);
    PND_RQB_SET_USER_INST_PTR(pRQB, this);

    pActivate->Addr.AddrType = pAddr->AddrType;
    pActivate->Addr.IODataType = pAddr->IODataType;
    pActivate->Addr.u.Addr = pAddr->u.Addr;

    pActivate->pnd_handle.cp_nr = this->m_hInstanceHandle;

    pActivate->DevActMode = mode;

    result = pnd_alloc_event( &this->m_pnd_ioc_wait_event );
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);

    result = pnd_free_event( this->m_pnd_ioc_wait_event );
    if(result != PNIO_RET_OK)          // PNIO_RET_OK != PNIO_OK !!!
    {
        PND_FATAL("event free failed");
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<device_activate");

    return PNIO_OK;
}

/*************************************************************************/

PNIO_UINT32 IController::check_log_adress(PNIO_ADDR * pAddr)
{
    if(pAddr == PNIO_NULL)
    {
        return PNIO_ERR_PRM_ADD;
    }

    PNIO_UINT32 laddr = pAddr->u.Addr;

    if ((laddr > PND_LADDR_MAX) || (laddr <= 0))
    {
        return PNIO_ERR_PRM_ADD;
    }
        
    if ((g_pnd_log_addr.logAddrList[laddr] > IOCSUBSLOT) || (g_pnd_log_addr.logAddrList[laddr] <= UNKNOWN)) 
    {
        return PNIO_ERR_PRM_ADD;
    }
    else
    {
        return PNIO_OK;
    }
}

/*************************************************************************/
PNIO_VOID IController::read_write_record_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;
    IController *pThis = (IController *)pInst;

    PNIO_ADDR Addr;
    PNIO_CBE_PRM cbf_prm;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>read_write_record_done");

    switch (LSA_RQB_GET_OPCODE(rqb_ptr))
    {
        case PND_OPC_REC_READ:
            {
                PND_RECORD_READ_WRITE_PTR_TYPE pRecRead = &rqb_ptr->args.rec_read;

                cbf_prm.CbeType = PNIO_CBE_REC_READ_CONF;
                cbf_prm.Handle  = get_handle(pThis);

                Addr.AddrType   = (PNIO_ADDR_TYPE)pRecRead->Addr.AddrType;
                Addr.IODataType = (PNIO_IO_TYPE)pRecRead->Addr.IODataType;
                Addr.u.Addr     = pRecRead->Addr.u.Addr;
                cbf_prm.RecReadConf.pAddr = &Addr;

                cbf_prm.RecReadConf.RecordIndex = pRecRead->RecordIndex;
            
                cbf_prm.RecReadConf.ReqRef  = pRecRead->UserRef;
                cbf_prm.RecReadConf.pBuffer = (PNIO_UINT8 *)pRecRead->pBuffer;
                cbf_prm.RecReadConf.Length  = pRecRead->Length;

                cbf_prm.RecReadConf.Err.ErrCode   = pRecRead->err.ErrCode;
                cbf_prm.RecReadConf.Err.ErrDecode = pRecRead->err.ErrDecode;
                cbf_prm.RecReadConf.Err.ErrCode1  = pRecRead->err.ErrCode1;
                cbf_prm.RecReadConf.Err.ErrCode2  = pRecRead->err.ErrCode2;
                cbf_prm.RecReadConf.Err.AddValue1 = (PNIO_UINT16) pRecRead->pnio_err;
                cbf_prm.RecReadConf.Err.AddValue2 = (PNIO_UINT16) pRecRead->pnio_err;

                if (pThis->m_pUserCbf_DR_Read)
                {
                  pThis->m_pUserCbf_DR_Read(&cbf_prm);
                }
                else
                {
                    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "NO CBF for Data Record Read/Write defined. ");
                }

                pnd_mem_free(pRecRead->pBuffer); // free record read Buffer allocated by record_read
                break;
            }
        case PND_OPC_REC_WRITE:
            {
                PND_RECORD_READ_WRITE_PTR_TYPE pRecWrite = &rqb_ptr->args.rec_write;
                cbf_prm.CbeType = PNIO_CBE_REC_WRITE_CONF;
                cbf_prm.Handle  = get_handle(pThis);

                cbf_prm.CbeType = PNIO_CBE_REC_WRITE_CONF;

                Addr.AddrType   = (PNIO_ADDR_TYPE)(pRecWrite->Addr.AddrType);
                Addr.IODataType = (PNIO_IO_TYPE)(pRecWrite->Addr.IODataType);
                Addr.u.Addr     = pRecWrite->Addr.u.Addr;
                cbf_prm.RecWriteConf.pAddr = &Addr;

                cbf_prm.RecWriteConf.RecordIndex = pRecWrite->RecordIndex;
                cbf_prm.RecWriteConf.ReqRef      = pRecWrite->UserRef;

                cbf_prm.RecWriteConf.Err.ErrCode   = pRecWrite->err.ErrCode;
                cbf_prm.RecWriteConf.Err.ErrDecode = pRecWrite->err.ErrDecode;
                cbf_prm.RecWriteConf.Err.ErrCode1  = pRecWrite->err.ErrCode1;
                cbf_prm.RecWriteConf.Err.ErrCode2  = pRecWrite->err.ErrCode2;
                cbf_prm.RecWriteConf.Err.AddValue1 = (PNIO_UINT16) pRecWrite->pnio_err;
                cbf_prm.RecWriteConf.Err.AddValue2 = (PNIO_UINT16) pRecWrite->pnio_err;

                if (pThis->m_pUserCbf_DR_Write)
                {
                  pThis->m_pUserCbf_DR_Write(&cbf_prm);
                }
                else
                {
                    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "NO CBF for Data Record Read/Write defined. ");
                }

                pnd_mem_free(pRecWrite->pBuffer); // free record write Buffer allocated by record_write
                break;
            }
        default:
        PND_FATAL("wrong OPCODE");;
    }

    pThis->pnd_lower_rqb_free(pRqb);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<read_write_record_done");
}

/*************************************************************************/

PNIO_VOID IController::ctrl_diag_req_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
	PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;
	IController *pThis = (IController *)pInst;

	PNIO_CBE_PRM cbf_prm;
	cbf_prm.Handle = pThis->get_handle(pThis);
	cbf_prm.CbeType = PNIO_CBE_CTRL_DIAG_CONF;

	if(pThis->m_pUserCbf_DiagReq) 
	{
		cbf_prm.CtrlDiagConf.pDiagData         = (PNIO_CTRL_DIAG *)rqb_ptr->args.diag_req.DiagReq;
		cbf_prm.CtrlDiagConf.DiagDataBufferLen = (PNIO_UINT32) rqb_ptr->args.diag_req.DiagDataBufferLen;
		cbf_prm.CtrlDiagConf.pDiagDataBuffer   = (PNIO_UINT8 *) rqb_ptr->args.diag_req.pDiagDataBuffer;
		cbf_prm.CtrlDiagConf.ErrorCode         = (PNIO_UINT32) rqb_ptr->args.diag_req.pnio_err;

		pThis->m_pUserCbf_DiagReq(&cbf_prm);
	} 
	else 
	{
        PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "No CBF for Diagnostic Request user callback() defined. ApplHandle= %d Mode= %d", cbf_prm.Handle, pThis->m_pUserCbf_DiagReq);
	}
	
	pnd_mem_free(rqb_ptr->args.diag_req.pDiagDataBuffer); 
	pnd_mem_free(rqb_ptr->args.diag_req.DiagReq); // free DiagReq allocated by ctrl_diag_request
	pnd_mem_free(rqb_ptr);

}

/*************************************************************************/
PNIO_UINT32 IController::record_read(PNIO_ADDR * pAddr, PNIO_REF ReqRef, PNIO_UINT32 RecordIndex, PNIO_UINT32 Length)
{
   PNIO_UINT32 result = PNIO_OK;
    PND_RQB_PTR_TYPE                pRQB;
    PND_RECORD_READ_WRITE_PTR_TYPE  pRecRead;
    PNIO_UINT8 * pRecBuffer;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>record_read");

    if (this->m_status != pnd_controller_opened)
        return PNIO_ERR_WRONG_HND;

    if(!pAddr ||
        pAddr->AddrType != PNIO_ADDR_LOG ||
        (pAddr->IODataType != PNIO_IO_IN && pAddr->IODataType != PNIO_IO_OUT))
        return PNIO_ERR_PRM_ADD;

        // sanity addres bound
    if(pAddr->u.Addr > 0x7FFF)
        return PNIO_ERR_PRM_ADD;

    result = check_log_adress(pAddr);
    if (result != PNIO_OK)
    {
        return result;
    }

	// check if not controller address
	if (g_pnd_log_addr.logAddrList[pAddr->u.Addr] != IOCDEVICE && 
		g_pnd_log_addr.logAddrList[pAddr->u.Addr] != IOCSLOT   &&
        g_pnd_log_addr.logAddrList[pAddr->u.Addr] != IOCSUBSLOT) 
    {
        return PNIO_ERR_PRM_ADD;
    }

    if(Length == 0 || Length > PNIOI_DREC_MAX_SIZE)
        return PNIO_ERR_VALUE_LEN;

    if(RecordIndex > 0xFFFF)
        return PNIO_ERR_PRM_REC_INDEX;

    pRecBuffer = (PNIO_UINT8 *)pnd_mem_alloc(Length);
    if (pRecBuffer == 0)
            PND_FATAL("mem alloc failed");

    pnd_memset(pRecBuffer,0,Length);

    pRQB = pnd_lower_rqb_alloc(PND_OPC_REC_READ, 0, this, read_write_record_done);

    if (!pRQB)
    {
        PNIO_CBE_PRM cbf_prm;

        PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>read_read CBF");

        cbf_prm.CbeType = PNIO_CBE_REC_READ_CONF;
        cbf_prm.Handle  = get_handle(this);
        cbf_prm.RecReadConf.Err.ErrCode   = 0xDE; // IODReadRes
        cbf_prm.RecReadConf.Err.ErrDecode = 0x80; // PNIORW
        cbf_prm.RecReadConf.Err.ErrCode1  = 0xC3; // Resource  + resource unavailable
        cbf_prm.RecReadConf.Err.ErrCode2  = 0;
        cbf_prm.RecReadConf.Err.AddValue1 = 0;
        cbf_prm.RecReadConf.Err.AddValue2 = 0;

        cbf_prm.RecReadConf.pAddr = pAddr;
        cbf_prm.RecReadConf.RecordIndex = RecordIndex;
        cbf_prm.RecReadConf.ReqRef = ReqRef;
        cbf_prm.RecReadConf.Length = Length;
        cbf_prm.RecReadConf.pBuffer = pRecBuffer;

        if (this->m_pUserCbf_DR_Read)
        {
            this->m_pUserCbf_DR_Read(&cbf_prm);
        }
        else
        {
            PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "NO CBF for Data Record Read defined. ");
        }

        pnd_mem_free(pRecBuffer);

        return PNIO_OK;
    }

    pRecRead = &pRQB->args.rec_read;

    pnd_memset(pRecRead, 0, sizeof(PND_RECORD_READ_WRITE_TYPE));

    pRecRead->Addr.AddrType   = pAddr->AddrType;
    pRecRead->Addr.IODataType = pAddr->IODataType;
    pRecRead->Addr.u.Addr     = pAddr->u.Addr;

    pRecRead->UserRef = ReqRef;

    pRecRead->RecordIndex = RecordIndex;
    pRecRead->Length      = Length;
    pRecRead->pBuffer     = pRecBuffer;

    pRecRead->pnd_handle.cp_nr = this->m_hInstanceHandle;

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<record_read");

    return result;
}

/*************************************************************************/
PNIO_UINT32 IController::record_write(PNIO_ADDR * pAddr, PNIO_REF ReqRef, PNIO_UINT32 RecordIndex,
    PNIO_UINT32 Length, PNIO_UINT8 * pBuffer)
{
    PNIO_UINT32 result = PNIO_OK;
    PND_RQB_PTR_TYPE                pRQB;
    PND_RECORD_READ_WRITE_PTR_TYPE  pRecWrite;
    PNIO_UINT8 * pRecBuffer;

    // Buffer[PNIOI_DREC_MAX_SIZE]; // in, out

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>record_write");

    if (this->m_status != pnd_controller_opened)
        return PNIO_ERR_WRONG_HND;

    if(!pAddr ||
        pAddr->AddrType != PNIO_ADDR_LOG ||
        (pAddr->IODataType != PNIO_IO_IN && pAddr->IODataType != PNIO_IO_OUT))
        return PNIO_ERR_PRM_ADD;

        // sanity addres bound
    if(pAddr->u.Addr > 0x7FFF)
        return PNIO_ERR_PRM_ADD;

    result = check_log_adress(pAddr);
    if (result != PNIO_OK)
    {
        return result;
    }

    // check if not controller address
	if (g_pnd_log_addr.logAddrList[pAddr->u.Addr] != IOCDEVICE && 
		g_pnd_log_addr.logAddrList[pAddr->u.Addr] != IOCSLOT   &&
        g_pnd_log_addr.logAddrList[pAddr->u.Addr] != IOCSUBSLOT) 
    {
        return PNIO_ERR_PRM_ADD;
    }

    if (pBuffer == PNIO_NULL)
    {
        return PNIO_ERR_PRM_BUF;
    }

    if(Length == 0 || Length > PNIOI_DREC_MAX_SIZE)
        return PNIO_ERR_VALUE_LEN;

    if(RecordIndex > 0xFFFF)
        return PNIO_ERR_PRM_REC_INDEX;

    pRQB = pnd_lower_rqb_alloc(PND_OPC_REC_WRITE, 0, this, read_write_record_done);

    if (!pRQB)
    {
        PNIO_CBE_PRM cbf_prm;

        PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>read_write CBF");

        cbf_prm.CbeType = PNIO_CBE_REC_WRITE_CONF;
        cbf_prm.Handle  = get_handle(this);
        cbf_prm.RecWriteConf.Err.ErrCode   = 0xDF; // IODWriteRes
        cbf_prm.RecWriteConf.Err.ErrDecode = 0x80; // PNIORW
        cbf_prm.RecWriteConf.Err.ErrCode1  = 0xC3; // Resource  + resource unavailable
        cbf_prm.RecWriteConf.Err.ErrCode2  = 0;
        cbf_prm.RecWriteConf.Err.AddValue1 = 0;
        cbf_prm.RecWriteConf.Err.AddValue2 = 0;

        cbf_prm.RecWriteConf.pAddr = pAddr;
        cbf_prm.RecWriteConf.RecordIndex = RecordIndex;
        cbf_prm.RecWriteConf.ReqRef = ReqRef;

        if (this->m_pUserCbf_DR_Write)
        {
            this->m_pUserCbf_DR_Write(&cbf_prm);
        }
        else
        {
            PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "NO CBF for Data Record Write defined. ");
        }
        return PNIO_OK;
    }

    pRecWrite =&pRQB->args.rec_write;

    pnd_memset(pRecWrite, 0, sizeof(PND_RECORD_READ_WRITE_TYPE));

    pRecBuffer = (PNIO_UINT8 *)pnd_mem_alloc(Length);
    if (pRecBuffer == 0)
            PND_FATAL("mem alloc failed");

    pnd_memcpy(pRecBuffer,pBuffer,Length);

    pnd_memset(pRQB, 0, sizeof (pRQB));

    LSA_RQB_SET_OPCODE( pRQB, PND_OPC_REC_WRITE);
    PND_RQB_SET_USER_CBF_PTR(pRQB, read_write_record_done);
    PND_RQB_SET_USER_INST_PTR(pRQB, this);

    pRecWrite->Addr.AddrType = pAddr->AddrType;
    pRecWrite->Addr.IODataType = pAddr->IODataType;
    pRecWrite->Addr.u.Addr = pAddr->u.Addr;

    pRecWrite->UserRef = ReqRef;

    pRecWrite->RecordIndex = RecordIndex;
    pRecWrite->Length = Length;
    pRecWrite->pBuffer = pRecBuffer;

    pRecWrite->pnd_handle.cp_nr = this->m_hInstanceHandle;

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<record_write");

    return result;
}

/* constructor */                                                               
/*************************************************************************/
IIOInterface::IIOInterface(PNIO_UINT32 UserHandle):
    m_pUserCbf_Alarm(PNIO_NULL),
    m_pUserCbf_DR_Read(PNIO_NULL),
	m_pUserCbf_SetIpAndNos(PNIO_NULL),
	m_pUserCbf_RemaRead(PNIO_NULL),
    m_CpIndex(0),
    m_set_ip_and_nos_running(PNIO_FALSE),
    m_status(pnd_interface_closed),
    m_pnd_pd_wait_event(0),
    m_nr_of_alarm_res(0),
    m_nr_of_rema_res(0)
{

    m_hInstanceHandle = (PNIO_UINT16)UserHandle;

    pnd_memset(g_pnd_log_addr.logAddrList, 0, sizeof(g_pnd_log_addr.logAddrList));
    g_pnd_log_addr.isInitialized = PNIO_FALSE;
}


/* destructor */
/*************************************************************************/
IIOInterface::~IIOInterface(void)
{
}

/*************************************************************************/
pnd_iob_interface_status IIOInterface::m_get_status(PNIO_VOID)
{
    return this->m_status;
}

/*************************************************************************/
PNIO_VOID  IIOInterface::m_set_status(pnd_iob_interface_status status)
{
    this->m_status = status;
}

/*************************************************************************/
PNIO_UINT32 IIOInterface::register_cbf(PNIO_CBE_TYPE CbeType, PNIO_CBF cbf)
{
    PNIO_UINT32 Ret = PNIO_OK;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>IIOInterface register_cbf");

    if (m_status != pnd_interface_opened)
        return PNIO_ERR_WRONG_HND;

    if(!cbf)
        return PNIO_ERR_PRM_CALLBACK;
	
    switch (CbeType) {

	case PNIO_CBE_IFC_SET_ADDR_CONF:
		if(m_pUserCbf_SetIpAndNos) {
			Ret = PNIO_ERR_ALREADY_DONE;
		} else {
			m_pUserCbf_SetIpAndNos = cbf;
			Ret = PNIO_OK;
		}
		break;

	case PNIO_CBE_REMA_READ_CONF:
		if(m_pUserCbf_RemaRead) {
			Ret = PNIO_ERR_ALREADY_DONE;
		} else {
			m_pUserCbf_RemaRead = cbf;
			Ret = PNIO_OK;
		}
		break;

	default:
        Ret = PNIO_ERR_PRM_TYPE;
        break;
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<< IIOInterface register_cbf");

    return Ret;
}

/*************************************************************************/
PNIO_VOID IIOInterface::de_register_all_cbf()
{
    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>de_register_cbf");

	m_pUserCbf_Alarm = 0;
	m_pUserCbf_SetIpAndNos = 0;
	m_pUserCbf_RemaRead = 0;
	m_pUserCbf_DR_Read = 0;
	
    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<de_register_cbf");
}

PND_RQB_PTR_TYPE IIOInterface::pnd_lower_rqb_alloc( LSA_OPCODE_TYPE opcode, LSA_HANDLE_TYPE handle, PNIO_VOID_PTR_TYPE pInst,PNIO_VOID(*func)(PNIO_VOID*_this, PNIO_VOID*_rqb) )
{
    // Allocate an user RQB for opcode
    PND_RQB_PTR_TYPE   pRQB;
    PND_RQB_LIST_TYPE *pRqbList = PNIO_NULL;

    if(opcode == PND_OPC_REC_READ)
    {
        pRqbList = &pnd_ifc_datarecord_read_rqb_list;
    }
    else
    {
        return PNIO_NULL;
    }

    pRQB = pnd_get_rqb_from_list(*pRqbList);

    if (!pRQB)
    {
        return PNIO_NULL;
    }

    pnd_memset( pRQB, 0,  sizeof(PND_RQB_TYPE) );

    LSA_RQB_SET_OPCODE( pRQB, opcode );
    LSA_RQB_SET_HANDLE( pRQB, handle );
    PND_RQB_SET_USER_INST_PTR(pRQB, pInst);
    PND_RQB_SET_USER_CBF_PTR(pRQB, func);

    return pRQB;
}

PNIO_VOID IIOInterface::pnd_lower_rqb_free( PNIO_VOID_PTR_TYPE rqb )
{
    PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)rqb;
    PND_RQB_LIST_TYPE *pRqbList = PNIO_NULL;

    if(LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_REC_READ)
    {
        pRqbList = &pnd_ifc_datarecord_read_rqb_list;
    }
    else
    {
        return;
    }
    
    pnd_put_rqb_to_list((PND_RQB_PTR_TYPE)pRQB, *pRqbList);
}

/*************************************************************************/
PNIO_UINT32 IIOInterface::get_handle(IIOInterface *ptr)
{
    if(ptr)
        return ptr->m_hInstanceHandle;
        else{
            PND_ASSERT(ptr);
        return 0xFFFFFFFF;
    }
}

/*************************************************************************/
PNIO_UINT32 IIOInterface::get_handle_by_index(PNIO_UINT32 CpIndex)
{
    if ((CpIndex >= 0) && (CpIndex < PND_MAX_INSTANCES))
    {
        if (g_pnd_user_core_cp_list[CpIndex].user_handle != PND_INVALID_HANDLE)
            return g_pnd_user_core_cp_list[CpIndex].user_handle;
        else
            return PND_INVALID_HANDLE;

    }
    else 
        return PND_INVALID_HANDLE;
}


/*************************************************************************/
IIOInterface* IIOInterface::get_instance(PNIO_UINT32 CpIndex)
{
    if ((CpIndex >= 0) && (CpIndex < PND_MAX_INSTANCES))
    {
        if (g_pnd_user_core_cp_list[CpIndex].interface_instance_pointer != PNIO_NULL)
			return g_pnd_user_core_cp_list[CpIndex].interface_instance_pointer;
        else
            return PNIO_NULL;
    }
    else 
        return PNIO_NULL;
}

/*************************************************************************/
PNIO_VOID IIOInterface::remove_instance(PNIO_UINT16 UserHandle)
{
	delete g_pnd_user_core_cp_list[UserHandle].interface_instance_pointer;
}

/*************************************************************************/
PNIO_UINT32 IIOInterface::interface_open(PNIO_UINT32 CpIndex,
    PNIO_CBF cbf_RecReadConf,
    PNIO_CBF cbf_AlarmInd,
    PNIO_UINT32 * pApplHandle)
{
    PNIO_UINT32 result = PNIO_OK;
	IIOInterface *pThis = PNIO_NULL; //IIOInterface

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>interface_open");

    if(!pApplHandle)
        return PNIO_ERR_PRM_HND;

    *pApplHandle = IIOInterface::get_handle_by_index(CpIndex);
    if (*pApplHandle == PND_INVALID_HANDLE)
        return PNIO_ERR_PRM_CP_ID;

    pThis = IIOInterface::get_instance(CpIndex);
    if(pThis == PNIO_NULL )
        return PNIO_ERR_CREATE_INSTANCE;

    if (pThis->m_status != pnd_interface_closed)
        return PNIO_ERR_WRONG_HND;

    pThis->m_CpIndex = CpIndex;
    pThis->m_pUserCbf_Alarm = cbf_AlarmInd;
    pThis->m_pUserCbf_DR_Read = cbf_RecReadConf;

    // Wait on callback, after request 
    /*******/

    PND_RQB_PTR_TYPE               pRQB;
    PND_IOB_CORE_OPEN_PTR_TYPE     pOpen;

    pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));

    if(!pRQB)
    {
        return PNIO_ERR_NO_RESOURCE;
    }

    if (pRQB == 0)
        PND_FATAL("mem alloc failed");

    pOpen = &pRQB->args.iob_open;

    pnd_memset(pOpen, 0, sizeof(PND_IOB_CORE_OPEN_TYPE));

    pOpen->pnd_handle.cp_nr = (PNIO_UINT16)CpIndex;

    if(g_pnd_log_addr.isInitialized != PNIO_TRUE)
    {
        pOpen->laddr_list_ptr = pnd_mem_alloc(sizeof(g_pnd_log_addr.logAddrList));
    }

    PND_ASSERT( pRQB );

    result = pnd_alloc_event( &pThis->m_pnd_pd_wait_event );
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    pnd_reset_event( pThis->m_pnd_pd_wait_event );

    LSA_RQB_SET_OPCODE( pRQB, PND_OPC_INTERFACE_OPEN );
    LSA_RQB_SET_HANDLE( pRQB, 0 );
    PND_RQB_SET_USER_INST_PTR(pRQB, pThis);
	PND_RQB_SET_USER_CBF_PTR(pRQB, IIOInterface::interface_open_done);
    
    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnd_iob_core_request, (PNIO_VOID_PTR_TYPE)pRQB);

    pnd_wait_event( pThis->m_pnd_pd_wait_event );

    result = pnd_free_event( pThis->m_pnd_pd_wait_event );
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    if(g_pnd_log_addr.isInitialized != PNIO_TRUE)
    {
        pnd_memcpy(g_pnd_log_addr.logAddrList, pOpen->laddr_list_ptr, sizeof(g_pnd_log_addr.logAddrList));
        g_pnd_log_addr.isInitialized = PNIO_TRUE;

        pnd_mem_free( pOpen->laddr_list_ptr);
    }

    pThis->m_nr_of_alarm_res = pOpen->nr_of_alarm_res;
    pThis->m_nr_of_rema_res = 1;  // 1 resource is provided to OHA

    result = pRQB->_response;
    if(result != PNIO_RET_OK)
        return PNIO_ERR_INTERNAL;
    else
        result = PNIO_OK;

    /******/
    result = pOpen->pnio_err;
    if (result != PNIO_OK)
        return result;

    pnd_mem_free(pRQB);

    result = pThis->alarm_provide(pThis, pThis->m_nr_of_alarm_res);
    if (result != PNIO_OK)
        return  PNIO_ERR_NO_RESOURCE;

    result = pThis->rema_provide(pThis, pThis->m_nr_of_rema_res);
    if (result != PNIO_OK)
        return  PNIO_ERR_NO_RESOURCE;
    
    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<interface_open");

    pThis->m_set_status( pnd_interface_opened );

    
    result = pnd_alloc_event(&pnd_ifc_datarecord_read_rqb_list.resource_free_event);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    pnd_set_event( pnd_ifc_datarecord_read_rqb_list.resource_free_event );

    return PNIO_OK;
}

/*************************************************************************/
PNIO_UINT32 IIOInterface::interface_close(PNIO_UINT32 ApplHandle)
{
    PNIO_UINT32 result = PNIO_OK;
	IIOInterface *pThis = PNIO_NULL;
    PND_RQB_PTR_TYPE                pRQB;
    PND_IOB_CORE_CLOSE_PTR_TYPE     pClose;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>interface_close");

	pThis = IIOInterface::get_instance(ApplHandle);

    PND_ASSERT( pThis != PNIO_NULL );

    if(!pThis) 
        return PNIO_ERR_WRONG_HND;

    if (pnd_interface_closed == pThis->m_get_status())
    {
        return PNIO_ERR_WRONG_HND;
    }

    pThis->de_register_all_cbf();

	pnd_wait_event(pnd_ifc_datarecord_read_rqb_list.resource_free_event);

    pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));
    
    if(!pRQB)
    {
        return PNIO_ERR_INTERNAL;
    }

    pClose = &pRQB->args.iob_close;

    pnd_memset(pClose, 0, sizeof(PND_IOB_CORE_CLOSE_TYPE));
    
    pClose->pnd_handle.cp_nr = (PNIO_UINT16)pThis->m_CpIndex;

    LSA_RQB_SET_OPCODE( pRQB, PND_OPC_INTERFACE_CLOSE );
    LSA_RQB_SET_HANDLE( pRQB, 0 );
    PND_RQB_SET_USER_INST_PTR(pRQB, pThis);
	PND_RQB_SET_USER_CBF_PTR(pRQB, IIOInterface::interface_close_done);

	result = pnd_alloc_event( &pThis->m_pnd_pd_wait_event );
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }
    
    pnd_reset_event( pThis->m_pnd_pd_wait_event );

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnd_iob_core_request, (PNIO_VOID_PTR_TYPE)pRQB);

    pnd_wait_event( pThis->m_pnd_pd_wait_event );

    result = pClose->pnio_err;

    pnd_mem_free( pRQB);

    result = pnd_free_event( pThis->m_pnd_pd_wait_event );
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }


    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<interface_close");

    pThis->m_set_status( pnd_interface_closed );

    result = pnd_free_event(pnd_ifc_datarecord_read_rqb_list.resource_free_event);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    return PNIO_OK;
}

/*************************************************************************/
PNIO_VOID IIOInterface::set_ip_and_nos_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
	PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;
	IIOInterface *pThis = (IIOInterface *)pInst;

	PNIO_CBE_PRM cbf_prm;
	cbf_prm.Handle  = pThis->get_handle(pThis);
	cbf_prm.CbeType = PNIO_CBE_IFC_SET_ADDR_CONF;

	if(pThis->m_pUserCbf_SetIpAndNos) 
	{
		cbf_prm.SetAddrConf.pStationName       = rqb_ptr->args.set_ip_and_nos.NoS.Nos;
		cbf_prm.SetAddrConf.StationNameLen     = rqb_ptr->args.set_ip_and_nos.NoS.Length;
		pnd_memcpy(cbf_prm.SetAddrConf.LocalIPAddress, rqb_ptr->args.set_ip_and_nos.IpV4.IpAddress, 4);
		pnd_memcpy(cbf_prm.SetAddrConf.LocalSubnetMask, rqb_ptr->args.set_ip_and_nos.IpV4.NetMask, 4);
		pnd_memcpy(cbf_prm.SetAddrConf.DefaultRouterAddr, rqb_ptr->args.set_ip_and_nos.IpV4.Gateway, 4);
		cbf_prm.SetAddrConf.Options            = rqb_ptr->args.set_ip_and_nos.Options;
		cbf_prm.SetAddrConf.Err                = rqb_ptr->args.set_ip_and_nos.pnio_err;
		cbf_prm.SetAddrConf.Mode               = rqb_ptr->args.set_ip_and_nos.mode;
		
		pThis->m_pUserCbf_SetIpAndNos(&cbf_prm);
	} 
	else 
	{
		PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "No CBF for Set IP and NoS user callback() defined. ApplHandle= %d Mode= %d", cbf_prm.Handle, pThis->m_pUserCbf_SetIpAndNos);
	}

    pThis->m_set_ip_and_nos_running = PNIO_FALSE;

	pnd_mem_free(pRqb);
}


/*************************************************************************/
PNIO_UINT32 IIOInterface::set_ip_and_nos(PNIO_UINT32 handle,
											PNIO_SET_IP_NOS_MODE_TYPE Mode, 
											PNIO_IPv4 IpV4, 
											PNIO_NOS NoS)
{
	PND_RQB_PTR_TYPE         pRQB = PNIO_NULL;
	PND_SET_IP_AND_NOS_TYPE* pSetIpAndNos;

    if (this->m_status != pnd_interface_opened)
        return PNIO_ERR_WRONG_HND;

    if( Mode != PNIO_SET_IP_MODE &&
        Mode != PNIO_SET_NOS_MODE &&
        Mode != (PNIO_SET_NOS_MODE|PNIO_SET_IP_MODE))
    {
        return PNIO_ERR_MODE_VALUE;
    }

    if( handle != get_handle(this) )
        return PNIO_ERR_PRM_HND;

    if( m_set_ip_and_nos_running == PNIO_TRUE )
        return PNIO_ERR_NOT_REENTERABLE;

    if( ((Mode & PNIO_SET_NOS_MODE) == PNIO_SET_NOS_MODE) && (NoS.Length > OHA_MAX_STATION_NAME_LEN) )
    {
        return PNIO_ERR_PRM_NOS_LEN;
    }
    
    m_set_ip_and_nos_running = PNIO_TRUE;

    pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));

	if(!pRQB)
    {
        return PNIO_ERR_INTERNAL;
    }

    LSA_RQB_SET_OPCODE( pRQB, PND_OPC_SET_ADDRESS );
    LSA_RQB_SET_HANDLE( pRQB, (PNIO_UINT8) handle );
    PND_RQB_SET_USER_INST_PTR(pRQB, this);
    PND_RQB_SET_USER_CBF_PTR(pRQB, IIOInterface::set_ip_and_nos_done);

	pSetIpAndNos = &pRQB->args.set_ip_and_nos;

	pSetIpAndNos->pnd_handle.cp_nr = (PNIO_UINT16)m_CpIndex;

	pSetIpAndNos->mode = Mode;
	pnd_memcpy( &pSetIpAndNos->IpV4, &IpV4, sizeof(PNIO_IPv4) );
	pnd_memcpy( &pSetIpAndNos->NoS,  &NoS,  sizeof(PNIO_NOS)  );

	pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);

	return PNIO_OK;
}

/*************************************************************************/
PNIO_VOID IIOInterface::record_read_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;
    IIOInterface *pThis = (IIOInterface *)pInst;

    PNIO_ADDR Addr;
    PNIO_CBE_PRM cbf_prm;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>read_write_record_done");

    switch (LSA_RQB_GET_OPCODE(rqb_ptr))
    {
        case PND_OPC_REC_READ:
            {
                PND_RECORD_READ_WRITE_PTR_TYPE pRecRead = &rqb_ptr->args.rec_read;

                cbf_prm.CbeType = PNIO_CBE_IFC_REC_READ_CONF;
                cbf_prm.Handle  = get_handle(pThis);

                Addr.AddrType   = (PNIO_ADDR_TYPE)pRecRead->Addr.AddrType;
                Addr.IODataType = (PNIO_IO_TYPE)pRecRead->Addr.IODataType;
                Addr.u.Addr     = pRecRead->Addr.u.Addr;
                cbf_prm.RecReadConf.pAddr = &Addr;

                cbf_prm.RecReadConf.RecordIndex = pRecRead->RecordIndex;
            
                cbf_prm.RecReadConf.ReqRef  = pRecRead->UserRef;
                cbf_prm.RecReadConf.pBuffer = (PNIO_UINT8 *)pRecRead->pBuffer;
                cbf_prm.RecReadConf.Length  = pRecRead->Length;

                cbf_prm.RecReadConf.Err.ErrCode   = pRecRead->err.ErrCode;
                cbf_prm.RecReadConf.Err.ErrDecode = pRecRead->err.ErrDecode;
                cbf_prm.RecReadConf.Err.ErrCode1  = pRecRead->err.ErrCode1;
                cbf_prm.RecReadConf.Err.ErrCode2  = pRecRead->err.ErrCode2;
                cbf_prm.RecReadConf.Err.AddValue1 = (PNIO_UINT16) pRecRead->pnio_err;
                cbf_prm.RecReadConf.Err.AddValue2 = (PNIO_UINT16) pRecRead->pnio_err;

                if (pThis->m_pUserCbf_DR_Read)
                {
                  pThis->m_pUserCbf_DR_Read(&cbf_prm);
                }
                else
                {
                    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "NO CBF for Data Record Read/Write defined. ");
                }

                pnd_mem_free(pRecRead->pBuffer); // free record read Buffer allocated by record_read
                break;
            }
        default:
        PND_FATAL("wrong OPCODE");;
    }

    pThis->pnd_lower_rqb_free(pRqb);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<read_write_record_done");
}

/*************************************************************************/
PNIO_UINT32 IIOInterface::check_log_adress(PNIO_ADDR * pAddr)
{
    if(pAddr == PNIO_NULL)
    {
        return PNIO_ERR_PRM_ADD;
    }

    PNIO_UINT32 laddr = pAddr->u.Addr;

    if ((laddr > PND_LADDR_MAX) || (laddr <= 0))
    {
        return PNIO_ERR_PRM_ADD;
    }
 
   if ((g_pnd_log_addr.logAddrList[laddr] > IOCSUBSLOT) || (g_pnd_log_addr.logAddrList[laddr] <= UNKNOWN)) 
    {
        return PNIO_ERR_PRM_ADD;
    }
    else
    {
        return PNIO_OK;
    }
}

/*************************************************************************/
PNIO_UINT32 IIOInterface::record_read(PNIO_ADDR * pAddr, PNIO_REF ReqRef, PNIO_UINT32 RecordIndex, PNIO_UINT32 Length)
{
   PNIO_UINT32 result = PNIO_OK;
    PND_RQB_PTR_TYPE                pRQB = PNIO_NULL;
    PND_RECORD_READ_WRITE_PTR_TYPE  pRecRead = PNIO_NULL;
    PNIO_UINT8 * pRecBuffer = PNIO_NULL;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>record_read");

    if (this->m_status != pnd_interface_opened)
        return PNIO_ERR_WRONG_HND;

    if(!pAddr ||
        pAddr->AddrType != PNIO_ADDR_LOG ||
        (pAddr->IODataType != PNIO_IO_IN && pAddr->IODataType != PNIO_IO_OUT))
        return PNIO_ERR_PRM_ADD;

        // sanity addres bound
    if(pAddr->u.Addr > 0x7FFF)
        return PNIO_ERR_PRM_ADD;

    result = check_log_adress(pAddr);
    if (result != PNIO_OK)
    {
        return result;
    }

	// check if interface address
	if (g_pnd_log_addr.logAddrList[pAddr->u.Addr] != PDSUBSLOTINTERFACE &&  
        g_pnd_log_addr.logAddrList[pAddr->u.Addr] != PDSUBSLOTPORT)
    {
        return PNIO_ERR_PRM_ADD;
    }

    if(Length == 0 || Length > PNIOI_DREC_MAX_SIZE)
        return PNIO_ERR_VALUE_LEN;

    if(RecordIndex > 0xFFFF)
        return PNIO_ERR_PRM_REC_INDEX;

    pRQB = pnd_lower_rqb_alloc(PND_OPC_REC_READ, 0, this, IIOInterface::record_read_done);

    if(!pRQB)
    {
	    return PNIO_ERR_NO_RESOURCE;
    }

    pRecBuffer = (PNIO_UINT8 *)pnd_mem_alloc(Length);
    if (pRecBuffer == 0)
            PND_FATAL("mem alloc failed");

    pnd_memset(pRecBuffer,0,Length);

    pRecRead = &pRQB->args.rec_read;

    pnd_memset(pRecRead, 0, sizeof(PND_RECORD_READ_WRITE_TYPE));

    pRecRead->Addr.AddrType   = pAddr->AddrType;
    pRecRead->Addr.IODataType = pAddr->IODataType;
    pRecRead->Addr.u.Addr     = pAddr->u.Addr;

    pRecRead->UserRef = ReqRef;

    pRecRead->RecordIndex = RecordIndex;
    pRecRead->Length      = Length;
    pRecRead->pBuffer     = pRecBuffer;

    pRecRead->pnd_handle.cp_nr = this->m_hInstanceHandle;

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<record_read");

    return result;
}

/*************************************************************************************************/
PNIO_VOID IIOInterface::alarm_indication(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
	IIOInterface *pThis = (IIOInterface *)pInst;
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;
    PND_ALARM_PTR_TYPE     pAlarm;

    pAlarm = &rqb_ptr->args.iob_alarm;

    /* fill in alarm data */

    if (LSA_RQB_GET_RESPONSE(rqb_ptr) == PNIO_OK_CANCEL)
     {
         // pThis->pnd_lower_rqb_free(rqb_ptr);
         pnd_mem_free(rqb_ptr);
     }
     else
     {

        if (pThis)
        {
            PNIO_ADDR Addr;
            PNIO_CBE_PRM cbf_prm;
            PNIO_CTRL_ALARM_DATA AlarmData;

            pnd_memset(&AlarmData,0,sizeof(AlarmData));

            cbf_prm.Handle = pThis->get_handle(pThis); // = CPIndex
            cbf_prm.CbeType = PNIO_CBE_IFC_ALARM_IND;

            cbf_prm.AlarmInd.IndRef = pAlarm->ind_ref;

            switch (pAlarm->alarm.AlarmType)
            {
                case PNIO_ALARM_REDUNDANCY:
                case PNIO_ALARM_PORT_DATA_CHANGED:
                case PNIO_ALARM_SYNC_DATA_CHANGED:
                case PNIO_ALARM_NETWORK_COMPONENT_PROBLEM:
                case PNIO_ALARM_TIME_DATA_CHANGED:
                {
                        AlarmData.AlarmType     = pAlarm->alarm.AlarmType;
                        AlarmData.AlarmPriority = pAlarm->alarm.AlarmPriority;
                        AlarmData.DeviceNum     = pAlarm->alarm.DeviceNum;
                        AlarmData.SlotNum       = pAlarm->alarm.SlotNum;
                        AlarmData.SubslotNum    = pAlarm->alarm.SubslotNum;
                        AlarmData.CpuAlarmPriority  = pAlarm->alarm.CpuAlarmPriority;
                        AlarmData.PnioCompatModtype = pAlarm->alarm.PnioCompatModtype;
                        AlarmData.AlarmTinfo.CompatDevGeoaddr = pAlarm->alarm.AlarmTinfo.CompatDevGeoaddr;
                        AlarmData.AlarmTinfo.ProfileType      = pAlarm->alarm.AlarmTinfo.ProfileType;
                        AlarmData.AlarmTinfo.AinfoType        = pAlarm->alarm.AlarmTinfo.AinfoType;
                        AlarmData.AlarmTinfo.ControllerFlags  = pAlarm->alarm.AlarmTinfo.ControllerFlags;
                        AlarmData.AlarmTinfo.DeviceFlag       = pAlarm->alarm.AlarmTinfo.DeviceFlag;
                        AlarmData.AlarmTinfo.PnioVendorIdent  = pAlarm->alarm.AlarmTinfo.PnioVendorIdent;
                        AlarmData.AlarmTinfo.PnioDevIdent     = pAlarm->alarm.AlarmTinfo.PnioDevIdent;
                        AlarmData.AlarmTinfo.PnioDevInstance  = pAlarm->alarm.AlarmTinfo.PnioDevInstance;

                        pnd_memcpy(AlarmData.DiagDs, pAlarm->alarm.DiagDs, sizeof(AlarmData.DiagDs));
                        pnd_memcpy(AlarmData.PrAlarmInfo, pAlarm->alarm.PrAlarmInfo, sizeof(AlarmData.PrAlarmInfo));

                        AlarmData.AlarmAinfo.BlockType    = pAlarm->alarm.AlarmAinfo.BlockType;
                        AlarmData.AlarmAinfo.BlockVersion = pAlarm->alarm.AlarmAinfo.BlockVersion;
                        AlarmData.AlarmAinfo.Api          = pAlarm->alarm.AlarmAinfo.Api;
                        AlarmData.AlarmAinfo.AlarmSpecifier   = pAlarm->alarm.AlarmAinfo.AlarmSpecifier;
                        AlarmData.AlarmAinfo.ModIdent         = pAlarm->alarm.AlarmAinfo.ModIdent;
                        AlarmData.AlarmAinfo.SubIdent         = pAlarm->alarm.AlarmAinfo.SubIdent;
                        AlarmData.AlarmAinfo.UserStrucIdent   = pAlarm->alarm.AlarmAinfo.UserStrucIdent;
                        AlarmData.AlarmAinfo.UserAlarmDataLen = pAlarm->alarm.AlarmAinfo.UserAlarmDataLen;

#ifdef PNIO_ALARM_OLD_STRUC
                        pnd_memcpy(AlarmData.AlarmAinfo.UserAlarmData,  pAlarm->alarm.AlarmAinfo.UserAlarmData,
                            AlarmData.AlarmAinfo.UserAlarmDataLen);
#else
                        if (pAlarm->alarm.AlarmAinfo.UAData.UserAlarmData)
                        {
                            pnd_memcpy(AlarmData.AlarmAinfo.UAData.UserAlarmData, pAlarm->alarm.AlarmAinfo.UAData.UserAlarmData,
                                AlarmData.AlarmAinfo.UserAlarmDataLen);
                        }
#endif
                        Addr.AddrType   = pAlarm->addr.AddrType;
                        Addr.IODataType = pAlarm->addr.IODataType;
                        Addr.u.Addr     = pAlarm->addr.u.Addr;
                        cbf_prm.AlarmInd.pAddr = &Addr;
                        break;
                    }
                default:
                    {
                        // PND_FATAL("unknowen alarm type");
                        break;
                    }
            } // end switch

            // alarm callback 

            cbf_prm.AlarmInd.pAlarmData = &AlarmData;

            if (pThis->m_pUserCbf_Alarm)
            {
                pThis->m_pUserCbf_Alarm(&cbf_prm);
            }
            else
            {
                PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "No CBF_ALARM_IND user callback() defined");
            }

            // reprovide alarm resource
            alarm_response(pThis, rqb_ptr);

        } // end if 
        else
        {
            PND_FATAL("wrong  instance ");
        }

    } // end else
}

/*************************************************************************/
PNIO_VOID IIOInterface::alarm_response(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  rqb_ptr)
{
    PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)rqb_ptr;
    PND_ASSERT( pRQB );

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>alarm_response");

	IIOInterface *pThis = (IIOInterface *)pInst;

    LSA_RQB_SET_OPCODE( pRQB, PND_OPC_ALARM_RESPONSE );
    LSA_RQB_SET_HANDLE( pRQB, 0 );
    PND_RQB_SET_USER_INST_PTR(pRQB, pThis);
    PND_RQB_SET_USER_CBF_PTR(pRQB,IIOInterface::alarm_indication);

	pRQB->args.iob_alarm.ind_ref = PNIO_REF_IFC;

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<alarm_response");
}

/*************************************************************************/
PNIO_UINT32 IIOInterface::alarm_provide(PNIO_VOID_PTR_TYPE  pInst, PNIO_UINT16 nrOfAlarmResource)
{
    IIOInterface *pThis = (IIOInterface *)pInst;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>alarm_provide");

    PND_RQB_PTR_TYPE       pRQB = 0;
    PND_ALARM_PTR_TYPE     pAlarm = 0;
    PNIO_UINT16            i = 0;

    for (i = 0; i< nrOfAlarmResource; i++)
    {
        pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));
    
        if(!pRQB)
        {
            return PNIO_ERR_INTERNAL;
        }

        pAlarm = &pRQB->args.iob_alarm;

        pnd_memset(pAlarm,0,sizeof(PND_ALARM_TYPE));

        pAlarm->pnd_handle.cp_nr = pThis->m_hInstanceHandle;
		pAlarm->ind_ref = PNIO_REF_IFC;

        LSA_RQB_SET_OPCODE( pRQB, PND_OPC_ALARM_PROVIDE );
        LSA_RQB_SET_HANDLE( pRQB, 0 );
        PND_RQB_SET_USER_INST_PTR(pRQB, pThis);
        PND_RQB_SET_USER_CBF_PTR(pRQB, IIOInterface::alarm_indication);

        pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<alarm_provide");

    return PNIO_OK;
}

/*************************************************************************/
PNIO_VOID IIOInterface::ifc_rema_done(PNIO_VOID_PTR_TYPE  pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
	PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;
	IIOInterface *pThis = (IIOInterface *)pInst;

	PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>ifc_rema_done");

	if (LSA_RQB_GET_RESPONSE(rqb_ptr) == PNIO_OK_CANCEL)
     {
         pnd_mem_free(rqb_ptr);
     }
	else
	{

		PNIO_CBE_PRM cbf_prm;
		cbf_prm.Handle  = pThis->get_handle(pThis);
		cbf_prm.CbeType = PNIO_CBE_REMA_READ_CONF;

		if(pThis->m_pUserCbf_RemaRead) 
		{
			cbf_prm.RemaReadConf.Err         = rqb_ptr->args.rema_data.pnio_err;
			cbf_prm.RemaReadConf.RemaXMLBuffer = rqb_ptr->args.rema_data.remaXMLbuffer;
			cbf_prm.RemaReadConf.RemaXMLBufferLength = rqb_ptr->args.rema_data.remaXMLbufferLength;

			pThis->m_pUserCbf_RemaRead(&cbf_prm);
		} 
		else 
		{
			PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "No CBF for Rema Read user callback() defined. ApplHandle = %d Cbf = %d", cbf_prm.Handle, pThis->m_pUserCbf_RemaRead);
		}

		pnd_mem_free(rqb_ptr->args.rema_data.data);
		pnd_mem_free(rqb_ptr->args.rema_data.remaXMLbuffer);

		PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<ifc_rema_done");
		
		// reprovide rema resource
        rema_response(pThis, rqb_ptr);
	}

}

/*************************************************************************/
PNIO_UINT32 IIOInterface::rema_provide(PNIO_VOID_PTR_TYPE pInst, PNIO_UINT16 nrOfRemaResource)
{
	IIOInterface *pThis = (IIOInterface *)pInst;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>rema_provide");

    PND_RQB_PTR_TYPE       pRQB = 0;
    PND_REMA_DATA_TYPE*    pRema;
    PNIO_UINT16            i = 0;

    for (i = 0;i< nrOfRemaResource; i++)
    {
        pRQB = (PND_RQB_PTR_TYPE) pnd_mem_alloc( sizeof(*pRQB));
    
        if(!pRQB)
        {
            return PNIO_ERR_INTERNAL;
        }

        pRema = &pRQB->args.rema_data;

        pnd_memset(pRema,0,sizeof(PND_REMA_DATA_TYPE));

        pRema->pnd_handle.cp_nr = pThis->m_hInstanceHandle;

        LSA_RQB_SET_OPCODE( pRQB, PND_OPC_REMA_PROVIDE );
        LSA_RQB_SET_HANDLE( pRQB, 0 );
        PND_RQB_SET_USER_INST_PTR(pRQB, pThis);
        PND_RQB_SET_USER_CBF_PTR(pRQB, IIOInterface::ifc_rema_done);

        pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<rema_provide");

    return PNIO_OK;
}

/*************************************************************************/
PNIO_VOID IIOInterface::rema_response(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  rqb_ptr)
{
    PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)rqb_ptr;
    PND_ASSERT( pRQB );

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>rema_response");

    IIOInterface *pThis = (IIOInterface *)pInst;

    LSA_RQB_SET_OPCODE( pRQB, PND_OPC_REMA_RESPONSE );
    LSA_RQB_SET_HANDLE( pRQB, 0 );
    PND_RQB_SET_USER_INST_PTR(pRQB, pThis);
    PND_RQB_SET_USER_CBF_PTR(pRQB, IIOInterface::ifc_rema_done);

    pnd_request_start(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<rema_response");
}

/*************************************************************************/
PNIO_VOID IIOInterface::interface_open_done(PNIO_VOID_PTR_TYPE pInst,PNIO_VOID_PTR_TYPE  pRqb)
{
    IIOInterface *pThis = (IIOInterface *)pInst;

    LSA_UNUSED_ARG (pRqb);

    pnd_set_event( pThis->m_pnd_pd_wait_event );
}

/*************************************************************************/
PNIO_VOID IIOInterface::interface_close_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    IIOInterface *pThis = (IIOInterface *)pInst;

    LSA_UNUSED_ARG (pRqb);

    pnd_set_event( pThis->m_pnd_pd_wait_event );
}



/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/