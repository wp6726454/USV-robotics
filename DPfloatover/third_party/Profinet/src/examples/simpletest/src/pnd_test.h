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
/*  F i l e               &F: pnd_test.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Test Application for PNDriver                                            */
/*                                                                           */
/*****************************************************************************/

#include <eps_rtos.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pniobase.h"
#include "pniousrx.h"
#include "servusrx.h"

#define TEST_IODU_MAX_DATA_LEN 1024

void pnd_test_buffer_full(PNIO_UINT8 *pBuffer, PNIO_UINT32 BufferSize);

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
// pnd_test functions
void pnd_test_set_mode(PNIO_MODE_TYPE mode);
void pnd_test_controller_open(PNIO_DEBUG_SETTINGS_PTR_TYPE DebugSettings);
void pnd_test_controller_close(void);
void pnd_test_register_setmode_cbf(void);
void pnd_test_register_devact_cbf(void);
void pnd_test_register_diag_req_cbf(void);
void pnd_test_register_iosystem_reconfig(void);
PNIO_UINT32 pnd_test_dev_activate(PNIO_DEV_ACT_TYPE activate_mode);
void pnd_test_iodu_write();
void pnd_test_iodu_read();
void pnd_test_iodu();
void pnd_test_trig_diag_req(void);
void pnd_test_ifc_set_ip_and_nos(void);
void pnd_test_iosystem_reconfig(void);
void pnd_test_interface_open(PNIO_DEBUG_SETTINGS_PTR_TYPE DebugSettings);
void pnd_test_interface_close();
void pnd_test_register_interface_set_ip_and_nos(void);
void pnd_test_register_interface_rema_read(void);
PNIO_UINT32 pnd_test_interface_data_read();
void pnd_test_interface_set_ip_and_nos(void);
void my_pnd_test_network_adapter_selection(PNIO_CP_ID_TYPE *cp_id,
                                           PNIO_CP_ID_PTR_TYPE cp_list,
                                           PNIO_UINT8 nrofcp, FILE *_file);
void pnd_test_network_adapter_selection(PNIO_CP_ID_TYPE *cp_id,
                                        PNIO_CP_ID_PTR_TYPE cp_list,
                                        PNIO_UINT8 nrofcp);

// helper functions
void pnd_test_quit_application(PNIO_UINT32 handle);

void send_20ms_50Hz(void);

void send_50ms_20Hz(void);

#ifdef __cplusplus
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
