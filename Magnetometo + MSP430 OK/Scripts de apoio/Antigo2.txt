/*
  Esse código deve funcionar caso o magnetometro apenas 
  utilize um endereço de 7-bits.
  CÓDIGO DO MASTER
  Esse código comanda o magnetometro.
*/

#include <msp430.h>
//#include <msp430_helper.h>

int libera = 0;
char Reg = 0;
char XRegAnterior = 0;
char YRegAnterior = 0;
char ZRegAnterior = 0;
char dadoAnterior = 0;


int piscaLedVermelhoContador = 0;
int piscarLedVerdeContador = 0;

int MASTER_I2C = 0;
int ESTADO = 0;
int ESTADO_ANTERIOR = 5;
int intervalo = 0;
char configurado = 0;
int counter = 0;

#define LigaLEDVerde P1OUT |= BIT0
#define LigaLEDVermelho P3OUT |= BIT6
#define DesligaLEDVerde P1OUT &= ~BIT0
#define DesligaLEDVermelho P3OUT &= ~BIT6

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;// Desliga WD
 
  P3DIR |= BIT6; // Direciona os LEDS para saída
  P1DIR |= BIT0;
  
  P3OUT &= ~BIT6;// Limpa os resultados anteriores do LED
  P1OUT &= ~BIT0;
  
  P1OUT &= ~BIT0;// Configura o I2C para as portas específicas
  P1DIR |= BIT0;
  
  PMAPPWD = 0x02D52;                        // Get write-access to port mapping regs  
  P1MAP3 = PM_UCB0SDA;                      // Map UCB0SDA output to P1.3 
  P1MAP2 = PM_UCB0SCL;                      // Map UCB0SCL output to P1.2 
  PMAPPWD = 0;                              // Lock port mapping registers 
  P1SEL |= BIT2 + BIT3;                     // Select P1.2 & P1.3 to I2C function 
  
  UCB0CTL1 |= UCSWRST;                      // Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
  UCB0BR0 = 2;                             // fSCL = SMCLK/12 = ~100kHz
  UCB0BR1 = 0;
  UCB0I2CSA = 0x1E;                         // Slave Address is 1Eh
  UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
  MASTER_I2C = 1;
  
  
  ESTADO = 1; // Transforma a placa em receptor.
  P1DIR |= BIT7 + BIT6;
  P1OUT &= ~BIT6 + ~BIT7;
  
  while(1)
  { 
    //verificarEstado(); // Verifica se o estado da placa foi alterado.
    if(ESTADO_ANTERIOR != ESTADO){
        
      if(MASTER_I2C && ESTADO){
          //tornarTxMasterI2C
        
      
          UCB0CTL1 |= UCTR + UCTXSTT;
          P1OUT ^= BIT6;  
          UCB0IE &= ~UCRXIE;
          P1OUT ^= BIT6;
          UCB0IE |= UCTXIE + UCSTPIE + UCSTTIE;
      }
      
      if(MASTER_I2C && !ESTADO){
          //tornarRxMasterI2C
          UCB0CTL1 &= ~UCTR + ~UCTXSTT ; 
          UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
          UCB0IE |= UCRXIE;
      }
      
      ESTADO_ANTERIOR = ESTADO;
    }
    
   __delay_cycles(50);
    while(UCB0CTL1 & UCTXSTP);              // Ensure stop condition got sent
    UCB0CTL1 |= UCTXSTT;                    // I2C start condition
    
    if((ESTADO == 0)){  
      while(UCB0CTL1 & UCTXSTT);              // Start condition sent?
      UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
     
    }
     __bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts
  }  
}
  
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
  switch(__even_in_range(UCB0IV,12))
  {
  case  0: break;                           // Vector  0: No interrupts
  case  2: break;                           // Vector  2: ALIFG
  case  4:                                  // Vector  4: NACKIFG
    if(ESTADO == 0){
      ESTADO = 1;
    }
    break;
    
  case  6: //Vector 6   // Só serve para o TX
    UCB0IFG &= ~UCSTTIFG; //salve inicio I2C
    break; 
    
  case  8: //Vector 8  // Só serve para o TX 
    UCB0IFG &= ~UCSTPIFG;// Fim do envio de dados do ciclo, não da transmissão.
    __bic_SR_register_on_exit(LPM0_bits);// Exit LPM0 if data was trans
    break;  
    
  case 10: 
    if(ESTADO == 0)
    {
      //piscarLedVermelho(300);
      intervalo = 300;
      if((piscaLedVermelhoContador % intervalo) == 1) P3OUT ^= BIT6;
      piscaLedVermelhoContador++;
    }
      
      // Verifica NAK
    if(UCB0RXBUF && BIT7)ESTADO = 1;
  
    __bic_SR_register_on_exit(LPM0_bits); 
    break;
   
  case 12: 

    //__delay_cycles(100000); // Tempo de envio entre um dado e outro.
    if(ESTADO == 1){ // Está enviando
      if(configurado == 0){
          counter++;
          // Setup
            switch(counter){
              
            case 1: UCB0TXBUF = 0x0A; ESTADO = 0; break;
            case 2: UCB0TXBUF = 0x0B; ESTADO = 0; break;
            case 3: UCB0TXBUF = 0x0C; ESTADO = 0; break;
            
            case 4: UCB0TXBUF = 0x01; break;
            case 5: UCB0TXBUF = 0x20; break;
            case 6: UCB0CTL1 |= UCTR + UCTXSTT; 
            
            case 7: UCB0TXBUF = 0x02; ESTADO = 0; break;
            
            case 8: UCB0TXBUF = 0x02; break;
            case 9: UCB0TXBUF = 0x00; break;
            case 10: UCB0CTL1 |= UCTR + UCTXSTT; 
            
            case 11: UCB0TXBUF = 0x00; ESTADO = 0; break;
            
            case 12: UCB0TXBUF = 0x00; break;
            case 13: UCB0TXBUF = 0x10; break;
            case 14: UCB0CTL1 |= UCTR + UCTXSTT; 
            
            case 15: UCB0TXBUF = 0x00; ESTADO = 0; break;
            
            case 16: UCB0TXBUF = 0x00; break;
            case 17: UCB0TXBUF = 0x10; break;
            case 18: UCB0CTL1 |= UCTR + UCTXSTT; 
            
            case 19: UCB0TXBUF = 0x01; break;
            case 20: UCB0TXBUF = 0x20; break;
            case 21: UCB0CTL1 |= UCTR + UCTXSTT; 
            
            case 22: UCB0TXBUF = 0x02; ESTADO = 0; break;
            
            case 23: UCB0TXBUF = 0x02; break;
            case 24: UCB0TXBUF = 0x01; ESTADO = 0; configurado = 1; counter = 0; break;
            default: UCB0TXBUF = 0xF1;
            }
            
        }
        else{
          // Loop
          if(counter == 0) __delay_cycles(5000);
          counter++;
          switch(counter){
            case 1: UCB0TXBUF = 0x02; ESTADO = 0; break;
            
            case 2: UCB0TXBUF = 0x02; break;
            case 3: UCB0TXBUF = 0x01; break;
            case 4: UCB0CTL1 |= UCTR + UCTXSTT; 
            
            // ACK + NACK
            case 5: UCB0TXBUF = 0x03; ESTADO = 0; break;
            case 6: UCB0TXBUF = 0x07; ESTADO = 0; break;
            case 7: UCB0TXBUF = 0x05; ESTADO = 0; break;
           
            case 8: UCB0TXBUF = 0x03; ESTADO = 0; break;
            case 9: UCB0TXBUF = 0x07; ESTADO = 0; break;
            case 10: UCB0TXBUF = 0x05; ESTADO = 0; counter = 0; break;
            default: UCB0TXBUF = 0xF2;
          }
        }
            
            intervalo = 300; 
            //void piscarLedVerde(int intervalo)
            if((piscarLedVerdeContador % intervalo) == 1) P1OUT ^= BIT0;
            piscarLedVerdeContador++;
      }
      
      
      
    P1OUT ^= BIT7;
    P1OUT ^= BIT7;
      __bic_SR_register_on_exit(LPM0_bits); 

    break;
  default: break; 
  }
}