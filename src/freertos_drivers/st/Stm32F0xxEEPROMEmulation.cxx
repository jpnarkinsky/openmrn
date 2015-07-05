/** \copyright
 * Copyright (c) 2015, Stuart W Baker
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are  permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \file Stm32F0xxEEPROMEmulation.cxx
 * This file implements STM32F0xx compatible EEPROM emulation in FLASH.
 *
 * @author Stuart W. Baker
 * @date 24 June 2015
 */

#include "Stm32F0xxEEPROMEmulation.hxx"

#include <cstring>

#include "stm32f0xx_hal_flash.h"

#if defined (STM32F030x6) || defined (STM32F031x6) || defined (STM32F038xx) \
 || defined (STM32F030x8) || defined (STM32F030xC) || defined (STM32F042x6) \
 || defined (STM32F048xx) || defined (STM32F051x8) || defined (STM32F058xx)
const size_t __attribute__((weak)) EEPROMEmulation::SECTOR_SIZE = 0x400;
#elif defined (STM32F070x6) || defined (STM32F070xB) || defined (STM32F071xB) \
   || defined (STM32F072xB) || defined (STM32F078xx) \
   || defined (STM32F091xC) || defined (STM32F098xx)
const size_t __attribute__((weak)) EEPROMEmulation::SECTOR_SIZE = 0x800;
#endif
const size_t EEPROMEmulation::BLOCK_SIZE = 4;
const size_t EEPROMEmulation::BYTES_PER_BLOCK = 2;
const uintptr_t Stm32EEPROMEmulation::FLASH_START = 0x08000000;


/** Constructor.
 * @param name device name
 * @param file_size maximum file size that we can grow to.
 */
Stm32EEPROMEmulation::Stm32EEPROMEmulation(const char *name, size_t file_size)
    : EEPROMEmulation(name, file_size)
{
    mount();
}

/** Simple hardware abstraction for FLASH erase API.
 * @param address the start address of the flash block to be erased
 */
void Stm32EEPROMEmulation::flash_erase(void *address)
{

    HASSERT(((uintptr_t)address % SECTOR_SIZE) == 0);
    HASSERT((uintptr_t)address >= (uintptr_t)&__eeprom_start);
    HASSERT((uintptr_t)address < (uintptr_t)(&__eeprom_start + (FLASH_SIZE >> 1)));

    uint32_t page_error;
    FLASH_EraseInitTypeDef erase_init;
    erase_init.TypeErase = TYPEERASE_PAGES;
    erase_init.PageAddress = (uint32_t)address;
    erase_init.NbPages = 1;

    portENTER_CRITICAL();
    HAL_FLASH_Unlock();
    HAL_FLASHEx_Erase(&erase_init, &page_error);
    HAL_FLASH_Lock();
    portEXIT_CRITICAL();
}

/** Simple hardware abstraction for FLASH program API.
 * @param data a pointer to the data to be programmed
 * @param address the starting address in flash to be programmed.
 *                Must be a multiple of BLOCK_SIZE
 * @param count the number of bytes to be programmed.
 *              Must be a multiple of BLOCK_SIZE
 */
void Stm32EEPROMEmulation::flash_program(uint32_t *data, void *address,
                                         uint32_t count)
{
    HASSERT(((uintptr_t)address % BLOCK_SIZE) == 0);
    HASSERT((uintptr_t)address >= (uintptr_t)&__eeprom_start);
    HASSERT((uintptr_t)address < (uintptr_t)(&__eeprom_start + (FLASH_SIZE >> 1)));
    HASSERT((count % BLOCK_SIZE) == 0);
    HASSERT(count <= WRITE_SIZE);

    uintptr_t uint_address = (uintptr_t)address;

    while (count)
    {
        portENTER_CRITICAL();
        HAL_FLASH_Unlock();
        HAL_FLASH_Program(TYPEPROGRAM_WORD, uint_address, *data);
        HAL_FLASH_Lock();
        portEXIT_CRITICAL();

        count -= BLOCK_SIZE;
        uint_address += sizeof(uint32_t);
        ++data;
    }
}


/** Lookup sector number from address.
 * @param address sector address;
 * @return sector number.
 */
int Stm32EEPROMEmulation::address_to_sector(const void *address)
{
    const uintptr_t uint_address = (const uintptr_t)address;

    int sector = (uint_address - FLASH_START) / SECTOR_SIZE;

#if defined (STM32F030x6) || defined (STM32F031x6) || defined (STM32F038xx) \
 || defined (STM32F030x8) || defined (STM32F030xC) || defined (STM32F042x6) \
 || defined (STM32F048xx) || defined (STM32F051x8) || defined (STM32F058xx)
    if (sector >= 64)
#elif defined (STM32F070x6) || defined (STM32F070xB) || defined (STM32F071xB) \
   || defined (STM32F072xB) || defined (STM32F078xx) \
   || defined (STM32F091xC) || defined (STM32F098xx)
    if (sector >= 128)
#endif
    {
        HASSERT(0);
    }

    return sector;
}

/** Lookup address number from sector number.
 * @param sector sector number;
 * @return sector address.
 */
uint32_t *Stm32EEPROMEmulation::sector_to_address(const int sector)
{
#if defined (STM32F030x6) || defined (STM32F031x6) || defined (STM32F038xx) \
 || defined (STM32F030x8) || defined (STM32F030xC) || defined (STM32F042x6) \
 || defined (STM32F048xx) || defined (STM32F051x8) || defined (STM32F058xx)
    if (sector < 64)
#elif defined (STM32F070x6) || defined (STM32F070xB) || defined (STM32F071xB) \
   || defined (STM32F072xB) || defined (STM32F078xx) \
   || defined (STM32F091xC) || defined (STM32F098xx)
    if (sector < 128)
#endif
    {
        return (uint32_t*)(FLASH_START + (sector * SECTOR_SIZE));
    }
    else
    {
        HASSERT(0);
    }

    return NULL;
}