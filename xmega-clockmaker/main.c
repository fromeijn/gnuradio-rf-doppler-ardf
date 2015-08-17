//       _____ ____          //
//      |___  |  _ \         //
//         _| | |_) |        //
//        |_  |  _ <         //
//          |_|_| \_\        //
//                           //
// Written By Floris Romeijn //


#define F_CPU 32000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stddef.h>
#include <stdio.h>

#include "avr_compiler.h"
#include "clksys_driver.h"
#define ENABLE_UART_F0    	1
#include "uart.h"
#include "usart_driver.h"

#define PROTO 

#ifdef PROTO

#define LED_ROOD_ON PORTF.OUTSET = PIN0_bm
#define LED_GROEN_ON PORTC.OUTSET = PIN0_bm
#define LED_BLAUW_ON PORTF.OUTSET = PIN1_bm

#define LED_ROOD_OFF PORTF.OUTCLR = PIN0_bm
#define LED_GROEN_OFF PORTC.OUTCLR = PIN0_bm
#define LED_BLAUW_OFF PORTF.OUTCLR = PIN1_bm

#else

#define LED_ROOD_ON PORTF.OUTSET = PIN1_bm
#define LED_GROEN_ON PORTF.OUTSET = PIN0_bm
#define LED_BLAUW_ON PORTC.OUTSET = PIN0_bm

#define LED_ROOD_OFF PORTF.OUTCLR = PIN1_bm
#define LED_GROEN_OFF PORTF.OUTCLR = PIN0_bm
#define LED_BLAUW_OFF PORTC.OUTCLR = PIN0_bm

#endif

static void Init32MhzFrom16MhzExternal(void);
static void EnableAllInterupts(void);
static void InitClockAndDac(void);

char str[256];

int main(void)
{
	PORTC.DIRSET = PIN0_bm;
	PORTF.DIRSET = PIN0_bm | PIN1_bm;

	Init32MhzFrom16MhzExternal();

	EnableAllInterupts();

	for (int i = 0; i < 10; ++i)
	{
		LED_BLAUW_ON;
		_delay_ms(2);
		LED_BLAUW_OFF;
		_delay_ms(20);
	}

	init_uart(&uartF0, &USARTF0, F_CPU, 230400, 0);
	sprintf(str, "\n\r\n\rxmega-clockmaker\n\rlast build: __DATE__ __TIME__ \n\r");
  	uart_puts(&uartF0, str);

  	InitClockAndDac();

	while(1)
	{
		//yolo
	}
}

ISR(TCC0_OVF_vect)
{
	static uint8_t counter;
	counter++;
	if(counter > 3)
		counter = 0;
	PORTD.OUT = 0b11 & counter;
	//debug leds
	PORTF.OUT = 0b11 & counter;

	//ahead for next event
	uint16_t todac = counter + 1;
	if(todac > 3)
		todac = 0;

	DACB.CH0DATA = todac<<10;
	//DACB.CH0DATA = 0xfff;
}


static void InitClockAndDac(void)
{
	PORTD.OUTCLR = PIN0_bm|PIN1_bm;
	PORTD.DIRSET = PIN0_bm|PIN1_bm;

	//TCC0.CTRLA = TC_CLKSEL_DIV256_gc;
	TCC0.CTRLA = TC_CLKSEL_DIV1024_gc;
	TCC0.CTRLB = TC_WGMODE_NORMAL_gc;
	TCC0.CTRLD = TC_EVACT_OFF_gc | TC_EVSEL_OFF_gc;
	TCC0.INTCTRLA = TC_OVFINTLVL_LO_gc;
	EVSYS.CH0MUX = EVSYS_CHMUX_TCC0_OVF_gc;
	//TCC0.PER = 24; //5kHz;
	TCC0.PER = 500000;

	PORTB.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc; /* DAC output; turn off input buffer (may or may not do much good) */

	DACB.CTRLA = DAC_CH0EN_bm | DAC_ENABLE_bm;
	DACB.CTRLB = DAC_CHSEL_SINGLE_gc | DAC_CH0TRIG_bm;
	DACB.CTRLC = DAC_REFSEL_AVCC_gc;
	DACB.EVCTRL = DAC_EVSEL_0_gc;
}


static void Init32MhzFrom16MhzExternal(void)
{
	CLKSYS_XOSC_Config( OSC_FRQRANGE_12TO16_gc, 0, OSC_XOSCSEL_XTAL_16KCLK_gc );
	CLKSYS_Enable( OSC_XOSCEN_bm );
	do {} while ( CLKSYS_IsReady( OSC_XOSCRDY_bm ) == 0 );

	CLKSYS_PLL_Config( OSC_PLLSRC_XOSC_gc, 2);
	CLKSYS_Enable( OSC_PLLEN_bm );
	do {} while ( CLKSYS_IsReady( OSC_PLLRDY_bm ) == 0 );

	CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_PLL_gc );
	CLKSYS_Disable( OSC_RC2MEN_bm );
	CLKSYS_Disable( OSC_RC32MEN_bm );
}

static void EnableAllInterupts(void)
{
  PMIC.CTRL |= PMIC_LOLVLEN_bm;
  PMIC.CTRL |= PMIC_MEDLVLEN_bm;
  PMIC.CTRL |= PMIC_HILVLEN_bm;
  sei();
}