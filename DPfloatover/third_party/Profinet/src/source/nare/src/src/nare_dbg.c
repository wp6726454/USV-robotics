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
/*  C o m p o n e n t     &C: NARE (NAmeREsolver)                       :C&  */
/*                                                                           */
/*  F i l e               &F: nare_dbg.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  NARE-Debugging support functions                 */
/*                          (system dependend)                               */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/

#define LTRC_ACT_MODUL_ID  4
#define NARE_MODULE_ID     LTRC_ACT_MODUL_ID /* NARE_MODULE_ID_DBG */

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/

#include "nare_inc.h"            /* nare headerfiles */
#include "nare_int.h"            /* internal header  */

NARE_FILE_SYSTEM_EXTENSION(NARE_MODULE_ID)

#ifdef NARE_CFG_DEBUG_ENABLE

/*****************************************************************************/
/*                                                                           */
/* Extern variables                                                          */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* Extern functions                                                          */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*                                                                           */
/* Globlal variables                                                         */
/*                                                                           */
/*****************************************************************************/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREDbgWriteString                          +*/
/*+  Input                 :    LSA_INT8 NARE_LOCAL_MEM_ATTR *pString       +*/
/*+  Output                     ----                                        +*/
/*+  Result                :    ----                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pString    : 0 terminated String to write to Debugport                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Writes String to Debugport                                +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NAREDbgWriteString(
    LSA_INT8 NARE_LOCAL_MEM_ATTR * pString)
{

    while (*pString) NARE_DBG_WRITE_BYTE(*pString++);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREDbgOutNumHex                            +*/
/*+  Input                 :    LSA_UINT32          n                       +*/
/*+  Input                 :    LSA_UINT32          digits                  +*/
/*+                                                                         +*/
/*+  Result                :    ----                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  n          : number to output as hex                                   +*/
/*+  digits     : minimum number of digits to print                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Writes hex-    number to debugport                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID NARE_LOCAL_FCT_ATTR NAREDbgOutNumHex(
    LSA_UINT32  n,
    LSA_UINT32  digits)
{
    if (digits)
    {
        digits--;
    }

    if ((n & ~0xf) || digits)
    {
        NAREDbgOutNumHex(n >> 4, digits);
        n &= 0xf;
    }

    if (n < 10)
    {
        NARE_DBG_WRITE_BYTE((LSA_UINT8)(n + '0'));
    }
    else
    {
        NARE_DBG_WRITE_BYTE((LSA_UINT8)(n - 10 + 'A'));
    }
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREDbgOutNumDec                            +*/
/*+  Input                 :    LSA_UINT32          n                       +*/
/*+  Output                     ----                                        +*/
/*+  Result                :    ----                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  n          : decimal number to output                                  +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Writes decimal number to debugport                        +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static LSA_VOID NARE_LOCAL_FCT_ATTR NAREDbgOutNumDec(
    LSA_UINT32 n)
{
    if (n >= 10)
    {
        NAREDbgOutNumDec(n / 10);
        n %= 10;
    }
    NARE_DBG_WRITE_BYTE((LSA_UINT8)(n + '0'));
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREDbgPrint                                +*/
/*+  Input                 :    LSA_INT8 NARE_LOCAL_MEM_ATTR * pString      +*/
/*+                             LSA_UINT32                P1                +*/
/*+                             LSA_UINT32                P2                +*/
/*+                             LSA_UINT32                P3                +*/
/*+  Result                :    ----                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pString    : 0 terminated String to write to Debugport                 +*/
/*+  P1/P2/p3   : Parameter                                                 +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Writes String to Debugport with optional max. 2 paras     +*/
/*+                                                                         +*/
/*+               Format-Strings:                                           +*/
/*+                                                                         +*/
/*+               %u  : unsigned                                            +*/
/*+               %d  : int                                                 +*/
/*+               %c  : char                                                +*/
/*+               %x  : hex                                                 +*/
/*+               %B  : 2-digit hex                                         +*/
/*+               %H  : 4-digit hex                                         +*/
/*+               %X    8-digig hex                                         +*/
/*+                                                                         +*/
/*+               Example:  NAREDbgPrint("Value 1: %d, \n",val1,0);         +*/
/*+               Example:  NAREDbgPrint("Val1 %H, Val2: %X",0x17,0x23)     +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NAREDbgPrint(
    LSA_INT8    NARE_LOCAL_MEM_ATTR * pString,
    LSA_UINT32  p1,
    LSA_UINT32  p2,
    LSA_UINT32  p3)
{
    LSA_UINT8        c;
    LSA_UINT32       Para[3];
    LSA_UINT32       Idx;


    Para[0] = p1;
    Para[1] = p2;
    Para[2] = p3;
    Idx     = 0;

    while (*pString)
    {
        c = *pString++;
        switch (c)
        {
            case (LSA_UINT8)'%':
                c = *pString++;
                if ( Idx > 2 ) Idx = 2; /* if to much we use the last */
                switch (c)
                {
                    case 'x':
                        NAREDbgOutNumHex(Para[Idx++], 0);
                        break;
                    case 'B':
                        NAREDbgOutNumHex(Para[Idx++], 2);
                        break;
                    case 'H':
                        NAREDbgOutNumHex(Para[Idx++], 4);
                        break;
                    case 'X':
                        NAREDbgOutNumHex(Para[Idx++], 8);
                        break;
                    case 'd':
                    {
                        LSA_INT32  l;

                        l = Para[Idx++];
                        if (l < 0)
                        {
                            NARE_DBG_WRITE_BYTE('-');
                            l = - l;
                        }
                        NAREDbgOutNumDec((unsigned long)l);
                    }
                    break;
                    case 'u':
                        NAREDbgOutNumDec(Para[Idx++]);
                        break;
                    case '%':
                        NARE_DBG_WRITE_BYTE('%');
                        break;
                    case 'c':
                        NARE_DBG_WRITE_BYTE((LSA_UINT8) Para[Idx++]);
                        break;

                    default:
                        NARE_DBG_WRITE_BYTE(' ');
                        break;
                }
                break;
            case '\n':
                NARE_DBG_WRITE_BYTE('\r');
                /* fall through */
            default:
                NARE_DBG_WRITE_BYTE(c);
        }
    }

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREDbgDumpFrame                            +*/
/*+  Input                 :    LSA_INT8 NARE_LOCAL_MEM_ATTR * pString      +*/
/*+                             LSA_UINT8 NARE_EDD_LOWER_MEM_ATTR * pFrame  +*/
/*+                             LSA_UINT32                     Len          +*/
/*+  Result                :    ----                                        +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Dump Frame to screen. hex/char output.                    +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NAREDbgDumpFrame(
    LSA_INT8  NARE_LOCAL_MEM_ATTR     * pString,
    LSA_UINT8 NARE_EDD_LOWER_MEM_ATTR * pFrame,
    LSA_UINT32                          Len)
{

    LSA_UINT32  i,j;
    LSA_UINT8   Chars[17];

    i=0;
    while (i<Len)
    {
        NAREDbgWriteString(pString);
        NAREDbgPrint("[%H]: ",i,0,0);
        for (j=0; (j<16) && (i<Len) ; j++)
        {
            NAREDbgPrint("%B ",pFrame[i],0,0);
            Chars[j] = pFrame[i];
            if ( Chars[j] < 0x20) Chars[j] = '.';
            i++;
        }
        Chars[j] = 0;
        while (j++<16) NAREDbgWriteString("   ");
        /*  NAREDbgWriteString(" "); */
        NAREDbgWriteString((LSA_INT8 NARE_LOCAL_MEM_ATTR *)Chars);
        NAREDbgWriteString("\r\n");
    }

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREDbgPrintHead                            +*/
/*+  Input                 :    LSA_UINT8                 HeaderID          +*/
/*+                        :    LSA_INT8 NARE_LOCAL_MEM_ATTR * pString      +*/
/*+                             LSA_UINT32                P1                +*/
/*+                             LSA_UINT32                P2                +*/
/*+                             LSA_UINT32                P3                +*/
/*+  Result                :    ----                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  HeaderID   : Defines Modul printing. A header is set at begining,      +*/
/*+               depending on HeaderID                                     +*/
/*+  pString    : 0 terminated String to write to Debugport                 +*/
/*+  P1/P2/P2   : Parameter                                                 +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Writes String to Debugport with optional max. 2 paras     +*/
/*+                                                                         +*/
/*+               see NAREDbgPrint()                                        +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NAREDbgPrintHead(
    LSA_UINT8   HeaderID,
    LSA_INT8 NARE_LOCAL_MEM_ATTR * pString,
    LSA_UINT32  p1,
    LSA_UINT32  p2,
    LSA_UINT32  p3
)
{

    switch ( HeaderID )
    {
        case NARE_DBG_HID_NARE  :
            NAREDbgWriteString("* NARE: ");
            break;

        case NARE_DBG_HID_NARE_IDENT :
            NAREDbgWriteString("       ");
            break;

        case NARE_DBG_HID_NARE_ERROR:
            NAREDbgWriteString("* NARE: [*** Error]: ");
            break;

        case NARE_DBG_HID_NARE_FCT_IN:
            NAREDbgWriteString("* NARE: [IN ]: ");
            break;

        case NARE_DBG_HID_NARE_FCT_OUT:
            NAREDbgWriteString("* NARE: [OUT]: ");
            break;

        case NARE_DBG_HID_NONE :
        default:
            break;
    }

    NAREDbgPrint(pString,p1,p2,p3);
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREDbgPrintRQBHeader                       +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+  Result                :    ----                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to RQB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Prints RQB-Header                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NAREDbgPrintRQBHead(NARE_UPPER_RQB_PTR_TYPE     pRQB)
{

    NAREDbgPrintHead(NARE_DBG_HID_NARE,"-RQB-Header-[%X]-----------------------------------------------\n\r",(LSA_UINT32) pRQB,0,0);
    NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT," pRQB->pNext   : %Xh\r\n",(LSA_UINT32) NARE_RQB_GET_NEXT_RQB_PTR(pRQB),0,0);
    NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT," pRQB->pPrev   : %Xh\r\n",(LSA_UINT32) NARE_RQB_GET_PREV_RQB_PTR(pRQB),0,0);
    NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT," pRQB->Opcode  : %Xh [",(LSA_UINT32) NARE_RQB_GET_OPCODE(pRQB),0,0);

    switch ( NARE_RQB_GET_OPCODE(pRQB))
    {
        case  NARE_OPC_OPEN_CHANNEL:
            NAREDbgWriteString("NARE_OPC_OPEN_CHANNEL]\r\n");
            break;
        case  NARE_OPC_CLOSE_CHANNEL:
            NAREDbgWriteString("NARE_OPC_CLOSE_CHANNEL]\r\n");
            break;
        case  NARE_OPC_RESOLVE_CONFIG:
            NAREDbgWriteString("NARE_OPC_RESOLVE_CONFIG]\r\n");
            break;
        case  NARE_OPC_RESOLVE_CONFIG_CANCEL:
            NAREDbgWriteString("NARE_OPC_RESOLVE_CONFIG_CANCEL]\r\n");
            break;
        case  NARE_OPC_TIMER:
            NAREDbgWriteString("NARE_OPC_TIMER]\r\n");
            break;
        default:
            NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT,"unknown opcode]\r\n",0,0,0);
            break;
    }


    NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT," pRQB->Handle  : %Xh\r\n",(LSA_UINT32) NARE_RQB_GET_HANDLE(pRQB),0,0);
    NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT," pRQB->Response: %Xh\r\n",(LSA_UINT32) NARE_RQB_GET_RESPONSE(pRQB),0,0);

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREDbgPrintRQBParam                        +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+  Result                :    ----                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to RQB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Prints RQB-Params                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LSA_VOID NARE_LOCAL_FCT_ATTR NAREDbgPrintRQBParam(NARE_UPPER_RQB_PTR_TYPE     pRQB)
{

    LSA_UINT32 i;


    switch ( NARE_RQB_GET_OPCODE(pRQB))
    {
        case  NARE_OPC_OPEN_CHANNEL:
            NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT," pRQB->args.channel:\r\n",0,0,0);
            NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT,"  HandleUpper          : %X\r\n",(LSA_UINT32)pRQB->args.channel.handle_upper,0,0);
            NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT,"  Cbf                  : %X\r\n",(LSA_UINT32)pRQB->args.channel.nare_request_upper_done_ptr,0,0);

            break;
        case  NARE_OPC_CLOSE_CHANNEL:
            break;
        case  NARE_OPC_RESOLVE_CONFIG_CANCEL:
            NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT," pRQB->args.ResConfCancel:\r\n",0,0,0);
            NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT,"  RequestID            : %d\r\n",pRQB->args.ResConfCancel.RequestID,0,0);
            break;
        case  NARE_OPC_TEST_IP:
            NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT," pRQB->args.TestIP:\r\n",0,0,0);
            NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT,"  IPAddr               : %d.%d.%d",
                             pRQB->args.TestIP.IPAddr[0],
                             pRQB->args.TestIP.IPAddr[1],
                             pRQB->args.TestIP.IPAddr[2]);
            NAREDbgPrint(".%d\r\n",
                         pRQB->args.TestIP.IPAddr[3],0,0);

            break;
        case  NARE_OPC_RESOLVE_CONFIG:
            NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT," pRQB->args.ResConf:\r\n",0,0,0);
            NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT,"  Mode                 : %d\r\n",pRQB->args.ResConf.Mode,0,0);
            NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT,"  RequestID            : %d\r\n",pRQB->args.ResConf.RequestID,0,0);
            NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT,"  StationNameLen       : %X\r\n",pRQB->args.ResConf.StationNameLen,0,0);
            NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT,"  StationName          : ",0,0,0);

            for (i=0; i<pRQB->args.ResConf.StationNameLen; i++)
            {
                NAREDbgPrint("%c",((NARE_UPPER_MEM_U8_PTR_TYPE)pRQB->args.ResConf.pStationName)[i],0,0);
            }


            NAREDbgPrint("\r\n",0,0,0);

            NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT,"  IPAddr               : %d.%d.%d",
                             pRQB->args.ResConf.Cfg.IPAddr[0],
                             pRQB->args.ResConf.Cfg.IPAddr[1],
                             pRQB->args.ResConf.Cfg.IPAddr[2]);

            NAREDbgPrint(".%d\r\n",
                         pRQB->args.ResConf.Cfg.IPAddr[3],0,0);


            NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT,"  SubnetMask           : %d.%d.%d",
                             pRQB->args.ResConf.Cfg.SubMask[0],
                             pRQB->args.ResConf.Cfg.SubMask[1],
                             pRQB->args.ResConf.Cfg.SubMask[2]);

            NAREDbgPrint(".%d\r\n",
                         pRQB->args.ResConf.Cfg.SubMask[3],0,0);

            NAREDbgPrintHead(NARE_DBG_HID_NARE_IDENT,"  DefRouter            : %d.%d.%d",
                             pRQB->args.ResConf.Cfg.DefRouter[0],
                             pRQB->args.ResConf.Cfg.DefRouter[1],
                             pRQB->args.ResConf.Cfg.DefRouter[2]);

            NAREDbgPrint(".%d\r\n",
                         pRQB->args.ResConf.Cfg.DefRouter[3],0,0);

            break;
        case  NARE_OPC_TIMER:
            break;
        default:
            break;
    }

    NAREDbgWriteString("\r\n");

}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    NAREDbgPrintRQB                             +*/
/*+  Input/Output          :    NARE_UPPER_RQB_PTR_TYPE     pRQB            +*/
/*+  Result                :    ----                                        +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to RQB                                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Prints RQB-Params                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


LSA_VOID NARE_LOCAL_FCT_ATTR NAREDbgPrintRQB(NARE_UPPER_RQB_PTR_TYPE     pRQB)
{

    NAREDbgPrintRQBHead(pRQB);
    NAREDbgPrintRQBParam(pRQB);

}

#endif

