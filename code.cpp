#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <IRremote.h>


LiquidCrystal_I2C lcd(0x27, 16, 2); 
const int BUZZER_PIN = 12;

const byte SATIR = 4; 
const byte SUTUN = 4; 
char tuslar[SATIR][SUTUN] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte satirPinleri[SATIR] = {9, 8, 7, 6}; 
byte sutunPinleri[SUTUN] = {5, 4, 3, 2}; 
Keypad tusTakimi = Keypad(makeKeymap(tuslar), satirPinleri, sutunPinleri, SATIR, SUTUN);


int RECV_PIN = 11; 
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
 
  lcd.init();
  lcd.backlight();
  
 
  irrecv.enableIRIn(); 

  
  lcd.setCursor(0,0);
  lcd.print("Sistem Aktif");

  manuelBip(1000, 200); 
  delay(1000);
  
  lcd.clear();
  lcd.print("Durum: Ofisteyim");
}

void loop() {
  char basilantus = tusTakimi.getKey();
  
  if (basilantus) {
    manuelBip(1500, 50); 
    
    if(basilantus == '1') ekraniGuncelle("Durum:", "Toplantidayim");
    else if(basilantus == '2') ekraniGuncelle("Durum:", "Yemege Ciktim");
    else if(basilantus == '3') ekraniGuncelle("Durum:", "Musaitim");
    else if(basilantus == 'A') ekraniGuncelle("Lutfen", "Bekleyiniz");
    else if(basilantus == '*') {
       manuelBip(2000, 100); 
       delay(100);
       manuelBip(2000, 100); 
       ekraniGuncelle("Ziyaretci Notu:", "Kargo Geldi");
       delay(2000); 
       ekraniGuncelle("Durum:", "Ofisteyim");
    }
  }

  if (irrecv.decode()) {
    Serial.print("IR Kodu: ");
    Serial.println(irrecv.decodedIRData.decodedRawData, HEX);

    if (irrecv.decodedIRData.decodedRawData != 0) { 
      manuelBip(1200, 50);
      if (irrecv.decodedIRData.decodedRawData == 0xCF30FF00) ekraniGuncelle("Durum:", "Toplantidayim");
      if (irrecv.decodedIRData.decodedRawData == 0xE718FF00) ekraniGuncelle("Durum:", "Yemege Ciktim");
    }
    irrecv.resume(); 
  }
}

void ekraniGuncelle(String s1, String s2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(s1);
  lcd.setCursor(0, 1);
  lcd.print(s2);
}

void manuelBip(int frekans, int sure) {
  long gecikme = 1000000 / frekans / 2;
  long dongu = frekans * sure / 1000;
  for (long i = 0; i < dongu; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delayMicroseconds(gecikme);
    digitalWrite(BUZZER_PIN, LOW);
    delayMicroseconds(gecikme);
  }
}