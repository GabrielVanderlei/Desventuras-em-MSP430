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

int main(void)
{
  desligarWatchDog(); // Desliga WD
  habilitarLeds(); // Direciona os LEDS para saída
  limparLeds(); // Limpa os resultados anteriores do LED
  configurarI2C(); // Configura o I2C para as portas específicas
  tornarMasterI2C(0x1E); // Torna a placa SLAVE de endereço 0x48
  ESTADO = 1; // Transforma a placa em receptor.
    P1DIR |= BIT7 + BIT6;
  P1OUT &= ~BIT6 + ~BIT7;
  
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
  case  6: slaveInicioI2C();break; // Só serve para o TX
  case  8: slaveFimI2C();__bic_SR_register_on_exit(LPM0_bits); break;  // Só serve para o TX // Exit LPM0 if data was trans
  case 10: 
    if(ESTADO == 0){
      
    counter++;
    
    if(counter < 8){
      if(dadoAnterior != dadosRecebidosI2C()){
        if(Reg != dadosRecebidosI2C()){
          Reg = dadosRecebidosI2C(); 
          piscarLedVerde(2);
        }
          dadoAnterior = dadosRecebidosI2C();
      }
      
    }
    if(counter == 8){
      counter = 2;
      ESTADO = 1;
    }
    }
      P1OUT ^= BIT6;
      P1OUT ^= BIT6;  
    __bic_SR_register_on_exit(LPM0_bits); 
    break;
  case 12: 

    __delay_cycles(100000); // Tempo de envio entre um dado e outro.
    if(ESTADO == 1){ // Está enviando
      if(configurado == 0){
            counter++;
            switch(counter){
            // 8 exemplos, 15 Hz, normal
            case 1: enviarDadosI2C(0x3C); break;
            case 2: enviarDadosI2C(0x00); break;
            case 3: enviarDadosI2C(0x70); break;
            
            // Ganho = 5
            case 4: enviarDadosI2C(0x3C); break;
            case 5: enviarDadosI2C(0x01); break;
            case 6: enviarDadosI2C(0xA0); break;
            
            // Medição contínua
            case 7: enviarDadosI2C(0x3C); break;
            case 8: enviarDadosI2C(0x02); break;
            case 9: enviarDadosI2C(0x00); configurado = 1; counter = 0; break;
            }
      }
      
      else{
        counter++;
        switch(counter){
        // Ler eixo X
        case 1: enviarDadosI2C(0x3D); break;
        case 2: enviarDadosI2C(0x06); ESTADO = 0;counter = 0;break;
        case 3: enviarDadosI2C(0x3C); break;
        case 4: enviarDadosI2C(0x03); counter = 0;break;
        }
        piscarLedVermelho(2);
      }
      
    }
      
    P1OUT ^= BIT7;
    P1OUT ^= BIT7;
      __bic_SR_register_on_exit(LPM0_bits); 

    break;
  default: break; 
  }
}
