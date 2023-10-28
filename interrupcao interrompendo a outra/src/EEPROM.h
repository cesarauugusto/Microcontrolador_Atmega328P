#include <Arduino.h>

void EEPROM_escrita(unsigned int uiEndereco, unsigned char ucDado) 
{ 
while(EECR & (1<<EEPE)); //espera completar um escrita prévia
EEAR = uiEndereco; //carrega o endereço para a escrita 
EEDR = ucDado; //carrega o dado a ser escrito 
EECR |= (1<<EEMPE); //escreve um lógico em EEMPE
EECR |= (1<<EEPE); //inicia a escrita ativando EEPE
} 
unsigned char EEPROM_leitura(unsigned int uiEndereco) 
{ 
while(EECR & (1<<EEPE)); //espera completar um escrita prévia
EEAR = uiEndereco; //escreve o endereço de leitura 
EECR |= (1<<EERE); //inicia a leitura ativando EERE 
return EEDR; //retorna o valor lido do registrador de 
//dados 
}