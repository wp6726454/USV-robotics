#ifndef EDDI_IRTE_H     /* ----- reinclude-protection ----- */
#define EDDI_IRTE_H

#ifdef __cplusplus      /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: EDDI (EthernetDeviceDriver for IRTE)      :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_irte.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/
/* Task / Description:                                                       */
/*  -                                                                        */
/*                                                                           */
/*****************************************************************************/
/*   Modification                                                            */
/*                                                                           */
/*   Date       Person       Modification                                    */
/*   29.09.03   Me           file created                                    */
/*                                                                           */
/*****************************************************************************/

#define  IRTE_UINT8    unsigned char
#define  IRTE_UINT32   unsigned int

//lint -save -e935

/*=================================*/
/*====== EDDI_SER_IRQ_IRT_BIT =====*/
/*=================================*/

#define    IRQ_NEW_CYCL              EDDI_PASTE_32(00,00,00,01)   // 1-Bit, new Isochron-Cycle
#define    IRQ_START_OPERATE         EDDI_PASTE_32(00,00,00,02)   // 1-Bit, start operation
#define    IRQ_IRT_TRANS_END         EDDI_PASTE_32(00,00,00,04)   // 1-Bit, IRT Transfer finished
#define    IRQ_DMA_IN                EDDI_PASTE_32(00,00,00,08)   // 1-Bit, could not finish DMA-Data-In-Transfer
#define    IRQ_DMA_OUT               EDDI_PASTE_32(00,00,00,10)   // 1-Bit, could not finish DMA_Data-Out-Transfer
#define    IRQ_IN_FAULT              EDDI_PASTE_32(00,00,00,20)   // 1-Bit, the synchronous application did not finish updating the Input-Data in time
#define    IRQ_OUT_FAULT             EDDI_PASTE_32(00,00,00,40)   // 1-Bit, the synchronous application did not finish updating the Output-Data in time
#define    IRQ_CRT_RCV_IRQ1          EDDI_PASTE_32(00,00,00,80)   // 1-Bit, receive IRT-Frame with IRT1
#define    IRQ_CRT_RCV_IRQ2          EDDI_PASTE_32(00,00,01,00)   // 1-Bit, receive IRT-Frame with IRT2
#define    IRQ_CRT_SND_IRQ1          EDDI_PASTE_32(00,00,02,00)   // 1-Bit, send IRT-Frame with IRT1
#define    IRQ_CRT_SND_IRQ2          EDDI_PASTE_32(00,00,04,00)   // 1-Bit, send IRT-Frame with IRT2

#define    IRQ_CRT_LIST_FATAL_ERR    EDDI_PASTE_32(00,00,08,00)   // 1-Bit, fatal error in FCW, TCW or IRT-List
#define    IRQ_APL_CLK               EDDI_PASTE_32(00,00,10,00)   // 1-Bit, AplClk
#define    IRQ_CLK_CY_INT            EDDI_PASTE_32(00,00,20,00)   // 1-Bit, Clkcy-Int
#define    IRQ_CLK_CY                EDDI_PASTE_32(00,00,40,00)   // 1-Bit, Clkcy
#define    IRQ_TRC_FIFO_ERR          EDDI_PASTE_32(00,00,80,00)   // 1-Bit, Trace-FIFO-Overrun
#define    IRQ_TCW_DONE              EDDI_PASTE_32(00,01,00,00)   // 1-Bit16, TCW ready
#define    IRQ_COMP1                 EDDI_PASTE_32(00,02,00,00)   // 1-Bit17, Comp1 Interrupt of AplClk
#define    IRQ_COMP2                 EDDI_PASTE_32(00,04,00,00)   // 1-Bit18, Comp2 Interrupt of AplClk
#define    IRQ_COMP3                 EDDI_PASTE_32(00,08,00,00)   // 1-Bit19, Comp3 Interrupt of AplClk
#define    IRQ_COMP4                 EDDI_PASTE_32(00,10,00,00)   // 1-Bit10, Comp4 Interrupt of AplClk

#define    IRQ_COMP5                 EDDI_PASTE_32(00,20,00,00)   // 1-Bit21, Comp5 Interrupt of AplClk
#define    IRQ_ACW_LIST_TERM         EDDI_PASTE_32(00,40,00,00)   // 1-Bit22, ACW-List Termination
#define    IRQ_ISRT_FAULT            EDDI_PASTE_32(00,80,00,00)   // 1-Bit23, NRT-Frame in the ISRT-PHASE
#define    IRQ_RX_OUT_OF_SYNC        EDDI_PASTE_32(01,00,00,00)   // 1-Bit, receive Frame out of Transfer-Phase
#define    IRQ_TX_OUT_OF_SYNC        EDDI_PASTE_32(02,00,00,00)   // 1-Bit, send Frame out of Transfer-Phase
#define    IRQ_SCORE_BOARD_SKIPPED   EDDI_PASTE_32(04,00,00,00)   // 1-Bit, can not start Remot-Producer-Control
#define    IRQ_PLL_EXT_IN            EDDI_PASTE_32(08,00,00,00)   // 1-Bit, PLL_EXT_IN signal received
#define    IRQ_SRT_CYCLE_SKIPPED     EDDI_PASTE_32(10,00,00,00)   // 1-Bit, could not send all SRT-Frames
#define    IRQ_READY_FOR_DMA_IN      EDDI_PASTE_32(20,00,00,00)   // 1-Bit, ready for DMA-Transfer of cRT-In-Data
#define    IRQ_DMA_OUT_DONE          EDDI_PASTE_32(40,00,00,00)   // 1-Bit, DMA-Transfer of cRT-Out-Data is ready
#define    IRQ_SCORE_BOARD_CHANGED   EDDI_PASTE_32(80,00,00,00)   // 1-Bit, indicate Score-Board-Change

/*=================================*/
/*====== EDDI_SER_IRQ_NRT_BIT =====*/
/*=================================*/

#define    IRQ_CHA0_SND_DONE         EDDI_PASTE_32(00,00,00,01)   // 1-Bit, Send of NRT/SRT-Frame done
#define    IRQ_CHA0_RCV_DONE         EDDI_PASTE_32(00,00,00,02)   // 1-Bit, NRT/SRT-Frame received
#define    IRQ_CHA1_SND_DONE         EDDI_PASTE_32(00,00,00,04)   // 1-Bit, Send of NRT/SRT-Frame done
#define    IRQ_CHA1_RCV_DONE         EDDI_PASTE_32(00,00,00,08)   // 1-Bit, NRT/SRT-Frame received

#define    IRQ_CHB0_SND_DONE         EDDI_PASTE_32(00,00,00,10)   // 1-Bit, Send of NRT/SRT-Frame done
#define    IRQ_CHB0_RCV_DONE         EDDI_PASTE_32(00,00,00,20)   // 1-Bit, NRT/SRT-Frame received
#define    IRQ_CHB1_SND_DONE         EDDI_PASTE_32(00,00,00,40)   // 1-Bit, Send of NRT/SRT-Frame done
#define    IRQ_CHB1_RCV_DONE         EDDI_PASTE_32(00,00,00,80)   // 1-Bit, NRT/SRT-Frame received

#define    IRQ_NRT_LIST_FATAL_ERR    EDDI_PASTE_32(00,00,01,00)   // 1-Bit, fatal error in DMACW, SCW or NRT-List
#define    IRQ_LINK_STATE_CHANGE     EDDI_PASTE_32(00,00,02,00)   // 1-Bit, link state change at one port
#define    IRQ_TRIGGER_UNIT          EDDI_PASTE_32(00,00,04,00)   // 1-Bit, Trigger by K-Bus-/SC-Bus-Errer
#define    IRQ_CLOCK_COUNTER_WRAP    EDDI_PASTE_32(00,00,08,00)   // 1-Bit, Only Rev7: Clock-Counter-Wrap

#define    IRQ_HP                    EDDI_PASTE_32(00,00,10,00)   // 1-Bit, IRQ from HP
#define    IRQ_SP                    EDDI_PASTE_32(00,00,20,00)   // 1-Bit, IRQ from SP
#define    IRQ_HOST_ACC_ERR          EDDI_PASTE_32(00,00,40,00)   // 1-Bit, wrong Adress => Accesss-Error
#define    IRQ_TIME_STAMP            EDDI_PASTE_32(00,00,80,00)   // 1-Bit, Save TimeStamp
#define    IRQ_COMMAND_DONE          EDDI_PASTE_32(00,01,00,00)   // 1-Bit, Instruction done
#define    IRQ_HOL_OVERFLOW          EDDI_PASTE_32(00,02,00,00)   // 1-Bit, Overflow of HOL
#define    IRQ_LOW_WATER_MARK        EDDI_PASTE_32(00,04,00,00)   // 1-Bit, Underflow of NRT-FCW or NRT-DB
#define    IRQ_SRT_FCW_EMPTY         EDDI_PASTE_32(00,08,00,00)   // 1-Bit, SRT-FCW-List is empty

#define    IRQ_NO_FREE_FCW           EDDI_PASTE_32(00,10,00,00)   // 1-Bit, No FCW in the FREE-FCW-LIST
#define    IRQ_NO_FREE_DB            EDDI_PASTE_32(00,20,00,00)   // 1-Bit, No DB in the FREE-DB-LIST
#define    IRQ_NO_FDB_ENTRY          EDDI_PASTE_32(00,40,00,00)   // 1-Bit, could not make a FDB-Entry
#define    IRQ_INT_A                 EDDI_PASTE_32(00,80,00,00)   // 1-Bit, reserved (no HW-support)

#define    IRQ_INT_B                 EDDI_PASTE_32(01,00,00,00)   // 1-Bit, reserved (no HW-support)
#define    IRQ_INT_C                 EDDI_PASTE_32(02,00,00,00)   // 1-Bit, reserved (no HW-support)
#define    IRQ_INT_D                 EDDI_PASTE_32(04,00,00,00)   // 1-Bit, reserved (no HW-support)
#define    IRQ_INT_E                 EDDI_PASTE_32(08,00,00,00)   // 1-Bit, reserved (no HW-support)

#define    IRQ_INT_F                 EDDI_PASTE_32(10,00,00,00)   // 1-Bit, reserved (no HW-support)
#define    IRQ_INT_G                 EDDI_PASTE_32(20,00,00,00)   // 1-Bit, reserved (no HW-support)
#define    IRQ_INT_H                 EDDI_PASTE_32(40,00,00,00)   // 1-Bit, reserved (no HW-support)
#define    IRQ_INT_I                 EDDI_PASTE_32(80,00,00,00)   // 1-Bit, reserved (no HW-support)

/*===========================================================================*/
#define EDDI_VERSION_BIT__Debug               EDDI_BIT_MASK_PARA( 7, 0)
#define EDDI_VERSION_BIT__Version             EDDI_BIT_MASK_PARA(15, 8)
#define EDDI_VERSION_BIT__MacroRevision       EDDI_BIT_MASK_PARA(23,16)
#define EDDI_VERSION_BIT__Variante            EDDI_BIT_MASK_PARA(27,24)
#define EDDI_VERSION_BIT__Location            EDDI_BIT_MASK_PARA(31,28)

#define IRTE_SET_BIT(pReg, bit)         (*(pReg) = *(pReg) | (bit))
#define IRTE_CLEAR_BIT(pReg, bit)       (*(pReg) = *(pReg) & ~(bit))

#define IRTE_INTERRUPT_MODE_BIT__R0     EDDI_PASTE_32(00,00,00,01)
#define IRTE_INTERRUPT_MODE_BIT__R1     EDDI_PASTE_32(00,00,00,02)
#define IRTE_INTERRUPT_MODE_BIT__L0     EDDI_PASTE_32(00,00,00,04)
#define IRTE_INTERRUPT_MODE_BIT__L1     EDDI_PASTE_32(00,00,00,08)

/***********************************************************************************/
/* Revision 5                                                                      */
/***********************************************************************************/

typedef struct IRTE_SW_MAKRO_S
{
    struct s00000   // IR_port
    {
        struct
        {
            // Port 0
            IRTE_UINT32        x00000_Port_Control_P0;
            IRTE_UINT32        x00004_Min_Preamble_Count_P0;
            IRTE_UINT32        x00008_IRT_Diagnose_Enable_P0;
            IRTE_UINT32        x0000c_Line_Delay_P0;
            IRTE_UINT32        x00010_NRT_Control_P0;
            IRTE_UINT32        x00014_Default_VLAN_TAG_P0;
            IRTE_UINT8         x00018_dummy[0x01000 - 0x18];
        } p0;

        struct
        {
            // Port 1
            IRTE_UINT32        x01000_Port_Control_P1;
            IRTE_UINT32        x01004_Min_Preamble_Count_P1;
            IRTE_UINT32        x01008_IRT_Diagnose_Enable_P1;
            IRTE_UINT32        x0100c_Line_Delay_P1;
            IRTE_UINT32        x01010_NRT_Control_P1;
            IRTE_UINT32        x01014_Default_VLAN_TAG_P1;
            IRTE_UINT8         x01018_dummy[0x02000 - 0x01018];
        } p1;

        struct
        {
            // Port 2
            IRTE_UINT32        x02000_Port_Control_P2;
            IRTE_UINT32        x02004_Min_Preamble_Count_P2;
            IRTE_UINT32        x02008_IRT_Diagnose_Enable_P2;
            IRTE_UINT32        x0200c_Line_Delay_P2;
            IRTE_UINT32        x02010_NRT_Control_P2;
            IRTE_UINT32        x02014_Default_VLAN_TAG_P2;
            IRTE_UINT8         x02018_dummy[0x03000 - 0x02018];
        } p2;

        struct
        {
            // Port 3
            IRTE_UINT32        x03000_Port_Control_P3;
            IRTE_UINT32        x03004_Min_Preamble_Count_P3;
            IRTE_UINT32        x03008_IRT_Diagnose_Enable_P3;
            IRTE_UINT32        x0300c_Line_Delay_P3;
            IRTE_UINT32        x03010_NRT_Control_P3;
            IRTE_UINT32        x03014_Default_VLAN_TAG_P3;
            IRTE_UINT8         x03018_dummy[0x04000 - 0x03018];
        } p3;

        IRTE_UINT8         x04000_dummy[0x8000 - 0x4000];
    } s00000_ethernet_port;

    struct s08000
    {
        // IRT-Trace-Control
        IRTE_UINT32        x08000_Diag_0_31;
        IRTE_UINT32        x08004_Diag_32_63;

        IRTE_UINT8         x08008_dummy[0x09000 - 0x08008];
    } s08000_irt_trace_control;

    struct s09000
    {
        // K-RAM - Control
        IRTE_UINT32        x09000_Count_Ctrl;
        IRTE_UINT32        x09004_Status_Ctrl;
        IRTE_UINT32        x09008_NRT_CT_Data;
        IRTE_UINT8         x0900c_dummy[0x0A000 - 0x0900c];
    } s09000_KRAM_control;


    struct s0a000
    {
        //Consistency-Control
        IRTE_UINT32        x0a000_ConsBlockReqRd;
        IRTE_UINT32        x0a004_ConsBlockAckRd;
        IRTE_UINT32        x0a008_ConsBlockReqWr;
        IRTE_UINT32        x0a00c_ConsBlockAckWr;

        IRTE_UINT8         x0a010_dummy[0x0b000 - 0x0a010];
    } s0a000_Konsistenz_control;

    struct s0b000
    {
        // IRT
        IRTE_UINT32        x0b000_IRT_StartTime_Snd_P0;
        IRTE_UINT32        x0b004_IRT_StartTime_Rcv_P0;
        IRTE_UINT32        x0b008_IRT_StartTime_Snd_P1;
        IRTE_UINT32        x0b00c_IRT_StartTime_Rcv_P1;
        IRTE_UINT32        x0b010_IRT_StartTime_Snd_P2;
        IRTE_UINT32        x0b014_IRT_StartTime_Rcv_P2;
        IRTE_UINT32        x0b018_IRT_StartTime_Snd_P3;
        IRTE_UINT32        x0b01c_IRT_StartTime_Rcv_P3;

        IRTE_UINT8         x0b020_dummy[0x10000 - 0x0b020];
    } s0b000_irt_control;

    struct s10000
    {
        //Taskmanagement
        IRTE_UINT32        x10000_Clk_Control;

        IRTE_UINT8         x10004_dummy[0x11000 - 0x10004];
    } s10000_task;

    struct s11000
    {
        //Cycle synchronisation
        IRTE_UINT32        x11000_AplClk_Divider;
        IRTE_UINT32        x11004_Clkcy_Delay;
        IRTE_UINT32        x11008_Clkcy_Int_Delay;
        IRTE_UINT32        x1100c_Clkcy_Int_Divider;
        IRTE_UINT32        x11010_Clkcy_Period;
        IRTE_UINT32        x11014_ClkGen_Command;
        IRTE_UINT32        x11018_Comperator_Value_1;
        IRTE_UINT32        x1101c_Comperator_Value_2;
        IRTE_UINT32        x11020_Comperator_Value_3;
        IRTE_UINT32        x11024_Comperator_Value_4;
        IRTE_UINT32        x11028_Comperator_Value_5;
        IRTE_UINT32        x1102c_Cycle_Length;
        IRTE_UINT32        x11030_Mux_Selektor;
        IRTE_UINT32        x11034_Phase_Correction;
        IRTE_UINT32        x11038_Cycle_Cnt_Entity;

        IRTE_UINT8         x1103c_dummy[0x11400 - 0x1103c];

        IRTE_UINT32        x11400_Cycle_Cnt_Val;
        IRTE_UINT32        x11404_Cycle_Time_Val;
        IRTE_UINT32        x11408_Clock_Cnt_Val_Begin_Ext;
        IRTE_UINT32        x1140c_AplClk_Timer;
        IRTE_UINT32        x11410_ClkGen_Status;
        IRTE_UINT32        x11414_Clock_Cnt_Val;
        IRTE_UINT32        x11418_Clock_Cnt_Val_Begin;
        IRTE_UINT32        x1141c_iSRT_Time;
        IRTE_UINT32        x11420_MAX_iSRT_Length;
        IRTE_UINT32        x11424_Clock_Cnt_Val_Snd;
        IRTE_UINT32        x11428_iSRT_OutOfBound;

        IRTE_UINT8         x1142c_dummy[0x12000 - 0x1142c];
    } s11000_Zyklussync;

    struct s12000
    {
        //NRT-Control
        IRTE_UINT32        x12000_NRT_Receive_MapCHA;
        IRTE_UINT32        x12004_NRT_Receive_MapCHB;
        IRTE_UINT32        x12008_NRT_Auto_Enable_CHB;

        IRTE_UINT8         x1200c_dummy[0x12400 - 0x1200c];

        IRTE_UINT32        x12400_NRT_Enable_CHA0;
        IRTE_UINT32        x12404_NRT_Send_Descriptor_CHA0;
        IRTE_UINT32        x12408_NRT_Receive_Descriptor_CHA0;

        IRTE_UINT32        x1240c_NRT_Enable_CHA1;
        IRTE_UINT32        x12410_NRT_Send_Descriptor_CHA1;
        IRTE_UINT32        x12414_NRT_Receive_Descriptor_CHA1;

        IRTE_UINT32        x12418_NRT_Enable_CHB0;
        IRTE_UINT32        x1241c_NRT_Send_Descriptor_CHB0;
        IRTE_UINT32        x12420_NRT_Receive_Descriptor_CHB0;

        IRTE_UINT32        x12424_NRT_Enable_CHB1;
        IRTE_UINT32        x12428_NRT_Send_Descriptor_CHB1;
        IRTE_UINT32        x1242c_NRT_Receive_Descriptor_CHB1;

        IRTE_UINT8         x12430_dummy[0x13000 - 0x12430];
    } s12000_NRT_control;

    struct s13000
    {
        //IRT-Control
        IRTE_UINT32               x13000_IRT_Control;
        IRTE_UINT32               x13004_ImageMode;
        IRTE_UINT32               x13008_ApplDataStatus_1;
        IRTE_UINT32               x1300c_ApplDataStatus_2;
        IRTE_UINT32               x13010_ApplDataStatus_3;
        IRTE_UINT32               x13014_ApplDataStatus_4;
        IRTE_UINT32               x13018_ApplDataStatus_5;
        IRTE_UINT32               x1301c_ApplDataStatus_6;
        IRTE_UINT32               x13020_ApplDataStatus_7;
        IRTE_UINT32               x13024_IRT_asyncMode;

        IRTE_UINT32               x13028_DMAInDestAdr0;
        IRTE_UINT32               x1302c_DMAInDestAdr1;
        IRTE_UINT32               x13030_DMAInSrcAdr;
        IRTE_UINT32               x13034_DMAInLength;
        IRTE_UINT32               x13038_DMAOutSrcAdr0;
        IRTE_UINT32               x1303c_DMAOutSrcAdr1;
        IRTE_UINT32               x13040_DMAOutDestAdr;
        IRTE_UINT32               x13044_DMAOutLength;
        IRTE_UINT32               x13048_IRT_TimeCompare;

        IRTE_UINT8                x1304c_dummy[0x13400 - 0x1304c];
        IRTE_UINT32               x13400_IRT_syncMode;
        IRTE_UINT32               x13404_Data_Update;

        IRTE_UINT32               x13408_IRT_Transfer_Err;
        IRTE_UINT32               x1340c_DMA_SwitchAdr;

        IRTE_UINT32               x13410_Appl_WatchDog;

        IRTE_UINT8                x13414_dummy[0x15000 - 0x13414];
    } s13000_IRT_control;

    struct s15000
    {
        // PHY
        IRTE_UINT32        x15000_MD_data;
        IRTE_UINT32        x15004_MD_CA;
        IRTE_UINT32        x15008_SMI_Configuration;
        IRTE_UINT32        x1500c_Link_Change;
    } s15000_PHY;

    struct s15010
    {
        // PHY-/SMI-Control
        IRTE_UINT32        x15010_PHY_Cmd_P0;
        IRTE_UINT32        x15014_PHY_Stat_P0;
        IRTE_UINT32        x15018_PHY_Cmd_P1;
        IRTE_UINT32        x1501c_PHY_Stat_P1;
        IRTE_UINT32        x15020_PHY_Cmd_P2;
        IRTE_UINT32        x15024_PHY_Stat_P2;
        IRTE_UINT32        x15028_PHY_Cmd_P3;
        IRTE_UINT32        x1502c_PHY_Stat_P3;

        IRTE_UINT8         x15030_dummy[0x15400 - 0x15030];
    } s15010_PHY_SMI_control;

    struct s15400   // IR_port_ctrl
    {
        struct
        {
            // Port 0
            IRTE_UINT32            x15400_P0_Pause_Cnt;            //?
            IRTE_UINT32            x15404_P0_Remote_Pause_Cnt;     //?

            IRTE_UINT8             x15408_dummy[0x15440 - 0x15408];

            IRTE_UINT32            x15440_P0_MAC_Control;
            IRTE_UINT32            x15444_P0_ARC_Control;
            IRTE_UINT32            x15448_P0_Transmit_Control;
            IRTE_UINT32            x1544c_P0_Transmit_Status;
            IRTE_UINT32            x15450_P0_Receive_Control;
            IRTE_UINT32            x15454_P0_Receive_Status;

            IRTE_UINT8             x15458_dummy[0x15460 - 0x15458];

            IRTE_UINT32            x15460_P0_ARC_Address;
            IRTE_UINT32            x15464_P0_ARC_Data;
            IRTE_UINT32            x15468_P0_ARC_Ena;

            IRTE_UINT8             x1546c_dummy[0x1547c - 0x1546c];

            IRTE_UINT32            x1547c_P0_Miss_Cnt;
        } p0;

        struct
        {
            // Port 1
            IRTE_UINT32           x15480_P1_Pause_Cnt;
            IRTE_UINT32           x15484_P1_Remote_Pause_Cnt;

            IRTE_UINT8            x15488_dummy[0x154c0 - 0x15488];

            IRTE_UINT32           x154c0_P1_MAC_Control;
            IRTE_UINT32           x154c4_P1_ARC_Control;
            IRTE_UINT32           x154c8_P1_Transmit_Control;
            IRTE_UINT32           x154cc_P1_Transmit_Status;
            IRTE_UINT32           x154d0_P1_Receive_Control;
            IRTE_UINT32           x154d4_P1_Receive_Status;

            IRTE_UINT8            x154d8_dummy[0x154e0 - 0x154d8];

            IRTE_UINT32           x154e0_P1_ARC_Address;
            IRTE_UINT32           x154e4_P1_ARC_Data;
            IRTE_UINT32           x154e8_P1_ARC_Ena;

            IRTE_UINT8            x154ec_dummy[0x154fc - 0x154ec];

            IRTE_UINT32           x154fc_P1_Miss_Cnt;
        } p1;

        struct
        {
            // Port 2
            IRTE_UINT32        x15500_P2_Pause_Cnt;               // ?
            IRTE_UINT32        x15504_P2_Remote_Pause_Cnt;        //?

            IRTE_UINT8         x15508_dummy[0x15540 - 0x15508];

            IRTE_UINT32        x15540_P2_MAC_Control;
            IRTE_UINT32        x15544_P2_ARC_Control;
            IRTE_UINT32        x15548_P2_Transmit_Control;
            IRTE_UINT32        x1554c_P2_Transmit_Status;
            IRTE_UINT32        x15550_P2_Receive_Control;
            IRTE_UINT32        x15554_P2_Receive_Status;

            IRTE_UINT8         x15558_dummy[0x15560 - 0x15558];

            IRTE_UINT32        x15560_P2_ARC_Address;
            IRTE_UINT32        x15564_P2_ARC_Data;
            IRTE_UINT32        x15568_P2_ARC_Ena;

            IRTE_UINT8         x1556c_dummy[0x1557c - 0x1556c];

            IRTE_UINT32        x1557c_P2_Miss_Cnt;
        } p2;

        struct
        {
            // Port 3
            IRTE_UINT32        x15580_P3_Pause_Cnt;
            IRTE_UINT32        x15584_P3_Remote_Pause_Cnt;

            IRTE_UINT8         x15588_dummy[0x155c0 - 0x15588];

            IRTE_UINT32        x155c0_P3_MAC_Control;
            IRTE_UINT32        x155c4_P3_ARC_Control;
            IRTE_UINT32        x155c8_P3_Transmit_Control;
            IRTE_UINT32        x155cc_P3_Transmit_Status;
            IRTE_UINT32        x155d0_P3_Receive_Control;
            IRTE_UINT32        x155d4_P3_Receive_Status;

            IRTE_UINT8         x155d8_dummy[0x155e0 - 0x155d8];

            IRTE_UINT32        x155e0_P3_ARC_Address;
            IRTE_UINT32        x155e4_P3_ARC_Data;
            IRTE_UINT32        x155e8_P3_ARC_Ena;

            IRTE_UINT8         x155ec_dummy[0x155fc - 0x155ec];

            IRTE_UINT32        x155fc_P3_Miss_Cnt;

            IRTE_UINT8         x15600_dummy[0x16000 - 0x15600];
        } p3;
    } s15400_port;

    struct s16000
    {
        //K-RAM Control
        IRTE_UINT32                   x16000_NRT_FCW_Limit;
        IRTE_UINT32                   x16004_NRT_DB_Limit;
        IRTE_UINT32                   x16008_HOL_Limit_CH;
        IRTE_UINT32                   x1600c_HOL_Limit_Port;

        IRTE_UINT32                   x16010_UCMC_Table_Base;
        IRTE_UINT32                   x16014_UCMC_Table_Length;
        IRTE_UINT32                   x16018_UCMC_LFSR_Mask;
        IRTE_UINT32                   x1601c_UCMC_Table_Range;

        IRTE_UINT32                   x16020_UC_Default_Ctrl;
        IRTE_UINT32                   x16024_MC_Default_Ctrl;
        IRTE_UINT32                   x16028_VLAN_Table_Base;
        IRTE_UINT32                   x1602c_VLAN_Table_Length;

        IRTE_UINT32                   x16030_VLAN_LFSR_Ctrl;
        IRTE_UINT32                   x16034_Time_Scoreboard_Base;
        IRTE_UINT32                   x16038_Time_Scoreboard_Entries;
        IRTE_UINT32                   x1603c_PS_Scoreboard_0_Base;

        IRTE_UINT32                   x16040_PS_Scoreboard_1_Base;
        IRTE_UINT32                   x16044_FC_Mask;
        IRTE_UINT32                   x16048_HOL_Mask_P0;
        IRTE_UINT32                   x1604c_HOL_Mask_P1;

        IRTE_UINT32                   x16050_HOL_Mask_P2;
        IRTE_UINT32                   x16054_HOL_Mask_P3;

        IRTE_UINT8                    x16058_dummy[0x16400 - 0x16058];

        IRTE_UINT32                   x16400_PRIM_Command;
        IRTE_UINT32                   x16404_SEC_Command;
        IRTE_UINT32                   x16408_Command_Result;
        IRTE_UINT32                   x1640c_Port_Control;

        IRTE_UINT32                   x16410_Flow_Control;
        IRTE_UINT32                   x16414_Producer_Control;
        IRTE_UINT32                   x16418_SS_Queue_Disable;
        IRTE_UINT32                   x1641c_ACW_List_Status;

        IRTE_UINT32                   x16420_NRT_FCW_Count;
        IRTE_UINT32                   x16424_NRT_DB_Count;
        IRTE_UINT32                   x16428_SRT_FCW_Count;
        IRTE_UINT32                   x1642c_HOL_Count_CHA_SS0;

        IRTE_UINT32                   x16430_HOL_Count_CHA_SS1;
        IRTE_UINT32                   x16434_List_Status_CHA;
        IRTE_UINT32                   x16438_HOL_Count_CHB_SS0;
        IRTE_UINT32                   x1643c_HOL_Count_CHB_SS1;

        IRTE_UINT32                   x16440_List_Status_CHB;
        IRTE_UINT32                   x16444_HOL_Count_P0;
        IRTE_UINT32                   x16448_List_Status_P0;
        IRTE_UINT32                   x1644c_HOL_Count_P1;

        IRTE_UINT32                   x16450_List_Status_P1;
        IRTE_UINT32                   x16454_HOL_Count_P2;
        IRTE_UINT32                   x16458_List_Status_P2;
        IRTE_UINT32                   x1645c_HOL_Count_P3;

        IRTE_UINT32                   x16460_List_Status_P3;

        IRTE_UINT8                    x16464_dummy[0x17000 - 0x16464];
    } s16000_KRAM_control;

    struct s17000
    {
        // Interrupt Controller
        IRTE_UINT32                    x17000_Interrupt_Mode_Register;
        IRTE_UINT32                    x17004_Interrupt_MaskRegister_IRT_IRQ0;
        IRTE_UINT32                    x17008_Interrupt_MaskRegister_NRT_IRQ0;
        IRTE_UINT32                    x1700c_Interrupt_MaskRegister_IRT_IRQ1;

        IRTE_UINT32                    x17010_Interrupt_MaskRegister_NRT_IRQ1;
    } s17000_interrupt_controller;

    struct s17014
    {
        // Switch Prozessor
        IRTE_UINT32                    x17014_SW_Interrupt_Mode_Register;
        IRTE_UINT32                    x17018_SW_Interrupt_MaskRegister_IRT_IRQ0;
        IRTE_UINT32                    x1701c_SW_Interrupt_MaskRegister_NRT_IRQ0;

        IRTE_UINT32                    x17020_SW_Interrupt_MaskRegister_IRT_IRQ1;
        IRTE_UINT32                    x17024_SW_Interrupt_MaskRegister_NRT_IRQ1;

        IRTE_UINT8                     x17028_dummy[0x17400 - 0x17028];
    } s17014_switch_prozessor;

    struct s17400
    {
        // Interrupt Controller
        IRTE_UINT32             x17400_Interrupt_Register_IRT_IRQ0;
        IRTE_UINT32             x17404_Interrupt_Register_NRT_IRQ0;
        IRTE_UINT32             x17408_Interrupt_Register_IRT_IRQ1;
        IRTE_UINT32             x1740c_Interrupt_Register_NRT_IRQ1;

        IRTE_UINT32             x17410_Interrupt_Ackn_Register_IRT;
        IRTE_UINT32             x17414_Interrupt_Ackn_Register_NRT;
        IRTE_UINT32             x17418_Interrupt_Request_Register_IRT;
        IRTE_UINT32             x1741c_Interrupt_Request_Register_NRT;

        IRTE_UINT32             x17420_EOI_IRQ0_Register;
        IRTE_UINT32             x17424_EOI_IRQ1_Register;

    } s17400_interrupt_controller;

    struct s17430
    {
        // Switch Prozessor
        IRTE_UINT32             x17428_SW_Interrupt_Register_IRT_IRQ0;
        IRTE_UINT32             x1742c_SW_Interrupt_Register_NRT_IRQ0;
        IRTE_UINT32             x17430_SW_Interrupt_Register_IRT_IRQ1;
        IRTE_UINT32             x17434_SW_Interrupt_Register_NRT_IRQ1;

        IRTE_UINT32             x17438_SW_Interrupt_Ackn_Register_IRT;
        IRTE_UINT32             x1743c_SW_Interrupt_Ackn_Register_NRT;
        IRTE_UINT32             x17440_SW_Interrupt_Request_Register_IRT;
        IRTE_UINT32             x17444_SW_Interrupt_Request_Register_NRT;


        IRTE_UINT32             x17448_SW_EOI_IRQ0_Register;
        IRTE_UINT32             x1744c_SW_EOI_IRQ1_Register;

        IRTE_UINT32             x17450_Activate_HP_Interrupt;
        IRTE_UINT32             x17454_Activate_SP_Interrupt;

        IRTE_UINT8              x17458_dummy[0x18000 - 0x17458];
    } s17430_switch_prozessor;

    // IRT-Trace-Control
    struct s18000
    {
        IRTE_UINT32             x18000_Trace_Base_Address;
        IRTE_UINT32             x18004_Trace_Buffer_Length;
        IRTE_UINT32             x18008_Trace_Transfer_Limit;
        IRTE_UINT8              x1800c_dummy[0x18400 - 0x1800c];

        IRTE_UINT32             x18400_Trace_Command_Register;
        IRTE_UINT32             x18404_Trace_Mode;
        IRTE_UINT32             x18408_Trace_Entry_Counter;
        IRTE_UINT32             x1840c_Trace_Fault_Counter;

        IRTE_UINT8              x18410_dummy[0x19000 - 0x18410];
    } s18000_irt_trace_control;

    // Global parameters
    struct s19000
    {
        IRTE_UINT32                     x19000_Ether_RT_Type_0;
        IRTE_UINT32                     x19004_Ether_RT_Type_1;
        IRTE_UINT32                     x19008_Ether_RT_Type_2;
        IRTE_UINT32                     x1900c_Ether_RT_Type_3;
        IRTE_UINT32                     x19010_IRT_Current_Cycle_ID;

        IRTE_UINT32                     x19014_IRT_DA_0_31;
        IRTE_UINT32                     x19018_IRT_DA_32_47;
        IRTE_UINT32                     x1901c_SA_0_31;

        IRTE_UINT32                     x19020_SA_32_47;
        IRTE_UINT32                     x19024_IRT_Ctrl_Base;
        IRTE_UINT32                     x19028_NRT_Ctrl_Base;
        IRTE_UINT32                     x1902c_Free_Ctrl_Base;

        IRTE_UINT32                     x19030_ACW_List_Base;
        IRTE_UINT32                     x19034_En_ACW_Offset;
        IRTE_UINT32                     x19038_Switch_Control;
        IRTE_UINT32                     x1903c_Min_idle_Time;

        IRTE_UINT32                     x19040_MAC_Empty_Count;
        IRTE_UINT32                     x19044_NRT_Transfer_Control;
        IRTE_UINT32                     x19048_NRT_SaftyMargin;
        IRTE_UINT32                     x1904c_Monitor_Control;

        IRTE_UINT32                     x19050_Statistic_Ctrl_Base;
        IRTE_UINT32                     x19054_Switch_Setup;

        IRTE_UINT8                      x19058_dummy[0x19400 - 0x19058];

        IRTE_UINT32                     x19400_Version_Number;
        IRTE_UINT32                     x19404_Switch_Status;
        IRTE_UINT32                     x19408_Activity_Status;

        IRTE_UINT8                      x1940c_dummy[0x1a000 - 0x1940c];
    } s19000_global_para;

    // Trigger Unit
    struct s1a000
    {
        IRTE_UINT32        x1a000_Trigger_Control;
        IRTE_UINT32        x1a004_Trigger_Address_Mask;
        IRTE_UINT32        x1a008_Trigger_Lower_Data_Mask;
        IRTE_UINT32        x1a00c_Trigger_Upper_Data_Mask;

        IRTE_UINT8         x1a010_dummy[0x1a400 - 0x1a010];

        IRTE_UINT32        x1a400_Trigger_Address;
        IRTE_UINT32        x1a404_Trigger_Lower_Data;
        IRTE_UINT32        x1a408_Trigger_Upper_Data;
        IRTE_UINT32        x1a40c_Trigger_Info;

        IRTE_UINT32        x1a410_Trigger_Reset;

        IRTE_UINT8         x1a414_dummy[0x1b000 - 0x1a414];

    } s1a000_trigger_unit;

    // Statistic-Control
    struct s1b000
    {
        struct
        {
            IRTE_UINT32        x1b000_Good_Bytes_Received;
            IRTE_UINT32        x1b004_Total_Bytes_Received;
            IRTE_UINT32        x1b008_Bytes_Sent;
            IRTE_UINT32        x1b00c_Frames_Sent;

            IRTE_UINT32        x1b010_Unicast_Frames_Received;
            IRTE_UINT32        x1b014_Unicast_Frames_Received_un;
            IRTE_UINT32        x1b018_Multicast_Frames_Received;
            IRTE_UINT32        x1b01c_Multicast_Frames_Received_un;

            IRTE_UINT32        x1b020_Broadcast_Frames_Received;
            IRTE_UINT32        x1b024_Unicast_Frames_Sent;
            IRTE_UINT32        x1b028_Multicast_Frames_Sent;
            IRTE_UINT32        x1b02c_Broadcast_Frames_Sent;

            IRTE_UINT32        x1b030_Frames_under_64Bytest;
            IRTE_UINT32        x1b034_Frames_with_length_of_64Bytes;
            IRTE_UINT32        x1b038_Frames_between_65_127Bytes;
            IRTE_UINT32        x1b03c_Frames_between_128_255Bytes;

            IRTE_UINT32        x1b040_Frames_between_256_511Bytes;
            IRTE_UINT32        x1b044_Frames_between_512_1023Bytes;
            IRTE_UINT32        x1b048_Frames_between_1024_MAXBytes;
            IRTE_UINT32        x1b04c_Good_Frames_Received;

            IRTE_UINT32        x1b050_Total_Frames_Received;
            IRTE_UINT32        x1b054_Received_Frames_64Bytes_with_bad_CRC;
            IRTE_UINT32        x1b058_Rx_Errors;
            IRTE_UINT32        x1b05c_Oversize_Frames;

            IRTE_UINT32        x1b060_CRC_and_Alignment_Error;
            IRTE_UINT32        x1b064_Frames_Dropped;
            IRTE_UINT32        x1b068_Collision;
            IRTE_UINT32        x1b06c_HOL_boundary_crossed;

            IRTE_UINT8         x1b070_dummy[0x1b100 - 0x1b070];
        } p0;

        struct
        {
            IRTE_UINT32        x1b100_Good_Bytes_Received;
            IRTE_UINT32        x1b104_Total_Bytes_Received;
            IRTE_UINT32        x1b108_Bytes_Sent;
            IRTE_UINT32        x1b10c_Frames_Sent;

            IRTE_UINT32        x1b110_Unicast_Frames_Received;
            IRTE_UINT32        x1b114_Unicast_Frames_Received_un;
            IRTE_UINT32        x1b118_Multicast_Frames_Received;
            IRTE_UINT32        x1b11c_Multicast_Frames_Received_un;

            IRTE_UINT32        x1b120_Broadcast_Frames_Received;
            IRTE_UINT32        x1b124_Unicast_Frames_Sent;
            IRTE_UINT32        x1b128_Multicast_Frames_Sent;
            IRTE_UINT32        x1b12c_Broadcast_Frames_Sent;

            IRTE_UINT32        x1b130_Frames_under_64Bytest;
            IRTE_UINT32        x1b134_Frames_with_length_of_64Bytes;
            IRTE_UINT32        x1b138_Frames_between_65_127Bytes;
            IRTE_UINT32        x1b13c_Frames_between_128_255Bytes;

            IRTE_UINT32        x1b140_Frames_between_256_511Bytes;
            IRTE_UINT32        x1b144_Frames_between_512_1023Bytes;
            IRTE_UINT32        x1b148_Frames_between_1024_MAXBytes;
            IRTE_UINT32        x1b14c_Good_Frames_Received;

            IRTE_UINT32        x1b150_Total_Frames_Received;
            IRTE_UINT32        x1b154_Received_Frames_64Bytes_with_bad_CRC;
            IRTE_UINT32        x1b158_Rx_Errors;
            IRTE_UINT32        x1b15c_Oversize_Frames;

            IRTE_UINT32        x1b160_CRC_and_Alignment_Error;
            IRTE_UINT32        x1b164_Frames_Dropped;
            IRTE_UINT32        x1b168_Collision;
            IRTE_UINT32        x1b16c_HOL_boundary_crossed;

            IRTE_UINT8         x1b170_dummy[0x1b200 - 0x1b170];
        } p1;

        struct
        {
            IRTE_UINT32        x1b200_Good_Bytes_Received;
            IRTE_UINT32        x1b204_Total_Bytes_Received;
            IRTE_UINT32        x1b208_Bytes_Sent;
            IRTE_UINT32        x1b20c_Frames_Sent;

            IRTE_UINT32        x1b210_Unicast_Frames_Received;
            IRTE_UINT32        x1b214_Unicast_Frames_Received_un;
            IRTE_UINT32        x1b218_Multicast_Frames_Received;
            IRTE_UINT32        x1b21c_Multicast_Frames_Received_un;

            IRTE_UINT32        x1b220_Broadcast_Frames_Received;
            IRTE_UINT32        x1b224_Unicast_Frames_Sent;
            IRTE_UINT32        x1b228_Multicast_Frames_Sent;
            IRTE_UINT32        x1b22c_Broadcast_Frames_Sent;

            IRTE_UINT32        x1b230_Frames_under_64Bytest;
            IRTE_UINT32        x1b234_Frames_with_length_of_64Bytes;
            IRTE_UINT32        x1b238_Frames_between_65_127Bytes;
            IRTE_UINT32        x1b23c_Frames_between_128_255Bytes;

            IRTE_UINT32        x1b240_Frames_between_256_511Bytes;
            IRTE_UINT32        x1b244_Frames_between_512_1023Bytes;
            IRTE_UINT32        x1b248_Frames_between_1024_MAXBytes;
            IRTE_UINT32        x1b24c_Good_Frames_Received;

            IRTE_UINT32        x1b250_Total_Frames_Received;
            IRTE_UINT32        x1b254_Received_Frames_64Bytes_with_bad_CRC;
            IRTE_UINT32        x1b258_Rx_Errors;
            IRTE_UINT32        x1b25c_Oversize_Frames;

            IRTE_UINT32        x1b260_CRC_and_Alignment_Error;
            IRTE_UINT32        x1b264_Frames_Dropped;
            IRTE_UINT32        x1b268_Collision;
            IRTE_UINT32        x1b26c_HOL_boundary_crossed;

            IRTE_UINT8         x1b270_dummy[0x1b300 - 0x1b270];
        } p2;

        struct
        {
            IRTE_UINT32        x1b300_Good_Bytes_Received;
            IRTE_UINT32        x1b304_Total_Bytes_Received;
            IRTE_UINT32        x1b308_Bytes_Sent;
            IRTE_UINT32        x1b30c_Frames_Sent;

            IRTE_UINT32        x1b310_Unicast_Frames_Received;
            IRTE_UINT32        x1b314_Unicast_Frames_Received_un;
            IRTE_UINT32        x1b318_Multicast_Frames_Received;
            IRTE_UINT32        x1b31c_Multicast_Frames_Received_un;

            IRTE_UINT32        x1b320_Broadcast_Frames_Received;
            IRTE_UINT32        x1b324_Unicast_Frames_Sent;
            IRTE_UINT32        x1b328_Multicast_Frames_Sent;
            IRTE_UINT32        x1b32c_Broadcast_Frames_Sent;

            IRTE_UINT32        x1b330_Frames_under_64Bytest;
            IRTE_UINT32        x1b334_Frames_with_length_of_64Bytes;
            IRTE_UINT32        x1b338_Frames_between_65_127Bytes;
            IRTE_UINT32        x1b33c_Frames_between_128_255Bytes;

            IRTE_UINT32        x1b340_Frames_between_256_511Bytes;
            IRTE_UINT32        x1b344_Frames_between_512_1023Bytes;
            IRTE_UINT32        x1b348_Frames_between_1024_MAXBytes;
            IRTE_UINT32        x1b34c_Good_Frames_Received;

            IRTE_UINT32        x1b350_Total_Frames_Received;
            IRTE_UINT32        x1b354_Received_Frames_64Bytes_with_bad_CRC;
            IRTE_UINT32        x1b358_Rx_Errors;
            IRTE_UINT32        x1b35c_Oversize_Frames;

            IRTE_UINT32        x1b360_CRC_and_Alignment_Error;
            IRTE_UINT32        x1b364_Frames_Dropped;
            IRTE_UINT32        x1b368_Collision;
            IRTE_UINT32        x1b36c_HOL_boundary_crossed;

            IRTE_UINT8         x1b370_dummy[0x1b400 - 0x1b370];
        } p3;

    } s1b000_statistic_control;

} IRTE_SW_MAKRO_T;

/***********************************************************************************/
/* Revision 6                                                                      */
/***********************************************************************************/

typedef struct IRTE_SW_MAKRO_REV6_S
{
    struct s00000_r6    // IR_port
    {
        struct
        {
            // Port 0
            IRTE_UINT32        x00000_Port_Control_P0;
            IRTE_UINT32        x00004_Min_Preamble_Count_P0;
            IRTE_UINT32        x00008_IRT_Diagnose_Enable_P0;
            IRTE_UINT32        x0000c_Line_Delay_P0;
            IRTE_UINT32        x00010_NRT_Control_P0;
            IRTE_UINT32        x00014_Default_VLAN_TAG_P0;
            IRTE_UINT8         x00018_dummy[0x01000 - 0x18];
        } p0;

        struct
        {
            // Port 1
            IRTE_UINT32        x01000_Port_Control_P1;
            IRTE_UINT32        x01004_Min_Preamble_Count_P1;
            IRTE_UINT32        x01008_IRT_Diagnose_Enable_P1;
            IRTE_UINT32        x0100c_Line_Delay_P1;
            IRTE_UINT32        x01010_NRT_Control_P1;
            IRTE_UINT32        x01014_Default_VLAN_TAG_P1;
            IRTE_UINT8         x01018_dummy[0x02000 - 0x01018];
        } p1;

        struct
        {
            // Port 2
            IRTE_UINT32        x02000_Port_Control_P2;
            IRTE_UINT32        x02004_Min_Preamble_Count_P2;
            IRTE_UINT32        x02008_IRT_Diagnose_Enable_P2;
            IRTE_UINT32        x0200c_Line_Delay_P2;
            IRTE_UINT32        x02010_NRT_Control_P2;
            IRTE_UINT32        x02014_Default_VLAN_TAG_P2;
            IRTE_UINT8         x02018_dummy[0x03000 - 0x02018];
        } p2;

        struct
        {
            // Port 3
            IRTE_UINT32        x03000_Port_Control_P3;
            IRTE_UINT32        x03004_Min_Preamble_Count_P3;
            IRTE_UINT32        x03008_IRT_Diagnose_Enable_P3;
            IRTE_UINT32        x0300c_Line_Delay_P3;
            IRTE_UINT32        x03010_NRT_Control_P3;
            IRTE_UINT32        x03014_Default_VLAN_TAG_P3;
            IRTE_UINT8         x03018_dummy[0x04000 - 0x03018];
        } p3;

        IRTE_UINT8          x04000_dummy[0x8000 - 0x4000];
    } s00000_ethernet_port;

    struct s08000_r6
    {
        // IRT-Trace-Control
        IRTE_UINT32        x08000_Diag_0_31;
        IRTE_UINT32        x08004_Diag_32_63;

        IRTE_UINT8         x08008_dummy[0x09000 - 0x08008];
    } s08000_irt_trace_control;

    struct s09000_r6
    {
        // K-RAM - Control
        IRTE_UINT32        x09000_Count_Ctrl;
        IRTE_UINT32        x09004_Status_Ctrl;
        IRTE_UINT32        x09008_NRT_CT_Data;
        IRTE_UINT8         x0900c_dummy[0x0A000 - 0x0900c];
    } s09000_KRAM_control;


    struct s0a000_r6
    {
        //Concistency- Control
        IRTE_UINT32        x0a000_ConsBlockReqRd;
        IRTE_UINT32        x0a004_ConsBlockAckRd;
        IRTE_UINT32        x0a008_ConsBlockReqWr;
        IRTE_UINT32        x0a00c_ConsBlockAckWr;

        IRTE_UINT8         x0a010_dummy[0x0a400 - 0x0a010];

        IRTE_UINT32        x0a400_ConsPortAddrRd;
        IRTE_UINT32        x0a404_ConsPortAddrWr;
        IRTE_UINT32        x0a408_ConsPortLockAddr;

        IRTE_UINT8         x0a40c_dummy[0x0b000 - 0x0a40c];
    } s0a000_Konsistenz_control;

    struct s0b000_r6
    {
        // IRT
        IRTE_UINT32        x0b000_IRT_StartTime_Snd_P0;
        IRTE_UINT32        x0b004_IRT_StartTime_Rcv_P0;
        IRTE_UINT32        x0b008_IRT_StartTime_Snd_P1;
        IRTE_UINT32        x0b00c_IRT_StartTime_Rcv_P1;
        IRTE_UINT32        x0b010_IRT_StartTime_Snd_P2;
        IRTE_UINT32        x0b014_IRT_StartTime_Rcv_P2;
        IRTE_UINT32        x0b018_IRT_StartTime_Snd_P3;
        IRTE_UINT32        x0b01c_IRT_StartTime_Rcv_P3;

        IRTE_UINT8         x0b020_dummy[0x10000 - 0x0b020];
    } s0b000_irt_control;

    struct s10000_r6
    {
        //Taskmanagement
        IRTE_UINT32        x10000_Clk_Control;

        IRTE_UINT8         x10004_dummy[0x11000 - 0x10004];
    } s10000_task;

    struct s11000_r6
    {
        //Cycle synchronisation
        IRTE_UINT32        x11000_AplClk_Divider;
        IRTE_UINT32        x11004_Clkcy_Delay;
        IRTE_UINT32        x11008_Clkcy_Int_Delay;
        IRTE_UINT32        x1100c_Clkcy_Int_Divider;
        IRTE_UINT32        x11010_Clkcy_Period;
        IRTE_UINT32        x11014_ClkGen_Command;
        IRTE_UINT32        x11018_Comperator_Value_1;
        IRTE_UINT32        x1101c_Comperator_Value_2;
        IRTE_UINT32        x11020_Comperator_Value_3;
        IRTE_UINT32        x11024_Comperator_Value_4;
        IRTE_UINT32        x11028_Comperator_Value_5;
        IRTE_UINT32        x1102c_Cycle_Length;
        IRTE_UINT32        x11030_Mux_Selektor;
        IRTE_UINT32        x11034_Phase_Correction;
        IRTE_UINT32        x11038_Drift_Correction;
        IRTE_UINT32        x1103c_Cycle_Cnt_Entity;
        IRTE_UINT32        x11040_Cycle_FrameID;

        IRTE_UINT8         x11044_dummy[0x11400 - 0x11044];

        IRTE_UINT32        x11400_Cycle_Cnt_Val;
        IRTE_UINT32        x11404_Cycle_Cnt_Val_47_32;
        IRTE_UINT32        x11408_Cycle_Time_Val;

        IRTE_UINT32        x1140c_Clock_Cnt_Val_Begin_Ext;
        IRTE_UINT32        x11410_AplClk_Timer;
        IRTE_UINT32        x11414_ClkGen_Status;
        IRTE_UINT32        x11418_Clock_Cnt_Val;
        IRTE_UINT32        x1141c_Clock_Cnt_Val_Begin;
        IRTE_UINT32        x11420_iSRT_Time;
        IRTE_UINT32        x11424_MAX_iSRT_Length;
        IRTE_UINT32        x11428_Clock_Cnt_Val_Snd;
        IRTE_UINT32        x1142c_iSRT_OutOfBound;

        IRTE_UINT8         x11430_dummy[0x12000 - 0x11430];
    } s11000_Zyklussync;

    struct s12000_r6
    {
        //NRT-Control
        IRTE_UINT32        x12000_NRT_Receive_MapCHA;
        IRTE_UINT32        x12004_NRT_Receive_MapCHB;
        IRTE_UINT32        x12008_NRT_Auto_Enable_CHB;
        IRTE_UINT32        x1200c_ARP_Table_Base;
        IRTE_UINT32        x12010_ARP_Table_Length;
        IRTE_UINT32        x12014_DCP_Table_Base;

        IRTE_UINT8         x12018_dummy[0x12400 - 0x12018];

        IRTE_UINT32        x12400_NRT_Enable_CHA0;
        IRTE_UINT32        x12404_NRT_Send_Descriptor_CHA0;
        IRTE_UINT32        x12408_NRT_Receive_Descriptor_CHA0;

        IRTE_UINT32        x1240c_NRT_Enable_CHA1;
        IRTE_UINT32        x12410_NRT_Send_Descriptor_CHA1;
        IRTE_UINT32        x12414_NRT_Receive_Descriptor_CHA1;

        IRTE_UINT32        x12418_NRT_Enable_CHB0;
        IRTE_UINT32        x1241c_NRT_Send_Descriptor_CHB0;
        IRTE_UINT32        x12420_NRT_Receive_Descriptor_CHB0;

        IRTE_UINT32        x12424_NRT_Enable_CHB1;
        IRTE_UINT32        x12428_NRT_Send_Descriptor_CHB1;
        IRTE_UINT32        x1242c_NRT_Receive_Descriptor_CHB1;

        IRTE_UINT8         x12430_dummy[0x13000 - 0x12430];
    } s12000_NRT_control;

    struct s13000_r6
    {
        //IRT-Control
        IRTE_UINT32               x13000_IRT_Control;
        IRTE_UINT32               x13004_ImageMode;

        IRTE_UINT32               x13008_ApplDataStatus_Mask;
        IRTE_UINT32               x1300c_ApplDataStatus_1;
        IRTE_UINT32               x13010_ApplDataStatus_2;
        IRTE_UINT32               x13014_ApplDataStatus_3;
        IRTE_UINT32               x13018_ApplDataStatus_4;
        IRTE_UINT32               x1301c_ApplDataStatus_5;
        IRTE_UINT32               x13020_ApplDataStatus_6;
        IRTE_UINT32               x13024_ApplDataStatus_7;
        IRTE_UINT32               x13028_IRT_asyncMode;

        IRTE_UINT32               x1302c_DMAInDestAdr0;
        IRTE_UINT32               x13030_DMAInDestAdr1;
        IRTE_UINT32               x13034_DMAInSrcAdr;
        IRTE_UINT32               x13038_DMAInLength;
        IRTE_UINT32               x1303c_DMAOutSrcAdr0;
        IRTE_UINT32               x13040_DMAOutSrcAdr1;
        IRTE_UINT32               x13044_DMAOutDestAdr;
        IRTE_UINT32               x13048_DMAOutLength;
        IRTE_UINT32               x1304c_IRT_TimeCompare;

        IRTE_UINT8                x13050_dummy[0x13400 - 0x13050];
        IRTE_UINT32               x13400_IRT_syncMode;
        IRTE_UINT32               x13404_Data_Update;
        IRTE_UINT32               x13408_IRT_Transfer_Err;
        IRTE_UINT32               x1340c_DMA_SwitchAdr;

        IRTE_UINT32               x13410_Appl_WatchDog;

        IRTE_UINT8                x13414_dummy[0x15000 - 0x13414];
    } s13000_IRT_control;

    struct s15000_r6
    {
        // PHY
        IRTE_UINT32        x15000_MD_data;
        IRTE_UINT32        x15004_MD_CA;
        IRTE_UINT32        x15008_SMI_Configuration;
        IRTE_UINT32        x1500c_Link_Change;
    } s15000_PHY;

    struct s15010_r6
    {
        // PHY-/SMI-Control
        IRTE_UINT32        x15010_PHY_Cmd_P0;
        IRTE_UINT32        x15014_PHY_Stat_P0;
        IRTE_UINT32        x15018_PHY_Cmd_P1;
        IRTE_UINT32        x1501c_PHY_Stat_P1;
        IRTE_UINT32        x15020_PHY_Cmd_P2;
        IRTE_UINT32        x15024_PHY_Stat_P2;
        IRTE_UINT32        x15028_PHY_Cmd_P3;
        IRTE_UINT32        x1502c_PHY_Stat_P3;

        IRTE_UINT8         x15030_dummy[0x15400 - 0x15030];
    } s15010_PHY_SMI_control;

    struct s15400_r6   // IR_port_ctrl
    {
        struct
        {
            // Port 0
            IRTE_UINT32        x15400_P0_Pause_Cnt;            //?
            IRTE_UINT32        x15404_P0_Remote_Pause_Cnt;     //?

            IRTE_UINT8         x15408_dummy[0x15440 - 0x15408];

            IRTE_UINT32        x15440_P0_MAC_Control;
            IRTE_UINT32        x15444_P0_ARC_Control;
            IRTE_UINT32        x15448_P0_Transmit_Control;
            IRTE_UINT32        x1544c_P0_Transmit_Status;
            IRTE_UINT32        x15450_P0_Receive_Control;
            IRTE_UINT32        x15454_P0_Receive_Status;

            IRTE_UINT8         x15458_dummy[0x15460 - 0x15458];

            IRTE_UINT32        x15460_P0_ARC_Address;
            IRTE_UINT32        x15464_P0_ARC_Data;
            IRTE_UINT32        x15468_P0_ARC_Ena;

            IRTE_UINT8         x1546c_dummy[0x1547c - 0x1546c];

            IRTE_UINT32        x1547c_P0_Miss_Cnt;
        } p0;

        struct
        {
            // Port 1
            IRTE_UINT32        x15480_P1_Pause_Cnt;
            IRTE_UINT32        x15484_P1_Remote_Pause_Cnt;

            IRTE_UINT8         x15488_dummy[0x154c0 - 0x15488];

            IRTE_UINT32        x154c0_P1_MAC_Control;
            IRTE_UINT32        x154c4_P1_ARC_Control;
            IRTE_UINT32        x154c8_P1_Transmit_Control;
            IRTE_UINT32        x154cc_P1_Transmit_Status;
            IRTE_UINT32        x154d0_P1_Receive_Control;
            IRTE_UINT32        x154d4_P1_Receive_Status;

            IRTE_UINT8         x154d8_dummy[0x154e0 - 0x154d8];

            IRTE_UINT32        x154e0_P1_ARC_Address;
            IRTE_UINT32        x154e4_P1_ARC_Data;
            IRTE_UINT32        x154e8_P1_ARC_Ena;

            IRTE_UINT8         x154ec_dummy[0x154fc - 0x154ec];

            IRTE_UINT32        x154fc_P1_Miss_Cnt;
        } p1;

        struct
        {
            // Port 2
            IRTE_UINT32        x15500_P2_Pause_Cnt;               // ?
            IRTE_UINT32        x15504_P2_Remote_Pause_Cnt;        //?

            IRTE_UINT8         x15508_dummy[0x15540 - 0x15508];

            IRTE_UINT32        x15540_P2_MAC_Control;
            IRTE_UINT32        x15544_P2_ARC_Control;
            IRTE_UINT32        x15548_P2_Transmit_Control;
            IRTE_UINT32        x1554c_P2_Transmit_Status;
            IRTE_UINT32        x15550_P2_Receive_Control;
            IRTE_UINT32        x15554_P2_Receive_Status;

            IRTE_UINT8         x15558_dummy[0x15560 - 0x15558];

            IRTE_UINT32        x15560_P2_ARC_Address;
            IRTE_UINT32        x15564_P2_ARC_Data;
            IRTE_UINT32        x15568_P2_ARC_Ena;

            IRTE_UINT8         x1556c_dummy[0x1557c - 0x1556c];

            IRTE_UINT32        x1557c_P2_Miss_Cnt;
        } p2;

        struct
        {
            // Port 3
            IRTE_UINT32        x15580_P3_Pause_Cnt;
            IRTE_UINT32        x15584_P3_Remote_Pause_Cnt;

            IRTE_UINT8         x15588_dummy[0x155c0 - 0x15588];

            IRTE_UINT32        x155c0_P3_MAC_Control;
            IRTE_UINT32        x155c4_P3_ARC_Control;
            IRTE_UINT32        x155c8_P3_Transmit_Control;
            IRTE_UINT32        x155cc_P3_Transmit_Status;
            IRTE_UINT32        x155d0_P3_Receive_Control;
            IRTE_UINT32        x155d4_P3_Receive_Status;

            IRTE_UINT8         x155d8_dummy[0x155e0 - 0x155d8];

            IRTE_UINT32        x155e0_P3_ARC_Address;
            IRTE_UINT32        x155e4_P3_ARC_Data;
            IRTE_UINT32        x155e8_P3_ARC_Ena;

            IRTE_UINT8         x155ec_dummy[0x155fc - 0x155ec];

            IRTE_UINT32        x155fc_P3_Miss_Cnt;

            IRTE_UINT8         x15600_dummy[0x16000 - 0x15600];
        } p3;
    } s15400_port;

    struct s16000_r6
    {
        //K-RAM Control
        IRTE_UINT32                    x16000_NRT_FCW_Upper_Limit;
        IRTE_UINT32                    x16004_NRT_FCW_Lower_Limit;
        IRTE_UINT32                    x16008_NRT_DB_Upper_Limit;
        IRTE_UINT32                    x1600c_NRT_DB_Lower_Limit;

        IRTE_UINT32                    x16010_HOL_Upper_Limit_CH;
        IRTE_UINT32                    x16014_HOL_Lower_Limit_CH;
        IRTE_UINT32                    x16018_HOL_Upper_Limit_Port;
        IRTE_UINT32                    x1601c_HOL_Lower_Limit_Port;

        IRTE_UINT32                    x16020_UC_Table_Base;
        IRTE_UINT32                    x16024_UC_Table_Length;
        IRTE_UINT32                    x16028_UC_Default_Ctrl;

        IRTE_UINT32                    x1602c_MC_Table_Base;
        IRTE_UINT32                    x16030_MC_Table_Length;
        IRTE_UINT32                    x16034_MC_Default_Ctrl;

        IRTE_UINT32                    x16038_UCMC_LFSR_Mask;
        IRTE_UINT32                    x1603c_UC_Table_Range;

        IRTE_UINT32                    x16040_VLAN_Table_Base;
        IRTE_UINT32                    x16044_VLAN_Table_Length;
        IRTE_UINT32                    x16048_VLAN_LFSR_Ctrl;

        IRTE_UINT32                    x1604c_Time_Scoreboard_Base;
        IRTE_UINT32                    x16050_Time_Scoreboard_Entries;

        IRTE_UINT32                    x16054_PS_Scoreboard_0_Base;
        IRTE_UINT32                    x16058_PS_Scoreboard_1_Base;

        IRTE_UINT32                    x1605c_TMList_Base;
        IRTE_UINT32                    x16060_TMList_Length;

        IRTE_UINT32                    x16064_FC_Mask;
        IRTE_UINT32                    x16068_HOL_Mask_P0;
        IRTE_UINT32                    x1606c_HOL_Mask_P1;
        IRTE_UINT32                    x16070_HOL_Mask_P2;
        IRTE_UINT32                    x16074_HOL_Mask_P3;

        IRTE_UINT8                     x16078_dummy[0x16400 - 0x16078];

        IRTE_UINT32                    x16400_PRIM_Command;
        IRTE_UINT32                    x16404_SEC_Command;
        IRTE_UINT32                    x16408_Command_Result;
        IRTE_UINT32                    x1640c_Port_Control;

        IRTE_UINT32                    x16410_Flow_Control;
        IRTE_UINT32                    x16414_Producer_Control;
        IRTE_UINT32                    x16418_SS_Queue_Disable;
        IRTE_UINT32                    x1641c_ACW_List_Status;

        IRTE_UINT32                    x16420_NRT_FCW_Count;

        IRTE_UINT32                    x16424_NRT_DB_Count;
        IRTE_UINT32                    x16428_SRT_FCW_Count;
        IRTE_UINT32                    x1642c_HOL_Count_CHA_SS0;

        IRTE_UINT32                    x16430_HOL_Count_CHA_SS1;
        IRTE_UINT32                    x16434_List_Status_CHA;
        IRTE_UINT32                    x16438_HOL_Count_CHB_SS0;
        IRTE_UINT32                    x1643c_HOL_Count_CHB_SS1;

        IRTE_UINT32                    x16440_List_Status_CHB;
        IRTE_UINT32                    x16444_HOL_Count_P0;
        IRTE_UINT32                    x16448_List_Status_P0;
        IRTE_UINT32                    x1644c_HOL_Count_P1;

        IRTE_UINT32                    x16450_List_Status_P1;
        IRTE_UINT32                    x16454_HOL_Count_P2;
        IRTE_UINT32                    x16458_List_Status_P2;
        IRTE_UINT32                    x1645c_HOL_Count_P3;

        IRTE_UINT32                    x16460_List_Status_P3;

        IRTE_UINT8                     x16464_dummy[0x17000 - 0x16464];
    } s16000_KRAM_control;

    struct s17000_r6
    {
        // Interrupt Controller
        IRTE_UINT32                    x17000_Interrupt_Mode_Register;
        IRTE_UINT32                    x17004_Interrupt_MaskRegister_IRT_IRQ0;
        IRTE_UINT32                    x17008_Interrupt_MaskRegister_NRT_IRQ0;
        IRTE_UINT32                    x1700c_Interrupt_MaskRegister_IRT_IRQ1;

        IRTE_UINT32                    x17010_Interrupt_MaskRegister_NRT_IRQ1;
    } s17000_interrupt_controller;

    struct s17014_r6
    {
        // Switch Prozessor
        IRTE_UINT32                    x17014_SW_Interrupt_Mode_Register;
        IRTE_UINT32                    x17018_SW_Interrupt_MaskRegister_IRT_IRQ0;
        IRTE_UINT32                    x1701c_SW_Interrupt_MaskRegister_NRT_IRQ0;

        IRTE_UINT32                    x17020_SW_Interrupt_MaskRegister_IRT_IRQ1;
        IRTE_UINT32                    x17024_SW_Interrupt_MaskRegister_NRT_IRQ1;

        IRTE_UINT8                     x17028_dummy[0x17400 - 0x17028];
    } s17014_switch_prozessor;

    struct s17400_r6
    {
        // Interrupt Controller
        IRTE_UINT32              x17400_Interrupt_Register_IRT_IRQ0;
        IRTE_UINT32              x17404_Interrupt_Register_NRT_IRQ0;
        IRTE_UINT32              x17408_Interrupt_Register_IRT_IRQ1;
        IRTE_UINT32              x1740c_Interrupt_Register_NRT_IRQ1;

        IRTE_UINT32              x17410_Interrupt_Ackn_Register_IRT;
        IRTE_UINT32              x17414_Interrupt_Ackn_Register_NRT;
        IRTE_UINT32              x17418_Interrupt_Request_Register_IRT;
        IRTE_UINT32              x1741c_Interrupt_Request_Register_NRT;

        IRTE_UINT32              x17420_EOI_IRQ0_Register;
        IRTE_UINT32              x17424_EOI_IRQ1_Register;

    } s17400_interrupt_controller;

    struct s17430_r6
    {
        // Switch Prozessor
        IRTE_UINT32              x17428_SW_Interrupt_Register_IRT_IRQ0;
        IRTE_UINT32              x1742c_SW_Interrupt_Register_NRT_IRQ0;
        IRTE_UINT32              x17430_SW_Interrupt_Register_IRT_IRQ1;
        IRTE_UINT32              x17434_SW_Interrupt_Register_NRT_IRQ1;

        IRTE_UINT32              x17438_SW_Interrupt_Ackn_Register_IRT;
        IRTE_UINT32              x1743c_SW_Interrupt_Ackn_Register_NRT;
        IRTE_UINT32              x17440_SW_Interrupt_Request_Register_IRT;
        IRTE_UINT32              x17444_SW_Interrupt_Request_Register_NRT;

        IRTE_UINT32              x17448_SW_EOI_IRQ0_Register;
        IRTE_UINT32              x1744c_SW_EOI_IRQ1_Register;

        IRTE_UINT32              x17450_Activate_HP_Interrupt;
        IRTE_UINT32              x17454_Activate_SP_Interrupt;

        IRTE_UINT32              x17458_Fatal_IRT_Error;
        IRTE_UINT32              x1745c_Fatal_NRT_Error;

        IRTE_UINT8               x17460_dummy[0x18000 - 0x17460];
    } s17430_switch_prozessor;

    // IRT-Trace-Control
    struct s18000_r6
    {
        IRTE_UINT32        x18000_Trace_Base_Address;
        IRTE_UINT32        x18004_Trace_Buffer_Length;
        IRTE_UINT32        x18008_Trace_Transfer_Limit;

        // Frame Trigger Unit
        IRTE_UINT32        x1800c_FM_Address_Value_1_31_0;
        IRTE_UINT32        x18010_FM_Address_Value_1_47_32;
        IRTE_UINT32        x18014_FM_Address_Value_2_31_0;
        IRTE_UINT32        x18018_FM_Address_Value_2_47_32;

        IRTE_UINT32        x1801c_FM_Offset_Data_Field_1;
        IRTE_UINT32        x18020_FM_Mask_Data_Field_1;
        IRTE_UINT32        x18024_FM_Data_Field_1;

        IRTE_UINT32        x18028_FM_Offset_Data_Field_2;
        IRTE_UINT32        x1802c_FM_Mask_Data_Field_2;
        IRTE_UINT32        x18030_FM_Data_Field_2;

        IRTE_UINT32        x18034_FM_Offset_Data_Field_3;
        IRTE_UINT32        x18038_FM_Mask_Data_Field_3;
        IRTE_UINT32        x1803c_FM_Data_Field_3;

        IRTE_UINT32        x18040_FM_Count;
        IRTE_UINT32        x18044_FM_Delta_Time;
        IRTE_UINT32        x18048_FM_Selector_Enable;
        IRTE_UINT32        x1804c_FM_Trigger_Enable;
        IRTE_UINT32        x18050_FM_Monitor_Count;
        IRTE_UINT32        x18054_FM_Control;

        // Ende Frame Trigger_Unit

        IRTE_UINT8             x18058_dummy[0x18400 - 0x18058];

        IRTE_UINT32            x18400_Trace_Command_Register;
        IRTE_UINT32            x18404_Trace_Mode;
        IRTE_UINT32            x18408_Trace_Entry_Counter;
        IRTE_UINT32            x1840c_Trace_Fault_Counter;

        IRTE_UINT8             x18410_dummy[0x19000 - 0x18410];
    } s18000_irt_trace_control;

    // Global parameters
    struct s19000_r6
    {
        IRTE_UINT32                    x19000_Ether_RT_Type_0;
        IRTE_UINT32                    x19004_Ether_RT_Type_1;
        IRTE_UINT32                    x19008_Ether_RT_Type_2;
        IRTE_UINT32                    x1900c_Ether_RT_Type_3;
        IRTE_UINT32                    x19010_IRT_Current_Cycle_ID;

        IRTE_UINT32                    x19014_IRT_DA_0_31;
        IRTE_UINT32                    x19018_IRT_DA_32_47;
        IRTE_UINT32                    x1901c_SA_0_31;

        IRTE_UINT32                    x19020_SA_32_47;
        IRTE_UINT32                    x19024_IRT_Ctrl_Base;
        IRTE_UINT32                    x19028_NRT_Ctrl_Base;
        IRTE_UINT32                    x1902c_Free_Ctrl_Base;

        IRTE_UINT32                    x19030_ACW_List_Base;
        IRTE_UINT32                    x19034_En_ACW_Offset;

        IRTE_UINT32                    x19038_Switch_Control;

        IRTE_UINT32                    x1903c_Min_idle_Time;

        IRTE_UINT32                    x19040_MAC_Empty_Count;
        IRTE_UINT32                    x19044_NRT_Transfer_Control;
        IRTE_UINT32                    x19048_NRT_SaftyMargin;
        IRTE_UINT32                    x1904c_Monitor_Control;

        IRTE_UINT32                    x19050_Statistic_Ctrl_Base;
        IRTE_UINT32                    x19054_Switch_Setup;

        IRTE_UINT32                    x19058_Filter_Ctrl_Base;
        IRTE_UINT32                    x1905c_Switch_CT_Delay;

        IRTE_UINT8                     x19060_dummy[0x19400 - 0x19060];

        IRTE_UINT32                    x19400_Version_Number;
        IRTE_UINT32                    x19404_Switch_Status;
        IRTE_UINT32                    x19408_Activity_Status;

        IRTE_UINT8                     x1940c_dummy[0x1a000 - 0x1940c];
    } s19000_global_para;

    // Trigger Unit
    struct s1a000_r6
    {
        IRTE_UINT32        x1a000_Trigger_Control;
        IRTE_UINT32        x1a004_Trigger_Address_Mask;
        IRTE_UINT32        x1a008_Trigger_Lower_Data_Mask;
        IRTE_UINT32        x1a00c_Trigger_Upper_Data_Mask;

        IRTE_UINT8         x1a010_dummy[0x1a400 - 0x1a010];

        IRTE_UINT32        x1a400_Trigger_Address;
        IRTE_UINT32        x1a404_Trigger_Lower_Data;
        IRTE_UINT32        x1a408_Trigger_Upper_Data;
        IRTE_UINT32        x1a40c_Trigger_Info;

        IRTE_UINT32        x1a410_Trigger_Reset;

        IRTE_UINT8         x1a414_dummy[0x1b000 - 0x1a414];

    } s1a000_trigger_unit;

    // Statistic-Control
    struct s1b000_r6
    {
        struct
        {
            IRTE_UINT32        x1b000_Good_Bytes_Received;
            IRTE_UINT32        x1b004_Total_Bytes_Received;
            IRTE_UINT32        x1b008_Bytes_Sent;
            IRTE_UINT32        x1b00c_Frames_Sent;

            IRTE_UINT32        x1b010_Unicast_Frames_Received;
            IRTE_UINT32        x1b014_Unicast_Frames_Received_un;
            IRTE_UINT32        x1b018_Multicast_Frames_Received;
            IRTE_UINT32        x1b01c_Multicast_Frames_Received_un;

            IRTE_UINT32        x1b020_Broadcast_Frames_Received;
            IRTE_UINT32        x1b024_Unicast_Frames_Sent;
            IRTE_UINT32        x1b028_Multicast_Frames_Sent;
            IRTE_UINT32        x1b02c_Broadcast_Frames_Sent;

            IRTE_UINT32        x1b030_Frames_under_64Bytest;
            IRTE_UINT32        x1b034_Frames_with_length_of_64Bytes;
            IRTE_UINT32        x1b038_Frames_between_65_127Bytes;
            IRTE_UINT32        x1b03c_Frames_between_128_255Bytes;

            IRTE_UINT32        x1b040_Frames_between_256_511Bytes;
            IRTE_UINT32        x1b044_Frames_between_512_1023Bytes;
            IRTE_UINT32        x1b048_Frames_between_1024_MAXBytes;
            IRTE_UINT32        x1b04c_Good_Frames_Received;

            IRTE_UINT32        x1b050_Total_Frames_Received;
            IRTE_UINT32        x1b054_Received_Frames_64Bytes_with_bad_CRC;
            IRTE_UINT32        x1b058_Rx_Errors;
            IRTE_UINT32        x1b05c_Oversize_Frames;

            IRTE_UINT32        x1b060_CRC_and_Alignment_Error;
            IRTE_UINT32        x1b064_Frames_Dropped;
            IRTE_UINT32        x1b068_Collision;
            IRTE_UINT32        x1b06c_HOL_boundary_crossed;

            IRTE_UINT32        x1b070_CC_diff_summary;
            IRTE_UINT32        x1b074_CC_delay_summary;
            IRTE_UINT32        x1b078_CC_count;

            IRTE_UINT8         x1b07c_dummy[0x1b100 - 0x1b07c];
        } p0;

        struct
        {
            IRTE_UINT32        x1b100_Good_Bytes_Received;
            IRTE_UINT32        x1b104_Total_Bytes_Received;
            IRTE_UINT32        x1b108_Bytes_Sent;
            IRTE_UINT32        x1b10c_Frames_Sent;

            IRTE_UINT32        x1b110_Unicast_Frames_Received;
            IRTE_UINT32        x1b114_Unicast_Frames_Received_un;
            IRTE_UINT32        x1b118_Multicast_Frames_Received;
            IRTE_UINT32        x1b11c_Multicast_Frames_Received_un;

            IRTE_UINT32        x1b120_Broadcast_Frames_Received;
            IRTE_UINT32        x1b124_Unicast_Frames_Sent;
            IRTE_UINT32        x1b128_Multicast_Frames_Sent;
            IRTE_UINT32        x1b12c_Broadcast_Frames_Sent;

            IRTE_UINT32        x1b130_Frames_under_64Bytest;
            IRTE_UINT32        x1b134_Frames_with_length_of_64Bytes;
            IRTE_UINT32        x1b138_Frames_between_65_127Bytes;
            IRTE_UINT32        x1b13c_Frames_between_128_255Bytes;

            IRTE_UINT32        x1b140_Frames_between_256_511Bytes;
            IRTE_UINT32        x1b144_Frames_between_512_1023Bytes;
            IRTE_UINT32        x1b148_Frames_between_1024_MAXBytes;
            IRTE_UINT32        x1b14c_Good_Frames_Received;

            IRTE_UINT32        x1b150_Total_Frames_Received;
            IRTE_UINT32        x1b154_Received_Frames_64Bytes_with_bad_CRC;
            IRTE_UINT32        x1b158_Rx_Errors;
            IRTE_UINT32        x1b15c_Oversize_Frames;

            IRTE_UINT32        x1b160_CRC_and_Alignment_Error;
            IRTE_UINT32        x1b164_Frames_Dropped;
            IRTE_UINT32        x1b168_Collision;
            IRTE_UINT32        x1b16c_HOL_boundary_crossed;

            IRTE_UINT32        x1b170_CC_diff_summary;
            IRTE_UINT32        x1b174_CC_delay_summary;
            IRTE_UINT32        x1b178_CC_count;

            IRTE_UINT8         x1b17c_dummy[0x1b200 - 0x1b17c];
        } p1;

        struct
        {
            IRTE_UINT32        x1b200_Good_Bytes_Received;
            IRTE_UINT32        x1b204_Total_Bytes_Received;
            IRTE_UINT32        x1b208_Bytes_Sent;
            IRTE_UINT32        x1b20c_Frames_Sent;

            IRTE_UINT32        x1b210_Unicast_Frames_Received;
            IRTE_UINT32        x1b214_Unicast_Frames_Received_un;
            IRTE_UINT32        x1b218_Multicast_Frames_Received;
            IRTE_UINT32        x1b21c_Multicast_Frames_Received_un;

            IRTE_UINT32        x1b220_Broadcast_Frames_Received;
            IRTE_UINT32        x1b224_Unicast_Frames_Sent;
            IRTE_UINT32        x1b228_Multicast_Frames_Sent;
            IRTE_UINT32        x1b22c_Broadcast_Frames_Sent;

            IRTE_UINT32        x1b230_Frames_under_64Bytest;
            IRTE_UINT32        x1b234_Frames_with_length_of_64Bytes;
            IRTE_UINT32        x1b238_Frames_between_65_127Bytes;
            IRTE_UINT32        x1b23c_Frames_between_128_255Bytes;

            IRTE_UINT32        x1b240_Frames_between_256_511Bytes;
            IRTE_UINT32        x1b244_Frames_between_512_1023Bytes;
            IRTE_UINT32        x1b248_Frames_between_1024_MAXBytes;
            IRTE_UINT32        x1b24c_Good_Frames_Received;

            IRTE_UINT32        x1b250_Total_Frames_Received;
            IRTE_UINT32        x1b254_Received_Frames_64Bytes_with_bad_CRC;
            IRTE_UINT32        x1b258_Rx_Errors;
            IRTE_UINT32        x1b25c_Oversize_Frames;

            IRTE_UINT32        x1b260_CRC_and_Alignment_Error;
            IRTE_UINT32        x1b264_Frames_Dropped;
            IRTE_UINT32        x1b268_Collision;
            IRTE_UINT32        x1b26c_HOL_boundary_crossed;

            IRTE_UINT32        x1b270_CC_diff_summary;
            IRTE_UINT32        x1b274_CC_delay_summary;
            IRTE_UINT32        x1b278_CC_count;

            IRTE_UINT8         x1b27c_dummy[0x1b300 - 0x1b27c];
        } p2;

        struct
        {
            IRTE_UINT32        x1b300_Good_Bytes_Received;
            IRTE_UINT32        x1b304_Total_Bytes_Received;
            IRTE_UINT32        x1b308_Bytes_Sent;
            IRTE_UINT32        x1b30c_Frames_Sent;

            IRTE_UINT32        x1b310_Unicast_Frames_Received;
            IRTE_UINT32        x1b314_Unicast_Frames_Received_un;
            IRTE_UINT32        x1b318_Multicast_Frames_Received;
            IRTE_UINT32        x1b31c_Multicast_Frames_Received_un;

            IRTE_UINT32        x1b320_Broadcast_Frames_Received;
            IRTE_UINT32        x1b324_Unicast_Frames_Sent;
            IRTE_UINT32        x1b328_Multicast_Frames_Sent;
            IRTE_UINT32        x1b32c_Broadcast_Frames_Sent;

            IRTE_UINT32        x1b330_Frames_under_64Bytest;
            IRTE_UINT32        x1b334_Frames_with_length_of_64Bytes;
            IRTE_UINT32        x1b338_Frames_between_65_127Bytes;
            IRTE_UINT32        x1b33c_Frames_between_128_255Bytes;

            IRTE_UINT32        x1b340_Frames_between_256_511Bytes;
            IRTE_UINT32        x1b344_Frames_between_512_1023Bytes;
            IRTE_UINT32        x1b348_Frames_between_1024_MAXBytes;
            IRTE_UINT32        x1b34c_Good_Frames_Received;

            IRTE_UINT32        x1b350_Total_Frames_Received;
            IRTE_UINT32        x1b354_Received_Frames_64Bytes_with_bad_CRC;
            IRTE_UINT32        x1b358_Rx_Errors;
            IRTE_UINT32        x1b35c_Oversize_Frames;

            IRTE_UINT32        x1b360_CRC_and_Alignment_Error;
            IRTE_UINT32        x1b364_Frames_Dropped;
            IRTE_UINT32        x1b368_Collision;
            IRTE_UINT32        x1b36c_HOL_boundary_crossed;

            IRTE_UINT32        x1b370_CC_diff_summary;
            IRTE_UINT32        x1b374_CC_delay_summary;
            IRTE_UINT32        x1b378_CC_count;

            IRTE_UINT8         x1b37c_dummy[0x1b400 - 0x1b37c];
        } p3;

    } s1b000_statistic_control;

} IRTE_SW_MAKRO_REV6_T;

/***********************************************************************************/
/* Revision 7                                                                      */
/***********************************************************************************/

typedef struct IRTE_SW_MAKRO_REV7_S
{
    struct s00000_r7   // IR_port
    {
        struct
        {
            // Port 0
            IRTE_UINT32        x00000_Port_Control_P0;
            IRTE_UINT32        x00004_Min_Preamble_Count_P0;
            IRTE_UINT32        x00008_IRT_Diagnose_Enable_P0;
            IRTE_UINT32        x0000c_Line_Delay_P0;
            IRTE_UINT32        x00010_NRT_Control_P0;
            IRTE_UINT32        x00014_Default_VLAN_TAG_P0;
            IRTE_UINT8         x00018_dummy[0x01000 - 0x18];
        } p0;

        struct
        {
            // Port 1
            IRTE_UINT32        x01000_Port_Control_P1;
            IRTE_UINT32        x01004_Min_Preamble_Count_P1;
            IRTE_UINT32        x01008_IRT_Diagnose_Enable_P1;
            IRTE_UINT32        x0100c_Line_Delay_P1;
            IRTE_UINT32        x01010_NRT_Control_P1;
            IRTE_UINT32        x01014_Default_VLAN_TAG_P1;
            IRTE_UINT8         x01018_dummy[0x02000 - 0x01018];
        } p1;

        struct
        {
            // Port 2
            IRTE_UINT32        x02000_Port_Control_P2;
            IRTE_UINT32        x02004_Min_Preamble_Count_P2;
            IRTE_UINT32        x02008_IRT_Diagnose_Enable_P2;
            IRTE_UINT32        x0200c_Line_Delay_P2;
            IRTE_UINT32        x02010_NRT_Control_P2;
            IRTE_UINT32        x02014_Default_VLAN_TAG_P2;
            IRTE_UINT8         x02018_dummy[0x03000 - 0x02018];
        } p2;

        struct
        {
            // Port 3
            IRTE_UINT32        x03000_Port_Control_P3;
            IRTE_UINT32        x03004_Min_Preamble_Count_P3;
            IRTE_UINT32        x03008_IRT_Diagnose_Enable_P3;
            IRTE_UINT32        x0300c_Line_Delay_P3;
            IRTE_UINT32        x03010_NRT_Control_P3;
            IRTE_UINT32        x03014_Default_VLAN_TAG_P3;
            IRTE_UINT8         x03018_dummy[0x04000 - 0x03018];
        } p3;

        IRTE_UINT8         x04000_dummy[0x8000 - 0x4000];
    } s00000_ethernet_port;

    struct s08000_r7
    {
        // IRT-Trace-Control
        IRTE_UINT32        x08000_Diag_0_31;
        IRTE_UINT32        x08004_Diag_32_63;

        IRTE_UINT8         x08008_dummy[0x09000 - 0x08008];
    } s08000_irt_trace_control;

    struct s09000_r7
    {
        // K-RAM - Control
        IRTE_UINT32        x09000_Count_Ctrl;
        IRTE_UINT32        x09004_Status_Ctrl;
        IRTE_UINT32        x09008_NRT_CT_Data;
        IRTE_UINT8         x0900c_dummy[0x0A000 - 0x0900c];
    } s09000_KRAM_control;

    struct s0a000_r7
    {
        //Consistency-Control
        IRTE_UINT32        x0a000_ConsBlockReqRd;
        IRTE_UINT32        x0a004_ConsBlockAckRd;
        IRTE_UINT32        x0a008_ConsBlockReqWr;
        IRTE_UINT32        x0a00c_ConsBlockAckWr;

        IRTE_UINT8         x0a010_dummy[0x0a400 - 0x0a010];

        IRTE_UINT32        x0a400_ConsPortAddrRd;
        IRTE_UINT32        x0a404_ConsPortAddrWr;
        IRTE_UINT32        x0a408_ConsPortLockAddr;

        IRTE_UINT8         x0a40c_dummy[0x0b000 - 0x0a40c];
    } s0a000_Konsistenz_control;

    struct s0b000_r7
    {
        // IRT
        IRTE_UINT32        x0b000_IRT_StartTime_Snd_P0;
        IRTE_UINT32        x0b004_IRT_StartTime_Rcv_P0;
        IRTE_UINT32        x0b008_IRT_StartTime_Snd_P1;
        IRTE_UINT32        x0b00c_IRT_StartTime_Rcv_P1;
        IRTE_UINT32        x0b010_IRT_StartTime_Snd_P2;
        IRTE_UINT32        x0b014_IRT_StartTime_Rcv_P2;
        IRTE_UINT32        x0b018_IRT_StartTime_Snd_P3;
        IRTE_UINT32        x0b01c_IRT_StartTime_Rcv_P3;

        IRTE_UINT8         x0b020_dummy[0x10000 - 0x0b020];
    } s0b000_irt_control;

    struct s10000_r7
    {
        //Taskmanagement
        IRTE_UINT32        x10000_Clk_Control;

        IRTE_UINT8         x10004_dummy[0x11000 - 0x10004];
    } s10000_task;

    struct s11000_r7
    {
        //Cycle synchronisation
        IRTE_UINT32        x11000_AplClk_Divider;
        IRTE_UINT32        x11004_Clkcy_Delay;
        IRTE_UINT32        x11008_Clkcy_Int_Delay;
        IRTE_UINT32        x1100c_Clkcy_Int_Divider;
        IRTE_UINT32        x11010_Clkcy_Period;
        IRTE_UINT32        x11014_ClkGen_Command;
        IRTE_UINT32        x11018_Comperator_Value_1;
        IRTE_UINT32        x1101c_Comperator_Value_2;
        IRTE_UINT32        x11020_Comperator_Value_3;
        IRTE_UINT32        x11024_Comperator_Value_4;
        IRTE_UINT32        x11028_Comperator_Value_5;
        IRTE_UINT32        x1102c_Cycle_Length;
        IRTE_UINT32        x11030_Mux_Selektor;
        IRTE_UINT32        x11034_Phase_Correction;
        IRTE_UINT32        x11038_Drift_Correction;
        IRTE_UINT32        x1103c_Cycle_Cnt_Entity;
        IRTE_UINT32        x11040_Cycle_FrameID;

        IRTE_UINT8         x11044_dummy[0x11400 - 0x11044];

        IRTE_UINT32        x11400_Cycle_Cnt_Val;
        IRTE_UINT32        x11404_Cycle_Cnt_Val_47_32;
        IRTE_UINT32        x11408_Cycle_Time_Val;

        IRTE_UINT32        x1140c_Clock_Cnt_Val_Begin_Ext;
        IRTE_UINT32        x11410_AplClk_Timer;
        IRTE_UINT32        x11414_ClkGen_Status;
        IRTE_UINT32        x11418_Clock_Cnt_Val;
        IRTE_UINT32        x1141c_Clock_Cnt_Val_Begin;
        IRTE_UINT32        x11420_iSRT_Time;
        IRTE_UINT32        x11424_MAX_iSRT_Length;
        IRTE_UINT32        x11428_Clock_Cnt_Val_Snd;
        IRTE_UINT32        x1142c_iSRT_OutOfBound;

        IRTE_UINT8         x11430_dummy[0x12000 - 0x11430];
    } s11000_Zyklussync;

    struct s12000_r7
    {
        //NRT-Control
        IRTE_UINT32        x12000_NRT_Receive_MapCHA;
        IRTE_UINT32        x12004_NRT_Receive_MapCHB;
        IRTE_UINT32        x12008_NRT_Auto_Enable_CHB;
        IRTE_UINT32        x1200c_ARP_Table_Base;
        IRTE_UINT32        x12010_ARP_Table_Length;
        IRTE_UINT32        x12014_DCP_Table_Base;

        IRTE_UINT8         x12018_dummy[0x12400 - 0x12018];

        IRTE_UINT32        x12400_NRT_Enable_CHA0;
        IRTE_UINT32        x12404_NRT_Send_Descriptor_CHA0;
        IRTE_UINT32        x12408_NRT_Receive_Descriptor_CHA0;

        IRTE_UINT32        x1240c_NRT_Enable_CHA1;
        IRTE_UINT32        x12410_NRT_Send_Descriptor_CHA1;
        IRTE_UINT32        x12414_NRT_Receive_Descriptor_CHA1;

        IRTE_UINT32        x12418_NRT_Enable_CHB0;
        IRTE_UINT32        x1241c_NRT_Send_Descriptor_CHB0;
        IRTE_UINT32        x12420_NRT_Receive_Descriptor_CHB0;

        IRTE_UINT32        x12424_NRT_Enable_CHB1;
        IRTE_UINT32        x12428_NRT_Send_Descriptor_CHB1;
        IRTE_UINT32        x1242c_NRT_Receive_Descriptor_CHB1;

        IRTE_UINT8         x12430_dummy[0x13000 - 0x12430];

    } s12000_NRT_control;

    struct s13000_r7
    {
        //IRT-Control
        IRTE_UINT32               x13000_IRT_Control;
        IRTE_UINT32               x13004_ImageMode;

        IRTE_UINT32               x13008_ApplDataStatus_Mask;
        IRTE_UINT32               x1300c_ApplDataStatus_1;
        IRTE_UINT32               x13010_ApplDataStatus_2;
        IRTE_UINT32               x13014_ApplDataStatus_3;
        IRTE_UINT32               x13018_ApplDataStatus_4;
        IRTE_UINT32               x1301c_ApplDataStatus_5;
        IRTE_UINT32               x13020_ApplDataStatus_6;
        IRTE_UINT32               x13024_ApplDataStatus_7;
        IRTE_UINT32               x13028_IRT_asyncMode;

        IRTE_UINT32               x1302c_DMAInDestAdr0;
        IRTE_UINT32               x13030_DMAInDestAdr1;
        IRTE_UINT32               x13034_DMAInSrcAdr;
        IRTE_UINT32               x13038_DMAInLength;
        IRTE_UINT32               x1303c_DMAOutSrcAdr0;
        IRTE_UINT32               x13040_DMAOutSrcAdr1;
        IRTE_UINT32               x13044_DMAOutDestAdr;
        IRTE_UINT32               x13048_DMAOutLength;
        IRTE_UINT32               x1304c_IRT_TimeCompare;

        IRTE_UINT32               x13050_MAX_IRT_Frame_ID;
        IRTE_UINT8                x13054_dummy[0x13400 - 0x13054];
        IRTE_UINT32               x13400_IRT_syncMode;
        IRTE_UINT32               x13404_Data_Update;
        IRTE_UINT32               x13408_IRT_Transfer_Err;
        IRTE_UINT32               x1340c_DMA_SwitchAdr;

        IRTE_UINT32               x13410_Appl_WatchDog;

        IRTE_UINT8                x13414_dummy[0x15000 - 0x13414];

    } s13000_IRT_control;

    struct s15000_r7
    {
        // PHY
        IRTE_UINT32        x15000_MD_data;
        IRTE_UINT32        x15004_MD_CA;
        IRTE_UINT32        x15008_SMI_Configuration;
        IRTE_UINT32        x1500c_Link_Change;
    } s15000_PHY;

    struct s15010_r7
    {
        // PHY-/SMI-Control
        IRTE_UINT32        x15010_PHY_Cmd_P0;
        IRTE_UINT32        x15014_PHY_Stat_P0;
        IRTE_UINT32        x15018_PHY_Cmd_P1;
        IRTE_UINT32        x1501c_PHY_Stat_P1;
        IRTE_UINT32        x15020_PHY_Cmd_P2;
        IRTE_UINT32        x15024_PHY_Stat_P2;
        IRTE_UINT32        x15028_PHY_Cmd_P3;
        IRTE_UINT32        x1502c_PHY_Stat_P3;

        IRTE_UINT8         x15030_dummy[0x15400 - 0x15030];

    } s15010_PHY_SMI_control;

    struct s15400_r7   // IR_port_ctrl
    {
        struct
        {
            // Port 0
            IRTE_UINT32        x15400_P0_Pause_Cnt;            //?
            IRTE_UINT32        x15404_P0_Remote_Pause_Cnt;     //?

            IRTE_UINT8         x15408_dummy[0x15440 - 0x15408];

            IRTE_UINT32        x15440_P0_MAC_Control;
            IRTE_UINT32        x15444_P0_ARC_Control;
            IRTE_UINT32        x15448_P0_Transmit_Control;
            IRTE_UINT32        x1544c_P0_Transmit_Status;
            IRTE_UINT32        x15450_P0_Receive_Control;
            IRTE_UINT32        x15454_P0_Receive_Status;

            IRTE_UINT8         x15458_dummy[0x15460 - 0x15458];

            IRTE_UINT32        x15460_P0_ARC_Address;
            IRTE_UINT32        x15464_P0_ARC_Data;
            IRTE_UINT32        x15468_P0_ARC_Ena;

            IRTE_UINT8         x1546c_dummy[0x1547c - 0x1546c];

            IRTE_UINT32        x1547c_P0_Miss_Cnt;
        } p0;

        struct
        {
            // Port 1
            IRTE_UINT32        x15480_P1_Pause_Cnt;
            IRTE_UINT32        x15484_P1_Remote_Pause_Cnt;

            IRTE_UINT8         x15488_dummy[0x154c0 - 0x15488];

            IRTE_UINT32        x154c0_P1_MAC_Control;
            IRTE_UINT32        x154c4_P1_ARC_Control;
            IRTE_UINT32        x154c8_P1_Transmit_Control;
            IRTE_UINT32        x154cc_P1_Transmit_Status;
            IRTE_UINT32        x154d0_P1_Receive_Control;
            IRTE_UINT32        x154d4_P1_Receive_Status;

            IRTE_UINT8         x154d8_dummy[0x154e0 - 0x154d8];

            IRTE_UINT32        x154e0_P1_ARC_Address;
            IRTE_UINT32        x154e4_P1_ARC_Data;
            IRTE_UINT32        x154e8_P1_ARC_Ena;

            IRTE_UINT8         x154ec_dummy[0x154fc - 0x154ec];

            IRTE_UINT32        x154fc_P1_Miss_Cnt;
        } p1;

        struct
        {
            // Port 2
            IRTE_UINT32        x15500_P2_Pause_Cnt;               // ?
            IRTE_UINT32        x15504_P2_Remote_Pause_Cnt;        //?

            IRTE_UINT8         x15508_dummy[0x15540 - 0x15508];

            IRTE_UINT32        x15540_P2_MAC_Control;
            IRTE_UINT32        x15544_P2_ARC_Control;
            IRTE_UINT32        x15548_P2_Transmit_Control;
            IRTE_UINT32        x1554c_P2_Transmit_Status;
            IRTE_UINT32        x15550_P2_Receive_Control;
            IRTE_UINT32        x15554_P2_Receive_Status;

            IRTE_UINT8         x15558_dummy[0x15560 - 0x15558];

            IRTE_UINT32        x15560_P2_ARC_Address;
            IRTE_UINT32        x15564_P2_ARC_Data;
            IRTE_UINT32        x15568_P2_ARC_Ena;

            IRTE_UINT8         x1556c_dummy[0x1557c - 0x1556c];

            IRTE_UINT32        x1557c_P2_Miss_Cnt;
        } p2;

        struct
        {
            // Port 3
            IRTE_UINT32        x15580_P3_Pause_Cnt;
            IRTE_UINT32        x15584_P3_Remote_Pause_Cnt;

            IRTE_UINT8         x15588_dummy[0x155c0 - 0x15588];

            IRTE_UINT32        x155c0_P3_MAC_Control;
            IRTE_UINT32        x155c4_P3_ARC_Control;
            IRTE_UINT32        x155c8_P3_Transmit_Control;
            IRTE_UINT32        x155cc_P3_Transmit_Status;
            IRTE_UINT32        x155d0_P3_Receive_Control;
            IRTE_UINT32        x155d4_P3_Receive_Status;

            IRTE_UINT8         x155d8_dummy[0x155e0 - 0x155d8];

            IRTE_UINT32        x155e0_P3_ARC_Address;
            IRTE_UINT32        x155e4_P3_ARC_Data;
            IRTE_UINT32        x155e8_P3_ARC_Ena;

            IRTE_UINT8         x155ec_dummy[0x155fc - 0x155ec];

            IRTE_UINT32        x155fc_P3_Miss_Cnt;

            IRTE_UINT8         x15600_dummy[0x16000 - 0x15600];
        } p3;

    } s15400_port;

    struct s16000_r7
    {
        //K-RAM Control
        IRTE_UINT32                    x16000_NRT_FCW_Upper_Limit;
        IRTE_UINT32                    x16004_NRT_FCW_Lower_Limit;
        IRTE_UINT32                    x16008_NRT_DB_Upper_Limit;
        IRTE_UINT32                    x1600c_NRT_DB_Lower_Limit;

        IRTE_UINT32                    x16010_HOL_Upper_Limit_CH;
        IRTE_UINT32                    x16014_HOL_Lower_Limit_CH;
        IRTE_UINT32                    x16018_HOL_Upper_Limit_Port;
        IRTE_UINT32                    x1601c_HOL_Lower_Limit_Port;

        IRTE_UINT32                    x16020_UC_Table_Base;
        IRTE_UINT32                    x16024_UC_Table_Length;
        IRTE_UINT32                    x16028_UC_Default_Ctrl;

        IRTE_UINT32                    x1602c_MC_Table_Base;
        IRTE_UINT32                    x16030_MC_Table_Length;
        IRTE_UINT32                    x16034_MC_Default_Ctrl;

        IRTE_UINT32                    x16038_UCMC_LFSR_Mask;
        IRTE_UINT32                    x1603c_UC_Table_Range;

        IRTE_UINT32                    x16040_VLAN_Table_Base;
        IRTE_UINT32                    x16044_VLAN_Table_Length;
        IRTE_UINT32                    x16048_VLAN_LFSR_Ctrl;
        IRTE_UINT32                    x1604c_Group_Number;

        IRTE_UINT32                    x16050_Time_Scoreboard_Base;
        IRTE_UINT32                    x16054_Time_Scoreboard_Entries;

        IRTE_UINT32                    x16058_PS_Scoreboard_0_Base;
        IRTE_UINT32                    x1605c_PS_Scoreboard_1_Base;

        IRTE_UINT32                    x16060_TMList_Base;
        IRTE_UINT32                    x16064_TMList_Length;

        IRTE_UINT32                    x16068_FC_Mask;
        IRTE_UINT32                    x1606c_HOL_Mask_P0;
        IRTE_UINT32                    x16070_HOL_Mask_P1;
        IRTE_UINT32                    x16074_HOL_Mask_P2;
        IRTE_UINT32                    x16078_HOL_Mask_P3;

        IRTE_UINT8                     x1607c_dummy[0x16400 - 0x1607c];

        IRTE_UINT32                    x16400_PRIM_Command;
        IRTE_UINT32                    x16404_SEC_Command;
        IRTE_UINT32                    x16408_Command_Result;
        IRTE_UINT32                    x1640c_Port_Control;

        IRTE_UINT32                    x16410_Flow_Control;
        IRTE_UINT32                    x16414_Producer_Control;
        IRTE_UINT32                    x16418_SS_Queue_Disable;
        IRTE_UINT32                    x1641c_ACW_List_Status;

        IRTE_UINT32                    x16420_Low_Water_Status;

        IRTE_UINT32                    x16424_NRT_FCW_Count;

        IRTE_UINT32                    x16428_NRT_DB_Count;
        IRTE_UINT32                    x1642c_SRT_FCW_Count;
        IRTE_UINT32                    x16430_HOL_Count_CHA_SS0;

        IRTE_UINT32                    x16434_HOL_Count_CHA_SS1;
        IRTE_UINT32                    x16438_List_Status_CHA;
        IRTE_UINT32                    x1643c_HOL_Count_CHB_SS0;
        IRTE_UINT32                    x16440_HOL_Count_CHB_SS1;

        IRTE_UINT32                    x16444_List_Status_CHB;
        IRTE_UINT32                    x16448_HOL_Count_P0;
        IRTE_UINT32                    x1644c_List_Status_P0;
        IRTE_UINT32                    x16450_HOL_Count_P1;

        IRTE_UINT32                    x16454_List_Status_P1;
        IRTE_UINT32                    x16458_HOL_Count_P2;
        IRTE_UINT32                    x1645c_List_Status_P2;
        IRTE_UINT32                    x16460_HOL_Count_P3;

        IRTE_UINT32                    x16464_List_Status_P3;

        IRTE_UINT8                     x16468_dummy[0x17000 - 0x16468];

    } s16000_KRAM_control;

    struct s17000_r7
    {
        // Interrupt Controller
        IRTE_UINT32                    x17000_Interrupt_Mode_Register;
        IRTE_UINT32                    x17004_Interrupt_MaskRegister_IRT_IRQ0;
        IRTE_UINT32                    x17008_Interrupt_MaskRegister_NRT_IRQ0;
        IRTE_UINT32                    x1700c_Interrupt_MaskRegister_IRT_IRQ1;

        IRTE_UINT32                    x17010_Interrupt_MaskRegister_NRT_IRQ1;

    } s17000_interrupt_controller;

    struct s17014_r7
    {
        // Switch Prozessor
        IRTE_UINT32                    x17014_SW_Interrupt_Mode_Register;
        IRTE_UINT32                    x17018_SW_Interrupt_MaskRegister_IRT_IRQ0;
        IRTE_UINT32                    x1701c_SW_Interrupt_MaskRegister_NRT_IRQ0;

        IRTE_UINT32                    x17020_SW_Interrupt_MaskRegister_IRT_IRQ1;
        IRTE_UINT32                    x17024_SW_Interrupt_MaskRegister_NRT_IRQ1;

        IRTE_UINT8                     x17028_dummy[0x17400 - 0x17028];

    } s17014_switch_prozessor;

    struct s17400_r7
    {
        // Interrupt Controller
        IRTE_UINT32       x17400_Interrupt_Register_IRT_IRQ0;
        IRTE_UINT32       x17404_Interrupt_Register_NRT_IRQ0;
        IRTE_UINT32       x17408_Interrupt_Register_IRT_IRQ1;
        IRTE_UINT32       x1740c_Interrupt_Register_NRT_IRQ1;

        IRTE_UINT32       x17410_Interrupt_Ackn_Register_IRT;
        IRTE_UINT32       x17414_Interrupt_Ackn_Register_NRT;
        IRTE_UINT32       x17418_Interrupt_Request_Register_IRT;
        IRTE_UINT32       x1741c_Interrupt_Request_Register_NRT;

        IRTE_UINT32       x17420_EOI_IRQ0_Register;
        IRTE_UINT32       x17424_EOI_IRQ1_Register;
    } s17400_interrupt_controller;

    struct s17430_r7
    {
        // Switch Prozessor
        IRTE_UINT32       x17428_SW_Interrupt_Register_IRT_IRQ0;
        IRTE_UINT32       x1742c_SW_Interrupt_Register_NRT_IRQ0;
        IRTE_UINT32       x17430_SW_Interrupt_Register_IRT_IRQ1;
        IRTE_UINT32       x17434_SW_Interrupt_Register_NRT_IRQ1;

        IRTE_UINT32       x17438_SW_Interrupt_Ackn_Register_IRT;
        IRTE_UINT32       x1743c_SW_Interrupt_Ackn_Register_NRT;
        IRTE_UINT32       x17440_SW_Interrupt_Request_Register_IRT;
        IRTE_UINT32       x17444_SW_Interrupt_Request_Register_NRT;

        IRTE_UINT32       x17448_SW_EOI_IRQ0_Register;
        IRTE_UINT32       x1744c_SW_EOI_IRQ1_Register;

        IRTE_UINT32       x17450_Activate_HP_Interrupt;
        IRTE_UINT32       x17454_Activate_SP_Interrupt;

        IRTE_UINT32       x17458_Fatal_IRT_Error;
        IRTE_UINT32       x1745c_Fatal_NRT_Error;

        IRTE_UINT8        x17460_dummy[0x18000 - 0x17460];

    } s17430_switch_prozessor;

    // IRT-Trace-Control
    struct s18000_r7
    {
        IRTE_UINT32        x18000_Trace_Base_Address;
        IRTE_UINT32        x18004_Trace_Buffer_Length;
        IRTE_UINT32        x18008_Trace_Transfer_Limit;

        // Frame Trigger Unit
        IRTE_UINT32        x1800c_FM_Address_Value_1_31_0;
        IRTE_UINT32        x18010_FM_Address_Value_1_47_32;
        IRTE_UINT32        x18014_FM_Address_Value_2_31_0;
        IRTE_UINT32        x18018_FM_Address_Value_2_47_32;

        IRTE_UINT32        x1801c_FM_Offset_Data_Field_1;
        IRTE_UINT32        x18020_FM_Mask_Data_Field_1;
        IRTE_UINT32        x18024_FM_Data_Field_1;

        IRTE_UINT32        x18028_FM_Offset_Data_Field_2;
        IRTE_UINT32        x1802c_FM_Mask_Data_Field_2;
        IRTE_UINT32        x18030_FM_Data_Field_2;

        IRTE_UINT32        x18034_FM_Offset_Data_Field_3;
        IRTE_UINT32        x18038_FM_Mask_Data_Field_3;
        IRTE_UINT32        x1803c_FM_Data_Field_3;

        IRTE_UINT32        x18040_FM_Count;
        IRTE_UINT32        x18044_FM_Delta_Time;
        IRTE_UINT32        x18048_FM_Selector_Enable;
        IRTE_UINT32        x1804c_FM_Trigger_Enable;
        IRTE_UINT32        x18050_FM_Monitor_Count;
        IRTE_UINT32        x18054_FM_Control;

        // End of Frame Trigger_Unit

        IRTE_UINT8              x18058_dummy[0x18400 - 0x18058];

        IRTE_UINT32             x18400_Trace_Command_Register;
        IRTE_UINT32             x18404_Trace_Mode;
        IRTE_UINT32             x18408_Trace_Entry_Counter;
        IRTE_UINT32             x1840c_Trace_Fault_Counter;

        IRTE_UINT8              x18410_dummy[0x19000 - 0x18410];
    } s18000_irt_trace_control;

    // Global parameters
    struct s19000_r7
    {
        IRTE_UINT32                    x19000_Ether_RT_Type_0;
        IRTE_UINT32                    x19004_Ether_RT_Type_1;
        IRTE_UINT32                    x19008_Ether_RT_Type_2;
        IRTE_UINT32                    x1900c_Ether_RT_Type_3;
        IRTE_UINT32                    x19010_IRT_Current_Cycle_ID;

        IRTE_UINT32                    x19014_IRT_DA_0_31;
        IRTE_UINT32                    x19018_IRT_DA_32_47;
        IRTE_UINT32                    x1901c_SA_0_31;

        IRTE_UINT32                    x19020_SA_32_47;

        IRTE_UINT32                    x19024_IP_Address0;
        IRTE_UINT32                    x19028_IP_Address1;
        IRTE_UINT32                    x1902c_IP_Address2;
        IRTE_UINT32                    x19030_IP_Address3;

        IRTE_UINT32                    x19034_IP_TTL;

        IRTE_UINT32                    x19038_UDP_PNIO_PORT0;
        IRTE_UINT32                    x1903c_UDP_PNIO_PORT1;
        IRTE_UINT32                    x19040_UDP_PNIO_PORT2;
        IRTE_UINT32                    x19044_UDP_PNIO_PORT3;

        IRTE_UINT32                    x19048_IRT_Ctrl_Base;
        IRTE_UINT32                    x1904c_NRT_Ctrl_Base;
        IRTE_UINT32                    x19050_Free_Ctrl_Base;

        IRTE_UINT32                    x19054_ACW_List_Base;
        IRTE_UINT32                    x19058_En_ACW_Offset;

        IRTE_UINT32                    x1905c_Switch_Control;

        IRTE_UINT32                    x19060_Min_idle_Time;

        IRTE_UINT32                    x19064_MAC_Empty_Count;
        IRTE_UINT32                    x19068_NRT_Transfer_Control;
        IRTE_UINT32                    x1906c_NRT_SaftyMargin;
        IRTE_UINT32                    x19070_Monitor_Control;

        IRTE_UINT32                    x19074_Statistic_Ctrl_Base;
        IRTE_UINT32                    x19078_Switch_Setup;

        IRTE_UINT32                    x1907c_Filter_Ctrl_Base;
        IRTE_UINT32                    x19080_Switch_CT_Delay;

        IRTE_UINT32                    x19084_IO_Control_Delay;
        IRTE_UINT8                     x19088_dummy[0x19400 - 0x19088];

        IRTE_UINT32                    x19400_Version_Number;
        IRTE_UINT32                    x19404_Switch_Status;
        IRTE_UINT32                    x19408_Activity_Status;

        IRTE_UINT32                    x1940c_Router_MAC_Address_31_0;
        IRTE_UINT32                    x19410_Router_MAC_Address_47_32;

        IRTE_UINT8                     x19414_dummy[0x1a000 - 0x19414];

    } s19000_global_para;

    // Trigger Unit
    struct s1a000_r7
    {
        IRTE_UINT32        x1a000_Trigger_Control;
        IRTE_UINT32        x1a004_Trigger_Address_Mask;
        IRTE_UINT32        x1a008_Trigger_Lower_Data_Mask;
        IRTE_UINT32        x1a00c_Trigger_Upper_Data_Mask;

        IRTE_UINT8         x1a010_dummy[0x1a400 - 0x1a010];

        IRTE_UINT32        x1a400_Trigger_Address;
        IRTE_UINT32        x1a404_Trigger_Lower_Data;
        IRTE_UINT32        x1a408_Trigger_Upper_Data;
        IRTE_UINT32        x1a40c_Trigger_Info;

        IRTE_UINT32        x1a410_Trigger_Reset;

        IRTE_UINT8         x1a414_dummy[0x1b000 - 0x1a414];

    } s1a000_trigger_unit;

    // Statistik-Control
    struct s1b000_r7
    {
        struct
        {
            IRTE_UINT32        x1b000_Good_Bytes_Received;
            IRTE_UINT32        x1b004_Total_Bytes_Received;
            IRTE_UINT32        x1b008_Bytes_Sent;
            IRTE_UINT32        x1b00c_Frames_Sent;

            IRTE_UINT32        x1b010_Unicast_Frames_Received;
            IRTE_UINT32        x1b014_Unicast_Frames_Received_un;
            IRTE_UINT32        x1b018_Multicast_Frames_Received;
            IRTE_UINT32        x1b01c_Multicast_Frames_Received_un;

            IRTE_UINT32        x1b020_Broadcast_Frames_Received;
            IRTE_UINT32        x1b024_Unicast_Frames_Sent;
            IRTE_UINT32        x1b028_Multicast_Frames_Sent;
            IRTE_UINT32        x1b02c_Broadcast_Frames_Sent;

            IRTE_UINT32        x1b030_Frames_under_64Bytest;
            IRTE_UINT32        x1b034_Frames_with_length_of_64Bytes;
            IRTE_UINT32        x1b038_Frames_between_65_127Bytes;
            IRTE_UINT32        x1b03c_Frames_between_128_255Bytes;

            IRTE_UINT32        x1b040_Frames_between_256_511Bytes;
            IRTE_UINT32        x1b044_Frames_between_512_1023Bytes;
            IRTE_UINT32        x1b048_Frames_between_1024_MAXBytes;
            IRTE_UINT32        x1b04c_Good_Frames_Received;

            IRTE_UINT32        x1b050_Total_Frames_Received;
            IRTE_UINT32        x1b054_Received_Frames_64Bytes_with_bad_CRC;
            IRTE_UINT32        x1b058_Rx_Errors;
            IRTE_UINT32        x1b05c_Oversize_Frames;

            IRTE_UINT32        x1b060_CRC_and_Alignment_Error;
            IRTE_UINT32        x1b064_Frames_Dropped;
            IRTE_UINT32        x1b068_Collision;
            IRTE_UINT32        x1b06c_HOL_boundary_crossed;

            IRTE_UINT32        x1b070_CC_diff_summary;
            IRTE_UINT32        x1b074_CC_delay_summary;
            IRTE_UINT32        x1b078_CC_count;

            IRTE_UINT8         x1b07c_dummy[0x1b100 - 0x1b07c];
        } p0;

        struct
        {
            IRTE_UINT32        x1b100_Good_Bytes_Received;
            IRTE_UINT32        x1b104_Total_Bytes_Received;
            IRTE_UINT32        x1b108_Bytes_Sent;
            IRTE_UINT32        x1b10c_Frames_Sent;

            IRTE_UINT32        x1b110_Unicast_Frames_Received;
            IRTE_UINT32        x1b114_Unicast_Frames_Received_un;
            IRTE_UINT32        x1b118_Multicast_Frames_Received;
            IRTE_UINT32        x1b11c_Multicast_Frames_Received_un;

            IRTE_UINT32        x1b120_Broadcast_Frames_Received;
            IRTE_UINT32        x1b124_Unicast_Frames_Sent;
            IRTE_UINT32        x1b128_Multicast_Frames_Sent;
            IRTE_UINT32        x1b12c_Broadcast_Frames_Sent;

            IRTE_UINT32        x1b130_Frames_under_64Bytest;
            IRTE_UINT32        x1b134_Frames_with_length_of_64Bytes;
            IRTE_UINT32        x1b138_Frames_between_65_127Bytes;
            IRTE_UINT32        x1b13c_Frames_between_128_255Bytes;

            IRTE_UINT32        x1b140_Frames_between_256_511Bytes;
            IRTE_UINT32        x1b144_Frames_between_512_1023Bytes;
            IRTE_UINT32        x1b148_Frames_between_1024_MAXBytes;
            IRTE_UINT32        x1b14c_Good_Frames_Received;

            IRTE_UINT32        x1b150_Total_Frames_Received;
            IRTE_UINT32        x1b154_Received_Frames_64Bytes_with_bad_CRC;
            IRTE_UINT32        x1b158_Rx_Errors;
            IRTE_UINT32        x1b15c_Oversize_Frames;

            IRTE_UINT32        x1b160_CRC_and_Alignment_Error;
            IRTE_UINT32        x1b164_Frames_Dropped;
            IRTE_UINT32        x1b168_Collision;
            IRTE_UINT32        x1b16c_HOL_boundary_crossed;

            IRTE_UINT32        x1b170_CC_diff_summary;
            IRTE_UINT32        x1b174_CC_delay_summary;
            IRTE_UINT32        x1b178_CC_count;

            IRTE_UINT8         x1b17c_dummy[0x1b200 - 0x1b17c];
        } p1;

        struct
        {
            IRTE_UINT32        x1b200_Good_Bytes_Received;
            IRTE_UINT32        x1b204_Total_Bytes_Received;
            IRTE_UINT32        x1b208_Bytes_Sent;
            IRTE_UINT32        x1b20c_Frames_Sent;

            IRTE_UINT32        x1b210_Unicast_Frames_Received;
            IRTE_UINT32        x1b214_Unicast_Frames_Received_un;
            IRTE_UINT32        x1b218_Multicast_Frames_Received;
            IRTE_UINT32        x1b21c_Multicast_Frames_Received_un;

            IRTE_UINT32        x1b220_Broadcast_Frames_Received;
            IRTE_UINT32        x1b224_Unicast_Frames_Sent;
            IRTE_UINT32        x1b228_Multicast_Frames_Sent;
            IRTE_UINT32        x1b22c_Broadcast_Frames_Sent;

            IRTE_UINT32        x1b230_Frames_under_64Bytest;
            IRTE_UINT32        x1b234_Frames_with_length_of_64Bytes;
            IRTE_UINT32        x1b238_Frames_between_65_127Bytes;
            IRTE_UINT32        x1b23c_Frames_between_128_255Bytes;

            IRTE_UINT32        x1b240_Frames_between_256_511Bytes;
            IRTE_UINT32        x1b244_Frames_between_512_1023Bytes;
            IRTE_UINT32        x1b248_Frames_between_1024_MAXBytes;
            IRTE_UINT32        x1b24c_Good_Frames_Received;

            IRTE_UINT32        x1b250_Total_Frames_Received;
            IRTE_UINT32        x1b254_Received_Frames_64Bytes_with_bad_CRC;
            IRTE_UINT32        x1b258_Rx_Errors;
            IRTE_UINT32        x1b25c_Oversize_Frames;

            IRTE_UINT32        x1b260_CRC_and_Alignment_Error;
            IRTE_UINT32        x1b264_Frames_Dropped;
            IRTE_UINT32        x1b268_Collision;
            IRTE_UINT32        x1b26c_HOL_boundary_crossed;

            IRTE_UINT32        x1b270_CC_diff_summary;
            IRTE_UINT32        x1b274_CC_delay_summary;
            IRTE_UINT32        x1b278_CC_count;

            IRTE_UINT8         x1b27c_dummy[0x1b300 - 0x1b27c];
        } p2;

        struct
        {
            IRTE_UINT32        x1b300_Good_Bytes_Received;
            IRTE_UINT32        x1b304_Total_Bytes_Received;
            IRTE_UINT32        x1b308_Bytes_Sent;
            IRTE_UINT32        x1b30c_Frames_Sent;

            IRTE_UINT32        x1b310_Unicast_Frames_Received;
            IRTE_UINT32        x1b314_Unicast_Frames_Received_un;
            IRTE_UINT32        x1b318_Multicast_Frames_Received;
            IRTE_UINT32        x1b31c_Multicast_Frames_Received_un;

            IRTE_UINT32        x1b320_Broadcast_Frames_Received;
            IRTE_UINT32        x1b324_Unicast_Frames_Sent;
            IRTE_UINT32        x1b328_Multicast_Frames_Sent;
            IRTE_UINT32        x1b32c_Broadcast_Frames_Sent;

            IRTE_UINT32        x1b330_Frames_under_64Bytest;
            IRTE_UINT32        x1b334_Frames_with_length_of_64Bytes;
            IRTE_UINT32        x1b338_Frames_between_65_127Bytes;
            IRTE_UINT32        x1b33c_Frames_between_128_255Bytes;

            IRTE_UINT32        x1b340_Frames_between_256_511Bytes;
            IRTE_UINT32        x1b344_Frames_between_512_1023Bytes;
            IRTE_UINT32        x1b348_Frames_between_1024_MAXBytes;
            IRTE_UINT32        x1b34c_Good_Frames_Received;

            IRTE_UINT32        x1b350_Total_Frames_Received;
            IRTE_UINT32        x1b354_Received_Frames_64Bytes_with_bad_CRC;
            IRTE_UINT32        x1b358_Rx_Errors;
            IRTE_UINT32        x1b35c_Oversize_Frames;

            IRTE_UINT32        x1b360_CRC_and_Alignment_Error;
            IRTE_UINT32        x1b364_Frames_Dropped;
            IRTE_UINT32        x1b368_Collision;
            IRTE_UINT32        x1b36c_HOL_boundary_crossed;

            IRTE_UINT32        x1b370_CC_diff_summary;
            IRTE_UINT32        x1b374_CC_delay_summary;
            IRTE_UINT32        x1b378_CC_count;

            IRTE_UINT8         x1b37c_dummy[0x1b400 - 0x1b37c];
        } p3;

    } s1b000_statistic_control;

} IRTE_SW_MAKRO_REV7_T;

/* ERTEC200 GPIOs (starting at APB-Offset 0x2500) */
typedef struct ERTEC200_GPIO_S
{
    IRTE_UINT32                    x0000000_GPIO_IOCTRL;
    IRTE_UINT32                    x0000004_GPIO_OUT;
    IRTE_UINT32                    x0000008_GPIO_IN;
    IRTE_UINT32                    x000000c_GPIO_PORT_MODE_L;

    IRTE_UINT32                    x0000010_GPIO_PORT_MODE_H;
    IRTE_UINT32                    x0000014_GPIO_POLSEL;
    IRTE_UINT32                    x0000018_GPIO_res[0x02];

    IRTE_UINT32                    x0000020_GPIO2_IOCTRL;
    IRTE_UINT32                    x0000024_GPIO2_OUT;
    IRTE_UINT32                    x0000028_GPIO2_IN;

} ERTEC200_GPIO_T;

/* ERTEC200 SCRB (starting at APB-Offset 0x2600) */
typedef struct ERTEC200_SCRB_S
{
    IRTE_UINT32                    x0000000_ID_REG;
    IRTE_UINT32                    x0000004_BOOT_REG;
    IRTE_UINT32                    x0000008_CONFIG_REG;
    IRTE_UINT32                    x000000c_Res_Ctrl;

    IRTE_UINT32                    x0000010_Res_Stat;
    IRTE_UINT32                    x0000014_PII__Stat_REG;

    IRTE_UINT32                    x0000018_res[0x04];

    IRTE_UINT32                    x0000028_QVZ_AHB_ADR;
    IRTE_UINT32                    x000002c_QVZ_AHB_CTRL;
    IRTE_UINT32                    x0000030_QVZ_AHB_M;
    IRTE_UINT32                    x0000034_QVZ_APB_ADR;
    IRTE_UINT32                    x0000038_QVZ_EMIF_ADR;

    IRTE_UINT32                    x000003c_res[0x02];

    IRTE_UINT32                    x0000044_MEM_SWAP;
    IRTE_UINT32                    x0000048_res;
    IRTE_UINT32                    x000004c_M_LOCK_CTRL;
    IRTE_UINT32                    x0000050_ARM9_CTRL;
    IRTE_UINT32                    x0000054_ARM9_WE;
    IRTE_UINT32                    x0000058_ERTEC200_TAG;
    IRTE_UINT32                    x000005c_PHY_CONFIG;
    IRTE_UINT32                    x0000060_PHY_STATUS;
    IRTE_UINT32                    x0000064_Handr_CTRL;
    IRTE_UINT32                    x0000068_Handr_Cnt1;
    IRTE_UINT32                    x000006c_Handr_Cnt2;
    IRTE_UINT32                    x0000070_UART_CLK;

    IRTE_UINT8                     x0000074_ssc[0x100 - 0x74];

} ERTEC200_SCRB_T;

/* ERTEC400 GPIOs (starting at APB-Offset 0x2500) */
typedef struct ERTEC400_GPIO_S
{
    IRTE_UINT32                    x0000000_GPIO_IOCTRL;
    IRTE_UINT32                    x0000004_GPIO_OUT;
    IRTE_UINT32                    x0000008_GPIO_IN;
    IRTE_UINT32                    x000000c_GPIO_PORT_MODE_L;

    IRTE_UINT32                    x0000010_GPIO_PORT_MODE_H;

    IRTE_UINT8                     x0000014_res[0x100 - 0x14];

} ERTEC400_GPIO_T;

/* ERTEC400 SCRB (starting at APB-Offset 0x2600) */
typedef struct ERTEC400_SCRB_S
{
    IRTE_UINT32                    x0000000_ID_REG;
    IRTE_UINT32                    x0000004_BOOT_REG;
    IRTE_UINT32                    x0000008_SER_CFG_REG;
    IRTE_UINT32                    x000000c_Res_Ctrl;

    IRTE_UINT32                    x0000010_Res_Stat;
    IRTE_UINT32                    x0000014_PII__Stat_REG;
    IRTE_UINT32                    x0000018_ClK_Ctrl_REG;
    IRTE_UINT32                    x000001c_PM_STATE_REQ_REG;

    IRTE_UINT32                    x0000020_PM_STATE_ACK_REG;
    IRTE_UINT32                    x0000024_PME_REG;
    IRTE_UINT32                    x0000028_QVZ_AHB_ADR;
    IRTE_UINT32                    x000002c_QVZ_AHB_CTRL;

    IRTE_UINT32                    x0000030_QVZ_AHB_M;
    IRTE_UINT32                    x0000034_QVZ_APB_ADR;
    IRTE_UINT32                    x0000038_QVZ_EMIF_ADR;
    IRTE_UINT32                    x000003c_PCI_RES_REQ;

    IRTE_UINT32                    x0000040_PCI_RES_ACK;
    IRTE_UINT32                    x0000044_MEM_SWAP;
    IRTE_UINT32                    x0000048_PCI_INT_CTRL;
    IRTE_UINT32                    x000004c_M_LOCK_CTRL;

    IRTE_UINT32                    x0000050_ARM9_CTRL;
    IRTE_UINT32                    x0000054_ARM9_WE;

    IRTE_UINT8                     x0000058_ssc[0x100 - 0x58];

} ERTEC400_SCRB_T;

typedef struct ERTEC200_TIMER_S
{
    IRTE_UINT32                    x0000000_CRTL_STAT0;
    IRTE_UINT32                    x0000004_CRTL_STAT1;
    IRTE_UINT32                    x0000008_RELD0;
    IRTE_UINT32                    x000000c_RELD1;

    IRTE_UINT32                    x0000010_CRTL_PREDIV;
    IRTE_UINT32                    x0000014_RELD_PREDIV;
    IRTE_UINT32                    x0000018_TIM0;
    IRTE_UINT32                    x000001c_TIM1;

    IRTE_UINT32                    x0000020_TIM2_CTRL;
    IRTE_UINT32                    x0000024_TIM2;
} ERTEC200_TIMER_T;

typedef struct ERTEC400_TIMER_S
{
    // Timer
    IRTE_UINT8                     x0000000_Timer[0x0100];
} ERTECTIMER_AUX_T;

//lint -restore

#if defined __cplusplus     /* If C++ - compiler: End of C linkage */
}
#endif

#endif /* ----- reinclude-protection ----- */


/****************************************************************************/
/*  end of file eddi_irte.h                                                 */
/****************************************************************************/
