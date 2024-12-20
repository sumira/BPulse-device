#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
const int LED_BUILTIN = 14;

// Display setup
Adafruit_PCD8544 display = Adafruit_PCD8544(18, 23, 4, 15, 2);

// Wi-Fi credentials
const char* ssid = "POCO";
const char* password = "11111111";

// MQTT broker credentials
const char* mqtt_server = "29e69e59ef7d4c5eb41e2507cb41dfc7.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_user = "node";
const char* mqtt_password = "ndm9897";

// Root certificate
static const char *root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

// MQTT client setup
WiFiClientSecure espClient;
PubSubClient client(espClient);

void displayText(const String& text, uint8_t textSize = 1, bool invert = false) {
  display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(invert ? WHITE : BLACK, invert ? BLACK : WHITE);
  display.setCursor(0, 0);
  display.println(text);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
}

void centerText(const String& text, int textSize, int yPos) {
  int textWidth = text.length() * 6 * textSize;
  int x = (display.width() - textWidth) / 2;
  display.setTextSize(textSize);
  display.setCursor(x, yPos);
  display.print(text);
}

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
void displayWiFiLogo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Connecting to WiFi...");
  // Draw a simple WiFi icon
  display.drawLine(10, 30, 30, 30, BLACK); // Base line
  display.drawLine(15, 25, 25, 25, BLACK); // Middle line
  display.drawLine(18, 20, 22, 20, BLACK); // Top line
  display.fillCircle(20, 35, 2, BLACK);    // Dot
  display.display();
}

void displayCloudLogo() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Connecting to Cloud...");
  // Draw a simple cloud icon
  display.fillCircle(20, 30, 5, BLACK); // Left part of cloud
  display.fillCircle(30, 30, 7, BLACK); // Right part of cloud
  display.fillRect(18, 32, 20, 5, BLACK); // Bottom part of cloud
  display.display();
}

// Wi-Fi setup
void setup_wifi() {
  displayWiFiLogo(); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.println("WiFi Connected!");
  display.display();;
}


// MQTT reconnect
void reconnect() {
  while (!client.connected()) {
    displayCloudLogo(); 

    if (client.connect("ESP32Client", mqtt_user, mqtt_password)) {
      display.clearDisplay();
      display.println("Connected with Bpulse Cloud!");
      display.display();

      // Subscribe to topics
      client.subscribe("test/integer");
      client.subscribe("test/first"); 
    } else {
      display.clearDisplay();
      display.println("MQTT Failed!");
      display.display();
      delay(5000);
    }
  }
}

// MQTT callback
void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  display.println("Received:");
  display.println("Topic:");
  display.println(topic);
  display.println("Message:");
  display.println(message);
  display.display();

  if (String(topic) == "test/integer") {
  if (message == "1") {
    digitalWrite(LED_BUILTIN, LOW);  
    } else if (message == "0") {
    digitalWrite(LED_BUILTIN, HIGH); 
    }
 }

}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); 


  display.begin();
  display.setContrast(60);
  display.clearDisplay();

  for (int i = 0; i <= 100; i += 10) {
  display.clearDisplay();
  centerText("BPulse", 2, 10);
  drawCenteredBatteryIcon(i);
  display.display();
  delay(200);
}


  setup_wifi();
  delay(100);

  espClient.setCACert(root_ca);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  reconnect();
}



void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
