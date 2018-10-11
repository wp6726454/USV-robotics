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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_cl41.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  MasterProject client MachineTailoring                                    */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	641
#define CM_MODULE_ID		641

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */

/*----------------------------------------------------------------------------*/
#if CM_CFG_MAX_CLIENTS

/*----------------------------------------------------------------------------*/
LSA_VOID  CM_LOCAL_FCT_ATTR
cm_clusr_dev_get_name (
	CM_CHANNEL_PTR_TYPE  pd_channel,
	LSA_UINT16  dev_nr,
	LSA_UINT16  *device_name_length,
	CM_UPPER_MEM_U8_PTR_TYPE  *device_name
	)
{
	CM_LOG_FCT("cm_clusr_dev_get_name")

	CM_CHANNEL_PTR_TYPE cl_channel = cm_peer_channel_get (pd_channel, CM_PATH_TYPE_CLIENT);

	if (is_not_null (cl_channel))
	{
		CM_CL_DEVICE_PTR_TYPE dev = cm_cl_dev_from_device_nr (cl_channel, dev_nr);
		
		if (is_not_null (dev))
		{
			*device_name_length = dev->dev_addr.device_name_length;
			*device_name = dev->dev_addr.device_name;
			return;
		}
	}

	*device_name_length = 0;
	*device_name = LSA_NULL;
}


/*------------------------------------------------------------------------------
//	Series and Modular Machines / MachineTailoring
//----------------------------------------------------------------------------*/

LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_cl_mp_is_valid_alias_name (
	CM_UPPER_ARCB_NARE_ALIAS_PTR_TYPE  alias,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd
	)
{
	if (alias->name_length == 0)
	{
		if (mtd->properties & CM_PD_MTD_PROP_I_ANY_PEER_PROG)
		{
			return LSA_TRUE;
		}
		else
		{
			return LSA_FALSE;
		}
	}

	if (cm_is_valid_alias_name (alias->name_length, alias->name))
	{
		return LSA_TRUE;
	}

	if ((mtd->properties & CM_PD_MTD_PROP_NOS) 
		&& cm_is_valid_port_name (alias->name_length, alias->name))
	{
		return LSA_TRUE;
	}

	return LSA_FALSE;
}


/*----------------------------------------------------------------------------*/
static  LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_cl_mp_prealloc_tailor_data (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd
	)
{
	CM_LOG_FCT("cm_cl_mp_prealloc_tailor_data")

	if (is_not_null (mtd))
	{
		CM_AR_GRAPH_AP_PTR_TYPE  ap;
		for (CmListForeach (ap, & argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
		{
			CM_AR_GRAPH_MODULE_PTR_TYPE  mod;
			for (CmListForeach (mod, & ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE))
			{
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;
				for (CmListForeach (sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
				{
					CM_AR_GRAPH_RECORD_PTR_TYPE  rec;
					for (CmListForeach (rec, & sub->cl.prm_record_list, CM_AR_GRAPH_RECORD_PTR_TYPE))
					{
						if (cm_pd_mpa_prm_parse_record (argr->ar_com.channel, mtd, rec) != CM_OK)
						{
							return LSA_FALSE;
						}

						if (cm_pd_mpa_prm_prealloc_tailor_data (LSA_NULL/*usecase cmcl*/, rec) != CM_OK)
						{
							CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
								, "dev(%u) ar(%u): tailor data pre-allocation error: record_index(0x%x)"
								, argr->device_nr, argr->ar_nr, rec->record_index
								);
							return LSA_FALSE;
						}
					}
				}
			}
		}

		if ((mtd->properties & CM_PD_MTD_PROP_DEVICE_OPTIONAL)  &&  (mtd->pdirglobaldata_version != 0))
		{
			CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;
			LSA_UINT i = 0;

			for (CmListForeach (iocr, & argr->io_cr_list, CM_AR_GRAPH_IO_CR_PTR_TYPE))
			{
				if (! iocr->cm.is_multicast
				&&  (iocr->iocr_properties & CM_IOCR_PROP_RT_CLASS_MASK) == CM_IOCR_PROP_RT_CLASS_3)
				{
					if (i < (sizeof(mtd->frame_id)/sizeof(mtd->frame_id[0])))
					{
						if (CM_REDUNDANT_FRAME_ID_BASE (iocr->frame_id) != 0)
						{
							mtd->frame_id[i] = CM_REDUNDANT_FRAME_ID_BASE (iocr->frame_id);
						}
						else
						{
							mtd->frame_id[i] = iocr->frame_id;
						}
						i++;
					}
					else
					{
						CM_CL_TRACE_00 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "tailor data preallocation error: more then 2 frames");
						return LSA_FALSE;
					}
				}
			}
		}
	}
	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
LSA_UINT32  CM_LOCAL_FCT_ATTR
cm_cl_mp_ar_graph_check_consistency (
	CM_CL_DEVICE_PTR_TYPE  dev,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd
	)
{
	CM_LOG_FCT("cm_cl_mp_ar_graph_check_consistency")


	if ((mtd->properties & CM_PD_MTD_PROP_NOS) && ((dev->planned_mode & CM_CL_PMODE_NOS_OVERWRITE_MASK) != CM_CL_PMODE_NOS_OVERWRITE_YES))
	{
		CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
			, "dev(%u) ar(%u): MasterProject needs property CM_CL_PMODE_NOS_OVERWRITE_YES", argr->device_nr, argr->ar_nr
			);
		return CM_PNIO_FAULTY_ARBlockReq (5/*ARUUID*/);
	}
	{
		LSA_UINT16  slot_nr = 0xFFFF;
		CM_AR_GRAPH_AP_PTR_TYPE  ap;

		for (CmListForeach (ap, & argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
		{
			CM_AR_GRAPH_MODULE_PTR_TYPE  mod;
			for (CmListForeach (mod, & ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE))
			{
				CM_AR_GRAPH_SUBMODULE_PTR_TYPE  sub;
				for (CmListForeach (sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
				{
					if (CM_IS_PDEV_SUBSLOT_NR (sub->subslot_nr))
					{
						if (slot_nr == 0xFFFF)
						{
							slot_nr = mod->slot_nr;
						}
						else if (slot_nr != mod->slot_nr)
						{
							/* see cm_pd_mtd_lookup_mtdp_from_index() */
							CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_ERROR
								, "dev(%u) ar(%u): all Pdev.Subslots of an MasterProject must located on the same slot"
								, argr->device_nr, argr->ar_nr
								);
							return CM_PNIO_FAULTY_ExpectedSubmoduleBlockReq (6/*slot_nr*/);
						}
					}
				}
			}
		}
	}

	if (! cm_cl_mp_prealloc_tailor_data (argr, mtd))
	{
		return CM_PNIO_FAULTY_ConnectBlockReq (62/*CMCTL*/, 3/*Out of resource*/);
	}

	return CM_PNIO_ERR_NONE;
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL CM_LOCAL_FCT_ATTR
cm_cl_mp_tailor_record_data (
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd
	)
{
	CM_LOG_FCT ("cm_cl_mp_tailor_record_data")
	CM_CHANNEL_PTR_TYPE            channel = argr->cm.cl.dev->channel;
	CM_AR_GRAPH_AP_PTR_TYPE        ap;
	CM_AR_GRAPH_MODULE_PTR_TYPE    mod;
	CM_AR_GRAPH_SUBMODULE_PTR_TYPE sub;
	CM_AR_GRAPH_RECORD_PTR_TYPE    rec, rec_8052 = LSA_NULL;
	CM_COMMON_MEM_U8_PTR_TYPE      nos_cl;
	LSA_UINT16                     nos_cl_len;
	LSA_UINT16                     rc;

	if (! cm_oha_get_name_of_station (channel, &nos_cl_len, &nos_cl))
	{
		return LSA_FALSE;
	}

	for (CmListForeach (ap, & argr->ap_list, CM_AR_GRAPH_AP_PTR_TYPE))
	{
		/* First loop to tailor all records but 0x8051 and 0x8053.
		//	If 0x8052 is found use it for tailoring 0x8051 and 0x8053
		*/
		rec_8052 = LSA_NULL;
		for (CmListForeach (mod, & ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE))
		{
			for (CmListForeach (sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
			{
				for (CmListForeach (rec, & sub->cl.prm_record_list, CM_AR_GRAPH_RECORD_PTR_TYPE))
				{
					switch (rec->record_index) 
					{
					case 0x8052: /* PDInterfaceMrpDataAdjust */
						rc = cm_pd_mpa_prm_write_tailor_data (channel, mtd, rec, LSA_NULL, nos_cl, nos_cl_len);
						if (rc != LSA_OK)
						{
							return LSA_FALSE;
						}
						rec_8052 = rec;
						break;

					case 0x802b: /* PDPortDataCheck */
					case 0x802f: /* PDPortDataAdjust */
					case 0x802d: /* PDSyncData */
					case 0x802C: /* PDIRData */
						rc = cm_pd_mpa_prm_write_tailor_data (channel, mtd, rec, LSA_NULL, nos_cl, nos_cl_len);
						if (rc != LSA_OK)
						{
							return LSA_FALSE;
						}
						break;

					default:
						break;
					}
				}
			}
		}
		/* If we have previously found a record with index 0x8052 and if we tailor mrp-data,
		loop through all records to tailor records 0x8051 and 0x8053 */
		if (is_not_null(rec_8052))
		{
			for (CmListForeach (mod, & ap->mod_list, CM_AR_GRAPH_MODULE_PTR_TYPE))
			{
				for (CmListForeach (sub, & mod->submod_list, CM_AR_GRAPH_SUBMODULE_PTR_TYPE))
				{
					for (CmListForeach (rec, & sub->cl.prm_record_list, CM_AR_GRAPH_RECORD_PTR_TYPE))
					{
						switch (rec->record_index) 
						{
						case 0x8051: /* PDInterfaceMrpDataCheck */
						case 0x8053: /* PDPortMrpDataAdjust */
							rc = cm_pd_mpa_prm_write_tailor_data (channel, mtd, rec, rec_8052, nos_cl, nos_cl_len);
							if (rc != CM_OK)
							{
								return LSA_FALSE;
							}
							break;

						default:
							break;
						}
					}
				}
			}
		}
	}
	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
static LSA_UINT16 CM_LOCAL_FCT_ATTR 
cm_cl_mp_tailor_name (
	CM_COMMON_MEM_U8_PTR_TYPE	dst,
	CM_COMMON_MEM_U8_PTR_TYPE	name,
	LSA_UINT16					namelen,
	CM_COMMON_MEM_U8_PTR_TYPE	ioc_nos,
	LSA_UINT16					ioc_nos_len
	)
{
	LSA_UINT16  len = namelen + ioc_nos_len + 1;

	CM_MEMCPY (dst, name, namelen);
	dst += namelen;
	*(dst++) = '.';
	CM_MEMCPY (dst, ioc_nos, ioc_nos_len);

	return len;
}


/*----------------------------------------------------------------------------*/
static LSA_BOOL CM_LOCAL_FCT_ATTR
cm_cl_mp_tailor_device_names (
	CM_CL_DEVICE_PTR_TYPE				dev,
	LSA_UINT16							ioc_nos_len,
	CM_COMMON_MEM_U8_PTR_TYPE			ioc_nos,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE	mtd
	)
{
	CM_LOG_FCT ("cm_cl_mp_tailor_device_names")
	CM_CL_NARE_ALIAS_PTR_TYPE  alias;
	LSA_UINT16                 index;


	if (mtd->properties & CM_PD_MTD_PROP_NOS)
	{
		if (dev->dev_addr.device_name_length + 1 + ioc_nos_len > 240)
		{
			CM_CL_TRACE_01 (dev->channel->trace_idx , LSA_TRACE_LEVEL_UNEXP, "dev(%u) length of tailored NameOfStation of IOD >240", dev->device_nr);
			return LSA_FALSE;
		}
		dev->dev_addr.tailored_device_name_length = cm_cl_mp_tailor_name (dev->dev_addr.tailored_device_name
			, dev->dev_addr.device_name, dev->dev_addr.device_name_length, ioc_nos, ioc_nos_len
		);
	}

	index = 0;
	for (CmListForeach (alias, & dev->dev_addr.device_alias_list, CM_CL_NARE_ALIAS_PTR_TYPE))
	{
		alias->name_length = 0;
		if (index < mtd->port_cnt) 
		{
			CM_CL_DEVICE_PTR_TYPE	peer_dev;
			LSA_UINT16				real_station_nr, real_slot_nr, real_subslot_nr, real_properties;

			cm_pd_mtdp_get_peer (&mtd->ports[index], &real_station_nr, &real_slot_nr, &real_subslot_nr, &real_properties);
			index++;

			if (! CM_IS_PDEV_SUBSLOT_NR (real_subslot_nr))
			{
				continue;
			}

			alias->name_length = cm_pd_mpa_put_port_name (alias->name, real_properties, real_subslot_nr, real_slot_nr);

			if (real_station_nr > 0)
			{
				peer_dev = cm_cl_dev_from_device_nr (dev->channel, real_station_nr);
				if (is_null(peer_dev))
				{
					CM_CL_TRACE_01 (dev->channel->trace_idx , LSA_TRACE_LEVEL_UNEXP
						, "required dev(%u) not yet added", real_station_nr
						);
					return LSA_FALSE;
				}

				CM_PUT8_HTON (alias->name, alias->name_length, '.');
				alias->name_length ++;

				CM_MEMCPY (alias->name + alias->name_length, peer_dev->dev_addr.device_name, peer_dev->dev_addr.device_name_length);
				alias->name_length += peer_dev->dev_addr.device_name_length;
			}

			if (real_station_nr == 0 || mtd->properties & CM_PD_MTD_PROP_NOS)
			{
				if (alias->name_length + 1 + ioc_nos_len  > OHA_MAX_STATION_NAME_LEN)
				{
					CM_CL_TRACE_02 (dev->channel->trace_idx , LSA_TRACE_LEVEL_UNEXP
						, "dev(%u) length of tailored Alias of IOD(%u) >255", dev->device_nr, real_station_nr
						);
					return LSA_FALSE;
				}

				CM_PUT8_HTON (alias->name, alias->name_length, '.');
				alias->name_length ++;

				CM_MEMCPY (alias->name + alias->name_length, ioc_nos, ioc_nos_len);
				alias->name_length += ioc_nos_len;
			}
		}
	}
	return LSA_TRUE;
}


/*----------------------------------------------------------------------------*/
LSA_BOOL CM_LOCAL_FCT_ATTR
cm_cl_mp_tailor_ipsuite (
	CM_CL_DEVICE_PTR_TYPE	dev,
	LSA_UINT32				ioc_ipaddr,
	LSA_UINT32				ioc_netmask
	)
{
	CM_LOG_FCT ("cm_cl_mp_tailor_ipsuite")

	/*
	// SPH Series and Modular Machines V1.0i06, chapter 6.5.1, subclause (e)
	//
	// IPAddress-IOD = IPAddress-IOC + Number-IOD
	// - shall be in the same subnet as the IO controller
	//
	// IPAddress is invalid if equals...
	// - the subnet address: the host part of the address is ??
	// - the directed broadcast address: the host part of the address is a series of consecutive binary ??
	// - the limited broadcast address (i.e. 255.255.255.255),
	// - the subnet part of the address is ??
	// - an address in the address range from 0.0.0.0 up to 0.255.255.255 (highest 8 bit of the address are set to ?000 0000?
	// - a loopback address: an address in the address range from 127.0.0.0 up to 127.255.255.255 (highest 8 bit of the address are set to ?111 1111?,
	// - a multicast address: an address in the address range from 224.0.0.0 up to 239.255.255.255 (highest 4 bit of the address are ?110?,
	// - a reserved address: an address in the address range from 240.0.0.0 up to 255.255.255.255 (highest 4 bit of the address are ?111?.
	//
	// See too SPH IP Configuration V1.0i06, chapter 3.4.1.
	*/

	ioc_ipaddr  = CM_NTOH32(ioc_ipaddr);
	ioc_netmask = CM_NTOH32(ioc_netmask);

	if (dev->device_nr == 0 /* always false */
	|| ioc_netmask == 0)    /* always false (see SPH IP Configuration, TIA 827678 and 903044) */
	{
		return LSA_FALSE;
	}
	else
	{
		LSA_UINT32 iod_ipaddr = ioc_ipaddr + dev->device_nr;

		if ((iod_ipaddr & ioc_netmask) != (ioc_ipaddr & ioc_netmask)) /* did overflow */
		{
			return LSA_FALSE;
		}
		else /* note: iod_ipaddr > ioc_ipaddr */
		{
			LSA_UINT32 host0_addr = (iod_ipaddr & ioc_netmask);
			LSA_UINT32 host1_addr = (iod_ipaddr | ~ioc_netmask);

			if (iod_ipaddr == host0_addr /* note: impossible */
			||  iod_ipaddr == host1_addr /* directed broadcast */
			||  iod_ipaddr == 0xFFFFFFFF /* 255.255.255.255 (limited broadcast) */
			||  (iod_ipaddr & ioc_netmask) == 0 /* 0.0.0.0/mask */
			||  (iod_ipaddr & 0xFF000000) == 0  /* 0.0.0.0/8 */
			||  (iod_ipaddr & 0xFF000000) == 0x7F000000  /* 127.0.0.0/8 (loopback) */
			||  (iod_ipaddr & 0xF0000000) == 0xE0000000  /* 224.0.0.0/4 (multicast) */
			||  (iod_ipaddr & 0xF0000000) == 0xF0000000) /* 240.0.0.0/4 (reserved) */
			{
				return LSA_FALSE;
			}
			else
			{
				dev->dev_addr.device_ip_addr = CM_HTON32(iod_ipaddr);
				return LSA_TRUE;
			}
		}
	}
}


/*----------------------------------------------------------------------------*/
LSA_BOOL  CM_LOCAL_FCT_ATTR
cm_cl_mp_tailor (
	CM_CHANNEL_PTR_TYPE  channel,
	CM_AR_GRAPH_PTR_TYPE  argr,
	CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  mtd,
	CM_CL_DOWN_INFO_PTR_TYPE  dinfo
	)
{
	CM_LOG_FCT("cm_cl_mp_tailor")
	CM_CL_DEVICE_PTR_TYPE  dev = argr->cm.cl.dev;
	CM_CHANNEL_PTR_TYPE  pd_channel = cm_peer_channel_get (channel, CM_PATH_TYPE_PD);
	LSA_UINT16  ioc_nos_len;
	CM_COMMON_MEM_U8_PTR_TYPE  ioc_nos;
	LSA_UINT32  ioc_ipaddr, ioc_netmask, ioc_gateway;

	/* CM_ERROR_CAUSE_MP_IN_PROGRESS		17 MasterProject: Machine- or AddressTailoring in progress */
	{
		if ((pd_channel->usr.pd.rtf.wf_sm != 0)
		||  (pd_channel->usr.pd.pdprm.wait_cnf != 0 || pd_channel->usr.pd.pdprm.wait_cnf_next != 0)
		||	(pd_channel->usr.pd.reclist.is_running_mpa))
		{
			CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "dev(%u) ar(%u) the PDev is not stable, tailoring in progress", argr->device_nr, argr->ar_nr);
			CM_CL_DINFO_SET (dinfo, CM_AR_REASON_ABORT, CM_ERROR_CAUSE_MP_IN_PROGRESS, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ABORT));
			return LSA_FALSE;
		}
	}
	
	/* CM_ERROR_CAUSE_MP_NOS_MISSING		18 MasterProject: NameOfStation adaption missing (PDev with invalid NoS) */
	{
		if (! cm_oha_get_name_of_station (channel, &ioc_nos_len, &ioc_nos))
		{
			CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "dev(%u) ar(%u) NameOfStation adaption missing (PDev with invalid NoS)", argr->device_nr, argr->ar_nr);
			CM_CL_DINFO_SET (dinfo, CM_AR_REASON_DCP_NAME, CM_ERROR_CAUSE_MP_NOS_MISSING, CM_PNIO_FAULTY_RTA(CM_AR_REASON_DCP_NAME));
			return LSA_FALSE;
		}
		if (! cm_is_valid_station_name (ioc_nos_len, ioc_nos))
		{
			CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "dev(%u) ar(%u) NameOfStation for PDev is invalid", argr->device_nr, argr->ar_nr);
			CM_CL_DINFO_SET (dinfo, CM_AR_REASON_DCP_NAME, CM_ERROR_CAUSE_MP_NOS_MISSING, CM_PNIO_FAULTY_RTA(CM_AR_REASON_DCP_NAME));
			return LSA_FALSE;
		}
	}

	/* CM_ERROR_CAUSE_MP_IPv4_MISSING		19 MasterProject: IPv4Suite adaption missing (PDev with invalid IPv4Suite) */
	{
		if (! cm_oha_get_ip_suite (channel, &ioc_ipaddr, &ioc_netmask, &ioc_gateway)) 
		{
			CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "dev(%u) ar(%u) IPv4Suite adaption missing (PDev with invalid IPv4Suite)", argr->device_nr, argr->ar_nr);
			CM_CL_DINFO_SET (dinfo, CM_AR_REASON_IPSUITE, CM_ERROR_CAUSE_MP_IPv4_MISSING, CM_PNIO_FAULTY_RTA(CM_AR_REASON_IPSUITE));
			return LSA_FALSE;
		}

		if (! cm_oha_ip_suite_is_notnull_nonconflicting_and_expected (channel))
		{
			CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "dev(%u) ar(%u) ip-suite not valid, must wait", argr->device_nr, argr->ar_nr);
			CM_CL_DINFO_SET (dinfo, CM_AR_REASON_IPSUITE, CM_ERROR_CAUSE_MP_IPv4_MISSING, CM_PNIO_FAULTY_RTA(CM_AR_REASON_IPSUITE));
			return LSA_FALSE;
		}
	}


	/* CM_ERROR_CAUSE_MP_TAILORING_MISSING	22 MasterProject: MachineTailoring missing */
	{
		CM_PD_MASTER_TAILOR_DATA_PTR_TYPE  pd_mtd = cm_pdusr_mpa_lookup_mtd (pd_channel->usr.pd.pdprm.channel, 0/*PDev*/);

		if (is_not_null (pd_mtd))
		{
			if (pd_mtd->properties & (CM_PD_MTD_PROP_I_ANY_PEER_OPT | CM_PD_MTD_PROP_I_ANY_PEER_PROG))
			{
				if (! (pd_mtd->set_by_itd & CM_PD_MTD_SET_IS_TAILORED))
				{
					CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
						"dev(%u) ar(%u) MachineTailoring missing, pd has optional/programable peer", argr->device_nr, argr->ar_nr);
					CM_CL_DINFO_SET (dinfo, CM_AR_REASON_ABORT, CM_ERROR_CAUSE_MP_TAILORING_MISSING, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ABORT));
					return LSA_FALSE;
				}
			}
		}

		if (mtd->properties & (CM_PD_MTD_PROP_DEVICE_OPTIONAL | CM_PD_MTD_PROP_I_ANY_PEER_OPT | CM_PD_MTD_PROP_I_ANY_PEER_PROG))
		{
			if (! (mtd->set_by_itd & CM_PD_MTD_SET_IS_TAILORED))
			{
				CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
					"dev(%u) ar(%u) MachineTailoring missing, device is optional or has optional/programable peer", argr->device_nr, argr->ar_nr);
				CM_CL_DINFO_SET (dinfo, CM_AR_REASON_ABORT, CM_ERROR_CAUSE_MP_TAILORING_MISSING, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ABORT));
				return LSA_FALSE;
			}
		}

	}

	/* CM_ERROR_CAUSE_MP_DEVICE_DISABLED	24	*/
	{
		if (mtd->properties & CM_PD_MTD_PROP_DEVICE_OPTIONAL)
		{
			if (! (mtd->set_by_itd & CM_PD_MTD_SET_DEVICE_ENABLED))
			{
				CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "dev(%u) ar(%u) MachineTailoring: do not activate a disabled device", argr->device_nr, argr->ar_nr);
				CM_CL_DINFO_SET (dinfo, CM_AR_REASON_ABORT, CM_ERROR_CAUSE_MP_DEVICE_DISABLED, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ABORT));
				return LSA_FALSE;
			}
		}
	}

	/* CM_ERROR_CAUSE_MP_NOS_INVALID		20 MasterProject: tailored NameOfStation of IOD is invalid */
	{
		if (mtd->properties & (CM_PD_MTD_PROP_NOS | CM_PD_MTD_PROP_I_ANY_PEER_OPT | CM_PD_MTD_PROP_I_ANY_PEER_PROG))
		{
			/* CM_ERROR_CAUSE_MP_CONFIG_INCOMPLETE	25	MasterProject: configuration incomplete (e.g. neighbour not exist because of device-remove) */
			if (! cm_pd_mtd_neighbour_exists (channel, mtd))
			{
				CM_CL_DINFO_SET (dinfo, CM_AR_REASON_ABORT, CM_ERROR_CAUSE_MP_CONFIG_INCOMPLETE, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ABORT));
				return LSA_FALSE;
			}

			if (! cm_cl_mp_tailor_device_names (dev, ioc_nos_len, ioc_nos, mtd))
			{   /* device-name & device-aliasnames */ 
				CM_CL_DINFO_SET (dinfo, CM_AR_REASON_DCP_NAME, CM_ERROR_CAUSE_MP_NOS_INVALID, CM_PNIO_FAULTY_RTA(CM_AR_REASON_DCP_NAME));
				return LSA_FALSE;
			}
		}

		if (! cm_cl_mp_tailor_record_data (argr, mtd))
		{
			/* NOS_INVALID: because length of tailored device-nos / syncdomain / mrpdomain may > 240 */
			CM_CL_DINFO_SET (dinfo, CM_AR_REASON_DCP_NAME, CM_ERROR_CAUSE_MP_NOS_INVALID, CM_PNIO_FAULTY_RTA(CM_AR_REASON_DCP_NAME));
			return LSA_FALSE;
		}
	}

	/* CM_ERROR_CAUSE_MP_IPv4_INVALID		21 MasterProject: tailored IPv4Suite of IOD is invalid */
	{
		if (mtd->properties & CM_PD_MTD_PROP_IP_SUITE)
		{
			if (! cm_cl_mp_tailor_ipsuite (dev, ioc_ipaddr, ioc_netmask))
			{
				CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "dev(%u) ar(%u) tailored IPv4Suite of IOD is invalid", argr->device_nr, argr->ar_nr);
				CM_CL_DINFO_SET (dinfo, CM_AR_REASON_IPSUITE, CM_ERROR_CAUSE_MP_IPv4_INVALID, CM_PNIO_FAULTY_RTA(CM_AR_REASON_IPSUITE));
				return LSA_FALSE;
			}
		}
	}

	/* CM_ERROR_CAUSE_MP_PDEV_ERROR		    23 MasterProject: PDev configuration error */
	if (! cm_pdcl_pdev_is_ready_for_schedule (channel))
	{
		CM_CL_TRACE_02 (channel->trace_idx, LSA_TRACE_LEVEL_NOTE, "dev(%u) ar(%u) the PDev has ARP, must wait", argr->device_nr, argr->ar_nr);
		CM_CL_DINFO_SET (dinfo, CM_AR_REASON_ABORT, CM_ERROR_CAUSE_MP_PDEV_ERROR, CM_PNIO_FAULTY_RTA(CM_AR_REASON_ABORT));
		return LSA_FALSE;
	}

	return LSA_TRUE;
}


#else
# ifdef CM_MESSAGE
#  pragma CM_MESSAGE("***** not compiling client part (CM_CFG_MAX_CLIENTS=" CM_STRINGIFY(CM_CFG_MAX_CLIENTS) ")")
# endif /* CM_MESSAGE */
#endif
/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
