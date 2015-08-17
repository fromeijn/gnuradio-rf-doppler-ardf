/*!
 *  \file    uart.h
 *  \author  Wim Dolman (<a href="email:w.e.dolman@hva.nl">w.e.dolman@hva.nl</a>)
 *  \date    29-11-2013
 *  \version 1.1
 *
 *  \brief   Wrapper for UART-driver of Atmel for Xmega
 *
 *  \details The files uart.c and uart.h uses the UART-driver uart_driver.c and
 *           uart_driver.h from Atmel, see
 *           <a href="http://www.atmel.com/Images/doc8049.pdf">http://www.atmel.com/Images/doc8049.pdf</a>
 *           and  <a href="http://www.atmel.com/Images/AVR1307.zip">http://www.atmel.com/Images/AVR1307.zip</a>.
 *
 *           It is a kind of wrapper with the same style as the UART-drivers from
 *           Peter Fleury (<a href="http://jump.to/fleury">http://jump.to/fleury</a>)
 *           for the ATmega.
 *
 *           For every buffered UART we need two Interrupt Service Routine:
 *           one for receiving and one for transmitting
 *           For each UART there is a condition.
 *           If the condition is true the two ISR's will be compiled,
 *           else they will be  skipped.
 *           The condition for UART \em uart_id depends on a macro ENABLE_UART_\em uart_id
 *
 *           To use, for example, UARTC1 define in the code just before including uart.h
 *           a macro ENABLE_UART_C1:
 * \verbatim
      #define ENABLE_UART_C1 1
      #include <uart.h>
   \endverbatim
 *           \par
 *
 *           \note An AVR-project can use multiple UART's. One shoud take care that
 *           in different source files there are no multiple ENABLE_UART_\em uart_id
 *           definitions for the same UART.
 */
#ifndef UART_H
#define UART_H
#endif

#ifndef COMPILER_AVR_H
#include "avr_compiler.h"
#endif

#ifndef USART_DRIVER_H
#include "usart_driver.h"
#endif

/*!
 * \brief Macro UART_NO_DATA is returned by uart_getc when no data is present
 */
#define UART_NO_DATA          0x0100

uint16_t calc_bsel(uint32_t f_cpu, uint32_t baud, int8_t scale, uint8_t clk2x);
uint16_t uart_getc(USART_data_t *uart);
void uart_putc(USART_data_t *uart, uint8_t data);
void uart_puts(USART_data_t *uart, char *s);
void set_usart_txrx_direction(USART_t *usart);
void init_uart(USART_data_t *uart, USART_t *usart, uint32_t f_cpu, uint32_t baud, uint8_t clk2x);
void init_uart_levels(USART_data_t *uart, USART_t *usart,
                      uint32_t f_cpu, uint32_t baud, uint8_t clk2x,
                      USART_RXCINTLVL_t  rxcIntLevel, USART_DREINTLVL_t dreIntLevel);

#if ENABLE_UART_C0
/*!
 *  \brief Global declaration uart with databuffers for UARTC0.
 *         This variable is only defined if the macro ENABLE_UART_C0 is defined.
 */
USART_data_t uartC0;

/*!
 *  \brief Interrupt Service Routine for receiving with UARTC0.
 *         This ISR is only defined if the macro ENABLE_UART_C0 is defined.
 */
ISR(USARTC0_RXC_vect)
{
  USART_RXComplete(&uartC0.usart);
}

/*!
 *  \brief Interrupt Service Routine for transmitting with UARTC0.
 *         This ISR is only defined if the macro ENABLE_UART_C0 is defined.
 */
ISR(USARTC0_DRE_vect)
{
  USART_DataRegEmpty(&uartC0.usart);
}
#endif


#if ENABLE_UART_C1
/*!
 *  \brief Global declaration uart with databuffers for UARTC1.
 *         This variable is only defined if the macro ENABLE_UART_C0 is defined.
 *
 */
USART_data_t uartC1;

/*!
 *  \brief Interrupt Service Routine for receiving with UARTC1.
 *         This ISR is only defined if the macro ENABLE_UART_C1 is defined.
 */
ISR(USARTC1_RXC_vect)
{
  USART_RXComplete(&uartC1);
}

/*!
 *  \brief Interrupt Service Routine for transmitting with UARTC1.
 *         This ISR is only defined if the macro ENABLE_UART_C1 is defined.
 */
ISR(USARTC1_DRE_vect)
{
  USART_DataRegEmpty(&uartC1);
}
#endif


#if ENABLE_UART_D0
/*!
 *  \brief Global declaration uart with databuffers for UARTD0.
 *         This variable is only defined if the macro ENABLE_UART_D0 is defined.
 */
USART_data_t uartD0;

/*!
 *  \brief Interrupt Service Routine for receiving with UARTD0.
 *         This ISR is only defined if the macro ENABLE_UART_D0 is defined.
 */
ISR(USARTD0_RXC_vect)
{
  USART_RXComplete(&uartD0);
}

/*!
 *  \brief Interrupt Service Routine for transmitting with UARTD0.
 *         This ISR is only defined if the macro ENABLE_UART_D0 is defined.
 */
ISR(USARTD0_DRE_vect)
{
  USART_DataRegEmpty(&uartD0);
}
#endif


#if ENABLE_UART_D1
/*!
 *  \brief Global declaration uart with databuffers for UARTD1.
 *         This variable is only defined if the macro ENABLE_UART_D1 is defined.
 */
USART_data_t uartD1;

/*!
 *  \brief Interrupt Service Routine for receiving with UARTD1.
 *         This ISR is only defined if the macro ENABLE_UART_D1 is defined.
 */
ISR(USARTD1_RXC_vect)
{
  USART_RXComplete(&uartD1);
}

/*!
 *  \brief Interrupt Service Routine for transmitting with UARTD1.
 *         This ISR is only defined if the macro ENABLE_UART_D1 is defined.
 */
ISR(USARTD1_DRE_vect)
{
  USART_DataRegEmpty(&uartD1);
}
#endif


#if ENABLE_UART_E0
/*!
 *  \brief Global declaration uart with databuffers for UARTE0.
 *         This variable is only defined if the macro ENABLE_UART_E0 is defined.
 */
USART_data_t uartE0;

/*!
 *  \brief Interrupt Service Routine for receiving with UARTE0.
 *         This ISR is only defined if the macro ENABLE_UART_E0 is defined.
 */
ISR(USARTE0_RXC_vect)
{
  USART_RXComplete(&uartE0);
}

/*!
 *  \brief Interrupt Service Routine for transmitting with UARTE0.
 *         This ISR is only defined if the macro ENABLE_UART_E0 is defined.
 */
ISR(USARTE0_DRE_vect)
{
  USART_DataRegEmpty(&uartE0);
}
#endif


#if ENABLE_UART_E1
/*!
 *  \brief Global declaration uart with databuffers for UARTE1.
 *         This variable is only defined if the macro ENABLE_UART_E1 is defined.
 */
USART_data_t uartE1;

/*!
 *  \brief Interrupt Service Routine for receiving with UARTE1.
 *         This ISR is only defined if the macro ENABLE_UART_E1 is defined.
 */
ISR(USARTE1_RXC_vect)
{
  USART_RXComplete(&uartE1);
}

/*!
 *  \brief Interrupt Service Routine for transmitting with UARTE1.
 *         This ISR is only defined if the macro ENABLE_UART_E1 is defined.
 */
ISR(USARTE1_DRE_vect)
{
  USART_DataRegEmpty(&uartE1);
}
#endif


#if ENABLE_UART_F0
/*!
 *  \brief Global declaration uart with databuffers for UARTF0.
 *         This variable is only defined if the macro ENABLE_UART_F0 is defined
 */
USART_data_t uartF0;

/*!
 *  \brief Interrupt Service Routine for receiving with UARTF0.
 *         This ISR is only defined if the macro ENABLE_UART_F0 is defined
 */
ISR(USARTF0_RXC_vect)
{
  USART_RXComplete(&uartF0);
}

/*!
 *  \brief Interrupt Service Routine for transmitting with UARTF0.
 *         This ISR is only defined if the macro ENABLE_UART_F0 is defined
 */
ISR(USARTF0_DRE_vect)
{
  USART_DataRegEmpty(&uartF0);
}
#endif


#if ENABLE_UART_F1
/*!
 *  \brief Global declaration uart with databuffers for UARTF1.
 *         This variable is only defined if the macro ENABLE_UART_F1 is defined
 */
USART_data_t uartF1;

/*!
 *  \brief Interrupt Service Routine for receiving with UARTF1.
 *         This ISR is only defined if the macro ENABLE_UART_F1 is defined
 */
ISR(USARTF1_RXC_vect)
{
  USART_RXComplete(&uartF1);
}

/*!
 *  \brief Interrupt Service Routine for transmitting with UARTF1.
 *         This ISR is only defined if the macro ENABLE_UART_F1 is defined
 */
ISR(USARTF1_DRE_vect)
{
  USART_DataRegEmpty(&uartF1);
}
#endif