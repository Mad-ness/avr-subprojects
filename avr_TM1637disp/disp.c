#define F_CPU 1200000LU
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//#include <TM1637Display.h>
//#include <avr/wdt.h> 

//unsigned char word_dht = 0x00;
//unsigned char time_bit[40];
//unsigned char data_dht[5];
//unsigned char bit_dht;


#define Clkpin      3 //Clkpin TM1637
#define Datapin     4 // Datapin TM1637

#define DispPort PORTB // обзываем порт

#define  BRIGHT_LCD 7  //Яркость дисплея от 0 до 7
#define CMD_BTN         PINB0
#define CMD_BUTTON      PINB0
#define PIN_ADC         PINB2
#define PIN_WORKLOAD    PINB1

#define RUN_MODE     1          // program mode: normal (0), programming (1)
#define BUTTON_PRG   2          // button PRG is pressed (1) or no (0)
#define BUTTON_SET   3          // button SET is pressed (1) or no (0)
#define BPRG_1ST     4          // PRG button is pressed first time (1) or no (0)
#define BSET_1ST     5          // SET button is pressed first time (1) or no (0)

#define MAX_VALUE    100        // maximum possible value of a sensor
#define MIN_VALUE    0          // minimal possible value of a sensor

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


#define DispNumber(num) (TM1637_digits(TubeTab[(uint8_t)(num)/1000], TubeTab[(uint8_t)(num)%1000/100], TubeTab[(uint8_t)(num)%100/10], TubeTab[(uint8_t)(num)%10]))
#define TIMER0_reset    {number_ticks=0;TCNT0=0x0;}

// wait for so overflow interrupts to get a second
#define TimerOverFlowMax	5

#define SETTING_NO		0
#define SETTING_P1		1
#define SETTING_P2		2
#define SETTING_P3		3

#define FALSE           0
#define TRUE            1

#define ADC_pause       (CLEARBIT(ADCSRA, ADEN))
#define ADC_resume      (SETBIT(ADCSRA, ADEN))
// uint8_t PROGRAM_FUNC = SETTING_NO;

unsigned char _PointFlag; 	//_PointFlag=1:the clock point on
unsigned char _DispType;
unsigned char DecPoint;
unsigned char BlankingFlag;
	
// uint8_t timer_on = 0;
		
static uint8_t TubeTab[10] = {0x3f,0x06,0x5b,0x4f,// знакогенератор
                           0x66,0x6d,0x7d,0x07,
                           0x7f,0x6f};//,0x77,0x7c,
                           //0x39,0x5e,0x79,0x71,
                          // 0x40,0x00};//0~9,A,b,C,d,E,F,"-"," "  
#define HLEVEL_NORMAL_L 10
#define HLEVEL_NORMAL_H 20

uint8_t HUMIDITY_LEVEL = HLEVEL_NORMAL_L;
uint8_t HUMIDITY_LEVEL_PREVIOUS = HLEVEL_NORMAL_L;

// it is used for estimating time between pressing the CMD_BUTTON
// volatile uint8_t number_ticks = 0;
// volatile uint8_t cmd_button_pressed = FALSE;
// volatile uint8_t cmd_button_unpressed = TRUE;

//void TM1637_showNumber(const int num);
//void TIMER0_start(void);
//void TIMER0_reset(void);


/*
ISR(TIM0_OVF_vect) {
	number_ticks++;
};

ISR(PCINT0_vect)
{
    if (cmd_button_unpressed) {
        cmd_button_pressed = TRUE;
        TIMER0_reset
    }
    cmd_button_unpressed = ! cmd_button_pressed;
}
*/
//SIGNAL(INT0_vect)
//{
//    //PORTB |= _BV(CMD_BTN);
//    //TM1637_showNumber(9999);
//    //STATUS_REG &= (1 << RUN_MODE);
//    SETBIT( PORTB, ! BITVAL ( PORTB, PIN_WORKLOAD ));
//}


/// Buttons handler
/*
void cfg_menu(void)
{
    if (!(STATUS_REG & (1 << RUN_MODE))) return;
    if (STATUS_REG & (1 << BUTTON_PRG)) {

    } else if (STATUS_REG & (1 << BUTTON_SET)) {

    }
}
*/
///

void TM1637_writeByte(char wr_data)// служебная функция записи данных по протоколу I2C, с подтверждением (ACK)
{
    unsigned char i;
    for (i = 0; i < 8; i++)        
    {
        DispPort &= ~(1<<Clkpin);
        if(wr_data & 0x01)
	    { 
            DispPort |= 1<<Datapin;
        } else {
            DispPort &= ~(1<<Datapin);
        }
	    _delay_us(3);
        wr_data = wr_data>>1;      
        DispPort |= 1<<Clkpin;
        _delay_us(3);  
    }  
 
    DispPort &= ~(1<<Clkpin);
    _delay_us(5);
    DDRB &= ~(1<<Datapin);// если поменяете порт на какой-то другой кроме DispPort, то тут тоже все DDRB на другие DDRx менять надо будет
    while((PINB & (1<<Datapin)));
    DDRB |= (1<<Datapin);
    DispPort |= 1<<Clkpin;
    _delay_us(2);
    DispPort &= ~(1<<Clkpin);  
}

void TM1637_start(void) // просто функция "старт" для протокола I2C
{
    DispPort |= 1<<Clkpin; 
    DispPort |= 1<<Datapin;
    _delay_us(2);
    DispPort &= ~(1<<Datapin); 
} 

void TM1637_stop(void) // просто функция "стоп" для протокола I2C
{
    DispPort &= ~(1<<Clkpin);
    _delay_us(2);
    DispPort &= ~(1<<Datapin);
    _delay_us(2);
    DispPort |= 1<<Clkpin;;
    _delay_us(2);
    DispPort |= 1<<Datapin;
}

// Инициализируем дисплей. Как оказалось, можно удалить из этого блока стандартной библиотеки практически всё, кроме инициализации пинов.
void TM1637_init(void) 
{
	DDRB |= (1<<Clkpin) | (1<<Datapin);
}

/*
void start_tmr_us(void)
{
    TCNT0 = 0x00;
    TCCR0B = 0x01; //Запускаем таймер0 с частотой 1,2 Мгц
}
*/

//ISR (WDT_vect) { }
/*
void TM1637_suitNumber(int num)
{
    unsigned char t;
    if (num >= 0 || num <= 9) {
        t = num;
    } else { 
        t = 0x0;
    }
    TM1637_writeByte(TubeTab[t]);
}
*/

void TM1637_digits(uint8_t pos1, uint8_t pos2, uint8_t pos3, uint8_t pos4)
{
    TM1637_start();                     //Выводим на дисплей влажность.  
    TM1637_writeByte(0x40);
    TM1637_stop();
    TM1637_start();
    TM1637_writeByte(0xC0);
    TM1637_writeByte(pos1);
    TM1637_writeByte(pos2);
    TM1637_writeByte(pos3);
    TM1637_writeByte(pos4);
    TM1637_stop();
    TM1637_start();
    TM1637_writeByte(0x8F);
    TM1637_stop();
    _delay_ms(5);
}
// --- ADC
void ADC_setup(void)
{
    //ADMUX = (1 << MUX0) | (1 << ADLAR) | (0 << REFS0);
    ADMUX |= (1 << MUX0)|(0 << MUX1)|(1 << ADLAR); // select PB2 as an input
    //ADMUX |= (1 << ADLAR);
    // Set the prescaler to clock/128 & enable ADC
    ADCSRA |= (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);
//    ADC_pause;
}

uint8_t ADC_read(void)
{
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
//    while (!(ADCSRA & (1 << ADIF)));
//    ADCSRA |= (1 << ADIF);
    _delay_ms(1);
    return ADCH;
}

// --- EEPROM
unsigned char EEPROM_read(unsigned int addr)
{
    while (EECR & (1 << EEPE)); // wait for write to finish 
    EEARL = addr;               // load the address to read from 
    EECR |= (1 << EERE);        // set the read flag 
                                // the clock is stopped for 4 cyles 
    EEARL = 0;                  // leave address at 0 so if power down corrupts location 0     
    return EEDR; 
}

void EEPROM_write(unsigned int addr, unsigned char data) 
/* see page 18 of http://www.atmel.com/Images/doc2535.pdf */
{ 
	if (EEPROM_read(addr) == data) return;
	while (EECR & (1 << EEPE));	// wait for completion of previos write
								// set programming mode
	EECR = (0 << EEPM1) | (0 >> EEPM0);
	EEARL = addr;				// set address
	EEDR = data;				// and data registers
	EECR |= (1 << EEMPE);
	EECR |= (1 << EEPE);		// start eeprom writing by setting EEPE
}
/*
void INT0_init(void)
{
//    DDRB &= ~_BV(CMD_BUTTON);           // initial status as zero
    PCMSK |= (1<<PCINT0);               // call interrupt on a level change
//    MCUCR |= (1<<ISC00)|(1<<ISC01);
    GIMSK |= (1<<PCIE);                 // enable this interrupt in a mask register
    sei();
}
*/
/*
void TIMER0_start(void)
{
	TIMSK0 |= (1 << TOIE0);		// unmask interrupt
 	TCNT0 = 0;					// initial counter value
	number_ticks = 0;
	TCCR0B |= (1 << CS00) | (1 << CS02);	// divider is 1024
	sei();						// enable interrupts
	timer_on = TRUE;
}
void TIMER0_stop(void)
{
	TCCR0B = 0x00;
	timer_on = FALSE;
};
*/
/*
void TIMER0_reset(void)
{
    number_ticks = 0;
    TCNT0 = 0x0;
}
*/
void PORTS_init(void)
{
//    DDRB |= (1 << PIN_WORKLOAD);
//    PORTB |= (1 << PIN_WORKLOAD);
//    CLEARBIT( DDRB, CMD_BUTTON );
      CLEARBIT( DDRB, CMD_BUTTON );
      SETBIT( DDRB, PIN_WORKLOAD );
//    SETBIT( DDRB, PIN_WORKLOAD );
//    CLEARBITS ( DDRB, (1<<PIN_ADC)|(0<<PIN_WORKLOAD)|(1<<PIN_BUTTON) );
//    CLEARBITS( PORTB, (1<<CMD_BUTTON)|(1<<PIN_WORKLOAD)|(1<<PIN_ADC) );
      CLEARBIT( PORTB, CMD_BUTTON );
      CLEARBIT( PORTB, PIN_WORKLOAD );
//      CLEARBIT( PORTB, PIN_ADC );
}

//uint8_t counter = 0;

void displayHumidity(void)
{
    do {
        uint8_t k = 0;
        uint16_t voltage = 0, nums = 200;
        //DispNumber(9);
        while (k++ < nums)
        {
    //      ADC_resume;
          uint8_t tmp = ADC_read();
    //      ADC_pause;
          if (tmp < 0) { tmp = 0; };
          if (tmp > 255) { tmp = 255; };
            voltage += tmp;
        }
    //    _delay_ms(300);
        //DispNumber(8);
        //counter++;
        //TM1637_showNumber( (voltage/nums) );
        uint8_t humidity_percent = (uint8_t)((voltage/nums) * 100 / 254);
        DispNumber(humidity_percent);
        if (humidity_percent < HLEVEL_NORMAL_L)
        {
            SETBIT( PORTB, PIN_WORKLOAD );
        } else if (humidity_percent > HLEVEL_NORMAL_H) {
            CLEARBIT( PORTB, PIN_WORKLOAD );
        }
    } while ( BITVAL( PINB, PIN_WORKLOAD ) > 0 );
    //TM1637_showHumidity( (voltage/nums) * 100 / 249 );
    //TM1637_showNumber( vol );
    //_delay_ms(150);
}



int main(void)
{
    ADC_setup();
    TM1637_init();
    _delay_ms(200);  //Зачем-то тупим немного. Иначе, вроде как, дисплей может не зажечься.
    DispNumber(0);
    PORTS_init();
//    INT0_init();
    while (1) {
        uint16_t counter = 0;
        while (counter++ < 30)     // showing the sensors values
        {
            displayHumidity();
            _delay_ms(100);
        };

        counter = 0;
        while (counter++ < 30)
        {
            TM1637_digits(
                0x38, 
                0x40, 
                TubeTab[HLEVEL_NORMAL_L%100/10], 
                TubeTab[HLEVEL_NORMAL_L%10]
            ); // H-<Higher level>
            _delay_ms(100);
        };

        counter = 0;
        while (counter++ < 30)
        {
            TM1637_digits(
                0x76, 
                0x40, 
                TubeTab[HLEVEL_NORMAL_H%100/10], 
                TubeTab[HLEVEL_NORMAL_H%10]
            ); // L-<Lower level>
            _delay_ms(100);
        };
//        DispNumber(counter);
//        _delay_ms(50);
    }
}

