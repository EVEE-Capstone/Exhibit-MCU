#include "usart.h"

static USART_STATE_MACHINE_STRUCT USART0_TX_STATE_MACHINE;
static USART_STATE_MACHINE_STRUCT USART0_RX_STATE_MACHINE;
static USART_STATE_MACHINE_STRUCT USART1_TX_STATE_MACHINE;
static USART_STATE_MACHINE_STRUCT USART1_RX_STATE_MACHINE;
static USART_STATE_MACHINE_STRUCT USART2_TX_STATE_MACHINE;
static USART_STATE_MACHINE_STRUCT USART2_RX_STATE_MACHINE;
static USART_STATE_MACHINE_STRUCT USART3_TX_STATE_MACHINE;
static USART_STATE_MACHINE_STRUCT USART3_RX_STATE_MACHINE;


void txbl_recevied(USART_STATE_MACHINE_STRUCT *sm);
void txc_recevied(USART_STATE_MACHINE_STRUCT *sm);
void rxdatav_recevied(USART_STATE_MACHINE_STRUCT *sm);
void usart_open(USART_TypeDef *usart, USART_OPEN_STRUCT *usart_setup);
void usart_start(USART_TypeDef *usart, char *string, uint32_t string_len);

// TODO: Update with correct UART settings
void ard_usart_open(void){
  USART_OPEN_STRUCT ard_usart;

  ard_usart.baudrate = 9600;                // update
  ard_usart.parity = usartNoParity;         // update
  ard_usart.stopbits = usartStopbits1;      // update
  ard_usart.refFreq = 0;
  ard_usart.databits = usartDatabits8;      // update
  ard_usart.rx_en = true;                   // true for testing only
  ard_usart.rx_loc = USART_ROUTELOC0_RXLOC_LOC10;
  ard_usart.tx_loc = USART_ROUTELOC0_TXLOC_LOC10;

  usart_open(ARD_USART, &ard_usart);
}

void ard_write(char* string){
  usart_start(ARD_USART, string, strlen(string));
}

bool ard_newData(void){
  return (bool)USART3_RX_STATE_MACHINE.rxdataValid;
}

void ard_read(char * str){
  strcpy(str, USART3_RX_STATE_MACHINE.string);
  USART3_RX_STATE_MACHINE.rxdataValid = 0;
}


void ble_usart_open(void){
  USART_OPEN_STRUCT ble_usart;

  ble_usart.baudrate = 9600;
  ble_usart.parity = usartNoParity;
  ble_usart.stopbits = usartStopbits1;
  ble_usart.refFreq = 0;
  ble_usart.databits = usartDatabits8;
  ble_usart.rx_en = true;
  ble_usart.rx_loc = USART_ROUTELOC0_RXLOC_LOC1;
  ble_usart.tx_loc = USART_ROUTELOC0_TXLOC_LOC1;

  usart_open(BLE_USART, &ble_usart);
}


void ble_write(char* string){
  usart_start(BLE_USART, string, strlen(string));
}

bool ble_newData(void){
  return (bool)USART2_RX_STATE_MACHINE.rxdataValid;
}

void ble_read(char * str){
  strcpy(str, USART2_RX_STATE_MACHINE.string);
  USART2_RX_STATE_MACHINE.rxdataValid = 0;
}

void usart_open(USART_TypeDef *usart, USART_OPEN_STRUCT *usart_setup){
  if(usart == USART0){
      CMU_ClockEnable(cmuClock_USART0, true);
      NVIC_EnableIRQ(USART0_RX_IRQn);
      NVIC_EnableIRQ(USART0_TX_IRQn);
      USART0_RX_STATE_MACHINE.counter = 0;
      USART0_RX_STATE_MACHINE.rxdataValid = 0;
      USART0_RX_STATE_MACHINE.usart = USART0;
      USART0_TX_STATE_MACHINE.usart = USART0;
  }
  if(usart == USART1){
      CMU_ClockEnable(cmuClock_USART1, true);
      NVIC_EnableIRQ(USART1_RX_IRQn);
      NVIC_EnableIRQ(USART1_TX_IRQn);
      USART1_RX_STATE_MACHINE.counter = 0;
      USART1_RX_STATE_MACHINE.rxdataValid = 0;
      USART1_RX_STATE_MACHINE.usart = USART1;
      USART1_TX_STATE_MACHINE.usart = USART1;
  }
  if(usart == USART2){
      CMU_ClockEnable(cmuClock_USART2, true);
      NVIC_EnableIRQ(USART2_RX_IRQn);
      NVIC_EnableIRQ(USART2_TX_IRQn);
      USART2_RX_STATE_MACHINE.counter = 0;
      USART2_RX_STATE_MACHINE.rxdataValid = 0;
      USART2_RX_STATE_MACHINE.usart = USART2;
      USART2_TX_STATE_MACHINE.usart = USART2;
  }
  if(usart == USART3){
      CMU_ClockEnable(cmuClock_USART3, true);
      NVIC_EnableIRQ(USART3_RX_IRQn);
      NVIC_EnableIRQ(USART3_TX_IRQn);
      USART3_RX_STATE_MACHINE.counter = 0;
      USART3_RX_STATE_MACHINE.rxdataValid = 0;
      USART3_RX_STATE_MACHINE.usart = USART3;
      USART3_TX_STATE_MACHINE.usart = USART3;
  }

  if ((usart->IF & 0x01) == 0) {
    usart->IFS = 0x01;
    EFM_ASSERT(usart->IF & 0x01);
    usart->IFC = 0x01;
  } else {
      usart->IFC = 0x01;
    EFM_ASSERT(!(usart->IF & 0x01));
  }

  USART_Reset(usart);

  USART_InitAsync_TypeDef initAsync = USART_INITASYNC_DEFAULT;
  initAsync.baudrate = usart_setup->baudrate;
  initAsync.enable = usartDisable;
  initAsync.databits = usart_setup->databits;
  initAsync.parity = usart_setup->parity;
  initAsync.stopbits = usart_setup->stopbits;
  initAsync.refFreq = usart_setup->refFreq;
  USART_InitAsync(usart, &initAsync);


  // for testing only
//  usart->CTRL |= USART_CTRL_LOOPBK;

  usart->CMD = USART_CMD_CLEARTX | USART_CMD_CLEARRX;


  usart->ROUTEPEN = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
  usart->ROUTELOC0 = usart_setup->rx_loc | usart_setup->tx_loc;


  USART_Enable(usart, usartEnable);

  if(usart_setup->rx_en){
      usart->IEN = USART_IEN_RXDATAV;
  }

}

/***************************************************************************//**
 * @brief USART0 IRQs
 *
 ******************************************************************************/
void USART0_RX_IRQHandler(void){
  uint32_t int_flag;
  int_flag = USART0->IF & USART0->IEN;
  USART0->IFC = int_flag;

  if(int_flag & USART_IF_RXDATAV){
    rxdatav_recevied(&USART0_RX_STATE_MACHINE);
  }

}

void USART0_TX_IRQHandler(void){
  uint32_t int_flag;
  int_flag = USART0->IF & USART0->IEN;
  USART0->IFC = int_flag;

  if(int_flag & USART_IF_TXBL){
    txbl_recevied(&USART0_TX_STATE_MACHINE);
  }
  if(int_flag & USART_IF_TXC){
    EFM_ASSERT(!(USART0->IF & USART_IF_TXC));
    txc_recevied(&USART0_TX_STATE_MACHINE);
  }
}

/***************************************************************************//**
 * @brief USART1 IRQs
 *
 ******************************************************************************/
void USART1_RX_IRQHandler(void){
  uint32_t int_flag;
  int_flag = USART1->IF & USART1->IEN;
  USART1->IFC = int_flag;

  if(int_flag & USART_IF_RXDATAV){
    rxdatav_recevied(&USART1_RX_STATE_MACHINE);
  }

}

void USART1_TX_IRQHandler(void){
  uint32_t int_flag;
  int_flag = USART1->IF & USART1->IEN;
  USART1->IFC = int_flag;

  if(int_flag & USART_IF_TXBL){
    txbl_recevied(&USART1_TX_STATE_MACHINE);
  }
  if(int_flag & USART_IF_TXC){
    EFM_ASSERT(!(USART1->IF & USART_IF_TXC));
    txc_recevied(&USART1_TX_STATE_MACHINE);
  }
}


/***************************************************************************//**
 * @brief USART2 IRQs
 *
 ******************************************************************************/
void USART2_RX_IRQHandler(void){
  uint32_t int_flag;
  int_flag = USART2->IF & USART2->IEN;
  USART2->IFC = int_flag;

  if(int_flag & USART_IF_RXDATAV){
    rxdatav_recevied(&USART2_RX_STATE_MACHINE);
  }

}

void USART2_TX_IRQHandler(void){
  uint32_t int_flag;
  int_flag = USART2->IF & USART2->IEN;
  USART2->IFC = int_flag;

  if(int_flag & USART_IF_TXBL){
    txbl_recevied(&USART2_TX_STATE_MACHINE);
  }
  if(int_flag & USART_IF_TXC){
    EFM_ASSERT(!(USART2->IF & USART_IF_TXC));
    txc_recevied(&USART2_TX_STATE_MACHINE);
  }
}

/***************************************************************************//**
 * @brief USART3 IRQs
 *
 ******************************************************************************/
void USART3_RX_IRQHandler(void){
  uint32_t int_flag;
  int_flag = USART3->IF & USART3->IEN;
  USART3->IFC = int_flag;

  if(int_flag & USART_IF_RXDATAV){
    rxdatav_recevied(&USART3_RX_STATE_MACHINE);
  }

}

void USART3_TX_IRQHandler(void){
  uint32_t int_flag;
  int_flag = USART3->IF & USART2->IEN;
  USART3->IFC = int_flag;

  if(int_flag & USART_IF_TXBL){
    txbl_recevied(&USART3_TX_STATE_MACHINE);
  }
  if(int_flag & USART_IF_TXC){
    EFM_ASSERT(!(USART3->IF & USART_IF_TXC));
    txc_recevied(&USART3_TX_STATE_MACHINE);
  }
}

/***************************************************************************//**
 * @brief
 *
 ******************************************************************************/
void usart_start(USART_TypeDef *usart, char *string, uint32_t string_len){
  while(usart_tx_busy(usart));
  USART_STATE_MACHINE_STRUCT *sm;
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  if(usart == USART0) sm = &USART0_TX_STATE_MACHINE;
  if(usart == USART1) sm = &USART1_TX_STATE_MACHINE;
  if(usart == USART2) sm = &USART2_TX_STATE_MACHINE;
  if(usart == USART3) sm = &USART3_TX_STATE_MACHINE;

  sm->sm_busy = true;
  sm->string_len = string_len;
  strcpy(sm->string, string);
  sm->counter = 0;
  sm->usart = usart;
  sm->curr_state = transmit;

//  sleep_block_mode(LEUART_TX_EM);

  usart->IEN |= USART_IEN_TXBL;
  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *  Handles TXBL interrupt event. Progresses through USART state machine.
 *
 * @details
 *  Checks current state of USART state machine and performs TXBL response action
 *  if defined. Otherwise enters error.
 *
 * @note
 *  TXBL is only expected in transmit state and is disabled when the end of string
 *  is reached.
 *
 * @param[in] usart
 *  Pointer to usart peripheral
 *
 ******************************************************************************/
void txbl_recevied(USART_STATE_MACHINE_STRUCT *sm){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  switch(sm->curr_state){
    case transmit:
      sm->usart->TXDATA = sm->string[sm->counter];
      sm->counter = sm->counter + 1;
      if(sm->counter == sm->string_len){
        sm->usart->IEN &= ~USART_IEN_TXBL;
        sm->curr_state = wait_finish;
        sm->usart->IEN |= USART_IEN_TXC;
      }
      break;
    default:
      EFM_ASSERT(false);
      break;
    }
  CORE_EXIT_CRITICAL();
}


/***************************************************************************//**
 * @brief
 *  Handles TXC interrupt event. Progresses through USART state machine.
 *
 * @details
 *  Checks current state of USART state machine and performs TXC response action
 *  if defined. Otherwise enters error.
 *
 * @note
 *  TXC is only expected in wait_finish state and closes the uart operation if seen.
 *
 * @param[in] usart
 *  Pointer to usart peripheral
 *
 ******************************************************************************/
void txc_recevied(USART_STATE_MACHINE_STRUCT *sm){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  switch(USART2_TX_STATE_MACHINE.curr_state){
    case wait_finish:
      sm->curr_state = idle;
      sm->usart->IEN &= ~USART_IEN_TXC;
      sm->sm_busy = false;
//      sleep_unblock_mode(LEUART_TX_EM);
//      add_scheduled_event(tx_done_evt);
      break;
    default:
      EFM_ASSERT(false);
      break;
    }
  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *  Handles RXDATAV received event. Progresses through USART state machine.
 *
 * @details
 *  Checks current state of LEAURT state machine and performs RXDATAV response action
 *  if defined. RXDATAV is additionally triggered on sigframe interrupts.
 *
 * @note
 *  SIG FRAME is read and discarded so it does not linger in RXDATAV register and
 *  trigger additional interrupts.
 *
 * @param[in] leaurt
 *  Pointer to USART peripheral
 *
 ******************************************************************************/
void rxdatav_recevied(USART_STATE_MACHINE_STRUCT *sm){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
    uint32_t rx;
    rx = sm->usart->RXDATA;

    switch(sm->curr_state){
    case idle:
      if(rx == BLE_STARTFRAME){
          // could add protection based on rxDataUSART
          sm->curr_state = receive_data;
          sm->counter = 0;
      }
      break;
    case receive_data:
      if(rx == BLE_SIGFRAME){
          sm->curr_state = idle;
          sm->string[sm->counter] = 0;
          sm->usart->CMD = USART_CMD_CLEARRX;
          sm->rxdataValid = 1;
      }
      else{
          sm->string[sm->counter] = rx;
          sm->counter++;
      }
      break;
    default:
      EFM_ASSERT(false);
      break;
    }
    CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * @brief
 *
 ******************************************************************************/

bool usart_tx_busy(USART_TypeDef *usart){
  if(usart == USART0)
    return USART0_TX_STATE_MACHINE.sm_busy;
  else if(usart == USART1)
    return USART1_TX_STATE_MACHINE.sm_busy;
  else if(usart == USART2)
    return USART2_TX_STATE_MACHINE.sm_busy;
  else if(usart == USART3)
    return USART3_TX_STATE_MACHINE.sm_busy;
  else return false;
}


