#include <AuthWebpage.h>
#include <OTAInclude.h>
#include <microDS18B20.h>
#include <DistilationPage.h>

uint8_t s1_addr[] = {0x28, 0x5D, 0x1E, 0x87, 0x0, 0x0, 0x0, 0xFB};
uint8_t s2_addr[] = {0x28, 0xF3, 0xCB, 0x86, 0x0, 0x0, 0x0, 0xB7};

MicroDS18B20<12, s1_addr> sensor1; //{0x28, 0x5D, 0x1E, 0x87, 0x0, 0x0, 0x0, 0xFB}
MicroDS18B20<12, s2_addr> sensor2; //{0x28, 0xF3, 0xCB, 0x86, 0x0, 0x0, 0x0, 0xB7}

bool term_wait = true, alert = false;
int term_delay, lastUpdate;

const int speakerPin = 16; // Пин для динамика

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512); // Инициализация EEPROM
  pinMode(ledPin, OUTPUT);

  // Загрузка сохраненных настроек сети Wi-Fi
  loadConfig();

  // Включение Arduino OTA
  OTA_Initialize();

  if (ssid.isEmpty() || password.isEmpty()) {
    // Если настроек нет, запускаем точку доступа
    digitalWrite(ledPin, LOW);
    startAP();
  } else {
    // Если настройки есть, пробуем подключиться к сети
    connectToWiFi();
    PageInitialization();
    PageInitialization();
  }

  Serial.println("HTTP server started");

  pinMode(speakerPin, OUTPUT);
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();

  if (term_wait)
    Term_Check();

  if(millis() - lastUpdate >= 3600000){
    lastUpdate = millis();
    tone(speakerPin, 100, 3000);
  }

  if (!alert && term2 > 65){
    tone(speakerPin, 1, 5000);
    alert = true;
  }

  if (term_delay + 1000 <= millis()){
    term1 = sensor1.getTemp() + error;
    term2 = sensor2.getTemp() + error;
    if (term2 >= melody_cuba)
      tone(speakerPin, 1, 1000);
    if (term1 >= melody_para)
      tone(speakerPin, 1, 1000);
    term_wait = true;
  }
}

void Term_Check(){
  sensor1.requestTemp();
  sensor2.requestTemp();
  term_delay = millis();
  term_wait = false;
} // сбор данных с датчиков температуры