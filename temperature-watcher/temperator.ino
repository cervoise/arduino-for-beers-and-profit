/*
  Needs Arduino Uno, Ethernet Shield (for SD card) and BMP180 from Sparkfun.
  Temperature mesure every DELAY and write it on SD card (needs FAT16).
*/

#include <SFE_BMP180.h>
#include <Wire.h>
#include <SD.h>

File tempTxtFile;
SFE_BMP180 pressure;

//in meters
#define ALTITUDE 30.0
//in millisecond
#define DELAY 5000

void setup()
{
  Serial.begin(9600);
  Serial.println("REBOOT");

  // Initialize the sensor (it is important to get calibration values stored on the device).
  if (pressure.begin())
    Serial.println("BMP init success.");
  else
  {
    Serial.println("BMP init fail.");
    return;
  }
  
  //init the SD card
  pinMode(10, OUTPUT);
   
  if (!SD.begin(4)) {
    Serial.println("SD initialization failed.");
    return;
  }
  Serial.println("SD initialization done.");
 
}

void loop()
{
  
  char status;
  double T;
  
  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
      Serial.print("temperature: ");
      Serial.print(T,2);
      Serial.println(" deg C.");
      
      //write on SD card
      // open the file. note that only one file can be open at a time,
      // so you have to close this one before opening another.
        tempTxtFile = SD.open("temp.txt", FILE_WRITE);
     
      // if the file opened okay, write to it:
      if (tempTxtFile) {
        Serial.print("Writing to temp.txt...");
        tempTxtFile.println(T,2);
        // close the file:
        tempTxtFile.close();
        Serial.println("done.");
      } else {
        // if the file didn't open, print an error:
        Serial.println("error opening temp.txt");
      }
     
    }

    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");

  delay(DELAY);
}
