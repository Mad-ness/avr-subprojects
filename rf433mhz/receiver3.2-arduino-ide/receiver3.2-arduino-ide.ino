#include <avr/io.h>
#include <avr/interrupt.h>
#include <TM1637Display.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define mem_address           0x06
#define PIN_RECEIVER          PB0
#define PIN_LED               PB1
#define PIN_RELAY             PIN_LED
#define PIN_BUTTON            PB2
#define PIN_CLK               PB4
#define PIN_DIO               PB3
#define RCSWITCH_MAX_CHANGES  67
#define nReceiveTolerance     60
#define sender_id             0x5
#define led_on()              sbi(PORTB, PIN_LED)
#define led_off()             cbi(PORTB, PIN_LED)

#define unpack_senderId(x)    ( 0x7 & ( x >> 11 ))
#define unpack_intpart(x)     ( 0x3F & x )
#define unpack_fracpart(x)    ( 0xF & ( x >> 6 ))
#define unpack_minus(x)       ( 0x1 & ( x >> 10 ))
#define clear_fracpart(x)     ( 0xFC3F & x )

//uint16_t display_data = 0;

uint16_t packtemp(int8_t intpart, int8_t realpart, bool is_minus) {
    return ( intpart ) | ( realpart << 6 ) | ( is_minus << 10 );
}

struct pulse_t {
    uint8_t high;
    uint8_t low;
};

struct protocol_t {
    int pulse_length;
    struct pulse_t sync_factor;
    struct pulse_t zero;
    struct pulse_t one;
    bool inverted;
};

const struct protocol_t proto = { 350, { 1, 31 }, { 1, 3 }, { 3, 1 }, false };
volatile unsigned int timings[RCSWITCH_MAX_CHANGES];
volatile uint16_t nReceivedValue = 0;
volatile uint8_t nReceivedDelay = 0;

volatile inline int diff(int A, int B) { return abs(A - B); }
inline uint16_t RCTSwitch_getValue() { return nReceivedValue; }
volatile inline bool RCTSwitch_available() { return nReceivedValue != 0; }
inline void RCTSwitch_reset() { nReceivedValue = 0; }

TM1637Display display(PIN_CLK, PIN_DIO);


void EEPROM_write(uint8_t ucAddress, int8_t ucData) {
    while ( EECR & ( 1 << EEPE ));
    EECR = ( 0 << EEPM1 ) | ( 0 << EEPM0 );
    EEAR = ucAddress;
    EEDR = ucData;
    cli();
    EECR |= ( 1 << EEMPE );
    EECR |= ( 1 << EEPE );
    sei();
}

int8_t EEPROM_read(unsigned char ucAddress) {
    while ( EECR & ( 1 << EEPE ));
    EEAR = ucAddress;
    EECR |= ( 1 << EERE );
    return EEDR;
}

void displayTemp(const uint16_t value, const bool show_colon=false);

bool RCTSwitch_receiveProtocol(const int p, const int changeCount) {
    const unsigned int syncLegthInPulses = ((proto.sync_factor.low) > (proto.sync_factor.high)) ? proto.sync_factor.low : proto.sync_factor.high;
    const uint16_t delay_ms = timings[0] / syncLegthInPulses;
    const uint16_t delayTolerance = ( delay_ms * nReceiveTolerance ) / 100;

    uint16_t code = 0;
    const uint16_t firstDataTiming = proto.inverted ? (2) : (1);
    uint16_t i;
    for ( i = firstDataTiming; i < changeCount - 1; i += 2 ) {
        code <<= 1;
        if (diff(timings[i], delay_ms*proto.zero.high) < delayTolerance &&
            diff(timings[i+1], delay_ms*proto.zero.low) < delayTolerance) {
            // zero
        } else if (diff(timings[i], delay_ms*proto.one.high) < delayTolerance &&
                   diff(timings[i+1], delay_ms*proto.one.low) < delayTolerance) {
            code |= 1;
        } else {
            // failed
            return false;
        }
    }
    if ( changeCount > 7 ) {
        //code &= ~( 1 << 7 );
        nReceivedValue = code;
        nReceivedDelay = delay_ms; 
        return true;
    }
    return false;
}

ISR(PCINT0_vect) {
    static unsigned int changeCount = 0;
    static unsigned long lastTime = 0;
    static unsigned int repeatCount = 0;
    const long time = micros();
    const unsigned int duration = time - lastTime;
    if ( duration > 4300 ) {
        if ( diff( duration, timings[0] ) < 200 ) {
            repeatCount++;
            if ( repeatCount == 2 ) {
                RCTSwitch_receiveProtocol(1, changeCount );
                repeatCount = 0;
            }
        }
        changeCount = 0;
    }
    if ( changeCount >= RCSWITCH_MAX_CHANGES ) {
        changeCount = 0;
        repeatCount = 0;
        //debug_num = 3;
    }
    timings[changeCount++] = duration;
    lastTime = time;
}
int8_t setup_min_value = -30;
int8_t setup_max_value = 40;

class DebounceButton {
    private:
        int m_buttonPin;
        int m_buttonState;
        int m_lastButtonState = LOW;
        unsigned long m_lastDebounceTime = 0;
        unsigned int m_debounceDelay = 50;
    public:
        DebounceButton(const int button_pin);
        bool hasPressed();
};

DebounceButton::DebounceButton(const int button_pin) {
    this->m_buttonPin = button_pin;
    pinMode( button_pin, INPUT );
};

bool DebounceButton::hasPressed() {
    int reading = digitalRead ( this->m_buttonPin );
    if ( reading != this->m_lastButtonState ) {
        this->m_lastDebounceTime = millis();
    }
    if (( millis() - this->m_lastDebounceTime ) > this->m_debounceDelay ) {
        if ( reading != this->m_buttonState ) {
            this->m_buttonState = reading;
            return ( this->m_buttonState == HIGH );
        }
    }
    this->m_lastButtonState = reading;
    return false;
}

enum DisplayMode { MTemp = 1, MSetup = 2 };
enum class LedMode { Off = 1, ReceivedPacket = 2, Alert = 3, PressedButton = 4, WriteSetup = 5 };
class Logistic {
    private:
        unsigned long m_timeStamp;
        struct {
            bool on = false;
            unsigned long lastTime;
            LedMode ledMode = LedMode::Off;
        } m_ledState;
        struct {
            int8_t writtenValue = 0xff;
            int8_t currentValue = 0;
            unsigned long lastTime;
            DebounceButton button = DebounceButton( PIN_BUTTON );
        } m_button;
        struct {
            DisplayMode previous;
            DisplayMode current = MTemp;
            bool needUpdate;
            bool showCounter = true;
            bool showColon = false;
            unsigned long lastTime;
            unsigned long colonLastBlinkingTime;
        } m_display;
        struct {
            uint16_t previous;
            uint16_t current;
            unsigned long lastTime;
        } m_receivedValue;
        void updateDisplay();
        void readSetupThreshold();
    public:
        Logistic(void);
        void ledOff();
        void ledOn();
        void beginIter(void);
        void postButtonPressed(void);
        void endIter();
        bool hasButtonPressed();
        bool hasDataReceived();
        bool hasThresholdReached();
        int8_t getRelayThreshold();
        void ledBlink();
        void setLedMode(const LedMode mode);
};
Logistic::Logistic(void) {
    this->m_display.current = MTemp;
    this->m_display.needUpdate = true;
    pinMode( PIN_LED, OUTPUT );
    this->getRelayThreshold();
}
int8_t Logistic::getRelayThreshold() {
    if ( this->m_button.writtenValue != this->m_button.currentValue ) {
         this->m_button.writtenValue = EEPROM_read( mem_address );
         this->m_button.currentValue = this->m_button.writtenValue;
    }
    return this->m_button.currentValue;
}
bool Logistic::hasThresholdReached() {
    int8_t current_temp = unpack_intpart( this->m_receivedValue.current );
    if ( unpack_minus( this->m_receivedValue.current ) > 0 ) {
        current_temp *= -1;
    }
    if ( this->m_receivedValue.current != 0 && current_temp <= this->m_button.currentValue ) {
        this->setLedMode( LedMode::Alert );
        return true;
    } else {
        this->setLedMode( LedMode::Off );
        return false;
    }
}
void Logistic::ledBlink() {
    switch ( this->m_ledState.ledMode ) {
        case LedMode::Off:
            if ( this->m_ledState.ledMode != LedMode::Alert ) {
                 this->ledOff();
            }
            break;
        case LedMode::WriteSetup:
        case LedMode::ReceivedPacket:
        case LedMode::PressedButton:
            if ( this->m_timeStamp - this->m_ledState.lastTime > 200 ) {
                this->ledOff();
                this->setLedMode( LedMode::Off );
            } else {
                this->ledOn();
            }
            break;
        case LedMode::Alert:
            if ( this->m_timeStamp - this->m_ledState.lastTime > 200 ) {
                if ( this->m_ledState.on ) {
                    this->ledOff();
                } else {
                    this->ledOn();
                }
            }
            break;
    }
}
void Logistic::ledOff() {
    if ( this->m_ledState.on == true ) {
        led_off();
        this->m_ledState.on = false;
        this->m_ledState.lastTime = this->m_timeStamp;
    }
}
void Logistic::ledOn() {
    if ( this->m_ledState.on == false ) {
        led_on();
        this->m_ledState.on = true;
        this->m_ledState.lastTime = this->m_timeStamp;
    }
}
void Logistic::setLedMode( const LedMode mode ) {
    switch ( mode ) {
        case LedMode::WriteSetup:
        case LedMode::PressedButton:
        case LedMode::ReceivedPacket:
            if ( this->m_ledState.ledMode != LedMode::Alert )
                this->m_ledState.ledMode = mode;
            break;
        case LedMode::Alert:
        case LedMode::Off:
            this->m_ledState.ledMode = mode;
            break;
    }
    //this->m_ledState.lastTime = this->m_timeStamp;
}
bool Logistic::hasDataReceived() {
    bool result = RCTSwitch_available();
    if ( result ) {
        uint16_t recv_data = RCTSwitch_getValue();
        RCTSwitch_reset();
        if ( this->m_receivedValue.current != recv_data ) {
            if ( sender_id == unpack_senderId( recv_data )) {
                this->m_receivedValue.previous = this->m_receivedValue.current;
                this->m_receivedValue.current = recv_data;  
                this->m_receivedValue.lastTime = this->m_timeStamp;              
                this->m_display.needUpdate = true;
                this->m_display.showCounter = true;
                this->setLedMode( LedMode::ReceivedPacket );
            }
        }
    }
    return result;
}
bool Logistic::hasButtonPressed() {
    if ( this->m_button.button.hasPressed() ) {
        this->m_button.lastTime = this->m_timeStamp;
        if ( this->m_display.current != MSetup ) {
             this->m_display.previous = this->m_display.current;
             this->m_display.current = MSetup;
             this->m_display.colonLastBlinkingTime = this->m_timeStamp;
             //this->getRelayThreshold();
        } else {
            this->m_button.currentValue++;
            if ( this->m_button.currentValue > setup_max_value || this->m_button.currentValue < setup_min_value ) {
                 this->m_button.currentValue = setup_min_value;
            }
        }
        this->m_display.needUpdate = true;
        this->setLedMode( LedMode::PressedButton );
        return true;
    }
    return false;
}
void Logistic::beginIter(void) {
    this->m_timeStamp = millis();
}
void Logistic::postButtonPressed(void) {
    if ( this->m_display.current == MSetup && this->m_timeStamp - this->m_button.lastTime > 3500 ) {
        if ( this->m_button.writtenValue != this->m_button.currentValue ) {
            this->m_button.writtenValue = this->m_button.currentValue;
            EEPROM_write( mem_address, this->m_button.writtenValue );
            this->setLedMode( LedMode::WriteSetup );
        }
        this->m_display.previous = this->m_display.current;
        this->m_display.current = MTemp;
        this->m_display.needUpdate = true;
    }
}
void Logistic::endIter() {
    this->postButtonPressed();
    this->ledBlink();
    this->updateDisplay();
}
void Logistic::updateDisplay() {
  
    if ( this->m_display.current == MTemp && this->m_display.showCounter == true ) {
        if ( this->m_timeStamp - this->m_receivedValue.lastTime > 500 ) {
            this->m_display.needUpdate = true;
            this->m_display.showCounter = false;
        }
    }

    if ( this->m_display.current == MSetup && this->m_timeStamp - this->m_display.colonLastBlinkingTime > 250 ) {
         this->m_display.needUpdate = true;
         this->m_display.showColon = ! this->m_display.showColon;
         this->m_display.colonLastBlinkingTime = this->m_timeStamp;
    }

    if ( this->m_display.needUpdate == true ) {
        uint16_t data;
        if ( this->m_display.current == MTemp ) {
            if ( this->m_display.showCounter == false ) {
                data = packtemp( unpack_intpart( this->m_receivedValue.current ), 0, unpack_fracpart( this->m_receivedValue.current ) < 0 ? 1 : 0 );
            } else {
                data = this->m_receivedValue.current;
            }
            this->m_display.showColon = true;
        } else if ( this->m_display.current == MSetup ) {
            data = packtemp( abs( this->m_button.currentValue ), 0, ( this->m_button.currentValue) < 0 ? 1 : 0 );
        }
        displayTemp( data, this->m_display.showColon );
        this->m_display.needUpdate = false;
    }
}

static Logistic mind;

void loop() {
    mind.beginIter();
    if ( mind.hasDataReceived() || mind.hasButtonPressed() ) {
        // do whatever you want
    }
    mind.hasThresholdReached();
    mind.endIter();
}

void displayTemp(const uint16_t value, const bool show_colon=false) {
    bool temp_below_0 = unpack_minus(value);
    uint8_t temp_value = unpack_intpart(value);
    uint8_t frac_value = unpack_fracpart(value);

    uint8_t seg_data[1];
    if ( unpack_minus( value ) == 0 ) {
        seg_data[0] = SEG_A | SEG_B | SEG_F | SEG_G;
    } else {
        seg_data[0] = SEG_A | SEG_B | SEG_F | SEG_G | SEG_D;
    }
    int dots;
    if ( show_colon == true ) {
        dots = 0xff;
    } else {
        dots = 0x0;
    }
    display.showNumberDecEx(temp_value, dots, true, 2, 0);
    display.showNumberDecEx(frac_value, dots, true, 1, 2);
    display.setSegments(seg_data, 1, 3);
}

void displayTemp2(const uint16_t value) {
    bool temp_below_0 = unpack_minus(value);
    uint8_t temp_value = unpack_intpart(value);
//    uint8_t frac_value = unpack_fracpart(value);

    uint8_t degree_letter[1] = { SEG_A | SEG_B | SEG_F | SEG_G };
    
    uint8_t minus_letter[1] = { 0 };
    if ( temp_below_0 == 1 ) 
        minus_letter[0] = SEG_D;
    display.showNumberDecEx(temp_value, 0x0, false, 2, 1);
//    display.showNumberDecEx(frac_value, dots, true, 1, 2);
    display.setSegments(degree_letter, 1, 3);
    display.setSegments(minus_letter, 1, 0);
}

void RCTSwitch_setup() {
    GIMSK |= ( 1 << PCIE ); //|(1<<INT0);
    PCMSK |= ( 1 << PCINT0 );
    MCUCR |= ( 1 << ISC00 );
}

void setup(void) {
    RCTSwitch_setup();
    display.setBrightness(0x04);
    DDRB |= ( 1 << PIN_LED ) | ( 1 << PIN_RELAY );
    DDRB &= ~( 1 << PIN_BUTTON );
    PORTB &= ~( 1 << PIN_BUTTON );
    pinMode(PIN_BUTTON, OUTPUT);
}

