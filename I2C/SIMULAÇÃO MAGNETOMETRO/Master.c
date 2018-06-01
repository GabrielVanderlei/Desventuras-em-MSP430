/****************
Interação com o magnetometro
(Script do MASTER)

Naturalmente, o magnetometro HMC5883L se comporta como SLAVE,
para permitir uma melhora análise do funcionamento do código
em conjunto com este script existe o código que deve ser colocado
em outra placa para testar a recepção I2C do SLAVE.

Além disso para facilitar o desenvolvimento, adicionei uma 
biblioteca que condensa as partes do código em funções totalmente 
escritas em português. <msp430_helper.c>

***********************************************************
Sobre o magnetometro
(Dados interpretados da DATASHEET)

O magnetometro se comunica via I2C com o msp através de 3 bytes 
por chamada. O primeiro se refere ao registrador de acesso () que será realizada, 
o segundo se refere ao registrador escolhido e o terceiro define 
as propriedades desse registrador.

E ainda existe o ponteiro de endereçamento, que faz a transição
 entre os diversos registradores, porém não devemos nos preocupar 
com ele pois o registrador de acesso cuidará de atualizar esse 
ponteiro automaticamente.

Exemplo de mensagem
0x3C 0x0A
0x3C - Byte de envio / É utilizado para indicar que o master está enviando dados.
0x0A - Seleciona o registrador 10.
Essa mensagem apenas direciona o ponteiro de endereço para o registrador 10.
No caso é apenas um exemplo, esse registrador não existe.

*********
Registradores disponíveis
Você só pode interagir com os registradores
* CRA, CRB e MR equivalem aos BITs de cada registrador
* Tabelas contento todas as possibilidades se encontram na DATASHEET do magnetometro

*******************************
0x00 (Registrador de configuração A)
CRA7 - Deve estar como 0
CR5, CR6 - Define a quantidade de envios do valor medido para o master.
CR4, CR3, CR2 - Define a taxa de transmissão de bits.
CRA1, CRA0 - Define o fluxo de medição.

Ex:
0x3C 0x00 0x70
0x3C - Escrita
0x00 - Registrador A
0x70 - 01110000

--
0 - Padrão
11 - Define 8 saídas para o master
100 - Define 15Hz de saída de dados
00 - Modo de medição normal.

*******************************
0x01 (Registrador de configuração B)
CRB7, CRB6, CRB5 - Definem o ganho.
CRB4, CRB3, CRB2, CRB1, CRB0 - Devem ser 0.

Ex:
0x3C 0x01 0xA0
0xA0 - 10100000
--
101 - Aproximadamente 5 de ganho.
00000 - Padrão

*******************************
0x02 (Registrador de configuração do modo de operação)
MR7, MR6, MR5, MR4, MR3, MR2 - Devem ser 0.
MR1, MR0 - Define o modo de operação.

Ex.
0x3C 0x02 0x00
0x00
--
000000 - Padrão
00 - Modo de medição contínua.

****************************
Com isso você vai ter condições de configurar o magnetometro.
Porém não irá conseguir nenhuma medida apenas aguardando o valor tx 
Os valores de medições ficam armazenados em registradores específicos
 que guardam os valores X, Y e Z dos valores medidos.

Para acessa-los você pode utilizar o código 
0x3D para iniciar a leitura e
0x03 e 0x04 - Dados do eixo X
0x05 e 0x06 - Dados do eixo Y
0x07 e 0x08 - Dados do eixo Z
(MSB) e (LSB)

MSB - BIT mais significativo
LSB - BIT menos significativo

Lembrando que o magnetometro precisa de 6ms para configurar
totalmente e de 67ms para poder enviar os dados que estão 
armazenados no buffer de saída.
****************/

#include <msp430.h>

// Inclui a biblioteca de apoio
#include <msp430_helper.c>

int config = 0;
int libera = 0;
int counter = 0;
char XReg = 0;
char YReg = 0;
char ZReg = 0;
char XRegAnterior = 0;
char YRegAnterior = 0;
char ZRegAnterior = 0;
char dadoAnterior = 0;
int i;

int main(void)
{
  desligarWatchDog(); // Desliga WD
  ligarLeds(); // Direciona os LEDS para saída
  limparLeds(); // Limpa os resultados anteriores do LED
  configurarI2C(); // Configura o I2C para as portas específicas
  tornarMasterI2C(0x48); // Torna a placa SLAVE de endereço 0x48
  ESTADO = 1; // Transforma a placa em receptor.
  
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
      piscaLedVermelho();
      switch(counter){
      case 3: XReg = dadosRecebidosI2C(); ESTADO = 1; break;
      case 25: YReg = dadosRecebidosI2C(); ESTADO = 1; break;
      case 35: ZReg = dadosRecebidosI2C(); ESTADO = 1; break;
      default: piscarLedVermelho(300);
      }
      dadoAnterior = dadosRecebidosI2C();
    }
    __bic_SR_register_on_exit(LPM0_bits); 
    break;
  
  case 12: 

    __delay_cycles(500000); // Tempo de envio entre um dado e outro.
    if(ESTADO == 1){ // Está enviando
            config++;
            switch(config){
            // 8 exemplos, 15 Hz, normal
            case 1: enviarDadosI2C(0x3C); break;
            case 2: enviarDadosI2C(0x00); break;
            case 3: enviarDadosI2C(0x70); break;
            
            // Ganho = 5
            case 4: enviarDadosI2C(0x3C); break;
            case 5: enviarDadosI2C(0x01); break;
            case 6: enviarDadosI2C(0xA0); break;
            
            // Medição contínua
            case 4: enviarDadosI2C(0x3C); break;
            case 5: enviarDadosI2C(0x02); break;
            case 6: enviarDadosI2C(0x00); break;
            
            // Período de espera
            default: enviarDadosI2C(0);
            }
            
            // Espera 6ms
            if(config > 1000){
              // Gera o loop
              // Espera 67 ms
              if((config % 10000) == 1){
                if(XReg != XRegAnterior) piscaLedVerde(300);
                libera = 1;
              }
              
              // Caso esteja liberado
              if(libera == 1){
                counter = ((config % 10000) - 2);
                switch(counter){
                // Ler eixo X
                case 1: enviarDadosI2C(0x3D); break;
                case 2: enviarDadosI2C(0x03); ESTADO = 0; break;
                
                // Ler eixo Y
                case 10: enviarDadosI2C(0x3D); break;
                case 20: enviarDadosI2C(0x05); ESTADO = 0; break;
                
                // Ler eixo Z
                case 20: enviarDadosI2C(0x3D); break;
                case 30: enviarDadosI2C(0x07); ESTADO = 0; break;
                }
              }
              
              // Armazena o valor anterior
              if((config % 30000) == 1){
                XRegAnterior = XReg;
                YRegAnterior = YReg;
                ZRegAnterior = ZReg;
              }
            }
    }
      
      __bic_SR_register_on_exit(LPM0_bits); 

    break;
  default: break; 
  }
}
