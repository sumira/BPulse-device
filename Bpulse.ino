#include <WiFi.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(18, 23, 4, 15, 2);

// Wi-Fi credentials
const char* ssid = "POCO";
const char* password = "11111111";

void drawCenteredBatteryIcon(int percentage) {
  int batteryWidth = 20;
  int batteryHeight = 10;
  int x = (display.width() - batteryWidth) / 2;
  int y = 35;

  display.drawRect(x, y, batteryWidth, batteryHeight, BLACK);
  display.drawRect(x + batteryWidth, y + 3, 2, 4, BLACK);

  int fillWidth = map(percentage, 0, 100, 0, batteryWidth - 2);
  display.fillRect(x + 1, y + 1, fillWidth, batteryHeight - 2, BLACK);
}

void displayText(const String& text, uint8_t textSize = 1, bool invert = false) {
  display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(invert ? WHITE : BLACK, invert ? BLACK : WHITE);
  display.setCursor(0, 0);
  display.println(text);
  display.display();
  delay(2000); // Display text for 2 seconds
  display.clearDisplay(); // Clear display after 2 seconds
  display.display();
}

void centerText(const String& text, int textSize, int yPos) {
  int textWidth = text.length() * 6 * textSize;
  int x = (display.width() - textWidth) / 2;
  display.setTextSize(textSize);
  display.setCursor(x, yPos);
  display.print(text);
}

void connectToWiFi() {
  displayText("Connecting to WIFI...");
  WiFi.begin(ssid, password);

  // Wait for connection
  int timeout = 15; // Timeout in seconds
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    delay(1000);
    timeout--;
    displayText("Connecting...");
  }

  if (WiFi.status() == WL_CONNECTED) {
    displayText("WIFI Connected");
  } else {
    displayText("Connection Failed!");
  }
}

void setup() {
  display.begin();
  display.setContrast(60);
  display.clearDisplay();

  // Full battery startup screen
  for (int i = 0; i <= 100; i += 10) {
    display.clearDisplay();
    centerText("BPulse", 2, 10);
    drawCenteredBatteryIcon(i);
    display.display();
    delay(200);
  }
  delay(1000);

  connectToWiFi(); // Connect to Wi-Fi
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    displayText("WIFI Connected Successfully!");
    
  } else {
    displayText("Reconnecting WIFI...");
    connectToWiFi();
  }
}
