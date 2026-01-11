#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <IRremote.h>

// --- AYARLAR ---
LiquidCrystal_I2C lcd(0x27, 16, 2); 
const int BUZZER_PIN = 12;

// Keypad Ayarları
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

// IR Kumanda
int RECV_PIN = 11; 
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // LCD Başlat
  lcd.init();
  lcd.backlight();
  
  // IR Başlat
  irrecv.enableIRIn(); 

  // Açılış Testi
  lcd.setCursor(0,0);
  lcd.print("Sistem Aktif");
  
  // Timer Çakışmasını önleyen özel bip fonksiyonu
  manuelBip(1000, 200); 
  delay(1000);
  
  lcd.clear();
  lcd.print("Durum: Ofisteyim");
}

void loop() {
  // --- KEYPAD KONTROLÜ ---
  char basilantus = tusTakimi.getKey();
  
  if (basilantus) {
    manuelBip(1500, 50); // Kısa tuş sesi
    
    if(basilantus == '1') ekraniGuncelle("Durum:", "Toplantidayim");
    else if(basilantus == '2') ekraniGuncelle("Durum:", "Yemege Ciktim");
    else if(basilantus == '3') ekraniGuncelle("Durum:", "Musaitim");
    else if(basilantus == 'A') ekraniGuncelle("Lutfen", "Bekleyiniz");
    else if(basilantus == '*') {
       manuelBip(2000, 100); 
       delay(100);
       manuelBip(2000, 100); // İki kere bip (Kargo uyarısı)
       ekraniGuncelle("Ziyaretci Notu:", "Kargo Geldi");
       delay(2000); 
       ekraniGuncelle("Durum:", "Ofisteyim");
    }
  }

  // --- IR KUMANDA ---
  if (irrecv.decode()) {
    // Gelen sinyali serial'a yaz (test için)
    Serial.print("IR Kodu: ");
    Serial.println(irrecv.decodedIRData.decodedRawData, HEX);

    // Tuşlara göre işlem
    if (irrecv.decodedIRData.decodedRawData != 0) { // Parazit değilse
      manuelBip(1200, 50);
      // Wokwi Kumanda Kodları (1 ve 2 tuşları)
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

// --- ÖZEL SES FONKSİYONU ---
// tone() komutu IRremote ile çakıştığı için bunu kullanıyoruz.
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