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
/*  F i l e               &F: pnd_iodu.cpp                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of parent class IODU                                      */
/*                                                                           */
/*****************************************************************************/


#define LTRC_ACT_MODUL_ID   1201
#define PND_MODULE_ID       1201

/////////////////////////////////////////////////////////////////////////////

#include "pnd_int.h"
#include "pnd_sys.h"
#include "pnd_trc.h"
#include "pnioerrx.h"
#include "pniousrx.h"

#include "pnd_iodu.h"


/* BTRACE-IF */
PND_FILE_SYSTEM_EXTENSION(PND_MODULE_ID) /* no semicolon */


    /////////////////////////////////////////////////////////////////////////////
    // CIODU class
    /////////////////////////////////////////////////////////////////////////////

CIODU::CIODU(PND_HANDLE_TYPE pndHandle)
{
    int i;

    this->pnd_handle = pndHandle;



    for(i=0; i<PND_IODU_CFG_MAX_CONSUMER_PI_BUFFERS; i++)
    {
      consumer_pi_buffer_array[i] = PNIO_NULL;
    }

    for(i=0; i<PND_IODU_CFG_MAX_PROVIDER_PI_BUFFERS; i++)
    {
      provider_pi_buffer_array[i] = PNIO_NULL;
    }

    for(i=0; i<PND_IODU_IQ_MAX_ADDRESS; i++)
    {
        this->I_addr_table[i] = PNIO_NULL;
        this->Q_addr_table[i] = PNIO_NULL;
    }
}

CIODU::~CIODU()
{

}

/*----------------------------------------------------------------------------*/
PNIO_UINT32 CIODU::alignSize( PNIO_UINT32 size, PNIO_UINT32 alignment )
{
    PNIO_UINT32 result = size;

    if (( alignment != 0 ) && (alignment != 1))
    {
        result = size/alignment;
        result += ((size % alignment) != 0) ? 1 : 0;
        result *= alignment;
    }

    return ( result );
}


/*----------------------------------------------------------------------------*/
PNIO_UINT16 CIODU::create_IQ_table(PND_RQB_PTR_TYPE  rqb_ptr)
{
    PND_BUFFER_PROPERTIES_PTR_TYPE        rqb_args_consumer;
    PND_BUFFER_PROPERTIES_PTR_TYPE        rqb_args_provider;
    PNIO_UINT16                            retval                  = LSA_OK;
    PND_IO_ADDR_MAPPING_PTR_TYPE          pAddr_mapping;
    PND_IO_ADDR_PTR_TYPE                  pIO_addr;

    PND_IODU_BUFFER_PTR_TYPE              pIODU_buffer_consumer;
    PND_IODU_BUFFER_PTR_TYPE              pIODU_buffer_provider;
    PNIO_UINT32                            k,j;

    rqb_args_consumer = &(rqb_ptr->args.pi_alloc.consumer_buffer);
    rqb_args_provider = &(rqb_ptr->args.pi_alloc.provider_buffer);

    pIODU_buffer_consumer = (PND_IODU_BUFFER_PTR_TYPE)rqb_args_consumer->buffer_handle;
    pIODU_buffer_provider = (PND_IODU_BUFFER_PTR_TYPE)rqb_args_provider->buffer_handle;

    //process consumer I address mapping
    for(k=0; k<rqb_args_consumer->number_of_IO_addr; k++)
    {
        pIO_addr = &rqb_args_consumer->IO_addr_array[k];
        if(pIO_addr->length_IO_data == 0)
        {
            //in case of a dataless submodule we set the IOCS status permanently to GOOD
            pIODU_buffer_provider->p_shadow_buffer[pIO_addr->offset_IOCS] = PND_IODU_IOXS_STATE_GOOD;
        }
        else if((pIO_addr->offset_IO_data + pIO_addr->length_IO_data) >= rqb_args_consumer->partial_length)
        {
            retval = LSA_RSP_ERR_PARAM;
            PND_IODU_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"create_IQ_table: offset_IO_data + length_IO_data > partial_length (consumer)! rqb(0x%08x)", rqb_ptr);
            break;
        }
        else
        {
            pAddr_mapping = (PND_IO_ADDR_MAPPING_PTR_TYPE)pnd_mem_alloc(sizeof(PND_IO_ADDR_MAPPING_TYPE));

            pAddr_mapping->consumer_buffer_handle   = pIODU_buffer_consumer;
            pAddr_mapping->provider_buffer_handle   = pIODU_buffer_provider;
            pAddr_mapping->IO_base_addr             = pIO_addr->IO_base_addr;
            pAddr_mapping->length_IO_data           = pIO_addr->length_IO_data;
            pAddr_mapping->offset_IOCS              = pIO_addr->offset_IOCS;
            pAddr_mapping->offset_IO_data           = pIO_addr->offset_IO_data;
            pAddr_mapping->device_nr                = rqb_ptr->args.pi_alloc.device_nr;
            pAddr_mapping->is_online                = PNIO_FALSE;

            //link all associated I/Q adresses to the same address mapping item
            for(j=0; j<pAddr_mapping->length_IO_data; j++)
            {
                if( this->I_addr_table[pAddr_mapping->IO_base_addr + j] == PNIO_NULL)
                {
                    this->I_addr_table[pAddr_mapping->IO_base_addr + j] = pAddr_mapping;
                }
                else
                {
                    retval = LSA_RSP_ERR_PARAM;
                    PND_IODU_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"create_IQ_table: there is already an input address stored on this location! rqb(0x%08x)", rqb_ptr);
                }
            }


        }
    }

    //process Q address mapping
    for(k=0; k<rqb_args_provider->number_of_IO_addr; k++)
    {
        pIO_addr = &rqb_args_provider->IO_addr_array[k];
        if(pIO_addr->length_IO_data == 0)
        {
            //in case of a dataless submodule we set the IOPS status permanently to GOOD
            pIODU_buffer_provider->p_shadow_buffer[pIO_addr->offset_IO_data] = PND_IODU_IOXS_STATE_GOOD;
        }
        else if((pIO_addr->offset_IO_data + pIO_addr->length_IO_data) >= rqb_args_provider->partial_length)
        {
            retval = LSA_RSP_ERR_PARAM;
            PND_IODU_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"create_IQ_table: offset_IO_data + length_IO_data > partial_length (provider)! rqb(0x%08x)", rqb_ptr);
        }
        else
        {
            pAddr_mapping = (PND_IO_ADDR_MAPPING_PTR_TYPE)pnd_mem_alloc(sizeof(PND_IO_ADDR_MAPPING_TYPE));

            pAddr_mapping->consumer_buffer_handle   = pIODU_buffer_consumer;
            pAddr_mapping->provider_buffer_handle   = pIODU_buffer_provider;
            pAddr_mapping->IO_base_addr             = pIO_addr->IO_base_addr;
            pAddr_mapping->length_IO_data           = pIO_addr->length_IO_data;
            pAddr_mapping->offset_IOCS              = pIO_addr->offset_IOCS;
            pAddr_mapping->offset_IO_data           = pIO_addr->offset_IO_data;
            pAddr_mapping->device_nr                = rqb_ptr->args.pi_alloc.device_nr;
            pAddr_mapping->is_online                = PNIO_FALSE;

            //link all associated I/Q adresses to the same address mapping item
            for(j=0; j<pAddr_mapping->length_IO_data; j++)
            {
                if( this->Q_addr_table[pAddr_mapping->IO_base_addr + j] == PNIO_NULL)
                {
                    this->Q_addr_table[pAddr_mapping->IO_base_addr + j] = pAddr_mapping;
                }
                else
                {
                    retval = LSA_RSP_ERR_PARAM;
                    PND_IODU_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"create_IQ_table: there is already an output address stored on this location! rqb(0x%08x)", rqb_ptr);
                }
            }
        }
    }

    // write all pre-set IOXS to actual hardware buffer
    this->update_provider_shadow_buffer( pIODU_buffer_provider );

    return retval;
}



LSA_RESPONSE_TYPE CIODU::check_params_IO_addr(PND_RQB_PTR_TYPE rqb_ptr)
{
    LSA_RESPONSE_TYPE           retval      = LSA_OK;
    PND_PI_ALLOC_PTR_TYPE       rqb_args    = &(rqb_ptr->args.pi_alloc);
    PNIO_UINT32                  i,k;

    if((rqb_args->consumer_buffer.number_of_IO_addr != 0))
    {
        PND_ASSERT( rqb_args->consumer_buffer.IO_addr_array != PNIO_NULL );
    }
    else
    {
        PND_ASSERT( rqb_args->consumer_buffer.IO_addr_array == PNIO_NULL );
    }

    if(rqb_args->provider_buffer.number_of_IO_addr != 0) 
    {
        PND_ASSERT(rqb_args->provider_buffer.IO_addr_array != PNIO_NULL );
    }
    else
    {
        PND_ASSERT(rqb_args->provider_buffer.IO_addr_array == PNIO_NULL );
    }


    //check consumer buffer
    for(i=0; i<rqb_args->consumer_buffer.number_of_IO_addr; i++)
    {
        //check if IO_base_addr is within valid range
        if((rqb_args->consumer_buffer.IO_addr_array[i].length_IO_data != 0/*submodule with no IO data and just IOPS/IOCS*/) &&
           (rqb_args->consumer_buffer.IO_addr_array[i].IO_base_addr + rqb_args->consumer_buffer.IO_addr_array[i].length_IO_data) > PND_IODU_IQ_MAX_ADDRESS)
        {
            PND_IODU_TRACE_01(this->pnd_handle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,"PI_Alloc: IO_base_addr is not within valid range (consumer)! rqb(0x%08x)", rqb_ptr);
            retval = LSA_RSP_ERR_PARAM;
        }

        //check if offset_IOCS is within valid range in provider buffer
        if(rqb_args->consumer_buffer.IO_addr_array[i].offset_IOCS >= rqb_args->provider_buffer.partial_length)
        {
            PND_IODU_TRACE_01(this->pnd_handle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,"PI_Alloc: offset_IOCS is not within valid range of provider buffer! rqb(0x%08x)", rqb_ptr);
            retval = LSA_RSP_ERR_PARAM;
        }

        for(k=i+1; k<rqb_args->consumer_buffer.number_of_IO_addr; k++)
        {//check if every offset_IOCS in this array is unique
            if(rqb_args->consumer_buffer.IO_addr_array[i].offset_IOCS == rqb_args->consumer_buffer.IO_addr_array[k].offset_IOCS)
            {
                PND_IODU_TRACE_01(this->pnd_handle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,"PI_Alloc: offset_IOCS in consumer buffer is not unique! rqb(0x%08x)", rqb_ptr);
                retval = LSA_RSP_ERR_PARAM;
            }
        }
    }

    //check provider buffer
    for(i=0; i<rqb_args->provider_buffer.number_of_IO_addr; i++)
    {
        //check if IO_base_addr is within valid range
        if(/*(rqb_args->provider_buffer.IO_addr_array[i].length_IO_data != 0*//*submodule with no IO data and just IOPS/IOCS*//*) && Note: There is no output submodule with no IO data*/
           (rqb_args->provider_buffer.IO_addr_array[i].IO_base_addr + rqb_args->provider_buffer.IO_addr_array[i].length_IO_data) > PND_IODU_IQ_MAX_ADDRESS)
        {
            PND_IODU_TRACE_01(this->pnd_handle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,"PI_Alloc: IO_base_addr is not within valid range (provider)! rqb(0x%08x)", rqb_ptr);
            retval = LSA_RSP_ERR_PARAM;
        }

        //check if offset_IOCS is within valid range in provider buffer
        if(rqb_args->provider_buffer.IO_addr_array[i].offset_IOCS >= rqb_args->consumer_buffer.partial_length)
        {
            PND_IODU_TRACE_01(this->pnd_handle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,"PI_Alloc: offset_IOCS is not within valid range of consumer buffer! rqb(0x%08x)", rqb_ptr);
            retval = LSA_RSP_ERR_PARAM;
        }

        for(k=i+1; k<rqb_args->provider_buffer.number_of_IO_addr; k++)
        {//check if every offset_IOCS in this array is unique
            if(rqb_args->provider_buffer.IO_addr_array[i].offset_IOCS == rqb_args->provider_buffer.IO_addr_array[k].offset_IOCS)
            {
                PND_IODU_TRACE_01(this->pnd_handle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,"PI_Alloc: offset_IOCS in provider buffer is not unique! rqb(0x%08x)", rqb_ptr);
                retval = LSA_RSP_ERR_PARAM;
            }
        }
    }


    return retval;
}


PNIO_VOID CIODU::delete_IQ_table_entries(PND_RQB_PTR_TYPE  rqb_ptr)
{
    PNIO_UINT32  i;

    PNIO_VOID_PTR_TYPE provider_buffer_handle = rqb_ptr->args.pi_free.provider_buffer_handle;
    PNIO_VOID_PTR_TYPE consumer_buffer_handle = rqb_ptr->args.pi_free.consumer_buffer_handle;

    // delete all associated IQ entries for the given consumer buffer
    for(i=0; i < PND_IODU_IQ_MAX_ADDRESS; i++)
    {
        //handle input address table
        if(this->I_addr_table[i] != PNIO_NULL)
        {
            //check every entry for buffer_handle match
            if(this->I_addr_table[i]->consumer_buffer_handle == consumer_buffer_handle)
            {
                //check if this is the last entry for this PND_IO_ADDR_MAPPING_TYPE
                if(i == (this->I_addr_table[i]->IO_base_addr + this->I_addr_table[i]->length_IO_data - 1))
                {
                    //last entry: free mem and delete reference
                    pnd_mem_free((PNIO_VOID*)(this->I_addr_table[i]));

                    this->I_addr_table[i] = PNIO_NULL;
                }
                else
                {
                    //just delete reference
                    this->I_addr_table[i] = PNIO_NULL;
                }
            }
        }

        //handle output address table
        if(this->Q_addr_table[i] != PNIO_NULL)
        {
            //check every entry for buffer_handle match
            if(this->Q_addr_table[i]->provider_buffer_handle == provider_buffer_handle)
            {
                //check if this is the last entry for this PND_IO_ADDR_MAPPING_TYPE
                if(i == (this->Q_addr_table[i]->IO_base_addr + this->Q_addr_table[i]->length_IO_data - 1))
                {
                    //last entry: free mem and delete reference
                    pnd_mem_free((PNIO_VOID*)(this->Q_addr_table[i]));

                    this->Q_addr_table[i] = PNIO_NULL;
                }
                else
                {
                    //just delete reference
                    this->Q_addr_table[i] = PNIO_NULL;
                }
            }
        }
    }
}



PNIO_UINT32 CIODU::data_io_rw(PNIO_ADDR * pAddr,
    PND_ACC_T accesst, PNIO_IOXS * pLocStat, PNIO_IOXS * pRemStat,
    PNIO_UINT32 * pDataLen, PNIO_UINT8 * pData)
{
    PND_IO_ADDR_MAPPING_PTR_TYPE    pMapping_entry;
    PNIO_UINT32                     offset_submodule;
    PNIO_UINT32                     Ret = PNIO_OK;

    if(!pData && (*pDataLen))
    {
        return PNIO_ERR_PRM_BUF;
    }

    if(!pRemStat)
    {
        return PNIO_ERR_PRM_RSTATE;
    }

    if(!pAddr ||
        pAddr->AddrType != PNIO_ADDR_LOG ||
        (pAddr->IODataType != PNIO_IO_IN && pAddr->IODataType != PNIO_IO_OUT))
    {
        return PNIO_ERR_PRM_ADD;
    }

    // sanity address bound
    if(pAddr->u.Addr > (PND_IODU_IQ_MAX_ADDRESS - 1))
    {
        return PNIO_ERR_PRM_ADD;
    }

    if(accesst == PND_ACC_T_WRITE) 
    {
        if(pAddr->IODataType != PNIO_IO_OUT) 
        {
            // user can write io-data only from type PNIO_IO_OUT
            return PNIO_ERR_PRM_IO_TYPE;
        }
    } else if(accesst == PND_ACC_T_READ) 
    {
        if(pAddr->IODataType != PNIO_IO_IN) 
        {
            // user can read io-data only from type PNIO_IO_IN
            return PNIO_ERR_PRM_IO_TYPE;
        }
    } else 
    {
        return PNIO_ERR_PRM_IO_TYPE;
    }

    if(*pDataLen > PNIO_MAX_IO_LEN)
    {
        return PNIO_ERR_VALUE_LEN;
    }
    if(*pLocStat != PNIO_S_GOOD && *pLocStat != PNIO_S_BAD)
    {
        return PNIO_ERR_PRM_LOC_STATE;
    }


    if(accesst == PND_ACC_T_READ)
    {

        pMapping_entry = this->I_addr_table[pAddr->u.Addr];

        if(pMapping_entry == PNIO_NULL)
        {
            return PNIO_ERR_UNKNOWN_ADDR;
        }

        if(pMapping_entry->is_online != PNIO_TRUE)
        {
            *pRemStat = PNIO_S_BAD;

            return PNIO_ERR_NO_CONNECTION;
        }

        //update consumer shadow buffer
        this->update_consumer_shadow_buffer(pMapping_entry->consumer_buffer_handle);

        //calculate offset within the submodule
        offset_submodule = pAddr->u.Addr - pMapping_entry->IO_base_addr;
        PND_ASSERT((offset_submodule >= 0) && (offset_submodule < pMapping_entry->length_IO_data));

        if(*pDataLen > (pMapping_entry->length_IO_data - offset_submodule))
        {
            return PNIO_ERR_PRM_LEN;
        }

        //copy input data from shadow buffer to user buffer
        pnd_memcpy(pData, pMapping_entry->consumer_buffer_handle->p_shadow_buffer + pMapping_entry->offset_IO_data + offset_submodule, *pDataLen);

        //set local state (IOCS) at provider buffer
        *(pMapping_entry->provider_buffer_handle->p_shadow_buffer + pMapping_entry->offset_IOCS) = ((*pLocStat == PNIO_S_GOOD) ? PND_IODU_IOXS_STATE_GOOD : PND_IODU_IOXS_STATE_BAD);

        //read remote state (IOPS) from consumer buffer
        *pRemStat = ((*(pMapping_entry->consumer_buffer_handle->p_shadow_buffer + pMapping_entry->offset_IO_data + pMapping_entry->length_IO_data) == PND_IODU_IOXS_STATE_GOOD) ? PNIO_S_GOOD : PNIO_S_BAD);

        //update provider shadow buffer for possible local IOCS change
        this->update_provider_shadow_buffer(pMapping_entry->provider_buffer_handle);

    }
    else if (accesst == PND_ACC_T_WRITE)
    {
        pMapping_entry = this->Q_addr_table[pAddr->u.Addr];

        if(pMapping_entry == PNIO_NULL)
        {
            return PNIO_ERR_UNKNOWN_ADDR;
        }

        if(pMapping_entry->is_online != PNIO_TRUE)
        {
            *pRemStat = PNIO_S_BAD;

            return PNIO_ERR_NO_CONNECTION;
        }

        //update consumer buffer for possible remote IOCS change
        this->update_consumer_shadow_buffer(pMapping_entry->consumer_buffer_handle);

        //calculate offset within the submodule
        offset_submodule = pAddr->u.Addr - pMapping_entry->IO_base_addr;
        PND_ASSERT((offset_submodule >= 0) && (offset_submodule < pMapping_entry->length_IO_data));

        if(*pDataLen > (pMapping_entry->length_IO_data - offset_submodule))
        {
            return PNIO_ERR_PRM_LEN;
        }

        //copy output data from user buffer to shadow buffer
        pnd_memcpy(pMapping_entry->provider_buffer_handle->p_shadow_buffer + pMapping_entry->offset_IO_data + offset_submodule, pData, *pDataLen);

        //set local state (IOPS) at provider buffer
        *(pMapping_entry->provider_buffer_handle->p_shadow_buffer + pMapping_entry->offset_IO_data + pMapping_entry->length_IO_data) = ((*pLocStat == PNIO_S_GOOD) ? PND_IODU_IOXS_STATE_GOOD : PND_IODU_IOXS_STATE_BAD);

        //read remote state (IOCS) from consumer buffer
        *pRemStat = ((*(pMapping_entry->consumer_buffer_handle->p_shadow_buffer + pMapping_entry->offset_IOCS) == PND_IODU_IOXS_STATE_GOOD) ? PNIO_S_GOOD : PNIO_S_BAD);

        //update provider buffer for possible local IOCS change
        this->update_provider_shadow_buffer(pMapping_entry->provider_buffer_handle);
    }
    else
    {
        return PNIO_ERR_INTERNAL;
    }

    return Ret;
}


PNIO_VOID CIODU::update_IQ_table(PNIO_UINT32 device_nr, PNIO_BOOL is_online)
{
    PNIO_UINT32  i;

    for(i=0; i < PND_IODU_IQ_MAX_ADDRESS; i++)
    {
        if(this->Q_addr_table[i] != PNIO_NULL)
        {
            if(this->Q_addr_table[i]->device_nr == device_nr)
            {
                this->Q_addr_table[i]->is_online = is_online;
            }
        }

        if(this->I_addr_table[i] != PNIO_NULL)
        {
            if(this->I_addr_table[i]->device_nr == device_nr)
            {
                this->I_addr_table[i]->is_online = is_online;
            }
        }
    }//for
}
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
