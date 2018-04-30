// Existem dois LEDS na placa analisada
// Um está localizado em P1.0 (Verde)
// E o outro está localizado em P3.6 (Vermelho)
// Caso essas informações não estejam funcionando em sua placa possivelmente as configurações são diferentes
// Nesse caso recomendo analisar todas as portas setando PxDIR = 0xFF e PxOUT = OxFF

#include <msp430.h>

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 
  P3DIR |= BIT6;                            
  P3OUT |= BIT6;
}

