#include <LiquidCrystal.h>
#include <DHT.h>
#include <DHT_U.h>

//Pines
int bombaAgua = 8;
int sensorTemp = 9; 
int boton = 10;

//variables    
int temp;
int hum;
int estadoBoton;

//Objetos
LiquidCrystal lcd(7,6,5,4,3,2);
DHT dht(sensorTemp, DHT11); 

void setup() {
 //setup Pantalla
  lcd.begin(16, 2);
  
  //setup rele
  pinMode(bombaAgua,OUTPUT);

  //setup Sensor temp
  dht.begin();

  //setup boton
  pinMode(boton, INPUT);
}

void loop() {
  //Inicializacion de Variables sensor
  temp = dht.readTemperature(); 
  hum = dht.readHumidity();
  estadoBoton=digitalRead(boton);

  //Pantalla
  lcd.setCursor(0,0);
  lcd.print("Temperatura: " + String(temp) +" °C");
  lcd.setCursor(0,1);
  lcd.print("Humedad: "+ String(hum) +" %   " );
  
  Serial.print("Humedad: ");
  Serial.print(sensorTemp);
  Serial.println("%");

  if(temp>=25&&hum<=20){ //Condicion para regar automaticamente
    digitalWrite(bombaAgua, LOW); //Prender bomba de agua
     
     lcd.setCursor(0,0);
     lcd.print("*****Regando****"); // Mostrar en la pantalla LCD
     lcd.setCursor(0,1);
     lcd.print("****************");
     delay(5000); //Durante 5 segundos
     
  } else if(estadoBoton==HIGH){ //Si se presiona el botón
     digitalWrite(bombaAgua, LOW); //Prender bomba de agua
     
     lcd.setCursor(0,0);
     lcd.print("*****Regando****"); // Mostrar en la pantalla LCD
     lcd.setCursor(0,1);
     lcd.print("****************");
     delay(1000); //Durante un segundo
     
  } else { //Si ninguna de las condiciones anteriores...
    digitalWrite(bombaAgua, HIGH); //Bomba apagada
  }

  //Si no recibe informacion del sensor
  if(temp==0&&hum==0){
     lcd.setCursor(0,0);
     lcd.print("*****ERROR******"); // Mostrar en la pantalla LCD
     lcd.setCursor(0,1);
     lcd.print("***NO SENSOR***");
     delay(1000); //Actualizacion cada segundo
  }

}

