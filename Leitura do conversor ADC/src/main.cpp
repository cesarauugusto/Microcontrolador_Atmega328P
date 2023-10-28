// Bibliotecas
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "LCD.h"

// Varaiveis Globais
char buffer[6] = "00000"; // buffer para converter os digitos em string
volatile unsigned int leADC_i, resto, valor, cont0;
unsigned int tempo_borda, delta, frequencia;
float leADC_f, periodo;

int main()
{

  // Setup para LCD
  DDRD = 0xFF;      // Todo o PORTB como saida
  UCSR0B = 0x00;    // Desabilita RX e TX
  inic_LCD_4bits(); // Inicia o LCD

  DDRB &= ~(1 << 0);                    // PB0 como entrada para captura de sinal
  TCCR1A = 0x00;                        // Modo normal
  TCCR1B |= (1 << ICES1) | (1 << CS12); // Borda de subida e Prescaler 256
  TIMSK1 = (1 << ICIE1);                // Habilita a interrupção por captura

  DDRC &= ~(1 << 0);   // PC0 como entrada para o LM35
  DIDR0 = (1 << 0);    // Desliga PC0 como digital
  ADMUX = 0b01000000;  // Tref=AVCC, justificado a direita, CANAL ADC0
  ADCSRA = 0b11101111; // Habilita ADC, inicia conversão, auto-trigger, interruoção habilita e Prescaler=128
  ADCSRB = 0x00;       // Triger no modo de conversão continua

  TCCR0A = 0x00;                      // Modo de operação normal
  TCCR0B = (1 << CS02) | (1 << CS00); // Prescaler = 1024
  TIMSK0 |= (1 << TOIE0);             // Habilita a interrupção do timer 0
  TCNT0 = 193;                        // Valor de TNCT0

  sei(); // Habilita as interrupções gerais
  cmd_LCD(0x01, 0);

  while (1)
  {
    cmd_LCD(0x80, 0);
    escreve_LCD("Freq:");
    cmd_LCD(0x85, 0);
    sprintf(buffer, "%d", frequencia); // Converte para string
    escreve_LCD(buffer);
    cmd_LCD(0x8A, 0);
    escreve_LCD("Hz");

    cmd_LCD(0xC0, 0);
    escreve_LCD("Temperatura:");
    cmd_LCD(0xCC, 0);
    sprintf(buffer, "%d", leADC_i); // Converte para string
    escreve_LCD(buffer);              // Escreve no LCD
    _delay_ms(200);
  }
}

ISR(TIMER1_CAPT_vect)
{
  if (TCCR1B & (1 << ICES1)) // Testa a borda de subida
    tempo_borda = ICR1;      // Atribi o valor de ICR1
  else
  {
    delta = (ICR1 - tempo_borda) * 16; // Calcula a difereça de tempo
    periodo = (delta * 2.0) / 1000.0;  // Calcula o periodo e conver us para ms
    frequencia = (1000 / periodo);     // Calcula a frequencia
  }
  TCCR1B ^= (1 << ICES1); // Inverte a borda
}

ISR(TIMER0_OVF_vect)
{
  cont0++;
  if (cont0 == 1000)
  {
    leADC_f = ((leADC_f * 5.0) / 1023.0) / 0.01;
    leADC_i = (int)leADC_f;
    cont0 = 0;
  }

  TCNT0 = 99; // TCT0 recebe o valor 99 para o timer de  0.1s
}

ISR(ADC_vect)
{
  leADC_f = ADC;
}