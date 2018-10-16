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
/*  F i l e               &F: pnd_agent.cpp                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PnDriver agent - startup /config /shutdown                */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID 1110
#define PND_MODULE_ID 1110

#include "pnd_int.h"
#include "pnd_sys.h"

#include "pniobase.h"
#include "pnioerrx.h"
#include "servusrx.h"

/* StdLib++ */
#include <vector>

#include <eps_pn_drv_if.h>
#include "eps_cp_hw.h"

using namespace std;

#include "pnd_iob_core.h"
#include "pnd_pnio_user_core.h"

#include "pntrc_inc.h"
#include "pntrc_trc.h"

#include <eps_app.h>
#include <eps_plf.h>

#include "pnd_ParamStore.h"  // component params

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#define HAVE_REMOTE
#include <packet32_inc.h> /* EDDS Lower Layer Intel Driver            */
#include "packet32.h"
#include "pcap.h"
#endif

#if (EPS_PLF == EPS_PLF_LINUX_X86)
#include "eps_pndevdrv.h"
#endif

PND_FILE_SYSTEM_EXTENSION(PNDRIVER_MODULE_ID)

/*---------------------------------------------------------------------------*/
/* PnDriver instance                                                         */
/*---------------------------------------------------------------------------*/

// TODO hier müssen die globalen Instance daten des Agents ergänzt werden

// have to be a global structure instead of malloc/free because memory pools are
// not available at the time when used
PNIO_DEBUG_SETTINGS_TYPE g_pnio_debug_settings;
/*---------------------------------------------------------------------------*/
/* types                                                                     */
/*---------------------------------------------------------------------------*/

enum pnd_agent_status { PND_AGENT_STATUS_STOP = 42, PND_AGENT_STATUS_RUN };

typedef struct pnd_sys_ptr_tag {
  PNIO_UINT16 sys1;  // HD number [1..N]
  PNIO_UINT16 sys2;  // IF number [1..N]
} PND_SYS_TYPE, *PND_SYS_PTR_TYPE;

typedef struct pnd_instance_tag {
  PNIO_BOOL init_done;          // Init sequence done ?
  PNIO_BOOL serv_cp_init_done;  // SERV CP Init sequence done ?
  PNIO_UINT16 wait_event;       // Wait event

  EPS_DATA_PTR_TYPE eps_sys_data_ptr;  // EPS data IF for core (read access)

  PND_PARAM_STARTUP_TYPE pnd_startup_param;
  pnd_agent_status status;
} PNDRIVER_INSTANCE_TYPE, *PNDRIVER_INSTANCE_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* instance data                                                             */
/*---------------------------------------------------------------------------*/

static PNDRIVER_INSTANCE_TYPE pnd_instance = {PNIO_FALSE};

/*---------------------------------------------------------------------------*/
/* prototypes                                                                */
/*---------------------------------------------------------------------------*/
static PNIO_UINT32 pnd_agent_iob_core_startup(
    PND_PARAM_STARTUP_PTR_TYPE pnd_startup_param);
static PNIO_VOID pnd_agent_iob_core_startup_done(PNIO_VOID_PTR_TYPE p_rqb);

static PNIO_UINT32 pnd_agent_iob_core_shutdown(PNIO_VOID);
static PNIO_VOID pnd_agent_iob_core_shutdown_done(PNIO_VOID_PTR_TYPE p_rqb);

static PNIO_UINT32 pnd_agent_pnio_user_core_startup(
    PND_PARAM_STARTUP_PTR_TYPE pnd_startup_param);
static PNIO_VOID pnd_agent_pnio_user_core_startup_done(
    PNIO_VOID_PTR_TYPE p_rqb);

static PNIO_UINT32 pnd_agent_pnio_user_core_shutdown(PNIO_VOID);
static PNIO_VOID pnd_agent_pnio_user_core_shutdown_done(
    PNIO_VOID_PTR_TYPE p_rqb);

static PNIO_UINT32 pnd_agent_shutdown(PNIO_VOID);
PNIO_VOID pnd_sys_init_done(PNIO_VOID);

/*---------------------------------------------------------------------------*/
static PNIO_VOID pnd_sys_undo_init(PNIO_VOID) {
  PNIO_UINT32 result = PNIO_OK;

  result = pnd_free_event(pnd_instance.wait_event);
  PND_ASSERT(result == PNIO_RET_OK);

  pnd_mbx_sys_undo_init();

  pnd_threads_undo_init();

  eps_undo_init();
}

/*---------------------------------------------------------------------------*/
static PNIO_UINT32 pnd_agent_startup(
    PNIO_CP_ID_PTR_TYPE CpList, PNIO_UINT32 NrOfCp, PNIO_UINT8 *pConfigData,
    PNIO_UINT32 ConfigDataLen, PNIO_UINT8 *pRemaData, PNIO_UINT32 RemaDataLen,
    PNIO_SYSTEM_DESCR *pSysDescr) {
  PNIO_UINT32 result = PNIO_OK;
  PNIO_BOOL decentral_io_system = PNIO_TRUE;

  pnd_pnio_user_core_init();

  pnd_iobase_core_init();

  // Create the Param storage, used for LD startup
  CParamStore *paramObj = CParamStore::GetInstance();

  if (paramObj != 0) {
    result = paramObj->Create(NrOfCp, CpList, pConfigData, ConfigDataLen,
                              pRemaData, RemaDataLen, pSysDescr);

    if (result == PNIO_OK ||
        result == PNIO_WARN_NO_DECENTRALIOSYSTEM)  // param config correct ?
    {
      EPS_OPEN_TYPE uOpen;
      PNIO_UINT16 hdNr = 0;
      PND_PARAM_PSI_LD_TYPE psiParams;
      PND_PARAM_HIF_LD_TYPE hifParam;

      if (result == PNIO_WARN_NO_DECENTRALIOSYSTEM) {
        decentral_io_system = PNIO_FALSE;
      } else {
        decentral_io_system = PNIO_TRUE;
      }

      pnd_instance.eps_sys_data_ptr = g_pEpsData;  // see eps_sys.h

      // Get HIF Params
      paramObj->GetParamHifLD(&hifParam);

      // Get PSI Params
      paramObj->GetParamPsiLD(&psiParams);

      PSI_MEMSET(&uOpen, 0, sizeof(EPS_OPEN_TYPE));

      uOpen.hd_count = psiParams.hd_count;
      for (hdNr = 0; hdNr < PSI_CFG_MAX_HD_CNT; hdNr++) {
        uOpen.hd_args[hdNr] = psiParams.hd_param[hdNr];
      }

      uOpen.imInput = psiParams.im_param;
      uOpen.pUsrHandle = PNIO_NULL;
      uOpen.ldRunsOnType = PSI_LD_RUNS_ON_LIGHT;

      /* startup EPS (open LD), wait for complete */
      eps_open(&uOpen, PNIO_NULL /*PNIO_NULL = without Cbf -> wait*/, 10);

      for (hdNr = 0; hdNr < psiParams.hd_count; hdNr++) {
        // update out params from PSI
        paramObj->SetParamPsiHD(hdNr + 1,
                                &uOpen.hd_out[hdNr]);  // Interface specific ???
      }

      result = paramObj->GetParamStartup(&pnd_instance.pnd_startup_param);
      if (result != PNIO_OK) {
        pnd_agent_shutdown();
        return result;
      }

      pnd_instance.pnd_startup_param.decentral_io_system = decentral_io_system;

      result = pnd_agent_iob_core_startup(&pnd_instance.pnd_startup_param);
      if (result != PNIO_OK) {
        pnd_agent_shutdown();
        return result;
      }

      // prepare startup request pnio_user_core
      result =
          pnd_agent_pnio_user_core_startup(&pnd_instance.pnd_startup_param);
      if (result != PNIO_OK) {
        pnd_agent_shutdown();
        return result;
      }
    } else {
      pnd_instance.serv_cp_init_done = PNIO_FALSE;

      pnd_iobase_core_undo_init();

      CParamStore::DeleteInstance();

      pnd_pnio_user_core_undo_init();

      pnd_sys_undo_init();
    }

  } else {
    pnd_instance.serv_cp_init_done = PNIO_FALSE;

    pnd_iobase_core_undo_init();

    pnd_pnio_user_core_undo_init();

    pnd_sys_undo_init();

    result = PNIO_ERR_CREATE_INSTANCE;
  }

  return (result);
}

/*---------------------------------------------------------------------------*/
static PNIO_UINT32 pnd_agent_shutdown(PNIO_VOID) {
  PNIO_UINT32 result;
  EPS_CLOSE_TYPE uClose;

  result = pnd_agent_iob_core_shutdown();
  if (result != PNIO_OK) {
    return result;
  }

  pnd_agent_pnio_user_core_shutdown();

  eps_close(&uClose, PNIO_NULL, 2);

  return PNIO_OK;
}

// Example for HIF, PSI system posting
/*---------------------------------------------------------------------------*/
/* PnDriver request function to EPS system  (using HIF or PSI)               */
/*---------------------------------------------------------------------------*/

PNIO_VOID pndriver_core_system_request_lower(PNIO_VOID_PTR_TYPE rqb_ptr) {
  /* PN system request to a lower PNIO comp (i.E.: CM) over HIF LD */
  PND_ASSERT(rqb_ptr != 0);

  psi_request_start(PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_system, rqb_ptr);
}

/*----------------------------------------------------------------------------*/
PNIO_VOID pndriver_core_open_channel_lower(PNIO_VOID_PTR_TYPE rqb_ptr) {
  /* PN open channel request to a lower PNIO comp (i.E.: CM) over HIF LD */
  PND_ASSERT(rqb_ptr != 0);

  psi_request_start(PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_u_open_channel,
                    rqb_ptr);
}

/*----------------------------------------------------------------------------*/
static PNIO_VOID pnd_agent_iob_core_startup_done(PNIO_VOID_PTR_TYPE p_rqb) {
  // runs in context of iobase core
  PND_ASSERT(p_rqb != 0);
  pnd_set_event(pnd_instance.wait_event);
}

/*----------------------------------------------------------------------------*/
static PNIO_UINT32 pnd_agent_iob_core_startup(
    PND_PARAM_STARTUP_PTR_TYPE pnd_startup_param) {
  PNIO_UINT32 result = PNIO_OK;

  PND_RQB_PTR_TYPE pRQB;
  PND_STARTUP_PTR_TYPE pStart;

  pRQB = (PND_RQB_PTR_TYPE)pnd_mem_alloc(sizeof(*pRQB));
  PND_ASSERT(pRQB);

  pStart = &pRQB->args.startup;

  pStart->startup_param = *pnd_startup_param;
  pStart->cbf = pnd_agent_iob_core_startup_done;
  pStart->pnio_err = PNIO_OK;

  LSA_RQB_SET_OPCODE(pRQB, PND_OPC_STARTUP);

  // Wait on callback, after request
  pnd_reset_event(pnd_instance.wait_event);

  pnd_request_start(PND_MBX_ID_IO_BASE_CORE,
                    (PND_REQUEST_FCT)pnd_iob_core_request,
                    (PNIO_VOID_PTR_TYPE)pRQB);

  pnd_wait_event(pnd_instance.wait_event);

  result = pStart->pnio_err;

  pnd_mem_free(pRQB);

  return result;
}

/*----------------------------------------------------------------------------*/
static PNIO_VOID pnd_agent_pnio_user_core_startup_done(
    PNIO_VOID_PTR_TYPE p_rqb) {
  // runs in context of iobase core
  PND_ASSERT(p_rqb != 0);
  pnd_set_event(pnd_instance.wait_event);
}

/*----------------------------------------------------------------------------*/
static PNIO_UINT32 pnd_agent_pnio_user_core_startup(
    PND_PARAM_STARTUP_PTR_TYPE pnd_startup_param) {
  PNIO_UINT32 result = PNIO_OK;

  PND_RQB_PTR_TYPE pRQB;
  PND_STARTUP_PTR_TYPE pStart;

  pRQB = (PND_RQB_PTR_TYPE)pnd_mem_alloc(sizeof(*pRQB));
  PND_ASSERT(pRQB);

  pStart = &pRQB->args.startup;

  pStart->startup_param = *pnd_startup_param;
  pStart->cbf = pnd_agent_pnio_user_core_startup_done;
  pStart->pnio_err = PNIO_OK;

  LSA_RQB_SET_OPCODE(pRQB, PND_OPC_STARTUP);

  // Wait on callback, after request
  pnd_reset_event(pnd_instance.wait_event);

  pnd_request_start(PND_MBX_ID_PNIO_USER_CORE,
                    (PND_REQUEST_FCT)pnd_pnio_user_core_request,
                    (PNIO_VOID_PTR_TYPE)pRQB);

  pnd_wait_event(pnd_instance.wait_event);

  result = pStart->pnio_err;

  pnd_mem_free(pRQB);

  return result;
}

/*----------------------------------------------------------------------------*/
static PNIO_VOID pnd_agent_iob_core_shutdown_done(PNIO_VOID_PTR_TYPE p_rqb) {
  // runs in context of iobase core
  PND_ASSERT(p_rqb != 0);
  pnd_set_event(pnd_instance.wait_event);
}

/*----------------------------------------------------------------------------*/
static PNIO_UINT32 pnd_agent_iob_core_shutdown(PNIO_VOID) {
  PNIO_UINT32 result = PNIO_OK;

  PND_RQB_PTR_TYPE pRQB;
  PND_SHUTDOWN_PTR_TYPE pStop;

  pRQB = (PND_RQB_PTR_TYPE)pnd_mem_alloc(sizeof(*pRQB));
  PND_ASSERT(pRQB);

  pStop = &pRQB->args.shutdown;

  pStop->cbf = pnd_agent_iob_core_shutdown_done;
  pStop->pnio_err = PNIO_OK;

  LSA_RQB_SET_OPCODE(pRQB, PND_OPC_SHUTDOWN);

  // Wait on callback, after request
  pnd_reset_event(pnd_instance.wait_event);

  pnd_request_start(PND_MBX_ID_IO_BASE_CORE,
                    (PND_REQUEST_FCT)pnd_iob_core_request,
                    (PNIO_VOID_PTR_TYPE)pRQB);

  pnd_wait_event(pnd_instance.wait_event);

  result = pStop->pnio_err;

  pnd_mem_free(pRQB);

  return result;
}

/*----------------------------------------------------------------------------*/
static PNIO_VOID pnd_agent_pnio_user_core_shutdown_done(
    PNIO_VOID_PTR_TYPE p_rqb) {
  // runs in context of iobase core
  PND_ASSERT(p_rqb != 0);
  pnd_set_event(pnd_instance.wait_event);
}

/*----------------------------------------------------------------------------*/
static PNIO_UINT32 pnd_agent_pnio_user_core_shutdown(PNIO_VOID) {
  PNIO_UINT32 result = PNIO_OK;

  PND_RQB_PTR_TYPE pRQB;
  PND_SHUTDOWN_PTR_TYPE pStop;

  pRQB = (PND_RQB_PTR_TYPE)pnd_mem_alloc(sizeof(*pRQB));
  PND_ASSERT(pRQB);

  pStop = &pRQB->args.shutdown;

  pStop->cbf = pnd_agent_pnio_user_core_shutdown_done;
  pStop->pnio_err = PNIO_OK;

  LSA_RQB_SET_OPCODE(pRQB, PND_OPC_SHUTDOWN);

  // Wait on callback, after request
  pnd_reset_event(pnd_instance.wait_event);

  pnd_request_start(PND_MBX_ID_PNIO_USER_CORE,
                    (PND_REQUEST_FCT)pnd_pnio_user_core_request,
                    (PNIO_VOID_PTR_TYPE)pRQB);

  pnd_wait_event(pnd_instance.wait_event);

  result = pStop->pnio_err;

  pnd_mem_free(pRQB);

  return result;
}

/*---------------------------------------------------------------------------*/
/* Implementation of IOBASE Interface                                        */
/*---------------------------------------------------------------------------*/

PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_startup(
    PNIO_CP_ID_PTR_TYPE CpList, PNIO_UINT32 NrOfCp, PNIO_UINT8 *pConfigData,
    PNIO_UINT32 ConfigDataLen, PNIO_UINT8 *pRemaData, PNIO_UINT32 RemaDataLen,
    PNIO_SYSTEM_DESCR *pSysDescr) {
  PNIO_UINT32 result = PNIO_OK;

  if ((pnd_instance.init_done == PNIO_TRUE) ||
      (pnd_instance.status == PND_AGENT_STATUS_RUN) ||
      (pnd_instance.serv_cp_init_done == PNIO_FALSE))
    return PNIO_ERR_SEQUENCE;

  pnd_instance.status = PND_AGENT_STATUS_STOP;

  if ((NrOfCp > PSI_CFG_MAX_HD_CNT) ||
      (ConfigDataLen == 0))  // invalid value args ?
  {
    result = PNIO_ERR_PRM_INVALIDARG;
  } else {
    if ((CpList == 0) || (pConfigData == 0))  // invalid pointer args  ?
    {
      result = PNIO_ERR_PRM_POINTER;
    } else {
      if (pnd_instance.status == PND_AGENT_STATUS_STOP)  // invalid state ?
      {
        // startup the iobase system
        result = pnd_agent_startup(CpList, NrOfCp, pConfigData, ConfigDataLen,
                                   pRemaData, RemaDataLen, pSysDescr);

        if (result == PNIO_OK)  // startup ok ?
        {
          // change to state "startup done"

          pnd_instance.init_done = PNIO_TRUE;
          pnd_instance.status = PND_AGENT_STATUS_RUN;
        } else {
          pnd_instance.init_done = PNIO_FALSE;
          pnd_instance.status = PND_AGENT_STATUS_STOP;
          return (result);
        }

      } else {
        // invalid state
        result = PNIO_ERR_SEQUENCE;
      }
    }
  }

  return result;
}

PNIO_UINT32 PNIO_CODE_ATTR
SERV_CP_init(PNIO_DEBUG_SETTINGS_PTR_TYPE DebugSetting) {
  PNIO_UINT32 result = PNIO_OK;
  PNIO_UINT32 i;

  if (pnd_instance.serv_cp_init_done == PNIO_TRUE) return PNIO_ERR_SEQUENCE;

  pnd_instance.serv_cp_init_done = PNIO_TRUE;

  // check if DebugSetting == 0 -> disable trace
  if (DebugSetting == 0) {
    g_pnio_debug_settings.CbfPntrcBufferFull = PNIO_NULL;

    for (i = 0; i < PNIO_TRACE_COMP_NUM; i++) {
      g_pnio_debug_settings.TraceLevels[i] = PNIO_TRACE_LEVEL_OFF;
    }
  } else {
    g_pnio_debug_settings = *DebugSetting;
  }

  eps_init((PNIO_VOID_PTR_TYPE)&g_pnio_debug_settings);

  pnd_mbx_sys_init();

  pnd_threads_init();

  pnd_start_threads();

  result = pnd_alloc_event(&pnd_instance.wait_event);
  PND_ASSERT(result == PNIO_RET_OK);

  PND_STATIC_ASSERT(sizeof(PNIO_INT8) == sizeof(LSA_INT8),
                    "must have same size");
  PND_STATIC_ASSERT(sizeof(PNIO_INT16) == sizeof(LSA_INT16),
                    "must have same size");
  PND_STATIC_ASSERT(sizeof(PNIO_INT32) == sizeof(LSA_INT32),
                    "must have same size");
  PND_STATIC_ASSERT(sizeof(PNIO_UINT8) == sizeof(LSA_UINT8),
                    "must have same size");
  PND_STATIC_ASSERT(sizeof(PNIO_UINT8) == sizeof(LSA_UINT8),
                    "must have same size");
  PND_STATIC_ASSERT(sizeof(PNIO_UINT16) == sizeof(LSA_UINT16),
                    "must have same size");
  PND_STATIC_ASSERT(sizeof(PNIO_UINT32) == sizeof(LSA_UINT32),
                    "must have same size");
  PND_STATIC_ASSERT(sizeof(PNIO_BOOL) == sizeof(LSA_BOOL),
                    "must have same size");
  PND_STATIC_ASSERT(sizeof(PNIO_VOID_PTR_TYPE) == sizeof(PNIO_VOID_PTR_TYPE),
                    "must have same size");

  return PNIO_OK;
}

PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_shutdown() {
  PNIO_UINT32 result = 0;

  if ((pnd_instance.init_done == PNIO_FALSE) ||
      (pnd_instance.status == PND_AGENT_STATUS_STOP) ||
      (pnd_instance.serv_cp_init_done == PNIO_FALSE))
    return PNIO_ERR_SEQUENCE;

  result = pnd_agent_shutdown();
  if (result != PNIO_OK) {
    return result;
  }

  pnd_instance.init_done = PNIO_FALSE;
  pnd_instance.status = PND_AGENT_STATUS_STOP;

  return result;
}

PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_undo_init() {
  PNIO_UINT32 result;

  if (pnd_instance.serv_cp_init_done == PNIO_FALSE) return PNIO_ERR_SEQUENCE;

  result = pnd_pnio_user_core_undo_init();
  if (result != PNIO_OK) {
    return result;
  }

  pnd_instance.serv_cp_init_done = PNIO_FALSE;

  pnd_iobase_core_undo_init();

  CParamStore::DeleteInstance();

  pnd_sys_undo_init();

  return PNIO_OK;
}

void pntrc_rqb_done(PNIO_VOID *pRQB0) {
  PNIO_UINT16 rc;
  PNTRC_RQB_PTR_TYPE pRQB;
  PNIO_PNTRC_SET_TRACE_LEVEL_DONE user_cbf;

  PND_ASSERT(pRQB0 != 0);

  pRQB = (PNTRC_RQB_PTR_TYPE)pRQB0;

  user_cbf = (PNIO_PNTRC_SET_TRACE_LEVEL_DONE)pRQB->_user_func;

  PNTRC_FREE_LOCAL_MEM(&rc, pRQB);

  PND_ASSERT(rc == PNIO_RET_OK);

  // call user CBF if there is any
  if (user_cbf != PNIO_NULL) {
    user_cbf();
  }
}

PNIO_UINT32 PNIO_CODE_ATTR
SERV_CP_set_trace_level(PNIO_UINT32 Component, PNIO_UINT32 TraceLevel,
                        PNIO_PNTRC_SET_TRACE_LEVEL_DONE CbfSetTraceLevelDone) {
  PNIO_UINT32 result = PNIO_OK;
  PNTRC_RQB_PTR_TYPE pRQB = 0;

  // check state
  if (pnd_instance.init_done == PNIO_FALSE) {
    result = PNIO_ERR_SEQUENCE;
  } else {
    if ((Component < PNIO_TRACE_COMP_NUM) &&
        (TraceLevel < PNIO_TRACE_LEVEL_NUM))  // parameter check
    {
      PNTRC_ALLOC_LOCAL_MEM((void **)&pRQB, sizeof(PNTRC_RQB_TYPE));

      PNTRC_RQB_SET_OPCODE(pRQB, PNTRC_OPC_SET_TRACE_LEVELS_COMP);

      pRQB->args.set_trace_levels_comp.comp = (LTRC_COMP_TYPE)Component;

      pRQB->args.set_trace_levels_comp.level = (PNTRC_LEVEL_TYPE)TraceLevel;

      pRQB->_user_func = (void (*)(void *, void *))CbfSetTraceLevelDone;

      pRQB->Cbf = pntrc_rqb_done;

      pntrc_request(pRQB);
    } else {
      result = PNIO_ERR_PRM_INVALIDARG;
    }
  }
  return result;
}

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)

#define PND_CFG_WPCAPDRV_MAX_BOARDS 50

typedef struct eps_stdmacdrv_device_info_tag {
  LSA_CHAR *pName;
  LSA_CHAR Name[300];
  PNIO_UINT8 uMac[6];
  LSA_CHAR description[300];
} PND_WPCAPDRV_DEVICE_INFO_TYPE, *PND_WPCAPDRV_DEVICE_INFO_PTR_TYPE;

typedef struct eps_wpcapdrv_store_tag {
  PND_WPCAPDRV_DEVICE_INFO_TYPE foundDevices[PND_CFG_WPCAPDRV_MAX_BOARDS];
  PNIO_UINT32 uCntFoundDevices;

} EPS_WPCAPDRV_STORE_TYPE, *EPS_WPCAPDRV_STORE_PTR_TYPE;

PNIO_VOID pnd_wpcapdrv_find_boards(EPS_WPCAPDRV_STORE_PTR_TYPE pPndWpcapDrv) {
  PNIO_INT32 result;
  pcap_if_t *alldevs;
  pcap_if_t *dev;
  char errbuf[PCAP_ERRBUF_SIZE];
  LPADAPTER lpAdapter = 0;
  char dev_name[PCAP_BUF_SIZE] = {0};

  struct {
    PACKET_OID_DATA pkt;
    char more[6];
  } buf;

  PND_ASSERT(pPndWpcapDrv != PNIO_NULL);

  pnd_memset(pPndWpcapDrv, 0, sizeof(EPS_WPCAPDRV_STORE_TYPE));

  result = pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf);

  if (result != -1) {
    for (dev = alldevs; (dev != PNIO_NULL) && (pPndWpcapDrv->uCntFoundDevices <
                                               PND_CFG_WPCAPDRV_MAX_BOARDS);
         dev = dev->next) {
      pnd_memcpy(dev_name, dev->name + pnd_strlen(PCAP_SRC_IF_STRING),
                 pnd_strlen(dev->name) - pnd_strlen(PCAP_SRC_IF_STRING));

      // query the mac-address
      lpAdapter = PacketOpenAdapter(dev_name);

      EPS_ASSERT(0 != lpAdapter);

      buf.pkt.Oid = OID_802_3_CURRENT_ADDRESS;
      buf.pkt.Length = 6;

      if (PacketRequest(lpAdapter, PNIO_FALSE, &buf.pkt) &&
          buf.pkt.Length == 6) {
        pnd_memcpy(pPndWpcapDrv->foundDevices[pPndWpcapDrv->uCntFoundDevices]
                       .description,
                   dev->description, pnd_strlen(dev->description));
        pnd_memcpy(
            pPndWpcapDrv->foundDevices[pPndWpcapDrv->uCntFoundDevices].uMac,
            buf.pkt.Data,
            sizeof(pPndWpcapDrv->foundDevices[pPndWpcapDrv->uCntFoundDevices]
                       .uMac));

        pPndWpcapDrv->uCntFoundDevices++;
      }
      PacketCloseAdapter(lpAdapter);
    }
  }

  pcap_freealldevs(alldevs);
}

PNIO_UINT32 pnd_get_mac_wpcapdrv(PNIO_CP_ID_PTR_TYPE cp_list,
                                 PNIO_UINT8 *NumberOfCps) {
  PNIO_CP_ID_PTR_TYPE pCpList = cp_list;
  PND_WPCAPDRV_DEVICE_INFO_PTR_TYPE pDevice;
  PNIO_UINT8 *subchar_array;
  EPS_WPCAPDRV_STORE_TYPE pndWpcapDrv;

  pnd_wpcapdrv_find_boards(&pndWpcapDrv);

  if (pndWpcapDrv.uCntFoundDevices == 0) {
    return PNIO_ERR_NO_ADAPTER_FOUND;
  }

  for (PNIO_UINT8 i = 0; i < pndWpcapDrv.uCntFoundDevices; i++) {
    pDevice = &pndWpcapDrv.foundDevices[i];

    pCpList[i].CpSelection = PNIO_CP_SELECT_WITH_MAC_ADDRESS;
    pCpList[i].CpMacAddr[0] = pDevice->uMac[0];
    pCpList[i].CpMacAddr[1] = pDevice->uMac[1];
    pCpList[i].CpMacAddr[2] = pDevice->uMac[2];
    pCpList[i].CpMacAddr[3] = pDevice->uMac[3];
    pCpList[i].CpMacAddr[4] = pDevice->uMac[4];
    pCpList[i].CpMacAddr[5] = pDevice->uMac[5];

    subchar_array = (PNIO_UINT8 *)strtok(pDevice->description, "'");

    if (subchar_array != PNIO_NULL) {
      subchar_array = (PNIO_UINT8 *)strtok(PNIO_NULL, "'");
      pnd_memcpy(pCpList[i].Description, subchar_array,
                 pnd_strlen((LSA_CHAR *)subchar_array));
    }
  }

  *NumberOfCps = (PNIO_UINT8)pndWpcapDrv.uCntFoundDevices;

  return PNIO_OK;
}

#endif

#if (EPS_PLF == EPS_PLF_LINUX_X86)

PNIO_UINT32 pnd_get_pci_pndevdrv(PNIO_CP_ID_PTR_TYPE cp_list,
                                 PNIO_UINT8 *NumberOfCps) {
  PNIO_CP_ID_PTR_TYPE pCpList = cp_list;
  EPS_PNDEV_IF_DEV_INFO_TYPE
      sPnBoard[10];  // Count must be same as BoardInfoStruct[]
  PNIO_UINT16 deviceCount = 0;
  PNIO_UINT16 validDeviceCount = 0;

  eps_pndev_if_get_registered_devices(sPnBoard, 10, &deviceCount);

  for (PNIO_UINT16 i = 0; i < deviceCount; i++) {
    if (sPnBoard[i].uPciAddrValid == 1) {
      PNIO_UINT8 board_name[64] = {0};

#if defined(EPS_CFG_USE_PNDEVDRV)
      switch (sPnBoard[i].eBoard) {
        case ePNDEV_BOARD_CP1616:  // CP1616 board
          eps_strcpy(board_name, (PNIO_UINT8 *)" CP1616 board");
          break;
        case ePNDEV_BOARD_DB_EB400_PCIE:  // DB-EB400-PCIe board
          eps_strcpy(board_name, (PNIO_UINT8 *)"DB-EB400-PCIe board");
          break;
        case ePNDEV_BOARD_EB200:  // EB200 board
          eps_strcpy(board_name, (PNIO_UINT8 *)"EB200 board");
          break;
        case ePNDEV_BOARD_DB_SOC1_PCI:  // Soc1-Dev board
          eps_strcpy(board_name, (PNIO_UINT8 *)"Soc1-Dev board - PCI");
          break;
        case ePNDEV_BOARD_DB_SOC1_PCIE:  // Soc1-Dev board
          eps_strcpy(board_name, (PNIO_UINT8 *)"Soc1-Dev board - PCIE");
          break;
        case ePNDEV_BOARD_FPGA1_ERTEC200P:  // FPGA1-Ertec200P board
          eps_strcpy(board_name, (PNIO_UINT8 *)"FPGA1-Ertec200P board");
          break;
        case ePNDEV_BOARD_EB200P:  // EB200P board
          eps_strcpy(board_name, (PNIO_UINT8 *)"EB200P board");
          break;
        case ePNDEV_BOARD_I210:  // I210 board
          eps_strcpy(board_name, (PNIO_UINT8 *)"I210 board");
          break;
        case ePNDEV_BOARD_I82574:  // I82574 board
          eps_strcpy(board_name, (PNIO_UINT8 *)"I82574 board");
          break;
        default:
          EPS_FATAL("Can't register a not supported board.");
          break;
      }
#endif

      pCpList[i].CpSelection = PNIO_CP_SELECT_WITH_PCI_LOCATION;
      pCpList[i].CpPciLocation.BusNr = sPnBoard[i].uPciBusNr;
      pCpList[i].CpPciLocation.DeviceNr = sPnBoard[i].uPciDeviceNr;
      pCpList[i].CpPciLocation.FunctionNr = sPnBoard[i].uPciFuncNr;

      pnd_memcpy(pCpList[i].Description, board_name, sizeof(board_name));

      validDeviceCount++;
    }
  }

  *NumberOfCps = validDeviceCount;

  if (validDeviceCount == 0) {
    return PNIO_ERR_NO_ADAPTER_FOUND;
  }

  return PNIO_OK;
}
#endif

PNIO_UINT32 PNIO_CODE_ATTR
SERV_CP_get_network_adapters(PNIO_CP_ID_PTR_TYPE CpList,  // output
                             PNIO_UINT8 *NumberOfCps)     // output
{
  PNIO_UINT32 result;

  PND_ASSERT(CpList != PNIO_NULL);

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  result = pnd_get_mac_wpcapdrv(CpList, NumberOfCps);
#elif (EPS_PLF == EPS_PLF_LINUX_X86)
  result = pnd_get_pci_pndevdrv(CpList, NumberOfCps);
#endif

  return result;
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
