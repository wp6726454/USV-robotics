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
/*  F i l e               &F: eps_pndevdrv.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PNDevDriver Interface Adaption                                       */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID 20031
#define EPS_MODULE_ID LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* - Includes
 * -------------------------------------------------------------------------------------
 */
#include <eps_rtos.h>
#include <eps_sys.h> /* Types / Prototypes / Fucns */
#include <psi_inc.h>

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || \
     (PSI_CFG_USE_EDDS == 1))
/*-------------------------------------------------------------------------*/

#include <PnDev_Driver_Inc.h> /* PnDevDrv Interface                       */
#include <precomp.h>

#include <eps_app.h>        /* EPS Application Api                      */
#include <eps_cp_hw.h>      /* EPS CP PSI adaption                      */
#include <eps_hif_drv_if.h> /* HIF-Driver Interface                     */
#include <eps_hif_universal_drv.h> /* Generic HIF-Driver                       */
#include <eps_hw_edds.h>           /* EDDS_LL					                */
#include <eps_hw_ertec200.h>   /* Ertec200 hw adaption                     */
#include <eps_hw_ertec400.h>   /* Ertec400 hw adaption                     */
#include <eps_hw_pnip.h>       /* Ertec200p HW adaption                    */
#include <eps_hw_soc.h>        /* Soc1 hw adaption                         */
#include <eps_ipc.h>           /* EPS Interprocessor communication         */
#include <eps_locks.h>         /* EPS Locks                                */
#include <eps_mem.h>           /* EPS Memory Interface                     */
#include <eps_plf.h>           /* EPS Platform Api                         */
#include <eps_pn_drv_if.h>     /* PN Device Driver Interface               */
#include <eps_pndevdrv.h>      /* PNDEV-Driver Interface implementation    */
#include <eps_shm_file.h>      /* EPS Shared Memory File Interface         */
#include <eps_shm_if.h>        /* EPS Shared Memory Interface              */
#include <eps_shm_if_config.h> /* EPS Shared Memory Configuration struct   */
#include <eps_tasks.h>         /* EPS TASK API                             */
#include <eps_trc.h>           /* Tracing                                  */

#include <pntrc_cfg.h>
#include <pntrc_sys.h>
#include <pntrc_usr.h>

#if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1))
#define EPS_CFG_USE_PNDEVDRV
#endif

#ifdef EPS_CFG_USE_PNDEVDRV

/* - Lokal Defines
 * --------------------------------------------------------------------------------
 */

#define EPS_CFG_PNDEVDRV_MAX_BOARDS 10

#ifndef EPS_SUPPRESS_PRINTF
#define EPS_PNDEVDRV_PRINTF_DRIVER_INFO
#define EPS_PNDEVDRV_PRINTF_OPEN_CLOSE_INFO
#define EPS_PNDEVDRV_PRINTF_FOUND_BOARDS
#define EPS_PNDEVDRV_PRINTF_EVENTS
//#define EPS_PNDEVDRV_PRINTF_ISR
#endif

#if (EPS_PLF != EPS_PLF_WINDOWS_X86)
// generate and use artifical MAC instead of MAC from pndev driver
#define EPS_PNDEVDRV_GEN_ARTIFICAL_MAC
#endif

/* - Function Forward Declaration
 * ----------------------------------------------------------------- */

LSA_VOID eps_pndevdrv_uninstall(LSA_VOID);
LSA_UINT16 eps_pndevdrv_enable_interrupt(
    EPS_PNDEV_HW_PTR_TYPE pHwInstIn,
    EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt,
    EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
LSA_UINT16 eps_pndevdrv_disable_interrupt(
    EPS_PNDEV_HW_PTR_TYPE pHwInstIn,
    EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt);
LSA_UINT16 eps_pndevdrv_set_gpio(struct eps_pndev_hw_tag* pHwInstIn,
                                 EPS_PNDEV_GPIO_DESC_TYPE gpio);
LSA_UINT16 eps_pndevdrv_clear_gpio(struct eps_pndev_hw_tag* pHwInstIn,
                                   EPS_PNDEV_GPIO_DESC_TYPE gpio);
LSA_UINT16 eps_pndevdrv_timer_ctrl_start(struct eps_pndev_hw_tag* pHwInstIn,
                                         EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
LSA_UINT16 eps_pndevdrv_timer_ctrl_stop(struct eps_pndev_hw_tag* pHwInstIn);
LSA_UINT16 eps_pndevdrv_read_trace_data(EPS_PNDEV_HW_PTR_TYPE pHwInstIn,
                                        LSA_UINT32 offset, LSA_UINT8* ptr,
                                        LSA_UINT32 size);
LSA_UINT16 eps_pndevdrv_write_trace_data(EPS_PNDEV_HW_PTR_TYPE pHwInstIn,
                                         LSA_UINT8* ptr, LSA_UINT32 offset,
                                         LSA_UINT32 size);
LSA_UINT16 eps_pndevdrv_save_dump(EPS_PNDEV_HW_PTR_TYPE pHwInstIn);
LSA_VOID eps_pndevdrv_convert_err_string(PNDEV_CHAR* pIn, LSA_CHAR* pOut);

static LSA_VOID eps_pndevdrv_enter(LSA_VOID);
static LSA_VOID eps_pndevdrv_exit(LSA_VOID);

/* - Typedefs
 * -------------------------------------------------------------------------------------
 */
struct eps_pndevdrv_store_tag;

typedef struct eps_pndevdrv_board_tag {
  LSA_BOOL bUsed;
  EPS_PNDEV_HW_TYPE sHw;
  LSA_UINT32 uCountIsrEnabled;
  EPS_PNDEV_CALLBACK_TYPE sIsrIsochronous;
  EPS_PNDEV_CALLBACK_TYPE sIsrPnGathered;
  EPS_PNDEV_CALLBACK_TYPE sIsrPnNrt;
  uPNDEV_GET_DEVICE_INFO_OUT* pDeviceInfo;
  uPNDEV_OPEN_DEVICE_OUT OpenDevice;
  EPS_PNDEV_LOCATION_TYPE Location;
  EPS_SYS_TYPE sysDev;
  LSA_BOOL bBoardFwUsed;
  EPS_HIF_DRV_HW_PTR_TYPE pHifDrvHwInstOut;
  EPS_SHM_UPPER_CONFIG_TYPE SharedMemConfig;
  struct {
    struct {
      struct {
        LSA_UINT32 uPhysicalBase;
        LSA_UINT32 uMsgVal;
        LSA_UINT32 uRxHandle;
      } Rx;
    } Ipc;
    struct {
      LSA_UINT8* pLocalAddress; /* Local Host Mem Address */
    } HostMem;
  } Hif;
  struct eps_pndevdrv_store_tag* pBackRef;
  struct {
    EPS_PNDEV_CALLBACK_TYPE sTimerCbf;
    LSA_UINT32 hThread;
    LSA_BOOL bRunning;
  } TimerCtrl;
  struct {
    LSA_BOOL bOpened;
    PNTRC_CPU_HANDLE_PTR_TYPE hPntrcCPU;
  } Pntrc;
#if ((PSI_CFG_USE_EDDS == 1) && (defined EDDS_CFG_HW_INTEL))
  EPS_EDDS_INTEL_LL_TYPE
      uEddsIntel;  // Intel Board specific (Hartwell, Springville)
#endif
#if ((PSI_CFG_USE_EDDS == 1) && \
     (defined EDDS_CFG_HW_KSZ88XX))  // Micrel specific
  EPS_EDDS_KSZ88XX_LL_TYPE uEddsKSZ88XX;
#endif
} EPS_PNDEVDRV_BOARD_TYPE, *EPS_PNDEVDRV_BOARD_PTR_TYPE;

typedef struct eps_pndevdrv_poll_tag {
  LSA_BOOL bRunning;
  LSA_UINT32 uCntUsers;
  LSA_UINT32 hThread;
  EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc[EPS_CFG_PNDEVDRV_MAX_BOARDS];
  EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard[EPS_CFG_PNDEVDRV_MAX_BOARDS];
} EPS_PNDEVDRV_POLL_TYPE, *EPS_PNDEVDRV_POLL_PTR_TYPE;

typedef struct eps_pndevdrv_store_tag {
  LSA_BOOL bInit;
  LSA_UINT16 hEnterExit;
  EPS_PNDEVDRV_BOARD_TYPE board[EPS_CFG_PNDEVDRV_MAX_BOARDS];
  LSA_UINT32 uCntFoundDevices;
  uPNDEV_GET_DEVICE_INFO_OUT foundDevices[EPS_CFG_PNDEVDRV_MAX_BOARDS];
  struct {
    EPS_PNDEVDRV_POLL_TYPE PnGatheredIsr;
    EPS_PNDEVDRV_POLL_TYPE PnNrtIsr;
  } poll;
  uPNDEV_DRIVER_DESC* pDrvHandle;
} EPS_PNDEVDRV_STORE_TYPE, *EPS_PNDEVDRV_STORE_PTR_TYPE;

/* - Global Data
 * ----------------------------------------------------------------------------------
 */
EPS_PNDEVDRV_STORE_TYPE g_EpsPnDevDrv;
EPS_PNDEVDRV_STORE_PTR_TYPE g_pEpsPnDevDrv = LSA_NULL;

extern LSA_UINT32 __PCI_HOST_MEM_START__;
extern LSA_UINT32 __PCI_HOST_MEM_END__;

/**
 * converts an error string of the pndevdrv
 *
 * pndevdrv returns unicode strings including \n
 * we have to convert from unicode and delete the \n to be able to trace the
 * string correctly
 *
 * @param pIn	pointer to the original string
 * @param pOut	pointer to the converted string
 * @return
 */
LSA_VOID eps_convert_pndev_err_string(PNDEV_CHAR* pIn, LSA_CHAR* pOut) {
  UINT32 lLoop = 0;
  UINT32 lNewPos = 0;
  UINT32 lSizeIn = 0;

#if defined(PNDEV_UNICODE)
  fnConvertStringWcharToChar(pIn, pOut);
#else
  eps_strcpy(pOut, pIn);
#endif

  lSizeIn = eps_strlen(pOut);

  for (lLoop = 0; lLoop <= lSizeIn; lLoop++)
  // every char in input string
  {
    if (pOut[lLoop] == '\n')
    // char is new line
    {
      // do nothing because we want to remove it
    } else if (pOut[lLoop] == '\0')
    // char is \0
    {
      // write the \0 one position forward, to overwrite the new line
      pOut[lNewPos] = pOut[lLoop];
    } else
    // every other char
    {
      // copy to actual position and increment pos
      pOut[lNewPos] = pOut[lLoop];
      lNewPos++;
    }
  }
}

LSA_VOID eps_pndevdrv_alloc_dump_buffer(
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard,
    EPS_FATAL_STORE_DUMP_PTR_TYPE pDumpStoreInfo, LSA_UINT8* pHWPtr) {
  pDumpStoreInfo->lBoardType = pBoard->OpenDevice.uHwInfo.eBoard;
  pDumpStoreInfo->lAsicType = pBoard->OpenDevice.uHwInfo.eAsic;
  pDumpStoreInfo->lAsicTypeInfo = pBoard->OpenDevice.uHwInfo.eAsicDetail;
  pDumpStoreInfo->pDumpBuf = (LSA_UINT8*)eps_mem_alloc(
      pDumpStoreInfo->lDumpSize, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_LOCAL_MEM);

  if (pDumpStoreInfo->pDumpBuf == 0) {
    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,
                        "EPS PnDevDrv: eps_pndevdrv_alloc_dump_buffer() gets "
                        "0-ptr at DumpBuf-allocation!!!, Allocated Size(%u)",
                        pDumpStoreInfo->lDumpSize);
    EPS_FATAL(
        "No memory for dump-buffer in eps_pndevdrv_alloc_dump_buffer()!!!");
  }

  eps_memcpy(pDumpStoreInfo->pDumpBuf, pHWPtr, pDumpStoreInfo->lDumpSize);
  EPS_APP_SAVE_DUMP_FILE(pDumpStoreInfo);
  eps_mem_free(pDumpStoreInfo->pDumpBuf, LSA_COMP_ID_PNBOARDS,
               PSI_MTYPE_LOCAL_MEM);
}

LSA_UINT16 eps_pndevdrv_save_dump(EPS_PNDEV_HW_PTR_TYPE pHwInstIn) {
  LSA_UINT16 result = EPS_PNDEV_RET_OK;
  EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
  EPS_FATAL_STORE_DUMP_PTR_TYPE pStoreDumpInfo = LSA_NULL;
  EPS_FATAL_STORE_DUMP_TYPE StoreDumpInfo;

  eps_pndevdrv_enter();

  pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;
  pStoreDumpInfo = &StoreDumpInfo;

  switch (pBoard->OpenDevice.uHwInfo.eAsic) {
    case ePNDEV_ASIC_ERTEC400:
    case ePNDEV_ASIC_ERTEC200:
    case ePNDEV_ASIC_SOC1:
      // copy dump of eddi irte to local buffer
      StoreDumpInfo.lDumpSize = pBoard->sHw.EpsBoardInfo.eddi.irte.size;
      StoreDumpInfo.lMemRamType = EPS_RAM_MEM_IRE;
      eps_pndevdrv_alloc_dump_buffer(
          pBoard, pStoreDumpInfo, pBoard->sHw.EpsBoardInfo.eddi.irte.base_ptr);
      break;

    case ePNDEV_ASIC_ERTEC200P:
      // copy dump of eddp pnip to local buffer
      StoreDumpInfo.lDumpSize = pBoard->sHw.EpsBoardInfo.eddp.pnip.size;
      StoreDumpInfo.lMemRamType = EPS_RAM_MEM_PNIP;
      eps_pndevdrv_alloc_dump_buffer(
          pBoard, pStoreDumpInfo, pBoard->sHw.EpsBoardInfo.eddp.pnip.base_ptr);

      StoreDumpInfo.lDumpSize = pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.size +
                                pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.size;
      StoreDumpInfo.lMemRamType = EPS_RAM_MEM_SDRAM;
      eps_pndevdrv_alloc_dump_buffer(
          pBoard, pStoreDumpInfo,
          pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.base_ptr);

      StoreDumpInfo.lDumpSize = pBoard->sHw.EpsBoardInfo.eddp.perif_ram.size;
      StoreDumpInfo.lMemRamType = EPS_RAM_MEM_PERIF_RAM;
      eps_pndevdrv_alloc_dump_buffer(
          pBoard, pStoreDumpInfo,
          pBoard->sHw.EpsBoardInfo.eddp.perif_ram.base_ptr);
      break;

    case ePNDEV_ASIC_I210:
    case ePNDEV_ASIC_I82574:
    case ePNDEV_ASIC_KSZ8841:
    case ePNDEV_ASIC_KSZ8842:
    case ePNDEV_ASIC_INVALID:
      break;

    default:
      break;
  }

  eps_pndevdrv_exit();

  return result;
}

/**
 * show fatal dump in AsicSdram (FirmwareFatal of PnCoreInterface)
 *
 * @param pBoard	pointer to current board structure
 * @return
 */
static LSA_VOID eps_pndevdrv_show_fatal_dump(
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard) {
  ePNDEV_RESULT drvResult;
  EPS_SHM_FATAL_STORE_PTR_TYPE pFatal;
  LSA_UINT8
      BufOut[sizeof(EPS_SHM_FATAL_STORE_TYPE) + sizeof(uPNDEV_COPY_DATA_OUT)];
  uPNDEV_COPY_DATA_OUT* pCopyDataOut = LSA_NULL;
  uPNDEV_COPY_DATA_IN copyDataIn;

  EPS_ASSERT(pBoard->bUsed == LSA_TRUE);

  pCopyDataOut = (uPNDEV_COPY_DATA_OUT*)BufOut;

  eps_memset(&copyDataIn, 0, sizeof(copyDataIn));

  switch (pBoard->OpenDevice.uHwInfo.eAsic) {
    case ePNDEV_ASIC_ERTEC200:
      copyDataIn.uArrayBlockRead[0].lOffsetAsic =
          pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwFatal.lOffset;
      break;
    case ePNDEV_ASIC_ERTEC200P:
      copyDataIn.uArrayBlockRead[0].lOffsetAsic =
          pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwFatal.lOffset;
      break;
    case ePNDEV_ASIC_ERTEC400:
      copyDataIn.uArrayBlockRead[0].lOffsetAsic =
          pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwFatal.lOffset;
      break;
    case ePNDEV_ASIC_SOC1:
      copyDataIn.uArrayBlockRead[0].lOffsetAsic =
          pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwFatal.lOffset;
      break;
    default:
      EPS_FATAL("Not supported ASIC found.");
  }

  copyDataIn.eHwRes = ePNDEV_COPY_HW_RES_SDRAM;
  copyDataIn.uDevice.hHandle = pBoard->OpenDevice.uDevice.hHandle;

  copyDataIn.uArrayBlockRead[0].lOffsetHostDataBuf = 0;
  copyDataIn.uArrayBlockRead[0].lSize = sizeof(EPS_SHM_FATAL_STORE_TYPE);

  drvResult = g_pEpsPnDevDrv->pDrvHandle->uIf.pFnCopyData(
      g_pEpsPnDevDrv->pDrvHandle, sizeof(copyDataIn), sizeof(BufOut),
      &copyDataIn, pCopyDataOut);

  EPS_ASSERT(drvResult == ePNDEV_OK);

  pFatal =
      (EPS_SHM_FATAL_STORE_PTR_TYPE)(BufOut + sizeof(uPNDEV_COPY_DATA_OUT));

  if (pFatal->bUsed != 0) {
    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL,
                        "EPS PnDevDrv: !!! EPS FATAL ON HD FW (hd_id=%d) !!!",
                        pBoard->sysDev.hd_nr);

    if (pFatal->uErrCode == EPS_SHM_ERR_CODE_FATAL) {
      EPS_SYSTEM_TRACE_02(
          0, LSA_TRACE_LEVEL_FATAL,
          "EPS PnDevDrv: !!! EPS FATAL ON HD FW (hd_id=%d) !!! => Line(%d)",
          pBoard->sysDev.hd_nr, pFatal->uLine);
      EPS_SYSTEM_TRACE_STRING(
          0, LSA_TRACE_LEVEL_FATAL,
          "EPS PnDevDrv: !!! EPS FATAL ON HD FW !!! => File %s", pFatal->sFile);
      EPS_SYSTEM_TRACE_STRING(
          0, LSA_TRACE_LEVEL_FATAL,
          "EPS PnDevDrv: !!! EPS FATAL ON HD FW !!! => Msg %s", pFatal->sMsg);
    }

    if (pFatal->uErrCode == EPS_SHM_ERR_CODE_FATAL_LSA) {
      EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL,
                              "!!! EPS LSA FATAL ON HD FW !!! => Comp %s",
                              pFatal->sComp);
      EPS_SYSTEM_TRACE_12(
          0, LSA_TRACE_LEVEL_FATAL,
          "!!! EPS LSA FATAL ON HD FW (hd_id=%d) !!! => CompId(%d) "
          "ModuleId(%d) Line(%d) ErrCode([0]=%d / 0x%x,[1]= %d / 0x%x,[2]= %d "
          "/ 0x%x,[3]= %d / 0x%x)",
          (int)pBoard->sysDev.hd_nr, (int)pFatal->lsa_fatal.lsa_component_id,
          (int)pFatal->lsa_fatal.module_id, (int)pFatal->lsa_fatal.line,
          (int)pFatal->lsa_fatal.error_code[0], pFatal->lsa_fatal.error_code[0],
          (int)pFatal->lsa_fatal.error_code[1], pFatal->lsa_fatal.error_code[1],
          pFatal->lsa_fatal.error_code[2], pFatal->lsa_fatal.error_code[2],
          (int)pFatal->lsa_fatal.error_code[3],
          pFatal->lsa_fatal.error_code[3]);
    }

#ifdef EPS_PNDEVDRV_PRINTF_DRIVER_INFO
    printf("\r\n\r\n");

    printf("!!! EPS FATAL ON HD FW (hd_id=%d) !!!", (int)pBoard->sysDev.hd_nr);

    if (pFatal->uErrCode == EPS_SHM_ERR_CODE_FATAL) {
      printf(" => File(%s) Line(%d) Msg(%s)", pFatal->sFile, (int)pFatal->uLine,
             pFatal->sMsg);
      eps_tasks_sleep(10); /* Give it some time to print out */
    }

    if (pFatal->uErrCode == EPS_SHM_ERR_CODE_FATAL_LSA) {
      printf(
          " => Comp(%s) CompId(%d) ModuleId(%d) Line(%d) ErrCode([0]=%d / "
          "0x%x,[1]= %d / 0x%x,[2]= %d / 0x%x,[3]= %d / 0x%x)",
          pFatal->sComp, (int)pFatal->lsa_fatal.lsa_component_id,
          (int)pFatal->lsa_fatal.module_id, (int)pFatal->lsa_fatal.line,
          (int)pFatal->lsa_fatal.error_code[0], pFatal->lsa_fatal.error_code[0],
          (int)pFatal->lsa_fatal.error_code[1], pFatal->lsa_fatal.error_code[1],
          pFatal->lsa_fatal.error_code[2], pFatal->lsa_fatal.error_code[2],
          (int)pFatal->lsa_fatal.error_code[3],
          pFatal->lsa_fatal.error_code[3]);
      eps_tasks_sleep(10); /* Give it some time to print out */
    }
    printf("\r\n\r\n");
    eps_tasks_sleep(20); /* Give it some time to print out */
#endif
  }
}

/**
 * Callbackfunction for event handling of PnDevDriver
 *
 * @param hDeviceIn		Handle of Device
 * @param eEventClassIn	Enume with different DeviceClasses
 * @param sEventIn		string with cause of event
 * @param pCbfParUserIn pointer to user defined parameter
 * @return
 */
void __stdcall eps_pndevdrv_event_cbf(const HANDLE hDeviceIn,
                                      const ePNDEV_EVENT_CLASS eEventClassIn,
                                      PNDEV_CHAR* sEventIn,
                                      const void* pCbfParUserIn) {
  EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard =
      (EPS_PNDEVDRV_BOARD_PTR_TYPE)pCbfParUserIn;

#ifdef EPS_PNDEVDRV_PRINTF_EVENTS

  LSA_UNUSED_ARG(hDeviceIn);

  printf("\r\nReceived PnDevDrv Event for %s:\r\n",
         (LSA_CHAR*)pBoard->OpenDevice.sPciLoc);
  printf("    ");
  switch (eEventClassIn) {
    case ePNDEV_EVENT_CLASS_INVALID:
      printf("Invalid:       ");
      break;
    case ePNDEV_EVENT_CLASS_TEST:
      printf("Test:          ");
      break;
    case ePNDEV_EVENT_CLASS_FATAL_FW:
      printf("Fatal FW:      ");
      break;
    case ePNDEV_EVENT_CLASS_FATAL_ASYNC:
      printf("Fatal Async:   ");
      break;
    case ePNDEV_EVENT_CLASS_WARNING:
      printf("Warning:       ");
      break;
    case ePNDEV_EVENT_CLASS_REQ_CLOSE:
      printf("Req Close:     ");
      break;
    case ePNDEV_EVENT_CLASS_DEBUG:
      printf("Debug:         ");
      break;
    default:
      printf("Unknown Event: ");
      break;
  }
  printf("%s", (LSA_CHAR*)sEventIn);
  printf("\r\n");

#else
  LSA_UNUSED_ARG(sEventIn);
  LSA_UNUSED_ARG(hDeviceIn);
#endif

  if (eEventClassIn == ePNDEV_EVENT_CLASS_FATAL_FW) {
    eps_pndevdrv_show_fatal_dump(pBoard);
    EPS_FATAL(
        "eps_pndevdrv_event_cbf - FATAL ERROR in Basic FW, please check the "
        "logfile!");
  }
}

/**
 * Callbackfunction for interrupt handling of PnDevDriver
 *
 * @param hDeviceIn		Handle of Device
 * @param bDeviceTestIn	indicates if it is a interrupt for testing purpose
 * @param eIntSrcIn		Enume with different DeviceClasses
 * @param pCbfParUserIn pointer to user defined parameter
 * @return
 */
void __stdcall eps_pndevdrv_isr_cbf(const HANDLE hDeviceIn,
                                    const BOOLEAN bDeviceTestIn,
                                    const ePNDEV_INT_SRC eIntSrcIn,
                                    const void* pCbfParUserIn) {
  EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard =
      (EPS_PNDEVDRV_BOARD_PTR_TYPE)pCbfParUserIn;

  UNREFERENCED_PARAMETER(hDeviceIn);

  // Attention:
  //	- it is not allowed to call any service of PnDev_Driver here!

  if (bDeviceTestIn) {
    switch (eIntSrcIn) {
      case ePNDEV_INT_SRC_TEST: {
#ifdef EPS_PNDEVDRV_PRINTF_ISR
        printf("\r\nReceived PnDevDrv Isr for %s: ePNDEV_INT_SRC_TEST\r\n",
               (LSA_CHAR*)pBoard->OpenDevice.sPciLoc);
#endif
        break;
      }
      default: {
        /* Unexpected */
        EPS_FATAL("Unknown interrupt src in pndevdrv isr test mode.");
        break;
      }
    }
  } else  // StandardMode
  {
    EPS_ASSERT(pBoard != LSA_NULL);
    EPS_ASSERT(pBoard->bUsed == LSA_TRUE);

    switch (pBoard->sHw.asic_type) {
#if (PSI_CFG_USE_EDDP == 1)
      case EPS_PNDEV_ASIC_ERTEC200P: {
        if (eIntSrcIn == ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3) {
          if (pBoard->sIsrIsochronous.pCbf != LSA_NULL) {
            pBoard->sIsrIsochronous.pCbf(pBoard->sIsrIsochronous.uParam,
                                         pBoard->sIsrIsochronous.pArgs);

            eps_hw_e200p_ack_isochronous_interrupt(
                pBoard->sHw.EpsBoardInfo.eddp.pnip.base_ptr, LSA_FALSE);
          } else {
#ifdef EPS_PNDEVDRV_PRINTF_ISR
            printf(
                "\r\nReceived PnDevDrv Isr for %s: "
                "ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3\r\n",
                (LSA_CHAR*)pBoard->OpenDevice.sPciLoc);
            printf(
                "  Error: Isochronous Interrupt while Isochronous Cbf not "
                "registered!\r\n");
#endif
          }
        } else {
#ifdef EPS_PNDEVDRV_PRINTF_ISR
          printf("\r\nReceived PnDevDrv Isr for %s: (%d)\r\n",
                 (LSA_CHAR*)pBoard->OpenDevice.sPciLoc, (int)eIntSrcIn);
          printf("  Error: Unknown not supported Interrupt!\r\n");
#endif
        }

        break;
      }
#endif
      default: {
        /* Unsupported Now */
        EPS_FATAL("Not supported asic for pndevdrv interrupt");

        break;
      }
    }
  }
}

/**
 * undo init critical section
 *
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_pndevdrv_undo_init_critical_section(LSA_VOID) {
  eps_free_critical_section(g_pEpsPnDevDrv->hEnterExit);
}

/**
 * init critical section
 *
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_pndevdrv_init_critical_section(LSA_VOID) {
  eps_alloc_critical_section(&g_pEpsPnDevDrv->hEnterExit, LSA_FALSE);
}

/**
 * enter critical section
 *
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_pndevdrv_enter(LSA_VOID) {
  eps_enter_critical_section(g_pEpsPnDevDrv->hEnterExit);
}

/**
 * leave critical section
 *
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_pndevdrv_exit(LSA_VOID) {
  eps_exit_critical_section(g_pEpsPnDevDrv->hEnterExit);
}

/**
 * alloc board entry
 * - create new board
 * - initialize board structure
 *
 * @param LSA_VOID
 * @return pBoard
 */
static EPS_PNDEVDRV_BOARD_PTR_TYPE eps_pndevdrv_alloc_board(LSA_VOID) {
  LSA_UINT32 i;
  EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;

  for (i = 0; i < EPS_CFG_PNDEVDRV_MAX_BOARDS; i++) {
    pBoard = &g_pEpsPnDevDrv->board[i];

    if (pBoard->bUsed == LSA_FALSE) {
      pBoard->sHw.hDevice = (LSA_VOID*)pBoard;
      pBoard->sHw.EnableIsr = eps_pndevdrv_enable_interrupt;
      pBoard->sHw.DisableIsr = eps_pndevdrv_disable_interrupt;
      pBoard->sHw.SetGpio = eps_pndevdrv_set_gpio;
      pBoard->sHw.ClearGpio = eps_pndevdrv_clear_gpio;
      pBoard->sHw.TimerCtrlStart = eps_pndevdrv_timer_ctrl_start;
      pBoard->sHw.TimerCtrlStop = eps_pndevdrv_timer_ctrl_stop;
      pBoard->sHw.ReadTraceData = eps_pndevdrv_read_trace_data;
      pBoard->sHw.WriteTraceData = eps_pndevdrv_write_trace_data;
      pBoard->sHw.SaveDump = eps_pndevdrv_save_dump;
      pBoard->pBackRef = g_pEpsPnDevDrv;
      pBoard->bUsed = LSA_TRUE;

      return pBoard;
    }
  }

  return pBoard;
}

/**
 * free board entry
 *
 * @param pBoard	ptr to current board structure
 * @return LSA_VOID
 */
static LSA_VOID eps_pndevdrv_free_board(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard) {
  EPS_ASSERT(pBoard != LSA_NULL);

  EPS_ASSERT(pBoard->bUsed == LSA_TRUE);

  // UT5507 - Memset to Zero of pBoard-Data
  eps_memset(pBoard, 0, sizeof(EPS_PNDEVDRV_BOARD_TYPE));
  pBoard->bUsed = LSA_FALSE;
}

/**
 * convert string to integer
 *
 * @param pStr			ptr to first charackter of string
 * @param pNextChar		ptr to next charachter of string
 * @param pbWCharStr	Width of every single string element
 * @return converted integer
 */
static LSA_INT32 eps_pndevdrv_str_to_int(LSA_CHAR* pStr, LSA_UINT32* pNextChar,
                                         LSA_BOOL bWChar) {
  LSA_INT32 i = 0, e, mul = 1, f = 1;
  LSA_INT32 nr = 0;

  if (bWChar == LSA_TRUE) f = 2;

  if (pStr == LSA_NULL) return -1; /* No String */

  while ((pStr[i] != 0) &&
         ((pStr[i] < '0') || (pStr[i] > '9'))) /* Find Start of Number*/
  {
    i += f;
  }

  if (pStr[i] == 0) return -1; /* End of String => No Number */

  e = i;

  while ((pStr[e] >= '0') && (pStr[e] <= '9')) /* Find End of Number */
  {
    e += f;
  }

  if (pNextChar != 0) *pNextChar = e;

  e -= f;

  while (e >= i) /* Convert String to int */
  {
    nr += (pStr[e] - 48) * mul;
    mul *= 10;
    e -= f;
  }

  return nr;
}

/**
 * parse pci location, which are included in input string
 *
 * @param pStr			ptr to first charackter of string
 * @param pBusNr		ptr containing computed BusNr
 * @param pDeviceNr		ptr containing computed DeviceNr
 * @param pFunctionNr	ptr containing computed FunctionNr
 * @return
 */
static LSA_VOID eps_pndevdrv_parse_location_string(LSA_CHAR* pStr,
                                                   LSA_INT* pBusNr,
                                                   LSA_INT* pDeviceNr,
                                                   LSA_INT* pFunctionNr) {
  LSA_UINT32 e = 0, ealt;
  LSA_BOOL bWChar = LSA_FALSE;

  EPS_ASSERT(pStr != LSA_NULL);
  EPS_ASSERT(pBusNr != LSA_NULL);
  EPS_ASSERT(pDeviceNr != LSA_NULL);
  EPS_ASSERT(pFunctionNr != LSA_NULL);

  if (pStr[1] == 0) bWChar = LSA_TRUE;

  /* Get Bus Nr */
  *pBusNr = eps_pndevdrv_str_to_int(pStr, &e, bWChar);
  EPS_ASSERT(*pBusNr >= 0);
  EPS_ASSERT(*pBusNr < 256);
  EPS_ASSERT(pStr[e] == ',');
  ealt = e;

  /* Get Device Nr */
  *pDeviceNr = eps_pndevdrv_str_to_int(&pStr[ealt], &e, bWChar);
  ealt += e;
  EPS_ASSERT(*pDeviceNr >= 0);
  EPS_ASSERT(pStr[ealt] == ',');

  /* Get Function Nr */
  *pFunctionNr = eps_pndevdrv_str_to_int(&pStr[ealt], &e, bWChar);
  ealt += e;
  EPS_ASSERT(*pFunctionNr >= 0);
  EPS_ASSERT(pStr[ealt] == 0);
}

#ifdef EPS_PNDEVDRV_PRINTF_FOUND_BOARDS
/**
 * print a list of boards, found by the PnDevDriver
 *
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_pndevdrv_printf_found_boards(LSA_VOID) {
  LSA_UINT32 i;
  LSA_INT uBusNr, uDeviceNr, uFctNr;
  uPNDEV_GET_DEVICE_INFO_OUT* pDevice;

  printf("\r\nEPS PnDevDrv Found PN Boards:\r\n");
  printf("----------------------------------------");

  for (i = 0; i < g_pEpsPnDevDrv->uCntFoundDevices; i++) {
    pDevice = &g_pEpsPnDevDrv->foundDevices[i];

    // printf("\r\n%s  -  ",(LSA_CHAR*)pDevice->sPciLoc); /* Commented because
    // of wchar problem */
    eps_pndevdrv_parse_location_string((LSA_CHAR*)pDevice->sPciLoc, &uBusNr,
                                       &uDeviceNr, &uFctNr);
    printf("\r\nBus %2d, Dev %2d, Fct %2d  -  ", uBusNr, uDeviceNr, uFctNr);

    switch (pDevice->eBoard) {
      case ePNDEV_BOARD_INVALID:
        printf("Invalid Board              ");
        break;
      case ePNDEV_BOARD_CP1616:  // CP1616 board
        printf("CP1616 Ertec400            ");
        break;
      case ePNDEV_BOARD_DB_EB400_PCIE:  // DB-EB400-PCIe board
        printf("EB400_PCIe Ertec400        ");
        break;
      case ePNDEV_BOARD_EB200:  // EB200 board
        printf("EB200 Ertec200             ");
        break;
      case ePNDEV_BOARD_DB_SOC1_PCI:  // Soc1 PCI Dev board
        printf("Soc1 PCI Dev Board Rev7    ");
        break;
      case ePNDEV_BOARD_DB_SOC1_PCIE:  // Soc1 PCIe Dev board
        printf("Soc1 PCIe Dev Board Rev7   ");
        break;
      case ePNDEV_BOARD_FPGA1_ERTEC200P:  // FPGA1-Ertec200P board
        printf("FPGA Ertec200P Rev1        ");
        break;
      case ePNDEV_BOARD_EB200P:  // EB200P board
        printf("EB200P Ertec200P Rev1      ");
        break;
      case ePNDEV_BOARD_I210:  // I210 board
        printf("SPVL I210				   ");
        break;
      case ePNDEV_BOARD_I82574:  // I82574 board
        printf("Hartwell I82574            ");
        break;
      case ePNDEV_BOARD_MICREL_KSZ8841:  // MICREL KSZ8841 board
        printf("MICREL KSZ8841             ");
        break;
      case ePNDEV_BOARD_MICREL_KSZ8842:  // MICREL KSZ8842 board
        printf("MICREL KSZ8842             ");
        break;
      default:
        printf("EPS Unknown Board          ");
        break;
    }
  }
  printf("\r\n----------------------------------------\r\n");
}
#endif

#ifdef EPS_PNDEVDRV_PRINTF_OPEN_CLOSE_INFO
/**
 * print information, once a board has been closed or opened
 *
 * @param bOpen		True, if it was opened, FALSE if it was closed
 * @param pBoard	ptr to current board structure
 * @return
 */
static LSA_VOID eps_pndevdrv_printf_openclose_board(
    LSA_BOOL bOpen, EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard) {
  if (bOpen == LSA_TRUE) {
    // printf("\r\nEPS PnDevDrv Board Opened %s:\r\n",
    // (LSA_CHAR*)pBoard->OpenDevice.sPciLoc); /* Commented because of wchar
    // problem */
    printf(
        "\r\nEPS PnDevDrv Board Opened PCI Bus %d, Dev %d, Fct %d (using FW "
        "(%d), SBL %d.%d.%d, Cnt Mac: %d)\r\n",
        (int)pBoard->Location.uBusNr, (int)pBoard->Location.uDeviceNr,
        (int)pBoard->Location.uFctNr, (int)pBoard->bBoardFwUsed,
        (int)pBoard->OpenDevice.uSblInfo.uVersion.lMajor,
        (int)pBoard->OpenDevice.uSblInfo.uVersion.lMinor,
        (int)pBoard->OpenDevice.uSblInfo.uVersion.lBuild,
        (int)pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lCtrMacAdr);
  } else {
    // printf("\r\nEPS PnDevDrv Board Closed %s:\r\n",
    // (LSA_CHAR*)pBoard->OpenDevice.sPciLoc); /* Commented because of wchar
    // problem */
    printf("\r\nEPS PnDevDrv Board Closed PCI Bus %d, Dev %d, Fct %d\r\n",
           (int)pBoard->Location.uBusNr, (int)pBoard->Location.uDeviceNr,
           (int)pBoard->Location.uFctNr);
  }
}
#endif

/**
 * register device infos
 *
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pndevdrv_register_device_infos(LSA_VOID) {
  LSA_UINT16 i;
  EPS_PNDEV_IF_DEV_INFO_TYPE sPnDevInfo;
  uPNDEV_GET_DEVICE_INFO_OUT* pDevice;
  LSA_INT uBusNr, uDeviceNr, uFuncNr;

  for (i = 0; i < g_pEpsPnDevDrv->uCntFoundDevices; i++) {
    pDevice = &g_pEpsPnDevDrv->foundDevices[i];

    eps_pndevdrv_parse_location_string((LSA_CHAR*)pDevice->sPciLoc, &uBusNr,
                                       &uDeviceNr, &uFuncNr);

    sPnDevInfo.uPciAddrValid = 1;
    sPnDevInfo.uPciBusNr = (LSA_UINT16)uBusNr;
    sPnDevInfo.uPciDeviceNr = (LSA_UINT16)uDeviceNr;
    sPnDevInfo.uPciFuncNr = (LSA_UINT16)uFuncNr;

    sPnDevInfo.uMacAddrValid = 0;

    switch (pDevice->eBoard) {
      case ePNDEV_BOARD_CP1616:         // CP1616 board
      case ePNDEV_BOARD_DB_EB400_PCIE:  // DB-EB400-PCIe board
      case ePNDEV_BOARD_EB200:          // EB200 board
      case ePNDEV_BOARD_DB_SOC1_PCI:    // Soc1-Dev board
      case ePNDEV_BOARD_DB_SOC1_PCIE:   // Soc1-Dev board
        sPnDevInfo.uEddType = LSA_COMP_ID_EDDI;
        break;
      case ePNDEV_BOARD_FPGA1_ERTEC200P:  // FPGA1-Ertec200P board
      case ePNDEV_BOARD_EB200P:           // EB200P board
        sPnDevInfo.uEddType = LSA_COMP_ID_EDDP;
        break;
      case ePNDEV_BOARD_I210:            // I210 board
      case ePNDEV_BOARD_I82574:          // I82574 board
      case ePNDEV_BOARD_MICREL_KSZ8841:  // MICREL KSZ8841 board
      case ePNDEV_BOARD_MICREL_KSZ8842:  // MICREL KSZ8842 board
        sPnDevInfo.uEddType = LSA_COMP_ID_EDDS;
        break;
      default:
        EPS_FATAL("Can't register a not supported board.");
        break;
    }

    sPnDevInfo.eBoard = pDevice->eBoard;

    eps_pndev_if_register_device(&sPnDevInfo);
  }
}

/**
 * call Service GetDeviceInfo of PnDevDriver and store Information in global
 * DeviceStructure (g_pEpsPnDevDrv)
 *
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_pndevdrv_get_device_infos(LSA_VOID) {
  ePNDEV_RESULT retVal;
  uPNDEV_GET_DEVICE_INFO_IN uGetDeviceInfoIn;

  PNDEV_CHAR sError[PNDEV_SIZE_STRING_BUF] = {0};

  eps_memset(&uGetDeviceInfoIn, 0, sizeof(uPNDEV_GET_DEVICE_INFO_IN));
  eps_memset(g_pEpsPnDevDrv->foundDevices, 0,
             sizeof(g_pEpsPnDevDrv->foundDevices));

  // Get DeviceInfo of all devices
  retVal = g_pEpsPnDevDrv->pDrvHandle->uIf.pFnGetDeviceInfo(
      g_pEpsPnDevDrv->pDrvHandle, sizeof(uPNDEV_GET_DEVICE_INFO_IN),
      (sizeof(uPNDEV_GET_DEVICE_INFO_OUT) * EPS_CFG_PNDEVDRV_MAX_BOARDS),
      EPS_CFG_PNDEVDRV_MAX_BOARDS, _countof(sError), &uGetDeviceInfoIn,
      g_pEpsPnDevDrv->foundDevices, (UINT32*)&g_pEpsPnDevDrv->uCntFoundDevices,
      sError);

#ifdef EPS_PNDEVDRV_PRINTF_FOUND_BOARDS
  eps_pndevdrv_printf_found_boards();
#endif

  /* OK or no devices found! */
  if ((retVal != ePNDEV_OK) && (retVal != ePNDEV_ERROR_NOT_FOUND)) {
    LSA_CHAR sErrConv[PNDEV_SIZE_STRING_BUF] = {0};
    eps_convert_pndev_err_string(sError, sErrConv);
    EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR, "EPS PnDevDrv %s",
                            sErrConv);

#ifdef EPS_PNDEVDRV_PRINTF_DRIVER_INFO
    printf("\r\n%s ln(%d): %s\r\n", (LSA_CHAR*)__FILE__, __LINE__,
           (LSA_CHAR*)sError);
#endif

    EPS_FATAL("Error getting device information via pndevdrv.");
  }
}

/**
 * look up device in global DeviceStructure (g_pEpsPnDevDrv)
 *
 * @param pLocation 	ptr including PciLocation
 * @param ppDecive		ptr including ptr to corresponding element in
 * DeviceStructure
 * @return 				ePNDEV_OK or ePNDEV_ERROR_NOT_FOUND, if board not
 * found
 */
static LSA_UINT32 eps_pndevdrv_get_device(
    EPS_PNDEV_LOCATION_PTR_TYPE pLocation,
    uPNDEV_GET_DEVICE_INFO_OUT** ppDevice) {
  LSA_UINT32 i;
  ePNDEV_RESULT drvResult = ePNDEV_ERROR_NOT_FOUND;
  uPNDEV_COMPARE_PCI_LOC_IN inComp;
  uPNDEV_COMPARE_PCI_LOC_OUT outComp;
  uPNDEV_GET_DEVICE_INFO_OUT* pDevice;

#if (EPS_PLF != EPS_PLF_WINDOWS_X86)
  LSA_INT uBusNr, uDeviceNr, uFuncNr;
#endif

  eps_memset(&inComp, 0, sizeof(inComp));
  eps_memset(&outComp, 0, sizeof(outComp));

  inComp.lPciBusNo = pLocation->uBusNr;
  inComp.lPciDeviceNo = pLocation->uDeviceNr;
  inComp.lPciFunctionNo = pLocation->uFctNr;

  for (i = 0; i < g_pEpsPnDevDrv->uCntFoundDevices; i++) {
    pDevice = &g_pEpsPnDevDrv->foundDevices[i];

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    // driver strings have w_char size
    eps_memcpy(inComp.sPciLoc, g_pEpsPnDevDrv->foundDevices[i].sPciLoc,
               sizeof(inComp.sPciLoc));
#else
    eps_memcpy(inComp.sPciLoc, g_pEpsPnDevDrv->foundDevices[i].sPciLoc,
               PNDEV_SIZE_STRING_BUF_SHORT);

    eps_pndevdrv_parse_location_string(
        (LSA_CHAR*)pDevice->sPciLoc, &uBusNr, &uDeviceNr,
        &uFuncNr); /* (LaM) Workaround for uIf.pFnComparePciLoc Bug */
    if ((pLocation->uBusNr == (LSA_UINT32)uBusNr) &&
        (pLocation->uDeviceNr == (LSA_UINT32)uDeviceNr) &&
        (pLocation->uFctNr == (LSA_UINT32)uFuncNr)) {
      *ppDevice = pDevice;
      break;
    }
#endif

    drvResult = g_pEpsPnDevDrv->pDrvHandle->uIf.pFnComparePciLoc(
        g_pEpsPnDevDrv->pDrvHandle, sizeof(inComp), sizeof(outComp), &inComp,
        &outComp);

    if (drvResult == ePNDEV_OK)  // Location matches
    {
      *ppDevice = pDevice;
      break;
    }
  }

  return i;
}

/**
 * set parameters for hif configuration
 *
 * @param pLocation 	ptr including PciLocation
 * @param ppDecive		ptr including ptr to corresponding element in
 * DeviceStructure
 * @return 				ePNDEV_OK or ePNDEV_ERROR_NOT_FOUND, if board not
 * found
 */
static LSA_VOID eps_pndevdrv_open_device_setup_cfg_record(
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, LSA_BOOL bSetupEmpty,
    LSA_UINT16 hd_id) {
#if ((EPS_PLF != EPS_PLF_WINDOWS_X86) && (EPS_PLF != EPS_PLF_LINUX_X86))

  eps_memset(&pBoard->SharedMemConfig, 0, sizeof(pBoard->SharedMemConfig));

  pBoard->SharedMemConfig.bValid = 0;

  // No valid HIF setup required (e.g. I210)
  if (bSetupEmpty == LSA_TRUE) {
    return;
  }

  switch (pBoard->pDeviceInfo->eBoard) {
    case ePNDEV_BOARD_CP1616:
    case ePNDEV_BOARD_DB_EB400_PCIE: {
      pBoard->SharedMemConfig.HIF.HifCfg.bUseSharedMemMode = LSA_TRUE;
      pBoard->SharedMemConfig.HIF.HifCfg.bUseRemoteMem = LSA_TRUE;
      pBoard->SharedMemConfig.uCfgLength = sizeof(pBoard->SharedMemConfig);
      pBoard->SharedMemConfig.HIF.bValid = 1;
      pBoard->SharedMemConfig.bValid = 1;
      break;
    }
    case ePNDEV_BOARD_EB200: {
      pBoard->SharedMemConfig.bValid = 0; /* (LaM) not supported now */
      break;
    }
    case ePNDEV_BOARD_DB_SOC1_PCI:
    case ePNDEV_BOARD_DB_SOC1_PCIE: {
      pBoard->SharedMemConfig.HIF.HifCfg.bUseSharedMemMode = LSA_TRUE;
      pBoard->SharedMemConfig.HIF.HifCfg.bUseRemoteMem = LSA_TRUE;
      pBoard->SharedMemConfig.HIF.HifCfg.bUseIpcInterrupts = LSA_TRUE;
      //			pBoard->SharedMemConfig.HIF.HifCfg.bUseDma
      //= LSA_TRUE;
      pBoard->SharedMemConfig.uCfgLength = sizeof(pBoard->SharedMemConfig);
      pBoard->SharedMemConfig.HIF.bValid = 1;
      pBoard->SharedMemConfig.bValid = 1;
      break;
    }
    case ePNDEV_BOARD_FPGA1_ERTEC200P: {
      pBoard->SharedMemConfig.bValid = 0; /* (LaM) not supported now */
      break;
    }
    case ePNDEV_BOARD_EB200P: {
      pBoard->SharedMemConfig.bValid = 0; /* (LaM) not supported now */
      break;
    }
    default: {
      EPS_FATAL(0); /* Board not supported */
      break;
    }
  }

  /* if shared mem config is not used, HIF parameters get set up by
   * eps_pndevdrv_open_hif_driver() */

  if (pBoard->SharedMemConfig.bValid == 1) {
    if (pBoard->SharedMemConfig.HIF.bValid == 1) {
      if (pBoard->SharedMemConfig.HIF.HifCfg.bUseIpcInterrupts == LSA_TRUE) {
        LSA_UINT16 retVal;

        retVal = eps_ipc_rx_alloc(&pBoard->Hif.Ipc.Rx.uRxHandle,
                                  &pBoard->SharedMemConfig.HIF.IPC.uPhysAddress,
                                  &pBoard->SharedMemConfig.HIF.IPC.uMsgValue);
        EPS_ASSERT(retVal == EPS_IPC_RET_OK);
      }
      if (pBoard->SharedMemConfig.HIF.HifCfg.bUseSharedMemMode == LSA_TRUE) {
        if (pBoard->SharedMemConfig.HIF.HifCfg.bUseRemoteMem == LSA_TRUE) {
          LSA_UINT32 uPciHostMemHifSize = 0x00200000;

          pBoard->SharedMemConfig.HIF.HostRam.uSize = uPciHostMemHifSize;
        }
      }
    }
  }
#else
  LSA_UNUSED_ARG(hd_id);
  LSA_UNUSED_ARG(bSetupEmpty);
  LSA_UNUSED_ARG(pBoard);
#endif
}

/**
 * look up device in global DeviceStructure (g_pEpsPnDevDrv)
 *
 * @param pBoard 			ptr to corresponding board structure
 * @param pOption			ptr to options, if FirmwareDownload and
 * Debugging will be executed
 * @param puOpenDeviceOut	ptr to OpenDeviceOut parameter
 * @param hd_id				index of hd
 * @return 					ePNDEV_OK if successfull
 */
static ePNDEV_RESULT eps_pndevdrv_open_device(
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption,
    uPNDEV_OPEN_DEVICE_OUT* puOpenDeviceOut, LSA_UINT16 hd_id) {
  EPS_APP_FILE_TAG_TYPE file_tag_fw;
  EPS_APP_FILE_TAG_TYPE file_tag_bootloader;
  uPNDEV_OPEN_DEVICE_IN uOpenDeviceIn;
  ePNDEV_RESULT drvResult;

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  LSA_BOOL bMaster =
      LSA_TRUE;  // Using on windows level the Master APP access is always set
#else
  LSA_BOOL bMaster = LSA_FALSE; /* (LaM) depends on PNDEV_CONNECT_LEG_INT */
#endif

  eps_memset(&uOpenDeviceIn, 0, sizeof(uOpenDeviceIn));

  if (bMaster == LSA_TRUE) {
    uOpenDeviceIn.bMasterApp = LSA_TRUE;
    uOpenDeviceIn.uCbfIsr.pPtr = &eps_pndevdrv_isr_cbf;
  } else {
    uOpenDeviceIn.bMasterApp = LSA_FALSE;
    uOpenDeviceIn.uCbfIsr.pPtr = LSA_NULL;
  }

// bReqHostMem = LSA_TRUE NRT Mem is in host
// bReqHostMem = LSA_FALSE NRT Mem is at PNBoard
//
// TODO: Solution for SPVL
//
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  uOpenDeviceIn.bReqHostMem = FALSE;
#else
  if ((pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1616) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_EB400_PCIE) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCI) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCIE) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_I210) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_I82574) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_MICREL_KSZ8841) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_MICREL_KSZ8842))
  // valid board
  {
    // get HostMemory
    uOpenDeviceIn.bReqHostMem = TRUE;
  } else {
    uOpenDeviceIn.bReqHostMem = FALSE;
  }
#endif

  uOpenDeviceIn.bInitPhy = LSA_TRUE; /* inits the PHY of an Soc1 Board  */
  uOpenDeviceIn.bVerifyEnvironment =
      LSA_TRUE; /* Interrupt Test / Reset of Board */

  uOpenDeviceIn.uCbfEvent.pPtr = &eps_pndevdrv_event_cbf;
  uOpenDeviceIn.uCbfParUser.pPtr = (void*)pBoard;

  uOpenDeviceIn.uThreadDll.lPrio = PNDEV_THREAD_PRIO_HIGH;
  uOpenDeviceIn.uThreadDll.lStackSize =
      0; /* Use Default 8kB according to documentation */
  uOpenDeviceIn.uThreadSys.lPrio = PNDEV_THREAD_PRIO_HIGH;
  uOpenDeviceIn.uThreadSys.lStackSize =
      0; /* Use Default 8kB according to documentation */
  uOpenDeviceIn.uThreadSys.lTimeCycle_10msec =
      0; /* Use Default 100ms according to documentation */
  EPS_ASSERT(pBoard->pDeviceInfo != LSA_NULL);

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  // driver strings have w_char size
  eps_memcpy(uOpenDeviceIn.sPciLoc, pBoard->pDeviceInfo->sPciLoc,
             sizeof(uOpenDeviceIn.sPciLoc));
#else
  eps_memcpy(uOpenDeviceIn.sPciLoc, pBoard->pDeviceInfo->sPciLoc,
             PNDEV_SIZE_STRING_BUF_SHORT);
#endif

  if (pOption->bLoadFw) {
    file_tag_fw.eBoard = pBoard->pDeviceInfo->eBoard;
    file_tag_fw.isHD = (LSA_BOOL)hd_id;
    file_tag_fw.FileType = EPS_APP_FILE_FW;

    EPS_APP_ALLOC_FILE(&file_tag_fw);

    uOpenDeviceIn.uUserFw.uFw[0].eSrc = ePNDEV_FW_SRC_BUF;
    uOpenDeviceIn.uUserFw.uFw[0].as.uBuf.uBase.pPtr = file_tag_fw.pFileData;
    uOpenDeviceIn.uUserFw.uFw[0].as.uBuf.lSize = file_tag_fw.uFileSize;
  }

  // This is to prevent the download of a BootFW for Boards which do not need a
  // BootFW (e.g. I210)
  if ((pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1616) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_EB400_PCIE) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCI) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCIE) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_EB200) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_EB200P) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_FPGA1_ERTEC200P)) {
    file_tag_bootloader.eBoard = pBoard->pDeviceInfo->eBoard;
    file_tag_bootloader.isHD = (LSA_BOOL)hd_id;
    file_tag_bootloader.FileType = EPS_APP_FILE_BOOTLOADER;

    EPS_APP_ALLOC_FILE(&file_tag_bootloader);

    uOpenDeviceIn.uBootFw.eSrc = ePNDEV_FW_SRC_BUF;
    uOpenDeviceIn.uBootFw.as.uBuf.uBase.pPtr = file_tag_bootloader.pFileData;
    uOpenDeviceIn.uBootFw.as.uBuf.lSize = file_tag_bootloader.uFileSize;
    pBoard->bBoardFwUsed = pOption->bLoadFw;
    uOpenDeviceIn.uUserFw.lCtrFw = (pOption->bLoadFw == LSA_TRUE);
    uOpenDeviceIn.uUserFw.bDebugMode = (pOption->bDebug == LSA_TRUE);
    uOpenDeviceIn.uUserFw.bShmDebug = (pOption->bShmDebug == LSA_TRUE);
    uOpenDeviceIn.uUserFw.bShmHaltOnStartup =
        (pOption->bShmHaltOnStartup == LSA_TRUE);
  } else {
    // set BootFW Source to Invalid, no need to set further params
    uOpenDeviceIn.uBootFw.eSrc = ePNDEV_FW_SRC_INVALID;
  }

  if (pOption->bDebug == LSA_TRUE || pOption->bShmDebug == LSA_TRUE) {
    // use default values of timeouts
    uOpenDeviceIn.uUserFw.lTimeoutFwLoad_10msec = 10000;
    uOpenDeviceIn.uUserFw.lTimeoutFwExe_10msec = 10000;
    uOpenDeviceIn.uUserFw.lTimeoutFwStartupStd_10msec = 10000;
    uOpenDeviceIn.uUserFw.lTimeoutFwStartupDebug_10msec = 10000;
    uOpenDeviceIn.uUserFw.lTimeoutDebugger_10msec = 10000;
    uOpenDeviceIn.uUserFw.lTimeWaitDebugger_10msec = 10000;
    uOpenDeviceIn.uUserFw.lTimeStartupSignOfLife_10msec = 10000;
    uOpenDeviceIn.uUserFw.lTimeUpdateSignOfLife_10msec = 10000;
  }

  eps_pndevdrv_open_device_setup_cfg_record(pBoard, !pOption->bLoadFw, hd_id);

#if (EPS_PLF == EPS_PLF_ADONIS_X86)
  EPS_ASSERT(sizeof(EPS_SHM_UPPER_CONFIG_TYPE) <=
             sizeof(uOpenDeviceIn.uUserFw.lArrayUserData));
  eps_memcpy(uOpenDeviceIn.uUserFw.lArrayUserData, &pBoard->SharedMemConfig,
             sizeof(pBoard->SharedMemConfig));
  uOpenDeviceIn.uUserFw.lBatHostAddress =
      0xF0000000; /* reconfigure (soc1/cp1616) pci host bridge  to make local
                     apic registers available (used for IPC messages) */
#endif

  drvResult = g_pEpsPnDevDrv->pDrvHandle->uIf.pFnOpenDevice(
      g_pEpsPnDevDrv->pDrvHandle, sizeof(uOpenDeviceIn),
      sizeof(*puOpenDeviceOut), &uOpenDeviceIn, puOpenDeviceOut);

  if (drvResult != ePNDEV_OK) {
    LSA_CHAR sErrConv[PNDEV_SIZE_STRING_BUF] = {0};
    eps_convert_pndev_err_string(puOpenDeviceOut->sError, sErrConv);
    EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR,
                            "eps_pndevdrv_open_device() %s", sErrConv);

#if ((EPS_PLF != EPS_PLF_ADONIS_X86) && (EPS_PLF != EPS_PLF_LINUX_X86))
    // trace "puOpenDeviceOut->sError"
    {
      LSA_CHAR sErrConv[PNDEV_SIZE_STRING_BUF] = {0};
      eps_convert_pndev_err_string(puOpenDeviceOut->sError, sErrConv);
      EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR,
                              "eps_pndevdrv_open_device(): sError(%s) ",
                              sErrConv);
    }
#endif

#ifdef EPS_PNDEVDRV_PRINTF_DRIVER_INFO
    printf(
        "\r\nEPS PnDevDrv Board Open failed! PCI Bus %d, Dev %d, Fct %d - "
        "(using FW (%d), SBL %d.%d.%d, Cnt Mac: %d)",
        (int)pBoard->Location.uBusNr, (int)pBoard->Location.uDeviceNr,
        (int)pBoard->Location.uFctNr, (int)pBoard->bBoardFwUsed,
        (int)puOpenDeviceOut->uSblInfo.uVersion.lMajor,
        (int)puOpenDeviceOut->uSblInfo.uVersion.lMinor,
        (int)puOpenDeviceOut->uSblInfo.uVersion.lBuild,
        (int)puOpenDeviceOut->uHwRes.uCommon.uRemanentData.lCtrMacAdr);
    printf("\r\n%s ln(%d): %s\r\n", (LSA_CHAR*)__FILE__, __LINE__,
           (LSA_CHAR*)puOpenDeviceOut->sError);
#endif
  } else {
    if (pBoard->SharedMemConfig.HIF.HifCfg.bUseSharedMemMode == LSA_TRUE) {
      if (pBoard->SharedMemConfig.HIF.HifCfg.bUseRemoteMem == LSA_TRUE) {
        // Use HIF Host Memory => use the first 2MB of uPciHostMemOffsetDevice
        // for HIF
        if ((pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1616) ||
            (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_EB400_PCIE))
        // Ertec400
        {
          EPS_ASSERT(pBoard->OpenDevice.uHwRes.as.uErtec400.uSharedHostSdram
                         .uBase.pPtr != LSA_NULL);

          pBoard->Hif.HostMem.pLocalAddress =
              (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec400
                  .uSharedHostSdram.uBase.pPtr;
          pBoard->SharedMemConfig.HIF.HostRam.uPhyAddress =
              pBoard->OpenDevice.uHwRes.as.uErtec400.uSharedHostSdram.lPhyAdr;

        } else if ((pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCI) ||
                   (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCIE))
        // Soc1
        {
          EPS_ASSERT(
              pBoard->OpenDevice.uHwRes.as.uSoc1.uSharedHostSdram.uBase.pPtr !=
              LSA_NULL);

          pBoard->Hif.HostMem.pLocalAddress =
              (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uSoc1.uSharedHostSdram
                  .uBase.pPtr;
          pBoard->SharedMemConfig.HIF.HostRam.uPhyAddress =
              pBoard->OpenDevice.uHwRes.as.uSoc1.uSharedHostSdram.lPhyAdr;
        }
      }
    }
  }

  // Free
  if (pOption->bLoadFw) {
    EPS_APP_FREE_FILE(&file_tag_fw);
  }

  // This is to prevent the FREE_FILE of a BootFW for Boards which do not need a
  // BootFW (e.g. I210)
  if ((pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_CP1616) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_EB400_PCIE) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCI) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_DB_SOC1_PCIE) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_EB200) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_EB200P) ||
      (pBoard->pDeviceInfo->eBoard == ePNDEV_BOARD_FPGA1_ERTEC200P)) {
    EPS_APP_FREE_FILE(&file_tag_bootloader);
  }

  return drvResult;
}

/**
 * register board for interrupt polling
 *
 * @param pPollIsr			ptr to PollIsr parameter
 * @param pBoard 			ptr to corresponding board structure
 * @param uIntSrc			enum with IntSrc information, (here
 * EPS_PNDEV_ISR_PN_GATHERED or EPS_PNDEV_ISR_PN_NRT)
 * @param bCreateThread		True if Thread should be created
 * @return 					EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_register_board_for_polling(
    EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr, EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard,
    EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc, LSA_BOOL* bCreateThread) {
  LSA_UINT32 i;

  *bCreateThread = LSA_FALSE;

  if (pPollIsr->uCntUsers >= EPS_CFG_PNDEVDRV_MAX_BOARDS) {
    return EPS_PNDEV_RET_ERR; /* No More Resources */
  }

  for (i = 0; i < EPS_CFG_PNDEVDRV_MAX_BOARDS; i++) {
    if (pPollIsr->pBoard[i] == pBoard) {
      return EPS_PNDEV_RET_ERR; /* Board polling already enabled */
    }
  }

  for (i = 0; i < EPS_CFG_PNDEVDRV_MAX_BOARDS; i++) {
    if (pPollIsr->pBoard[i] == LSA_NULL) {
      pPollIsr->pBoard[i] = pBoard; /* Register Board for polling */
      pPollIsr->uIntSrc[i] = uIntSrc;
      break;
    }
  }

  pPollIsr->uCntUsers++;

  if (pPollIsr->bRunning == LSA_FALSE) {
    /* Create Thread */
    *bCreateThread = LSA_TRUE;
  }

  return EPS_PNDEV_RET_OK;
}

/**
 * unregister board for interrupt polling
 *
 * @param pPollIsr			ptr to PollIsr parameter
 * @param pBoard 			ptr to corresponding board structure
 * @param bKillThread		True if Thread should be killed
 * @return 					EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_unregister_board_for_polling(
    EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr, EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard,
    LSA_BOOL* bKillThread) {
  LSA_UINT32 i;

  *bKillThread = LSA_FALSE;

  if (pPollIsr->uCntUsers == 0) {
    return EPS_PNDEV_RET_ERR; /* ??? Somethings wrong! */
  }

  for (i = 0; i < EPS_CFG_PNDEVDRV_MAX_BOARDS; i++) {
    if (pPollIsr->pBoard[i] == pBoard) {
      pPollIsr->pBoard[i] = LSA_NULL;

      pPollIsr->uCntUsers--;
      if (pPollIsr->uCntUsers == 0) {
        *bKillThread = LSA_TRUE;
      }

      return EPS_PNDEV_RET_OK; /* Board polling already enabled */
    }
  }

  return EPS_PNDEV_RET_ERR; /* Board not found */
}

/**
 * polling isr, which will be called periodically
 *
 * @param uParam	Parameterstructure (unused)
 * @param arg 		ptr to corresponding structure, containing board and isr
 * parameter
 * @return
 */
LSA_VOID eps_pndevdrv_pn_poll_isr_thread(LSA_UINT32 uParam, void* arg) {
  EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr = (EPS_PNDEVDRV_POLL_PTR_TYPE)arg;
  EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;
  LSA_UINT32 i;

  LSA_UNUSED_ARG(uParam);

  for (i = 0; i < EPS_CFG_PNDEVDRV_MAX_BOARDS; i++) {
    pBoard = pPollIsr->pBoard[i];

    if (pBoard != LSA_NULL) {
      switch (pPollIsr->uIntSrc[i]) {
        case EPS_PNDEV_ISR_PN_GATHERED:
          if (pBoard->sIsrPnGathered.pCbf != LSA_NULL) {
            pBoard->sIsrPnGathered.pCbf(pBoard->sIsrPnGathered.uParam,
                                        pBoard->sIsrPnGathered.pArgs);
          } else {
            EPS_FATAL(
                "No callbackfunction for gathered interrupt registered in "
                "pndevdrv polling isr"); /* Unexpected */
          }
          break;
        case EPS_PNDEV_ISR_PN_NRT:
          if (pBoard->sIsrPnNrt.pCbf != LSA_NULL) {
            pBoard->sIsrPnNrt.pCbf(pBoard->sIsrPnNrt.uParam,
                                   pBoard->sIsrPnNrt.pArgs);
          } else {
            EPS_FATAL(
                "No callbackfunction for nrt interrupt registered in pndevdrv "
                "polling isr"); /* Unexpected */
          }
          break;
        default:
          EPS_FATAL("Unknown interrupt source in pndevdrv polling isr");
          break;
      }
    }
  }
}

/**
 * polling isr, which will be called periodically
 *
 * @param uParam	Parameterstructure (unused)
 * @param arg 		ptr to corresponding structure, containing board and isr
 * parameter
 * @return
 */
LSA_VOID eps_pndevdrv_timer_poll_isr_thread(LSA_UINT32 uParam,
                                            LSA_VOID* pArgs) {
  EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pArgs;

  LSA_UNUSED_ARG(uParam);

  pBoard->TimerCtrl.sTimerCbf.pCbf(pBoard->TimerCtrl.sTimerCbf.uParam,
                                   pBoard->TimerCtrl.sTimerCbf.pArgs);

  return;
}

/**
 * create periodically poll thread
 *
 * @param cName			internal thread name
 * @param pPollIsr 		ptr to corresponding structure for isr parameter
 * @param pBoard 		ptr to corresponding board structure
 * @return EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_create_poll_thread(
    LSA_CHAR* cName, EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr,
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard) {
  EPS_SYSTEM_TRACE_STRING(
      0, LSA_TRACE_LEVEL_NOTE_HIGH,
      "eps_pndevdrv_create_poll_thread() - starting poll thread - Name: %s",
      cName);

  switch (pBoard->pDeviceInfo->eBoard) {
    // SPVL -> HighPerformance adaptive poll-thread (1msec)
    case ePNDEV_BOARD_I210:
    case ePNDEV_BOARD_I82574:
    case ePNDEV_BOARD_MICREL_KSZ8841:
    case ePNDEV_BOARD_MICREL_KSZ8842: {
      pPollIsr->hThread = eps_tasks_start_us_poll_thread(
          cName, EPS_POSIX_THREAD_PRIORITY_HIGH_PERFORMANCE, eSchedFifo, 1000,
          eps_pndevdrv_pn_poll_isr_thread, 0, (LSA_VOID*)pPollIsr);
      break;
    }

    // normal poll-thread (1msec)
    case ePNDEV_BOARD_CP1616:
    case ePNDEV_BOARD_EB200:
    case ePNDEV_BOARD_DB_SOC1_PCI:
    case ePNDEV_BOARD_DB_SOC1_PCIE:
    case ePNDEV_BOARD_FPGA1_ERTEC200P:
    case ePNDEV_BOARD_EB200P:
    case ePNDEV_BOARD_DB_EB400_PCIE: {
      pPollIsr->hThread = eps_tasks_start_poll_thread(
          cName, EPS_POSIX_THREAD_PRIORITY_HIGH, eSchedFifo, 1,
          eps_pndevdrv_pn_poll_isr_thread, 0, (LSA_VOID*)pPollIsr);
      break;
    }

    // unknown board
    default: {
      EPS_FATAL("eps_pndevdrv_create_poll_thread(): Unknown Board-Type!!!");
      break;
    }
  }

  EPS_ASSERT(pPollIsr->hThread != 0);

  pPollIsr->bRunning = LSA_TRUE;

#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
  eps_tasks_group_thread_add(pPollIsr->hThread, EPS_TGROUP_USER);
#endif

  return EPS_PNDEV_RET_OK;
}

/**
 * kill periodically poll thread
 *
 * @param cName			internal thread name
 * @param pPollIsr 		ptr to corresponding structure for isr parameter
 * @param pBoard 		ptr to corresponding board structure
 * @return 	EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_kill_poll_thread(
    LSA_CHAR* cName, EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr,
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard) {
  LSA_UNUSED_ARG(pBoard);
  LSA_UNUSED_ARG(cName);
  EPS_SYSTEM_TRACE_STRING(
      0, LSA_TRACE_LEVEL_NOTE_HIGH,
      "eps_pndevdrv_kill_poll_thread() - stopping poll thread - Name: %s",
      cName);
  pPollIsr->bRunning = LSA_FALSE;

#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
  eps_tasks_group_thread_remove(pPollIsr->hThread);
#endif

  eps_tasks_stop_poll_thread(pPollIsr->hThread);

  return EPS_PNDEV_RET_OK;
}

/**
 * create periodically poll thread for gathered method
 *
 * @param pBoard 		ptr to corresponding board structure
 * @return 	EPS_PNDEV_RET_OK, upon successful execution
 */
static LSA_UINT16 eps_pndevdrv_create_pn_gathered_polling_thread(
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard) {
  LSA_UINT16 retVal;
  LSA_BOOL bCreateThread;
  EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr = &g_pEpsPnDevDrv->poll.PnGatheredIsr;

  retVal = eps_pndevdrv_register_board_for_polling(
      pPollIsr, pBoard, EPS_PNDEV_ISR_PN_GATHERED, &bCreateThread);

  if (bCreateThread == LSA_TRUE) {
    retVal =
        eps_pndevdrv_create_poll_thread("EPS_PNDEVGPISR", pPollIsr, pBoard);
    if (retVal != EPS_PNDEV_RET_OK) {
      eps_pndevdrv_unregister_board_for_polling(pPollIsr, pBoard,
                                                &bCreateThread);
    }
  }

  return retVal;
}

/**
 * kill periodically poll thread for gathered method
 *
 * @param pBoard 	ptr to corresponding board structure
 * @return LSA_TRUE	EPS_PNDEV_RET_OK, upon successful execution
 */
static LSA_UINT16 eps_pndevdrv_kill_pn_gathered_polling_thread(
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard) {
  LSA_UINT16 retVal;
  LSA_BOOL bKillThread;
  EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr = &g_pEpsPnDevDrv->poll.PnGatheredIsr;

  EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,
                      "eps_pndevdrv_kill_pn_gathered_polling_thread() - "
                      "stopping gathered poll thread");
  retVal =
      eps_pndevdrv_unregister_board_for_polling(pPollIsr, pBoard, &bKillThread);

  if (bKillThread == LSA_TRUE) {
    eps_pndevdrv_kill_poll_thread("EPS_PNDEVGPISR", pPollIsr, pBoard);
  }

  return retVal;
}

/**
 * create periodically poll thread for nrt purpose
 *
 * @param pBoard 		ptr to corresponding board structure
 * @return 	EPS_PNDEV_RET_OK, upon successful execution
 */
static LSA_UINT16 eps_pndevdrv_create_pn_nrt_polling_thread(
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard) {
  LSA_UINT16 retVal;
  LSA_BOOL bCreateThread;
  EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr = &g_pEpsPnDevDrv->poll.PnNrtIsr;

  EPS_SYSTEM_TRACE_00(
      0, LSA_TRACE_LEVEL_NOTE_HIGH,
      "eps_pndevdrv_create_pn_nrt_polling_thread() - creating nrt poll thread");
  retVal = eps_pndevdrv_register_board_for_polling(
      pPollIsr, pBoard, EPS_PNDEV_ISR_PN_NRT, &bCreateThread);

  if (bCreateThread == LSA_TRUE) {
    retVal =
        eps_pndevdrv_create_poll_thread("EPS_PNDEVNRTPISR", pPollIsr, pBoard);
    if (retVal != EPS_PNDEV_RET_OK) {
      eps_pndevdrv_unregister_board_for_polling(pPollIsr, pBoard,
                                                &bCreateThread);
    }
  }

  return retVal;
}

/**
 * kill periodically poll thread for nrt purpose
 *
 * @param pBoard 		ptr to corresponding board structure
 * @return 	LSA_TRUE, upon successful execution
 */
static LSA_UINT16 eps_pndevdrv_kill_pn_nrt_polling_thread(
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard) {
  LSA_UINT16 retVal;
  LSA_BOOL bKillThread;
  EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr = &g_pEpsPnDevDrv->poll.PnNrtIsr;

  EPS_SYSTEM_TRACE_00(
      0, LSA_TRACE_LEVEL_NOTE_HIGH,
      "eps_pndevdrv_kill_pn_nrt_polling_thread() - stopping nrt poll thread");
  retVal =
      eps_pndevdrv_unregister_board_for_polling(pPollIsr, pBoard, &bKillThread);

  if (bKillThread == LSA_TRUE) {
    eps_pndevdrv_kill_poll_thread("EPS_PNDEVNRTPISR", pPollIsr, pBoard);
  }

  return retVal;
}

/**
 * enable mentioned interrupt method for current board
 *
 * @param pHwInstIn 		pointer to strucute with hardware configuration
 * @param pInterrupt		pointer to enum with method type
 * @param pCbf				pointer to Callbackfunction
 * @return EPS_PNDEV_RET_OK, upon successful execution
 * @return EPS_PNDEV_RET_ERR
 */
static LSA_UINT16 eps_pndevdrv_enable_interrupt_intern(
    EPS_PNDEV_HW_PTR_TYPE pHwInstIn,
    EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt,
    EPS_PNDEV_CALLBACK_PTR_TYPE pCbf) {
  EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
  uPNDEV_CHANGE_INT_IN uChangeIntIn;
  uPNDEV_CHANGE_INT_OUT uChangeIntOut;
  ePNDEV_RESULT drvResult;
  LSA_UINT16 result = EPS_PNDEV_RET_OK;

  pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

  EPS_ASSERT(pBoard->bUsed == LSA_TRUE);

  switch (*pInterrupt) {
    case EPS_PNDEV_ISR_ISOCHRONOUS:
      if (pBoard->sIsrIsochronous.pCbf != LSA_NULL) {
        result = EPS_PNDEV_RET_ERR;
        break;
      }

      switch (pHwInstIn->asic_type) {
        case EPS_PNDEV_ASIC_ERTEC400:
          ///@todo LaM: IRT for ERTEC400 not implemented
          EPS_FATAL("isochronous interrupt not implemented yet for ERTEC400");
          break;
        case EPS_PNDEV_ASIC_ERTEC200:
          ///@todo LaM: IRT for ERTEC200 not implemented
          EPS_FATAL("isochronous interrupt not implemented yet for ERTEC200");
          break;
        case EPS_PNDEV_ASIC_IRTEREV7:
          ///@todo LaM: IRT for IRTEREV7 not implemented
          EPS_FATAL("isochronous interrupt not implemented yet for IRTEREV7");
          break;
#if (PSI_CFG_USE_EDDP == 1)
        case EPS_PNDEV_ASIC_ERTEC200P:
          eps_hw_e200p_enable_isochronous_interrupt(
              pHwInstIn->EpsBoardInfo.eddp.pnip.base_ptr,
              pHwInstIn->EpsBoardInfo.eddp.apb_periph_perif.base_ptr,
              LSA_FALSE);
          break;
#endif
        case EPS_PNDEV_ASIC_INTEL_XX:
          ///@todo IRT for INTEL_XX not implemented
          EPS_FATAL("isochronous interrupt not implemented yet for INTEL_XX");
          break;
        case EPS_PNDEV_ASIC_KSZ88XX:
          ///@todo IRT for Micrel not implemented
          EPS_FATAL("isochronous interrupt not implemented yet for KSZ88XX");
          break;
        default:
          EPS_FATAL("not supported ASIC type for enabling intern interrupt");
          break;
      }

      eps_memset(&uChangeIntIn, 0, sizeof(uChangeIntIn));
      uChangeIntIn.uDevice.hHandle = pBoard->OpenDevice.uDevice.hHandle;
      uChangeIntIn.eIntAction = ePNDEV_INT_ACTION_UNMASK;
      uChangeIntIn.lPrioThread = PNDEV_THREAD_PRIO_UNCHANGED;

      // unmask interrupts
      uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3] =
          LSA_TRUE;  // enable accumulative interrupts ICU

      // ack interrupts
      uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3] =
          LSA_TRUE;  // ack pending accumulative interrupts ICU

      // enable desired interrupt source(s)
      drvResult = g_pEpsPnDevDrv->pDrvHandle->uIf.pFnChangeInt(
          g_pEpsPnDevDrv->pDrvHandle, sizeof(uPNDEV_CHANGE_INT_IN),
          sizeof(uPNDEV_CHANGE_INT_OUT), &uChangeIntIn, &uChangeIntOut);

      EPS_ASSERT(drvResult == ePNDEV_OK);

      pBoard->sIsrIsochronous = *pCbf;

      break;
    case EPS_PNDEV_ISR_PN_GATHERED:
      /* In PCI Mode only 1 real interrupt is available which is used for
       * Isochrnous Mode */
      /* So we create polling threads for other pn interrupts */
      if (pBoard->sIsrPnGathered.pCbf != LSA_NULL) {
        result = EPS_PNDEV_RET_ERR;
        break;
      }

      pBoard->sIsrPnGathered = *pCbf;

      result = eps_pndevdrv_create_pn_gathered_polling_thread(pBoard);

      if (result != EPS_PNDEV_RET_OK) {
        pBoard->sIsrPnGathered.pCbf = LSA_NULL;
      }

      break;
    case EPS_PNDEV_ISR_PN_NRT:
      /* In PCI Mode only 1 real interrupt is available which is used for
       * Isochrnous Mode */
      /* So we create polling threads for other pn interrupts */
      if (pBoard->sIsrPnNrt.pCbf != LSA_NULL) {
        result = EPS_PNDEV_RET_ERR;
        break;
      }

      pBoard->sIsrPnNrt = *pCbf;

      result = eps_pndevdrv_create_pn_nrt_polling_thread(pBoard);

      if (result != EPS_PNDEV_RET_OK) {
        pBoard->sIsrPnNrt.pCbf = LSA_NULL;
      }

      break;
    default:
      result = EPS_PNDEV_RET_UNSUPPORTED;
      break;
  }

  if (result == EPS_PNDEV_RET_OK) {
    pBoard->uCountIsrEnabled++;
  }

  return result;
}

/**
 * disable mentioned interrupt method for current board
 *
 * @param pHwInstIn 		pointer to strucute with hardware configuration
 * @param pInterrupt		pointer to enum with method type
 * @return 	EPS_PNDEV_RET_OK, upon successful execution or EPS_PNDEV_RET_ERR
 */
static LSA_UINT16 eps_pndevdrv_disable_interrupt_intern(
    EPS_PNDEV_HW_PTR_TYPE pHwInstIn,
    EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt) {
  EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
  uPNDEV_CHANGE_INT_IN uChangeIntIn;
  uPNDEV_CHANGE_INT_OUT uChangeIntOut;
  ePNDEV_RESULT drvResult;
  LSA_UINT16 result = EPS_PNDEV_RET_OK;

  pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

  EPS_ASSERT(pBoard->bUsed == LSA_TRUE);
  EPS_ASSERT(pBoard->uCountIsrEnabled != 0);

  switch (*pInterrupt) {
    case EPS_PNDEV_ISR_ISOCHRONOUS:
      if (pBoard->sIsrIsochronous.pCbf == LSA_NULL) {
        result = EPS_PNDEV_RET_ERR;
        break;
      }

      switch (pHwInstIn->asic_type) {
        case EPS_PNDEV_ASIC_ERTEC400:
          break;
        case EPS_PNDEV_ASIC_ERTEC200:
          break;
        case EPS_PNDEV_ASIC_IRTEREV7:
          break;
#if (PSI_CFG_USE_EDDP == 1)
        case EPS_PNDEV_ASIC_ERTEC200P:
          eps_hw_e200p_disable_isochronous_interrupt(
              pHwInstIn->EpsBoardInfo.eddp.pnip.base_ptr, LSA_FALSE);
          break;
#endif
        case EPS_PNDEV_ASIC_INTEL_XX:
          EPS_FATAL("IRT interrupts not supported for INTEL_XX.");
          break;
        case EPS_PNDEV_ASIC_KSZ88XX:
          EPS_FATAL("IRT interrupts not supported for KSZ88XX.");
          break;
        default:
          EPS_FATAL("Unknown ASIC type for disabling IRT interrupt.");
          break;
      }

      eps_memset(&uChangeIntIn, 0, sizeof(uChangeIntIn));
      uChangeIntIn.uDevice.hHandle = pBoard->OpenDevice.uDevice.hHandle;
      uChangeIntIn.eIntAction = ePNDEV_INT_ACTION_UNMASK;
      uChangeIntIn.lPrioThread = PNDEV_THREAD_PRIO_UNCHANGED;

      // mask interrupts
      uChangeIntIn.bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3] =
          LSA_FALSE;  // enable accumulative interrupts ICU

      // disable ack interrupts
      uChangeIntIn.bArrayIntAck[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3] =
          LSA_FALSE;  // ack pending accumulative interrupts ICU

      // enable desired interrupt source(s)
      drvResult = g_pEpsPnDevDrv->pDrvHandle->uIf.pFnChangeInt(
          g_pEpsPnDevDrv->pDrvHandle, sizeof(uPNDEV_CHANGE_INT_IN),
          sizeof(uPNDEV_CHANGE_INT_OUT), &uChangeIntIn, &uChangeIntOut);

      pBoard->sIsrIsochronous.pCbf = LSA_NULL;

      EPS_ASSERT(drvResult == ePNDEV_OK);

      break;
    case EPS_PNDEV_ISR_PN_GATHERED:
      /* In PCI Mode only 1 real interrupt is available which is used for
       * Isochrnous Mode */
      /* Kill polling threads */
      if (pBoard->sIsrPnGathered.pCbf == LSA_NULL) {
        result = EPS_PNDEV_RET_ERR;
        break;
      }

      result = eps_pndevdrv_kill_pn_gathered_polling_thread(pBoard);

      if (result == EPS_PNDEV_RET_OK) {
        pBoard->sIsrPnGathered.pCbf = LSA_NULL;
      }

      break;
    case EPS_PNDEV_ISR_PN_NRT:
      /* In PCI Mode only 1 real interrupt is available which is used for
       * Isochrnous Mode */
      /* Kill polling threads */
      if (pBoard->sIsrPnNrt.pCbf == LSA_NULL) {
        result = EPS_PNDEV_RET_ERR;
        break;
      }

      result = eps_pndevdrv_kill_pn_nrt_polling_thread(pBoard);

      if (result == EPS_PNDEV_RET_OK) {
        pBoard->sIsrPnNrt.pCbf = LSA_NULL;
      }

      break;
    default:
      result = EPS_PNDEV_RET_UNSUPPORTED;
      break;
  }

  if (result == EPS_PNDEV_RET_OK) {
    pBoard->uCountIsrEnabled--;
  }

  return result;
}

#if 0  // not used yet
/**
 * create periodically poll thread for nrt purpose
 * 
 * @param pBoard 		ptr to corresponding board structure
 * @return 	EPS_PNDEV_RET_OK, upon successful execution		
 */
static LSA_UINT16 eps_pndevdrv_create_timer_polling_thread (EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard)
{
	pBoard->TimerCtrl.hThread = eps_tasks_start_us_poll_thread("EPS_PNDEVTMRPOLL", EPS_POSIX_THREAD_PRIORITY_HIGH, eSchedFifo, pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval, eps_pndevdrv_timer_poll_isr_thread, 0, (LSA_VOID*)pBoard);
	EPS_ASSERT(pBoard->TimerCtrl.hThread != 0);
#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
	eps_tasks_group_thread_add ( pBoard->TimerCtrl.hThread, EPS_TGROUP_USER );
#endif
	return EPS_PNDEV_RET_OK;
}
#endif

/**
 * start timer intern, for eddi SiiExtTimerMode
 *
 * @param pHwInstIn 		pointer to strucute with hardware configuration
 * @param pCbf				Callbackfunction, as argument for ISR
 * @return 	EPS_PNDEV_RET_OK, upon successful execution	or
 * EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pndevdrv_timer_ctrl_start_intern(
    struct eps_pndev_hw_tag* pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf) {
  EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
  LSA_UINT16 result = EPS_PNDEV_RET_ERR;

  EPS_ASSERT(pHwInstIn != LSA_NULL);
  EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

  pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

  EPS_ASSERT(pBoard->bUsed == LSA_TRUE);
  EPS_ASSERT(pBoard->TimerCtrl.bRunning == LSA_FALSE);

  if (pBoard->sHw.EpsBoardInfo.edd_type == LSA_COMP_ID_EDDI) {
    if ((pBoard->sHw.EpsBoardInfo.board_type == EPS_PNDEV_BOARD_SOC1_PCI) ||
        (pBoard->sHw.EpsBoardInfo.board_type == EPS_PNDEV_BOARD_SOC1_PCIE)) {
      pBoard->TimerCtrl.hThread = eps_tasks_start_us_poll_thread(
          "EPS_PNDEXTTMR", EPS_POSIX_THREAD_PRIORITY_HIGH, eSchedFifo,
          pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval, pCbf->pCbf,
          pCbf->uParam, pCbf->pArgs);
      EPS_ASSERT(pBoard->TimerCtrl.hThread != 0);

      result = EPS_PNDEV_RET_OK;
    } else {
      EPS_FATAL("Can't aktivate poll thread for non IRTEREV7DEV boards");
    }
  } else {
    result = EPS_PNDEV_RET_UNSUPPORTED;
  }

  return result;
}

/**
 * stop timer intern, for eddi SiiExtTimerMode
 *
 * @param pHwInstIn 		pointer to strucute with hardware configuration
 * @return 	EPS_PNDEV_RET_OK, upon successful execution	or
 * EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pndevdrv_timer_ctrl_stop_intern(
    struct eps_pndev_hw_tag* pHwInstIn) {
  EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
  LSA_UINT16 result = EPS_PNDEV_RET_OK;

  pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

  pBoard->TimerCtrl.bRunning = LSA_FALSE;

  if (pBoard->sHw.EpsBoardInfo.edd_type == LSA_COMP_ID_EDDI) {
    eps_tasks_stop_poll_thread(pBoard->TimerCtrl.hThread);
  } else {
    result = EPS_PNDEV_RET_UNSUPPORTED;
  }

  return result;
}

/**
 * initialize and set parameters for hif communication and open hif driver
 *
 * @param pBoard pointer to corresponding board structue
 * @return 	EPS_HIF_DRV_RET_OK
 */
static LSA_VOID eps_pndevdrv_open_hif_driver(
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard) {
  LSA_UINT16 retVal;
  EPS_HIF_DRV_OPENPAR_TYPE InPars;

  EPS_ASSERT(g_pEpsPnDevDrv != LSA_NULL);

  if (pBoard->bBoardFwUsed ==
      LSA_TRUE) /* HIF Driver is only installed if we are working with an
                   Firmware equipped Board */
  {
    eps_memset(&InPars, 0, sizeof(InPars));

    /* this function is related to eps_pndevdrv_open_device_setup_cfg_record()
     */

    InPars.Mode = EPS_HIF_DRV_HIF_TM_SHARED_MEM;
    InPars.if_pars.SendIrqMode =
        HIF_SEND_IRQ_AFTER_READ_BACK;  // read back before sending irqs to
                                       // pnboards, to prevent from unecassary
                                       // hif_requests

    if ((pBoard->SharedMemConfig.bValid == 1) &&
        (pBoard->SharedMemConfig.HIF.bValid == LSA_TRUE) &&
        (pBoard->SharedMemConfig.HIF.HifCfg.bUseRemoteMem == 1)) {
      /* Host Memory used => fill in addresses */
      InPars.if_pars.SharedMem.Local.Base = pBoard->Hif.HostMem.pLocalAddress;
      InPars.if_pars.SharedMem.Local.Size =
          pBoard->SharedMemConfig.HIF.HostRam.uSize;
      InPars.if_pars.SharedMem.Remote.Base =
          pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr;
      InPars.if_pars.SharedMem.Remote.Size =
          pBoard->sHw.EpsBoardInfo.hif_mem.size;
    } else {
      /* Legacy = No Interrupts / No Host Memory (splitted shared memory) /
       * Shared Memory */
      pBoard->SharedMemConfig.HIF.HifCfg.bUseSharedMemMode =
          LSA_TRUE; /* That's needed for selecting a valid HIF Driver */
      InPars.if_pars.SharedMem.Local.Base =
          pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr;
      InPars.if_pars.SharedMem.Local.Size =
          pBoard->sHw.EpsBoardInfo.hif_mem.size / 2;
      InPars.if_pars.SharedMem.Remote.Base =
          pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr +
          (pBoard->sHw.EpsBoardInfo.hif_mem.size / 2);
      InPars.if_pars.SharedMem.Remote.Size =
          pBoard->sHw.EpsBoardInfo.hif_mem.size / 2;
    }

    if ((pBoard->SharedMemConfig.bValid == 1) &&
        (pBoard->SharedMemConfig.HIF.bValid == LSA_TRUE) &&
        (pBoard->SharedMemConfig.HIF.HifCfg.bUseIpcInterrupts == 1)) {
      /* IPC interrupts => fill in already allocated handles */
      InPars.drv_type_specific.pars.ipc.bUsehIpcRx = LSA_TRUE;
      InPars.drv_type_specific.pars.ipc.hIpcRx = pBoard->Hif.Ipc.Rx.uRxHandle;
    }

    retVal = eps_hif_drv_if_open(pBoard->sysDev.hd_nr, LSA_TRUE /*bUpper*/,
                                 &pBoard->SharedMemConfig.HIF.HifCfg, &InPars,
                                 &pBoard->pHifDrvHwInstOut);

    EPS_ASSERT(retVal == EPS_HIF_DRV_RET_OK);
  }
}

/**
 * close hif driver
 *
 * @param pBoard pointer to corresponding board structue
 * @return 	EPS_HIF_DRV_RET_OK
 */
static LSA_VOID eps_pndevdrv_close_hif_driver(
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard) {
  LSA_UINT16 retVal;

  if (pBoard->bBoardFwUsed == LSA_TRUE) {
    retVal = eps_hif_drv_if_close(pBoard->sysDev.hd_nr, LSA_TRUE /*bUpper*/);

    EPS_ASSERT(retVal == EPS_HIF_DRV_RET_OK);
  }
}

/**
 * startup PnDevDriver (open DLL)
 *
 * @param LSA_VOID
 * @return 	ePNDEV_OK
 */
static LSA_BOOL eps_pndevdrv_startup(LSA_VOID) {
  PNDEV_CHAR drvError[PNDEV_SIZE_STRING_BUF];
  ePNDEV_RESULT eResult = ePNDEV_FATAL;
  LSA_BOOL bSuccess = LSA_FALSE;

#if (EPS_PLF == EPS_PLF_LINUX_X86)
  // PNDEV_CHAR*     pVobName   =
  // _TEXT("../../../pndevdrv/PnDev_Driver/bin/Linux/PnDev_DriverU32.so");
  // PNDEV_CHAR*     pItestName =
  // _TEXT("../../pndevdrv/PnDev_Driver/bin/Linux/PnDev_DriverU32.so");
  // PNDEV_CHAR*     pName      = _TEXT("./PnDev_DriverU32.so");
  PNDEV_CHAR* pVobName = _TEXT(
      "/home/skloe/Coding/no_warning/src/source/pndevdrv/PnDev_Driver/bin/"
      "Linux/PnDev_DriverU32.so");
  PNDEV_CHAR* pItestName = _TEXT(
      "/home/skloe/Coding/no_warning/src/source/pndevdrv/PnDev_Driver/bin/"
      "Linux/PnDev_DriverU32.so");
  PNDEV_CHAR* pName = _TEXT(
      "/home/skloe/Coding/no_warning/src/examples/test_app/linux32/build/"
      "nDev_DriverU32.so");
#elif (EPS_PLF == EPS_PLF_WINDOWS_X86)
  PNDEV_CHAR* pVobName =
      L"..\\..\\..\\pndevdrv\\PnDev_Driver\\bin\\PnDev_DriverU32.dll";
  PNDEV_CHAR* pItestName =
      L"..\\..\\pndevdrv\\PnDev_Driver\\bin\\PnDev_DriverU32.dll";
  PNDEV_CHAR* pName = L"PnDev_DriverU32.dll";
#else
  // No DLL for Adonis x86
  PNDEV_CHAR* pVobName = "\0";
  PNDEV_CHAR* pItestName = "\0";
  PNDEV_CHAR* pName = "\0";
#endif

  // First try startup PnDevDrv DLL explizit from VOB dir (Debug usecase)
  // note: Function pointers and version check is internal done by with driver
  // startup service

  eResult = fnPnDev_StartUpDriver(pVobName, PNDEV_SIZE_STRING_BUF,
                                  &g_pEpsPnDevDrv->pDrvHandle, drvError);

  if (eResult != ePNDEV_OK)  // First startup failed
  {
    // try startup PN-IP driver DLL explizit from start dir (iTest usecase)
    eResult = fnPnDev_StartUpDriver(pItestName, PNDEV_SIZE_STRING_BUF,
                                    &g_pEpsPnDevDrv->pDrvHandle, drvError);
  }

  if (eResult != ePNDEV_OK)  // First startup failed
  {
    // try startup PN-IP driver DLL explizit from start dir (iTest usecase)
    eResult = fnPnDev_StartUpDriver(pName, PNDEV_SIZE_STRING_BUF,
                                    &g_pEpsPnDevDrv->pDrvHandle, drvError);
  }

  if (eResult == ePNDEV_OK) {
    bSuccess = LSA_TRUE;
  } else {
    LSA_CHAR sErrConv[PNDEV_SIZE_STRING_BUF] = {0};
    eps_convert_pndev_err_string(drvError, sErrConv);
    EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR, "EPS PnDevDrv %s",
                            sErrConv);

#ifdef EPS_PNDEVDRV_PRINTF_DRIVER_INFO
    printf("\r\n%s ln(%d): %s\r\n", (LSA_CHAR*)__FILE__, __LINE__,
           (LSA_CHAR*)drvError);
#endif
  }

  return bSuccess;
}

#if (PSI_CFG_USE_EDDI == 1)
/**
 * Translate local address to PciHostAdress (only for AhbPciBridge for Soc1 and
 * Ertec400)
 *
 * @param 	pBoard						pointer to current board
 * structure
 * @param 	pPnDevDrvPciParamsArray		pointer to
 * AhbPciBridgeConfiguration
 * @param 	uCntBars					counter of bars
 * @param 	pLocalPhyAddress			pointer with local
 * address
 * @return 	pTargetAddress				pointer with
 * PciHostAddress
 */
static LSA_UINT8* eps_pndevdrv_calc_phy_pci_host_address(
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard,
    uPNCORE_PCI_HOST_PAR* pPnDevDrvPciParamsArray, LSA_UINT32 uCntBars,
    LSA_UINT8* pLocalPhyAddress) {
  // Get the SOC/ERTEC400 AHB Address for an local physical pci address

  LSA_UINT32 i;
  uPNCORE_PCI_HOST_PAR* pHostBar;
  LSA_UINT8* pPciAddress;
  LSA_UINT8* pAhbAddress;
  LSA_UINT8* pTargetAddress = LSA_NULL;

  if (uCntBars != 0) {
    if (pBoard->bBoardFwUsed == LSA_FALSE) /* (LaM) Workaround for PnDevDrv
                                              2.7.1.1 => Works for Soc1 and
                                              Ertec400 */
    {
      pHostBar = &pPnDevDrvPciParamsArray[0];

      pHostBar->lBarHost = 0xC0000000;
      pHostBar->lBatPciHost = 0x00000000;
      pHostBar->lSizePciHost = 0x20000000;
    }

    EPS_ASSERT(pLocalPhyAddress < (LSA_UINT8*)0xFFFFFFFF);  // (LaM)
                                                            // Soc/Ertec400 can
                                                            // only reach the
                                                            // first 4 GB

    for (i = 0; i < uCntBars; i++) {
      pHostBar = &pPnDevDrvPciParamsArray[i];
      pPciAddress = (LSA_UINT8*)pHostBar->lBatPciHost;
      pAhbAddress = (LSA_UINT8*)pHostBar->lBarHost;
      if ((pLocalPhyAddress > pPciAddress) &&
          (pLocalPhyAddress <= (pPciAddress - 1 + pHostBar->lSizePciHost))) {
        // found Fitting Window
        pTargetAddress = pLocalPhyAddress - pPciAddress + pAhbAddress;
        break;
      }
    }
  }

  return pTargetAddress;
}
#endif

#if (PSI_CFG_USE_EDDP == 1)
/**
 * init and set board parameters after OpenDevice of PnDevDriver for eb200p
 *
 * @param	pBoard				pointer to current board
 * structure
 * @param 	hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_open_eb200p(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard,
                                           LSA_UINT16 hd_id) {
  LSA_UINT16 result = EPS_PNDEV_RET_OK;
  LSA_UINT32 macCount, macIdx, ifIdx, portIdx;
  LSA_UINT32 nrtSize = 0;
  LSA_UINT32 crtSize = 0;
  LSA_UINT16 nrOfIF = 1;
  EPS_MAC_PTR_TYPE pMac = LSA_NULL;

  pBoard->sHw.asic_type = EPS_PNDEV_ASIC_ERTEC200P;

  eps_memset(&pBoard->sHw.EpsBoardInfo, 0, sizeof(pBoard->sHw.EpsBoardInfo));

  switch (pBoard->OpenDevice.uHwInfo.eBoard) {
    case ePNDEV_BOARD_EB200P:
      pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_EB200P;
      break;
    case ePNDEV_BOARD_FPGA1_ERTEC200P:
      pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_FPGA_ERTEC200P;
      break;
    default:
      result = EPS_PNDEV_RET_UNSUPPORTED;
      break;
  }

  pBoard->sysDev.hd_nr = hd_id;
  pBoard->sysDev.if_nr = 1;       // don't care
  pBoard->sysDev.pnio_if_nr = 0;  // don't care
  pBoard->sysDev.edd_comp_id = LSA_COMP_ID_EDDP;

  pBoard->sHw.EpsBoardInfo.hd_sys_handle = &pBoard->sysDev;
  pBoard->sHw.EpsBoardInfo.edd_type = LSA_COMP_ID_EDDP;

  // Init User to HW port mapping (used PSI GET HD PARAMS)
  eps_hw_init_board_port_param(&pBoard->sHw.EpsBoardInfo);

  // EDDP CRT App (Transfer End) settings
  pBoard->sHw.EpsBoardInfo.eddp.appl_timer_mode =
      EDDP_APPL_TIMER_CFG_MODE_TRANSFER_END;  // EDDP CRT APP Transfer End
                                              // Interrupt not used

  /* Board Type specific setup */
  switch (pBoard->sHw.EpsBoardInfo.board_type) {
    case EPS_PNDEV_BOARD_FPGA_ERTEC200P:
      pBoard->sHw.EpsBoardInfo.eddp.board_type =
          EPS_EDDP_BOARD_TYPE_FPGA1__ERTEC200P_REV1;
      pBoard->sHw.EpsBoardInfo.eddp.hw_type = EDDP_HW_ERTEC200P_FPGA;
      pBoard->sHw.EpsBoardInfo.eddp.appl_timer_reduction_ratio =
          4;  // CRT Transfer End 4:1 reduction ratio
      pBoard->sHw.EpsBoardInfo.eddp.is_transfer_end_correction_pos =
          LSA_TRUE;  //(LaM) ToDo CRT Transfer End Isr Offset Correction
      pBoard->sHw.EpsBoardInfo.eddp.transfer_end_correction_value =
          0;  //(LaM) ToDo CRT Transfer End Isr Offset Correction

      // setup portmapping (1:2, 2:1)
      pBoard->sHw.EpsBoardInfo.port_map[0][1].hw_phy_nr = 1;
      pBoard->sHw.EpsBoardInfo.port_map[0][1].hw_port_id = 2;

      pBoard->sHw.EpsBoardInfo.port_map[0][2].hw_phy_nr = 0;
      pBoard->sHw.EpsBoardInfo.port_map[0][2].hw_port_id = 1;
      break;
    case EPS_PNDEV_BOARD_EB200P:
      pBoard->sHw.EpsBoardInfo.eddp.board_type =
          EPS_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV1;
      pBoard->sHw.EpsBoardInfo.eddp.hw_type = EDDP_HW_ERTEC200P;
      pBoard->sHw.EpsBoardInfo.eddp.appl_timer_reduction_ratio =
          1;  // CRT Transfer End 1:1 reduction ratio
      pBoard->sHw.EpsBoardInfo.eddp.is_transfer_end_correction_pos =
          LSA_TRUE;  //(LaM) ToDo CRT Transfer End Isr Offset Correction
      pBoard->sHw.EpsBoardInfo.eddp.transfer_end_correction_value =
          0;  //(LaM) ToDo CRT Transfer End Isr Offset Correction

      // setup portmapping (=1:1)
      pBoard->sHw.EpsBoardInfo.port_map[0][1].hw_phy_nr = 0;
      pBoard->sHw.EpsBoardInfo.port_map[0][1].hw_port_id = 1;

      pBoard->sHw.EpsBoardInfo.port_map[0][2].hw_phy_nr = 1;
      pBoard->sHw.EpsBoardInfo.port_map[0][2].hw_port_id = 2;
      break;
    default:
      EPS_FATAL("not supported board type");
      break;
  }

  pBoard->sHw.EpsBoardInfo.eddp.icu_location = EDDP_LOCATION_EXT_HOST;

  /* PNIP settings */
  pBoard->sHw.EpsBoardInfo.eddp.pnip.base_ptr =
      (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec200P.uPnIp.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddp.pnip.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec200P.uPnIp.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddp.pnip.size =
      pBoard->OpenDevice.uHwRes.as.uErtec200P.uPnIp.lSize;

  // calculate memory ranges in SDRAM based on avaliable SDRAM
  eps_hw_e200p_calculateSizeInSDRAM(
      pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uDirect.lSize, &nrtSize,
      &crtSize);

  /* SDRam settings for NRT */
  pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.base_ptr =
      (LSA_UINT8*)
          pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uDirect.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uDirect.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.size = nrtSize;

  /* SDRam settings for CRT (used for ext. CRT memory pool) */
  pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.base_ptr =
      (LSA_UINT8*)
          pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uDirect.uBase.pPtr +
      nrtSize;
  pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uDirect.lAdrAhb +
      nrtSize;
  pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.size = crtSize;

  /* PERIF ram settings (used for EDDP PERIF setup) */
  pBoard->sHw.EpsBoardInfo.eddp.perif_ram.base_ptr =
      (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec200P.uPerIf.uBase
          .pPtr;  // user adress mapped from AHB Adress to PERIF
  pBoard->sHw.EpsBoardInfo.eddp.perif_ram.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec200P.uPerIf
          .lAdrAhb;  // physical adress to AHB Adress to PERIF
  pBoard->sHw.EpsBoardInfo.eddp.perif_ram.size =
      pBoard->OpenDevice.uHwRes.as.uErtec200P.uPerIf.lSize;  // size of perif

  /* K32 ram settings (used for K32 DLL attach) */
  pBoard->sHw.EpsBoardInfo.eddp.k32_tcm.base_ptr =
      (LSA_UINT8*)
          pBoard->OpenDevice.uHwRes.as.uErtec200P.uKrisc32Tcm.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddp.k32_tcm.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec200P.uKrisc32Tcm.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddp.k32_tcm.size =
      pBoard->OpenDevice.uHwRes.as.uErtec200P.uKrisc32Tcm.lSize;

  /* APB-Peripherials SCRB */
  pBoard->sHw.EpsBoardInfo.eddp.apb_periph_scrb.base_ptr =
      (LSA_UINT8*)
          pBoard->OpenDevice.uHwRes.as.uErtec200P.uApbPer.uScrb.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddp.apb_periph_scrb.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec200P.uApbPer.uScrb.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddp.apb_periph_scrb.size =
      pBoard->OpenDevice.uHwRes.as.uErtec200P.uApbPer.uScrb.lSize;

  /* APB-Peripherials PERIF */
  pBoard->sHw.EpsBoardInfo.eddp.apb_periph_perif.base_ptr =
      (LSA_UINT8*)
          pBoard->OpenDevice.uHwRes.as.uErtec200P.uApbPer.uPerIf.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddp.apb_periph_perif.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec200P.uApbPer.uPerIf.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddp.apb_periph_perif.size =
      pBoard->OpenDevice.uHwRes.as.uErtec200P.uApbPer.uPerIf.lSize;

  /* setup NRT buffer */
  pBoard->sHw.EpsBoardInfo.nrt_mem.base_ptr =
      pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.base_ptr;
  pBoard->sHw.EpsBoardInfo.nrt_mem.phy_addr =
      pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.phy_addr;
  pBoard->sHw.EpsBoardInfo.nrt_mem.size =
      pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.size;

  /* Prepare process image settings (KRAM is used) */
  /* Note: we use the full size of KRAM, real size is calculated in PSI */
  pBoard->sHw.EpsBoardInfo.pi_mem.base_ptr =
      pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.base_ptr;
  pBoard->sHw.EpsBoardInfo.pi_mem.phy_addr =
      pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.phy_addr;
  pBoard->sHw.EpsBoardInfo.pi_mem.size =
      pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.size;

  /* setup HIF buffer => HIF HD on same processor */
  pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr =
      (LSA_UINT8*)
          pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwHif.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwHif.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.hif_mem.size =
      pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwHif.lSize;

  /* Set POF LED function (board specific) */
  pBoard->sHw.EpsBoardInfo.eddp.pof_led_fct = LSA_NULL; /* ToDo! */

  // MAC addresses
  // IF specific setup
  nrOfIF = 1;

  macCount = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lCtrMacAdr;

  for (ifIdx = 0; ifIdx < nrOfIF; ifIdx++) {
    // Use MAC from driver
    pMac = (EPS_MAC_PTR_TYPE)&pBoard->OpenDevice.uHwRes.uCommon.uRemanentData
               .lArrayMacAdr[ifIdx];

    pBoard->sHw.EpsBoardInfo.if_mac[ifIdx] = *pMac;
    pBoard->sHw.EpsBoardInfo.if_sys_handle[ifIdx] = &pBoard->sysDev;
  }

  // Port specific setup
  pBoard->sHw.EpsBoardInfo.nr_of_ports = 2;

  for (portIdx = 0; portIdx < pBoard->sHw.EpsBoardInfo.nr_of_ports; portIdx++) {
    macIdx = portIdx + nrOfIF;

    // Use next MAC from driver for user port
    pMac = (EPS_MAC_PTR_TYPE)&pBoard->OpenDevice.uHwRes.uCommon.uRemanentData
               .lArrayMacAdr[macIdx];

    pBoard->sHw.EpsBoardInfo.port_mac[portIdx + 1] = *pMac;
  }

  // Set valid and store the data for the HD
  pBoard->sHw.EpsBoardInfo.eddp.is_valid = LSA_TRUE;

  ///*------------------------------------------------*/

  eps_hw_e200p_init(pBoard->sHw.EpsBoardInfo.eddp.apb_periph_perif.base_ptr);

  return result;
}
#endif

#if (PSI_CFG_USE_EDDI == 1)
/**
 * init and set board parameters after OpenDevice of PnDevDriver for ertec400
 *
 * @param	pBoard				pointer to current board
 * structure
 * @param 	hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_open_ertec400(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard,
                                             LSA_UINT16 hd_id) {
  LSA_UINT16 result = EPS_PNDEV_RET_OK;

  EPS_MAC_PTR_TYPE pMac = LSA_NULL;
  LSA_UINT32 macCount, macIdx, ifIdx, portIdx;
  LSA_UINT16 nrOfIF = 1;

#ifdef EPS_PNDEVDRV_GEN_ARTIFICAL_MAC
  EPS_MAC_TYPE art_mac;
#endif

  EPS_ASSERT(pBoard != LSA_NULL); /* No more resources */

  eps_memset(&pBoard->sHw.EpsBoardInfo, 0, sizeof(pBoard->sHw.EpsBoardInfo));

  switch (pBoard->OpenDevice.uHwInfo.eBoard) {
    case ePNDEV_BOARD_CP1616:

      pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_CP1616;

      break;
    case ePNDEV_BOARD_DB_EB400_PCIE:

      pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_EB400_PCIE;

      break;
    default:
      result = EPS_PNDEV_RET_UNSUPPORTED;
      break;
  }

  pBoard->sysDev.hd_nr = hd_id;
  pBoard->sysDev.if_nr = 1;       // don't care
  pBoard->sysDev.pnio_if_nr = 0;  // don't care
  pBoard->sysDev.edd_comp_id = LSA_COMP_ID_EDDI;

  pBoard->sHw.EpsBoardInfo.hd_sys_handle = &pBoard->sysDev;
  pBoard->sHw.EpsBoardInfo.edd_type = LSA_COMP_ID_EDDI;

  // Init User to HW port mapping (used PSI GET HD PARAMS)
  eps_hw_init_board_port_param(&pBoard->sHw.EpsBoardInfo);

  pBoard->sHw.asic_type = EPS_PNDEV_ASIC_ERTEC400;
  pBoard->sHw.EpsBoardInfo.eddi.device_type = EDD_HW_TYPE_USED_ERTEC_400;

  /*------------------------*/
  // IRTE settings
  pBoard->sHw.EpsBoardInfo.eddi.irte.base_ptr =
      (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec400.uIrte.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddi.irte.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec400.uIrte.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddi.irte.size =
      pBoard->OpenDevice.uHwRes.as.uErtec400.uIrte.lSize;

  // KRAM settings
  pBoard->sHw.EpsBoardInfo.eddi.kram.base_ptr =
      (LSA_UINT8*)(pBoard->OpenDevice.uHwRes.as.uErtec400.uIrte.uBase.pPtr +
                   0x100000);  // PCIOX_KRAM_START_ERTEC400 aus pciox_hw.h
  pBoard->sHw.EpsBoardInfo.eddi.kram.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec400.uIrte.lAdrAhb +
      0x100000;  // PCIOX_KRAM_START_ERTEC400 aus pciox_hw_h
  pBoard->sHw.EpsBoardInfo.eddi.kram.size = EDDI_KRAM_SIZE_ERTEC400;

  // SDRAM settings (for NRT)

  pBoard->sHw.EpsBoardInfo.eddi.sdram.size = 0x300000;

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  // using SDRam mapping delivered by driver
  EPS_ASSERT(pBoard->sHw.EpsBoardInfo.eddi.sdram.size <=
             pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uDirect.lSize);

  pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr =
      (LSA_UINT8*)
          pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uDirect.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uDirect.lAdrAhb;
#else
  if (pBoard->bBoardFwUsed ==
      LSA_FALSE) /* We use host mem for NRT/DMACW if there is no HD Firmware */
  {
    EPS_ASSERT(
        pBoard->OpenDevice.uHwRes.as.uErtec400.uSharedHostSdram.uBase.pPtr !=
        LSA_NULL);

    pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr =
        (LSA_UINT8*)
            pBoard->OpenDevice.uHwRes.as.uErtec400.uSharedHostSdram.uBase.pPtr;
    pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr =
        pBoard->OpenDevice.uHwRes.as.uErtec400.uSharedHostSdram.lPhyAdr;
    pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr =
        (LSA_UINT32)eps_pndevdrv_calc_phy_pci_host_address(
            pBoard, pBoard->OpenDevice.uHwRes.as.uSoc1.uPciMaster,
            sizeof(pBoard->OpenDevice.uHwRes.as.uSoc1.uPciMaster) /
                sizeof(uPNCORE_PCI_HOST_PAR),
            (LSA_UINT8*)pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr);
  } else {
    EPS_ASSERT(pBoard->sHw.EpsBoardInfo.eddi.sdram.size <=
               pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uDirect.lSize);

    pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr =
        (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uDirect
            .uBase.pPtr;
    pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr =
        pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uDirect.lAdrAhb;
  }
#endif

  // Shared Mem settings
  pBoard->sHw.EpsBoardInfo.eddi.shared_mem.base_ptr =
      (LSA_UINT8*)
          pBoard->OpenDevice.uHwRes.as.uErtec400.uSharedHostSdram.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddi.shared_mem.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec400.uSharedHostSdram.lPhyAdr;
  pBoard->sHw.EpsBoardInfo.eddi.shared_mem.size =
      pBoard->OpenDevice.uHwRes.as.uErtec400.uSharedHostSdram.lSize;

  // APB-Periphericals SCRB
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.base_ptr =
      (LSA_UINT8*)
          pBoard->OpenDevice.uHwRes.as.uErtec400.uApbPer.uScrb.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec400.uApbPer.uScrb.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.size =
      pBoard->OpenDevice.uHwRes.as.uErtec400.uApbPer.uScrb.lSize;

  // APB-Peripherals TIMER
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.base_ptr = 0;
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.phy_addr = 0;
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.size = 0;

  // APB-Peripherials GPIO
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.base_ptr =
      (LSA_UINT8*)
          pBoard->OpenDevice.uHwRes.as.uErtec400.uApbPer.uGpio.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec400.uApbPer.uGpio.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.size =
      pBoard->OpenDevice.uHwRes.as.uErtec400.uApbPer.uGpio.lSize;

  // Set default CycleBaseFactor
  pBoard->sHw.EpsBoardInfo.eddi.cycle_base_factor = 32;

// note: not setting the EDDI spec functions for this runlevel
//       if EDDI runs not on this level the functions never called otherwise
//       FATAL
#if (((PSI_CFG_USE_EDDI == 1) && (defined(EDDI_CFG_ERTEC_400))) || \
     (EPS_PLF == EPS_PLF_ADONIS_X86))
  // Prepare the board specific HW functions and GPIO numbers
  // Note used for PHY LED, PLL and LEDs
  pBoard->sHw.EpsBoardInfo.eddi.has_ext_pll = LSA_TRUE;
  pBoard->sHw.EpsBoardInfo.eddi.extpll_out_gpio_nr = 25;
  pBoard->sHw.EpsBoardInfo.eddi.extpll_in_gpio_nr = 25;

  // Set POF, PHY-Led and PLL functions
  pBoard->sHw.EpsBoardInfo.eddi.set_pll_port_fct = eps_set_pllport_ERTEC400;
  pBoard->sHw.EpsBoardInfo.eddi.blink_start_fct = LSA_NULL; /* Done by EDDI */
  pBoard->sHw.EpsBoardInfo.eddi.blink_end_fct = LSA_NULL;   /* Done by EDDI */
  pBoard->sHw.EpsBoardInfo.eddi.blink_set_mode_fct =
      LSA_NULL; /* Done by EDDI */
  pBoard->sHw.EpsBoardInfo.eddi.pof_led_fct = eps_pof_set_led_ERTEC400;

  // Set I2C functions
  pBoard->sHw.EpsBoardInfo.eddi.i2c_set_scl_low_high =
      eps_i2c_set_scl_low_high_ERTEC400;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_set_sda_low_high =
      eps_i2c_set_sda_low_high_ERTEC400;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_sda_read = eps_i2c_sda_read_ERTEC400;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_set_sda_in_out =
      eps_i2c_set_sda_in_out_ERTEC400;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_set_scl_in_out =
      eps_i2c_set_scl_in_out_ERTEC400;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_select = eps_i2c_select_ERTEC400;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_read_offset =
      eps_i2c_ll_read_offset_ERTEC400;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_write_offset =
      eps_i2c_ll_write_offset_ERTEC400;

  // SII settings (EDDI runs on EPC environment, ExtTimer exists)
  pBoard->sHw.EpsBoardInfo.eddi.SII_IrqSelector = EDDI_SII_IRQ_HP;
  pBoard->sHw.EpsBoardInfo.eddi.SII_IrqNumber = EDDI_SII_IRQ_1;

#if defined(EDDI_CFG_SII_EXTTIMER_MODE_ON)
  pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval =
      1000UL;  // 0, 250, 500, 1000 in us
#else
  pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval =
      0UL;  // 0, 250, 500, 1000 in us
#endif

  // NRT memory settings
  eps_ertec400_FillNrtMemParams(&pBoard->sHw.EpsBoardInfo);
#endif

  switch (pBoard->pDeviceInfo->eBoard) {
    case (ePNDEV_BOARD_CP1616): {
      // setup portmapping (=1:1)
      pBoard->sHw.EpsBoardInfo.port_map[0][1].hw_phy_nr = 0;
      pBoard->sHw.EpsBoardInfo.port_map[0][1].hw_port_id = 1;

      pBoard->sHw.EpsBoardInfo.port_map[0][2].hw_phy_nr = 1;
      pBoard->sHw.EpsBoardInfo.port_map[0][2].hw_port_id = 2;

      pBoard->sHw.EpsBoardInfo.port_map[0][3].hw_phy_nr = 2;
      pBoard->sHw.EpsBoardInfo.port_map[0][3].hw_port_id = 3;

      pBoard->sHw.EpsBoardInfo.port_map[0][4].hw_phy_nr = 3;
      pBoard->sHw.EpsBoardInfo.port_map[0][4].hw_port_id = 4;

      break;
    }
    case (ePNDEV_BOARD_DB_EB400_PCIE): {
      // setup portmapping (= 1:4, 2:3, 3:2, 4:1)
      pBoard->sHw.EpsBoardInfo.port_map[0][1].hw_phy_nr = 3;
      pBoard->sHw.EpsBoardInfo.port_map[0][1].hw_port_id = 4;

      pBoard->sHw.EpsBoardInfo.port_map[0][2].hw_phy_nr = 2;
      pBoard->sHw.EpsBoardInfo.port_map[0][2].hw_port_id = 3;

      pBoard->sHw.EpsBoardInfo.port_map[0][3].hw_phy_nr = 1;
      pBoard->sHw.EpsBoardInfo.port_map[0][3].hw_port_id = 2;

      pBoard->sHw.EpsBoardInfo.port_map[0][4].hw_phy_nr = 0;
      pBoard->sHw.EpsBoardInfo.port_map[0][4].hw_port_id = 1;

      break;
    }
    default: {
      // setup portmapping (=1:1)
      pBoard->sHw.EpsBoardInfo.port_map[0][1].hw_phy_nr = 0;
      pBoard->sHw.EpsBoardInfo.port_map[0][1].hw_port_id = 1;

      pBoard->sHw.EpsBoardInfo.port_map[0][2].hw_phy_nr = 1;
      pBoard->sHw.EpsBoardInfo.port_map[0][2].hw_port_id = 2;

      pBoard->sHw.EpsBoardInfo.port_map[0][3].hw_phy_nr = 2;
      pBoard->sHw.EpsBoardInfo.port_map[0][3].hw_port_id = 3;

      pBoard->sHw.EpsBoardInfo.port_map[0][4].hw_phy_nr = 3;
      pBoard->sHw.EpsBoardInfo.port_map[0][4].hw_port_id = 4;

      break;
    }
  }

  // Prepare process image settings (KRAM is used)
  // Note: we use the full size of KRAM, real size is calculated in PSI
  pBoard->sHw.EpsBoardInfo.pi_mem.base_ptr =
      pBoard->sHw.EpsBoardInfo.eddi.kram.base_ptr;
  pBoard->sHw.EpsBoardInfo.pi_mem.phy_addr =
      pBoard->sHw.EpsBoardInfo.eddi.kram.phy_addr;
  pBoard->sHw.EpsBoardInfo.pi_mem.size =
      pBoard->sHw.EpsBoardInfo.eddi.kram.size;

  // setup HIF buffer
  pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr =
      (LSA_UINT8*)
          pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwHif.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwHif.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.hif_mem.size =
      pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwHif.lSize;

  /*------------------------*/

  // MAC addresses
  // IF specific setup
  nrOfIF = 1;

  macCount = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lCtrMacAdr;

  for (ifIdx = 0; ifIdx < nrOfIF; ifIdx++) {
#ifdef EPS_PNDEVDRV_GEN_ARTIFICAL_MAC
    // MAC : 0x0E : 0x0F : BusNr : DeviceNr : FctNr : running #
    art_mac.mac[0] = 0x0E;
    art_mac.mac[1] = 0x0F;
    art_mac.mac[2] = pBoard->Location.uBusNr;
    art_mac.mac[3] = pBoard->Location.uDeviceNr;
    art_mac.mac[4] = pBoard->Location.uFctNr;
    art_mac.mac[5] = ifIdx;

    pMac = &art_mac;
#else
    // Use MAC from driver
    pMac = (EPS_MAC_PTR_TYPE)&pBoard->OpenDevice.uHwRes.uCommon.uRemanentData
               .lArrayMacAdr[ifIdx];
#endif
    pBoard->sHw.EpsBoardInfo.if_mac[ifIdx] = *pMac;
    pBoard->sHw.EpsBoardInfo.if_sys_handle[ifIdx] = &pBoard->sysDev;
  }

  // Port specific setup
  pBoard->sHw.EpsBoardInfo.nr_of_ports = 4;

  for (portIdx = 0; portIdx < pBoard->sHw.EpsBoardInfo.nr_of_ports; portIdx++) {
    macIdx = portIdx + nrOfIF;
#ifdef EPS_PNDEVDRV_GEN_ARTIFICAL_MAC
    art_mac.mac[5] = art_mac.mac[5] + 1;

    pMac = &art_mac;
#else
    // Use next MAC from driver for user port
    pMac = (EPS_MAC_PTR_TYPE)&pBoard->OpenDevice.uHwRes.uCommon.uRemanentData
               .lArrayMacAdr[macIdx];
#endif

    pBoard->sHw.EpsBoardInfo.port_mac[portIdx + 1] = *pMac;
  }

  // Set valid and store the data for the HD
  pBoard->sHw.EpsBoardInfo.eddi.is_valid = LSA_TRUE;

///*------------------------------------------------*/

// note: not setting the EDDI spec functions for this runlevel
//       if EDDI runs not on this level the functions never called otherwise
//       FATAL
#if (((PSI_CFG_USE_EDDI == 1) && (defined(EDDI_CFG_ERTEC_400))) || \
     (EPS_PLF == EPS_PLF_ADONIS_X86))
  if (result == EPS_PNDEV_RET_OK) {
    eps_init_hw_ertec400(hd_id);
  }
#endif

  return result;
}

/**
 * init and set board parameters after OpenDevice of PnDevDriver for ertec200
 *
 * @param	pBoard				pointer to current board
 * structure
 * @param 	hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_open_ertec200(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard,
                                             LSA_UINT16 hd_id) {
  LSA_UINT16 result = EPS_PNDEV_RET_OK;

  EPS_MAC_PTR_TYPE pMac = LSA_NULL;
  LSA_UINT32 macCount, macIdx, ifIdx, portIdx;
  LSA_UINT16 nrOfIF = 1;

#ifdef EPS_PNDEVDRV_GEN_ARTIFICAL_MAC
  EPS_MAC_TYPE art_mac;
#endif

  eps_memset(&pBoard->sHw.EpsBoardInfo, 0, sizeof(pBoard->sHw.EpsBoardInfo));

  pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_EB200;

  pBoard->sysDev.hd_nr = hd_id;
  pBoard->sysDev.if_nr = 1;       // don't care
  pBoard->sysDev.pnio_if_nr = 0;  // don't care
  pBoard->sysDev.edd_comp_id = LSA_COMP_ID_EDDI;

  pBoard->sHw.EpsBoardInfo.hd_sys_handle = &pBoard->sysDev;
  pBoard->sHw.EpsBoardInfo.edd_type = LSA_COMP_ID_EDDI;
  pBoard->sHw.EpsBoardInfo.eddi.device_type = EDD_HW_TYPE_USED_ERTEC_200;

  // Init User to HW port mapping (used PSI GET HD PARAMS)
  eps_hw_init_board_port_param(&pBoard->sHw.EpsBoardInfo);

  pBoard->sHw.asic_type = EPS_PNDEV_ASIC_ERTEC200;

  /*------------------------*/
  // IRTE settings
  pBoard->sHw.EpsBoardInfo.eddi.irte.base_ptr =
      (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec200.uIrte.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddi.irte.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec200.uIrte.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddi.irte.size =
      pBoard->OpenDevice.uHwRes.as.uErtec200.uIrte.lSize;

  // KRAM settings
  pBoard->sHw.EpsBoardInfo.eddi.kram.base_ptr =
      (LSA_UINT8*)(pBoard->OpenDevice.uHwRes.as.uErtec200.uIrte.uBase.pPtr +
                   0x100000);  // PCIOX_KRAM_START_ERTEC200 aus pciox_hw.h
  pBoard->sHw.EpsBoardInfo.eddi.kram.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec200.uIrte.lAdrAhb +
      0x100000;  // PCIOX_KRAM_START_ERTEC200 aus pciox_hw.h
  pBoard->sHw.EpsBoardInfo.eddi.kram.size = EDDI_KRAM_SIZE_ERTEC200; /* 64 kB */

  // SDRAM settings (for NRT)
  pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr =
      (LSA_UINT8*)
          pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uDirect.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uDirect.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddi.sdram.size =
      pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uDirect.lSize;

  // Shared Mem settings
  pBoard->sHw.EpsBoardInfo.eddi.shared_mem.base_ptr =
      0;  // Doesn't exist
          // //(LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec200.uSharedHostSdram.pBase;
  pBoard->sHw.EpsBoardInfo.eddi.shared_mem.phy_addr =
      0;  // Doesn't exist
          // //pBoard->OpenDevice.uHwRes.as.uErtec200.uSharedHostSdram.lPhyAdr;
  pBoard->sHw.EpsBoardInfo.eddi.shared_mem.size =
      0;  // Doesn't exist
          // //pBoard->OpenDevice.uHwRes.as.uErtec200.uSharedHostSdram.lSize;

  // APB-Periphericals SCRB
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.base_ptr =
      (LSA_UINT8*)
          pBoard->OpenDevice.uHwRes.as.uErtec200.uApbPer.uScrb.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec200.uApbPer.uScrb.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.size =
      pBoard->OpenDevice.uHwRes.as.uErtec200.uApbPer.uScrb
          .lSize; /* 164 Byte - EB200 Handbook */

  // APB-Peripherials GPIO
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.base_ptr =
      (LSA_UINT8*)
          pBoard->OpenDevice.uHwRes.as.uErtec200.uApbPer.uGpio.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec200.uApbPer.uGpio.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.size =
      pBoard->OpenDevice.uHwRes.as.uErtec200.uApbPer.uGpio.lSize;

  // Set default CycleBaseFactor
  pBoard->sHw.EpsBoardInfo.eddi.cycle_base_factor = 32;

  // Prepare the board specific HW functions and GPIO numbers
  // Note used for PHY LED, PLL and LEDs
  pBoard->sHw.EpsBoardInfo.eddi.has_ext_pll = LSA_TRUE;
  pBoard->sHw.EpsBoardInfo.eddi.extpll_out_gpio_nr = 25;
  pBoard->sHw.EpsBoardInfo.eddi.extpll_in_gpio_nr = 25;

// note: not setting the EDDI spec functions for this runlevel
//       if EDDI runs not on this level the functions never called otherwise
//       FATAL
#if (((PSI_CFG_USE_EDDI == 1) && (defined(EDDI_CFG_ERTEC_200))) || \
     (EPS_PLF == EPS_PLF_ADONIS_X86))
  // Set POF, PHY-Led and PLL functions
  pBoard->sHw.EpsBoardInfo.eddi.set_pll_port_fct = eps_set_pllport_ERTEC200;
  pBoard->sHw.EpsBoardInfo.eddi.blink_start_fct = eps_phy_led_blink_begin_EB200;
  pBoard->sHw.EpsBoardInfo.eddi.blink_end_fct = eps_phy_led_blink_end_EB200;
  pBoard->sHw.EpsBoardInfo.eddi.blink_set_mode_fct =
      eps_phy_led_blink_set_mode_EB200;
  pBoard->sHw.EpsBoardInfo.eddi.pof_led_fct = eps_pof_set_led_EB200;

  // Set I2C functions
  pBoard->sHw.EpsBoardInfo.eddi.i2c_set_scl_low_high =
      eps_i2c_set_scl_low_high_EB200;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_set_sda_low_high =
      eps_i2c_set_sda_low_high_EB200;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_sda_read = eps_i2c_sda_read_EB200;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_set_sda_in_out =
      eps_i2c_set_sda_in_out_EB200;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_set_scl_in_out =
      eps_i2c_set_scl_in_out_EB200;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_select = eps_i2c_select_EB200;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_read_offset =
      eps_i2c_ll_read_offset_EB200;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_write_offset =
      eps_i2c_ll_write_offset_EB200;

  // SII settings (EDDI runs on EPC environment, ExtTimer exists)
  pBoard->sHw.EpsBoardInfo.eddi.SII_IrqSelector = EDDI_SII_IRQ_HP;
  pBoard->sHw.EpsBoardInfo.eddi.SII_IrqNumber = EDDI_SII_IRQ_1;

#if defined(EDDI_CFG_SII_EXTTIMER_MODE_ON)
  pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval =
      1000UL;  // 0, 250, 500, 1000 in us
#else
  pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval =
      0UL;  // 0, 250, 500, 1000 in us
#endif

  // NRT memory settings
  eps_ertec200_FillNrtMemParams(&pBoard->sHw.EpsBoardInfo);
#endif

  // setup portmapping (=1:1)
  pBoard->sHw.EpsBoardInfo.port_map[0][1].hw_phy_nr = 0;
  pBoard->sHw.EpsBoardInfo.port_map[0][1].hw_port_id = 1;

  pBoard->sHw.EpsBoardInfo.port_map[0][2].hw_phy_nr = 1;
  pBoard->sHw.EpsBoardInfo.port_map[0][2].hw_port_id = 2;

  // Prepare process image settings (KRAM is used)
  // Note: we use the full size of KRAM, real size is calculated in PSI
  pBoard->sHw.EpsBoardInfo.pi_mem.base_ptr =
      pBoard->sHw.EpsBoardInfo.eddi.kram.base_ptr;
  pBoard->sHw.EpsBoardInfo.pi_mem.phy_addr =
      pBoard->sHw.EpsBoardInfo.eddi.kram.phy_addr;
  pBoard->sHw.EpsBoardInfo.pi_mem.size =
      pBoard->sHw.EpsBoardInfo.eddi.kram.size;

  // setup HIF buffer
  pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr =
      (LSA_UINT8*)
          pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwHif.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwHif.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.hif_mem.size =
      pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwHif.lSize;

  // MAC addresses
  // IF specific setup
  nrOfIF = 1;

  macCount = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lCtrMacAdr;

  for (ifIdx = 0; ifIdx < nrOfIF; ifIdx++) {
#ifdef EPS_PNDEVDRV_GEN_ARTIFICAL_MAC
    // MAC : 0x0E : 0x0F : BusNr : DeviceNr : FctNr : running #
    art_mac.mac[0] = 0x0E;
    art_mac.mac[1] = 0x0F;
    art_mac.mac[2] = pBoard->Location.uBusNr;
    art_mac.mac[3] = pBoard->Location.uDeviceNr;
    art_mac.mac[4] = pBoard->Location.uFctNr;
    art_mac.mac[5] = ifIdx;

    pMac = &art_mac;
#else
    // Use MAC from driver
    pMac = (EPS_MAC_PTR_TYPE)&pBoard->OpenDevice.uHwRes.uCommon.uRemanentData
               .lArrayMacAdr[ifIdx];
#endif
    pBoard->sHw.EpsBoardInfo.if_mac[ifIdx] = *pMac;
    pBoard->sHw.EpsBoardInfo.if_sys_handle[ifIdx] = &pBoard->sysDev;
  }

  // Port specific setup
  pBoard->sHw.EpsBoardInfo.nr_of_ports = 2;

  for (portIdx = 0; portIdx < pBoard->sHw.EpsBoardInfo.nr_of_ports; portIdx++) {
    macIdx = portIdx + nrOfIF;
#ifdef EPS_PNDEVDRV_GEN_ARTIFICAL_MAC
    art_mac.mac[5] = art_mac.mac[5] + 1;

    pMac = &art_mac;
#else
    // Use next MAC from driver for user port
    pMac = (EPS_MAC_PTR_TYPE)&pBoard->OpenDevice.uHwRes.uCommon.uRemanentData
               .lArrayMacAdr[macIdx];
#endif

    pBoard->sHw.EpsBoardInfo.port_mac[portIdx + 1] = *pMac;
  }

  // Set valid and store the data for the HD
  pBoard->sHw.EpsBoardInfo.eddi.is_valid = LSA_TRUE;

///*------------------------------------------------*/

#if (((PSI_CFG_USE_EDDI == 1) && (defined(EDDI_CFG_ERTEC_200))) || \
     (EPS_PLF == EPS_PLF_ADONIS_X86))
  if (result == EPS_PNDEV_RET_OK) {
    eps_init_hw_ertec200(hd_id);
  }
#endif

  return result;
}

/**
 * init and set board parameters after OpenDevice of PnDevDriver for soc1
 *
 * @param	pBoard				pointer to current board
 * structure
 * @param 	hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_open_soc1(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard,
                                         LSA_UINT16 hd_id) {
  LSA_UINT16 result = EPS_PNDEV_RET_OK;

  LSA_UINT32 macCount, macIdx, ifIdx, portIdx;
  LSA_UINT32 nrtSize = 0;
  LSA_UINT32 crtSize = 0;
  LSA_UINT16 nrOfIF = 1;
  EPS_MAC_PTR_TYPE pMac = LSA_NULL;

  LSA_UNUSED_ARG(nrtSize);
  LSA_UNUSED_ARG(crtSize);

#ifdef EPS_PNDEVDRV_GEN_ARTIFICAL_MAC
  EPS_MAC_TYPE art_mac;
#endif

  eps_memset(&pBoard->sHw.EpsBoardInfo, 0, sizeof(pBoard->sHw.EpsBoardInfo));

  pBoard->sysDev.hd_nr = hd_id;
  pBoard->sysDev.if_nr = 1;       // don't care
  pBoard->sysDev.pnio_if_nr = 0;  // don't care
  pBoard->sysDev.edd_comp_id = LSA_COMP_ID_EDDI;

  pBoard->sHw.EpsBoardInfo.hd_sys_handle = &pBoard->sysDev;
  pBoard->sHw.EpsBoardInfo.edd_type = LSA_COMP_ID_EDDI;
  pBoard->sHw.EpsBoardInfo.eddi.device_type = EDD_HW_TYPE_USED_SOC;
  pBoard->sHw.EpsBoardInfo.eddi.cycle_base_factor = 32;

  pBoard->sHw.asic_type = EPS_PNDEV_ASIC_IRTEREV7;

  // Init User to HW port mapping (used PSI GET HD PARAMS)
  eps_hw_init_board_port_param(&pBoard->sHw.EpsBoardInfo);

  switch (pBoard->OpenDevice.uHwInfo.eBoard) {
    case ePNDEV_BOARD_DB_SOC1_PCI:

      pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_SOC1_PCI;

      // setup portmapping (1:2, 2:3, 3:1)
      pBoard->sHw.EpsBoardInfo.port_map[0][1].hw_phy_nr = 1;
      pBoard->sHw.EpsBoardInfo.port_map[0][1].hw_port_id = 2;

      pBoard->sHw.EpsBoardInfo.port_map[0][2].hw_phy_nr = 2;
      pBoard->sHw.EpsBoardInfo.port_map[0][2].hw_port_id = 3;

      pBoard->sHw.EpsBoardInfo.port_map[0][3].hw_phy_nr = 0;
      pBoard->sHw.EpsBoardInfo.port_map[0][3].hw_port_id = 1;
      break;
    case ePNDEV_BOARD_DB_SOC1_PCIE:

      pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_SOC1_PCIE;

      // setup portmapping (=1:1)
      pBoard->sHw.EpsBoardInfo.port_map[0][1].hw_phy_nr = 0;
      pBoard->sHw.EpsBoardInfo.port_map[0][1].hw_port_id = 1;

      pBoard->sHw.EpsBoardInfo.port_map[0][2].hw_phy_nr = 1;
      pBoard->sHw.EpsBoardInfo.port_map[0][2].hw_port_id = 2;

      pBoard->sHw.EpsBoardInfo.port_map[0][3].hw_phy_nr = 2;
      pBoard->sHw.EpsBoardInfo.port_map[0][3].hw_port_id = 3;
      break;
    default:
      result = EPS_PNDEV_RET_UNSUPPORTED;
      break;
  }

  // IRTE settings (light)
  pBoard->sHw.EpsBoardInfo.eddi.irte.base_ptr =
      (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uSoc1.uIrte.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddi.irte.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uSoc1.uIrte.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddi.irte.size =
      pBoard->OpenDevice.uHwRes.as.uSoc1.uIrte.lSize;

  // KRAM settings (light)
  // Note: for PI Image the IOCC ram is used
  pBoard->sHw.EpsBoardInfo.eddi.kram.base_ptr =
      (LSA_UINT8*)(pBoard->OpenDevice.uHwRes.as.uSoc1.uIrte.uBase.pPtr +
                   0x100000);  // PCIOX_KRAM_START_SOC1 aus pciox_hw.h
  pBoard->sHw.EpsBoardInfo.eddi.kram.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uSoc1.uIrte.lAdrAhb +
      0x100000;  // PCIOX_KRAM_START_SOC1 aus pciox_hw.h
  pBoard->sHw.EpsBoardInfo.eddi.kram.size = EDDI_KRAM_SIZE_SOC;

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  {
    // using the mapped memory from driver for NRT
    LSA_UINT32 sdSize =
        LSA_MIN(0x00C00000, pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram
                                .uDirect.lSize);  // 12 Mb minimum recommended
    pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr =
        (LSA_UINT8*)
            pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uDirect.uBase.pPtr;
    pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr =
        pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uDirect.lAdrAhb;
    pBoard->sHw.EpsBoardInfo.eddi.sdram.size = sdSize;
  }
#else
  // SDRAM settings (for NRT) (light)
  // Annahme: HD runs on basic
  {
    LSA_UINT32 sdSize =
        LSA_MIN(0x00C00000, pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram
                                .uDirect.lSize);  // 12 Mb minimum recommended

    if (pBoard->bBoardFwUsed == LSA_FALSE) /* We use host mem for NRT/DMACW if
                                              there is no HD Firmware */
    {
      EPS_ASSERT(sdSize <=
                 pBoard->OpenDevice.uHwRes.as.uSoc1.uSharedHostSdram.lSize);
      EPS_ASSERT(
          pBoard->OpenDevice.uHwRes.as.uSoc1.uSharedHostSdram.uBase.pPtr !=
          LSA_NULL);

      pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr =
          (LSA_UINT8*)
              pBoard->OpenDevice.uHwRes.as.uSoc1.uSharedHostSdram.uBase.pPtr;
      pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr =
          pBoard->OpenDevice.uHwRes.as.uSoc1.uSharedHostSdram.lPhyAdr;
      pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr =
          (LSA_UINT32)eps_pndevdrv_calc_phy_pci_host_address(
              pBoard, pBoard->OpenDevice.uHwRes.as.uSoc1.uPciMaster,
              sizeof(pBoard->OpenDevice.uHwRes.as.uSoc1.uPciMaster) /
                  sizeof(uPNCORE_PCI_HOST_PAR),
              (LSA_UINT8*)pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr);
    } else {
      pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr =
          (LSA_UINT8*)
              pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uDirect.uBase.pPtr +
          0x400000;
      pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr =
          pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uDirect.lAdrAhb +
          0x400000;
    }

    pBoard->sHw.EpsBoardInfo.eddi.sdram.size = sdSize;
  }
#endif

  // APB-Periphericals SCRB (light)
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.base_ptr =
      (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uScrb.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uScrb.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.size =
      pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uScrb.lSize;

  // APB-Periphericals GPIO (light)
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.base_ptr =
      (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uGpio.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uGpio.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.size =
      pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uGpio.lSize;

  // APB-Periphericals TIMER (light)
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.base_ptr =
      (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uTimer.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uTimer.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.size =
      pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uTimer.lSize;
  // pBoard->sHw.EpsBoardInfo.eddi.info_exttimer.TimerStarted = LSA_FALSE;

  // IOCC settings (light)
  pBoard->sHw.EpsBoardInfo.eddi.iocc.base_ptr =
      (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uSoc1.uIocc.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.eddi.iocc.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uSoc1.uIocc.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.eddi.iocc.size =
      pBoard->OpenDevice.uHwRes.as.uSoc1.uIocc.lSize;

  // I2C setting
  pBoard->sHw.EpsBoardInfo.eddi.i2c_base_ptr =
      (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uI2c.uBase.pPtr;

  pBoard->sHw.EpsBoardInfo.eddi.paea.base_ptr =
      (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uSoc1.uIocc.uBase.pPtr +
      EPS_IOCC_PAEA_START_SOC1;
  pBoard->sHw.EpsBoardInfo.eddi.paea.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uSoc1.uIocc.lAdrAhb +
      EPS_IOCC_PAEA_START_SOC1;
  pBoard->sHw.EpsBoardInfo.eddi.paea.size =
      (EPS_U_IOCC_PA_EA_DIRECT_end - EPS_U_IOCC_PA_EA_DIRECT_start) / 4 +
      1;  // RAM size

  pBoard->sHw.EpsBoardInfo.eddi.has_ext_pll = LSA_TRUE;
  pBoard->sHw.EpsBoardInfo.eddi.extpll_out_gpio_nr = 156;  // use ISO_OUT4
  pBoard->sHw.EpsBoardInfo.eddi.extpll_in_gpio_nr = 94;    // use ISO_IN0

// note: not setting the EDDI spec functions for this runlevel
//       if EDDI runs not on this level the functions never called otherwise
//       FATAL
#if ((PSI_CFG_USE_EDDI == 1) && (defined(EDDI_CFG_SOC)))
  // Set POF, PHY-Led and PLL functions
  pBoard->sHw.EpsBoardInfo.eddi.set_pll_port_fct = eps_set_pllport_SOC;
  pBoard->sHw.EpsBoardInfo.eddi.blink_start_fct = LSA_NULL; /* Done by EDDI */
  pBoard->sHw.EpsBoardInfo.eddi.blink_end_fct = LSA_NULL;   /* Done by EDDI */
  pBoard->sHw.EpsBoardInfo.eddi.blink_set_mode_fct =
      LSA_NULL; /* Done by EDDI */
  pBoard->sHw.EpsBoardInfo.eddi.pof_led_fct = eps_pof_set_led_SOC;

  // Set I2C functions
  pBoard->sHw.EpsBoardInfo.eddi.i2c_set_scl_low_high =
      eps_i2c_set_scl_low_high_SOC;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_set_sda_low_high =
      eps_i2c_set_sda_low_high_SOC;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_sda_read = eps_i2c_sda_read_SOC;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_set_sda_in_out = eps_i2c_set_sda_in_out_SOC;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_set_scl_in_out = eps_i2c_set_scl_in_out_SOC;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_select = eps_i2c_select_SOC;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_read_offset = eps_i2c_ll_read_offset_SOC;
  pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_write_offset =
      eps_i2c_ll_write_offset_SOC;

  // SII settings (EDDI runs on PC environment, no ExtTimer exists)
  pBoard->sHw.EpsBoardInfo.eddi.SII_IrqSelector = EDDI_SII_IRQ_HP;
  pBoard->sHw.EpsBoardInfo.eddi.SII_IrqNumber = EDDI_SII_IRQ_1;

#if defined(EDDI_CFG_SII_EXTTIMER_MODE_ON)
  pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval =
      1000UL;  // 0, 250, 500, 1000 in us
#else
  pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval =
      0UL;  // 0, 250, 500, 1000 in us
#endif

  // NRT memory settings #EF ? richtige Funktion
  eps_soc_FillNrtMemParams(&pBoard->sHw.EpsBoardInfo);
#endif

  // setup PI (PAEA is used, with full size
  // Note: we use the full size of PAEA, real size is calculated in PSI
  pBoard->sHw.EpsBoardInfo.pi_mem.base_ptr =
      pBoard->sHw.EpsBoardInfo.eddi.paea.base_ptr;
  pBoard->sHw.EpsBoardInfo.pi_mem.phy_addr =
      pBoard->sHw.EpsBoardInfo.eddi.paea.phy_addr;
  pBoard->sHw.EpsBoardInfo.pi_mem.size =
      pBoard->sHw.EpsBoardInfo.eddi.paea.size;

  // setup HIF buffer
  pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr =
      (LSA_UINT8*)
          pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwHif.uBase.pPtr;
  pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr =
      pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwHif.lAdrAhb;
  pBoard->sHw.EpsBoardInfo.hif_mem.size =
      pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwHif.lSize;

  // MAC addresses
  // IF specific setup
  nrOfIF = 1;

  macCount = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lCtrMacAdr;

  for (ifIdx = 0; ifIdx < nrOfIF; ifIdx++) {
#ifdef EPS_PNDEVDRV_GEN_ARTIFICAL_MAC
    // MAC : 0x0E : 0x0F : BusNr : DeviceNr : FctNr : running #
    art_mac.mac[0] = 0x0E;
    art_mac.mac[1] = 0x0F;
    art_mac.mac[2] = pBoard->Location.uBusNr;
    art_mac.mac[3] = pBoard->Location.uDeviceNr;
    art_mac.mac[4] = pBoard->Location.uFctNr;
    art_mac.mac[5] = ifIdx;

    pMac = &art_mac;
#else
    // Use MAC from driver
    pMac = (EPS_MAC_PTR_TYPE)&pBoard->OpenDevice.uHwRes.uCommon.uRemanentData
               .lArrayMacAdr[ifIdx];
#endif
    pBoard->sHw.EpsBoardInfo.if_mac[ifIdx] = *pMac;
    pBoard->sHw.EpsBoardInfo.if_sys_handle[ifIdx] = &pBoard->sysDev;
  }

  // Port specific setup
  pBoard->sHw.EpsBoardInfo.nr_of_ports = 3;

  for (portIdx = 0; portIdx < pBoard->sHw.EpsBoardInfo.nr_of_ports; portIdx++) {
    macIdx = portIdx + nrOfIF;
#ifdef EPS_PNDEVDRV_GEN_ARTIFICAL_MAC
    art_mac.mac[5] = art_mac.mac[5] + 1;

    pMac = &art_mac;
#else
    // Use next MAC from driver for user port
    pMac = (EPS_MAC_PTR_TYPE)&pBoard->OpenDevice.uHwRes.uCommon.uRemanentData
               .lArrayMacAdr[macIdx];
#endif

    pBoard->sHw.EpsBoardInfo.port_mac[portIdx + 1] = *pMac;
  }

  // Set valid and store the data for the HD
  pBoard->sHw.EpsBoardInfo.eddi.is_valid = LSA_TRUE;

#if ((PSI_CFG_USE_EDDI == 1) && (defined(EDDI_CFG_SOC)))
  if (result == EPS_PNDEV_RET_OK) {
    eps_init_hw_SOC(hd_id);
  }
#endif

  return result;
}

#endif

#if ((PSI_CFG_USE_EDDS == 1) && (defined EDDS_CFG_HW_KSZ88XX))
/**
 * init and set board parameters after OpenDevice of PnDevDriver for ksz8841
 *
 * @param	pBoard				pointer to current board
 * structure
 * @param 	hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_open_ksz8841(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard,
                                            LSA_UINT16 hd_id) {
  LSA_UINT16 result = EPS_PNDEV_RET_OK;

  result = eps_ksz88xx_open(
      &pBoard->uEddsKSZ88XX, &pBoard->sHw, &pBoard->sysDev,
      (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uKSZ8841.uBar0.uBase.pPtr, hd_id,
      EPS_KSZ88XX_PCI_VENDOR_ID, EPS_KSZ88XX_PCI_DEVICE_ID_KSZ8841);

  if (result == EPS_PNDEV_RET_OK)
  // success
  {
    // setup NRT buffer (local mem is used)
    pBoard->sHw.EpsBoardInfo.nrt_mem.size =
        pBoard->OpenDevice.uHwRes.as.uKSZ8841.uSharedHostSdram.lSize;
    pBoard->sHw.EpsBoardInfo.nrt_mem.base_ptr =
        (LSA_UINT8*)
            pBoard->OpenDevice.uHwRes.as.uKSZ8841.uSharedHostSdram.uBase.pPtr;
    pBoard->sHw.EpsBoardInfo.nrt_mem.phy_addr =
        pBoard->OpenDevice.uHwRes.as.uKSZ8841.uSharedHostSdram.lPhyAdr;
    EPS_ASSERT(pBoard->sHw.EpsBoardInfo.nrt_mem.base_ptr != LSA_NULL);
  }

  return result;
}

/**
 * init and set board parameters after OpenDevice of PnDevDriver for ksz8841
 *
 * @param	pBoard				pointer to current board
 * structure
 * @param 	hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_open_ksz8842(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard,
                                            LSA_UINT16 hd_id) {
  LSA_UINT16 result = EPS_PNDEV_RET_OK;

  result = eps_ksz88xx_open(
      &pBoard->uEddsKSZ88XX, &pBoard->sHw, &pBoard->sysDev,
      (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uKSZ8842.uBar0.uBase.pPtr, hd_id,
      EPS_KSZ88XX_PCI_VENDOR_ID, EPS_KSZ88XX_PCI_DEVICE_ID_KSZ8842);

  if (result == EPS_PNDEV_RET_OK)
  // success
  {
    // setup NRT buffer (local mem is used)
    pBoard->sHw.EpsBoardInfo.nrt_mem.size =
        pBoard->OpenDevice.uHwRes.as.uKSZ8842.uSharedHostSdram.lSize;
    pBoard->sHw.EpsBoardInfo.nrt_mem.base_ptr =
        (LSA_UINT8*)
            pBoard->OpenDevice.uHwRes.as.uKSZ8842.uSharedHostSdram.uBase.pPtr;
    pBoard->sHw.EpsBoardInfo.nrt_mem.phy_addr =
        pBoard->OpenDevice.uHwRes.as.uKSZ8842.uSharedHostSdram.lPhyAdr;
    EPS_ASSERT(pBoard->sHw.EpsBoardInfo.nrt_mem.base_ptr != LSA_NULL);
  }

  return result;
}

#endif

#if ((PSI_CFG_USE_EDDS == 1) && (defined EDDS_CFG_HW_INTEL))
/**
 * init and set board parameters after OpenDevice of PnDevDriver for i210
 *
 * @param	pBoard				pointer to current board
 * structure
 * @param 	hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_open_i210(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard,
                                         LSA_UINT16 hd_id) {
  LSA_UINT16 result = EPS_PNDEV_RET_OK;

  result = eps_intelLL_open(
      &pBoard->uEddsIntel, &pBoard->sHw, &pBoard->sysDev,
      (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uI210.uBar0.uBase.pPtr, hd_id,
      EPS_INTEL_PCI_VENDOR_ID, EPS_INTEL_PCI_DEVICE_ID_I210);

  if (result == EPS_PNDEV_RET_OK)
  // success
  {
    // setup NRT buffer (local mem is used)
    pBoard->sHw.EpsBoardInfo.nrt_mem.size =
        pBoard->OpenDevice.uHwRes.as.uI210.uSharedHostSdram.lSize;
    pBoard->sHw.EpsBoardInfo.nrt_mem.base_ptr =
        (LSA_UINT8*)
            pBoard->OpenDevice.uHwRes.as.uI210.uSharedHostSdram.uBase.pPtr;
    pBoard->sHw.EpsBoardInfo.nrt_mem.phy_addr =
        pBoard->OpenDevice.uHwRes.as.uI210.uSharedHostSdram.lPhyAdr;
    EPS_ASSERT(pBoard->sHw.EpsBoardInfo.nrt_mem.base_ptr != LSA_NULL);
  }

  return result;
}

/**
 * init and set board parameters after OpenDevice of PnDevDriver for i82574
 *
 * @param   pBoard              pointer to current board structure
 * @param   hd_id               index of hd
 * @return  EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_open_i82574(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard,
                                           LSA_UINT16 hd_id) {
  LSA_UINT16 result = EPS_PNDEV_RET_OK;

  result = eps_intelLL_open(
      &pBoard->uEddsIntel, &pBoard->sHw, &pBoard->sysDev,
      (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uI82574.uBar0.uBase.pPtr, hd_id,
      EPS_INTEL_PCI_VENDOR_ID, EPS_INTEL_PCI_DEVICE_ID_I82574);

  if (result == EPS_PNDEV_RET_OK)
  // success
  {
    // setup NRT buffer (local mem is used)
    pBoard->sHw.EpsBoardInfo.nrt_mem.size =
        pBoard->OpenDevice.uHwRes.as.uI82574.uSharedHostSdram.lSize;
    pBoard->sHw.EpsBoardInfo.nrt_mem.base_ptr =
        (LSA_UINT8*)
            pBoard->OpenDevice.uHwRes.as.uI82574.uSharedHostSdram.uBase.pPtr;
    pBoard->sHw.EpsBoardInfo.nrt_mem.phy_addr =
        pBoard->OpenDevice.uHwRes.as.uI82574.uSharedHostSdram.lPhyAdr;
    EPS_ASSERT(pBoard->sHw.EpsBoardInfo.nrt_mem.base_ptr != LSA_NULL);
  }

  return result;
}
#endif

/**
 * uninstall PnDevDriver
 *
 * @param	LSA_VOID
 * @return 	LSA_VOID
 */
LSA_VOID eps_pndevdrv_uninstall(LSA_VOID) {
  PNDEV_CHAR drvError[PNDEV_SIZE_STRING_BUF];

  EPS_ASSERT(g_pEpsPnDevDrv->bInit == LSA_TRUE);

  if (g_pEpsPnDevDrv->pDrvHandle != 0) {
    ePNDEV_RESULT eResult;

    eps_memset(drvError, 0, sizeof(drvError));

    // ShutDownDriver (disconnect driver connection)
    // Note: DLL cleanup (unload, cleanup Fct list) is done by the driver
    // service
    //       Tracing not done, possible already disabled
    eResult = fnPnDev_ShutDownDriver(g_pEpsPnDevDrv->pDrvHandle,
                                     PNDEV_SIZE_STRING_BUF, drvError);

    if (eResult != ePNDEV_OK) {
      LSA_CHAR sErrConv[PNDEV_SIZE_STRING_BUF] = {0};
      eps_convert_pndev_err_string(drvError, sErrConv);
      EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR, "EPS PnDevDrv %s",
                              sErrConv);

#ifdef EPS_PNDEVDRV_PRINTF_DRIVER_INFO
      printf("\r\n%s ln(%d): %s\r\n", (LSA_CHAR*)__FILE__, __LINE__,
             (LSA_CHAR*)drvError);
#endif
    }

    EPS_ASSERT(eResult == ePNDEV_OK);
  }

  g_pEpsPnDevDrv->pDrvHandle = 0;

  eps_pndevdrv_undo_init_critical_section();

  g_pEpsPnDevDrv->bInit = LSA_FALSE;

  g_pEpsPnDevDrv = LSA_NULL;
}

/**
 * install PnDevDriver
 *
 * @param	LSA_VOID
 * @return 	LSA_VOID
 */
LSA_VOID eps_pndevdrv_install(LSA_VOID) {
  EPS_PNDEV_IF_TYPE sPnDevDrvIf;
  LSA_BOOL bIfLoadOk = LSA_FALSE;
  LSA_BOOL bMapResult;

  LSA_UNUSED_ARG(bMapResult);

  // Init Internals
  eps_memset(&g_EpsPnDevDrv, 0, sizeof(g_EpsPnDevDrv));
  g_pEpsPnDevDrv = &g_EpsPnDevDrv;

  bIfLoadOk = eps_pndevdrv_startup();

  if (bIfLoadOk == LSA_FALSE) {
#ifdef EPS_PNDEVDRV_PRINTF_DRIVER_INFO
    printf("\r\n");
    printf("EPS PnDevDrv install failed!\r\n");
    printf("    => PnDevDrv Dll available? (Windows only)\r\n");
    printf("    => PnDevDrv Boards are in (startup) reset state?\r\n");
    printf(
        "    => correct Bootloader version installed on your PnDevDrv "
        "Boards?\r\n");
#endif

    return;
  }

#ifdef EPS_PNDEVDRV_PRINTF_DRIVER_INFO
  printf("\r\n");
  printf(
      "EPS PnDevDrv (%c%02d.%02d.%02d.%02d_%02d.%02d.%02d.%02d) install "
      "done\r\n",
      g_pEpsPnDevDrv->pDrvHandle->uVersion.kind,
      g_pEpsPnDevDrv->pDrvHandle->uVersion.version,
      g_pEpsPnDevDrv->pDrvHandle->uVersion.distribution,
      g_pEpsPnDevDrv->pDrvHandle->uVersion.fix,
      g_pEpsPnDevDrv->pDrvHandle->uVersion.hotfix,
      g_pEpsPnDevDrv->pDrvHandle->uVersion.project_number,
      g_pEpsPnDevDrv->pDrvHandle->uVersion.increment,
      g_pEpsPnDevDrv->pDrvHandle->uVersion.integration_counter,
      g_pEpsPnDevDrv->pDrvHandle->uVersion.gen_counter);
#endif

  eps_pndevdrv_init_critical_section();
  eps_pndevdrv_get_device_infos(); /* Find all devices */
  eps_pndevdrv_register_device_infos();

  // Init Interface
  sPnDevDrvIf.open = eps_pndevdrv_open;
  sPnDevDrvIf.close = eps_pndevdrv_close;
  sPnDevDrvIf.uninstall = eps_pndevdrv_uninstall;

  g_pEpsPnDevDrv->bInit = LSA_TRUE;

  eps_pndev_if_register(&sPnDevDrvIf);
}

/**
 * open pntrc lower cpu
 *
 * @param pBoard	pointer to current board structur
 * @param pOption	ptr to options, if FirmwareDownload and Debugging will be
 * executed
 * @return 	LSA_VOID
 */
static LSA_VOID eps_pndevdrv_open_pntrc_lower_cpu(
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard,
    EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption) {
  if (pOption != LSA_NULL) {
    if (pOption->bLoadFw == LSA_TRUE) {
      pBoard->Pntrc.hPntrcCPU =
          pntrc_open_lower_cpu((PNTRC_SYS_HANDLE_LOWER_PTR_TYPE)&pBoard->sHw);
      pBoard->Pntrc.bOpened = LSA_TRUE;
    }
  }
}

/**
 * close pntrc lower cpu
 *
 * @param pBoard	pointer to current board structur
 * @return 	LSA_VOID
 */
static LSA_UINT16 eps_pndevdrv_close_pntrc_lower_cpu(
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard) {
  if (pBoard->Pntrc.bOpened) {
    pntrc_close_lower_cpu(pBoard->Pntrc.hPntrcCPU);
    pBoard->Pntrc.bOpened = LSA_FALSE;
  }
  return EPS_PNDEV_RET_OK;
}

/**
 * open device with PnDevDriver
 *
 * @param	pLocation 		ptr including PciLocation
 * @param	pOption			ptr to options, if FirmwareDownload and
 * Debugging will be executed
 * @param	ppHwInstOut		pointer to ptr of strucute with hardware
 * configuration
 * @param 	hd_id			index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_open(EPS_PNDEV_LOCATION_PTR_TYPE pLocation,
                             EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption,
                             EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut,
                             LSA_UINT16 hd_id) {
  EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;
  LSA_UINT32 uStoreIndex;
  ePNDEV_RESULT drvResult;
  LSA_UINT16 result = EPS_PNDEV_RET_OK;

  EPS_ASSERT(g_pEpsPnDevDrv->bInit == LSA_TRUE);
  EPS_ASSERT(pLocation != LSA_NULL);
  EPS_ASSERT(ppHwInstOut != LSA_NULL);

  PNTRC_ENTER_SERVICE();  // Hack -> Prevents Deadlock with PNTRC -> will be
                          // fixed in RQ 1071129
  eps_pndevdrv_enter();

  pBoard = eps_pndevdrv_alloc_board();
  EPS_ASSERT(pBoard != LSA_NULL); /* No more resources */

  pBoard->pDeviceInfo = LSA_NULL;

  if (pLocation->eLocationSelector == EPS_PNDEV_LOCATION_PCI) {
    uStoreIndex = eps_pndevdrv_get_device(pLocation, &pBoard->pDeviceInfo);
  }

  if (pBoard->pDeviceInfo == LSA_NULL) {
    result = EPS_PNDEV_RET_DEV_NOT_FOUND;
    EPS_SYSTEM_TRACE_00(
        0, LSA_TRACE_LEVEL_ERROR,
        "eps_pndevdrv_open: Error: EPS_PNDEV_RET_DEV_NOT_FOUND");
  }

  if (result == EPS_PNDEV_RET_OK) {
    drvResult = eps_pndevdrv_open_device(pBoard, pOption, &pBoard->OpenDevice,
                                         hd_id);  // uses pBoard->pDeviceInfo!
    if (ePNDEV_OK != drvResult) {
      result = EPS_PNDEV_RET_DEV_OPEN_FAILED;
      EPS_SYSTEM_TRACE_00(
          0, LSA_TRACE_LEVEL_ERROR,
          "eps_pndevdrv_open: Error: EPS_PNDEV_RET_DEV_OPEN_FAILED");
    }
  }

  if (result == EPS_PNDEV_RET_OK) {
    pBoard->Location = *pLocation;

    switch (pBoard->OpenDevice.uHwInfo.eAsic) {
#if (PSI_CFG_USE_EDDP == 1)
      case ePNDEV_ASIC_ERTEC200P:
        result = eps_pndevdrv_open_eb200p(pBoard, hd_id);
        break;
#endif
#if (PSI_CFG_USE_EDDI == 1)
      case ePNDEV_ASIC_SOC1:
        result = eps_pndevdrv_open_soc1(pBoard, hd_id);
        break;
      case ePNDEV_ASIC_ERTEC400:
        result = eps_pndevdrv_open_ertec400(pBoard, hd_id);
        break;
      case ePNDEV_ASIC_ERTEC200:
        result = eps_pndevdrv_open_ertec200(pBoard, hd_id);
        break;
#endif
#if ((PSI_CFG_USE_EDDS == 1) && (defined EDDS_CFG_HW_INTEL))
      case ePNDEV_ASIC_I210:
        result = eps_pndevdrv_open_i210(pBoard, hd_id);
        break;
      case ePNDEV_ASIC_I82574:
        result = eps_pndevdrv_open_i82574(pBoard, hd_id);
        break;
#endif
#if ((PSI_CFG_USE_EDDS == 1) && (defined EDDS_CFG_HW_KSZ88XX))
      case ePNDEV_ASIC_KSZ8841:
        result = eps_pndevdrv_open_ksz8841(pBoard, hd_id);
        break;
      case ePNDEV_ASIC_KSZ8842:
        result = eps_pndevdrv_open_ksz8842(pBoard, hd_id);
        break;
#endif

      default:
        result = EPS_PNDEV_RET_UNSUPPORTED;
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR,
                            "eps_pndevdrv_open: Error: "
                            "EPS_PNDEV_RET_UNSUPPORTED (wrong compile "
                            "variant?)");
        break;
    }
  }

  if (result == EPS_PNDEV_RET_OK) {
    eps_pndevdrv_open_pntrc_lower_cpu(pBoard, pOption);

    eps_pndevdrv_open_hif_driver(pBoard);

#ifdef EPS_PNDEVDRV_PRINTF_OPEN_CLOSE_INFO
    eps_pndevdrv_printf_openclose_board(LSA_TRUE, pBoard);
#endif

    *ppHwInstOut = &pBoard->sHw;
  } else {
    eps_pndevdrv_free_board(pBoard);
  }

  eps_pndevdrv_exit();
  PNTRC_EXIT_SERVICE();  // Hack -> Prevents Deadlock with PNTRC -> will be
                         // fixed in RQ 1071129

  return result;
}

/**
 * close device with PnDevDriver
 *
 * @param	pHwInstIn		pointer of strucute with hardware
 * configuration
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_close(EPS_PNDEV_HW_PTR_TYPE pHwInstIn) {
  ePNDEV_RESULT drvResult;
  uPNDEV_CLOSE_DEVICE_IN uCloseDeviceIn;
  uPNDEV_CLOSE_DEVICE_OUT uCloseDeviceOut;
  EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
  EPS_PNDEV_INTERRUPT_DESC_TYPE IsrDesc;

  EPS_ASSERT(pHwInstIn != LSA_NULL);
  EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

  PNTRC_ENTER_SERVICE();  // Hack -> Prevents Deadlock with PNTRC -> will be
                          // fixed in RQ 1071129
  eps_pndevdrv_enter();

  pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

  EPS_ASSERT(pBoard->bUsed == LSA_TRUE);

  eps_pndevdrv_close_pntrc_lower_cpu(pBoard);

  eps_pndevdrv_close_hif_driver(pBoard);

  if (pBoard->uCountIsrEnabled != 0) {
    if (pBoard->sIsrIsochronous.pCbf != LSA_NULL) {
      IsrDesc = EPS_PNDEV_ISR_ISOCHRONOUS;
      eps_pndevdrv_disable_interrupt_intern(pHwInstIn, &IsrDesc);
    }

    if (pBoard->sIsrPnGathered.pCbf != LSA_NULL) {
      IsrDesc = EPS_PNDEV_ISR_PN_GATHERED;
      eps_pndevdrv_disable_interrupt_intern(pHwInstIn, &IsrDesc);
    }

    if (pBoard->sIsrPnNrt.pCbf != LSA_NULL) {
      IsrDesc = EPS_PNDEV_ISR_PN_NRT;
      eps_pndevdrv_disable_interrupt_intern(pHwInstIn, &IsrDesc);
    }
  }

  EPS_ASSERT(pBoard->uCountIsrEnabled == 0);

  if (pBoard->TimerCtrl.bRunning == LSA_TRUE) {
    eps_pndevdrv_timer_ctrl_stop_intern(pHwInstIn);
  }

  EPS_ASSERT(pBoard->TimerCtrl.bRunning == LSA_FALSE);

  switch (pBoard->OpenDevice.uHwInfo.eAsic) {
// "PSI_CFG_USE_EDDI" and "EDDI_CFG_SOC" are also defined by EPS_PLF_ADONIS_X86
#if ((PSI_CFG_USE_EDDI == 1) && (defined(EDDI_CFG_SOC)))
    case ePNDEV_ASIC_SOC1:
      eps_undo_init_hw_SOC(pBoard->sysDev.hd_nr);
      break;
#endif

// "EDDI_CFG_ERTEC_400" is only defined by "EPS_PLF_ERTEC400_ARM9", so the
// (EPS_PLF == EPS_PLF_ADONIS_X86) is necessary here
#if (((PSI_CFG_USE_EDDI == 1) && (defined(EDDI_CFG_ERTEC_400))) || \
     (EPS_PLF == EPS_PLF_ADONIS_X86))
    case ePNDEV_ASIC_ERTEC400:
      eps_undo_init_hw_ertec400(pBoard->sysDev.hd_nr);
      break;
#endif

// "EDDI_CFG_ERTEC_200" is only defined by "EPS_PLF_ERTEC200_ARM9", so the
// (EPS_PLF == EPS_PLF_ADONIS_X86) is necessary here
#if (((PSI_CFG_USE_EDDI == 1) && (defined(EDDI_CFG_ERTEC_200))) || \
     (EPS_PLF == EPS_PLF_ADONIS_X86))
    case ePNDEV_ASIC_ERTEC200:
      eps_undo_init_hw_ertec200(pBoard->sysDev.hd_nr);
      break;
#endif
    default: /* nothing to do */
      break;
  }

  eps_memset(&uCloseDeviceIn, 0, sizeof(uCloseDeviceIn));
  eps_memset(&uCloseDeviceOut, 0, sizeof(uCloseDeviceOut));

  uCloseDeviceIn.uDevice.hHandle = pBoard->OpenDevice.uDevice.hHandle;
  uCloseDeviceIn.bDoAnyway = LSA_FALSE;

  drvResult = g_pEpsPnDevDrv->pDrvHandle->uIf.pFnCloseDevice(
      g_pEpsPnDevDrv->pDrvHandle, sizeof(uCloseDeviceIn),
      sizeof(uCloseDeviceOut), &uCloseDeviceIn, &uCloseDeviceOut);

  if (drvResult != ePNDEV_OK) {
    uPNDEV_CLOSE_DEVICE_OUT* pOut = &uCloseDeviceOut;
    LSA_CHAR sErrConv[PNDEV_SIZE_STRING_BUF] = {0};
    eps_convert_pndev_err_string(pOut->sError, sErrConv);
    EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "eps_pndevdrv_close() %s",
                            sErrConv);

    EPS_FATAL("Error at eps_pndevdrv_close - drvResult != ePNDEV_OK");
  }

#ifdef EPS_PNDEVDRV_PRINTF_OPEN_CLOSE_INFO
  eps_pndevdrv_printf_openclose_board(LSA_FALSE, pBoard);
#endif

  eps_pndevdrv_free_board(pBoard);

  eps_pndevdrv_exit();
  PNTRC_EXIT_SERVICE();  // Hack -> Prevents Deadlock with PNTRC -> will be
                         // fixed in RQ 1071129

  return EPS_PNDEV_RET_OK;
}

/**
 * enable interrupt for board
 *
 * @param	pHwInstIn		pointer of strucute with hardware
 * configuration
 * @param	pInterrupt		pointer to enum with method type
 * @param	pCbf			ptr to Callbackfunction
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_enable_interrupt(
    EPS_PNDEV_HW_PTR_TYPE pHwInstIn,
    EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt,
    EPS_PNDEV_CALLBACK_PTR_TYPE pCbf) {
  LSA_UINT16 result = EPS_PNDEV_RET_OK;

  EPS_ASSERT(pHwInstIn != LSA_NULL);
  EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

  EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,
                      "eps_pndevdrv_enable_interrupt() - enable interrupt");

  eps_pndevdrv_enter();

  result = eps_pndevdrv_enable_interrupt_intern(pHwInstIn, pInterrupt, pCbf);

  eps_pndevdrv_exit();

  return result;
}

/**
 * disable interrupt for board
 *
 * @param	pHwInstIn		pointer of strucute with hardware
 * configuration
 * @param	pInterrupt		pointer to enum with method type
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_disable_interrupt(
    EPS_PNDEV_HW_PTR_TYPE pHwInstIn,
    EPS_PNDEV_INTERRUPT_DESC_PTR_TYPE pInterrupt) {
  LSA_UINT16 result = EPS_PNDEV_RET_OK;

  EPS_ASSERT(pHwInstIn != LSA_NULL);
  EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

  EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,
                      "eps_pndevdrv_disable_interrupt() - disable interrupt");
  eps_pndevdrv_enter();

  result = eps_pndevdrv_disable_interrupt_intern(pHwInstIn, pInterrupt);

  eps_pndevdrv_exit();

  return result;
}

/**
 * set gpio
 *
 * @param	pHwInstIn		pointer of strucute with hardware
 * configuration
 * @param	gpio			enum with gpios
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_set_gpio(struct eps_pndev_hw_tag* pHwInstIn,
                                 EPS_PNDEV_GPIO_DESC_TYPE gpio) {
  EPS_ASSERT(pHwInstIn != LSA_NULL);
  EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

  LSA_UNUSED_ARG(gpio);

  switch (pHwInstIn->EpsBoardInfo.board_type) {
#if (PSI_CFG_USE_EDDP == 1)
    case EPS_PNDEV_BOARD_EB200P:
      switch (gpio) {
        case EPS_PNDEV_DEBUG_GPIO_1:
          eps_hw_e200p_set_gpio(
              pHwInstIn->EpsBoardInfo.eddp.apb_periph_perif.base_ptr,
              EPS_HW_E200P_DEBUG_GPIO_1);
          break;
        case EPS_PNDEV_DEBUG_GPIO_2:
          eps_hw_e200p_set_gpio(
              pHwInstIn->EpsBoardInfo.eddp.apb_periph_perif.base_ptr,
              EPS_HW_E200P_DEBUG_GPIO_2);
          break;
        default:
          return EPS_PNDEV_RET_UNSUPPORTED;
      }
      break;
#endif
    default:
      /* Do Nothing */
      break;
  }

  return EPS_PNDEV_RET_OK;
}

/**
 * clear gpio
 *
 * @param	pHwInstIn		pointer of strucute with hardware
 * configuration
 * @param	gpio			enum with gpios
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_clear_gpio(struct eps_pndev_hw_tag* pHwInstIn,
                                   EPS_PNDEV_GPIO_DESC_TYPE gpio) {
  EPS_ASSERT(pHwInstIn != LSA_NULL);
  EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

  LSA_UNUSED_ARG(gpio);

  switch (pHwInstIn->EpsBoardInfo.board_type) {
#if (PSI_CFG_USE_EDDP == 1)
    case EPS_PNDEV_BOARD_EB200P:
      switch (gpio) {
        case EPS_PNDEV_DEBUG_GPIO_1:
          eps_hw_e200p_clear_gpio(
              pHwInstIn->EpsBoardInfo.eddp.apb_periph_perif.base_ptr,
              EPS_HW_E200P_DEBUG_GPIO_1);
          break;
        case EPS_PNDEV_DEBUG_GPIO_2:
          eps_hw_e200p_clear_gpio(
              pHwInstIn->EpsBoardInfo.eddp.apb_periph_perif.base_ptr,
              EPS_HW_E200P_DEBUG_GPIO_2);
          break;
        default:
          return EPS_PNDEV_RET_UNSUPPORTED;
      }
      break;
#endif
    default:
      /* Do Nothing */
      break;
  }

  return EPS_PNDEV_RET_OK;
}

/**
 * start timer
 *
 * @param pHwInstIn 		pointer to strucute with hardware configuration
 * @param pCbf				ptr to Callbackfunction
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_timer_ctrl_start(struct eps_pndev_hw_tag* pHwInstIn,
                                         EPS_PNDEV_CALLBACK_PTR_TYPE pCbf) {
  EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
  LSA_UINT16 result = EPS_PNDEV_RET_OK;

  EPS_ASSERT(pHwInstIn != LSA_NULL);
  EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);
  EPS_ASSERT(pCbf != LSA_NULL);

  eps_pndevdrv_enter();

  pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

  EPS_ASSERT(pBoard->bUsed == LSA_TRUE);
  EPS_ASSERT(pBoard->TimerCtrl.bRunning == LSA_FALSE);

  pBoard->TimerCtrl.sTimerCbf = *pCbf;

  if (pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval != 0) {
    result = eps_pndevdrv_timer_ctrl_start_intern(pHwInstIn, pCbf);
    EPS_ASSERT(result == EPS_PNDEV_RET_OK);
  }

  pBoard->TimerCtrl.bRunning = LSA_TRUE;

  eps_pndevdrv_exit();

  return result;
}

/**
 * stop timer
 *
 * @param pHwInstIn 		pointer to strucute with hardware configuration
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_timer_ctrl_stop(struct eps_pndev_hw_tag* pHwInstIn) {
  LSA_UINT16 result;

  EPS_ASSERT(pHwInstIn != LSA_NULL);
  EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

  eps_pndevdrv_enter();

  result = eps_pndevdrv_timer_ctrl_stop_intern(pHwInstIn);

  eps_pndevdrv_exit();

  return result;
}

/**
 * read trace data
 *
 * @param pHwInstIn 			pointer to strucute with hardware
 * configuration
 * @param offset				offset where should be read from
 * @param ptr					destination pointer for the data
 * @param size					size of data
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_read_trace_data(EPS_PNDEV_HW_PTR_TYPE pHwInstIn,
                                        LSA_UINT32 offset, LSA_UINT8* ptr,
                                        LSA_UINT32 size) {
  ePNDEV_RESULT drvResult;
  EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
  LSA_UINT16 result = EPS_PNDEV_RET_OK;
  LSA_UINT32 lreadHostBufSize = size;
  LSA_UINT32 lSizeBufOut = sizeof(uPNDEV_COPY_DATA_OUT) + lreadHostBufSize;
  LSA_UINT8* pBufOut = LSA_NULL;
  uPNDEV_COPY_DATA_OUT* pCopyDataOut = LSA_NULL;
  LSA_UINT8* pHostBuf = LSA_NULL;
  uPNDEV_COPY_DATA_IN copyDataIn;
  LSA_UINT32 PntrcAsicOffset = 0;

  EPS_ASSERT(pHwInstIn != LSA_NULL);
  EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);
  EPS_ASSERT(ptr != LSA_NULL);

  eps_pndevdrv_enter();

  pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;
  EPS_ASSERT(pBoard->bUsed == LSA_TRUE);

  // allocate output buffer
  pBufOut = (LSA_UINT8*)eps_mem_alloc(lSizeBufOut, LSA_COMP_ID_PNBOARDS,
                                      PSI_MTYPE_IGNORE);
  EPS_ASSERT(pBufOut != 0);

  pCopyDataOut = (uPNDEV_COPY_DATA_OUT*)pBufOut;
  pHostBuf = pBufOut + sizeof(uPNDEV_COPY_DATA_OUT);

  eps_memset(&copyDataIn, 0, sizeof(copyDataIn));

  switch (pBoard->OpenDevice.uHwInfo.eAsic) {
    case ePNDEV_ASIC_ERTEC200:
      PntrcAsicOffset =
          pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwTrace[0].lOffset;
      break;
    case ePNDEV_ASIC_ERTEC200P:
      PntrcAsicOffset =
          pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwTrace[0].lOffset;
      break;
    case ePNDEV_ASIC_ERTEC400:
      PntrcAsicOffset =
          pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwTrace[0].lOffset;
      break;
    case ePNDEV_ASIC_SOC1:
      PntrcAsicOffset =
          pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwTrace[0].lOffset;
      break;
    default:
      EPS_FATAL("PnDev FATAL occured");
  }

  copyDataIn.eHwRes = ePNDEV_COPY_HW_RES_SDRAM;
  copyDataIn.uDevice.hHandle = pBoard->OpenDevice.uDevice.hHandle;

  copyDataIn.uArrayBlockRead[0].lOffsetAsic = PntrcAsicOffset + offset;
  copyDataIn.uArrayBlockRead[0].lOffsetHostDataBuf = 0;
  copyDataIn.uArrayBlockRead[0].lSize = lreadHostBufSize;

  drvResult = g_pEpsPnDevDrv->pDrvHandle->uIf.pFnCopyData(
      g_pEpsPnDevDrv->pDrvHandle, sizeof(copyDataIn), lSizeBufOut, &copyDataIn,
      pCopyDataOut);

  EPS_ASSERT(drvResult == ePNDEV_OK);

  eps_memcpy(ptr, pHostBuf, size);

  eps_mem_free(pBufOut, 0, 0);

  eps_pndevdrv_exit();

  return result;
}

/**
 * read trace data
 *
 * @param pHwInstIn 			pointer to strucute with hardware
 * configuration
 * @param ptr					source pointer for the data
 * @param offset				offset where should be write to
 * @param size					size of data
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_write_trace_data(EPS_PNDEV_HW_PTR_TYPE pHwInstIn,
                                         LSA_UINT8* ptr, LSA_UINT32 offset,
                                         LSA_UINT32 size) {
  ePNDEV_RESULT drvResult;
  uPNDEV_COPY_DATA_IN copyDataIn;
  LSA_UINT16 result = EPS_PNDEV_RET_OK;
  EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
  LSA_UINT32 lWriteHostBufSize = size;
  LSA_UINT32 lReadHostBufSize = 0;
  LSA_UINT32 lWriteBlock0OffsetHostDataBuf = 0;
  LSA_UINT32 lSizeBufOut =
      sizeof(uPNDEV_COPY_DATA_OUT) + lWriteHostBufSize + lReadHostBufSize;
  LSA_UINT8* pBufOut = LSA_NULL;
  uPNDEV_COPY_DATA_OUT* pCopyDataOut = LSA_NULL;
  LSA_UINT8* pHostBuf = LSA_NULL;
  LSA_UINT32 PntrcAsicOffset = 0;

  EPS_ASSERT(pHwInstIn != LSA_NULL);
  EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);
  EPS_ASSERT(ptr != LSA_NULL);

  eps_pndevdrv_enter();

  pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;
  EPS_ASSERT(pBoard->bUsed == LSA_TRUE);

  // allocate output buffer
  pBufOut = (LSA_UINT8*)eps_mem_alloc(lSizeBufOut, LSA_COMP_ID_PNBOARDS,
                                      PSI_MTYPE_IGNORE);
  EPS_ASSERT(pBufOut != 0);

  pCopyDataOut = (uPNDEV_COPY_DATA_OUT*)pBufOut;
  pHostBuf = pBufOut + sizeof(uPNDEV_COPY_DATA_OUT);

  eps_memcpy(pHostBuf, ptr, size);
  eps_memset(&copyDataIn, 0, sizeof(copyDataIn));

  switch (pBoard->OpenDevice.uHwInfo.eAsic) {
    case ePNDEV_ASIC_ERTEC200:
      PntrcAsicOffset =
          pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwTrace[0].lOffset;
      break;
    case ePNDEV_ASIC_ERTEC200P:
      PntrcAsicOffset =
          pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwTrace[0].lOffset;
      break;
    case ePNDEV_ASIC_ERTEC400:
      PntrcAsicOffset =
          pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwTrace[0].lOffset;
      break;
    case ePNDEV_ASIC_SOC1:
      PntrcAsicOffset =
          pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwTrace[0].lOffset;
      break;
    default:
      EPS_FATAL("PnDev FATAL occured");
  }

  copyDataIn.eHwRes = ePNDEV_COPY_HW_RES_SDRAM;
  copyDataIn.uDevice.hHandle = pBoard->OpenDevice.uDevice.hHandle;

  copyDataIn.uArrayBlockWrite[0].lOffsetAsic = PntrcAsicOffset + offset;
  copyDataIn.uArrayBlockWrite[0].lOffsetHostDataBuf =
      lWriteBlock0OffsetHostDataBuf;
  copyDataIn.uArrayBlockWrite[0].lSize = lWriteHostBufSize;

  drvResult = g_pEpsPnDevDrv->pDrvHandle->uIf.pFnCopyData(
      g_pEpsPnDevDrv->pDrvHandle, sizeof(copyDataIn), lSizeBufOut, &copyDataIn,
      pCopyDataOut);
  EPS_ASSERT(drvResult == ePNDEV_OK);

  eps_mem_free(pBufOut, 0, 0);

  eps_pndevdrv_exit();

  return result;
}

#else /* EDDP and or EDDI not configured  */

LSA_VOID eps_pndevdrv_install(LSA_VOID) {
  /* Do nothing if PnDevDrv not supported */
}

#endif /* (PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) */

#endif /* ( (PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || \
          (PSI_CFG_USE_EDDS == 1) )   */
