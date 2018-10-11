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
/*  C o m p o n e n t     &C: PROFINET IO Runtime Software              :C&  */
/*                                                                           */
/*  F i l e               &F: pnio_version.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  This component implements only the version function of the system LSA    */
/*  interface .                                                              */
/*  The function provides the version of the actual used PNIO stack.         */
/*                                                                           */
/*****************************************************************************/

#include "lsa_cfg.h"
#include "lsa_sys.h"
#include "pnio_version.h"

/*------------------------------------------------------------------------------
//	return the current version-info of PNIO in the needed length
//----------------------------------------------------------------------------*/
LSA_UINT16 pnio_version ( LSA_UINT16 length, LSA_VERSION_TYPE *version )
{
  LSA_UINT8 *p_version, *p_v;
  LSA_UINT16 i;
 
  /* Initialize version struct */
  LSA_VERSION_TYPE v = {
	                    PNIO_LSA_COMPONENT_ID, PNIO_LSA_PREFIX,
		 	            PNIO_KIND,             PNIO_VERSION,
			            PNIO_DISTRIBUTION,     PNIO_FIX,
			            PNIO_HOTFIX,           PNIO_PROJECT_NUMBER,
			            PNIO_INCREMENT,        PNIO_INTEGRATION_COUNTER,
			            PNIO_GEN_COUNTER
                       };

  /* Adapt length if necessary */
  if (length > sizeof(v)) 
  {
    length = sizeof(v);
  }

  /* Initialize pointers and copy version structure */
  p_version = (LSA_UINT8 *) version;
  p_v 		= (LSA_UINT8 *) &v;
  for ( i=0;i<length; i++ ) { p_version[i] = p_v[i]; };
  
  return (LSA_UINT16)(sizeof(v) - length);
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
