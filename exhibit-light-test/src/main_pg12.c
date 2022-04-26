/***************************************************************************//**
 * @file main_pg12.c
 * @brief This project reads input on the LEUART RX port and echoes it back to
 * the TX port one line at a time. It idles in EM2. See readme.txt for details.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Evaluation Quality
 * This code has been minimally tested to ensure that it builds and is suitable 
 * as a demonstration for evaluation purposes only. This code will be maintained
 * at the sole discretion of Silicon Labs.
 ******************************************************************************/
#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_leuart.h"
#include "em_chip.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "usart.h"

#define RX_BUFFER_SIZE 80             // Software receive buffer size

/**************************************************************************//**
 * @brief
 *    Initialize the GPIO pins for the LEUART module
 *****************************************************************************/
void initGpio(void)
{
  // GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Initialize LEUART0 TX and RX pins
  GPIO_PinModeSet(gpioPortD, 10, gpioModePushPull, 1); // TX
  GPIO_PinModeSet(gpioPortD, 11, gpioModeInput, 0);    // RX

  // Initialize USART2 TX and RX pins for BLE
  GPIO_PinModeSet(gpioPortA, 6, gpioModePushPull, 1); // TX
  GPIO_PinModeSet(gpioPortA, 7, gpioModeInput, 0);    // RX

  // Initialize USART3 TX and RX pins for ARDUINO
  GPIO_PinModeSet(gpioPortB, 6, gpioModePushPull, 1); // USART3_TX -> PB6 -> expansion 7
  GPIO_PinModeSet(gpioPortB, 7, gpioModeInput, 0);    // USART3_RX -> PB7 -> expansion 9
}

/**************************************************************************//**
 * @brief
 *    Main function
 *
 * @details
 *    Initialize the GPIO and LEUART modules. Send a welcome message over the TX
 *    pin. After the RX handler tells us that we have data, start
 *    processing the received data. Start by disabling RX and TX interrupts so
 *    that the data we received in rxBuffer does not get overwritten by one of
 *    the handlers before we read it. Copy the data in rxBuffer into the
 *    txBuffer and then trigger a TX interrupt to echo it out over the TX pin.
 *    If there is no data availabe, go into EM2 until an RX or TX interrupt
 *    wakes up the CPU.
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initialization
  initGpio();
  ard_usart_open();

  // Send 0 to Arduino to turn all light strip on
  ard_write("0");
  while(usart_tx_busy(USART3));

  for(int i = 0; i < 100000; i++);

  // Send 1 to Arduino to turn all lights on, strip by strip
  ard_write("1");
  while(usart_tx_busy(USART3));

  for(int i = 0; i < 100000; i++);

  // Send 2 to Arduino to turn all lights, flowing strip by strip
  ard_write("2");
  while(usart_tx_busy(USART3));

  for(int i = 0; i < 100000; i++);

  // lowest energy mode we can enter is EM1 due to USART
  EMU_EnterEM1();
}

