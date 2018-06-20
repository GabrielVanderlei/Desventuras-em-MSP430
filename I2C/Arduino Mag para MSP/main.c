/*
  Esse código deve funcionar caso o magnetometro apenas 
  utilize um endereço de 7-bits.
  CÓDIGO DO MASTER
  Esse código comanda o magnetometro.
*/

#include <msp430.h>
#include <msp430_helper.h>

int libera = 0;
char Reg = 0;
char XRegAnterior = 0;
char YRegAnterior = 0;
char ZRegAnterior = 0;
char dadoAnterior = 0;

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
  case  4: 
    if(ESTADO == 0){
      ESTADO = 1;
    }
    break;                           // Vector  4: NACKIFG
  case  6: slaveInicioI2C();break; // Só serve para o TX
  case  8: slaveFimI2C();__bic_SR_register_on_exit(LPM0_bits); break;  // Só serve para o TX // Exit LPM0 if data was trans
  case 10: 
    if(ESTADO == 0){
      piscarLedVermelho(300);
      
      // Verifica NAK
      if(dadosRecebidosI2C() && BIT7){ 
        ESTADO = 1;
      }
    }
      P1OUT ^= BIT6;
      P1OUT ^= BIT6;  
    __bic_SR_register_on_exit(LPM0_bits); 
    break;
  case 12: 

    //__delay_cycles(100000); // Tempo de envio entre um dado e outro.
    if(ESTADO == 1){ // Está enviando
      if(configurado == 0){
          counter++;
          // Setup
            switch(counter){
              
            case 1: enviarDadosI2C(0x0A); ESTADO = 0; break;
            case 2: enviarDadosI2C(0x0B); ESTADO = 0; break;
            case 3: enviarDadosI2C(0x0C); ESTADO = 0; break;
            
            case 4: enviarDadosI2C(0x01); break;
            case 5: enviarDadosI2C(0x20); break;
            case 6: UCB0CTL1 |= UCTR + UCTXSTT; 
            
            case 7: enviarDadosI2C(0x02); ESTADO = 0; break;
            
            case 8: enviarDadosI2C(0x02); break;
            case 9: enviarDadosI2C(0x00); break;
            case 10: UCB0CTL1 |= UCTR + UCTXSTT; 
            
            case 11: enviarDadosI2C(0x00); ESTADO = 0; break;
            
            case 12: enviarDadosI2C(0x00); break;
            case 13: enviarDadosI2C(0x10); break;
            case 14: UCB0CTL1 |= UCTR + UCTXSTT; 
            
            case 15: enviarDadosI2C(0x00); ESTADO = 0; break;
            
            case 16: enviarDadosI2C(0x00); break;
            case 17: enviarDadosI2C(0x10); break;
            case 18: UCB0CTL1 |= UCTR + UCTXSTT; 
            
            case 19: enviarDadosI2C(0x01); break;
            case 20: enviarDadosI2C(0x20); break;
            case 21: UCB0CTL1 |= UCTR + UCTXSTT; 
            
            case 22: enviarDadosI2C(0x02); ESTADO = 0; break;
            
            case 23: enviarDadosI2C(0x02); break;
            case 24: enviarDadosI2C(0x01); ESTADO = 0; configurado = 1; counter = 0; break;
            default: enviarDadosI2C(0xF1);
            }
            
        }
        else{
          // Loop
          if(counter == 0) __delay_cycles(5000);
          counter++;
          switch(counter){
            case 1: enviarDadosI2C(0x02); ESTADO = 0; break;
            
            case 2: enviarDadosI2C(0x02); break;
            case 3: enviarDadosI2C(0x01); break;
            case 4: UCB0CTL1 |= UCTR + UCTXSTT; 
            
            // ACK + NACK
            case 5: enviarDadosI2C(0x03); ESTADO = 0; break;
            case 6: enviarDadosI2C(0x07); ESTADO = 0; break;
            case 7: enviarDadosI2C(0x05); ESTADO = 0; break;
           
            case 8: enviarDadosI2C(0x03); ESTADO = 0; break;
            case 9: enviarDadosI2C(0x07); ESTADO = 0; break;
            case 10: enviarDadosI2C(0x05); ESTADO = 0; counter = 0; break;
            default: enviarDadosI2C(0xF2);
          }
        }
            piscarLedVerde(300);
      }
      
      
      
    P1OUT ^= BIT7;
    P1OUT ^= BIT7;
      __bic_SR_register_on_exit(LPM0_bits); 

    break;
  default: break; 
  }
}