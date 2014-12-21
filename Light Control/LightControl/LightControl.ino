// Written by Andrew Donelick
// Copyright (c) 2014 Apatite to Zircon
// All rights reserved.

// For information, please contact:
// <adonelick@hmc.edu>

#define CONTROL_PIN 3
int lightValue = 255;

void setup()
{
  Serial.begin(9600);
  pinMode(CONTROL_PIN, OUTPUT);
  analogWrite(CONTROL_PIN, 255);
}

void loop()
{
  if (Serial.available()) 
  {
    lightValue = 255 - Serial.read();
    if (0 <= lightValue && lightValue <= 255)
      analogWrite(CONTROL_PIN, lightValue);
  }
}
