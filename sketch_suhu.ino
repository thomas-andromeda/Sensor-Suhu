#define BLYNK_TEMPLATE_ID "TMPL6Jopwf1UL"
#define BLYNK_TEMPLATE_NAME "Sensor Suhu Berdasarkan Device Ruangan"
#define BLYNK_AUTH_TOKEN "pU4zEfvXVI9vTgRX6F0Ubcohih0qPWkX"

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

char ssid[] = "sendangrejo";
char pass[] = "thomas777";

#define DHTPIN 4 
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
BlynkTimer timer;

bool isRunning = true; 

void sendSensorData() {
  // Jika status isRunning false, maka jangan ambil/kirim data
  if (!isRunning) return;

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Gagal membaca sensor!");
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!   ");
    return;
  }

  // Kirim ke Blynk
  if (Blynk.connected()) {
    Blynk.virtualWrite(V1, t);
    Blynk.virtualWrite(V2, h);
  }

  lcd.setCursor(0, 0);
  lcd.print("T: "); lcd.print(t, 1); lcd.write(223); lcd.print("C      ");
  lcd.setCursor(0, 1);
  lcd.print("H: "); lcd.print(h, 1); lcd.print("%      ");

  Serial.print("Suhu: "); Serial.print(t);
  Serial.print(" | Lembab: "); Serial.println(h);
}

void setup() {
  Serial.begin(115200);
  
  dht.begin();
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0,0);
  lcd.print("Connecting...");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  
  lcd.clear();
  lcd.print("Blynk Online!");
  Serial.println("System Ready. Ketik 's' untuk Stop, 'r' untuk Run.");
  delay(2000);
  
  timer.setInterval(5000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();

  if (Serial.available() > 0) {
    char command = Serial.read();
    
    if (command == 's') {
      isRunning = false;
      Serial.println("--- PENGIRIMAN DATA DIHENTIKAN ---");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Status: STOPPED");
    } 
    else if (command == 'r') {
      isRunning = true;
      Serial.println("--- PENGIRIMAN DATA DIMULAI KEMBALI ---");
      lcd.clear();
    }
  }
}