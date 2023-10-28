#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "LCD.h"
#include "EEPROM.h"

// Macro para manipulação de bit a bit
#define set_bit(y, bit) (y |= 1 << bit)
#define clr_bit(y, bit) (y &= ~(1 << bit))
#define cpl_bit(y, bit) (y ^= 1 << bit)
#define tst_bit(y, bit) (y &= (1 << bit))

PROGMEM const char msg1[] = "IFCE-Sobral"; // Variavel na memoria flash
char msg2[] = "Numero: ";                  // Variavel armazenada na RAM
char buffer[4] = "000";                    // buffer para converter os digitos em string

int main()
{
  DDRB = 0x05;  // Defino entradas e saidas do PORTD
  PORTB = 0x18; // Habilito PULL-UP
  DDRD = 0xFF;  // Habilito todo o PORTD como saida

  PCICR = 0x01;  // Habilito interrupção no registrador B
  PCMSK0 = 0x18; // Habilita os pinos para gerar interrupção
  sei();         // Habilito a chave de interrupção geral

  inic_LCD_4bits(); // Inicia LCD

  cmd_LCD(0x80, 0);
  escreve_LCD("Teste EEPROM");

  sprintf(buffer, "%.2d", EEPROM_leitura(0x00)); // Converte para string
  cmd_LCD(0xC0, 0);                              // Cursosr linha 2, coluna 10
  escreve_LCD(buffer);                           // Escreve no LCD

  while ((1))
  {
    _delay_ms(10000);
  }
}

ISR(PCINT0_vect)
{
  unsigned char sreg;
  if (!(PINB & (1 << 4)))
  {
    cmd_LCD(0xC0, 0);
    escreve_LCD("escrevendo...");

    sreg = SREG;
    PCICR &= ~(1 << PCINT4);
    sei();
    for (int i = 0; i < 1024; i++)
    {
      EEPROM_escrita(i, 200);
      _delay_ms(5);
    }
    PCICR |= (1 << PCINT4);
    SREG = sreg;
  }
  if (!(PINB & (1 << 3)))
  {
    cmd_LCD(0xC0, 0);
    escreve_LCD("apagando...");

    sreg = SREG;
    PCICR &= ~(1 << PCINT3);
    sei();
    for (int i = 0; i < 1024; i++)
    {
      EEPROM_escrita(i, 0xFF);
      _delay_ms(5);
    }
    PCICR |= (1 << PCINT3);
    SREG = sreg;
  }
  _delay_ms(20);
}