#define PIN_PHOTOSENSOR         PB0
#define PIN_MOVEMENTSENSOR      PB1

#define PIN_LED_PHOTO           PB2
#define PIN_LED_MOVEMENT        PB3

#define pin_off(pin)            (PORTB&=~(1<<pin))
#define pin_on(pin)             (PORTB|=(1<<pin))

#define DELAY_SWITCH            5000
uint8_t old_photosensor_value = LOW;
static unsigned long last_changed = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_LED_PHOTO, OUTPUT);
  pinMode(PIN_LED_MOVEMENT, OUTPUT);
  pinMode(PIN_PHOTOSENSOR, INPUT);
  pinMode(PIN_MOVEMENTSENSOR, INPUT);
  digitalWrite(PIN_MOVEMENTSENSOR, LOW);
}

void loop() {
  if (millis() % 200 == 0) {
    uint8_t has_light = digitalRead(PIN_PHOTOSENSOR);
    uint8_t photosensor_value = digitalRead(PIN_PHOTOSENSOR);

    if (abs(millis() - last_changed) > DELAY_SWITCH) {
      if (has_light == HIGH) {
        digitalWrite(PIN_LED_MOVEMENT, digitalRead(PIN_MOVEMENTSENSOR));
      } else {
        digitalWrite(PIN_LED_MOVEMENT, LOW);
      }
      last_changed = millis();
    } // if abs(...)
    
  }
}

void loop2() {
  // put your main code here, to run repeatedly:
  if (millis() % 200 == 0) {
    uint8_t has_light = digitalRead(PIN_PHOTOSENSOR);
    //uint8_t photosensor_value = digitalRead(PIN_PHOTOSENSOR);
    //if (photosensor_value != old_photosensor_value) {
    if (1) {
      digitalWrite(PIN_LED_PHOTO, has_light);
      //unsigned long curr_time = millis();
      if (has_light == HIGH) {
          digitalWrite(PIN_LED_MOVEMENT, digitalRead(PIN_MOVEMENTSENSOR));
      } else {
          digitalWrite(PIN_LED_MOVEMENT, LOW);
      }
      /*
      if (abs(curr_time - last_changed) > DELAY_SWITCH) { 
        if (has_light == HIGH) {
          digitalWrite(PIN_LED_MOVEMENT, digitalRead(PIN_MOVEMENTSENSOR));
        } else {
          digitalWrite(PIN_LED_MOVEMENT, LOW);
        }
        last_changed = millis();
      }
      old_photosensor_value = photosensor_value;
      */
    }
    
  }
}

