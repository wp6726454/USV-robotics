#ifndef EDDS_CFG_H                  /* ----- reinclude-protection ----- */
#define EDDS_CFG_H

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
/*  F i l e               &F: edds_cfg.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P05.04.00.00_00.02.00.40         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-07-28                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Configuration for EDDS:                                                  */
/*  Defines constants, types and macros for edds.                            */
/*                                                                           */
/*  This file has to be overwritten during system integration, because       */
/*  some definitions depend on the different system, compiler or             */
/*  operating system.                                                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  28.04.09    AH    initial version. LSA_EDDS_P04.02.00.00_00.01.01.01     */
/*  31.06.10    AH    added protection configuration defines (AP00952063)    */
#endif
/*****************************************************************************/

/*===========================================================================*/
/*                              defines                                      */
/*===========================================================================*/

void edds_psi_startstop( int start );

#if defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)

    /*---------------------------------------------------------------------------*/
    /* New scheduling mode.                                                      */
    /*                                                                           */
    /* In this new mode, no EDDS interrupts are required anymore.                */
    /* All LLIF relevant Interrupt functions and functionalities must be removed.*/
    /* This mode supersedes (and unite) the following modes                      */
    /*   - EDDS_CFG_ISR_MODE_STANDARD with EDDS_CFG_SEND_CYCLE_BASED             */
    /*   - EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD                                   */
    /* These modes must not be defined if using new scheduling mode.             */
    /*                                                                           */
    /* As a result, all other (sub-)modes are not available anymore:             */
    /*   - EDDS-send modes 'blocked' and 'continuous'                            */
    /*                                                                           */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_CYCLIC_SCHEDULING_MODE

    /*---------------------------------------------------------------------------*/
    /* Components to include in EDDS                                             */
    /* undefine if not to include                                                */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_SRT_INCLUDE

    /*---------------------------------------------------------------------------*/
    /* Set multicast MAC count for EDDS.                                         */
    /* Defines the number ob multicast addresses supported by LL.                */
    /*---------------------------------------------------------------------------*/
    #define LLIF_CFG_MC_MAC_CNT   10

    /*---------------------------------------------------------------------------*/
    /* SWITCH-Support                                                            */
    /* Define if multiple ports present and switch functions supported.          */
    /*---------------------------------------------------------------------------*/

    #ifdef LLIF_CFG_SWITCH_SUPPORT

    /*---------------------------------------------------------------------------*/
    /* RT-Bit support (used with switches)                                       */
    /* Define if hardware supports the RT-Bit for use with redundancy.           */
    /*---------------------------------------------------------------------------*/

    #undef LLIF_CFG_SWITCH_RT_BIT_SUPPORT

    /*---------------------------------------------------------------------------*/
    /* CutThrough support (used with switches)                                   */
    /* Define if switch supports CutThrough mode.                                */
    /*---------------------------------------------------------------------------*/

    #undef LLIF_CFG_SWITCH_CT_SUPPORT

    #endif //LLIF_CFG_SWITCH_SUPPORT

    /*---------------------------------------------------------------------------*/
    /* Don't use interrupt source calculation by LLIF                            */
    /*---------------------------------------------------------------------------*/
    #define LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT

    /*---------------------------------------------------------------------------*/
    /* Ethernethardware to include (only one at a time!)                         */
    /*                                                                           */
    /* EDDS_CFG_HW_TEMPLATE : No hardwaresupport. Use template.                  */
    /* EDDS_CFG_HW_POWERPC : Support for PowerPC (PPC405)                        */
    /* EDDS_CFG_HW_NETARM  : Support for NetARM                                  */
    /* EDDS_CFG_HW_XSCALE  : Support for XSCALE                                  */
    /* EDDS_CFG_HW_WINCE   : Support for WINCE (operator panals)                 */
    /* EDDS_CFG_HW_KSZ88XX : Support for Micrel KSZ88XX-PMQL                     */
    /* EDDS_CFG_HW_PACKET32: Support for Packet32 (WPCAP)                        */
    /*                                                                           */
    /* Note: The EDDS-Package includes native Micrel and Packet32 (WinPCAP)      */
    /*       LLIF Support.                                                       */
    /*       For all other LLIF an additional HW-depended package                */
    /*       (Low-Level-functions for EDDS) must be added to this EDDS. These    */
    /*       packages can be obtained from the responsible developers. See       */
    /*       release notes.                                                      */
    /*                                                                           */
    /*       The template HW contains only empty LL functions. The EDDS can      */
    /*       be compiled with this setting, but has no functionality. This       */
    /*       template is intended to help developing a new hardware LLIF.        */
    /*                                                                           */
    /*---------------------------------------------------------------------------*/

    //EDDS_CFG_HW_xxx is defined external in psi_cfg_pf_eps.h/pciox_psi_cfg_plf_xxx.h!

    //#define EDDS_CFG_HW_TEMPLATE
    //#define EDDS_CFG_HW_KSZ88XX
    //#define EDDS_CFG_HW_INTEL
    //#define EDDS_CFG_HW_PACKET32

    /*---------------------------------------------------------------------------*/
    /* If LSA_RESULT is not defined by LSA, we define it here.                   */
    /*---------------------------------------------------------------------------*/

    /* Note: LSA_RESULT has to be the same type as "response" within             */
    /*       LSA-RQB-Header. This is currently LSA_UINT16!                       */

    #ifndef LSA_RESULT
    #define LSA_RESULT    LSA_RESPONSE_TYPE
    #endif

    /*---------------------------------------------------------------------------*/
    /* maximum number of devices supported                                       */
    /*---------------------------------------------------------------------------*/

    //EDDS_CFG_MAX_DEVICES is defined external in psi_cfg.h!

    #ifndef EDDS_CFG_MAX_DEVICES
    #error "EDDS_CFG_MAX_DEVICES not defined"
    #endif

    /*---------------------------------------------------------------------------*/
    /* maximum number of communication channels supported                        */
    /* Note: Maximum is limited by LSA_HANDLE_TYPE range!                        */
    /*---------------------------------------------------------------------------*/

    //EDDS_CFG_MAX_CHANNELS is defined external in psi_cfg.h!

    #ifndef EDDS_CFG_MAX_CHANNELS
    #error "EDDS_CFG_MAX_CHANNELS not defined"
    #endif

    /*---------------------------------------------------------------------------*/
    /* If EDDS_CFG_LED_BLINK_SUPPORT is defined the service EDD_SRV_LED_BLINK is */
    /* supported by EDDS (Note: Needed by other PNIO LSA components!)            */
    /* EDDS_CFG_BLINK_LED_NR defines the LED-Nr (0..x) used with service         */
    /* EDD_SRV_LED_BLINK. The number of LEDs supported depends on PHY.           */
    /*---------------------------------------------------------------------------*/

    #undef EDDS_CFG_LED_BLINK_SUPPORT
    #define EDDS_CFG_BLINK_LED_NR  0

    /*---------------------------------------------------------------------------*/
    /* Set some local_mem static or dynamic:                                     */
    /* If EDDS_CFG_LOCAL_MEM_STATIC is not defined, all local-memory will be     */
    /* allocated dynamically. If defined, some local memory is static.           */
    /*---------------------------------------------------------------------------*/

    #define EDDS_CFG_LOCAL_MEM_STATIC

    /*---------------------------------------------------------------------------*/
    /* Host-Byte-Order format:                                                   */
    /* big-endian   : define EDDS_CFG_BIG_ENDIAN                                 */
    /* little-endian: undef EDDS_CFG_BIG_ENDIAN (e.g. intel)                     */
    /*---------------------------------------------------------------------------*/

    #ifdef LSA_HOST_ENDIANESS_BIG
    #define EDDS_CFG_BIG_ENDIAN
    #endif

    /*---------------------------------------------------------------------------*/
    /* If a transmit-timeout should result in a fatal-error-message. If not      */
    /* defined we try to reinit the transmit unit and continue.                  */
    /*---------------------------------------------------------------------------*/

    #define EDDS_CFG_SEND_TIMEOUT_FATAL

    /*---------------------------------------------------------------------------*/
    /* If a a memory-free call error should result in an fatal-error call        */
    /* with EDDS_FATAL_ERR_MEM_FREE define EDDS_CFG_FREE_ERROR_IS_FATAL. If not  */
    /* memory-free errors will be ignored.                                       */
    /*---------------------------------------------------------------------------*/

    #define EDDS_CFG_FREE_ERROR_IS_FATAL

    /*---------------------------------------------------------------------------*/
    /* For VLAN-Tagged frames the max framelength (without CRC) may              */
    /* exceed from 1514 to 1518 octets. To enable this all buffers must be       */
    /* increased and the receiver must be set into a mode for receiving "long"   */
    /* frames. Note that in this mode the receiver dont checks for invalid (to   */
    /* long) frames!                                                             */
    /* Note: Long frame support is not necessary to receive xRT frames!          */
    /*       because xRT-Frames dont exceed 1514 bytes include VLAN-tag.         */
    /*       So leave this setting undefined.                                    */
    /* Note: Leave on if you use this EDDS together with an EDDI or EDDP to have */
    /*       identical behaviour.                                                */
    /*---------------------------------------------------------------------------*/

    #define EDDS_CFG_LONG_FRAME_SUPPORT

    /*---------------------------------------------------------------------------*/
    /* Max number of acyclic Frames to be underway at once (sum of NRT, Low aSRT */
    /* and High aSRT). For each acyclic frame an additonal individual limit can  */
    /* be configured too (see below).                                            */
    /* Note that the max-number also limited by the max number of Tx-Resources   */
    /* (TxBufferCnt in DPB). So TxBufferCnt should be great enough!              */
    /*                                                                           */
    /* Range: 1..x                                                               */
    /*                                                                           */
    /* Note: Recommended for CPU317:   4                                         */
    /*---------------------------------------------------------------------------*/

    #define EDDS_CFG_ACYCL_MAX_SEND_AT_ONCE     82 /* 1GBit max Frames: wirespeed*/

    /*---------------------------------------------------------------------------*/
    /* For NRT the maximum number of frames send at once can be configured for   */
    /* each priority.                                                            */
    /*                                                                           */
    /* This value is limited to the number of tx-resources present in hardware   */
    /* and the maximum acyclic frames possible to send at once (see above).      */
    /* If only one priority is supported, only the low-setting is used.          */
    /* With a value of 1 (default) only one NRT frame is send at once. Further   */
    /* frames will be send after the previous finished.                          */
    /*                                                                           */
    /* If the value is set to 2 or higher the EDDS will send more (up to         */
    /* EDDS_CFG_NRT_MAX_SEND_AT_ONCE_XXX, depending on free tx-resources in under-*/
    /* laying controller and EDDS_CFG_ACYCL_MAX_SEND_AT_ONCE) frames at once.    */
    /* So NRT will send faster but may delay higher prio SRT frames for a longer */
    /* time.                                                                     */
    /*                                                                           */
    /* Range: 1..x (except EDDS_CFG_NRT_MAX_SEND_AT_ONCE_ORG)                    */
    /*                                                                           */
    /* EDDS_CFG_NRT_MAX_SEND_AT_ONCE_ORG may be set to 0. If so all sends with   */
    /* ORG-Prio will be mapped to LOW prio and the EDDS dont handles a ORG prio  */
    /* queue (speeds up transmit handling). ORG Prio is used with MRP and GSY    */
    /* which is not neccecary used within StandardMAC EDDS. And GSY (if          */
    /* present) never sends with StandardMAC EDDS because of lack of timestamps. */
    /*                                                                           */
    /* If MRP and/or GSY is used, set EDDS_CFG_NRT_MAX_SEND_AT_ONCE_ORG to:      */
    /* 2 (MRP) + PortCnt-1 (GSY). Also adjust EDDS_CFG_ACYCL_MAX_SEND_AT_ONCE.   */
    /* (Note: GSY only used if timestamps present within EDDS! currently not)    */
    /*                                                                           */
    /* Note: Recommended for CPU317:  Low: 2                                     */
    /*---------------------------------------------------------------------------*/

    #define EDDS_CFG_NRT_MAX_SEND_AT_ONCE_ORG       0
    #define EDDS_CFG_NRT_MAX_SEND_AT_ONCE_HIGH      1489
    #define EDDS_CFG_NRT_MAX_SEND_AT_ONCE_MEDIUM    1489
    #define EDDS_CFG_NRT_MAX_SEND_AT_ONCE_LOW       1489

    /*---------------------------------------------------------------------------*/
    /* Configure EDDS-send mode (blocked or continuous)                          */
    /*---------------------------------------------------------------------------*/
    /*-Continuous-mode: ---(undef EDDS_CFG_SEND_BLOCKED)-------------------------*/
    /*                                                                           */
    /* The EDDS fills up all tx-resources of LLIF.If a resource gets free,it will*/
    /* be filled if tx-requests pending. Typically each transmitted frame will   */
    /* generate an interrupt (EDDS only uses EDDS_LL_SEND() of LLIF)             */
    /*                                                                           */
    /*-Blocked-mode: ------(define EDDS_CFG_SEND_BLOCKED)------------------------*/
    /*                                                                           */
    /* The EDDS fills up all tx-resources of LLIF. The EDDS only fills new       */
    /* frames if all frames sent out. Typically only one interrupt is generated  */
    /* for the last frame of a block. (EDDS uses EDDS_LL_SEND() and              */
    /* EDDS_LL_SEND_TRIGGER() of LLIF)                                           */
    /*                                                                           */
    /*---------------------------------------------------------------------------*/
    /* Which mode is supported by LLIF depends on LLIF and Ethernetcontroller.   */
    /*---------------------------------------------------------------------------*/

    #define EDDS_CFG_SEND_BLOCKED

    /*---------------------------------------------------------------------------*/
    /* Cycle-based blocked send mode (EDDS_CFG_SEND_CYCLE_BASED)                 */
    /*---------------------------------------------------------------------------*/
    /* If EDDS_CFG_SEND_BLOCKED is defined, the EDDS can be configured to send   */
    /* blocks only cycle based. This is enabled with EDDS_CFG_SEND_CYCLE_BASED.  */
    /*                                                                           */
    /* If EDDS_CFG_SEND_CYCLE_BASED is defined, sends are done blocked with every*/
    /* start of SRT-Cycle. So EDDS_CFG_SEND_CYCLE_BASED must not be defined if   */
    /* SRT is not configured and EDDS_CFG_SEND_BLOCKED is not configured.        */
    /* In this mode the maximum NRT and aSRT databytes per block                 */
    /* can be configured. Note that it is neccessary that in this mode the       */
    /* LowerLevel Interface is configured with enough send-resources to be able  */
    /* to manage all frames sended in one block. This is about all cyclic SRT    */
    /* frames + at least 1 Frame for aSRT/NRT! not checked by EDDS (TxBufferCnt  */
    /* in DPB)                                                                   */
    /*                                                                           */
    /* IF EDDS_CFG_SEND_CYCLE_BASED is defined                                   */
    /* EDDS_CFG_SEND_BLOCKED_MAX_ACYCL_BYTES_10xxx                               */
    /* must be configured to limit NRT and aSRT Data send per Cycle.             */
    /*                                                                           */
    /* Note: If EDDS_CFG_SEND_CYCLE_BASED is defined, send is only possible if a */
    /*       cyclic SRT channel is opened! (instead no cyclic-timer is running!) */
    /*                                                                           */
    /* Note: EDDS_CFG_SEND_CYCLE_BASED must only be defined if the LLIF is       */
    /*       configured for blocked mode too (EDDS_CFG_SEND_BLOCKED)             */
    /*                                                                           */
    /* Range:                                                                    */
    /*                                                                           */
    /* EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_10000 : 1518..x   (10  Gbit)          */
    /* EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_1000 : 1518..x    (1   Gbit)          */
    /* EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_100  : 1518..x    (100 Mbit)          */
    /* EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_10   : 1518..x    (10  Mbit)          */
    /*                                                                           */
    /* If range < 1518  the EDDS maybe not able to send a frame!                 */
    /*                                                                           */
    /*                                                                           */
    /* Note: Recommended for CPU317:  all (2*EDD_IEEE_FRAME_LENGTH)              */
    /*---------------------------------------------------------------------------*/

    #if defined (EDDS_CFG_SEND_BLOCKED)
    #if defined (EDDS_CFG_SRT_INCLUDE)
    #define EDDS_CFG_SEND_CYCLE_BASED
    #endif //EDDS_CFG_SRT_INCLUDE
    #endif //EDDS_CFG_SEND_BLOCKED

    #if defined (EDDS_CFG_SEND_CYCLE_BASED)
        #define EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_10000 1250304
        #define EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_1000  125952
        #define EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_100   13312
        #define EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_10    2048
    #endif //EDDS_CFG_SEND_CYCLE_BASED

    /*---------------------------------------------------------------------------*/
    /* If defined the Systemadaption can check if enough time elapsed since      */
    /* last consumer-check (it can check if two consumer checks occur back to    */
    /* back. e.g. to fast after each other, because of system delays/jitter)     */
    /* The output-macros EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT() and              */
    /* EDDS_CSRT_CONSUMER_CYCLE_START() used for this check if the               */
    /* jitter check is defined.                                                  */
    /*---------------------------------------------------------------------------*/

    #undef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK

    /*---------------------------------------------------------------------------*/
    /* If EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK is defined, the EDDS can be  */
    /* configured to call EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT_END() at the end  */
    /* of consumer checks. With this the systemadaption can verify the time the  */
    /* consumer checks ended. So on the next start of consumer checks it may     */
    /* verify the time since last end instead of last start of consumer checks.  */
    /* This is optional.                                                         */
    /*---------------------------------------------------------------------------*/

    #undef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK_END

    /*---------------------------------------------------------------------------*/
    /* Memory allocation                                                         */
    /* If defined, some more local memory within edds will be allocated within   */
    /* fast memory (EDDS_ALLOC_LOCAL_FAST_MEM) instead of normal memory.         */
    /* Define if enough fast memory is present.                                  */
    /*                                                                           */
    /* GLOB:  Global EDDS management. Includes LLIF management.                  */
    /* NRT :  NRT management                                                     */
    /*                                                                           */
    /* Note: SRT management always uses fast memory!                             */
    /*---------------------------------------------------------------------------*/

    #define EDDS_CFG_GLOB_FAST_MEM
    #undef EDDS_CFG_NRT_FAST_MEM

    /*===========================================================================*/
    /* Compiler/HW-dependend optimization settings                               */
    /*===========================================================================*/

    /*===========================================================================*/
    /* defaults for needed but not defined values..                              */
    /*===========================================================================*/

    #ifndef EDDS_FAR_FCT
    #define EDDS_FAR_FCT
    #endif

    /*---------------------------------------------------------------------------*/
    /* Type of systemhandles used by system for device and interface.            */
    /*---------------------------------------------------------------------------*/

    #define EDDS_SYS_HANDLE   LSA_SYS_PTR_TYPE // usage of SYS_PTR

    /*===========================================================================*/
    /*                         code- and data-attributes                         */
    /*===========================================================================*/

    #define EDDS_STATIC  static

    /*=====  code attributes  =====*/

    #define EDDS_SYSTEM_IN_FCT_ATTR  EDD_ATTR_FAR   /* system to prefix */
    #define EDDS_SYSTEM_OUT_FCT_ATTR EDD_ATTR_FAR   /* prefix to system */
    #define EDDS_LOCAL_FCT_ATTR      EDD_ATTR_NEAR  /* local function */

    /*=====  data attributes  =====*/

    #define EDDS_LOWER_MEM_ATTR      EDD_ATTR_HUGE  /* lower-memory data */
    #define EDDS_SYSTEM_MEM_ATTR     EDD_ATTR_HUGE  /* system-memory data */
    #define EDDS_LOCAL_MEM_ATTR      EDD_ATTR_SHUGE /* local data */

    /*===========================================================================*/
    /* macro name:    EDDS_FILE_SYSTEM_EXTENSION(module_id_)                     */
    /*                                                                           */
    /* macro:         System integrators can use the macro to smuggle something  */
    /*                into the component source files. By default it's empty.    */
    /*                                                                           */
    /*                The system integrators are responsible for the contents of */
    /*                that macro.                                                */
    /*                                                                           */
    /*                Internal data of LSA components may not be changed!        */
    /*                                                                           */
    /* parameter:     Module-id                                                  */
    /*                                                                           */
    /*                There's no prototype for that macro!                       */
    /*===========================================================================*/

    #define EDDS_FILE_SYSTEM_EXTENSION(module_id_)

    /*===========================================================================*/
    /* LTRC - Support (LSA-Trace)                                                */
    /*===========================================================================*/

    /*---------------------------------------------------------------------------*/
    /* define the Trace mode for EDDS                                            */
    /* 0: no traces                                                              */
    /* 1: enable LSA Traces                                                      */
    /* 2: enable LSA Idx Traces                                                  */
    /*---------------------------------------------------------------------------*/

    //EDDS_CFG_TRACE_MODE is defined external in psi_cfg.h!

    #ifndef EDDS_CFG_TRACE_MODE
    #error "EDDS_CFG_TRACE_MODE not defined"
    #endif

    /*===========================================================================*/
    /* Debugging                                                                 */
    /*===========================================================================*/

    /*---------------------------------------------------------------------------*/
    /* Reentrance/prio check for EDDS function context.                          */
    /* Adds a check for reentrance/prio mismatch within EDDS.                    */
    /* Can be activated to check for mismatch with context prios of EDDS requests */
    /* Should only be used for testing because it slows down the EDDS.           */
    /* If a mismatch is detected the EDDS issues a fatal error with errorcode    */
    /* EDDS_FATAL_ERR_REENTRANCE                                                 */
    /*---------------------------------------------------------------------------*/

    #undef EDDS_CFG_REENTRANCE_CHECK

    /*---------------------------------------------------------------------------*/
    /* Assertions                                                                */
    /* Adds some additional error checks (assertions). causing a fatal error     */
    /* on occurence.                                                             */
    /*---------------------------------------------------------------------------*/

    #define EDDS_CFG_DEBUG_ASSERT

    /*---------------------------------------------------------------------------*/
    /* For additional internal debugging                                         */
    /*---------------------------------------------------------------------------*/

    #undef EDDS_CFG_DEBUG_ENABLE

    /*===========================================================================*/
    /*                               protection                                  */
    /*===========================================================================*/

    /*---------------------------------------------------------------------------*/
    /* EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT:                         */
    /*                                                                           */
    /*   defined: Call EDDS_ENTER_HIGH and EDDS_EXIT_HIGH protection macros      */
    /*            within HIGH context.                                           */
    /*                                                                           */
    /*   undef  : Dont use EDDS_ENTER_HIGH and EDDS_EXIT_HIGH protection macros  */
    /*            within HIGH context.                                           */
    /*                                                                           */
    /*   Can be defined if the protection is implemented with semaphore or       */
    /*   mutex.                                                                  */
    /*---------------------------------------------------------------------------*/

    #define EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT

    /*---------------------------------------------------------------------------*/
    /* EDDS_CFG_USE_ENTER_EXIT_MIDDLE_WITHIN_MIDDLE_CONTEXT:                     */
    /*                                                                           */
    /*   defined: Call EDDS_ENTER_MIDDLE and EDDS_EXIT_MIDDLE protection macros  */
    /*            within MIDDLE context.                                         */
    /*                                                                           */
    /*   undef  : Dont use EDDS_ENTER_MIDDLE and EDDS_EXIT_MIDDLE protection     */
    /*            macros within MIDDLE context.                                  */
    /*                                                                           */
    /*   Can be defined if the protection is implemented with semaphore or       */
    /*   mutex.                                                                  */
    /*---------------------------------------------------------------------------*/

    #define EDDS_CFG_USE_ENTER_EXIT_MIDDLE_WITHIN_MIDDLE_CONTEXT

    /*---------------------------------------------------------------------------*/
    /* EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT                               */
    /*                                                                           */
    /*   defined: Call EDDS_LOCK and EDDS_UNLOCK protection macros               */
    /*            within ISR context.                                            */
    /*                                                                           */
    /*   undef  : Dont use EDDS_LOCK and EDDS_UNLOCK protection                  */
    /*            macros within LOCK context.                                    */
    /*                                                                           */
    /*   Can be defined if the protection is implemented with semaphore or       */
    /*   mutex.                                                                  */
    /*---------------------------------------------------------------------------*/

    #ifndef PSI_EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT
    #error "PSI_EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT not defined"
    #endif

    #if PSI_EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT
    #define EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT
    #else
    #undef EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT
    #endif

    /*===========================================================================*/
    /*                                 macros                                    */
    /*===========================================================================*/

    /* cpu cache line size in byte; could be a constant or a global variable (LSA_UINT) */
    /* #define EDDS_CPU_CACHE_LINE_SIZE 32 */

    /*===========================================================================*/
    /*                                io buffer                                  */
    /*===========================================================================*/

    /*---------------------------------------------------------------------------*/
    /* If defined IOBuffer is supported and compiled with EDDS. Enable this      */
    /* option for application and EDDS running on the same processor. In         */
    /* distributed  case (application with IOBuffer handler and EDDS running on  */
    /* a different processor) this define can be undefined.                      */
    /* Undefine if EDDS is compiled without EDDS_CFG_SRT_INCLUDE support.        */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_USE_IOBUFFER

    /*---------------------------------------------------------------------------*/
    /* The system adaption has to set the type properly to meet the requirements */
    /* for an implementation of the synchronization mechanism depending on the   */
    /* destination platform. The type must be at least LSA_UINT16.               */
    /*---------------------------------------------------------------------------*/
    #define EDDS_EXCHANGE_TYPE  long

    /*---------------------------------------------------------------------------*/
    /* undocumented */
    #undef EDDS_CFG_NO_PROVIDER_OVERLOAD_HANDLING

#elif !defined (PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE)

    /*---------------------------------------------------------------------------*/
    /* Interrupt handler mode.                                                   */
    /*                                                                           */
    /* EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD:                                      */
    /* This enables cyclic polling mode within EDDS. This mode can not be set    */
    /* interface granular but applies to all interfaces.                         */
    /* In this mode the hardware interrupts are disabled. The edds_interrupt     */
    /* handler has to be called cyclically by system adaption. Pending events    */
    /* must be obtained depending on used LL. The event source must be           */
    /* supplied at the call to edds_interrupt.                                   */
    /* In this mode a receive overload limitation is active. See                 */
    /* RxIsrMaxFrameCnt in EDDS_DPB_TYPE.                                        */
    /* Note: This mode can not be used together with EDDS_CFG_SRT_INCLUDE!       */
    /*                                                                           */
    /* EDDS_CFG_ISR_MODE_STANDARD:                                               */
    /* This mode can be used in two use cases. Either interrupts are enabled and */
    /* the edds interrupt handler is triggered by hardware interrupts or the     */
    /* edds interrupt handler is triggered cyclically. In this mode there is no  */
    /* receive overload limitation active. In both cases the event source has to */
    /* be obtained depending on LL. The event source must be supplied at         */
    /* the call to edds_interrupt. This mode can not be set interface granular   */
    /* but applies to all interfaces. This mode can be used together with        */
    /* EDDS_CFG_SRT_INCLUDE.                                                     */
    /*---------------------------------------------------------------------------*/
    #undef EDDS_CFG_ISR_MODE_CYCLIC_RXOVERLOAD
    #define EDDS_CFG_ISR_MODE_STANDARD

    /*---------------------------------------------------------------------------*/
    /* INTEL LLIF Interface                                                      */
    /*---------------------------------------------------------------------------*/

    #if defined (EDDS_CFG_HW_INTEL) || defined (EDDS_CFG_HW_KSZ88XX)

    /*---------------------------------------------------------------------------*/
    /* Set multicast MAC count for EDDS.                                         */
    /* Defines the number ob multicast addresses supported by LL.                */
    /*---------------------------------------------------------------------------*/
    #define LLIF_CFG_MC_MAC_CNT   10

    /*---------------------------------------------------------------------------*/
    /* SWITCH-Support                                                            */
    /* Define if multiple ports present and switch functions supported.          */
    /*---------------------------------------------------------------------------*/

    #ifdef LLIF_CFG_SWITCH_SUPPORT
    /*---------------------------------------------------------------------------*/
    /* RT-Bit support (used with switches)                                       */
    /* Define if hardware supports the RT-Bit for use with redundancy.           */
    /*---------------------------------------------------------------------------*/

    #undef LLIF_CFG_SWITCH_RT_BIT_SUPPORT

    /*---------------------------------------------------------------------------*/
    /* CutThrough support (used with switches)                                   */
    /* Define if switch supports CutThrough mode.                                */
    /*---------------------------------------------------------------------------*/

    #undef LLIF_CFG_SWITCH_CT_SUPPORT
    #endif

    /*---------------------------------------------------------------------------*/
    /* Support of DROP-Frames detection with forwarding  (used with switches)    */
    /*---------------------------------------------------------------------------*/

    #undef LLIF_CFG_SWITCH_DROP_FRAME_SUPPORT

    /*---------------------------------------------------------------------------*/
    /* Don't use interrupt source calculation by LLIF                            */
    /*---------------------------------------------------------------------------*/
    #define LLIF_CFG_DONT_USE_LL_INTERRUPT_FCT

    /*---------------------------------------------------------------------------*/
    /* Enable/disable arp filter in LL                                           */
    /*---------------------------------------------------------------------------*/
    #undef LLIF_CFG_USE_LL_ARP_FILTER

    #endif //EDDS_CFG_HW_INTEL || EDDS_CFG_HW_KSZ88XX

    /*---------------------------------------------------------------------------*/
    /* Components to include in EDDS                                             */
    /* undefine if not to include                                                */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_NRT_INCLUDE
    #define EDDS_CFG_SRT_INCLUDE

    /*---------------------------------------------------------------------------*/
    /* If LSA_RESULT is not defined by LSA, we define it here.                   */
    /*---------------------------------------------------------------------------*/

    /* Note: LSA_RESULT has to be the same type as "response" within             */
    /*       LSA-RQB-Header. This is currently LSA_UINT16!                       */

    #ifndef LSA_RESULT
    #define LSA_RESULT    LSA_RESPONSE_TYPE
    #endif

    /*---------------------------------------------------------------------------*/
    /* maximum number of devices supported                                       */
    /*---------------------------------------------------------------------------*/
    #ifndef EDDS_CFG_MAX_DEVICES
    #error "EDDS_CFG_MAX_DEVICES not defined"
    #endif

    /*---------------------------------------------------------------------------*/
    /* maximum number of communication channels supported                        */
    /* Note: Maximum is limited by LSA_HANDLE_TYPE range!                        */
    /*---------------------------------------------------------------------------*/
    #ifndef EDDS_CFG_MAX_CHANNELS
    #error "EDDS_CFG_MAX_CHANNELS not defined"
    #endif

    /*---------------------------------------------------------------------------*/
    /* If EDDS_CFG_LED_BLINK_SUPPORT is defined the service EDD_SRV_LED_BLINK is */
    /* supported by EDDS (Note: Needed by other PNIO LSA components!)            */
    /* EDDS_CFG_BLINK_LED_NR defines the LED-Nr (0..x) used with service         */
    /* EDD_SRV_LED_BLINK. The number of LEDs supported depends on PHY.           */
    /*---------------------------------------------------------------------------*/
    #undef EDDS_CFG_LED_BLINK_SUPPORT
    #define EDDS_CFG_BLINK_LED_NR  0

    /*---------------------------------------------------------------------------*/
    /* Set some local_mem static or dynamic:                                     */
    /* If EDDS_CFG_LOCAL_MEM_STATIC is not defined, all local-memory will be     */
    /* allocated dynamically. If defined, some local memory is static.           */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_LOCAL_MEM_STATIC

    /*---------------------------------------------------------------------------*/
    /* Host-Byte-Order format:                                                   */
    /* big-endian   : define EDDS_CFG_BIG_ENDIAN                                 */
    /* little-endian: undef EDDS_CFG_BIG_ENDIAN (e.g. intel,ibc32)               */
    /*---------------------------------------------------------------------------*/
    #ifdef LSA_HOST_ENDIANESS_BIG
    #define EDDS_CFG_BIG_ENDIAN
    #endif

    /*---------------------------------------------------------------------------*/
    /* If a transmit-timeout should result in a fatal-error-message. If not      */
    /* defined we try to reinit the transmit unit and continue.                  */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_SEND_TIMEOUT_FATAL

    /*---------------------------------------------------------------------------*/
    /* If a a memory-free call error should result in an fatal-error call        */
    /* with EDDS_FATAL_ERR_MEM_FREE define EDDS_CFG_FREE_ERROR_IS_FATAL. If not  */
    /* memory-free errors will be ignored.                                       */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_FREE_ERROR_IS_FATAL

    /*---------------------------------------------------------------------------*/
    /* For VLAN-Tagged frames the max framelength (without CRC) may              */
    /* exceed from 1514 to 1518 octets. To enable this all buffers must be       */
    /* increased and the receiver must be set into a mode for receiving "long"   */
    /* frames. Note that in this mode the receiver dont checks for invalid (to   */
    /* long) frames!                                                             */
    /* Note: Long frame support is not necessary to receive xRT frames!          */
    /*       because xRT-Frames dont exceed 1514 bytes include VLAN-tag.         */
    /*       So leave this setting undefined.                                    */
    /* Note: Leave on if you use this EDDS together with an EDDI or EDDP to have */
    /*       identical behaviour.                                                */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_LONG_FRAME_SUPPORT

    /*---------------------------------------------------------------------------*/
    /* Max number of acyclic Frames to be underway at once (sum of NRT, Low aSRT */
    /* and High aSRT). For each acyclic frame an additonal individual limit can  */
    /* be configured too (see below).                                            */
    /* Note that the max-number also limited by the max number of Tx-Resources   */
    /* (TxBufferCnt in DPB). So TxBufferCnt should be great enough!              */
    /*                                                                           */
    /* Range: 1..x                                                               */
    /*                                                                           */
    /* Note: Recommended for CPU317:   4                                         */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_ACYCL_MAX_SEND_AT_ONCE     2

    /*---------------------------------------------------------------------------*/
    /* For NRT the maximum number of frames send at once can be configured for   */
    /* each priority.                                                            */
    /*                                                                           */
    /* This value is limited to the number of tx-resources present in hardware   */
    /* and the maximum acyclic frames possible to send at once (see above).      */
    /* If only one priority is supported, only the low-setting is used.          */
    /* With a value of 1 (default) only one NRT frame is send at once. Further   */
    /* frames will be send after the previous finished.                          */
    /*                                                                           */
    /* If the value is set to 2 or higher the EDDS will send more (up to         */
    /* EDDS_CFG_NRT_MAX_SEND_AT_ONCE_XXX, depending on free tx-resources in under-*/
    /* laying controller and EDDS_CFG_ACYCL_MAX_SEND_AT_ONCE) frames at once.    */
    /* So NRT will send faster but may delay higher prio SRT frames for a longer */
    /* time.                                                                     */
    /*                                                                           */
    /* Range: 1..x (except EDDS_CFG_NRT_MAX_SEND_AT_ONCE_ORG)                    */
    /*                                                                           */
    /* EDDS_CFG_NRT_MAX_SEND_AT_ONCE_ORG may be set to 0. If so all sends with   */
    /* ORG-Prio will be mapped to LOW prio and the EDDS dont handles a ORG prio  */
    /* queue (speeds up transmit handling). ORG Prio is used with MRP and GSY    */
    /* which is not neccecary used within StandardMAC EDDS. And GSY (if          */
    /* present) never sends with StandardMAC EDDS because of lack of timestamps. */
    /*                                                                           */
    /* If MRP and/or GSY is used, set EDDS_CFG_NRT_MAX_SEND_AT_ONCE_ORG to:      */
    /* 2 (MRP) + PortCnt-1 (GSY). Also adjust EDDS_CFG_ACYCL_MAX_SEND_AT_ONCE.   */
    /* (Note: GSY only used if timestamps present within EDDS! currently not)    */
    /*                                                                           */
    /* Note: Recommended for CPU317:  Low: 2                                     */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_NRT_MAX_SEND_AT_ONCE_ORG       0
    #define EDDS_CFG_NRT_MAX_SEND_AT_ONCE_HIGH      1
    #define EDDS_CFG_NRT_MAX_SEND_AT_ONCE_MEDIUM    1
    #define EDDS_CFG_NRT_MAX_SEND_AT_ONCE_LOW       1

    /*---------------------------------------------------------------------------*/
    /* Configure EDDS-send mode (blocked or contineous)                          */
    /*---------------------------------------------------------------------------*/
    /*-Contineous-mode: ---(undef EDDS_CFG_SEND_BLOCKED)-------------------------*/
    /*                                                                           */
    /* The EDDS fills up all tx-resources of LLIF. If a resource gets free it will*/
    /* be filled if tx-requests pending. Typically each transmitted frame will   */
    /* generate an interrupt (EDDS only uses EDDS_LL_SEND() of LLIF)             */
    /*                                                                           */
    /*-Blocked-mode: ------(define EDDS_CFG_SEND_BLOCKED)------------------------*/
    /*                                                                           */
    /* The EDDS fills up all tx-resources of LLIF. The EDDS only fills new       */
    /* frames if all frames sent out. Typically only one interrupt is generated  */
    /* for the last frame of a block. (EDDS uses EDDS_LL_SEND() and              */
    /* EDDS_LL_SEND_TRIGGER() of LLIF)                                           */
    /*                                                                           */
    /*---------------------------------------------------------------------------*/
    /* Which mode is supported by LLIF depends on LLIF and Ethernetcontroller.   */
    /*                                                                           */
    /* For IBC32 both modes supported, but use EDDS_CFG_SEND_BLOCKED (because of */
    /* bug-workarounds)                                                          */
    /*---------------------------------------------------------------------------*/
    #undef EDDS_CFG_SEND_BLOCKED /* pcIOX with WinPcap: continuous mode */

    /*---------------------------------------------------------------------------*/
    /* Cycle-based blocked send mode (EDDS_CFG_SEND_CYCLE_BASED)                 */
    /*---------------------------------------------------------------------------*/
    /* If EDDS_CFG_SEND_BLOCKED is defined, the EDDS can be configured to send   */
    /* blocks only cycle based. This is enabled with EDDS_CFG_SEND_CYCLE_BASED.  */
    /*                                                                           */
    /* If EDDS_CFG_SEND_CYCLE_BASED is defined, sends are done blocked with every*/
    /* start of SRT-Cycle. So EDDS_CFG_SEND_CYCLE_BASED must not be defined if   */
    /* SRT is not configured and EDDS_CFG_SEND_BLOCKED is not configured.        */
    /* In this mode the maximum NRT and aSRT databytes per block                 */
    /* can be configured. Note that it is neccessary that in this mode the       */
    /* LowerLevel Interface is configured with enough send-resources to be able  */
    /* to manage all frames sended in one block. This is about all cyclic SRT    */
    /* frames + at least 1 Frame for aSRT/NRT! not checked by EDDS (TxBufferCnt  */
    /* in DPB)                                                                   */
    /*                                                                           */
    /* IF EDDS_CFG_SEND_CYCLE_BASED is defined                                   */
    /* EDDS_CFG_SEND_BLOCKED_MAX_ACYCL_BYTES_10xxx                               */
    /* must be configured to limit NRT and aSRT Data send per Cycle.             */
    /*                                                                           */
    /* Note: If EDDS_CFG_SEND_CYCLE_BASED is defined, send is only possible if a */
    /*       cyclic SRT channel is opened! (instead no cyclic-timer is running!) */
    /*                                                                           */
    /* Note: EDDS_CFG_SEND_CYCLE_BASED must only be defined if the LLIF is       */
    /*       configured for blocked mode too (EDDS_CFG_SEND_BLOCKED)             */
    /*                                                                           */
    /* Range:                                                                    */
    /*                                                                           */
    /* EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_10000 : 1518..x   (10  Gbit)          */
    /* EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_1000  : 1518..x   (1   Gbit)          */
    /* EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_100   : 1518..x   (100 Mbit)          */
    /* EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_10    : 1518..x   (10  Mbit)          */
    /*                                                                           */
    /* If range < 1518  the EDDS maybe not able to send a frame!                 */
    /*                                                                           */
    /*                                                                           */
    /* Note: Recommended for CPU317:  all (2*EDD_IEEE_FRAME_LENGTH)              */
    /*---------------------------------------------------------------------------*/
    /* Note: Needed to be defined for Tx-Under BUG-Workaround for IBC32!!!       */
    /*---------------------------------------------------------------------------*/

    #ifdef EDDS_CFG_HW_IBC32
    #ifdef EDDS_CFG_SEND_BLOCKED
    #ifdef EDDS_CFG_SRT_INCLUDE
    #define EDDS_CFG_SEND_CYCLE_BASED
    #define EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_10000   2000
    #define EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_1000    2000
    #define EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_100     2000
    #define EDDS_CFG_SEND_CYCLE_MAX_ACYCL_BYTES_10      2000
    #endif
    #endif
    #endif

    /*---------------------------------------------------------------------------*/
    /* If defined the Systemadaption can check if enough time elapsed since      */
    /* last consumer-check (it can check if two consumer checks occur back to    */
    /* back. e.g. to fast after each other, because of system delays/jitter)     */
    /* The output-macros EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT() and              */
    /* EDDS_CSRT_CONSUMER_CYCLE_START() used for this check if the               */
    /* jitter check is defined.                                                  */
    /*---------------------------------------------------------------------------*/
    #undef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK

    /*---------------------------------------------------------------------------*/
    /* If EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK is defined, the EDDS can be  */
    /* configured to call EDDS_CSRT_CONSUMER_CYCLE_CHECK_EVENT_END() at the end  */
    /* of consumer checks. With this the systemadaption can verify the time the  */
    /* consumer checks ended. So on the next start of consumer checks it may     */
    /* verify the time since last end instead of last start of consumer checks.  */
    /* This is optional.                                                         */
    /*---------------------------------------------------------------------------*/
    #undef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK_END

    /*---------------------------------------------------------------------------*/
    /* Memory allocation                                                         */
    /* If defined, some more local memory within edds will be allocated within   */
    /* fast memory (EDDS_ALLOC_LOCAL_FAST_MEM) instead of normal memory.         */
    /* Define if enough fast memory is present.                                  */
    /*                                                                           */
    /* GLOB:  Global EDDS management. Includes LLIF management.                  */
    /* NRT :  NRT management                                                     */
    /*                                                                           */
    /* Note: SRT management always uses fast memory!                             */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_GLOB_FAST_MEM
    #undef EDDS_CFG_NRT_FAST_MEM

    /*---------------------------------------------------------------------------*/
    /* EDDS receive overload check                                               */
    /*                                                                           */
    /* The EDDS optionally monitors the receiver load and, if to heavy, switches */
    /* OFF the receiver for some time. This is to prevent the CPU from beeing    */
    /* inoperable because of heavy network traffic.                              */
    /*                                                                           */
    /* EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE: Enables the rx overload check          */
    /* EDDS_CFG_RX_OVERLOAD_CHECK_FACTOR: Defines the check cycle for rx overload*/
    /*                                   as a multiple of 100ms (1..x)           */
    /*                                   (e.g. 1=100ms, 2=200ms)                 */
    /* EDDS_CFG_RX_OVERLOAD_RX_LIMIT   : Defines the maximum number of received  */
    /*                                   frames within one check cycle. If more  */
    /*                                   frames received, the receiver is        */
    /*                                   switched OFF for a time (RXOFF1/2/3)    */
    /*                                   (e.g. EDDS_CFG_RX_OVERLOAD_CHECK_FACTOR */
    /*                                    * 5000 for 5000 Frames/100ms)          */
    /* EDDS_CFG_RX_OVERLOAD_RXOFF_1    : Switch OFF time in multiple of 100ms    */
    /*                                   (1..x). e.g. 1                          */
    /* EDDS_CFG_RX_OVERLOAD_RXOFF_2    : Switch OFF time in multiple of 100ms    */
    /*                                   (1..x). e.g. 5                          */
    /* EDDS_CFG_RX_OVERLOAD_RXOFF_3    : Switch OFF time in multiple of 100ms    */
    /*                                   (1..x). e.g. 10                         */
    /* EDDS_CFG_RX_OVERLOAD_RXON_MIN   : Minimum number of check cycles the      */
    /*                                   receiver has to remain on without       */
    /*                                   overload to remain on.                  */
    /*                                   (0..x). e.g. 1. 0 = none                */
    /*---------------------------------------------------------------------------*/

    #undef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE

    #ifdef EDDS_CFG_RX_OVERLOAD_CHECK_ENABLE
    #define EDDS_CFG_RX_OVERLOAD_CHECK_FACTOR   1
    #define EDDS_CFG_RX_OVERLOAD_RX_LIMIT       (EDDS_CFG_RX_OVERLOAD_CHECK_FACTOR * 5000)
    #define EDDS_CFG_RX_OVERLOAD_RXOFF_1        1
    #define EDDS_CFG_RX_OVERLOAD_RXOFF_2        5
    #define EDDS_CFG_RX_OVERLOAD_RXOFF_3        10
    #define EDDS_CFG_RX_OVERLOAD_RXON_MIN       1
    #endif

    /*===========================================================================*/
    /* Compiler/HW-dependend optimization settings and Bug-workaround            */
    /*===========================================================================*/

    /*=IBC32/CPU317 =============================================================*/

    #ifdef EDDS_CFG_HW_IBC32
    /*---------------------------------------------------------------------------*/
    /* Special optimization for ISR-Routines of IBC32. This will cause the       */
    /* code to be put into a special section and optimize set to 2               */
    /* Section:   SCRATCH_PAD_CODE                                               */
    /* (for use with ibc32/tasking)                                              */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_OPTIMIZE_ISR_IBC32_TASKING

    /*---------------------------------------------------------------------------*/
    /* Defines how to handle rx-isr                                              */
    /* defined:    With this setting, we clear out the isr-fifo on isr-entry and */
    /*             after each handled rx-frame. So we are not limited by the 16  */
    /*             isr-fifo entrys and we can use more then 16 rx-descriptors.   */
    /* undefined:  On each interrupt only one isr-fifo entry is read out and     */
    /*             max. one rx-descriptor is handled. because of this, more      */
    /*             then 16 rx-descriptors are worthless because we only have     */
    /*             16 isr-fifo entry and if they are filled the dr-unit stops.   */
    /*             (this mode was used in previous versions of EDDS)             */
    /*---------------------------------------------------------------------------*/
    #undef EDDS_CFG_IBC32_RX_MODE_FIFO_CLEAR

    /*---------------------------------------------------------------------------*/
    /* For functions called from scratch-pad code but located outside (and vice  */
    /* versa) we need an _far attribute with the tasking-Compiler in order to get*/
    /* the correct call - opcode (if not the code will be out of range when      */
    /* linked!)                                                                  */
    /*---------------------------------------------------------------------------*/
    #ifdef EDDS_CFG_OPTIMIZE_ISR_IBC32_TASKING
    #define EDDS_FAR_FCT _far
    #endif

    /*---------------------------------------------------------------------------*/
    /* Bugworkaround for MAC-Receiver hang at Half-Duplex-Mode                   */
    /* Define the number of SRT-Provider cycles after which a MAC-Reset will be  */
    /* done if no frame was received meanwhile (assuming receiver is stuck).     */
    /* Only works with SRT present and blocked-send-mode!                        */
    /* undef if not used.                                                        */
    /* Note: This bug is fixed with IBC32 BC-Step. Enable if for previous Steps. */
    /*---------------------------------------------------------------------------*/
    #ifdef EDDS_CFG_SEND_BLOCKED
    #ifdef EDDS_CFG_SRT_INCLUDE
    #if 0
    #define EDDS_CFG_IBC32_MAC_HANG_WORKAROUND  200   /* 200 provider Cycles     */
    #endif
    #endif
    #endif

    /*---------------------------------------------------------------------------*/
    /* PHY-Setting.                                                              */
    /*---------------------------------------------------------------------------*/
    /* On default the EDDS uses the LXT971A PHY with IBC32 (used in CPU317).     */
    /* If the PHY can not be accessed and should not be handled by software      */
    /* the define EDDS_CFG_IBC32_PHY_NONE can be used.                           */
    /* If defined the EDDS assumes the following:                                */
    /* - Link is always up with 100Mbit/Fullduplex                               */
    /* - No Link-State setting possible                                          */
    /* - No LED-settings possible.                                               */
    /* - No PHY-Interrupt expected                                               */
    /* - No Linkchangedetection.                                                 */
    /*---------------------------------------------------------------------------*/
    #undef EDDS_CFG_IBC32_PHY_NONE

    #endif
    /*=end IBC32 ================================================================*/

    /*= NetARM ==================================================================*/
    #ifdef EDDS_CFG_HW_NETARM
    #define EDDS_CFG_NA_MC_ADDR_CNT   1                /* number of MC addresses */
    #endif

    /*=end NetARM ===============================================================*/

    /*===========================================================================*/
    /* defaults for needed but not defined values..                              */
    /*===========================================================================*/
    #ifndef EDDS_FAR_FCT
    #define EDDS_FAR_FCT
    #endif

    /*---------------------------------------------------------------------------*/
    /* Type of systemhandles used by system for device and interface.            */
    /*---------------------------------------------------------------------------*/
    #define EDDS_SYS_HANDLE   LSA_SYS_PTR_TYPE // usage of SYS_PTR

    /*===========================================================================*/
    /*                         code- and data-attributes                         */
    /*===========================================================================*/

    #define EDDS_STATIC  static

    /*=====  code attributes  =====*/

    #define EDDS_SYSTEM_IN_FCT_ATTR  EDD_ATTR_FAR   /* system to prefix */
    #define EDDS_SYSTEM_OUT_FCT_ATTR EDD_ATTR_FAR   /* prefix to system */
    #define EDDS_LOCAL_FCT_ATTR      EDD_ATTR_NEAR  /* local function */

    /*=====  data attributes  =====*/

    #define EDDS_LOWER_MEM_ATTR      EDD_ATTR_HUGE  /* lower-memory data */
    #define EDDS_SYSTEM_MEM_ATTR     EDD_ATTR_HUGE  /* system-memory data */
    #define EDDS_LOCAL_MEM_ATTR      EDD_ATTR_SHUGE /* local data */

    /*===========================================================================*/
    /* macro name:    EDDS_FILE_SYSTEM_EXTENSION(module_id_)                     */
    /*                                                                           */
    /* macro:         System integrators can use the macro to smuggle something  */
    /*                into the component source files. By default it's empty.    */
    /*                                                                           */
    /*                The system integrators are responsible for the contents of */
    /*                that macro.                                                */
    /*                                                                           */
    /*                Internal data of LSA components may not be changed!        */
    /*                                                                           */
    /* parameter:     Module-id                                                  */
    /*                                                                           */
    /*                There's no prototype for that macro!                       */
    /*===========================================================================*/
    #define EDDS_FILE_SYSTEM_EXTENSION(module_id_)

    /*===========================================================================*/
    /* LTRC - Support (LSA-Trace)                                                */
    /*===========================================================================*/

    /*---------------------------------------------------------------------------*/
    /* define the Trace mode for EDDS                                            */
    /* 0: no traces or external traces (default)                                 */
    /* 1: enable Traces and use LTRC (LSA-Trace module) see edds_ltrc.h          */
    /* 2: enable Traces and use EDDS internal trace-functions (developer only)   */
    /*---------------------------------------------------------------------------*/

    //EDDS_CFG_TRACE_MODE is defined external in psi_cfg.h!

    #ifndef EDDS_CFG_TRACE_MODE
    #error "EDDS_CFG_TRACE_MODE not defined"
    #endif

    /*=============================================================================
     * If the LSA component LTRC isn't used for trace in the target system, then
     * the EDDS trace macros can be defined here. On default they are empty.
     *
     * See also files edds_trc.h and lsa_cfg.h/txt.
     *
     *===========================================================================*/

    #if EDDS_CFG_TRACE_MODE == 0
    #define EDDS_UPPER_TRACE_00(level_, msg_)
    #define EDDS_UPPER_TRACE_01(level_, msg_, para1_)
    #define EDDS_UPPER_TRACE_02(level_, msg_, para1_, para2_)
    #define EDDS_UPPER_TRACE_03(level_, msg_, para1_, para2_, para3_)
    #define EDDS_UPPER_TRACE_04(level_, msg_, para1_, para2_, para3_, para4_)
    #define EDDS_UPPER_TRACE_05(level_, msg_, para1_, para2_, para3_, para4_, para5_)

    #define EDDS_LOWER_TRACE_00(level_, msg_)
    #define EDDS_LOWER_TRACE_01(level_, msg_, para1_)
    #define EDDS_LOWER_TRACE_02(level_, msg_, para1_, para2_)
    #define EDDS_LOWER_TRACE_03(level_, msg_, para1_, para2_, para3_)
    #define EDDS_LOWER_TRACE_04(level_, msg_, para1_, para2_, para3_, para4_)

    #define EDDS_SYSTEM_TRACE_00(level_, msg_)
    #define EDDS_SYSTEM_TRACE_01(level_, msg_, para1_)
    #define EDDS_SYSTEM_TRACE_02(level_, msg_, para1_, para2_)
    #define EDDS_SYSTEM_TRACE_03(level_, msg_, para1_, para2_, para3_)
    #define EDDS_SYSTEM_TRACE_04(level_, msg_, para1_, para2_, para3_, para4_)

    #define EDDS_FUNCTION_TRACE_00(level_, msg_)
    #define EDDS_FUNCTION_TRACE_01(level_, msg_, para1_)
    #define EDDS_FUNCTION_TRACE_02(level_, msg_, para1_, para2_)
    #define EDDS_FUNCTION_TRACE_03(level_, msg_, para1_, para2_, para3_)
    #define EDDS_FUNCTION_TRACE_04(level_, msg_, para1_, para2_, para3_, para4_)

    #define EDDS_PROGRAM_TRACE_00(level_, msg_)
    #define EDDS_PROGRAM_TRACE_01(level_, msg_, para1_)
    #define EDDS_PROGRAM_TRACE_02(level_, msg_, para1_, para2_)
    #define EDDS_PROGRAM_TRACE_03(level_, msg_, para1_, para2_, para3_)
    #define EDDS_PROGRAM_TRACE_04(level_, msg_, para1_, para2_, para3_, para4_)
    #define EDDS_PROGRAM_TRACE_05(level_, msg_, para1_, para2_, para3_, para4_, para5_)
    #define EDDS_PROGRAM_TRACE_06(level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
    #define EDDS_PROGRAM_TRACE_07(level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
    #define EDDS_PROGRAM_TRACE_08(level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)

    #define EDDS_LL_SYSTEM_TRACE_00(level_, msg_)
    #define EDDS_LL_SYSTEM_TRACE_01(level_, msg_, para1_)
    #define EDDS_LL_SYSTEM_TRACE_02(level_, msg_, para1_, para2_)
    #define EDDS_LL_SYSTEM_TRACE_03(level_, msg_, para1_, para2_, para3_)
    #define EDDS_LL_SYSTEM_TRACE_04(level_, msg_, para1_, para2_, para3_, para4_)

    #define EDDS_LL_FUNCTION_TRACE_00(level_, msg_)
    #define EDDS_LL_FUNCTION_TRACE_01(level_, msg_, para1_)
    #define EDDS_LL_FUNCTION_TRACE_02(level_, msg_, para1_, para2_)
    #define EDDS_LL_FUNCTION_TRACE_03(level_, msg_, para1_, para2_, para3_)
    #define EDDS_LL_FUNCTION_TRACE_04(level_, msg_, para1_, para2_, para3_, para4_)

    #define EDDS_LL_PROGRAM_TRACE_00(level_, msg_)
    #define EDDS_LL_PROGRAM_TRACE_01(level_, msg_, para1_)
    #define EDDS_LL_PROGRAM_TRACE_02(level_, msg_, para1_, para2_)
    #define EDDS_LL_PROGRAM_TRACE_03(level_, msg_, para1_, para2_, para3_)
    #define EDDS_LL_PROGRAM_TRACE_04(level_, msg_, para1_, para2_, para3_, para4_)
    #define EDDS_LL_PROGRAM_TRACE_05(level_, msg_, para1_, para2_, para3_, para4_, para5_)
    #define EDDS_LL_PROGRAM_TRACE_06(level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_)
    #define EDDS_LL_PROGRAM_TRACE_07(level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_)
    #define EDDS_LL_PROGRAM_TRACE_08(level_, msg_, para1_, para2_, para3_, para4_, para5_, para6_, para7_, para8_)
    #endif

    /*---------------------------------------------------------------------------*/
    /* If EDDS_CFG_TRACE_MODE = 1 we can activate the EDDS trace compile levels in */
    /* addition to the LTRC compile level. Because the LTRC compile level only   */
    /* works for all subsystem and components within the system we are not able  */
    /* to deactivate/set a level for a specific subsystem. With this we can      */
    /* With EDDS_CFG_TRACE_MODE = 2 the compile levels are always used!          */
    /*---------------------------------------------------------------------------*/
    #undef EDDS_CFG_LTRC_USE_EDD_COMPILE_LEVEL

    /*---------------------------------------------------------------------------*/
    /* EDDS Trace compile levels.                                                */
    /*---------------------------------------------------------------------------*/
    /* If EDDS_CFG_TRACE_MODE = 1 and EDDS_CFG_LTRC_USE_EDD_COMPILE_LEVEL is     */
    /* defined, or if EDDS_CFG_TRACE_MODE = 2 we can set the compile-level for   */
    /* traces. Traces above this level will not be included into the program code*/
    /* (e.g. 3 means 7..4 is not included. 0 means nothing is included)          */
    /* 0 : off (no traces)                                                       */
    /* 1 : fatal errors                                                          */
    /* 2 : errors                                                                */
    /* 3 : unexpected situations                                                 */
    /* 4 : warning                                                               */
    /* 5 : note high                                                             */
    /* 6 : note                                                                  */
    /* 7 : note low                                                              */
    /* 8 : chat                                                                  */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_DEBUG_UPPER_TRACE_COMPILE_LEVEL           3
    #define EDDS_CFG_DEBUG_LOWER_TRACE_COMPILE_LEVEL           3
    #define EDDS_CFG_DEBUG_SYSTEM_TRACE_COMPILE_LEVEL          3
    #define EDDS_CFG_DEBUG_FUNCTION_TRACE_COMPILE_LEVEL        3
    #define EDDS_CFG_DEBUG_PROGRAM_TRACE_COMPILE_LEVEL         3
    #define EDDS_CFG_DEBUG_LL_SYSTEM_TRACE_COMPILE_LEVEL       3
    #define EDDS_CFG_DEBUG_LL_FUNCTION_TRACE_COMPILE_LEVEL     3
    #define EDDS_CFG_DEBUG_LL_PROGRAM_TRACE_COMPILE_LEVEL      3

    /*===========================================================================*/
    /* Debugging                                                                 */
    /*===========================================================================*/

    /*---------------------------------------------------------------------------*/
    /* Reentrance/prio check for EDDS function context.                          */
    /* Adds a check for reentrance/prio mismatch within EDDS.                    */
    /* Can be activated to check for mismatch with context prios of EDDS requests */
    /* Should only be used for testing because it slows down the EDDS.           */
    /* If a mismatch is detected the EDDS issues a fatal error with errorcode    */
    /* EDDS_FATAL_ERR_REENTRANCE                                                 */
    /*                                                                           */
    /* Important: This check shall NOT be activated if one of the following      */ 
    /*            is defined, because the check works only with priority         */
    /*            based protection!                                              */
    /*                                                                           */
    /*            EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT               */
    /*            EDDS_CFG_USE_ENTER_EXIT_MIDDLE_WITHIN_MIDDLE_CONTEXT           */
    /*            EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR                            */
    /*---------------------------------------------------------------------------*/
    #undef EDDS_CFG_REENTRANCE_CHECK

    /*---------------------------------------------------------------------------*/
    /* Assertions                                                                */
    /* Adds some additional error checks (assertions). causing a fatal error     */
    /* on occurence.                                                             */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_DEBUG_ASSERT

    /*---------------------------------------------------------------------------*/
    /* For additional internal debugging                                         */
    /*---------------------------------------------------------------------------*/
    #undef EDDS_CFG_DEBUG_ENABLE

    /*===========================================================================*/
    /*                               protection                                  */
    /*===========================================================================*/

    /*---------------------------------------------------------------------------*/
    /* EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT:                         */
    /*                                                                           */
    /*   defined: Call EDDS_ENTER_HIGH and EDDS_EXIT_HIGH protection macros      */
    /*            within HIGH context.                                           */
    /*                                                                           */
    /*   undef  : Dont use EDDS_ENTER_HIGH and EDDS_EXIT_HIGH protection macros  */
    /*            within HIGH context.                                           */
    /*                                                                           */
    /*   Can be defined if the protection is implemented with semaphore or       */
    /*   mutex.                                                                  */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_USE_ENTER_EXIT_HIGH_WITHIN_HIGH_CONTEXT

    /*---------------------------------------------------------------------------*/
    /* EDDS_CFG_USE_ENTER_EXIT_MIDDLE_WITHIN_MIDDLE_CONTEXT:                     */
    /*                                                                           */
    /*   defined: Call EDDS_ENTER_MIDDLE and EDDS_EXIT_MIDDLE protection macros  */
    /*            within MIDDLE context.                                         */
    /*                                                                           */
    /*   undef  : Dont use EDDS_ENTER_MIDDLE and EDDS_EXIT_MIDDLE protection     */
    /*            macros within MIDDLE context.                                  */
    /*                                                                           */
    /*   Can be defined if the protection is implemented with semaphore or       */
    /*   mutex.                                                                  */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_USE_ENTER_EXIT_MIDDLE_WITHIN_MIDDLE_CONTEXT

    /*---------------------------------------------------------------------------*/
    /* EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT                               */
    /*                                                                           */
    /*   defined: Call EDDS_LOCK and EDDS_UNLOCK protection macros               */
    /*            within ISR context.                                            */
    /*                                                                           */
    /*   undef  : Dont use EDDS_LOCK and EDDS_UNLOCK protection                  */
    /*            macros within LOCK context.                                    */
    /*                                                                           */
    /*   Can be defined if the protection is implemented with semaphore or       */
    /*   mutex.                                                                  */
    /*---------------------------------------------------------------------------*/

    #ifndef PSI_EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT
    #error "PSI_EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT not defined"
    #endif

    #if PSI_EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT
    #define EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT
    #else
    #undef EDDS_CFG_USE_LOCK_UNLOCK_WITHIN_ISR_CONTEXT
    #endif

    /*===========================================================================*/
    /*                                 macros                                    */
    /*===========================================================================*/

    /* cpu cache line size in byte; could be a constant or a global variable (LSA_UINT) */
    /* #define EDDS_CPU_CACHE_LINE_SIZE 32 */

    /*===========================================================================*/
    /*                                io buffer                                  */
    /*===========================================================================*/

    /*---------------------------------------------------------------------------*/
    /* If defined IOBuffer is supported and compiled with EDDS. Enable this      */
    /* option for application and EDDS running on the same processor. In         */
    /* distributed  case (application with IOBuffer handler and EDDS running on  */
    /* a different processor) this define can be undefined.                      */
    /*---------------------------------------------------------------------------*/
    #define EDDS_CFG_USE_IOBUFFER

    /*---------------------------------------------------------------------------*/
    /* The system adaption has to set the type properly to meet the requirements */
    /* for an implementation of the synchronization mechanism depending on the   */
    /* destination platform. The type must be at least LSA_UINT16.               */
    /*---------------------------------------------------------------------------*/
    #define EDDS_EXCHANGE_TYPE  long

    /*---------------------------------------------------------------------------*/
    /* undocumented */
    #undef EDDS_CFG_NO_PROVIDER_OVERLOAD_HANDLING

#endif //PSI_EDDS_CFG_CYCLIC_SCHEDULING_MODE

/*****************************************************************************/
/*  end of file EDDS_CFG.H                                                   */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_CFG_H */
