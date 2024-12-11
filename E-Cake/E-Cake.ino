// Comment AM2320 or TMP36 codes accordingly to switch between the 2 sensors
// AM2320 is used for physical circuit while TMP36 is used for Tinkercad simulation
// Comment out '#include<Wire.h>' for Tinkercad

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// AM2320
#include "Adafruit_Sensor.h"
#include "Adafruit_AM2320.h"
// #include <AM2320_asukiaaa.h>

// C++ code
// Inits

// TMP36
// const int sensorPin = A0;

// const int volumePin = A1;
const int switchPin = 2;
const int resetPin = 3;
const int greenPin = 9;
const int bluePin = 10;
const int redPin = 11;

/// int volumeValue = 0;
int switchState = HIGH;
int lastSwitchState = HIGH;
unsigned long debounceDelay = 50;  // Debounce- to avoid multiple toggles?
// unsigned long lastDebounceTime = 0;  // Last time the switch state changed
int resetState = HIGH;
int lastResetState = HIGH;
// bool isMelodyPlaying = false;
volatile bool systemOn = true;  // System state, volatile- Interrupt...
// volatile bool systemReset = false;
bool isOffBefore = true;
bool isOnBefore = true;
bool isBlown = false;
// bool isFinished = false;
float lastTemp;

LiquidCrystal_I2C lcd(0x26, 16, 2);  // Address 0x26, 0x20, 16x2 LCD
// LiquidCrystal_I2C for PCF8574-based, Adafruit_LiquidCrystal for MCP23008-based

// AM2320
Adafruit_AM2320 am2320 = Adafruit_AM2320();
// AM2320_asukiaaa mySensor;

// Addresses- AM2320 is 0x5C, LCD is 0x26

/*                                       
  Robson Couto, 2019
  
  Library originally added 18 Apr 2008
  by David A. Mellis
  modified 7 Nov 2016
  by Arturo Guadalupi
*/
// Notes
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST 0

// Melody- note then duration
// 4- quarter note, 8- 8th note, negative- dotted (-4 is dotted quarter), and so on...
int melody[] = {
  // HBD Song
  NOTE_C5, -8, NOTE_C5, 16, NOTE_D5, 4, NOTE_C5, 4, NOTE_F5, 4, NOTE_E5, 2, 
  NOTE_C5, -8, NOTE_C5, 16, NOTE_D5, 4, NOTE_C5, 4, NOTE_G5, 4, NOTE_F5, 2, 
  NOTE_C5, -8, NOTE_C5, 16, NOTE_C6, 4, NOTE_A5, 4, NOTE_F5, -8, NOTE_F5, 16, NOTE_E5, 4, NOTE_D5, 1, 
  NOTE_AS5, -8, NOTE_AS5, 16, NOTE_A5, 4, NOTE_F5, 4, NOTE_G5, 4, NOTE_F5, 2, 
  REST, 1, 
};

int tempo = 125;  // Tempo of HBD Song
int buzzer = 5;  // Pin 3, 13

// Sizeof gives the #bytes, each int value is composed of 2bytes (16bits)
// There are 2 values per note (pitch and duration), so for each note there are 4bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// This calculates the duration of a whole note in ms (60s/tempo)*4 beats
int wholeNote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;
unsigned long lastNoteTime = 0;
int currentNote = 0;

void setup() {
  // put your setup code here, to run once:
  // pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  while(!Serial) {
    delay(10);
  };
  
  // pinMode(volumePin, INPUT);
  pinMode(switchPin, INPUT_PULLUP);  // Pull UP, Down
  pinMode(resetPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(switchPin), toggleSystem, FALLING);  // Trigger on Falling Edge
  attachInterrupt(digitalPinToInterrupt(resetPin), toggleReset, FALLING);
  pinMode(buzzer, OUTPUT);
  
  // RGB
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  // LCD
  lcd.init();
  lcd.backlight();

  // TMP36
  // int sensorValue = analogRead(sensorPin);
  // float sensorVoltage = sensorValue * (5.0 / 1023.0);
  // lastTemp = (sensorVoltage - 0.5) * 100.0;

  // AM2320
  Serial.println("AM2320 started");
  am2320.begin();  //**
  // Wire.begin();
  // mySensor.setWire(&Wire);
  lastTemp = am2320.readTemperature();
}

void loop() {
  // put your main code here, to run repeatedly:
  // digitalWrite(LED_BUILTIN, HIGH);
  // delay(1000); // Wait for 1000 millisecond(s)
  // digitalWrite(LED_BUILTIN, LOW);
  // delay(1000); // Wait for 1000 millisecond(s)
  
  /*
  // Pullup resistor test
  int state = digitalRead(resetPin);  // Read switch state
  if (state == LOW) {
    Serial.println("Switch pressed");
  }
  else {
    Serial.println("Switch released");
  }
  delay(2000);
  */
  
  /*
  // Switch
  switchState = digitalRead(switchPin);
  if (switchState == LOW && lastSwitchState == HIGH) {  // Falling edge
    systemOn = !systemOn;
    Serial.println(systemOn ? "System On" : "System Off");
    delay(debounceDelay);  // Simple debounce
  }
  lastSwitchState = switchState;
  // break;
  */
  
  if (systemOn) {
    if (isOffBefore) {
      // LCD
      lcd.backlight();
      lcd.clear();
  	  lcd.setCursor(0, 0);  // col 0 row 0
      lcd.print("System On");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Happy Birthday!");
      lcd.setCursor(0, 1);
      lcd.print("Blow your candle");

      // delay(1000);

      // Buzzer
      lastNoteTime = millis();
      currentNote = 0;
      
      isOffBefore = false;
    }

    // TMP36
    // sensorValue = analogRead(sensorPin);
    // sensorVoltage = sensorValue * (5.0 / 1023.0);
    // float temp = (sensorVoltage - 0.5) * 100.0;
    // Serial.print("Temperature: ");
    // Serial.print(temp);
    // Serial.println(" degC");

    // AM2320
    float temp = am2320.readTemperature();
    float hum = am2320.readHumidity();
    Serial.print("Temp: "); Serial.print(temp); Serial.println("degC");
    Serial.print("Hum: "); Serial.print(hum); Serial.println("%RH");

    // Candle
    if (temp < (lastTemp + 1) && !isBlown) {
      setRGB(255, 5, 0);
    }
    else if (temp >= (lastTemp + 1) && !isBlown) {
      setRGB(0, 0, 0);
      isBlown = true;
    }
    // Change + 1 accordingly (like threshold), can also use humidity- better but my AM2320 has humidity reading issues
    
    playMelody();
    // delay(10);
  }
  
  else {
    if (isOnBefore) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("System off");
      delay(2000);
      lcd.clear();
      lcd.noBacklight();
      
      setRGB(0, 0, 0);
      noTone(buzzer);
      
      isOnBefore = false;
    }
  }
}

// Functions
void setRGB(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

void playMelody() {
  /*
  // Iterate over the notes of melody
  // The array is twice the #notes (note + duration)
  for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
    // Calculates the duration of each note
    divider = melody[thisNote + 1];
    if (divider > 0) {  // Regular note then just proceed
      noteDuration = wholeNote / divider;
    }
    else if (divider < 0) {  // Dotted
      noteDuration = wholeNote / abs(divider);
      noteDuration *= 1.5;  // *Half...
    }
    
    // Volume
  	// int volumeValue = analogRead(volumePin);
  	// int volume = map(volumeValue, 0, 1023, 0, 225);  // Map to range 0-255
   	
    tone(buzzer, melody[thisNote], noteDuration * 0.9);  // Only play for 90% of duration, leave 10% as pause
   	// analogWrite(buzzer, volume);  // Set volume to buzzer
    delay(noteDuration);  // Wait for the specified duration before playing next note
    noTone(buzzer);  // Stop waveform generation before the next note
    analogWrite(buzzer, 0);  // Reset volume (turn off sound completely)
  }
  */
  
  if (currentNote < notes * 2) {
  	unsigned long now = millis();
  	if (now - lastNoteTime >= noteDuration) {
      divider = melody[currentNote + 1];
      if (divider > 0) {
        noteDuration = wholeNote / divider;
      }
      else if (divider < 0) {
        noteDuration = wholeNote / abs(divider);
        noteDuration *= 1.5;
      }
      
      tone(buzzer, melody[currentNote], noteDuration * 0.9);
      
      lastNoteTime = now;
      currentNote += 2;
      
      if (currentNote >= notes * 2) {
        noTone(buzzer);
        // currentNote = 0;
      }
    }
  }
}

void toggleSystem() {
  systemOn = !systemOn;
  if (!systemOn) {
    isOffBefore = true;
    Serial.println("System Off");
  }
  else {
    isOnBefore = true;
    Serial.println("System On");
  }
  // delay(50); // Debounce delay
}

void toggleReset() {
  // systemReset = !systemReset;
  // systemReset = false;
  currentNote = 0;
  isBlown = false;
  Serial.println("System Reset");
  // delay(50); // Debounce delay
}