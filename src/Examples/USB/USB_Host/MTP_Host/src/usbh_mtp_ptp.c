/*!
    \file  usbh_mtp_ptp.c
    \brief this file includes the PTP operations layer

    \version 2016-08-15, V1.0.0, firmware for GD32F4xx
    \version 2018-12-12, V2.0.0, firmware for GD32F4xx
*/

/*
    Copyright (c) 2018, GigaDevice Semiconductor Inc.

    All rights reserved.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "usbh_mtp_ptp.h"
#include "usbh_mtp.h"
#include "usbh_ctrl.h"

static void ptp_decodedeviceinfo (usbh_host_struct *phost, PTP_DeviceInfoTypedef *dev_info);
static void ptp_getstorageids (usbh_host_struct *phost, PTP_StorageIDsTypedef *stor_ids);
static void ptp_getstorageinfo (usbh_host_struct *phost, uint32_t storage_id, PTP_StorageInfoTypedef *stor_info);
static void ptp_getobjectpropdesc (usbh_host_struct *phost, PTP_ObjectPropDescTypeDef *opd, uint32_t opdlen);
static void ptp_decodedeviceinfo (usbh_host_struct *phost, PTP_DeviceInfoTypedef *dev_info);
static void ptp_getdevicepropvalue(usbh_host_struct *phost, uint32_t *offset, uint32_t total, 
                                            PTP_PropertyValueTypedef* value, uint16_t datatype);
static uint32_t ptp_getobjectproplist (usbh_host_struct *phost, mtp_propertiestypedef *props, uint32_t len);
static void ptp_bufferfullcallback(usbh_host_struct *phost);
static void ptp_getstring(uint8_t *str, uint8_t* data, uint16_t *len);
static uint32_t ptp_getarray16 (uint16_t *array, uint8_t *data, uint32_t offset);
static uint32_t ptp_getarray32 (uint32_t *array, uint8_t *data, uint32_t offset);

/*!
    \brief      the function Initializes the PTP protocol
    \param[in]  phost: USB host handler
    \param[out] none
    \retval     USB host operation status
*/
usbh_status_enum usbh_ptp_init(usbh_host_struct *phost)
{
    /* Set state to idle to be ready for operations */
    mtp_handle.ptp.state = PTP_IDLE;
    mtp_handle.ptp.req_state = PTP_REQ_SEND;

    return USBH_OK;
}

/*!
    \brief      the function handle the PTP protocol
	\param[in]  pudev: USB core handler
    \param[in]  phost: USB host handler
    \param[out] none
    \retval     USB host operation status
*/
usbh_status_enum usbh_ptp_process (usb_core_handle_struct *pudev ,usbh_host_struct *phost)
{
    usbh_status_enum   status = USBH_BUSY;
    urb_state_enum URB_Status = URB_IDLE;  
    ptp_containertypedef  ptp_container;  
    uint32_t  len;

    switch (mtp_handle.ptp.state) {
        case PTP_IDLE:
            /*Do Nothing */
            break;

        case PTP_OP_REQUEST_STATE:
            usbh_xfer (pudev, (uint8_t*)&(mtp_handle.ptp.op_container), 
                       mtp_handle.dataoutpipe, mtp_handle.ptp.op_container.length);

            mtp_handle.ptp.state = PTP_OP_REQUEST_WAIT_STATE;
            break;

        case PTP_OP_REQUEST_WAIT_STATE:
            URB_Status = hcd_urb_state_get(pudev, mtp_handle.dataoutpipe);

            if (URB_Status == URB_DONE) {
                if (mtp_handle.ptp.flags == PTP_DP_NODATA) {
                    mtp_handle.ptp.state = PTP_RESPONSE_STATE;
                } else if (mtp_handle.ptp.flags == PTP_DP_SENDDATA) {
                    mtp_handle.ptp.state = PTP_DATA_OUT_PHASE_STATE;
                } else if (mtp_handle.ptp.flags == PTP_DP_GETDATA) {
                    mtp_handle.ptp.state = PTP_DATA_IN_PHASE_STATE;
                }
                #if (USBH_USE_OS == 1)
                osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
                #endif
            } else if (URB_Status == URB_NOTREADY) {
                /* Resend Request */
                mtp_handle.ptp.state = PTP_OP_REQUEST_STATE;
                #if (USBH_USE_OS == 1)
                osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
                #endif
            } else if (URB_Status == URB_STALL) {
                mtp_handle.ptp.state  = PTP_ERROR;
                #if (USBH_USE_OS == 1)
                osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
                #endif 
            }
            break;

        case PTP_DATA_OUT_PHASE_STATE:
            usbh_xfer (pudev, 
                     mtp_handle.ptp.data_ptr, 
                     mtp_handle.dataoutpipe,
                     mtp_handle.dataoutepsize);

            mtp_handle.ptp.state  = PTP_DATA_OUT_PHASE_WAIT_STATE;    
            break;

        case PTP_DATA_OUT_PHASE_WAIT_STATE:
            URB_Status = hcd_urb_state_get(pudev, mtp_handle.dataoutpipe);     

            if (URB_Status == URB_DONE) {
                /* Adjust Data pointer and data length */
                if (mtp_handle.ptp.data_length > mtp_handle.dataoutepsize) {
                    mtp_handle.ptp.data_ptr += mtp_handle.dataoutepsize;
                    mtp_handle.ptp.data_length -= mtp_handle.dataoutepsize; 
                    mtp_handle.ptp.data_packet += mtp_handle.dataoutepsize;

                    if (mtp_handle.ptp.data_packet >= PTP_USB_BULK_PAYLOAD_LEN_READ) {
                        ptp_bufferfullcallback (phost);
                        mtp_handle.ptp.data_packet = 0;
                        mtp_handle.ptp.iteration++;            
                    }
                } else {
                    mtp_handle.ptp.data_length = 0;
                } 

                /* More Data To be Sent */
                if (mtp_handle.ptp.data_length > 0) {
                    usbh_xfer (pudev, 
                             mtp_handle.ptp.data_ptr, 
                             mtp_handle.dataoutpipe,
                             mtp_handle.dataoutepsize);
                } else {
                    /* If value was 0, and successful transfer, then change the state */
                    mtp_handle.ptp.state  = PTP_RESPONSE_STATE;
                }
                #if (USBH_USE_OS == 1)
                osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
                #endif       
            } else if (URB_Status == URB_NOTREADY) {
                /* Resend same data */      
                mtp_handle.ptp.state = PTP_DATA_OUT_PHASE_STATE;
                #if (USBH_USE_OS == 1)
                osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
                #endif       
            } else if (URB_Status == URB_STALL) {
                mtp_handle.ptp.state  = PTP_ERROR;    
                #if (USBH_USE_OS == 1)
                osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
                #endif       
            }
            break;

        case PTP_DATA_IN_PHASE_STATE:
            usbh_xfer (pudev, 
                     mtp_handle.ptp.data_ptr, 
                     mtp_handle.datainpipe,
                     mtp_handle.datainepsize);

            mtp_handle.ptp.state  = PTP_DATA_IN_PHASE_WAIT_STATE;    
            break;

        case PTP_DATA_IN_PHASE_WAIT_STATE:
            URB_Status = hcd_urb_state_get(pudev, mtp_handle.datainpipe);

            if (URB_Status == URB_DONE) {
                len = hcd_xfer_count_get(pudev, mtp_handle.datainpipe);

                if (mtp_handle.ptp.data_packet_counter++ == 0) {
                    /* This is the first packet; so retrieve exact data length from payload */ 
                    mtp_handle.ptp.data_length = *(uint32_t*)(mtp_handle.ptp.data_ptr);
                    mtp_handle.ptp.iteration = 0;
                }

                if ((len >=  mtp_handle.datainepsize) && (mtp_handle.ptp.data_length > 0)) {
                    mtp_handle.ptp.data_ptr += len;
                    mtp_handle.ptp.data_length -= len;
                    mtp_handle.ptp.data_packet += len;

                    if (mtp_handle.ptp.data_packet >= PTP_USB_BULK_PAYLOAD_LEN_READ) {
                        ptp_bufferfullcallback (phost);
                        mtp_handle.ptp.data_packet = 0;
                        mtp_handle.ptp.iteration++;            
                    }

                    /* Continue receiving data*/        
                    usbh_xfer (pudev,
                       mtp_handle.ptp.data_ptr, 
                       mtp_handle.datainpipe,
                       mtp_handle.datainepsize);
                } else {
                    mtp_handle.ptp.data_length -= len;
                    mtp_handle.ptp.state = PTP_RESPONSE_STATE;
                    #if (USBH_USE_OS == 1)
                    osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
                    #endif          
                }
            } else if (URB_Status == URB_STALL) {
                mtp_handle.ptp.state  = PTP_ERROR;
                #if (USBH_USE_OS == 1)
                osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
                #endif       
            }
            break;

        case PTP_RESPONSE_STATE:
            usbh_xfer (pudev,
                   (uint8_t*)&(mtp_handle.ptp.resp_container), 
                   mtp_handle.datainpipe,
                   PTP_USB_BULK_REQ_RESP_MAX_LEN );

            mtp_handle.ptp.state  = PTP_RESPONSE_WAIT_STATE;
            break;

        case PTP_RESPONSE_WAIT_STATE:
            URB_Status = hcd_urb_state_get(pudev, mtp_handle.datainpipe);

            if (URB_Status == URB_DONE) {
                usbh_ptp_getresponse (phost, &ptp_container);

                if (ptp_container.Code == PTP_RC_OK) {
                    status = USBH_OK;
                } else {
                    status = USBH_FAIL;
                }
                mtp_handle.ptp.req_state = PTP_REQ_SEND;
            } else if (URB_Status == URB_STALL) {
                mtp_handle.ptp.state  = PTP_ERROR;
                #if (USBH_USE_OS == 1)
                osMessagePut ( phost->os_event, USBH_URB_EVENT, 0);
                #endif
            }
            break;

        case PTP_ERROR:
            mtp_handle.ptp.req_state = PTP_REQ_SEND;
            break;

        default:
            break;
    }

    return status;
}

/*!
    \brief      send PTP request
    \param[in]  phost: USB host handler
    \param[in]  req: PTP request
    \param[out] none
    \retval     USB host operation status
*/
usbh_status_enum usbh_ptp_sendrequest (usbh_host_struct *phost, ptp_containertypedef  *req)
{
    usbh_status_enum  status = USBH_OK; 

    /* Clear PTP Data container*/
    USBH_memset(&(mtp_handle.ptp.op_container), 0, sizeof(PTP_OpContainerTypedef));

    /* build appropriate USB container */
    mtp_handle.ptp.op_container.length = PTP_USB_BULK_REQ_LEN- (sizeof(uint32_t)*(5-req->Nparam));
    mtp_handle.ptp.op_container.type = PTP_USB_CONTAINER_COMMAND;
    mtp_handle.ptp.op_container.code = req->Code;
    mtp_handle.ptp.op_container.trans_id = req->Transaction_ID;
    mtp_handle.ptp.op_container.param1 = req->Param1;
    mtp_handle.ptp.op_container.param2 = req->Param2;
    mtp_handle.ptp.op_container.param3 = req->Param3;
    mtp_handle.ptp.op_container.param4 = req->Param4;
    mtp_handle.ptp.op_container.param5 = req->Param5;

    return status;
}

/*!
    \brief      get PTP response
    \param[in]  phost: USB host handler
    \param[in]  resp: PTP response
    \param[out] none
    \retval     USB host operation status
*/
usbh_status_enum usbh_ptp_getresponse (usbh_host_struct *phost, ptp_containertypedef  *resp)
{
    usbh_status_enum  status = USBH_OK; 

    /* build an appropriate PTPContainer */
    resp->Code = mtp_handle.ptp.resp_container.code;
    resp->SessionID = mtp_handle.ptp.session_id;
    resp->Transaction_ID = mtp_handle.ptp.resp_container.trans_id;
    resp->Param1 = mtp_handle.ptp.resp_container.param1;
    resp->Param2 = mtp_handle.ptp.resp_container.param2;
    resp->Param3 = mtp_handle.ptp.resp_container.param3;
    resp->Param4 = mtp_handle.ptp.resp_container.param4;
    resp->Param5 = mtp_handle.ptp.resp_container.param5;

    return status;
}

/*!
    \brief      informs user that data buffer is full
    \param[in]  phost: USB host handler
    \param[out] none
    \retval     none
*/
void ptp_bufferfullcallback(usbh_host_struct *phost)
{
    switch (mtp_handle.ptp.data_container.code) {
        case PTP_OC_GetDeviceInfo:
            ptp_decodedeviceinfo (phost, &(mtp_handle.info.devinfo));
            break;

        case PTP_OC_GetPartialObject:    
        case PTP_OC_GetObject:
            /* first packet is in the PTP data payload buffer */
            if (mtp_handle.ptp.iteration == 0) {
                /* copy it to object */
                USBH_MEMCPY(mtp_handle.ptp.object_ptr, mtp_handle.ptp.data_container.payload.data, PTP_USB_BULK_PAYLOAD_LEN_READ);

                /* next packet should be directly copied to object */
                mtp_handle.ptp.data_ptr = (mtp_handle.ptp.object_ptr + PTP_USB_BULK_PAYLOAD_LEN_READ); 
            }
            break;

        case PTP_OC_SendObject:    
            /* first packet is in the PTP data payload buffer */
            if(mtp_handle.ptp.iteration == 0)
            {
            /* next packet should be directly copied to object */
            mtp_handle.ptp.data_ptr = (mtp_handle.ptp.object_ptr + PTP_USB_BULK_PAYLOAD_LEN_READ); 
            }
            break;    

        default:
            break;
    }
}

/*!
    \brief      gets device info dataset and fills device info structure
    \param[in]  phost: USB host handler
	\param[in]  dev_info: device info structure
    \param[out] none
    \retval     none
*/
static void ptp_decodedeviceinfo (usbh_host_struct *phost, PTP_DeviceInfoTypedef *dev_info)
{
    uint8_t *data = mtp_handle.ptp.data_container.payload.data;
    uint32_t totallen; 
    uint16_t len;

    /* max device info is PTP_USB_BULK_HS_MAX_PACKET_LEN_READ */
    lcd_log_print((uint8_t *)(" MTP device info size exceeds internal buffer size.Only available data are decoded."), 0, LCD_COLOR_BLACK);
               
    if (mtp_handle.ptp.iteration == 0) {
        dev_info->StandardVersion = LE16(&data[PTP_di_StandardVersion]);
        dev_info->VendorExtensionID = LE32(&data[PTP_di_VendorExtensionID]);
        dev_info->VendorExtensionVersion = LE16(&data[PTP_di_VendorExtensionVersion]);
        ptp_getstring(dev_info->VendorExtensionDesc, &data[PTP_di_VendorExtensionDesc], &len);

        totallen=len*2+1;
        dev_info->FunctionalMode = LE16(&data[PTP_di_FunctionalMode+totallen]);
        dev_info->OperationsSupported_len = ptp_getarray16 ((uint16_t *)&dev_info->OperationsSupported, 
                                                      data, 
                                                      PTP_di_OperationsSupported+totallen);

        totallen=totallen+dev_info->OperationsSupported_len*sizeof(uint16_t)+sizeof(uint32_t);
        dev_info->EventsSupported_len = ptp_getarray16 ((uint16_t *)&dev_info->EventsSupported, 
                                                      data, 
                                                      PTP_di_OperationsSupported+totallen);

        totallen=totallen+dev_info->EventsSupported_len*sizeof(uint16_t)+sizeof(uint32_t);
        dev_info->DevicePropertiesSupported_len = ptp_getarray16 ((uint16_t *)&dev_info->DevicePropertiesSupported, 
                                                      data, 
                                                      PTP_di_OperationsSupported+totallen);

        totallen=totallen+dev_info->DevicePropertiesSupported_len*sizeof(uint16_t)+sizeof(uint32_t);

        dev_info->CaptureFormats_len = ptp_getarray16 ((uint16_t *)&dev_info->CaptureFormats, 
                                                      data, 
                                                      PTP_di_OperationsSupported+totallen);

        totallen=totallen+dev_info->CaptureFormats_len*sizeof(uint16_t)+sizeof(uint32_t);
        dev_info->ImageFormats_len =  ptp_getarray16 ((uint16_t *)&dev_info->ImageFormats, 
                                                      data, 
                                                      PTP_di_OperationsSupported+totallen);

        totallen=totallen+dev_info->ImageFormats_len*sizeof(uint16_t)+sizeof(uint32_t); 
        ptp_getstring(dev_info->Manufacturer, &data[PTP_di_OperationsSupported+totallen], &len);

        totallen+=len*2+1;
        ptp_getstring(dev_info->Model, &data[PTP_di_OperationsSupported+totallen], &len);

        totallen+=len*2+1;
        ptp_getstring(dev_info->DeviceVersion, &data[PTP_di_OperationsSupported+totallen], &len);

        totallen+=len*2+1;
        ptp_getstring(dev_info->SerialNumber, &data[PTP_di_OperationsSupported+totallen], &len);
    }
}

/*!
    \brief      gets storage IDs and fills stor_ids structure.
    \param[in]  phost: USB host handler
	\param[in]  stor_ids: storage ID structure
    \param[out] none
    \retval     none
*/
static void ptp_getstorageids (usbh_host_struct *phost, PTP_StorageIDsTypedef *stor_ids)
{
    uint8_t *data = mtp_handle.ptp.data_container.payload.data;

    stor_ids->n = ptp_getarray32 (stor_ids->Storage, data, 0); 
}

/*!
    \brief      gets storage info and fills stor_info structure.
    \param[in]  phost: USB host handler
	\param[in]  stor_ids: storage info structure
    \param[out] none
    \retval     none
*/
static void ptp_getstorageinfo (usbh_host_struct *phost, uint32_t storage_id, PTP_StorageInfoTypedef *stor_info)
{
    uint8_t *data = mtp_handle.ptp.data_container.payload.data;

    uint16_t len;

    stor_info->StorageType=LE16(&data[PTP_si_StorageType]);
    stor_info->FilesystemType=LE16(&data[PTP_si_FilesystemType]);
    stor_info->AccessCapability=LE16(&data[PTP_si_AccessCapability]);
    stor_info->MaxCapability=LE64(&data[PTP_si_MaxCapability]);
    stor_info->FreeSpaceInBytes=LE64(&data[PTP_si_FreeSpaceInBytes]);
    stor_info->FreeSpaceInImages=LE32(&data[PTP_si_FreeSpaceInImages]);

    ptp_getstring(stor_info->StorageDescription, &data[PTP_si_StorageDescription], &len);
    ptp_getstring(stor_info->VolumeLabel, &data[PTP_si_StorageDescription+len*2+1], &len);   
}

/*!
    \brief      gets object info and fills object_info structure.
    \param[in]  phost: USB host handler
	\param[in]  object_info: object info structure
    \param[out] none
    \retval     none
*/
static void ptp_getobjectinfo (usbh_host_struct *phost, ptp_objectinfotypedef *object_info)
{ 
    uint8_t *data = mtp_handle.ptp.data_container.payload.data;
    uint16_t filenamelen;

    object_info->StorageID=LE32(&data[PTP_oi_StorageID]);
    object_info->ObjectFormat=LE16(&data[PTP_oi_ObjectFormat]);
    object_info->ProtectionStatus=LE16(&data[PTP_oi_ProtectionStatus]);
    object_info->ObjectCompressedSize=LE32(&data[PTP_oi_ObjectCompressedSize]);

    /* For Samsung Galaxy */
    if ((data[PTP_oi_filenamelen] == 0) && (data[PTP_oi_filenamelen+4] != 0)) {
        data += 4;
    }
    object_info->ThumbFormat=LE16(&data[PTP_oi_ThumbFormat]);
    object_info->ThumbCompressedSize=LE32(&data[PTP_oi_ThumbCompressedSize]);
    object_info->ThumbPixWidth=LE32(&data[PTP_oi_ThumbPixWidth]);
    object_info->ThumbPixHeight=LE32(&data[PTP_oi_ThumbPixHeight]);
    object_info->ImagePixWidth=LE32(&data[PTP_oi_ImagePixWidth]);
    object_info->ImagePixHeight=LE32(&data[PTP_oi_ImagePixHeight]);
    object_info->ImageBitDepth=LE32(&data[PTP_oi_ImageBitDepth]);
    object_info->ParentObject=LE32(&data[PTP_oi_ParentObject]);
    object_info->AssociationType=LE16(&data[PTP_oi_AssociationType]);
    object_info->AssociationDesc=LE32(&data[PTP_oi_AssociationDesc]);
    object_info->SequenceNumber=LE32(&data[PTP_oi_SequenceNumber]);
    ptp_getstring(object_info->Filename, &data[PTP_oi_filenamelen], &filenamelen);
}


/*!
    \brief      gets object descriptor info and fills object descriptor info structure.
    \param[in]  phost: USB host handler
	\param[in]  opd: object prop descriptor structure
	\param[in]  opdlen: object prop descriptor length 
    \param[out] none
    \retval     none
*/
static void ptp_getobjectpropdesc (usbh_host_struct *phost, PTP_ObjectPropDescTypeDef *opd, uint32_t opdlen)
{ 
    uint8_t *data = mtp_handle.ptp.data_container.payload.data;
    uint32_t offset = 0, i;

    opd->ObjectPropertyCode=LE16(&data[PTP_opd_ObjectPropertyCode]);
    opd->DataType=LE16(&data[PTP_opd_DataType]);
    opd->GetSet=*(uint8_t *)(&data[PTP_opd_GetSet]);

    offset = PTP_opd_FactoryDefaultValue;
    ptp_getdevicepropvalue (phost, &offset, opdlen, &opd->FactoryDefaultValue, opd->DataType);

    opd->GroupCode=LE32(&data[offset]);
    offset+=sizeof(uint32_t);

    opd->FormFlag=*(uint8_t *)(&data[offset]);
    offset+=sizeof(uint8_t);

    switch (opd->FormFlag) {
        case PTP_OPFF_Range:
            ptp_getdevicepropvalue(phost, &offset, opdlen, &opd->FORM.Range.MinimumValue, opd->DataType);
            ptp_getdevicepropvalue(phost, &offset, opdlen, &opd->FORM.Range.MaximumValue, opd->DataType);
            ptp_getdevicepropvalue(phost, &offset, opdlen, &opd->FORM.Range.StepSize, opd->DataType);
            break;

        case PTP_OPFF_Enumeration:
            opd->FORM.Enum.NumberOfValues = LE16(&data[offset]);
            offset+=sizeof(uint16_t);

            for (i=0 ; i < opd->FORM.Enum.NumberOfValues ; i++) {
                ptp_getdevicepropvalue(phost, &offset, opdlen, &opd->FORM.Enum.SupportedValue[i], opd->DataType);
            }
            break;
        default:
            break;
    }
}

/*!
    \brief      gets device prop value and fills relevant structure.
    \param[in]  phost: USB host handler
	\param[in]  offset: 
	\param[in]  total: 
	\param[in]  value:
    \param[in]  datatype:
    \param[out] none
    \retval     none
*/  
static void ptp_getdevicepropvalue(usbh_host_struct *phost, 
                                   uint32_t *offset, 
                                   uint32_t total, 
                                   PTP_PropertyValueTypedef* value, 
                                   uint16_t datatype)
{
    uint8_t *data = mtp_handle.ptp.data_container.payload.data;
    uint16_t len;
    switch (datatype) {
        case PTP_DTC_INT8:
            value->i8 = *(uint8_t *)&(data[*offset]);
            *offset += 1;
            break;

        case PTP_DTC_UINT8:
            value->u8 = *(uint8_t *)&(data[*offset]);
            *offset += 1;                
            break;

        case PTP_DTC_INT16:
            value->i16 = LE16(&(data[*offset]));
            *offset += 2;                
            break;

        case PTP_DTC_UINT16:
            value->u16 = LE16(&(data[*offset]));                
            *offset += 2;                
            break;

        case PTP_DTC_INT32:
            value->i32 = LE32(&(data[*offset]));              
            *offset += 4;                
            break;

        case PTP_DTC_UINT32:
            value->u32 = LE32(&(data[*offset]));             
            *offset += 4;                
            break;

        case PTP_DTC_INT64:
            value->i64 = LE64(&(data[*offset]));            
            *offset += 8;                
            break;

        case PTP_DTC_UINT64:
            value->u64 = LE64(&(data[*offset]));              
            *offset += 8;                
            break;

        case PTP_DTC_UINT128:
            *offset += 16;
            break;

        case PTP_DTC_INT128:
            *offset += 16;
            break;

        case PTP_DTC_STR:
            ptp_getstring((uint8_t *)value->str, (uint8_t *)&(data[*offset]), &len);
            *offset += len*2+1;
            break;

        default:
            break;
    }
}

/*!
    \brief      gets object prop list and fills relevant structure.
    \param[in]  phost: USB host handler
	\param[in]  props: 
	\param[in]  len:
    \param[out] none
    \retval     none
*/
static uint32_t ptp_getobjectproplist (usbh_host_struct *phost, mtp_propertiestypedef *props, uint32_t len)
{ 
    uint8_t *data = mtp_handle.ptp.data_container.payload.data;
    uint32_t prop_count;
    uint32_t offset = 0, i;

    prop_count = LE32(data);


    if (prop_count == 0) {
        return 0;
    }

    data += sizeof(uint32_t);
    len -= sizeof(uint32_t);

    for (i = 0; i < prop_count; i++) {
        if (len <= 0) {
            return 0;
        }

        props[i].ObjectHandle = LE32(data);
        data += sizeof(uint32_t);
        len -= sizeof(uint32_t);

        props[i].property = LE16(data);
        data += sizeof(uint16_t);
        len -= sizeof(uint16_t);

        props[i].datatype = LE16(data);
        data += sizeof(uint16_t);
        len -= sizeof(uint16_t);

        offset = 0;

        ptp_getdevicepropvalue(phost, &offset, len, &props[i].propval, props[i].datatype);

        data += offset;
        len -= offset;
    }

    return prop_count;
}

/*!
    \brief      gets ASCII string from device info
    \param[in]  str: ASCII string
	\param[in]  data: device info structure
	\param[in]  len: device info data length
    \param[out] none
    \retval     none
*/
static void ptp_getstring (uint8_t *str, uint8_t* data, uint16_t *len)
{
    uint16_t strlength;
    uint16_t idx;

    *len = data[0];
    strlength = 2 * data[0]; 
    data ++; /* Adjust the offset ignoring the String Len */

    for (idx = 0; idx < strlength; idx+=2 ) {
        /* Copy Only the string and ignore the UNICODE ID, hence add the src */
        *str =  data[idx];
        str++;
    }
    *str = 0; /* mark end of string */  
}
              
/*!
    \brief      gets 16-bits array
    \param[in]  array: 16-bits array
	\param[in]  data: device info structure
	\param[in]  offset: 
    \param[out] none
    \retval     none
*/
static uint32_t ptp_getarray16 (uint16_t *array, uint8_t *data, uint32_t offset)
{
    uint32_t size, idx = 0;

    size=LE32(&data[offset]);
    while (size > idx) {
        array[idx] = LE16(&data[offset+(sizeof(uint16_t)*(idx+2))]);
        idx++;
    }

    return size;
}

/*!
    \brief      gets 32-bits array
    \param[in]  array: 32-bits array
	\param[in]  data: device info structure
	\param[in]  offset: 
    \param[out] none
    \retval     none
*/             
static uint32_t ptp_getarray32 (uint32_t *array, uint8_t *data, uint32_t offset)
{
    uint32_t size, idx = 0;

    size=LE32(&data[offset]);
    while (size > idx) {
        array[idx] = LE32(&data[offset+(sizeof(uint32_t)*(idx+1))]);
        idx++;
    }

    return size;
}

/*******************************************************************************
                             
                          PTP Requests

*******************************************************************************/

/*!
    \brief      open a new session
    \param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  session: session ID (must be > 0)
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_ptp_opensession (usb_core_handle_struct *pudev, usbh_host_struct *phost, uint32_t session)
{
    usbh_status_enum   status = USBH_BUSY; 
    ptp_containertypedef  ptp_container;

    switch (mtp_handle.ptp.req_state) {
        case PTP_REQ_SEND:
            /* Init session params */
            mtp_handle.ptp.transaction_id = 0x00000000;
            mtp_handle.ptp.session_id = session;
            mtp_handle.ptp.flags = PTP_DP_NODATA;

            /* Fill operation request params */      
            ptp_container.Code = PTP_OC_OpenSession;
            ptp_container.SessionID = session;
            ptp_container.Transaction_ID = mtp_handle.ptp.transaction_id ++;
            ptp_container.Param1 = session;
            ptp_container.Nparam = 1;

            /* convert request packet inti USB raw packet*/
            usbh_ptp_sendrequest (phost, &ptp_container); 

            /* Setup State machine and start transfer */
            mtp_handle.ptp.state = PTP_OP_REQUEST_STATE;
            mtp_handle.ptp.req_state = PTP_REQ_WAIT;
            status = USBH_BUSY;
            #if (USBH_USE_OS == 1)
            osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
            #endif      
            break;

        case PTP_REQ_WAIT:
            status = usbh_ptp_process(pudev,phost);
            break;

        default:
            break;
    }

    return status;
}

/*!
    \brief      get device property descriptor
    \param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  propcode:
	\param[in]  devicepropertydesc:
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_ptp_getdevicepropdesc (usb_core_handle_struct *pudev, 
                                             usbh_host_struct *phost,
                                             uint16_t propcode, 
											 ptp_devicepropdesctypdef* devicepropertydesc)
{
    usbh_status_enum   status = USBH_BUSY; 
    ptp_containertypedef  ptp_container;
    uint8_t               *data = mtp_handle.ptp.data_container.payload.data;

    switch (mtp_handle.ptp.req_state) {
        case PTP_REQ_SEND:
            /* Set operation request type */
            mtp_handle.ptp.flags = PTP_DP_GETDATA;
            mtp_handle.ptp.data_ptr = (uint8_t *)&(mtp_handle.ptp.data_container);
            mtp_handle.ptp.data_length = 0;
            mtp_handle.ptp.data_packet_counter = 0;
            mtp_handle.ptp.data_packet = 0;

            /* Fill operation request params */      
            ptp_container.Code = PTP_OC_GetDevicePropDesc;
            ptp_container.SessionID = mtp_handle.ptp.session_id;
            ptp_container.Transaction_ID = mtp_handle.ptp.transaction_id ++;
            ptp_container.Param1 = propcode;     
            ptp_container.Nparam = 1;

            /* convert request packet into USB raw packet*/
            usbh_ptp_sendrequest (phost, &ptp_container); 

            /* Setup State machine and start transfer */
            mtp_handle.ptp.state = PTP_OP_REQUEST_STATE;
            mtp_handle.ptp.req_state = PTP_REQ_WAIT;
            status = USBH_BUSY;
            #if (USBH_USE_OS == 1)
            osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
            #endif      
            break;

        case PTP_REQ_WAIT:
            status = usbh_ptp_process(pudev, phost);

            if(status == USBH_OK) {
                devicepropertydesc->DevicePropertyCode = LE16(&data[PTP_dpd_DevicePropertyCode]);
                devicepropertydesc->DataType = LE16(&data[PTP_dpd_DataType]);
                devicepropertydesc->GetSet = *(uint8_t *)(&data[PTP_dpd_GetSet]);
                devicepropertydesc->FormFlag =  PTP_DPFF_None;
            }
            break;

        default:
            break;
    }

    return status;
}

/*!
    \brief      get device info data set
    \param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  dev_info:
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_ptp_getdeviceinfo (usb_core_handle_struct *pudev, usbh_host_struct *phost, PTP_DeviceInfoTypedef *dev_info)
{
    usbh_status_enum   status = USBH_BUSY;  
    ptp_containertypedef  ptp_container;

    switch (mtp_handle.ptp.req_state) {
        case PTP_REQ_SEND:
            /* Set operation request type */
            mtp_handle.ptp.flags = PTP_DP_GETDATA;
            mtp_handle.ptp.data_ptr = (uint8_t *)&(mtp_handle.ptp.data_container);
            mtp_handle.ptp.data_length = 0;
            mtp_handle.ptp.data_packet_counter = 0;
            mtp_handle.ptp.data_packet = 0;

            /* Fill operation request params */      
            ptp_container.Code = PTP_OC_GetDeviceInfo;
            ptp_container.SessionID = mtp_handle.ptp.session_id;
            ptp_container.Transaction_ID = mtp_handle.ptp.transaction_id ++;
            ptp_container.Nparam = 0;

            /* convert request packet inti USB raw packet*/
            usbh_ptp_sendrequest (phost, &ptp_container); 

            /* Setup State machine and start transfer */
            mtp_handle.ptp.state = PTP_OP_REQUEST_STATE;
            mtp_handle.ptp.req_state = PTP_REQ_WAIT;
            status = USBH_BUSY; 
            #if (USBH_USE_OS == 1)
            osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
            #endif     
            break;

        case PTP_REQ_WAIT:
            status = usbh_ptp_process(pudev, phost);

            if (status == USBH_OK) {
                ptp_decodedeviceinfo (phost, dev_info);
            }
            break;

        default:
            break;
    }

    return status;
}

/*!
    \brief      get storage ID
    \param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  storage_ids:
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_ptp_getstorageids (usb_core_handle_struct *pudev, usbh_host_struct *phost, PTP_StorageIDsTypedef *storage_ids)
{
    usbh_status_enum   status = USBH_BUSY; 
    ptp_containertypedef  ptp_container;

    switch (mtp_handle.ptp.req_state) {
        case PTP_REQ_SEND:
            /* Set operation request type */
            mtp_handle.ptp.flags = PTP_DP_GETDATA;
            mtp_handle.ptp.data_ptr = (uint8_t *)&(mtp_handle.ptp.data_container);
            mtp_handle.ptp.data_length = 0;
            mtp_handle.ptp.data_packet_counter = 0;
            mtp_handle.ptp.data_packet = 0;

            /* Fill operation request params */      
            ptp_container.Code = PTP_OC_GetStorageIDs;
            ptp_container.SessionID = mtp_handle.ptp.session_id;
            ptp_container.Transaction_ID = mtp_handle.ptp.transaction_id ++;
            ptp_container.Nparam = 0;

            /* convert request packet inti USB raw packet*/
            usbh_ptp_sendrequest (phost, &ptp_container); 

            /* Setup State machine and start transfer */
            mtp_handle.ptp.state = PTP_OP_REQUEST_STATE;
            mtp_handle.ptp.req_state = PTP_REQ_WAIT;
            status = USBH_BUSY;
            #if (USBH_USE_OS == 1)
            osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
            #endif      
            break;

        case PTP_REQ_WAIT:
            status = usbh_ptp_process(pudev, phost);

            if(status == USBH_OK)
            {
            ptp_getstorageids (phost, storage_ids);
            }
            break;

        default:
            break;
    }

    return status;
}

/*!
    \brief      get storage information
    \param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  storage_id:
	\param[in]  storage_info:
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_ptp_getstorageinfo (usb_core_handle_struct *pudev, 
                                          usbh_host_struct *phost, 
										  uint32_t storage_id, 
										  PTP_StorageInfoTypedef *storage_info)
{
    usbh_status_enum   status = USBH_BUSY; 
    ptp_containertypedef  ptp_container;

    switch (mtp_handle.ptp.req_state) {
        case PTP_REQ_SEND:
            /* Set operation request type */
            mtp_handle.ptp.flags = PTP_DP_GETDATA;
            mtp_handle.ptp.data_ptr = (uint8_t *)&(mtp_handle.ptp.data_container);
            mtp_handle.ptp.data_length = 0;
            mtp_handle.ptp.data_packet_counter = 0;
            mtp_handle.ptp.data_packet = 0;

            /* Fill operation request params */      
            ptp_container.Code = PTP_OC_GetStorageInfo;
            ptp_container.SessionID = mtp_handle.ptp.session_id;
            ptp_container.Transaction_ID = mtp_handle.ptp.transaction_id ++;
            ptp_container.Param1 = storage_id;    
            ptp_container.Nparam = 1;

            /* convert request packet inti USB raw packet*/
            usbh_ptp_sendrequest (phost, &ptp_container); 

            /* Setup State machine and start transfer */
            mtp_handle.ptp.state = PTP_OP_REQUEST_STATE;
            mtp_handle.ptp.req_state = PTP_REQ_WAIT;
            status = USBH_BUSY;
            #if (USBH_USE_OS == 1)
            osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
            #endif      
            break;

        case PTP_REQ_WAIT:
            status = usbh_ptp_process(pudev, phost);

            if (status == USBH_OK) {
                ptp_getstorageinfo (phost, storage_id, storage_info);
            }
            break;

        default:
            break;
    }

    return status;
}

/*!
    \brief      get object number
    \param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  storage_id:
	\param[in]  objectformatcode:
    \param[in]  numobs:
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_ptp_getnumobjects (usb_core_handle_struct *pudev,
                                         usbh_host_struct *phost, 
                                         uint32_t storage_id, 
                                         uint32_t objectformatcode, 
                                         uint32_t associationOH,
                                         uint32_t* numobs)
{
    usbh_status_enum   status = USBH_BUSY; 
    ptp_containertypedef  ptp_container;

    switch (mtp_handle.ptp.req_state) {
        case PTP_REQ_SEND:
            /* Set operation request type */
            mtp_handle.ptp.flags = PTP_DP_NODATA;

            /* Fill operation request params */      
            ptp_container.Code = PTP_OC_GetNumObjects;
            ptp_container.SessionID = mtp_handle.ptp.session_id;
            ptp_container.Transaction_ID = mtp_handle.ptp.transaction_id ++;
            ptp_container.Param1 = storage_id;  
            ptp_container.Param2 = objectformatcode;
            ptp_container.Param3 = associationOH;    
            ptp_container.Nparam = 3;

            /* convert request packet into USB raw packet*/
            usbh_ptp_sendrequest (phost, &ptp_container); 

            /* Setup State machine and start transfer */
            mtp_handle.ptp.state = PTP_OP_REQUEST_STATE;
            mtp_handle.ptp.req_state = PTP_REQ_WAIT;
            status = USBH_BUSY;
            #if (USBH_USE_OS == 1)
            osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
            #endif      
            break;

        case PTP_REQ_WAIT:
            status = usbh_ptp_process(pudev, phost);

            if (status == USBH_OK) {
                *numobs = mtp_handle.ptp.resp_container.param1;
            }
            break;

        default:
            break;
    }
    return status;
}

/*!
    \brief      get object handle
    \param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  storage_id:
	\param[in]  objectformatcode:
	\param[in]  associationOH:
	\param[in]  objecthandles:
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_ptp_getobjecthandles (usb_core_handle_struct *pudev,
                                            usbh_host_struct *phost, 
                                            uint32_t storage_id, 
                                            uint32_t objectformatcode, 
                                            uint32_t associationOH,
                                            ptp_objecthandlestypedef* objecthandles)
{
    usbh_status_enum   status = USBH_BUSY; 
    ptp_containertypedef  ptp_container;

    switch (mtp_handle.ptp.req_state) {
        case PTP_REQ_SEND:
        /* Set operation request type */
        mtp_handle.ptp.flags = PTP_DP_GETDATA;
        mtp_handle.ptp.data_ptr = (uint8_t *)&(mtp_handle.ptp.data_container);
        mtp_handle.ptp.data_length = 0;
        mtp_handle.ptp.data_packet_counter = 0;
        mtp_handle.ptp.data_packet = 0;

        /* Fill operation request params */      
        ptp_container.Code = PTP_OC_GetObjectHandles;
        ptp_container.SessionID = mtp_handle.ptp.session_id;
        ptp_container.Transaction_ID = mtp_handle.ptp.transaction_id ++;
        ptp_container.Param1 = storage_id;  
        ptp_container.Param2 = objectformatcode;
        ptp_container.Param3 = associationOH;    
        ptp_container.Nparam = 3;

        /* convert request packet into USB raw packet*/
        usbh_ptp_sendrequest (phost, &ptp_container); 

        /* Setup State machine and start transfer */
        mtp_handle.ptp.state = PTP_OP_REQUEST_STATE;
        mtp_handle.ptp.req_state = PTP_REQ_WAIT;
        status = USBH_BUSY;
        #if (USBH_USE_OS == 1)
        osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
        #endif      
        break;

    case PTP_REQ_WAIT:
        status = usbh_ptp_process(pudev, phost);

        if (status == USBH_OK) {
        objecthandles->n = ptp_getarray32 (objecthandles->handler, 
                                  mtp_handle.ptp.data_container.payload.data, 
                                  0); 
        }
        break;

    default:
        break;
    }

    return status;
}

/*!
    \brief      get object information
    \param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  handle:
	\param[in]  object_info:
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_ptp_getobjectinfo (usb_core_handle_struct *pudev,
                                         usbh_host_struct *phost, 
                                         uint32_t handle, 
                                         ptp_objectinfotypedef* object_info)
{
    usbh_status_enum   status = USBH_BUSY; 
    ptp_containertypedef  ptp_container;

    switch (mtp_handle.ptp.req_state) {
        case PTP_REQ_SEND:
            /* Set operation request type */
            mtp_handle.ptp.flags = PTP_DP_GETDATA;
            mtp_handle.ptp.data_ptr = (uint8_t *)&(mtp_handle.ptp.data_container);
            mtp_handle.ptp.data_length = 0;
            mtp_handle.ptp.data_packet_counter = 0;
            mtp_handle.ptp.data_packet = 0;

            /* Fill operation request params */      
            ptp_container.Code = PTP_OC_GetObjectInfo;
            ptp_container.SessionID = mtp_handle.ptp.session_id;
            ptp_container.Transaction_ID = mtp_handle.ptp.transaction_id ++;
            ptp_container.Param1 = handle;  
            ptp_container.Nparam = 1;

            /* convert request packet into USB raw packet*/
            usbh_ptp_sendrequest (phost, &ptp_container); 

            /* Setup State machine and start transfer */
            mtp_handle.ptp.state = PTP_OP_REQUEST_STATE;
            mtp_handle.ptp.req_state = PTP_REQ_WAIT;
            status = USBH_BUSY;
            #if (USBH_USE_OS == 1)
            osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
            #endif      
            break;

        case PTP_REQ_WAIT:
            status = usbh_ptp_process(pudev, phost);

            if (status == USBH_OK) {
                ptp_getobjectinfo (phost, object_info);
            }
            break;

        default:
            break;
    }

    return status;
}

/*!
    \brief      delete an object
    \param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  handle:
	\param[in]  objectformatcode:
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_ptp_deleteobject (usb_core_handle_struct *pudev,
                                        usbh_host_struct *phost,
                                        uint32_t handle,
                                        uint32_t objectformatcode)
{
    usbh_status_enum   status = USBH_BUSY;  
    ptp_containertypedef  ptp_container;

    switch (mtp_handle.ptp.req_state) {
        case PTP_REQ_SEND:
            /* Set operation request type */
            mtp_handle.ptp.flags = PTP_DP_NODATA;

            /* Fill operation request params */      
            ptp_container.Code = PTP_OC_DeleteObject;
            ptp_container.SessionID = mtp_handle.ptp.session_id;
            ptp_container.Transaction_ID = mtp_handle.ptp.transaction_id ++;
            ptp_container.Param1 = handle;  
            ptp_container.Param2 = objectformatcode;
            ptp_container.Nparam = 2;

            /* convert request packet into USB raw packet*/
            usbh_ptp_sendrequest (phost, &ptp_container); 

            /* Setup State machine and start transfer */
            mtp_handle.ptp.state = PTP_OP_REQUEST_STATE;
            mtp_handle.ptp.req_state = PTP_REQ_WAIT;
            status = USBH_BUSY;
            #if (USBH_USE_OS == 1)
            osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
            #endif      
            break;

        case PTP_REQ_WAIT:
            status = usbh_ptp_process(pudev, phost);
            break;

        default:
            break;
    }
    return status;
}

/*!
    \brief      get object
    \param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  handle:
	\param[in]  object:
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_ptp_getobject (usb_core_handle_struct *pudev, 
                                     usbh_host_struct *phost, 
                                     uint32_t handle, 
                                     uint8_t *object)
{
    usbh_status_enum   status = USBH_BUSY; 
    ptp_containertypedef  ptp_container;

    switch (mtp_handle.ptp.req_state) {
        case PTP_REQ_SEND:
            /* Set operation request type */
            mtp_handle.ptp.flags = PTP_DP_GETDATA;
            mtp_handle.ptp.data_ptr = (uint8_t *)&(mtp_handle.ptp.data_container);
            mtp_handle.ptp.data_length = 0;
            mtp_handle.ptp.data_packet_counter = 0;
            mtp_handle.ptp.data_packet = 0;

            /* set object control params */
            mtp_handle.ptp.object_ptr = object;

            /* Fill operation request params */      
            ptp_container.Code = PTP_OC_GetObject;
            ptp_container.SessionID = mtp_handle.ptp.session_id;
            ptp_container.Transaction_ID = mtp_handle.ptp.transaction_id ++;
            ptp_container.Param1 = handle;  
            ptp_container.Nparam = 1;


            /* convert request packet into USB raw packet*/
            usbh_ptp_sendrequest (phost, &ptp_container); 

            /* Setup State machine and start transfer */
            mtp_handle.ptp.state = PTP_OP_REQUEST_STATE;
            mtp_handle.ptp.req_state = PTP_REQ_WAIT;
            status = USBH_BUSY;
            #if (USBH_USE_OS == 1)
            osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
            #endif      
            break;

        case PTP_REQ_WAIT:
            status = usbh_ptp_process(pudev, phost);

            if (status == USBH_OK) {
            /* first packet is in the PTP data payload buffer */
            if (mtp_handle.ptp.iteration == 0) {
                /* copy it to object */
                USBH_MEMCPY(mtp_handle.ptp.object_ptr, mtp_handle.ptp.data_container.payload.data, PTP_USB_BULK_PAYLOAD_LEN_READ);
            }
            }
            break;

        default:
            break;
    }

    return status;
}

/*!
    \brief      get object partially
    \param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  handle:
	\param[in]  offset:
	\param[in]  maxbytes:
	\param[in]  object:
	\param[in]  len:
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_ptp_getpartialobject(usb_core_handle_struct *pudev,
                                           usbh_host_struct *phost, 
                                           uint32_t handle, 
                                           uint32_t offset,
                                           uint32_t maxbytes, 
                                           uint8_t *object,
                                           uint32_t *len)
{
    usbh_status_enum   status = USBH_BUSY;  
    ptp_containertypedef  ptp_container;

    switch (mtp_handle.ptp.req_state) {
        case PTP_REQ_SEND:
            /* Set operation request type */
            mtp_handle.ptp.flags = PTP_DP_GETDATA;
            mtp_handle.ptp.data_ptr = (uint8_t *)&(mtp_handle.ptp.data_container);
            mtp_handle.ptp.data_length = 0;
            mtp_handle.ptp.data_packet_counter = 0;
            mtp_handle.ptp.data_packet = 0;

            /* set object control params */
            mtp_handle.ptp.object_ptr = object;

            /* Fill operation request params */
            ptp_container.Code = PTP_OC_GetPartialObject;
            ptp_container.SessionID = mtp_handle.ptp.session_id;
            ptp_container.Transaction_ID = mtp_handle.ptp.transaction_id ++;
            ptp_container.Param1 = handle;
            ptp_container.Param2 = offset;
            ptp_container.Param3 = maxbytes;
            ptp_container.Nparam = 3;

            /* convert request packet into USB raw packet*/
            usbh_ptp_sendrequest (phost, &ptp_container); 

            /* Setup State machine and start transfer */
            mtp_handle.ptp.state = PTP_OP_REQUEST_STATE;
            mtp_handle.ptp.req_state = PTP_REQ_WAIT;
            status = USBH_BUSY;
            #if (USBH_USE_OS == 1)
            osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
            #endif
            break;

        case PTP_REQ_WAIT:
            status = usbh_ptp_process(pudev, phost);

            if (status == USBH_OK) {
                *len = mtp_handle.ptp.resp_container.param1;
                /* first packet is in the PTP data payload buffer */
                if (mtp_handle.ptp.iteration == 0) {
                /* copy it to object */
                USBH_MEMCPY(mtp_handle.ptp.object_ptr, mtp_handle.ptp.data_container.payload.data, *len);
                }
            }
            break;

        default:
            break;
        }

    return status;
}

/*!
    \brief      get object prop support
    \param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  ofc:
	\param[in]  propnum:
	\param[in]  props:
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_ptp_getobjectpropssupported (usb_core_handle_struct *pudev,
                                                   usbh_host_struct *phost,
                                                   uint16_t ofc,
                                                   uint32_t *propnum, 
                                                   uint16_t *props)
{
    usbh_status_enum   status = USBH_BUSY; 
    ptp_containertypedef  ptp_container;

    switch (mtp_handle.ptp.req_state) {
        case PTP_REQ_SEND:
            /* Set operation request type */
            mtp_handle.ptp.flags = PTP_DP_GETDATA;
            mtp_handle.ptp.data_ptr = (uint8_t *)&(mtp_handle.ptp.data_container);
            mtp_handle.ptp.data_length = 0;
            mtp_handle.ptp.data_packet_counter = 0;
            mtp_handle.ptp.data_packet = 0;

            /* Fill operation request params */      
            ptp_container.Code = PTP_OC_GetObjectPropsSupported;
            ptp_container.SessionID = mtp_handle.ptp.session_id;
            ptp_container.Transaction_ID = mtp_handle.ptp.transaction_id ++;
            ptp_container.Param1 = ofc;  
            ptp_container.Nparam = 1;

            /* convert request packet into USB raw packet*/
            usbh_ptp_sendrequest (phost, &ptp_container); 

            /* Setup State machine and start transfer */
            mtp_handle.ptp.state = PTP_OP_REQUEST_STATE;
            mtp_handle.ptp.req_state = PTP_REQ_WAIT;
            status = USBH_BUSY;
            #if (USBH_USE_OS == 1)
            osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
            #endif
            break;

        case PTP_REQ_WAIT:
            status = usbh_ptp_process(pudev, phost);

            if (status == USBH_OK) {
                *propnum = ptp_getarray16 (props, mtp_handle.ptp.data_container.payload.data, 0);      
            }
            break;

        default:
            break;
    }

    return status;
}

/*!
    \brief      get object prop descriptor
    \param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  opc:
	\param[in]  ofc:
	\param[in]  opd:
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_ptp_getobjectpropdesc (usb_core_handle_struct *pudev, 
                                             usbh_host_struct *phost,
                                             uint16_t opc, 
											 uint16_t ofc, 
											 PTP_ObjectPropDescTypeDef *opd)
{
    usbh_status_enum   status = USBH_BUSY; 
    ptp_containertypedef  ptp_container;

    switch (mtp_handle.ptp.req_state) {
        case PTP_REQ_SEND:
            /* Set operation request type */
            mtp_handle.ptp.flags = PTP_DP_GETDATA;
            mtp_handle.ptp.data_ptr = (uint8_t *)&(mtp_handle.ptp.data_container);
            mtp_handle.ptp.data_length = 0;
            mtp_handle.ptp.data_packet_counter = 0;
            mtp_handle.ptp.data_packet = 0;

            /* Fill operation request params */      
            ptp_container.Code = PTP_OC_GetObjectPropDesc;
            ptp_container.SessionID = mtp_handle.ptp.session_id;
            ptp_container.Transaction_ID = mtp_handle.ptp.transaction_id ++;
            ptp_container.Param1 = opc;  
            ptp_container.Param2 = ofc;      
            ptp_container.Nparam = 2;

            /* convert request packet into USB raw packet*/
            usbh_ptp_sendrequest (phost, &ptp_container); 

            /* Setup State machine and start transfer */
            mtp_handle.ptp.state = PTP_OP_REQUEST_STATE;
            mtp_handle.ptp.req_state = PTP_REQ_WAIT;
            status = USBH_BUSY;
            #if (USBH_USE_OS == 1)
            osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
            #endif
            break;

        case PTP_REQ_WAIT:
            status = usbh_ptp_process(pudev, phost);

            if (status == USBH_OK) {
                ptp_getobjectpropdesc(phost, opd, mtp_handle.ptp.data_length);
            }
            break;

        default:
            break;
    }

    return status;
}

/*!
    \brief      get object prop list
    \param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  pprops:
	\param[in]  nrofprops:
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_ptp_getobjectproplist (usb_core_handle_struct *pudev, usbh_host_struct *phost,
                                             uint32_t handle, 
                                             mtp_propertiestypedef *pprops, 
                                             uint32_t *nrofprops)
{
    usbh_status_enum   status = USBH_BUSY; 
    ptp_containertypedef  ptp_container;

    switch (mtp_handle.ptp.req_state) {
        case PTP_REQ_SEND:
            /* Set operation request type */
            mtp_handle.ptp.flags = PTP_DP_GETDATA;
            mtp_handle.ptp.data_ptr = (uint8_t *)&(mtp_handle.ptp.data_container);
            mtp_handle.ptp.data_length = 0;
            mtp_handle.ptp.data_packet_counter = 0;
            mtp_handle.ptp.data_packet = 0;

            /* copy first packet of the object into data container */
            USBH_MEMCPY(mtp_handle.ptp.data_container.payload.data, mtp_handle.ptp.object_ptr, PTP_USB_BULK_PAYLOAD_LEN_READ);

            /* Fill operation request params */      
            ptp_container.Code = PTP_OC_GetObjPropList;
            ptp_container.SessionID = mtp_handle.ptp.session_id;
            ptp_container.Transaction_ID = mtp_handle.ptp.transaction_id ++;
            ptp_container.Param1 = handle;  
            ptp_container.Param2 = 0x00000000U;  /* 0x00000000U should be "all formats" */
            ptp_container.Param3 = 0xFFFFFFFFU;  /* 0xFFFFFFFFU should be "all properties" */
            ptp_container.Param4 = 0x00000000U;
            ptp_container.Param5 = 0xFFFFFFFFU;  /* Return full tree below the Param1 handle */
            ptp_container.Nparam = 5;  

            /* convert request packet into USB raw packet*/
            usbh_ptp_sendrequest (phost, &ptp_container); 

            /* Setup State machine and start transfer */
            mtp_handle.ptp.state = PTP_OP_REQUEST_STATE;
            mtp_handle.ptp.req_state = PTP_REQ_WAIT;
            status = USBH_BUSY;
            #if (USBH_USE_OS == 1)
            osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
            #endif      
            break;

        case PTP_REQ_WAIT:
            status = usbh_ptp_process(pudev, phost);

            if (status == USBH_OK) {
                ptp_getobjectproplist (phost, pprops,  mtp_handle.ptp.data_length);
            }
            break;

        default:
            break;
    }

    return status;
}

/*!
    \brief      send an object
    \param[in]  pudev: pointer to USB core handler
	\param[in]  phost: pointer to USB host handler
	\param[in]  handle:
	\param[in]  object:
	\param[in]  size:
    \param[out] none
    \retval     none
*/
usbh_status_enum usbh_ptp_sendobject (usb_core_handle_struct *pudev, 
                                      usbh_host_struct *phost, 
                                      uint32_t handle, 
                                      uint8_t *object,
                                      uint32_t size)
{
    usbh_status_enum   status = USBH_BUSY; 
    ptp_containertypedef  ptp_container;

    switch (mtp_handle.ptp.req_state) {
        case PTP_REQ_SEND:
            /* Set operation request type */
            mtp_handle.ptp.flags = PTP_DP_SENDDATA;
            mtp_handle.ptp.data_ptr = (uint8_t *)&(mtp_handle.ptp.data_container);
            mtp_handle.ptp.data_packet_counter = 0;
            mtp_handle.ptp.data_packet = 0;
            mtp_handle.ptp.iteration = 0;

            /* set object control params */
            mtp_handle.ptp.object_ptr = object;
            mtp_handle.ptp.data_length = size;    

            /* Fill operation request params */      
            ptp_container.Code = PTP_OC_SendObject;
            ptp_container.SessionID = mtp_handle.ptp.session_id;
            ptp_container.Transaction_ID = mtp_handle.ptp.transaction_id ++;
            ptp_container.Nparam = 0;


            /* convert request packet into USB raw packet*/
            usbh_ptp_sendrequest (phost, &ptp_container); 

            /* Setup State machine and start transfer */
            mtp_handle.ptp.state = PTP_OP_REQUEST_STATE;
            mtp_handle.ptp.req_state = PTP_REQ_WAIT;
            status = USBH_BUSY; 
            #if (USBH_USE_OS == 1)
            osMessagePut ( phost->os_event, USBH_STATE_CHANGED_EVENT, 0);
            #endif
            break;

        case PTP_REQ_WAIT:
            status = usbh_ptp_process(pudev, phost);
            break;

        default:
            break;
    }

    return status;
}
