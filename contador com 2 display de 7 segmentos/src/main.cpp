#include <avr/io.h>
#include <util/delay.h>

/*Rotina principal*/
int main()
{
  /*Variaveis locais*/
  unsigned char display[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
  char flagbtn1 = 0;
  int cont = 0;
  int unidade = 0, dezena = 0;

  /*Configuração das I/O*/
  DDRD = 0xFF;  // Todo o registrador D para saida
  DDRC = 0b11110000;  // registrador C como entrada
  PORTC =0b00001100; // Ativa o PULL-UP 

  PORTC |= 1 << 4; // Nivel logico alto no PC5
  PORTC |= 1 << 5; // Nivel logico alto no PC6

  UCSR0B = 0x00;
  PORTD = display[cont]; // Inicia o display em 0

  /*Laço infinito*/
  while (1)
  {
    // Verifica se o botão foi pressionado
    if (!(PINC & (1 << 3)))
    {
      _delay_ms(10); // Debounce
      flagbtn1 = 1;  // Flag recebe 1
    }
    // Verifica se o botão foi solto
    if (PINC & (1 << 5) && flagbtn1 == 1)
    {
      _delay_ms(10); // Debounce
      cont++;        // incremta o cont
      if (cont > 15) // Verifica o cont
      {
        cont = 0;
      }

      unidade = cont % 10;
      dezena = cont / 10;
      flagbtn1 = 0; // flag recebe 0
    }
    PORTC |= (1 << 5);
    PORTD = display[dezena];
    _delay_ms(5);
    PORTC &= ~(1 << 5);

    PORTC |= (1 << 4);
    PORTD = display[unidade];
    _delay_ms(5);
    PORTC &= ~(1 << 4);
  }
}