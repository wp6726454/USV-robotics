#ifndef PND_PNIO_USER_CORE_H
#define PND_PNIO_USER_CORE_H

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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_pnio_user_core.h                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Definition of the PnDriver PNIO user core interface                      */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/* construction and destruction C-IF */
LSA_UINT32 pnd_pnio_user_core_init( LSA_VOID );
LSA_UINT32 pnd_pnio_user_core_undo_init( LSA_VOID );

/* request C-IF */
LSA_VOID pnd_pnio_user_core_request( PND_RQB_PTR_TYPE  rqb_ptr );
LSA_VOID pnd_pnio_user_core_request_done( PND_RQB_PTR_TYPE  rqb_ptr );

/*---------------------------------------------------------------------------*/

LSA_VOID pnd_pnio_user_request_local(  // local request to PNIO User task
	LSA_VOID_PTR_TYPE  rqb_ptr,    // RQB
	LSA_VOID_PTR_TYPE  p_inst,     // Instance
	PND_CORE_DONE_FCT  p_fct       // User done function
);

LSA_VOID pnd_pnio_user_request_local_done( LSA_VOID_PTR_TYPE p_rqb );

/*----------------------------------------------------------------------------*/
/* central CBF function used for all PNIO stack user channels at open channel */
LSA_VOID pnd_pnio_user_request_lower_done( LSA_VOID_PTR_TYPE p_rqb );

/*---------------------------------------------------------------------------*/
/* Plain C-Methodes from PND PNIO user core to IO-Base core messages         */
/*---------------------------------------------------------------------------*/

LSA_VOID pnd_pnio_user_request_iobc(  // request to IO-Base core
	LSA_VOID_PTR_TYPE  rqb_ptr,       // RQB
	LSA_VOID_PTR_TYPE  p_inst,        // Instance
	PND_CORE_DONE_FCT  p_fct          // User done function
);

LSA_VOID pnd_pnio_user_request_iobc_done( LSA_VOID_PTR_TYPE rqb_ptr );


/*---------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PND_PNIO_USER_CORE_H */
