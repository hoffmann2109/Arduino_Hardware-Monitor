#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0); // screen object

float temps[15]; // array for all temperature readings
int cores = 0;
String labels[15];

void setup() {
  Serial.begin(115200); // Serical connection with 115200 baud (bits per second)
  u8g2.begin(); // Initialize the display
}

void loop() {
  int counter = 0;

  while (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    int colon = line.indexOf(':');
    if (colon > 0) {
      String label = line.substring(0, colon);
      float val = line.substring(colon+1).toFloat();

      if (label.indexOf("Core") == -1) { // Only if NOT a Core
          labels[counter] = label;   // Save the label
          temps[counter] = val;      // Save the temp
          counter++;                 // Now increment
      }
    }
  }

  cores = counter; // Update how many temperatures we saved

  // Now draw:
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);

  for (int i = 0; i < cores; i++) { // Only up to real readings
    u8g2.setCursor(0, 15 + i*10);
    u8g2.print(labels[i]);
    u8g2.print(": ");
    u8g2.print(temps[i], 1);
    u8g2.print("°C");
  }

  u8g2.sendBuffer();
  delay(3000);
}

// TODO: Write a method to map output names to real hardware names
