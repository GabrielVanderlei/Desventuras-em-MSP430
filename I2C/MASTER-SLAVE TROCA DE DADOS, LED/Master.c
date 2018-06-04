
#include <msp430.h>
#include <msp430_helper.h>

int counter = 0;
int i;
char DADO_ANTERIOR = 100;

int main(void)
{
  desligarWatchDog(); // Desliga WD
  ligarLeds(); // Direciona os LEDS para saída
  limparLeds(); // Limpa os resultados anteriores do LED
  configurarI2C(); // Configura o I2C para as portas específicas
  tornarMasterI2C(0x48); // Torna a placa SLAVE de endereço 0x48
  P1DIR |= BIT7 + BIT6;
  P1OUT &= ~BIT6 + ~BIT7;
  ESTADO = 1; // Transforma a placa em transmissor.
  
  while(1)
  { 
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
  case  6: break; // Só serve para o TX
  case  8: break;  // Só serve para o TX // Exit LPM0 if data was trans
  case 10: 
    // Vetor de recebimento
    if(DADO_ANTERIOR != dadosRecebidosI2C()){
      limparLeds();
      switch(dadosRecebidosI2C()){
        case 0: DesligaLEDVerde;
                //DesligaLEDVermelho;
                break;
        case 1: LigaLEDVerde;
                //DesligaLEDVermelho;
                break;
        case 2: DesligaLEDVerde;
                //LigaLEDVermelho;
                break;
        case 3: LigaLEDVerde;
                //LigaLEDVermelho;
                break;
      case 4: ESTADO = 1;
               break;
      default: LigaLEDVermelho;
      }
      
      P1OUT ^= BIT6;
      P1OUT ^= BIT6;
      DADO_ANTERIOR = dadosRecebidosI2C();
    }
    __bic_SR_register_on_exit(LPM0_bits); 
    break;
  
  case 12: 
      __delay_cycles(500000); // Tempo de envio entre um dado e outro.
    if(ESTADO == 1){
            // Ao enviar 4, ambos devem mudar de estado
      piscarLedVermelho(4); 
      enviarDadosI2C(counter); // Transmite os dados do contador para a outra placa
      counter++; // Soma o contador.
        
      if(counter > 4){ 
        counter = 0; // Reinicia o contador  
        ESTADO = 0; // Transforma a placa em receptor
        limparLeds(); 
       // LigaLEDVerde;
      }
    P1OUT ^= BIT7;
    P1OUT ^= BIT7;
    }
      __bic_SR_register_on_exit(LPM0_bits); 

    break;
  default: break; 
  }
}