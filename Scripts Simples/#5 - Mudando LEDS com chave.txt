#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 
  P1DIR |= BIT0;                           
  P3DIR |= BIT6;
  
  while(1){
    if(P1IN & BIT7){ 
      P1OUT |= BIT0;
      P3OUT &= ~BIT6;
    }
    else{ 
      P1OUT &= ~BIT0;
      P3OUT |= BIT6;
    }
  }
}
