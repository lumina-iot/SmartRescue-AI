#define BLYNK_TEMPLATE_ID "TMPL3Cbo1dD1E"
#define BLYNK_TEMPLATE_NAME "Smart Rescue AI"
#define BLYNK_AUTH_TOKEN "GrZCADeUHYtv_i38KUy7iJ9eGoOnq46_"


#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <time.h>
#include <math.h>

// ---------- Wi-Fi ----------
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

// ---------- Hardware pins ----------
#define SDA_PIN     21
#define SCL_PIN     22
#define BUTTON_PIN  18
#define LED_PIN     23
#define BUZZER_PIN  19

// ---------- Simulated location ----------
// Example coordinates in Bengaluru, India.
// These are NOT live GPS coordinates.
const double SIM_LATITUDE  = 12.971600;
const double SIM_LONGITUDE = 77.594600;

// ---------- Detection settings ----------
const float IMPACT_THRESHOLD = 25.0;  // m/s^2
const float NORMAL_THRESHOLD = 15.0;  // m/s^2

Adafruit_MPU6050 mpu;
BlynkTimer timer;

bool mpuReady = false;
bool accidentLatched = false;
bool lastButtonState = HIGH;

float acceleration = 0.0;
int alertCount = 0;

unsigned long lastButtonTime = 0;
unsigned long normalSince = 0;

// ---------- Timestamp ----------
String getTimestamp() {
  struct tm timeinfo;

  if (getLocalTime(&timeinfo, 1000)) {
    char buffer[30];
    strftime(buffer, sizeof(buffer),
             "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(buffer) + " IST";
  }

  return "Time not synced";
}

// ---------- Google Maps URL ----------
String getMapURL() {
  String url = "https://www.google.com/maps?q=";
  url += String(SIM_LATITUDE, 6);
  url += ",";
  url += String(SIM_LONGITUDE, 6);
  return url;
}

// ---------- Accident alert ----------
void sendAccidentAlert(const char* source) {
  if (accidentLatched) {
    Serial.println("Alert already active.");
    return;
  }

  accidentLatched = true;
  normalSince = 0;
  alertCount++;

  // Local alarm works even if Wi-Fi is disconnected.
  digitalWrite(LED_PIN, HIGH);
  tone(BUZZER_PIN, 2000, 1500);

  String timestamp = getTimestamp();
  String mapURL = getMapURL();

  String message = "SmartRescue: POSSIBLE ACCIDENT. ";
  message += "Time: " + timestamp;
  message += ". Location: ";
  message += String(SIM_LATITUDE, 6);
  message += ", ";
  message += String(SIM_LONGITUDE, 6);
  message += ". Map: " + mapURL;
  message += ". Acceleration: ";
  message += String(acceleration, 2);
  message += " m/s^2. Trigger: ";
  message += String(source);

  Serial.println("\n===== SMARTRESCUE ALERT =====");
  Serial.println(message);
  Serial.println("=============================");

  if (Blynk.connected()) {
    Blynk.virtualWrite(V0, 1);
    Blynk.virtualWrite(V1, acceleration);
    Blynk.virtualWrite(V2, "POSSIBLE ACCIDENT");
    Blynk.virtualWrite(V3, alertCount);
    Blynk.virtualWrite(V4, timestamp);
    Blynk.virtualWrite(V5, mapURL);

    // Event code must match your Blynk event name.
    Blynk.logEvent("accident_alert", message);

    Serial.println("Alert sent to Blynk.");
  } else {
    Serial.println("Blynk offline; alert printed to Serial.");
  }
}

// ---------- Read MPU6050 ----------
void readSensors() {
  if (!mpuReady) return;

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Acceleration magnitude in m/s^2.
  acceleration = sqrt(
    a.acceleration.x * a.acceleration.x +
    a.acceleration.y * a.acceleration.y +
    a.acceleration.z * a.acceleration.z
  );

  Serial.print("Acceleration: ");
  Serial.print(acceleration, 2);
  Serial.println(" m/s^2");

  if (Blynk.connected()) {
    Blynk.virtualWrite(V1, acceleration);
  }

  // Detect a possible high-impact event.
  if (acceleration > IMPACT_THRESHOLD &&
      !accidentLatched) {
    sendAccidentAlert("MPU6050 impact threshold");
  }

  // Rearm only after readings remain normal for 10 seconds.
  if (accidentLatched && acceleration < NORMAL_THRESHOLD) {
    if (normalSince == 0) {
      normalSince = millis();
    }

    if (millis() - normalSince >= 10000UL) {
      accidentLatched = false;
      normalSince = 0;

      digitalWrite(LED_PIN, LOW);
      noTone(BUZZER_PIN);

      if (Blynk.connected()) {
        Blynk.virtualWrite(V0, 0);
        Blynk.virtualWrite(V2, "NORMAL");
      }

      Serial.println("System rearmed: NORMAL.");
    }
  } else {
    normalSince = 0;
  }
}

// ---------- Button test ----------
void checkButton() {
  bool buttonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH &&
      buttonState == LOW &&
      millis() - lastButtonTime >= 2000UL) {

    lastButtonTime = millis();
    sendAccidentAlert("Manual push-button test");
  }

  lastButtonState = buttonState;
}

// ---------- Blynk connection ----------
void maintainConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to Wi-Fi...");
    WiFi.reconnect();
    return;
  }

  if (!Blynk.connected()) {
    Serial.println("Connecting to Blynk...");
    Blynk.connect(1000);
  }

  if (Blynk.connected()) {
    Blynk.virtualWrite(V0, accidentLatched ? 1 : 0);
    Blynk.virtualWrite(V1, acceleration);
    Blynk.virtualWrite(
      V2, accidentLatched ? "POSSIBLE ACCIDENT" : "NORMAL"
    );
    Blynk.virtualWrite(V3, alertCount);
    Blynk.virtualWrite(V4, getTimestamp());
    Blynk.virtualWrite(V5, getMapURL());
  }
}

// ---------- Setup ----------
void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);

  Serial.println("\nStarting SmartRescue AI...");

  // Start I2C and initialise MPU6050.
  Wire.begin(SDA_PIN, SCL_PIN);

  if (mpu.begin()) {
    mpuReady = true;

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    Serial.println("MPU6050 ready.");
  } else {
    Serial.println(
      "MPU6050 not found. Button testing is still available."
    );
  }

  // Start Wi-Fi without blocking the entire program.
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  // India Standard Time = UTC + 5 hours 30 minutes.
  configTime(19800, 0, "pool.ntp.org", "time.google.com");

  // Configure Blynk and reconnect in the background.
  Blynk.config(BLYNK_AUTH_TOKEN);

  timer.setInterval(250L, readSensors);
  timer.setInterval(50L, checkButton);
  timer.setInterval(5000L, maintainConnection);

  Serial.println("SmartRescue ready.");
  Serial.println("Press the button to test an alert.");
}

// ---------- Main loop ----------
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    Blynk.run();
  }

  timer.run();
}
