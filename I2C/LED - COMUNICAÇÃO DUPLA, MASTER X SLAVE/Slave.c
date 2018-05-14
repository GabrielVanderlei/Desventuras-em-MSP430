/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 * 
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//  CC430F614x Demo - USCI_B0 I2C Master RX multiple bytes from MSP430 Slave
//
//  Description: This demo connects two MSP430's via the I2C bus. The slave
//  transmits to the master. This is the MASTER CODE. It continuously
//  receives an array of data and demonstrates how to implement an I2C
//  master receiver receiving multiple bytes using the USCI_B0 TX interrupt.
//  ACLK = n/a, MCLK = SMCLK = BRCLK = default DCO = ~1.045MHz
//
// *** to be used with "cc430x614x_uscib0_i2c_11.c" ***
//
//                                /|\  /|\
//               CC430F6147      10k  10k     CC430F6147
//                   slave         |    |        master
//             -----------------   |    |   -----------------
//           -|XIN  P1.3/UCB0SDA|<-|----+->|P1.3/UCB0SDA  XIN|-
//            |                 |  |       |                 | 32kHz
//           -|XOUT             |  |       |             XOUT|-
//            |     P1.2/UCB0SCL|<-+------>|P1.2/UCB0SCL     |
//            |                 |          |             P1.0|--> LED
//
//   G. Larmore
//   Texas Instruments Inc.
//   June 2012
//   Built with CCS v5.2 and IAR Embedded Workbench Version: 5.40.1
//******************************************************************************

#include <msp430.h>

void desligarWatchDog(){
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
}

void configurarI2C(){
  PMAPPWD = 0x02D52;                        // Get write-access to port mapping regs  
  P1MAP3 = PM_UCB0SDA;                      // Map UCB0SDA output to P1.3 
  P1MAP2 = PM_UCB0SCL;                      // Map UCB0SCL output to P1.2 
  PMAPPWD = 0;                              // Lock port mapping registers 
  
  P1SEL |= BIT2 + BIT3;                     // Select P1.2 & P1.3 to I2C function
}

void tornarMasterI2C(int endereco){
  UCB0CTL1 |= UCSWRST;                      // Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
  UCB0BR0 = 12;                             // fSCL = SMCLK/12 = ~100kHz
  UCB0BR1 = 0;
  UCB0I2CSA = endereco;                         // Slave Address is 048h
  UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
}

void tornarSlaveI2C(int endereco){
  UCB0CTL1 |= UCSWRST;                      // Enable SW reset
  UCB0CTL0 = UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
  UCB0I2COA = endereco;                         // Slave Address is 048h}
}

void tornarRxSlaveI2C(){
  UCB0CTL1 &= ~UCSWRST; 
  UCB0IE &= ~UCTXIE;
  UCB0IE |= UCRXIE + UCSTPIE + UCSTTIE;
}

void tornarTxSlaveI2C(){
  UCB0CTL1 &= ~UCSWRST; 
  UCB0IE &= ~UCRXIE;
  UCB0IE |= UCTXIE + UCSTTIE + UCSTPIE;
}

void tornarRxMasterI2C(){
  UCB0CTL1 &= ~UCSWRST; 
  UCB0IE &= ~UCTXIE;
  UCB0IE |= UCRXIE;
}

void tornarTxMasterI2C(){
  UCB0CTL1 &= ~UCSWRST; 
  UCB0IE &= ~UCRXIE;
  UCB0IE |= UCTXIE;
}

void comecarI2C(){
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTXSTT;                    // I2C start condition
}

void habilitarInterrupcoes(){
    __bis_SR_register(LPM0_bits + GIE);     // Enter LPM0, enable interrupts
}

unsigned char dadosRecebidosI2C(){ 
  return UCB0RXBUF;
}

void enviarDadosI2C(unsigned char dados){ 
  UCB0TXBUF = dados;
}

int piscarLedVerdeContador = 0;
void piscarLedVerde(int intervalo){
  P1DIR |= BIT0;
  if((piscarLedVerdeContador % intervalo) == 1) P1OUT ^= BIT0;
  piscarLedVerdeContador++;
}

int piscaLedVermelhoContador = 0;
void piscarLedVermelho(int intervalo){
  P3DIR |= BIT6;
  if((piscaLedVermelhoContador % intervalo) == 1) P3OUT ^= BIT6;
  piscaLedVermelhoContador++;
}

void slaveInicioI2C(){
  UCB0IFG &= ~UCSTTIFG; 
}

void slaveFimI2C(){
  // Fim do envio de dados do ciclo, não da transmissão.
  UCB0IFG &= ~UCSTPIFG;
}

void ligarLeds(){
  P3OUT &= ~BIT6;
  P1OUT &= ~BIT0;
  P3DIR |= BIT6;
  P1DIR |= BIT0;
}

int counter = 2;
int main(void)
{
  desligarWatchDog();
  ligarLeds();
  configurarI2C();
  tornarSlaveI2C(0x48);
  tornarTxSlaveI2C();
  while(1) habilitarInterrupcoes();              // read out the RxBuffer buffer
}
  
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
  switch(__even_in_range(UCB0IV,12))
  {
  case  0: break;                           // Vector  0: No interrupts
  case  2: break;                           // Vector  2: ALIFG
  case  4: break;                           // Vector  4: NACKIFG
  case  6: slaveInicioI2C(); break;
  case  8: slaveFimI2C(); __bic_SR_register_on_exit(LPM0_bits); break;   // Exit LPM0 if data was trans
  case 10: 
    piscarLedVermelho(300);
    //if((counter % 3000) == 1) tornarTxI2C();
    counter++;
    break; // RXIFG
  
  case 12: 
    // Envia dados
    enviarDadosI2C(2);
    piscarLedVerde(300);
    //if(counter > 300) tornarRxSlaveI2C();
    counter++;
    //tornarRxI2C();
    __bic_SR_register_on_exit(LPM0_bits); 
    break;                           // Vector 12: TXIFG
  default: break; 
  }
}
