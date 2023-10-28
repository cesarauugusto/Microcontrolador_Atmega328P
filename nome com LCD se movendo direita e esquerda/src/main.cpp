// Bibliotecas
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "LCD.h"

// Macro para manipulação de bit a bit
#define set_bit(y, bit) (y |= 1 << bit)
#define clr_bit(y, bit) (y &= ~(1 << bit))
#define cpl_bit(y, bit) (y ^= 1 << bit)
#define tst_bit(y, bit) (y &= (1 << bit))

PROGMEM const char msg1[] = "IFCE-Sobral"; // Variavel na memoria flash
char msg3[] = "Teste";                     // Variavel armazenada na RAM

int main()
{
  DDRD = 0xFF; // Habilito todo o PORTD como saida

  inic_LCD_4bits();        // Inicia LCD
  cmd_LCD(0x83, 0);        // Cursor na linha 1, coluna 4
  escreve_LCD_Flash(msg1); // Escreve msg1 no LCD
  //_delay_ms(2000);

  while (1)
  {
    cmd_LCD(0x01, 0); // Limpa o LCD
    escreve_LCD(msg3);
    for (int i = 0; i < 16; i++)
    {
      _delay_ms(400);
      cmd_LCD(0x18, 0);
    }

    cmd_LCD(0x01, 0); // Limpa o LCD
    escreve_LCD(msg3);
    for (int i = 0; i < 16; i--)
    {
      _delay_ms(400);
      cmd_LCD(0x00, 0);
    }
  }
}