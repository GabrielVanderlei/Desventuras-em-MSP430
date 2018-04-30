#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;
  
  P1DIR |= BIT0; 
  
  while(1){
    P1OUT ^= BIT0;
    __delay_cycles(1000000);
  }
}
