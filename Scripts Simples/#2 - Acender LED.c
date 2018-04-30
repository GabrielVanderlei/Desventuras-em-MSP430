/*****
// Primeiramente use o script abaixo para verificar qual o BIT que está conectado ao LED (Pois varia de versão para versão)

#include <msp430.h>

int main(void)
{
  // Para o contador WD -> Se o sistema não responder em um período de tempo ele reseta.
  WDTCTL = WDTPW + WDTHOLD;
  
  // Portanto, devemos utilizar o |= para setar algum valor para '1' -> Pois x+1 = 1
  P1DIR |= 0x01; 
  P1OUT &= ~0x01;
  P1DIR &= ~0x02; 
  //P1DIR &= ~0x10;
  while(1){
    //if(!(BIT0 & P1IN)) P1OUT |= 0x01;
    if(!(BIT1 & P1IN)) P1OUT |= 0x01;
    if(!(BIT2 & P1IN)) P1OUT |= 0x01;
    if(!(BIT3 & P1IN)) P1OUT |= 0x01;
    if(!(BIT4 & P1IN)) P1OUT |= 0x01;
    if(!(BIT5 & P1IN)) P1OUT |= 0x01;
    if(!(BIT6 & P1IN)) P1OUT |= 0x01;
    if(!(BIT7 & P1IN)) P1OUT |= 0x01;
  }
  // Envia 1 para a saída (P1.0)
  //P1OUT ^= 0x01;
  // ~0x01 = 11111110
  // &= -> é necessário para setar o valor para '0' pois x.0 = 0
  //P1OUT &= ~0x01;
  
/*
  while (1)                                 // Testa P1.4
  {
    if (BIT4 & P1IN)
     P1OUT |= 0x01;                        // Se P1.4 = 1; P1.0 = 1
    else
      P1OUT &= ~0x01;                       // Se P1.4 = 0; Reseta.
  }*/
}

***/


#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;
  
  P1DIR |= 0x01; 
  
  while(1){
    if(!(BIT7 & P1IN)) P1OUT |= 0x01;
    else P1OUT &= ~0x01;
  }
}

