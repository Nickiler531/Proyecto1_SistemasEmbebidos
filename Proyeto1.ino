//Author/Contact: Camilo Alejandro Medina, Nicolas Eduardo Velasquez
//TITLE: MEDICION HUMEDAD TEMPERATURA

//Declaracion de librerias
#include "DHT.h"
#include <Wire.h>
#include "rgb_lcd.h"
#include <SD.h>

#define DHTIN 2     // Pin Conexion
#define DHTOUT 3    // Pin Diodo
#define DHTTYPE DHT11   // Tipo de sensor usado

//Definicion color pantalla LCD
const int colorR = 189;
const int colorG = 236;
const int colorB = 182;
DHT dht(DHTIN, DHTOUT, DHTTYPE); //Instancia sensor
rgb_lcd lcd; //Variable pantalla LCD

void setup() {
  Serial.begin(115200);
  //Inicializacion del sensor
  Serial.println("DHTxx test!");
  dht.begin();
  //Inicializacion pantalla
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);
  lcd.setCursor(0, 0);
  lcd.print("TEMPERATURA");
  lcd.setCursor(0, 1);
  lcd.print("Y HUMEDAD");
  delay(2000);
  lcd.clear();
  // Inicializacion de la SD CARD
  Serial.print("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");
  system("/sbin/fdisk -l > /dev/ttyGS0");
}

void loop() {

  delay(5000); //Delay entre cada medicion
  float h = dht.readHumidity(); //Leer Humedad
  float t = dht.readTemperature(); //Leer Temperatura
  // Preparar los datos para almacenarlos
  String dataString = "";
  system("date '+%H:%M:%S' > /home/root/time.txt");
  FILE *fp;
  fp = fopen("/home/root/time.txt", "r");
  char buf[9];
  fgets(buf, 9, fp);
  fclose(fp);
//Agregar simbolo 'X' cuando la lectura es erronea o sino agregar el dato correcto 
  if (isnan(h) || isnan(t)) {
    String tempmal = "X";
    String hummal = "X";
    dataString += String(buf) + "  ";
    dataString += String(tempmal) + "  ";
    dataString += String(hummal) ;
  } else {
    int temp = (int)t;
    int hum = (int)h;
    dataString += String(buf) + "  ";
    dataString += String(temp) + "  ";
    dataString += String(hum) ;
  }
// Almacenar la cadena de datos (Hora + Temperatura + Humedad)
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  dataFile.println(dataString);
  dataFile.close();
// Imprimir los datos en el puerto serial
  Serial.print("Logged data: ");
  Serial.println(dataString);
// delay(1000);
//Cambiar color del fondo de la pantalla si la temperatura supera 25°C
  if (t > 25) {
    lcd.setRGB(179, 40, 33);
  } else {
    lcd.setRGB(colorR, colorG, colorB);
  }
// Imprimir en LCD si el dato no es correcto
  if (isnan(h) || isnan(t)) {
    lcd.setCursor(0, 0);
    lcd.print("HUM");
    lcd.setCursor(11, 0);
    lcd.print("  X   ");
    lcd.setCursor(0, 1);
    lcd.print("TEMP");
    lcd.setCursor(11, 1);
    lcd.print("  X   ");
    return;
  }
// Imprimir el dato correcto
  lcd.setCursor(0, 0);
  lcd.print("HUM[%]");
  lcd.setCursor(12, 0);
  lcd.print(h);
  lcd.setCursor(0, 1);
  lcd.print("TEMP[C]");
  lcd.setCursor(12, 1);
  lcd.print(t);
}
