/*!
 *  \file    uart.c
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

#include <avr/io.h>
#include <math.h>
#include "avr_compiler.h"
#include "usart_driver.h"
#include "uart.h"

/*! \brief Get a byte from the circular receive buffer
 *
 *  \param  uart      pointer to UART datastructure with buffers
 *
 *  \return received byte from circulair buffer (low byte) or
 *          UART_NO_DATA if buffer is empty
 */
uint16_t uart_getc(USART_data_t *uart)
{
  uint8_t data;

  if ( ! USART_RXBufferData_Available(uart) ) {
    return UART_NO_DATA;
  }

  data = USART_RXBuffer_GetByte(uart);

  return (data & 0x00FF);
}

/*! \brief Write a byte to the circular transmit buffer
 *
 *  \param  uart      pointer to UART datastructure with buffers
 *  \param  data      byte to be written
 *
 *  \return void
 */
void uart_putc(USART_data_t *uart, uint8_t data)
{
  if ( USART_TXBuffer_FreeSpace(uart) ) {
    USART_TXBuffer_PutByte(uart, data);
  }
}

/*! \brief Write a string to the circulair transmit buffer
 *
 *  \param  uart      pointer to UART datastructure with buffers
 *  \param  s         pointer to string to be written
 *
 *  \return void
 */
void uart_puts(USART_data_t *uart, char *s)
{
  char c;

  while ( (c = *s++) ) {
    uart_putc(uart, c);
  }
}

/*! \brief Set direction for the transmit and receive pin
 *
 *  \param  usart    pointer to UART datastructure  (UART module)
 *
 *  \return void
 */
void set_usart_txrx_direction(USART_t *usart)
{
  #ifdef USARTC0
   if ( (uint16_t) usart == (uint16_t) &USARTC0 ) {
     PORTC.DIRSET      = PIN3_bm;
     PORTC.DIRCLR      = PIN2_bm;
     return;
   }
  #endif
  #ifdef USARTC1
   if ( (uint16_t) usart == (uint16_t) &USARTC1 ) {
     PORTC.DIRSET      = PIN7_bm;
     PORTC.DIRCLR      = PIN6_bm;
     return;
   }
  #endif
  #ifdef USARTD0
   if ( (uint16_t) usart == (uint16_t) &USARTD0) {
     PORTD.DIRSET      = PIN3_bm;
     PORTD.DIRCLR      = PIN2_bm;
     return;
   }
  #endif
  #ifdef USARTD1
   if ( (uint16_t) usart == (uint16_t) &USARTD1 ) {
     PORTD.DIRSET      = PIN7_bm;
     PORTD.DIRCLR      = PIN6_bm;
     return;
   }
  #endif
  #ifdef USARTE0
   if ( (uint16_t) usart == (uint16_t) &USARTE0) {
     PORTE.DIRSET      = PIN3_bm;
     PORTE.DIRCLR      = PIN2_bm;
     return;
   }
  #endif
  #ifdef USARTE1
   if ( (uint16_t) usart == (uint16_t) &USARTE1) {
     PORTE.DIRSET      = PIN7_bm;
     PORTE.DIRCLR      = PIN6_bm;
     return;
   }
  #endif
  #ifdef USARTF0
   if ( (uint16_t) usart == (uint16_t) &USARTF0) {
     PORTF.DIRSET      = PIN3_bm;
     PORTF.DIRCLR      = PIN2_bm;
     return;
   }
  #endif
  #ifdef USARTF1
   if ( (uint16_t) usart == (uint16_t) &USARTF1) {
     PORTF.DIRSET      = PIN7_bm;
     PORTF.DIRCLR      = PIN6_bm;
     return;
   }
  #endif
}

/*! \brief Calculates the baud rate value BSEL
 *
 *  \param  f_cpu       system clock (F_CPU)
 *  \param  baud        desired baud rate
 *  \param  scale       scale factor (BSCALE)
 *  \param  clk2x       clock speed double (1 for double, 0 for no double)
 *
 *  It calculates the baud selection value BSEL from the system clock,
 *  the baud rate, the scale factor and a boolean for clock doubling.
 *
 *  The formula to calculate BSEL is:
 *  \f{eqnarray*}{
 *      \mbox{BSCALE}>=0\quad &:& \quad
 *      \mbox{BSEL} =  \frac{f_{\mbox{cpu}}}{N\ 2^{\mbox{BSCALE}}\ f_{\mbox{baud}}} - 1 \\[3pt]
 *      \mbox{BSCALE}<0\quad  &:& \quad
 *      \mbox{BSEL} =  \frac{1}{2^{\mbox{BSCALE}}}\
 *                     \left( \frac{f_{\mbox{cpu}}}{N\ f_{\mbox{baud}}} - 1 \right)
 *  \f}
 *  N is a factor which is 16 with no clock doubling and 8 with clock doubling
 *
 *  \return the calculated BSEL
 */
uint16_t calc_bsel(uint32_t f_cpu, uint32_t baud, int8_t scale, uint8_t clk2x)
{
  uint8_t factor = 16;

  factor = factor >> (clk2x & 0x01);
  if ( scale < 0 ) {
    return round(  (((double)(f_cpu)/(factor*(double)(baud))) - 1) * (1<<-(scale))  );
  } else {
    return round(  ((double)(f_cpu)/(factor*(double)(baud))/(1<<(scale))) - 1);
  }
}

/*! \brief Determines the scale factor BSCALE
 *
 *  \param  f_cpu       system clock (F_CPU)
 *  \param  baud        desired baud rate
 *  \param  clk2x       clock speed double (1 for double, 0 for no double)
 *
 *  It determines the scale factor BSCALE from the system clock, the baud rate,
 *  and a boolean for clock doubling.
 *
 *  \return the scale factor BSCALE
 */
int8_t calc_bscale(uint32_t f_cpu, uint32_t baud, uint8_t clk2x)
{
  int8_t   bscale;
  uint16_t bsel;

  for (bscale = -7; bscale<8; bscale++) {
    if ( (bsel = calc_bsel(f_cpu, baud, bscale, clk2x)) < 4096 ) return bscale;
  }

  return bscale;
}

/*! \brief Initializes the UART
 *
 *  \param  uart    pointer to a UART datastructure with buffers
 *  \param  usart   pointer to a UART datastructure
 *  \param  f_cpu   system clock (F_CPU)
 *  \param  baud    desired baud rate
 *  \param  clk2x   clock speed double (1 for double, 0 for no double)
 *
 *  It calculates the scale factor BSCALE and the baud selection value BSEL.
 *  It selects what USART module to use and it initializes receive and transmit buffer.
 *  It initializes the USART module and sets the direction of TXD and RXD pin.
 *  The interrupt levels of the DRE interrupt function and the RXC interrupt function
 *  are both set to a low level.
 *
 *  \return void
 */
void init_uart(USART_data_t *uart, USART_t *usart, uint32_t f_cpu, uint32_t baud, uint8_t clk2x)
{
  uint16_t bsel;
  int8_t bscale;

  bscale = calc_bscale(f_cpu, baud, clk2x);
  bsel   = calc_bsel(f_cpu, baud, bscale, clk2x);

  USART_InterruptDriver_Initialize(uart, usart, USART_DREINTLVL_LO_gc);
  USART_Format_Set(uart->usart, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, !USART_SBMODE_bm);
  USART_Rx_Enable(uart->usart);
  USART_Tx_Enable(uart->usart);
  USART_RxdInterruptLevel_Set(uart->usart, USART_RXCINTLVL_LO_gc);
  USART_Baudrate_Set(uart->usart, bsel, bscale);

  set_usart_txrx_direction(uart->usart);
}

/*! \brief Initializes the UART
 *
 *  \param  uart    pointer to a UART datastructure with buffers
 *  \param  usart   pointer to a UART datastructure (UART module)
 *  \param  f_cpu   system clock (F_CPU)
 *  \param  baud    desired baud rate
 *  \param  clk2x   clock speed double (1 for double, 0 for no double)
 *  \param  rxcIntLevel   RXC interrupt level
 *  \param  dreIntLevel   DRE interrupt level
 *
 *  It calculates the scale factor BSCALE and the baud selection value BSEL.
 *  It selects what USART module to use and it initializes receive and transmit buffer.
 *  It initializes the USART module and sets the direction of TXD and RXD pin.
 *  The interrupt level of the DRE interrupt function and the RXC interrupt function
 *  are set to \em dreIntLevel and \em rxcIntLevel.
 *
 *  \return void
 */
void init_uart_levels(USART_data_t *uart, USART_t *usart,
                      uint32_t f_cpu, uint32_t baud, uint8_t clk2x,
                      USART_RXCINTLVL_t  rxcIntLevel, USART_DREINTLVL_t dreIntLevel)
{
  uint16_t bsel;
  int8_t bscale;

  bscale = calc_bscale(f_cpu, baud, clk2x);
  bsel   = calc_bsel(f_cpu, baud, bscale, clk2x);

  USART_InterruptDriver_Initialize(uart, usart, dreIntLevel);
  USART_Format_Set(uart->usart, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, !USART_SBMODE_bm);
  USART_Rx_Enable(uart->usart);
  USART_Tx_Enable(uart->usart);
  USART_RxdInterruptLevel_Set(uart->usart, rxcIntLevel);
  USART_Baudrate_Set(uart->usart, bsel, bscale);

  set_usart_txrx_direction(uart->usart);
}