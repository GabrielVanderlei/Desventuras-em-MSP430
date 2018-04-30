/*
  O Sistema de IO aqui estudado trabalha apenas com estados (0 e 1)
  Por isso existem algumas peculiaridades.
*/

#include <msp430.h>

int main(void)
{
  // Para o contador WD -> Se o sistema não responder em um período de tempo ele reseta.
  WDTCTL = WDTPW + WDTHOLD;
  // Determina a direção de saída para P1.0
  // Ao manipular entradas de IO não podemos utilizar apenas o '=' pois assim iriamos modificar todos os dados da porta.
  // Esses dados incluem informações do prórpio sistema, impedindo o uso mesmo em caso de não termos definido nada anteriormente.
  // Portanto, devemos utilizar o |= para setar algum valor para '1' -> Pois x+1 = 1
  P1DIR |= 0x01; 
  
  // Envia 1 para a saída (P1.0)
  P1OUT |= 0x01;
  // ~0x01 = 11111110
  // &= -> é necessário para setar o valor para '0' pois x.0 = 0
  P1OUT &= ~0x01;
}
