
#define LigaLEDVerde P1OUT |= BIT0
#define LigaLEDVermelho P3OUT |= BIT6
#define DesligaLEDVerde P1OUT &= ~BIT0
#define DesligaLEDVermelho P3OUT &= ~BIT6

int ESTADO = 0;
int ESTADO_ANTERIOR = 5;

void desligarWatchDog(){
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
}

void configurarI2C(){
  P1OUT &= ~BIT0;
  P1DIR |= BIT0;
  PMAPPWD = 0x02D52;                        // Get write-access to port mapping regs  
  P1MAP3 = PM_UCB0SDA;                      // Map UCB0SDA output to P1.3 
  P1MAP2 = PM_UCB0SCL;                      // Map UCB0SCL output to P1.2 
  PMAPPWD = 0;                              // Lock port mapping registers 
  
  P1SEL |= BIT2 + BIT3;                     // Select P1.2 & P1.3 to I2C function
}

int MASTER_I2C = 0;
void tornarMasterI2C(int endereco){
  UCB0CTL1 |= UCSWRST;                      // Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
  UCB0BR0 = 12;                             // fSCL = SMCLK/12 = ~100kHz
  UCB0BR1 = 0;
  UCB0I2CSA = endereco;                         // Slave Address is 048h
  UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation
  MASTER_I2C = 1;
}

void tornarSlaveI2C(int endereco){
  UCB0CTL1 |= UCSWRST;                      // Enable SW reset
  UCB0CTL0 = UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
  UCB0I2COA = endereco;                         // Slave Address is 048h}
  MASTER_I2C = 0;
  UCB0CTL1 &= ~UCSWRST;
}


void tornarRxMasterI2C(){
  UCB0CTL1 &= ~UCTR + ~UCTXSTT; 
  UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
  UCB0IE |= UCRXIE;
}

void tornarTxMasterI2C(){
  UCB0CTL1 |= UCTR + UCTXSTT;
  UCB0IE &= ~UCRXIE;
  UCB0IE |= UCTXIE + UCSTPIE + UCSTTIE;
}

void tornarRxSlaveI2C(){
  UCB0CTL1 &= ~UCTR + ~UCTXSTT; 
  UCB0IE &= ~UCTXIE + ~UCSTPIE + ~UCSTTIE;
  UCB0IE |= UCRXIE;
}

void tornarTxSlaveI2C(){
  UCB0CTL1 |= UCTR + UCTXSTT;
  UCB0IE &= ~UCRXIE;
  UCB0IE |= UCTXIE + UCSTPIE + UCSTTIE;
}

void comecarI2C(){
  __delay_cycles(50);
  while(UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
  UCB0CTL1 |= UCTXSTT;                    // I2C start condition
  if((ESTADO == 0) && (MASTER_I2C == 1)){  
    while(UCB0CTL1 & UCTXSTT);              // Start condition sent?
    UCB0CTL1 |= UCTXSTP;
    // I2C stop condition
  }
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
  //P1DIR |= BIT0;
  if((piscarLedVerdeContador % intervalo) == 1) P1OUT ^= BIT0;
  piscarLedVerdeContador++;
}

int piscaLedVermelhoContador = 0;
void piscarLedVermelho(int intervalo){
  //P3DIR |= BIT6;
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


void habilitarLeds(){
  P3DIR |= BIT6;
  P1DIR |= BIT0;
}

void limparLeds(){
  P3OUT &= ~BIT6;
  P1OUT &= ~BIT0;
}

// ESTADO 0 -> Opera em RX
// ESTADO 1 -> Opera em TX
void verificarEstado(){
  if(ESTADO_ANTERIOR != ESTADO){
    if(MASTER_I2C && ESTADO) tornarTxMasterI2C();
    if(MASTER_I2C && !ESTADO) tornarRxMasterI2C();
    if( !MASTER_I2C && ESTADO) tornarTxSlaveI2C();
    if( !MASTER_I2C && !ESTADO) tornarRxSlaveI2C();
    ESTADO_ANTERIOR = ESTADO;
  }
}