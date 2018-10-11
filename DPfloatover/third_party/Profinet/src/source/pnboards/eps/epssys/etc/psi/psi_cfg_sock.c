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
/*  C o m p o n e n t     &C: EPS (Embedded Profinet System)            :C&  */
/*                                                                           */
/*  F i l e               &F: psi_cfg_sock                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of SOCK adaption without TCIP     .        */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID     32007
#define LTRC_ACT_MODUL_ID 32007

#include "eps_sys.h"
#include "psi_inc.h"

#if (( PSI_CFG_USE_SOCK == 1 ) && ( PSI_CFG_USE_TCIP == 0 ))
/*----------------------------------------------------------------------------*/

#include "sock_inc.h"  // sock_mib_xx

PSI_FILE_SYSTEM_EXTENSION( PSI_MODULE_ID )

/*----------------------------------------------------------------------------*/
/* dummy for snmp agent parsing */
LSA_INT snmp_agt_parse( 
	LSA_UINT8* data,          /* IN pointer to packet data (past UDP header) */
	LSA_UINT   length,        /* IN/SCRTACH length of packet data */
	LSA_UINT8* out_data,      /* IN/OUT buffer for response packet */
	LSA_UINT   out_length)    /* IN/SCRTACH size of output buffer */
{
	LSA_UNUSED_ARG( data );
	LSA_UNUSED_ARG( length );
	LSA_UNUSED_ARG( out_data );
	LSA_UNUSED_ARG( out_length );

	return ( 0 );  /* not implemented */
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#ifndef PSI_SOCK_SET_IPSUITE
LSA_BOOL PSI_SOCK_SET_IPSUITE(
	LSA_UINT32  IfID,
	LSA_UINT32  ip_addr,
	LSA_UINT32  net_mask,
	LSA_UINT32  gateway )
{
	LSA_UNUSED_ARG( IfID );
	LSA_UNUSED_ARG( ip_addr );
	LSA_UNUSED_ARG( net_mask );
	LSA_UNUSED_ARG( gateway );

	return ( LSA_FALSE ); /* not implemented */
}
#else
# error "by design a function"
#endif

/*===========================================================================*/
/*===========================================================================*/

#ifndef PSI_SOCK_SET_MIB2_IFGROUP_DATA
LSA_BOOL PSI_SOCK_SET_MIB2_IFGROUP_DATA(
	LSA_UINT32          IfID,
	LSA_UINT16          PortId, 
	PSI_SOCK_NAME_TYPE  IfDescr )
{
	LSA_UNUSED_ARG( IfID );
	LSA_UNUSED_ARG( PortId );
	LSA_UNUSED_ARG( IfDescr );

	return ( LSA_TRUE ); /* not implemented, but must set OK */
}
#else
# error "by design a function"
#endif

/*===========================================================================*/

#ifndef PSI_SOCK_SET_MIB2_SYSGROUP_DATA
LSA_BOOL PSI_SOCK_SET_MIB2_SYSGROUP_DATA( 
	PSI_SOCK_NAME_TYPE SysName,
	PSI_SOCK_NAME_TYPE SysDescr,
	PSI_SOCK_NAME_TYPE SysContact,
	PSI_SOCK_NAME_TYPE SysLocation )
{
	LSA_UNUSED_ARG( SysName );
	LSA_UNUSED_ARG( SysDescr );
	LSA_UNUSED_ARG( SysContact );
	LSA_UNUSED_ARG( SysLocation );

	return ( LSA_TRUE ); /* not implemented, but must set OK */
}
#else
# error "by design a function"
#endif

/*===========================================================================*/

#ifndef PSI_SOCK_GET_MIB2_IFINDEX
LSA_BOOL PSI_SOCK_GET_MIB2_IFINDEX(
	LSA_UINT32    IfID, 
	LSA_UINT16    PortID, 
	LSA_UINT32*   p_IfIndex )
{
	LSA_UNUSED_ARG( IfID );
	LSA_UNUSED_ARG( PortID );
	LSA_UNUSED_ARG( p_IfIndex );

	return ( LSA_FALSE ); /* not implemented */
}
#else
# error "by design a function"
#endif


/*===========================================================================*/
/*===========================================================================*/

#ifndef PSI_SOCK_TCPIP_SET_TIMEPARAMS
LSA_BOOL PSI_SOCK_TCPIP_SET_TIMEPARAMS(
	LSA_UINT32  KeepAliveTime,
	LSA_UINT32  ConnectionTimeout,
	LSA_UINT32  RetransTimeout,
	LSA_UINT32  RetransCount )
{
	LSA_UNUSED_ARG(KeepAliveTime);
	LSA_UNUSED_ARG(ConnectionTimeout);
	LSA_UNUSED_ARG(RetransTimeout);
	LSA_UNUSED_ARG(RetransCount);

	return( LSA_FALSE ); /* not implemented */
}
#else
#error "by design a function"
#endif

/*===========================================================================*/

#ifndef PSI_SOCK_TCPIP_GET_TIMEPARAMS
LSA_BOOL PSI_SOCK_TCPIP_GET_TIMEPARAMS(
	LSA_UINT32  *KeepAliveTime,
	LSA_UINT32  *ConnectionTimeout,
	LSA_UINT32  *RetransTimeout,
	LSA_UINT32  *RetransCount )
{
	LSA_UNUSED_ARG(KeepAliveTime);
	LSA_UNUSED_ARG(ConnectionTimeout);
	LSA_UNUSED_ARG(RetransTimeout);
	LSA_UNUSED_ARG(RetransCount);

	return( LSA_FALSE ); /* not implemented */
}
#else
#error "by design a function"
#endif

/*----------------------------------------------------------------------------*/

#endif /* PSI_CFG_USE_SOCK && PSI_CFG_USE_TCIP */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
