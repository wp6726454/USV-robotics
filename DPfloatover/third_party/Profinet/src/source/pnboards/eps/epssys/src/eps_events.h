#ifndef EPS_EVENTS_H_
#define EPS_EVENTS_H_

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
/*  F i l e               &F: eps_events.h                              :F&  */
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
/*****************************************************************************/

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/

#define EPS_EVENT_ID_INVALID   0

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID eps_events_init            ( LSA_VOID );
LSA_VOID eps_events_undo_init       ( LSA_VOID );

LSA_RESPONSE_TYPE   eps_alloc_event ( LSA_UINT16 *event_id_ptr);
LSA_RESPONSE_TYPE   eps_free_event  ( LSA_UINT16 event_id );

LSA_VOID            eps_set_event   ( LSA_UINT16 event_id );
LSA_VOID            eps_reset_event ( LSA_UINT16 event_id );
LSA_RESPONSE_TYPE   eps_wait_event  ( LSA_UINT16 event_id, LSA_UINT16 wait_sec );

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_EVENTS_H_ */