#ifndef _PNSERVERT_H_
#define _PNSERVERT_H_

#include <eps_rtos.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pniousrx.h"

#include <stdio.h>
#include "servusrx.h"

#include "constants.h"
#include "realtimedata.h"

static FILE *PNTRC_fp = 0;
#define TEST_IODU_MAX_DATA_LEN 1024
static PNIO_DEBUG_SETTINGS_TYPE debSet;
static PNIO_UINT32 g_ApplHandle = 0xFFFF;

#ifdef __cplusplus
extern "C" {
#endif

// pnd_test functions
void pnd_test_buffer_full(PNIO_UINT8 *pBuffer, PNIO_UINT32 BufferSize);
void pnd_test_set_mode(PNIO_MODE_TYPE mode);
void pnd_test_controller_open(PNIO_DEBUG_SETTINGS_PTR_TYPE DebugSettings);
void pnd_test_controller_close(void);
void pnd_test_register_setmode_cbf(void);
void pnd_test_register_devact_cbf(void);
void pnd_test_register_diag_req_cbf(void);
void pnd_test_register_iosystem_reconfig(void);

void my_pnd_test_network_adapter_selection(PNIO_CP_ID_TYPE *cp_id,
                                           PNIO_CP_ID_PTR_TYPE cp_list,
                                           PNIO_UINT8 nrofcp, FILE *_file);
void pnd_test_network_adapter_selection(PNIO_CP_ID_TYPE *cp_id,
                                        PNIO_CP_ID_PTR_TYPE cp_list,
                                        PNIO_UINT8 nrofcp);

// helper functions
void pnd_test_quit_application(PNIO_UINT32 handle);

void terminationHandler(int sigNum);
void pnd_init();
PNIO_UINT32 _SERV_CP_shutdown();
PNIO_UINT32 _SERV_CP_Startup();
void _openpncontroller();
void _closepncontroller();
void send_20ms_50Hz(void);
void send_50ms_20Hz(void);
void pntest();

void send2firstvessel(const realtimevessel_first *_realtimevessel_first,
                      FILE *_file);
void send2bothvessel(const realtimevessel_first *_realtimevessel_first,
                     const realtimevessel_second *_realtimevessel_second,
                     FILE *_file);
#ifdef __cplusplus
}
#endif

#endif /* _PNSERVERT_H_ */