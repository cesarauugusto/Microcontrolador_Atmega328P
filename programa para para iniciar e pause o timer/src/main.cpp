#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Variaveis Globais
char cont, cont2, flag = 0, flagbtn = 0;
volatile char disp;
int unidade = 0;
int dezena = 0;
unsigned char display[] = {0x7D, 0x0C, 0xB5, 0x9D, 0xCC, 0xD9, 0xF9, 0x0D, 0xFF, 0xDF};

int main()
{

  // Configuração de I/O
  DDRC = 0b11110011;  // Habilito todo PORTC como entrada
  PORTC = 0b00001100; // Habilito o PULLUP todo no PORTC
  DDRD = 0xFF;        // Habilito todo o PORTD como saida
  DDRB = 0xFF;
  UCSR0B = 0x00; // Desabilita RX e TX

  PORTC |= (1 << PC5); // Ligo o display da unidade
  PORTC |= (1 << PC4); //  Ligo o display da dezena
  PORTD = display[0];  // Inicializa o display em 0

  PCICR = (1 << PCIE1);                     // Habilito interrupção no registrador C
  PCMSK1 = (1 << PCINT10) | (1 << PCINT11); // Habilita os pinos para gerar interrupção

  TCCR0A = 0x00;                      // Modo de operação normal
  TCCR0B = (1 << CS02) | (1 << CS00); // Prescaler = 1024
  TIMSK0 |= (1 << TOIE0);             // Habilita a interrupção do timer 0
  TCNT0 = 193;                        // Valor de TNCT0 começa em 177.875

  TCCR1A = 0x00;
  TIMSK1 |= (1 << TOIE1);
  TCNT1 = 49911;

  sei(); // Habilita as interrupções gerais

  while (1)
    ;
}

ISR(PCINT1_vect)
{
  if (!(PINC & (1 << 2))) // Verifica quando o botão é pressionado
  {
    flagbtn++;
    if (flagbtn == 1)
    {
      TCCR1B = (1 << CS12) | (1 << CS10);
    }
    else if (flagbtn == 2) // Verifica quando o botão é pressionado
    {
      TCCR1B = 0x00;
      flagbtn = 0;
    }
  }

  if (!(PINC & (1 << 3))) // Verifica quando o botão é pressionado
  {
    TCCR1B = 0x00;
    disp = 0;
    unidade = 0;
    dezena = 0;
  }
  _delay_ms(60); // Delay para evitar o Deboucing
}

// Serviço de interrupção
ISR(TIMER0_OVF_vect)
{
  switch (flag)
  {
  case 0:
    PORTC &= ~(1 << PC4);     // Desliga o display da unidade
    PORTC &= ~(1 << PC5);     // Desliga o display da unidade
    PORTC |= (1 << PC5);      // Liga o display da unidade
    PORTD = display[unidade]; // Mostra o valor da unidade do display
    flag = 1;
    break;

  case 1:
    PORTC &= ~(1 << PC5);    // Desliga o display da unidade
    PORTC &= ~(1 << PC4);    // Desliga o display da unidade
    PORTD = display[dezena]; // Mostra o valor da dezena do display
    PORTC |= (1 << PC4);     // Liga o display da unidade
    flag = 0;
    break;
  }

  TCNT0 = 193; // TCT0 recebe o valor 177.875 novamente
}

ISR(TIMER1_OVF_vect)
{
  cont2++;
  if (cont2 == 1) // Se cont for igual a 200
  {
    disp++;
    unidade = disp % 10; // Trata o valor para ser usado como unidade
    dezena = disp / 10;  // Trata o valor para ser usado como dezena
    cont2 = 0;           // Variavel cont zera

    if (disp == 60)
      disp = 0;
  }
  TCNT1 = 49911; // TCT0 recebe o valor 177.875 novamente
}

// CONTAS PARA CONTROLAR O TIMER 1

// TCNT1 = 65536 -(Tempo*Fosc/Prescaler)
// 49911 = 65536 -(1*16000000/64)

// Tempomax = (65536)*(1/Fosc)*1024
// Tempomax = 4.19430

// CONTAS PARA CONTROLAR O TIMER 0

// TCNT0 = 256 -(Tempo*Fosc/Prescaler)
// 6 = 256 -(0.01*16000000/1024)

// Tempomax = (256)*(1/Fosc)*1024
// Tempomax = 0.001024
