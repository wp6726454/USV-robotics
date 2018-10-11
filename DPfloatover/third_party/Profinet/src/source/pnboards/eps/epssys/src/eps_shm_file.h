#ifndef EPS_SHM_FILE_H_
#define EPS_SHM_FILE_H_

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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_shm_file.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#include "eps_sys.h"
#include <eps_plf.h>   
#include <eps_app.h>   
#include "eps_rtos.h"

#if defined (EPS_CFG_USE_PNDEVDRV)
#include <precomp.h>
#include <PnDev_Driver_Inc.h> /* PnDevDrv Interface   */
#endif

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

typedef struct eps_shm_file_transfer_tag {
	struct
	{
		LSA_UINT32   bReqStart;
		LSA_UINT32   uReqPacketRead;
		LSA_UINT32   isHD;

        #if defined (EPS_CFG_USE_PNDEVDRV)
        ePNDEV_BOARD eBoard;
        #endif
        EPS_APP_FILE_TAG_ENUM FileType;   //IN
	} req;
	struct
	{
		LSA_UINT32   bRspDone;
		LSA_UINT32   bRspErr;
		LSA_UINT32   uFileSize;
		LSA_UINT32   uActPacket;
		LSA_UINT32   uActPacketSize;
		LSA_UINT8  uData[EPS_APP_FILE_CHUNK_SIZE]; 
		LSA_UINT32   bTransferDone;
		LSA_UINT32   bTransferDoneCnf;		        		
	} rsp;
} EPS_SHM_FILE_TRANSFER_TYPE, *EPS_SHM_FILE_TRANSFER_PTR_TYPE;


/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID eps_shm_file_init(LSA_UINT8* pBase);
LSA_VOID eps_shm_file_undo_init(LSA_VOID);
LSA_VOID eps_file_init(LSA_VOID);
LSA_VOID eps_file_undo_init(LSA_VOID);
LSA_VOID eps_shm_file_alloc(EPS_APP_FILE_TAG_PTR_TYPE pFile);
LSA_VOID eps_shm_file_free(EPS_APP_FILE_TAG_PTR_TYPE pFile);
EPS_APP_FILE_TAG_PTR_TYPE eps_read_file(LSA_CHAR* path);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_SHM_IF_H_ */
