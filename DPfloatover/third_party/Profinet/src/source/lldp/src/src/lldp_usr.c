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
/*  C o m p o n e n t     &C: LLDP (Link Layer Discovery Protocol)      :C&  */
/*                                                                           */
/*  F i l e               &F: lldp_usr.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  LLDP user-interface functions with RQBs          */
/*                                                                           */
/*                          - lldp_open_channel()                            */
/*                          - lldp_request()                                 */
/*                          - lldp_close_channel()                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef LLDP_MESSAGE
/*  19.04.04    JS    initial version.                                       */
/*  16.06.04    JS    dont assign LSA_NULL to pSys, because pSys is not      */
/*                    neccessary a pointer. pSys cant be traced.             */
/*  29.06.04    JS    added brackets to prevent compiler warning             */
/*  15.07.04    JS    PortID/SenderID range changed from 0..x to 1..x        */
/*                    PortCnt now from EDD not from CDB.                     */
/*  22.09.04    JS    changed multicast mac to 01-80-c2-00-00-0e according to*/
/*                    802.1AB D11.                                           */
/*                    End of LLDPDU TLV is now mandatory.                    */
/*  18.10.04    JS    After a Link-Up we will send the data and dont wait for*/
/*                    transmit delay timer. But we may wait for TxDelayCnt   */
/*                    MsgTxInterval >= LLDP_CFG_TX_DELAY                     */
/*  26.04.04    JS    bugfix with asserts enabled                            */
/*  08.07.05    JS    bugfix. no more sending if Set/Start/Set because of    */
/*                    next send time was wrong if txdelay is 0.              */
/*  29.08.05    JS    LLDP_CFG_TX_DELAY changed a bit. now 0..x              */
/*  06.10.05    JS    Start sending immediatly if LLDP_CFG_TX_REINIT_DELAY   */
/*                    and/or LLDP_CFG_TX_DELAY are 0 on SET or START request */
/*                    (dont wait for next timer tick)                        */
/*  21.06.06    JS    added PortMAC Address support                          */
/*  26.06.06    JS    wrong assertions removed/changed.                      */
/*  27.06.06    JS    added  LLDP_FILE_SYSTEM_EXTENSION                      */
/*  23.03.07    JS    bugfix: restore original PortID on Stop if request     */
/*                    finished synchronous.                                  */
/*  02.05.07    JS    bugfix. within lldp_close_channel always handle 0 was  */
/*                    closed (may cause an error if multiple LLDP channels   */
/*                    used).                                                 */
/*  01.10.07    JS    added Loopback check.                                  */
/*  03.12.07    JS    changed module id from 0 to 7                          */
/*  27.11.08    JS    added LSA_EXTERN to om 0 to LLDP_BroadcastMac to       */
/*                    avoid C++ problems with global constants used extern.  */
/*  13.05.09    JS    moved LLDP_ConstructFrame() to lldp_edd.c              */
/*                    moved LLDP_BroadcastMacuctFrame() to lldp_edd.c        */
/*  18.09.09    JS    added LLDP_CheckTLVEqual(). Only set new TLV chain if  */
/*                    different to current one.                              */
/*  21.06.10    JS    bugfix: used variable "i" twice within                 */
/*                    lldp_requesttimer (was not causing a error if only     */
/*                    one channel is open with lldp)                         */
#endif
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7
#define LLDP_MODULE_ID     LTRC_ACT_MODUL_ID /* LLDP_MODULE_ID_LLDP_TIM */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "lldp_inc.h"            /* LLDP headerfiles */
#include "lldp_int.h"            /* internal header */

LLDP_FILE_SYSTEM_EXTENSION(LLDP_MODULE_ID)

/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/

/*===========================================================================*/
/*                                external functions                         */
/*===========================================================================*/


/*===========================================================================*/
/*                                 Macros                                    */
/*===========================================================================*/


/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_UserGetFreeHDB                         +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE LLDP_LOCAL..pHDBUser      +*/
/*+  Result                :    LLDP_RSP                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDBUser     : LLDP user HDB to get                                    +*/
/*+  PortCnt      : Number of Ports for HDB (1..)                           +*/
/*+  PortMaxSender: Number of max supported sender per port (1..)           +*/
/*+                                                                         +*/
/*+  Result     : LLDP_RSP_OK                                               +*/
/*+               LLDP_RSP_ERR_RESOURCE                                     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets Pointer to a free User-HDB                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_UserGetFreeHDB(
    LLDP_HDB_PTR_TYPE LLDP_LOCAL_MEM_ATTR *pHDB)
{

    LLDP_RSP                  Status;
    LSA_BOOL                  Found;
    LLDP_HDB_PTR_TYPE         pHDBTemp;
    LSA_UINT32                i;
    LSA_HANDLE_TYPE           Help;

    LLDP_FUNCTION_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_UserGetFreeHDB(ppHDB: %Xh)",pHDB);

    pHDBTemp  = LLDP_MGM_GET_HUSER_PTR()->HDB;
    i         = 0;
    Found     = LSA_FALSE;
    Status    = LLDP_RSP_OK;

    /* --------------------------------------------------------------------------*/
    /* Search for free handle-management                                         */
    /* --------------------------------------------------------------------------*/

    while ( (! Found)  &&  ( i < LLDP_MGM_GET_HUSER_PTR()->HandleCnt))
    {
        if ( pHDBTemp->State == LLDP_HANDLE_STATE_FREE )
        {
            Found = LSA_TRUE;
        }
        else
        {
            i++;
            pHDBTemp++;
        }
    }

    /* --------------------------------------------------------------------------*/
    /* If a free handle management was found we initialize it..                  */
    /* --------------------------------------------------------------------------*/

    if ( Found )
    {
        /* save thishandle before clearing out memory */
        Help =  pHDBTemp->ThisHandle;

        /* initialize memory with 0 */
        LLDP_MEMSET_LOCAL(pHDBTemp,0,sizeof(LLDP_HDB_TYPE));

        /* restore/set some values .. */
        pHDBTemp->State      = LLDP_HANDLE_STATE_FREE;
        pHDBTemp->ThisHandle = Help;

        LLDP_MGM_GET_HUSER_PTR()->UsedCnt++;

        pHDBTemp->State = LLDP_HANDLE_STATE_CLOSED;
        pHDBTemp->TraceIdx =0;              /* set trace index to 0 until EDDx channel is opened! */
        *pHDB = pHDBTemp;
    }
    else
    {
        Status = LLDP_RSP_ERR_RESOURCE;
        LLDP_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_HIGH,"LLDP_UserGetFreeHDB(). No more free handles.");
    }
    LLDP_FUNCTION_TRACE_02(0,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_UserGetFreeHDB(Status: 0x%X, pHDB: 0x%X )",Status,*pHDB);
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_UserReleaseHDB                         +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE pHDB                      +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to User Handle to release                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Release User-HDB                                          +*/
/*+               Release memory (port management)                          +*/
/*+               Sets Handle-state to LLDP_HANDLE_STATE_FREE               +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_LOCAL_FCT_ATTR LLDP_UserReleaseHDB(
    LLDP_HDB_PTR_TYPE pHDB)
{

    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_UserReleaseHDB(pHDB: 0x%X)",pHDB);

    LLDP_ASSERT_NULL_PTR(pHDB);
    LLDP_EDDFreePortMgm(pHDB);
    pHDB->State = LLDP_HANDLE_STATE_FREE;
    LLDP_MGM_GET_HUSER_PTR()->UsedCnt--;
    LLDP_FUNCTION_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_UserReleaseHDB()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_CylicTimerCB                           +*/
/*+  Input                      LLDP_INTERNAL_REQUEST_PTR_TYPE   pIntRQB    +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pIntRQB    : Pointer to internal RQB-mgm.                              +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handles Timerevent for Cylcic Timer.                      +*/
/*+               Called by LLDP-Timerhandler                               +*/
/*+                                                                         +*/
/*+               It can be configured if we should call the output macro   +*/
/*+               LLDP_DO_TIMER_REQUEST for doing a context change or if    +*/
/*+               we should do the handling inside the function.            +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_CylicTimerCB(
    LLDP_INTERNAL_REQUEST_PTR_TYPE   pIntRQB)
{
    if (!pIntRQB->InUse )
    {
        /* Note that the RQB is already filled up in startup! */
        pIntRQB->InUse = LSA_TRUE;

#ifdef LLDP_CFG_HANDLE_DO_TIMEOUT_INTERN
		LLDP_RequestTimer(pIntRQB->pRQB);
#else
		LLDP_DO_TIMER_REQUEST(pIntRQB->pRQB);
#endif
	}
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_IndicateCheckTooMany                   +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                             LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl         +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  pPCtrl               : Pointer to PortManagement                       +*/
/*+                                                                         +*/
/*+  Result               : LSA_TRUE : indicated                            +*/
/*+                         LSA_FALSE: could not indicate                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates too many sender if needed.                      +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_BOOL  LLDP_LOCAL_FCT_ATTR LLDP_IndicateCheckTooMany(
    LLDP_HDB_PTR_TYPE            pHDB,
    LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl)
{

    LSA_BOOL                     Status;
    LLDP_UPPER_RQB_PTR_TYPE      pRQB;

    Status = LSA_TRUE;

    if ( pPCtrl->TooManyState == LLDP_RX_FRAME_TOO_MANY_OCCURED )
    {
        /* get indication rqb */
        LLDP_RQB_REM_BLOCK_BOTTOM(
            pHDB->IndReq.pBottom,
            pHDB->IndReq.pTop,
            pRQB);
        if (! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL))
        {
            pRQB->args.IndProvide.Event    = LLDP_EVENT_TOO_MANY_SENDER;
            pRQB->args.IndProvide.PORTId   = (LSA_UINT16) pPCtrl->ThisIdx+1;
            pRQB->args.IndProvide.SenderId = 0; /* not used */
            pHDB->IndReqPending--;
            pPCtrl->TooManyState = LLDP_RX_FRAME_TOO_MANY_IND;
            LLDP_UserRequestFinish(pHDB,pRQB,LLDP_RSP_OK);
        }
        else
        {
            Status = LSA_FALSE;
        }
    }
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_IndicateCheckLastInvalid               +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                             LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl         +*/
/*+                             LSA_UINT32                   Idx            +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  pPCtrl               : Pointer to PortManagement                       +*/
/*+  Idx                  : Rx-Idx (Sender-ID)                              +*/
/*+                                                                         +*/
/*+  Result               : LSA_TRUE : indicated                            +*/
/*+                         LSA_FALSE: could not indicate                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates last invalid sender if pending                  +*/
/*+               This can occur, if we are in a VALID-State, got INVALID   +*/
/*+               and VALID for a new sender. If we have no indication      +*/
/*+               resources, the user need a INVALID-Indication for the     +*/
/*+               last-sender. This check is only needed if new indication  +*/
/*+               resources provided after none present.                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_BOOL  LLDP_LOCAL_FCT_ATTR LLDP_IndicateCheckLastInvalid(
    LLDP_HDB_PTR_TYPE            pHDB,
    LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl,
    LSA_UINT32                   Idx)
{
    LLDP_UPPER_RQB_PTR_TYPE      pRQB;
    LLDP_RX_FRAME_INFO_PTR_TYPE  pRx;
    LSA_BOOL                     Status;

    Status = LSA_TRUE;

    pRx = &pPCtrl->pRx[Idx];

    if (( pRx->Ind.IndLastInvalid  ) &&
        ( pRx->Ind.StateIndicated == LLDP_RX_FRAME_STATE_DATA_VALID ))
    {
        /* get indication rqb */
        LLDP_RQB_REM_BLOCK_BOTTOM(
            pHDB->IndReq.pBottom,
            pHDB->IndReq.pTop,
            pRQB);

        if (! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL))
        {
            pRQB->args.IndProvide.Event    = LLDP_EVENT_DATA_INVALID;
            pRQB->args.IndProvide.PORTId   = (LSA_UINT16) pPCtrl->ThisIdx+1;
            pRQB->args.IndProvide.SenderId = (LSA_UINT16) pRx->ThisIdx+1;

            LLDP_COPY_MAC_ADDR_COMMON(
                (LLDP_COMMON_MEM_PTR_TYPE) pRQB->args.IndProvide.MACAddr,
                (LLDP_COMMON_MEM_PTR_TYPE) &pRx->Ind.LastMACAddr);

            pHDB->IndReqPending--;

            pRx->Ind.IndLastInvalid = LSA_FALSE;
            pRx->Ind.StateIndicated = LLDP_RX_FRAME_STATE_DATA_INVALID;

            LLDP_UserRequestFinish(pHDB,pRQB,LLDP_RSP_OK);
        }
        else
        {
            Status = LSA_FALSE;
        }
    }
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_IndicateCheckState                     +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                             LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl         +*/
/*+                             LSA_UINT32                   Idx            +*/
/*+  Result                :    LSA_BOOL                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  pPCtrl               : Pointer to PortManagement                       +*/
/*+  Idx                  : Rx-Idx (Sender-ID)                              +*/
/*+                                                                         +*/
/*+  Result               : LSA_TRUE : indicated or no indication needed    +*/
/*+                         LSA_FALSE: could not indicate                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Indicates actual status of Port to user if something pend.+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_BOOL  LLDP_LOCAL_FCT_ATTR LLDP_IndicateCheckState(
    LLDP_HDB_PTR_TYPE            pHDB,
    LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl,
    LSA_UINT32                   Idx)
{
    LLDP_UPPER_RQB_PTR_TYPE      pRQB;
    LLDP_RX_FRAME_INFO_PTR_TYPE  pRx;
    LSA_BOOL                     Status;

    Status = LSA_TRUE;

    pRx = &pPCtrl->pRx[Idx];

    if ( ( pRx->State != pRx->Ind.StateIndicated ) ||
         ( pRx->State == LLDP_RX_FRAME_STATE_DATA_VALID &&
           pRx->Ind.Changed ))
    {
        /* get indication rqb */
        LLDP_RQB_REM_BLOCK_BOTTOM(
            pHDB->IndReq.pBottom,
            pHDB->IndReq.pTop,
            pRQB);

        if (! LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL))
        {
            pRx->Ind.StateIndicated = pRx->State;
            pRx->Ind.Changed        = LSA_FALSE;

            switch ( pRx->State )
            {
                case LLDP_RX_FRAME_STATE_SHUTDOWN:
                    pRQB->args.IndProvide.Event = LLDP_EVENT_SHUTDOWN;
                    break;
                case LLDP_RX_FRAME_STATE_DATA_VALID:
                    pRQB->args.IndProvide.Event = LLDP_EVENT_DATA_VALID;
                    break;
                case LLDP_RX_FRAME_STATE_DATA_INVALID:
                    pRQB->args.IndProvide.Event = LLDP_EVENT_DATA_INVALID;
                    break;
                default:
                    LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
                    break;
            }

            pRQB->args.IndProvide.PORTId   = (LSA_UINT16) pPCtrl->ThisIdx+1;
            pRQB->args.IndProvide.SenderId = (LSA_UINT16) pRx->ThisIdx+1;

            /* if data valid we copy mac and, if needed, the tlv */
            if ( pRx->State == LLDP_RX_FRAME_STATE_DATA_VALID )
            {
                /* copy MAC */
                LLDP_COPY_MAC_ADDR_COMMON(
                    (LLDP_COMMON_MEM_PTR_TYPE) pRQB->args.IndProvide.MACAddr,
                    (LLDP_COMMON_MEM_PTR_TYPE) &pRx->Frame[LLDP_FRAME_SRC_MAC_OFFSET]);

                /* copy mac to LastValidMac */
                /* only used for indication if indication resources missing */

                LLDP_COPY_MAC_ADDR_COMMON(
                    (LLDP_COMMON_MEM_PTR_TYPE) pRx->Ind.LastMACAddr,
                    (LLDP_COMMON_MEM_PTR_TYPE) &pRx->Frame[LLDP_FRAME_SRC_MAC_OFFSET]);

                pRx->Ind.IndLastInvalid = LSA_FALSE;

                /* if data valid and pTLV present, we copy the actual TLV chain */
                if (! LSA_HOST_PTR_ARE_EQUAL(pRQB->args.IndProvide.pTLV, LSA_NULL))
                {
                    LLDP_MEMCOPY_COMMON(
                        (LLDP_COMMON_MEM_PTR_TYPE) pRQB->args.IndProvide.pTLV->Data,
                        (LLDP_COMMON_MEM_PTR_TYPE) &pRx->Frame[pRx->TLVDataOffset],
                        pRx->TLVDataLength);
                }

                pRQB->args.IndProvide.TLVLength = pRx->TLVDataLength;
            }
            else pRQB->args.IndProvide.TLVLength = 0;

            pHDB->IndReqPending--;

            /* incdicate new status to user */
            LLDP_UserRequestFinish(pHDB,pRQB,LLDP_RSP_OK);
        }
        else
        {
            Status = LSA_FALSE;
        }
    }
    return(Status);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_UserChannelStateMachine                +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                             LLDP_RSP                     Response       +*/
/*+  Result                :    ---                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB       : Pointer to HDB to handle Channel request for              +*/
/*+  Response   : Responsecode of actual action taken (depends on state)    +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Statemachine for open/close channel.                      +*/
/*+                                                                         +*/
/*+               Actions taken to fullfill the task to open or close       +*/
/*+               the channel. The action depends on the actual "State"     +*/
/*+               of the channel.                                           +*/
/*+                                                                         +*/
/*+  States:                                                                +*/
/*+                                                                         +*/
/*+  Open without error: LLDP_HANDLE_STATE_OPEN_EDD_RUNNING                 +*/
/*+                      LLDP_HANDLE_STATE_GET_MAC_EDD_RUNNING              +*/
/*+                      LLDP_HANDLE_STATE_SET_MC_EDD_RUNNING               +*/
/*+                      LLDP_HANDLE_STATE_SET_MC_EDD_RUNNING               +*/
/*+                      LLDP_HANDLE_STATE_OPEN                             +*/
/*+                                                                         +*/
/*+  Close without error:LLDP_HANDLE_STATE_CLOSE_WAIT_TIMER                 +*/
/*+                      LLDP_HANDLE_STATE_CANCEL_EDD_RUNNING               +*/
/*+                      LLDP_HANDLE_STATE_DISABLE_MC_EDD_RUNNING           +*/
/*+                      LLDP_HANDLE_STATE_CLOSE_EDD_RUNNING                +*/
/*+                      LLDP_HANDLE_STATE_FREE                             +*/
/*+                                                                         +*/
/*+  On error while opening, we have to undo several things using the states+*/
/*+                                                                         +*/
/*+                      LLDP_HANDLE_STATE_DISABLE_MC_ERROR_EDD_RUNNING     +*/
/*+                      LLDP_HANDLE_STATE_CANCEL_ERROR_EDD_RUNNING         +*/
/*+                      LLDP_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING          +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_UserChannelStateMachine(
    LLDP_HDB_PTR_TYPE       pHDB,
    LLDP_RSP                Response)
{
    LLDP_UPPER_RQB_PTR_TYPE      pRQB;
    LLDP_UPPER_RQB_PTR_TYPE      pRQBHelp;
    LSA_BOOL                     Finish;
    LSA_VOID                     LSA_FCT_PTR(LLDP_UPPER_OUT_FCT_ATTR, Cbf) (LLDP_UPPER_RQB_PTR_TYPE pRQB);

    LLDP_ASSERT_NULL_PTR(pHDB);
    LLDP_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_UserChannelStateMachine(pHDB: 0x%X, Response: 0x%X)",pHDB,Response);

    Finish = LSA_FALSE; /* don't finish request by default */

    pRQB   = pHDB->pOCReq;
    Cbf    = pHDB->Params.Cbf;  /* save Cbf from HDB because handle maybe freed!*/

    #if 0
    LLDP_ASSERT_NULL_PTR(pRQB);
    #endif

    switch (pHDB->State)
    {
            /*-------------------------------------------------------------------*/
            /* Just finished a EDD-Open Channel request.                         */
            /* Next Get MAC and PortCnt from EDD                                 */
            /*-------------------------------------------------------------------*/
        case LLDP_HANDLE_STATE_OPEN_EDD_RUNNING:
            if ( Response == LLDP_RSP_OK)
            {
                /* Get MAC-Address and PortCnt from EDD. */
                pHDB->State  = LLDP_HANDLE_STATE_GET_MAC_EDD_RUNNING;
                Response = LLDP_EDDGetParams(pHDB);
                if ( Response != LLDP_RSP_OK)
                {
                    pHDB->OpenResponse = Response; /* save response for later..*/
                    LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD Issuing GetParamsRequest failed.");
                    pHDB->State  = LLDP_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING;
                    LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Closing Channel because of error.");
                    if ( LLDP_EDDCloseChannel(pHDB) != LLDP_RSP_OK)
                    {
                        LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel (Handle: 0x%X).",pHDB->ThisHandle);
                        LLDP_FATAL(LLDP_FATAL_ERR_EDD);
                    }
                }
            }
            else
            {
                LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"Open EDD-Channel-Request failed. (Rsp: %Xh)",Response);
                pHDB->ReleaseSysPtr = LSA_TRUE; /* AP00999034: mark to be released after CBF is called */
                Finish = LSA_TRUE;
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a GET-Params EDD-request.                           */
            /* Get PortParams from EDD                                           */
            /*-------------------------------------------------------------------*/
        case LLDP_HANDLE_STATE_GET_MAC_EDD_RUNNING:
            if ( Response == LLDP_RSP_OK)
            {
                /* now PortCnt within Handle is set, so we can allocate the  */
                /* Portmanagement..                                          */
                if (( pHDB->PortCnt == 0 ) || ( pHDB->PortCnt > LLDP_PORT_CNT_MAX ))
                {
                    Response = LLDP_RSP_ERR_LL;
                    LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"PortCnt not supported (%d)",pHDB->PortCnt);
                }
                else
                {
                    Response = LLDP_EDDInitPortMgm(pHDB,pHDB->PortCnt,pHDB->PortMaxSender);
                    if ( Response == LLDP_RSP_OK)
                    {
                        pHDB->PortGetParamsCnt = 0;
                        /* Get PortParams from EDD. */
                        Response = LLDP_EDDGetPortParams(pHDB,pHDB->PortGetParamsCnt+1);
                        if ( Response == LLDP_RSP_OK)
                        {
                            /* Set Multicast  */
                            pHDB->State  = LLDP_HANDLE_STATE_GET_PORT_MAC_EDD_RUNNING;
                        }
                        else
                        {
                            LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD Issuing EDDGetPortParams failed.");
                        }
                    }
                    else
                    {
                        LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"Cannot init PortManagment");
                    }
                }
                if ( Response != LLDP_RSP_OK)
                {
                    pHDB->OpenResponse = Response; /* save response for later..*/
                    pHDB->State  = LLDP_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING;
                    LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Closing Channel because of error");
                    if ( LLDP_EDDCloseChannel(pHDB) != LLDP_RSP_OK)
                    {
                        LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel().");
                        LLDP_FATAL(LLDP_FATAL_ERR_EDD);
                    }
                }
            }
            else
            {
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD GetParams request failed.");
                pHDB->OpenResponse = Response; /* save for later */
                pHDB->State  = LLDP_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING;
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Closing Channel because of error");
                if ( LLDP_EDDCloseChannel(pHDB) != LLDP_RSP_OK)
                {
                    LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel().");
                    LLDP_FATAL(LLDP_FATAL_ERR_EDD);
                }
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a GETPort Params EDD-request.                       */
            /* Next GetPortParams or Set Multicast address for LLDP              */
            /*-------------------------------------------------------------------*/
        case LLDP_HANDLE_STATE_GET_PORT_MAC_EDD_RUNNING:
            if ( Response == LLDP_RSP_OK)
            {
                /* now PortCnt within Handle is set, so we can allocate the  */
                /* Portmanagement..                                          */
                pHDB->PortGetParamsCnt++; /* just finished a GetPortParams */
                if ( pHDB->PortGetParamsCnt < pHDB->PortCnt )
                {
                    /* next GetPortParams */
                    Response = LLDP_EDDGetPortParams(pHDB,pHDB->PortGetParamsCnt+1);
                    if ( Response != LLDP_RSP_OK)
                    {
                        LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD Issuing EDDGetPortParams failed.");
                    }
                }
                else
                {
                    /* provide EDD-SetMulticast requests. */
                    Response = LLDP_EDDSetMulticast(pHDB,EDD_MULTICAST_ENABLE);
                    if ( Response == LLDP_RSP_OK)
                    {
                        /* Set Multicast  */
                        pHDB->State  = LLDP_HANDLE_STATE_SET_MC_EDD_RUNNING;
                    }
                    else
                    {
                        LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"Cannot set Multicast MAC");
                    }
                }
                if ( Response != LLDP_RSP_OK)
                {
                    pHDB->OpenResponse = Response; /* save response for later..*/
                    pHDB->State  = LLDP_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING;
                    LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Closing Channel because of error");
                    if ( LLDP_EDDCloseChannel(pHDB) != LLDP_RSP_OK)
                    {
                        LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel().");
                        LLDP_FATAL(LLDP_FATAL_ERR_EDD);
                    }
                }
            }
            else
            {
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD GetParams request failed.");
                pHDB->OpenResponse = Response; /* save for later */
                pHDB->State  = LLDP_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING;
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Closing Channel because of error");
                if ( LLDP_EDDCloseChannel(pHDB) != LLDP_RSP_OK)
                {
                    LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel().");
                    LLDP_FATAL(LLDP_FATAL_ERR_EDD);
                }
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a Set-Multicast EDD-Request while opening channel   */
            /* Provide Recv and Linkindication requests. Start cyclic Timer.     */
            /*-------------------------------------------------------------------*/
        case LLDP_HANDLE_STATE_SET_MC_EDD_RUNNING:
            if ( Response == LLDP_RSP_OK)
            {
                /* NOTE: Providing Recv and Link-Indication Request must be */
                /*       the last action before going into State "open".    */
                /*       So only Indciations while in Open-state must be    */
                /*       handled and reprovided!                            */
                /* provide EDD-Recv-Requests. Two per Port. */
                Response = LLDP_EDDRecvProvide(pHDB,(pHDB->PortCnt * LLDP_CFG_RECV_RESOURCE_PER_PORT));
                /* Note: On error there may still some recv-requests pending within EDD !*/
                if ( Response == LLDP_RSP_OK)
                {
                    /* provide Link-Indication Requests */
                    Response = LLDP_EDDLinkIndProvide(pHDB,pHDB->PortCnt);
                }
                if ( Response == LLDP_RSP_OK )
                {
                    /* Start the cyclic Timer if not already started */
                    if (!LLDP_MGM_GET_GLOB_PTR()->CyclicTimerUserCnt )
                    {
                        /* Start the 1s timer */
                        LLDP_TimerStart(pHDB,LLDP_MGM_GET_GLOB_PTR()->CyclicTimerID,1);
                    }
                    LLDP_MGM_GET_GLOB_PTR()->CyclicTimerUserCnt++;
                    /* no more actions needed. Channel is open now. */
                    pHDB->State  = LLDP_HANDLE_STATE_OPEN;
                    Finish = LSA_TRUE;
                }
                else
                {
                    /* Recv, or Ind-Request failed. Cancel Recv-Requests */
                    pHDB->OpenResponse = Response; /* save response for later..*/
                    pHDB->State  = LLDP_HANDLE_STATE_CANCEL_ERROR_EDD_RUNNING;
                    LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Nrt-Cancel because of error while opening channel.");
                    if ( LLDP_EDDNrtCancel(pHDB) != LLDP_RSP_OK)
                    {
                        LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot Cancel EDD request (Handle: 0x%X).",pHDB->ThisHandle);
                        LLDP_FATAL(LLDP_FATAL_ERR_EDD);
                    }
                }
            }
            else
            {
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD Set Multicast request failed.");
                pHDB->OpenResponse = Response; /* save for later */
                pHDB->State  = LLDP_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING;
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Closing Channel because of error");
                if ( LLDP_EDDCloseChannel(pHDB) != LLDP_RSP_OK)
                {
                    LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel().");
                    LLDP_FATAL(LLDP_FATAL_ERR_EDD);
                }
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a EDD-NRT-Cancel because of error within Open       */
            /* Disable Multicast address.                                        */
            /*-------------------------------------------------------------------*/
        case LLDP_HANDLE_STATE_CANCEL_ERROR_EDD_RUNNING:
            if ( Response == LLDP_RSP_OK)
            {
                /* check if all pending requests cancled (should be!!) */
                if ( pHDB->RxPendingCnt )
                {
                    LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"EDD Recv-Requests not canceled!");
                    LLDP_FATAL(LLDP_FATAL_ERR_EDD);
                }
                pHDB->State  = LLDP_HANDLE_STATE_DISABLE_MC_ERROR_EDD_RUNNING;
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Disable Multicast because of error.");
                if ( LLDP_EDDSetMulticast(pHDB,EDD_MULTICAST_DISABLE) != LLDP_RSP_OK)
                {
                    LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot Disable Multicast address (Handle: 0x%X).",pHDB->ThisHandle);
                    LLDP_FATAL(LLDP_FATAL_ERR_EDD);
                }
            }
            else
            {
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot cancel NRT().");
                LLDP_FATAL(LLDP_FATAL_ERR_EDD);
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a Disable Multicast because of error while open     */
            /*-------------------------------------------------------------------*/
        case LLDP_HANDLE_STATE_DISABLE_MC_ERROR_EDD_RUNNING:
            if ( Response == LLDP_RSP_OK)
            {
                pHDB->State  = LLDP_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING;
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Closing Channel because of error.");
                if ( LLDP_EDDCloseChannel(pHDB) != LLDP_RSP_OK)
                {
                    LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel (Handle: 0x%X).",pHDB->ThisHandle);
                    LLDP_FATAL(LLDP_FATAL_ERR_EDD);
                }
            }
            else
            {
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot disable Multicast address.");
                LLDP_FATAL(LLDP_FATAL_ERR_EDD);
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a EDD-Close-Channel because of error within Open    */
            /*-------------------------------------------------------------------*/
        case LLDP_HANDLE_STATE_CLOSE_ERROR_EDD_RUNNING:
            if ( Response == LLDP_RSP_OK)
            {
                /* check if all pending requests cancled (should be!!) */
                if ( pHDB->LinkIndPendingCnt )
                {
                    LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"EDD Link-Indications not canceled!");
                    LLDP_FATAL(LLDP_FATAL_ERR_EDD);
                }
                pHDB->ReleaseSysPtr = LSA_TRUE; /* AP00999034: mark to be released after CBF is called */
                Response = pHDB->OpenResponse;  /* get Error-Response  */
                /* we release the HDB in LLDP_CallCbf */
            }
            else
            {
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel().");
                LLDP_FATAL(LLDP_FATAL_ERR_EDD);
            }
            Finish = LSA_TRUE;
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished waiting for timer to get free while closing channel */
            /* Note: If further handling failed we do not restart the timer or   */
            /*       reprovide rx-resource. The handle will not be useful anymore*/
            /*-------------------------------------------------------------------*/
        case LLDP_HANDLE_STATE_CLOSE_WAIT_TIMER:
            if ( Response == LLDP_RSP_OK)
            {
                /* cancel all pending indication resources */
                while (pHDB->IndReq.pBottom)
                {
                    LLDP_RQB_REM_BLOCK_BOTTOM(
                        pHDB->IndReq.pBottom,
                        pHDB->IndReq.pTop,
                        pRQBHelp);
                    pHDB->IndReqPending--;
                    /* because UserReqPendCnt was >0 we dont expect LSA_NULL here */
                    LLDP_ASSERT_NULL_PTR(pRQBHelp);
                    LLDP_UserRequestFinish(pHDB,pRQBHelp,LLDP_RSP_OK_CANCEL);
                }
                /* continue by canceling all Rcv-NRT-Requests pending */
                Response = LLDP_EDDNrtCancel(pHDB);
                if ( Response == LLDP_RSP_OK )
                {
                    /* Request is underway now.. */
                    pHDB->State  = LLDP_HANDLE_STATE_CANCEL_EDD_RUNNING;
                }
                else
                {
                    LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot Cancel EDD-Requests (Handle: 0x%X).",pHDB->ThisHandle);
                    Finish = LSA_TRUE;
                }
            }
            else
            {
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Unexpected error.");
            }
            break;

            /*-------------------------------------------------------------------*/
            /* Just finished a EDD-NRT-Cancel while closing channel              */
            /* Note: If further handling failed we do not restart the timer or   */
            /*       reprovide rx-resource. The handle will not be useful anymore*/
            /*-------------------------------------------------------------------*/
        case LLDP_HANDLE_STATE_CANCEL_EDD_RUNNING:
            if ( Response == LLDP_RSP_OK)
            {
                /* check if all pending requests cancled (should be!!) */
                if ( pHDB->RxPendingCnt )
                {
                    LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"EDD Recv-Requests not canceled!");
                    LLDP_FATAL(LLDP_FATAL_ERR_EDD);
                }
                pHDB->State  = LLDP_HANDLE_STATE_DISABLE_MC_EDD_RUNNING;
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Disable Multicast.");
                Response = LLDP_EDDSetMulticast(pHDB,EDD_MULTICAST_DISABLE);
                if ( Response != LLDP_RSP_OK)
                {
                    LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot disable Multicast (Handle: 0x%X).",pHDB->ThisHandle);
                    Finish = LSA_TRUE;
                }
            }
            else
            {
                /* is this a fatal error ?                      */
                Finish = LSA_TRUE;
            }
            break;
            /*-------------------------------------------------------------------*/
            /* Just finished a Disable Multicast while closing channel           */
            /*-------------------------------------------------------------------*/

        case LLDP_HANDLE_STATE_DISABLE_MC_EDD_RUNNING:
            /* ignore error because it doesnt really matter */
            if ( Response != LLDP_RSP_OK)
            {
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"EDD Disable Multicast failed!");
            }
            pHDB->State  = LLDP_HANDLE_STATE_CLOSE_EDD_RUNNING;
            LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"EDD-Closing Channel.");
            Response = LLDP_EDDCloseChannel(pHDB);
            if ( Response != LLDP_RSP_OK)
            {
                LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"Cannot Close EDD Channel (Handle: 0x%X).",pHDB->ThisHandle);
                Finish = LSA_TRUE;
            }
            break;
            /*-------------------------------------------------------------------*/
            /* Just finished a EDD-Close-Channel request                         */
            /* Note: If handling failed we do not restart the timer or           */
            /*       reprovide rx-resource. The handle will not be useful anymore*/
            /*-------------------------------------------------------------------*/

        case LLDP_HANDLE_STATE_CLOSE_EDD_RUNNING:
            if ( Response == LLDP_RSP_OK)
            {
                /* check if all pending requests cancled (should be!!) */
                if ( pHDB->LinkIndPendingCnt )
                {
                    LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"EDD Link-Indications not canceled!");
                    LLDP_FATAL(LLDP_FATAL_ERR_EDD);
                }
                pHDB->ReleaseSysPtr = LSA_TRUE; /* AP00999034: mark to be released after CBF is called */
				/* we release the HDB in LLDP_CallCbf           */
            }
            else
            {   
                /* is this a fatal error ?                      */
            }
            Finish = LSA_TRUE;
            break;
            /*-------------------------------------------------------------------*/
            /* Other states not expected within this function.                   */
            /*-------------------------------------------------------------------*/

        default:
            LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
            break;
    } /* switch */
    if ( Finish )  /* finish the request ? */
    {
        pHDB->pOCReq = LSA_NULL;  /* to signal no more request pending.      */
        LLDP_RQBSetResponse(pRQB,Response);
        /* upper - handler already set */
        LLDP_UPPER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",LLDP_RQB_GET_OPCODE(pRQB),LLDP_RQB_GET_HANDLE(pRQB),Response);
        LLDP_CallCbf(pHDB,Cbf,pRQB);
    }
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_UserChannelStateMachine()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_UserCloseChannel                       +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                             LLDP_LOCAL_MEM_BOOL_PTR_TYPE pFinish        +*/
/*+                                                                         +*/
/*+  Result                :    LLDP_RSP                                    +*/
/*+                             LLDP_RSP_ERR_SEQUENCE                       +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                : User HDB                                         +*/
/*+  pFinish             : Output: LSA_TRUE : Finish the request            +*/
/*+                                LSA_FALSE: Waiting for TimerRQB          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes User-Handle.                                       +*/
/*+               Stops Cyclic Timer. If Timer RQB is underway we have to   +*/
/*+               wait for the RQB arrives so we go into Wait-for-timer     +*/
/*+               State and leave with pFinish = LSA_FALSE.                 +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LLDP_RSP LLDP_LOCAL_FCT_ATTR LLDP_UserCloseChannel(
    LLDP_HDB_PTR_TYPE             pHDB,
    LLDP_LOCAL_MEM_BOOL_PTR_TYPE  pFinish)
{

    LLDP_RSP                                    Status;
    LSA_BOOL                                    WaitTimer;

    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_UserCloseChannel(pHDBUser: 0x%X)",pHDB);
    Status   = LLDP_RSP_OK;
    *pFinish = LSA_FALSE;
    if (!pHDB->InStartCnt ) /* all ports stopped ? */
    {
        WaitTimer = LSA_FALSE;
        /* if we are not in sate OPEN, a previos attempt to close */
        /* this handle failed and we must not stop the timer again*/
        if ( pHDB->State == LLDP_HANDLE_STATE_OPEN )
        {
            /* if this is the last user, we stop the timer */
            if ( LLDP_MGM_GET_GLOB_PTR()->CyclicTimerUserCnt == 1 )
            {
                LLDP_TimerStop(pHDB,LLDP_MGM_GET_GLOB_PTR()->CyclicTimerID);
                WaitTimer = LSA_TRUE;
            }
            LLDP_MGM_GET_GLOB_PTR()->CyclicTimerUserCnt--;
        }
        /* check if timer RQB is free. if not we have to wait if we are    */
        /* the last one using the timer                                    */
        /* (the timer RQB maybe pending because a timer event occured just */
        /* before we stopped the timer)                                    */
        pHDB->State  = LLDP_HANDLE_STATE_CLOSE_WAIT_TIMER;
        if (!WaitTimer ||
             ( LLDP_TimerRQBCheckFree(LLDP_MGM_GET_GLOB_PTR()->CyclicTimerID)))
        {
            /* dont need to wait for timer. continue... */
            LLDP_UserChannelStateMachine(pHDB,LLDP_RSP_OK);
        }
        else
        {
            LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Have to wait for Timer before closing channel.");
            /* Request is underway now. check in timer-handling again and  */
            /* finish the request.                                         */
        }
    }
    else
    {
        LLDP_UPPER_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_ERROR,"Close User channel failed. Not all Ports stopped!");
        Status   = LLDP_RSP_ERR_SEQUENCE;
        *pFinish = LSA_TRUE;
    }
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_UserCloseChannel()");
    return(Status);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_UserHandleTxDone                       +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                             LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  pPCtrl               : PortControlBlock                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handling after send ready.                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_UserHandleTxDone(
    LLDP_HDB_PTR_TYPE            pHDB,
    LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl)
{

    LSA_BOOL  Ready;
    LLDP_RSP  Response;

    LLDP_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_UserHandleTxDone(pHDB: 0x%X, pPCtrl: 0x%X)",pHDB,pPCtrl);

    Ready    = LSA_FALSE;
    Response = LLDP_RSP_OK;

    switch  (pPCtrl->Tx.State )
    {
        case LLDP_TX_FRAME_STATE_STOP_SEND_SHUTDOWN:
            /* we should send a shutdown for this port...*/
            #ifdef LLDP_CFG_SEND_SHUTDOWN_ON_STOP
            if ( pPCtrl->LinkStatus.Status == EDD_LINK_UP )
            {
                LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Sending shutdown for Port (Port: %d)",pPCtrl->ThisIdx);
                /* send shutdown */
                Response = LLDP_EDDSendLLDPFrame(pHDB,pPCtrl,LSA_TRUE);
                if ( Response == LLDP_RSP_OK )
                {
                    /* wait for Shutdown to be send..*/
                    pPCtrl->Tx.State = LLDP_TX_FRAME_STATE_STOP_WAIT_SHUTDOWN;
                }
                else
                {
                    LLDP_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"Sending shutdown failed (Port: %d, Resp: 0x%X)",pPCtrl->ThisIdx,Response);
                    Ready = LSA_TRUE;
                    pPCtrl->Stats.TxShutdownFailed++;
                }
            }
            else Ready = LSA_TRUE;
            #else
            Ready = LSA_TRUE;
            #endif
            break;

        case LLDP_TX_FRAME_STATE_STOP_WAIT_SHUTDOWN:
            /* sending shutdown finished. */
            LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Sending shutdown finished (Port: %d)",pPCtrl->ThisIdx);
            Ready = LSA_TRUE;
            break;

        case LLDP_TX_FRAME_STATE_START:
            if (pPCtrl->Tx.DoSend )/* should we immediate start a new send ? */
            {
                LLDP_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Send Done. Sending frame for channel %d, port %d.",pHDB->ThisHandle,pPCtrl->ThisIdx);

                /* send the frame */
                if ( LLDP_EDDSendLLDPFrame(pHDB,pPCtrl,LSA_FALSE) != LLDP_RSP_OK )
                {
                    pPCtrl->Stats.TxDataFailed++;
                }
                /* set timer counter for next send */
                pPCtrl->Tx.NextSend = LLDP_MGM_GET_GLOB_PTR()->CyclicTimerCnt+pPCtrl->Tx.Interval+1;
            }
            break;
        default:
            break;
    }
    if ( Ready )
    {
        /* if sending shutdown frame fails or link is down, we  */
        /* continue without shutdown frame for this port.       */

        pPCtrl->Tx.State            = LLDP_TX_FRAME_STATE_STOP_SET;
        pPCtrl->Tx.ReinitDelayCnt   = pHDB->LLDP_ChannelReInitDelay;
        pHDB->InStartCnt--;
        /* if this is the last port to be stopped we finish the */
        /* stop request. PORTid is misused as Counter of        */
        /* remaining ports to be stopped.                       */
        if (--pPCtrl->pStopReq->args.Stop.PORTId == 0)
        {
            LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"All Ports shutdown. Stop ready.");
            /* restore PORTId */
            pPCtrl->pStopReq->args.Stop.PORTId = pPCtrl->StopPORTId;
            /* Finish STOP-Request*/
            LLDP_UserRequestFinish(pHDB,pPCtrl->pStopReq,LLDP_RSP_OK);
            pPCtrl->pStopReq  = LSA_NULL;
        }
    }
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_UserHandleTxDone()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_UserHandleLinkInd                      +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                             LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  pPCtrl               : PortControlBlock                                +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handling of LinkIndications                               +*/
/*+                                                                         +*/
/*+            If the port is in state LLDP_RX_FRAME_STATE_MULTIPLE_SENDER  +*/
/*+            and we get a link-down we go backt to INVALID-state to       +*/
/*+            accept new incoming data.                                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_UserHandleLinkInd(
    LLDP_HDB_PTR_TYPE            pHDB,
    LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl)
{

    LSA_BOOL                    Indicate;
    LLDP_RX_FRAME_INFO_PTR_TYPE  pRx;
    LSA_UINT32                   i;

    LLDP_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_UserHandleLinkInd(pHDB: 0x%X, pPCtrl: 0x%X)",pHDB,pPCtrl);

    Indicate = LSA_FALSE;
    if ( pPCtrl->LinkStatus.Status == EDD_LINK_DOWN )
    {
        pRx = pPCtrl->pRx;

        /* reset too many state, so a new event will be inidicated */
        pPCtrl->TooManyState = LLDP_RX_FRAME_NOT_TOO_MANY;

        /* for all sender we go to invalid-state.*/
        for ( i = 0; i< pHDB->PortMaxSender; i++)
        {
            switch ( pRx->State )
            {
                case LLDP_RX_FRAME_STATE_DATA_VALID:
                    /* if we got a link down while we are in valid we go to */
                    /* invalid state. This is because after Link up we may  */
                    /* got frames from another sender!                      */
                    pRx->State  = LLDP_RX_FRAME_STATE_DATA_INVALID;
                    LLDP_IndicateCheckState(pHDB,pPCtrl,i);
                    break;

                default:
                    break;
            } /* switch */
            pRx++;
        } /* for */
    }
    else /* LINK-UP */
    {
        /* after link-up, if sender is active we send data at once,             */
        /* so the device will signal its presents                               */
        #ifndef LLDP_CFG_DONT_SEND_AFTER_LINK_UP
        switch ( pPCtrl->Tx.State )
        {
            case LLDP_TX_FRAME_STATE_START:
                if (!pPCtrl->Tx.Underway )
                {
                    if (!pPCtrl->Tx.TxDelayCnt )  /* we can send at once. no delay neccessary */
                    {
                        LLDP_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Link-UP. Sending frame for channel %d, port %d.",pHDB->ThisHandle,pPCtrl->ThisIdx);
                        /* send the frame */
                        if ( LLDP_EDDSendLLDPFrame(pHDB,pPCtrl,LSA_FALSE) != LLDP_RSP_OK )
                        {
                            pPCtrl->Stats.TxDataFailed++;
                        }
                        /* set timer counter for next send */
                        pPCtrl->Tx.NextSend = LLDP_MGM_GET_GLOB_PTR()->CyclicTimerCnt+pPCtrl->Tx.Interval+1;
                    }
                    else
                    {
                        LLDP_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Link-UP. Sending frame with delay (%d) for channel %d, port %d.",pPCtrl->Tx.TxDelayCnt,pHDB->ThisHandle,pPCtrl->ThisIdx);
                        /* set timer counter for next send */
                        pPCtrl->Tx.NextSend = LLDP_MGM_GET_GLOB_PTR()->CyclicTimerCnt+pPCtrl->Tx.TxDelayCnt;
                    }
                }
                break;
            default:
                break;
        } /* switch */
        #endif
    }
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_UserHandleLinkInd()");
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_UserHandleRxFrame                      +*/
/*+  Input/Output          :    LLDP_HDB_PTR_TYPE            pHDB           +*/
/*+                             LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl         +*/
/*+                             LLDP_COMMON_MEM_U8_PTR_TYPE  pFrame         +*/
/*+                             LSA_UINT32_                  DataOffset     +*/
/*+                             LSA_UINT32                   DataLen        +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  pPCtrl               : PortControlBlock                                +*/
/*+  pFrame               : pointer to start of IEEE frame                  +*/
/*+  DataOffset           : Offset of start of LLDP-Data within frame       +*/
/*+  DataLen              : length of LLDP-Data (may include pad bytes)     +*/
/*+  IOCount              : length of frame     (may include pad bytes)     +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Handling of incoming LLDP-frames                          +*/
/*+                                                                         +*/
/*+               check for valid LLDP-Frame                                +*/
/*+               check if data changed since last frame                    +*/
/*+               indication statechanges to user                           +*/
/*+               actualize TTL counters                                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_UserHandleRxFrame(
    LLDP_HDB_PTR_TYPE            pHDB,
    LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl,
    LLDP_COMMON_MEM_U8_PTR_TYPE  pFrame,
    LSA_UINT32                   DataOffset,
    LSA_UINT32                   DataLen)
{

    LSA_UINT32                   NLength;
    LSA_UINT32                   TTL;
    LSA_BOOL                     NewData;
    LSA_BOOL                     EndPresent;
    LLDP_RSP                     Response;
    LLDP_RX_FRAME_INFO_PTR_TYPE  pRx;
    LSA_BOOL                     ExistingFound;
    LSA_BOOL                     FreeFound;
    LSA_UINT32                   FreeIdx;
    LSA_UINT32                   i;


    LLDP_FUNCTION_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_UserHandleRxFrame(pHDB: 0x%X, pPCtrl: 0x%X)",pHDB,pPCtrl);

    LLDP_ASSERT_NULL_PTR(pPCtrl);
    LLDP_ASSERT_NULL_PTR(pFrame);

    NewData       = LSA_FALSE;
    ExistingFound = LSA_FALSE;
    pRx           = LSA_NULL;

    /*---------------------------------------------------------------------------*/
    /* We only handle frame if link is up and receive is enabled                 */
    /*---------------------------------------------------------------------------*/

    if ( (pPCtrl->LinkStatus.Status == EDD_LINK_UP) && (!pPCtrl->RxStoped) )
    {
        /* Validate the TLV-Chain */
        Response = LLDP_TLVValidateChain(
                       &pFrame[DataOffset],
                       DataLen,
                       &TTL,
                       &NLength,
                       &EndPresent);

		if (( Response == LLDP_RSP_OK )&& EndPresent) /* AP00988794: check also for END-TLV */
        {
            i              = 0;
            pRx            = pPCtrl->pRx;
            FreeIdx        = 0;
            FreeFound      = LSA_FALSE;

            /* -------------------------------------------------------------------*/
            /* search for source-mac within Rx-array, or new free idx             */
            /* -------------------------------------------------------------------*/

            while ((!ExistingFound ) && ( i < pHDB->PortMaxSender))
            {
                if ( pRx->State == LLDP_RX_FRAME_STATE_DATA_VALID )
                {
                    /* compare MAC-SRC. If the MAC differs, we have a different sender */

                    if ( LLDP_MEMCMP_COMMON(
                             (LLDP_COMMON_MEM_PTR_TYPE) &pFrame[LLDP_FRAME_SRC_MAC_OFFSET],
                             (LLDP_COMMON_MEM_PTR_TYPE) &pRx->Frame[LLDP_FRAME_SRC_MAC_OFFSET],
                             LLDP_MAC_ADDR_SIZE) )
                    {
                        ExistingFound = LSA_TRUE;
                    }
                }
                else /* this is a free for use index */
                {
                    if (!FreeFound )
                    {
                        FreeIdx   = i;
                        FreeFound = LSA_TRUE;
                    }
                }
                if (!ExistingFound )
                {
                    pRx++;  /* next slot if not found yet */
                }
                i++;
            }
            /* -------------------------------------------------------------------*/
            if ( ExistingFound ) /* New data for a already present sender arrived.*/
            {
                if ( TTL == 0 ) /* we got a shutdown LLDP-frame */
                {
                    LLDP_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Got Shutdown frame for Channel %d Port %d",pHDB->ThisHandle,pPCtrl->ThisIdx);
                    pRx->State = LLDP_RX_FRAME_STATE_SHUTDOWN;
                    LLDP_IndicateCheckState(pHDB,pPCtrl,pRx->ThisIdx);
                }
                else
                {
                    /* different TLV-length ? -> new Data arrived from sender */
                    if ( pRx->TLVDataLength != NLength )
                    {
                        NewData  = LSA_TRUE;
                    }
                    else
                    {
                        /* same length. compare TLV-chain */
                        if (!LLDP_MEMCMP_COMMON(
                                 (LLDP_COMMON_MEM_PTR_TYPE) &pFrame[DataOffset],
                                 (LLDP_COMMON_MEM_PTR_TYPE) &pRx->Frame[pRx->TLVDataOffset],
                                 NLength ))
                        {
                            /* buffers differ -> new Data */
                            NewData  = LSA_TRUE;
                        }
                        else
                        {
                            /* same TLV received. update elaspsecounter only    */
                            /* (we add 1 because we have a jitter of 1)         */
                            pRx->ElapseCnt = LLDP_MGM_GET_GLOB_PTR()->CyclicTimerCnt + pRx->TTLCnt+1;
                        }
                    }
                }
            }
            else /* new sender */
            {    /* we ignore shutdown for a new sender */
                if ( TTL != 0 ) /* no shutdown */
                {
                    if (!FreeFound )
                    {
                        /* Too Many Senders */
                        LLDP_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_ERROR,"To many senders for Channel %d Port %d",pHDB->ThisHandle,pPCtrl->ThisIdx);
                        pPCtrl->Stats.RxTooManyTotal++;
                        /* if we dont already have too many indicated, we need to..*/
                        if ( pPCtrl->TooManyState == LLDP_RX_FRAME_NOT_TOO_MANY )
                        {
                            pPCtrl->TooManyState = LLDP_RX_FRAME_TOO_MANY_OCCURED;
                            LLDP_IndicateCheckTooMany(pHDB,pPCtrl);
                        }
                    }
                    else
                    {
                        /* New Sender and we have a free slot */
                        pRx     = &pPCtrl->pRx[FreeIdx];
                        NewData = LSA_TRUE;
                    }
                }
            }
            /* -------------------------------------------------------------------*/
        }
        else
        {
            pPCtrl->Stats.RxFramesDiscardedTotal++;
            pPCtrl->Stats.RxFramesInErrors++;
        }

        /* -----------------------------------------------------------------------*/
        /* if new valid data arrived we copy the data parameters to port mgm.     */
        /* -----------------------------------------------------------------------*/

        if ( NewData )
        {
            LLDP_PROGRAM_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"New frame for Channel %d Port %d Idx: %d",pHDB->ThisHandle,pPCtrl->ThisIdx,pRx->ThisIdx);

            pRx->TLVDataLength = NLength;
            pRx->TLVDataOffset = DataOffset;
            pRx->FrameLen      = DataOffset + NLength,
                                 /* copy frame to port-buffer */
                                 LLDP_MEMCOPY_COMMON(
                                     (LLDP_COMMON_MEM_PTR_TYPE) pRx->Frame,
                                     (LLDP_COMMON_MEM_PTR_TYPE) pFrame,
                                     pRx->FrameLen);

            pRx->TTLCnt      = TTL;
            /* (we add 1 because we have a jitter of 1)         */
            pRx->ElapseCnt   = LLDP_MGM_GET_GLOB_PTR()->CyclicTimerCnt + TTL +1;

            /* this is a little special. If we got a new sender and the user*/
            /* does not yet have a indication that the last one got invalid */
            /* (can only occur if indication resources missing) we set a    */
            /* flag that we have to do it later.                            */

            if (!ExistingFound )
            {
                /* user thinks last one still valid (this can only occur, if*/
                /* no indication resources present.)                        */
                if ( pRx->Ind.StateIndicated == LLDP_RX_FRAME_STATE_DATA_VALID )
                {
                    /* if this is a differnt sender then last valid one..   */
                    if (!LLDP_MEMCMP_COMMON(
                             (LLDP_COMMON_MEM_PTR_TYPE) &pRx->Ind.LastMACAddr,
                             (LLDP_COMMON_MEM_PTR_TYPE) &pRx->Frame[LLDP_FRAME_SRC_MAC_OFFSET],
                             LLDP_MAC_ADDR_SIZE) )
                    {
                        /* will cause a lastinvalid-indication after new ind-res.*/
                        /* provided, so the user knows the last one got invalid  */
                        pRx->Ind.IndLastInvalid = LSA_TRUE;
                    }
                }
            }
            pRx->Ind.Changed  = LSA_TRUE;
            pRx->State = LLDP_RX_FRAME_STATE_DATA_VALID;
            LLDP_IndicateCheckState(pHDB,pPCtrl,pRx->ThisIdx);
        } /* new data */
    } /* link up or loopback */
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_UserHandleRxFrame()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_RequestTimer                           +*/
/*+  Input/Output          :    LLDP_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:     LLDP_OPC_TIMER                  +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LLDP_RSP                Response    Response                        +*/
/*+     LLDP_RQB_ERROR_TYPE     Error       Addition errorcodes from LowerL.+*/
/*+     LLDP_RQB_ARGS_TYPE      args.Timer: LLDP_RQB_TIMER_TYPE             +*/
/*+                                                                         +*/
/*+     args.Timer:                                                         +*/
/*+                                                                         +*/
/*+     TimerID  : LLDP-Timer ID                                            +*/
/*+     UserID   : not used yet                                             +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request initiated from LLDP within lldp_timeout().        +*/
/*+               Handles Cyclic Timer event.                               +*/
/*+                                                                         +*/
/*+               Send data and checks for TTL.                             +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_RequestTimer(LLDP_UPPER_RQB_PTR_TYPE pRQB)
{

    LLDP_HDB_PTR_TYPE            pHDB,pHDBinitial;
    LSA_UINT32                   Channel,i,j;
    LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl;
    LLDP_RX_FRAME_INFO_PTR_TYPE  pRx;
    LSA_UINT32                   TimerCnt;

    pHDBinitial = pHDB = LLDP_MGM_GET_HUSER_PTR()->HDB;
    LLDP_ASSERT_NULL_PTR(pHDB);

    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_RequestTimer(pRQB: 0x%X)",pRQB);

    if (pRQB->args.Timer.UserID !=  LLDP_TIMER_USER_ID_CYLIC )
    {
        LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"lldp_RequestTimer. Invalid Timer-UserID (0x%X)",pRQB->args.Timer.UserID);
        LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
    }
    LLDP_TimerRQBFree(pRQB->args.Timer.TimerID);
    TimerCnt = ++LLDP_MGM_GET_GLOB_PTR()->CyclicTimerCnt;
    /* For all Channels we do the timer handling.                          */
    for (Channel=0; Channel<LLDP_CFG_MAX_CHANNELS; Channel++)
    {
        /* check if we are in waiting for Timer to close the channel. If so we */
        /* finish the close-handling                                           */
        /* Note: Currently we only have one User-Handle!!                      */
        switch (pHDB->State)
        {
            case LLDP_HANDLE_STATE_CLOSE_WAIT_TIMER:
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Timer event while closing. continue.");

                if (!LSA_HOST_PTR_ARE_EQUAL(pHDB->pOCReq, LSA_NULL) )
                {
                    /* timer RQB is free now. */
                    LLDP_UserChannelStateMachine(pHDB,LLDP_RSP_OK);
                }
                else
                {
                    LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"pOCReq is NULL. Cannot Close channel");
                    LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
                }
                break;
            case LLDP_HANDLE_STATE_OPEN:
                pPCtrl = pHDB->pPCtrl;
                for ( j = 0; j < pHDB->PortCnt; j++) /* for all ports */
                {
                    /* ---------------------------------------------- */
                    /* Send-handling first                            */
                    /* check for Ports to be sended and start sending */
                    /* ---------------------------------------------- */

                    if ( pPCtrl->Tx.State == LLDP_TX_FRAME_STATE_START )
                    {
                        /* send timer reached ? */
                        if ( pPCtrl->Tx.NextSend == TimerCnt )
                        {
                            /* set timer counter for next send */
                            pPCtrl->Tx.NextSend += pPCtrl->Tx.Interval;

                            if (!pPCtrl->Tx.Underway )
                            {
                                /* only send if link is up. */
                                if ( pPCtrl->LinkStatus.Status == EDD_LINK_UP )
                                {
                                    LLDP_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Sending frame for channel %d, port %d.",Channel,j);
                                    /* send the frame */
                                    if ( LLDP_EDDSendLLDPFrame(pHDB,pPCtrl,LSA_FALSE) != LLDP_RSP_OK )
                                    {
                                        pPCtrl->Stats.TxDataFailed++;
                                    }
                                    else
                                    {
                                        /* set delay-counter. Used to delay first send after new data or */
                                        /* after LinkUP                                                  */
                                        pPCtrl->Tx.TxDelayCnt = pHDB->LLDP_ChannelTxDelay+1; /* take channel specific setting */
                                    }
                                }
                            }
                            else
                            {
                                pPCtrl->Stats.TxStillUnderway++;
                            }
                        }
                    }
                    /* decrement ReinitDelayCnt */
                    if ( pPCtrl->Tx.ReinitDelayCnt)
                    {
                        pPCtrl->Tx.ReinitDelayCnt--;
                    }
                    /* decrement TxDelayCnt */
                    if ( pPCtrl->Tx.TxDelayCnt)
                    {
                        pPCtrl->Tx.TxDelayCnt--;
                    }

                    /* ---------------------------------------------- */
                    /* TTL-Checks for all sender of port              */
                    /* ---------------------------------------------- */
                    pRx = pPCtrl->pRx;
                    for ( i =0; i<pHDB->PortMaxSender; i++ )
                    {
                        /* we only need to check if data is valid */
                        if ( pRx->State == LLDP_RX_FRAME_STATE_DATA_VALID )
                        {
                            if ( pRx->ElapseCnt == TimerCnt )
                            {
                                LLDP_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"TTL expired for channel %d, port %d.",Channel,j);

                                /* TTL timed out and no new Data arrived */
                                /* -> Data gets invalid.                 */

                                pPCtrl->Stats.RxAgeoutsTotal++;

                                pRx->State = LLDP_RX_FRAME_STATE_DATA_INVALID;
                                LLDP_IndicateCheckState(pHDB,pPCtrl,i);

                            } /* timer elapsed */
                        } /* if data valid */
                        pRx++; /* next sender */
                    } /* for */
                    pPCtrl++; /* next port */
                }
                break;
            default:
                /* ignore timer */
                break;
        }
        pHDB++;
    }
    /* ap01177368 on exit use the initial pHDB */
    LLDP_FUNCTION_TRACE_00(pHDBinitial->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_RequestTimer()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_RequestIndicationProvide               +*/
/*+  Input/Output          :    LLDP_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                        :    LLDP_HDB_PTR_TYPE           pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      LLDP_OPC_INDICATION_PROVIDE    +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LLDP_RSP                Response     Response                       +*/
/*+     LLDP_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     LLDP_RQB_ARGS_TYPE      args.IndProvide                             +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to provide Indication Resources                   +*/
/*+                                                                         +*/
/*+               Queues the RQB wthin User-Handlemanagement                +*/
/*+               If this is the first RQB we check if some indications     +*/
/*+               pending to be indicated. If so we indicate them, if not   +*/
/*+               we queue the request.                                     +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_RequestIndicationProvide(
    LLDP_UPPER_RQB_PTR_TYPE   pRQB,
    LLDP_HDB_PTR_TYPE         pHDB)
{

    LSA_UINT32                   j,i;
    LSA_BOOL                     Ready;
    LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl;
    LLDP_RSP                     Response;

    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_RequestIndicationProvide(pRQB: 0x%X)",pRQB);

    Response = LLDP_RSP_OK;

    LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Indication provide. Queuing block (pRQB: 0x%X).",pRQB);

    /* queue the request */
    LLDP_RQB_PUT_BLOCK_TOP(pHDB->IndReq.pBottom,
                           pHDB->IndReq.pTop,
                           pRQB);

    pHDB->IndReqPending++;

    /* if this is the first Indication Resource, we check if some */
    /* indications pending                                        */
    if ( pHDB->IndReqPending == 1 )
    {
        LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Indication provide. Check indications.");

        Ready = LSA_FALSE;
        j      = pHDB->PortCnt;
        pPCtrl = &pHDB->pPCtrl[0];

        /*-----------------------------------------------------------------------*/
        /* First we check if some old INVALID-States have to be indicated.       */
        /* Note: we check all ports/sender to indicate an INVALID before a VALID */
        /*       so the user dont becomes too much confused.                     */
        /*-----------------------------------------------------------------------*/
        while ( j  && (! Ready)) /* all Ports */
        {
            i  = 0;
            /* all sender (rx) */
            while ( (i<pHDB->PortMaxSender) && (! Ready ))
            {
                /* check for last-invalid to be indicated first!!. */
                LLDP_IndicateCheckLastInvalid(pHDB,pPCtrl,i);
                if (!pHDB->IndReqPending ) /* no more ind. resources pending ? */
                {
                    Ready = LSA_TRUE;
                }
                i++;
            }
            pPCtrl++;
            j--;
        }
        j      = pHDB->PortCnt;
        pPCtrl = &pHDB->pPCtrl[0];

        /*-----------------------------------------------------------------------*/
        /* Next we check if some New State present.                              */
        /*-----------------------------------------------------------------------*/
        while ( j  && (! Ready))    /* all Ports */
        {
            i   = 0;
            /* all sender (rx) */
            while ( (i<pHDB->PortMaxSender) && (! Ready ))
            {
                /* check for new states to be indicated . */
                LLDP_IndicateCheckState(pHDB,pPCtrl,i);
                if (!pHDB->IndReqPending ) /* no more ind. resources pending ? */
                {
                    Ready = LSA_TRUE;
                }
                i++;
            }
            pPCtrl++;
            j--;
        }
        j      = pHDB->PortCnt;
        pPCtrl = &pHDB->pPCtrl[0];

        /*-----------------------------------------------------------------------*/
        /* Next we check if a too many sender event occured.                     */
        /*-----------------------------------------------------------------------*/
        while ( j  && (! Ready)) /* all Ports */
        {
            /* check for too many sender occured for port */
            LLDP_IndicateCheckTooMany(pHDB,pPCtrl);
            if (!pHDB->IndReqPending ) /* no more ind. resources pending ? */
            {
                Ready = LSA_TRUE;
            }
            pPCtrl++;
            j--;
        }
    }
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_RequestIndicationProvide()");
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_RequestGetConfig                       +*/
/*+  Input/Output          :    LLDP_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                        :    LLDP_HDB_PTR_TYPE           pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      LLDP_OPC_GET_CONFIG            +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LLDP_RSP                Response     Response                       +*/
/*+     LLDP_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     LLDP_RQB_ARGS_TYPE      args.GetConfig                              +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to get config parameters from LLDP                +*/
/*+                                                                         +*/
/*+               Fills RQB with config data and confirms request.          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_RequestGetConfig(
    LLDP_UPPER_RQB_PTR_TYPE   pRQB,
    LLDP_HDB_PTR_TYPE         pHDB)
{

    LLDP_RSP                  Response;

    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_RequestGetConfig(pRQB: 0x%X)",pRQB);

    Response = LLDP_RSP_OK;

    pRQB->args.GetConfig.PORTCnt       = (LSA_UINT16) pHDB->PortCnt;
    pRQB->args.GetConfig.PORTMaxSender = (LSA_UINT16) pHDB->PortMaxSender;
    LLDP_UserRequestFinish(pHDB,pRQB,Response);
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_RequestGetConfig()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_RequestSetGlobals                      +*/
/*+  Input/Output          :    LLDP_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                        :    LLDP_HDB_PTR_TYPE           pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      LLDP_OPC_GET_CONFIG            +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LLDP_RSP                Response     Response                       +*/
/*+     LLDP_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     LLDP_RQB_ARGS_TYPE      args.GetConfig                              +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to get config parameters from LLDP                +*/
/*+                                                                         +*/
/*+               Fills RQB with config data and confirms request.          +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_RequestSetGlobals(
    LLDP_UPPER_RQB_PTR_TYPE   pRQB,
    LLDP_HDB_PTR_TYPE         pHDB)
{

    LLDP_RSP                  Response;

    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_RequestSetGlobals(pRQB: 0x%X)",pRQB);

    Response = LLDP_RSP_OK;

    if((pRQB->args.SetGlobals.MsgTxInterval > 0xffff ) || (pRQB->args.SetGlobals.MsgTxInterval == 0))
        Response = LLDP_RSP_ERR_PARAM;
    if(pRQB->args.SetGlobals.ReInitDelay > 10)
        Response = LLDP_RSP_ERR_PARAM;
    if((pRQB->args.SetGlobals.TxDelay > 8192))
        Response = LLDP_RSP_ERR_PARAM;
    if(Response == LLDP_RSP_OK)
    { /* all is ok then set the variables */
        pHDB->LLDP_ChannelMsgTxInterval  = pRQB->args.SetGlobals.MsgTxInterval;
        pHDB->LLDP_ChannelReInitDelay    = pRQB->args.SetGlobals.ReInitDelay;
        pHDB->LLDP_ChannelTxDelay        = pRQB->args.SetGlobals.TxDelay;
    }
    LLDP_UserRequestFinish(pHDB,pRQB,Response);
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_RequestSetGlobals()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_RequestStats                           +*/
/*+  Input/Output          :    LLDP_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                        :    LLDP_HDB_PTR_TYPE           pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      LLDP_OPC_STATS                 +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LLDP_RSP                Response     Response                       +*/
/*+     LLDP_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     LLDP_RQB_ARGS_TYPE      args.Stats                                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to get config statistics from LLDP                +*/
/*+                                                                         +*/
/*+               Fills RQB with stats data and confirms request.           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_RequestStats(
    LLDP_UPPER_RQB_PTR_TYPE   pRQB,
    LLDP_HDB_PTR_TYPE         pHDB)
{

    LLDP_RSP                  Response;
    LLDP_EDD_PCTRL_PTR_TYPE   pPCtrl;

    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_RequestStats(pRQB: 0x%X)",pRQB);

    Response = LLDP_RSP_OK;

    if (( pRQB->args.Stats.PORTId > 0 ) &&
        ( pRQB->args.Stats.PORTId <= pHDB->PortCnt ))
    {
        /* copy statistics to RQB */
        pPCtrl = &pHDB->pPCtrl[pRQB->args.Stats.PORTId-1];
        pRQB->args.Stats.Cntr = pPCtrl->Stats;
        /* reset to 0 if requested */
        if (pRQB->args.Stats.Reset)
        {
            LLDP_MEMSET_LOCAL(&pPCtrl->Stats,0,sizeof(pPCtrl->Stats));
        }
    }
    else
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    LLDP_UserRequestFinish(pHDB,pRQB,Response);
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_RequestStats()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_RequestGet                             +*/
/*+  Input/Output          :    LLDP_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                        :    LLDP_HDB_PTR_TYPE           pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      LLDP_OPC_GET                   +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LLDP_RSP                Response     Response                       +*/
/*+     LLDP_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     LLDP_RQB_ARGS_TYPE      args.Get                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to get actual TLV for NIC/Port                    +*/
/*+                                                                         +*/
/*+               Fills RQB with TLV data and confirms request.             +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_RequestGet(
    LLDP_UPPER_RQB_PTR_TYPE   pRQB,
    LLDP_HDB_PTR_TYPE         pHDB)
{

    LLDP_RSP                    Response;
    LLDP_EDD_PCTRL_PTR_TYPE     pPCtrl;
    LLDP_RX_FRAME_INFO_PTR_TYPE pRx;

    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_RequestGet(pRQB: 0x%X)",pRQB);

    Response = LLDP_RSP_OK;

    if (( pRQB->args.Get.PORTId   >  0             ) &&
        ( pRQB->args.Get.PORTId   <= pHDB->PortCnt ) &&
        ( pRQB->args.Get.SenderId >  0             ) &&
        ( pRQB->args.Get.SenderId <= pHDB->PortMaxSender ))
    {
        LLDP_ASSERT_NULL_PTR(pHDB->pPCtrl);
        LLDP_ASSERT_NULL_PTR(pHDB->pPCtrl->pRx);
        pPCtrl = &pHDB->pPCtrl[pRQB->args.Get.PORTId-1];
        pRx    = &pPCtrl->pRx[pRQB->args.Get.SenderId-1];
        if ( pRx->State == LLDP_RX_FRAME_STATE_DATA_VALID )
        {
            pRQB->args.Get.Present    = LSA_TRUE;

            LLDP_MEMCOPY_COMMON(
                (LLDP_COMMON_MEM_PTR_TYPE) pRQB->args.Get.pTLV->Data,
                (LLDP_COMMON_MEM_PTR_TYPE) &pRx->Frame[pRx->TLVDataOffset],
                pRx->TLVDataLength);

            pRQB->args.Get.TLVLength = pRx->TLVDataLength;
            LLDP_COPY_MAC_ADDR_COMMON(
                (LLDP_COMMON_MEM_PTR_TYPE) pRQB->args.Get.MACAddr,
                (LLDP_COMMON_MEM_PTR_TYPE) &pRx->Frame[LLDP_FRAME_SRC_MAC_OFFSET]);
        }
        else
        {
            pRQB->args.Get.Present    = LSA_FALSE;
            pRQB->args.Get.TLVLength  =0;
        }
    }
    else
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    LLDP_UserRequestFinish(pHDB,pRQB,Response);
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_RequestGet()");
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_RequestSet                             +*/
/*+  Input/Output          :    LLDP_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                        :    LLDP_HDB_PTR_TYPE           pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      LLDP_OPC_SET                   +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LLDP_RSP                Response     Response                       +*/
/*+     LLDP_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     LLDP_RQB_ARGS_TYPE      args.Set                                    +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to set new TLV-Data for sender.                   +*/
/*+               Set new Send-interval.                                    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_RequestSet(
    LLDP_UPPER_RQB_PTR_TYPE   pRQB,
    LLDP_HDB_PTR_TYPE         pHDB)
{

    LLDP_RSP                     Response;
    LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl;
    LSA_BOOL                     EndPresent;
    LSA_UINT32                   TTL;
    LLDP_COMMON_MEM_U8_PTR_TYPE  pTLV;
    LSA_UINT32                   TLVLength;
    LSA_UINT32                   NLength; /* netto length */
    LSA_BOOL                     Skip;

    LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_RequestSet(pRQB: 0x%X)",pRQB);

    Response = LLDP_RSP_OK;
    pTLV     = LSA_NULL;

    if ( ( pRQB->args.Set.PORTId    >  0             ) &&
         ( pRQB->args.Set.PORTId    <= pHDB->PortCnt ) &&
         ( pRQB->args.Set.TLVLength <= LLDP_MAX_TLV_DATA_SIZE ) &&
         (!LSA_HOST_PTR_ARE_EQUAL(pRQB->args.Set.pTLV, LSA_NULL))/*  &&
          removed MsgTxInterval from set !
         ( pRQB->args.Set.MsgTxInterval != 0                 ) &&
         ( pRQB->args.Set.MsgTxInterval >= LLDP_CFG_TX_DELAY )*/)
    {
        LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Set for port %d",pRQB->args.Set.PORTId);
        pPCtrl = &pHDB->pPCtrl[pRQB->args.Set.PORTId-1];
        /* Validate the TLV-Chain */
        Response = LLDP_TLVValidateChain(
                                            (LLDP_COMMON_MEM_U8_PTR_TYPE) pRQB->args.Set.pTLV,
                                            pRQB->args.Set.TLVLength,
                                            &TTL,
                                            &NLength,
                                            &EndPresent);
        /* If the user sets a frame with TLVs behind the End-TLV we */
        /* set a error.                                             */
        if (( Response == LLDP_RSP_OK ) &&
            ( pRQB->args.Set.TLVLength != NLength ))
        {
            Response = LLDP_RSP_ERR_TLV_MISMATCH;
        }
        /* If chain is ok (including TLVLength <= LLDP_MAX_TLV_DATA_SIZE) we continue */
        if ( Response == LLDP_RSP_OK )
        {
            TLVLength = pRQB->args.Set.TLVLength;
            /* We append a End-TLV if not present */
            if (! EndPresent )
            {
                /* check if we have enough room to append a End-TLV */
                if ( TLVLength <= (LLDP_MAX_TLV_DATA_SIZE - (LLDP_TL_SIZE + LLDP_TLV_LLDPDU_END_SIZE)) )
                {
                    pTLV = (LLDP_COMMON_MEM_U8_PTR_TYPE) LLDP_MGM_GET_GLOB_PTR()->Temp;
                    LLDP_MEMCOPY_COMMON(
                        (LLDP_COMMON_MEM_PTR_TYPE) pTLV,
                        (LLDP_COMMON_MEM_PTR_TYPE) pRQB->args.Set.pTLV,
                        TLVLength);
                    pTLV = &LLDP_MGM_GET_GLOB_PTR()->Temp[TLVLength];
                    /* append End-TLV */
                    LLDP_TLVSetCommon(
                        &pTLV,
                        &TLVLength,
                        LLDP_TLV_TYPE_LLDPDU_END,
                        LLDP_TLV_LLDPDU_END_SIZE,
                        LSA_NULL);
                    /* point to start of TLV */
                    pTLV = (LLDP_COMMON_MEM_U8_PTR_TYPE) LLDP_MGM_GET_GLOB_PTR()->Temp;
                }
                else
                {
                    /* no room to append End TLV */
                    Response = LLDP_RSP_ERR_PARAM;
                }
            }
            else
            {
                pTLV = (LLDP_COMMON_MEM_U8_PTR_TYPE) pRQB->args.Set.pTLV;
            }
            if ( Response == LLDP_RSP_OK )
            {
                /* check if TLVChain not equal to the one within Frame */
                /* If equal we do nothing. If nothing is set or the    */
                /* interval is changed we always trigger a new send.   */
                Skip = LSA_FALSE;
                if (( pPCtrl->Tx.State    !=  LLDP_TX_FRAME_STATE_STOP    ) && 
                    ( pPCtrl->Tx.Interval == pHDB->LLDP_ChannelMsgTxInterval ))
                {  
                    if( LLDP_CheckTLVEqual(
                        (LLDP_COMMON_MEM_U8_PTR_TYPE) pPCtrl->Tx.Frame,
                        pTLV,
                        TLVLength))
                    {
                        Skip = LSA_TRUE;
                    }
                }
                if (! Skip)
                {
                    LLDP_ConstructFrame(
                        (LLDP_COMMON_MEM_U8_PTR_TYPE) pPCtrl->Tx.Frame,
                        pTLV,
                        TLVLength,
                        (LLDP_COMMON_MEM_U8_PTR_TYPE) &pPCtrl->MACAddr[0]);

                    pPCtrl->Tx.DataLen  = TLVLength + LLDP_FRAME_HEADER_SIZE;
                    pPCtrl->Tx.DataNew  = LSA_TRUE;
                    pPCtrl->Tx.Interval = pHDB->LLDP_ChannelMsgTxInterval;

                    if ( pPCtrl->Tx.State == LLDP_TX_FRAME_STATE_STOP )
                    {
                        pPCtrl->Tx.State = LLDP_TX_FRAME_STATE_STOP_SET;
                    }
                    /*-----------------------------------------------------------------*/
                    /* if we already started we and we can send immediatly, we do so   */
                    /*-----------------------------------------------------------------*/
                    if (( pPCtrl->Tx.State == LLDP_TX_FRAME_STATE_START ) && (!pPCtrl->Tx.TxDelayCnt ))
                    {
                        if (!pPCtrl->Tx.Underway ) /* send not currently underway ? */
                        {
                            LLDP_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Set. Sending frame for channel %d, port %d.",pHDB->ThisHandle,pPCtrl->ThisIdx);

                            /* send the frame */
                            if ( LLDP_EDDSendLLDPFrame(pHDB,pPCtrl,LSA_FALSE) != LLDP_RSP_OK )
                            {
                                pPCtrl->Stats.TxDataFailed++;
                            }

                            /* set timer counter for next send */
                            pPCtrl->Tx.NextSend = LLDP_MGM_GET_GLOB_PTR()->CyclicTimerCnt+pPCtrl->Tx.Interval+1;
                        }
                        else
                        {
                            LLDP_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Set. Sending underway. send delayed for channel %d, port %d.",pHDB->ThisHandle,pPCtrl->ThisIdx);
                            /* set flag to send after current send finished */
                            pPCtrl->Tx.DoSend = LSA_TRUE;
                        }
                    }
                    else
                    {
                        /* We will cause this new data to be send with next cycle */
                        /* cycle (takes only effect if already started)           */

                        pPCtrl->Tx.NextSend = LLDP_MGM_GET_GLOB_PTR()->CyclicTimerCnt+1+pPCtrl->Tx.TxDelayCnt;
                    }
                }
                else
                {
                    LLDP_UPPER_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"LLDP_RequestSet. Identical TLV. Do nothing");
                }
            }
        }
        else
        {
            LLDP_UPPER_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_ERROR,"LLDP_RequestSet. Invalid TLV");
        }
    }
    else
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    LLDP_UserRequestFinish(pHDB,pRQB,Response);
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_RequestSet()");
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_RequestStart                           +*/
/*+  Input/Output          :    LLDP_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                        :    LLDP_HDB_PTR_TYPE           pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      LLDP_OPC_START                 +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LLDP_RSP                Response     Response                       +*/
/*+     LLDP_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     LLDP_RQB_ARGS_TYPE      args.Start                                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to start TLV-Data sending.                        +*/
/*+               Only valid if Data set for this Port                      +*/
/*+               If STOP is in progress we abort                           +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_RequestStart(
    LLDP_UPPER_RQB_PTR_TYPE   pRQB,
    LLDP_HDB_PTR_TYPE         pHDB)
{
    LLDP_RSP                     Response;
    LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl;
    LSA_UINT32                   Start,End;
    LSA_UINT16                   PortID;
    LSA_UINT32                   i;

    LLDP_FUNCTION_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_RequestStart(pRQB: 0x%X) Port: %d Mode: %d",pRQB,pRQB->args.Start.PORTId,pRQB->args.Start.Mode);

    Response = LLDP_RSP_OK;
    if((pRQB->args.Start.Mode == 0) ||(pRQB->args.Start.Mode > LLDP_RXTX_MODE))
    {   /* invalid Mode */
        Response = LLDP_RSP_ERR_PARAM;
    }
    else
    {
        PortID   = pRQB->args.Start.PORTId;
        if ((( PortID > 0 ) && ( PortID <= pHDB->PortCnt )) || ( PortID == LLDP_PORT_ID_ALL ))
        {
            if ( PortID == LLDP_PORT_ID_ALL  ) /* start all ports */
            {
                LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Starting all ports");
                Start = 0;
                End   = pHDB->PortCnt-1;
            }
            else
            {
                LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Starting port %d",PortID);
                Start = PortID-1;
                End   = PortID-1;
            }
            pPCtrl = &pHDB->pPCtrl[Start];
            /* check if we can Start the/all requested ports */
            for ( i=Start; i<= End; i++)
            {
                /* check if we need to enable receiving */
                if(pRQB->args.Start.Mode & LLDP_RX_MODE ) /*Rx bit set */
                {
                    pPCtrl->RxStoped = LSA_FALSE;
                }
                if(pRQB->args.Start.Mode & LLDP_TX_MODE ) /*Tx bit set */
				{
					switch (pPCtrl->Tx.State)
					{
					case LLDP_TX_FRAME_STATE_STOP_SEND_SHUTDOWN:
					case LLDP_TX_FRAME_STATE_STOP_WAIT_SHUTDOWN:
					case LLDP_TX_FRAME_STATE_STOP_SHUTDOWN_DELAY:
						LLDP_UPPER_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_ERROR,"LLDP_RequestStop. Stop still in progress (Port:%d)",i);
						Response = LLDP_RSP_ERR_SEQUENCE;
						break;

					case LLDP_TX_FRAME_STATE_STOP:
						/* No valid tx-Data set yet, so we cant start */
						LLDP_UPPER_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_ERROR,"LLDP_RequestSet. No Date set yet (Port: %d)",i);
						Response = LLDP_RSP_ERR_SEQUENCE;
						break;

					case LLDP_TX_FRAME_STATE_STOP_SET:
					case LLDP_TX_FRAME_STATE_START:
						/* we can start or dont need to start */
						break;

					default:
						LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"LLDP_RequestStart. Invalid State (0x%X)",pPCtrl->Tx.State);
						LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
						break;
					}
				} /* block moved RQ-AP01464049 */
				pPCtrl++;
			}
			if ( (pRQB->args.Start.Mode & LLDP_TX_MODE ) /* Tx bit set */
				&& ( Response == LLDP_RSP_OK ))          /* and if all ok: start all requested ports */
			{
				pPCtrl = &pHDB->pPCtrl[Start];
				/* Start all ports */
				for ( i=Start; i<= End; i++)
				{
					if (pPCtrl->Tx.State == LLDP_TX_FRAME_STATE_STOP_SET )
					{
						/* we are in stop and valid tx-Data present. we can start */
						pPCtrl->Tx.State    = LLDP_TX_FRAME_STATE_START;
						if (! pPCtrl->Tx.ReinitDelayCnt) /* if no delay we start sending */
						{
							LLDP_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Start. Sending frame for channel %d, port %d.",pHDB->ThisHandle,pPCtrl->ThisIdx);
							/* send the frame */
							if ( LLDP_EDDSendLLDPFrame(pHDB,pPCtrl,LSA_FALSE) != LLDP_RSP_OK )
							{
								pPCtrl->Stats.TxDataFailed++;
							}
							/* set timer counter for next send */
							pPCtrl->Tx.NextSend = LLDP_MGM_GET_GLOB_PTR()->CyclicTimerCnt+pPCtrl->Tx.Interval+1;
						}
						else
						{	/* The first send will occur with ReinitDelay  */
							pPCtrl->Tx.NextSend = LLDP_MGM_GET_GLOB_PTR()->CyclicTimerCnt+1+pPCtrl->Tx.ReinitDelayCnt;
						}
						/* set DataNew, because we may resently used the Tx-buffer*/
						/* for shutdown                                           */
						pPCtrl->Tx.DataNew  = LSA_TRUE;
						pHDB->InStartCnt++;
					}
					pPCtrl++;
				}
			}
		} /* Block added RQ-AP01464049 */
		else
		{
			Response = LLDP_RSP_ERR_PARAM;
		} /* 2 Blocks removed  RQ-AP01464049 */
	}
	LLDP_UserRequestFinish(pHDB,pRQB,Response);
	LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_RequestStart()");
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    LLDP_RequestStop                            +*/
/*+  Input/Output          :    LLDP_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+                        :    LLDP_HDB_PTR_TYPE           pHDB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+  pHDB       : User HDB                                                  +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:      LLDP_OPC_STOP                  +*/
/*+     LSA_HANDLE_TYPE         handle:                                     +*/
/*+     LSA_USER_ID_TYPE        user_id:                                    +*/
/*+     LLDP_RSP                Response     Response                       +*/
/*+     LLDP_RQB_ERROR_TYPE     Error        Addition errorcodes from LowerL+*/
/*+     LLDP_RQB_ARGS_TYPE      args.Start                                  +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to stop TLV-Data sending.                         +*/
/*+               If a send is just underway (within edd) we wait for the   +*/
/*+               send to finish before confirming the request.             +*/
/*+               We send a shutdown TLV-frame for this Port                +*/
/*+               We have to wait till shutdown is send and no more send(s) +*/
/*+               underway for this port                                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_LOCAL_FCT_ATTR LLDP_RequestStop(
    LLDP_UPPER_RQB_PTR_TYPE   pRQB,
    LLDP_HDB_PTR_TYPE         pHDB)
{
    LLDP_RSP                     Response;
    LLDP_EDD_PCTRL_PTR_TYPE      pPCtrl;
    LSA_BOOL                     Finish;
    LSA_BOOL                     Ready;
    LSA_UINT32                   Start,End;
    LSA_UINT16                   PortID;
    LSA_UINT32                   i;

    LLDP_FUNCTION_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : LLDP_RequestStop(pRQB: 0x%X) Port: %d Mode: %d",pRQB,pRQB->args.Stop.PORTId,pRQB->args.Stop.Mode);

    Response    = LLDP_RSP_OK;
    Finish      = LSA_TRUE;
    PortID      = pRQB->args.Stop.PORTId;

    if((pRQB->args.Start.Mode == 0) ||(pRQB->args.Start.Mode > LLDP_RXTX_MODE))
    {   /* invalid Mode */
        Response = LLDP_RSP_ERR_PARAM;
        LLDP_UserRequestFinish(pHDB,pRQB,Response);
        LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_RequestStop() ERROR: invalid Mode");
        return;
    }
    if ((( PortID > 0 ) && ( PortID <= pHDB->PortCnt )) || ( PortID == LLDP_PORT_ID_ALL ))
    {
        if (PortID == LLDP_PORT_ID_ALL)  /* stop all ports */
        {
            LLDP_PROGRAM_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Stopping all ports");
            Start = 0;
            End   = pHDB->PortCnt-1;
        }
        else
        {
            LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Stopping port %d",PortID);
            Start = PortID-1;
            End   = PortID-1;
        }
        pPCtrl = &pHDB->pPCtrl[Start];
        /* check if we can Stop the/all requested ports */
        for ( i=Start; i<= End; i++)
        {
			if(pRQB->args.Stop.Mode & LLDP_RX_MODE ) /*Rx bit set */
			{
				pPCtrl->RxStoped = LSA_TRUE;
			}
			if(pRQB->args.Stop.Mode & LLDP_TX_MODE ) /*Tx bit set */
			{
				switch (pPCtrl->Tx.State)
				{
					case LLDP_TX_FRAME_STATE_STOP_SEND_SHUTDOWN:
					case LLDP_TX_FRAME_STATE_STOP_WAIT_SHUTDOWN:
					case LLDP_TX_FRAME_STATE_STOP_SHUTDOWN_DELAY:
						LLDP_UPPER_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_ERROR,"LLDP_RequestStop. Stop already in progress (Port:%d)",i);
						Response = LLDP_RSP_ERR_SEQUENCE;
						break;

					case LLDP_TX_FRAME_STATE_STOP:
					case LLDP_TX_FRAME_STATE_STOP_SET:
					case LLDP_TX_FRAME_STATE_START:
						/* ok. */
						break;

					default:
						LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_FATAL,"LLDP_RequestStart. Invalid State (0x%X)",pPCtrl->Tx.State);
						LLDP_FATAL(LLDP_FATAL_ERR_INCONSISTENZ);
						break;
				}
			}
            pPCtrl++;
        }
        /* if all ok stop all requested ports if Tx Mode is set */
        if (( Response == LLDP_RSP_OK ) &&(pRQB->args.Stop.Mode & LLDP_TX_MODE))
        {
            /* we temporary use the PortID as Counter for the number of */
            /* shutdowns underway. So we know when to finish the Stop   */
            /* request to user. The original PortID will be restored    */
            /* from StopPORTId later.                                   */
            pRQB->args.Stop.PORTId = 0;
            pPCtrl = &pHDB->pPCtrl[Start];
            /* Stop all ports */
            for ( i=Start; i<= End; i++)
            {
                if (pPCtrl->Tx.State == LLDP_TX_FRAME_STATE_START )
                {
                    pPCtrl->pStopReq         = pRQB;
                    pPCtrl->StopPORTId       = PortID;/* save original PortID */
                    /* Send Shutdown-Frame for Port if currently no send */
                    /* underway for this port. If something is underway  */
                    /* we send the Shutdown after the send is finished   */
                    if (!pPCtrl->Tx.Underway )
                    {
                        Ready = LSA_TRUE;
#ifdef LLDP_CFG_SEND_SHUTDOWN_ON_STOP
                        if ( pPCtrl->LinkStatus.Status == EDD_LINK_UP )
                        {
                            LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Sending shutdown for Port (Port: %d)",pPCtrl->ThisIdx);
                            Response = LLDP_EDDSendLLDPFrame(pHDB,pPCtrl,LSA_TRUE);
                            if ( Response == LLDP_RSP_OK )
                            {
                                pPCtrl->Tx.State = LLDP_TX_FRAME_STATE_STOP_WAIT_SHUTDOWN;
                                pRQB->args.Stop.PORTId++; /* number of stop underway */
                                Finish = LSA_FALSE;
                                Ready  = LSA_FALSE;
                            }
                            else
                            {   /* if sending shutdown fails we continue without */
                                LLDP_PROGRAM_TRACE_02(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"Sending shutdown failed (Port: %d, Resp: 0x%X)",pPCtrl->ThisIdx,Response);
                                pPCtrl->Stats.TxShutdownFailed++;
                            }
                        }
#endif
                        if (Ready)
                        {   /* if sending shutdown frame fails or link is down, we  */
                            /* continue without shutdown frame for this port.       */
                            /* handling for this port is ready */
                            pPCtrl->Tx.State = LLDP_TX_FRAME_STATE_STOP_SET;
                            pHDB->InStartCnt--;
                            Response = LLDP_RSP_OK;
                        }
                    }
                    else
                    {
                        LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE_LOW,"Tx-underway. Delay Shutdown (Port: %d)",pPCtrl->ThisIdx);
                        /* send the shutdown after current send finished */
                        pPCtrl->Tx.State = LLDP_TX_FRAME_STATE_STOP_SEND_SHUTDOWN;
                        pRQB->args.Stop.PORTId++; /* number of stop underway */
                        Finish = LSA_FALSE;
                    }
                }
                pPCtrl++;
            }
        }
    }
    else
    {
        Response = LLDP_RSP_ERR_PARAM;
    }
    if (Finish)
    {
        pRQB->args.Stop.PORTId = PortID;  /* restore original PORTId */
        LLDP_UserRequestFinish(pHDB,pRQB,Response);
    }
    LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: LLDP_RequestStop()");
}

/*===========================================================================*/
/*                            main-functions                                 */
/*===========================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    lldp_open_channel                           +*/
/*+  Input/Output          :    LLDP_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:       LLDP_OPC_OPEN_CHANNEL         +*/
/*+     LSA_HANDLE_TYPE         handle:       -                             +*/
/*+     LSA_USER_ID_TYPE        user_id:      -                             +*/
/*+     LLDP_RSP                Response      -                             +*/
/*+     LLDP_RQB_ERROR_TYPE     Error         -                             +*/
/*+     LLDP_RQB_ARGS_TYPE      Args.Channel: LLDP_RQB_OPEN_CHANNEL_TYPE    +*/
/*+                                                                         +*/
/*+     Args.Channel:                                                       +*/
/*+                                                                         +*/
/*+     LSA_HANDLE_TYPE         nandle:       channel-handle of LLDP (ret)  +*/
/*+     LSA_HANDLE_TYPE         handle_upper  channel-handle of user        +*/
/*+     LSA_SYS_PATH_TYPE       sys_path      system-path of channel        +*/
/*+     LSA_VOID                LSA_FCT_PTR(LLDP_UPPER_OUT_FCT_ATTR,        +*/
/*+                                         LLDP_request_upper_done_ptr)    +*/
/*+                                         (LLDP_UPPER_RQB_PTR_TYPE pRQB)  +*/
/*+                                             callback-function           +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:     returned channel of user. This  +*/
/*+                                         is HandleUpper from Args.Channel+*/
/*+     LLDP_RSP                Response    LLDP_RSP_OK                     +*/
/*+                                         LLDP_RSP_ERR_RESOURCE           +*/
/*+                                         LLDP_RSP_ERR_SYS_PATH           +*/
/*+                                         LLDP_RSP_ERR_PARAM              +*/
/*+                                         ...                             +*/
/*+     LLDP_RQB_ERROR_TYPE     Error       additional errorcodes from LL   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Request to open a channel                                 +*/
/*+                                                                         +*/
/*+               Within this request the output - macro LLDP_GET_PATH_INFO +*/
/*+               is used to get further channel-descriptons and parameters +*/
/*+                                                                         +*/
/*+               After a successful open a channel-handle is returned      +*/
/*+               in Args.Channel.Handle for use with further requests.     +*/
/*+                                                                         +*/
/*+               HandleUpper:                                              +*/
/*+                                                                         +*/
/*+               This handle will be stored within LLDP and return as      +*/
/*+               handle on request-confirmation (in RQB-header).           +*/
/*+                                                                         +*/
/*+               cbf:                                                      +*/
/*+                                                                         +*/
/*+               The request is always finished by calling the spezified   +*/
/*+               callback-function. This callback-funktion will also be    +*/
/*+               used for all other other future requests for this channel.+*/
/*+                                                                         +*/
/*+               SysPath:                                                  +*/
/*+                                                                         +*/
/*+               The SysPath variable will not be used inside but given to +*/
/*+               systemadaption via LLDP_GET_PATH_INFO.                    +*/
/*+                                                                         +*/
/*+               This function is most likely done asynchroneous because   +*/
/*+               of calling lower layer open channel (EDD)                 +*/
/*+               depending on channel type spezified in CDP (detail-ptr)   +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro LLDP_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error an are of the type              +*/
/*+                                                                         +*/
/*+               LLDP_RSP_ERR_OPCODE                                       +*/
/*+               LLDP_RSP_ERR_PARAM                                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID LLDP_UPPER_IN_FCT_ATTR  lldp_open_channel(LLDP_UPPER_RQB_PTR_TYPE pRQB)
{
    LLDP_RSP                  Status;
    LSA_BOOL                  Finish;
    LSA_SYS_PTR_TYPE          pSys;
    LLDP_DETAIL_PTR_TYPE      pDetail;
    LSA_UINT16                PathStat;
    LLDP_HDB_PTR_TYPE         pHDB;
    LSA_BOOL                  PathOpen;

    LLDP_ENTER();
    LLDP_FUNCTION_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"IN : lldp_open_channel(pRQB: 0x%X)",pRQB);

    Status   = LLDP_RSP_OK;
    Finish   = LSA_FALSE;
    PathOpen = LSA_FALSE;
    {
        LSA_SYS_PTR_TYPE tmp = {0};
        pSys  = tmp; /* pSys may not be a pointer ! */
    }
    pDetail  = LSA_NULL;
    LLDP_ASSERT_NULL_PTR(pRQB);
    /* AP00999034  if done function is NULL we throw a fatal */
    if(LSA_HOST_PTR_ARE_EQUAL(pRQB->args.channel.lldp_request_upper_done_ptr, LSA_NULL))
    {
        LLDP_UPPER_TRACE_00(0,LSA_TRACE_LEVEL_FATAL,"lldp_open_channel(): Cbf is NULL");
        LLDP_FATAL(LLDP_FATAL_ERR_NULL_PTR);
    }
    /*----------------------------------------------------------------------------*/
    /* setup some return parameters within RQB.                                   */
    /*----------------------------------------------------------------------------*/
    LLDP_RQB_SET_HANDLE(pRQB,pRQB->args.channel.handle_upper); /* set upper handle */
    /*----------------------------------------------------------------------------*/
    /* Checks some parameters                                                     */
    /*----------------------------------------------------------------------------*/
    if ( LLDP_RQB_GET_OPCODE(pRQB) != LLDP_OPC_OPEN_CHANNEL )
    {
        LLDP_RQB_SET_RESPONSE(pRQB, LLDP_RSP_ERR_OPCODE);
        Status = LLDP_RSP_ERR_OPCODE;
        LLDP_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_UNEXP,">>>: LLDP_open_channel(): Invalid RQB-Opcode (0x%X)",LLDP_RQB_GET_OPCODE(pRQB));
        LLDP_RQB_ERROR(pRQB);
    }
    else
    {
        LLDP_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,">>>: Request: LLDP_OPC_OPEN_CHANNEL (Handle: 0x%X)",LLDP_RQB_GET_HANDLE(pRQB));
    }
    /*----------------------------------------------------------------------------*/
    /* if all ok handle request                                                   */
    /*----------------------------------------------------------------------------*/
    if (Status == LLDP_RSP_OK)
    {
        Finish = LSA_TRUE;
        /*------------------------------------------------------------------------*/
        /* Call LSA Output-macro..                                                */
        /*------------------------------------------------------------------------*/
        LLDP_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"-->: Calling LLDP_GET_PATH_INFO.");
        LLDP_GET_PATH_INFO(&PathStat, &pSys, &pDetail, pRQB->args.channel.sys_path);
        if ( PathStat != LSA_RET_OK)
        {
            LLDP_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_FATAL,"<--: LLDP_GET_PATH_INFO failed (Rsp: 0x%X).",PathStat);
            /* AP00999034: we  need the sys_ptr! Status = LLDP_RSP_ERR_SYS_PATH; */
            LLDP_FATAL(LLDP_RSP_ERR_SYS_PATH);
        }
        else
        {
            LLDP_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<--: LLDP_GET_PATH_INFO success (Detail-Ptr: 0x%X).",pDetail);
            if ((LSA_HOST_PTR_ARE_EQUAL(pDetail, LSA_NULL)) )
            {
                Status = LLDP_RSP_ERR_PARAM;
            }
            else
            {
                PathOpen = LSA_TRUE;
            }
        }
        /*------------------------------------------------------------------------*/
        /* Get Infos from Detail-Ptr (CDB) and start handling..                   */
        /*------------------------------------------------------------------------*/
        if ( Status == LLDP_RSP_OK )
        {
            LLDP_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_LOW,"Opening USER-Channel");
            /* Get a free, initialized HDB */
            Status = LLDP_UserGetFreeHDB(&pHDB);
            if ( Status == LLDP_RSP_OK )
            {
                LLDP_ASSERT_NULL_PTR(pHDB);
                pHDB->Params.pSys               = pSys;
                pHDB->Params.pDetail            = pDetail;
                pHDB->Params.Cbf                = pRQB->args.channel.lldp_request_upper_done_ptr;
                pHDB->Params.SysPath            = pRQB->args.channel.sys_path;
                pHDB->Params.HandleUpper        = pRQB->args.channel.handle_upper;
                /* AP01512673 changed from config; removed PortMaxSender from detail ptr  AND 656969 -> lldp_usr.h    */
                pHDB->PortMaxSender             = LLDP_MAX_PORT_SENDER; /* pDetail->PORTMaxSender; */
                pHDB->PortCnt                   = 0;                    /* currently none present  */
                pHDB->LLDP_ChannelTxDelay       = LLDP_DEFAULT_TX_DELAY;
                pHDB->LLDP_ChannelReInitDelay   = LLDP_DEFAULT_TX_REINIT_DELAY;
                pHDB->LLDP_ChannelMsgTxInterval = LLDP_DEFAULT_MSG_DELAY;
                pHDB->ReleaseSysPtr             = LSA_FALSE;
                /* set our handle in RQB */
                pRQB->args.channel.handle       = pHDB->ThisHandle;
                /* Open the EDD-Channel */
                Status = LLDP_EDDOpenChannel(pHDB);
                if ( Status == LLDP_RSP_OK )
                {   /* Request is underway now.. */
                    pHDB->State  = LLDP_HANDLE_STATE_OPEN_EDD_RUNNING;
                    pHDB->pOCReq = pRQB; /* save pointer to request for later use */
                    Finish       = LSA_FALSE;
                }
                else
                {
                    LLDP_PROGRAM_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"Open LLDP-EDD channel failed. (Rsp: %Xh)",Status);
                    LLDP_UserReleaseHDB(pHDB);
                }
            }
            else
            {   /* Error on getting handle. Abort. */
                LLDP_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"Open LLDP USER-channel failed. Cannot get USER handle! (Rsp: 0x%X).",Status);
            }
        }
    }
    /*----------------------------------------------------------------------------*/
    /* Should we finish the request ?                                             */
    /*----------------------------------------------------------------------------*/
    if ( Finish )  /* finish the request ? */
    {
        /* upper - handler already set */
        LLDP_RQBSetResponse(pRQB,Status);
        LLDP_UPPER_TRACE_03(0,LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",LLDP_RQB_GET_OPCODE(pRQB),LLDP_RQB_GET_HANDLE(pRQB),Status);
        /* AP00999034: LLDP_CallCbf(pRQB->args.channel.lldp_request_upper_done_ptr,pRQB); */
        LLDP_REQUEST_UPPER_DONE(pRQB->args.channel.lldp_request_upper_done_ptr,pRQB,pSys);
    }
    /*----------------------------------------------------------------------------*/
    /* On error, if Syspath was opened we close the path                          */
    /*----------------------------------------------------------------------------*/
    if ( ( Status != LLDP_RSP_OK ) &&  ( PathOpen)  )
    {
        LLDP_SYSTEM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"-->: Calling LLDP_RELEASE_PATH_INFO.");
        LLDP_RELEASE_PATH_INFO(&PathStat,pSys,pDetail);
        LLDP_SYSTEM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE,"<--: LLDP_RELEASE_PATH_INFO (Rsp: 0x%X).",PathStat);
        if ( PathStat != LSA_RET_OK)
        {
            LLDP_FATAL(LLDP_FATAL_ERR_RELEASE_PATH_INFO);
        }
    }
    LLDP_FUNCTION_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"OUT: lldp_open_channel()");
    LLDP_EXIT();
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    lldp_close_channel                          +*/
/*+  Input/Output          :    LLDP_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:       LLDP_OPC_CLOSE_CHANNEL        +*/
/*+     LSA_HANDLE_TYPE         handle:       -                             +*/
/*+     LSA_USER_ID_TYPE        user_id:      -                             +*/
/*+     LLDP_RSP                Response      -                             +*/
/*+     LLDP_RQB_ERROR_TYPE     Error         -                             +*/
/*+     LLDP_RQB_ARGS_TYPE      Args          -                             +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:     returned channel of user.       +*/
/*+     LLDP_RSP                Response    LLDP_RSP_OK                     +*/
/*+                                         LLDP_RSP_ERR_RESOURCE           +*/
/*+                                         LLDP_RSP_ERR_PARAM              +*/
/*+                                         ...                             +*/
/*+     LLDP_RQB_ERROR_TYPE     Error       additional errorcodes from LL   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes a channel.                                         +*/
/*+               Close lower-layer channel if neccessary. (depends on      +*/
/*+               channel type). Releases Path-info. Note that this         +*/
/*+               function is done asynchron because of calling lower layer +*/
/*+               functions with callback. On error we may finish synchron. +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro LLDP_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error an are of the type              +*/
/*+                                                                         +*/
/*+               LLDP_RSP_ERR_OPCODE                                       +*/
/*+               LLDP_RSP_ERR_PARAM                                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_UPPER_IN_FCT_ATTR  lldp_close_channel(LLDP_UPPER_RQB_PTR_TYPE pRQB)
{
    LLDP_RSP                  Status;
    LSA_HANDLE_TYPE           Handle;
    LSA_BOOL                  Finish;
    LLDP_HDB_PTR_TYPE         pHDB;
    LSA_VOID                  LSA_FCT_PTR(LLDP_UPPER_OUT_FCT_ATTR, Cbf) (LLDP_UPPER_RQB_PTR_TYPE pRQB);

    LLDP_ENTER();

    LLDP_FUNCTION_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"IN : lldp_close_channel(pRQB: 0x%X)",pRQB);
    LLDP_ASSERT_NULL_PTR(pRQB);
    Status      = LLDP_RSP_OK;
    Handle      = LLDP_RQB_GET_HANDLE(pRQB);
    Cbf         = LSA_NULL;
    /*---------------------------------------------------------------------------*/
    /* setup some return parameters within RQB.                                  */
    /*---------------------------------------------------------------------------*/
    pHDB = LLDP_GetUserHDBFromHandle(Handle);
    LLDP_UPPER_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: Request: LLDP_OPC_CLOSE_CHANNEL (Handle: 0x%X)",Handle);
    if (! LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
    {
        Finish = LSA_TRUE;
        Cbf    = pHDB->Params.Cbf;
        LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : lldp_close_channel(pRQB: 0x%X)",pRQB);

        /* set upper handle in RQB */
        LLDP_RQB_SET_HANDLE(pRQB, pHDB->Params.HandleUpper );
        if ( LLDP_RQB_GET_OPCODE(pRQB) == LLDP_OPC_CLOSE_CHANNEL )
        {
            /* only if no open/close request in progress */
            if (! LSA_HOST_PTR_ARE_EQUAL(pHDB->pOCReq, LSA_NULL))
            {
                LLDP_UPPER_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_ERROR,"lldp_close_channel(): Close channel failed. Open or Close already running");
                Status = LLDP_RSP_ERR_SEQUENCE;
            }
            else
            {
                Status = LLDP_UserCloseChannel(pHDB,&Finish);
                if (!Finish )
                {
                    pHDB->pOCReq = pRQB; /* save RQB for later use */
                }
            }
        }
        else
        {
            LLDP_UPPER_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_UNEXP,"lldp_close_channel(): Invalid RQB-Opcode (0x%X)",LLDP_RQB_GET_OPCODE(pRQB));
            Status = LLDP_RSP_ERR_PARAM;
        }
        /*-----------------------------------------------------------------------*/
        /* Finish the request if needed                                          */
        /*-----------------------------------------------------------------------*/
        if ( Finish )
        {
            LLDP_RQBSetResponse(pRQB,Status);
            LLDP_UPPER_TRACE_03(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,"<<<: Open/Close-Request finished. Opcode: 0x%X, UpperHandle: 0x%X, Response: 0x%X",LLDP_RQB_GET_OPCODE(pRQB),LLDP_RQB_GET_HANDLE(pRQB),Status);
            LLDP_CallCbf(pHDB,Cbf,pRQB);
        }
    }
    else
    {
        LLDP_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,"lldp_close_channel(): Invalid LLDP-User Handle! (0x%X)",Handle);
        LLDP_RQB_SET_RESPONSE(pRQB, LLDP_RSP_ERR_PARAM);
        LLDP_RQB_ERROR(pRQB);
    }
    LLDP_FUNCTION_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"OUT: lldp_close_channel()");
    LLDP_EXIT();
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    lldp_request                                +*/
/*+  Input/Output          :    LLDP_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_OPCODE_TYPE         opcode:       varies:                       +*/
/*+                                           LLDP_OPC_xxx                  +*/
/*+                                           LLDP_OPC_TIMER                +*/
/*+     LSA_HANDLE_TYPE         handle:       -                             +*/
/*+     LSA_USER_ID_TYPE        user_id:      -                             +*/
/*+     LLDP_RSP                Response      -                             +*/
/*+     LLDP_RQB_ERROR_TYPE     Error         -                             +*/
/*+     LLDP_RQB_ARGS_TYPE      Args          varies depending on opcode    +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function:                               +*/
/*+     RQB-header:                                                         +*/
/*+     LSA_HANDLE_TYPE         handle:     upper-handle from open_channel  +*/
/*+     LLDP_RSP                Response    LLDP_RSP_OK                     +*/
/*+                                         LLDP_RSP_ERR_RESOURCE           +*/
/*+                                         LLDP_RSP_ERR_PARAM              +*/
/*+                                         ...                             +*/
/*+     LLDP_RQB_ERROR_TYPE     Error       additional errorcodes from LL   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Do a channel request.                                     +*/
/*+                                                                         +*/
/*+               For this requests a valid channel handle is needed. The   +*/
/*+               handle will be given on open_channel-request.             +*/
/*+                                                                         +*/
/*+  Note on error-handling:                                                +*/
/*+                                                                         +*/
/*+               In some cases it is not possible to call the call-back-   +*/
/*+               function of the requestor to confirm the request. in this +*/
/*+               case we call the output-macro LLDP_RQB_ERROR with the RQB +*/
/*+               to notify this error. This erros are most likely caused   +*/
/*+               by a implementation error and are of the type             +*/
/*+                                                                         +*/
/*+               LLDP_RSP_ERR_OPCODE                                       +*/
/*+               LLDP_RSP_ERR_PARAM                                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID  LLDP_UPPER_IN_FCT_ATTR  lldp_request(LLDP_UPPER_RQB_PTR_TYPE pRQB)
{

    LSA_HANDLE_TYPE           Handle;
    LLDP_HDB_PTR_TYPE         pHDB = LSA_NULL;

    LLDP_ENTER();
    LLDP_FUNCTION_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,"IN : lldp_request(pRQB: 0x%X)",pRQB);
    LLDP_ASSERT_NULL_PTR(pRQB);
    Handle   = LLDP_RQB_GET_HANDLE(pRQB);
    /* TIMER-Request needs special handling  */
    if (  LLDP_RQB_GET_OPCODE(pRQB) == LLDP_OPC_TIMER )
    {
        LLDP_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_CHAT,">>>: Request: LLDP_OPC_TIMER (Handle: 0x%X)",LLDP_RQB_GET_HANDLE(pRQB));
        LLDP_RequestTimer(pRQB);
    }
    else
    {
        pHDB = LLDP_GetUserHDBFromHandle(Handle);
        if (! LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL) )
        {
            LLDP_FUNCTION_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"IN : lldp_request(pRQB: 0x%X)",pRQB);
            /* no open/close runnning ? and channel open ?                    */
            /* Note: If a close-channel failed within some closing state. The */
            /*       channel can not be used anymore for this user - requests */
            /*       The channel is in some inoperable state and it can only  */
            /*       be tried to do another close-request..                   */
            if ( (pHDB->State == LLDP_HANDLE_STATE_OPEN) &&
                 LSA_HOST_PTR_ARE_EQUAL(pHDB->pOCReq, LSA_NULL) )
            {
                switch ( LLDP_RQB_GET_OPCODE(pRQB))
                {

                    case LLDP_OPC_GET_CONFIG:
                        LLDP_UPPER_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: Request: LLDP_OPC_GET_CONFIG (Handle: 0x%X)",LLDP_RQB_GET_HANDLE(pRQB));
                        LLDP_RequestGetConfig(pRQB,pHDB);
                        break;
                    case LLDP_OPC_SET:
                        LLDP_UPPER_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: Request: LLDP_OPC_SET (Handle: 0x%X)",LLDP_RQB_GET_HANDLE(pRQB));
                        LLDP_RequestSet(pRQB,pHDB);
                        break;
                    case LLDP_OPC_START:
                        LLDP_UPPER_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: Request: LLDP_OPC_START (Handle: 0x%X)",LLDP_RQB_GET_HANDLE(pRQB));
                        LLDP_RequestStart(pRQB,pHDB);
                        break;
                    case LLDP_OPC_STOP:
                        LLDP_UPPER_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: Request: LLDP_OPC_STOP (Handle: 0x%X)",LLDP_RQB_GET_HANDLE(pRQB));
                        LLDP_RequestStop(pRQB,pHDB);
                        break;
                    case LLDP_OPC_INDICATION_PROVIDE:
                        LLDP_UPPER_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: Request: LLDP_OPC_INDICATION_PROVIDE (Handle: 0x%X)",LLDP_RQB_GET_HANDLE(pRQB));
                        LLDP_RequestIndicationProvide(pRQB,pHDB);
                        break;
                    case LLDP_OPC_GET:
                        LLDP_UPPER_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: Request: LLDP_OPC_GET (Handle: 0x%X)",LLDP_RQB_GET_HANDLE(pRQB));
                        LLDP_RequestGet(pRQB,pHDB);
                        break;
                    case LLDP_OPC_STATS:
                        LLDP_UPPER_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: Request: LLDP_OPC_STATS (Handle: 0x%X)",LLDP_RQB_GET_HANDLE(pRQB));
                        LLDP_RequestStats(pRQB,pHDB);
                        break;
                    case LLDP_OPC_SET_GLOBALS:
                        LLDP_UPPER_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_NOTE,">>>: Request: LLDP_OPC_STATS (Handle: 0x%X)",LLDP_RQB_GET_HANDLE(pRQB));
                        LLDP_RequestSetGlobals(pRQB,pHDB);
                        break;
                    default:
                        LLDP_UPPER_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_ERROR,">>>: lldp_request(): Invalid RQB-Opcode (0x%X)",LLDP_RQB_GET_OPCODE(pRQB));
                        LLDP_UserRequestFinish(pHDB,pRQB,LLDP_RSP_ERR_OPCODE);
                        break;

                } /* switch */
            }
            else
            {
                LLDP_UPPER_TRACE_01(pHDB->TraceIdx,LSA_TRACE_LEVEL_ERROR,">>>: lldp_request(): Channel not open or Open/Close running (0x%X)",LLDP_RQB_GET_OPCODE(pRQB));
                LLDP_UserRequestFinish(pHDB,pRQB,LLDP_RSP_ERR_SEQUENCE);
            }
        }
        else
        {
            LLDP_RQB_SET_RESPONSE(pRQB, LLDP_RSP_ERR_PARAM);
            LLDP_UPPER_TRACE_01(0,LSA_TRACE_LEVEL_ERROR,">>>: lldp_request(): Invalid handle (0x%X)",Handle);
            LLDP_RQB_ERROR(pRQB);
        }
    }
    if(pHDB == LSA_NULL)
    {
        LLDP_FUNCTION_TRACE_00(0,LSA_TRACE_LEVEL_CHAT,"OUT: lld_request()");
    }
    else
    {
        LLDP_FUNCTION_TRACE_00(pHDB->TraceIdx,LSA_TRACE_LEVEL_CHAT,"OUT: lld_request()");

    }
    LLDP_EXIT();

}

/*****************************************************************************/
/*  end of file LLDP_USR.C                                                    */
/*****************************************************************************/

