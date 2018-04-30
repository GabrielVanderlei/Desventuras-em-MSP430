/**
O LED verde fica aceso por 20 segundos, em seguida o LED pisca por 10 segundos e o
LED vermelho acende or 10 segundos, após isto o LED vermelho pisca e após mais 10 segundos o sistema reinicia.
**/

#include <msp430.h>
#define intervalo 62500
#define ACENDE_LED_VERDE P1OUT |= BIT0;
#define ACENDE_LED_VERMELHO P3OUT |= BIT6;
#define APAGA_LED_VERDE P1OUT &= ~BIT0;
#define APAGA_LED_VERMELHO P3OUT &= ~BIT6;
#define PISCA_LED_VERDE P1OUT ^= BIT0;
#define PISCA_LED_VERMELHO P3OUT ^= BIT6;

float timer = 0;
int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                
  P1DIR |= BIT0;                            
  P3DIR |= BIT6;                            
  TA1CCTL0 = CCIE;                         
  TA1CCR0 = intervalo;
  TA1CTL = TASSEL_2 + MC_3 + TACLR + ID_2;         

  __bis_SR_register(LPM0_bits + GIE);       
  __no_operation();                        
}

#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
  timer+=0.5;
  if(timer <= 20){
    ACENDE_LED_VERDE;
    APAGA_LED_VERMELHO;
  }
  else if(timer <= 30){
    PISCA_LED_VERDE;
  }
  else if(timer <= 40){
    APAGA_LED_VERDE;
    ACENDE_LED_VERMELHO;
  }
  else if(timer <= 50){
    PISCA_LED_VERMELHO;
  }
  else{
    timer = 0;
  }
}

