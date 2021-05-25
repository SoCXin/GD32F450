/*!
    \file  dfu_mal.c
    \brief USB DFU device media access layer functions

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

#include "dfu_mal.h"
#include "flash_if.h"
#include "usb_defines.h"
#include "dfu_core.h"
#include "usb_delay.h"

/* The reference tables of global memories callback and string descriptors.
   To add a new memory, You can do as follows: 
   1. Modify the value of MAX_USED_MEDIA in usbd_dfu_mal.h
   2. Add the pointer to the callback structure in this table
   3. Add the pointer to the memory string descriptor in usbd_dfu_StringDesc table
   No other operation is required.
 */
DFU_MAL_Property_TypeDef* tMALTab[MAX_USED_MEMORY_MEDIA] = {
    &DFU_Flash_cb
};

/* The list of memory interface string descriptor pointers. This list
   can be updated whenever a memory has to be added or removed */
const uint8_t* USBD_DFU_StringDesc[MAX_USED_MEMORY_MEDIA] = 
{
    (const uint8_t *)FLASH_IF_STRING
};

/* Memory buffer for downloaded data */
uint8_t  MAL_Buffer[TRANSFER_SIZE];

extern usb_core_handle_struct usbhs_core_dev;

static uint8_t  DFU_MAL_CheckAddr (uint32_t Addr);

/*!
    \brief      initialize the memory media on the GD32
    \param[in]  none
    \param[out] none
    \retval     MAL_OK
*/
uint8_t  DFU_MAL_Init (void)
{
    uint32_t memIdx = 0;

    /* Initialize all supported memory medias */
    for(memIdx = 0; memIdx < MAX_USED_MEMORY_MEDIA; memIdx++)
    {
        /* Check if the memory media exists */
        if (tMALTab[memIdx]->pMAL_Init != NULL)
        {
            tMALTab[memIdx]->pMAL_Init();
        }
    }

    return MAL_OK;
}

/*!
    \brief      de-initialize the memory media on the GD32
    \param[in]  none
    \param[out] none
    \retval     MAL_OK
*/
uint8_t  DFU_MAL_DeInit (void)
{
    uint32_t memIdx = 0;

    /* Deinitializes all supported memory medias */
    for(memIdx = 0; memIdx < MAX_USED_MEMORY_MEDIA; memIdx++)
    {
        /* Check if the memory media exists */
        if (tMALTab[memIdx]->pMAL_DeInit != NULL)
        {
            tMALTab[memIdx]->pMAL_DeInit();
        }
    }

    return MAL_OK;
}

/*!
    \brief      erase the memory sector
    \param[in]  Addr: memory sector address/code
    \param[out] none
    \retval     MAL_OK if all operations are OK, MAL_FAIL else
*/
uint8_t  DFU_MAL_Erase (uint32_t Addr)
{
    uint32_t memIdx = DFU_MAL_CheckAddr(Addr);

    /* Check if the address is in protected area */
    if (IS_PROTECTED_AREA(Addr))
    {
        return MAL_FAIL;
    }

    if (memIdx < MAX_USED_MEMORY_MEDIA)
    {
        /* Check if the operation is supported */
        if (tMALTab[memIdx]->pMAL_Erase != NULL)
        {
            return tMALTab[memIdx]->pMAL_Erase(Addr);
        }
        else
        {
            return MAL_FAIL;
        }
    }
    else
    {
        return MAL_FAIL;
    }
}

/*!
    \brief      write data to sectors of memory
    \param[in]  Addr: memory sector address/code
    \param[in]  Len: length of data to be written (in bytes)
    \param[out] none
    \retval     MAL_OK if all operations are OK, MAL_FAIL else
*/
uint8_t  DFU_MAL_Write (uint32_t Addr, uint32_t Len)
{
    uint32_t memIdx = DFU_MAL_CheckAddr(Addr);

    /* Check if the address is in protected area */
    if (IS_PROTECTED_AREA(Addr))
    {
        return MAL_FAIL;
    }
    
    if ((Addr & MAL_MASK_OB) == OB_RDPT0) {
        usbd_ctlstatus_rx(&usbhs_core_dev);
        usb_mdelay(100);
        Option_Byte_Write(Addr,MAL_Buffer);
        NVIC_SystemReset();
        return MAL_OK;
    }

    if (memIdx < MAX_USED_MEMORY_MEDIA)
    {
        /* Check if the operation is supported */
        if (tMALTab[memIdx]->pMAL_Write != NULL)
        {
            return tMALTab[memIdx]->pMAL_Write(Addr, Len);
        }
        else
        {
            return MAL_FAIL;
        }
    }
    else
    {
        return MAL_FAIL;
    }
}

/*!
    \brief      read data to sectors of memory
    \param[in]  Addr: memory sector address/code
    \param[in]  Len: length of data to be written (in bytes)
    \param[out] none
    \retval     MAL_OK if all operations are OK, MAL_FAIL else
*/
uint8_t*  DFU_MAL_Read (uint32_t Addr, uint32_t Len)
{
    uint32_t memIdx = 0;

    if((Addr != OB_RDPT0) && (Addr != OB_RDPT1))
    {
        memIdx = DFU_MAL_CheckAddr(Addr);
    }

    if (memIdx < MAX_USED_MEMORY_MEDIA)
    {
        /* Check if the operation is supported */
        if (tMALTab[memIdx]->pMAL_Read != NULL)
        {
            return tMALTab[memIdx]->pMAL_Read(Addr, Len);
        }
        else
        {
            return MAL_Buffer;
        }
    }
    else
    {
        return MAL_Buffer;
    }
}

/*!
    \brief      get the status of a given memory and store in buffer
    \param[in]  Addr: memory sector address/code
    \param[in]  Cmd: 0 for erase and 1 for write
    \param[in]  buffer: pointer to the buffer where the status data will be stored
    \param[out] none
    \retval     MAL_OK if all operations are OK, MAL_FAIL else
*/
uint8_t  DFU_MAL_GetStatus (uint32_t Addr, uint8_t Cmd, uint8_t *buffer)
{
    uint32_t memIdx = DFU_MAL_CheckAddr(Addr);

    if (memIdx < MAX_USED_MEMORY_MEDIA)
    {
        if (Cmd & 0x01)
        {
            SET_POLLING_TIMEOUT(tMALTab[memIdx]->WriteTimeout);
        }
        else
        {
            SET_POLLING_TIMEOUT(tMALTab[memIdx]->EraseTimeout);
        }

        return MAL_OK;
    }
    else
    {
        return MAL_FAIL;
    }
}

/*!
    \brief      check the address is supported
    \param[in]  Addr: memory sector address/code
    \param[out] none
    \retval     index of the addressed memory
*/
static uint8_t  DFU_MAL_CheckAddr (uint32_t Addr)
{
    uint32_t memIdx = 0;

    /* Check with all supported memories */
    for(memIdx = 0; memIdx < MAX_USED_MEMORY_MEDIA; memIdx++)
    {
        /* If the check address is supported, return the memory index */
        if (tMALTab[memIdx]->pMAL_CheckAdd(Addr) == MAL_OK)
        {
            return memIdx;
        }
    }

    /* If there is no memory found, return MAX_USED_MEDIA */
    return (MAX_USED_MEMORY_MEDIA);
}
