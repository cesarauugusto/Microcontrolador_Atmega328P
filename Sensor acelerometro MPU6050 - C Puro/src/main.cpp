#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>

/* Endereço do slave */
#define MPU6050_ADDRESS 0x68

/* Definição do baud rate*/
#define BAUD 9600
#define MYUBRR F_CPU / 16 / BAUD - 1

// Variaveis de Buffer
char buffer[3];     // buffer para converter os digitos em string
char bufferfreq[6]; // buffer para converter os digitos em string
char bufferX[3];    // buffer para converter os digitos em string
char bufferY[3];    // buffer para converter os digitos em string
char bufferZ[3];    // buffer para converter os digitos em string

// Varaiveis Globais
volatile unsigned int leADC_i, resto, valor, cont0;
unsigned int tempo_borda, delta, frequencia;
float leADC_f, periodo;

/* Inicializa o I2C */
void i2c_init()
{
  /* Frequência do barramento I2C */
  TWSR = 0;                            // modo de operação padrão
  TWBR = ((F_CPU / 400000L) - 16) / 2; // velocidade de transmissão de 400kHz
  TWCR = 1 << TWEN;                    // TWEB ativo, habilita I2C
}

/* Transmite a condição de início */
void i2c_start()
{
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // condição de início (TWEN ativo, habilita I2C)
  while (!(TWCR & (1 << TWINT)))
    ; // aguarde até que a operação I2C seja concluída antes de continuar.
}
/* Transmite a condição de parada */
void i2c_stop()
{
  TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // condição de parada (TWEN ativo, habilita I2C)
}
/* Transmite o dado */
void i2c_write(uint8_t data)
{
  TWDR = data;                       // carrega o dado
  TWCR = (1 << TWINT) | (1 << TWEN); // inicia a transmissão do dado
  while (!(TWCR & (1 << TWINT)))
    ; // aguarde até que a operação I2C seja concluída antes de continuar.
}
/* Leitura dos dados com ACK ativo */
uint8_t i2c_read_ack()
{
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // inicia a leitura dos dados com ACK ativo. TWINT ativo, indica operação I2C concluída
  while (!(TWCR & (1 << TWINT)))
    ;          // aguarde até que a operação I2C seja concluída antes de continuar
  return TWDR; // retorna o dado
}
/* Leitura dos dados com ACK inativo */
uint8_t i2c_read_nack()
{
  TWCR = (1 << TWINT) | (1 << TWEN); // inicia a leitura dos dados. TWINT ativo, indica operação I2C concluída
                                     // TWEN ativo, habilita I2C
  while (!(TWCR & (1 << TWINT)))
    ;          // aguarde até que a operação I2C seja concluída antes de continuar.
  return TWDR; // retorna valor lido
}
/* Inicializa o MPU6050 */
void mpu6050_init()
{
  i2c_start();
  i2c_write(MPU6050_ADDRESS << 1);
  i2c_write(0x6B); // Registrador PWR_MGMT_1
  i2c_write(0x00); // Desativar o modo de baixo consumo
  i2c_stop();
}
/* Leitura dos dados do acelerômetro */
void mpu6050_read_accelerometer(int16_t *x, int16_t *y, int16_t *z)
{
  i2c_start();
  i2c_write(MPU6050_ADDRESS << 1); // escreve o endereço do dispositivo MPU6050 no barramento I2C para iniciar a comunicação com o acelerômetro
  i2c_write(0x3B);                 // escreve o endereço do registrador ACCEL_XOUT_H no MPU6050
                                   // que é onde os dados do acelerômetro X são armazenados
  i2c_stop();

  i2c_start();
  i2c_write((MPU6050_ADDRESS << 1) | 1);        // escreve o endereço do dispositivo MPU6050 com o bit de leitura (bit menos significativo definido como 1) para iniciar a leitura dos dados.
  *x = (i2c_read_ack() << 8) | i2c_read_ack();  // lê o byte mais significativo (ACCEL_XOUT_H) e o byte menos significativo (ACCEL_XOUT_L) do registrador de dados do acelerômetro X e armazena o valor no ponteiro *x.
  *y = (i2c_read_ack() << 8) | i2c_read_ack();  // lê o byte mais significativo (ACCEL_YOUT_H) e o byte menos significativo (ACCEL_YOUT_L) do registrador de dados do acelerômetro Y e armazena o valor no ponteiro *y.
  *z = (i2c_read_ack() << 8) | i2c_read_nack(); // lê o byte mais significativo (ACCEL_ZOUT_H) e o byte menos significativo (ACCEL_ZOUT_L) do registrador de dados do acelerômetro Z e armazena o valor no ponteiro *z.
  i2c_stop();
}

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
void setup() {
    
    i2c_init();
    mpu6050_init();
}
/* Rotina principal */
void loop() {
    int16_t accelerometer_x, accelerometer_y, accelerometer_z;
    mpu6050_read_accelerometer(&accelerometer_x, &accelerometer_y, &accelerometer_z);

    sprintf(bufferX, "freq:%d", accelerometer_x);
    USART_String(bufferX);
    sprintf(bufferY, "freq:%d", accelerometer_y);
    USART_String(bufferY);
    sprintf(bufferZ, "freq:%d", accelerometer_z);
    USART_String(bufferZ);

    delay(500);
}