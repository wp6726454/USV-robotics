
#ifndef EDDS_TOOLBOX_H                    /* ----- reinclude-protection ----- */
#define EDDS_TOOLBOX_H

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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_toolbox.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal toolbox library for EDDS and Lower Layer Interface (LLIF).      */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  12.04.02    MA    initial version.                                       */
#endif
/*****************************************************************************/

/*===========================================================================*/
/* timer operations                                                          */
/*===========================================================================*/
/** Wait until 'condition' is true.
 * Execute 'ontimeout' after 'microseconds' if condition is still false
 */
#define EDDS_WAIT_UNTIL_TIMEOUT(condition, microseconds, ontimeout) \
	{\
		LSA_INT32 count = microseconds;\
		while(!(condition)&&(0<count)) {\
			EDDS_WAIT_US(1);\
			--count;\
		}\
		if(0>=count) { \
			ontimeout ;\
		}\
	}

/** Execute a statement and wait until 'condition' is true;
 * Execute 'ontimeout' after 'microseconds' if condition is still false
 */
#define EDDS_DO_UNTIL_TIMEOUT(statement, condition, microseconds, ontimeout) \
  {\
    LSA_INT32 count = microseconds;\
    do {\
      statement ;\
      if(!(condition)&&(0<count)) { EDDS_WAIT_US(1);--count;}\
      else { break; }\
    } while(1); \
    if(0>=count) { \
      ontimeout ;\
		}\
	}


/*****************************************************************************/
/*  end of file edds_tools.h                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_TOOLBOX_H */
