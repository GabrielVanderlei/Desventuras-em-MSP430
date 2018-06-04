
#include <msp430.h>
#include <msp430_helper.h>


int libera = 0;
int counter = 0;
char Reg = 0;
char r1 = 0, r2 = 0, r3 = 0;
char dadoAnterior = 0;
char configurado = 0;
int i;

int main(void)
{
  desligarWatchDog(); // Desliga WD
  ligarLeds(); // Direciona os LEDS para saída
  limparLeds(); // Limpa os resultados anteriores do LED
  configurarI2C(); // Configura o I2C para as portas específicas
  tornarSlaveI2C(0x48); // Torna a placa SLAVE de endereço 0x48
  P1DIR |= BIT7 + BIT6;
  P1OUT &= ~BIT6 + ~BIT7;
  ESTADO = 0; // Transforma a placa em receptor.
  
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
    if(dadoAnterior != dadosRecebidosI2C()){
    limparLeds();
    counter++;
    switch(counter){
    case 1: r1 = dadosRecebidosI2C();break;
    case 2: r2 = dadosRecebidosI2C();break;
    case 3: r3 = dadosRecebidosI2C();counter = 0;break;
    }
    dadoAnterior=dadosRecebidosI2C();
    }
    
    if(configurado < 3){
      if(
         (r3 == 0x3C) &&
         (r2 == 0x00) &&
         (r1 == 0x70)
         ) configurado++;
      
      
      if(
         (r1 == 0x3C) &&
         (r2 == 0x01) &&
         (r3 == 0xA0)
         ) configurado++;
      
      
      if(
         (r1 == 0x3C) &&
         (r2 == 0x02) &&
         (r3 == 0x00)
         ) configurado++;
    }
    else{
      if(
         (r1 == 0x3D) &&
         (r2 == 0x06) 
           ){ ESTADO = 1;piscarLedVerde(300);}
      
    }
    
      P1OUT ^= BIT6;
      P1OUT ^= BIT6;
      
    __bic_SR_register_on_exit(LPM0_bits); 
    break;
  
  case 12: 
    __delay_cycles(10000);
    if(ESTADO == 1){
      enviarDadosI2C(1);
      ESTADO = 0;
      piscarLedVermelho(300);
    }  
    
    P1OUT ^= BIT7;
    P1OUT ^= BIT7;
    
    __bic_SR_register_on_exit(LPM0_bits); 
    break;
  default: break; 
  }
}
