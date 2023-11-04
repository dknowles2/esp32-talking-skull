#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include "ESP32Servo.h"

#define RXD2 16
#define TXD2 17

DFRobotDFPlayerMini mp3;
Servo servo;

const int kAudioHigh = 10;
const int kAudioLow = 0;
const uint8_t kPinAudioBusy = 4;  // Audio player busy
const uint8_t kPinAudioLevel = 2; // Audio level samples
const int kPinServo = 12;

const int kMouthOpen = 135;
const int kMouthClosed = 0;
const int kMouthDelay = 200;

void setup()
{
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  servo.attach(kPinServo, 610, 1995);
  servo.write(kMouthClosed);

  pinMode(kPinAudioLevel, INPUT);
  pinMode(kPinAudioBusy, INPUT);

  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);

  while (!mp3.begin(Serial1, /*isACK=*/true, /*doReset=*/true))
  {
    Serial.println();
    Serial.println("Failed to initialize DFPlayer");
    delay(1000);
  }
  mp3.volume(30);
}

void loop()
{
  Serial.println("");
  mp3.play(1);
  delay(100);

  while (digitalRead(kPinAudioBusy) == 0)
  {
    int maxVol = 0;
    for (int i = 0; i < 20; ++i)
    {
      int vol = analogRead(kPinAudioLevel);
      if (vol > maxVol)
        maxVol = vol;
    }
    if (maxVol > kAudioHigh)
      maxVol = kAudioHigh;

    int pos = kMouthClosed;
    if (maxVol > 0)
      pos = kMouthOpen;
    Serial.printf("maxVol=%d, pos=%d\n", maxVol, pos);
    servo.write(pos);
    delay(50);
  }
}
