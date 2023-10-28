// Bibliotecas
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "LCD.h"
#include "EEPROM.h"

//Macro para manipulação de bit a bit
#define set_bit(y, bit) (y |= 1 << bit)
#define clr_bit(y, bit) (y &= ~(1 << bit))
#define cpl_bit(y, bit) (y ^= 1 << bit)
#define tst_bit(y, bit) (y &= (1 << bit))

//Varaiveis Globais
PROGMEM const char msg1[] = "Ligar CI autom.?"; //Variavel na memoria flash
char msg2[] = "(B1)Sim (B2)Nao";          //Variavel armazenada na RAM
char msg3[] = "Inicializando CI... ";     //Variavel armazenada na RAM
char msg4[] = "Apagar EEPROM?";           //Variavel armazenada na RAM
char msg5[] = "Rotina Principal";         //Variavel armazenada na RAM
char buffer[4] = "000";                   //buffer para converter os digitos em string

volatile char control = 0; //Variavel de controle
volatile char nao = 0;     //Variavel de controle
char b2 = 0;               //Variavel de controle

//Rotina principal
int main() 
{
  DDRB = 0x05;  //Defino entradas e saidas do PORTD
  PORTB = 0x18; //Habilito PULL-UP
  DDRD = 0xFF;  //Habilito todo o PORTD como saida

  inic_LCD_4bits(); //Inicia LCD

  PCICR = 0x01;  //Habilito interrupção no registrador B
  PCMSK0 = 0x18; //Habilita os pinos para gerar interrupção
  sei();         //Habilito a chave de interrupção geral

  if (EEPROM_leitura(0x00) == 0) //Leitura da EEPROM
    PORTB = 0b00011001;          //Liga o LED vermelho

  if (EEPROM_leitura(0x00) == 1) //Leitura da EEPROM
    PORTB = 0b00011100;          //Liga o LED vermelho

  while (EEPROM_leitura(0x00) == 0 && nao == 0) //Leitura da EEPROM
  {
    cmd_LCD(0x80, 0);        // Cursor na linha 1, coluna 4
    escreve_LCD_Flash(msg1); // Escreve msg1 no LCD

    cmd_LCD(0xC0, 0);        // Cursor na linha 2, coluna 1
    escreve_LCD(msg2);       // Escreve msg2 no LCD

    if (control == 1)           //Ler a variavel control
      EEPROM_escrita(0x00, 1); 
      EEPROM_escrita(0x01, 2);
      EEPROM_escrita(0x02, 3); //Escreve na EEPROM

    if (nao == 1)       //Ler a variavel nao
    {
      cmd_LCD(0xC0, 0); // Cursor na linha 2, coluna 4
      limpa_LCD();      //Limpa o LCD
      b2=1;
    }
  }

  if (EEPROM_leitura(0x00) == 0) //Leitura da EEPROM
    PORTB = 0b00011001;          //Liga o LED vermelho

  if (EEPROM_leitura(0x00) == 1) //Leitura da EEPROM
    PORTB = 0b00011100;          //Liga o LED amarelo

  cmd_LCD(0xC0, 0);      //Cursor na linha 2, coluna 1
  limpa_LCD();           //Limpa o LCD

  if (EEPROM_leitura(0x00) == 1)  //Leitura da EEPROM
  {
    cmd_LCD(0x80, 0);  //Cursor na linha 1, coluna 4
    escreve_LCD(msg3); //Escreve msg2 no LCD
    _delay_ms(2000);   //Delay de 2 segundos
  }

  cmd_LCD(0x80, 0);    // Cursor na linha 2, coluna 4
  limpa_LCD();         //Limpa o LCD

  control = 0;  //Zera a variavel control
  nao = 0;      //Zera a variavel nao

  while (1)
  {
    while (control == 0 && nao == 0 && b2==0) //Fica em loop enquanto a variavel control, b2 e nao = 0
    {
      cmd_LCD(0x81, 0);  // Cursor na linha 1, coluna 4
      escreve_LCD(msg4); // Escreve msg2 no LCD
      cmd_LCD(0xC0, 0);  // Cursor na linha 2, coluna 4
      escreve_LCD(msg2); // Escreve msg2 no LCD

      if (control == 1)   //Ler a variavel control
      {
        EEPROM_escrita(0x00, 0); //Escreve na EEPROM
        PORTB = 0b00011001;      //Liga o led amarelo
        cmd_LCD(0x81, 0);        // Cursor na linha 1, coluna 2
        limpa_LCD();             //Limpa o LCD
        cmd_LCD(0xC0, 0);        // Cursor na linha 2, coluna 1
        limpa_LCD();             //Limpa o LCD
      }
      if (nao == 1)              //Ler a variavel nao
      {
        cmd_LCD(0xC0, 0);        // Cursor na linha 2, coluna 1
        limpa_LCD();             //Limpa o LCD
      }
    }

    cmd_LCD(0x80, 0);  // Cursor na linha 1, coluna 1
    escreve_LCD(msg5); // Escreve msg2 no LCD
  }
}

//Rotina de serviço de interrupção externa
ISR(PCINT0_vect)
{
  if (!(PINB & (1 << 4)))      //Ler a entrada PB4
  {
    control = 1;               //Atribui o valor 1 para variavel control
  }
  else if (!(PINB & (1 << 3))) //Ler a entrada PB3
  {
    nao = 1;                   //Atribui o valor 1 para variavel nao
  }
  _delay_ms(20);               //delay de 20ms
}