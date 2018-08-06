/*
  Esse código deve funcionar caso o magnetometro apenas 
  utilize um endereço de 7-bits.
  CÓDIGO DO MASTER
  Esse código comanda o magnetometro.
*/

#include <msp430.h>

volatile unsigned char *PRxData;	// Apontador para a lista de bytes recebidos
volatile unsigned char RXByteCtr;	// Numero de bytes que restam para receber

volatile unsigned char *PTxData;	// Apontador da lista de bytes a transmitir
volatile unsigned char TXByteCtr;	// Numero de bytes que restam para transmitir

/* Inicializa a interface UCB0 como i2c com interrupcoes */
void ini_i2c(void)
{
  PMAPPWD = 0x02D52;                        // Get write-access to port mapping regs  
  P1MAP3 = PM_UCB0SDA;                      // Map UCB0SDA output to P1.3 
  P1MAP2 = PM_UCB0SCL;                      // Map UCB0SCL output to P1.2 
  PMAPPWD = 0;                              // Lock port mapping registers 
  P1SEL |= BIT2 + BIT3;                     // Select P1.2 & P1.3 to I2C function 
  
  UCB0CTL1 |= UCSWRST;                      // Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
  UCB0BR0 = 250;                             // fSCL = SMCLK/12 = ~100kHz
  UCB0BR1 = 0;
  UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
  UCB0IE |= UCTXIE + UCRXIE;
}

/* Escreve dados em um dispositivo i2c
addri2c	Endereco i2c do dispositivo
buf	Local onde os dados estao armazenados
n	Numero de bytes que devem ser escritos
Retorna ZERO em caso de sucesso */
int escreve_i2c(int addri2c, unsigned char *buf, int n)
{
  UCB0I2CSA = addri2c;			// Endereco I2C do dispositivo
  PTxData = buf;
  TXByteCtr = n;
  while (UCB0CTL1 & UCTXSTP);		// Espera terminar o STOP
  UCB0CTL1 |= UCTR + UCTXSTT;		// I2C TX,START  UCTR=0x10   UCTXSTT = 0X02
  __bis_SR_register(LPM0_bits + GIE);        // Entra em LPM0 c/ interrupts 
  return TXByteCtr;	// Retorna o numero de bytes restantes (deve ser 0)
}

/* Le dados de um dispositivo i2c
addri2c	Endereco i2c do dispositivo
buf	Local onde deve armazenar os dados lidos
n	Numero de bytes que devem ser lidos
Retorna ZERO se conseguiu ler */
int le_i2c(int addri2c, unsigned char *buf, int n)
{
  UCB0I2CSA = addri2c;
  PRxData = buf;
  RXByteCtr = n;
  while (UCB0CTL1 & UCTXSTP);		// Espera terminar o STOP
  UCB0CTL1 &= ~UCTR;			// apaga UCTR para START TX
  UCB0CTL1 |= UCTXSTT;			// I2C RX start condition
  __bis_SR_register(LPM0_bits + GIE);	// Enter LPM0 w/ interrupts
  return 0;
}


int main(void)
{
  // setup
  P1DIR |= BIT0;
  P3DIR |= BIT6;
  
  P1OUT &= ~BIT0;
  P3OUT &= ~BIT6;
  
  WDTCTL = WDTPW + WDTHOLD;
  ini_i2c();
  
  unsigned char REGS[6];
  int REG_X, REG_Y, REG_Z;
  
  unsigned char COMANDOS_MAGNETOMETRO[] = {
      0x01, 0x04, 0x00
  };
  
  unsigned char PEDIR_MEDICAO[] = { 0x03 };
  
  while(escreve_i2c(0x1E, COMANDOS_MAGNETOMETRO, 4));
  
  // loop
  while(1){
    __delay_cycles(67000);
    while(escreve_i2c(0x1E, PEDIR_MEDICAO, 1));
    while(le_i2c(0x1E, REGS, 6));
    
    REG_X = REGS[0]<<8;
    REG_X |= REGS[1];
    REG_Y = REGS[2]<<8;
    REG_Y |= REGS[3];
    REG_Z = REGS[4]<<8;
    REG_Z |= REGS[5];
    
    if(REG_Z > 100) P3OUT |= BIT6;
    else P3OUT &= ~BIT6;
    
    
  }
}
  
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
  switch(__even_in_range(UCB0IV,12))
  {
  case  0: break;                           // Vector  0: No interrupts
  case  2: break;                           // Vector  2: ALIFG
  case  4: break;
    
  case  6: break; 
    
  case  8: break;  
    
  case 10: // Recebe
    RXByteCtr--;
    if (RXByteCtr) {
            *PRxData++ = UCB0RXBUF;			// Le dado recebido e armazena em *PRxData
            if (RXByteCtr == 1)			// Ultimo byte?
                    UCB0CTL1 |= UCTXSTP;		// Gera I2C stop
            }
    else	{
            *PRxData = UCB0RXBUF;			// Le ultimo dado
            __bic_SR_register_on_exit(LPM0_bits);	// Coloca em modo de repouso LPM0
            }
    
    __bic_SR_register_on_exit(LPM0_bits); 
    break;
   
  case 12: // Envia
    if (TXByteCtr) {		// Ve se tem dados a transmitir
		UCB0TXBUF = *PTxData++;	// Escreve dado no transmissor
		TXByteCtr--;
		}
	else	{
		UCB0CTL1 |= UCTXSTP;			// Gera STOP
		UCB0IFG &= ~UCTXIE;			// Limpa USCI_B0 TX int flag
		__bic_SR_register_on_exit(LPM0_bits);	// Exit LPM0
		}
    break;
  default: break; 
  }
}