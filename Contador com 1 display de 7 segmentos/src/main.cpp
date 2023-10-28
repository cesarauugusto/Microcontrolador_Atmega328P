#include <avr/io.h>
#include <util/delay.h>

/*Rotina principal*/
int main()
{
  /*Variaveis locais*/
  unsigned char display[] = {0x3F,0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,0x7F,0x6F};
  char flag = 0;
  int cont = 0;

  /*Configuração das I/O*/
  DDRD = 0xFF; // Todo o registrador D para saida
  DDRC = 0x00; // Todo o registrador C como entrada
  PORTC = 0xFF; // Ativa o PULL-UP em todo o registrador C
  
  
  DDRB = 0xFF;     // Todo o registrador B para saida
  PORTB |= 1 << 0; // Nivel logico alto no PB0

  PORTD = 0x3F; // Inicia o display em 0

  /*Laço infinito*/
  while (1)
  {
    // Verifica se o botão foi pressionado
    if (!(PINC & (1 << 3)))
    {
      _delay_ms(10); // Debounce
      flag = 1;      // Flag recebe 1
    }
    // Verifica se o botão foi solto
    if (PINC & (1 << 3) && flag == 1)
    {
      _delay_ms(10); // Debounce
      cont++;        // incremta o cont
      if (cont > 9)  // Verifica o cont
      {
        cont = 0;
      }
      PORTD = display[cont];
      flag = 0; // flag recebe 0
    }
  }
}