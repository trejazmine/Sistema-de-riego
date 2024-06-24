#include <WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>
#include "DHT.h"

// Definimos los pines
#define TierraPin 32
#define AguaPower 14
#define AguaPin 35
#define DHTPIN  15      
#define DHTTYPE DHT22 //Definimos que el tipo de sensor usado - DHT22
DHT dht(DHTPIN, DHTTYPE);
#define rele 12  // Pin GPIO 12 conectado al pin IN del relé
const int buzzerPin = 4; // El pin 4 está conectado al zumbador

// Definimos los umbrales
#define HUMED_LIMIT 50
#define TEMP_MAX 30
#define TEMP_MIN 27 //----------------
#define HUMED_MAX 85
#define HUMED_MIN 75 //-----------------
#define NIVEL_LIMIT_MAX 3.80
#define NIVEL_LIMIT_MIN 2.00 //-----------------
#define alturabajosensor 1.50

float nivelAgua = 0;
float temperatura;
float humedad;

// Definimos como constantes Char las credenciales de acceso a la red WiFi
const char* ssid = "Jazmin";
const char* password = "04072002";
// Definimos las credenciales para la conexión a la plataforma
unsigned long channelID = 2563815;
const char* WriteAPIKey = "TTXX1LT58J5NNOHI";
WiFiClient cliente; // Definimos el cliente WiFi que usaremos

void setup() {
  pinMode(AguaPower, OUTPUT);
  digitalWrite(AguaPower, LOW);

  Serial.begin(9600);
  analogReadResolution(12);
  
  // Rele --------
  pinMode(rele, OUTPUT);  // Configura el pin del relé como salida
  digitalWrite(rele, LOW);  // Asegura que el relé esté inicialmente apagado
  delay(1000);
  //buzzer
  pinMode(buzzerPin, OUTPUT);

  Serial.println("Sensores Instalados y listos"); //Imprimimos una frase
  dht.begin(); // Iniciamos nuestro sensor DHT

  //Iniciamos la conexión a la red WiFi, y se imprimirán caracteres indicando el tiempo que tarda la conexión
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Conectado al WiFi");   // Una vez conectado, se imprimirá una frase
  ThingSpeak.begin(cliente); // se iniciará la conexión a la Plataforma usando el cliente definido anteriormente
  delay(1000);
}

void loop() {
    // Leer humedad de la tierra
  float sensorTierra  = analogRead(TierraPin);
  float humedTierra = map(sensorTierra, 0, 4095, 1000, 0)/10.0;

  // Leer humedad y temperatura ambiental
  temperatura = dht.readTemperature();
  humedad = dht.readHumidity();

  // Leer el valor del sensor de nivel de agua
  digitalWrite(AguaPower, HIGH);
  float sensorAgua  = analogRead(AguaPin);
  float nivelAgua = map(sensorAgua, 0, 2185, 0, 4000)/1000.00;  // Mapea a milímetros
  delay(1000);
  digitalWrite(AguaPower, LOW);

  // Comprobar lecturas
  if (isnan(humedad) || isnan(temperatura) || humedTierra <= 10) {
    Serial.println("Revisar la conexion de los sensores");
    return;
  }

  // Mostrar valores -------------------------------
  Serial.print("Temperatura ambiental: ");
  Serial.print(temperatura);
  Serial.println("°C");
  delay(1000);

  Serial.print("Humedad de ambiente: ");
  Serial.print(humedad);
  Serial.println("%");
  delay(1000);

  Serial.print("Humedad de suelo: ");
  Serial.print(humedTierra,2); 
  Serial.println("%");
  delay(1000);

  // CONTROL DEL NIVEL DE AGUA ----------------------------------------------------------------------
  Serial.print("Nivel de agua: ");
  float altura = nivelAgua + alturabajosensor;
  Serial.print(altura,2);
  if (nivelAgua < NIVEL_LIMIT_MIN) {
    digitalWrite(buzzerPin, HIGH);
    delay(1000); // Mantenerlo encendido por 1 segundo
    // Apagar el buzzer
    digitalWrite(buzzerPin, LOW);
    delay(1000); // Mantenerlo apagado por 1 segundo

  } else if (nivelAgua >= NIVEL_LIMIT_MIN && nivelAgua < NIVEL_LIMIT_MAX) {
    digitalWrite(buzzerPin, LOW); // Apagar el buzzer
    Serial.println("cm");

  } else if (nivelAgua >= NIVEL_LIMIT_MAX) {
    digitalWrite(buzzerPin, LOW); // Apagar el buzzer
    Serial.println(" cm -> VACIAR EL TANQUE!");
  }
  delay(1000);


  // CONTROL DEL RIEGO ----------------------------------------------------------------------
  bool activarriego = false;

  // Evaluar cada caso
  if (humedTierra < HUMED_LIMIT) {
    if (humedad >= HUMED_MIN && humedad <= HUMED_MAX && temperatura >= 15 && temperatura <= 25) {
      activarriego = true; // Caso 1
    } else if (humedad < HUMED_MIN && temperatura >= 15 && temperatura <= 25) {
      activarriego = true; // Caso 2
    } else if (humedad >= HUMED_MIN && humedad <= HUMED_MAX && temperatura > TEMP_MAX) {
      activarriego = true; // Caso 3
    } else if (humedad < HUMED_MIN && temperatura > TEMP_MAX) {
      activarriego = true; // Caso 4
    } else if (humedad >= HUMED_MIN && humedad <= HUMED_MAX && temperatura < TEMP_MIN) {
      activarriego = true; // Caso 5
    } else if (humedad < HUMED_MIN && temperatura < TEMP_MIN) {
      activarriego = true; // Caso 6
    }
  } else if (humedTierra >= HUMED_LIMIT && humedTierra <= 60.00) {
    if (humedad < HUMED_MIN && temperatura >= 15.00 && temperatura <= 25.00) {
      activarriego = false; // Caso 7
    } else if (humedad < HUMED_MIN && temperatura > TEMP_MAX) {
      activarriego = false; // Caso 8
    } else if (humedad < HUMED_MIN && temperatura < TEMP_MIN) {
      activarriego = false; // Caso 9
    }
  } else if (humedTierra > 60.0) {
    if (humedad >= HUMED_MIN && humedad <= HUMED_MAX && temperatura >= 15.0 && temperatura <= 25.0) {
      activarriego = false; // Caso 10
    } else if
 (humedad < HUMED_MIN && temperatura >= 15.0 && temperatura <= 25.0) {
      activarriego = false; // Caso 11
    } else if (humedad >= HUMED_MIN && humedad <= HUMED_MAX && temperatura > TEMP_MAX) {
      activarriego = false; // Caso 12
    } else if (humedad >= HUMED_MIN && humedad <= HUMED_MAX && temperatura < TEMP_MIN) {
      activarriego = false; // Caso 13
    }
  }

  // Control de riego
  if (activarriego) {
    Serial.print(" ");
    Serial.println("Regando...");
    digitalWrite(rele, HIGH); // Encender el relé (activar riego)
    delay(1500); // Mantener la bomba encendida durante 1.5 segundos
    digitalWrite(rele, LOW); // Apagar el relé (detener riego)
  } else {
    digitalWrite(rele, LOW); // Asegurarse de que el relé está apagado
    Serial.print(" ");
    Serial.println("ESTADO: No es necesario regar");
  }

  // Esperar antes de la siguiente lectura
  delay(5000); // 5 segundos  
  
  // Indicamos el orden de envío por campos o Field, en el orden definido de la plataforma, junto a los valores del sensor
  ThingSpeak.setField(1, temperatura);
  ThingSpeak.setField(2, humedad);
  ThingSpeak.setField(3, humedTierra);
  ThingSpeak.setField(4, altura);

  // Hacemos la conexión y envío de datos a la plataforma, utilizando las credenciales definidas anteriormente
  int x = ThingSpeak.writeFields(channelID, WriteAPIKey);
  if(x == 200){
    Serial.print("Datos enviados a ThingSpeak!");
    Serial.println(" ");
  } else {
    Serial.println("Error al enviar datos a ThingSpeak: " + String(x));
  }
  delay(1000);

  Serial.println("-----------------------------------------");

}
