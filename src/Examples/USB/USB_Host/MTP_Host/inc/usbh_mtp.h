/*!
    \file  usbh_mtp.h
    \brief this file contains all the prototypes for the usbh_mtp.c

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

#ifndef USBH_MTP_H
#define USBH_MTP_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "usbh_mtp_ptp.h"
#include "usbh_core.h"
#include "usbh_hcs.h"
#include "usbh_ctrl.h"

/*Communication Class codes*/
//#define USB_MTP_CLASS                                         0x06 /* Still Image Class)*/
#define USB_MTP_CLASS                                           0xFF /* Still Image Class)*/
#define MTP_MAX_STORAGE_UNITS_NBR                               PTP_MAX_STORAGE_UNITS_NBR
#define MTP_PROTOCOL                                            0x00

typedef struct
{
    usbh_status_enum (*class_req_polling)  (usb_core_handle_struct *pudev, usbh_host_struct *puhost, void *pustate);
    usbh_status_enum (*class_polling)      (usb_core_handle_struct *pudev, usbh_host_struct *puhost, void *pustate);
}class_polling_fun_cb_struct;

typedef enum
{   
    MTP_IDLE = 0,
    MTP_GETDEVICEINFO ,           
    MTP_OPENSESSION ,             
    MTP_CLOSESESSION ,            
    MTP_GETSTORAGEIDS ,           
    MTP_GETSTORAGEINFO ,          
}MTP_StateTypeDef;


typedef enum
{   
    MTP_EVENTS_INIT = 0,
    MTP_EVENTS_GETDATA ,                   
}MTP_EventsStateTypeDef;


typedef struct
{
    MTP_EventsStateTypeDef   state;
    __IO uint32_t timer;
    uint16_t poll;
    PTP_EventContainerTypedef  container;       
}MTP_EventHandleTypedef;

typedef struct
{
    uint32_t        CurrentStorageId;
    uint32_t        ObjectFormatCode;
    uint32_t        CurrentObjectHandler;
    uint8_t         ObjectHandlerNbr;
    uint32_t        Objdepth;
}MTP_ParamsTypedef;


typedef struct
{
    PTP_DeviceInfoTypedef     devinfo;
    PTP_StorageIDsTypedef     storids;
    PTP_StorageInfoTypedef    storinfo[MTP_MAX_STORAGE_UNITS_NBR]; 
    ptp_objecthandlestypedef  Handles;
}MTP_InfoTypedef;

/* Structure for MTP process */
typedef struct _mtp_process
{
    MTP_InfoTypedef       info;
    MTP_ParamsTypedef     params;

    uint8_t               datainpipe; 
    uint8_t               dataoutpipe;
    uint8_t               notificationpipe;  

    uint8_t               dataoutep;
    uint8_t               datainep;
    uint8_t               notificationep;  

    uint16_t              dataoutepsize;
    uint16_t              datainepsize;
    uint16_t              notificationepsize; 
    MTP_StateTypeDef      state;
    MTP_EventHandleTypedef events;
    ptp_handletypedef     ptp;
    uint32_t              current_storage_unit;
    uint32_t              is_ready;  
}mtp_handletypedef;

#define MTP_StorageInfoTypedef      PTP_StorageInfoTypedef
#define mtp_objecthandlestypedef    ptp_objecthandlestypedef
#define mtp_objectinfotypedef       ptp_objectinfotypedef
#define USBH_MTP_CLASS    &MTP_Class
extern mtp_handletypedef mtp_handle;

usbh_status_enum usbh_mtp_interfaceinit (usb_core_handle_struct *pudev,void *phost);
void USBH_MTP_InterfaceDeInit (usb_core_handle_struct *pudev, void *phost);
uint8_t            usbh_mtp_isready (usbh_host_struct *phost);
usbh_status_enum usbh_mtp_selectstorage (usbh_host_struct *phost, uint8_t storage_idx);
usbh_status_enum usbh_mtp_getnumstorage (usbh_host_struct *phost, uint8_t *storage_num);
usbh_status_enum usbh_mtp_getnumobjects (usb_core_handle_struct *pudev, usbh_host_struct *phost, 
                                           uint32_t storage_idx, 
                                           uint32_t objectformatcode, 
                                           uint32_t associationOH,
                                           uint32_t* numobs);
usbh_status_enum usbh_mtp_getstorageinfo (usbh_host_struct *phost, 
                                            uint8_t storage_idx, 
                                            MTP_StorageInfoTypedef *info);

usbh_status_enum usbh_mtp_getobjecthandles (usb_core_handle_struct *pudev, usbh_host_struct *phost, 
                                           uint32_t storage_id, 
                                           uint32_t objectformatcode, 
                                           uint32_t associationOH,
                                           ptp_objecthandlestypedef* objecthandles);

usbh_status_enum usbh_mtp_getobjectinfo (usb_core_handle_struct *pudev, usbh_host_struct *phost, 
                                           uint32_t handle, 
                                           ptp_objectinfotypedef* objectinfo);

usbh_status_enum usbh_mtp_deleteobject (usb_core_handle_struct *pudev, usbh_host_struct *phost,
                                          uint32_t handle,
                                          uint32_t objectformatcode);

usbh_status_enum usbh_mtp_getobject (usb_core_handle_struct *pudev,usbh_host_struct *phost, 
                                           uint32_t handle, 
                                           uint8_t *object);

usbh_status_enum usbh_mtp_getpartialobject(usb_core_handle_struct *pudev, usbh_host_struct *phost, 
                                           uint32_t handle, 
                                           uint32_t offset,
                                           uint32_t maxbytes, 
                                           uint8_t *object,
                                           uint32_t *len);

usbh_status_enum usbh_mtp_getobjectpropssupported (usb_core_handle_struct *pudev, usbh_host_struct *phost,
                                                     uint16_t ofc,
                                                     uint32_t *propnum, 
                                                     uint16_t *props);

usbh_status_enum usbh_mtp_getobjectpropdesc (usb_core_handle_struct *pudev, usbh_host_struct *phost,
                                                uint16_t opc, 
                                                uint16_t ofc, 
                                                PTP_ObjectPropDescTypeDef *opd);

usbh_status_enum usbh_mtp_getobjectproplist (usb_core_handle_struct *pudev, usbh_host_struct *phost,
                                                uint32_t handle, 
                                                mtp_propertiestypedef *pprops, 
                                                uint32_t *nrofprops);

usbh_status_enum usbh_mtp_sendobject (usb_core_handle_struct *pudev, usbh_host_struct *phost, 
                                           uint32_t handle, 
                                           uint8_t *object,
                                           uint32_t size);

usbh_status_enum usbh_mtp_getdevicepropdesc (usb_core_handle_struct *pudev, usbh_host_struct *phost,
                                                uint16_t propcode, 
                                                ptp_devicepropdesctypdef* devicepropertydesc);

void usbh_mtp_eventscallback(usbh_host_struct *phost, uint32_t event, uint32_t param);

#ifdef __cplusplus
}
#endif

#endif /* USBH_MTP_H */
