// Bibliotecas
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
PROGMEM const char msg1[] = "Ligar CI autom.?"; // Variavel na memoria flash
char msg2[] = "(B1)Sim (B2)Nao";                // Variavel armazenada na RAM
char msg3[] = "Inicializando CI... ";           // Variavel armazenada na RAM

// Rotina principal
int main()
{
    // Saida para o led
    DDRC = 0x02; // PC02 Como saida para acionar o led

    // Setup para teclado matricial
    DDRD = 0x0F;   // PC0-PC03 como saida para colunas e PC4-PC7 como entradas para linhas do telcado
    PORTD = 0xFF;  // Habilbita PULL-UP para as linhas e escreve alto nas colunas
    UCSR0B = 0x00; // Desabilita RX e TX

    // Setup para LCD
    DDRB = 0xFF;      // Todo o PORTB como saida
    inic_LCD_4bits(); // Iinicia o LCD

    // Variaeis locais
    // Variaveis auxiliares
    unsigned char tecla;
    char menu = 1;
    char admin = 0;
    char user = 0;
    char gravar_senha = 0;
    char teste_senha = 0;
    char teste_senha_adm = 0;
    char senha[3];
    char trocar_senha = 0;

    // Loop infitinto
    while (1)
    {
        char i = 0;       // Zera a variavel i
        while (menu == 1) // Enquanto menu == 1 permance dentro do laço
        {
            cmd_LCD(0x81, 0);                // Cursor Linha 1 e Coluna 2
            escreve_LCD("Menu Principal");   // Escreve no LCD
            cmd_LCD(0xC0, 0);                // Cursor Linha 0 e Coluna 1
            escreve_LCD("(1)Admin (2)User"); // Escreve no LCD
            tecla = ler_teclado();           // Variavel tecla recebe o caracter que foi apertado no telcado

            if (tecla != 0xFF) // Entra no if quando tecla for diferente de FF
            {
                if (tecla == '1') // Se caracter 1 for apertado entra na condição
                {
                    cmd_LCD(0x01, 0); // Limpa o LCD
                    admin = 1;        // Variavel admin recebe 1
                    menu = 0;         // Variavel menu recebe 0
                }
                if (tecla == '2') // se caracter 2 for apertado entra na condição
                {
                    cmd_LCD(0x01, 0); // Limpa o LCD
                    user = 1;         // Váriavel user recebe 1
                    menu = 0;         // Variavel menu recebe 0
                }
            }
        }

        while (admin == 1) // Enquanto admin for igual a 1 ele fica no laço
        {
            cmd_LCD(0x80, 0);               // Cursor Linha 1 e Coluna 1
            escreve_LCD("(1)Acionar rele"); // Escreve no LCD
            cmd_LCD(0xC0, 0);               // Cursor Linha 1 e Coluna 1
            escreve_LCD("(2)Trocar senha"); // Escreve no LCD

            tecla = ler_teclado(); // Variavel tecla recebe o caracter que foi apertado no telcado
            if (tecla != 0xFF)     // Entra no if quando tecla for diferente de FF
            {
                if (tecla == '1') // Se caracter 1 for apertado entra na condição
                {
                    cmd_LCD(0x01, 0); // Limpa o LCD
                    teste_senha = 1;  // Variavel teste_senha recebe 1
                    i = 0;            // Vairiavel i recebe 0
                    admin = 0;        // Variavel admin recebe 0
                }
                if (tecla == '2') // se caracter 2 for apertado entra na condição
                {
                    cmd_LCD(0x01, 0); // Limpa o LCD
                    // gravar_senha = 1; //Para gravar uma senha
                    teste_senha_adm = 1; // variavel teste_senha_adm limpa o LCD
                    admin = 0;           // Variavel admin recebe 0
                    i = 0;               // Vairiavel i recebe 0
                }
            }
        }

        while (user == 1) // Enquanto admin for igual a 1 ele fica no laço
        {
            cmd_LCD(0x80, 0);               // Cursor Linha 1 e Coluna 1
            escreve_LCD("(1)Acionar rele"); // Escreve no LCD
            cmd_LCD(0xC0, 0);               // Cursor Linha 1 e Coluna 1
            escreve_LCD("(2)Ir ao Menu");   // Escreve no LCD
            tecla = ler_teclado();          // Variavel tecla recebe o caracter que foi apertado no telcado

            if (tecla != 0xFF) // Entra no if quando tecla for diferente de FF
            {
                if (tecla == '1') // Se caracter 1 for apertado entra na condição
                {
                    cmd_LCD(0x01, 0); // Limpa LCD
                    teste_senha = 1;  // Variavel teste_senha recebe 1
                    user = 0;         // Variavel user recebe 0
                }
                if (tecla == '2') // Se caracter 2 for apertado entra na condição
                {
                    cmd_LCD(0x01, 0); // Limpa o LCD
                    menu = 1;         // Variavel menu recebe 1
                    user = 0;         // Variavel user recebe 0
                }
            }
        }

        while (teste_senha == 1) // Enquanto admin for igual a 1 ele fica no laço
        {
            cmd_LCD(0x80, 0);               // Cursor Linha 1 e Coluna 1
            escreve_LCD("Digite a senha:"); // Escreve no LCD
            tecla = ler_teclado();          // Variavel tecla recebe o caracter que foi apertado no telcado

            if (tecla != 0xFF) // Entra no if quando tecla for diferente de FF
            {
                if (i == 3) // Quando o i for igual a 3 entra na condição
                {
                    cmd_LCD(0xC3, 0);  // Cursor Linha 1 e Coluna 4
                    escreve_LCD("\0"); // Escreve no LCD nulo
                }
                else // Se o i não for igual a 3
                {
                    cmd_LCD(0xC0 + i, 0); // Cursor Linha 1 e Coluna (1+i)
                    cmd_LCD(tecla, 1);    // Escreve no LCD o caracter da tecla
                    senha[i] = tecla;     // O vetor senha [i] recebe a tecla apertada
                    i++;                  // Incremeta i
                }

                if ((tecla == '#') && (i >= 3)) // Se o i for maior ou igual a 3 e apertar # entra na condição
                {
                    teste_senha = 0; // Variavel Teste senha recebe 0
                    i = 0;           // Variavel i recebe 0
                    if (EEPROM_leitura(0x00) == senha[0] && EEPROM_leitura(0x01) == senha[1] && EEPROM_leitura(0x02) == senha[2] && trocar_senha == 0)
                    {                                  // Se o vetor senha[1],[2],[3] for igal a EEPROM_leitura 1,2,3 entra na condição
                        cmd_LCD(0x01, 0);              // Limpa LCD
                        cmd_LCD(0x80, 0);              // Cursor Linha 1 e Coluna 1
                        escreve_LCD("Rele Acionado!"); // Escreve no LCD
                        PORTC = 0x02;                  // Envia nivel logico alto para o PC2
                        _delay_ms(2000);               // Delay de 2 segundos
                        PORTC = 0x00;                  // Nivel logico baixo para o PC2
                        cmd_LCD(0x01, 0);              // Limpa LCD
                        menu = 1;                      // Variavel menu recebe 1
                    }
                    else // Se a conidição if não for verdadeira entra nessa conidção
                    {
                        cmd_LCD(0x01, 0);             // Limpa LCD
                        cmd_LCD(0x80, 0);             // Cursor Linha 1 e Coluna 1
                        escreve_LCD("Senha errada!"); // Escreve no LCD
                        _delay_ms(2000);              // Delay de 2 segundos
                        cmd_LCD(0x01, 0);             // Limpa LCD
                        menu = 1;                     // Variavel menu recebe 1
                    }
                }
            }
        }
        while (gravar_senha == 1) // Enquanto gravar_senha for igual a 1 ele fica no laço
        {
            cmd_LCD(0x80, 0);           // Cursor Linha 1 e Coluna 1
            escreve_LCD("Nova Senha:"); // Escreve no LCD
            tecla = ler_teclado();      // Variavel tecla recebe o caracter que foi apertado no telcado

            if (tecla != 0xFF) // Entra no if quando tecla for diferente de FF
            {
                if (i == 3) // Se i for igual a 3 entra na condição
                {
                    cmd_LCD(0xC3, 0);    // Cursor Linha 1 e Coluna 4
                    escreve_LCD("null"); // Escreve nulo
                }
                else // Se i não for igual a 3
                {
                    cmd_LCD(0xC0 + i, 0);     // Cursor Linha 1 e Coluna (1+i)
                    cmd_LCD(tecla, 1);        // Escreve no LCD o caracter da tecla
                    EEPROM_escrita(i, tecla); // Escreve na memoria i o caracter apertado
                    i++;                      // Incrementa o i
                }

                if ((tecla == '#') && (i >= 3)) // Se o i for maior ou igual a 3 e apertar # entra na condição
                {
                    cmd_LCD(0x01, 0);              // Limpa o LCD
                    cmd_LCD(0x80, 0);              // Cursor Linha 1 e Coluna 1
                    escreve_LCD("Senha alterada"); // Escreve no LCD
                    _delay_ms(2000);               // Delay de 2 segundos
                    cmd_LCD(0x01, 0);              // Limpa o LCD
                    menu = 1;                      // Variavel menu recebe 1
                    gravar_senha = 0;              // Variavel gravar_senha recebe 0
                }
            }
        }
        while (teste_senha_adm == 1) // Enquanto teste_senha_adm for igual a 1 ele fica no laço
        {
            cmd_LCD(0x80, 0);               // Cursor Linha 1 e Coluna 1
            escreve_LCD("Digite a senha:"); // Escreve no LCD
            tecla = ler_teclado();          // Variavel tecla recebe o caracter que foi apertado no telcado

            if (tecla != 0xFF) // Entra no if quando tecla for diferente de FF
            {
                if (i == 3) // Se i for igual a 3 entra na condição
                {
                    cmd_LCD(0xC3, 0);  // Cursor Linha 1 e Coluna 4
                    escreve_LCD("\0"); // Escreve nulo
                }
                else // se i não for igual a 3
                {
                    cmd_LCD(0xC0 + i, 0); // Cursor Linha 1 e Coluna (1+i)
                    cmd_LCD(tecla, 1);    // Escreve no LCD o caracter da tecla
                    senha[i] = tecla;     // O vetor senha [i] recebe a tecla apertada
                    i++;                  // Incrementa o i
                }

                if ((tecla == '#') && (i >= 3)) // Se o i for maior ou igual a 3 e apertar # entra na condição
                {
                    teste_senha = 0; // Variavel teste_senha reebe 0
                    i = 0;           // Variavel i recebe 0
                    if (EEPROM_leitura(0x00) == senha[0] && EEPROM_leitura(0x01) == senha[1] && EEPROM_leitura(0x02) == senha[2] && trocar_senha == 0)
                    {                                 // Se o vetor senha[1],[2],[3] for igal a EEPROM_leitura 1,2,3 entra na condição
                        cmd_LCD(0x01, 0);             // Limpa o LCD
                        cmd_LCD(0x80, 0);             // Cursor Linha 1 e Coluna 1
                        escreve_LCD("Senha correta"); // Escreve no LCD
                        _delay_ms(2000);              // Delay de 2 segundos
                        cmd_LCD(0x01, 0);             // Limpa o LCD
                        gravar_senha = 1;             // Variavel gravar_senha recebe 1
                        teste_senha_adm = 0;          // Variavel teste_senha_adm recebe 0
                    }
                    else // Se a conidição if não for verdadeira entra nessa conidção
                    {
                        cmd_LCD(0x01, 0);             // Limpa o LCD
                        cmd_LCD(0x80, 0);             // Cursor Linha 1 e Coluna 1
                        escreve_LCD("Senha errada!"); // Escreve no LCD
                        _delay_ms(2000);              // Delay de 2 segundos
                        cmd_LCD(0x01, 0);             // Limpa o LCD
                        menu = 1;                     // Variavel menu recebe 1
                    }
                }
            }
        }
    }
}