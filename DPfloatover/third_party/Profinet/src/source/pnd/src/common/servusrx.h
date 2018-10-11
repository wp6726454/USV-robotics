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
/*  F i l e               &F: servusrx.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Local download user interface functions                                  */
/*                                                                           */
/*****************************************************************************/

#ifndef SERVUSER_H
#define SERVUSER_H

#include "pniobase.h"

#undef ATTR_PACKED
#if defined(_MSC_VER)
  #pragma pack( push, safe_old_packing, 4 )
  #define ATTR_PACKED
#elif defined(__GNUC__)
  #define ATTR_PACKED  __attribute__ ((aligned (4)))
#elif defined(BYTE_ATTR_PACKING)
  #include "pack.h"
  #define ATTR_PACKED PPC_BYTE_PACKED
#else
  #error please adapt pniousrx.h header for your compiler
#endif

typedef enum
{
    PNIO_CP_SELECT_WITH_PCI_LOCATION  = 1,  // CP selected by PCI location 
    PNIO_CP_SELECT_WITH_MAC_ADDRESS   = 2   // CP selected by MAC address
} PNIO_CP_SELECT_TYPE;

#define PNIO_MAC_ADDR_SIZE  6
typedef PNIO_UINT8 PNIO_MAC_ADDR_TYPE[PNIO_MAC_ADDR_SIZE];  /* MAC address type */

typedef struct pnio_sw_version_type_s 
{
	PNIO_UINT8 revision_prefix;          // revision prefix (i.E. T, P, ..)
	PNIO_UINT8 functional_enhancement;   // revision number for functional enhancement
	PNIO_UINT8 bug_fix;                  // revision number for bug fix
	PNIO_UINT8 internal_change;          // revision number for internal change
} PNIO_SW_VERSION_TYPE;

typedef struct pnio_system_description_s 
{
    PNIO_UINT8              Vendor[128];       
    PNIO_UINT8              ProductFamily[128];
    PNIO_UINT8              ProductSerialNr[128];
    PNIO_UINT8              IM_DeviceType[25+1];  //TO-DO: Make a desicion how to use PSI_MAX_SIZE_IM_DEVICE_TYPE here
    PNIO_UINT8              IM_OrderId[20+1];  //TO-DO: Make a desicion how to use PSI_MAX_SIZE_IM_ORDER_ID here    
    PNIO_UINT16 		    IM_HwRevision;
    PNIO_SW_VERSION_TYPE    IM_SwVersion; 
    PNIO_UINT16 		    IM_RevisionCounter;
    PNIO_UINT16 		    IM_Version;
    PNIO_UINT16 		    IM_Supported;
    PNIO_UINT16 		    IM_ProfileSpecificType;
    PNIO_UINT16 		    IM_ProfileId;
} PNIO_SYSTEM_DESCR;

typedef struct pnio_pci_location_s
{
    PNIO_UINT16 BusNr;       // Bus number
    PNIO_UINT16 DeviceNr;    // Device number
    PNIO_UINT16 FunctionNr;  // Function number

} PNIO_PCI_LOCATION_TYPE, *PNIO_PCI_LOCATION_PTR_TYPE;


typedef struct pnio_cp_id_s
{
    PNIO_CP_SELECT_TYPE        CpSelection;      // Selector for board selection
    PNIO_MAC_ADDR_TYPE         CpMacAddr;        // MAC address of CP
    PNIO_PCI_LOCATION_TYPE     CpPciLocation;    // PCI location of CP
    PNIO_UINT8                 Description[300]; // Wpcap: 揑ntel(R) Ethernet Connection I217-V?/ 揚nDevDriver: I210 board?
} PNIO_CP_ID_TYPE, *PNIO_CP_ID_PTR_TYPE;

// enum of PNIO stack components (used for trace level setting)
enum pnio_stack_comp {
    PNIO_TRACE_COMP_ACP,
    PNIO_TRACE_COMP_ADONIS,
    PNIO_TRACE_COMP_CLRPC,
    PNIO_TRACE_COMP_CM,
    PNIO_TRACE_COMP_DCP,
    PNIO_TRACE_COMP_EDDI,
    PNIO_TRACE_COMP_EDDP,
    PNIO_TRACE_COMP_EDDS,
    PNIO_TRACE_COMP_EPS,
    PNIO_TRACE_COMP_GSY,
    PNIO_TRACE_COMP_HIF,
    PNIO_TRACE_COMP_IOH,
    PNIO_TRACE_COMP_LLDP,
    PNIO_TRACE_COMP_MRP,
    PNIO_TRACE_COMP_NARE,
    PNIO_TRACE_COMP_OHA,
    PNIO_TRACE_COMP_PNTRC,
    PNIO_TRACE_COMP_POF,
    PNIO_TRACE_COMP_PSI,
    PNIO_TRACE_COMP_SNMPX,
    PNIO_TRACE_COMP_SOCK,
    PNIO_TRACE_COMP_TCIP,
    PNIO_TRACE_COMP_PCIOX,
    PNIO_TRACE_COMP_PNTEST,
    PNIO_TRACE_COMP_PND,

    PNIO_TRACE_COMP_NUM //has to be last element!
};

enum pnio_trace_level
{
    PNIO_TRACE_LEVEL_OFF,
    PNIO_TRACE_LEVEL_FATAL,
    PNIO_TRACE_LEVEL_ERROR,
    PNIO_TRACE_LEVEL_UNEXP,
    PNIO_TRACE_LEVEL_WARN,
    PNIO_TRACE_LEVEL_NOTE_HIGH,
    PNIO_TRACE_LEVEL_NOTE,
    PNIO_TRACE_LEVEL_NOTE_LOW,
    PNIO_TRACE_LEVEL_CHAT,

    PNIO_TRACE_LEVEL_NUM //has to be last element!
};


typedef void (*PNIO_PNTRC_BUFFER_FULL)
       ( PNIO_UINT8       * pBuffer,     
         PNIO_UINT32        BufferSize );

typedef struct pnio_debug_settings_s
{
  PNIO_PNTRC_BUFFER_FULL      CbfPntrcBufferFull;                // CBF for receiving trace data; if NULL debug trace is turned off
  PNIO_UINT8                  TraceLevels[PNIO_TRACE_COMP_NUM];  // set trace levels for each component over all CPs
} PNIO_DEBUG_SETTINGS_TYPE, *PNIO_DEBUG_SETTINGS_PTR_TYPE;


typedef void (*PNIO_PNTRC_SET_TRACE_LEVEL_DONE)(void);

#ifdef __cplusplus
extern "C" {
#endif

  PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_shutdown ();


  PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_startup (
                                              PNIO_CP_ID_PTR_TYPE           CpList,         //in
                                              PNIO_UINT32                   NrOfCp,         //in
                                              PNIO_UINT8                  * pConfigData,    //in
                                              PNIO_UINT32                   ConfigDataLen,  //in
											  PNIO_UINT8                  * pRemaData,		//in
                                              PNIO_UINT32                   RemaDataLen, //in
                                              PNIO_SYSTEM_DESCR           * pSysDescr);

  PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_set_trace_level ( 
                                              PNIO_UINT32                       Component,              //in   select PNIO stack component (enum of pnio_stack_comp)
                                              PNIO_UINT32                       TraceLevel,             //in   select PNIO stack trace level (enum of pnio_trace_level)
                                              PNIO_PNTRC_SET_TRACE_LEVEL_DONE   CbfSetTraceLevelDone);  //in   CBF signals completion of transaction 

  PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_init (PNIO_DEBUG_SETTINGS_PTR_TYPE  DebugSetting);

  PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_get_network_adapters( PNIO_CP_ID_PTR_TYPE CpList, // output
                                            PNIO_UINT8 *NumberOfCps );   // output

  PNIO_UINT32 PNIO_CODE_ATTR SERV_CP_undo_init ();
#ifdef __cplusplus
}
#endif



#if defined(_MSC_VER)
  #pragma pack( pop, safe_old_packing )
#elif defined(BYTE_ATTR_PACKING)
  #include "unpack.h"
#endif

#endif /* SERVUSER_H */

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/