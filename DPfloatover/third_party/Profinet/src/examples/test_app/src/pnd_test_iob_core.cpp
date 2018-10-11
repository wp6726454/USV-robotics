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
/*  F i l e               &F: pnd_test_iob_core.cpp                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Functions & Callback functions for PNDriver Test Application             */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
#include "psi_cfg.h"
#include "stdio.h"

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include "conio.h"
#endif

#include "pniobase.h"
#include "pniobase.h"
#include "pniousrx.h"
#include "servusrx.h"

#include "eps_sys.h"

#include "pnd_int.h"
#include "pnd_sys.h"

#include "pnd_test.h"

#define PND_INVALID_HANDLE 0xFFFF

volatile PNIO_MODE_TYPE g_currentMode = PNIO_MODE_OFFLINE;
#ifdef __cplusplus
extern "C" {
#endif
PNIO_UINT32 g_ApplHandle = PND_INVALID_HANDLE;
#ifdef __cplusplus
}
#endif

#define ERROR_CAUSE_DESC                                                      \
  {0 /* CM_ERROR_CAUSE_NONE */, "No problem, everything OK"},                 \
      {1 /* CM_ERROR_CAUSE_IP_MULTIPLE */,                                    \
       "IP address exists multiple times"},                                   \
      {2 /* CM_ERROR_CAUSE_NOS_MULTIPLE */,                                   \
       "NameOfStation exists multiple times"},                                \
      {3 /* CM_ERROR_CAUSE_LINK_DOWN */, "LINK Down"},                        \
      {4 /* CM_ERROR_CAUSE_MAU_MISMATCH */, "MAUType mismatch"},              \
      {5 /* CM_ERROR_CAUSE_RPC_ERROR */, "RPC protocol error"},               \
      {6 /* CM_ERROR_CAUSE_PNIO_CONFIGURATION */,                             \
       "PNIO configuration protocol error"},                                  \
      {7 /* CM_ERROR_CAUSE_RPC_TIMEOUT */, "RPC application timeout"},        \
      {8 /* CM_ERROR_CAUSE_IP_SET_ABORTED */,                                 \
       "IP address assignment to IO device aborted"},                         \
      {9 /* CM_ERROR_CAUSE_IOD_NOT_FOUND */, "IO device not found"},          \
      {10 /* CM_ERROR_CAUSE_OUT_OF_RESOURCES */, "Out of AR resources"},      \
      {11 /* CM_ERROR_CAUSE_MISS */, "WDT / DHT expired"},                    \
      {12 /* CM_ERROR_CAUSE_NOS_SET_ABORTED */,                               \
       "Device with wrong NameOfStation found on topological position"},      \
      {13 /* CM_ERROR_CAUSE_IOC_ABORTED */, "Controller local abort reason"}, \
      {14 /* CM_ERROR_CAUSE_IOD_ABORTED */, "Device abort reason"},           \
      {15 /* CM_ERROR_CAUSE_SRD_ABORTED */, "SystemRedundancy abort reason"}, \
      {16 /* CM_ERROR_CAUSE_VENDORID_ABORTED */,                              \
       "Device with wrong VendorID / DeviceID / Instance found on "           \
       "topological position"},                                               \
      {17 /* CM_ERROR_CAUSE_MP_IN_PROGRESS */,                                \
       "MasterProject: Machine- or AddressTailoring in progress"},            \
      {18 /* CM_ERROR_CAUSE_MP_NOS_MISSING */,                                \
       "MasterProject: NameOfStation adaption missing (PDev with invalid "    \
       "NoS)"},                                                               \
      {19 /* CM_ERROR_CAUSE_MP_IPv4_MISSING */,                               \
       "MasterProject: IPv4Suite adaption missing (PDev with invalid "        \
       "IPv4Suite)"},                                                         \
      {20 /* CM_ERROR_CAUSE_MP_NOS_INVALID */,                                \
       "MasterProject: tailored NameOfStation of IOD is invalid"},            \
      {21 /* CM_ERROR_CAUSE_MP_IPv4_INVALID */,                               \
       "MasterProject: tailored IPv4Suite of IOD is invalid"},                \
      {22 /* CM_ERROR_CAUSE_MP_TAILORING_MISSING */,                          \
       "MasterProject: MachineTailoring missing"},                            \
      {23 /* CM_ERROR_CAUSE_MP_PDEV_ERROR */,                                 \
       "MasterProject: PDev configuration error"},                            \
      {24 /* CM_ERROR_CAUSE_MP_DEVICE_DISABLED */,                            \
       "MasterProject: disabled Device is activated"},                        \
  {                                                                           \
    25 /* CM_ERROR_CAUSE_MP_CONFIG_INCOMPLETE */,                             \
        "MasterProject: configuration incomplete"                             \
  }

#define REASON_CODE_DESC                                                     \
  {0 /* CM_AR_REASON_NONE */, "CM_AR_REASON_NONE"},                          \
      {1 /* CM_AR_REASON_1 */,                                               \
       "Sequence爊umbers燿o爊ot爉atch?no爈onger爑sed爄n爒ersions?=燰3.9)"},  \
      {2 /* CM_AR_REASON_2 */,                                               \
       "Alarm instance closed (no longer used in versions >= V3.6)"},        \
      {3 /* CM_AR_REASON_MEM */, "Out of memory"},                           \
      {4 /* CM_AR_REASON_FRAME */, "Adding provider or consumer failed"},    \
      {5 /* CM_AR_REASON_MISS */, "Miss (consumer)"},                        \
      {6 /* CM_AR_REASON_TIMER */, "Cmi timeout"},                           \
      {7 /* CM_AR_REASON_ALARM */, "Alarm-open failed"},                     \
      {8 /* CM_AR_REASON_ALSND */, "Alarm-send.cnf(-)"},                     \
      {9 /* CM_AR_REASON_ALACK */, "Alarm-ack-send.cnf(-)"},                 \
      {10 /* CM_AR_REASON_ALLEN */, "Alarm-data too long"},                  \
      {11 /* CM_AR_REASON_ASRT */, "Alarm.ind(err)"},                        \
      {12 /* CM_AR_REASON_RPC */, "Rpc-client call.cnf(-)"},                 \
      {13 /* CM_AR_REASON_ABORT */, "Ar-abort.req"},                         \
      {14 /* CM_AR_REASON_RERUN */, "Rerun aborts existing"},                \
      {15 /* CM_AR_REASON_REL */, "Got release.ind"},                        \
      {16 /* CM_AR_REASON_PAS */, "Device passivated"},                      \
      {17 /* CM_AR_REASON_RMV */, "Device / AR removed"},                    \
      {18 /* CM_AR_REASON_PROT */, "Protocol violation"},                    \
      {19 /* CM_AR_REASON_NARE */, "NARE error"},                            \
      {20 /* CM_AR_REASON_BIND */, "RPC-Bind error"},                        \
      {21 /* CM_AR_REASON_CONNECT */, "RPC-Connect error"},                  \
      {22 /* CM_AR_REASON_READ */, "RPC-Read error"},                        \
      {23 /* CM_AR_REASON_WRITE */, "RPC-Write error"},                      \
      {24 /* CM_AR_REASON_CONTROL */, "RPC-Control error"},                  \
      {25 /* CM_AR_REASON_25 */,                                             \
       "Reserved (formerly: pull or plug in forbidden window)"},             \
      {26 /* CM_AR_REASON_26 */, "Reserved (formerly: AP removed)"},         \
      {27 /* CM_AR_REASON_LNK_DOWN */, "Link down, for local purpose only"}, \
      {28 /* CM_AR_REASON_MMAC */, "Could not register multicast-mac "},     \
      {29 /* CM_AR_REASON_SYNC */,                                           \
       "Not synchronized (cannot start companion-ar)"},                      \
      {30 /* CM_AR_REASON_TOPO */,                                           \
       "Wrong topology (cannot start companion-ar)"},                        \
      {31 /* CM_AR_REASON_DCP_NAME */, "Dcp, station-name changed"},         \
      {32 /* CM_AR_REASON_DCP_RESET */,                                      \
       "Dcp, reset to factory or factory reset"},                            \
      {33 /* CM_AR_REASON_33 */,                                             \
       "Reserved (formerly: cannot start companion-ar)"},                    \
      {34 /* CM_AR_REASON_IRDATA */, "No irdata record yet"},                \
      {35 /* CM_AR_REASON_PDEV */, "Ownership of physical device"},          \
      {36 /* CM_AR_REASON_LNK_MODE */, "Link mode not full duplex"},         \
      {37 /* CM_AR_REASON_IPSUITE */,                                        \
       "IP-Suite [of the IOC] changed by means of DCP_set(IPParameter) or "  \
       "local engineering"},                                                 \
      {38 /* CM_AR_REASON_RDHT */, "IOCARSR RDHT expired"},                  \
      {39 /* CM_AR_REASON_PDEV_PRM */,                                       \
       "IOCARSR PDev, parametrization impossible"},                          \
  {                                                                          \
    40 /* CM_AR_REASON_ARDY */, "Remote application timeout expired"         \
  }

#define ERROR_CODE2_DESC \
  {0, "test 0"}, { 1, "test 2" }

typedef struct {
  PNIO_UINT16 value;
  const char *desc;
} TRANSLATE_TYPE;

TRANSLATE_TYPE ErrorCause[] = {ERROR_CAUSE_DESC};
TRANSLATE_TYPE ReasonCode[] = {REASON_CODE_DESC};
TRANSLATE_TYPE ErrorCode2[] = {ERROR_CODE2_DESC};  // not used yet.

const char *translate(TRANSLATE_TYPE *type, PNIO_UINT16 value) {
  return type[value].desc;
}

typedef struct {
  PNIO_BLOCK_HEADER BlockHeader; /* BlockType: 0x0020 */
  PNIO_UINT8 VendorIDHigh;
  PNIO_UINT8 VendorIDLow;
  PNIO_UINT8 OrderID[20];
  PNIO_UINT8 IM_Serial_Number[16];
  PNIO_UINT16 IM_Hardware_Revision; /* (big-endian) */
  PNIO_UINT8 IM_Software_Revision[4];
  PNIO_UINT16 IM_Revision_Counter;      /* (big-endian) */
  PNIO_UINT16 IM_Profile_ID;            /* (big-endian) */
  PNIO_UINT16 IM_Profile_Specific_Type; /* (big-endian) */
  PNIO_UINT8 IM_Version_Major;
  PNIO_UINT8 IM_Version_Minor;
  PNIO_UINT16
  IM_Supported; /* Bitliste Bit 1 --> Supports IM1... (big-endian) */
} ATTR_PACKED PNIO_IM0_TYPE;

typedef struct {
  PNIO_BLOCK_HEADER BlockHeader; /*  BlockType: 0x0021 */
  PNIO_UINT8 IM_Tag_Function[32];
  PNIO_UINT8 IM_Tag_Location[22];
} PNIO_IM1_TYPE;

typedef struct {
  PNIO_BLOCK_HEADER BlockHeader; /*  BlockType: 0x0022 */
  PNIO_UINT8 IM_Date[16];
} PNIO_IM2_TYPE;

typedef struct {
  PNIO_BLOCK_HEADER BlockHeader; /*  BlockType: 0x0023 */
  PNIO_UINT8 IM_Descriptor[54];
} PNIO_IM3_TYPE;

typedef struct {
  PNIO_BLOCK_HEADER BlockHeader; /*  BlockType: 0x0024 */
  PNIO_UINT8 IM_Signature[54];
} PNIO_IM4_TYPE;

void callback_for_alarm_ind(PNIO_CBE_PRM *pCbfPrm) {
/**************************************************************/
/* Attention :                                                */
/* this is a callback and must be returned as soon as possible*/
/* don't use any endless or time consuming functions          */
/* e.g. exit() would be fatal                                 */
/* defer all time consuming functionality to other threads    */
/**************************************************************/

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  wait_for_con();
#endif

  printf("\nCP Number / Handle : %d \n", (int)pCbfPrm->Handle);

  /* Check if correct callback type */
  if (pCbfPrm->CbeType == PNIO_CBE_ALARM_IND ||
      pCbfPrm->CbeType == PNIO_CBE_IFC_ALARM_IND) {
    switch (pCbfPrm->AlarmInd.pAlarmData->AlarmType) {
      case PNIO_ALARM_DIAGNOSTIC:
        printf("\nPNIO_ALARM_DIAGNOSTIC \n");
        printf("ChannelErrorType  : 0x%04x\n",
               pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.diag
                   .ChannelErrorType);
        printf(
            "ChannelNumber     : 0x%04x\n",
            pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.diag.ChannelNumber);
        printf("ChannelProperties : 0x%04x\n",
               pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.diag
                   .ChannelProperties);

        /********************************************************************/
        /* Check what kind of diagnostic alarm                              */
        /*   UserStrucIdent = user structure identifier for user alarm data */
        /********************************************************************/
        switch (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UserStrucIdent) {
          case 0x8000: /* 0x8000 :  diagnosis alarm */
            printf("\nPNIO_ALARM_DIAGNOSTIC -> diagnosis alarm \n");

            printf("ChannelNumber     = 0x%04x\n",
                   (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.diag
                        .ChannelNumber));
            printf("ChannelProperties = 0x%04x\n",
                   (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.diag
                        .ChannelProperties));
            printf("ChannelErrorType  = 0x%04x\n",
                   (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.diag
                        .ChannelErrorType));

            break;

          case 0x8002: /* 0x8002 :  extended diagnosis alarm */
            printf("\nPNIO_ALARM_DIAGNOSTIC -> extended diagnosis alarm  \n");

            printf("ChannelNumber       = 0x%04x\n",
                   (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.ext_diag
                        .ChannelNumber));
            printf("ChannelProperties   = 0x%04x\n",
                   (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.ext_diag
                        .ChannelProperties));
            printf("ChannelErrorType    = 0x%04x\n",
                   (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.ext_diag
                        .ChannelErrorType));
            printf("ExtChannelErrorType = 0x%04x\n",
                   (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.ext_diag
                        .ExtChannelErrorType));
            printf("ExtChannelAddValue  = 0x%08lx\n",
                   (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.ext_diag
                        .ExtChannelAddValue));

            break;

          default:
            printf("\nPNIO_ALARM_DIAGNOSTIC USI = 0x%04x\n",
                   pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UserStrucIdent);
            break;
        }

        break;

        break;
      case PNIO_ALARM_PROCESS:
        printf("PNIO_ALARM_PROCESS \n");
        break;
      case PNIO_ALARM_PULL:
        printf("PNIO_ALARM_PULL \n");
        break;

      case PNIO_ALARM_PLUG:
        printf("PNIO_ALARM_PLUG \n");
        break;

      case PNIO_ALARM_STATUS:
        printf("PNIO_ALARM_STATUS \n");
        break;

      case PNIO_ALARM_UPDATE:
        printf("PNIO_ALARM_UPDATE \n");
        break;

      case PNIO_ALARM_REDUNDANCY:
        printf("PNIO_ALARM_REDUNDACY \n");
        break;

      case PNIO_ALARM_CONTROLLED_BY_SUPERVISOR:
        printf("PNIO_ALARM_CONTROLLED_BY_SUPERVISOR \n");
        break;

      case PNIO_ALARM_RELEASED_BY_SUPERVISOR:
        printf("PNIO_ALARM_RELEASED_BY_SUPERVISOR \n");
        break;

      case PNIO_ALARM_PLUG_WRONG:
        printf("PNIO_ALARM_PLUG_WRONG \n");
        break;

      case PNIO_ALARM_RETURN_OF_SUBMODULE:
        printf("PNIO_ALARM_RETURN_OF_SUBMODULE  \n");
        break;

      case PNIO_ALARM_DIAGNOSTIC_DISAPPEARS:
        printf("PNIO_ALARM_DIAGNOSTIC_DISAPPEARS  \n");
        break;

      case PNIO_ALARM_MCR_MISMATCH:
        printf("PNIO_ALARM_MCR_MISMATCH  \n");
        break;

      case PNIO_ALARM_SYNC_DATA_CHANGED:
        printf("PNIO_ALARM_SYNC_DATA_CHANGED  \n");
        break;

      case PNIO_ALARM_ISOCHRONE_MODE_PROBLEM:
        printf("PNIO_ALARM_ISOCHRONE_MODE_PROBLEM \n");
        break;

      case PNIO_ALARM_NETWORK_COMPONENT_PROBLEM:
        printf("PNIO_ALARM_NETWORK_COMPONENT_PROBLEM  \n");
        break;

      case PNIO_ALARM_UPLOAD_AND_STORAGE:
        printf("PNIO_ALARM_UPLOAD_AND_STORAGE \n");
        break;

      case PNIO_ALARM_PULL_MODULE:
        printf("PNIO_ALARM_PULL_MODULE  \n");
        break;

      case PNIO_ALARM_DEV_FAILURE:
        printf("PNIO_ALARM_DEV_FAILURE \n");
        break;

      case PNIO_ALARM_DEV_RETURN:
        printf("PNIO_ALARM_DEV_RETURN \n");
        break;

      case PNIO_ALARM_PORT_DATA_CHANGED: {
        printf("PNIO_ALARM_PORT_DATA_CHANGED \n");

        switch (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UserStrucIdent) {
          case 0x8000: /* 0x8000 :  diagnosis alarm */
            printf("\nPNIO_ALARM_DIAGNOSTIC -> diagnosis alarm \n");

            printf("ChannelNumber     = 0x%04x\n",
                   (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.diag
                        .ChannelNumber));
            printf("ChannelProperties = 0x%04x\n",
                   (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.diag
                        .ChannelProperties));
            printf("ChannelErrorType  = 0x%04x\n",
                   (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.diag
                        .ChannelErrorType));

            break;
          case 0x8002: /* 0x8002 :  extended diagnosis alarm */
            printf("\nPNIO_ALARM_DIAGNOSTIC -> extended diagnosis alarm  \n");

            printf("ChannelNumber       = 0x%04x\n",
                   (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.ext_diag
                        .ChannelNumber));
            printf("ChannelProperties   = 0x%04x\n",
                   (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.ext_diag
                        .ChannelProperties));
            printf("ChannelErrorType    = 0x%04x\n",
                   (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.ext_diag
                        .ChannelErrorType));
            printf("ExtChannelErrorType = 0x%04x\n",
                   (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.ext_diag
                        .ExtChannelErrorType));
            printf("ExtChannelAddValue  = 0x%08lx\n",
                   (pCbfPrm->AlarmInd.pAlarmData->AlarmAinfo.UAData.ext_diag
                        .ExtChannelAddValue));
            break;
        }
        break;
      }
      default:
        printf("callback_for_alarm_indication called with wrong type \n");
        break;
    }
  }

  printf("Device Number   %d \n", pCbfPrm->AlarmInd.pAlarmData->DeviceNum);
  printf("Slot Number     %d \n", pCbfPrm->AlarmInd.pAlarmData->SlotNum);
  printf("SubSlot Number  %d \n", pCbfPrm->AlarmInd.pAlarmData->SubslotNum);
}

#define SWAP_16(var) (((((var)) & 0xFF00L) >> 8) + ((((var)) & 0x00FFL) << 8))

void print_ium0(PNIO_IM0_TYPE *im0) {
  char orderid[21];
  char serialnum[17];

  memcpy(&orderid, im0->OrderID, 20);
  memcpy(&serialnum, im0->IM_Serial_Number, 16);

  orderid[20] = '\0';
  serialnum[16] = '\0';

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  wait_for_con();
#endif

  printf(
      "\t  "
      "####################################################################\n");
  printf(
      "\t  # BlockHeader->BlockType:         0x%02x                            "
      "\n",
      im0->BlockHeader.BlockType);
  printf(
      "\t  # BlockHeader->BlockLength:       0x%02x                            "
      "\n",
      im0->BlockHeader.BlockLength);
  printf(
      "\t  # BlockHeader->BlockVersionHigh:  0x%02x                            "
      "\n",
      im0->BlockHeader.BlockVersionHigh);
  printf(
      "\t  # BlockHeader->BlockVersionLow:   0x%02x                            "
      "\n",
      im0->BlockHeader.BlockVersionLow);
  printf(
      "\t  # VendorIDHigh:                0x%02x                            \n",
      im0->VendorIDHigh);
  printf(
      "\t  # VendorIDLow:                 0x%02x                            \n",
      im0->VendorIDLow);
  printf(
      "\t  # OrderID:                     %s                                \n",
      orderid);
  printf(
      "\t  # Serial Number:               %s                                \n",
      serialnum);
  printf(
      "\t  # Hardware_Revision:              %ld                               "
      "\n",
      SWAP_16(im0->IM_Hardware_Revision));
  printf(
      "\t  # Software_Revision:              %c%d%d%d                          "
      "\n",
      (char)im0->IM_Software_Revision[0], im0->IM_Software_Revision[1],
      im0->IM_Software_Revision[2], im0->IM_Software_Revision[3]);
  printf(
      "\t  # Revision_Counter:               %ld                               "
      "\n",
      SWAP_16(im0->IM_Revision_Counter));
  printf(
      "\t  # Profile_ID:                     %ld                               "
      "\n",
      SWAP_16(im0->IM_Profile_ID));
  printf(
      "\t  # Profile_Specific_Type:          %ld                               "
      "\n",
      SWAP_16(im0->IM_Profile_Specific_Type));
  printf(
      "\t  # Version_Major:               %d                                \n",
      im0->IM_Version_Major);
  printf(
      "\t  # Version_Minor:               %d                                \n",
      im0->IM_Version_Minor);
  printf(
      "\t  # Supported:                      0x%02lx                           "
      "\n",
      SWAP_16(im0->IM_Supported));
  printf(
      "\t  "
      "####################################################################\n");
}

void print_ium1(PNIO_IM1_TYPE *im1) {
  char IM_Tag_Function[33];
  char IM_Tag_Location[23];

  memcpy(&IM_Tag_Function, im1->IM_Tag_Function, 32);
  memcpy(&IM_Tag_Location, im1->IM_Tag_Location, 22);

  IM_Tag_Function[32] = '\0';
  IM_Tag_Location[22] = '\0';

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  wait_for_con();
#endif

  printf("I&M data Read \n");

  printf(
      "\t  "
      "####################################################################\n");
  printf(
      "\t  # BlockHeader->BlockType:         0x%02x                            "
      "\n",
      im1->BlockHeader.BlockType);
  printf(
      "\t  # BlockHeader->BlockLength:       0x%02x                            "
      "\n",
      im1->BlockHeader.BlockLength);
  printf(
      "\t  # BlockHeader->BlockVersionHigh:  0x%02x                            "
      "\n",
      im1->BlockHeader.BlockVersionHigh);
  printf(
      "\t  # BlockHeader->BlockVersionLow:   0x%02x                            "
      "\n",
      im1->BlockHeader.BlockVersionLow);
  printf(
      "\t  # Function Tag:                %s                                 "
      "\n",
      IM_Tag_Function);
  printf(
      "\t  # Location Tag:                %s                                 "
      "\n",
      IM_Tag_Location);
  printf(
      "\t  "
      "####################################################################\n");
}

void print_ium2(PNIO_IM2_TYPE *im2) {
  char date[17];
  memcpy(&date, im2->IM_Date, 16);

  date[16] = '\0';

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  wait_for_con();
#endif

  printf(
      "\t  "
      "####################################################################\n");
  printf(
      "\t  # BlockHeader->BlockType:         0x%02x                            "
      "\n",
      im2->BlockHeader.BlockType);
  printf(
      "\t  # BlockHeader->BlockLength:       0x%02x                            "
      "\n",
      im2->BlockHeader.BlockLength);
  printf(
      "\t  # BlockHeader->BlockVersionHigh:  0x%02x                            "
      "\n",
      im2->BlockHeader.BlockVersionHigh);
  printf(
      "\t  # BlockHeader->BlockVersionLow:   0x%02x                            "
      "\n",
      im2->BlockHeader.BlockVersionLow);
  printf(
      "\t  # Installation Date:           %s                                 "
      "\n",
      date);
  printf(
      "\t  "
      "####################################################################\n");
}

void print_ium3(PNIO_IM3_TYPE *im3) {
  char descriptor[55];
  memcpy(&descriptor, im3->IM_Descriptor, 54);

  descriptor[54] = '\0';

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  wait_for_con();
#endif

  printf(
      "\t  "
      "####################################################################\n");
  printf(
      "\t  # BlockHeader->BlockType:         0x%02x                            "
      "\n",
      im3->BlockHeader.BlockType);
  printf(
      "\t  # BlockHeader->BlockLength:       0x%02x                            "
      "\n",
      im3->BlockHeader.BlockLength);
  printf(
      "\t  # BlockHeader->BlockVersionHigh:  0x%02x                            "
      "\n",
      im3->BlockHeader.BlockVersionHigh);
  printf(
      "\t  # BlockHeader->BlockVersionLow:   0x%02x                            "
      "\n",
      im3->BlockHeader.BlockVersionLow);
  printf(
      "\t  # Descriptor:                  %s                                 "
      "\n",
      descriptor);
  printf(
      "\t  "
      "####################################################################\n");
}

void print_ium4(PNIO_IM4_TYPE *im4) {
  char signature[55];

  memcpy(&signature, im4->IM_Signature, 54);

  signature[54] = '\0';

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  wait_for_con();
#endif

  printf(
      "\t  ################################################################### "
      "\n");
  printf(
      "\t  # BlockHeader->BlockType:         0x%02x                            "
      "\n",
      im4->BlockHeader.BlockType);
  printf(
      "\t  # BlockHeader->BlockLength:       0x%02x                            "
      "\n",
      im4->BlockHeader.BlockLength);
  printf(
      "\t  # BlockHeader->BlockVersionHigh:  0x%02x                            "
      "\n",
      im4->BlockHeader.BlockVersionHigh);
  printf(
      "\t  # BlockHeader->BlockVersionLow:   0x%02x                            "
      "\n",
      im4->BlockHeader.BlockVersionLow);
  printf(
      "\t  # Signature:                   %s                                 "
      "\n",
      signature);
  printf(
      "\t  "
      "####################################################################\n");
}

void print_pdiosysteminfo(const PNIO_UINT8 *pBuffer) {
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  wait_for_con();
#endif

  printf("  ###########################################\n");
  printf("  # BlockHeader->BlockType:         0x%04x  #\n",
         pnd_hton16(*(PNIO_UINT16 *)(pBuffer + 0)));
  printf("  # BlockHeader->BlockLength:       0x%04x  #\n",
         pnd_hton16(*(PNIO_UINT16 *)(pBuffer + 2)));
  printf("  # BlockHeader->BlockVersionHigh:  0x%02x    #\n",
         *(PNIO_UINT8 *)(pBuffer + 4));
  printf("  # BlockHeader->BlockVersionLow:   0x%02x    #\n",
         *(PNIO_UINT8 *)(pBuffer + 5));
  printf("  # IOSystemProperties:             0x%04x  #\n",
         pnd_hton16(*(PNIO_UINT16 *)(pBuffer + 6)));
  printf("  # Number Of Devices:              %d       #\n",
         pnd_hton16(*(PNIO_UINT16 *)(pBuffer + 8)));
  printf("  ###########################################\n");

  PNIO_UINT16 nr_of_devices = pnd_hton16(*(PNIO_UINT16 *)(pBuffer + 8));

  for (PNIO_UINT16 i = 0; i < nr_of_devices; i++) {
    printf("  # Station Number:                 0x%04x  #\n",
           pnd_hton16(*(PNIO_UINT16 *)(pBuffer + 10 + 4 * i)));
    printf("  # Device Properties:              0x%04x  #\n",
           pnd_hton16(*(PNIO_UINT16 *)(pBuffer + 12 + 4 * i)));
    printf("  ###########################################\n");
  }
}

void callback_for_ds_read_conf(PNIO_CBE_PRM *pCbfPrm) {
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  wait_for_con();
#endif

  switch (pCbfPrm->RecReadConf.RecordIndex) {
    case 0xAFF0: {
      PNIO_IM0_TYPE *im0 = (PNIO_IM0_TYPE *)pCbfPrm->RecReadConf.pBuffer;
      print_ium0(im0);

      break;
    }
    case 0xAFF1: {
      PNIO_IM1_TYPE *im1 = (PNIO_IM1_TYPE *)pCbfPrm->RecReadConf.pBuffer;
      print_ium1(im1);

      break;
    }
    case 0xAFF2: {
      PNIO_IM2_TYPE *im2 = (PNIO_IM2_TYPE *)pCbfPrm->RecReadConf.pBuffer;
      print_ium2(im2);

      break;
    }
    case 0xAFF3: {
      PNIO_IM3_TYPE *im3 = (PNIO_IM3_TYPE *)pCbfPrm->RecReadConf.pBuffer;
      print_ium3(im3);

      break;
    }
    case 0xAFF4: {
      PNIO_IM4_TYPE *im4 = (PNIO_IM4_TYPE *)pCbfPrm->RecReadConf.pBuffer;
      print_ium4(im4);

      break;
    }
    default:
      printf("Data Record Read Ref: %ld\n", pCbfPrm->RecReadConf.ReqRef);
      break;
  }
}

void callback_for_ds_write_conf(PNIO_CBE_PRM *pCbfPrm) {
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  wait_for_con();
#endif

  switch (pCbfPrm->RecWriteConf.RecordIndex) {
    case 0xAFF1:

      if (pCbfPrm->RecWriteConf.Err.ErrCode != 0) {
        printf("ErrCode    : %x\n", pCbfPrm->RecWriteConf.Err.ErrCode);
        printf("ErrDecode  : %x\n", pCbfPrm->RecWriteConf.Err.ErrDecode);
        printf("ErrCode1   : %x\n", pCbfPrm->RecWriteConf.Err.ErrCode1);
        printf("ErrCode2   : %x\n", pCbfPrm->RecWriteConf.Err.ErrCode2);
      } else {
        printf("I&M data Write done !\n");
      }

      break;
    default:
      printf("Data Record Write \n");
      break;
  };
}

void callback_for_mode_change_indication(PNIO_CBE_PRM *pCbfPrm) {
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  wait_for_con();
#endif

  /* Check if correct callback type */
  if (pCbfPrm->CbeType == PNIO_CBE_MODE_IND) {
    /* Callback has correct type so check mode change */
    /* and set global variable                        */
    switch (pCbfPrm->ModeInd.Mode) {
      case PNIO_MODE_OFFLINE:
        g_currentMode = PNIO_MODE_OFFLINE;
        printf("mode changed to PNIO_MODE_OFFLINE \n");
        break;
      case PNIO_MODE_CLEAR:
        g_currentMode = PNIO_MODE_CLEAR;
        printf("mode changed to PNIO_MODE_CLEAR \n");
        break;
      case PNIO_MODE_OPERATE:
        g_currentMode = PNIO_MODE_OPERATE;
        printf("mode changed to PNIO_MODE_OPERATE \n");
        break;
      default:
        printf("callback_for_mode_change_indication called with wrong mode \n");
        break;
    };
  };

  printf("\ncallback_for_mode_change_indication was called \n");
}

/*-------------------------------------------------------------*/
/* this function will be called from IO-BASE to signal that    */
/* a device was activated or deactivated                       */
/*                                                             */
/* parameters                                                  */
/*    pCbfPrm         : Callback information                   */
/*-------------------------------------------------------------*/
void callback_for_device_activation(PNIO_CBE_PRM *pCbfPrm) {
/**************************************************************/
/* Attention :                                                */
/* this is a callback and must be returned as soon as possible*/
/* don't use any endless or time consuming functions          */
/* e.g. exit() would be fatal                                 */
/* defer all time consuming functionality to other threads    */
/**************************************************************/
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  wait_for_con();
#endif

  printf("Device Activation \\ Deactivation CBF: \n");

  switch (pCbfPrm->DevActConf.Mode) {
    case PNIO_DA_TRUE:
      printf("Device with address %d has been activated with result %x \n",
             (int)pCbfPrm->DevActConf.pAddr->u.Addr,
             (unsigned int)pCbfPrm->DevActConf.Result);
      break;
    case PNIO_DA_FALSE:
      printf("Device with address %d has been deactivated with result %x \n",
             (int)pCbfPrm->DevActConf.pAddr->u.Addr,
             (unsigned int)pCbfPrm->DevActConf.Result);
      break;
  };
}

void callback_for_diagnostics_request(PNIO_CBE_PRM *pCbfPrm) {
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  wait_for_con();
#endif

  if (pCbfPrm->CtrlDiagConf.ErrorCode != PNIO_OK) {
    printf("\nDiagnose Error! ErrorCode = 0x%x\n",
           pCbfPrm->CtrlDiagConf.ErrorCode);
    return;
  }

  switch (pCbfPrm->CtrlDiagConf.pDiagData->DiagService) {
    case PNIO_CTRL_DIAG_CONFIG_SUBMODULE_LIST: {
      PNIO_CTRL_DIAG_CONFIG_SUBMODULE *pDiagConfSub;
      PNIO_UINT32 n_pnio_submodule_cnt = 0;
      PNIO_UINT32 l = 0;

      pDiagConfSub = (PNIO_CTRL_DIAG_CONFIG_SUBMODULE *)
                         pCbfPrm->CtrlDiagConf.pDiagDataBuffer;

      n_pnio_submodule_cnt = (pCbfPrm->CtrlDiagConf.DiagDataBufferLen) /
                             sizeof(PNIO_CTRL_DIAG_CONFIG_SUBMODULE);

      printf("\n\n");
      printf(
          "Read out submodule list ... (%3d submodules) bufferlen: %d size: %d "
          "\n\r",
          n_pnio_submodule_cnt, pCbfPrm->CtrlDiagConf.DiagDataBufferLen,
          sizeof(PNIO_CTRL_DIAG_CONFIG_SUBMODULE));

      printf("\n");
      for (l = 0; l < n_pnio_submodule_cnt; l++) {
        printf("##########################\n");
        printf("# Count            = %1d \n", l + 1);
        printf("# AddrType         = %3s \n",
               pDiagConfSub[l].Address.AddrType ? "GEO" : "LOG");
        printf("# Address          = %d  \n", pDiagConfSub[l].Address.u.Addr);
        printf("# DataLength       = %d  \n", pDiagConfSub[l].DataLength);
        printf("# DataType         = %s  \n",
               pDiagConfSub[l].DataType ? "IRT" : "RT");
        printf("# ComType          = %d  \n", pDiagConfSub[l].ComType);

        printf("# Api              = %d  \n", pDiagConfSub[l].Api);
        printf("# ReductionFactor  = %d  \n", pDiagConfSub[l].ReductionFactor);
        printf("# Phase            = %d  \n", pDiagConfSub[l].Phase);
        printf("# CycleTime        = %d  \n", pDiagConfSub[l].CycleTime);

        printf("# StatNo           = %d  \n", pDiagConfSub[l].StatNo);
        printf("# Slot             = %d  \n", pDiagConfSub[l].Slot);
        printf("# SubSlot          = 0x%x\n", pDiagConfSub[l].Subslot);
      }
      printf("##########################\n");

      break;
    }
    case PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC: {
      PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC_DATA *DiagDeviceState;

      DiagDeviceState = (PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC_DATA *)
                            pCbfPrm->CtrlDiagConf.pDiagDataBuffer;

      // Big Endian!
      PND_SWAP_U16(&DiagDeviceState->ErrorCause);

      printf("#########################\n");
      printf("Device Mode             : %s\n",
             DiagDeviceState->Mode ? "Active" : "Inactive");
      printf("Error Cause             : 0x%04x\n", DiagDeviceState->ErrorCause);
      printf("Error Cause Description : %s\n",
             translate(ErrorCause, DiagDeviceState->ErrorCause));
      printf("Reason Code             : 0x%02x\n", DiagDeviceState->ReasonCode);
      printf("Reason Code Description : %s\n",
             translate(ReasonCode, DiagDeviceState->ReasonCode));
      printf("OrigPNIOStatus          : |0x%02x|0x%02x|0x%02x|0x%02x|\n",
             DiagDeviceState->AdditionalInfo[0],
             DiagDeviceState->AdditionalInfo[1],
             DiagDeviceState->AdditionalInfo[2],
             DiagDeviceState->AdditionalInfo[3]);
      switch (DiagDeviceState->ErrorCause) {
        case 1:  // CM_ERROR_CAUSE_IP_MULTIPLE
        case 2:  // CM_ERROR_CAUSE_NOS_MULTIPLE
        case 8:  // CM_ERROR_CAUSE_IP_SET_ABORTED
          // Big Endian!
          printf("MAC Address             : %02x:%02x:%02x:%02x:%02x:%02x\n",
                 DiagDeviceState->AdditionalInfo[9],
                 DiagDeviceState->AdditionalInfo[8],
                 DiagDeviceState->AdditionalInfo[7],
                 DiagDeviceState->AdditionalInfo[6],
                 DiagDeviceState->AdditionalInfo[5],
                 DiagDeviceState->AdditionalInfo[4]);
          break;
        case 5:  // CM_ERROR_CAUSE_RPC_ERROR
          // Big Endian Conversion!
          PND_SWAP_U32((PNIO_UINT32 *)&DiagDeviceState->AdditionalInfo[4]);

          printf("NCA Fault Status        : 0x%08x\n",
                 (PNIO_UINT32)DiagDeviceState->AdditionalInfo[4]);
          break;
        case 14:  // CM_ERROR_CAUSE_IOD_ABORTED
          // Big Endian Conversion!
          PND_SWAP_U32((PNIO_UINT32 *)&DiagDeviceState->AdditionalInfo[4]);

          printf("Remote PNIO Status      : 0x%08x\n",
                 (PNIO_UINT32)DiagDeviceState->AdditionalInfo[4]);
          break;
      }
      printf("#########################\n");

      break;
    }
    case PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO: {
      PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO_DATA *DiagConfigNameAddrInfo;

      DiagConfigNameAddrInfo = (PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO_DATA *)
                                   pCbfPrm->CtrlDiagConf.pDiagDataBuffer;

      printf("#########################\n");
      printf("Device Name   : %s\n", DiagConfigNameAddrInfo->name);
      printf("Device Type   : %s\n", DiagConfigNameAddrInfo->TypeOfStation);
      printf("IP Addr       : %d.%d.%d.%d\n",
             (DiagConfigNameAddrInfo->ip_addr >> 24 & 0xFF),
             (DiagConfigNameAddrInfo->ip_addr >> 16 & 0xFF),
             (DiagConfigNameAddrInfo->ip_addr >> 8 & 0xFF),
             (DiagConfigNameAddrInfo->ip_addr & 0xFF));
      printf("IP Mask       : %d.%d.%d.%d\n",
             (DiagConfigNameAddrInfo->ip_mask >> 24 & 0xFF),
             (DiagConfigNameAddrInfo->ip_mask >> 16 & 0xFF),
             (DiagConfigNameAddrInfo->ip_mask >> 8 & 0xFF),
             (DiagConfigNameAddrInfo->ip_mask & 0xFF));
      printf("Default Router: %d.%d.%d.%d\n",
             (DiagConfigNameAddrInfo->default_router >> 24 & 0xFF),
             (DiagConfigNameAddrInfo->default_router >> 16 & 0xFF),
             (DiagConfigNameAddrInfo->default_router >> 8 & 0xFF),
             (DiagConfigNameAddrInfo->default_router & 0xFF));
      printf("#########################\n");

      break;
    }

    default: {
      printf("Diagnostic Request Ref: %d\n",
             pCbfPrm->CtrlDiagConf.pDiagData->ReqRef);

      break;
    }
  }
}

void callback_for_ifc_set_ip_and_nos(PNIO_CBE_PRM *pCbfPrm) {
  PNIO_UINT8 Mode;

#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  wait_for_con();
#endif
  printf("\n-----------------------------------------------------");
  printf("\nCallback_for_ifc_set_ip_and_nos function is called!!!\n\n");
  printf("\n-----------------------------------------------------");

  Mode = pCbfPrm->SetAddrConf.Mode;

  if ((Mode & PNIO_SET_IP_MODE) == PNIO_SET_IP_MODE) {
    if (pCbfPrm->SetAddrConf.Err == PNIO_OK) {
      printf("\n##############################\n");
      printf("IP Address      : %d.%d.%d.%d\n",
             (pCbfPrm->SetAddrConf.LocalIPAddress[0]),
             (pCbfPrm->SetAddrConf.LocalIPAddress[1]),
             (pCbfPrm->SetAddrConf.LocalIPAddress[2]),
             (pCbfPrm->SetAddrConf.LocalIPAddress[3]));
      printf("Gateway Address : %d.%d.%d.%d\n",
             (pCbfPrm->SetAddrConf.DefaultRouterAddr[0]),
             (pCbfPrm->SetAddrConf.DefaultRouterAddr[1]),
             (pCbfPrm->SetAddrConf.DefaultRouterAddr[2]),
             (pCbfPrm->SetAddrConf.DefaultRouterAddr[3]));
      printf("Netmask         : %d.%d.%d.%d\n",
             (pCbfPrm->SetAddrConf.LocalSubnetMask[0]),
             (pCbfPrm->SetAddrConf.LocalSubnetMask[1]),
             (pCbfPrm->SetAddrConf.LocalSubnetMask[2]),
             (pCbfPrm->SetAddrConf.LocalSubnetMask[3]));
      printf("##############################\n");
    } else {
      printf("\nSet IP Suite failure!!!\n");
      printf("Response: 0x%x\n", pCbfPrm->SetAddrConf.Err);
      return;
    }
  }

  if ((Mode & PNIO_SET_NOS_MODE) == PNIO_SET_NOS_MODE) {
    if (pCbfPrm->SetAddrConf.Err == PNIO_OK) {
      printf("\n###########################\n");
      printf("Name of Station : %s\n",
             static_cast<char const *>(pCbfPrm->SetAddrConf.pStationName));
      printf("###########################\n");
    } else {
      printf("\nSet Name Of Station failure!!!\n");
      printf("Response: 0x%x\n", pCbfPrm->SetAddrConf.Err);
    }
  }
}

void callback_for_ifc_rema_read(PNIO_CBE_PRM *pCbfPrm) {
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  wait_for_con();
#endif

  printf("\ncallback_for_ifc_rema_read is called!!\n\n");

  FILE *hXMLFile = fopen("rema.xml", "w+");

  fwrite(pCbfPrm->RemaReadConf.RemaXMLBuffer, 1,
         pCbfPrm->RemaReadConf.RemaXMLBufferLength, (FILE *)hXMLFile);

  fflush(hXMLFile);
  fclose(hXMLFile);
}

void callback_for_iosystem_reconfig(PNIO_CBE_PRM *pCbfPrm) {
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  wait_for_con();
#endif

  printf("\n\nDevice Activation \\ Deactivation CBF of IOSystem Reconfig: \n");

  if (pCbfPrm->IoSystemReconf.Status != PNIO_OK) {
    printf("Device Activation/Deactivation failed!\n");
    return;
  }

  switch (pCbfPrm->IoSystemReconf.Mode) {
    case PNIO_DA_TRUE:
      printf("All Configured Devices have been activated! \n");
      break;
    case PNIO_DA_FALSE:
      printf("All Devices have been deactivated! \n");
      break;
  };
}

void callback_for_ifc_record_read_conf(PNIO_CBE_PRM *pCbfPrm) {
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
  wait_for_con();
#endif

  if (pCbfPrm->RecReadConf.Err.AddValue1 != PNIO_OK) {
    printf("\nInterface record read error! ErrorCode = 0x%x\n",
           pCbfPrm->RecReadConf.Err.AddValue1);
    return;
  }
  switch (pCbfPrm->RecReadConf.RecordIndex) {
    case 0x0000B081:  // PDIOSystemInfo
    {
      print_pdiosysteminfo(pCbfPrm->RecReadConf.pBuffer);

      break;
    }

    default:
      printf("Data Record Read Ref: %ld\n", pCbfPrm->RecReadConf.ReqRef);
      break;
  }
}

PNIO_UINT32 pnd_test_dev_activate(PNIO_DEV_ACT_TYPE activate_mode) {
  PNIO_ADDR Addr;
  PNIO_UINT32 result = PNIO_OK;

  Addr.AddrType = PNIO_ADDR_LOG;
  Addr.IODataType = PNIO_IO_OUT;

  printf("\n ----------------------------------------");
  printf("\n Any logical address of the device : ");
  scanf("%d", (int *)&Addr.u.Addr);

  // a logical address has no direction (don't care) but we set it to a valid
  // value
  Addr.IODataType = PNIO_IO_IN;

  result = PNIO_device_activate(g_ApplHandle, &Addr, activate_mode);

  if (result != PNIO_OK) {
    printf("PNIO_device_activate returned with error: %d \n", (int)result);
  }

  return result;
}

void pnd_test_set_mode(PNIO_MODE_TYPE mode) {
  if (!(g_ApplHandle == PND_INVALID_HANDLE)) {
    PNIO_set_mode(g_ApplHandle, mode);
  } else {
    printf("set_mode : INVALID_HANDLE \n");
  }
}

PNIO_UINT32 pnd_test_IM_data_read() {
  PNIO_REF ReqRef = 0;
  PNIO_UINT32 Length = 0;

  PNIO_ADDR Addr;
  PNIO_UINT32 result = PNIO_OK;

  Addr.AddrType = PNIO_ADDR_LOG;
  Addr.IODataType = PNIO_IO_IN;

  printf("\n\n\n");
  printf("----------------------------------------");
  printf("\n Logical address : ");
  scanf("%d", (int *)&Addr.u.Addr);

  Length = 64;

  int cmd = 777;

  while (cmd != 5) {
    printf("\n\n");
    printf("0...I&M Data 0              ( Index 0xAFF0 )\n");
    printf("1...Function/ Location Tag  ( Index 0xAFF1 )\n");
    printf("2...Build in date           ( Index 0xAFF2 )\n");
    printf("3...Descriptor              ( Index 0xAFF3 )\n");
    printf("4...Signatur                ( Index 0xAFF4 )\n");
    printf("============================================\n");
    printf("5..Quit This\n\n");

    scanf("%d", &cmd);

    switch (cmd) {
      case 0: {
        result = PNIO_rec_read_req(g_ApplHandle, &Addr, 0xAFF0, ReqRef, Length);
        break;
      }
      case 1: {
        result = PNIO_rec_read_req(g_ApplHandle, &Addr, 0xAFF1, ReqRef, Length);
        break;
      }
      case 2: {
        result = PNIO_rec_read_req(g_ApplHandle, &Addr, 0xAFF2, ReqRef, Length);
        break;
      }
      case 3: {
        result = PNIO_rec_read_req(g_ApplHandle, &Addr, 0xAFF3, ReqRef, Length);
        break;
      }
      case 4: {
        result = PNIO_rec_read_req(g_ApplHandle, &Addr, 0xAFF4, ReqRef, Length);
        break;
      }
    }
  }

  return result;
}

PNIO_UINT32 pnd_test_IM_data_write() {
  PNIO_REF ReqRef = 1;
  PNIO_UINT32 Length = 0;

  char c_tmp[60];
  int i = 0;

  PNIO_ADDR Addr;
  PNIO_UINT32 result = PNIO_OK;

  Addr.AddrType = PNIO_ADDR_LOG;
  Addr.IODataType = PNIO_IO_OUT;

  printf("\n ----------------------------------------");
  printf("\n Logical address : ");
  scanf("%d", (int *)&Addr.u.Addr);

  Length = 60;

  int cmd = 777;

  while (cmd != 5) {
    printf("\n\n");
    printf("1...Function/ Location Tag  ( Index 0xAFF1 )\n");
    printf("2...Build in date           ( Index 0xAFF2 )\n");
    printf("3...Descriptor              ( Index 0xAFF3 )\n");
    printf("4...Signature               ( Index 0xAFF4 )\n");
    printf("============================================\n");
    printf("5...Quit this\n\n");

    scanf("%d", &cmd);

    switch (cmd) {
      case 1: {
        PNIO_IM1_TYPE im_data;
        PNIO_UINT32 RecordIndex = 0xAFF1;

        im_data.BlockHeader.BlockLength = 0x3800;
        im_data.BlockHeader.BlockType = 0x2100;
        im_data.BlockHeader.BlockVersionHigh = 0x01;
        im_data.BlockHeader.BlockVersionLow = 0x00;

        // Function tag
        printf("\n\tPlease enter function tag (max 32 characters): ");
        get_string_from_stdio(c_tmp, 33);
        for (i = 0; i < 32; i++) im_data.IM_Tag_Function[i] = c_tmp[i];
        fill_buffer_with_blank_char(im_data.IM_Tag_Function,
                                    sizeof(im_data.IM_Tag_Function));

        // Location tag
        printf("\n\tPlease enter the location tag (max 22 characters): ");
        get_string_from_stdio(c_tmp, 23);
        for (i = 0; i < 22; i++) im_data.IM_Tag_Location[i] = c_tmp[i];
        fill_buffer_with_blank_char(im_data.IM_Tag_Location,
                                    sizeof(im_data.IM_Tag_Location));

        result = PNIO_rec_write_req(g_ApplHandle, &Addr, RecordIndex, ReqRef,
                                    Length, (PNIO_UINT8 *)&im_data);

        break;
      }
      case 2: {
        PNIO_IM2_TYPE im_data;
        PNIO_UINT32 RecordIndex = 0xAFF2;

        im_data.BlockHeader.BlockLength = 0x1200;
        im_data.BlockHeader.BlockType = 0x2200;
        im_data.BlockHeader.BlockVersionHigh = 0x01;
        im_data.BlockHeader.BlockVersionLow = 0x00;

        // Datum input
        printf(
            "\n\tPlease enter build in date: (YEAR-MONTH-DAY: 2014-12-31): ");
        get_string_from_stdio(c_tmp, 17);
        for (i = 0; i < 16; i++) im_data.IM_Date[i] = c_tmp[i];
        fill_buffer_with_blank_char(im_data.IM_Date, sizeof(im_data.IM_Date));

        result = PNIO_rec_write_req(g_ApplHandle, &Addr, RecordIndex, ReqRef,
                                    22, (PNIO_UINT8 *)&im_data);

        break;
      }
      case 3: {
        PNIO_IM3_TYPE im_data;
        PNIO_UINT32 RecordIndex = 0xAFF3;

        im_data.BlockHeader.BlockLength = 0x3800;
        im_data.BlockHeader.BlockType = 0x2300;
        im_data.BlockHeader.BlockVersionHigh = 0x01;
        im_data.BlockHeader.BlockVersionLow = 0x00;

        // Descriptor input
        printf("\n\tPlease enter descriptor (max 54 characters): ");
        get_string_from_stdio(c_tmp, 55);
        for (i = 0; i < 54; i++) im_data.IM_Descriptor[i] = c_tmp[i];
        fill_buffer_with_blank_char(im_data.IM_Descriptor,
                                    sizeof(im_data.IM_Descriptor));

        result = PNIO_rec_write_req(g_ApplHandle, &Addr, RecordIndex, ReqRef,
                                    Length, (PNIO_UINT8 *)&im_data);

        break;
      }
      case 4: {
        PNIO_IM4_TYPE im_data;
        PNIO_UINT32 RecordIndex = 0xAFF4;

        im_data.BlockHeader.BlockLength = 0x3800;
        im_data.BlockHeader.BlockType = 0x2300;
        im_data.BlockHeader.BlockVersionHigh = 0x01;
        im_data.BlockHeader.BlockVersionLow = 0x00;

        // Descriptor eingabe
        printf("\n\tPlease enter new signature (max 54): ");
        get_string_from_stdio(c_tmp, 55);
        for (i = 0; i < 54; i++) im_data.IM_Signature[i] = c_tmp[i];
        fill_buffer_with_blank_char(im_data.IM_Signature,
                                    sizeof(im_data.IM_Signature));

        result = PNIO_rec_write_req(g_ApplHandle, &Addr, RecordIndex, ReqRef,
                                    Length, (PNIO_UINT8 *)&im_data);

        break;
      }

      default:
        break;
    }

    printf("\n ================ \n");
  }

  return result;
}

void pnd_test_controller_open(PNIO_DEBUG_SETTINGS_PTR_TYPE DebugSettings) {
  PNIO_UINT32 result;

  LSA_UNUSED_ARG(DebugSettings);

  result = PNIO_controller_open(
      1, PNIO_CEP_MODE_CTRL, callback_for_ds_read_conf,
      callback_for_ds_write_conf, callback_for_alarm_ind, &g_ApplHandle);
  if (result != PNIO_OK) {
    printf("PNIO_controller_open returned error: %d\n", (int)result);
  }
}

void pnd_test_controller_close(void) {
  PNIO_UINT32 result;
  result = PNIO_controller_close(g_ApplHandle);
  if (result != PNIO_OK) {
    printf("pnd_test_controller_close returned Error No.: %d \n", (int)result);
  }
}

void pnd_test_trig_diag_req(void) {
  PNIO_CTRL_DIAG *pDiagReq = (PNIO_CTRL_DIAG *)malloc(sizeof(PNIO_CTRL_DIAG));

  pDiagReq->ReqRef = 0;
  PNIO_ADDR Addr;

  printf("\n\n\n");
  printf("----------------------------------------");
  printf("\n Which diagnostic service would you like to request? \n");
  printf("----------------------------------------");

  int cmd = 777;
  PNIO_UINT32 result;

  while (cmd != 4) {
    printf("\n\n");
    printf("============================================\n");
    printf("1...PNIO_CTRL_DIAG_CONFIG_SUBMODULE_LIST	\n");
    printf("2...PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO	\n");
    printf("3...PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC	    \n");
    printf("============================================\n");
    printf("4...Quit This                               \n");
    printf("============================================\n\n");

    scanf("%d", &cmd);

    if (cmd >= 4)  // quit this!
    {
      break;
    }

    switch (cmd) {
      case 1:  // PNIO_CTRL_DIAG_CONFIG_SUBMODULE_LIST:
        pDiagReq->DiagService = PNIO_CTRL_DIAG_CONFIG_SUBMODULE_LIST;
        break;

      case 2:  // PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO:
        pDiagReq->DiagService = PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO;
        break;

      case 3:  // PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC:
        pDiagReq->DiagService = PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC;

        printf("\n ----------------------------------------");
        printf("\n Logical address :                      \n");

        scanf("%d", (PNIO_UINT32 *)&Addr.u.Addr);

        pDiagReq->u.Addr.u.Addr = Addr.u.Addr;
        pDiagReq->u.Addr.AddrType = PNIO_ADDR_LOG;
        pDiagReq->u.Addr.IODataType = PNIO_IO_IN;
        break;
    }

    result = PNIO_ctrl_diag_req(g_ApplHandle,
                                pDiagReq);  // TODO: return error values are
                                            // defined in PGH_IO-Base_76.pdf, bu
                                            // errrorlari da bildir.

    printf("Diagnostic returned result: 0x%x\n", result);
  }

  free(pDiagReq);
}

void pnd_test_iosystem_reconfig(void) {
  PNIO_IOS_RECONFIG_MODE Mode;
  PNIO_UINT32 DeviceCnt = 0;
  PNIO_ADDR *DeviceList = PNIO_NULL;
  PNIO_UINT32 PortInterconnectionCnt = 0;
  PNIO_ADDR *PortInterconnectionList = PNIO_NULL;
  PNIO_UINT32 result;

  unsigned int cmd = 777;

  printf("\n\n\n");
  printf("--------------------------------------------------\n");
  printf("Which step would you like to choose?               \n");
  printf("--------------------------------------------------\n");

  printf("\n");
  printf("=================================\n");
  printf("1...PNIO_IOS_RECONFIG_MODE_DEACT \n");
  printf("2...PNIO_IOS_RECONFIG_MODE_TAILOR\n");
  printf("=================================\n");
  printf("3...Quit This                    \n");
  printf("=================================\n\n");

  scanf("%d", &cmd);

  if (cmd >= 3 || cmd <= 0) {
    return;
  }

  Mode = static_cast<PNIO_IOS_RECONFIG_MODE>(cmd);

  switch (Mode) {
    case PNIO_IOS_RECONFIG_MODE_DEACT:  // Step-1
    {
      // No need to do anything,
      // This step only performs deactivation to the devices

      break;
    }
    case PNIO_IOS_RECONFIG_MODE_TAILOR:  // Step-2
    {
      int count = 0;
      int laddr = 0;

      printf("\n");
      printf(
          "--------------------------------------------------------------------"
          "----\n");
      printf(
          "Please type the number of the optional IODevices in new "
          "configuration.    \n");
      printf(
          "--------------------------------------------------------------------"
          "----\n");

      printf("Device Count: ");
      scanf("%d", &count);
      DeviceCnt = count;

      if (DeviceCnt != 0) {
        DeviceList = (PNIO_ADDR *)malloc(DeviceCnt * sizeof(PNIO_ADDR));
        if (DeviceList == NULL) {
          printf("DeviceList allocation failure!\n");
          return;
        }

        printf("\n");
        printf(
            "------------------------------------------------------------------"
            "----------    \n");
        printf(
            "Please type logical addresses of the optional IODevices in new "
            "configuration.     \n");
        printf(
            "------------------------------------------------------------------"
            "----------    \n");

        for (PNIO_UINT16 i = 0; i < DeviceCnt; i++) {
          printf("\nDevice #%02d: ", i + 1);
          scanf("%d", &laddr);
          DeviceList[i].AddrType = PNIO_ADDR_LOG;
          DeviceList[i].IODataType = PNIO_IO_IN;
          DeviceList[i].u.Addr = laddr;
        }
      }

      printf("\n");
      printf(
          "--------------------------------------------------------------------"
          "----\n");
      printf(
          "Please type the number of entries in the PortInterconnectionList.   "
          "      \n");
      printf(
          "--------------------------------------------------------------------"
          "----\n");

      printf("PortInterconnectionCnt: ");
      scanf("%d", &count);
      PortInterconnectionCnt = count;

      if (PortInterconnectionCnt != 0) {
        PortInterconnectionList =
            (PNIO_ADDR *)malloc(2 * PortInterconnectionCnt * sizeof(PNIO_ADDR));
        if (PortInterconnectionList == NULL) {
          printf("PortInterconnectionList allocation failure!\n");
          return;
        }

        printf("\n");
        printf(
            "------------------------------------------------------------------"
            "------\n");
        printf(
            "Please type logical addresses of the PortInterconnectionList "
            "pairs.       \n");
        printf(
            "------------------------------------------------------------------"
            "------\n");
      }

      for (PNIO_UINT16 i = 0; i < (2 * PortInterconnectionCnt); i = i + 2) {
        printf("\nPortInterconnectionList #%2d,1: ", (i / 2) + 1);
        scanf("%d", &laddr);
        PortInterconnectionList[i].AddrType = PNIO_ADDR_LOG;
        PortInterconnectionList[i].IODataType = PNIO_IO_IN;
        PortInterconnectionList[i].u.Addr = laddr;
        printf("\nPortInterconnectionList #%2d,2: ", (i / 2) + 1);
        scanf("%d", &laddr);
        PortInterconnectionList[i + 1].AddrType = PNIO_ADDR_LOG;
        PortInterconnectionList[i + 1].IODataType = PNIO_IO_IN;
        PortInterconnectionList[i + 1].u.Addr = laddr;
      }

      break;
    }
  }

  result =
      PNIO_iosystem_reconfig(g_ApplHandle, Mode, DeviceCnt, DeviceList,
                             PortInterconnectionCnt, PortInterconnectionList);

  printf("PNIO_iosystem_reconfig returned result: 0x%x\n", result);

  if (Mode == PNIO_IOS_RECONFIG_MODE_TAILOR) {
    if (DeviceList != PNIO_NULL) free(DeviceList);

    if (PortInterconnectionList != PNIO_NULL) free(PortInterconnectionList);
  }
}

void pnd_test_register_setmode_cbf(void) {
  PNIO_register_cbf(g_ApplHandle, PNIO_CBE_MODE_IND,
                    callback_for_mode_change_indication);
}

void pnd_test_register_devact_cbf(void) {
  PNIO_register_cbf(g_ApplHandle, PNIO_CBE_DEV_ACT_CONF,
                    callback_for_device_activation);
}

void pnd_test_register_diag_req_cbf(void) {
  PNIO_register_cbf(g_ApplHandle, PNIO_CBE_CTRL_DIAG_CONF,
                    callback_for_diagnostics_request);
}

void pnd_test_register_iosystem_reconfig(void) {
  PNIO_register_cbf(g_ApplHandle, PNIO_CBE_IOSYSTEM_RECONFIG,
                    callback_for_iosystem_reconfig);
}

void pnd_test_interface_open(PNIO_DEBUG_SETTINGS_PTR_TYPE DebugSettings) {
  PNIO_UINT32 result;

  LSA_UNUSED_ARG(DebugSettings);

  result = PNIO_interface_open(1, callback_for_ifc_record_read_conf,
                               callback_for_alarm_ind, &g_ApplHandle);
  if (result != PNIO_OK) {
    printf("PNIO_interface_open returned error: %d\n", (int)result);
  }
}

void pnd_test_interface_close(void) {
  PNIO_UINT32 result;
  result = PNIO_interface_close(g_ApplHandle);
  if (result != PNIO_OK) {
    printf("pnd_test_controller_close returned Error No.: %d \n", (int)result);
  }
}

void pnd_test_register_interface_set_ip_and_nos(void) {
  PNIO_interface_register_cbf(g_ApplHandle, PNIO_CBE_IFC_SET_ADDR_CONF,
                              callback_for_ifc_set_ip_and_nos);
}

void pnd_test_register_interface_rema_read(void) {
  PNIO_interface_register_cbf(g_ApplHandle, PNIO_CBE_REMA_READ_CONF,
                              callback_for_ifc_rema_read);
}

PNIO_UINT32 pnd_test_interface_data_read() {
  PNIO_REF ReqRef = 0;
  PNIO_UINT32 Length = 0;

  PNIO_ADDR Addr;
  PNIO_UINT32 result = PNIO_OK;

  Addr.AddrType = PNIO_ADDR_LOG;
  Addr.IODataType = PNIO_IO_IN;

  printf("\n\n\n");
  printf("----------------------------------------");
  printf("\n Logical address : ");
  scanf("%d", (int *)&Addr.u.Addr);

  Length = 266;

  int cmd = 777;

  while (cmd != 1) {
    printf("\n\n");
    printf("0...PDIOSystem Info         ( Index 0x0000B081 )\n");
    printf("================================================\n");
    printf("1..Quit This\n\n");

    scanf("%d", &cmd);

    switch (cmd) {
      case 0: {
        result = PNIO_interface_rec_read_req(g_ApplHandle, &Addr, 0x0000B081,
                                             ReqRef, Length);
        break;
      }
    }
  }

  return result;
}

void pnd_test_interface_set_ip_and_nos(void) {
  PNIO_UINT8 Mode;
  PNIO_IPv4 IpV4;
  PNIO_NOS NoS;

  printf("\n\n\n");
  printf("--------------------------------------------------\n");
  printf("Which mode would you like to set?                   \n");
  printf("--------------------------------------------------\n");

  unsigned int cmd = 777;
  PNIO_UINT32 result;

  while (cmd != 4) {
    printf("\n");
    printf("===========================\n");
    printf("1...IPv4 (IPv4 Suite)	   \n");
    printf("2...NoS  (Name Of Station) \n");
    printf("3...IPv4 & NoS             \n");
    printf("===========================\n");
    printf("4...Quit This              \n");
    printf("===========================\n\n");

    scanf("%d", &cmd);

    if (cmd >= 4) {
      break;
    }

    memset(NoS.Nos, 0, 256);
    memset(IpV4.IpAddress, 0, 4);
    memset(IpV4.Gateway, 0, 4);
    memset(IpV4.NetMask, 0, 4);

    Mode = 0;

    if (cmd == 1 || cmd == 3) {
      int dump_address[4];
      int dump_remanent;

      printf("\n");
      printf(
          "--------------------------------------------------------------------"
          "----\n");
      printf(
          "Please type IpAddress, Gateway, NetMask and Remanent state "
          "respectively.  \n");
      printf(
          "--------------------------------------------------------------------"
          "----\n");
      // be careful about scanf! %d -> decimal integer which is 4 bytes!
      printf("Ip Address     : ");
      scanf("%d%d%d%d", &dump_address[0], &dump_address[1], &dump_address[2],
            &dump_address[3]);
      for (PNIO_UINT8 i = 0; i < 4; i++)
        IpV4.IpAddress[i] = static_cast<PNIO_UINT8>(dump_address[i]);

      printf("\nGateway Address: ");
      scanf("%d%d%d%d", &dump_address[0], &dump_address[1], &dump_address[2],
            &dump_address[3]);
      for (PNIO_UINT8 i = 0; i < 4; i++)
        IpV4.Gateway[i] = static_cast<PNIO_UINT8>(dump_address[i]);

      printf("\nNetmask        : ");
      scanf("%d%d%d%d", &dump_address[0], &dump_address[1], &dump_address[2],
            &dump_address[3]);
      for (PNIO_UINT8 i = 0; i < 4; i++)
        IpV4.NetMask[i] = static_cast<PNIO_UINT8>(dump_address[i]);

      printf("\nRemanent State : ");
      scanf("%d", &dump_remanent);
      IpV4.Remanent = static_cast<PNIO_BOOL>(dump_remanent);

      Mode = PNIO_SET_IP_MODE;
    }

    if (cmd == 2 || cmd == 3) {
      int dump_remanent;
      char string[256];
      PNIO_UINT16 length;

      printf("\n");
      printf("------------------------------------------------\n");
      printf("Please type Name of Station and Remanent state.  \n");
      printf("------------------------------------------------\n");

      printf("Name of Station : ");
      scanf("%s", string);

      length = (PNIO_UINT16)pnd_strlen(string);

      if (length >= 256) {
        printf("Name of station size overflow!");
        break;
      }

      memcpy(&NoS.Nos[0], string, length);

      NoS.Length = length;

      printf("\nRemanent State  : ");
      scanf("%d", &dump_remanent);
      NoS.Remanent = static_cast<PNIO_BOOL>(dump_remanent);

      Mode |= PNIO_SET_NOS_MODE;
    }

    result = PNIO_interface_set_ip_and_nos(
        g_ApplHandle, (PNIO_SET_IP_NOS_MODE_TYPE)Mode, IpV4, NoS);

    printf("PNIO_interface_set_ip_and_nos, Result: %#x\n", result);
  }
}

/*****************************************************************************/
/*  Copyright (C) 2015 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
