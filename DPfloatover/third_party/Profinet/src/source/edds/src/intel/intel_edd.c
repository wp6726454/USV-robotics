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
/*  F i l e               &F: INTEL_edd.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  Low Level functions for EDDS (INTEL)             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  26.07.10    AM    initial version.                                       */
/*  27.06.11    AM    replace "INTEL" in file with your LLIF namespace,      */
/*                    e.g. "MY_ETHERNET_CONTROLLER".                         */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20
#define EDDS_MODULE_ID     LTRC_ACT_MODUL_ID  /* Ids 16..31 reserved for LL */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "edds_inc.h"           /* edds headerfiles */
#include "intel_inc.h"
#include "intel_reg.h"

/*===========================================================================*/
/*                              global Variables                             */
/*===========================================================================*/

/*===========================================================================*/
/*                             external functions                            */
/*===========================================================================*/

#ifdef EDDS_CFG_HW_INTEL

//lint -esym(613,handle,data)
//JB 12/11/2014 handle checked where called - data will be set
//WARNING: be careful when using this function, make sure not to use handle as null ptr!
EDDS_STATIC LSA_BOOL intel_readPHYReg(INTEL_LL_HANDLE_TYPE* const handle,
		LSA_UINT8 PHYRegAddr, LSA_UINT16* data) {
	LSA_BOOL retval;
	LSA_UINT32 mdic;

	intel_reg32(INTEL_MDIC) =
	    INTEL_BIT(INTEL_MDIC_R,0) 							/* clear ready */
			| 	INTEL_BIT(INTEL_MDIC_I,0)								/* no interrupt */
			| INTEL_BIT(INTEL_MDIC_E,0)								/* clear error */
			| INTEL_BITS(INTEL_MDIC_OP,INTEL_MDIC_OP_MDI_READ)		/* read operation */
			| INTEL_BITS(INTEL_MDIC_PHYADD,1)							/* internal mii phy address is always 1 */
			| INTEL_BITS(INTEL_MDIC_REGADD,PHYRegAddr); /* phy register address */

	// check, if PHY has finished and therefore has set the ready-bit
	EDDS_WAIT_UNTIL_TIMEOUT(
			(mdic = intel_reg32(INTEL_MDIC),(1 == INTEL_GETBIT(INTEL_MDIC_R,mdic))||(1 == INTEL_GETBIT(INTEL_MDIC_E,mdic))),
			100, retval = LSA_FALSE;);

	if (INTEL_GETBIT(INTEL_MDIC_E,mdic)) {
		retval = LSA_FALSE;
	} else {
		*data = INTEL_GETBITS(INTEL_MDIC_DATA,mdic);
		retval = LSA_TRUE;
	}

	return retval;
}
//lint -esym(613,handle)

EDDS_STATIC LSA_BOOL intel_writePHYReg(INTEL_LL_HANDLE_TYPE* const handle,
		LSA_UINT8 PHYRegAddr, LSA_UINT16 data) {
	LSA_UINT32 mdic;

	LSA_BOOL writtenOK;

	intel_reg32(INTEL_MDIC) =
	    INTEL_BITS(INTEL_MDIC_DATA,data)
			| INTEL_BIT(INTEL_MDIC_R,0)							/* clear ready */
			| INTEL_BIT(INTEL_MDIC_I,0) 							/* no interrupt */
			| INTEL_BIT(INTEL_MDIC_E,0)							/* clear error */
			| INTEL_BITS(INTEL_MDIC_OP,INTEL_MDIC_OP_MDI_WRITE)	/* read operation */
			| INTEL_BITS(INTEL_MDIC_PHYADD,1)						/* internal mii phy address is always 1 */
			| INTEL_BITS(INTEL_MDIC_REGADD,PHYRegAddr); 			/* phy register address */

	// check, if PHY has finished and therefore has set the ready-bit
	writtenOK = LSA_TRUE;
	EDDS_WAIT_UNTIL_TIMEOUT(
			(mdic = intel_reg32(INTEL_MDIC),(1 == INTEL_GETBIT(INTEL_MDIC_R,mdic))||(1 == INTEL_GETBIT(INTEL_MDIC_E,mdic))),
			100, writtenOK = LSA_FALSE; );

	return writtenOK;
}

EDDS_STATIC LSA_BOOL intel_acquire_semaphore(INTEL_LL_HANDLE_TYPE* const handle, LSA_UINT8 semaphore)
{
    LSA_UINT32 sw_fw_sync;
    LSA_BOOL success;
    LSA_INT retries = 0;

    do
    {
        retries = 10;
        success = !(LSA_FALSE);

        switch(handle->params.DeviceID)
        {
            case I210:
            {
                /* 1.) Get ownership of the software/firmware semaphore SWSM.SWESMBI */
                EDDS_DO_UNTIL_TIMEOUT(INTEL_SETBIT(intel_reg32(INTEL_SWSM),INTEL_SWSM_SWESMBI),
                       0 != INTEL_GETBIT(INTEL_SWSM_SWESMBI,intel_reg32(INTEL_SWSM)), 20, success = LSA_FALSE;);

                if (LSA_FALSE != success)
                {

                    sw_fw_sync = intel_reg32(INTEL_SW_FW_SYNC);

                    /* 2.) check if FW semapore is set */
                    if (0 == INTEL_GETBIT(semaphore + 16,sw_fw_sync))
                    {
                        /* 3.) set SW semaphore */
                        INTEL_SETBIT(sw_fw_sync,semaphore + 0);
                        intel_reg32(INTEL_SW_FW_SYNC) = sw_fw_sync;
                        /* 4.) release ownership of the software/firmware semaphore SWSM.SWESMBI */
                        INTEL_CLRBIT(intel_reg32(INTEL_SWSM), INTEL_SWSM_SWESMBI);
                    }
                    else
                    {
                        success = LSA_FALSE;
                        /* 4.) release ownership of the software/firmware semaphore SWSM.SWESMBI */
                        INTEL_CLRBIT(intel_reg32(INTEL_SWSM), INTEL_SWSM_SWESMBI);
                        EDDS_WAIT_US(1);    /* wait for firmware to finish phy-access */
                    }
                }
            }
            break;

            case IG82574L:
            {
                /* 1.) Get ownership of the MDIO/NVM semaphore registers */
                EDDS_DO_UNTIL_TIMEOUT(INTEL_SETBIT(intel_reg32(INTEL_EXTCNF_CTRL),INTEL_EXTCNF_CTRL_MDIO_SW_OWNERSHIP),
                       0 != INTEL_GETBIT(INTEL_EXTCNF_CTRL_MDIO_SW_OWNERSHIP,intel_reg32(INTEL_EXTCNF_CTRL)), 20, success = LSA_FALSE;);
            }
            break;

            default:
                /* no semaphore mechanism */
                break;
        }
        --retries;
    } while ((LSA_FALSE == success) && (0 < retries));

    return success;
}

EDDS_STATIC LSA_BOOL intel_release_semaphore(INTEL_LL_HANDLE_TYPE* const handle, LSA_UINT8 semaphore)
{
    LSA_UINT32 sw_fw_sync;
    LSA_BOOL success = !(LSA_FALSE);

    switch(handle->params.DeviceID)
    {
        case I210:
        {
            /* 1.) Get ownership of the software/firmware semaphore SWSM.SWESMBI */
            EDDS_DO_UNTIL_TIMEOUT(INTEL_SETBIT(intel_reg32(INTEL_SWSM),INTEL_SWSM_SWESMBI), 0 != INTEL_GETBIT(INTEL_SWSM_SWESMBI,intel_reg32(INTEL_SWSM)),
                    20, success = LSA_FALSE;);

            if (LSA_FALSE != success)
            {
                sw_fw_sync = intel_reg32(INTEL_SW_FW_SYNC);
                /* 2. clear SW semaphore (don't touch FW semaphore) */
                INTEL_CLRBIT(sw_fw_sync, semaphore + 0);
                intel_reg32(INTEL_SW_FW_SYNC) = sw_fw_sync;
                /* 3.) release ownership of the software/firmware semaphore SWSM.SWESMBI */
                INTEL_CLRBIT(intel_reg32(INTEL_SWSM), INTEL_SWSM_SWESMBI);
            }
        }
        break;

        case IG82574L:
        {
            /* 1.) Release ownership of the MDIO/NVM semaphore registers */
            INTEL_CLRBIT(intel_reg32(INTEL_EXTCNF_CTRL), INTEL_EXTCNF_CTRL_MDIO_SW_OWNERSHIP);
        }
        break;

        default:
            /* no semaphore mechanism */
            break;
    }

    return success;
}


//lint -esym(438,readOK,writeOK)
//lint -esym(550,readOK,writeOK)
//JB 28/11/2014 why only assigned them atm - we will changes this within NEA6.0
EDDS_STATIC LSA_RESULT intel_set_link(INTEL_LL_HANDLE_TYPE* const handle, LSA_UINT8 LinkStat,
		LSA_UINT8 PHYPower)
{
	LSA_RESULT Status;
	LSA_BOOL readOK, writeOK;
	LSA_UINT16 phyreg;
	LSA_BOOL autoneg;
	LSA_UINT32 ctrl;

	if (EDD_LINK_UNCHANGED != LinkStat)
	{
		handle->savedLinkStat = LinkStat;
	}
	if (EDDS_PHY_POWER_UNCHANGED != PHYPower)
	{
		handle->savedPHYPower = PHYPower;
	}

	Status = EDD_STS_OK;

	if(0 == handle->resetState)
	{
	    /* Acquire ownership of phy */
	    if (LSA_FALSE == intel_acquire_semaphore(handle, INTEL_SW_FW_SYNC_SW_PHY_SM))
	    {
	        Status = EDD_STS_ERR_HW;
	    }
	    else
	    {

            readOK = intel_readPHYReg(handle, INTEL_PHY_PCTRL, &phyreg);
            //if(!readOK)
            //	return EDD_STS_ERR_PARAM;  //@fixme: we do not accept this code untested - NEA6.0
            autoneg = INTEL_GETBITI(12, phyreg);
            ctrl = intel_reg32(INTEL_CTRL);
            //ctrl_ext = intel_reg32(INTEL_CTRL_EXT);
            if (EDD_LINK_UNCHANGED != LinkStat)
            {
                /* save current phy configuration */
                handle->LinkSpeedModeConfigured = LinkStat;

                /* smart power down off etc.*/

                INTEL_CLRBIT(phyreg, 6); /* speed selection LSB (1== Gigabit); clear it */
                INTEL_CLRBIT(phyreg, 13); /* speed selection MSB clear it */
                INTEL_CLRBIT(phyreg, 8); /* duplex mode; clear it */
                INTEL_CLRBIT(phyreg, 9); /* restart autoneg */

                if (EDD_LINK_AUTONEG != LinkStat)
                {
                    INTEL_CLRBIT(phyreg, 12); /* autoneg off */
                    /* update MAC ctrl register */
                    INTEL_SETBIT(ctrl, INTEL_CTRL_FRCSPD);
                    INTEL_SETBIT(ctrl, INTEL_CTRL_FRCDPLX);
                    //INTEL_CLRBIT(ctrl,INTEL_CTRL_ASDE);
                    INTEL_CLRBIT(ctrl, INTEL_CTRL_FD);
                    INTEL_CLRBITS(ctrl, INTEL_CTRL_SPEED);
                    //INTEL_SETBIT(ctrl_ext,INTEL_CTRL_EXT_SPD_BYPS);
                }
                switch (LinkStat)
                {
                case EDD_LINK_AUTONEG:
                    //INTEL_SETBIT(ctrl,INTEL_CTRL_ASDE);
                    INTEL_CLRBIT(ctrl, INTEL_CTRL_FRCSPD);
                    INTEL_CLRBIT(ctrl, INTEL_CTRL_FRCDPLX);
                    INTEL_SETBIT(phyreg, 12); /* autoneg on */
                    if (0 == autoneg)
                    {
                        INTEL_SETBIT(phyreg, 9); /* restart autoneg */
                    }
                    //INTEL_CLRBIT(ctrl_ext,INTEL_CTRL_EXT_SPD_BYPS);
                    //INTEL_SETBIT(intel_reg32(INTEL_PHPM),INTEL_PHPM_Disable_1000);

                    break;
                case EDD_LINK_100MBIT_HALF:
                    INTEL_SETBIT(phyreg, 13); /* speed selection 6,13 = 01 = 100Mbps */
                    /* update MAC CTRL register */
                    INTEL_SETBITS(ctrl, INTEL_CTRL_SPEED, 0x1);
                    break;
                case EDD_LINK_100MBIT_FULL:
                    INTEL_SETBIT(phyreg, 13); /* speed selection 6,13 = 01 = 100Mbps */
                    INTEL_SETBIT(phyreg, 8); /* full duplex */
                    /* update MAC CTRL register */
                    INTEL_SETBIT(ctrl, INTEL_CTRL_FD);
                    INTEL_SETBITS(ctrl, INTEL_CTRL_SPEED, 0x1);
                    break;
                case EDD_LINK_10MBIT_HALF:
                    /* update MAC CTRL register */
                    INTEL_SETBITS(ctrl, INTEL_CTRL_SPEED, 0x0);
                    break;
                case EDD_LINK_10MBIT_FULL:
                    INTEL_SETBIT(phyreg, 8); /* full duplex */
                    /* update MAC CTRL register */
                    INTEL_SETBIT(ctrl, INTEL_CTRL_FD);
                    INTEL_SETBITS(ctrl, INTEL_CTRL_SPEED, 0x0);
                    break;
                case EDD_LINK_1GBIT_HALF:
                    INTEL_SETBIT(phyreg, 6); /* speed selection 6,13 = 10 = 1Gbps */
                    /* update MAC CTRL register */
                    INTEL_SETBITS(ctrl, INTEL_CTRL_SPEED, 0x2);
                    //INTEL_CLRBIT(intel_reg32(INTEL_PHPM),INTEL_PHPM_Disable_1000);
                    break;
                case EDD_LINK_1GBIT_FULL:
                    INTEL_SETBIT(phyreg, 6); /* speed selection 6,13 = 10 = 1Gbps */
                    INTEL_SETBIT(phyreg, 8); /* full duplex */
                    /* update MAC CTRL register */
                    INTEL_SETBIT(ctrl, INTEL_CTRL_FD);
                    INTEL_SETBITS(ctrl, INTEL_CTRL_SPEED, 0x2);
                    //INTEL_CLRBIT(intel_reg32(INTEL_PHPM),INTEL_PHPM_Disable_1000);
                    break;
                case EDD_LINK_10GBIT_HALF:
                case EDD_LINK_10GBIT_FULL:
                    Status = EDD_STS_ERR_PARAM;
                    break;
                default:
                    Status = EDD_STS_ERR_PARAM;
                }
            }
            switch (PHYPower)
            {
            case EDDS_PHY_POWER_ON:
                INTEL_CLRBIT(phyreg, 11);
                //INTEL_SETBIT(ctrl,INTEL_CTRL_SLU);
                break;
            case EDDS_PHY_POWER_OFF:
                INTEL_SETBIT(phyreg, 11);
                //INTEL_CLRBIT(ctrl,INTEL_CTRL_SLU);           /* Set LinkDown */
                break;
            case EDDS_PHY_POWER_UNCHANGED:
                break;
            default:
                Status = EDD_STS_ERR_PARAM;
            }
            if (EDD_STS_OK == Status)
            {
                intel_reg32(INTEL_CTRL) = ctrl; //lint !e845 JB 14/11/2014 #def = 0 on purpose

                /* force pci posted write to finish. This is automatically done in intel_write_PHYReg. */
                writeOK = intel_writePHYReg(handle, INTEL_PHY_PCTRL, phyreg);
                //if(!writeOK)
                //	Status = EDD_STS_ERR_PARAM; //@fixme: we do not accept this code untested NEA6.0
                //else
                //    Status = EDD_STS_OK;
            }

            /* Release ownership of phy, regardless of Status */
            if (LSA_FALSE == intel_release_semaphore(handle, INTEL_SW_FW_SYNC_SW_PHY_SM))
            {
                Status = EDD_STS_ERR_HW;
            }
        }
	}
	return Status;
}
//lint +esym(550,readOK,writeOK)
//lint +esym(438,readOK,writeOK)

/** save statistic counters.
 * This function accumulates the statistic counters from hardware to the
 * software shadow counter, because hardware registers are set to zero after read.
 */
EDDS_STATIC LSA_VOID intel_save_statistic_counter(INTEL_LL_HANDLE_TYPE* const handle)
{
	LSA_UINT32 count;
	/* Here: only Port1 available on Intel => Port0 == Port1 */
	for (count = 0; count < 0x40; ++count)
	{
		/* get all raw statistic counter */
		{ /* @note: 64bit counter should be correct handled in future. */
			LSA_UINT32 crcerr = INTEL_CRCERRS+(count*4);
			handle->PortStats[count] += intel_reg32(crcerr);
		}
	}
}


/** Save volatile hardware registers.
 * This function saves ethernet controller registers that are modified directly
 * by hardware during normal operation.
 */
EDDS_STATIC LSA_VOID intel_save_volatile_registers(INTEL_LL_HANDLE_TYPE* const handle)
{
	intel_save_statistic_counter(handle);
	handle->lastRXHead = intel_reg32(INTEL_RDH);
	//EDDS_ASSERT(handle->lastRXHead == handle->RXDescriptorIndex);
}

/** setup all registers after a reset.
 *
 * This function is called after a reset to setup all
 * used registers.
 */
EDDS_STATIC LSA_RESULT intel_setup_registers(INTEL_LL_HANDLE_TYPE* const handle)
{
	LSA_RESULT Status;
	LSA_UINT32 c;
	//LSA_UINT32 ctrl;
    //enum INTEL_DEVICE_ID deviceID;



    //deviceID = (enum INTEL_DEVICE_ID) (handle->params.DeviceID);

	Status = EDD_STS_OK;

	handle->TXDescriptorsUsed = 0;
	handle->RXDescriptorIndex = 0;
	handle->TXDescriptorIndex = 0;
    handle->TXDescriptorIndexEnd = 0;
    /* make all RDESC available (one space descriptor left) */
    handle->RXDescriptorIndexEnd = handle->RxFrameBufferCount -1;

	intel_reg32(INTEL_LEDCTL) = handle->savedLEDCTL;

	//if (EDD_STS_OK == Status) //no need to check
	{
		//LSA_UINT32 count;
		LSA_UINT64 physicalAddr;

		/* set mac address */
		{

			if((0 != handle->InterfaceMAC.MacAdr[0])||(0 != handle->InterfaceMAC.MacAdr[1])||(0 != handle->InterfaceMAC.MacAdr[2])||(0 != handle->InterfaceMAC.MacAdr[3])||(0 != handle->InterfaceMAC.MacAdr[4])||(0 != handle->InterfaceMAC.MacAdr[5]))
			{
	        	intel_reg32(INTEL_RAL_0_15(0)) =
	        			(((LSA_UINT32)handle->InterfaceMAC.MacAdr[3])<<24) | (((LSA_UINT32)handle->InterfaceMAC.MacAdr[2])<<16) |
	        			(((LSA_UINT32)handle->InterfaceMAC.MacAdr[1])<<8) | (((LSA_UINT32)handle->InterfaceMAC.MacAdr[0])/*<<0*/)
						;  //lint !e845 JB 12/11/2014 null within INTEL_RAL_0_15 on purpose
	        	intel_reg32(INTEL_RAH_0_15(0)) =
	            		INTEL_BIT(INTEL_RAH_AV,1) | /* enable MAC address */
	        			(((LSA_UINT32)handle->InterfaceMAC.MacAdr[5])<<8) | (((LSA_UINT32)handle->InterfaceMAC.MacAdr[4])/*<<0*/)
						;  //lint !e845 JB 12/11/2014 null within INTEL_RAH_0_15 on purpose
			}else /* write eeprom MAC address back to setup structure */
			{
				LSA_UINT32 tmp;
	        	tmp = intel_reg32(INTEL_RAH_0_15(0));  //lint !e845 JB 12/11/2014 null within INTEL_RAH_0_15 on purpose
	        	if (0 != INTEL_GETBIT(INTEL_RAH_AV,tmp) )		/* interface mac is present in first receive address (RAH/L[0])... */  //1 would be int, not unsiged int
	        	{
	        		handle->InterfaceMAC.MacAdr[4] = (tmp /*>> 0*/) & 0xFF;
	        		handle->InterfaceMAC.MacAdr[5] = (tmp >> 8) & 0xFF;
	        		tmp = intel_reg32(INTEL_RAL_0_15(0)); //lint !e845 JB 12/11/2014 null within INTEL_RAL_0_15 on purpose
	        		handle->InterfaceMAC.MacAdr[0] = (tmp /*>> 0*/) & 0xFF;
	        		handle->InterfaceMAC.MacAdr[1] = (tmp >> 8) & 0xFF;
	        		handle->InterfaceMAC.MacAdr[2] = (tmp >> 16) & 0xFF;
	        		handle->InterfaceMAC.MacAdr[3] = (tmp >> 24) & 0xFF;
	        	}else /* no interface mac address stored in eeprom, no mac address from DSB...*/
	        	{
	        		EDDS_MEMSET_UPPER(handle->InterfaceMAC.MacAdr,0,sizeof(handle->InterfaceMAC.MacAdr));
	        	}
			}
		}

		for(c=1;c<16;++c)	/* first receive address is interface MAC, skip it */
		{
			intel_reg32(INTEL_RAL_0_15(c)) = handle->MCRegL[c];
			intel_reg32(INTEL_RAH_0_15(c)) = handle->MCRegH[c];
		}

	    for (c = 0; c < 128; ++c) /* disable Multicast Table Array & VLAN Filter Table Array */
	    {
	        intel_reg32(INTEL_MTA_0_127(c)) = 0;
	        intel_reg32(INTEL_VFTA_0_127(c)) = 0;
	    }

		/* setup Transmit Descriptor Ring Structure */
		EDDS_PTR_TO_ADDR64_UPPER(handle->hSysDev,handle->pTxDescriptors, &physicalAddr);
		intel_reg64(INTEL_TDBAL) = physicalAddr;
		intel_reg32(INTEL_TDLEN) = handle->TxFrameBufferCount
				* sizeof(*(handle->pTxDescriptors));

		/* setup Receive Descriptor Ring Structure */
		EDDS_PTR_TO_ADDR64_UPPER(handle->hSysDev,handle->pRxDescriptors, &physicalAddr);
		intel_reg64(INTEL_RDBAL) = physicalAddr;
		intel_reg32(INTEL_RDLEN) = handle->RxFrameBufferCount
				* sizeof(*(handle->pRxDescriptors));

		/* first setup empty; all values should be zero */
		//EDDS_ASSERT(0 == handle->TXDescriptorIndex);
		//EDDS_ASSERT(0 == handle->TXDescriptorIndexEnd);
		//EDDS_ASSERT(0 == handle->RXDescriptorIndex);
		//EDDS_ASSERT(0 == handle->RXDescriptorIndexEnd);

        intel_reg32(INTEL_TDH) = handle->TXDescriptorIndex;
        intel_reg32(INTEL_TDT) = handle->TXDescriptorIndexEnd;
        intel_reg32(INTEL_RDH) = handle->RXDescriptorIndex;
        intel_reg32(INTEL_RDT) = handle->RXDescriptorIndex;		/* first setup: make tail same as head */

        /* buffer size is 2048 after reset (@see RCTL.BSIZE + RCTL.BSEX) */
        {
			LSA_UINT32 val;

			//val = INTEL_BITS(INTEL_RXDCTL_WTHRESH,0x0) | INTEL_BIT(INTEL_RXDCTL_ENABLE,1);
			//intel_reg32(INTEL_RXDCTL) = val;
			INTEL_SETBIT(intel_reg32(INTEL_RXDCTL),INTEL_RXDCTL_ENABLE);

			EDDS_WAIT_UNTIL_TIMEOUT(1 == INTEL_GETBIT(INTEL_RXDCTL_ENABLE,intel_reg32(INTEL_RXDCTL)),
					10, Status = EDD_STS_ERR_HW;)

			if(EDD_STS_OK == Status)
			{

              /* we use Software workaround for "send on LinkDown" */
				intel_reg32(INTEL_RCTL) =
				    INTEL_BIT(INTEL_RCTL_EN,1)			        /* enable receive unit */
						|	INTEL_BIT(INTEL_RCTL_UPE,0)			/* unicast promiscuous disable */
						|	INTEL_BIT(INTEL_RCTL_MPE,0)			/* multicast promiscuous disable */
						|	INTEL_BIT(INTEL_RCTL_BAM,1)			/* broadcast accept mode */
						|   INTEL_BIT(INTEL_RCTL_SECRC,1)   	/* strip ethernet crc from received packets */
						;

				INTEL_SETBIT(intel_reg32(INTEL_TXDCTL),INTEL_TXDCTL_ENABLE);

				EDDS_WAIT_UNTIL_TIMEOUT(1 == INTEL_GETBIT(INTEL_TXDCTL_ENABLE,intel_reg32(INTEL_TXDCTL)),
						10, Status = EDD_STS_ERR_HW;)

				if(EDD_STS_OK == Status)
				{

					val = intel_reg32(INTEL_TCTL);
					INTEL_SETBIT(val,INTEL_TCTL_EN);		/* enable transmit unit */
					INTEL_SETBIT(val,INTEL_TCTL_PSP);		/* pad short packets */
					INTEL_SETBITS(val,INTEL_TCTL_CT,0xF);		/* back-off algorythm: collision threshold 0fh */
					INTEL_SETBITS(val,INTEL_TCTL_COLD,0x40);	/* collision distance: 64 byte time */
														/**
														 *     0x200 (512 byte time) for half-duplex
														 *     0x40  (64 byte time) for full -duplex
														 */
					intel_reg32(INTEL_TCTL) = val;
				}
			}
		}

        /* second setup: now write to receive tail to make RDESC available for receive */
        intel_reg32(INTEL_RDT) = handle->RXDescriptorIndexEnd;

	}

    /* interrupt throttling, interval is in 256ns increments */
	intel_reg32(INTEL_ITR) = (INTEL_CFG_INTERRUPT_THROTTLING_NANOSECONDS / 256) & 0x0000FFFF;  //lint !e835 !e845 #def 0, define specific behaviour

    #if ! (defined (EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD) || defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE))
	intel_reg32(INTEL_IMS) =
	    INTEL_BIT(INTEL_INTERRUPT_LSC ,1) 	/* enable link status change */
			|	handle->transmitInterruptBitMask
			|	handle->receiveInterruptBitMask	/* enable receive */
			;
    #endif

	intel_reg32(INTEL_ICR) = 0xFFFFFFFF; /* acknowledge all pending interrupts :
										  * a read acknowledges all pending interrupts
										  * writing a 1 to a bit clears the pending interrupt
										  */

	INTEL_SETBIT(intel_reg32(INTEL_CTRL_EXT), INTEL_CTRL_EXT_DRV_LOAD); /* set "driver loaded" indication bit */

	return (Status);
}


EDDS_STATIC LSA_VOID intel_reset(INTEL_LL_HANDLE_TYPE* const handle, LSA_BOOL deviceReset)
{
	LSA_UINT32 ctrl;

	intel_reg32(INTEL_IMC) = 0xFFFFFFFF; /* disable all interrupts */
    intel_reg32(INTEL_ICR) = 0xFFFFFFFF; /* clear all interrupts */

	ctrl = intel_reg32(INTEL_CTRL);
	INTEL_SETBIT(ctrl, INTEL_CTRL_RST);
	INTEL_SETBIT(ctrl, INTEL_CTRL_LRST);
	if(LSA_FALSE == deviceReset)
	{
		INTEL_CLRBIT(ctrl, INTEL_CTRL_DEV_RST);
	}else
	{
		INTEL_SETBIT(ctrl, INTEL_CTRL_DEV_RST);
	}

	intel_reg32(INTEL_CTRL) = ctrl; //lint !e845 JB 14/11/2014 #def = 0 on purpose
}


EDDS_STATIC LSA_RESULT intel_reset_finish(INTEL_LL_HANDLE_TYPE* const handle)
{
    LSA_RESULT Status = EDD_STS_OK;
    LSA_UINT32 ctrl;

    EDDS_WAIT_UNTIL_TIMEOUT(
            0 == INTEL_GETBIT(INTEL_CTRL_RST,intel_reg32(INTEL_CTRL)), 10,
            Status = EDD_STS_ERR_HW;) //lint !e845 JB 14/11/2014 #def = 0 on purpose
    if (EDD_STS_OK == Status)
    {
        if(I210 == handle->params.DeviceID)
        {
        EDDS_WAIT_UNTIL_TIMEOUT(
                1 == INTEL_GETBIT(INTEL_STATUS_PF_RST_DONE,intel_reg32(INTEL_STATUS)),
                10, Status = EDD_STS_ERR_HW;)
        }
        if (EDD_STS_OK == Status)
        {
            EDDS_WAIT_UNTIL_TIMEOUT(
                    1 == INTEL_GETBIT(INTEL_EDCD_AUTO_RD,intel_reg32(INTEL_EECD)),
                    10, Status = EDD_STS_ERR_HW;)
        }
    }

    intel_reg32(INTEL_IMC) = 0xFFFFFFFF; /* disable all interrupts again */
    ctrl = intel_reg32(INTEL_CTRL);
    INTEL_CLRBIT(ctrl,INTEL_CTRL_ILOS);
    INTEL_SETBIT(ctrl,INTEL_CTRL_SLU);
    INTEL_CLRBIT(ctrl,INTEL_CTRL_RFCE);
    INTEL_CLRBIT(ctrl,INTEL_CTRL_TFCE);
    INTEL_CLRBIT(ctrl,INTEL_CTRL_FRCSPD);
    INTEL_CLRBIT(ctrl,INTEL_CTRL_FRCDPLX);
    intel_reg32(INTEL_CTRL) = ctrl;

    return Status;
}

//lint -esym(550,readOK,writeOK)
//lint -esym(438,readOK,writeOK)
//JB 01/12/2014 this will be fixed within NEA6.0 - we need to check our fix
EDDS_STATIC LSA_RESULT intel_reset_phy(INTEL_LL_HANDLE_TYPE* const handle)
{
	LSA_RESULT Status;
	LSA_UINT32 ctrl;

	Status = EDD_STS_OK;

    /* Acquire ownership of phy */
    if (LSA_FALSE == intel_acquire_semaphore(handle, INTEL_SW_FW_SYNC_SW_PHY_SM))
    {
        return EDD_STS_ERR_HW; /* it's ok to return directly here on an error */
    }

	/* Reset PHY.
	 * @see CTRL register description, PHY_RST bit
	 * 8254x_GBe_SDM.pdf (317453006EN.PDF), Table 13-3.
	 *
	 * The technique is to set the bit, wait approximately 3us, then
	 * clear the bit.
	 *
	 * @note For the 82546GB, when resetting the PHY through the
	 * MAC, the PHY should be held in reset for a minimum of 10 ms
	 * before releasing the reset signal.
	 * Actually we will not support the 82546GB; to support it,
	 * we should finish the phy reset in setup, after a minimum time period
	 * of 10 ms between set/unset of PHY_RST bit.
	 */
	/**@FIXME SPRINGVILLE 4.3.1.4 PHY Reset */
	ctrl = intel_reg32(INTEL_CTRL); //lint !e845 JB 14/11/2014 #def = 0 on purpose
	INTEL_SETBIT(ctrl, INTEL_CTRL_PHY_RST);
    INTEL_CLRBIT(intel_reg32(INTEL_CTRL_EXT),INTEL_CTRL_EXT_SPD_BYPS);
	intel_reg32(INTEL_CTRL) = ctrl; //lint !e845 JB 14/11/2014 #def = 0 on purpose
	EDDS_WAIT_US(300);
	INTEL_CLRBIT(ctrl, INTEL_CTRL_PHY_RST);
	intel_reg32(INTEL_CTRL) = ctrl;  //lint !e845 JB 14/11/2014 #def = 0 on purpose
	EDDS_WAIT_US(300);

	/* Release ownership of phy, regardless of Status */
    if (LSA_FALSE == intel_release_semaphore(handle, INTEL_SW_FW_SYNC_SW_PHY_SM))
    {
        return EDD_STS_ERR_HW; /* it's ok to return directly here on an error */
    }

    if(I210 == handle->params.DeviceID)
    {
        /* it's ok to return directly here on an error */
        EDDS_WAIT_UNTIL_TIMEOUT(INTEL_GETBIT(INTEL_EEMNGCTL_CFG_DONE,intel_reg32(INTEL_EEMNGCTL)), 600, return EDD_STS_ERR_HW);
    }

    /* Acquire ownership of phy */
    if (LSA_FALSE == intel_acquire_semaphore(handle, INTEL_SW_FW_SYNC_SW_PHY_SM))
    {
        return EDD_STS_ERR_HW; /* it's ok to return directly here on an error */
    }

	/* setup phy register */
	{
	    LSA_BOOL readOK;
		LSA_UINT16 phyreg;

		// ---------------- GBit support is disabled ----------------
        if(LSA_FALSE != handle->params.Disable1000MBitSupport)
        {
            // ---------------- GBit shall not be advertised on AutoNeg ----------------
            EDDS_WAIT_UNTIL_TIMEOUT(LSA_FALSE != intel_readPHYReg(handle,INTEL_PHY_GCON,&phyreg),1000,Status=EDD_STS_ERR_HW;);
            if(EDD_STS_OK == Status)
            {
                INTEL_CLRBIT(phyreg,INTEL_PHY_GCON_1000BASETHD);
                INTEL_CLRBIT(phyreg,INTEL_PHY_GCON_1000BASETFD);

                if(LSA_FALSE == intel_writePHYReg(handle,INTEL_PHY_GCON,phyreg))
                {
                    Status = EDD_STS_ERR_HW;
                }
            }
        }

        // ---------------- GBit support is not disabled ----------------
        if(LSA_FALSE == handle->params.Disable1000MBitSupport)
        {
            // ---------------- enable down shift ----------------
            // SmartSpeed is an enhancement to auto-negotiation that enables the PHY to react intelligently to
            // network conditions that prohibit establishment of a 1000BASE-T link, such as cable problems. Such
            // problems might allow auto-negotiation to complete, but then inhibit completion of the training phase.
            // Normally, if a 1000BASE-T link fails, the PHY returns to the auto-negotiation state with the same speed
            // settings indefinitely. With SmartSpeed enabled by setting the Downshift Enable field, after a
            // configurable number of failed attempts, as configured in the Downshift counter field the PHY
            // automatically downgrades the highest ability it advertises to the next lower speed: from 1000 to
            // 100 to 10 Mb/s.
            EDDS_WAIT_UNTIL_TIMEOUT(LSA_FALSE != intel_readPHYReg(handle,INTEL_PHY_PSCON,&phyreg),1000,Status=EDD_STS_ERR_HW;);
            if(EDD_STS_OK == Status)
            {
                // enable down shift and set counter to 1 --> 2 attempts
                INTEL_SETBITI(phyreg,INTEL_PHY_PSCON_DOWNSHIFT_ENABLE);
                INTEL_SETBITSI(phyreg, INTEL_PHY_PSCON_DOWNSHIFT_COUNTER_s, INTEL_PHY_PSCON_DOWNSHIFT_COUNTER_e, 1);

                if(LSA_FALSE == intel_writePHYReg(handle,INTEL_PHY_PSCON,phyreg))
                {
                    Status = EDD_STS_ERR_HW;
                }
            }
        }

        // ---------------- set phy power off ----------------
		EDDS_WAIT_UNTIL_TIMEOUT(LSA_FALSE != intel_readPHYReg(handle,INTEL_PHY_PCTRL,&phyreg),1000,Status=EDD_STS_ERR_HW;);
        if(EDD_STS_OK == Status)
        {
            INTEL_SETBITI(phyreg,11);
            if(LSA_FALSE == intel_writePHYReg(handle,INTEL_PHY_PCTRL,phyreg))
            {
                Status = EDD_STS_ERR_HW;
            }
        }

        // ---------------- get advertised autoneg capability ----------------
        readOK = intel_readPHYReg(handle, INTEL_PHY_ANA,&phyreg);
        if (1 != INTEL_GETBITS(INTEL_PHY_ANA_Selector,phyreg))
        {
            handle->AutoNegCapAdvertised = EDD_AUTONEG_CAP_OTHER;
        }
        else
        {
            handle->AutoNegCapAdvertised = 0;
            handle->AutoNegCapAdvertised |=
                INTEL_GETBIT(INTEL_PHY_ANA_10BASETXHD,phyreg) ?
                            EDD_AUTONEG_CAP_10BASET : 0;
            handle->AutoNegCapAdvertised |=
                INTEL_GETBIT(INTEL_PHY_ANA_10BASETXFD,phyreg) ?
                            EDD_AUTONEG_CAP_10BASETFD : 0;
            handle->AutoNegCapAdvertised |=
                INTEL_GETBIT(INTEL_PHY_ANA_100BASETXHD,phyreg) ?
                            EDD_AUTONEG_CAP_100BASETX : 0;
            handle->AutoNegCapAdvertised |=
                INTEL_GETBIT(INTEL_PHY_ANA_100BASETXFD,phyreg) ?
                            EDD_AUTONEG_CAP_100BASETXFD : 0;

            if(LSA_FALSE == handle->params.Disable1000MBitSupport)
            {
                readOK = intel_readPHYReg(handle, INTEL_PHY_GCON,&phyreg);
				//if(!readOK)
				//	Status = EDD_STS_ERR_PARAM;	//@fixme: we do not accept this code untested - NEA6.0
                if (0xFFFF != phyreg) {
                    handle->AutoNegCapAdvertised |=
                            INTEL_GETBIT(INTEL_PHY_GCON_1000BASETHD,phyreg) ?
                                    EDD_AUTONEG_CAP_1000BASET : 0;
                    handle->AutoNegCapAdvertised |=
                            INTEL_GETBIT(INTEL_PHY_GCON_1000BASETFD,phyreg) ?
                                    EDD_AUTONEG_CAP_1000BASETFD : 0;
                }
            }
        }
	}

    /* NOTE: only for I210 */
    if(I210 == handle->params.DeviceID)
    {
        LSA_UINT32 ipcnfg;
        LSA_UINT32 phpm;

        /* disable EEE --> enabled on default */
        ipcnfg = intel_reg32(INTEL_IPCNFG);
        INTEL_CLRBIT(ipcnfg,INTEL_IPCNFG_EEE_100M_AN);
        INTEL_CLRBIT(ipcnfg,INTEL_IPCNFG_EEE_1G_AN);
        intel_reg32(INTEL_IPCNFG) = ipcnfg;

        /* disable SPD (smart power down) --> enabled on default */
        /* disable Go Link disconnect --> might be set in NVM to start without link */
        phpm = intel_reg32(INTEL_PHPM);
        INTEL_CLRBIT(phpm,INTEL_PHPM_SPD_EN);
        INTEL_CLRBIT(phpm,INTEL_PHPM_GO_LINK_DISCONNECT);
        intel_reg32(INTEL_PHPM) = phpm;
    }

    /* Release ownership of phy, regardless of Status */
    if (LSA_FALSE == intel_release_semaphore(handle, INTEL_SW_FW_SYNC_SW_PHY_SM))
    {
        Status = EDD_STS_ERR_HW;
    }

    return Status;
}
//lint +esym(550,readOK,writeOK)
//lint +esym(438,readOK,writeOK)

//lint -esym(613,intel_rdesc_done)
//JB 05/12/2014 we will check this within a MACRO
EDDS_STATIC LSA_BOOL intel_rdesc_done(INTEL_RDESC* const currentRD)
{
	return (1 == INTEL_GETBITI(INTEL_RDESC_STATUS_DD, currentRD->Status))?LSA_TRUE:LSA_FALSE;
}
//lint +esym(613,intel_rdesc_done)

// helper function - allocates aligned memory, depending on deviceID
EDDS_STATIC LSA_VOID intel_alloc_TxRxDescr( INTEL_LL_HANDLE_TYPE* const handle )
{
  LSA_UINT16 deviceID;
  LSA_UINT32 alignment; // valid 2,4,8,16,32,64,128 ...
  LSA_UINT64 physicalAddrTx;
  LSA_UINT64 physicalAddrRx;
  LSA_UINT64 offsetTx;
  LSA_UINT64 offsetRx;

  deviceID = handle->params.DeviceID;

  // get alignment depending on deviceID
  switch ((enum INTEL_DEVICE_ID) (deviceID))
  {
    case I210:
      alignment = 128;
      break;
    case IG82574L:
      alignment = 16;
      break;
    default:
      // can not happen, is checked in open before --> sanity
      alignment = 16;
      break;
  }

  // allocate descriptor for Rx and Tx
  EDDS_ALLOC_DEV_BUFFER_MEM( handle->hSysDev,
                             (EDD_UPPER_MEM_PTR_TYPE*)&handle->pTxDescrAlloc,
                             (handle->TxFrameBufferCount*sizeof(INTEL_TDESC) + alignment));
  EDDS_ALLOC_DEV_BUFFER_MEM( handle->hSysDev,
                             (EDD_UPPER_MEM_PTR_TYPE*)&handle->pRxDescrAlloc,
                             (handle->RxFrameBufferCount*sizeof(INTEL_RDESC) + alignment));

  // get physical address as LSA_UINT64 from virtual address
  EDDS_PTR_TO_ADDR64_UPPER( handle->hSysDev, handle->pTxDescrAlloc, &physicalAddrTx );
  EDDS_PTR_TO_ADDR64_UPPER( handle->hSysDev, handle->pRxDescrAlloc, &physicalAddrRx );

  // calculate offset to aligned address with physical address
  offsetTx = ((physicalAddrTx + (alignment-1)) & ~(alignment-1)) - physicalAddrTx;
  offsetRx = ((physicalAddrRx + (alignment-1)) & ~(alignment-1)) - physicalAddrRx;

  // add offset to virtual address
  handle->pTxDescriptors = (EDD_UPPER_MEM_PTR_TYPE) ( ((LSA_UINT8*)handle->pTxDescrAlloc) + offsetTx);
  handle->pRxDescriptors = (EDD_UPPER_MEM_PTR_TYPE) ( ((LSA_UINT8*)handle->pRxDescrAlloc) + offsetRx);
}

// helper function - free's aligned memory
EDDS_STATIC LSA_RESULT intel_free_TxRxDescr( INTEL_LL_HANDLE_TYPE* const handle )
{
  LSA_RESULT Status = EDD_STS_OK;

  // it is assured that descriptor memory for rx and tx is allocated, so it is not necessary to check it here
  EDDS_FREE_DEV_BUFFER_MEM( handle->hSysDev,
                            &Status,
                            (EDD_UPPER_MEM_PTR_TYPE)handle->pTxDescrAlloc );

  if( EDD_STS_OK == Status )
    EDDS_FREE_DEV_BUFFER_MEM( handle->hSysDev,
                              &Status,
                              (EDD_UPPER_MEM_PTR_TYPE)handle->pRxDescrAlloc );

  return Status;
}



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_OPEN                               +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             EDDS_HANDLE                   hDDB          +*/
/*+                             EDDS_UPPER_DPB_PTR_TYPE       pDPB          +*/
/*+                             LSA_UINT32                    TraceIdx      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  hDDB         : Handle to Device Description Block (Device-management)  +*/
/*+  pDPB         : Pointer to Device Parameter Block                       +*/
/*+  TraceIdx     : Trace index to use with this EDDS LLIF instance         +*/
/*+                                                                         +*/
/*+  Result       :         EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_PARAM                               +*/
/*+                         EDDS_STS_ERR_RESOURCES                          +*/
/*+                         EDD_STS_ERR_ALIGN                               +*/
/*+                         others.                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Initialize LL Ethernetcontroller using DPB parameters     +*/
/*+                                                                         +*/
/*+               See DPB documentation.                                    +*/
/*+                                                                         +*/
/*+               global:                                                   +*/
/*+                                                                         +*/
/*+               pDPB->RxBufferCnt     : number of rx-buffers/descriptors  +*/
/*+               pDPB->TxBufferCnt     : number of tx-buffers/descriptors  +*/
/*+               pDPB->RxBufferSize    : rx-buffers size                   +*/
/*+               pDPB->RxBufferOffset  : rx-buffer offset                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_OPEN(LSA_VOID_PTR_TYPE pLLManagement,
		EDDS_HANDLE hDDB, EDDS_UPPER_DPB_PTR_TYPE pDPB, LSA_UINT32 TraceIdx)
{
    enum INTEL_DEVICE_ID deviceID;
	LSA_RESULT Status;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	EDDS_LL_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN :INTEL_LL_OPEN()");

	handle->params  = *((INTEL_LL_PARAMETER_TYPE*) pDPB->pLLParameter);
	handle->hDDB    = hDDB;
	handle->hSysDev = pDPB->hSysDev;

	handle->TraceIdx = TraceIdx;
	handle->pendingIRQs = 0;
	handle->isLinkUp = LSA_FALSE;
	handle->resetState = 0;
	handle->InUcastPktsShadow = 0;
	handle->OutUcastPktsShadow = 0;

	handle->fakeSendCounter = 0;

	deviceID = (enum INTEL_DEVICE_ID) (handle->params.DeviceID);

	switch (deviceID) {
	case I210:
		handle->receiveInterruptBitMask  = INTEL_BIT(INTEL_INTERRUPT_RXT0,1);
		handle->transmitInterruptBitMask = INTEL_BIT(INTEL_INTERRUPT_TXDW,1);
		
        /* save recv interrupt bit mask for overload handling */
        handle->receiveInterruptBitMaskBackup = handle->receiveInterruptBitMask;
        
		/* ok, supported */
		Status = EDD_STS_OK;
		break;
	case IG82574L:
		/* ok, supported */
		Status = EDD_STS_OK;
		handle->receiveInterruptBitMask  = INTEL_BIT(INTEL_INTERRUPT_RXT0,1) | INTEL_BIT(INTEL_INTERRUPT_RXQ0,1);
		handle->transmitInterruptBitMask = INTEL_BIT(INTEL_INTERRUPT_TXDW,1) |
                                           INTEL_BIT(INTEL_INTERRUPT_TXQE,1) |
		                                   INTEL_BIT(INTEL_INTERRUPT_TXQ0,1);
		
		/* save recv interrupt bit mask for overload handling */
		handle->receiveInterruptBitMaskBackup = handle->receiveInterruptBitMask;
		break;
	default:
		Status = EDD_STS_ERR_PARAM;
	}

	/* intel needs an receive framebuffer with minimum of 2048 byte
	 * to receive a maximum frame with 1518+4+4 (frame with vlan tag and crc)
	 */
	if(EDD_FRAME_BUFFER_LENGTH < sizeof(INTEL_RX_FRAMEBUFFER))		//lint !e774 !e506 JB 13/11/2014 defined based behaviour
		//fixme: compile time verification
	{
	  return EDD_STS_ERR_ABORT;
	}

	if (EDD_STS_OK == Status) {

	  /* only n*8 transmit/receive descriptors allowed. */
      handle->TxFrameBufferCount = (((LSA_UINT32)pDPB->TxBufferCnt)+INTEL_TDESC_ARRAY_GRANULARITY) & ~(((LSA_UINT32)INTEL_TDESC_ARRAY_GRANULARITY)-1);
      handle->RxFrameBufferCount = (((LSA_UINT32)pDPB->RxBufferCnt)+INTEL_RDESC_ARRAY_GRANULARITY) & ~(((LSA_UINT32)INTEL_RDESC_ARRAY_GRANULARITY)-1);

      /**@note intel descriptor rings need an extra "space" descriptor to work correctly!
	   * pDPB->TxBufferCnt : allocate pDPB->TxBufferCnt+1 descriptors
	   * pDPB->RxBufferCnt : allocate pDPB->RxBufferCnt+1 descriptors
	   */
		if ((INTEL_MAX_TX_DESCRIPTORS < handle->TxFrameBufferCount)
				|| (INTEL_MAX_RX_DESCRIPTORS < handle->RxFrameBufferCount)
				) {
			Status = EDD_STS_ERR_PARAM;
		} else {
			LSA_UINT32 count;

			// alloc tx and rx descriptors
			// NOTE: Receive/Transmit Descriptor Base Address (xDBAL) must point to a ...
			//       I82574 --> 16-byte
			//       I210   --> 128-byte
			//       ... aligned block of data
			intel_alloc_TxRxDescr(handle);

			for(count=0; count < handle->TxFrameBufferCount; ++count)
			{
				handle->pTxDescriptors[count].legacy.BufferAddress = 0;
				handle->pTxDescriptors[count].legacy.Length = 0;
				handle->pTxDescriptors[count].legacy.CSO = 0;
				handle->pTxDescriptors[count].legacy.CSS = 0;
				handle->pTxDescriptors[count].legacy.CMD =
				    INTEL_BITI(5,0)			/* DEXT = 0b (legacy TDESC type) */
						|	INTEL_BITI(3,1) | INTEL_BITI(1,1) | INTEL_BITI(0,1)	/* RS + IFCS + EOP */
						;
				handle->pTxDescriptors[count].legacy.STA_RSV = INTEL_BITI(0,1);	/* clear status, but set DD */
																			    /* (==ready for software, used by INTEL_LL_SEND!) */
				handle->pTxDescriptors[count].legacy.Special = 0; /* clear special */
			}

			/* allocate array of RX Framebuffer Pointers */
			EDDS_ALLOC_LOCAL_MEM((LSA_VOID**)&handle->RxFrameBufferPtr,sizeof(handle->RxFrameBufferPtr[0])*handle->RxFrameBufferCount);

			for(count=0; count < handle->RxFrameBufferCount; ++count)
			{
                EDDS_ALLOC_RX_TRANSFER_BUFFER_MEM(handle->hSysDev,(LSA_VOID**)&handle->RxFrameBufferPtr[count],sizeof(INTEL_RX_FRAMEBUFFER));
                EDDS_PTR_TO_ADDR64_UPPER(handle->hSysDev,handle->RxFrameBufferPtr[count],&handle->pRxDescriptors[count].BufferAddress);
				handle->pRxDescriptors[count].Length = 0;			/* clear received frame size */
				/** @note buffer size is 2048 after reset (@see RCTL.BSIZE + RCTL.BSEX) */
				handle->pRxDescriptors[count].packetChecksum = 0;	/* clear checksum */
				handle->pRxDescriptors[count].Status = 0;	/* clear status */
				handle->pRxDescriptors[count].Errors = 0;	/* clear errors */
				handle->pRxDescriptors[count].Special = 0;	/* clear special */
			}
			EDDS_MEMSET_UPPER(handle->PortStats,0,sizeof(handle->PortStats));
        }

	}

	EDDS_LL_TRACE_00(TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT:INTEL_LL_OPEN()");

	return (Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_SETUP                              +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             EDDS_UPPER_DSB_PTR_TYPE         pDSB        +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pDSB       : Pointer to Device Setup Block                             +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_PARAM                               +*/
/*+                         EDDS_STS_ERR_RESOURCES                          +*/
/*+                         EDD_STS_ERR_TIMEOUT                             +*/
/*+                         EDD_STS_ERR_HW                                  +*/
/*+                         others.                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Setup LL Ethernetcontroller and start it.                 +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SETUP(LSA_VOID_PTR_TYPE pLLManagement,
		EDDS_UPPER_DSB_PTR_TYPE pDSB)

{
	LSA_RESULT Status;
	LSA_UINT32 c;
	//LSA_UINT32 ctrl;
    //enum INTEL_DEVICE_ID deviceID;

	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;
    //deviceID = (enum INTEL_DEVICE_ID) (handle->params.DeviceID);

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_SETUP()");

	/* initialize saved Link State variables */
	handle->savedLinkStat = EDD_LINK_UNCHANGED;
	handle->savedPHYPower = EDDS_PHY_POWER_UNCHANGED;

	intel_reset(handle, LSA_TRUE);
	// wait 3ms before accessing the register again
	EDDS_WAIT_US(3000);

	Status = intel_reset_finish(handle);

	if(EDD_STS_OK == Status)
	{
		/* setup all port mac addresses */
		for(c=0;c < 1 /* intel port count == 1*/ ; ++c)
		{
			if((0 != pDSB->MACAddress[c+1].MacAdr[0])||(0 != pDSB->MACAddress[c+1].MacAdr[1])||(0 != pDSB->MACAddress[c+1].MacAdr[2])||(0 != pDSB->MACAddress[c+1].MacAdr[3])||(0 != pDSB->MACAddress[c+1].MacAdr[4])||(0 != pDSB->MACAddress[c+1].MacAdr[5]))
			{
				handle->PortMAC[c] = pDSB->MACAddress[c+1];
			}else /* write eeprom MAC address back to setup structure */
			{
			  EDDS_MEMSET_UPPER(pDSB->MACAddress[c+1].MacAdr,0,sizeof(pDSB->MACAddress[c+1].MacAdr));
				handle->PortMAC[c] = pDSB->MACAddress[c+1];
			}
		}

		for(c=0;c<16;++c)	/* first receive address is interface MAC, skip it */
		{
			handle->MCRegL[c] = 0;
			handle->MCRegH[c] = 0;
		}

        handle->lastRXHead = 0;
        /* THis is done in intel_setup_registers:
    	   handle->TXDescriptorsUsed = 0;
    	   handle->RXDescriptorIndex = 0;
    	   handle->TXDescriptorIndex = 0;
           handle->TXDescriptorIndexEnd = 0;
         */
        /* make all RDESC available (one space descriptor left) */
        handle->RXDescriptorIndexEnd = handle->RxFrameBufferCount -1;
        /* save LEDCTL for first time */
		handle->backupLEDCTL = intel_reg32(INTEL_LEDCTL);
		handle->savedLEDCTL = handle->backupLEDCTL;

		/* setup initial configured link speed mode */
        handle->LinkSpeedModeConfigured = EDD_LINK_AUTONEG;

		handle->InterfaceMAC = pDSB->MACAddress[0];
		Status = intel_reset_phy(handle);
		if(EDD_STS_OK == Status)
		{
			Status = intel_setup_registers(handle);
			pDSB->MACAddress[0] = handle->InterfaceMAC;
		}
	}

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_SETUP()");

	return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_SHUTDOWN                           +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_TIMEOUT                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Shutdown LL Ethernetcontroller. Stops it                  +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SHUTDOWN(
		LSA_VOID_PTR_TYPE pLLManagement) {
    LSA_UINT16 phyreg;
	LSA_RESULT Status;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_SHUTDOWN()");

	Status = EDD_STS_OK;

	intel_reg32(INTEL_IMC) = 0xFFFFFFFF; /* disable all interrupts */

	INTEL_CLRBIT(intel_reg32(INTEL_TCTL),INTEL_TCTL_EN);	/* disable transmit unit */
	INTEL_CLRBIT(intel_reg32(INTEL_RCTL),INTEL_RCTL_EN);    /* disable receive unit */

    /* Acquire ownership of phy */
    if (LSA_FALSE == intel_acquire_semaphore(handle, INTEL_SW_FW_SYNC_SW_PHY_SM))
    {
        Status = EDD_STS_ERR_HW;
    }

    if(EDD_STS_OK == Status)
    {
        /* set PHY power off */
        EDDS_WAIT_UNTIL_TIMEOUT(LSA_TRUE == intel_readPHYReg(handle,INTEL_PHY_PCTRL,&phyreg),1000,Status=EDD_STS_ERR_HW;);  //lint !e731 JB 13/11/2014 on purpose
        INTEL_SETBITI(phyreg,11);
        if(LSA_FALSE == intel_writePHYReg(handle,INTEL_PHY_PCTRL,phyreg))
        {
            Status = EDD_STS_ERR_HW;
        }

        /* Release ownership of phy, regardless of Status */
        if (LSA_FALSE == intel_release_semaphore(handle, INTEL_SW_FW_SYNC_SW_PHY_SM))
        {
            Status = EDD_STS_ERR_HW;
        }

    }

	intel_reg32(INTEL_LEDCTL) = handle->backupLEDCTL;       /* restore LEDCTL*/

	INTEL_CLRBIT(intel_reg32(INTEL_CTRL_EXT), INTEL_CTRL_EXT_DRV_LOAD); /* clear "driver loaded" indication bit */
	intel_reg32(INTEL_CTRL_EXT);	/* force pci posted write to finish. */

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_SHUTDOWN()");

	return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_CLOSE                              +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_TIMEOUT                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes LL Ethernetcontroller. Stops it                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(550,retval)
//JB 05/12/2014 system adaption - fix ouput macro
//@fixme retval redundant, fix if possible #MA_REF
LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_CLOSE(LSA_VOID_PTR_TYPE pLLManagement) {
	LSA_RESULT Status;
	LSA_UINT32 count;
	LSA_UINT16 retval;

	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_CLOSE()");

	for(count=0; count < handle->RxFrameBufferCount; ++count)
	{
	  INTEL_RX_FRAMEBUFFER* ptr = handle->RxFrameBufferPtr[count];

	  if( 0 != ptr )
	    EDDS_FREE_RX_TRANSFER_BUFFER_MEM(handle->hSysDev,&retval,ptr);  //lint !e774 JB 09/12/2014 system adaption
	}

	EDDS_FREE_LOCAL_MEM(&retval,handle->RxFrameBufferPtr);

	Status = intel_free_TxRxDescr(handle);

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_CLOSE()");

	return (Status);

}
//lint +esym(550,retval)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    INTEL_LL_RECV                                       +*/
/*+  Input/Output  :    LSA_VOID_PTR_TYPE             pLLManagement         +*/
/*+                     EDDS_UPPER_MEM_PTR EDDS_LOCAL_MEM_ATTR *pBufferAddr +*/
/*+                     EDDS_LOCAL_MEM_U32_PTR_TYPE           pLength       +*/
/*+                     EDDS_LOCAL_MEM_U32_PTR_TYPE           pPortID       +*/
/*+  Result        :    LSA_RESULT                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pBufferAddr:  Pointer to pointer to buffer with Frame-netto-data       +*/
/*+                received if EDD_STS_OK.                                  +*/
/*+  pLength    :  Pointer to address with length of frame received.        +*/
/*+  pPortID    :  Pointer to address with PortID of frame received.        +*/
/*+                                                                         +*/
/*+  Result     :  EDD_STS_OK                                               +*/
/*+                EDD_STS_OK_NO_DATA                                       +*/
/*+                EDD_STS_ERR_RX                                           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions checks if a frame was received. If not it  +*/
/*+               returns EDD_STS_OK_NO_DATA. If a frame was received, the  +*/
/*+               functions returns pBufferAddr and pLength and sets        +*/
/*+               EDD_STS_OK or EDD_STS_ERR_RX. The buffer returned must    +*/
/*+               not be accessed anymore by the LLIF! The EDDS calls       +*/
/*+               INTEL_LL_RECV_PROVIDE() to provide a new receive buffer+*/
/*+               to the LL. EDD_STS_ERR_RX is set, if the frame has a      +*/
/*+               error (e.g. wrong CRC). The EDDS discards this frame      +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+ Use of this function within EDDS:                                       +*/
/*+                                                                         +*/
/*+                    Receive Interrupt                                    +*/
/*+                           |                                             +*/
/*+                           |                                             +*/
/*+       +------------------>+                                             +*/
/*+       |                   |                                             +*/
/*+       |                INTEL_LL_RECV()                                  +*/
/*+       |           +-------+------------+                                +*/
/*+       |           |                    |                                +*/
/*+       |     Frame present            No Frame                           +*/
/*+       |     (good or bad)              |                                +*/
/*+       |           |                    |                                +*/
/*+       |   handle buffer if good        |                                +*/
/*+       |           |                    |                                +*/
/*+       |    INTEL_LL_RECV_PROVIDE()     |                                +*/
/*+       |           |                    |                                +*/
/*+       +-----------+                    |                                +*/
/*+                                       End (wait for receive interrupt)  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pBufferAddr,pLength,pPortID)
//JB 17/11/2014 params set within this function
LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_RECV(LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR *pBufferAddr,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pLength,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pPortID) {

	INTEL_RDESC* currentRD;
    LSA_UINT32 RxDescIdx;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;
	LSA_RESULT Status;

	EDDS_LL_TRACE_02(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_RECV(pBufferAddr: 0x%X, pLength: 0x%X)",
			pBufferAddr,
			pLength);

	// get current receive descriptor index, this descriptor contains received frame
	RxDescIdx = handle->RXDescriptorIndex;
	currentRD = &(handle->pRxDescriptors[RxDescIdx]);

	/**@todo	honor cache line size and alignment to reduce number of EDDS_DMA_SYNC_FROM_DEVICE calls!
	 */
	EDDS_DMA_SYNC_FROM_DEVICE(handle->hSysDev,currentRD,sizeof(*currentRD));

    /* check DD for current receive dscr */
	if( intel_rdesc_done(currentRD) )
	{
	  Status = EDD_STS_OK;

	  /* clear DD bit */
	  INTEL_CLRBITI(currentRD->Status, INTEL_RDESC_STATUS_DD);

	  /* set length of received frame and virtual buffer address */
	  *pLength = currentRD->Length;
	  *pPortID = 1;
	  *pBufferAddr = handle->RxFrameBufferPtr[RxDescIdx];

	  /* reset physical buffer address and length */
	  currentRD->BufferAddress = 0;
	  currentRD->Length        = 0;

	  /* set virtual buffer address to NULL */
	  handle->RxFrameBufferPtr[RxDescIdx] = 0;

	  /**@warning	to use EDDS_DMA_SYNC_FROM_DEVICE,
	   * 			the pointer "*pBufferAddr" must be cache line aligned to prevent destroying of other data!
	   */
	  EDDS_DMA_SYNC_FROM_DEVICE(handle->hSysDev,*pBufferAddr,*pLength);

	  // increase current receive descriptor index (regarding descriptor ring size)
	  ++handle->RXDescriptorIndex;
	  if(handle->RxFrameBufferCount <= handle->RXDescriptorIndex)
	  {
	    handle->RXDescriptorIndex = 0;
	  }
	}
	else
	{
	  Status = EDD_STS_OK_NO_DATA;
	}

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_RECV(), Status: 0x%X",
			Status);

	return (Status);

}
//lint +esym(613,pBufferAddr,pLength,pPortID)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname  :    INTEL_LL_RECV_PROVIDE                               +*/
/*+  Input/Output  :    LSA_VOID_PTR_TYPE             pLLManagement         +*/
/*+                     EDD_UPPER_MEM_PTR_TYPE                pBufferAddr   +*/
/*+  Result        :    LSA_RESULT                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pBufferAddr:  Pointer to receive buffer. The receive buffer is         +*/
/*+                EDD_FRAME_BUFFER_LENGTH in size.                         +*/
/*+                                                                         +*/
/*+  Result     :  EDD_STS_OK                                               +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions provides a new receive buffer to the LL.   +*/
/*+               This function is called after the LL returned a buffer    +*/
/*+               with INTEL_LL_RECV() to the EDDS.                         +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+ Use of this function within EDDS:                                       +*/
/*+                                                                         +*/
/*+                    Receive Interrupt                                    +*/
/*+                           |                                             +*/
/*+                           |                                             +*/
/*+       +------------------>+                                             +*/
/*+       |                   |                                             +*/
/*+       |                INTEL_LL_RECV()                                  +*/
/*+       |           +-------+------------+                                +*/
/*+       |           |                    |                                +*/
/*+       |     Frame present            No Frame                           +*/
/*+       |     (good or bad)              |                                +*/
/*+       |           |                    |                                +*/
/*+       |   handle buffer if good        |                                +*/
/*+       |           |                    |                                +*/
/*+       |    INTEL_LL_RECV_PROVIDE()     |                                +*/
/*+       |           |                    |                                +*/
/*+       +-----------+                    |                                +*/
/*+                                       End (wait for receive interrupt)  +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_RECV_PROVIDE(
		LSA_VOID_PTR_TYPE pLLManagement, EDD_UPPER_MEM_PTR_TYPE pBufferAddr) {

	LSA_RESULT Status;
	INTEL_RDESC* lastRD;

	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_RECV_PROVIDE(pBufferAddr: 0x%X)",
			pBufferAddr);

	// get tail receive descriptor; it identifies the location beyond the last descriptor
	// hardware can progress; this is the location where software writes the first new descriptor
	// NOTE: there is always one descriptor that is not usable by hardware or software, all other
	//       descriptors belong to hardware; this is the descriptor where the tail index points to;
	//       when a receiption is signaled per interrupt a descriptor temporarily belongs to software
	//       for buffer exchange; the tail descriptor is incremented, so that it always points to
	//       the cleared descriptor from the previous call to LL_RECV; hardware would assume that
	//       there is no free descriptor when head would be equal to tail --> this is not permitted;
	++handle->RXDescriptorIndexEnd;

	if(handle->RxFrameBufferCount <= handle->RXDescriptorIndexEnd)
	{
	  handle->RXDescriptorIndexEnd = 0;
	}

	lastRD = &handle->pRxDescriptors[handle->RXDescriptorIndexEnd];
	handle->RxFrameBufferPtr[handle->RXDescriptorIndexEnd] = pBufferAddr;
	EDDS_PTR_TO_ADDR64_UPPER(handle->hSysDev,pBufferAddr, &lastRD->BufferAddress);  /* set new ethernet frame buffer */
	EDDS_DMA_SYNC_TO_DEVICE(handle->hSysDev,lastRD,sizeof(*lastRD));

	intel_reg32(INTEL_RDT) = handle->RXDescriptorIndexEnd;

	Status = EDD_STS_OK;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_RECV_PROVIDE(), Status: 0x%X",
			Status);

	return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_SEND                               +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             EDDS_UPPER_MEM_PTR pBufferAddr              +*/
/*+                             LSA_UINT32        Length                    +*/
/*+                             LSA_UINT32        PortID                    +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pBufferAddr:  Pointer to buffer with Frame-netto-data to send          +*/
/*+                This address will be used and put to the controller!     +*/
/*+                (The data is not copied!) So the buffer must not be      +*/
/*+                accessed until it is released!                           +*/
/*+  Length     :  Number of Bytes pBuffer points to  (max. 1514/1518 Byte) +*/
/*+  PortID     :  PortID to send                                           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_PARAM                               +*/
/*+                         EDDS_STS_ERR_RESOURCES                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Start sending a Frame. The frame (pBufferAddr) is given   +*/
/*+               to  the low-level function for further handling. Length   +*/
/*+               is the length of the frame. The frame has to include      +*/
/*+               mac-addresses, type/len, and data. FCS is added be low-   +*/
/*+               level function (normally be controller)                   +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SEND(LSA_VOID_PTR_TYPE pLLManagement,
		EDD_UPPER_MEM_PTR_TYPE pBufferAddr, LSA_UINT32 Length,
		LSA_UINT32 PortID)

{
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;
	INTEL_TDESC* currentTD;
	LSA_RESULT Status;
	LSA_UINT32 tail;
	EDDS_LL_TRACE_03(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_SEND(pBufferAddr: 0x%X, Length: 0x%X, PortID: %d)",
			pBufferAddr,
			Length,
			PortID);

	/*-------------------------------------------------------------------------*/
	/* check for valid length                                                  */
	/*-------------------------------------------------------------------------*/

	if ( Length > EDD_IEEE_FRAME_LENGTH || 1 < PortID) return(EDD_STS_ERR_PARAM);


	if((LSA_FALSE != handle->isLinkUp)&&(0 == handle->resetState))
	{
	  tail = handle->TXDescriptorIndexEnd;

	  currentTD = &(handle->pTxDescriptors[tail]);

	  /* only a maximum of handle->TxFrameBufferCount-1 frames may be queued at a time,
	   * since intel descriptor rings need an "space" descriptor!
	   */
	  if(handle->TXDescriptorsUsed < (handle->TxFrameBufferCount-1))	/* extra test; should always be true */
	  {
	    EDDS_PTR_TO_ADDR64_UPPER(handle->hSysDev,pBufferAddr, &currentTD->legacy.BufferAddress);
	    currentTD->legacy.Length = (LSA_UINT16) Length;
	    INTEL_CLRBITI(currentTD->legacy.STA_RSV,0);		/* clear DD */
	    ++handle->TXDescriptorsUsed;
	    ++tail;
	    if(handle->TxFrameBufferCount <= tail)
	    {
	      tail = 0;
	    }

	    /**@todo	honor cache line size and alignment to reduce number of EDDS_DMA_SYNC_TO_DEVICE calls!
	     */
	    EDDS_DMA_SYNC_TO_DEVICE(handle->hSysDev,currentTD,sizeof(*currentTD));
	    EDDS_DMA_SYNC_TO_DEVICE(handle->hSysDev,pBufferAddr,Length);

        handle->TXDescriptorIndexEnd = tail;
        EDDS_LL_TRACE_03(handle->TraceIdx,LSA_TRACE_LEVEL_NOTE,"INTEL_LL_SEND(0x%08X): BufferAddress: 0x%X, ++TXDescriptorsUsed: %d",handle,currentTD->legacy.BufferAddress, handle->TXDescriptorsUsed);

#if !defined(EDDS_CFG_SEND_BLOCKED)
		if(0 != handle->resetState)
		{
			EDDS_TRIGGER_SW_INTERRUPT(handle->hDDB,EDDS_INTERRUPT_TX);
		}else
		{
			intel_reg32(INTEL_TDT) = handle->TXDescriptorIndexEnd;
		}
#else /* !defined(EDDS_CFG_SEND_BLOCKED) */
		/* do nothing */
#endif/* !defined(EDDS_CFG_SEND_BLOCKED) */
		Status = EDD_STS_OK;
		}else
		{
			Status = EDD_STS_ERR_RESOURCE;
		}

	}
	else
	{
		++handle->fakeSendCounter;
#ifndef EDDS_CFG_SEND_BLOCKED
		EDDS_TRIGGER_SW_INTERRUPT(handle->hDDB,EDDS_INTERRUPT_TX);
#endif /* !defined(EDDS_CFG_SEND_BLOCKED) */
		Status = EDD_STS_OK;
	}
	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_SEND(), Status: 0x%X",
			Status);

	return (Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_SEND_TRIGGER                       +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    --                                          +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Starts transmission of frames applied by INTEL_LL_SEND.   +*/
/*+               Only called if EDDS_CFG_SEND_BLOCKED is defined (edd_cfg.h)+*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifdef EDDS_CFG_SEND_BLOCKED
LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_SEND_TRIGGER(LSA_VOID_PTR_TYPE pLLManagement)
{
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*)pLLManagement;
	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_SEND_TRIGGER()");

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
	if(0 != handle->resetState)
	{
		/* do a fake TX interrupt */
		EDDS_TRIGGER_SW_INTERRUPT(handle->hDDB,EDDS_INTERRUPT_TX);
	}else
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */
	{
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
		if(0 != handle->fakeSendCounter)
		{
			/* do a fake TX interrupt */
			EDDS_TRIGGER_SW_INTERRUPT(handle->hDDB,EDDS_INTERRUPT_TX);
		}
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

		intel_reg32(INTEL_TDT) = handle->TXDescriptorIndexEnd;
	}

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_SEND()");

}
#endif /* EDDS_CFG_SEND_BLOCKED */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_SEND_STS                           +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK           : frame successul transm.  +*/
/*+                         EDD_STS_OK_NO_DATA   : no frame transmitted     +*/
/*+                         ... transmit errors                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Checks for finished transmissions. If a transmission was  +*/
/*+               finished, EDD_STS_OK is returned. If no (more) finished   +*/
/*+               EDD_STS_OK_NO_DATA is returned.                           +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SEND_STS(
		LSA_VOID_PTR_TYPE pLLManagement) {
	LSA_RESULT Status;
	INTEL_TDESC* lastTD;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_SEND_STS()");

	Status = EDD_STS_OK;

	if(0 != handle->TXDescriptorsUsed)
	{
	  lastTD = &handle->pTxDescriptors[handle->TXDescriptorIndex];

	  /**@warning	to use EDDS_DMA_SYNC_FROM_DEVICE,
	   * 			the pointer "lastTD" must be cache line aligned to prevent destroying of other data.
	   */
	  EDDS_DMA_SYNC_FROM_DEVICE(handle->hSysDev,lastTD,sizeof(*lastTD));

	  if(1 == INTEL_GETBITI(0,lastTD->legacy.STA_RSV)		/* DD == 1; Descriptor Done */
			  &&(handle->TXDescriptorIndex != handle->TXDescriptorIndexEnd)
			  &&(0 < handle->TXDescriptorsUsed))
	  {
	      Status = EDD_STS_OK;

	      ++handle->TXDescriptorIndex;
	      --handle->TXDescriptorsUsed;
	      if(handle->TxFrameBufferCount <= handle->TXDescriptorIndex)
	      {
	        handle->TXDescriptorIndex = 0;
	      }
          EDDS_LL_TRACE_03(handle->TraceIdx,LSA_TRACE_LEVEL_NOTE,"INTEL_LL_SEND_STS(0x%08X): pBufferAddr: 0x%X, --TXDescriptorsUsed: %d",handle,lastTD->legacy.BufferAddress, handle->TXDescriptorsUsed);

          lastTD->legacy.BufferAddress = 0;
          /**@todo  honor cache line size and alignment to reduce number of EDDS_DMA_SYNC_TO_DEVICE calls!
           */
          EDDS_DMA_SYNC_TO_DEVICE(handle->hSysDev,lastTD,sizeof(*lastTD));
	  }
	  else
	  {
	    Status = EDD_STS_OK_NO_DATA;
	  }

	}
	else if(0 != handle->fakeSendCounter)
	{
		--handle->fakeSendCounter;
		Status = EDD_STS_OK;
	}else
	{
		Status = EDD_STS_OK_NO_DATA;
	}

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_SEND_STS(), Status: 0x%X",
			Status);

	return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_REINIT_TX                          +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_TIMEOUT  :                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reinits Transmit-unit of ethernetcontroller. Called after +*/
/*+               a transmit-timeout occured                                +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_REINIT_TX(
		LSA_VOID_PTR_TYPE pLLManagement) {
	LSA_RESULT Status;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;
	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_REINIT_TX()");

	Status = EDD_STS_OK;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_REINIT_TX(), Status: 0x%X",
			Status);

	return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_GET_STATS                          +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                      PortID      +*/
/*+                             EDDS_LOCAL_STATISTICS_PTR_TYPE  pStats      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : PortID to read statistics for                             +*/
/*+               0: local interface                                        +*/
/*+               1..x: Port                                                +*/
/*+                                                                         +*/
/*+               Note: if only one port present, local interface is equal  +*/
/*+                     to the port statistic                               +*/
/*+                                                                         +*/
/*+  pStats     : Pointer to Statistics Structure to be filled              +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets statistics from Low-Level-functions                   */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_GET_STATS(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID,
		EDDS_LOCAL_STATISTICS_PTR_TYPE pStats) {
	LSA_RESULT Status;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	EDDS_LL_TRACE_02(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_GET_STATS(PortID: 0x%X, pStats: 0x%X)",
			PortID,
			pStats);

	if((1 < PortID) || (0 == pStats))
	{
		/* PortID not expected */
		Status = EDD_STS_ERR_PRM_PORTID;
	}
	else
	{
		LSA_UINT64 tmp, GPRC, GPTC;

		/* read statistic registers */
		intel_save_statistic_counter(handle);

		/* ---------------------- */
		/*    input statistics    */
		/* ---------------------- */

		tmp = handle->PortStats[(INTEL_BPRC-INTEL_CRCERRS)/4] + handle->PortStats[(INTEL_MPRC-INTEL_CRCERRS)/4];
		pStats->InOctets        = (LSA_UINT32)handle->PortStats[(INTEL_GORCL-INTEL_CRCERRS)/4];
		pStats->InNUcastPkts	= (LSA_UINT32)tmp;
		GPRC                    = handle->PortStats[(INTEL_GPRC-INTEL_CRCERRS)/4];

		/* Determine amount of received unicast frames. */
		/* NOTE: The statistic registers can not be read consistently and incoming frames may alter some */
		/*       counters in between. This might lead to temporary wrong counter values. Therefore the   */
		/*       last counter is saved in a shadow variable. In addition LSA_UINT64 counter values are   */
		/*       used so that we dont have to care about wrap arounds.                                   */
		if((handle->InUcastPktsShadow + tmp) > GPRC)
		{
		    pStats->InUcastPkts = (LSA_UINT32)handle->InUcastPktsShadow;
		}
		else
		{
		    handle->InUcastPktsShadow = (LSA_UINT64)(GPRC - tmp); /* save current unicast frame counter in shadow */
		    pStats->InUcastPkts       = (LSA_UINT32)handle->InUcastPktsShadow; /* unicast = GPRC - MPRC - BPRC */
		}

		pStats->InDiscards		= (LSA_UINT32)handle->PortStats[(INTEL_MPC-INTEL_CRCERRS)/4];
		pStats->InErrors		= (LSA_UINT32)(handle->PortStats[(INTEL_CRCERRS-INTEL_CRCERRS)/4] + handle->PortStats[(INTEL_RXERRC-INTEL_CRCERRS)/4] + handle->PortStats[(INTEL_RLEC-INTEL_CRCERRS)/4]);  //lint !e845 !e778 JB 08/12/2014 readability
		pStats->InUnknownProtos	= 0;

		/* ---------------------- */
		/*   output statistics    */
		/* ---------------------- */

		tmp = handle->PortStats[(INTEL_MPTC-INTEL_CRCERRS)/4]+handle->PortStats[(INTEL_BPTC-INTEL_CRCERRS)/4];
		pStats->OutOctets		= (LSA_UINT32)handle->PortStats[(INTEL_GOTCL-INTEL_CRCERRS)/4];
		pStats->OutNUcastPkts   = (LSA_UINT32)tmp;
		GPTC                    = handle->PortStats[(INTEL_GPTC-INTEL_CRCERRS)/4];

		/* determine amount of sent unicast frames */
        /* NOTE: The statistic registers can not be read consistently and outgoing frames may alter some */
        /*       counters in between. This might lead to temporary wrong counter values. Therefore the   */
        /*       last counter is saved in a shadow variable. In addition LSA_UINT64 counter values are   */
        /*       used so that we dont have to care about wrap arounds.                                   */
        if((handle->OutUcastPktsShadow + tmp) > GPTC)
        {
            pStats->OutUcastPkts = (LSA_UINT32)handle->OutUcastPktsShadow;
        }
        else
        {
            handle->OutUcastPktsShadow = (LSA_UINT64)(GPTC - tmp); /* save current unicast frame counter in shadow */
            pStats->OutUcastPkts       = (LSA_UINT32)handle->OutUcastPktsShadow; /* unicast = GPRC - MPRC - BPRC */
        }

		pStats->OutDiscards		= 0;
		pStats->OutErrors		= 0;
		pStats->OutQLen			= 0;

		Status = EDD_STS_OK;
	}
	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_GET_STATS(), Status: 0x%X",
			Status);

	return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :  INTEL_LL_GET_CAPS                             +*/
/*+  Input/Output          :  LSA_VOID_PTR_TYPE             pLLManagement   +*/
/*+                           EDDS_LOCAL_EDDS_LL_CAPS_PTR_TYPE pCaps [out]  +*/
/*+  Result                :  LSA_RESULT                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : PortID to read statistics for                             +*/
/*+               0: local interface                                        +*/
/*+               1..x: Port                                                +*/
/*+                                                                         +*/
/*+               Note: if only one port present, local interface is equal  +*/
/*+                     to the port statistic                               +*/
/*+                                                                         +*/
/*+  pStats     : Pointer to Statistics Structure to be filled              +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                           :                                             +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets statistics from Low-Level-functions                   */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_GET_CAPS(
		LSA_VOID_PTR_TYPE pLLManagement, EDDS_LOCAL_EDDS_LL_CAPS_PTR_TYPE pCaps) {
	LSA_RESULT Status;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_GET_CAPS()");

	/* your code here */
	Status = EDD_STS_OK;
	pCaps->HWFeatures = EDDS_LL_CAPS_HWF_NONE | EDDS_LL_CAPS_HWF_CRC_GENERATE;  //lint !e835 JB 13/11/2014 define-based behaviour
	pCaps->SWFeatures = EDDS_LL_CAPS_SWF_NONE | EDDS_LL_CAPS_SWF_SEND_ASYNCHRONOUS | EDDS_LL_CAPS_SWF_SRT; //lint !e835 JB 13/11/2014 define-based behaviour
	pCaps->PortCnt = 1;
	pCaps->MaxFrameSize = 1536;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_GET_CAPS(), Status: 0x%X",
			Status);

	return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname      : INTEL_LL_GET_LINK_STATE                            +*/
/*+  Input/Output      : LSA_VOID_PTR_TYPE                  pLLManagement   +*/
/*+                      EDD_UPPER_GET_LINK_STATUS_PTR_TYPE   pLinkStat,    +*/
/*+                      EDDS_LOCAL_MEM_U16_PTR_TYPE  pMAUType,             +*/
/*+                      EDDS_LOCAL_MEM_U8_PTR_TYPE   pMediaType,           +*/
/*+                      EDDS_LOCAL_MEM_U32_PTR_TYPE  pPortStatus,          +*/
/*+                      EDDS_LOCAL_MEM_U32_PTR_TYPE  pAutonegCapAdvertised)+*/
/*+                                                                         +*/
/*+  Result             :    LSA_RESULT                                     +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to get link status from  (1..x)                      +*/
/*+  pLinkStat  : Pointer to LinkStatus to be filled                        +*/
/*+                                                                         +*/
/*+  pMAUType   : Pointer to address for actual MAUType                     +*/
/*+               EDD_MAUTYPE_UNKNOWN (=0) if unknown. Else IEC MAUType     +*/
/*+                                                                         +*/
/*+  pMediaType : Pointer to address for actual MediaType                   +*/
/*+               EDD_MEDIATYPE_UNKNOWN (=0) if unknown. Else IEC MediaType +*/
/*+                                                                         +*/
/*+  pPortStatus: Pointer to address for actual PortStatus (see edd_usr.h)  +*/
/*+                                                                         +*/
/*+  pAutonegCapAdvertised:    Pointer to address for actual autoneg        +*/
/*+                            advertised capabilities.                     +*/
/*+                            Note: 0 if no autoneg supported.             +*/
/*+                            EDDS_AUTONEG_CAP_xxx see edd_usr.h           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+             :           EDD_STS_ERR_PARAM                               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets LinkStatus from Low-Level-functions                  +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pMAUType,pMediaType,pAutonegCapAdvertised,pPortStatus)
//JB 17/11/2014 params will be set within this function
LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_GET_LINK_STATE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID,
		EDD_UPPER_GET_LINK_STATUS_PTR_TYPE pLinkStat,
		EDDS_LOCAL_MEM_U16_PTR_TYPE pMAUType,
		EDDS_LOCAL_MEM_U8_PTR_TYPE pMediaType,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pPortStatus,
		EDDS_LOCAL_MEM_U32_PTR_TYPE pAutonegCapAdvertised,
		EDDS_LOCAL_MEM_U8_PTR_TYPE  pLinkSpeedModeConfigured) {
	LSA_RESULT Status = EDD_STS_OK;
	LSA_UINT32 value;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	if( (LSA_NULL == handle) || (LSA_NULL == pLinkStat) )
		return EDD_STS_ERR_PARAM;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_GET_LINK_STATE(PortID: %d)",
			PortID);

	if (1 != PortID) {
		Status = EDD_STS_ERR_PARAM;
	} else {
		//LSA_BOOL readOK;
		//LSA_UINT16 phyreg;

		*pMAUType   = EDD_MAUTYPE_UNKNOWN;
		*pMediaType = EDD_MEDIATYPE_UNKNOWN;
		*pAutonegCapAdvertised = handle->AutoNegCapAdvertised;
		*pLinkSpeedModeConfigured = handle->LinkSpeedModeConfigured;
		pLinkStat->PortID = (LSA_UINT16) PortID;
		pLinkStat->Mode = EDD_LINK_UNKNOWN;
		pLinkStat->Speed = EDD_LINK_UNKNOWN;

		if(0 == handle->resetState)
		{
			value = intel_reg32(INTEL_STATUS);

			pLinkStat->Status =
					(0 == INTEL_GETBIT(INTEL_STATUS_LU,value)) ?
							EDD_LINK_DOWN : EDD_LINK_UP;

			if(EDD_LINK_DOWN != pLinkStat->Status)
			{
				pLinkStat->Mode =
						(0 == INTEL_GETBIT(INTEL_STATUS_FD,value)) ?
								EDD_LINK_MODE_HALF : EDD_LINK_MODE_FULL;

			switch(INTEL_GETBITS(INTEL_STATUS_SPEED,value))
			{
				case 0:
				pLinkStat->Speed = EDD_LINK_SPEED_10;
				break;
				case 1:
				pLinkStat->Speed = EDD_LINK_SPEED_100;
				break;
				case 2:
				case 3:
				pLinkStat->Speed = EDD_LINK_SPEED_1000;
				break;
				default:
					Status = EDD_STS_ERR_HW;
					break;
			}
			}else
			{
				pLinkStat->Mode = EDD_LINK_UNKNOWN;
				pLinkStat->Speed = EDD_LINK_UNKNOWN;
			}
		}else
		{
			pLinkStat->Status = EDD_LINK_DOWN;
		}
		EDDS_GET_PHY_PARAMS(handle->hSysDev, handle->hDDB, PortID, pLinkStat->Speed, pLinkStat->Mode, pMAUType, pMediaType);
		*pPortStatus = EDD_PORT_PRESENT;
	}

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_GET_LINK_STATE(), Status: 0x%X",
			Status);

	return (Status);

}
//lint +esym(613,pMAUType,pMediaType,pAutonegCapAdvertised,pPortStatus)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_SET_LINK_STATE                     +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                          PortID  +*/
/*+                             LSA_UINT8                           LinkStat+*/
/*+                             LSA_UINT8                           PHYPower+*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to set Link for                                      +*/
/*+  LinkStat   : LinkStatus to be setup                                    +*/
/*+               EDD_LINK_AUTONEG                                          +*/
/*+               EDD_LINK_100MBIT_HALF                                     +*/
/*+               EDD_LINK_100MBIT_FULL                                     +*/
/*+               EDD_LINK_10MBIT_HALF                                      +*/
/*+               EDD_LINK_10MBIT_FULL                                      +*/
/*+               EDD_LINK_UNCHANGED     (do not change LinkStat)           +*/
/*+  PHYPower   : Power of PHY                                              +*/
/*+               EDDS_PHY_POWER_ON                                         +*/
/*+               EDDS_PHY_POWER_OFF                                        +*/
/*+               EDDS_PHY_POWER_UNCHANGED(do not change power)             +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Set Link to given values (only if hardware is setup)      +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SET_LINK_STATE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID, LSA_UINT8 LinkStat,
		LSA_UINT8 PHYPower) {
	LSA_RESULT Status;
	LSA_BOOL Disabled1000MBitSupport;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	EDDS_LL_TRACE_03(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_SET_LINK_STATE(PortID: %d, LinkStat: 0x%X, Power: 0x%X)",
			PortID,
			LinkStat,
			PHYPower);

	Disabled1000MBitSupport = handle->params.Disable1000MBitSupport && (EDD_LINK_1GBIT_FULL == LinkStat || EDD_LINK_1GBIT_HALF == LinkStat);

	if(1 != PortID || Disabled1000MBitSupport)
	{
		Status = EDD_STS_ERR_PARAM;
	}else
	{
		Status = intel_set_link(handle, LinkStat, PHYPower);
	}

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_SET_LINK_STATE(), Status: 0x%X",
			Status);

	return (Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_CHECK_MAUTYPE                      +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                          PortID  +*/
/*+                             LSA_UINT16                          MAUType +*/
/*+                             LSA_UINT8                           PHYPower+*/
/*+                             EDDS_LOCAL_MEM_U8_PTR_TYPE         pLinkStat+*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to check LinkStat for (1..EDDS_MAX_PORT_CNT)         +*/
/*+  MAUType    : MAUType to be checked                                     +*/
/*+               EDD_MAUTYPE_UNKNOWN (=0)                                  +*/
/*+               else IEEE MAUType (1..x)                                  +*/
/*+  PHYPower   : Power of PHY to be checked                                +*/
/*+               EDDS_PHY_POWER_ON                                         +*/
/*+               EDDS_PHY_POWER_OFF                                        +*/
/*+               EDDS_PHY_POWER_UNCHANGED(do not check)                    +*/
/*+  pLinkStat  : LinkState  setup                                          +*/
/*+               EDD_LINK_100MBIT_HALF                                     +*/
/*+               EDD_LINK_100MBIT_FULL                                     +*/
/*+               EDD_LINK_10MBIT_HALF                                      +*/
/*+               EDD_LINK_10MBIT_FULL                                      +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Check if the MAUType/PHYPower can be set for Port         +*/
/*+                                                                         +*/
/*+               If a MAUType is present (<> EDD_MAUTYPE_UNKNOWN) check if +*/
/*+               it is supported and return pLinkStat with Speed/Mode for  +*/
/*+               MAUType.                                                  +*/
/*+                                                                         +*/
/*+               If PHYPower is <> EDDS_PHY_POWER_UNCHANGED check if PHY   +*/
/*+               power is supported.                                       +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_CHECK_MAUTYPE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID, LSA_UINT16 MAUType,
		LSA_UINT8 PHYPower, EDDS_LOCAL_MEM_U8_PTR_TYPE pLinkStat) {
	LSA_RESULT Status;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;
	EDDS_LL_TRACE_03(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_CHECK_MAUTYPE(PortID: %d, MAUType: %d, Power: 0x%X)",
			PortID,
			MAUType,
			PHYPower);

	if (1 == PortID) {
		Status = EDD_STS_OK;

		if (EDD_MAUTYPE_UNKNOWN != MAUType) {
			EDDS_CHECK_AND_DECODE_MAUTYPE(&Status, handle->hSysDev, handle->hDDB, PortID,
					MAUType, pLinkStat);

			if( (Status == EDD_STS_OK) &&
			    (LSA_TRUE == handle->params.Disable1000MBitSupport) )  //lint !e731 JB 13/11/2014 on purpose
			{
			    if(EDD_LINK_1GBIT_HALF == *pLinkStat ||
			       EDD_LINK_1GBIT_FULL == *pLinkStat)
			    {
			        EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT, "INTEL_LL_CHECK_MAUTYPE: 1000MBit support disabled");

			        Status = EDD_STS_ERR_PARAM;
			    }
			}
			/* check MAUTYPE */
		} else {
			if(0 == handle->resetState)
			{
			/* get actual link state (baud rate +duplexity)*/
			LSA_UINT32 value = intel_reg32(INTEL_STATUS);

			switch(INTEL_GETBITS(INTEL_STATUS_SPEED,value))
			{
				case 0:
				*pLinkStat = (0 == INTEL_GETBIT(INTEL_STATUS_FD,value)) ? EDD_LINK_10MBIT_HALF : EDD_LINK_10MBIT_FULL;
				break;
				case 1:
				*pLinkStat = (0 == INTEL_GETBIT(INTEL_STATUS_FD,value)) ? EDD_LINK_100MBIT_HALF : EDD_LINK_100MBIT_FULL;
				break;
				case 2:
				case 3:
				*pLinkStat = (0 == INTEL_GETBIT(INTEL_STATUS_FD,value)) ? EDD_LINK_1GBIT_HALF : EDD_LINK_1GBIT_FULL;
				break;
				default:
					Status = EDD_STS_ERR_RESOURCE;
					break;
			}
			}
		}

		if (EDD_STS_OK == Status &&
		    EDDS_PHY_POWER_UNCHANGED != PHYPower) {
			Status = EDD_STS_OK; /* OK, supported (EDDS_PHY_POWER_ON and EDDS_PHY_POWER_OFF). */
		}

	} else {
		Status = EDD_STS_ERR_PARAM;
	}

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_CHECK_MAUTYPE(), Status: 0x%X",
			Status);

	return (Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_GET_LINK_STATE_SETUP               +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                          PortID  +*/
/*+                             EDDS_LOCAL_MEM_U8_PTR_TYPE         pLinkStat+*/
/*+                             EDDS_LOCAL_MEM_U8_PTR_TYPE         pPHYPower+*/
/*+                             LSA_BOOL EDDS_LOCAL_MEM_ATTR *     pIsWirele+*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to set Link for                                      +*/
/*+  pLinkStat  : LinkStatus setup                                          +*/
/*+               EDD_LINK_AUTONEG                                          +*/
/*+               EDD_LINK_100MBIT_HALF                                     +*/
/*+               EDD_LINK_100MBIT_FULL                                     +*/
/*+               EDD_LINK_10MBIT_HALF                                      +*/
/*+               EDD_LINK_10MBIT_FULL                                      +*/
/*+  pPHYPower  : PHY Power setup                                           +*/
/*+               EDDS_PHY_POWER_ON                                         +*/
/*+               EDDS_PHY_POWER_OFF                                        +*/
/*+  pIsWireless: Is this port a wireless port?                             +*/
/*+               LSA_FALSE  : no                                           +*/
/*+               LSA_TRUE   : yes                                          +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Reads the actual Setup of LinkStat and PHYPower           +*/
/*+               (Note: This are the Setup-values!)                        +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,pLinkStat,pPHYPower,pIsWireless)
//JB 17/11/2014 params will be set within this function
LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_GET_LINK_STATE_SETUP(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID,
		EDDS_LOCAL_MEM_U8_PTR_TYPE pLinkStat,
		EDDS_LOCAL_MEM_U8_PTR_TYPE pPHYPower,
		LSA_BOOL EDDS_LOCAL_MEM_ATTR * pIsWireless) {
	LSA_RESULT Status;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;
	LSA_UNUSED_ARG(PortID);

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_GET_LINK_STATE_SETUP()");

	/* your code here */
	Status = EDD_STS_OK;
	*pLinkStat = handle->LinkSpeedModeConfigured;
	*pPHYPower = EDDS_PHY_POWER_OFF;
	*pIsWireless = LSA_FALSE;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_GET_LINK_STATE_SETUP()");

	return (Status);

}
//lint +esym(613,pLinkStat,pPHYPower,pIsWireless)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_MC_ENABLE                          +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr+*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  pMCAddr    : Pointer to 6 Byte Multicastaddress to Enable              +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_RESOURCE      : no more free MC entrys        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Enables Multicastaddress                                  +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_MC_ENABLE(
        LSA_VOID_PTR_TYPE pLLManagement,
        EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr)
{
    LSA_RESULT Status;
    LSA_UINT i;
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_MC_ENABLE(pMCAddr: 0x%X)", pMCAddr);

    for (i = 1; i < 16; ++i) /* first receive address is interface MAC, skip it */
    {
        LSA_UINT32 tmp;
        tmp = handle->MCRegH[i];
        if (0 == INTEL_GETBIT(INTEL_RAH_AV,tmp)) /* extra port MAC address not enabled*/
        {
            handle->MCRegL[i] =
                    (((LSA_UINT32) pMCAddr->MacAdr[3]) << 24) | (((LSA_UINT32) pMCAddr->MacAdr[2]) << 16) | (((LSA_UINT32) pMCAddr->MacAdr[1]) << 8) | (((LSA_UINT32) pMCAddr->MacAdr[0])/*<<0*/);
            handle->MCRegH[i] =
            		INTEL_BIT(INTEL_RAH_AV,1) | /* enable MAC address */
                    (((LSA_UINT32) pMCAddr->MacAdr[5]) << 8) | (((LSA_UINT32) pMCAddr->MacAdr[4])/*<<0*/);

            intel_reg32(INTEL_RAL_0_15(i)) = handle->MCRegL[i];
            intel_reg32(INTEL_RAH_0_15(i)) = handle->MCRegH[i];
            break;
        }
    }
    Status = (16 == i) ? /* no empty entry found */
    EDD_STS_ERR_RESOURCE : EDD_STS_OK;

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_MC_ENABLE(), Status: 0x%X", Status);

    return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_MC_DISABLE                         +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_BOOL                        DisableAll  +*/
/*+                             EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr+*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  DisableAll  :LSA_TRUE: frees all Multicast addresses. pMCAddr          +*/
/*+                         not used.                                       +*/
/*+               LSA_FALSE:free pMACAddr only                              +*/
/*+  pMCAddr    : Pointer to 6 Byte Multicastaddress to Disable if          +*/
/*+               DisableAll = LSA_FALSE                                    +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Disables Multicastaddress                                 +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_MC_DISABLE(
        LSA_VOID_PTR_TYPE pLLManagement,
        LSA_BOOL DisableAll,
        EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr)
{
    LSA_RESULT Status;
    LSA_UINT32 i;
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_02(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_MC_DISABLE(DisableAll: 0x%X, pMCAddr: 0x%X)",
            DisableAll, pMCAddr);

    for (i = 1; i < 16; ++i) /* first receive address is interface MAC, skip it */
    {
        LSA_UINT32 tmp;
        tmp = handle->MCRegH[i];
        if (LSA_FALSE == DisableAll)
        {
            /* check if last 2 byte are the same and MAC address is enabled */
            if ((0x8000FFFF & tmp) == (INTEL_BIT(INTEL_RAH_AV,1) | /* enable MAC address */
            (((LSA_UINT32) pMCAddr->MacAdr[5]) << 8) | (((LSA_UINT32) pMCAddr->MacAdr[4])/*<<0*/)))
            {
                LSA_UINT32 tmp2 = handle->MCRegL[i];
                /* check if first 4 byte are the same */
                if (tmp2 == ((((LSA_UINT32) pMCAddr->MacAdr[3]) << 24) | (((LSA_UINT32) pMCAddr->MacAdr[2]) << 16) | (((LSA_UINT32) pMCAddr->MacAdr[1]) << 8) | (((LSA_UINT32) pMCAddr->MacAdr[0])/*<<0*/)))
                {
                    /* disable MAC address */
                    handle->MCRegH[i] = 0;
                    handle->MCRegL[i] = 0;
                    intel_reg32(INTEL_RAH_0_15(i)) = handle->MCRegH[i];
                    intel_reg32(INTEL_RAL_0_15(i)) = handle->MCRegL[i];
                    break;
                }
            }
        }
        else /* disable all*/
        {
            handle->MCRegH[i] = 0;
            handle->MCRegL[i] = 0;
            intel_reg32(INTEL_RAH_0_15(i)) = handle->MCRegH[i];
            intel_reg32(INTEL_RAL_0_15(i)) = handle->MCRegL[i];
        }
    }

    Status = ((16 == i) && (!DisableAll)) ? /* no MAC entry found */
    EDD_STS_ERR_RESOURCE : EDD_STS_OK;

    EDDS_LL_TRACE_01(handle->TraceIdx, LSA_TRACE_LEVEL_CHAT,
            "OUT:INTEL_LL_MC_DISABLE(), Status: 0x%X", Status);

    return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :   INTEL_LL_GET_MAC_ADDR                        +*/
/*+  Input/Output          :   LSA_VOID_PTR_TYPE             pLLManagement  +*/
/*+                            LSA_UINT32                       PortID      +*/
/*+                            EDD_MAC_ADR_TYPE EDDS_LOCAL_MEM_ATTR *pMACAddr+*/
/*+  Result                :   LSA_RESULT                                   +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : 0    : MAC Address from Station (Interface)               +*/
/*+               1..EDDS_MAX_PORT_CNT : Port MAC Address                   +*/
/*+  pMCAddr    : Pointer to 6 Byte MAC                                     +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets station or port MAC-address of Ethernetcontroller    +*/
/*+               If MAC address is 0, LLIF should get a valid MAC from     +*/
/*+               e.g. EEPROM in ethernet controller or generate an error   +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_GET_MAC_ADDR(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID,
		EDD_MAC_ADR_TYPE EDDS_LOCAL_MEM_ATTR *pMACAddr) {
	LSA_RESULT Status = EDD_STS_OK;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	EDDS_LL_TRACE_02(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_GET_MAC_ADDR(PortID: %d, pMACAddr: 0x%X)",
			PortID,
			pMACAddr);

	if (PortID > 1)
	{
		Status = EDD_STS_ERR_PARAM;
	}
	else
	{
        if(0 == PortID)
        {
            *pMACAddr = handle->InterfaceMAC;
        }
        else
        {
            *pMACAddr = handle->PortMAC[0];
        }
	}

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_GET_MAC_ADDR(), Status: 0x%X",
			Status);

	return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_LED_BACKUP_MODE                    +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                      PortID      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to set LED mode for                                  +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_TIMEOUT                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Backup LED-Mode for PHY LEDs                              +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_LED_BACKUP_MODE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID) {
	LSA_RESULT Status;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_LED_BACKUP_MODE(PortID: %d)",
			PortID);

	if(1 == PortID)
	{
		if(0 == handle->resetState)
		{
			handle->backupLEDCTL  = intel_reg32(INTEL_LEDCTL);
		}else
		{
			handle->backupLEDCTL  = handle->savedLEDCTL;
		}
		Status = EDD_STS_OK;
	}else
	{
	  Status = EDD_STS_ERR_PARAM;
	}

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_LED_BACKUP_MODE(), Status: 0x%X",
			Status);

	return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_LED_RESTORE_MODE                   +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                      PortID      +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to set LED mode for                                  +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_TIMEOUT                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: RESTORE LED-Mode for PHY LEDs                             +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_LED_RESTORE_MODE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID) {
	LSA_RESULT Status;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_LED_RESTORE_MODE(PortID: %d)",
			PortID);

	if(1==PortID)
	{
		if(0 == handle->resetState)
		{
			intel_reg32(INTEL_LEDCTL) = handle->backupLEDCTL;
		}else
		{
			handle->savedLEDCTL = handle->backupLEDCTL;
		}

      Status = EDD_STS_OK;
	}else
    {
      Status = EDD_STS_ERR_PARAM;
    }

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_LED_RESTORE_MODE(), Status: 0x%X",
			Status);

	return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_LED_SET_MODE                       +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                      PortID      +*/
/*+                             LSA_BOOL                        LEDOn       +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  PortID     : Port to set LED mode for                                  +*/
/*+  LEDOn      : Mode of LED to set (LSA_TRUE/LSA_FALSE)                   +*/
/*+                                                                         +*/
/*+  Result     : EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+               EDD_STS_ERR_TIMEOUT                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Sets LED-Mode for PHY LEDs                                +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_LED_SET_MODE(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 PortID, LSA_BOOL LEDOn) {
	LSA_RESULT Status;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	EDDS_LL_TRACE_02(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_LED_SET_MODE(PortID: %d = %d)",
			PortID,
			LEDOn);

	if(1 == PortID)
	{
		handle->savedLEDCTL = (LSA_FALSE == LEDOn) ? INTEL_LEDCTL_DEFAULT_LEDS_OFF : INTEL_LEDCTL_DEFAULT_LEDS_ON;
		if(0 == handle->resetState)
		{
			intel_reg32(INTEL_LEDCTL) = handle->savedLEDCTL;
		}
	  Status = EDD_STS_OK;
	}else
    {
      Status = EDD_STS_ERR_PARAM;
    }

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_LED_SET_MODE(), Status: 0x%X",
			Status);

	return (Status);

}

#ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_RX_OVERLOAD_RX_ON                  +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function is used if Rx-Overload check is enabled     +*/
/*+               within EDDS.                                              +*/
/*+               After the receiver was turned off with                    +*/
/*+               INTEL_LL_RX_OVERLOAD_CTRL_OFF it will be turned on        +*/
/*+               after an idle time with this function.                    +*/
/*+                                                                         +*/
/*+               This function is called within EDDS low prio context.     +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_RX_OVERLOAD_RX_ON(
		LSA_VOID_PTR_TYPE pLLManagement)
{
    enum INTEL_DEVICE_ID deviceID;
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*)pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
            "IN :INTEL_LL_RX_OVERLOAD_RX_ON()");

    deviceID = (enum INTEL_DEVICE_ID) (handle->params.DeviceID);
    
    /* bug workaround for I82574 only */
    /* Disabling Receive Logic During Packet Reception Might Cause Receive Failures */
    /* Problem:     Clearing the RCTL.EN bit while a packet is being received might */
    /*              cause a failure of the receive packet buffer pointer mechanism. */
    /* Implication: Unpredictable behaviour and/or hand of receive packet buffer    */ 
    /*              control logic.                                                  */
    /* Solution: Enable receive interrupts and restore receive interrupt bit mask.  */
    if(IG82574L == deviceID)
    {
        /* Restore receive interrupt bis mask, see LL_INTERRUPT_POLL. */
        handle->receiveInterruptBitMask = handle->receiveInterruptBitMaskBackup;
        
        #if ! (defined (EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD) || defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE))
        intel_reg32(INTEL_IMS) = handle->receiveInterruptBitMaskBackup; /* enable receive interrupts */
        intel_reg32(INTEL_IMS); /* force posted write */
        
        /* set receive interrupt condition */
        intel_reg32(INTEL_ICS) = handle->receiveInterruptBitMask;
        #endif
    }
    else
    {
        /* Receive Control Register - RCTL (0x00100; RW)                         */
        /* EN - The receiver is enabled when this bit is set to 1b. Writing this */
        /*      bit to 0b, stops reception after receipt of any in progress      */
        /*      packet. All subsequent packets are then immediately dropped      */
        /*      until this bit is set to 1b.                                     */
    
        INTEL_SETBIT(intel_reg32(INTEL_RCTL), INTEL_RCTL_EN);
    }

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_RX_OVERLOAD_RX_ON()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_RX_OVERLOAD_RX_OFF                 +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function is used if Rx-Overload check is enabled     +*/
/*+               within EDDS.                                              +*/
/*+               If the the EDDS detects an Rx-Overload, it calls this     +*/
/*+               function to turn the receiver OFF.                        +*/
/*+                                                                         +*/
/*+               This function is called within EDDS edds_interrupt()      +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_RX_OVERLOAD_RX_OFF(
		LSA_VOID_PTR_TYPE pLLManagement)
{
    enum INTEL_DEVICE_ID deviceID;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*)pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_RX_OVERLOAD_RX_OFF()");

	deviceID = (enum INTEL_DEVICE_ID) (handle->params.DeviceID);
	
    /* bug workaround for I82574 only */
    /* Disabling Receive Logic During Packet Reception Might Cause Receive Failures */
    /* Problem:     Clearing the RCTL.EN bit while a packet is being received might */
    /*              cause a failure of the receive packet buffer pointer mechanism. */
    /* Implication: Unpredictable behaviour and/or hand of receive packet buffer    */ 
    /*              control logic.                                                  */
    /* Solution:    Disable receive interrupts and reset receive interrupt bit mask.*/
    if(IG82574L == deviceID)
    {
        #if ! (defined (EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD) || defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE))
        intel_reg32(INTEL_IMC) = handle->receiveInterruptBitMask; /* disable receive interrupts */
        intel_reg32(INTEL_IMC); /* force posted write */
        #endif
        
        /* NOTE: With receive interrupts disabled ICR still contains receive events. */
        /*       Set receive interrupt bit mask to 0. See LL_INTERRUPT_POOL.         */
        handle->receiveInterruptBitMask = 0;
    }
    else
    {
        /* Receive Control Register - RCTL (0x00100; RW)                         */
        /* EN - The receiver is enabled when this bit is set to 1b. Writing this */
        /*      bit to 0b, stops reception after receipt of any in progress      */
        /*      packet. All subsequent packets are then immediately dropped      */
        /*      until this bit is set to 1b.                                     */
    
        INTEL_CLRBIT(intel_reg32(INTEL_RCTL), INTEL_RCTL_EN);
    }

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_RX_OVERLOAD_RX_OFF()");

}
#endif

#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)

#ifndef LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_INTERRUPT                          +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+                             LSA_UINT32                       IntSource  +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+  IntSource  : Interrupt Source                                          +*/
/*+                                                                         +*/
/*+               EDDS_INTERRUPT_TX                                         +*/
/*+               EDDS_INTERRUPT_RX                                         +*/
/*+               EDDS_INTERRUPT_PHY                                        +*/
/*+               others (see edd_usr.h)                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  Return       EDDS_INTERRUPT_TX                                         +*/
/*+               EDDS_INTERRUPT_RX                                         +*/
/*+               EDDS_INTERRUPT_PHY                                        +*/
/*+               EDDS_INTERRUPT_NONE                                       +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function is called by EDDS on edds_interrupt() entry. +*/
/*+               The LL can handle specific interrupts not supported by    +*/
/*+               EDDS.                                                     +*/
/*+                                                                         +*/
/*+               The EDDS itself handles TX,RX and PHY interrupts.         +*/
/*+               The LL has to return these interrupt sources. All other   +*/
/*+               interrupts have to be handled here (return NONE)          +*/
/*+                                                                         +*/
/*+               After this the EDDS calls                                 +*/
/*+                                                                         +*/
/*+               INTEL_LL_INTERRUPT_TX()  if IntSource is EDDS_INTERRUPT_TX +*/
/*+               INTEL_LL_INTERRUPT_RX()  if IntSource is EDDS_INTERRUPT_RX +*/
/*+               INTEL_LL_INTERRUPT_PHY() if IntSource is EDDS_INTERRUPT_PHY+*/
/*+                                                                         +*/
/*+               Note: If the LL needs no special handling it can define   +*/
/*+                     LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT.                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_UINT32 EDDS_LOCAL_FCT_ATTR INTEL_LL_INTERRUPT(
		LSA_VOID_PTR_TYPE pLLManagement,
		LSA_UINT32 IntSource)
{

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_INTERRUPT(IntSource: 0x%X)",
			IntSource);

	/* your code here */
	Status = EDD_STS_ERR_NOT_IMPL;
#warning	insert your code here.

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_INTERRUPT(), IntSource: 0x%X",
			IntSource);

	return(IntSource);
}
#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_INTERRUPT_TX                       +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Return       EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions is called by EDDS within edds_interrupt()  +*/
/*+               if a TX-Interrupt occured (EDDS_INTERRUPT_TX).            +*/
/*+                                                                         +*/
/*+               The LL can now acknoledge this interrupt.                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_INTERRUPT_TX(
		LSA_VOID_PTR_TYPE pLLManagement) {
	LSA_RESULT Status;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;
	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_INTERRUPT_TX(0x%08X)",handle);

	/* no acknowledge of ICR is necessary - ICR is already cleared on read in INTEL_LL_INTERRUPT_POLL */
	Status = EDD_STS_OK;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_INTERRUPT_TX(), Status: 0x%X",
			Status);

	return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_INTERRUPT_RX                       +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Return       EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions is called by EDDS within edds_interrupt()  +*/
/*+               if a RX-Interrupt occured (EDDS_INTERRUPT_RX).            +*/
/*+                                                                         +*/
/*+               The LL can now acknoledge this interrupt.                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_INTERRUPT_RX(
		LSA_VOID_PTR_TYPE pLLManagement) {
	LSA_RESULT Status;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"IN :INTEL_LL_INTERRUPT_RX()");

    /* no acknowledge of ICR is necessary - ICR is already cleared on read in INTEL_LL_INTERRUPT_POLL */
	Status = EDD_STS_OK;

	EDDS_LL_TRACE_01(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
			"OUT:INTEL_LL_INTERRUPT_RX(), Status: 0x%X",
			Status);

	return (Status);

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_INTERRUPT_POLL                     +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Return       LSA_TRUE     if interrupt more Interrupts are pending     +*/
/*+               LSA_FALSE    if interrupt no other Interrupts are pending +*/
/*+               IntSource [OUT]  (highest) pending interrupt 			    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions returns the highest pending  interrupt     +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

//lint -esym(613,IntSource)
//JB 17/11/2014 will be set within this function
LSA_BOOL EDDS_LOCAL_FCT_ATTR INTEL_LL_INTERRUPT_POLL(
		LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32* IntSource) {
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	if(0 == handle->resetState)
	{
		/* no more software pending irqs available? */
		if(0 == handle->pendingIRQs)
		{
		  /* read pending irq events from hardware; acknowledged by read! */
		  handle->pendingIRQs = intel_reg32(INTEL_ICR); /* this read acknowledges all interrupts! */
		  EDDS_LL_TRACE_02(handle->TraceIdx,LSA_TRACE_LEVEL_NOTE,"INTEL_LL_INTERRUPT_POLL(0x%08X): ICR == 0x%08X",handle,handle->pendingIRQs);
		  handle->pendingIRQs &= INTEL_BIT(INTEL_INTERRUPT_LSC,1)
			  | handle->transmitInterruptBitMask
			  | handle->receiveInterruptBitMask
			  ;

		  #if defined (EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD)
		  handle->pendingIRQs |= handle->receiveInterruptBitMask;
		  #endif
		}

		if (0 != INTEL_GETBIT(INTEL_INTERRUPT_LSC,handle->pendingIRQs))
		{
			*IntSource = EDDS_INTERRUPT_PHY;
			return (0 != (INTEL_CLRBIT(handle->pendingIRQs,INTEL_INTERRUPT_LSC)));
		}
		else if (0 != (handle->pendingIRQs & handle->transmitInterruptBitMask))
		{
			*IntSource = EDDS_INTERRUPT_TX;
			return (0 != (handle->pendingIRQs&=~(handle->transmitInterruptBitMask)));
		}
		else if (0 != (handle->pendingIRQs & handle->receiveInterruptBitMask))
		{
			*IntSource = EDDS_INTERRUPT_RX;
			return (0 != (handle->pendingIRQs&=~(handle->receiveInterruptBitMask)));
		}
		/* this should not be executed */
		*IntSource = EDDS_INTERRUPT_NONE;
	}
	else
	{
		handle->pendingIRQs = 0;	/* clear pendingIRQs, to force INTEL_ICR read after resetState is zero again. */
		*IntSource = EDDS_INTERRUPT_NONE;
	}

	return LSA_FALSE;
}
//lint +esym(613,IntSource)

#endif  /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_INTERRUPT_PHY                      +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Return       EDD_STS_OK                                                +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This functions is called by EDDS within edds_interrupt()  +*/
/*+               if a PHY-Interrupt occured (EDDS_INTERRUPT_PHY).          +*/
/*+                                                                         +*/
/*+               The LL can now acknoledge this interrupt.                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+               pLLManagement contains LLIF device structure!             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*LSA_EXTERN */LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_INTERRUPT_PHY(
		LSA_VOID_PTR_TYPE pLLManagement)
{
	LSA_RESULT Status;

	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	Status = EDD_STS_OK;

	/* no acknowledge of ICR is necessary - ICR is already cleared on read in INTEL_LL_INTERRUPT_POLL */

	/* is linkDown? sending not allowed! */
	handle->isLinkUp = (0 != INTEL_GETBIT(INTEL_STATUS_LU,intel_reg32(INTEL_STATUS))?LSA_TRUE:LSA_FALSE);

	if(0 == handle->resetState)
	{

		if(LSA_FALSE == handle->isLinkUp)
		{
			/* do a reset, set resetState for INEL_LL_TIMEOUT */
			++handle->resetState;

			{
			  LSA_UINT32 idx, count;
			  LSA_UINT32 txdctl = intel_reg32(INTEL_TXDCTL);
			  LSA_UINT32 tctl = intel_reg32(INTEL_TCTL);

			  /* is transmit unit enabled? */
			  if(0 != INTEL_GETBIT(INTEL_TCTL_EN,tctl))
			  {

				INTEL_CLRBIT(txdctl,INTEL_TXDCTL_ENABLE);  /* disable transmit queue */
				intel_reg32(INTEL_TXDCTL) = txdctl;
				INTEL_CLRBIT(tctl,INTEL_TCTL_EN);          /* disable transmit unit */
				intel_reg32(INTEL_TCTL) = tctl;

				INTEL_CLRBIT(intel_reg32(INTEL_RXDCTL),INTEL_RXDCTL_ENABLE);  /* disable receive queue */
				INTEL_CLRBIT(intel_reg32(INTEL_RCTL),INTEL_RCTL_EN);          /* disable receive unit */

				/* set all enqueued descriptors to DD */
				idx = handle->TXDescriptorIndex;
				for(count = handle->TXDescriptorsUsed; count>0; --count)
				{
				  INTEL_TDESC* tdesc = &handle->pTxDescriptors[idx];

				  /**@todo  honor cache line size and alignment to reduce number of EDDS_DMA_SYNC_FROM_DEVICE calls!
				   */
				  EDDS_DMA_SYNC_FROM_DEVICE(handle->hSysDev,tdesc,sizeof(*tdesc));

				  INTEL_SETBIT(tdesc->legacy.STA_RSV,INTEL_TDESC_STA_RSV_DD);

				  /* we didn't need to do a EDDS_DMA_SYNC_TO_DEVICE(handle->hSysDev,tdesc,sizeof(*tdesc))
				   * since the transmit unit is disabled!
				   * this is done on normal send operation in EDDS_LL_SEND.
				   */

				  ++idx;
				  if(handle->TxFrameBufferCount <= idx)
				  {
					idx = 0;
				  }
				}

				/* A fake TX interrupt is triggered after reset, to clean up all descriptors with DD set.
				 */

			  }else
			  {
				/* queue has already been disabled */
			  }
			}

			/* start LLTimer */
			edds_ll_start_timer(handle->hDDB,1);	/* wait at least 1ms */
		}
	}

	return (Status);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_TIMEOUT                            +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+  Result     :           EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_TIMEOUT                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Timeout Callback Function.                                +*/
/*+                                                                         +*/
/*+               This function is called after a LL timeout occurs.        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_TIMEOUT(
LSA_VOID_PTR_TYPE pLLManagement)
{
	LSA_RESULT Status = EDD_STS_OK;
	INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
	"IN :INTEL_LL_TIMEOUT()");

	switch(handle->resetState)
	{
	case 1:
	{

	    /** \am2219 fixme, do some __RX__ fake interrupt equivalent handling here...*/
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
		if(0 != handle->TXDescriptorsUsed)
		{
			/* set fake tx interrupt */
			EDDS_TRIGGER_SW_INTERRUPT(handle->hDDB,EDDS_INTERRUPT_TX);
		}

		/* as long as there are receive descriptors with DD set... */
		while(LSA_FALSE != intel_rdesc_done(&(handle->pRxDescriptors[handle->RXDescriptorIndex])))
		{
			/* ...finish them! */
			EDDS_TRIGGER_SW_INTERRUPT(handle->hDDB,EDDS_INTERRUPT_RX);
		}
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

		++handle->resetState;

		/* save registers */
		intel_save_volatile_registers(handle);
		intel_reset(handle,LSA_FALSE);

		/* start LLTimer */
		edds_ll_start_timer(handle->hDDB,3);	/* wait at least 3ms */
	}
		break;
	case 2:				/* reset finished, setup chip */
	{
		//LSA_RESULT status;

		if(0 != handle->TXDescriptorsUsed)
		{
#if !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE)
			/* set fake tx interrupt */
			EDDS_TRIGGER_SW_INTERRUPT(handle->hDDB,EDDS_INTERRUPT_TX);
#endif /* !defined(EDDS_CFG_CYCLIC_SCHEDULING_MODE) */

			/* start LLTimer */
			edds_ll_start_timer(handle->hDDB,1);	/* wait at least 1ms */
			break;	/* break switch case; we do this case again in at least 1ms */
		}

		Status = intel_reset_finish(handle);

		if(EDD_STS_OK == Status)
		{
			Status = intel_reset_phy(handle);

			if(EDD_STS_OK == Status)
			{
				//EDDS_ASSERT(0 == handle->TXDescriptorsUsed);
				//EDDS_ASSERT(handle->TXDescriptorIndex == handle->TXDescriptorIndexEnd);
				//EDDS_ASSERT(handle->RXDescriptorIndex == handle->lastRXHead);
				//EDDS_ASSERT(handle->RXDescriptorIndex == ((handle->RXDescriptorIndexEnd +1) % handle->RxFrameBufferCount));

				Status = intel_setup_registers(handle);

				handle->resetState = 0;

				if(EDD_STS_OK == Status)
				{
					Status = intel_set_link(handle, handle->savedLinkStat, handle->savedPHYPower);
				}
			}
		}
	}
		break;

	case 0: 			/* do nothing */
	default:
	{
		EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_UNEXP,
		"UNEXP: INTEL_LL_TIMEOUT() called without a pending event!");

		Status = EDD_STS_OK;
	}
		break;

	}

	EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
	"OUT:INTEL_LL_TIMEOUT()");

	return (Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    INTEL_LL_RECURRING_TASK                     +*/
/*+  Input/Output          :    LSA_VOID_PTR_TYPE             pLLManagement +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pLLManagement: Pointer to LLIF specific management structure           +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Recurring tasks of the LLIF can be processed in this      +*/
/*+               function.                                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_RECURRING_TASK(
LSA_VOID_PTR_TYPE pLLManagement)
{
    INTEL_LL_HANDLE_TYPE* const handle = (INTEL_LL_HANDLE_TYPE*) pLLManagement;

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
    "IN :INTEL_LL_RECURRING_TASK()");

    /* get link status change */
    if(0 != INTEL_GETBIT(INTEL_INTERRUPT_LSC,intel_reg32(INTEL_ICR)))
    {
        INTEL_LL_INTERRUPT_PHY(pLLManagement);
    }

    EDDS_LL_TRACE_00(handle->TraceIdx,LSA_TRACE_LEVEL_CHAT,
    "OUT:INTEL_LL_RECURRING_TASK()");
}

#endif

/*****************************************************************************/
/*  end of file INTEL_edd.c                                                  */
/*****************************************************************************/

