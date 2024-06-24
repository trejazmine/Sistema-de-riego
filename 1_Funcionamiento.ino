#include <WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>
#include "DHT.h"

// Definimos los pines
#define TierraPin 4
#define AguaPower 14
#define AguaPin 35
#define DHTPIN  15      
#define DHTTYPE DHT22 //Definimos que el tipo de sensor usado - DHT22
DHT dht(DHTPIN, DHTTYPE);
#define rele 12  // Pin GPIO 12 conectado al pin IN del relé
#define buzzerPin 33 // El pin 3 está conectado al zumbador

// Definimos los umbrales
#define HUMED_LIMIT 40
#define TEMP_MAX 30
#define TEMP_MIN 10
#define HUMED_MIN 40
#define HUMED_MAX 60
#define NIVEL_LIMIT_MAX 3.80
#define NIVEL_LIMIT_MIN 1.50
#define alturabajosensor 4.50

float nivelAgua = 0;
float temperatura;
float humedad;

void setup() {
  pinMode(AguaPower, OUTPUT);
  digitalWrite(AguaPower, LOW);

  Serial.begin(9600);
  analogReadResolution(12);
  
  // Rele --------
  pinMode(rele, OUTPUT);  // Configura el pin del relé como salida
  digitalWrite(rele, LOW);  // Asegura que el relé esté inicialmente apagado
  delay(1000);
  
  Serial.println("Sensores Instalados y listos"); //Imprimimos una frase
  dht.begin(); // Iniciamos nuestro sensor DHT
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
    digitalWrite(buzzerPin, HIGH); // Encender el buzzer
    Serial.println(" cm -> LLENAR EL TANQUE!");

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
    Serial.println("Regando...");
    digitalWrite(rele, HIGH); // Encender el relé (activar riego)
    delay(3000); // Mantener la bomba encendida durante 3 segundos
    digitalWrite(rele, LOW); // Apagar el relé (detener riego)
  } else {
    digitalWrite(rele, LOW); // Asegurarse de que el relé está apagado
    Serial.println("ESTADO: No es necesario regar");
  }

  // Esperar antes de la siguiente lectura
  delay(5000); // 5 segundos  
  Serial.println("-----------------------------------------");

}


