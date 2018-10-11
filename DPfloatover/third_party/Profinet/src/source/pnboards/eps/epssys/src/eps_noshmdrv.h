#ifndef EPS_NOSHMDRV_H_
#define EPS_NOSHMDRV_H_

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
/*  F i l e               &F: eps_noshmdrv.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS No Shared Memory - Shared Memory Driver                              */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/

typedef LSA_VOID (*EPS_NOSHMDRV_APPREADY_CBF)(EPS_SHM_HW_PTR_TYPE pHw, LSA_VOID_PTR_TYPE pHifHandle);
typedef LSA_VOID (*EPS_NOSHMDRV_APPSHUTD_CBF)(EPS_SHM_HW_PTR_TYPE pHw);

#if ( PSI_CFG_USE_PNTRC == 1 )
typedef LSA_VOID (*EPS_SHM_TRACE_BUFFER_FULL_USER_CBF) (LSA_VOID_PTR_TYPE hSys, LSA_VOID_PTR_TYPE hTrace, LSA_UINT32 uBufferId, LSA_UINT8* pBuffer, LSA_UINT32 uLength);
#endif

typedef struct
{
	EPS_NOSHMDRV_APPREADY_CBF     pAppReadyCbf;
	EPS_NOSHMDRV_APPSHUTD_CBF     pAppShutdownCbf;

#if ( PSI_CFG_USE_PNTRC == 1 )
	EPS_SHM_TRACE_BUFFER_FULL_USER_CBF pTraceBufferFullCbf;
	EPS_SHM_PNTRC_PARAM_TYPE      sPntrcParam;
#endif
} EPS_NOSHMDRV_INSTALL_ARGS_TYPE, *EPS_NOSHMDRV_INSTALL_ARGS_PTR_TYPE;

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID   eps_noshmdrv_install  (EPS_NOSHMDRV_INSTALL_ARGS_PTR_TYPE pInstallArgs);
LSA_UINT16 eps_noshmdrv_open     (LSA_VOID_PTR_TYPE hSys, EPS_SHM_OPEN_OPTION_PTR_TYPE pOption, EPS_SHM_HW_PTR_PTR_TYPE ppHwInstOut);
LSA_UINT16 eps_noshmdrv_close    (EPS_SHM_HW_PTR_TYPE pHwInstIn);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_NOSHMDRV_H_ */