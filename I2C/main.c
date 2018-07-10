/*
  Esse código deve funcionar caso o magnetometro apenas 
  utilize um endereço de 7-bits.
  CÓDIGO DO MASTER
  Esse código comanda o magnetometro.
*/

#include <msp430.h>

int counter = 0;
int configurado = 0;
int endereco = 0x1E;

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD; 
  P1OUT &= ~BIT0;
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
  UCB0I2CSA = endereco;                         // Slave Address is 048h
  UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
  
  UCB0CTL1 |= UCTR + UCTXSTT;
  UCB0IE &= ~UCRXIE;
  UCB0IE |= UCTXIE + UCSTPIE + UCSTTIE;
  
  while(1)
  { 
      __delay_cycles(50);
      
      while(UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
      UCB0CTL1 |= UCTXSTT;                    // I2C start condition
     
      
      __bis_SR_register(LPM0_bits + GIE); 
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
  
  case  6: 
    UCB0IFG &= ~UCSTTIFG; 
    break; 
    
  case  8: 
    UCB0IFG &= ~UCSTPIFG;
    __bic_SR_register_on_exit(LPM0_bits); 
    break;  
    
  case 10: 
      
    if(UCB0RXBUF && BIT7){ 
      UCB0CTL1 |= UCTR + UCTXSTP;
      UCB0IE &= ~UCRXIE;
      UCB0IE |= UCTXIE + UCSTPIE + UCSTTIE;
      UCB0CTL1 &= ~UCTXSTT ; 
    }
    
    __bic_SR_register_on_exit(LPM0_bits); 
    break;
  case 12: 

      if(configurado == 0){
          counter++;
          // Setup
            switch(counter){
              
            case 1: 
              UCB0TXBUF = 0x0A;   
              UCB0CTL1 &= ~UCTR + ~UCTXSTT ; 
              UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
              UCB0IE |= UCRXIE;
              break;
              
            case 2: 
              UCB0TXBUF = 0x0B;   
              UCB0CTL1 &= ~UCTR + ~UCTXSTT ; 
              UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
              UCB0IE |= UCRXIE;
              break;
              
            case 3:
              UCB0TXBUF = 0x0C;   
              UCB0CTL1 &= ~UCTR + ~UCTXSTT ; 
              UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
              UCB0IE |= UCRXIE;
              break;
            
            case 4:
              UCB0TXBUF = 0x01;   
              break;
              
            case 5:
              UCB0TXBUF = 0x20;   
              break;
              
            case 6: 
              UCB0CTL1 |= UCTR + UCTXSTT;
              break; 
            
            case 7: 
              UCB0TXBUF = 0x02;   
              UCB0CTL1 &= ~UCTR + ~UCTXSTT ; 
              UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
              UCB0IE |= UCRXIE;
              break;
            
            case 8: 
              UCB0TXBUF = 0x02;  
              break;
              
            case 9: 
              UCB0TXBUF = 0x00;  
              break;
              
            case 10: 
              UCB0CTL1 |= UCTR + UCTXSTT;
              break;
            
            case 11: 
              UCB0TXBUF = 0x00;
              UCB0CTL1 &= ~UCTR + ~UCTXSTT ; 
              UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
              UCB0IE |= UCRXIE;
              break;
            
            case 12: 
              UCB0TXBUF = 0x00;
              break;
              
            case 13:
              UCB0TXBUF = 0x10;
              break;
              
            case 14: 
              UCB0CTL1 |= UCTR + UCTXSTT;
              break;
            
            case 15:
              UCB0TXBUF = 0x00;
              UCB0CTL1 &= ~UCTR + ~UCTXSTT ; 
              UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
              UCB0IE |= UCRXIE;
              break;
            
            case 16:
              UCB0TXBUF = 0x00; 
              break;
              
            case 17:
              UCB0TXBUF = 0x10;
              break;
              
            case 18: 
              UCB0CTL1 |= UCTR + UCTXSTT; 
              break;
            
            case 19: 
              UCB0TXBUF = 0x01; 
              break;
              
            case 20: 
              UCB0TXBUF = 0x02;
              break;
            
            case 21: 
              UCB0CTL1 |= UCTR + UCTXSTT;
              break;
            
            case 22:
              UCB0TXBUF = 0x02;
              UCB0CTL1 &= ~UCTR + ~UCTXSTT ; 
              UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
              UCB0IE |= UCRXIE;
              break;
            
            case 23:
              UCB0TXBUF = 0x02;
              break;
              
            case 24: 
              UCB0TXBUF = 0x01;
              UCB0CTL1 &= ~UCTR + ~UCTXSTT ; 
              UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
              UCB0IE |= UCRXIE;
              configurado = 1; 
              counter = 0; 
              break;
              
            default: 
              UCB0TXBUF = 0xFF;
              break;
            }
            
        }
        else{
          // Loop
          if(counter == 0) __delay_cycles(5000);
          counter++;
          switch(counter){
            case 1:
              UCB0TXBUF = 0x02;
              UCB0CTL1 &= ~UCTR + ~UCTXSTT ; 
              UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
              UCB0IE |= UCRXIE;
              break;
            
            case 2:
              UCB0TXBUF = 0x02;
              break;
              
            case 3:
              UCB0TXBUF = 0x01;
              break;
              
            case 4: 
              UCB0CTL1 |= UCTR + UCTXSTT; 
              break;
            
            // ACK + NACK
            case 5:
              UCB0TXBUF = 0x03;
              UCB0CTL1 &= ~UCTR + ~UCTXSTT ; 
              UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
              UCB0IE |= UCRXIE;
              break;
              
            case 6:
              UCB0TXBUF = 0x07;
              UCB0CTL1 &= ~UCTR + ~UCTXSTT ; 
              UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
              UCB0IE |= UCRXIE;
              break;
              
            case 7: 
              UCB0TXBUF = 0x05;
              UCB0CTL1 &= ~UCTR + ~UCTXSTT ; 
              UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
              UCB0IE |= UCRXIE;
              break;
           
            case 8:
              UCB0TXBUF = 0x03;
              UCB0CTL1 &= ~UCTR + ~UCTXSTT ; 
              UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
              UCB0IE |= UCRXIE;
              break;
              
            case 9: 
              UCB0TXBUF = 0x03;
              UCB0CTL1 &= ~UCTR + ~UCTXSTT ; 
              UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
              UCB0IE |= UCRXIE;
              break;
              
            case 10: 
              UCB0TXBUF = 0x05;
              UCB0CTL1 &= ~UCTR + ~UCTXSTT ; 
              UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
              UCB0IE |= UCRXIE;
              counter = 0;
              break;
              
            default: 
              UCB0TXBUF = 0xFF;
              break;
          }
      }
      
      __bic_SR_register_on_exit(LPM0_bits); 

    break;
  default: break; 
  }
}