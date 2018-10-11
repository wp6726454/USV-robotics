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
/*  F i l e               &F: pnd_iodu_edds.cpp                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of IODU_EDDS class                                       */
/*                                                                           */
/*****************************************************************************/


#define LTRC_ACT_MODUL_ID   1202
#define PND_MODULE_ID       1202

/////////////////////////////////////////////////////////////////////////////

#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_trc.h"
#include "pnioerrx.h"

#include "pnd_pnio_user_core.h"

#include "pnd_iodu_edds.h"

/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */


/////////////////////////////////////////////////////////////////////////////
// CUserStore class
/////////////////////////////////////////////////////////////////////////////

CIODU_EDDS::~CIODU_EDDS()
{

}

/*----------------------------------------------------------------------------*/
CIODU_EDDS::CIODU_EDDS( PND_HANDLE_TYPE pndHandle) : CIODU( pndHandle)
{
    this->ioHandle = (EDDS_IOBUFFER_HANDLE_TYPE)psi_hd_get_edd_io_handle( (PNIO_UINT16)pndHandle.hd_nr );
}


/*----------------------------------------------------------------------------*/

PNIO_VOID CIODU_EDDS::PI_Alloc(PND_RQB_PTR_TYPE  rqb_ptr)
{
    PND_BUFFER_PROPERTIES_PTR_TYPE        rqb_args;
    PNIO_UINT16                            retval                  = LSA_OK;
    PNIO_UINT32                            m_partialLength;
    PNIO_UINT16                            m_allocSize;
    PNIO_UINT8*                            m_pImage;
    PNIO_UINT16                            pHandle;
    PND_IODU_BUFFER_PTR_TYPE              pIODU_Buffer;
    PNIO_UINT32                            k;
    PNIO_UINT32                            pi_buffer_array_index = 0;
    PNIO_UINT32                            no_buffer_needed = PNIO_FALSE;

    for(k=1; k<=2; k++)//do the same for the consumer_buffer and provider_buffer
    {
        if(k==1)//pick consumer buffer
        {
            rqb_args = &(rqb_ptr->args.pi_alloc.consumer_buffer);
        }
        else//pick provider buffer
        {
            rqb_args = &(rqb_ptr->args.pi_alloc.provider_buffer);
        }


        if ( rqb_args->partial_length != 0 ) // buffer to allocate ?
        {

            PND_ASSERT(k==1 || k==2);

            m_partialLength = rqb_args->partial_length;

            if((rqb_args->is_provider == PNIO_TRUE) && (k==2)/*plausibility check*/)
            {
                /* Find a free spot to store a link to the provider buffer */
                for(pi_buffer_array_index = 0; pi_buffer_array_index < PND_IODU_CFG_MAX_PROVIDER_PI_BUFFERS; pi_buffer_array_index++)
                {
                    if(PNIO_NULL == (this->provider_pi_buffer_array[pi_buffer_array_index]))
                    {
                        break;
                    }
                }
                if(pi_buffer_array_index >= PND_IODU_CFG_MAX_PROVIDER_PI_BUFFERS)
                {
                    retval = LSA_RSP_ERR_RESOURCE;
                    continue; //process next PI_ALLOC request
                }
            }
            else if((rqb_args->is_provider == PNIO_FALSE) && (k==1)/*plausibility check*/)
            {
                /* Find a free spot to store a link to the consumer buffer */
                for(pi_buffer_array_index = 0; pi_buffer_array_index < PND_IODU_CFG_MAX_CONSUMER_PI_BUFFERS; pi_buffer_array_index++)
                {
                    if(PNIO_NULL == (this->consumer_pi_buffer_array[pi_buffer_array_index]))
                    {
                        break;
                    }
                }
                if(pi_buffer_array_index >= PND_IODU_CFG_MAX_CONSUMER_PI_BUFFERS)
                {
                    retval = LSA_RSP_ERR_RESOURCE;
                    continue; //process next PI_ALLOC request
                }
            }
            else
            {
                PND_FATAL("is_provider is not set correctly!");
            }

   
            m_allocSize = (PNIO_UINT16)rqb_args->partial_length;

            // these arguments are not used with EDDS
            rqb_args->edd_data_offset       = 0;
            rqb_args->edd_forward_mode      = 0;
            rqb_args->edd_ct_outdatastart   = 0;
            rqb_args->edd_properties        = 0;

            // Alloc the buffer by using EDDS API
            if(rqb_args->is_provider == PNIO_TRUE)
            {
                pHandle = EDDS_IOBuffer_ProviderAlloc( this->ioHandle, m_allocSize, EDDS_CSRT_PROVIDER_TYPE_XRT );

                if ( pHandle != EDD_CRNUMBER_UNDEFINED ) // EDDS buffer avaliable ?
                {
                    // Set CR number of this buffer
                    rqb_args->edd_cr_number = pHandle;

                    m_pImage = EDDS_IOBuffer_ProviderLockCurrent( this->ioHandle, pHandle );
                    // Init the user buffer
                    pnd_memset((PNIO_VOID_PTR_TYPE)m_pImage, 0, m_allocSize);

                    EDDS_IOBuffer_ProviderUnlock( this->ioHandle, pHandle );

                    PND_IODU_TRACE_01(this->pnd_handle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,"PI_Alloc: EDDS provider buffer alloc done. rqb(0x%08x)", rqb_ptr);
                }
                else
                {
                    retval = LSA_RSP_ERR_RESOURCE;
                    PND_IODU_TRACE_02(this->pnd_handle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,"PI_Alloc: No EDDS provider buffer available! rqb(0x%08x), pHandle(0x%08x)", rqb_ptr, pHandle);
                }
            }
            else
            {
                pHandle = EDDS_IOBuffer_ConsumerAlloc( this->ioHandle, m_allocSize, EDDS_CSRT_CONSUMER_TYPE_XRT );

                if ( pHandle != EDD_CRNUMBER_UNDEFINED ) // EDDS buffer avaliable ?
                {
                    // Set CR number of this buffer
                    rqb_args->edd_cr_number=  pHandle;

                    m_pImage = EDDS_IOBuffer_ConsumerLock( this->ioHandle, pHandle );
                    // Init the user buffer
                    pnd_memset((PNIO_VOID_PTR_TYPE)m_pImage, 0, m_allocSize);

                    EDDS_IOBuffer_ConsumerUnlock( this->ioHandle, pHandle );

                    PND_IODU_TRACE_01(this->pnd_handle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,"PI_Alloc: EDDS consumer buffer alloc done. rqb(0x%08x)", rqb_ptr);
                }
                else
                {
                    retval = LSA_RSP_ERR_RESOURCE;
                    PND_IODU_TRACE_02(this->pnd_handle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,"PI_Alloc: No EDDS consumer buffer available! rqb(0x%08x), pHandle(0x%08x)", rqb_ptr, pHandle);
                }
            }


            if(retval == LSA_OK)
            {//alloc shadow buffer                
                rqb_args->buffer_handle = pnd_mem_alloc(sizeof(PND_IODU_BUFFER_TYPE)); 
                if(PNIO_NULL == (rqb_args->buffer_handle))
                {
                    retval = LSA_RSP_ERR_RESOURCE;
                    //free Buffer on EDDS
                    (rqb_args->is_provider == PNIO_TRUE) ? EDDS_IOBuffer_ProviderFree (this->ioHandle, pHandle) : EDDS_IOBuffer_ConsumerFree (this->ioHandle, pHandle);

                    PND_IODU_TRACE_01(this->pnd_handle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,"PI_Alloc: No local Mem available (buffer_handle)! rqb(0x%08x)", rqb_ptr);
                }
                else
                {
                    pIODU_Buffer = (PND_IODU_BUFFER_PTR_TYPE)rqb_args->buffer_handle;
                    pIODU_Buffer->is_provider = rqb_args->is_provider;
                    pIODU_Buffer->p_shadow_buffer = (PNIO_UINT8*)pnd_mem_alloc(m_allocSize);
                    if(PNIO_NULL == (pIODU_Buffer->p_shadow_buffer))
                    {
                        retval = LSA_RSP_ERR_RESOURCE;
                        //free Buffer on EDDS
                        (rqb_args->is_provider == PNIO_TRUE) ? EDDS_IOBuffer_ProviderFree (this->ioHandle, pHandle) : EDDS_IOBuffer_ConsumerFree (this->ioHandle, pHandle);
                        pnd_mem_free((PNIO_VOID*)pIODU_Buffer); //free buffer_handle
                        PND_IODU_TRACE_01(this->pnd_handle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,"PI_Alloc: No local Mem available (p_shadow_buffer)! rqb(0x%08x)", rqb_ptr);
                    }
                    else
                    {//all allocs successful
                        pnd_memset((PNIO_VOID_PTR_TYPE)pIODU_Buffer->p_shadow_buffer, 0, m_allocSize); 
                        pIODU_Buffer->buffer_handle = pHandle;
                        pIODU_Buffer->shadow_buffer_size = m_allocSize;

                        if(rqb_args->is_provider == PNIO_TRUE)
                        {
                            this->provider_pi_buffer_array[pi_buffer_array_index] = pIODU_Buffer; //store IODU buffer handle in global provider PI buffer array
                        }
                        else
                        {
                            this->consumer_pi_buffer_array[pi_buffer_array_index] = pIODU_Buffer; //store IODU buffer handle in global consumer PI buffer array
                        }

                        PND_IODU_TRACE_01(this->pnd_handle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,"PI_Alloc: Shadow buffer alloc done. rqb(0x%08x)", rqb_ptr);
                    }
                }
            }  
        }
        else
        {
            PND_IODU_TRACE_01(this->pnd_handle.pnio_if_nr, LSA_TRACE_LEVEL_NOTE,"PI_Alloc: partial_length == 0 -> No EDDS buffer needed. rqb(0x%08x)", rqb_ptr);
            no_buffer_needed = PNIO_TRUE;
        }
    }

    if(no_buffer_needed == PNIO_FALSE)//continue with creating IQ table entry
    {
        //create IQ address to IO buffer mapping
        if(retval == LSA_OK)
        {
            retval = this->check_params_IO_addr(rqb_ptr);
            if(retval == LSA_OK)
            {
                retval = this->create_IQ_table(rqb_ptr);
            }
        }
    }

    LSA_RQB_SET_RESPONSE(rqb_ptr, retval); //LSA ERROR codes verwenden

    if(retval == LSA_OK)
    {
        rqb_ptr->args.pi_alloc.pnio_err = PNIO_OK;
    }
    else
    {
        rqb_ptr->args.pi_alloc.pnio_err = PNIO_ERR_INTERNAL;
    }

    //return rqb to caller
    pnd_request_start(PND_MBX_ID_PNIO_USER_CORE, (PND_REQUEST_FCT)pnd_pnio_user_core_request_done, rqb_ptr);
}


/*----------------------------------------------------------------------------*/
PNIO_VOID CIODU_EDDS::PI_Free(PND_RQB_PTR_TYPE  rqb_ptr)
{
    PND_PI_FREE_PTR_TYPE                  rqb_args;
    PND_IODU_BUFFER_PTR_TYPE              pIODU_Buffer;
    PNIO_UINT16                            retval            = LSA_OK;
    PNIO_UINT32                            pi_buffer_array_index;

    rqb_args = &(rqb_ptr->args.pi_free);

    if( (rqb_args->consumer_buffer_handle != 0) && (rqb_args->provider_buffer_handle != 0) )
    {

        pIODU_Buffer = (PND_IODU_BUFFER_PTR_TYPE)rqb_args->consumer_buffer_handle;

        /* Delete IODU buffer handle from PI buffer handle array */
        for(pi_buffer_array_index = 0; pi_buffer_array_index < PND_IODU_CFG_MAX_CONSUMER_PI_BUFFERS; pi_buffer_array_index++)
        {
            if(this->consumer_pi_buffer_array[pi_buffer_array_index] == pIODU_Buffer)
            {
                this->consumer_pi_buffer_array[pi_buffer_array_index] = PNIO_NULL;
                // Free the given buffer
                EDDS_IOBuffer_ConsumerFree (this->ioHandle, (PNIO_UINT16)pIODU_Buffer->buffer_handle);

                pIODU_Buffer->buffer_handle = PNIO_NULL;

                //free shadow buffer and buffer handle
                pnd_mem_free(pIODU_Buffer->p_shadow_buffer);
                pnd_mem_free((PNIO_VOID*)pIODU_Buffer);

                pIODU_Buffer = PNIO_NULL;

                break;
            }
        }

        pIODU_Buffer = (PND_IODU_BUFFER_PTR_TYPE)rqb_args->provider_buffer_handle;

        /* Delete IODU buffer handle from PI buffer handle array */
        for(pi_buffer_array_index = 0; pi_buffer_array_index < PND_IODU_CFG_MAX_PROVIDER_PI_BUFFERS; pi_buffer_array_index++)
        {
            if(this->provider_pi_buffer_array[pi_buffer_array_index] == pIODU_Buffer)
            {
                this->provider_pi_buffer_array[pi_buffer_array_index] = PNIO_NULL;
                // Free the given buffer
                EDDS_IOBuffer_ProviderFree (this->ioHandle, (PNIO_UINT16)pIODU_Buffer->buffer_handle);

                pIODU_Buffer->buffer_handle = PNIO_NULL;

                //free shadow buffer and buffer handle
                pnd_mem_free(pIODU_Buffer->p_shadow_buffer);
                pnd_mem_free((PNIO_VOID*)pIODU_Buffer);

                pIODU_Buffer = PNIO_NULL;

                break;
            }
        }
    }//if
    else
    {
        retval = LSA_RSP_ERR_RESOURCE;
        PND_IODU_TRACE_01(this->pnd_handle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,"PI_Free: at least one buffer handle is NULL! rqb(0x%08x)", rqb_ptr);
    }
    
    this->delete_IQ_table_entries(rqb_ptr);

    LSA_RQB_SET_RESPONSE(rqb_ptr, retval);

    if(retval == LSA_OK)
    {
        rqb_args->pnio_err = PNIO_OK;
    }
    else
    {
        rqb_args->pnio_err = PNIO_ERR_INTERNAL;
    }

    pnd_request_start(PND_MBX_ID_PNIO_USER_CORE, (PND_REQUEST_FCT)pnd_pnio_user_core_request_done, rqb_ptr);
}


PNIO_VOID CIODU_EDDS::update_provider_shadow_buffer(PND_IODU_BUFFER_PTR_TYPE pIODU_buffer_handle)
{
    PNIO_UINT8*  pi_buffer_dest;

    PND_ASSERT(pIODU_buffer_handle);

    pi_buffer_dest = EDDS_IOBuffer_ProviderLockNew( this->ioHandle, (PNIO_UINT16)(pIODU_buffer_handle->buffer_handle) );

    pnd_memcpy((PNIO_VOID*)(pi_buffer_dest), (PNIO_VOID*)(pIODU_buffer_handle->p_shadow_buffer), pIODU_buffer_handle->shadow_buffer_size);

    EDDS_IOBuffer_ProviderUnlock( this->ioHandle, (PNIO_UINT16)(pIODU_buffer_handle->buffer_handle) );
}

PNIO_VOID CIODU_EDDS::update_consumer_shadow_buffer(PND_IODU_BUFFER_PTR_TYPE pIODU_buffer_handle)
{
    PNIO_UINT8*  pi_buffer_src;

    PND_ASSERT(pIODU_buffer_handle);

    pi_buffer_src = EDDS_IOBuffer_ConsumerLock( this->ioHandle, (PNIO_UINT16)(pIODU_buffer_handle->buffer_handle) );

    pnd_memcpy((PNIO_VOID*)(pIODU_buffer_handle->p_shadow_buffer), (PNIO_VOID*)(pi_buffer_src), pIODU_buffer_handle->shadow_buffer_size);

    EDDS_IOBuffer_ConsumerUnlock( this->ioHandle, (PNIO_UINT16)(pIODU_buffer_handle->buffer_handle) );
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
