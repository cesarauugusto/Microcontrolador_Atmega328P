// Bibliotecas
#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "LCD.h"
#include "EEPROM.h"
#include "KEYPAD.h"

// Macro para manipulação de bit a bit
#define set_bit(y, bit) (y |= 1 << bit)
#define clr_bit(y, bit) (y &= ~(1 << bit))
#define cpl_bit(y, bit) (y ^= 1 << bit)
#define tst_bit(y, bit) (y &= (1 << bit))

// Varaiveis Globais
// Variaveis de uso do Display LCD
PROGMEM const char msg1[] = "Ligar CI autom.?"; // Variavel na memoria flash
char msg2[] = "(B1)Sim (B2)Nao";                // Variavel armazenada na RAM
char msg3[] = "Inicializando CI... ";           // Variavel armazenada na RAM
// Variaveis de uso do Display de 7 segmentos
volatile char cont;
int unidade = 0;
int dezena = 0;
unsigned char display[] = {0x7D, 0x0C, 0xB5, 0x9D, 0xCC, 0xD9, 0xF9, 0x0D, 0xFF, 0xDF};

char escolhe_programa = 0;
volatile char program1 = 0;
volatile char program2 = 0;

unsigned char tecla;
char menu = 1;
char admin = 0;
char user = 0;
char gravar_senha = 0;
char teste_senha = 0;
char teste_senha_adm = 0;
char senha[3];
char trocar_senha = 0;

int main()
{
  DDRC = 0b11110011;  // Habilito todo PORTC como entrada
  PORTC = 0b00001100; // Habilito o PULLUP todo no PORTC

  DDRB = 0xFF;
  inic_LCD_4bits();
  cmd_LCD(0x82, 0);
  escreve_LCD("Inicializar");
  cmd_LCD(0xC0, 0);
  escreve_LCD("(1)Cont (2)Senha");

  while (escolhe_programa == 0)
  {

    if (!(PINC & (1 << 2))) // Verifica quando o botão é pressionado
    {
      program1 = 1;
      escolhe_programa = 1;
    }
    while (!(PINC & (1 << 2)))
      ;

    if (!(PINC & (1 << 3))) // Verifica quando o botão é pressionado
    {
      program2 = 1;
      cmd_LCD(0x01, 0);
      escolhe_programa = 1;
    }
    while (!(PINC & (1 << 3)))
      ;
  }

  if (program2 == 1)
  {
    cmd_LCD(0x81, 0);
    escreve_LCD("Programa cont");
    cmd_LCD(0xC0, 0);
    escreve_LCD("Inicializado...");

    DDRC = 0b11110011;     // Habilito todo PORTC como entrada
    PORTC = 0b00001100;    // Habilito o PULLUP todo no PORTC
    DDRD = 0xFF;           // Habilito todo o PORTD como saida
    PORTC |= 1 << PCINT13; // Ligo o display da unidade
    PORTC |= 1 << PCINT12; //  Ligo o display da dezena
    PORTD = display[0];    // Inicializa o display em 0

    PCICR = (1 << PCIE1);                     // Habilito interrupção no registrador C
    PCMSK1 = (1 << PCINT10) | (1 << PCINT11); // Habilita os pinos para gerar interrupção
    sei();

    UCSR0B = 0x00; // Desabilita RX e TX
  }
  if (program1 == 1)
  {
    DDRD = 0x0F;
    PORTD = 0xFF;
    UCSR0B = 0x00;
  }
  while (1)
  {
    while (program1 == 1)
    {
      char i = 0;
      while (menu == 1)
      {
        cmd_LCD(0x81, 0);
        escreve_LCD("Menu Principal");
        cmd_LCD(0xC0, 0);
        escreve_LCD("(1)Admin (2)User");
        tecla = ler_teclado();

        if (tecla != 0xFF)
        {
          if (tecla == '1')
          {
            cmd_LCD(0x01, 0);
            admin = 1;
            menu = 0;
          }
          if (tecla == '2')
          {
            cmd_LCD(0x01, 0);
            user = 1;
            menu = 0;
          }
        }
      }

      while (admin == 1)
      {
        cmd_LCD(0x80, 0);
        escreve_LCD("(1)Acionar rele");
        cmd_LCD(0xC0, 0);
        escreve_LCD("(2)Trocar senha");
        tecla = ler_teclado();
        if (tecla != 0xFF)
        {
          if (tecla == '1')
          {
            cmd_LCD(0x01, 0);
            teste_senha = 1;
            i = 0;
            admin = 0;
          }
          if (tecla == '2')
          {
            cmd_LCD(0x01, 0);
            // gravar_senha = 1;
            teste_senha_adm = 1;
            admin = 0;
            i = 0;
          }
        }
      }

      while (user == 1)
      {
        cmd_LCD(0x80, 0);
        escreve_LCD("(1)Acionar rele");
        cmd_LCD(0xC0, 0);
        escreve_LCD("(2)Ir ao Menu");
        tecla = ler_teclado();

        if (tecla != 0xFF)
        {
          if (tecla == '1')
          {
            cmd_LCD(0x01, 0);
            teste_senha = 1;
            user = 0;
          }
          if (tecla == '2')
          {
            cmd_LCD(0x01, 0);
            menu = 1;
            user = 0;
          }
        }
      }

      while (teste_senha == 1)
      {
        cmd_LCD(0x80, 0);
        escreve_LCD("Digite a senha:");
        tecla = ler_teclado();

        if (tecla != 0xFF)
        {
          if (i == 3)
          {
            cmd_LCD(0xC3, 0);
            escreve_LCD("\0");
          }
          else
          {
            cmd_LCD(0xC0 + i, 0);
            cmd_LCD(tecla, 1);
            senha[i] = tecla;
            i++;
          }

          if ((tecla == '#') && (i >= 3))
          {
            teste_senha = 0;
            i = 0;
            if (EEPROM_leitura(0x00) == senha[0] && EEPROM_leitura(0x01) == senha[1] && EEPROM_leitura(0x02) == senha[2] && trocar_senha == 0)
            {
              cmd_LCD(0x01, 0);
              cmd_LCD(0x80, 0);
              escreve_LCD("Rele Acionado!");
              PORTC = 0x02;
              _delay_ms(2000);
              PORTC = 0x00;
              cmd_LCD(0x01, 0);
              menu = 1;
            }
            else
            {
              cmd_LCD(0x01, 0);
              cmd_LCD(0x80, 0);
              escreve_LCD("Senha errada!");
              _delay_ms(2000);
              cmd_LCD(0x01, 0);
              menu = 1;
            }
          }
        }
      }
      while (gravar_senha == 1)
      {
        cmd_LCD(0x80, 0);
        escreve_LCD("Nova Senha:");
        tecla = ler_teclado();

        if (tecla != 0xFF)
        {
          if (i == 3)
          {
            cmd_LCD(0xC3, 0);
            escreve_LCD("\0");
          }
          else
          {
            cmd_LCD(0xC0 + i, 0);
            cmd_LCD(tecla, 1);
            EEPROM_escrita(i, tecla);
            i++;
          }

          if ((tecla == '#') && (i >= 3))
          {
            cmd_LCD(0x01, 0);
            cmd_LCD(0x80, 0);
            escreve_LCD("Senha alterada");
            _delay_ms(2000);
            cmd_LCD(0x01, 0);
            menu = 1;
            gravar_senha = 0;
          }
        }
      }
      while (teste_senha_adm == 1)
      {
        cmd_LCD(0x80, 0);
        escreve_LCD("Digite a senha:");
        tecla = ler_teclado();

        if (tecla != 0xFF)
        {
          if (i == 3)
          {
            cmd_LCD(0xC3, 0);
            escreve_LCD("\0");
          }
          else
          {
            cmd_LCD(0xC0 + i, 0);
            cmd_LCD(tecla, 1);
            senha[i] = tecla;
            i++;
          }

          if ((tecla == '#') && (i >= 3))
          {
            teste_senha = 0;
            i = 0;
            if (EEPROM_leitura(0x00) == senha[0] && EEPROM_leitura(0x01) == senha[1] && EEPROM_leitura(0x02) == senha[2] && trocar_senha == 0)
            {
              cmd_LCD(0x01, 0);
              cmd_LCD(0x80, 0);
              escreve_LCD("Senha correta");
              _delay_ms(2000);
              cmd_LCD(0x01, 0);
              gravar_senha = 1;
              teste_senha_adm = 0;
            }
            else
            {
              cmd_LCD(0x01, 0);
              cmd_LCD(0x80, 0);
              escreve_LCD("Senha errada!");
              _delay_ms(2000);
              cmd_LCD(0x01, 0);
              menu = 1;
            }
          }
        }
      }
    }
    while (program2 == 1)
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
}

ISR(PCINT1_vect)
{
  if (!(PINC & (1 << 2))) // Verifica quando o botão é pressionado
  {
    cont--;       // Subtrai 1 quando entra na codição
    if (cont < 1) // Quando menor que 1 contador trava em 1
      cont = 1;
  }
  else if (!(PINC & (1 << 3))) // Verifica quando o botão é pressionado
  {
    cont++;        // Adiciona 1 quando entra na condição
    if (cont > 16) // Quando maior que 16 contador trava em 16
      cont = 16;
  }
  _delay_ms(30); // Delay para evitar o Deboucing
}
