//This is based on the example code provided by the ESP32 board
//Author: Matias Saavedra Silva

#include "BluetoothSerial.h"
#include <ADS1299.h>

#define BME_CS 14
#define ADS_DRDY 15
#define ADS_START 27
#define ADS_RESET 32

#define CHANNELS 8

SleepSure_ADS1299 ads(BME_CS, ADS_DRDY, ADS_START, ADS_RESET);


#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
//int counts[8];
int *counts;
bool synced = false;
int recv;

void setup() {
    Serial.begin(115200);
    SerialBT.begin("SleepSure"); //Bluetooth device name
    Serial.println("The device started, now you can pair it with bluetooth!");

    bool pass = ads.begin();
    if (!pass)
    {
        Serial.println("ADS1299 ID not recieved correctly.");
    }
    Serial.print("Detected sensor ID: ");
    Serial.println(ads.getID(), 16);
    counts = new int[CHANNELS];
}

void getData(int nums[]) {
  for(int i=0; i<8;i++) {
    nums[i] = random(0,16777216);
    //nums[i] = random(0,16);
  }
}

void loop() {
  // Do not start sending data unless the receiver is ready
  if(synced) {
    ads.readChannels(counts, CHANNELS);
    //getData(counts);
    // Iterate through each channel and send the data one byte at a time
    // If the data takes only one byte, pad it with 0x00. The data from each channel with its padding will always be 3 bytes
    for(int i=0; i<8;i++) {
      Serial.println(counts[i]-8388608);
      if(counts[i] < 256) {
        SerialBT.write(counts[i]);
        SerialBT.write(0);
        SerialBT.write(0);
        SerialBT.write(0); 
      }
      else if(counts[i] < 65536) {
        SerialBT.write(counts[i]>>8);
        SerialBT.write(counts[i] & 0xff);
        SerialBT.write(0);
        SerialBT.write(0);
      }
      else if(counts[i] < 16777216) {
        SerialBT.write(counts[i]>>16);
        SerialBT.write((counts[i]>>8) & 0xff);
        SerialBT.write(counts[i] & 0xff);
        SerialBT.write(0);
      }
      else {
        SerialBT.write(counts[i]>>24);
        SerialBT.write((counts[i]>>16) & 0xff);
        SerialBT.write((counts[i]>>8) & 0xff);
        SerialBT.write(counts[i] & 0xff);
      }
      SerialBT.write(0);
    }
    // After all the data is sent, mark the end of the data with 0x0010
    SerialBT.write(255);
    SerialBT.write(255);
    delay(3);
  }
  else {
    // Wait for the receiver to send 0xff
    if(SerialBT.available()) {
      recv = SerialBT.read();
      if (recv == 255) {
        synced=true;
        Serial.println("Synced!");
      }
      delay(10);
    }
  }
}
