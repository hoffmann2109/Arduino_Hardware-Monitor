#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

// last‐seen temps
float coreTemps[8];  
int cores = 0;

void setup() {
  Serial.begin(115200);
  u8g2.begin();
}

void loop() {
  // Read all available lines
  while (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    // e.g. "Core 0: 42.37"
    int colon = line.indexOf(':');
    if (colon > 0) {
      String label = line.substring(0, colon);        // "Core 0"
      float val   = line.substring(colon+1).toFloat();// 42.37
      int idx = label.substring(5).toInt();           // extract core number
      if (idx >= 0 && idx < 8) {
        coreTemps[idx] = val;
        cores = max(cores, idx+1);
      }
    }
  }

  // Draw all cores
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  for (int i = 0; i < cores; i++) {
    u8g2.setCursor(0, 15 + i*10);
    u8g2.print("C"); u8g2.print(i); u8g2.print(": ");
    u8g2.print(coreTemps[i], 1);
    u8g2.print("C");
  }

  u8g2.sendBuffer();
  delay(50);
}
