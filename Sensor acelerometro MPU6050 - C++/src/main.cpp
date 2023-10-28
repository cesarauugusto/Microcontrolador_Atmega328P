#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "HX711.h"
#include "Wire.h"
#include <Arduino.h>

const int LOADCELL_DOUT_PIN = 12;
const int LOADCELL_SCK_PIN = 11;
HX711 scale;

// Varaiveis Globais
char buffer[6] = "00000"; // buffer para converter os digitos em string
int main()
{
  
  Serial.begin(9600);

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.read(); // Aguada o termino de verificação do peso
  scale.set_scale(538965);
  scale.tare(0); // Fixa o peso como tara

  while (1)
  {

    // Config HX711
    Serial.print("Valor da Leitura:  ");
    Serial.println(scale.get_units(20), 3); // Retorna peso descontada a tara
    // valor = (scale.get_units(20), 3);

  }
}