#ifndef ___SERVO_H_INCLUDED_
#define ___SERVO_H_INCLUDED_

#define OCR1A_MIN   610

#include<inttypes.h>

void PWM_init( void );
void rotate_to(unsigned short angle);

#endif
