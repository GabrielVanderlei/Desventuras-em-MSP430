
#include <msp430.h>
int mostrar(int numero);

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 
  P1DIR |= BIT0;                           
  P3DIR |= BIT6;
  
  int i = 0;
  while(1){
      i++;
      mostrar(i % 4);
      __delay_cycles(1000000);
  }
}

int mostrar(int numero){
  switch(numero){
  case 0:
    P1OUT &= ~BIT0;
    P3OUT &= ~BIT6;
    break;
  case 1:
    P1OUT |= BIT0;
    P3OUT &= ~BIT6;
    break;
  case 2:
    P1OUT &= ~BIT0;
    P3OUT |= BIT6;
    break;
  case 3:
    P1OUT |= BIT0;
    P3OUT |= BIT6;
    break;
  }
  
  return 0;
}

