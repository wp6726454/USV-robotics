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
/*  C o m p o n e n t     &C: SOCK (SOCKet interface)                   :C&  */
/*                                                                           */
/*  F i l e               &F: sock_mib2.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  MIB2 related functions                                                   */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID 13
#define SOCK_MODULE_ID    13

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

static LSA_BOOL   SOCK_LOCAL_FCT_ATTR sock_mib2_cmp_system_params(SOCK_MIB2_SYSGROUP_DATA_PTR_TYPE p_mib2_sys);
static LSA_UINT16 SOCK_LOCAL_FCT_ATTR sock_mib2_copy_params_to_local_storage(SOCK_MIB2_SYSGROUP_DATA_PTR_TYPE p_mib2_sys);
static LSA_VOID   SOCK_LOCAL_FCT_ATTR sock_mib2_mov_system_params_to_rqb(SOCK_MIB2_SYSGROUP_DATA_PTR_TYPE p_mib2_sys);


/*------------------------------------------------------------------------------
// SOCK_OPC_GET_MIB2_IFINDEX: return interface index
//----------------------------------------------------------------------------*/
LSA_UINT16 SOCK_LOCAL_FCT_ATTR
sock_mib2_get_ifindex(
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_GET_MIB2_IFINDEX_PTR_TYPE gif = & rqb->args.get_mib2_ifindex;
	LSA_BOOL ret;

#if SOCK_CFG_INTERNICHE
	ret = tcip_get_mib2_ifindex(gif->InterfaceID, gif->PortID, &gif->IfIndex );
#else
	ret = SOCK_GET_MIB2_IFINDEX(gif->InterfaceID, gif->PortID, &gif->IfIndex);
#endif

	if( ret )
	{
		return SOCK_RSP_OK;
	}
	else
	{
		return SOCK_RSP_ERR_PARAM;
	}
}


/*------------------------------------------------------------------------------
// SOCK_OPC_SET_MIB2_IFGROUP_DATA: set mib2 interface group data
//----------------------------------------------------------------------------*/
LSA_UINT16 SOCK_LOCAL_FCT_ATTR
sock_mib2_set_if_params(
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_MIB2_IFGROUP_DATA_PTR_TYPE ifd = &rqb->args.set_mib2_ifgroup_data;
	LSA_BOOL ret;

#if SOCK_CFG_INTERNICHE
	ret = tcip_set_mib2_ifgroup_data(ifd->InterfaceID, ifd->PortID, ifd->ifDescr.pName, ifd->ifDescr.Length );
#else
	ret = SOCK_SET_MIB2_IFGROUP_DATA(ifd->InterfaceID, ifd->PortID, ifd->ifDescr);
#endif

	if( ret )
	{
		return SOCK_RSP_OK;
	}
	else
	{
		return SOCK_RSP_ERR_PARAM;
	}
}


/*------------------------------------------------------------------------------
// SOCK_OPC_SET_MIB2_SYSGROUP_DATA: set mib2 system group data
//----------------------------------------------------------------------------*/
LSA_UINT16 SOCK_LOCAL_FCT_ATTR
sock_mib2_set_system_params(
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	SOCK_LOG_FCT("sock_mib2_set_system_params")

	LSA_UINT16 response = SOCK_RSP_OK;
	SOCK_MIB2_SYSGROUP_DATA_PTR_TYPE sgd = &rqb->args.set_mib2_sysgroup_data;
	LSA_BOOL ret;

#if SOCK_CFG_INTERNICHE
	ret = tcip_set_mib2_sysgroup_data( sgd->sysName.pName, sgd->sysName.Length,
									 sgd->sysDescr.pName, sgd->sysDescr.Length,
									 sgd->sysContact.pName, sgd->sysContact.Length,
									 sgd->sysLocation.pName, sgd->sysLocation.Length  );
#else
	ret = SOCK_SET_MIB2_SYSGROUP_DATA(sgd->sysName, sgd->sysDescr, sgd->sysContact, sgd->sysLocation);
#endif

	if( ret )
	{
		response = sock_mib2_copy_params_to_local_storage(sgd);

		sock_mib2_check_sys_changes();
	}
	else
	{
		response = SOCK_RSP_ERR_PARAM;
	}

	return response;
}


/*------------------------------------------------------------------------------
// SOCK_OPC_MIB2_SYSGROUP_DATA_IND
//----------------------------------------------------------------------------*/
LSA_UINT16 SOCK_LOCAL_FCT_ATTR
sock_mib2_set_sysgroup_data_ind(
	SOCK_CHANNEL_PTR_TYPE cha,
	SOCK_UPPER_RQB_PTR_TYPE rqb
) {
	LSA_BOOL    b_mib2_unchanged = LSA_TRUE;
	LSA_UINT16  response;
	SOCK_MIB2_SYSGROUP_DATA_PTR_TYPE mib2sys;

	/*
	 * return sequence error if user has already provided indication resource
	 */
	if( sock_is_not_null(cha->rqb_mib2_ind) )
	{
		return SOCK_RSP_ERR_SEQUENCE;
	}

	mib2sys = &rqb->args.mib2_sysgroup_data_ind;

	/*
	 * sysDescr, sysContact, sysLocation, sysName of indication resource equals internally stored info ?
	 */
	b_mib2_unchanged = sock_mib2_cmp_system_params( mib2sys );

	if( b_mib2_unchanged ) /* store indication rqb */
	 {
		cha->rqb_mib2_ind = rqb;
		response = SOCK_RSP_OK_ACTIVE;
	}
	else
	{
		sock_mib2_mov_system_params_to_rqb( mib2sys );
		response = SOCK_RSP_OK;
	}
	return (response);
}


/*------------------------------------------------------------------------------
// copy the mib2 params to the indication
//----------------------------------------------------------------------------*/
static LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_mib2_mov_system_params_to_rqb(
	SOCK_MIB2_SYSGROUP_DATA_PTR_TYPE p_mib2_sys
) {
	SOCK_LOG_FCT("sock_mib2_mov_system_params_to_rqb")

	SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_NOTE,">>");

	/*
	 * sysName
	 */
	if( sock_is_not_null(p_mib2_sys->sysName.pName) )
	{
		SOCK_MEMCPY(p_mib2_sys->sysName.pName, sock_data.sysName.pName, sock_data.sysName.Length);
		p_mib2_sys->sysName.Length = sock_data.sysName.Length;
		p_mib2_sys->sysName.Remanent = sock_data.sysName.Remanent;
	}

	/*
	 * sysDescr
	 */
	if( sock_is_not_null(p_mib2_sys->sysDescr.pName) )
	{
		SOCK_MEMCPY(p_mib2_sys->sysDescr.pName, sock_data.sysDescr.pName, sock_data.sysDescr.Length);
		p_mib2_sys->sysDescr.Length = sock_data.sysDescr.Length;
		p_mib2_sys->sysDescr.Remanent = sock_data.sysDescr.Remanent;
	}

	/*
	 * sysContact
	 */
	if( sock_is_not_null(p_mib2_sys->sysContact.pName) )
	{
		SOCK_MEMCPY(p_mib2_sys->sysContact.pName, sock_data.sysContact.pName, sock_data.sysContact.Length);
		p_mib2_sys->sysContact.Length = sock_data.sysContact.Length;
		p_mib2_sys->sysContact.Remanent = sock_data.sysContact.Remanent;
	}

	/*
	 * sysLocation
	 */
	if( sock_is_not_null(p_mib2_sys->sysLocation.pName) )
	{
		SOCK_MEMCPY(p_mib2_sys->sysLocation.pName, sock_data.sysLocation.pName, sock_data.sysLocation.Length);
		p_mib2_sys->sysLocation.Length = sock_data.sysLocation.Length;
		p_mib2_sys->sysLocation.Remanent = sock_data.sysLocation.Remanent;
	}

	SOCK_UPPER_TRACE_00(0, LSA_TRACE_LEVEL_NOTE,"<<");
}


/*------------------------------------------------------------------------------
// copy the mib2 sys stetting to the local storage
//----------------------------------------------------------------------------*/
static LSA_UINT16 SOCK_LOCAL_FCT_ATTR
sock_mib2_copy_params_to_local_storage(
	SOCK_MIB2_SYSGROUP_DATA_PTR_TYPE p_mib2_sys
) {
	SOCK_LOG_FCT("sock_mib2_copy_params_to_local_storage")

	/*
	 *  check arguments
	 */
	if ( sock_is_not_null(p_mib2_sys->sysName.pName ) && p_mib2_sys->sysName.Length  > SOCK_MAX_MIBII_IFSTRING_LENGTH  )
	{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "passed sysName string longer than 255 characters (%d)", p_mib2_sys->sysName.Length);
		return SOCK_RSP_ERR_PARAM;
	}

	if ( sock_is_not_null(p_mib2_sys->sysDescr.pName ) && p_mib2_sys->sysDescr.Length  > SOCK_MAX_MIBII_IFSTRING_LENGTH )
	{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "passed sysDescr string longer than 255 characters (%d)", p_mib2_sys->sysDescr.Length);
		return SOCK_RSP_ERR_PARAM;
	}

	if ( sock_is_not_null(p_mib2_sys->sysContact.pName) && p_mib2_sys->sysContact.Length  > SOCK_MAX_MIBII_IFSTRING_LENGTH )
	{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "passed sysContact string longer than 255 characters (%d)", p_mib2_sys->sysContact.Length);
		return SOCK_RSP_ERR_PARAM;
	}

	if ( sock_is_not_null(p_mib2_sys->sysLocation.pName ) && p_mib2_sys->sysLocation.Length  > SOCK_MAX_MIBII_IFSTRING_LENGTH )
	{
		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "passed sysLocation string longer than 255 characters (%d)", p_mib2_sys->sysLocation.Length);
		return SOCK_RSP_ERR_PARAM;
	}


	/*
	 * transfer data if ok
	 */
	if ( sock_is_not_null(p_mib2_sys->sysName.pName) )
	{
		SOCK_MEMCPY(sock_data.sysName.pName, p_mib2_sys->sysName.pName, p_mib2_sys->sysName.Length );
		sock_data.sysName.Length = p_mib2_sys->sysName.Length;
		sock_data.sysName.Remanent = p_mib2_sys->sysName.Remanent;
	}

	if ( sock_is_not_null(p_mib2_sys->sysDescr.pName) )
	{
		SOCK_MEMCPY(sock_data.sysDescr.pName, p_mib2_sys->sysDescr.pName, p_mib2_sys->sysDescr.Length );
		sock_data.sysDescr.Length = p_mib2_sys->sysDescr.Length;
		sock_data.sysDescr.Remanent = p_mib2_sys->sysDescr.Remanent;
	}

	if ( sock_is_not_null(p_mib2_sys->sysContact.pName) )
	{
		SOCK_MEMCPY(sock_data.sysContact.pName, p_mib2_sys->sysContact.pName, p_mib2_sys->sysContact.Length );
		sock_data.sysContact.Length = p_mib2_sys->sysContact.Length;
		sock_data.sysContact.Remanent = p_mib2_sys->sysContact.Remanent;
	}

	if ( sock_is_not_null(p_mib2_sys->sysLocation.pName) )
	{
		SOCK_MEMCPY(sock_data.sysLocation.pName, p_mib2_sys->sysLocation.pName, p_mib2_sys->sysLocation.Length );
		sock_data.sysLocation.Length = p_mib2_sys->sysLocation.Length;
		sock_data.sysLocation.Remanent = p_mib2_sys->sysLocation.Remanent;
	}

	return SOCK_RSP_OK;
}


/*------------------------------------------------------------------------------
// returns user provided ind resource if available and deviations are detected
//----------------------------------------------------------------------------*/
LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_mib2_check_sys_changes(
	LSA_VOID
) {
	SOCK_LOG_FCT("sock_mib2_check_sys_changes")

	SOCK_CHANNEL_PTR_TYPE channel;
	int i;

	for(i = 0; i < SOCK_CFG_MAX_CHANNELS; i++)
	{
		channel = &sock_data.sock_channels[i];

		if( channel->state != SOCK_CHANNEL_OPENED )
		{
			continue;
		}

		/* indicate change to all users which provided SOCK_OPC_MIB2_SYSGROUP_DATA_IND */

		if ( sock_is_not_null(channel->rqb_mib2_ind) )
		{
			LSA_BOOL b_sysgroup_not_changed = sock_mib2_cmp_system_params( &(channel->rqb_mib2_ind->args.mib2_sysgroup_data_ind)  );

			if( !b_sysgroup_not_changed )
			{
				SOCK_MIB2_SYSGROUP_DATA_PTR_TYPE sdi = &(channel->rqb_mib2_ind->args.mib2_sysgroup_data_ind);
				sock_mib2_mov_system_params_to_rqb( sdi );
				sock_usr_callback(channel, SOCK_RSP_OK, &channel->rqb_mib2_ind);
			}
		}
	}
}


/*------------------------------------------------------------------------------
// returns LSA_FALSE if a difference between locally stored, otherwise true
//----------------------------------------------------------------------------*/
static LSA_BOOL SOCK_LOCAL_FCT_ATTR
sock_mib2_cmp_system_params(
	SOCK_MIB2_SYSGROUP_DATA_PTR_TYPE p_mib2_sys
) {
	/*
	 * sysName
	 */
	if( sock_is_not_null(p_mib2_sys->sysName.pName) )
	{
		if((sock_data.sysName.Length) != (p_mib2_sys->sysName.Length))
		{
			return LSA_FALSE;
		}

		if( sock_data.sysName.Remanent != p_mib2_sys->sysName.Remanent )
		{
			return LSA_FALSE;
		}

		/*
		 * the same length -> compare the buffer content
		 */
		 if (SOCK_MEMCMP(sock_data.sysName.pName, p_mib2_sys->sysName.pName, sock_data.sysName.Length))
		{
			return LSA_FALSE;
		}
	}

	/*
	 * sysDescr
	 */
	if( sock_is_not_null(p_mib2_sys->sysDescr.pName) )
	{
		if((sock_data.sysDescr.Length ) != (p_mib2_sys->sysDescr.Length))
		{
			return LSA_FALSE;
		}

		if( sock_data.sysDescr.Remanent != p_mib2_sys->sysDescr.Remanent )
		{
			return LSA_FALSE;
		}

		/*
		 * the same length -> compare the buffer content
		 */
		 if (SOCK_MEMCMP(sock_data.sysDescr.pName, p_mib2_sys->sysDescr.pName, sock_data.sysDescr.Length ))
		{
			return LSA_FALSE;
		}
	}

	/*
	 * sysContact
	 */
	if( sock_is_not_null(p_mib2_sys->sysContact.pName) )
	{
		if((sock_data.sysContact.Length ) != (p_mib2_sys->sysContact.Length))
		{
			return LSA_FALSE;
		}

		if( sock_data.sysContact.Remanent != p_mib2_sys->sysContact.Remanent )
		{
			return LSA_FALSE;
		}

		/*
		 * the same length -> compare the buffer content
		 */
		 if (SOCK_MEMCMP(sock_data.sysContact.pName, p_mib2_sys->sysContact.pName, sock_data.sysContact.Length ))
		{
			return LSA_FALSE;
		}
	}

	/*
	 * sysLocation
	 */
	if( sock_is_not_null(p_mib2_sys->sysLocation.pName) )
	{
		if((sock_data.sysLocation.Length ) != (p_mib2_sys->sysLocation.Length))
		{
			return LSA_FALSE;
		}

		if( sock_data.sysLocation.Remanent != p_mib2_sys->sysLocation.Remanent )
		{
			return LSA_FALSE;
		}

		/*
		 * the same length -> compare the buffer content
		 */
		 if (SOCK_MEMCMP(sock_data.sysLocation.pName, p_mib2_sys->sysLocation.pName, sock_data.sysLocation.Length ) )
		{
			return LSA_FALSE;
		}
	}


	return LSA_TRUE;
}


/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
