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
/*  F i l e               &F: pnd_test_pduser.cpp                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  PDUser test functions for PNDriver                                       */
/*                                                                           */
/*****************************************************************************/

#include "pniobase.h"
#include "servusrx.h"
#include "pniobase.h"
#include "pniousrx.h"

#include "eps_sys.h"

#include "pnd_int.h"
#include "pnd_sys.h"

#include "pnd_test.h"

#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_trc.h"
#include "pnioerrx.h"
#include "pnd_pnstack.h"
#include "pnd_pnio_user_core.h"

/* StdLib++ */
#include <string>
#include <map>
#include <vector>

using namespace std;

#include "pnd_BgzdStore.h"
#include "pnd_ValueHelper.h"
#include "pnd_UserNode.h"
#include "pnd_UserStore.h"

#include "pnd_ParamStore.h"
#include "pnd_PDEventHandler.h"
#include "pnd_PDSubslot.h"
#include "pnd_PDUser.h"

void pnd_test_pd_user(PNIO_DEBUG_SETTINGS_PTR_TYPE  DebugSettings)
{
	
    PNIO_VOID_PTR_TYPE rqb_ptr = 0;
    PND_PARAM_PD_HD_IF_TYPE params;
    PND_PARAM_PD_SUB_ENTRY_TYPE subslot_info[5];

    params.map_count = 1;
    params.pdev_validation = PNIO_TRUE;
    for (int i = 0; i<5; i++)
    {
        subslot_info[i].slot_nr = 1;
        subslot_info[i].subslot_nr = 1;
        subslot_info[i].laddr = 0;
        subslot_info[i].sub_ident = 0;

        params.pdev_sub[i] = subslot_info[i];
    }

	eps_init((PNIO_VOID_PTR_TYPE)DebugSettings);

	pnd_mbx_sys_init();

	pnd_threads_init();

	pnd_start_threads();

	rqb_ptr = pnd_mem_alloc(sizeof(PND_RQB_PTR_TYPE));
	PND_RQB_PTR_TYPE              pRQB;
	pRQB = (PND_RQB_PTR_TYPE)rqb_ptr;

	CPDUser *pUserPD = new CPDUser();
	PND_ASSERT( pUserPD != 0 );

	pUserPD->Create( PNIO_NULL, &params );
    pUserPD->Startup(pRQB);

    pnd_mbx_sys_undo_init();

    pnd_threads_undo_init();

    eps_undo_init();
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/