#ifndef PSI_PLS_H                   /* ----- reinclude-protection ----- */
#define PSI_PLS_H

#ifdef __cplusplus                  /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: psi_pls.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Check the configuration                                                  */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                              compiler errors                              */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* General                                                                   */
/*---------------------------------------------------------------------------*/
#if ((PSI_CFG_MAX_PNIO_IF_CNT < 1) || (PSI_CFG_MAX_PNIO_IF_CNT > 4))
    #error "PSI_CFG_MAX_PNIO_IF_CNT is out of valid range [1..4]"
#endif

#if defined (PSI_CFG_USE_IOD_MC)
    #if (PSI_CFG_USE_IOD_MC != 0)
        #error "don't activate, feature IOD MC not released"
    #endif
#endif

/*---------------------------------------------------------------------------*/
/* SNMPX                                                                     */
/*---------------------------------------------------------------------------*/
#if ( PSI_CFG_USE_SNMPX == 0 ) && defined (PSI_CFG_USE_SNMPX_MGR) && ( PSI_CFG_USE_SNMPX_MGR == 1 )
    #error "PSI_CFG_USE_SNMPX_MGR is only allowed in combination with PSI_CFG_USE_SNMPX"
#endif

#if ( PSI_CFG_USE_SNMPX == 0 ) && defined (PSI_CFG_USE_SNMPX_AGT) && ( PSI_CFG_USE_SNMPX_AGT == 1 )
    #error "PSI_CFG_USE_SNMPX_AGT is only allowed in combination with PSI_CFG_USE_SNMPX"
#endif

#if defined (PSI_CFG_USE_SNMPX_AGT)
    #if (PSI_CFG_USE_SNMPX_AGT != 0)
        #error "don't activate, feature SNMPX AGENT not released"
    #endif
#endif

/*---------------------------------------------------------------------------*/
/* TRACE                                                                     */
/*---------------------------------------------------------------------------*/
#if !defined (PSI_CFG_TRACE_MODE)
    #error "config error, PSI_CFG_TRACE_MODE not defined"
#endif

#if ( (!defined (PSI_CFG_MAX_SOCK_APP_CHANNELS)) || (PSI_CFG_MAX_SOCK_APP_CHANNELS < 1) || (PSI_CFG_MAX_SOCK_APP_CHANNELS > 8) )
    #error "PSI_CFG_MAX_SOCK_APP_CHANNELS not defined or out of range (1..8)"
#endif

/*---------------------------------------------------------------------------*/
/* TCIP                                                                      */
/*---------------------------------------------------------------------------*/
#ifndef PSI_TCIP_CFG_COPY_ON_SEND
#error " PSI_TCIP_CFG_COPY_ON_SEND not defined"
#endif

#if defined (PSI_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT) && (PSI_TCIP_CFG_COPY_ON_SEND != 1)
    #error "PSI_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT is only allowed in combination with PSI_TCIP_CFG_COPY_ON_SEND"
#endif

#if ( (EDD_CFG_MAX_INTERFACE_CNT > 1) && (!defined (PSI_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT)) )
    #error "EDD_CFG_MAX_INTERFACE_CNT > 1 but PSI_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT not defined"
#endif

#if ( (EDD_CFG_MAX_INTERFACE_CNT > 1) && (PSI_TCIP_CFG_COPY_ON_SEND != 1) )
    #error "EDD_CFG_MAX_INTERFACE_CNT > 1 and PSI_TCIP_CFG_COPY_ON_SEND != 1"
#endif

/*---------------------------------------------------------------------------*/
/* EDDx                                                                      */
/*---------------------------------------------------------------------------*/
#if (PSI_CFG_USE_EDDI == 0) && (PSI_CFG_USE_EDDP == 0) && (PSI_CFG_USE_EDDS == 0)
    #error "neither PSI_CFG_USE_EDDI nor PSI_CFG_USE_EDDP nor PSI_CFG_USE_EDDS is defined"
#endif

/*---------------------------------------------------------------------------*/
/* EDDS                                                                      */
/*---------------------------------------------------------------------------*/
#if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE) && defined (EDDS_CFG_HW_PACKET32)
    #error "PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE is not allowed in combination with EDDS_CFG_HW_PACKET32"
#endif

/*---------------------------------------------------------------------------*/
/* HIF                                                                       */
/*---------------------------------------------------------------------------*/
#if ( PSI_CFG_USE_HIF == 0 ) && defined (PSI_CFG_USE_HIF_HD) && ( PSI_CFG_USE_HIF_HD == 1 )
    #error "PSI_CFG_USE_HIF_HD is only allowed in combination with PSI_CFG_USE_HIF"
#endif

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of PSI_PLS_H */