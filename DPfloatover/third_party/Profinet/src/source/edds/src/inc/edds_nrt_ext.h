
#ifndef EDDS_NRT_EXT_H                        /* ----- reinclude-protection ----- */
#define EDDS_NRT_EXT_H

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
/*  F i l e               &F: edds_nrt_ext.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  NRT- Externals                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  12.04.02    JS    initial version. LSA P02.00.01.003                     */
/*  23.05.02    JS    ported to LSA_GLOB_P02.00.00.00_00.01.04.01            */
/*  23.10.03    JS    Parameter changed                                      */
/*  12.02.04    JS    added support for multiple send priorities (3)         */
/*  28.06.04    JS    changes to support xrt over udp                        */
/*  06.07.05    JS    added multiport support                                */
#endif
/*****************************************************************************/


/*---------------------------------------------------------------------------*/
/* External functions                                                        */
/*---------------------------------------------------------------------------*/

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_NRTRequest(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTSend(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTRecv(
    EDD_UPPER_RQB_PTR_TYPE      pRQB,
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTTxReady(
    EDDS_LOCAL_DDB_PTR_TYPE     pDDB,
    LSA_UINT32                  Prio,
    LSA_RESULT                  Status);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTTxQuery(
    EDDS_LOCAL_DDB_PTR_TYPE                           pDDB,
    LSA_UINT32                                        Prio,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR       *pBuffer,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                       pLength,
    EDDS_LOCAL_MEM_U32_PTR_TYPE                       pPortID,
    LSA_UINT32                                        MaxLength);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTRxIndicate(
    EDDS_LOCAL_DDB_PTR_TYPE                           pDDB,
    EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR       *pBuffer,
    LSA_UINT32                                        pLength,
    LSA_UINT32                                        queueIdx,
    LSA_UINT32                                        PortID,
    LSA_UINT32                                        State);

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_NRTRxResetDscrLookUpCounter(
    EDDS_LOCAL_DDB_COMP_NRT_PTR_TYPE pNRTComp);

LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_NRTRxRequestInterrupt(
    EDDS_LOCAL_DDB_PTR_TYPE    pDDB);


LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTCloseChannel(
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

LSA_EXTERN LSA_RESULT EDDS_LOCAL_FCT_ATTR EDDS_NRTOpenChannel(
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB,
    EDDS_UPPER_CDB_NRT_PTR_TYPE pCDB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_NRTInitComponent(EDD_UPPER_RQB_PTR_TYPE pRQB);
LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_NRTRelComponent(EDD_UPPER_RQB_PTR_TYPE pRQB);

LSA_EXTERN LSA_VOID EDDS_LOCAL_FCT_ATTR EDDS_NRTCancelPart2(
    EDDS_LOCAL_HDB_PTR_TYPE     pHDB);

/*****************************************************************************/
/*  end of file nrt_ext.h                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of NRT_EXT_H */
