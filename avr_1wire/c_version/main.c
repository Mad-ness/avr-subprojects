/*
* DispTiny13C.c
* Created: 09.11.2013 09:40:27
* Author: Volker
* Mikrocontroller :						ATtiny13
*									  -----------
*									 |			 |
*  (/RESET/PCINT5/ADC0/dW) <---PB5---|1			8|--- VCC
*		(PCINT3/CLKI/ADC3) <---PB3---|2			7|---PB2---> (SCK/ADC1/T0/PCINT2)
*			 (PCINT4/ADC2) <---PB4---|3			6|---PB1---> (MISO/AIN1/OC0B/INT0/PCINT1)
*							  GND ---|4			5|---PB0---> (MOSI/AIN0/OC0A/PCINT0)
*									 |			 |
*									  ----------
* The code is based on the source taken from here http://www.mikrocontroller.net/attachment/196873/DispTiny13C.c
* but modified to work with my real China device :)
*/ 
#define F_CPU 1200000UL 
#include <avr/io.h>
#include <avr/cpufunc.h>
#include <util/delay.h>
//#include <avr/interrupt.h>



/* Bit Operations */
#define BIT(x)          (1 << (x)) 
#define SETBITS(x,y)    ((x) |= (y)) 
#define CLEARBITS(x,y)  ((x) &= (~(y))) 
#define SETBIT(x,y)     SETBITS((x), (BIT((y))))            /* EXAMPLE SETBIT(PORTB,2) sets the 2 bit of PORTB */
#define CLEARBIT(x,y)   CLEARBITS((x), (BIT((y)))) 
#define BITSET(x,y)     ((x) & (BIT(y))) 
#define BITCLEAR(x,y)   !BITSET((x), (y)) 
#define BITSSET(x,y)    (((x) & (y)) == (y)) 
#define BITSCLEAR(x,y)  (((x) & (y)) == 0) 
#define BITVAL(x,y)     (((x)>>(y)) & 1) 


#define pin_LED             PINB1

static int8_t TubeTab[] = { 
							0x3f,0x06,0x5b,0x4f,0x66,
							0x6d,0x7d,0x07,0x7f,0x6f,				// 0-9
							0x40,0x76,0x79,0x38						// -,H,E,L	[11-14]
						  };

#define TM1637_CLK          PINB3
#define TM1637_DATA         PINB4


#define LED_ON()        SETBIT(PORTB, pin_LED)
#define LED_OFF()       CLEARBIT(PORTB, pin_LED)


//setup connection
#define DS18B20_PORT 				PORTB
#define DS18B20_DDR 				DDRB
#define DS18B20_PIN 				PINB
#define DS18B20_DQ 					PB0

//commands
#define DS18B20_CMD_CONVERTTEMP 	0x44
#define DS18B20_CMD_RSCRATCHPAD 	0xbe
#define DS18B20_CMD_WSCRATCHPAD 	0x4e
#define DS18B20_CMD_CPYSCRATCHPAD 	0x48
#define DS18B20_CMD_RECEEPROM 		0xb8
#define DS18B20_CMD_RPWRSUPPLY 		0xb4
#define DS18B20_CMD_SEARCHROM 		0xf0
#define DS18B20_CMD_READROM 		0x33
#define DS18B20_CMD_MATCHROM 		0x55
#define DS18B20_CMD_SKIPROM 		0xcc
#define DS18B20_CMD_ALARMSEARCH 	0xec



//stop any interrupt on read

#define DS18B20_STOPINTERRUPTONREAD 1



/*
 * ds18b20 init
 */
uint8_t ds18b20_reset(void) {
	uint8_t i;
	//low for 480us
	DS18B20_PORT &= ~ (1<<DS18B20_DQ); //low
	DS18B20_DDR |= (1<<DS18B20_DQ); //output
	_delay_us(480);
	//release line and wait for 60uS
	DS18B20_DDR &= ~(1<<DS18B20_DQ); //input
	_delay_us(60);
	//get value and wait 420us
	i = (DS18B20_PIN & (1<<DS18B20_DQ));
	_delay_us(420);
	//return the read value, 0=ok, 1=error
	return i;
}

/*
 * write one bit
 */
void ds18b20_writebit(uint8_t bit){
	//low for 1uS
	DS18B20_PORT &= ~ (1<<DS18B20_DQ); //low
	DS18B20_DDR |= (1<<DS18B20_DQ); //output
	_delay_us(1);
	//if we want to write 1, release the line (if not will keep low)
	if(bit)
		DS18B20_DDR &= ~(1<<DS18B20_DQ); //input
	//wait 60uS and release the line
	_delay_us(60);
	DS18B20_DDR &= ~(1<<DS18B20_DQ); //input
}

/*
 * read one bit
 */
uint8_t ds18b20_readbit(void){
	uint8_t bit=0;
	//low for 1uS
	DS18B20_PORT &= ~ (1<<DS18B20_DQ); //low
	DS18B20_DDR |= (1<<DS18B20_DQ); //output
	_delay_us(1);
	//release line and wait for 14uS
	DS18B20_DDR &= ~(1<<DS18B20_DQ); //input
	_delay_us(14);
	//read the value
	if(DS18B20_PIN & (1<<DS18B20_DQ))
		bit=1;
	//wait 45uS and return read value
	_delay_us(45);
	return bit;
}

/*
 * write one byte
 */
void ds18b20_writebyte(uint8_t byte){
	uint8_t i=8;
	while(i--){
		ds18b20_writebit(byte&1);
		byte >>= 1;
	}
}

/*
 * read one byte
 */
uint8_t ds18b20_readbyte(void){
	uint8_t i=8, n=0;
	while(i--){
		n >>= 1;
		n |= (ds18b20_readbit()<<7);
	}
	return n;
}

/*
 * get temperature
 */
int16_t ds18b20_gettemp(void) {
	uint8_t temperature_l;
	uint8_t temperature_h;
	//double retd = 0;
	//#if DS18B20_STOPINTERRUPTONREAD == 1
	//cli();
	//#endif
	ds18b20_reset(); //reset
	ds18b20_writebyte(DS18B20_CMD_SKIPROM); //skip ROM
	ds18b20_writebyte(DS18B20_CMD_CONVERTTEMP); //start temperature conversion

	while(!ds18b20_readbit()); //wait until conversion is complete

	ds18b20_reset(); //reset
	ds18b20_writebyte(DS18B20_CMD_SKIPROM); //skip ROM
	ds18b20_writebyte(DS18B20_CMD_RSCRATCHPAD); //read scratchpad
	//read 2 byte from scratchpad
	temperature_l = ds18b20_readbyte();
	temperature_h = ds18b20_readbyte();
	//#if DS18B20_STOPINTERRUPTONREAD == 1
	//sei();
	//#endif
	//convert the 12 bit value obtained
//	retd = ( ( temperature_h << 8 ) + temperature_l )/16;// * 0.0625;	
	int16_t retd = (( temperature_h << 8 ) + temperature_l )*5/8;
	return retd;
}




void TM1637_writeByte(int8_t wr_data) {
	uint8_t i;
	for(i=0;i<8;i++)        //send 8bit data
	{
        CLEARBIT(PORTB, TM1637_CLK);
        if (wr_data & 0x01) 
            SETBIT(PORTB, TM1637_DATA);
        else
            CLEARBIT(PORTB, TM1637_DATA);
		wr_data >>= 1;
        SETBIT(PORTB, TM1637_CLK);
	}
	PORTB &= ~(1<<TM1637_CLK);										// auf ACK warten
	DDRB &= ~(1<<TM1637_DATA);
    while (BITVAL(PINB, TM1637_DATA));
    SETBIT(DDRB, TM1637_DATA);
    SETBIT(PORTB, TM1637_CLK);
    CLEARBIT(PORTB, TM1637_CLK);
}


//send start signal to TM1637
void TM1637_start(void) {
	PORTB |= (1<<TM1637_CLK);										// TM1637_CLK High
	PORTB |= (1<<TM1637_DATA);										// TM1637_DATA High
    _NOP();
	PORTB &= ~(1<<TM1637_DATA);										// TM1637_DATA Low
}

//End of transmission
void TM1637_stop(void) {
	PORTB &= ~(1<<TM1637_CLK);										// TM1637_CLK Low
    _NOP();
	PORTB &= ~(1<<TM1637_DATA);										// TM1637_DATA Low
    _NOP();
	PORTB |= (1<<TM1637_CLK);										// TM1637_CLK High
    _NOP();
	PORTB |= (1<<TM1637_DATA);										// TM1637_DATA High
    _NOP();
}

void TM1637_display(uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4) {
	TM1637_start();												//start signal sent to TM1637 from MCU
	TM1637_writeByte(0x40);
	TM1637_stop();
	TM1637_start();
	TM1637_writeByte(0xC0);
	TM1637_writeByte(d1);
    TM1637_writeByte(d2);
	TM1637_writeByte(d3);
    TM1637_writeByte(d4);
	TM1637_stop();
	TM1637_start();
	TM1637_writeByte(0x88);                                     // it sets brightness (0x88 + x)
	TM1637_stop();
    if (BITVAL(PINB, pin_LED))
        CLEARBIT(PORTB, pin_LED);
    else
        SETBIT(PORTB, pin_LED);
}

void TM1637_clear(void) {
	TM1637_display( TubeTab[0], TubeTab[0], TubeTab[0], TubeTab[0] );
}

void TM1637_init(void) {
    SETBIT(DDRB, TM1637_CLK);
    SETBIT(DDRB, TM1637_DATA);
}

void init_leds(void) 
{
    SETBIT(DDRB, pin_LED);
}

void show_temp(void)
{
	uint8_t d3, d2, d1;
	int16_t temp = ds18b20_gettemp();
    uint16_t buf = 0;
	uint8_t i = 0;
	buf = temp;
	while (buf >= 100) {
		buf -= 100;
		i++;
	}
	d3 = i;
	i = 0;
	buf = temp - d3*100; 
	while (buf >= 10) {
		buf -= 10;
		i++;
	}
	d2 = i;
	d1 = (uint8_t)((int16_t)(temp - d3*100 - d2*10));
    if (temp < 0) {
        d1 = d2;
        d2 = d3;
        d3 = 0x40;                  // sign '-' minus
    }
	
	TM1637_display(TubeTab[d3], TubeTab[d2], TubeTab[d1], 0x63);
}

int main(void) {
    init_leds();
//    int i = 2;
    TM1637_init();
	TM1637_display( TubeTab[11], TubeTab[12], TubeTab[13], TubeTab[0] );
    LED_ON();
    _delay_ms(4000);
//    i = 0;
	while (1) {
		show_temp();
		if (BITVAL(PINB, pin_LED))
			LED_OFF();
		else
			LED_ON();
		_delay_ms(300);
	};
	/*
    while (1) {
        if (i++ >= 9) i = 0;
	    TM1637_display(TubeTab[0], TubeTab[0], TubeTab[0], TubeTab[i]);
        _delay_ms(500);
    } 
	*/
}

