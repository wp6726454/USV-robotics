#ifndef EPS_APP_H_
#define EPS_APP_H_

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
/*  F i l e               &F: eps_app.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS APP adapttion interface                                              */
/*                                                                           */
/*****************************************************************************/

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#include "psi_inc.h"
#include "eps_rtos.h"

#if defined (EPS_CFG_USE_PNDEVDRV)
#include <precomp.h>
#include <PnDev_Driver_Inc.h> /* PnDevDrv Interface   */
#endif

#define EPS_APP_FILE_CHUNK_SIZE   1024*128 /*128 KB*/

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

typedef enum eps_app_file_type_enum { 
	EPS_APP_FILE_FW         = 0x00,  // Firmware Image
	EPS_APP_FILE_BOOTLOADER = 0x01,  // Bootloader Image
	EPS_APP_FILE_INVALID    = 0x02,  // Invalid File
} EPS_APP_FILE_TAG_ENUM;

typedef struct eps_app_file_tag
{
    LSA_UINT32            nHandle;    //OUT
	LSA_UINT32            uFileSize;  //OUT
	LSA_UINT8*            pFileData;  //OUT
    LSA_BOOL              isHD;       //IN      0 = LD, 1 = HD
    #if defined (EPS_CFG_USE_PNDEVDRV)
    ePNDEV_BOARD          eBoard;     //IN
    #endif
    EPS_APP_FILE_TAG_ENUM FileType;   //IN

} EPS_APP_FILE_TAG_TYPE, *EPS_APP_FILE_TAG_PTR_TYPE;

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID EPS_APP_INIT(LSA_VOID_PTR_TYPE hSys, EPS_HW_INFO_PTR_TYPE pEpsHw);
LSA_VOID EPS_APP_UNDO_INIT(LSA_VOID_PTR_TYPE hSys, EPS_HW_INFO_PTR_TYPE pEpsHw); 

//File (FW & Bootloader)
LSA_VOID EPS_APP_ALLOC_FILE(EPS_APP_FILE_TAG_PTR_TYPE file_tag); //0 = LD, 1 = HD
LSA_VOID EPS_APP_FREE_FILE(EPS_APP_FILE_TAG_PTR_TYPE file_tag);
LSA_VOID EPS_APP_INSTALL_DRV_OPEN(PSI_LD_RUNS_ON_TYPE ldRunsOnType);
LSA_VOID EPS_APP_SAVE_DUMP_FILE(EPS_FATAL_STORE_DUMP_PTR_TYPE pDumpStoreInfo);

LSA_VOID EPS_APP_FATAL_HOOK(LSA_VOID);
LSA_VOID EPS_APP_KILL_EPS(LSA_VOID);
LSA_VOID EPS_APP_HALT(LSA_VOID);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_APP_H_ */
