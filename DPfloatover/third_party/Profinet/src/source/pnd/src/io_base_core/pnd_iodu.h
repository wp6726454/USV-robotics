#ifndef PND_IODU_H
#define PND_IODU_H

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
/*  F i l e               &F: pnd_iodu.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Interface for IODU (IO Data Update)                                      */
/*****************************************************************************/

#define PND_IODU_IQ_MAX_ADDRESS                         0x8000
#define PND_IODU_CFG_MAX_CONSUMER_PI_BUFFERS            128
#define PND_IODU_CFG_MAX_PROVIDER_PI_BUFFERS            128

#define PND_IODU_IOXS_STATE_GOOD                        0x80
#define PND_IODU_IOXS_STATE_BAD                         0x00

/*
 * Access types
 */
enum PND_ACC_T 
{
	PND_ACC_T_READ,
	PND_ACC_T_WRITE,
};

typedef struct
{
    PNIO_UINT16            buffer_handle;
    PNIO_UINT8*            p_shadow_buffer;        //virt. adr. of shadow buffers for this CR
    PNIO_UINT32            shadow_buffer_size;     //size of CR
    PNIO_BOOL              is_provider;            
} PND_IODU_BUFFER_TYPE;

typedef PND_IODU_BUFFER_TYPE * PND_IODU_BUFFER_PTR_TYPE;

typedef struct {  
    PND_IODU_BUFFER_PTR_TYPE    provider_buffer_handle;
    PND_IODU_BUFFER_PTR_TYPE    consumer_buffer_handle;
    PNIO_UINT32                  IO_base_addr;        //don't care if length_IO_data 0
    PNIO_UINT32                  offset_IO_data;      //offset of IO data in frame buffer
    PNIO_UINT32                  length_IO_data;      //0 indicates a dataless submodule (was ist bei unterdrückten IO daten?)
    PNIO_UINT32                  offset_IOCS;         //offset of IOCS data in output frame buffer (controller view)
    PNIO_UINT32                  device_nr;           //device nr this IO address range belongs to
    PNIO_BOOL                    is_online;           //indicates if corresponding device is "INDATA"

} PND_IO_ADDR_MAPPING_TYPE;
typedef PND_IO_ADDR_MAPPING_TYPE * PND_IO_ADDR_MAPPING_PTR_TYPE;




class CIODU
{
    // Attributes
private:

protected:

    PND_HANDLE_TYPE pnd_handle;

    PND_IO_ADDR_MAPPING_PTR_TYPE I_addr_table[PND_IODU_IQ_MAX_ADDRESS];
    PND_IO_ADDR_MAPPING_PTR_TYPE Q_addr_table[PND_IODU_IQ_MAX_ADDRESS];

    PND_IODU_BUFFER_PTR_TYPE    consumer_pi_buffer_array[PND_IODU_CFG_MAX_CONSUMER_PI_BUFFERS];
    PND_IODU_BUFFER_PTR_TYPE    provider_pi_buffer_array[PND_IODU_CFG_MAX_PROVIDER_PI_BUFFERS];

protected:
    CIODU(PND_HANDLE_TYPE pndHandle);

    PNIO_UINT32 alignSize(PNIO_UINT32 size, PNIO_UINT32 alignment);
    LSA_RESPONSE_TYPE create_IQ_table(PND_RQB_PTR_TYPE  rqb_ptr);
    LSA_RESPONSE_TYPE check_params_IO_addr(PND_RQB_PTR_TYPE rqb_ptr);
    PNIO_VOID delete_IQ_table_entries(PND_RQB_PTR_TYPE  rqb_ptr);

    virtual PNIO_VOID update_provider_shadow_buffer(PND_IODU_BUFFER_PTR_TYPE pIODU_buffer_handle) = 0;
    virtual PNIO_VOID update_consumer_shadow_buffer(PND_IODU_BUFFER_PTR_TYPE pIODU_buffer_handle) = 0;

public:
    virtual ~CIODU();

    // pure virtual methodes
    virtual PNIO_VOID PI_Alloc(PND_RQB_PTR_TYPE  rqb_ptr) = 0;
    virtual PNIO_VOID PI_Free(PND_RQB_PTR_TYPE  rqb_ptr) = 0;

    PNIO_UINT32 data_io_rw(PNIO_ADDR * pAddr, PND_ACC_T accesst, PNIO_IOXS * pLocStat, PNIO_IOXS * pRemStat, PNIO_UINT32 * pDataLen, PNIO_UINT8 * pData);
    PNIO_VOID update_IQ_table(PNIO_UINT32 device_nr, PNIO_BOOL is_online);

};

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif  /* of PND_IODU_H */
