#ifndef PND_IODU_EDDS_H
#define PND_IODU_EDDS_H

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
/*  F i l e               &F: pnd_iodu_edds.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Child class for IODU for Std MAC (EDDS)                                  */
/*****************************************************************************/

#include "pnd_iodu.h"
#include "edds_iobuf_inc.h"

class CIODU;

class CIODU_EDDS : public CIODU
{
	// Attributes
private:
	EDDS_IOBUFFER_HANDLE_TYPE ioHandle;

    virtual PNIO_VOID update_provider_shadow_buffer(PND_IODU_BUFFER_PTR_TYPE pIODU_buffer_handle);
    virtual PNIO_VOID update_consumer_shadow_buffer(PND_IODU_BUFFER_PTR_TYPE pIODU_buffer_handle);

public:
	CIODU_EDDS(PND_HANDLE_TYPE pnd_handle);
	virtual ~CIODU_EDDS();


	// implementation of abstract class methodes
	virtual PNIO_VOID	PI_Alloc(PND_RQB_PTR_TYPE  rqb_ptr);
	virtual PNIO_VOID	PI_Free(PND_RQB_PTR_TYPE  rqb_ptr);

protected:

private:
};

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_IODU_EDDS_H */
