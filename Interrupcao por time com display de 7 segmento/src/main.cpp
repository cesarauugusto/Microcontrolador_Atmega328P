#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Variaveis globais
volatile char cont;
int unidade = 0;
int dezena = 0;
unsigned char display[] = {0x7D, 0x0C, 0xB5, 0x9D, 0xCC, 0xD9, 0xF9, 0x0D, 0xFF, 0xDF};

// Rotina principal
int main()
{
  // Configuração de I/O
  DDRC = 0b11110011;  // Habilito todo PORTC como entrada
  PORTC = 0b00001100; // Habilito o PULLUP todo no PORTC
  DDRD = 0xFF;        // Habilito todo o PORTD como saida

  PORTC |= 1 << PCINT13; // Ligo o display da unidade
  PORTC |= 1 << PCINT12; //  Ligo o display da dezena
  PORTD = display[0];    // Inicializa o display em 0

  PCICR = (1 << PCIE1);                     // Habilito interrupção no registrador C
  PCMSK1 = (1 << PCINT10) | (1 << PCINT11); // Habilita os pinos para gerar interrupção
  sei();                                    // Habilita as interrupções globais

  UCSR0B = 0x00; // Desabilita RX e TX

  while (1)
  {

    unidade = cont % 10; // Trata o valor para ser usado como unidade
    dezena = cont / 10;  // Trata o valor para ser usado como dezena

    PORTC |= (1 << PC5);      // Liga o display da unidade
    PORTD = display[unidade]; // Mostra o valor da unidade do display
    _delay_ms(5);             // Delay de 5 ms
    PORTC &= ~(1 << PC5);     // Desliga o display da unidade

    PORTC |= (1 << PC4);     // Liga o display da unidade
    PORTD = display[dezena]; // Mostra o valor da dezena do display
    _delay_ms(5);            // Delay de 5 ms
    PORTC &= ~(1 << PC4);    // Desliga o display da unidade
  }
}
// Rotina de serviço de interrupção externa
ISR(PCINT1_vect)
{
  if (!(PINC & (1 << 2))) // Verifica quando o botão é pressionado
  {
    cont++;        // Adiciona 1 quando entra na condição
    if (cont > 16) // Quando maior que 16 contador trava em 16
      cont = 16;
  }
  else if (!(PINC & (1 << 3))) // Verifica quando o botão é pressionado
  {
    cont--;       // Subtrai 1 quando entra na codição
    if (cont < 1) // Quando menor que 1 contador trava em 1
      cont = 1;
  }
  _delay_ms(30); // Delay para evitar o Deboucing
}
