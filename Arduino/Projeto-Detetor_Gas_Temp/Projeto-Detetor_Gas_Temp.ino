#include "dht.h"

// definição dos pinos dos sensores
int pinMQ2 = A0; //Pino do sensor de gás MQ2
int pinDHT = A2; //Pino do sensor de temperatura DHT

// definição dos pinos dos sinalizadores
int buzzer = 8;  //Pino
int led_MQ2 = 2; //led para reportar mudança de leitura do sensor de gas
int led_DHT = 3; //led para reportar mudança de leitura do sensor de temperatura

// definição das variáveis para acionar o buzzer e led do buzzer
int limite_gas = 41; //Variável para selecionar a quantidade de Gás/Fumaça detectada
float limite_temp = 55.0;
int leitura_gas = 0;
int leitura_temp = 0;

dht DHT;

void setup()
{
  Serial.begin(9600);       //Inicia porta Serial em 9600 baud
  pinMode(buzzer, OUTPUT);
  pinMode(led_MQ2, OUTPUT);
  pinMode(led_DHT, OUTPUT);
}

void loop() {
  Serial.println("-----+++++-----+++++-----+++++");

  //Aquisição de gases inflamáveis pelo sensor MQ2 e retorno do valor na porta serial
  leitura_gas = analogRead(pinMQ2); //Faz a leitura da entrada do sensor
  leitura_gas = map(leitura_gas, 0, 1023, 0, 100); //Faz a conversão da variável para porcentagem
  Serial.println("Concentração de gás: "); //Escreve o valor na porta Serial
  Serial.println(leitura_gas);

  //Aquisição da temperatura pelo sensor DHT11 e retorno do valor no monitor serial
  leitura_temp = DHT.read11(pinDHT); //Faz a leitura da entrada do sensor
  Serial.println("Temperatura: "); //Escreve o valor na porta Serial
  Serial.println(leitura_temp);

  //Acionamento dos sinalizadores
  if (leitura_temp >= limite_temp) {   // Caso a temperatura lida seja maior que o limite definido
    digitalWrite(led_DHT, HIGH);       // aciona o led amarelo
  }
  else {
    digitalWrite(led_DHT, LOW);
  }

  if (leitura_gas >= limite_gas) {    // Caso o valor da leitura do gas seja maior que o limite definido
    digitalWrite(led_MQ2, HIGH);      // aciona o led vermelho e
    digitalWrite(buzzer, HIGH);       // aciona o buzzer (Alarme de vazamento)
    // tone(buzzer,80);
    Serial.println("Alarme disparado!!!"); //Apresenta mensagem na porta serial
    delay(1000); //Tempo de disparo do alarme
    digitalWrite(buzzer, LOW); //Desliga o alarme
    // noTone(buzzer);
    delay(1000); //Aguarda
  }
  else {
    digitalWrite(led_MQ2, LOW);
    delay(1000);
  }
}
