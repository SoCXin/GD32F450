/*!
    \file  flash_if.c
    \brief flash interface functions

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

#include "flash_if.h"
#include "dfu_mal.h"

static uint8_t  Flash_If_Init      (void);
static uint8_t  Flash_If_DeInit    (void);
static uint8_t  Flash_If_Erase     (uint32_t Addr);
static uint8_t  Flash_If_Write     (uint32_t Addr, uint32_t Len);
static uint8_t* Flash_If_Read      (uint32_t Addr, uint32_t Len);
static uint8_t  Flash_If_CheckAddr (uint32_t Addr);

static uint32_t fmc_sector_get (uint32_t address);
static void fmc_erase_sector   (uint32_t fmc_sector);

DFU_MAL_Property_TypeDef DFU_Flash_cb =
{
    (const uint8_t *)FLASH_IF_STRING,

    Flash_If_Init,
    Flash_If_DeInit,
    Flash_If_Erase,
    Flash_If_Write,
    Flash_If_Read,
    Flash_If_CheckAddr,
    60, /* Flash erase timeout in ms */
    80  /* Flash programming timeout in ms (80us * RAM Buffer size (1024 Bytes) */
};

/*!
    \brief      option Byte write routine.
    \param[in]  none
    \param[out] none
    \retval     the state of the FMC after write option Byte operation
*/
fmc_state_enum Option_Byte_Write(uint32_t Mem_Add,uint8_t* data)
{
    uint8_t index;
    fmc_state_enum status ;

    /* unlock the flash program erase controller */
    fmc_unlock();

    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);

    status = fmc_ready_wait();
      
    /* Authorize the small information block programming */
    ob_unlock();
    
    /* start erase the option byte */
    ob_erase();
    
    {
        uint32_t nRST_STDBY = 0;
        uint32_t nRST_DPSLP = 0;
        uint32_t nWDG_HW = 0;
        uint32_t BB = 0;
        uint32_t BOR_TH = 0;
        uint16_t WP0 = 0;
        uint16_t WP1 = 0;
        uint32_t reg0 = FMC_OBCTL0;
        uint32_t reg1 = FMC_OBCTL1;
        uint32_t reg = 0;
        fmc_state_enum fmc_state = FMC_READY;
        
        ob_start();
        
        if (((data[0]>>7) & 0x01) == 1) {
            nRST_STDBY = OB_STDBY_NRST;
        } else {
            nRST_STDBY = OB_STDBY_RST;
        }
        
        if (((data[0]>>6) & 0x01) == 1) {
            nRST_DPSLP = OB_DEEPSLEEP_NRST;
        } else {
            nRST_DPSLP = OB_DEEPSLEEP_RST;
        }
        
        if (((data[0]>>5) & 0x01) == 1) {
            nWDG_HW = OB_FWDGT_SW;
        } else {
            nWDG_HW = OB_FWDGT_HW;
        }
        
        ob_user_write(nWDG_HW,nRST_DPSLP,nRST_STDBY);
        
        ob_security_protection_config(data[1]);
        
        reg0 = FMC_OBCTL0;
        reg1 = FMC_OBCTL1;
        if (((data[0]>>2) & 0x03) == 0) {
            BOR_TH = OB_BOR_TH_VALUE3;
        } else if (((data[0]>>2) & 0x03) == 1) {
            BOR_TH = OB_BOR_TH_VALUE2;
        } else if (((data[0]>>2) & 0x03) == 2) {
            BOR_TH = OB_BOR_TH_VALUE1;
        } else {
            BOR_TH = OB_BOR_TH_OFF;
        }
        ob_user_bor_threshold(BOR_TH);
        
        if (((data[0]>>4) & 0x01) == 1) {
            BB = OB_BB_ENABLE;
        } else {
            BB = OB_BB_DISABLE;
        }
        ob_boot_mode_config(BB);
        
        WP0 = (data[8] | (data[9] << 8)) & 0x0FFF;
        WP1 = (data[24] | (data[25] << 8)) & 0x0FFF;
        
        if(RESET != (FMC_OBCTL0 & FMC_OBCTL0_DRP)){
            while(1){
            }
        }
        /* wait for the FMC ready */
        fmc_state = fmc_ready_wait();
        
        reg0 &= (~((uint32_t)0x0FFF << 16));
        reg1 &= (~((uint32_t)0x0FFF << 16));
        
        if(FMC_READY == fmc_state){
            reg = (WP0 << 16);
            FMC_OBCTL0 = reg0 | reg;
            reg = (WP1 << 16);
            FMC_OBCTL1 = reg1 | reg;
        }
        
        ob_start();
    }

    fmc_lock();

    return status;
}

/**
  * @brief  Flash memory interface initialization routine.
  * @param  None
  * @retval MAL_OK if the operation is right, MAL_FAIL else
*/
static uint8_t  Flash_If_Init (void)
{
    /* Unlock the internal flash */
    fmc_unlock();

    return MAL_OK;
}

/**
  * @brief  Flash memory interface deinitialization routine.
  * @param  None
  * @retval MAL_OK if all operation is right, MAL_FAIL else
  */
static uint8_t  Flash_If_DeInit (void)
{
    /* Lock the internal flash */
    fmc_lock();

    return MAL_OK;
}

/**
  * @brief  Erase flash sector
  * @param  Addr: address to be written to.
  * @retval MAL_OK
  */
static uint8_t  Flash_If_Erase (uint32_t Addr)
{
    uint32_t start_sector = 0;

    /* get the number of sector */
    start_sector = fmc_sector_get(Addr);

    /* erase the sector */
    fmc_erase_sector(start_sector);

    return MAL_OK;
}

/**
  * @brief  Flash memory write routine
  * @param  Addr: address to be written to
  * @param  Len: length of data to be written (in bytes).
  * @retval MAL_OK
  */
static uint8_t  Flash_If_Write (uint32_t Addr, uint32_t Len)
{
    uint32_t idx = 0;

    /* unlock the flash program erase controller */
    fmc_unlock();

    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);

    if (Len & 0x03) /* Not an aligned data */
    {
        for (idx = Len; idx < ((Len & 0xFFFC) + 4); idx++)
        {
            MAL_Buffer[idx] = 0xFF;
        }
    }

    /* Data received are word multiple */
    for (idx = 0; idx < Len; idx += 4)
    {
        fmc_word_program(Addr, *(uint32_t *)(MAL_Buffer + idx));
        Addr += 4;
    }

    fmc_lock();

    return MAL_OK;
}

/**
  * @brief  Flash memory read routine
  * @param  Addr: address to be read from
  * @param  Len: length of data to be read (in bytes)
  * @retval Pointer to the physical address where data should be read
  */
static uint8_t *Flash_If_Read (uint32_t Addr, uint32_t Len)
{
    return  (uint8_t *)(Addr);
}

/**
  * @brief  Check if the address is an allowed address for this memory
  * @param  Addr: address to be checked.
  * @retval MAL_OK if the address is allowed, MAL_FAIL else.
  */
static uint8_t  Flash_If_CheckAddr (uint32_t Addr)
{
    if ((Addr >= FLASH_START_ADDR) && (Addr < FLASH_END_ADDR))
    {
        return MAL_OK;
    }
    else
    {
        return MAL_FAIL;
    }
}

/*!
    \brief      erases the sector of a given sector number
    \param[in]  fmc_sector: a given sector number
      \arg        CTL_SECTOR_NUMBER_0:  sector 0
      \arg        CTL_SECTOR_NUMBER_1:  sector 1
      \arg        CTL_SECTOR_NUMBER_2:  sector 2
      \arg        CTL_SECTOR_NUMBER_3:  sector 3
      \arg        CTL_SECTOR_NUMBER_4:  sector 4
      \arg        CTL_SECTOR_NUMBER_5:  sector 5
      \arg        CTL_SECTOR_NUMBER_6:  sector 6
      \arg        CTL_SECTOR_NUMBER_7:  sector 7
      \arg        CTL_SECTOR_NUMBER_8:  sector 8
      \arg        CTL_SECTOR_NUMBER_9:  sector 9
      \arg        CTL_SECTOR_NUMBER_10: sector 10
      \arg        CTL_SECTOR_NUMBER_11: sector 11
    \param[out] none
    \retval     none
*/
static void fmc_erase_sector(uint32_t fmc_sector)
{
    fmc_state_enum state;

    /* unlock the flash program erase controller */
    fmc_unlock();

    /* clear pending flags */
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);

    fmc_ready_wait();

    state = fmc_sector_erase(fmc_sector);

    /* wait the erase operation complete*/
    if (FMC_READY != state) {
        while(1);
    }

    /* lock the flash program erase controller */
    fmc_lock();
}

/*!
    \brief      gets the sector of a given address
    \param[in]  address: a given address(0x08000000~0x08300000)
    \param[out] none
    \retval     the sector of a given address
*/
static uint32_t fmc_sector_get(uint32_t addr)
{
    uint32_t sector = 0;

    if ((addr < ADDR_FMC_SECTOR_1) && (addr >= ADDR_FMC_SECTOR_0)) {
        sector = CTL_SECTOR_NUMBER_0;
    } else if ((addr < ADDR_FMC_SECTOR_2) && (addr >= ADDR_FMC_SECTOR_1)) {
        sector = CTL_SECTOR_NUMBER_1;
    } else if ((addr < ADDR_FMC_SECTOR_3) && (addr >= ADDR_FMC_SECTOR_2)) {
        sector = CTL_SECTOR_NUMBER_2;
    } else if ((addr < ADDR_FMC_SECTOR_4) && (addr >= ADDR_FMC_SECTOR_3)) {
        sector = CTL_SECTOR_NUMBER_3;
    } else if ((addr < ADDR_FMC_SECTOR_5) && (addr >= ADDR_FMC_SECTOR_4)) {
        sector = CTL_SECTOR_NUMBER_4;
    } else if ((addr < ADDR_FMC_SECTOR_6) && (addr >= ADDR_FMC_SECTOR_5)) {
        sector = CTL_SECTOR_NUMBER_5;
    } else if ((addr < ADDR_FMC_SECTOR_7) && (addr >= ADDR_FMC_SECTOR_6)) {
        sector = CTL_SECTOR_NUMBER_6;
    } else if ((addr < ADDR_FMC_SECTOR_8) && (addr >= ADDR_FMC_SECTOR_7)) {
        sector = CTL_SECTOR_NUMBER_7;
    } else if ((addr < ADDR_FMC_SECTOR_9) && (addr >= ADDR_FMC_SECTOR_8)) {
        sector = CTL_SECTOR_NUMBER_8;
    } else if ((addr < ADDR_FMC_SECTOR_10) && (addr >= ADDR_FMC_SECTOR_9)) {
        sector = CTL_SECTOR_NUMBER_9;
    } else if ((addr < ADDR_FMC_SECTOR_11) && (addr >= ADDR_FMC_SECTOR_10)) {
        sector = CTL_SECTOR_NUMBER_10;
    } else if ((addr < ADDR_FMC_SECTOR_12) && (addr >= ADDR_FMC_SECTOR_11)) {
        sector = CTL_SECTOR_NUMBER_11;
    } else if ((addr < ADDR_FMC_SECTOR_13) && (addr >= ADDR_FMC_SECTOR_12)) {
        sector = CTL_SECTOR_NUMBER_12;
    } else if ((addr < ADDR_FMC_SECTOR_14) && (addr >= ADDR_FMC_SECTOR_13)) {
        sector = CTL_SECTOR_NUMBER_13;
    } else if ((addr < ADDR_FMC_SECTOR_15) && (addr >= ADDR_FMC_SECTOR_14)) {
        sector = CTL_SECTOR_NUMBER_14;  
    } else if ((addr < ADDR_FMC_SECTOR_16) && (addr >= ADDR_FMC_SECTOR_15)) {
        sector = CTL_SECTOR_NUMBER_15;
    } else if ((addr < ADDR_FMC_SECTOR_17) && (addr >= ADDR_FMC_SECTOR_16)) {
        sector = CTL_SECTOR_NUMBER_16;
    } else if ((addr < ADDR_FMC_SECTOR_18) && (addr >= ADDR_FMC_SECTOR_17)) {
        sector = CTL_SECTOR_NUMBER_17;
    } else if ((addr < ADDR_FMC_SECTOR_19) && (addr >= ADDR_FMC_SECTOR_18)) {
        sector = CTL_SECTOR_NUMBER_18;
    } else if ((addr < ADDR_FMC_SECTOR_20) && (addr >= ADDR_FMC_SECTOR_19)) {
        sector = CTL_SECTOR_NUMBER_19;
    } else if ((addr < ADDR_FMC_SECTOR_21) && (addr >= ADDR_FMC_SECTOR_20)) {
        sector = CTL_SECTOR_NUMBER_20;
    } else if ((addr < ADDR_FMC_SECTOR_22) && (addr >= ADDR_FMC_SECTOR_21)) {
        sector = CTL_SECTOR_NUMBER_21;
    } else if ((addr < ADDR_FMC_SECTOR_23) && (addr >= ADDR_FMC_SECTOR_22)) {
        sector = CTL_SECTOR_NUMBER_22;
    } else if ((addr < ADDR_FMC_SECTOR_24) && (addr >= ADDR_FMC_SECTOR_23)) {
        sector = CTL_SECTOR_NUMBER_23;
    } else if ((addr < ADDR_FMC_SECTOR_25) && (addr >= ADDR_FMC_SECTOR_24)) {
        sector = CTL_SECTOR_NUMBER_24;
    } else if ((addr < ADDR_FMC_SECTOR_26) && (addr >= ADDR_FMC_SECTOR_25)) {
        sector = CTL_SECTOR_NUMBER_25;
    } else if ((addr < ADDR_FMC_SECTOR_27) && (addr >= ADDR_FMC_SECTOR_26)) {
        sector = CTL_SECTOR_NUMBER_26;
    }

    return sector;
}

