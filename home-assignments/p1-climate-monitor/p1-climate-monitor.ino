#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>

// --- OLED Setup ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- DHT11 Setup ---
DHTesp dht;

// --- Pin Definitions ---
const int DHT_PIN = 4;
const int RED_LED = 25;
const int GREEN_LED = 26;
const int BUZZER = 27;

// --- Timing Variables (for millis) ---
unsigned long lastReadTime = 0;
unsigned long lastLogTime = 0;
unsigned long lastScreenTime = 0;
unsigned long buzzerTurnOffTime = 0;

// --- Global Data ---
float currentTemp = 0.0;
float currentHum = 0.0;
float maxTemp = -100.0; // Starting low so first reading overrides it
float minTemp = 100.0;  // Starting high so first reading overrides it

String statusLabel = "STARTING";
bool buzzerActive = false;
int screenState = 0; // 0 = Main Live Readings, 1 = Bonus Max/Min screen

void setup() {
  Serial.begin(115200);

  // Initialize Output Pins
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Initialize DHT11
  dht.setup(DHT_PIN, DHTesp::DHT11);

  // Initialize OLED (Address 0x3C is standard for 0.96" displays)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed. Check wiring!"));
    for(;;); // Halt execution if screen fails
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  
  // Print CSV Header for Serial Monitor
  Serial.println("millis(),temp,humidity,status");
}

void loop() {
  unsigned long currentMillis = millis();

  // ---------------------------------------------------------
  // 1. Read DHT11 & Update Logic (Every 2 Seconds)
  // ---------------------------------------------------------
  if (currentMillis - lastReadTime >= 2000) {
    lastReadTime = currentMillis;

    TempAndHumidity data = dht.getTempAndHumidity();

    // Proceed only if the reading is successful
    if (dht.getStatus() == 0) {
      currentTemp = data.temperature;
      currentHum = data.humidity;

      // Bonus: Update Max/Min Tracker
      if (currentTemp > maxTemp) maxTemp = currentTemp;
      if (currentTemp < minTemp) minTemp = currentTemp;

      // Check Danger Thresholds
      bool isDanger = (currentTemp > 38.0 || currentHum > 80.0);

      if (isDanger) {
        statusLabel = "DANGER";
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);
        
        // Trigger buzzer for 1 second (non-blocking)
        if (!buzzerActive) {
            digitalWrite(BUZZER, HIGH);
            buzzerActive = true;
            buzzerTurnOffTime = currentMillis + 1000;
        }
      } else {
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
        
        // Calculate Comfort Index
        if (currentTemp < 24.0) {
          statusLabel = "COOL";
        } else if (currentTemp <= 28.0 && currentHum <= 60.0) {
          statusLabel = "COMFORT";
        } else {
          statusLabel = "HOT";
        }
      }
    }
  }

  // ---------------------------------------------------------
  // 2. Buzzer Turn-Off Timer (Runs independently)
  // ---------------------------------------------------------
  if (buzzerActive && currentMillis >= buzzerTurnOffTime) {
    digitalWrite(BUZZER, LOW);
    buzzerActive = false;
  }

  // ---------------------------------------------------------
  // 3. Screen Switching (Bonus) & Display Update (Every 5 Secs)
  // ---------------------------------------------------------
  if (currentMillis - lastScreenTime >= 5000) {
    lastScreenTime = currentMillis;
    screenState = !screenState; // Toggle between screen 0 and 1
  }

  display.clearDisplay();
  
  if (screenState == 0) {
    // --- MAIN SCREEN ---
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Smart Climate Monitor");
    display.drawLine(0, 10, 128, 10, WHITE); // Header line separator
    
    display.setCursor(0, 16);
    display.setTextSize(1);
    display.print("Temp: "); display.print(currentTemp, 1); display.println(" C");
    display.print("Hum:  "); display.print(currentHum, 1); display.println(" %");
    
    display.setCursor(0, 48);
    display.print("Index: "); 
    display.print(statusLabel);
  } else {
    // --- BONUS SCREEN: Daily Tracker ---
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Daily Tracker");
    display.drawLine(0, 10, 128, 10, WHITE);
    
    display.setCursor(0, 20);
    display.print("Max Temp: "); 
    if(maxTemp != -100.0) display.print(maxTemp, 1); else display.print("--");
    display.println(" C");
    
    display.setCursor(0, 40);
    display.print("Min Temp: "); 
    if(minTemp != 100.0) display.print(minTemp, 1); else display.print("--");
    display.println(" C");
  }
  display.display();

  // ---------------------------------------------------------
  // 4. CSV Logging (Every 5 Seconds)
  // ---------------------------------------------------------
  if (currentMillis - lastLogTime >= 5000) {
    lastLogTime = currentMillis;
    
    // Format: millis(),temp,humidity,status
    Serial.print(currentMillis);
    Serial.print(",");
    Serial.print(currentTemp, 1);
    Serial.print(",");
    Serial.print(currentHum, 1);
    Serial.print(",");
    Serial.println(statusLabel);
  }
}