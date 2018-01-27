#ifndef __RF433_H__
#define __RF433_H__


#define OUTPUT_PORT PORTB

void RF433_send(const int pin, char* sCodeWord);
void RF433_sendbyte(const int pin, const int data);
void RF433_sendbyte2(const int pin, const int data, const int attempts);
void RF433_setup();

#endif // __RF433_H__

