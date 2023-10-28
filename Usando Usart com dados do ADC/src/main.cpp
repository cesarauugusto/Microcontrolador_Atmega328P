#include <Arduino.h>

/* Definição do baud rate*/
#define BAUD 9600
#define MYUBRR F_CPU / 16 / BAUD - 1

/* Variáveis */
unsigned char rec[4]; // LL1\r ou DL1\r ou LL2\r ou DL1\r
int i = 0;
float leADC_f, periodo;
unsigned int tempo_borda, delta, frequencia;
unsigned int leADC_i, resto, valor;

char bufferTemp[16];
char bufferVel[16];

/* Rotina de inicialização da USART */
void USART_Init(unsigned int ubrr)
{
  UBRR0H = (unsigned char)(ubrr >> 8); // Ajusta a taxa de transmissão
  UBRR0L = (unsigned char)ubrr;
  UCSR0B = (1 << RXEN0) | (1 << TXEN0); // Habilita o transmissor e o receptor
  UCSR0B |= (1 << RXCIE0);              // Habilita a interrupção na recepção completa
  UCSR0C = (3 << UCSZ00);               // Ajusta o formato do frame:
                                        // 8 bits de dados e 1 de parada
}
/* Rotina de transmissão da USART */
void USART_Transmit(unsigned char data)
{
  while (!(UCSR0A & (1 << UDRE0)))
    ;          // Espera a limpeza do registr. de transmissão
  UDR0 = data; // Coloca o dado no registrador e o envia
}
/* Rotina de transmissão de strings */
void USART_String(char *data)
{ // LED1 Ligado\0
  for (; *data != 0; data++)
    USART_Transmit(*data);
}
/* Rotina de recepção da USART */
unsigned char USART_Receive(void)
{
  while (!(UCSR0A & (1 << RXC0)))
    ;          // Espera o dado ser recebido
  return UDR0; // Lê o dado recebido e retorna
}
/* Rotina de inicialização do ADC */
void ADC_init()
{
  /* Setup do ADC */
  DDRC &= ~(1 << 5);   // PC5 como entrada para ADC0
  DIDR0 = (1 << 5);    // desliga PC5 como digital
  ADMUX = 0b01000101;  // Tref=AVCC,justificado a direita,canal ADC0
  ADCSRA = 0b11101111; // habilita ADC, inicia conversão, auto-trigger, interrupção habilitada e PR=128
  ADCSRB = 0x00;       // trigger no modo de conversão contínua
}

void CAPT_init()
{
  DDRB &= ~(1 << 0);                    // PB0 como entrada para captura de sinal
  TCCR1A = 0x00;                        // Modo normal
  TCCR1B |= (1 << ICES1) | (1 << CS12); // Borda de subida e Prescaler 256
  TIMSK1 = (1 << ICIE1);                // Habilita a interrupção por captura
}

/* Rotina principal */
int main()
{
  USART_Init(MYUBRR);
  ADC_init();
  CAPT_init();
  sei();

  while (1)
  {

    

    sprintf(bufferTemp, "Temp = %d ", leADC_i);
    USART_String(bufferTemp);

    sprintf(bufferVel, "Freq = %d\n", frequencia);
    USART_String(bufferVel);

    _delay_ms(1000);
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

/* Rotina de serviço da interrupção do ADC */
ISR(ADC_vect)
{
  leADC_f = ADC;
  leADC_f = ((leADC_f * 5.0) / 1023.0) / 0.01;
    leADC_i = (int)leADC_f;
}