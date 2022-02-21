#include "usart.h"

static USART_STATE_MACHINE_STRUCT USART2_TX_STATE_MACHINE;
static USART_STATE_MACHINE_STRUCT USART2_RX_STATE_MACHINE;
static char store_str[80];

static uint32_t rxDataUSART = 0;


void txbl_recevied(USART_TypeDef *usart);
void txc_recevied(USART_TypeDef *usart);
void rxdatav_recevied(USART_TypeDef *usart);
void usart_open(USART_TypeDef *usart, USART_OPEN_STRUCT *usart_setup);
void usart_start(USART_TypeDef *usart, char *string, uint32_t string_len);

void ble_usart_open(void){
  USART_OPEN_STRUCT ble_usart;

  ble_usart.baudrate = 9600;
  ble_usart.parity = usartNoParity;
  ble_usart.stopbits = usartStopbits1;
  ble_usart.refFreq = 0;
  ble_usart.databits = usartDatabits8;

  ble_usart.rx_loc = USART_ROUTELOC0_RXLOC_LOC1;
  ble_usart.tx_loc = USART_ROUTELOC0_TXLOC_LOC1;

  usart_open(BLE_USART, &ble_usart);
}


void ble_write(char* string){
  strcpy(store_str, string);
  usart_start(BLE_USART, store_str, strlen(store_str));
}

bool ble_newData(void){
  return (bool)rxDataUSART;
}

void ble_read(char * str){
  strcpy(str, USART2_RX_STATE_MACHINE.string);
  rxDataUSART = 0;
}

void usart_open(USART_TypeDef *usart, USART_OPEN_STRUCT *usart_setup){
  if(usart == USART0){
      CMU_ClockEnable(cmuClock_USART0, true);
      NVIC_EnableIRQ(USART0_RX_IRQn);
      NVIC_EnableIRQ(USART0_TX_IRQn);
  }
  if(usart == USART1){
      CMU_ClockEnable(cmuClock_USART1, true);
      NVIC_EnableIRQ(USART1_RX_IRQn);
      NVIC_EnableIRQ(USART1_TX_IRQn);
  }
  if(usart == USART2){
      CMU_ClockEnable(cmuClock_USART2, true);
      NVIC_EnableIRQ(USART2_RX_IRQn);
      NVIC_EnableIRQ(USART2_TX_IRQn);
  }
  if(usart == USART3){
      CMU_ClockEnable(cmuClock_USART3, true);
      NVIC_EnableIRQ(USART3_RX_IRQn);
      NVIC_EnableIRQ(USART3_TX_IRQn);
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

  usart->IEN = USART_IEN_RXDATAV;
  USART2_RX_STATE_MACHINE.counter = 0;

}

void USART2_RX_IRQHandler(void){
  uint32_t int_flag;
  int_flag = USART2->IF & USART2->IEN;
  USART2->IFC = int_flag;

  if(int_flag & USART_IF_RXDATAV){
    rxdatav_recevied(USART2);
  }

}

void USART2_TX_IRQHandler(void){
  uint32_t int_flag;
  int_flag = USART2->IF & USART2->IEN;
  USART2->IFC = int_flag;

  if(int_flag & USART_IF_TXBL){
    txbl_recevied(USART2);
  }
  if(int_flag & USART_IF_TXC){
    EFM_ASSERT(!(USART2->IF & USART_IF_TXC));
    txc_recevied(USART2);
  }
}

/***************************************************************************//**
 * @brief
 *
 ******************************************************************************/

void usart_start(USART_TypeDef *usart, char *string, uint32_t string_len){
  while(usart_tx_busy(usart));

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  USART2_TX_STATE_MACHINE.sm_busy = true;
  USART2_TX_STATE_MACHINE.string_len = string_len;
  strcpy(USART2_TX_STATE_MACHINE.string, string);
  USART2_TX_STATE_MACHINE.counter = 0;
  USART2_TX_STATE_MACHINE.usart = usart;
  USART2_TX_STATE_MACHINE.curr_state = transmit;

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
void txbl_recevied(USART_TypeDef *usart){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  switch(USART2_TX_STATE_MACHINE.curr_state){
    case transmit:
      usart->TXDATA = USART2_TX_STATE_MACHINE.string[USART2_TX_STATE_MACHINE.counter];
      USART2_TX_STATE_MACHINE.counter = USART2_TX_STATE_MACHINE.counter + 1;
      if(USART2_TX_STATE_MACHINE.counter == USART2_TX_STATE_MACHINE.string_len){
        usart->IEN &= ~USART_IEN_TXBL;
        USART2_TX_STATE_MACHINE.curr_state = wait_finish;
        usart->IEN |= USART_IEN_TXC;
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
void txc_recevied(USART_TypeDef *usart){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  switch(USART2_TX_STATE_MACHINE.curr_state){
    case wait_finish:
      USART2_TX_STATE_MACHINE.curr_state = idle;
      usart->IEN &= ~USART_IEN_TXC;
      USART2_TX_STATE_MACHINE.sm_busy = false;
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
void rxdatav_recevied(USART_TypeDef *usart){
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  uint32_t rx;
    rx = usart->RXDATA;

    switch(USART2_RX_STATE_MACHINE.curr_state){
    case idle:
      if(rx == BLE_STARTFRAME){
          // could add protection based on rxDataUSART
          USART2_RX_STATE_MACHINE.curr_state = receive_data;
          USART2_RX_STATE_MACHINE.counter = 0;
      }
      break;
    case receive_data:
      if(rx == BLE_SIGFRAME){
          USART2_RX_STATE_MACHINE.curr_state = idle;
          USART2_RX_STATE_MACHINE.string[USART2_RX_STATE_MACHINE.counter] = 0;
          usart->CMD = USART_CMD_CLEARRX;
          rxDataUSART = 1;
      }
      else{
          USART2_RX_STATE_MACHINE.string[USART2_RX_STATE_MACHINE.counter] = rx;
          USART2_RX_STATE_MACHINE.counter++;
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
  if(usart == USART2)
    return USART2_TX_STATE_MACHINE.sm_busy;
  else return false;
}


