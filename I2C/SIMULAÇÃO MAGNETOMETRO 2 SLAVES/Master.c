/*
  Esse código deve funcionar caso o magnetometro apenas 
  utilize um endereço de 7-bits.
  CÓDIGO DO MASTER
  Esse código comanda o magnetometro.
*/

#include <msp430.h>
#include <msp430_helper.h>

int libera = 0;
int counter = 0;
char Reg = 0;
char XRegAnterior = 0;
char YRegAnterior = 0;
char ZRegAnterior = 0;
char dadoAnterior = 0;
char configurado = 0;
int i;
int SLAVE = 1;

int main(void)
{
  desligarWatchDog(); // Desliga WD
  ligarLeds(); // Direciona os LEDS para saída
  limparLeds(); // Limpa os resultados anteriores do LED
  configurarI2C(); // Configura o I2C para as portas específicas
  tornarMasterI2C(0x1E); // Torna a placa SLAVE de endereço 0x48
  ESTADO = 1; // Transforma a placa em receptor.
    P1DIR |= BIT7 + BIT6;
  P1OUT &= ~BIT6 + ~BIT7;
  
  while(1)
  { 
    if((counter % 2) == 0) trocarEndereco(0x3C);
    else trocarEndereco(0x3D);
    verificarEstado(); // Verifica se o estado da placa foi alterado.
    comecarI2C(); // Inicia o protocolo I2C
    habilitarInterrupcoes();
  }  
}
  
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
  switch(__even_in_range(UCB0IV,12))
  {
  case  0: break;                           // Vector  0: No interrupts
  case  2: break;                           // Vector  2: ALIFG
  case  4: break;                           // Vector  4: NACKIFG
  case  6: slaveInicioI2C();break; // Só serve para o TX
  case  8: slaveFimI2C();__bic_SR_register_on_exit(LPM0_bits); break;  // Só serve para o TX // Exit LPM0 if data was trans
  case 10: 
    if(ESTADO == 0){
      if(dadosRecebidosI2C() == 1) LigaLEDVerde;
    }
      P1OUT ^= BIT6;
      P1OUT ^= BIT6;  
    __bic_SR_register_on_exit(LPM0_bits); 
    break;
  case 12: 

    __delay_cycles(100000); // Tempo de envio entre um dado e outro.
    if(ESTADO == 1){ // Está enviando
      counter++;
      if(verificarEndereco() == 0x3C){ SLAVE = 0; enviarDadosI2C(counter % 2);}
      if(verificarEndereco() == 0x3D){ SLAVE = 1; enviarDadosI2C(counter % 2);}
    }
      
    P1OUT ^= BIT7;
    P1OUT ^= BIT7;
      __bic_SR_register_on_exit(LPM0_bits); 

    break;
  default: break; 
  }
}
