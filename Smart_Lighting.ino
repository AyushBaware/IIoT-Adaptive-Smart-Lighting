#include <Adafruit_LiquidCrystal.h>

// Initialize the LCD (I2C address 0)
Adafruit_LiquidCrystal lcd(0);

// --- Pin Definitions ---
int pirPin = 2;       
int ledPin = 9;       
int ldrPin = A0;      
int trigPin = 4;      
int echoPin = 5;      

// --- Variables ---
int motionState = 0;
int ldrValue = 0;
int targetBrightness = 0;
long duration;
int distance;
unsigned long lastPresenceTime = 0;
const unsigned long timeoutPeriod = 5000; 

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);

  // Start the LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("IIoT System Init");
  delay(1500);
  lcd.clear();
}

void loop() {
  motionState = digitalRead(pirPin);
  ldrValue = analogRead(ldrPin);
  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; 

  targetBrightness = map(ldrValue, 50, 900, 255, 0);
  targetBrightness = constrain(targetBrightness, 0, 255);

  bool isPresent = false;
  if (motionState == HIGH || (distance > 0 && distance < 100)) {
    isPresent = true;
    lastPresenceTime = millis(); 
  }

  if (isPresent) {
    analogWrite(ledPin, targetBrightness); 
  } else {
    if (millis() - lastPresenceTime > timeoutPeriod) {
      analogWrite(ledPin, 0);  
      targetBrightness = 0;    
    } else {
      analogWrite(ledPin, 50); 
    }
  }

  int energySaved = map(targetBrightness, 0, 255, 100, 0); 

  // --- Dashboard Display Logic ---
  lcd.setCursor(0, 0);
  if (isPresent) {
    lcd.print("Status: OCCUPIED");
  } else {
    lcd.print("Status: EMPTY   ");
  }

  lcd.setCursor(0, 1);
  lcd.print("Saved: ");
  lcd.print(energySaved);
  lcd.print("%   "); // Extra spaces to clear old numbers

  delay(200); 
}
