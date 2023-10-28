// Bibliotecas
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

// Varaiveis Globais
PROGMEM const char msg1[]  = "Ligar CI autom.?"; // Variavel na memoria flash
char msg4[] = "(B1)Sim (B2)Nao";                // Variavel armazenada na RAM
char buffer[4] = "000";                         // buffer para converter os digitos em string

volatile float cont = 0;   // Variavel de controle
volatile char control = 0; // Variavel de controle
unsigned char disp;

int main()
{

  // Configuração de I/O
  DDRC = 0b11110011;  // Habilito todo PORTC como entrada
  PORTC = 0b00001100; // Habilito o PULLUP todo no PORTC
  DDRD = 0xFF;        // Habilito todo o PORTD como saida
  UCSR0B = 0x00;      // Desabilita RX e TX

  PCICR = (1 << PCIE1);                     // Habilito interrupção no registrador C
  PCMSK1 = (1 << PCINT10) | (1 << PCINT11); // Habilita os pinos para gerar interrupção

  TCCR0A = 0b01100011;
  TCCR0B = 0b00001011;

  OCR0A = 125;
  OCR0B = 0;

  // Setup para LCD
  DDRB = 0xFF;      // Todo o PORTB como saida
  inic_LCD_4bits(); // Iinicia o LCD

  sei();                       // Habilita as interrupções gerais
  cmd_LCD(0x82, 0);            // Cursor Linha 1 e Coluna 2
  escreve_LCD("Motor Parado"); // Escreve no LCD // Cursor Linha 0 e Coluna 1

  OCR0B = EEPROM_leitura(0x00);
  cont = EEPROM_leitura(0x00);
  disp = (cont / 125) * 100;
  while (control == 0)
  {
    sprintf(buffer, "F=2kHz Duty:%3d%",disp); // Converte para string
    cmd_LCD(0xC0, 0);
    escreve_LCD(buffer); // Escreve no LCD 
    _delay_ms(200);
  }

  cmd_LCD(0x01, 0);

  while (1)
  {
    disp = (cont / 125) * 100;
    cmd_LCD(0x82, 0);                   // Cursor Linha 1 e Coluna 2
    escreve_LCD("Motor Ligado");        // Escreve no LCD
    sprintf(buffer, "F=2kHz Duty:%3d",disp); // Converte para string
    cmd_LCD(0xC0, 0);
    escreve_LCD(buffer); // Escreve no LCD
    _delay_ms(200);
  }
}

ISR(PCINT1_vect)
{
  if (!(PINC & (1 << 3))) // Verifica quando o botão é pressionado
  {
    control = 1;

    if (cont > 120)
    {
      cont = 125;
    }
    else
    {
      cont += 5;
    }
    OCR0B = cont;
    EEPROM_escrita(0x00, cont);
  }

  if (!(PINC & (1 << 2))) // Verifica quando o botão é pressionado
  {

    if (cont <= 0)
    {
      cont = 0;
    }
    else
    {
      cont -= 5;
    }
    OCR0B = cont;
    EEPROM_escrita(0x00, cont);
  }
  _delay_ms(30);
}
