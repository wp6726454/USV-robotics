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
/*  F i l e               &F: diagnosis_helper.cpp                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: V1.1.0                                    :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2015-11-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*****************************************************************************/
#include "diagnosis_helper.h"
#include "device.h"
#include "submodule_data.h"
#include "address_info_data.h"
#include "diagnosis_info_data.h"
#include "im_data.h"
#include "helper_functions.h"
#include "pnd_xml_std_interface.h"

#define ERROR_CAUSE_DESC \
{ 0  /* CM_ERROR_CAUSE_NONE */                      , "No problem, everything OK" }, \
{ 1  /* CM_ERROR_CAUSE_IP_MULTIPLE */               , "IP address exists multiple times" }, \
{ 2  /* CM_ERROR_CAUSE_NOS_MULTIPLE */              , "NameOfStation exists multiple times"}, \
{ 3  /* CM_ERROR_CAUSE_LINK_DOWN */                 , "LINK Down"}, \
{ 4  /* CM_ERROR_CAUSE_MAU_MISMATCH */              , "MAUType mismatch"}, \
{ 5  /* CM_ERROR_CAUSE_RPC_ERROR */                 , "RPC protocol error"}, \
{ 6  /* CM_ERROR_CAUSE_PNIO_CONFIGURATION */        , "PNIO configuration protocol error"}, \
{ 7  /* CM_ERROR_CAUSE_RPC_TIMEOUT */               , "RPC application timeout"}, \
{ 8  /* CM_ERROR_CAUSE_IP_SET_ABORTED */            , "IP address assignment to IO device aborted"}, \
{ 9  /* CM_ERROR_CAUSE_IOD_NOT_FOUND */             , "IO device not found"}, \
{ 10 /* CM_ERROR_CAUSE_OUT_OF_RESOURCES */          , "Out of AR resources"}, \
{ 11 /* CM_ERROR_CAUSE_MISS */                      , "WDT / DHT expired"}, \
{ 12 /* CM_ERROR_CAUSE_NOS_SET_ABORTED */           , "Device with wrong NameOfStation found on topological position"}, \
{ 13 /* CM_ERROR_CAUSE_IOC_ABORTED */               , "Controller local abort reason"}, \
{ 14 /* CM_ERROR_CAUSE_IOD_ABORTED */               , "Device abort reason"}, \
{ 15 /* CM_ERROR_CAUSE_SRD_ABORTED */               , "SystemRedundancy abort reason"}, \
{ 16 /* CM_ERROR_CAUSE_VENDORID_ABORTED */          , "Device with wrong VendorID / DeviceID / Instance found on topological position"}, \
{ 17 /* CM_ERROR_CAUSE_MP_IN_PROGRESS */            , "MasterProject: Machine- or AddressTailoring in progress"}, \
{ 18 /* CM_ERROR_CAUSE_MP_NOS_MISSING */            , "MasterProject: NameOfStation adaption missing (PDev with invalid NoS)"}, \
{ 19 /* CM_ERROR_CAUSE_MP_IPv4_MISSING */           , "MasterProject: IPv4Suite adaption missing (PDev with invalid IPv4Suite)"}, \
{ 20 /* CM_ERROR_CAUSE_MP_NOS_INVALID */            , "MasterProject: tailored NameOfStation of IOD is invalid"}, \
{ 21 /* CM_ERROR_CAUSE_MP_IPv4_INVALID */           , "MasterProject: tailored IPv4Suite of IOD is invalid"}, \
{ 22 /* CM_ERROR_CAUSE_MP_TAILORING_MISSING */      , "MasterProject: MachineTailoring missing"}, \
{ 23 /* CM_ERROR_CAUSE_MP_PDEV_ERROR */             , "MasterProject: PDev configuration error"}, \
{ 24 /* CM_ERROR_CAUSE_MP_DEVICE_DISABLED */        , "MasterProject: disabled Device is activated"}, \
{ 25 /* CM_ERROR_CAUSE_MP_CONFIG_INCOMPLETE */      , "MasterProject: configuration incomplete"} 

#define REASON_CODE_DESC \
{ 0  /* CM_AR_REASON_NONE */                        , "CM_AR_REASON_NONE" }, \
{ 1  /* CM_AR_REASON_1 */                           , "Sequence numbers do not match (no longer used in versions >= V3.9)" }, \
{ 2  /* CM_AR_REASON_2 */                           , "Alarm instance closed (no longer used in versions >= V3.6)"}, \
{ 3  /* CM_AR_REASON_MEM */                         , "Out of memory"}, \
{ 4  /* CM_AR_REASON_FRAME */                       , "Adding provider or consumer failed"}, \
{ 5  /* CM_AR_REASON_MISS */                        , "Miss (consumer)"}, \
{ 6  /* CM_AR_REASON_TIMER */                       , "Cmi timeout"}, \
{ 7  /* CM_AR_REASON_ALARM */                       , "Alarm-open failed"}, \
{ 8  /* CM_AR_REASON_ALSND */                       , "Alarm-send.cnf(-)"}, \
{ 9  /* CM_AR_REASON_ALACK */                       , "Alarm-ack-send.cnf(-)"}, \
{ 10 /* CM_AR_REASON_ALLEN */                       , "Alarm-data too long"}, \
{ 11 /* CM_AR_REASON_ASRT */                        , "Alarm.ind(err)"}, \
{ 12 /* CM_AR_REASON_RPC */                         , "Rpc-client call.cnf(-)"}, \
{ 13 /* CM_AR_REASON_ABORT */                       , "Ar-abort.req"}, \
{ 14 /* CM_AR_REASON_RERUN */                       , "Rerun aborts existing"}, \
{ 15 /* CM_AR_REASON_REL */                         , "Got release.ind"}, \
{ 16 /* CM_AR_REASON_PAS */                         , "Device passivated"}, \
{ 17 /* CM_AR_REASON_RMV */                         , "Device / AR removed"}, \
{ 18 /* CM_AR_REASON_PROT */                        , "Protocol violation"}, \
{ 19 /* CM_AR_REASON_NARE */                        , "NARE error"}, \
{ 20 /* CM_AR_REASON_BIND */                        , "RPC-Bind error"}, \
{ 21 /* CM_AR_REASON_CONNECT */                     , "RPC-Connect error"}, \
{ 22 /* CM_AR_REASON_READ */                        , "RPC-Read error"}, \
{ 23 /* CM_AR_REASON_WRITE */                       , "RPC-Write error"}, \
{ 24 /* CM_AR_REASON_CONTROL */                     , "RPC-Control error"}, \
{ 25 /* CM_AR_REASON_25 */                          , "Reserved (formerly: pull or plug in forbidden window)"}, \
{ 26 /* CM_AR_REASON_26 */                          , "Reserved (formerly: AP removed)"}, \
{ 27 /* CM_AR_REASON_LNK_DOWN */                    , "Link down, for local purpose only"}, \
{ 28 /* CM_AR_REASON_MMAC */                        , "Could not register multicast-mac "}, \
{ 29 /* CM_AR_REASON_SYNC */                        , "Not synchronized (cannot start companion-ar)"}, \
{ 30 /* CM_AR_REASON_TOPO */                        , "Wrong topology (cannot start companion-ar)"}, \
{ 31 /* CM_AR_REASON_DCP_NAME */                    , "Dcp, station-name changed"}, \
{ 32 /* CM_AR_REASON_DCP_RESET */                   , "Dcp, reset to factory or factory reset"}, \
{ 33 /* CM_AR_REASON_33 */                          , "Reserved (formerly: cannot start companion-ar)"}, \
{ 34 /* CM_AR_REASON_IRDATA */                      , "No irdata record yet"}, \
{ 35 /* CM_AR_REASON_PDEV */                        , "Ownership of physical device"}, \
{ 36 /* CM_AR_REASON_LNK_MODE */                    , "Link mode not full duplex"}, \
{ 37 /* CM_AR_REASON_IPSUITE */                     , "IP-Suite [of the IOC] changed by means of DCP_set(IPParameter) or local engineering"}, \
{ 38 /* CM_AR_REASON_RDHT */                        , "IOCARSR RDHT expired"}, \
{ 39 /* CM_AR_REASON_PDEV_PRM */                    , "IOCARSR PDev, parametrization impossible"}, \
{ 40 /* CM_AR_REASON_ARDY */                        , "Remote application timeout expired"}

typedef struct {
	PNIO_UINT16 value;
	const char* desc;
}TRANSLATE_TYPE;

TRANSLATE_TYPE ErrorCause[] = { ERROR_CAUSE_DESC };
TRANSLATE_TYPE ReasonCode[] = { REASON_CODE_DESC };

const char* translate(TRANSLATE_TYPE* type, PNIO_UINT16 value)
{
	return type[value].desc;
}


DiagnosisHelper::DiagnosisHelper(void)
{
    m_errorCode = PNIO_OK;
}


DiagnosisHelper::~DiagnosisHelper(void)
{}

void DiagnosisHelper::extractDevices(const PNIO_UINT8* pBuffer, PNIO_UINT32 bufferLen)
{
    PNIO_CTRL_DIAG_CONFIG_SUBMODULE *pDiagConfSub;
    PNIO_UINT32 submodule_cnt;
    Device *device = PNIO_NULL;
    SubmoduleData *submoduleData;

    pDiagConfSub = (PNIO_CTRL_DIAG_CONFIG_SUBMODULE *)pBuffer;
    submodule_cnt = (bufferLen) / sizeof (PNIO_CTRL_DIAG_CONFIG_SUBMODULE);

    for(PNIO_UINT32 i = 0; i<submodule_cnt; i++)
    {
        if( getDevice(m_deviceMap, pDiagConfSub[i].StatNo) == PNIO_NULL )
        {
            device = new Device(pDiagConfSub[i].StatNo);

            addNode( m_deviceMap, pDiagConfSub[i].StatNo, device );
        }
        
        submoduleData = new SubmoduleData();

        submoduleData->setAddress(pDiagConfSub[i].Address);
        submoduleData->setDataLength(pDiagConfSub[i].DataLength);
        submoduleData->setDataType(pDiagConfSub[i].DataType);
        submoduleData->setComType(pDiagConfSub[i].ComType);
        submoduleData->setApi(pDiagConfSub[i].Api);
        submoduleData->setReductionFactor(pDiagConfSub[i].ReductionFactor);
        submoduleData->setPhase(pDiagConfSub[i].Phase);
        submoduleData->setCycleTime(pDiagConfSub[i].CycleTime);
        submoduleData->setStatNo(pDiagConfSub[i].StatNo);
        submoduleData->setSlot(pDiagConfSub[i].Slot);
        submoduleData->setSubslot(pDiagConfSub[i].Subslot);

        device->getSubmoduleList().push_back(submoduleData);
    }
}

void DiagnosisHelper::extractSubmodules(SubmoduleList& submoduleList, const PNIO_UINT8* pBuffer, PNIO_UINT32 bufferLen)
{
    PNIO_CTRL_DIAG_CONFIG_SUBMODULE *pDiagConfSub;
    PNIO_UINT32 submodule_cnt;
    SubmoduleData *submoduleData;

    pDiagConfSub = (PNIO_CTRL_DIAG_CONFIG_SUBMODULE *)pBuffer;
    submodule_cnt = (bufferLen) / sizeof (PNIO_CTRL_DIAG_CONFIG_SUBMODULE);

    for(PNIO_UINT32 i = 0; i<submodule_cnt; i++)
    {
        submoduleData = new SubmoduleData();

        submoduleData->setAddress(pDiagConfSub[i].Address);
        submoduleData->setDataLength(pDiagConfSub[i].DataLength);
        submoduleData->setDataType(pDiagConfSub[i].DataType);
        submoduleData->setComType(pDiagConfSub[i].ComType);
        submoduleData->setApi(pDiagConfSub[i].Api);
        submoduleData->setReductionFactor(pDiagConfSub[i].ReductionFactor);
        submoduleData->setPhase(pDiagConfSub[i].Phase);
        submoduleData->setCycleTime(pDiagConfSub[i].CycleTime);
        submoduleData->setStatNo(pDiagConfSub[i].StatNo);
        submoduleData->setSlot(pDiagConfSub[i].Slot);
        submoduleData->setSubslot(pDiagConfSub[i].Subslot);

        submoduleList.push_back(submoduleData);
    }
}

void DiagnosisHelper::extractAddressInfo(const PNIO_UINT8* pBuffer)
{
    PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO_DATA *pDiagAddrInfoData;
    
    pDiagAddrInfoData = (PNIO_CTRL_DIAG_CONFIG_NAME_ADDR_INFO_DATA *)pBuffer;
    
    m_addressInfoData.setName(pDiagAddrInfoData->name);
    m_addressInfoData.setTypeOfStation(pDiagAddrInfoData->TypeOfStation);
    m_addressInfoData.setIpAddr(pDiagAddrInfoData->ip_addr);
    m_addressInfoData.setIpMask(pDiagAddrInfoData->ip_mask);
    m_addressInfoData.setDefaultRouter(pDiagAddrInfoData->default_router);

}

void DiagnosisHelper::extractDiagnosisInfo(const PNIO_UINT8* pBuffer)
{
    PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC_DATA *pDiagDeviceData;
    
    pDiagDeviceData = (PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC_DATA *)pBuffer;

    PNIO_UINT16 errorCause = pDiagDeviceData->ErrorCause;
    SWAP_U16(&errorCause);

    m_diagnosisInfoData.setMode(pDiagDeviceData->Mode);
    m_diagnosisInfoData.setErrorCause(errorCause);
    m_diagnosisInfoData.setReasonCode(pDiagDeviceData->ReasonCode);
    m_diagnosisInfoData.setAdditionalInfo(pDiagDeviceData->AdditionalInfo);

}

void DiagnosisHelper::extractIMData(const PNIO_UINT8* pBuffer, IM_INDEX_TYPE imType)
{
    switch(imType)
    {
        case IM0_TYPE:
            PNIO_IM0_TYPE *im0;

            im0 = (PNIO_IM0_TYPE*)pBuffer;

            PNIO_UINT16 supportedIM;
            PNIO_UINT16 hwRevision;
            PNIO_UINT16 profileSpecificType;

            supportedIM = im0->imSupported;
            hwRevision = im0->imHardwareRevision;
            profileSpecificType = im0->imProfileSpecificType;

            // Big Endian!
            SWAP_U16(&hwRevision);
            SWAP_U16(&profileSpecificType);
            SWAP_U16(&supportedIM);

            m_iMData.setVendorId((im0->vendorIdHigh<<8) + im0->endorIdLow);
            m_iMData.setHWRevision(hwRevision);
            m_iMData.setOrderId(im0->orderID);
            m_iMData.setProfileSpecificType(profileSpecificType);
            m_iMData.setSerialNumber(im0->imSerialNumber);
            m_iMData.setSupportedIm(supportedIM);
            break;
        case IM1_TYPE:
            PNIO_IM1_TYPE *im1;
        
            im1 = (PNIO_IM1_TYPE*)pBuffer;

            m_iMData.setIMFunction(im1->imTagFunction);
            m_iMData.setIMLocation(im1->imTagLocation);
            break;
        case IM2_TYPE:
            PNIO_IM2_TYPE *im2;
        
            im2 = (PNIO_IM2_TYPE*)pBuffer;

            m_iMData.setIMDate(im2->imDate);
            break;
        case IM3_TYPE:
            PNIO_IM3_TYPE *im3;
        
            im3 = (PNIO_IM3_TYPE*)pBuffer;

            m_iMData.setIMDescriptor(im3->imDescriptor);
            break;
        case IM4_TYPE:
            PNIO_IM4_TYPE *im4;
        
            im4= (PNIO_IM4_TYPE*)pBuffer;

            m_iMData.setIMSignature(im4->imSignature);
            break;
        }
}

void DiagnosisHelper::printDevice()
{
    for (DeviceMap::iterator it=m_deviceMap.begin(); it!=m_deviceMap.end(); ++it)
    {
        PNIO_UINT16 deviceNr = it->first;
        Device* device = it->second;
        
        IMData* pIMData = device->getIMData();
        SubmoduleVector submoduleList = device->getSubmoduleList();
        DiagnosisInfoData* pDiagnosisInfoData = device->getDiagnosisInfoData();

        PNIO_UINT8 orderId[20];
        PNIO_UINT8 serialNr[16];
        PNIO_UINT8 imFunction[32];
        PNIO_UINT8 imLocation[22];
        PNIO_UINT8 imDate[16];
        PNIO_UINT8 imDescriptor[54];
        PNIO_UINT8 imSignature[54];
        PNIO_UINT16 supportedIm = pIMData->getSupportedIm();
        PNIO_BOOL anyImSupported = PNIO_FALSE;
        pIMData->getOrderId(orderId);
        pIMData->getSerialNumber(serialNr);
        pIMData->getIMFunction(imFunction);
        pIMData->getIMLocation(imLocation);
        pIMData->getIMDate(imDate);
        pIMData->getIMDescriptor(imDescriptor);
        pIMData->getIMSignature(imSignature);
        
        printf("\n************************************************************\n");
        printf("IODevice DeviceNr: %d\n", deviceNr);
        printf("     VendorID: %d\n", pIMData->getVendorId());
        printf("     OrderID: %.18s\n", orderId);
        printf("     HWRevision: %d\n", pIMData->getHwRevision());
        printf("     SerialNumber: %.14s\n", serialNr);
        printf("     ProfileSpecificType: %d\n", pIMData->getProfileSpecificType());
        printf("     Supported IM: ");
        for(PNIO_UINT32 index=1; index<=sizeof(IM_INDEX_TYPE); index++)
        {
            if(supportedIm & (0x01<<index))
            {
                printf("I&M%d ", index);
                anyImSupported = PNIO_TRUE;
            }
        }
        if(anyImSupported == PNIO_FALSE)
        {
            printf("-");
        }
        printf("\n");
        printf("     IMFunction: %.32s\n", supportedIm & (0x01<<1) ? (char*)imFunction : "N/A");
        printf("     IMLocation: %.22s\n", supportedIm & (0x01<<1) ? (char*)imLocation : "N/A");
        printf("     IMDate: %.16s\n", supportedIm & (0x01<<2) ? (char*)imDate : "N/A");
        printf("     IMDescriptor: %.54s\n", supportedIm & (0x01<<3) ? (char*)imDescriptor : "N/A");
        printf("     IMSignature: %.54s\n", supportedIm & (0x01<<4) ? (char*)imSignature : "N/A");
        printf("     Diagnosis Info: \n");
        printf("             Reason Code        : %s\n", translate(ReasonCode, m_diagnosisInfoData.getReasonCode() ) );
        printf("             Error Cause        : %s\n", translate( ErrorCause, pDiagnosisInfoData->getErrorCause() ) );
        printf("     Configured Modules:\n");

        
        for(PNIO_UINT32 i = 0; i<submoduleList.size(); i++)
        {
            SubmoduleData* submoduleData = submoduleList.at(i);

            printf("             Slot: %#x\n", submoduleData->getSlot());
            printf("                      Subslot: %#x\n", submoduleData->getSubslot());
            printf("                                DataLen:         %02d\n", submoduleData->getDataLength());
            printf("                                DataType:        %02d\n", submoduleData->getDataType());
            printf("                                ReductionFactor: %02d\n", submoduleData->getReductionFactor());
            printf("                                Phase:           %02d\n", submoduleData->getPhase());
            printf("                                CycleTime:       %02d\n", submoduleData->getCyleTime());
        }

    }

    printf("************************************************************\n");

    freeDeviceMap();
}

void DiagnosisHelper::printSubmoduleList()
{
    SubmoduleData *submoduleData;

    for (DeviceMap::iterator it=m_deviceMap.begin(); it!=m_deviceMap.end(); ++it)
    {
        Device* device = it->second;

        SubmoduleVector submoduleList = device->getSubmoduleList();

        for(PNIO_UINT32 i = 0; i < submoduleList.size(); i++)
        {

            submoduleData = submoduleList.at(i);

            printf("LogAdr: %02d | API: %02d | DeviceNr: %02d | Slot: %02d | Subslot:%#x | DataLen: %02d | DataType: %02d | ComType: %02d | ReductionFactor: %02d | Phase: %02d | CycleTime: %02d\n\n",
                submoduleData->getAddress().u.Addr,
                submoduleData->getApi(),
                submoduleData->getStatNo(),
                submoduleData->getSlot(),
                submoduleData->getSubslot(),
                submoduleData->getDataLength(),
                submoduleData->getDataType(),
                submoduleData->getComType(),
                submoduleData->getReductionFactor(),
                submoduleData->getPhase(),
                submoduleData->getCyleTime());
        }
    }

    freeDeviceMap();
}

void DiagnosisHelper::printAddressInfo()
{
    PNIO_UINT8 name[256];
    PNIO_UINT8 typeOfStation[256];
    PNIO_UINT32 ipAddr;
    PNIO_UINT32 ipMask;
    PNIO_UINT32 defaultRouter;

    m_addressInfoData.getName(name);
    m_addressInfoData.getTypeOfStation(typeOfStation);
    ipAddr = m_addressInfoData.getIpAddr();
    ipMask = m_addressInfoData.getIpMask();
    defaultRouter = m_addressInfoData.getDefaultRouter();

	printf("\nIIOInterface:\n");
    printf("      Name:                     %s\n\n", name);
    printf("      IP Parameters:            %d.%d.%d.%d\n",   
                                                    (ipAddr>>24 & 0xFF),
				                                    (ipAddr>>16 & 0xFF),
				                                    (ipAddr>>8 & 0xFF),
				                                    (ipAddr & 0xFF));
    printf("      IP Parameters:            %d.%d.%d.%d\n",   
                                                    (ipMask>>24 & 0xFF),
				                                    (ipMask>>16 & 0xFF),
				                                    (ipMask>>8 & 0xFF),
				                                    (ipMask & 0xFF));
    printf("      IP Parameters:            %d.%d.%d.%d\n\n",   
                                                    (defaultRouter>>24 & 0xFF),
				                                    (defaultRouter>>16 & 0xFF),
				                                    (defaultRouter>>8 & 0xFF),
				                                    (defaultRouter & 0xFF));
    printf("      Type of Station:          %s\n\n", typeOfStation);
}

void DiagnosisHelper::printDiagnosisInfo()
{
    PNIO_UINT8 additionalInfo[10];
    m_diagnosisInfoData.getAdditionalInfo(additionalInfo);
    printf("\n");
    printf("Device Mode             : %s\n", m_diagnosisInfoData.getMode() ? "Active" : "Inactive");
	printf("Error Cause             : %s\n", translate(ErrorCause, m_diagnosisInfoData.getErrorCause() ) ); 
	printf("Reason Code             : %s\n", translate(ReasonCode, m_diagnosisInfoData.getReasonCode() ) );
	printf("OrigPNIOStatus          : |0x%02x|0x%02x|0x%02x|0x%02x|\n", additionalInfo[0]
					                                                  , additionalInfo[1]
																      , additionalInfo[2]
																	  , additionalInfo[3]);
	switch(m_diagnosisInfoData.getErrorCause())
	{
		case 1: // CM_ERROR_CAUSE_IP_MULTIPLE
		case 2: // CM_ERROR_CAUSE_NOS_MULTIPLE
		case 8: // CM_ERROR_CAUSE_IP_SET_ABORTED
			// Big Endian!
			printf("MAC Address             : %02x:%02x:%02x:%02x:%02x:%02x\n", 
					additionalInfo[9], additionalInfo[8]
				  , additionalInfo[7], additionalInfo[6]
				  , additionalInfo[5], additionalInfo[4]);
			break;
		case 5: // CM_ERROR_CAUSE_RPC_ERROR
			// Big Endian Conversion!
			SWAP_U32((PNIO_UINT32 *)&additionalInfo[4]);

			printf("NCA Fault Status        : 0x%08x\n", (PNIO_UINT32)additionalInfo[4]);
			break;
		case 14: // CM_ERROR_CAUSE_IOD_ABORTED
			// Big Endian Conversion!
			SWAP_U32((PNIO_UINT32 *)&additionalInfo[4]);

			printf("Remote PNIO Status      : 0x%08x\n", additionalInfo[4]);
			break;
	}
}

PNIO_UINT32 DiagnosisHelper::makeDiagnosisRequest(PNIO_UINT32 handle, PNIO_CTRL_DIAG_ENUM diag_service, PNIO_UINT16 diag_wait_event, PNIO_UINT32 laddr)
{
    PNIO_CTRL_DIAG * pDiagReq = (PNIO_CTRL_DIAG *) malloc( sizeof(PNIO_CTRL_DIAG) );
	PNIO_UINT32 result;

    // Create Event
    allocateEvent(diag_wait_event);

    resetEvent(diag_wait_event);

    pDiagReq->ReqRef = 0;
    pDiagReq->DiagService = diag_service;

    if(pDiagReq->DiagService == PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC)
    {
        pDiagReq->u.Addr.u.Addr = laddr;
	    pDiagReq->u.Addr.AddrType = PNIO_ADDR_LOG;
	    pDiagReq->u.Addr.IODataType = PNIO_IO_IN;
    }

    result = PNIO_ctrl_diag_req(handle, pDiagReq);

    free(pDiagReq);

    if(result == PNIO_OK)
    {
        //wait
        waitEvent(diag_wait_event);
    }

    // Free event
    freeEvent(diag_wait_event);

    return result;
}

PNIO_UINT32 DiagnosisHelper::getDeviceConfAndDiag(PNIO_UINT32 handle, PNIO_UINT16 diag_wait_event, PNIO_UINT16 rec_read_wait_event)
{
    PNIO_UINT32 result;
    PNIO_REF reqRef = 0;
    PNIO_UINT32 length = 64;
        
    result = makeDiagnosisRequest(handle, PNIO_CTRL_DIAG_CONFIG_SUBMODULE_LIST, diag_wait_event);

    if(result != PNIO_OK)
    {
        return result;
    }

    for (DeviceMap::iterator it=m_deviceMap.begin(); it!=m_deviceMap.end(); ++it)
    {
        Device* device = it->second;
        PNIO_ADDR laddr;
        
        // if no submodule exists, no laddr is provided.
        if(device->getSubmoduleList().size() > 0)
        {
            SubmoduleData *submoduleData = device->getSubmoduleList().at(0);

            laddr = submoduleData->getAddress();

            result = makeDiagnosisRequest(handle, PNIO_CTRL_DIAG_DEVICE_DIAGNOSTIC, diag_wait_event, laddr.u.Addr);
        
            if(result != PNIO_OK)
            {
                freeDeviceMap();
                return result;
            }

            device->setDiagnosisInfoData(m_diagnosisInfoData);

            // Create Event
            allocateEvent(rec_read_wait_event);

            for(PNIO_UINT32 index = IM0_TYPE; index <= IM4_TYPE; index++)
            {
                if(index == IM0_TYPE || m_iMData.getSupportedIm() & (0x01<<(index-IM0_TYPE)))
                {
                    //reset
                    resetEvent(rec_read_wait_event);

                    result = PNIO_rec_read_req(handle, &laddr, index, reqRef, length);

                    if(result != PNIO_OK)
                    {
                        freeEvent(rec_read_wait_event);
                        freeDeviceMap();
                        return result;
                    }

                    //wait
                    waitEvent(rec_read_wait_event);

                    result = m_errorCode;
                    if(result != PNIO_OK)
                    {
                        freeEvent(rec_read_wait_event);
                        freeDeviceMap();
                        return result;
                    }
                }
            }          

            //free
            freeEvent(rec_read_wait_event);

            device->setIMData(m_iMData);
        }
        
    }
    
    return result;
}

void DiagnosisHelper::setErrorCode(PNIO_UINT32 errorCode)
{
    m_errorCode = errorCode;
}

PNIO_UINT32 DiagnosisHelper::getErrorCode()
{
    return m_errorCode;
}

void DiagnosisHelper::freeDeviceMap()
{
    for (DeviceMap::iterator it=m_deviceMap.begin(); it!=m_deviceMap.end(); ++it)
    {
        Device* device = it->second;

        delete device;
    }

    m_deviceMap.clear();
}

void DiagnosisHelper::addNode(DeviceMap& map, PNIO_UINT16 device_nr, Device* device)
{
    DeviceMap::iterator m_it;

	m_it = map.find( device_nr );

	if( m_it == map.end())  // in MAP ?
	{
	    map[device_nr] = device;
	}
}

Device* DiagnosisHelper::getDevice(DeviceMap& map, PNIO_UINT16 device_nr)
{
    Device *device = PNIO_NULL;

	if ( !map.empty()) //empty map ?
	{
		DeviceMap::iterator mapIter;
        
		mapIter = map.find( device_nr );

		if ( mapIter != map.end() )  // in MAP ?
		{
			device = mapIter->second;
		}
	}

	return ( device );
}

void DiagnosisHelper::emptyMap(DeviceMap& map)
{
    map.clear();
}