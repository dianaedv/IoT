/*************************************************************
  WARNING!
    It's very tricky to get it working. Please read this article:
    http://help.blynk.cc/hardware-and-libraries/arduino/esp8266-with-at-firmware

  You’ll need:
   - Blynk IoT app (download from App Store or Google Play)
   - Arduino Mega 2560 board
   - Decide how to connect to Blynk
     (USB, Ethernet, Wi-Fi, Bluetooth, ...)

  There is a bunch of great example sketches included to show you how to get
  started. Think of them as LEGO bricks  and combine them as you wish.
  For example, take the Ethernet Shield sketch and combine it with the
  Servo example, or choose a USB sketch and add a code from SendData
  example.
 *************************************************************/

#include "DHT.h"
 
#define DHTPIN A1 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

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
float leitura_temp = 0;
float leitura_umidade = 0;


/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPLpxB4nST2"
#define BLYNK_DEVICE_NAME "GasDetector"
#define BLYNK_AUTH_TOKEN "NelcQi10cdnztchB_tB-TMrBt4Isctgg"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "dlink-4";
char pass[] = "abcd1234";

// Hardware Serial on Mega, Leonardo, Micro...
#define EspSerial Serial3

// or Software Serial on Uno, Nano...
// #include <SoftwareSerial.h>
// SoftwareSerial EspSerial(15, 14); // RX, TX

// Your ESP8266 baud rate:
#define ESP8266_BAUD 9600

ESP8266 wifi(&EspSerial);
BlynkTimer timer;

void setup()
{
  Serial.begin(9600);
  Serial.println("DHTxx test!");
  pinMode(buzzer, OUTPUT);
  pinMode(led_normal, OUTPUT);
  pinMode(led_MQ2, OUTPUT);
  pinMode(led_DHT, OUTPUT);
  dht.begin();
  
  // Debug console
  Serial.begin(9600);

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(BLYNK_AUTH_TOKEN, wifi, ssid, pass);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, wifi, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, wifi, ssid, pass, IPAddress(192,168,1,100), 8080);

  timer.setInterval(1000L, sendUptime);
}

void sendUptime(){
  Blynk.virtualWrite(0, leitura_gas); // ENVIA AO WIDGET GAUGE (PINO VIRTUAL 0) O VALOR DA LEITURA DO GAS
  Blynk.virtualWrite(1, leitura_temp); //ENVIA AO WIDGET GAUGE (PINO VIRTUAL 1) O VALOR DA TEMPERATURA
  Blynk.virtualWrite(2, leitura_umidade); //ENVIA AO WIDGET GAUGE (PINO VIRTUAL 2) O VALOR DA UMIDADE
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
  float h = dht.readHumidity();
  leitura_umidade = h;
  float t = dht.readTemperature();
  leitura_temp = t;
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
  // You can inject your own code or combine it with other sketches.
  // Check other examples on how to communicate with Blynk. Remember
  // to avoid delay() function!
