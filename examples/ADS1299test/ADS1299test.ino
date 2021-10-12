/***************************************************************************
 * Designed to test the ADS1299 bridge with an ESP32 processor. 
 * Tested and developed on an Adafruit Huzzah32. Output of all channels is
 * printed out to Serial Port.
 * 
 * Author: Ian Romano
 ***************************************************************************/

#include <SPI.h>
#include <ADS1299.h>

#define BME_CS 14
#define ADS_DRDY 15
#define ADS_START 27
#define ADS_RESET 32

#define CHANNELS 8

SleepSure_ADS1299 ads(BME_CS, ADS_DRDY, ADS_START, ADS_RESET);

int* values;

void setup() 
{
    Serial.begin(115200);
    while(!Serial);
    Serial.println("ADS1299 test");

    values = new int[CHANNELS];

    bool pass = ads.begin();
    if (!pass)
    {
        Serial.println("ADS1299 ID not recieved correctly.");
    }
    Serial.print("Detected sensor ID: ");
    Serial.println(ads.getID(), 16);
    Serial.println("ADS1299 setup complete");
}


void loop()
{ 
    ads.readChannels(values, CHANNELS);
    printValues();
}

void printValues()
{

    for(int i=0; i<CHANNELS; i++)
    {
        Serial.print("Channel ");
        double mVolt =  (0.000298 * values[i]) + 2500;
        Serial.print(i);
        Serial.print(": ");
        Serial.println(mVolt);
        Serial.print(" ");
    }
    

}

double mVolt(int byte3) { return 0.000298 * byte3 + 2500; }