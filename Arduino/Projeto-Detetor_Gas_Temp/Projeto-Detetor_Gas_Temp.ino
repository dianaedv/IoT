#include <ESP8266_Lib.h> //INCLUSÃO DE BIBLIOTECA
#include <BlynkSimpleShieldEsp8266.h> //INCLUSÃO DE BIBLIOTECA
#include <SoftwareSerial.h> //INCLUSÃO DE BIBLIOTECA

#include "DHT.h"
 
#define DHTPIN A1 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// conexão do esp8266 com o blynk
SoftwareSerial EspSerial(10, 11); // 10 = RX, 11 = TX

char auth[] = "YourAuthToken"; // Auth Token fornecido pelo projeto no Blynk.
BlynkTimer timer; // objeto do tipo BlynkTimer
char ssid[] = "Nome__Wifi"; // Nome da rede sem fio
char pass[] = "Senha_Wifi"; // Senha da rede sem fio

#define ESP8266_BAUD 9600 // taxa de comunicação do esp8266 com o arduino uno

ESP8266 wifi(&EspSerial);

// definição dos pinos dos sensores
int pinMQ2 = A0; //Pino do sensor de gás MQ2
int pinDHT = A2; //Pino do sensor de temperatura DHT

// definição dos pinos dos sinalizadores
int buzzer = 8;  //Pino
int led_normal = 11; //led para reportar mudança de leitura do sensor de gas
int led_DHT = 12; //led para reportar mudança de leitura do sensor de temperatura
int led_MQ2 = 13; //led para reportar mudança de leitura do sensor de gas

// definição das variáveis para acionar o buzzer e led do buzzer
int limite_gas = 31; //Variável para selecionar a quantidade de Gás/Fumaça detectada
float limite_temp = 55.0;
int leitura_gas = 0;
int leitura_temp = 0;

float h = 0;
float t = 0;
float g = 0;

void setup() 
{
  pinMode(buzzer, OUTPUT);
  pinMode(led_normal, OUTPUT);
  pinMode(led_MQ2, OUTPUT);
  pinMode(led_DHT, OUTPUT);
  Serial.begin(9600);

  dht.begin();
  delay(10);
  EspSerial.begin(ESP8266_BAUD);
  delay(100);
  Blynk.begin(auth, wifi, ssid, pass);

  timer.setInterval(1000L, sendUptime);
  
}

void sendUptime(){
  Blynk.virtualWrite(1, t); //ENVIA AO WIDGET GAUGE (PINO VIRTUAL 1) O VALOR DA TEMPERATURA
  Blynk.virtualWrite(2, h); //ENVIA AO WIDGET GAUGE (PINO VIRTUAL 2) O VALOR DA UMIDADE
}
}
 
void loop() 
{
  Serial.println("-----+++++-----+++++-----+++++");
  digitalWrite(led_normal, HIGH);
  
  // Aquisição de gases inflamáveis pelo sensor MQ2 e retorno do valor na porta serial
  leitura_gas = analogRead(pinMQ2); //Faz a leitura da entrada do sensor
  leitura_gas = map(leitura_gas, 0, 1023, 0, 100); //Faz a conversão da variável para porcentagem
  Serial.print("Concentração de gás: "); //Escreve o valor na porta Serial
  Serial.print(leitura_gas);
  Serial.println(" %");
  

  // Aquisição da temperatura pelo sensor DHT11 e retorno do valor no monitor serial
  // A leitura da temperatura e umidade pode levar 250ms!
  // O atraso do sensor pode chegar a 2 segundos.
  h = dht.readHumidity();
  t = dht.readTemperature();
  // testa se retorno é valido, caso contrário algo está errado.
  if (isnan(t) || isnan(h)) 
  {
    Serial.println("Failed to read from DHT");
  } 
  else
  {
    Serial.print("Umidade: ");
    Serial.print(h);
    Serial.println(" %t");
    Serial.print("Temperatura: ");
    Serial.print(t);
    Serial.println(" ºC");
  }
  
  // Acionamento dos sinalizadores
  if (leitura_temp >= limite_temp) {   // Caso a temperatura lida seja maior que o limite definido
    digitalWrite(led_normal, LOW);
    digitalWrite(led_DHT, HIGH);       // aciona o led amarelo
  }
  else {
    digitalWrite(led_DHT, LOW);
  }
  if (leitura_gas >= limite_gas) {    // Caso o valor da leitura do gas seja maior que o limite definido
    digitalWrite(led_normal, LOW);
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
  Blynk.run();
  timer.run();
}
