
#include <Arduino.h>
// Mapemanto do MCU

#define LIN PIND  // registrador para a leitura das linhas
#define COL PORTD // registrador para a escrita nas colunas

// Prototipo de função
const unsigned char teclado[4][4] PROGMEM =

    // {{'1', '2', '3', 'A'},
    //  {'4', '5', '6', 'B'},
    //  {'7', '8', '9', 'C'},
    //  {'*', '0', '#', 'D'}};

    // Para o SIMULIDE
    // {{'A', 'B', 'C', 'D'},
    //  {'3', '6', '9', '#'},
    //  {'2', '5', '8', '0'},
    //  {'1', '4', '7', '*'}};

    // Para a placa
    {{'B', 'A', 'C', 'D'},
     {'6', '3', '9', '#'},
     {'5', '2', '8', '0'},
     {'4', '1', '7', '*'}};

// Rotina para leitura do teclado
unsigned char ler_teclado()
{
    unsigned char key = 0xFF;
    // varredura para escrever nas colunas
    for (int j = 0; j < 4; j++)
    {
        COL &= ~(1 << j);
        _delay_ms(10);
        // Varredura para ler nas linhas
        for (int i = 0; i < 4; i++)
        {
            if (!((PIND >> 4) & (1 << i)))
            {
                key = pgm_read_byte(&teclado[i][j]);
                _delay_ms(200);
            }
        }
        COL |= (1 << j);
    }
    return key;
}
