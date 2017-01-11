#define F_CPU 1200000LU
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//#include <avr/wdt.h> 

//unsigned char word_dht = 0x00;
//unsigned char time_bit[40];
//unsigned char data_dht[5];
//unsigned char bit_dht;


#define Clkpin      3 //Clkpin TM1637
#define Datapin     4 // Datapin TM1637
#define PIN_DHT     0 //Пин датчика DHT

#define DispPort PORTB // обзываем порт

#define  BRIGHT_LCD 7  //Яркость дисплея от 0 до 7
		
unsigned char _PointFlag; 	//_PointFlag=1:the clock point on
unsigned char _DispType;
unsigned char DecPoint;
unsigned char BlankingFlag;
	
		
static unsigned char TubeTab[10] = {0x3f,0x06,0x5b,0x4f,// знакогенератор
                           0x66,0x6d,0x7d,0x07,
                           0x7f,0x6f};//,0x77,0x7c,
                           //0x39,0x5e,0x79,0x71,
                          // 0x40,0x00};//0~9,A,b,C,d,E,F,"-"," "  

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


void TM1637_init()// Инициализируем дисплей. Как оказалось, можно удалить из этого блока стандартной библиотеки практически всё, кроме инициализации пинов.
{
	DDRB |= (1<<Clkpin) | (1<<Datapin);
}


void start_tmr_us()
{
    TCNT0 = 0x00;
    TCCR0B = 0x01; //Запускаем таймер0 с частотой 1,2 Мгц
}

void TM1637_clearDisp()
{

}

//ISR (WDT_vect) { }

void TM1637_showNumber(const int num)
{
    TM1637_start();                     //Выводим на дисплей влажность.  
    TM1637_writeByte(0x40);
    TM1637_stop();
    TM1637_start();
    TM1637_writeByte(0xC0);
//        TM1637_writeByte(0x76);
//    TM1637_writeByte(0x00);
    TM1637_writeByte(TubeTab[ num / 1000 ]);
    TM1637_writeByte(TubeTab[ num % 1000 / 100 ]);
    TM1637_writeByte(TubeTab[ num % 100 / 10 ]);
    TM1637_writeByte(TubeTab[ num % 10 ]);
        //TM1637_writeByte(0x8f);
        _delay_ms(450);
        //TM1637_stop();
        //TM1637_start();
        //TM1637_writeByte(0xC0);
        //TM1637_writeByte(0xff);
    TM1637_stop();
    TM1637_start();
    TM1637_writeByte(0x8F);
    TM1637_stop();
}

int main(void)
{
//    WDTCR |= (1 << WDTIE);
//    sei();
    TM1637_init();
    _delay_ms(500);  //Зачем-то тупим немного. Иначе, вроде как, дисплей может не зажечься.

    int i;
    while(1)
    {
    for (i=0; i<10000; i++) {
        TM1637_showNumber(i);
    }
    /*
        TM1637_start();                     //Выводим на дисплей влажность.  
        TM1637_writeByte(0x40);
        TM1637_stop();
        TM1637_start();
        TM1637_writeByte(0xC0);
//        TM1637_writeByte(0x76);
        TM1637_writeByte(0x00);
        int i;
        for (i = 0; i < 10; i++) {
            TM1637_writeByte(TubeTab[i]);
            //TM1637_writeByte(0x8f);
            _delay_ms(450);
            //TM1637_stop();
            //TM1637_start();
            //TM1637_writeByte(0xC0);
            //TM1637_writeByte(0xff);
        }
        TM1637_stop();
        TM1637_start();
        TM1637_writeByte(0x8F);
        TM1637_stop();
    */    
        _delay_ms(50);
    }
}

