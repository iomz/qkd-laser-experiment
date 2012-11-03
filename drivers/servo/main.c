/* -----------------------------------------------------------------------
 * Title:    PWM High Speed (Analog 8bits input(PC2) PWM-HS output PD6)
 * Hardware: Atmega168
 * Software: WinAVR 20060421
 * -----------------------------------------------------------------------*/
 
#include <avr/io.h>
 
char adc()
{
  ADCSRA |= (1 << ADEN);            // Analog-Digital enable bit
 
  ADCSRA |= (1 << ADSC);            // Discarte first conversion
  while (ADCSRA & (1 << ADSC)); // wait until conversion is done
 
  ADCSRA |= (1 << ADSC);            // start single conversion
  while (ADCSRA & (1 << ADSC))  // wait until conversion is done
 
    ADCSRA &= ~(1<<ADEN);             // shut down the ADC
 
  return ADCH;
}
 
// ************************** init timer ****************************************************
 
void InitPWM()
{
 
  DDRD = 0x40;                          // Set direction output on PD6 (OC0A)
 
  TCCR0B |= (1<<CS00);                  // Timer2 Settings: Timer Prescaler /1,
 
  TCCR0A |= ((1<<WGM00)|(1<<WGM01)|     // Use H-Speed PWM mode
	     (1<<COM0A1)|(1<<COM0A0));
 
}
 
// **************************** Init ADC ****************************************************
 
void InitADC()
{
 
  ADCSRA |= (1 << ADEN) |               // Analog-Digital enable bit
    (1 << ADPS1)|               // set prescaler to 8    (clock / 8)
    (1 << ADPS0);               // set prescaler to 8    (doc Atmega168 23.8.2 p.258)
 
  ADMUX |=  (1 << ADLAR)|               // AD result store in (more significant bit in ADCH)
    (1 << MUX1);                // Choose AD input AD2 (PC2)
 
}
 
int main() {
 
  InitPWM();
  InitADC();
 
  for (;;)
    {
      OCR0A = adc();                     // PWM output = ADC input
    }
 
  return 0;
}
