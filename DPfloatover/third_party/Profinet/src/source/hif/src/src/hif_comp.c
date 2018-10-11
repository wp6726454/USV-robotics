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
/*  C o m p o n e n t     &C: HIF (Host Interface)                      :C&  */
/*                                                                           */
/*  F i l e               &F: hif_comp.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements functions accessing component specific RQB arguments          */
/*                                                                           */
/*****************************************************************************/
#define HIF_MODULE_ID		7
#define LTRC_ACT_MODUL_ID	7

#include "hif_int.h"
/// @cond PREVENT_DOXYGEN
HIF_FILE_SYSTEM_EXTENSION(HIF_MODULE_ID)
/// @endcond

/**
 *\brief Union to collect all pointers to different RQBs
 */
typedef union {
	#if (HIF_CFG_USE_ACP == 1)
	ACP_RQB_TYPE*     pAcpRQB;
	#endif
	#if (HIF_CFG_USE_CLRPC == 1)
	CLRPC_RQB_TYPE*   pClrpcRQB;
	#endif
	#if (HIF_CFG_USE_CM == 1)
	CM_RQB_TYPE*      pCmRQB;
	#endif
	#if (HIF_CFG_USE_DCP == 1)
	DCP_RQB_TYPE*     pDcpRQB;
	#endif
	#if (HIF_CFG_USE_EDD == 1)
	EDD_RQB_TYPE*     pEddRQB;
	#endif
	#if (HIF_CFG_USE_GSY == 1)
	GSY_RQB_TYPE*     pGsyRQB;
	#endif
	HIF_RQB_PTR_TYPE  pHifRQB;
	#if (HIF_CFG_USE_IOH == 1)
	IOH_RQB_TYPE*     pIohRQB;
	#endif
	#if (HIF_CFG_USE_LLDP == 1)
	LLDP_RQB_TYPE*    pLldpRQB;
	#endif
	#if (HIF_CFG_USE_MRP == 1)
	MRP_RQB_TYPE*     pMrpRQB;
	#endif
	#if (HIF_CFG_USE_NARE == 1)
	NARE_RQB_TYPE*    pNareRQB;
	#endif
	#if (HIF_CFG_USE_OHA == 1)
	OHA_RQB_TYPE*     pOhaRQB;
	#endif
	#if (HIF_CFG_USE_PSI == 1)
	PSI_RQB_TYPE*     pPsiRQB;
	#endif
	#if (HIF_CFG_USE_SOCK == 1)
	SOCK_RQB_TYPE*    pSockRQB;
	#endif
	#if (HIF_CFG_USE_SNMPX)
	SNMPX_RQB_TYPE*   pSnmpxRQB;
	#endif
} HIF_UNIVERSAL_RQB_TYPE;

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
/**
 * 
 * @param hSysDev
 * @param pHifPipeHandle
 * @param hShmPool
 * @param pRqbSrc
 * @param ppRqbDest
 * @param pSerRqbLen
 * @return
 */
LSA_UINT16 hif_serialize_admin_request_upper( HIF_SYS_HANDLE hSysDev, 
                                              LSA_VOID* pHifPipeHandle, 
                                              LSA_INT hShmPool,
                                              LSA_VOID* pRqbSrc,
                                              LSA_VOID** ppRqbDest,
                                              LSA_UINT32* pSerRqbLen)
{
	LSA_COMP_ID_TYPE comp_id;
	HIF_UNIVERSAL_RQB_TYPE uniRQB;
	LSA_UINT16 res;


    HIF_PROGRAM_TRACE_06(0, LSA_TRACE_LEVEL_CHAT, "hif_serialize_admin_request_upper(+) - hSysDev(%d) pHifPipeHandle(0x%x) hShmPool(0x%x) pRqbSrc(0x%x) ppRqbDest(0x%x) pSerRqbLen(0x%x)", hSysDev, pHifPipeHandle, hShmPool, pRqbSrc, ppRqbDest, pSerRqbLen);

	uniRQB.pHifRQB = (HIF_RQB_PTR_TYPE)pRqbSrc;

	comp_id = HIF_RQB_GET_COMP_ID(uniRQB.pHifRQB);

	HIF_ASSERT(g_pHifData->CompStore.CompSys[comp_id].bUsed == LSA_TRUE);
	HIF_ASSERT(g_pHifData->CompStore.CompSys[comp_id].Comp.SerializeUpper != LSA_NULL);
	res = g_pHifData->CompStore.CompSys[comp_id].Comp.SerializeUpper(hSysDev, pHifPipeHandle, hShmPool, pRqbSrc, ppRqbDest, pSerRqbLen);

	return res;
}
#endif

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
/**
 * 
 * @param hSysDev
 * @param pHifPipeHandle
 * @param hShmPool
 * @param pRqbSrc
 * @param ppRqbDest
 * @param pSerRqbLen
 * @return
 */
LSA_UINT16 hif_serialize_admin_request_lower( HIF_SYS_HANDLE hSysDev, 
                                              LSA_VOID* pHifPipeHandle, 
                                              LSA_INT hShmPool, 
                                              LSA_VOID* pRqbSrc, 
                                              LSA_VOID** ppRqbDest, 
                                              LSA_UINT32* pSerRqbLen)
{
	LSA_COMP_ID_TYPE comp_id;
	HIF_UNIVERSAL_RQB_TYPE uniRQB;
	LSA_UINT16 res;

    HIF_PROGRAM_TRACE_06(0, LSA_TRACE_LEVEL_CHAT, "hif_serialize_admin_request_lower(+) - hSysDev(%d) pHifPipeHandle(0x%x) hShmPool(0x%x) pRqbSrc(0x%x) ppRqbDest(0x%x) pSerRqbLen(0x%x)", hSysDev, pHifPipeHandle, hShmPool, pRqbSrc, ppRqbDest, pSerRqbLen);

	uniRQB.pHifRQB = (HIF_RQB_PTR_TYPE)pRqbSrc;

	comp_id = HIF_RQB_GET_COMP_ID(uniRQB.pHifRQB);

	HIF_ASSERT(g_pHifData->CompStore.CompSys[comp_id].bUsed == LSA_TRUE);
	HIF_ASSERT(g_pHifData->CompStore.CompSys[comp_id].Comp.SerializeLower != LSA_NULL);
	res = g_pHifData->CompStore.CompSys[comp_id].Comp.SerializeLower(hSysDev, pHifPipeHandle, hShmPool, pRqbSrc, ppRqbDest, pSerRqbLen);

	return res;
}
#endif

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
/**
 * 
 * @param hSysDev
 * @param pHifPipeHandle
 * @param pRqbSrc
 * @param ppRqbDest
 * @return
 */
LSA_UINT16 hif_deserialize_admin_request_upper( HIF_SYS_HANDLE hSysDev, 
                                                LSA_VOID* pHifPipeHandle, 
                                                LSA_VOID* pRqbSrc,
                                                LSA_VOID** ppRqbDest)
{
	LSA_UINT16 res;
	LSA_COMP_ID_TYPE comp_id;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_deserialize_admin_request_upper(+) - hSysDev(%d) pHifPipeHandle(0x%x) pRqbSrc(0x%x) ppRqbDest(0x%x)", hSysDev, pHifPipeHandle, pRqbSrc, ppRqbDest);

	HIF_DESER_GET_COMP_ID_FROM_SHM(pRqbSrc, comp_id);	

	HIF_ASSERT(g_pHifData->CompStore.CompSys[comp_id].bUsed == LSA_TRUE);
	HIF_ASSERT(g_pHifData->CompStore.CompSys[comp_id].Comp.DeserializeUpper != LSA_NULL);
	res = g_pHifData->CompStore.CompSys[comp_id].Comp.DeserializeUpper(hSysDev, pHifPipeHandle, pRqbSrc, ppRqbDest);

	return res;
}
#endif

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
/**
 * 
 * @param hSysDev
 * @param pHifPipeHandle
 * @param pRqbSrc
 * @param ppRqbDest
 * @return
 */
LSA_UINT16 hif_deserialize_admin_request_lower( HIF_SYS_HANDLE hSysDev, 
                                                LSA_VOID* pHifPipeHandle, 
                                                LSA_VOID* pRqbSrc, 
                                                LSA_VOID** ppRqbDest)
{
	LSA_UINT16 res;
	LSA_COMP_ID_TYPE comp_id;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_deserialize_admin_request_lower(+) - hSysDev(%d) pHifPipeHandle(0x%x) pRqbSrc(0x%x) ppRqbDest(0x%x)", hSysDev, pHifPipeHandle, pRqbSrc, ppRqbDest);

	HIF_DESER_GET_COMP_ID_FROM_SHM(pRqbSrc, comp_id);	

	HIF_ASSERT(g_pHifData->CompStore.CompSys[comp_id].bUsed == LSA_TRUE);
	HIF_ASSERT(g_pHifData->CompStore.CompSys[comp_id].Comp.DeserializeLower != LSA_NULL);
	res = g_pHifData->CompStore.CompSys[comp_id].Comp.DeserializeLower(hSysDev, pHifPipeHandle, pRqbSrc, ppRqbDest);

	return res;
}
#endif

#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
/**
 * 
 * @param pPipe
 * @return
 */
LSA_VOID hif_update_serialization_funcs(HIF_PIPE_PTR_TYPE pPipe)
{

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_update_serialization_funcs(+) - pPipe(0x%x)", pPipe);

	if(pPipe->pipe_id == HIF_GLOB_ADMIN) /* PSI System request use HIF Admin Pipe => adapt comp_id */
	{		
			if(hif_is_upper_device(pPipe->pInstance))
			{
				pPipe->func.Serialize   = hif_serialize_admin_request_upper;
				pPipe->func.Deserialize = hif_deserialize_admin_request_upper;
			}
			else
			{
				pPipe->func.Serialize   = hif_serialize_admin_request_lower;
				pPipe->func.Deserialize = hif_deserialize_admin_request_lower;
			}
	}
	else
	{
		HIF_ASSERT(pPipe->func.pCompFuncs != LSA_NULL);

		if(hif_is_upper_pipe(pPipe))
		{
			pPipe->func.Serialize   = pPipe->func.pCompFuncs->SerializeUpper;
			pPipe->func.Deserialize = pPipe->func.pCompFuncs->DeserializeUpper;
		}
		else
		{
			pPipe->func.Serialize   = pPipe->func.pCompFuncs->SerializeLower;
			pPipe->func.Deserialize = pPipe->func.pCompFuncs->DeserializeLower;
		}
	}
}
#endif

/**
 * 
 * @param phH
 * @param pRQB
 * @return
 */
LSA_VOID hif_get_sys_request_hH_hif_dummy(
	HIF_HANDLE* phH,
	HIF_RQB_PTR_TYPE pRQB)
{
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_get_sys_request_hH_hif_dummy(+) - phH(0x%x), pRQB(0x%x)", phH, pRQB);

	LSA_UNUSED_ARG(phH);
	LSA_UNUSED_ARG(pRQB);

	HIF_FATAL(0);
}

/**
 * 
 * @param hSysDev
 * @param pRQB
 * @param ppCbf
 * @return
 */
LSA_VOID hif_get_sys_request_cbf_hif_dummy(
	HIF_SYS_HANDLE hSysDev,
	HIF_RQB_PTR_TYPE pRQB,
	HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE *ppCbf)
{
    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_get_sys_request_cbf_hif_dummy(+) -  hSysDev(0x%x), pRQB(0x%x), ppCbf(0x%x)", hSysDev, pRQB, ppCbf);

	LSA_UNUSED_ARG(hSysDev);
	LSA_UNUSED_ARG(pRQB);
	LSA_UNUSED_ARG(ppCbf);

	HIF_FATAL(0);
}

/**
 * 
 * @param hSysDev
 * @param pRQB
 * @param pCbf
 * @return
 */
LSA_VOID hif_replace_sys_request_cbf_hif_dummy(
	HIF_SYS_HANDLE hSysDev,
	HIF_RQB_PTR_TYPE pRQB,
	HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE pCbf)
{
    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_sys_request_cbf_hif_dummy(+) -  hSysDev(0x%x), pRQB(0x%x), pCbf(0x%x)", hSysDev, pRQB, pCbf);
    
	LSA_UNUSED_ARG(hSysDev);
	LSA_UNUSED_ARG(pRQB);
	LSA_UNUSED_ARG(pCbf);

	HIF_FATAL(0);
}

#if (HIF_CFG_USE_PSI == 1)
/**
 * 
 * @param phH
 * @param pRQB
 */
LSA_VOID hif_get_sys_request_hH_psi(
	HIF_HANDLE* phH,
	HIF_RQB_PTR_TYPE pRQB)
{
	HIF_UNIVERSAL_RQB_TYPE pUniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_get_sys_request_hH_psi(+) -  phH(0x%x), pRQB(0x%x)", phH, pRQB);

	HIF_ASSERT(HIF_RQB_GET_COMP_ID(pRQB) == LSA_COMP_ID_PSI);

	pUniRQB.pHifRQB = pRQB;

	switch(PSI_RQB_GET_OPCODE(pUniRQB.pPsiRQB))
	{
		case PSI_OPC_LD_OPEN_DEVICE:
			*phH = pUniRQB.pPsiRQB->args.ld_open.hH;
			break;
		case PSI_OPC_HD_OPEN_DEVICE:
			*phH = pUniRQB.pPsiRQB->args.hd_open.hH;	
			break;
		case PSI_OPC_LD_CLOSE_DEVICE:
			*phH = pUniRQB.pPsiRQB->args.ld_close.hH;
			break;
		case PSI_OPC_HD_CLOSE_DEVICE:
			*phH = pUniRQB.pPsiRQB->args.hd_close.hH;
			break;
		default:
			HIF_FATAL(0);
			break;
	}
}

/**
 * 
 * @param hSysDev
 * @param pRQB
 * @param *ppCbf
 * @return
 */
LSA_VOID hif_get_sys_request_cbf_psi(
	HIF_SYS_HANDLE hSysDev,
	HIF_RQB_PTR_TYPE pRQB,
	HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE *ppCbf)
{
	HIF_UNIVERSAL_RQB_TYPE pUniRQB;

    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_get_sys_request_hH_psi(+) -  hSysDev(0x%x), pRQB(0x%x), ppCbf(0x%x)", hSysDev, pRQB, ppCbf);

	HIF_ASSERT(HIF_RQB_GET_COMP_ID(pRQB) == LSA_COMP_ID_PSI);

	LSA_UNUSED_ARG(hSysDev);

	pUniRQB.pHifRQB = pRQB;

	switch(PSI_RQB_GET_OPCODE(pUniRQB.pPsiRQB))
	{
		case PSI_OPC_LD_OPEN_DEVICE:
			*ppCbf = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)pUniRQB.pPsiRQB->args.ld_open.psi_request_upper_done_ptr;
			break;
		case PSI_OPC_HD_OPEN_DEVICE:
			*ppCbf = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)pUniRQB.pPsiRQB->args.hd_open.psi_request_upper_done_ptr;	
			break;
		case PSI_OPC_LD_CLOSE_DEVICE:
			*ppCbf = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)pUniRQB.pPsiRQB->args.ld_close.psi_request_upper_done_ptr;
			break;
		case PSI_OPC_HD_CLOSE_DEVICE:
			*ppCbf = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)pUniRQB.pPsiRQB->args.hd_close.psi_request_upper_done_ptr;
			break;
		default:
			HIF_FATAL(0);
			break;
	}
}

/**
 * 
 * @param hSysDev
 * @param pRQB
 * @param pCbf
 * @return
 */
LSA_VOID hif_replace_sys_request_cbf_psi(
	HIF_SYS_HANDLE hSysDev,
	HIF_RQB_PTR_TYPE pRQB,
	HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE pCbf)
{
	HIF_UNIVERSAL_RQB_TYPE pUniRQB;

    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_sys_request_cbf_psi(+) -  hSysDev(0x%x), pRQB(0x%x), pCbf(0x%x)", hSysDev, pRQB, pCbf);

	HIF_ASSERT(HIF_RQB_GET_COMP_ID(pRQB) == LSA_COMP_ID_PSI);

	LSA_UNUSED_ARG(hSysDev);

	pUniRQB.pHifRQB = pRQB;

	switch(PSI_RQB_GET_OPCODE(pUniRQB.pPsiRQB))
	{
		case PSI_OPC_LD_OPEN_DEVICE:
			pUniRQB.pPsiRQB->args.ld_open.psi_request_upper_done_ptr = (PSI_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;
			break;
		case PSI_OPC_HD_OPEN_DEVICE:
			pUniRQB.pPsiRQB->args.hd_open.psi_request_upper_done_ptr = (PSI_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;	
			break;
		case PSI_OPC_LD_CLOSE_DEVICE:
			pUniRQB.pPsiRQB->args.ld_close.psi_request_upper_done_ptr = (PSI_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;
			break;
		case PSI_OPC_HD_CLOSE_DEVICE:
			pUniRQB.pPsiRQB->args.hd_close.psi_request_upper_done_ptr = (PSI_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;
			break;
		default:
			HIF_FATAL(0);
			break;
	}
}
#endif

#if (HIF_CFG_USE_SOCK == 1)
/**
 * 
 * @param pRQB
 * @return
 */
LSA_BOOL hif_is_close_channel_rqb_sock(LSA_VOID* pRQB)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;
	
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_is_close_channel_rqb_sock(+) -  pRQB(0x%x)", pRQB);    
    
    UniRQB.pSockRQB = (SOCK_RQB_TYPE*)pRQB;

	if(SOCK_OPC_CLOSE_CHANNEL == SOCK_RQB_GET_OPCODE(UniRQB.pSockRQB))
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/**
 * 
 * @param pRQB
 * @param hHifChHandle
 * @return
 */
LSA_VOID hif_replace_open_channel_lower_handle_sock(
	HIF_RQB_PTR_TYPE pRQB,
	LSA_HANDLE_TYPE hHifChHandle)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_lower_handle_sock(+) -  pRQB(0x%x), hHifChHandle(0x%x)", pRQB, hHifChHandle);   

	UniRQB.pSockRQB = (SOCK_RQB_TYPE*)pRQB;

	if(SOCK_RQB_GET_OPCODE(UniRQB.pSockRQB) == SOCK_OPC_OPEN_CHANNEL)
	{
		UniRQB.pSockRQB->args.channel.handle = hHifChHandle;
	}
	else
	{
		HIF_FATAL(0);
	}
}

/**
 * 
 * @param pRQB
 * @return
 */
LSA_HANDLE_TYPE hif_get_open_channel_lower_handle_sock(
	HIF_RQB_PTR_TYPE pRQB)
{
	LSA_HANDLE_TYPE h = 0;
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_lower_handle_sock(+) -  pRQB(0x%x)", pRQB); 

	UniRQB.pSockRQB = (SOCK_RQB_TYPE*)pRQB;

	if(SOCK_RQB_GET_OPCODE(UniRQB.pSockRQB) == SOCK_OPC_OPEN_CHANNEL)
	{
		h  = UniRQB.pSockRQB->args.channel.handle;
	}
	else
	{
		HIF_FATAL(0);
	}

	return h;
}

/**
 * 
 * @param pRQB
 * @param pCbf
 * @return
 */
LSA_UINT16 hif_replace_open_channel_cbf_sock(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE pCbf)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_cbf_sock(+) -  pRQB(0x%x), pCbf(0x%x)", pRQB, pCbf); 

	UniRQB.pSockRQB = (SOCK_RQB_TYPE*)pRQB;

	if(SOCK_RQB_GET_OPCODE(UniRQB.pSockRQB) == SOCK_OPC_OPEN_CHANNEL)
	{
		UniRQB.pSockRQB->args.channel.sock_request_upper_done_ptr = (SOCK_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}

/**
 * 
 * @param pRQB
 * @param *pCbf
 * @param *pPath
 * @param *hUpper
 * @return
 */
LSA_UINT16 hif_get_open_channel_comp_info_sock(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE *pCbf,
	LSA_SYS_PATH_TYPE *pPath,
	LSA_HANDLE_TYPE *hUpper
)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_comp_info_sock(+) -  pRQB(0x%x), pCbf(0x%x), pPath(0x%x), hUpper(0x%x)", pRQB, pCbf, pPath, hUpper);

	UniRQB.pSockRQB = (SOCK_RQB_TYPE*)pRQB;

	if(SOCK_RQB_GET_OPCODE(UniRQB.pSockRQB) == SOCK_OPC_OPEN_CHANNEL)
	{
		*pPath  = UniRQB.pSockRQB->args.channel.sys_path;
		*hUpper = UniRQB.pSockRQB->args.channel.handle_upper;
		*pCbf   = (HIF_UPPER_CALLBACK_FCT_PTR_TYPE)UniRQB.pSockRQB->args.channel.sock_request_upper_done_ptr;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}
#endif

#if (HIF_CFG_USE_SNMPX == 1)
/**
 * 
 * @param pRQB
 * @return
 */
LSA_BOOL hif_is_close_channel_rqb_snmpx(LSA_VOID* pRQB)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_is_close_channel_rqb_snmpx(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pSnmpxRQB = (SNMPX_RQB_TYPE*)pRQB;

	if(SNMPX_OPC_CLOSE_CHANNEL == SNMPX_RQB_GET_OPCODE(UniRQB.pSnmpxRQB))
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/**
 * 
 * @param pRQB
 * @param hHifChHandle
 * @return
 */
LSA_VOID hif_replace_open_channel_lower_handle_snmpx(
	HIF_RQB_PTR_TYPE pRQB,
	LSA_HANDLE_TYPE hHifChHandle)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;
	
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_lower_handle_snmpx(+) -  pRQB(0x%x), hHifChHandle(0x%x)", pRQB, hHifChHandle);    
    
    UniRQB.pSnmpxRQB = (SNMPX_RQB_TYPE*)pRQB;

	if(SNMPX_RQB_GET_OPCODE(UniRQB.pSnmpxRQB) == SNMPX_OPC_OPEN_CHANNEL)
	{
		UniRQB.pSnmpxRQB->args.Channel.handle = hHifChHandle;
	}
	else
	{
		HIF_FATAL(0);
	}
}

/**
 * 
 * @param pRQB
 * @return
 */
LSA_HANDLE_TYPE hif_get_open_channel_lower_handle_snmpx(
	HIF_RQB_PTR_TYPE pRQB)
{
	LSA_HANDLE_TYPE h = 0;
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_lower_handle_snmpx(+) -  pRQB(0x%x)", pRQB);   

	UniRQB.pSnmpxRQB = (SNMPX_RQB_TYPE*)pRQB;

	if(SNMPX_RQB_GET_OPCODE(UniRQB.pSnmpxRQB) == SNMPX_OPC_OPEN_CHANNEL)
	{
		h  = UniRQB.pSnmpxRQB->args.Channel.handle;
	}
	else
	{
		HIF_FATAL(0);
	}

	return h;
}

/**
 * 
 * @param pRQB
 * @param pCbf
 * @return
 */
LSA_UINT16 hif_replace_open_channel_cbf_snmpx(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE pCbf)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_cbf_snmpx(+) -  pRQB(0x%x), pCbf(0x%x)", pRQB, pCbf);

	UniRQB.pSnmpxRQB = (SNMPX_RQB_TYPE*)pRQB;

	if(SNMPX_RQB_GET_OPCODE(UniRQB.pSnmpxRQB) == SNMPX_OPC_OPEN_CHANNEL)
	{
		UniRQB.pSnmpxRQB->args.Channel.snmpx_request_upper_done_ptr = (SNMPX_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}

/**
 * 
 * @param pRQB
 * @param *pCbf
 * @param *pPath
 * @param *hUpper
 * @return
 */
LSA_UINT16 hif_get_open_channel_comp_info_snmpx(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE *pCbf,
	LSA_SYS_PATH_TYPE *pPath,
	LSA_HANDLE_TYPE *hUpper
)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_comp_info_snmpx(+) -  pRQB(0x%x), pCbf(0x%x), pPath(0x%x), hUpper(0x%x)", pRQB, pCbf, pPath, hUpper);

	UniRQB.pSnmpxRQB = (SNMPX_RQB_TYPE*)pRQB;

	if(SNMPX_RQB_GET_OPCODE(UniRQB.pSnmpxRQB) == SNMPX_OPC_OPEN_CHANNEL)
	{
		*pPath  = UniRQB.pSnmpxRQB->args.Channel.sys_path;
		*hUpper = UniRQB.pSnmpxRQB->args.Channel.handle_upper;
		*pCbf   = (HIF_UPPER_CALLBACK_FCT_PTR_TYPE)UniRQB.pSnmpxRQB->args.Channel.snmpx_request_upper_done_ptr;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}
#endif

#if (HIF_CFG_USE_OHA == 1)
/**
 * 
 * @param pRQB
 * @return
 */
LSA_BOOL hif_is_close_channel_rqb_oha(LSA_VOID* pRQB)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_is_close_channel_rqb_oha(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pOhaRQB = (OHA_RQB_TYPE*)pRQB;

	if(OHA_OPC_CLOSE_CHANNEL == OHA_RQB_GET_OPCODE(UniRQB.pOhaRQB))
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/**
 * 
 * @param pRQB
 * @param hHifChHandle
 * @return
 */
LSA_VOID hif_replace_open_channel_lower_handle_oha(
	HIF_RQB_PTR_TYPE pRQB,
	LSA_HANDLE_TYPE hHifChHandle)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_lower_handle_oha(+) -  pRQB(0x%x), hHifChHandle(0x%x)", pRQB, hHifChHandle);

    UniRQB.pOhaRQB = (OHA_RQB_TYPE*)pRQB;

	if(OHA_RQB_GET_OPCODE(UniRQB.pOhaRQB) == OHA_OPC_OPEN_CHANNEL)
	{
		UniRQB.pOhaRQB->args.channel.handle = hHifChHandle;
	}
	else
	{
		HIF_FATAL(0);
	}
}

/**
 * 
 * @param pRQB
 * @return
 */
LSA_HANDLE_TYPE hif_get_open_channel_lower_handle_oha(
	HIF_RQB_PTR_TYPE pRQB)
{
	LSA_HANDLE_TYPE h = 0;
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_lower_handle_oha(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pOhaRQB = (OHA_RQB_TYPE*)pRQB;

	if(OHA_RQB_GET_OPCODE(UniRQB.pOhaRQB) == OHA_OPC_OPEN_CHANNEL)
	{
		h  = UniRQB.pOhaRQB->args.channel.handle;
	}
	else
	{
		HIF_FATAL(0);
	}

	return h;
}

/**
 * 
 * @param pRQB
 * @param pCbf
 * @return
 */
LSA_UINT16 hif_replace_open_channel_cbf_oha(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE pCbf)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_cbf_oha(+) -  pRQB(0x%x), pCbf(0x%x)", pRQB, pCbf);

	UniRQB.pOhaRQB = (OHA_RQB_TYPE*)pRQB;

	if(OHA_RQB_GET_OPCODE(UniRQB.pOhaRQB) == OHA_OPC_OPEN_CHANNEL)
	{
		UniRQB.pOhaRQB->args.channel.oha_request_upper_done_ptr = (OHA_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}

/**
 * 
 * @param pRQB
 * @param *pCbf
 * @param *pPath
 * @param *hUpper
 * @return
 */
LSA_UINT16 hif_get_open_channel_comp_info_oha(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE *pCbf,
	LSA_SYS_PATH_TYPE *pPath,
	LSA_HANDLE_TYPE *hUpper
)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_comp_info_oha(+) -  pRQB(0x%x), pCbf(0x%x), pPath(0x%x), hUpper(0x%x)", pRQB, pCbf, pPath, hUpper);

	UniRQB.pOhaRQB = (OHA_RQB_TYPE*)pRQB;

	if(OHA_RQB_GET_OPCODE(UniRQB.pOhaRQB) == OHA_OPC_OPEN_CHANNEL)
	{
		*pPath  = UniRQB.pOhaRQB->args.channel.sys_path;
		*hUpper = UniRQB.pOhaRQB->args.channel.handle_upper;
		*pCbf   = (HIF_UPPER_CALLBACK_FCT_PTR_TYPE)UniRQB.pOhaRQB->args.channel.oha_request_upper_done_ptr;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}
#endif

#if (HIF_CFG_USE_CM == 1)
/**
 * 
 * @param pRQB
 * @return
 */
LSA_BOOL hif_is_close_channel_rqb_cm(LSA_VOID* pRQB)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_is_close_channel_rqb_cm(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pCmRQB = (CM_RQB_TYPE*)pRQB;

	if(CM_OPC_CLOSE_CHANNEL == CM_RQB_GET_OPCODE(UniRQB.pCmRQB))
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/**
 * 
 * @param pRQB
 * @param hHifChHandle
 * @return
 */
LSA_VOID hif_replace_open_channel_lower_handle_cm(
	HIF_RQB_PTR_TYPE pRQB,
	LSA_HANDLE_TYPE hHifChHandle)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_is_close_channel_rqb_cm(+) -  pRQB(0x%x), hHifChHandle(0x%x)", pRQB, hHifChHandle);

	UniRQB.pCmRQB = (CM_RQB_TYPE*)pRQB;

	if(CM_RQB_GET_OPCODE(UniRQB.pCmRQB) == CM_OPC_OPEN_CHANNEL)
	{
		UniRQB.pCmRQB->args.channel.open->handle = hHifChHandle;
	}
	else
	{
		HIF_FATAL(0);
	}
}

/**
 * 
 * @param pRQB
 * @return
 */
LSA_HANDLE_TYPE hif_get_open_channel_lower_handle_cm(
	HIF_RQB_PTR_TYPE pRQB)
{
	LSA_HANDLE_TYPE h = 0;
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_lower_handle_cm(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pCmRQB = (CM_RQB_TYPE*)pRQB;

	if(CM_RQB_GET_OPCODE(UniRQB.pCmRQB) == CM_OPC_OPEN_CHANNEL)
	{
		h  = UniRQB.pCmRQB->args.channel.open->handle;
	}
	else
	{
		HIF_FATAL(0);
	}

	return h;
}

/**
 * 
 * @param pRQB
 * @param pCbf
 * @return
 */
LSA_UINT16 hif_replace_open_channel_cbf_cm(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE pCbf)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_cbf_cm(+) -  pRQB(0x%x), pCbf(0x%x)", pRQB, pCbf);

	UniRQB.pCmRQB = (CM_RQB_TYPE*)pRQB;

	if(CM_RQB_GET_OPCODE(UniRQB.pCmRQB) == CM_OPC_OPEN_CHANNEL)
	{
		UniRQB.pCmRQB->args.channel.open->cm_request_upper_done_ptr = (CM_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}

/**
 * 
 * @param pRQB
 * @param *pCbf
 * @param *pPath
 * @param *hUpper
 * @return
 */
LSA_UINT16 hif_get_open_channel_comp_info_cm(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE *pCbf,
	LSA_SYS_PATH_TYPE *pPath,
	LSA_HANDLE_TYPE *hUpper
)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_comp_info_cm(+) -  pRQB(0x%x), pCbf(0x%x), pPath(0x%x), hUpper(0x%x)", pRQB, pCbf, pPath, hUpper);

	UniRQB.pCmRQB = (CM_RQB_TYPE*)pRQB;

	if(CM_RQB_GET_OPCODE(UniRQB.pCmRQB) == CM_OPC_OPEN_CHANNEL)
	{
		*pPath  = UniRQB.pCmRQB->args.channel.open->sys_path;
		*hUpper = UniRQB.pCmRQB->args.channel.open->handle_upper;
		*pCbf   = (HIF_UPPER_CALLBACK_FCT_PTR_TYPE)UniRQB.pCmRQB->args.channel.open->cm_request_upper_done_ptr;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}
#endif

#if (HIF_CFG_USE_CLRPC == 1)
/**
 * 
 * @param pRQB
 * @return
 */
LSA_BOOL hif_is_close_channel_rqb_clrpc(LSA_VOID* pRQB)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_is_close_channel_rqb_clrpc(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pClrpcRQB = (CLRPC_RQB_TYPE*)pRQB;

	if(CLRPC_OPC_CLOSE_CHANNEL == CLRPC_RQB_GET_OPCODE(UniRQB.pClrpcRQB))
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/**
 * 
 * @param pRQB
 * @param hHifChHandle
 * @return
 */
LSA_VOID hif_replace_open_channel_lower_handle_clrpc(
	HIF_RQB_PTR_TYPE pRQB,
	LSA_HANDLE_TYPE hHifChHandle)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_lower_handle_clrpc(+) -  pRQB(0x%x), hHifChHandle(0x%x)", pRQB, hHifChHandle);

	UniRQB.pClrpcRQB = (CLRPC_RQB_TYPE*)pRQB;

	if(CLRPC_RQB_GET_OPCODE(UniRQB.pClrpcRQB) == CLRPC_OPC_OPEN_CHANNEL)
	{
		UniRQB.pClrpcRQB->args.channel.open->handle = hHifChHandle;
	}
	else
	{
		HIF_FATAL(0);
	}
}

/**
 * 
 * @param pRQB
 * @return
 */
LSA_HANDLE_TYPE hif_get_open_channel_lower_handle_clrpc(
	HIF_RQB_PTR_TYPE pRQB)
{
	LSA_HANDLE_TYPE h = 0;
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_lower_handle_clrpc(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pClrpcRQB = (CLRPC_RQB_TYPE*)pRQB;

	if(CLRPC_RQB_GET_OPCODE(UniRQB.pClrpcRQB) == CLRPC_OPC_OPEN_CHANNEL)
	{
		h  = UniRQB.pClrpcRQB->args.channel.open->handle;
	}
	else
	{
		HIF_FATAL(0);
	}

	return h;
}

/**
 * 
 * @param pRQB
 * @param pCbf
 * @return
 */
LSA_UINT16 hif_replace_open_channel_cbf_clrpc(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE pCbf)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_cbf_clrpc(+) -  pRQB(0x%x), pCbf(0x%x)", pRQB, pCbf);

	UniRQB.pClrpcRQB = (CLRPC_RQB_TYPE*)pRQB;

	if(CLRPC_RQB_GET_OPCODE(UniRQB.pClrpcRQB) == CLRPC_OPC_OPEN_CHANNEL)
	{
		UniRQB.pClrpcRQB->args.channel.open->clrpc_request_upper_done_ptr = (CLRPC_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}

/**
 * 
 * @param pRQB
 * @param *pCbf
 * @param *pPath
 * @param *hUpper
 * @return
 */
LSA_UINT16 hif_get_open_channel_comp_info_clrpc(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE *pCbf,
	LSA_SYS_PATH_TYPE *pPath,
	LSA_HANDLE_TYPE *hUpper
)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_comp_info_clrpc(+) -  pRQB(0x%x), pCbf(0x%x), pPath(0x%x), hUpper(0x%x)", pRQB, pCbf, pPath, hUpper);

	UniRQB.pClrpcRQB = (CLRPC_RQB_TYPE*)pRQB;

	if(CLRPC_RQB_GET_OPCODE(UniRQB.pClrpcRQB) == CLRPC_OPC_OPEN_CHANNEL)
	{
		*pPath  = UniRQB.pClrpcRQB->args.channel.open->sys_path;
		*hUpper = UniRQB.pClrpcRQB->args.channel.open->handle_upper;
		*pCbf   = (HIF_UPPER_CALLBACK_FCT_PTR_TYPE)UniRQB.pClrpcRQB->args.channel.open->clrpc_request_upper_done_ptr;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}

#endif

#if (HIF_CFG_USE_IOH == 1)
/**
 * 
 * @param pRQB
 * @return
 */
LSA_BOOL hif_is_close_channel_rqb_ioh(LSA_VOID* pRQB)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_is_close_channel_rqb_ioh(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pIohRQB = (IOH_RQB_TYPE*)pRQB;

	if(IOH_OPC_CLOSE_CHANNEL == IOH_RQB_GET_OPCODE(UniRQB.pIohRQB))
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/**
 * 
 * @param pRQB
 * @param hHifChHandle
 * @return
 */
LSA_VOID hif_replace_open_channel_lower_handle_ioh(
	HIF_RQB_PTR_TYPE pRQB,
	LSA_HANDLE_TYPE hHifChHandle)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_lower_handle_ioh(+) -  pRQB(0x%x), hHifChHandle(0x%x)", pRQB, hHifChHandle);

	UniRQB.pIohRQB = (IOH_RQB_TYPE*)pRQB;

	if(IOH_RQB_GET_OPCODE(UniRQB.pIohRQB) == IOH_OPC_OPEN_CHANNEL)
	{
		UniRQB.pIohRQB->args.channel->handle = hHifChHandle;
	}
	else
	{
		HIF_FATAL(0);
	}
}

/**
 * 
 * @param pRQB
 * @return
 */
LSA_HANDLE_TYPE hif_get_open_channel_lower_handle_ioh(
	HIF_RQB_PTR_TYPE pRQB)
{
	LSA_HANDLE_TYPE h = 0;
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_lower_handle_ioh(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pIohRQB = (IOH_RQB_TYPE*)pRQB;

	if(IOH_RQB_GET_OPCODE(UniRQB.pIohRQB) == IOH_OPC_OPEN_CHANNEL)
	{
		h  = UniRQB.pIohRQB->args.channel->handle;
	}
	else
	{
		HIF_FATAL(0);
	}

	return h;
}

/**
 * 
 * @param pRQB
 * @param pCbf
 * @return
 */
LSA_UINT16 hif_replace_open_channel_cbf_ioh(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE pCbf)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_cbf_ioh(+) -  pRQB(0x%x), pCbf(0x%x)", pRQB, pCbf);

	UniRQB.pIohRQB = (IOH_RQB_TYPE*)pRQB;

	if(IOH_RQB_GET_OPCODE(UniRQB.pIohRQB) == IOH_OPC_OPEN_CHANNEL)
	{
		UniRQB.pIohRQB->args.channel->ioh_request_upper_done_ptr = (IOH_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}

/**
 * 
 * @param pRQB
 * @param *pCbf
 * @param *pPath
 * @param *hUpper
 * @return
 */
LSA_UINT16 hif_get_open_channel_comp_info_ioh(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE *pCbf,
	LSA_SYS_PATH_TYPE *pPath,
	LSA_HANDLE_TYPE *hUpper
)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_comp_info_ioh(+) -  pRQB(0x%x), pCbf(0x%x), pPath(0x%x), hUpper(0x%x)", pRQB, pCbf, pPath, hUpper);

	UniRQB.pIohRQB = (IOH_RQB_TYPE*)pRQB;

	if(IOH_RQB_GET_OPCODE(UniRQB.pIohRQB) == IOH_OPC_OPEN_CHANNEL)
	{
		*pPath  = UniRQB.pIohRQB->args.channel->sys_path;
		*hUpper = UniRQB.pIohRQB->args.channel->handle_upper;
		*pCbf   = (HIF_UPPER_CALLBACK_FCT_PTR_TYPE)UniRQB.pIohRQB->args.channel->ioh_request_upper_done_ptr;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}
#endif

#if (HIF_CFG_USE_EDD == 1)
/**
 * 
 * @param pRQB
 * @return
 */
LSA_BOOL hif_is_close_channel_rqb_edd(LSA_VOID* pRQB)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_is_close_channel_rqb_edd(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pEddRQB = (EDD_RQB_TYPE*)pRQB;

	if(EDD_OPC_CLOSE_CHANNEL == EDD_RQB_GET_OPCODE(UniRQB.pEddRQB))
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/**
 * 
 * @param pRQB
 * @param hHifChHandle
 * @return
 */
LSA_VOID hif_replace_open_channel_lower_handle_edd(
	HIF_RQB_PTR_TYPE pRQB,
	LSA_HANDLE_TYPE hHifChHandle)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_lower_handle_edd(+) -  pRQB(0x%x), hHifChHandle(0x%x)", pRQB, hHifChHandle);

	UniRQB.pEddRQB = (EDD_RQB_TYPE*)pRQB;

	if(EDD_RQB_GET_OPCODE(UniRQB.pEddRQB) == EDD_OPC_OPEN_CHANNEL)
	{
		((EDD_RQB_OPEN_CHANNEL_TYPE*)(UniRQB.pEddRQB->pParam))->Handle = hHifChHandle;
	}
	else
	{
		HIF_FATAL(0);
	}
}

/**
 * 
 * @param pRQB
 * @return
 */
LSA_HANDLE_TYPE hif_get_open_channel_lower_handle_edd(
	HIF_RQB_PTR_TYPE pRQB)
{
	LSA_HANDLE_TYPE h = 0;
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_lower_handle_edd(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pEddRQB = (EDD_RQB_TYPE*)pRQB;

	if(EDD_RQB_GET_OPCODE(UniRQB.pEddRQB) == EDD_OPC_OPEN_CHANNEL)
	{
		h  = ((EDD_RQB_OPEN_CHANNEL_TYPE*)(UniRQB.pEddRQB->pParam))->Handle;
	}
	else
	{
		HIF_FATAL(0);
	}

	return h;
}

/**
 * 
 * @param pRQB
 * @param pCbf
 * @return
 */
LSA_UINT16 hif_replace_open_channel_cbf_edd(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE pCbf)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_cbf_edd(+) -  pRQB(0x%x), pCbf(0x%x)", pRQB, pCbf);

	UniRQB.pEddRQB = (EDD_RQB_TYPE*)pRQB;

	if(EDD_RQB_GET_OPCODE(UniRQB.pEddRQB) == EDD_OPC_OPEN_CHANNEL)
	{
		((EDD_RQB_OPEN_CHANNEL_TYPE*)(UniRQB.pEddRQB->pParam))->Cbf = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}

/**
 * 
 * @param pRQB
 * @param *pCbf
 * @param *pPath
 * @param *hUpper
 * @return
 */
LSA_UINT16 hif_get_open_channel_comp_info_edd(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE *pCbf,
	LSA_SYS_PATH_TYPE *pPath,
	LSA_HANDLE_TYPE *hUpper
)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_comp_info_edd(+) -  pRQB(0x%x), pCbf(0x%x), pPath(0x%x), hUpper(0x%x)", pRQB, pCbf, pPath, hUpper);

	UniRQB.pEddRQB = (EDD_RQB_TYPE*)pRQB;

	if(EDD_RQB_GET_OPCODE(UniRQB.pEddRQB) == EDD_OPC_OPEN_CHANNEL)
	{
		*pPath  = ((EDD_RQB_OPEN_CHANNEL_TYPE*)(UniRQB.pEddRQB->pParam))->SysPath;
		*hUpper = ((EDD_RQB_OPEN_CHANNEL_TYPE*)(UniRQB.pEddRQB->pParam))->HandleUpper;
		*pCbf   = (HIF_UPPER_CALLBACK_FCT_PTR_TYPE)(((EDD_RQB_OPEN_CHANNEL_TYPE*)(UniRQB.pEddRQB->pParam))->Cbf);
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}
#endif

#if (HIF_CFG_USE_LLDP == 1)
/**
 * 
 * @param pRQB
 * @return
 */
LSA_BOOL hif_is_close_channel_rqb_lldp(LSA_VOID* pRQB)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_is_close_channel_rqb_lldp(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pLldpRQB = (LLDP_RQB_TYPE*)pRQB;

	if(LLDP_OPC_CLOSE_CHANNEL == LLDP_RQB_GET_OPCODE(UniRQB.pLldpRQB))
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/**
 * 
 * @param pRQB
 * @param hHifChHandle
 * @return
 */
LSA_VOID hif_replace_open_channel_lower_handle_lldp(
	HIF_RQB_PTR_TYPE pRQB,
	LSA_HANDLE_TYPE hHifChHandle)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_lower_handle_lldp(+) -  pRQB(0x%x), hHifChHandle(0x%x)", pRQB, hHifChHandle);

	UniRQB.pLldpRQB = (LLDP_RQB_TYPE*)pRQB;

	if(LLDP_RQB_GET_OPCODE(UniRQB.pLldpRQB) == LLDP_OPC_OPEN_CHANNEL)
	{
		UniRQB.pLldpRQB->args.channel.handle = hHifChHandle;
	}
	else
	{
		HIF_FATAL(0);
	}
}

/**
 * 
 * @param pRQB
 * @return
 */
LSA_HANDLE_TYPE hif_get_open_channel_lower_handle_lldp(
	HIF_RQB_PTR_TYPE pRQB)
{
	LSA_HANDLE_TYPE h = 0;
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_lower_handle_lldp(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pLldpRQB = (LLDP_RQB_TYPE*)pRQB;

	if(LLDP_RQB_GET_OPCODE(UniRQB.pLldpRQB) == LLDP_OPC_OPEN_CHANNEL)
	{
		h  = UniRQB.pLldpRQB->args.channel.handle;
	}
	else
	{
		HIF_FATAL(0);
	}

	return h;
}

/**
 * 
 * @param pRQB
 * @param pCbf
 * @return
 */
LSA_UINT16 hif_replace_open_channel_cbf_lldp(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE pCbf)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_cbf_lldp(+) -  pRQB(0x%x), pCbf(0x%x)", pRQB, pCbf);

	UniRQB.pLldpRQB = (LLDP_RQB_TYPE*)pRQB;

	if(LLDP_RQB_GET_OPCODE(UniRQB.pLldpRQB) == LLDP_OPC_OPEN_CHANNEL)
	{
		UniRQB.pLldpRQB->args.channel.lldp_request_upper_done_ptr = (LLDP_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}

/**
 * 
 * @param pRQB
 * @param *pCbf
 * @param *pPath
 * @param *hUpper
 * @return
 */
LSA_UINT16 hif_get_open_channel_comp_info_lldp(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE *pCbf,
	LSA_SYS_PATH_TYPE *pPath,
	LSA_HANDLE_TYPE *hUpper
)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_comp_info_lldp(+) -  pRQB(0x%x), pCbf(0x%x), pPath(0x%x), hUpper(0x%x)", pRQB, pCbf, pPath, hUpper);

	UniRQB.pLldpRQB = (LLDP_RQB_TYPE*)pRQB;

	if(LLDP_RQB_GET_OPCODE(UniRQB.pLldpRQB) == LLDP_OPC_OPEN_CHANNEL)
	{
		*pPath  = UniRQB.pLldpRQB->args.channel.sys_path;
		*hUpper = UniRQB.pLldpRQB->args.channel.handle_upper;
		*pCbf   = (HIF_UPPER_CALLBACK_FCT_PTR_TYPE)UniRQB.pLldpRQB->args.channel.lldp_request_upper_done_ptr;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}
#endif

#if (HIF_CFG_USE_MRP == 1)
/**
 * 
 * @param pRQB
 * @return
 */
LSA_BOOL hif_is_close_channel_rqb_mrp(LSA_VOID* pRQB)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_is_close_channel_rqb_mrp(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pMrpRQB = (MRP_RQB_TYPE*)pRQB;

	if(MRP_OPC_CLOSE_CHANNEL == MRP_RQB_GET_OPCODE(UniRQB.pMrpRQB))
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/**
 * 
 * @param pRQB
 * @param hHifChHandle
 * @return
 */
LSA_VOID hif_replace_open_channel_lower_handle_mrp(
	HIF_RQB_PTR_TYPE pRQB,
	LSA_HANDLE_TYPE hHifChHandle)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_lower_handle_mrp(+) -  pRQB(0x%x), hHifChHandle(0x%x)", pRQB, hHifChHandle);

	UniRQB.pMrpRQB = (MRP_RQB_TYPE*)pRQB;

	if(MRP_RQB_GET_OPCODE(UniRQB.pMrpRQB) == MRP_OPC_OPEN_CHANNEL)
	{
		UniRQB.pMrpRQB->args.channel.open.handle = hHifChHandle;
	}
	else
	{
		HIF_FATAL(0);
	}
}

/**
 * 
 * @param pRQB
 * @return
 */
LSA_HANDLE_TYPE hif_get_open_channel_lower_handle_mrp(
	HIF_RQB_PTR_TYPE pRQB)
{
	LSA_HANDLE_TYPE h = 0;
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_lower_handle_mrp(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pMrpRQB = (MRP_RQB_TYPE*)pRQB;

	if(MRP_RQB_GET_OPCODE(UniRQB.pMrpRQB) == MRP_OPC_OPEN_CHANNEL)
	{
		h  = UniRQB.pMrpRQB->args.channel.open.handle;
	}
	else
	{
		HIF_FATAL(0);
	}

	return h;
}

/**
 * 
 * @param pRQB
 * @param pCbf
 * @return
 */
LSA_UINT16 hif_replace_open_channel_cbf_mrp(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE pCbf)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_cbf_mrp(+) -  pRQB(0x%x), pCbf(0x%x)", pRQB, pCbf);

	UniRQB.pMrpRQB = (MRP_RQB_TYPE*)pRQB;

	if(MRP_RQB_GET_OPCODE(UniRQB.pMrpRQB) == MRP_OPC_OPEN_CHANNEL)
	{
		UniRQB.pMrpRQB->args.channel.open.pCbf = (MRP_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}

/**
 * 
 * @param pRQB
 * @param *pCbf
 * @param *pPath
 * @param *hUpper
 * @return
 */
LSA_UINT16 hif_get_open_channel_comp_info_mrp(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE *pCbf,
	LSA_SYS_PATH_TYPE *pPath,
	LSA_HANDLE_TYPE *hUpper
)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_comp_info_mrp(+) -  pRQB(0x%x), pCbf(0x%x), pPath(0x%x), hUpper(0x%x)", pRQB, pCbf, pPath, hUpper);

	UniRQB.pMrpRQB = (MRP_RQB_TYPE*)pRQB;

	if(MRP_RQB_GET_OPCODE(UniRQB.pMrpRQB) == MRP_OPC_OPEN_CHANNEL)
	{
		*pPath  = UniRQB.pMrpRQB->args.channel.open.sys_path;
		*hUpper = UniRQB.pMrpRQB->args.channel.open.handle_upper;
		*pCbf   = (HIF_UPPER_CALLBACK_FCT_PTR_TYPE)UniRQB.pMrpRQB->args.channel.open.pCbf;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}
#endif

#if (HIF_CFG_USE_DCP == 1)
/**
 * 
 * @param pRQB
 * @return
 */
LSA_BOOL hif_is_close_channel_rqb_dcp(LSA_VOID* pRQB)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_is_close_channel_rqb_dcp(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pDcpRQB = (DCP_RQB_TYPE*)pRQB;

	if(DCP_OPC_CLOSE_CHANNEL == DCP_UPPER_RQB_OPCODE_GET(UniRQB.pDcpRQB))
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/**
 * 
 * @param pRQB
 * @param hHifChHandle
 * @return
 */
LSA_VOID hif_replace_open_channel_lower_handle_dcp(
	HIF_RQB_PTR_TYPE pRQB,
	LSA_HANDLE_TYPE hHifChHandle)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_cbf_cm(+) -  pRQB(0x%x), hHifChHandle(0x%x)", pRQB, hHifChHandle);

	UniRQB.pDcpRQB = (DCP_RQB_TYPE*)pRQB;

	if(DCP_UPPER_RQB_OPCODE_GET(UniRQB.pDcpRQB) == DCP_OPC_OPEN_CHANNEL)
	{
		UniRQB.pDcpRQB->args.channel.handle = hHifChHandle;
	}
	else
	{
		HIF_FATAL(0);
	}
}

/**
 * 
 * @param pRQB
 * @return
 */
LSA_HANDLE_TYPE hif_get_open_channel_lower_handle_dcp(
	HIF_RQB_PTR_TYPE pRQB)
{
	LSA_HANDLE_TYPE h = 0;
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_lower_handle_dcp(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pDcpRQB = (DCP_RQB_TYPE*)pRQB;

	if(DCP_UPPER_RQB_OPCODE_GET(UniRQB.pDcpRQB) == DCP_OPC_OPEN_CHANNEL)
	{
		h  = UniRQB.pDcpRQB->args.channel.handle;
	}
	else
	{
		HIF_FATAL(0);
	}

	return h;
}

/**
 * 
 * @param pRQB
 * @param pCbf
 * @return
 */
LSA_UINT16 hif_replace_open_channel_cbf_dcp(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE pCbf)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_cbf_dcp(+) -  pRQB(0x%x), pCbf(0x%x)", pRQB, pCbf);

	UniRQB.pDcpRQB = (DCP_RQB_TYPE*)pRQB;

	if(DCP_UPPER_RQB_OPCODE_GET(UniRQB.pDcpRQB) == DCP_OPC_OPEN_CHANNEL)
	{
		UniRQB.pDcpRQB->args.channel.dcp_request_upper_done_ptr = (DCP_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}

/**
 * 
 * @param pRQB
 * @param *pCbf
 * @param *pPath
 * @param *hUpper
 * @return
 */
LSA_UINT16 hif_get_open_channel_comp_info_dcp(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE *pCbf,
	LSA_SYS_PATH_TYPE *pPath,
	LSA_HANDLE_TYPE *hUpper
)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_comp_info_dcp(+) -  pRQB(0x%x), pCbf(0x%x), pPath(0x%x), hUpper(0x%x)", pRQB, pCbf, pPath, hUpper);

	UniRQB.pDcpRQB = (DCP_RQB_TYPE*)pRQB;

	if(DCP_UPPER_RQB_OPCODE_GET(UniRQB.pDcpRQB) == DCP_OPC_OPEN_CHANNEL)
	{
		*pPath  = UniRQB.pDcpRQB->args.channel.sys_path;
		*hUpper = UniRQB.pDcpRQB->args.channel.handle_upper;
		*pCbf   = (HIF_UPPER_CALLBACK_FCT_PTR_TYPE)UniRQB.pDcpRQB->args.channel.dcp_request_upper_done_ptr;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}
#endif

#if (HIF_CFG_USE_NARE == 1)
/**
 * 
 * @param pRQB
 * @return
 */
LSA_BOOL hif_is_close_channel_rqb_nare(LSA_VOID* pRQB)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_is_close_channel_rqb_nare(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pNareRQB = (NARE_RQB_TYPE*)pRQB;

	if(NARE_OPC_CLOSE_CHANNEL == NARE_RQB_GET_OPCODE(UniRQB.pNareRQB))
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/**
 * 
 * @param pRQB
 * @param hHifChHandle
 * @return
 */
LSA_VOID hif_replace_open_channel_lower_handle_nare(
	HIF_RQB_PTR_TYPE pRQB,
	LSA_HANDLE_TYPE hHifChHandle)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_lower_handle_nare(+) -  pRQB(0x%x), hHifChHandle(0x%x)", pRQB, hHifChHandle);

	UniRQB.pNareRQB = (NARE_RQB_TYPE*)pRQB;

	if(NARE_RQB_GET_OPCODE(UniRQB.pNareRQB) == NARE_OPC_OPEN_CHANNEL)
	{
		UniRQB.pNareRQB->args.channel.handle = hHifChHandle;
	}
	else
	{
		HIF_FATAL(0);
	}
}

/**
 * 
 * @param pRQB
 * @return
 */
LSA_HANDLE_TYPE hif_get_open_channel_lower_handle_nare(
	HIF_RQB_PTR_TYPE pRQB)
{
	LSA_HANDLE_TYPE h = 0;
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_lower_handle_nare(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pNareRQB = (NARE_RQB_TYPE*)pRQB;

	if(NARE_RQB_GET_OPCODE(UniRQB.pNareRQB) == NARE_OPC_OPEN_CHANNEL)
	{
		h  = UniRQB.pNareRQB->args.channel.handle;
	}
	else
	{
		HIF_FATAL(0);
	}

	return h;
}

/**
 * 
 * @param pRQB
 * @param pCbf
 * @return
 */
LSA_UINT16 hif_replace_open_channel_cbf_nare(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE pCbf)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_cbf_nare(+) -  pRQB(0x%x), pCbf(0x%x)", pRQB, pCbf);

	UniRQB.pNareRQB = (NARE_RQB_TYPE*)pRQB;

	if(NARE_RQB_GET_OPCODE(UniRQB.pNareRQB) == NARE_OPC_OPEN_CHANNEL)
	{
		UniRQB.pNareRQB->args.channel.nare_request_upper_done_ptr = (NARE_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}

/**
 * 
 * @param pRQB
 * @param *pCbf
 * @param *pPath
 * @param *hUpper
 * @return
 */
LSA_UINT16 hif_get_open_channel_comp_info_nare(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE *pCbf,
	LSA_SYS_PATH_TYPE *pPath,
	LSA_HANDLE_TYPE *hUpper
)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_comp_info_nare(+) -  pRQB(0x%x), pCbf(0x%x), pPath(0x%x), hUpper(0x%x)", pRQB, pCbf, pPath, hUpper);

	UniRQB.pNareRQB = (NARE_RQB_TYPE*)pRQB;

	if(NARE_RQB_GET_OPCODE(UniRQB.pNareRQB) == NARE_OPC_OPEN_CHANNEL)
	{
		*pPath  = UniRQB.pNareRQB->args.channel.sys_path;
		*hUpper = UniRQB.pNareRQB->args.channel.handle_upper;
		*pCbf   = (HIF_UPPER_CALLBACK_FCT_PTR_TYPE)UniRQB.pNareRQB->args.channel.nare_request_upper_done_ptr;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}
#endif

#if (HIF_CFG_USE_GSY == 1)
/**
 * 
 * @param pRQB
 * @return
 */
LSA_BOOL hif_is_close_channel_rqb_gsy(LSA_VOID* pRQB)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_is_close_channel_rqb_gsy(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pGsyRQB = (GSY_RQB_TYPE*)pRQB;

	if(GSY_OPC_CLOSE_CHANNEL == GSY_UPPER_RQB_OPCODE_GET(UniRQB.pGsyRQB))
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/**
 * 
 * @param pRQB
 * @param hHifChHandle
 * @return
 */
LSA_VOID hif_replace_open_channel_lower_handle_gsy(
	HIF_RQB_PTR_TYPE pRQB,
	LSA_HANDLE_TYPE hHifChHandle)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_lower_handle_gsy(+) -  pRQB(0x%x), hHifChHandle(0x%x)", pRQB, hHifChHandle);

	UniRQB.pGsyRQB = (GSY_RQB_TYPE*)pRQB;

	if(GSY_UPPER_RQB_OPCODE_GET(UniRQB.pGsyRQB) == GSY_OPC_OPEN_CHANNEL)
	{
		UniRQB.pGsyRQB->args.channel.handle = hHifChHandle;
	}
	else
	{
		HIF_FATAL(0);
	}
}

/**
 * 
 * @param pRQB
 * @return
 */
LSA_HANDLE_TYPE hif_get_open_channel_lower_handle_gsy(
	HIF_RQB_PTR_TYPE pRQB)
{
	LSA_HANDLE_TYPE h = 0;
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_lower_handle_gsy(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pGsyRQB = (GSY_RQB_TYPE*)pRQB;

	if(GSY_UPPER_RQB_OPCODE_GET(UniRQB.pGsyRQB) == GSY_OPC_OPEN_CHANNEL)
	{
		h  = UniRQB.pGsyRQB->args.channel.handle;
	}
	else
	{
		HIF_FATAL(0);
	}

	return h;
}

/**
 * 
 * @param pRQB
 * @param pCbf
 * @return
 */
LSA_UINT16 hif_replace_open_channel_cbf_gsy(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE pCbf)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;
	UniRQB.pGsyRQB = (GSY_RQB_TYPE*)pRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_cbf_gsy(+) -  pRQB(0x%x), pCbf(0x%x)", pRQB, pCbf);

	if(GSY_UPPER_RQB_OPCODE_GET(UniRQB.pGsyRQB) == GSY_OPC_OPEN_CHANNEL)
	{
		UniRQB.pGsyRQB->args.channel.gsy_request_upper_done_ptr = (GSY_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}

/**
 * 
 * @param pRQB
 * @param *pCbf
 * @param *pPath
 * @param *hUpper
 * @return
 */
LSA_UINT16 hif_get_open_channel_comp_info_gsy(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE *pCbf,
	LSA_SYS_PATH_TYPE *pPath,
	LSA_HANDLE_TYPE *hUpper
)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_comp_info_gsy(+) -  pRQB(0x%x), pCbf(0x%x), pPath(0x%x), hUpper(0x%x)", pRQB, pCbf, pPath, hUpper);

	UniRQB.pGsyRQB = (GSY_RQB_TYPE*)pRQB;

	if(GSY_UPPER_RQB_OPCODE_GET(UniRQB.pGsyRQB) == GSY_OPC_OPEN_CHANNEL)
	{
		*pPath  = UniRQB.pGsyRQB->args.channel.sys_path;
		*hUpper = UniRQB.pGsyRQB->args.channel.handle_upper;
		*pCbf   = (HIF_UPPER_CALLBACK_FCT_PTR_TYPE)UniRQB.pGsyRQB->args.channel.gsy_request_upper_done_ptr;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}
#endif

#if (HIF_CFG_USE_ACP == 1)
/**
 * 
 * @param pRQB
 * @return
 */
LSA_BOOL hif_is_close_channel_rqb_acp(LSA_VOID* pRQB)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_is_close_channel_rqb_acp(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pAcpRQB = (ACP_RQB_TYPE*)pRQB;

	if(ACP_OPC_CLOSE_CHANNEL == ACP_RQB_GET_OPCODE(UniRQB.pAcpRQB))
	{
		return LSA_TRUE;
	}
	return LSA_FALSE;
}

/**
 * 
 * @param pRQB
 * @param hHifChHandle
 * @return
 */
LSA_VOID hif_replace_open_channel_lower_handle_acp(
	HIF_RQB_PTR_TYPE pRQB,
	LSA_HANDLE_TYPE hHifChHandle)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;
	
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_lower_handle_acp(+) -  pRQB(0x%x), hHifChHandle(0x%x)", pRQB, hHifChHandle);    

    UniRQB.pAcpRQB = (ACP_RQB_TYPE*)pRQB;

	if(ACP_RQB_GET_OPCODE(UniRQB.pAcpRQB) == ACP_OPC_OPEN_CHANNEL)
	{
		UniRQB.pAcpRQB->args->channel.open.handle = hHifChHandle;
	}
	else
	{
		HIF_FATAL(0);
	}
}

/**
 * 
 * @param pRQB
 * @return
 */
LSA_HANDLE_TYPE hif_get_open_channel_lower_handle_acp(
	HIF_RQB_PTR_TYPE pRQB)
{
	LSA_HANDLE_TYPE h = 0;
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_lower_handle_acp(+) -  pRQB(0x%x)", pRQB);

	UniRQB.pAcpRQB = (ACP_RQB_TYPE*)pRQB;

	if(ACP_RQB_GET_OPCODE(UniRQB.pAcpRQB) == ACP_OPC_OPEN_CHANNEL)
	{
		h  = UniRQB.pAcpRQB->args->channel.open.handle;
	}
	else
	{
		HIF_FATAL(0);
	}

	return h;
}

/**
 * 
 * @param pRQB
 * @param pCbf
 * @return
 */
LSA_UINT16 hif_replace_open_channel_cbf_acp(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE pCbf)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_replace_open_channel_cbf_acp(+) -  pRQB(0x%x), pCbf(0x%x)", pRQB, pCbf);

	UniRQB.pAcpRQB = (ACP_RQB_TYPE*)pRQB;

	if(ACP_RQB_GET_OPCODE(UniRQB.pAcpRQB) == ACP_OPC_OPEN_CHANNEL)
	{
		UniRQB.pAcpRQB->args->channel.open.acp_request_upper_done_ptr = (ACP_UPPER_CALLBACK_FCT_PTR_TYPE)pCbf;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}

/**
 * 
 * @param pRQB
 * @param *pCbf
 * @param *pPath
 * @param *hUpper
 * @return
 */
LSA_UINT16 hif_get_open_channel_comp_info_acp(
	HIF_RQB_PTR_TYPE pRQB,
	HIF_UPPER_CALLBACK_FCT_PTR_TYPE *pCbf,
	LSA_SYS_PATH_TYPE *pPath,
	LSA_HANDLE_TYPE *hUpper
)
{
	HIF_UNIVERSAL_RQB_TYPE UniRQB;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_comp_info_acp(+) -  pRQB(0x%x), pCbf(0x%x), pPath(0x%x), hUpper(0x%x)", pRQB, pCbf, pPath, hUpper);

	UniRQB.pAcpRQB = (ACP_RQB_TYPE*)pRQB;

	if(ACP_RQB_GET_OPCODE(UniRQB.pAcpRQB) == ACP_OPC_OPEN_CHANNEL)
	{
		*pPath  = UniRQB.pAcpRQB->args->channel.open.sys_path;
		*hUpper = UniRQB.pAcpRQB->args->channel.open.handle_upper;
		*pCbf   = (HIF_UPPER_CALLBACK_FCT_PTR_TYPE)UniRQB.pAcpRQB->args->channel.open.acp_request_upper_done_ptr;
	}
	else
	{
		return LSA_RET_ERR_PARAM;
	}

	return LSA_RET_OK;
}
#endif

LSA_VOID hif_update_std_comp_access_funcs(HIF_PIPE_PTR_TYPE pPipe)
{
	HIF_COMPONENT_SUPPORT_STD_PTR_TYPE pCompFuncs;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_update_std_comp_access_funcs(+) -  pPipe(0x%x)", pPipe);

	if(g_pHifData->CompStore.CompStd[pPipe->comp_id].bUsed == LSA_FALSE)
	{
		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_ERROR, "hif_update_std_comp_access_funcs(-) callback functions for handling compId(0x%x) RQBs missing (hH(%d) pipeId(%d))", pPipe->comp_id, pPipe->pInstance->hH, pPipe->pipe_id);    
		HIF_FATAL(0);
	}

	pCompFuncs = &g_pHifData->CompStore.CompStd[pPipe->comp_id].Comp;

	HIF_ASSERT(pCompFuncs != LSA_NULL);

	pPipe->func.pCompFuncs = pCompFuncs;
}

/**
 * 
 * @return
 */
LSA_VOID hif_install_integrated_comp_access_funcs(LSA_VOID)
{
	HIF_COMPONENT_SUPPORT_STD_TYPE CompFuncs;
	HIF_COMPONENT_SUPPORT_SYS_TYPE SysCompFuncs;
	LSA_UINT16 retVal;

    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "hif_install_integrated_comp_access_funcs(+)");

	SysCompFuncs.GetSysRequestCbf = hif_get_sys_request_cbf_hif_dummy;
	SysCompFuncs.GetSysRequesthH = hif_get_sys_request_hH_hif_dummy;
	SysCompFuncs.ReplaceSysRequestCbf = hif_replace_sys_request_cbf_hif_dummy;
	#if(HIF_CFG_COMPILE_SERIALIZATION == 1)
	SysCompFuncs.SerializeUpper = hif_hif_serialize_upper;
	SysCompFuncs.DeserializeUpper = hif_hif_deserialize_upper;
	SysCompFuncs.SerializeLower = hif_hif_serialize_lower;
	SysCompFuncs.DeserializeLower = hif_hif_deserialize_lower;
	#endif
	retVal = hif_register_sys_component(LSA_COMP_ID_HIF, &SysCompFuncs);
	HIF_ASSERT(retVal == LSA_RET_OK);

	#if (HIF_CFG_USE_PSI == 1)
		SysCompFuncs.GetSysRequestCbf = hif_get_sys_request_cbf_psi;
		SysCompFuncs.GetSysRequesthH = hif_get_sys_request_hH_psi;
		SysCompFuncs.ReplaceSysRequestCbf = hif_replace_sys_request_cbf_psi;
		#if(HIF_CFG_COMPILE_SERIALIZATION == 1)
		SysCompFuncs.SerializeUpper = hif_psi_serialize_upper;
		SysCompFuncs.DeserializeUpper = hif_psi_deserialize_upper;
		SysCompFuncs.SerializeLower = hif_psi_serialize_lower;
		SysCompFuncs.DeserializeLower = hif_psi_deserialize_lower;
		#endif
		retVal = hif_register_sys_component(LSA_COMP_ID_PSI, &SysCompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
	#endif

	#if (HIF_CFG_USE_SOCK == 1)
		CompFuncs.IsCloseChannelRqb = hif_is_close_channel_rqb_sock;
		CompFuncs.GetOpenChannelCompInfo = hif_get_open_channel_comp_info_sock;
		CompFuncs.GetOpenChannelLowerHandle = hif_get_open_channel_lower_handle_sock;
		CompFuncs.ReplaceOpenChannelCbf = hif_replace_open_channel_cbf_sock;
		CompFuncs.ReplaceOpenChannelLowerHandle = hif_replace_open_channel_lower_handle_sock;
		#if(HIF_CFG_COMPILE_SERIALIZATION == 1)
		CompFuncs.SerializeUpper = hif_sock_serialize_upper;
		CompFuncs.DeserializeUpper = hif_sock_deserialize_upper;
		CompFuncs.SerializeLower = hif_sock_serialize_lower;
		CompFuncs.DeserializeLower = hif_sock_deserialize_lower;
		#endif
		retVal = hif_register_standard_component(LSA_COMP_ID_SOCK, &CompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
	#endif
	#if (HIF_CFG_USE_SNMPX == 1)
		CompFuncs.IsCloseChannelRqb = hif_is_close_channel_rqb_snmpx;
		CompFuncs.GetOpenChannelCompInfo = hif_get_open_channel_comp_info_snmpx;
		CompFuncs.GetOpenChannelLowerHandle = hif_get_open_channel_lower_handle_snmpx;
		CompFuncs.ReplaceOpenChannelCbf = hif_replace_open_channel_cbf_snmpx;
		CompFuncs.ReplaceOpenChannelLowerHandle = hif_replace_open_channel_lower_handle_snmpx;
		#if(HIF_CFG_COMPILE_SERIALIZATION == 1)
		CompFuncs.SerializeUpper = hif_snmpx_serialize_upper;
		CompFuncs.DeserializeUpper = hif_snmpx_deserialize_upper;
		CompFuncs.SerializeLower = hif_snmpx_serialize_lower;
		CompFuncs.DeserializeLower = hif_snmpx_deserialize_lower;
		#endif
		retVal = hif_register_standard_component(LSA_COMP_ID_SNMPX, &CompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
	#endif
	#if (HIF_CFG_USE_OHA == 1)
		CompFuncs.IsCloseChannelRqb = hif_is_close_channel_rqb_oha;
		CompFuncs.GetOpenChannelCompInfo = hif_get_open_channel_comp_info_oha;
		CompFuncs.GetOpenChannelLowerHandle = hif_get_open_channel_lower_handle_oha;
		CompFuncs.ReplaceOpenChannelCbf = hif_replace_open_channel_cbf_oha;
		CompFuncs.ReplaceOpenChannelLowerHandle = hif_replace_open_channel_lower_handle_oha;
		#if(HIF_CFG_COMPILE_SERIALIZATION == 1)
		CompFuncs.SerializeUpper = hif_oha_serialize_upper;
		CompFuncs.DeserializeUpper = hif_oha_deserialize_upper;
		CompFuncs.SerializeLower = hif_oha_serialize_lower;
		CompFuncs.DeserializeLower = hif_oha_deserialize_lower;
		#endif
		retVal = hif_register_standard_component(LSA_COMP_ID_OHA, &CompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
	#endif
	#if (HIF_CFG_USE_CM == 1)
		CompFuncs.IsCloseChannelRqb = hif_is_close_channel_rqb_cm;
		CompFuncs.GetOpenChannelCompInfo = hif_get_open_channel_comp_info_cm;
		CompFuncs.GetOpenChannelLowerHandle = hif_get_open_channel_lower_handle_cm;
		CompFuncs.ReplaceOpenChannelCbf = hif_replace_open_channel_cbf_cm;
		CompFuncs.ReplaceOpenChannelLowerHandle = hif_replace_open_channel_lower_handle_cm;
		#if(HIF_CFG_COMPILE_SERIALIZATION == 1)
		CompFuncs.SerializeUpper = hif_cm_serialize_upper;
		CompFuncs.DeserializeUpper = hif_cm_deserialize_upper;
		CompFuncs.SerializeLower = hif_cm_serialize_lower;
		CompFuncs.DeserializeLower = hif_cm_deserialize_lower;
		#endif
		retVal = hif_register_standard_component(LSA_COMP_ID_CM, &CompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
	#endif
	#if (HIF_CFG_USE_CLRPC == 1)
		CompFuncs.IsCloseChannelRqb = hif_is_close_channel_rqb_clrpc;
		CompFuncs.GetOpenChannelCompInfo = hif_get_open_channel_comp_info_clrpc;
		CompFuncs.GetOpenChannelLowerHandle = hif_get_open_channel_lower_handle_clrpc;
		CompFuncs.ReplaceOpenChannelCbf = hif_replace_open_channel_cbf_clrpc;
		CompFuncs.ReplaceOpenChannelLowerHandle = hif_replace_open_channel_lower_handle_clrpc;
		#if(HIF_CFG_COMPILE_SERIALIZATION == 1)
		CompFuncs.SerializeUpper = hif_clrpc_serialize_upper;
		CompFuncs.DeserializeUpper = hif_clrpc_deserialize_upper;
		CompFuncs.SerializeLower = hif_clrpc_serialize_lower;
		CompFuncs.DeserializeLower = hif_clrpc_deserialize_lower;
		#endif
		retVal = hif_register_standard_component(LSA_COMP_ID_CLRPC, &CompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
	#endif
	#if (HIF_CFG_USE_IOH == 1)
		CompFuncs.IsCloseChannelRqb = hif_is_close_channel_rqb_ioh;
		CompFuncs.GetOpenChannelCompInfo = hif_get_open_channel_comp_info_ioh;
		CompFuncs.GetOpenChannelLowerHandle = hif_get_open_channel_lower_handle_ioh;
		CompFuncs.ReplaceOpenChannelCbf = hif_replace_open_channel_cbf_ioh;
		CompFuncs.ReplaceOpenChannelLowerHandle = hif_replace_open_channel_lower_handle_ioh;
		#if(HIF_CFG_COMPILE_SERIALIZATION == 1)
		CompFuncs.SerializeUpper = hif_ioh_serialize_upper;
		CompFuncs.DeserializeUpper = hif_ioh_deserialize_upper;
		CompFuncs.SerializeLower = hif_ioh_serialize_lower;
		CompFuncs.DeserializeLower = hif_ioh_deserialize_lower;
		#endif
		retVal = hif_register_standard_component(LSA_COMP_ID_IOH, &CompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
	#endif
	#if (HIF_CFG_USE_EDD == 1)
		CompFuncs.IsCloseChannelRqb = hif_is_close_channel_rqb_edd;
		CompFuncs.GetOpenChannelCompInfo = hif_get_open_channel_comp_info_edd;
		CompFuncs.GetOpenChannelLowerHandle = hif_get_open_channel_lower_handle_edd;
		CompFuncs.ReplaceOpenChannelCbf = hif_replace_open_channel_cbf_edd;
		CompFuncs.ReplaceOpenChannelLowerHandle = hif_replace_open_channel_lower_handle_edd;
		#if(HIF_CFG_COMPILE_SERIALIZATION == 1)
		CompFuncs.SerializeUpper = hif_edd_serialize_upper;
		CompFuncs.DeserializeUpper = hif_edd_deserialize_upper;
		CompFuncs.SerializeLower = hif_edd_serialize_lower;
		CompFuncs.DeserializeLower = hif_edd_deserialize_lower;
		#endif
		retVal = hif_register_standard_component(LSA_COMP_ID_EDD, &CompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
		retVal = hif_register_standard_component(LSA_COMP_ID_EDDI, &CompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
		retVal = hif_register_standard_component(LSA_COMP_ID_EDDS, &CompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
		retVal = hif_register_standard_component(LSA_COMP_ID_EDDP, &CompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
	#endif
	#if (HIF_CFG_USE_LLDP == 1)
		CompFuncs.IsCloseChannelRqb = hif_is_close_channel_rqb_lldp;
		CompFuncs.GetOpenChannelCompInfo = hif_get_open_channel_comp_info_lldp;
		CompFuncs.GetOpenChannelLowerHandle = hif_get_open_channel_lower_handle_lldp;
		CompFuncs.ReplaceOpenChannelCbf = hif_replace_open_channel_cbf_lldp;
		CompFuncs.ReplaceOpenChannelLowerHandle = hif_replace_open_channel_lower_handle_lldp;
		#if(HIF_CFG_COMPILE_SERIALIZATION == 1)
		CompFuncs.SerializeUpper = hif_lldp_serialize_upper;
		CompFuncs.DeserializeUpper = hif_lldp_deserialize_upper;
		CompFuncs.SerializeLower = hif_lldp_serialize_lower;
		CompFuncs.DeserializeLower = hif_lldp_deserialize_lower;
		#endif
		retVal = hif_register_standard_component(LSA_COMP_ID_LLDP, &CompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
	#endif
	#if (HIF_CFG_USE_MRP == 1)
		CompFuncs.IsCloseChannelRqb = hif_is_close_channel_rqb_mrp;
		CompFuncs.GetOpenChannelCompInfo = hif_get_open_channel_comp_info_mrp;
		CompFuncs.GetOpenChannelLowerHandle = hif_get_open_channel_lower_handle_mrp;
		CompFuncs.ReplaceOpenChannelCbf = hif_replace_open_channel_cbf_mrp;
		CompFuncs.ReplaceOpenChannelLowerHandle = hif_replace_open_channel_lower_handle_mrp;
		#if(HIF_CFG_COMPILE_SERIALIZATION == 1)
		CompFuncs.SerializeUpper = hif_mrp_serialize_upper;
		CompFuncs.DeserializeUpper = hif_mrp_deserialize_upper;
		CompFuncs.SerializeLower = hif_mrp_serialize_lower;
		CompFuncs.DeserializeLower = hif_mrp_deserialize_lower;
		#endif
		retVal = hif_register_standard_component(LSA_COMP_ID_MRP, &CompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
	#endif
	#if (HIF_CFG_USE_DCP == 1)
		CompFuncs.IsCloseChannelRqb = hif_is_close_channel_rqb_dcp;
		CompFuncs.GetOpenChannelCompInfo = hif_get_open_channel_comp_info_dcp;
		CompFuncs.GetOpenChannelLowerHandle = hif_get_open_channel_lower_handle_dcp;
		CompFuncs.ReplaceOpenChannelCbf = hif_replace_open_channel_cbf_dcp;
		CompFuncs.ReplaceOpenChannelLowerHandle = hif_replace_open_channel_lower_handle_dcp;
		#if(HIF_CFG_COMPILE_SERIALIZATION == 1)
		CompFuncs.SerializeUpper = hif_dcp_serialize_upper;
		CompFuncs.DeserializeUpper = hif_dcp_deserialize_upper;
		CompFuncs.SerializeLower = hif_dcp_serialize_lower;
		CompFuncs.DeserializeLower = hif_dcp_deserialize_lower;
		#endif
		retVal = hif_register_standard_component(LSA_COMP_ID_DCP, &CompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
	#endif
	#if (HIF_CFG_USE_NARE == 1)
		CompFuncs.IsCloseChannelRqb = hif_is_close_channel_rqb_nare;
		CompFuncs.GetOpenChannelCompInfo = hif_get_open_channel_comp_info_nare;
		CompFuncs.GetOpenChannelLowerHandle = hif_get_open_channel_lower_handle_nare;
		CompFuncs.ReplaceOpenChannelCbf = hif_replace_open_channel_cbf_nare;
		CompFuncs.ReplaceOpenChannelLowerHandle = hif_replace_open_channel_lower_handle_nare;
		#if(HIF_CFG_COMPILE_SERIALIZATION == 1)
		CompFuncs.SerializeUpper = hif_nare_serialize_upper;
		CompFuncs.DeserializeUpper = hif_nare_deserialize_upper;
		CompFuncs.SerializeLower = hif_nare_serialize_lower;
		CompFuncs.DeserializeLower = hif_nare_deserialize_lower;
		#endif
		retVal = hif_register_standard_component(LSA_COMP_ID_NARE, &CompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
	#endif
	#if (HIF_CFG_USE_GSY == 1)
		CompFuncs.IsCloseChannelRqb = hif_is_close_channel_rqb_gsy;
		CompFuncs.GetOpenChannelCompInfo = hif_get_open_channel_comp_info_gsy;
		CompFuncs.GetOpenChannelLowerHandle = hif_get_open_channel_lower_handle_gsy;
		CompFuncs.ReplaceOpenChannelCbf = hif_replace_open_channel_cbf_gsy;
		CompFuncs.ReplaceOpenChannelLowerHandle = hif_replace_open_channel_lower_handle_gsy;
		#if(HIF_CFG_COMPILE_SERIALIZATION == 1)
		CompFuncs.SerializeUpper = hif_gsy_serialize_upper;
		CompFuncs.DeserializeUpper = hif_gsy_deserialize_upper;
		CompFuncs.SerializeLower = hif_gsy_serialize_lower;
		CompFuncs.DeserializeLower = hif_gsy_deserialize_lower;
		#endif
		retVal = hif_register_standard_component(LSA_COMP_ID_GSY, &CompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
	#endif
	#if (HIF_CFG_USE_ACP == 1)
		CompFuncs.IsCloseChannelRqb = hif_is_close_channel_rqb_acp;
		CompFuncs.GetOpenChannelCompInfo = hif_get_open_channel_comp_info_acp;
		CompFuncs.GetOpenChannelLowerHandle = hif_get_open_channel_lower_handle_acp;
		CompFuncs.ReplaceOpenChannelCbf = hif_replace_open_channel_cbf_acp;
		CompFuncs.ReplaceOpenChannelLowerHandle = hif_replace_open_channel_lower_handle_acp;
		#if(HIF_CFG_COMPILE_SERIALIZATION == 1)
		CompFuncs.SerializeUpper = hif_acp_serialize_upper;
		CompFuncs.DeserializeUpper = hif_acp_deserialize_upper;
		CompFuncs.SerializeLower = hif_acp_serialize_lower;
		CompFuncs.DeserializeLower = hif_acp_deserialize_lower;
		#endif
		retVal = hif_register_standard_component(LSA_COMP_ID_ACP, &CompFuncs);
		HIF_ASSERT(retVal == LSA_RET_OK);
	#endif
}

/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/
