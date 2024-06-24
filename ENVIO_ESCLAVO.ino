#include <WiFi.h>
#include <WiFiClient.h>

const char* ssid = "Jazmin";
const char* password = "04072002";

IPAddress receiverIP(192, 168, 1, 100); // IP del ESP32 receptor
const uint16_t receiverPort = 12345;     // Puerto en el ESP32 receptor

WiFiClient client;

// Variables para almacenar las lecturas de los sensores
float temperatura = 0.0;
float humedad = 0.0;
float humedTierra = 0.0;

void setup() {
  Serial.begin(9600);
  analogReadResolution(12);
  delay(1000);

  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  /*
    // Leer humedad de la tierra
  float sensorTierra  = analogRead(TierraPin);
  float humedTierra = map(sensorTierra, 0, 4095, 1000, 0)/10.0;

  // Leer humedad y temperatura ambiental
  temperatura = dht.readTemperature();
  humedad = dht.readHumidity();
  */


  // Simula lecturas de sensores (reemplaza con tus lecturas reales)
  temperatura = random(20, 30);       // Ejemplo de temperatura entre 20 y 30 grados Celsius
  humedad = random(40, 60);           // Ejemplo de humedad entre 40% y 60%
  humedTierra = random(0, 1023);     // Ejemplo de lectura de humedad del suelo (valores entre 0 y 1023)

  if (!client.connected()) {
    if (client.connect(receiverIP, receiverPort)) {
      Serial.println("Conectado al servidor");

      // Construye la cadena de datos a enviar
      String dataToSend = String(temperatura, 1) + "," + String(humedad, 1) + "," + String(humedTierra);

      client.println(dataToSend);

      Serial.print("Datos enviados: ");
      Serial.println(dataToSend);

      client.stop(); // Cierra la conexión después de enviar los datos

      delay(2000); // Intervalo de envío de datos
    } else {
      Serial.println("Error de conexión al servidor");
    }
  }
}
