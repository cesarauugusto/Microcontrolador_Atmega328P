// Bibliotecas
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "LCD.h"

// Macro para manipulação de bit a bit
#define set_bit(y, bit) (y |= 1 << bit)
#define clr_bit(y, bit) (y &= ~(1 << bit))
#define cpl_bit(y, bit) (y ^= 1 << bit)
#define tst_bit(y, bit) (y &= (1 << bit))

PROGMEM const char msg1[] = "IFCE-Sobral"; // Variavel na memoria flash
char msg2[] = "Numero: ";                  // Variavel armazenada na RAM
char buffer[4] = "000";                     // buffer para converter os digitos em string

volatile char cont = 0; // Variavel de contagem

int main()
{
  DDRD = 0xFF;  // Habilito todo o PORTD como saida

  inic_LCD_4bits();        // Inicia LCD
  cmd_LCD(0x83, 0);        // Cursor na linha 1, coluna 4
  escreve_LCD_Flash(msg1); // Escreve msg1 no LCD
  cmd_LCD(0xC4, 0);        // Cursor na linha 2, coluna 4
  escreve_LCD(msg2);       // Escreve msg2 no LCD
  _delay_ms(2000);

  while (1)
  {
    _delay_ms(200);
    sprintf(buffer, "%.2d", cont); // Converte para string
    cmd_LCD(0xCB, 0);              // Cursosr linha 2, coluna 10
    escreve_LCD(buffer);           // Escreve no LCD
  }
}
