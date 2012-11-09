/* Convert integer to a string */
#include <inttypes.h>
char* itoa(uint16_t number, uint8_t base)
{
  if(!number){
    return "0\r\n";
  }
  
  static char buf[16] = {0};
  register char i = 12;
  buf[14] = '\n';
  buf[13] = '\r';
  char m = 0;
  
  if(number < 0){
    number = number * (-1);
    m = 1;
  }
  
  for(; number && i ; --i, number /= base)
    buf[i] = "0123456789ABCDEF"[number % base];
  
  if(m){
    buf[i] = '-';
    return &buf[i];
  }else{
    return &buf[i+1];
  }
}

int main(void)
{
  int value;
  char *num = itoa(132, 10);

  return 0;
} 
